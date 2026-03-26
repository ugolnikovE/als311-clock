#ifndef SYSTIMER_H
#define SYSTIMER_H

#include <stdint.h>

// Periodic callback function pointer
typedef void (*systimer_callback_t)(void);

// Configure Timer0 in CTC mode with 1ms tick
void     systimer_init(void);

// Register callback invoked every `period` ms, returns 0 on success
uint8_t  systimer_register_callback(systimer_callback_t cb, uint16_t period);

// Return elapsed milliseconds since init (atomic read)
uint64_t systimer_millis(void);

#endif
