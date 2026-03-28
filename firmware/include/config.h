#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>

// MCU Clock
#define F_CPU 16000000UL

// Shift Register
#define SR_DS_PORT    PORTD
#define SR_DS_PIN     PD2
#define SR_CLK_PORT   PORTD
#define SR_CLK_PIN    PD3
#define SR_LATCH_PORT PORTD
#define SR_LATCH_PIN  PD4

// UART
#define BAUD 9600

#endif // CONFIG_H
