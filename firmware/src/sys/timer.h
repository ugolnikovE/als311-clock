#ifndef TIMER_H
#define TIMER_H

// Initialize Timer/Counter0 in CTC mode 
void timer_init(void);

// Returns the time in milliseconds since the system started
unsigned long millis(void);

#endif //TIMER_H