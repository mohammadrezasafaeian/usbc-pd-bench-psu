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

/* ------------------------------------------------------------------- ADC */

void adc_init(void)
{
    ADC_CR |= (1u << 28);                       /* ADVREGEN */
    for (volatile int i = 0; i < 1000; i++) ;   /* regulator startup */
    ADC_CR |= (1u << 31);                       /* ADCAL - must finish before ADEN */
    while (ADC_CR & (1u << 31)) ;
    ADC_SMPR = 6u;                              /* long: the dividers are high impedance */
    ADC_CR |= (1u << 0);                        /* ADEN */
    while (!(ADC_ISR & (1u << 0))) ;
}

static uint16_t adc_read(uint8_t channel)
{
    ADC_CHSELR = 1u << channel;
    ADC_CR |= 1u << 2;                     /* ADSTART */
    while (!(ADC_ISR & (1u << 2))) ;       /* EOC     */
    return (uint16_t)ADC_DR;
}

/* Four samples: switching noise sits well above the loop bandwidth. */
static uint16_t adc_avg(uint8_t channel)
{
    uint32_t acc = 0;
    for (int i = 0; i < 4; i++) acc += adc_read(channel);
    return (uint16_t)(acc / 4);
}

/* counts -> mV at the pin -> mV at the node */
static uint16_t adc_scaled(uint8_t channel, uint16_t div_num)
{
    uint32_t mv = ((uint32_t)adc_avg(channel) * VREF_MV) / ADC_FULL;
    return (uint16_t)((mv * div_num) / DIV_DEN);
}

void measure(void)
{
    psu.v_out = adc_scaled(ADC_CH_VOUT, DIV_VOUT_NUM);
    psu.v_pre = adc_scaled(ADC_CH_VPRE, DIV_VPRE_NUM);
    psu.v_bus = adc_scaled(ADC_CH_VBUS, DIV_VBUS_NUM);

    /* INA181A2: 0R05 x50 = 2.5 V/A, so 1 mV = 0.4 mA */
    uint32_t mv = ((uint32_t)adc_avg(ADC_CH_ISNS) * VREF_MV) / ADC_FULL;
    psu.i_out = (uint16_t)((mv * 2) / 5);
}
