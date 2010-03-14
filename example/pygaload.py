#!/usr/bin/env python
"""This program acts as a client for a bootloader that implements the MegaLoad
protocol version 3, 4, or 5. Only FLASH programming is supported, not EEPROM
or lock bit programming.

This program is free software licensed under the terms of the GNU General
Public License. See http://www.gnu.org/licenses/gpl.html for the license terms.

The protocol is as follows:

* Upon boot, if autobaud is in effect, the character 0x55 ('U') is repeatedly
  transmitted until 0x55 is received in return. If 0x55 is not received after
  all baud rate divisors have been tried, the main FLASH code at address 0x0000
  is executed.

  NOTE: MegaLoad 3 sends the '>' character (0x3E) instead of 0x55 and expects
  '<' (0x3C) in return.

* If auto-OSCCAL is in effect, then 0x55 characters continue to be
  sent out, and 0x55 is expected in return.

  MegaLoad 3 does not do auto-OSCCAL.

* Following this, any received character is flushed and a '>' character (0x3E)
  is sent.  If a '<' character (0x3C) is received, then bootloading begins.
  Otherwise, the user's code at address 0x0000 is executed.

  MegaLoad 3 and MegaLoad 4 do not send this '>' character and do not wait for '<'.
  Bootloading begins immediately after the auto-OSCCAL step.

* Following this, the DeviceID is sent out. This is:  
    Mega8:      'A' (0x41)
    Mega16:     'B' (0x42)
    Mega64:     'C' (0x43)
    Mega128:    'D' (0x44)
    Mega32:     'E' (0x45)
    Mega162:    'F' (0x46)
    Mega169:    'G' (0x47)
    Mega8515:   'H' (0x48)
    Mega8535:   'I' (0x49)
    Mega163:    'J' (0x4A)
    Mega323:    'K' (0x4B)
    Mega48:     'L' (0x4C)
    Mega88:     'M' (0x4D)
    Mega168:    'N' (0x4E)
    Tiny2313:   'O' (0x4F)
    Tiny13:     'P' (0x50)
    Mega165:    0x80
    Mega3250:   0x81
    Mega6450:   0x82
    Mega3290:   0x83
    Mega6490:   0x84
    Mega406:    0x85
    Mega640:    0x86
    Mega1280:   0x87
    Mega2560:   0x88

* Following the DeviceID, the FlashSize is sent out. This is:
    Flash1k:    'g' (0x67)
    Flash2k:    'h' (0x68)
    Flash4k:    'i' (0x69)
    Flash8k:    'l' (0x6C)
    Flash16k:   'm' (0x6D)
    Flash32k:   'n' (0x6E)
    Flash64k:   'o' (0x6F)
    Flash128k:  'p' (0x70)
    Flash256k:  'q' (0x71)
    Flash40k:   'r' (0x72)

* Following the FlashSize, the BootSize IN WORDS is sent out. This is:    
    Boot128:    'a' (0x61)
    Boot256:    'b' (0x62)
    Boot512:    'c' (0x63)
    Boot1024:   'd' (0x64)
    Boot2048:   'e' (0x65)
    Boot4096:   'f' (0x66)

* Following the BootSize, the PageSize is sent out. This is:
    Page32:   'Q' (0x51)
    Page64:   'R' (0x52)
    Page128:  'S' (0x53)
    Page256:  'T' (0x54)
    Page512:  'V' (0x56)

  NOTE: MegaLoad 3 uses 'U' (0x55) for Page512.

* Following the PageSize, the EEPROMSize is sent out. This is:    
    EEprom64:     '.' (0x2E)
    EEprom128:    '/' (0x2F)
    EEprom256:    '0' (0x30)
    EEprom512:    '1' (0x31)
    EEprom1024:   '2' (0x32)
    EEprom2048:   '3' (0x33)
    EEprom4096:   '4' (0x34)

* MegaLoad then reads any incoming character, which is ignored. NOTE: There is
  no waiting for a character...it's just a read of the UDR so no character needs
  to be sent.

* The FLASH loading process then begins. The character '!' (0x21) is transmitted.

  NOTE: MegaLoad 4 transmits the '>' character before the '!' character.

* The process of loading pages begins. For each page:
   - The 16-bit page number must be sent MSB-first. The page address is
     constructed from the page number by shifting it left by log2(PageSize) bits.
     For example, for the ATmega32, the page number is shifted left by 7 bits.

   - If the page number is 0xFFFF, the process is complete.

   - Once the page number/address is obtained, the page data is obtained.
     PageByte characters are received in sequence, LSB first. That is, each
     16-bit AVR program word is sent LSB-first, the way they are stored in
     HEX files.

   - An 8-bit checksum over all PageByte characters is computed. The next
     character received by MegaLoad is expected to be this checksum. If not,
     MegaLoad transmits '@' (0x40). It is possible to then retransmit this page
     if so desired, beginning with the 16-bit page number.

   - If the checksum is correct, the FLASH is written. If the write is successful,
     '!' (0x21) is written by MegaLoad, else '@' (0x40) is written.

* Once the FLASH process is complete, EEPROM loading begins. MegaLoad transmits ')' (0x29)
  followed by '!' (0x21).

  For each EEPROM character to write, MegaLoad expects the 16-bit address to be sent, MSB-first,
  followed by the 8-bit data byte, followed by the 8-bit checksum over all 3 bytes sent so
  far (two address bytes plus data byte).

  An EEPROM address of 0xFFFF terminates the EEPROM loading process immediately
  without expecting a data byte or checksum byte.

  If the checksum matches and the EEPROM write is successful, '!' (0x21) is transmitted by
  MegaLoad, or '@' (0x40) in all other cases.

* Once EEPROM writing is complete, Lock Bit programming begins. MegaLoad transmits '%' (0x25)
  then waits for two characters. The two characters are expected to be one's complements
  of each other. If not, nothing is done.

  If the two characters are one's complements of each other, the lock bits are written with
  the second byte received (i.e., the one's complement of the first byte received).

  There is no indication of whether this process was successful or not.
"""  

