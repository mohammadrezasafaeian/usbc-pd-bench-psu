/* GPIO, ADC and PWM: the only file that touches registers. */

#include "psu.h"

void gpio_mode_output(uint32_t port, int n)
{
    MODER(port) = (MODER(port) & ~(3u << (n * 2))) | (1u << (n * 2));
}

void gpio_mode_input_pullup(uint32_t port, int n)
{
    MODER(port) &= ~(3u << (n * 2));
    PUPDR(port)  = (PUPDR(port) & ~(3u << (n * 2))) | (1u << (n * 2));
}

void gpio_mode_analog(uint32_t port, int n)
{
    MODER(port) |= 3u << (n * 2);
}

void gpio_mode_af(uint32_t port, int n, int af)
{
    MODER(port) = (MODER(port) & ~(3u << (n * 2))) | (2u << (n * 2));
    if (n < 8) AFRL(port) = (AFRL(port) & ~(0xFu << (n * 4))) | ((uint32_t)af << (n * 4));
    else       AFRH(port) = (AFRH(port) & ~(0xFu << ((n - 8) * 4))) | ((uint32_t)af << ((n - 8) * 4));
}
