#include <avr/interrupt.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include "drivers/ds3231.h"
#include "hal/systimer.h"
#include "hal/i2c.h"
#include "drivers/als311.h"
#include "drivers/button.h"
#include "app/clock.h"

#define CLOCK_STR_LEN 6
#define CLOCK_SETTING_COUNT 5
#define CLOCK_TRANSITION_COUNT 10

typedef struct {
        clock_state_t    current;
        btn_name_t       btn_name;
        btn_event_type_t btn_event;
        clock_state_t    next;
        void (*action)(void);
} transition_t;

typedef struct {
        void (*format)(ds3231_time_t *, char *);
        uint8_t blink_pos;
        uint8_t blink_len;
        uint8_t *field;
        uint8_t min, max;
} setting_step_t;

typedef struct {
        char          buf[CLOCK_STR_LEN];
        ds3231_time_t tm;
        btn_event_t   e;
        clock_state_t st;
        uint32_t      sec_toggle_time;
        uint8_t       need_sep;
        uint8_t       setting_ctr;
        uint8_t       setting_need_off;
        uint32_t      setting_toggle_time;
} clock_t;

static clock_t clk = {
        .buf = "--:--\0",
        .e = { BTN_1, BTN_EVENT_NONE },
        .tm = { 0 },
        .st = CLOCK_TIME,
        .sec_toggle_time = 0,
        .need_sep = 1,
};

static void clock_string_formatter(char *format, char *buf, ...)
{
        va_list arg_list;
        va_start(arg_list, buf);

        uint8_t i = 0;
        for (; format[i] != '\0'; i++) {
                if (format[i] == 'Q' && format[i+1] == 'Q') {
                        uint8_t n = (uint8_t)va_arg(arg_list, int);
                        buf[i]   = '0' + n / 10;
                        buf[i+1] = '0' + n % 10;
                        i++;
                } else {
                        buf[i] = format[i];
                }
        }
        buf[i] = '\0';

        va_end(arg_list);
}

static void time_to_str(ds3231_time_t *tm, char *buf)
{
        clock_string_formatter("QQ:QQ", clk.buf, tm->hours, tm->minutes);
}

static void date_to_str(ds3231_time_t *tm, char *buf)
{
        clock_string_formatter("QQ.QQ", clk.buf, tm->date, tm->month);
}

static void year_to_str(ds3231_time_t *tm, char *buf)
{
        clock_string_formatter("Y: QQ", clk.buf, tm->year);
}

static setting_step_t clock_setting_steps[] = {
        { time_to_str, 0, 2, &clk.tm.hours, 0, 23 },
        { time_to_str, 3, 2, &clk.tm.minutes, 0, 59 },
        { date_to_str, 0, 2, &clk.tm.date, 1, 31 },
        { date_to_str, 3, 2, &clk.tm.month, 1, 12 },
        { year_to_str, 3, 2, &clk.tm.year, 0, 99 },
};

static void setting_sync_time(void)
{
        clk.tm.seconds = 0;
        ds3231_set_time(&clk.tm);
}

static void setting_apply(int8_t v)
{
        setting_step_t *step = &clock_setting_steps[clk.setting_ctr];

        int16_t span = step->max - step->min + 1;
        int16_t pos = *step->field - step->min;

        pos += v;

        pos = ((pos % span) + span) % span;

        *step->field = pos + step->min;
}

static void setting_plus_one(void)
{
        setting_apply(1);
}

static void setting_minus_one(void)
{
        setting_apply(-1);
}

static void setting_plus_five(void)
{
        setting_apply(5);
}

static void setting_minus_five(void)
{
        setting_apply(-5);
}

static void setting_zero_ctr(void)
{
        clk.setting_ctr = 0;
}

static void setting_move_ctr(void)
{
        clk.setting_ctr = (clk.setting_ctr + 1) % CLOCK_SETTING_COUNT;
}

static void clock_st_time(void)
{
        ds3231_get_time(&clk.tm);
        if (systimer_millis() - clk.sec_toggle_time >= 1000) {
                clk.sec_toggle_time = systimer_millis();
                clk.need_sep ^= 1;
        }
        time_to_str(&clk.tm, clk.buf);
        clk.buf[2] = (clk.need_sep) ? ':' : ' ';
        als_write(clk.buf);
}

static void clock_st_date(void)
{
        ds3231_get_time(&clk.tm);
        date_to_str(&clk.tm, clk.buf);
        als_write(clk.buf);
}

static void clock_st_setting(void)
{
        if (systimer_millis() - clk.setting_toggle_time >= 500) {
                clk.setting_toggle_time = systimer_millis();
                clk.setting_need_off ^= 1;
        }

        setting_step_t *step = &clock_setting_steps[clk.setting_ctr];

        step->format(&clk.tm, clk.buf);
        if (clk.setting_need_off) {
                for (uint8_t i = 0; i < step->blink_len; i++) {
                        clk.buf[step->blink_pos + i] = ' ';
                }
        }
        als_write(clk.buf);
}

static transition_t clock_transition_table[] = {
        { CLOCK_TIME, BTN_MODE, BTN_EVENT_CLICK, CLOCK_DATE, NULL },
        { CLOCK_TIME, BTN_MODE, BTN_EVENT_LONG, CLOCK_SETTING, setting_zero_ctr },
        { CLOCK_DATE, BTN_MODE, BTN_EVENT_CLICK, CLOCK_TIME, NULL },
        { CLOCK_DATE, BTN_MODE, BTN_EVENT_LONG, CLOCK_SETTING, setting_zero_ctr },
        { CLOCK_SETTING, BTN_MODE, BTN_EVENT_LONG, CLOCK_TIME, setting_sync_time },
        { CLOCK_SETTING, BTN_MODE, BTN_EVENT_CLICK, CLOCK_SETTING, setting_move_ctr },
        { CLOCK_SETTING, BTN_1, BTN_EVENT_CLICK, CLOCK_SETTING, setting_minus_one },
        { CLOCK_SETTING, BTN_2, BTN_EVENT_CLICK, CLOCK_SETTING, setting_plus_one },
        { CLOCK_SETTING, BTN_1, BTN_EVENT_REPEAT, CLOCK_SETTING, setting_minus_five },
        { CLOCK_SETTING, BTN_2, BTN_EVENT_REPEAT, CLOCK_SETTING, setting_plus_five },
};

static void clock_make_transition(void)
{
        for (uint8_t i = 0; i < CLOCK_TRANSITION_COUNT; i++) {
                const transition_t *t = &clock_transition_table[i];

                if (t->current == clk.st &&
                    t->btn_name == clk.e.name &&
                    t->btn_event == clk.e.type) {
                        if (t->action != NULL)  {
                                t->action();
                        }
                        clk.st = t->next;
                        break;
                    }
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
}

void clock_update(void)
{
        btn_get_event(&clk.e);
        clock_make_transition();

        switch (clk.st) {
                case CLOCK_TIME: clock_st_time(); break;
                case CLOCK_DATE: clock_st_date(); break;
                case CLOCK_SETTING: clock_st_setting(); break;
                default: als_write("error"); break;
        }
}
