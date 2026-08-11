#ifndef DS3231_H
#define DS3231_H
#include <stdint.h>

// Decoded RTC time
typedef struct {
        uint8_t seconds;        // 0-59
        uint8_t minutes;        // 0-59
        uint8_t hours;          // 0-23 (24-hour format)
        uint8_t week;           // 1-7, day of week
        uint8_t data;           // 1-31, day of month
        uint8_t month;          // 1-12
        uint8_t year;           // 0-99 (offset from 2000)
} rtc_time_t;

// Encode tm to BCD and start an async write. Non-blocking.
// tm must stay valid until i2c status leaves I2C_WORK. Returns 0 if accepted, 1 if busy/invalid
uint8_t rtc_set_time(rtc_time_t *tm);

// Start an async read of all time registers into the internal buffer. Non-blocking.
// Call rtc_poll() to decode once i2c status leaves I2C_WORK. Returns 0 if accepted, 1 if busy/invalid
uint8_t rtc_get_time(void);

// Decode the last read buffer into tm. Call only after rtc_get_time() completed
// (i2c status left I2C_WORK without error); decoding stale/partial data yields garbage
void rtc_poll(rtc_time_t *tm);


#endif // DS3231_H
