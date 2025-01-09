#define F_CPU 8000000UL
#include <util/delay.h>
#include "matrix.c"

#define DDRB (*(volatile unsigned char *)0x37)
#define PORTB (*(volatile unsigned char *)0x38)
#define ADMUX (*(volatile unsigned char *)0x27)
#define ADCSRA (*(volatile unsigned char *)0x26)
#define ADCH (*(volatile unsigned char *)0x25)
#define ADCL (*(volatile unsigned char *)0x24)

uint16_t analog_read()
{
    ADCSRA |= (1 << 6); // start conversion
    // wait for conversion
    while ((ADCSRA >> 6) & 1)
    {
    }
    // ADCL must be read first, ADLAR bit in ADMUX is 0
    return ADCL | (ADCH << 8);
}

int main()
{
    // registers
    ADCSRA = 0b10000000; // enables the ADC
    ADMUX = 0b00000010;  // right adjusted ADC2 (on PB4)
    DDRB = 0b000111;     // PB0, PB1, and PB2 output

    uint8_t i;
    uint8_t j;
    uint16_t input;
    while (1)
    {
        input = analog_read();
        if (input > 1000)
        {
            set_led(3, 3);
            set_led(3, 4);
            set_led(4, 3);
            set_led(4, 4);
        }
        else if (input > 700)
        {
            // right
            set_led(5, 3);
            set_led(6, 3);
            set_led(7, 3);
            set_led(5, 4);
            set_led(6, 4);
            set_led(7, 4);
        }
        else if (input > 600)
        {
            // down
            set_led(3, 5);
            set_led(3, 6);
            set_led(3, 7);
            set_led(4, 5);
            set_led(4, 6);
            set_led(4, 7);
        }
        else if (input > 400)
        {
            // left
            set_led(0, 3);
            set_led(1, 3);
            set_led(2, 3);
            set_led(0, 4);
            set_led(1, 4);
            set_led(2, 4);
        }
        else if (input <= 400)
        {
            // up
            set_led(3, 0);
            set_led(3, 1);
            set_led(3, 2);
            set_led(4, 0);
            set_led(4, 1);
            set_led(4, 2);
        }
    }
}
