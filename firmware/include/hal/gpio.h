#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

// Set pin as output via DDR
static inline void gpio_pin_output(volatile uint8_t *port, uint8_t pin)
{
        *(port - 1) |= (1 << pin);
}

// Set pin as input with pull-up enabled
static inline void gpio_pin_input(volatile uint8_t *port, uint8_t pin)
{
        *(port - 1) &= ~(1 << pin);
        *port       |= (1 << pin);
}

// Drive pin high
static inline void gpio_pin_high(volatile uint8_t *port, uint8_t pin)
{
        *port |= (1 << pin);
}

// Drive pin low
static inline void gpio_pin_low(volatile uint8_t *port, uint8_t pin)
{
        *port &= ~(1 << pin);
}

// Toggle pin by writing to PIN register
static inline void gpio_pin_toggle(volatile uint8_t *port, uint8_t pin)
{
        *(port - 2) = (1 << pin);
}

// Read pin state from PIN register
static inline uint8_t gpio_pin_read(volatile uint8_t *port, uint8_t pin)
{
        return *(port - 2) & (1 << pin);
}

#endif // GPIO_H
