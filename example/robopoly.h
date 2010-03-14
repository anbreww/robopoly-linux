/***************************************************************************************
 *
 * Libraire C de Robopoly v1.0
 * Fichier: robopoly.c
 * Date: 13.08.2008 
 * Auteur(s): Christophe Winter, Thierry Barras
 * 
 * Cette librairie a été crée par le club de robotique de l'EPFL Robopoly. 
 * Cette librairie est distribuée gratuitement aux membres de Robopoly et les sources 
 * appartiennent à Robopoly.
 *
 ***************************************************************************************/



#ifndef _ROBOPOLY_H
#define _ROBOPOLY_H
 
typedef struct
	{
	   unsigned char 				bit0 : 1,
									bit1 : 1,
									bit2 : 1,
									bit3 : 1,
									bit4 : 1,
									bit5 : 1,
									bit6 : 1,
									bit7 : 1;
	} bit_field;

#define Bit(port) (*(volatile bit_field *) (_SFR_ADDR(port)))

//	BUS A
#define	_PORTA0	Bit(PORTA).bit0
#define	_PORTA1	Bit(PORTA).bit1
#define	_PORTA2	Bit(PORTA).bit2
#define	_PORTA3	Bit(PORTA).bit3
#define	_PORTA4	Bit(PORTA).bit4
#define	_PORTA5	Bit(PORTA).bit5
#define	_PORTA6	Bit(PORTA).bit6
#define	_PORTA7	Bit(PORTA).bit7

#define _DDRA0	Bit(DDRA).bit0
#define _DDRA1	Bit(DDRA).bit1
#define _DDRA2	Bit(DDRA).bit2
#define _DDRA3	Bit(DDRA).bit3
#define _DDRA4	Bit(DDRA).bit4
#define _DDRA5	Bit(DDRA).bit5
#define _DDRA6	Bit(DDRA).bit6
#define _DDRA7	Bit(DDRA).bit7

#define _PINA0	Bit(PINA).bit0
#define _PINA1	Bit(PINA).bit1
#define _PINA2	Bit(PINA).bit2
#define _PINA3	Bit(PINA).bit3
#define _PINA4	Bit(PINA).bit4
#define _PINA5	Bit(PINA).bit5
#define _PINA6	Bit(PINA).bit6
#define _PINA7	Bit(PINA).bit7

//	BUS B
#define	_PORTB0	Bit(PORTB).bit0
#define	_PORTB1	Bit(PORTB).bit1
#define	_PORTB2	Bit(PORTB).bit2
#define	_PORTB3	Bit(PORTB).bit3
#define	_PORTB4	Bit(PORTB).bit4
#define	_PORTB5	Bit(PORTB).bit5
#define	_PORTB6	Bit(PORTB).bit6
#define	_PORTB7	Bit(PORTB).bit7

#define _DDRB0	Bit(DDRB).bit0
#define _DDRB1	Bit(DDRB).bit1
#define _DDRB2	Bit(DDRB).bit2
#define _DDRB3	Bit(DDRB).bit3
#define _DDRB4	Bit(DDRB).bit4
#define _DDRB5	Bit(DDRB).bit5
#define _DDRB6	Bit(DDRB).bit6
#define _DDRB7	Bit(DDRB).bit7

#define _PINB0	Bit(PINB).bit0
#define _PINB1	Bit(PINB).bit1
#define _PINB2	Bit(PINB).bit2
#define _PINB3	Bit(PINB).bit3
#define _PINB4	Bit(PINB).bit4
#define _PINB5	Bit(PINB).bit5
#define _PINB6	Bit(PINB).bit6
#define _PINB7	Bit(PINB).bit7

//	BUS C
#define	_PORTC0	Bit(PORTC).bit0
#define	_PORTC1	Bit(PORTC).bit1
#define	_PORTC2	Bit(PORTC).bit2
#define	_PORTC3	Bit(PORTC).bit3
#define	_PORTC4	Bit(PORTC).bit4
#define	_PORTC5	Bit(PORTC).bit5
#define	_PORTC6	Bit(PORTC).bit6
#define	_PORTC7	Bit(PORTC).bit7

#define _DDRC0	Bit(DDRC).bit0
#define _DDRC1	Bit(DDRC).bit1
#define _DDRC2	Bit(DDRC).bit2
#define _DDRC3	Bit(DDRC).bit3
#define _DDRC4	Bit(DDRC).bit4
#define _DDRC5	Bit(DDRC).bit5
#define _DDRC6	Bit(DDRC).bit6
#define _DDRC7	Bit(DDRC).bit7