import sys
import os
import termios
import select
import time
import array
from optparse import OptionParser

VERSION_MAJOR=1
VERSION_MINOR=1

_usage="""\
%prog [Options] prog.hex

The 'prog.hex' file must be the file to download in HEX format.

The optional reset string (--send-reset parameter) can include C-style control
characters such as \\n and \\r. For example:

    pygaload.py --send-reset='reset\\r\\n' ...
    pygaload.py --send-reset='\\x03' ...  """

Default_DevicePort = "/dev/ttyUSB0"
Default_BaudRate   = 38400
Default_Timeout    = 10

##############################################################################

Processors = {0x41: 'ATmega8',
              0x42: 'ATmega16',
              0x43: 'ATmega64',
              0x44: 'ATmega128',
              0x45: 'ATmega32',
              0x46: 'ATmega162',
              0x47: 'ATmega169',
              0x48: 'ATmega8515',
              0x49: 'ATmega8535',
              0x4A: 'ATmega163',
              0x4B: 'ATmega323',
              0x4C: 'ATmega48',
              0x4D: 'ATmega88',
              0x4E: 'ATmega168',
              0x4F: 'ATtiny2313',
              0x50: 'ATtiny13',
              0x80: 'ATmega165',
              0x81: 'ATmega3250',
              0x82: 'ATmega6450',
              0x83: 'ATmega3290',
              0x84: 'ATmega6490',
              0x85: 'ATmega406',
              0x86: 'ATmega640',
              0x87: 'ATmega1280',
              0x88: 'ATmega2560' }

FlashSize = {
    0x67: 1024,
    0x68: 2048,
    0x69: 4096,
    0x6C: 8192,
    0x6D: 16384,
    0x6E: 32768,
    0x6F: 65536,
    0x70: 2*65536,
    0x71: 4*65536,
    0x72: 40*1024
}

# The boot size is reported in WORDS
BootSize = {    
    0x61: 128,
    0x62: 256,
    0x63: 512,
    0x64: 1024,
    0x65: 2048,
    0x66: 4096
}    

PageSize = {    
    0x51: 32,
    0x52: 64,
    0x53: 128,
    0x54: 256,
    0x55: 512,  # MegaLoad 3 uses 0x55 for Page512
    0x56: 512
}    

EEPROMSize = {    
    0x2E: 64,
    0x2F: 128,
    0x30: 256,
    0x31: 512,
    0x32: 1024,
    0x33: 2048,
    0x34: 4096
}    

HEXLAT = {}
for val in range(256):
  HEXLAT['%02X' % val] = val

def parse8(line, offset, HEXLAT=HEXLAT):
  return HEXLAT[line[offset:offset+2]]

def parse16(line, offset):
  return parse8(line, offset)*256 + parse8(line,offset+2)

