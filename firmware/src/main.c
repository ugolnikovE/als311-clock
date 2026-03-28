#include <avr/interrupt.h>
#include "hal/systimer.h"
#include "drivers/als311.h"

int main(void)
{
        cli();

        systimer_init();
        als_init();

        sei();

        als_write("12:34");

        while(1);
}
