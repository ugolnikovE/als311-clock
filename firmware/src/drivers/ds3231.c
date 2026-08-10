#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "drivers/ds3231.h"
#include "hal/i2c.h"

static uint8_t buf[8] = {0};

static inline uint8_t int_to_bcd(uint8_t n)
{
        return (n / 10 << 4) | (n % 10);
}

static inline uint8_t bcd_to_int(uint8_t n)
{
        return (n >> 4) * 10 + (n & 0x0F);
}

uint8_t rtc_set_time(rtc_time_t *tm)
{
        if (tm == NULL || i2c_get_status() == I2C_WORK) {
                return 1;
        }

        buf[0] = 0x00;
        buf[1] = int_to_bcd(tm->seconds);
        buf[2] = int_to_bcd(tm->minutes);
        buf[3] = int_to_bcd(tm->hours) & 0x3F;
        buf[4] = int_to_bcd(tm->week);
        buf[5] = int_to_bcd(tm->data);
        buf[6] = int_to_bcd(tm->month);
        buf[7] = int_to_bcd(tm->year);

        i2c_write_async(0x68, buf, 8);

        return 0;
}


uint8_t rtc_get_time(rtc_time_t *tm)
{
        if (tm == NULL || i2c_get_status() == I2C_WORK) {
                return 1;
        }

        i2c_read_async(0x68, 0x00, buf + 1, 7);

        return 0;
}

void rtc_poll(rtc_time_t *tm)
{
        tm->seconds = bcd_to_int(buf[1] & 0x7F);
        tm->minutes = bcd_to_int(buf[2] & 0x7F);
        tm->hours   = bcd_to_int(buf[3] & 0x3F);
        tm->week    = bcd_to_int(buf[4] & 0x07);
        tm->data    = bcd_to_int(buf[5] & 0x3F);
        tm->month   = bcd_to_int(buf[6] & 0x1F);
        tm->year    = bcd_to_int(buf[7]);
}