def readHEX(options, args):
  try:
    fid = file(args[0], 'rt')
  except Exception, detail:
    print 'Unable to open HEX file:\n  %s' % str(detail)
    sys.exit(1)

  datalines = []
  linenum = 0
  for line in fid:
    linenum += 1

    line = line.strip()
    if line[0] != ':':
      print "Line does not begin with ':' at file %s line #%d" % (args[0], linenum)
      sys.exit(1)

    if len(line) < 11:
      print "Line too short at file %s line #%d" % (args[0], linenum)
      sys.exit(1)

    numBytes = parse8(line, 1)
    if len(line) != numBytes*2 + 11:
      print "Line length does not match byte count at file %s line #%d" % (args[0], linenum)
      sys.exit(1)

    addr = parse16(line, 3)
    rectype = parse8(line, 7)

    if rectype == 0x01:
      break

    if rectype != 0:
      print "Unexpected record type %d at file %s line #%d" % (rectype, args[0], linenum)
      sys.exit(1)

    checksum = rectype + numBytes + addr + (addr >> 8)
    data = array.array('B', '\xFF'*numBytes) # Type 'B' is unsigned char stored as Python int's

    for i in range(numBytes):
      data[i] = parse8(line, 2*i+9)
      checksum += data[i]

    checksum += parse8(line, 2*numBytes+9)
    if (checksum & 0xFF) != 0:
      print "Record checksum error at file %s line #%d" % (args[0], linenum)

    datalines.append( (addr, data) )

  fid.close()

  datalines.sort()  # by starting address
  return datalines

def openDevice(options):
  try:
    dev = os.open(options.DevicePort, os.O_RDWR | os.O_NOCTTY | os.O_NDELAY)
  except Exception, detail:
    print "Unable to open device '%s':\n  %s" % (options.DevicePort, str(detail))
    sys.exit(1)

  if options.Verbose:
    print 'Opened %s ...' % options.DevicePort

  try:
    exec 'baudmask = termios.B%d' % options.BaudRate
  except:
    print 'Unable to set baud rate to %d' % options.BaudRate
    sys.exit(1)

  try:
    # TCGETADDR returns a list:
    #   [iflag, oflag, cflag, lflag, ispeed, ospeed, cc]
    attr = termios.tcgetattr(dev)
    attr[0] = termios.IGNBRK # Leave termios.IGNPAR unset so framing errors translate characters to \0
    attr[1] = 0
    # Two stop bits seems to work better in some cases, hence CSTOPB.
    attr[2] = termios.CSTOPB|termios.CS8|termios.CREAD|termios.CLOCAL|baudmask
    attr[3] = 0
    attr[4] = baudmask
    attr[5] = baudmask

    # By setting VMIN to 0 and VTIME to non-zero, a read will return when
    # either 1 character has been received or the time has elapsed. But this
    # doesn't seem to work, so we use poll() and that seems to work fine.
    attr[6][termios.VMIN] = 1   # minimum characters to fulfill a read request
    attr[6][termios.VTIME] = 0  # deciseconds to wait for reads
    termios.tcsetattr(dev, termios.TCSAFLUSH, attr)
  except:
    print 'Unable to set terminal attributes on device %s' % options.DevicePort
    sys.exit(1)

  if options.Verbose:
    print "Set terminal attributes on %s ..." % options.DevicePort

  return dev

CONNECT  = 1   # Waiting for bootloader sync character
SYNCED3  = 2   # Received 0x3E sync character from MegaLoad 3 bootloader
SYNCED4  = 3   # Received 0x55 sync character from MegaLoad 4 bootloader
GOTPROC  = 4   # Received processor type
GOTFLASH = 5   # Received flash size
GOTBOOT  = 6   # Received boot size
GOTPAGE  = 7   # Received page size
GOTEEP   = 8   # Received EEPROM size

class Proc:
  def __init__(self):
    pass

