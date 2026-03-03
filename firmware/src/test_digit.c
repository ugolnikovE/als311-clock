#include <avr/io.h>
#include <stdint.h>
#include "sys/timer.h"

#define SEGA    PD5
#define SEGB    PD4
#define SEGC    PB0
#define SEGD    PD6
#define SEGE    PB1
#define SEGF    PD3
#define SEGG    PD2
#define SEGDP   PD7


const uint8_t digits[10] = {
        0b00111111,
        0b00000110,
        0b01011011,
        0b01001111,
        0b01100110,
        0b01101101,
        0b01111101,
        0b00000111,
        0b01111111,
        0b01101111
};

void clear_digit()
{
        PORTD &= ~(1 << SEGA) & ~(1 << SEGB) & ~(1 << SEGD) & ~(1 << SEGF) & ~(1 << SEGG) & ~(1 << SEGDP);
        PORTB &= ~(1 << SEGC) & ~(1 << SEGE);
}

void write_digit(uint8_t segments)
{
        clear_digit();
        PORTD |= (((segments >> 0) & 1) << SEGA) | (((segments >> 1) & 1) << SEGB) | (((segments >> 3) & 1) << SEGD) |
                 (((segments >> 5) & 1) << SEGF) | (((segments >> 6) & 1) << SEGG) | (((segments >> 7) & 1) << SEGDP);
        PORTB |= (((segments >> 2) & 1) << SEGC) | (((segments >> 4) & 1) << SEGE);
}


int main(void)
{

        timer_init();

        unsigned long last_toggle = millis();

        DDRD |= (1 << SEGA) | (1 << SEGB) | (1 << SEGD) | (1 << SEGF) | (1 << SEGG) | (1 << SEGDP);
        DDRB |= (1 << SEGC) | (1 << SEGE);

        uint8_t n = 0;

        while (1) {
                if (millis() - last_toggle >= 1000) {
                        last_toggle = millis();

                        write_digit(digits[n]);

                        n = (n + 1) % 10;
                }
        }
}
