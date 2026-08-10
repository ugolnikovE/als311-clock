#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include "hal/systimer.h"
#include "config.h"

// 1ms tick on 16MHz crystall
#define TIMER_FREQ      1000UL
#define TIMER_PRESCALER 64UL
#define TIMER_OCR_VALUE ((F_CPU / (TIMER_PRESCALER * TIMER_FREQ)) - 1)

static volatile uint32_t tick = 0;


#define MAX_CALLBACKS 4

// Callback slot: fires fn() every `period` ticks
typedef struct {
        systimer_callback_t fn;
        uint16_t            period;
        uint16_t            counter;
} callback_slot_t;

static callback_slot_t slots[MAX_CALLBACKS];
static uint8_t         slots_count = 0;



ISR(TIMER0_COMPA_vect)
{
        tick++;

        // dispatch registered callbacks
        for (uint8_t i = 0; i < slots_count; i++) {
                if (++slots[i].counter >= slots[i].period) {
                        slots[i].counter = 0;
                        slots[i].fn();
                }
        }
}

void systimer_init()
{
        TCNT0  = 0;
        OCR0A  = TIMER_OCR_VALUE;
        TCCR0A = (1 << WGM01);
        TCCR0B = (1 << CS01) | (1 << CS00);
        TIMSK0 = (1 << OCIE0A);
}

uint32_t systimer_millis()
{
        uint32_t t;
        uint8_t sreg = SREG;
        cli();
        t = tick;
        SREG = sreg;
        return t;
}

uint8_t systimer_register_callback(systimer_callback_t cb, uint16_t period)
{
        if (slots_count >= MAX_CALLBACKS || cb == NULL) return 1;

        slots[slots_count].fn      = cb;
        slots[slots_count].period  = period;
        slots[slots_count].counter = 0;
        slots_count++;
        return 0;
}