def doConnect(options):  
  state = CONNECT

  P = Proc()
  P.loaderversion = 0 # Not known yet

  tic = time.time()
  while (time.time()-tic) < options.Timeout:
    L = poll.poll(1000)
    if L:
      c = ord(os.read(options.dev,1))

      if state==CONNECT:
        if c == 0x55: # For MegaLoad 4 and 5
          os.write(options.dev, '\x55')
          state = SYNCED4
          P.loaderversion = 4
        elif c == 0x3E: # For MegaLoad 3
          os.write(options.dev, '\x3C')
          state = SYNCED3  # Maybe it's just junk, though, from a MegaLoad 4 bootloader
          P.loaderversion = 3

      elif state in (SYNCED3, SYNCED4):
        if Processors.has_key(c):
          # Looks good...could be good data
          if options.Verbose:
            print '\n    Processor: %s' % Processors[c]
          state = GOTPROC
          P.proc = Processors[c]
        else:
          # Nope...we've just got garbage. Alternatively, we keep getting 0x55's from
          # a MegaLoad 4 bootloader that is doing auto-OSCCAL, or we got '>' from
          # MegaLoad 5.
          if state==SYNCED4:
            if c == 0x55:  # We're in auto-OSCCAL
              pass
            elif c == 0x3E: # MegaLoad 5 sends '>'
              os.write(options.dev, '\x3C')
              P.loaderversion = 5
            else:
              print '*** Unexpected processor type code: %s' % hex(c)
              sys.exit(1)
          else:
            state = CONNECT

      elif state == GOTPROC:
        if FlashSize.has_key(c):
          if options.Verbose:
            print '   Flash Size: %dk' % (FlashSize[c]/1024)
          state = GOTFLASH
          P.flash = FlashSize[c]
        else:
          print '*** Unexpected flash size code: %s' % hex(c)
          sys.exit(1)

      elif state == GOTFLASH:
        if BootSize.has_key(c):
          if options.Verbose:
            print '    Boot Size: %d words' % (BootSize[c])
          state = GOTBOOT
          P.boot = BootSize[c]*2
        else:
          print '*** Unexpected boot size code: %s' % hex(c)
          sys.exit(1)

      elif state == GOTBOOT:
        if PageSize.has_key(c):
          if options.Verbose:
            print '    Page Size: %d bytes' % PageSize[c]
          state = GOTPAGE
          P.page = PageSize[c]
        else:
          print '*** Unexpected page size code: %s' % hex(c)
          sys.exit(1)

      elif state == GOTPAGE:
        if EEPROMSize.has_key(c):
          if options.Verbose:
            print '  EEPROM Size: %s bytes' % EEPROMSize[c]
          state = GOTEEP
          P.eeprom = EEPROMSize[c]
        else:
          print '*** Unexpected EEPROM size code: %s' % hex(c)
          sys.exit(1)

      elif state == GOTEEP:
        if c == 0x21:     # '!' means we're all done
          if options.Verbose:
            print '\nUsing MegaLoad %d protocol ...' % P.loaderversion
          return P
        elif c == 0x3E:   # '>' is MegaLoad 4, and comes before '!'
          pass
        else:
          print '*** Unexpected FLASH start code: %s' % hex(c)
          sys.exit(1)

    elif options.Verbose:
      sys.stdout.write('.')
      sys.stdout.flush()

  # Timeout
  if options.Verbose:
    print '\n*** Timeout waiting for bootloader to connect'

  return None

def downloadFlash(options, proc, datalines):
  # Basic checks:
  #   - make sure we're not going beyond max address where bootloader starts
  #   - make sure flash is an integer number of pages
  #   - make sure boot section is an integer number of pages

  lastAddr = datalines[-1][0]
  lastAddr += len(datalines[-1][1])

  if lastAddr > (proc.flash - proc.boot):
    print '*** HEX file contents extends into bootloader'
    return 0

  NumPages = proc.flash // proc.page
  if NumPages*proc.page != proc.flash:
    print '*** FLASH size is not an integer number of pages'
    return 0

  NumPages -= proc.boot // proc.page
  if NumPages*proc.page + proc.boot != proc.flash:
    print '*** Bootloader size is not an integer number of pages'
    return 0

  emptypage = '\xFF'*proc.page

  datalinesix = 0
  addr = datalines[datalinesix][0]
  data = datalines[datalinesix][1]
  addrix = 0

  if options.Debug:
    dumpfid = file('dump.txt','wt')

  for pagenum in range(NumPages):
    startAddr = pagenum*proc.page
    endAddr = startAddr + proc.page

    if options.Verbose:
      print '\r    Page %d ...' % pagenum,
      sys.stdout.flush()

    if addr >= endAddr:
      # Next byte to write is past this page
      continue

    page = array.array('c', emptypage)

    while 1:
      while (addrix < len(data)) and (addr < endAddr):
        page[addr-startAddr] = chr(data[addrix])
        addrix += 1
        addr += 1

      if addrix >= len(data):
        datalinesix += 1
        if datalinesix < len(datalines):
          addr = datalines[datalinesix][0]
          data = datalines[datalinesix][1]
          addrix = 0
        else:
          break

      if addr >= endAddr:
        break

    towrite = page.tostring()

    if towrite != emptypage:
      for tries in range(3):
        if options.Debug:
          print >> dumpfid, 'Page:', pagenum
          for ix in range(len(towrite)):
            print >> dumpfid, '%02X ' % ord(towrite[ix]),
            if (ix & 0x0F) == 0x0F:
              print >> dumpfid
        else:
          #print 'Writing length-%d string' % len(towrite)
          os.write(options.dev, chr((pagenum >> 8) & 0xFF))
          os.write(options.dev, chr(pagenum & 0xFF))
          #for ix in range(len(towrite)):
          #  os.write(options.dev, towrite[ix])
          os.write(options.dev, towrite)

        checksum = 0
        for ix in range(len(towrite)):
          checksum += ord(towrite[ix])

        if options.Debug:
          print >> dumpfid, "Checksum:", hex(checksum & 0xFF)
        else:
          #print 'Writing checksum:', chr(checksum & 0xFF)
          os.write(options.dev, chr(checksum & 0xFF))

        if options.Debug:
          if 0:
            print 'Response to page #%d' % pagenum, '(! or @ or Enter):'
            s = raw_input()
            if s:
              c = ord(s[0])
            else:
              c = None
          else:
            c = ord('!')
        else:
          L = options.poll.poll(3000)
          if L:
            c = ord(os.read(options.dev,1))
          else:
            c = None

        if c is not None:
          if c == 0x21:
            break   # successful write
          elif c == 0x40:
            if options.Verbose:
              print 'failed'
              print '\r    Page %d ...' % pagenum,
              sys.stdout.flush()
          else:
            print '\n*** Unexpected response %02X to FLASH page write' % c
            return 0
        else:
          print '\n*** No response from bootloader'
          return 0
      else:
        print '\n*** Giving up after 3 tries'
        return 0

  # Flash writing is all done...we must send a page number of 0xFFFF
  os.write(options.dev, '\xFF\xFF')

  if options.Verbose:
    print
  return 1

