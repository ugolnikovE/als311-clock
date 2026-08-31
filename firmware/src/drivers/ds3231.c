#include <avr/common.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "drivers/ds3231.h"
#include "hal/i2c.h"
#include "hal/systimer.h"

#define DS3231_I2C_ADDR     0x68
#define DS3231_I2C_TIME_REG 0x00
#define DS3231_I2C_TEMP_REG 0x11
#define DS3231_UPDATE_MS    10
#define DS3231_SYNC_MS      25

typedef enum {
        OP_READ,
        OP_WRITE
} ds3231_op_t;

static uint8_t buf[8];
static uint8_t sync_ctr = DS3231_SYNC_MS;
static ds3231_status_t st = DS3231_IDLE;
static ds3231_time_t b_tm;
static ds3231_op_t op;
static uint8_t pending_write = 0;
static ds3231_time_t pending_tm;

static inline uint8_t int_to_bcd(uint8_t n)
{
        return (n / 10 << 4) | (n % 10);
}

static inline uint8_t bcd_to_int(uint8_t n)
{
        return (n >> 4) * 10 + (n & 0x0F);
}

void ds3231_parse_buf()
{
        b_tm.seconds = bcd_to_int(buf[1] & 0x7F);
        b_tm.minutes = bcd_to_int(buf[2] & 0x7F);
        b_tm.hours   = bcd_to_int(buf[3] & 0x3F);
        b_tm.week    = bcd_to_int(buf[4] & 0x07);
        b_tm.date    = bcd_to_int(buf[5] & 0x3F);
        b_tm.month   = bcd_to_int(buf[6] & 0x1F);
        b_tm.year    = bcd_to_int(buf[7]);
}

static void ds3231_sync_time(void)
{
        if(!i2c_read_async(DS3231_I2C_ADDR, DS3231_I2C_TIME_REG, buf + 1, 7)) {
                st = DS3231_WORK;
                op = OP_READ;
        }
}

static void ds3231_start_write(void)
{
        buf[0] = 0x00;
        buf[1] = int_to_bcd(pending_tm.seconds);
        buf[2] = int_to_bcd(pending_tm.minutes);
        buf[3] = int_to_bcd(pending_tm.hours) & 0x3F;
        buf[4] = int_to_bcd(pending_tm.week);
        buf[5] = int_to_bcd(pending_tm.date);
        buf[6] = int_to_bcd(pending_tm.month);
        buf[7] = int_to_bcd(pending_tm.year);

        if (!i2c_write_async(DS3231_I2C_ADDR, buf, 8)) {
                st = DS3231_WORK;
                op = OP_WRITE;
        }
}


static void ds3231_update(void)
{
        i2c_status_t i2c_st = i2c_get_status();
        if (st == DS3231_WORK) {
                if (i2c_st == I2C_OK) {
                        if (op == OP_READ) {
                                if (!pending_write) ds3231_parse_buf();
                        } else {
                                pending_write = 0;
                        }
                        st = DS3231_OK;
                } else if (i2c_st == I2C_ERR) {
                        if (op == OP_WRITE) pending_write = 0;
                        st = DS3231_ERR;
                }
                return;
        }

        if (i2c_st == I2C_WORK) return;

        if (pending_write) {
                ds3231_start_write();
                return;
        }

        if (++sync_ctr >= DS3231_SYNC_MS) {
                ds3231_sync_time();
                sync_ctr = 0;
        }
}

void ds3231_init(void)
{
        systimer_register_callback(&ds3231_update, DS3231_UPDATE_MS);
}

uint8_t ds3231_set_time(const ds3231_time_t *tm)
{
        if (tm == NULL) return 1;

        uint8_t sreg = SREG;
        cli();

        pending_tm = *tm;
        pending_write = 1;
        b_tm = *tm;

        SREG = sreg;
        return 0;
}

void ds3231_get_time(ds3231_time_t *tm)
{
        uint8_t sreg = SREG;
        cli();

        *tm = b_tm;

        SREG = sreg;
}

ds3231_status_t ds3231_get_status(void)
{
        return st;
}
