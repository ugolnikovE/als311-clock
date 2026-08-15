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

// I2C
#define I2C_FREQUENCY 400000UL

// Buttons
#define BTN_COUNT  3
#define BTN_1_PORT PORTD
#define BTN_1_PIN  PD5
#define BTN_2_PORT PORTD
#define BTN_2_PIN  PD6
#define BTN_3_PORT PORTD
#define BTN_3_PIN  PD7

// UART
#define BAUD 9600

#endif // CONFIG_H