if __name__ == "__main__":
  parser = OptionParser(usage=_usage)
  parser.add_option("-p", "--port", dest="DevicePort", \
                    help="Device port for communication (default: %s)" % Default_DevicePort, \
                    metavar="DEV", default=Default_DevicePort)
  parser.add_option("-b", "--baud-rate", type="int", dest="BaudRate", help="Baud rate (default: %d)" % Default_BaudRate, \
                    metavar="BAUD", default=Default_BaudRate)
  parser.add_option("-V", "--verbose", dest="Verbose", action="store_true", default=False, help="Print verbose progress reports")
  parser.add_option("-t", "--timeout", dest="Timeout", type="float", default=Default_Timeout, \
                    help="How long to wait for bootloader response (default: %g seconds)" % Default_Timeout, metavar="SEC")
  parser.add_option("-D", "--debug", dest="Debug", action="store_true", default=False, help="Debugging mode (not for normal use)")
  parser.add_option("-s", "--send-reset", dest="SendReset", metavar="STRING", default=None, \
                    help="String to send to invoke bootloader")
  parser.add_option("-v", "--version", dest="Version", action="store_true", default=False, help="Print version info and exit")
      
  (options, args) = parser.parse_args()
  if options.Version:
    print """
PYGALOAD version %d.%d

This is free software licensed under the terms of the GNU General
Public License. See http://www.gnu.org/licenses/gpl.html for the license 
terms.

Andrew Sterian
Padnos College of Engineering and Computing
Grand Valley State University
<steriana@gvsu.edu> -- <http://claymore.engineer.gvsu.edu/~steriana>
""" % (VERSION_MAJOR, VERSION_MINOR)
    sys.exit(0)

  if len(args) != 1:
    parser.error("You must specify a HEX file for programming")

  datalines = readHEX(options, args)
  if not datalines:
    print '*** HEX file is empty...nothing to download'
    sys.exit(1)

  if not options.Debug:
    dev = openDevice(options)
    options.dev = dev
  
    poll = select.poll()
    poll.register(dev, select.POLLIN|select.POLLPRI)
    options.poll = poll

    if options.SendReset is not None:
      if options.Verbose:
        sys.stdout.write('Sending reset string ...\n')
      exec('s="%s"' % options.SendReset)
      os.write(options.dev, s)

    if options.Verbose:
      sys.stdout.write('Waiting for bootloader to connect ...')
      sys.stdout.flush()

    proc = doConnect(options)
  else:
    proc = Proc()
    proc.proc = 'ATmega32'
    proc.flash = 32768
    proc.boot = 512*2
    proc.page = 128
    proc.eeprom = 1024

  if proc:
    if options.Verbose:
      print 'Downloading FLASH ...'
  else:
    print '*** Downloading failed'
    sys.exit(1)

  if downloadFlash(options, proc, datalines):
    print 'Downloading successful'
  else:
    print '*** Downloading failed'
    sys.exit(1)

  sys.exit(0)
