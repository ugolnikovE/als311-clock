#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdint.h>
#include <string.h>

#include "hal/i2c.h"
#include "config.h"

typedef struct {
        uint8_t         addr;
        uint8_t         reg;
        uint8_t        *ptr;
        uint8_t         sz;
        uint8_t         counter;
        uint8_t         is_read;
        volatile i2c_status_t st;
} i2c_ctx_t;

static i2c_ctx_t ctx = { .st = I2C_IDLE };

void i2c_init()
{
	TWSR = 0x00;
	TWBR = (F_CPU / I2C_FREQUENCY - 16) / 2;
}

static inline void i2c_start()
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
}

static inline void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

static inline void i2c_step()
{
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
}

static inline void i2c_step_ack()
{
        TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

i2c_status_t i2c_get_status(void)
{
        return ctx.st;
}

uint8_t i2c_read_async(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t size)
{
        if ((data == NULL) || (size == 0) || (ctx.st == I2C_WORK))
                return 1;

        ctx.addr        = addr;
        ctx.reg         = reg;
        ctx.ptr         = data;
        ctx.sz          = size;
        ctx.counter     = 0;
        ctx.is_read     = 1;
        ctx.st          = I2C_WORK;
        i2c_start();
        return 0;
}

uint8_t i2c_write_async(uint8_t addr, const uint8_t *data, uint8_t size)
{
        if ((data == NULL) || (size == 0) || (ctx.st == I2C_WORK))
                return 1;

        ctx.addr        = addr;
        ctx.reg         = 0;
        ctx.ptr         = (uint8_t*)data;
        ctx.sz          = size;
        ctx.counter     = 0;
        ctx.is_read     = 0;
        ctx.st          = I2C_WORK;
        i2c_start();
        return 0;
}

ISR(TWI_vect)
{
        switch (TW_STATUS) {
                case TW_START:
                        TWDR = (ctx.addr << 1) | TW_WRITE;
                        i2c_step();
                        break;
                case TW_REP_START:
                        TWDR = (ctx.addr << 1) | TW_READ;
                        i2c_step();
                        break;
                case TW_MT_SLA_ACK:
                        if (ctx.is_read) {
                                TWDR = ctx.reg;
                        } else if (ctx.counter < ctx.sz){
                                TWDR = ctx.ptr[ctx.counter++];
                        } else {
                                ctx.st = I2C_OK;
                                i2c_stop();
                                break;
                        }
                        i2c_step();
                        break;

                case TW_MT_DATA_ACK:
                        if (ctx.is_read) {
                                i2c_start();
                        } else if (ctx.counter < ctx.sz) {
                                TWDR = ctx.ptr[ctx.counter++];
                                i2c_step();
                        } else {
                                ctx.st = I2C_OK;
                                i2c_stop();
                                break;
                        }
                        break;

                case TW_MR_SLA_ACK:
                        if (ctx.sz > 1) {
                                i2c_step_ack();
                        } else {
                                i2c_step();
                        }
                        break;

                case TW_MR_DATA_ACK:
                        ctx.ptr[ctx.counter++] = TWDR;
                        if (ctx.counter < ctx.sz - 1) {
                                i2c_step_ack();
                        } else {
                                i2c_step();
                        }
                        break;

                case TW_MR_DATA_NACK:
                        ctx.ptr[ctx.counter++] = TWDR;
                        ctx.st = I2C_OK;
                        i2c_stop();
                        break;

                case TW_MT_SLA_NACK:
                case TW_MT_DATA_NACK:
                case TW_MR_SLA_NACK:
                        ctx.st = I2C_ERR;
                        i2c_stop();
                        break;

                default:
                        ctx.st = I2C_ERR;
                        i2c_stop();
                        break;
        }
}
