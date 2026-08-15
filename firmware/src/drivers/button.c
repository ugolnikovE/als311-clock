#include <avr/interrupt.h>
#include <stdint.h>

#include "drivers/button.h"
#include "config.h"
#include "hal/gpio.h"
#include "hal/systimer.h"

#define DEBOUNCE_MAX 4
#define PRESSED_LIM  25
#define REPEATED_TICKS 25

typedef enum {
        BTN_LEVEL_UP,
        BTN_LEVEL_DOWN,
} btn_level_t;

typedef struct {
        btn_name_t        name;
        btn_level_t       level;
        btn_event_type_t  event;
        volatile uint8_t *port;
        uint8_t           pin;
        uint8_t           integ;
        uint8_t           pressed_tick;
        uint8_t           repeated_tick;
} btn_t;


static btn_t btns[BTN_COUNT] = {
        { BTN_1,    BTN_LEVEL_UP, BTN_EVENT_NONE, &BTN_1_PORT, BTN_1_PIN, 0, 0, 0 },
        { BTN_2,    BTN_LEVEL_UP, BTN_EVENT_NONE, &BTN_2_PORT, BTN_2_PIN, 0, 0, 0 },
        { BTN_MODE, BTN_LEVEL_UP, BTN_EVENT_NONE, &BTN_3_PORT, BTN_3_PIN, 0, 0, 0 }
};

void btn_update(void)
{
        for (uint8_t i = 0; i < BTN_COUNT; i++) {
                uint8_t down = gpio_pin_read(btns[i].port, btns[i].pin);

                // Debounce
                if (down) {
                        if (btns[i].integ < DEBOUNCE_MAX) btns[i].integ++;
                } else {
                        if (btns[i].integ > 0) btns[i].integ--;
                }

                if (btns[i].integ == DEBOUNCE_MAX) {
                        btns[i].level = BTN_LEVEL_DOWN;
                } else if (btns[i].integ == 0) {
                        if (btns[i].level == BTN_LEVEL_DOWN && btns[i].pressed_tick < PRESSED_LIM) {
                                btns[i].event = BTN_EVENT_CLICK;
                        }
                        btns[i].level = BTN_LEVEL_UP;
                        btns[i].pressed_tick = 0;
                        btns[i].repeated_tick = 0;
                }

                if (btns[i].level == BTN_LEVEL_DOWN) {
                        if (btns[i].pressed_tick < PRESSED_LIM) {
                                btns[i].pressed_tick++;
                                if (btns[i].pressed_tick == PRESSED_LIM) {
                                        btns[i].event = BTN_EVENT_LONG;
                                }
                        } else {
                                btns[i].repeated_tick++;
                                if (btns[i].repeated_tick >= REPEATED_TICKS && btns[i].event == BTN_EVENT_NONE) {
                                        btns[i].event = BTN_EVENT_REPEAT;
                                        btns[i].repeated_tick = 0;
                                }
                        }
                }
        }
}

void btn_init(void)
{
        for (uint8_t i = 0; i < BTN_COUNT; i++) {
                gpio_pin_input(btns[i].port, btns[i].pin);
        }

        systimer_register_callback(btn_update, 20);
}

void btn_get_event(btn_event_t* e)
{
        uint8_t sreg = SREG;
        cli();

        e->type = BTN_EVENT_NONE;

        for (uint8_t i = 0; i < BTN_COUNT; i++) {
                if (btns[i].event != BTN_EVENT_NONE) {
                        e->name = btns[i].name;
                        e->type = btns[i].event;
                        btns[i].event = BTN_EVENT_NONE;
                        break;
                }
        }

        SREG = sreg;
}
