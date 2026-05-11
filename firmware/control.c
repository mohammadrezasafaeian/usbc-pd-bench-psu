/* Regulation. The LM358 + PNP Darlington holds the output continuously;
 * this only sets the reference and keeps V_PRE above V_OUT. */

#include "psu.h"

static uint16_t target_headroom(uint16_t i_ma)
{
    /* Flat 2 V is thin at 3 A once Darlington saturation, the shunt drop
     * and the switcher's droop are added up. */
    int32_t h = HEADROOM_MIN + ((int32_t)i_ma * HEADROOM_RI) / 1000;
    return (uint16_t)clamp(h, HEADROOM_MIN, HEADROOM_MAX);
}

/* A cell sags under load, and that is the whole model. */
static uint16_t battery_target(void)
{
    int32_t sag = ((int32_t)psu.i_out * psu.bat_r_int) / 1000;
    return (uint16_t)clamp((int32_t)psu.bat_v_oc - sag, 0, VOUT_MAX_MV);
}

void control_step(void)
{
    measure();

    if (!psu.output_on) {
        psu.pi_integral = 0;               /* no wind-up while off */
        return;
    }

    uint16_t target = (psu.mode == MODE_BATTERY) ? battery_target() : psu.v_set;

    /* Current limit beats voltage. The analogue loop does the limiting;
     * this only walks the reference down. */
    psu.cc_active = (psu.i_out >= psu.i_limit);
    if (psu.cc_active) {
        int32_t over = (int32_t)psu.i_out - (int32_t)psu.i_limit;
        target = (uint16_t)clamp((int32_t)target - over * 4, 0, VOUT_MAX_MV);
    }
    set_ref_dac(target);

    /* Enough headroom to regulate, little enough to stay cool. */
    uint16_t want_pre = target + target_headroom(psu.i_out);
    if (want_pre > VOUT_MAX_MV + HEADROOM_MAX) want_pre = VOUT_MAX_MV + HEADROOM_MAX;

    int32_t err = (int32_t)want_pre - (int32_t)psu.v_pre;
    psu.pi_integral = clamp(psu.pi_integral + err, -200000, 200000);

    int32_t duty = clamp(err * 6 + psu.pi_integral / 64, 0, PWM_PERIOD - 1);

    /* One leg chops, the other parks - a 4-switch buck-boost, not two
     * cascaded converters. */
    if (want_pre < psu.v_bus)
        set_leg_duty((uint16_t)duty, PWM_PERIOD - 1);   /* buck  */
    else
        set_leg_duty(PWM_PERIOD - 1, (uint16_t)duty);   /* boost */

    if (psu.v_out > VOUT_MAX_MV + 2000) {
        psu.fault = FAULT_OVERVOLTAGE;
        output_enable(false);
    }
    if (psu.v_pre + 500 < psu.v_out) {     /* pre-reg fell below the output */
        psu.fault = FAULT_TRACKING;
        output_enable(false);
    }
}
