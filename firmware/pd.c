/* PD contract selection. The buck-boost decouples input from output, so any
 * contract reaches any voltage - the choice is about keeping the duty cycle
 * controllable, not about reachability. */

#include "psu.h"

static void request_pdo(uint8_t cfg)      /* 0=5V 1=9V 2=12V 3=15V 4=20V */
{
    static const uint8_t bits[5] = { 0x1, 0x0, 0x4, 0x6, 0x2 };
    uint8_t b = bits[cfg];
    if (b & 1) pin_high(PIN_CFG1); else pin_low(PIN_CFG1);
    if (b & 2) pin_high(PIN_CFG2); else pin_low(PIN_CFG2);
    if (b & 4) pin_high(PIN_CFG3); else pin_low(PIN_CFG3);
}

static uint8_t pdo_for_setpoint(uint16_t v_set_mv)
{
    if (v_set_mv <  4000) return 1;       /*  9 V - keeps duty off the floor */
    if (v_set_mv <  9000) return 2;       /* 12 V */
    if (v_set_mv < 13000) return 3;       /* 15 V */
    return 4;                             /* 20 V - least step-up */
}

/* CFG1 is pulled high in hardware, so a bare board already asks for 5 V. */
void pd_negotiate(void)
{
    request_pdo(pdo_for_setpoint(psu.v_set));

    for (volatile uint32_t i = 0; i < 800000; i++) ;   /* ~500 ms settle */
    measure();

    if (psu.v_bus < 6000) {
        /* Legacy 5 V port: not a failure, just a smaller envelope. */
        psu.fault = FAULT_LEGACY_CLAMP;
        if (psu.v_set > 4000) psu.v_set = 4000;
    } else if (!pin_read(PIN_PD_PG)) {
        psu.fault = FAULT_PD_FAILED;
    }
}

/* Drop the output across a renegotiation: VBUS collapses briefly and that
 * must not reach the load. */
void pd_follow_setpoint(void)
{
    static uint8_t cur_pdo = 0xFF;

    uint8_t want = pdo_for_setpoint(psu.v_set);
    if (want == cur_pdo || psu.fault == FAULT_LEGACY_CLAMP) return;

    bool was_on = psu.output_on;
    output_enable(false);
    request_pdo(want);
    cur_pdo = want;
    for (volatile uint32_t i = 0; i < 200000; i++) ;   /* ~125 ms */
    if (was_on) output_enable(true);
}
