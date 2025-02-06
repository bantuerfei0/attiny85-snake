#define F_CPU 8000000UL
#include <util/delay.h>
#include "matrix.c"

#define DDRB (*(volatile unsigned char *)0x37)
#define PORTB (*(volatile unsigned char *)0x38)
#define ADMUX (*(volatile unsigned char *)0x27)
#define ADCSRA (*(volatile unsigned char *)0x26)
#define ADCH (*(volatile unsigned char *)0x25)
#define ADCL (*(volatile unsigned char *)0x24)

#define NONE 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

uint16_t analog_read();
void reset_game();
void update_snake();
void draw();
uint8_t get_direction(uint16_t input);

uint8_t snake[64];
uint8_t food;
uint8_t length = 1;
uint16_t timer = 0;
uint8_t direction = NONE;
uint8_t prev_direction = NONE; // only updated once the snake moves

int main()
{
    // registers
    ADCSRA = 0b10000000; // enables the ADC
    ADMUX = 0b00000010;  // right adjusted ADC2 (on PB4)
    DDRB = 0b000111;     // PB0, PB1, and PB2 output

    reset_game();
    while (1)
    {
        timer++;
        uint8_t temp = get_direction(analog_read());
        if (temp != NONE) {
            direction = temp;
        }
        if (timer % (1000 - 15 * length)  == 0)
        {
            update_snake();
        }
        draw();
    }
}

void draw()
{
    uint8_t i, x, y, food_x, food_y;
    for (i = 0; i < length; i++)
    {
        x = (uint8_t)(snake[i] >> 4);
        y = (uint8_t)(snake[i] & 0x0F);
        if (snake[i] != food) {
            set_led(x, y);
        }
    }
    food_x = (uint8_t)(food >> 4);
    food_y = (uint8_t)(food & 0x0F);
    if (timer % 1000 < 500) {
        set_led(food_x, food_y);
    }
}

uint8_t get_direction(uint16_t input)
{
    if (input > 1000) return NONE; // 3.28V
    else if (input > 720 && prev_direction != LEFT) return RIGHT; // 2.46
    else if (input > 600 && prev_direction != UP) return DOWN; // 2.18
    else if (input > 400 && prev_direction != RIGHT) return LEFT; // 1.64
    else if (input < 100 && prev_direction != DOWN) return UP; // 0
    return NONE;
}

void update_snake()
{
    // byte to position
    int8_t x = (uint8_t)(snake[0] >> 4);
    int8_t y = (uint8_t)(snake[0] & 0x0F);
    // get direction input
    switch (direction) {
        case UP:
            if (y == 0) {
                reset_game();
                return;
            }
            y--;
            break;
        case DOWN:
            if (y == 7) {
                reset_game();
                return;
            }
            y++;
            break;
        case LEFT:
            if (x == 0) {
                reset_game();
                return;
            }
            x--;
            break;
        case RIGHT:
            if (x == 7) {
                reset_game();
                return;
            }
            x++;
            break;
        default:
            break;
    }
    prev_direction = direction;
    // check food
    uint8_t temp_snake_head = ((x % 8) << 4 | (y % 8));
    if (temp_snake_head == food) {
        length++;
        generate_food();
    }
    // move entire snake
    uint8_t i;
    for (i = length - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }
    // move head of snake
    snake[0] = temp_snake_head;
    for (i = 1; i < length; i++)
    {
        if (snake[i] == snake[0]) {
            reset_game();
        }
    }
    if (length == 64) {
        reset_game();
    }
}

void generate_food()
{
    food = ((rand() % 8) << 4 | (rand() % 8));
}

void reset_game()
{
    length = 3;
    snake[0] = 0b00010100;
    snake[1] = 0b00010100;
    snake[2] = 0b00010100;
    generate_food();
}

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