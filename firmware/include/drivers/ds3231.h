#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>

/* Driver state, reflects the last/current bus op */
typedef enum {
        DS3231_IDLE,    /* nothing has run yet */
        DS3231_WORK,    /* op in flight */
        DS3231_OK,      /* last op succeeded */
        DS3231_ERR,     /* last op failed on the bus */
} ds3231_status_t;

/* Decoded RTC time */
typedef struct {
        uint8_t seconds;        /* 0-59 */
        uint8_t minutes;        /* 0-59 */
        uint8_t hours;          /* 0-23, 24-hour */
        uint8_t week;           /* 1-7, day of week */
        uint8_t date;           /* 1-31, day of month */
        uint8_t month;          /* 1-12 */
        uint8_t year;           /* 0-99, offset from 2000 */
} ds3231_time_t;

/* Register the update callback and start self-syncing. Call once at startup */
void ds3231_init(void);

/* Latch a new time, fire-and-forget. Copies tm, updates snapshot at once.
 * Driver commits when the bus is free; no retry needed. Returns 1 if tm is NULL */
uint8_t ds3231_set_time(const ds3231_time_t *tm);

/* Copy the current time snapshot into tm. Non-blocking, atomic, ~250ms stale */
void ds3231_get_time(ds3231_time_t *tm);

/* Current driver state */
ds3231_status_t ds3231_get_status(void);

#endif /* DS3231_H */