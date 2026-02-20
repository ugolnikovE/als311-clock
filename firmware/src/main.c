#include <avr/io.h>
#include "sys/timer.h"

#define DS      PD2
#define SH_CP   PD3
#define ST_CP   PD4


int main(void)
{

        timer_init();

        unsigned long last_toggle = millis();

        unsigned short int b = 0b00000011;

        DDRD |= (1 << DS) | (1 << ST_CP) | (1 << SH_CP);

        PORTD |= (0 << ST_CP);

        while (1) {
                if (millis() - last_toggle >= 500) {
                        last_toggle = millis();

                        for (int i = 7; i > -1; i--) {
                                PORTD &= ~(1 << SH_CP);
                                PORTD &= ~(1 << DS);
                                PORTD |= (((b >> i) & 1) << DS);
                                PORTD |= (1 << SH_CP);
                        }
                        PORTD |= (1 << ST_CP);
                        PORTD &= ~(1 << ST_CP);

                        b = b << 2;

                        if (b == 0b00110000) {
                                b = 0b00000011;
                        }
                }
        }
}
