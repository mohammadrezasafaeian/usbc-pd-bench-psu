/* USB-C PD Bench Power Supply
 * STM32G030C8T6, bare metal, no HAL. */

#include "stm32g030.h"

static void clock_init(void)
{
    /* Reset default HSI16 = 16 MHz, ample for a 5 kHz loop, so no PLL. */
    RCC_IOPENR  |= 0xF;                     /* GPIOA..GPIOD */
    RCC_APBENR1 |= (1u << 1);               /* TIM3         */
    RCC_APBENR2 |= (1u << 20) | (1u << 17); /* ADC, TIM16   */
}

int main(void)
{
    clock_init();
    for (;;) ;
}
