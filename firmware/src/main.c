#include <avr/io.h>
#include "hal/systimer.h"
#include "hal/gpio.h"

#define LED_PIN  PB5
#define LED_PORT &PORTB

static void led_fn(void)
{
        gpio_pin_toggle(LED_PORT, LED_PIN);
}

int main(void)
{
        systimer_init();

        gpio_pin_output(LED_PORT, LED_PIN);

        systimer_register_callback(&led_fn, 500);

        while(1);
}
