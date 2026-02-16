#include "timer.h"
#include "config.h"
#include <avr/interrupt.h>

#define TIMER_FREQ      1000UL
#define TIMER_PRESCALER 64UL
#define TIMER_OCR_VALUE ((F_CPU / (TIMER_PRESCALER * TIMER_FREQ)) - 1)

static volatile unsigned long tick = 0;


// Timer interrupt
ISR(TIMER0_COMPA_vect)
{
        tick++;
}


void timer_init(void)
{
        cli();

        TCNT0  = 0;
        OCR0A  = TIMER_OCR_VALUE;
        TCCR0A = (1 << WGM01);
        TCCR0B = (1 << CS01) | (1 << CS00);
        TIMSK0 = (1 << OCIE0A);

        sei();
}


unsigned long millis(void)
{
        unsigned long t;
        cli();
        t = tick;
        sei();
        return t;
}