#define _PINC0	Bit(PINC).bit0
#define _PINC1	Bit(PINC).bit1
#define _PINC2	Bit(PINC).bit2
#define _PINC3	Bit(PINC).bit3
#define _PINC4	Bit(PINC).bit4
#define _PINC5	Bit(PINC).bit5
#define _PINC6	Bit(PINC).bit6
#define _PINC7	Bit(PINC).bit7

//	BUS D
#define	_PORTD0	Bit(PORTD).bit0
#define	_PORTD1	Bit(PORTD).bit1
#define	_PORTD2	Bit(PORTD).bit2
#define	_PORTD3	Bit(PORTD).bit3
#define	_PORTD4	Bit(PORTD).bit4
#define	_PORTD5	Bit(PORTD).bit5
#define	_PORTD6	Bit(PORTD).bit6
#define	_PORTD7	Bit(PORTD).bit7

#define _DDRD0	Bit(DDRD).bit0
#define _DDRD1	Bit(DDRD).bit1
#define _DDRD2	Bit(DDRD).bit2
#define _DDRD3	Bit(DDRD).bit3
#define _DDRD4	Bit(DDRD).bit4
#define _DDRD5	Bit(DDRD).bit5
#define _DDRD6	Bit(DDRD).bit6
#define _DDRD7	Bit(DDRD).bit7

#define _PIND0	Bit(PIND).bit0
#define _PIND1	Bit(PIND).bit1
#define _PIND2	Bit(PIND).bit2
#define _PIND3	Bit(PIND).bit3
#define _PIND4	Bit(PIND).bit4
#define _PIND5	Bit(PIND).bit5
#define _PIND6	Bit(PIND).bit6
#define _PIND7	Bit(PIND).bit7

#define A		'A'
#define B		'B'
#define C		'C'
#define D 		'D'
#define BYTE 	0xFF

#define F_CPU 	8000000
#define BAUD  	9600

// Fonctions

//#define lineWrite(port,bit, value)		{_SFR_IO8(_SFR_IO_ADDR(port)-1) |= (1<<bit); port = (port & (~(1<<bit))) + (value << bit);}
//#define lineRead(port, bit, result)		{_SFR_IO8(_SFR_IO_ADDR(port)-1) &= ~(1<<bit); result = (_SFR_IO8(_SFR_IO_ADDR(port)-2) >> bit) & 1;}
void digitalWrite (unsigned char port, unsigned char bit, unsigned char value);
unsigned char digitalRead (unsigned char port, unsigned char bit);
unsigned char analogReadPortA(unsigned char bit);

void waitms(unsigned int iter);
void waitus(unsigned char iter);


void uartSendByte(unsigned char a);
void uartSendString(const char *text);
unsigned char uartGetByte(void);


void setupMotorPWM(int vLeft, int vRight);

/// To disable all agenda structure and functions, juste add the '-D noagenda=definition' compilation rule to entire project.
#ifndef	noagenda
char addNewCallback(void (* newcallbackaddr)(void), unsigned int duration, unsigned char executionNumber);
void stopCallback(char callbackNumber);
#endif



/// To disable all servo structure and functions, juste add the '-D noservo=definition' compilation rule to entire project.
#ifndef noservo
//Definition des emplacements des servos. 
//Pour modifier un emplacement (ligne), changer _portXX ainsi que _ddrXX ci dessous. 

#define		SERVO_0  		_PORTC3
#define 	SERVO_0_DIR		_DDRC3

#define		SERVO_1  		_PORTC4
#define 	SERVO_1_DIR		_DDRC4

#define		SERVO_2  		_PORTC5
#define 	SERVO_2_DIR		_DDRC5

#define		SERVO_3  		_PORTC6
#define 	SERVO_3_DIR		_DDRC6

#define		SERVO_4  		_PORTC7
#define 	SERVO_4_DIR		_DDRC7

#define		SERVO_5  		_PORTB0
#define 	SERVO_5_DIR		_DDRB0

#define		SERVO_6  		_PORTB1
#define 	SERVO_6_DIR		_DDRB1

#define		SERVO_7  		_PORTB2
#define 	SERVO_7_DIR		_DDRB2
	
#define		SERVO_8  		_PORTB3
#define 	SERVO_8_DIR		_DDRB3

#define		SERVO_9  		_PORTB4
#define 	SERVO_9_DIR		_DDRB4	
#endif

void set_servo(unsigned char num_servo, char angle_servo);
#endif
