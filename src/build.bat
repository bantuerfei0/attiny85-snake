@echo off
echo cleaning
del ".\a.out"
del ".\a.hex"
echo compiling
avr-gcc -Os -mmcu=attiny85 main.c
echo converting out to ihex
avr-objcopy -O ihex -j .text -j .data a.out a.hex
avrdude -P COM3 -p t85 -V -c arduino -U flash:w:a.hex:i