/* STM32G030C8T6 registers - only what this project touches.
 * Offsets and names follow RM0454. */

#ifndef STM32G030_H
#define STM32G030_H

#include <stdint.h>

#define PERIPH   0x40000000UL
#define AHB2     0x50000000UL

#define RCC      (PERIPH + 0x21000)
#define ADC1     (PERIPH + 0x12400)
#define TIM3     (PERIPH + 0x00400)
#define TIM16    (PERIPH + 0x14400)
#define GPIOA    (AHB2   + 0x0000)
#define GPIOB    (AHB2   + 0x0400)
#define GPIOC    (AHB2   + 0x0800)
#define GPIOD    (AHB2   + 0x0C00)

#define REG(a)   (*(volatile uint32_t *)(a))

#define RCC_IOPENR   REG(RCC + 0x34)
#define RCC_APBENR1  REG(RCC + 0x3C)
#define RCC_APBENR2  REG(RCC + 0x40)

#define MODER(p)  REG((p) + 0x00)
#define OTYPER(p) REG((p) + 0x04)
#define PUPDR(p)  REG((p) + 0x0C)
#define IDR(p)    REG((p) + 0x10)
#define BSRR(p)   REG((p) + 0x18)
#define AFRL(p)   REG((p) + 0x20)
#define AFRH(p)   REG((p) + 0x24)

#define ADC_ISR    REG(ADC1 + 0x00)
#define ADC_CR     REG(ADC1 + 0x08)
#define ADC_SMPR   REG(ADC1 + 0x14)
#define ADC_CHSELR REG(ADC1 + 0x28)
#define ADC_DR     REG(ADC1 + 0x40)

/* Same layout on TIM3 and TIM16 for the bits used here. */
#define TIM_CR1(t)   REG((t) + 0x00)
#define TIM_CCMR1(t) REG((t) + 0x18)
#define TIM_CCER(t)  REG((t) + 0x20)
#define TIM_PSC(t)   REG((t) + 0x28)
#define TIM_ARR(t)   REG((t) + 0x2C)
#define TIM_CCR1(t)  REG((t) + 0x34)
#define TIM_CCR2(t)  REG((t) + 0x38)
#define TIM_BDTR(t)  REG((t) + 0x44)

#endif
