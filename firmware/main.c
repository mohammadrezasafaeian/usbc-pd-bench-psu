/* USB-C PD Bench Power Supply - v1.2
 * STM32G030C8T6, bare metal, no HAL. */

#include "psu.h"

struct psu psu = {
    .mode      = MODE_SUPPLY,
    .v_set     = 5000,
    .i_limit   = 1000,
    .bat_v_oc  = 3700,       /* one Li-ion cell */
    .bat_r_int = 100,        /* 100 mOhm        */
};

static void clock_init(void)
{
    /* Reset default HSI16 = 16 MHz, ample for a 5 kHz loop, so no PLL. */
    RCC_IOPENR  |= 0xF;                     /* GPIOA..GPIOD */
    RCC_APBENR1 |= (1u << 1);               /* TIM3         */
    RCC_APBENR2 |= (1u << 20) | (1u << 17); /* ADC, TIM16   */
}

static void gpio_init(void)
{
    gpio_mode_analog(GPIOA, 0);    /* VBUS   */
    gpio_mode_analog(GPIOA, 1);    /* VPRE   */
    gpio_mode_analog(GPIOA, 4);    /* VOUT   */
    gpio_mode_analog(GPIOA, 5);    /* ISENSE */
    gpio_mode_analog(GPIOB, 10);   /* VOUT_FB monitor */
    gpio_mode_analog(GPIOB, 11);   /* VFB_DRV monitor */

    gpio_mode_af(GPIOA, 6, 1);     /* TIM3_CH1  leg A */
    gpio_mode_af(GPIOA, 7, 1);     /* TIM3_CH2  leg B */
    gpio_mode_af(GPIOB, 8, 2);     /* TIM16_CH1 ref DAC */

    gpio_mode_output(PIN_LED_OUT);
    gpio_mode_output(PIN_LED_CC);
    gpio_mode_output(PIN_LED_HB);
    gpio_mode_output(PIN_CFG1);
    gpio_mode_output(PIN_CFG2);
    gpio_mode_output(PIN_CFG3);

    OTYPER(GPIOB) |= (1u << 12);   /* SHDN_N open drain */
    gpio_mode_output(PIN_SHDN_N);
    pin_low(PIN_SHDN_N);           /* start shut down   */

    gpio_mode_input_pullup(PIN_BTN_OUT);
    gpio_mode_input_pullup(PIN_BTN_SEL);
    gpio_mode_input_pullup(PIN_BTN_BACK);
    gpio_mode_input_pullup(PIN_ENC_A);
    gpio_mode_input_pullup(PIN_ENC_B);
    gpio_mode_input_pullup(PIN_ENC_SW);
    gpio_mode_input_pullup(PIN_PD_PG);
}

int main(void)
{
    clock_init();
    gpio_init();
    adc_init();

    for (;;) {
        measure();
    }
}
