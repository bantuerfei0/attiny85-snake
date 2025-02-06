/**
 * Alex Liu
 * 1/4/2025
 *
 * Made specifically for the ATTiny85-Snake project
 *
 * Functions for working with the 788Bs 8x8 LED Matrix and 2 x SN74HC595N Shift Registers
 */

#include <inttypes.h>

#define PORTB (*(volatile unsigned char *)0x38)

/**
 * Shifts out 16-bit data
 *
 * @param data 16-bits of data to shift out
 */
void shift_out(uint16_t data)
{
    uint8_t i;
    for (i = 0; i < 16; i++)
    {
        uint8_t output = data & 1;
        PORTB = (output << 2); // set PB2
        // clock SRCLK
        PORTB |= (1 << 1);
        PORTB &= ~(1 << 1);
        data >>= 1; // next bit
    }
    // clock RCLK on the shift registers
    PORTB |= 1;
    PORTB &= ~1;
}

/**
 * Set an LED on a 8x8 Matrix
 *
 * @param x LED position 0 -> 7
 * @param y LED position 0 -> 7
 */
void set_led(uint8_t x, uint8_t y)
{
    // not sure if this is better than having 2 arrays
    // [7:4] x [3:0] y
    static const uint_least8_t xy_map[] = {
        0b01001100, // 4 12
        0b00010010, // 1 2
        0b01100011, // 6 3
        0b00001001, // 0 9
        0b10110101, // 11 5
        0b01111010, // 7 10
        0b11011110, // 13 14
        0b10001111  // 8 15
    };
    uint8_t x_shift = xy_map[x] >> 4;
    uint8_t y_shift = xy_map[y] & 0x0F;
    // all off
    uint16_t data = 0b1100101110010100;
    data |= (1 << (15 - y_shift));  // set y
    data &= ~(1 << (15 - x_shift)); // unset x
    shift_out(data);
}