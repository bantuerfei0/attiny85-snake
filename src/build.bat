@echo off
echo cleaning
del ".\main.out"
del ".\main.hex"
echo compiling
avr-gcc -O2 -mmcu=attiny85 main.c -o main.out -Wall -Wextra
echo converting out to ihex
avr-objcopy -O ihex -j .text -j .data main.out main.hex
avrdude -P COM7 -p t85 -V -c arduino -U flash:w:main.hex:i