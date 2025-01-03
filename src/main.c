#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdbool.h>
#define DDRB (*(volatile unsigned char *)0x37)
#define PORTB (*(volatile unsigned char *)0x38)

/**
 * Clocks out 16 bits meant for two daisy-chained shift registers
 * in order to light up one LED on a 8x8 LED matrix
 *
 * Contains values specific for the project.
 */
void light_led(uint8_t x, uint8_t y)
{
    // "maps" used to map the right row/column to the correct pin
    // the pins on the 3x3 matrix are actually diabolical in the way
    // they're arranged.
    static const uint8_t x_map[] = {5, 2, 7, 1, 12, 8, 14, 9};
    static const uint8_t y_map[] = {13, 3, 4, 10, 6, 11, 15, 16};

    // the bit position for the corresponding x, y
    uint8_t x_shift = x_map[x];
    uint8_t y_shift = y_map[y];

    // initialize the output to have all the LEDs off
    // this value is magic (it is specific to the way the 8x8 was wired)
    // the 16th bit is on the very right
    uint16_t shift_output = 0b0011010001101011;

    shift_output |= (1 << (16 - x_shift)); // set x bit
    shift_output ^= (1 << (16 - y_shift)); // set y bit

    uint8_t i;
    for (i = 0; i < 16; i++)
    {
        // take LSB and output to correct pin
        bool output = shift_output & 1;
        PORTB = (output << 2); // PB2
        // clock SRCLK
        PORTB |= 0b000010;
        PORTB = 0b000000;
        // shift right for the next bit
        shift_output >>= 1;
    }

    // clock RCLK on the shift registers
    PORTB = 0b000001;
    PORTB = 0b000000;
}

int main()
{
    // PB0, PB1, and PB2 output
    DDRB = 0b000111;

    // simple iteration through all the leds in the 8x8 matrix
    uint8_t i, j;
    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                light_led(j, i);
                _delay_ms(10);
            }
        }
    }
}