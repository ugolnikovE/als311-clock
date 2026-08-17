#include <avr/interrupt.h>
#include <stdint.h>

#include "drivers/ds3231.h"
#include "hal/systimer.h"
#include "hal/i2c.h"
#include "drivers/als311.h"
#include "drivers/button.h"
#include "app/clock.h"

#define CLOCK_STR_LEN 6

typedef struct {
        char          buf[CLOCK_STR_LEN];
        ds3231_time_t tm;
        btn_event_t   e;
        clock_state_t st;
        uint32_t      sec_toggle_time;
        uint8_t       need_sep;
} clock_t;

static clock_t clk = {
        .buf = "--:--\0",
        .e = { BTN_1, BTN_EVENT_NONE },
        .tm = { 0 },
        .st = CLOCK_TIME,
        .sec_toggle_time = 0,
        .need_sep = 1,
};

static void ds3231_time_to_str(ds3231_time_t *tm, char *buf, uint8_t need_sep)
{
        buf[0] = '0' + tm->hours / 10;
        buf[1] = '0' + tm->hours % 10;
        buf[2] = (need_sep) ? ':' : ' ';
        buf[3] = '0' + tm->minutes / 10;
        buf[4] = '0' + tm->minutes % 10;
        buf[CLOCK_STR_LEN - 1] = '\0';
}

static void clock_step()
{
        switch (clk.st) {
                case CLOCK_TIME:
                        
                        
                        break;

                case CLOCK_DATE:

                        break;

                case CLOCK_SETTING:

                        break;

                default:
                        break;
        }
}

void clock_setup(void)
{
        cli();

        systimer_init();
        i2c_init();
        ds3231_init();
        als_init();
        btn_init();
        sei();
        
        clk.sec_toggle_time = systimer_millis();
        ds3231_get_time(&clk.tm);
}

void clock_update(void)
{
        btn_get_event(&clk.e);
        clock_step();
}
