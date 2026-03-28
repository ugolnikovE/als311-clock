#ifndef ALS311_H
#define ALS311_H

#include <stdint.h>

// Init shift register, pins defined in config.h
void als_init(void);

// Map str through 7-segment font and output to display
void als_write(const char *str);

#endif // ALS311_H