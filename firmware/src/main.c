#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned long millis = 0;

int main(void)
{
        cli();

        TCNT0  = 0;
        OCR0A  = 249;
        TCCR0A = (1 << WGM01);
        TCCR0B = (1 << CS01) | (1 << CS00);
        TIMSK0 = (1 << OCIE0A);


        sei();

        cli();
        unsigned long last_toggle = millis;
        sei();

        DDRB |= (1 << PB5);
        while (1) {
                if (millis - last_toggle >= 1000) {
                        cli();
                        last_toggle = millis;
                        sei();
                        PORTB ^= (1 << PB5);
                }
        }
}

ISR(TIMER0_COMPA_vect)
{
        millis += 1;
}
