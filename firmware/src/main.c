#include <avr/io.h>
#include "sys/timer.h"

int main(void)
{

        timer_init();

        unsigned long last_toggle = millis();

        DDRB |= (1 << PB5);
        while (1) {
                if (millis() - last_toggle >= 1000) {
                        last_toggle = millis();
                        PORTB ^= (1 << PB5);
                }
        }
}
