#define F_CPU 8000000UL
#include <util/delay.h>

#define DDRB (*(volatile unsigned char *)0x37)
#define PORTB (*(volatile unsigned char *)0x38)
#define ADMUX (*(volatile unsigned char *)0x27)
#define ADCSRA (*(volatile unsigned char *)0x26)
#define ADCH (*(volatile unsigned char *)0x25)
#define ADCL (*(volatile unsigned char *)0x24)

void shift_out(uint16_t data)
{
    uint8_t i;
    for (i = 0; i < 16; i++)
    {
        // take LSB and output to correct pin
        uint8_t output = data & 1;
        PORTB = (output << 2); // PB2
        // clock SRCLK
        PORTB |= 0b000010;
        PORTB = 0b000000;
        // shift right for the next bit
        data >>= 1;
    }

    // clock RCLK on the shift registers
    PORTB = 0b000001;
    PORTB = 0b000000;
}

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

    shift_out(shift_output);
}

uint16_t read_input()
{
    ADCSRA |= 0b01000000; // start conversion
    // hang while not done converting
    while ((ADCSRA >> 6) & 1)
    {
    }
    // adcl must be read first according to datasheet
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    // ADLAR bit in ADMUX is 0
    return (high << 8) | low;
}

int main()
{
    ADCSRA = 0b10000000;
    // right adjusted ADC2 (on PB4)
    ADMUX = 0b00000010;
    // PB0, PB1, and PB2 output
    DDRB = 0b000111;

    // simple iteration through all the leds in the 8x8 matrix
    uint8_t i, j;
    uint8_t accum = 0;
    while (1)
    {
        // 3.3v = 1024 2.47 = 766 2.20 = 682 1.65 = 512
        uint16_t value = read_input(); // 0b1111111111111111
        if (value >= 1000)
        {
            light_led(3, 3);
            light_led(3, 4);
            light_led(4, 3);
            light_led(4, 4);
        }
        else if (value >= 720)
        {
            // RIGHT
            light_led(5, 3);
            light_led(6, 3);
            light_led(7, 3);
            light_led(5, 4);
            light_led(6, 4);
            light_led(7, 4);
        }
        else if (value >= 640)
        {
            // DOWN
            light_led(3, 5);
            light_led(3, 6);
            light_led(3, 7);
            light_led(4, 5);
            light_led(4, 6);
            light_led(4, 7);
        }
        else if (value >= 400)
        {
            // LEFT
            light_led(0, 3);
            light_led(1, 3);
            light_led(2, 3);
            light_led(0, 4);
            light_led(1, 4);
            light_led(2, 4);
        }
        else
        {
            // TOP
            light_led(3, 0);
            light_led(3, 1);
            light_led(3, 2);
            light_led(4, 0);
            light_led(4, 1);
            light_led(4, 2);
        }
        light_led(8, 8);
    }
}