/* Board wiring and scaling, taken from the schematic netlist.
 * The only file that changes if the hardware changes. */

#ifndef BOARD_H
#define BOARD_H

#include "stm32g030.h"

/* Pins expand to (port, number), so calls read pin_high(PIN_LED_CC). */
#define PIN_LED_OUT   GPIOA, 15
#define PIN_LED_CC    GPIOB, 0
#define PIN_LED_HB    GPIOB, 1
#define PIN_SHDN_N    GPIOB, 12      /* open drain, wired-AND with the comparator */

#define PIN_CFG1      GPIOD, 0
#define PIN_CFG2      GPIOD, 2
#define PIN_CFG3      GPIOD, 3
#define PIN_PD_PG     GPIOD, 1

#define PIN_BTN_OUT   GPIOC, 13
#define PIN_BTN_SEL   GPIOC, 14
#define PIN_BTN_BACK  GPIOC, 15
#define PIN_ENC_A     GPIOB, 3
#define PIN_ENC_B     GPIOB, 4
#define PIN_ENC_SW    GPIOB, 5

/* ADC1 channels: PA0/PA1/PA4/PA5 */
#define ADC_CH_VBUS   0
#define ADC_CH_VPRE   1
#define ADC_CH_VOUT   4
#define ADC_CH_ISNS   5

/* Divider ratios in tenths, so the maths stays integer.
 *   VOUT, VPRE 150k/10k = 16.0:1    VBUS 100k/10k = 11.0:1 */
#define VREF_MV        3300
#define ADC_FULL       4095
#define DIV_VOUT_NUM   160
#define DIV_VPRE_NUM   160
#define DIV_VBUS_NUM   110
#define DIV_DEN        10

#define VOUT_MAX_MV    36000
#define IOUT_MAX_MA    3000
#define HEADROOM_MIN   2000          /* mV V_PRE must stay above V_OUT   */
#define HEADROOM_MAX   3000
#define HEADROOM_RI    350           /* extra headroom, microvolts per mA */

#define LOOP_HZ        5000
#define UI_HZ          100
#define PWM_PERIOD     1024          /* shared by the legs and the ref DAC */

#endif
