#ifndef I2C_H
#define I2C_H

#include <stdint.h>

// Driver status; poll via i2c_get_status() after an async call
typedef enum {
        I2C_IDLE,       // no transaction has run yet
        I2C_OK,         // last transaction finished successfully
        I2C_ERR,        // last transaction failed (NACK / bus error)
        I2C_WORK        // transaction in progress, bus busy
} i2c_status_t;

// Configure TWI hardware (bit rate, prescaler); call once at startup. Requires sei()
void          i2c_init(void);

// Return status of the current/last transaction
i2c_status_t  i2c_get_status(void);

// Write size bytes from data to device addr (7-bit). Non-blocking.
// data must stay valid until status leaves I2C_WORK. Returns 0 if accepted, 1 if busy/invalid
uint8_t       i2c_write_async(uint8_t addr, const uint8_t *data, uint8_t size);

// Read size bytes from register reg of device addr (7-bit) into data. Non-blocking.
// data must stay valid until status leaves I2C_WORK. Returns 0 if accepted, 1 if busy/invalid
uint8_t       i2c_read_async(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t size);

#endif // I2C_H