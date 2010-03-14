Robopoly Linux documentation
============================

## How to install the necessary tools for programming the PRisme on Linux

More information at [Robopoly](http://robopoly.epfl.ch)

this project currently lives at http://github.com/tunebird/robopoly-linux

to get the latest version, use the following command:
    git clone git://github.com/tunebird/robopoly-linux.git

to compile docs and contribute, easiest is to install the texlive
distribution (if disk space is not an issue)
    pacman -S texlive-most
or
    sudo apt-get install texlive-full

## What's included:
- Complete LaTeX source for documentation
- Robopoly libraries which compile with avr-gcc
- Pygaload script for downloading to flash
- Example project with working Makefile


## Current TODO : 

- add some documentation on tools to install

### Later : 

- add a script for creating a new project from a template
- possibly document setting up Eclipse as an IDE for avr
- translate this to french :)
