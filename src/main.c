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
    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                set_led(j, i);
                _delay_ms(10);
            }
        }
    }
}
