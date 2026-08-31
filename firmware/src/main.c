#include <avr/interrupt.h>
#include "app/clock.h"

int main(void)
{
        clock_setup();
        while(1) {
                clock_update();
        }
}
