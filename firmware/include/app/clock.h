#ifndef CLOCK_H
#define CLOCK_H

typedef enum {
        CLOCK_TIME,
        CLOCK_DATE,
        CLOCK_SETTING
} clock_state_t;

void clock_setup(void);
void clock_update(void);

#endif // CLOCK_H