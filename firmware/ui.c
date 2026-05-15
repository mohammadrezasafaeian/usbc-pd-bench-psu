/* Front panel. Only three LEDs are on GPIO; the other seven are wired to
 * hardware signals, so the fault lamp still works when the MCU is dead. */

#include "psu.h"

static void poll_encoder(void)
{
    static uint8_t prev;

    uint8_t now = (uint8_t)((pin_read(PIN_ENC_A) << 1) | pin_read(PIN_ENC_B));
    if (now == prev) return;

    /* Quadrature: direction is in the transition. RC networks on the
     * contacts already remove the bounce. */
    int8_t dir = 0;
    if ((prev == 0 && now == 1) || (prev == 1 && now == 3) ||
        (prev == 3 && now == 2) || (prev == 2 && now == 0)) dir = +1;
    if ((prev == 0 && now == 2) || (prev == 2 && now == 3) ||
        (prev == 3 && now == 1) || (prev == 1 && now == 0)) dir = -1;

    if (dir) {
        uint16_t step = pin_read(PIN_ENC_SW) ? 1000 : 100;   /* push = coarse */
        uint16_t *v = (psu.mode == MODE_BATTERY) ? &psu.bat_v_oc : &psu.v_set;
        *v = (uint16_t)clamp(*v + dir * step, 0, VOUT_MAX_MV);
    }
    prev = now;
}

static void poll_keys(void)
{
    static bool p_out, p_sel;
    static uint16_t back_held;

    bool out  = pin_read(PIN_BTN_OUT);
    bool sel  = pin_read(PIN_BTN_SEL);
    bool back = pin_read(PIN_BTN_BACK);

    /* OUTPUT is unconditional: never buried behind a menu. */
    if (out && !p_out) {
        psu.fault = FAULT_NONE;
        output_enable(!psu.output_on);
    }

    if (sel && !p_sel)
        psu.mode = (psu.mode == MODE_SUPPLY) ? MODE_BATTERY : MODE_SUPPLY;

    /* BACK held ~1 s clears a fault without touching the output. */
    if (back) {
        if (++back_held > UI_HZ) { psu.fault = FAULT_NONE; back_held = 0; }
    } else {
        back_held = 0;
    }

    p_out = out;
    p_sel = sel;
}

/* HB carries the rhythm; OUT and CC carry the low two bits of the fault. */
static void update_leds(uint32_t tick_ms)
{
    if (psu.fault == FAULT_NONE) {
        if ((tick_ms % 1000) < 500) pin_high(PIN_LED_HB); else pin_low(PIN_LED_HB);
        if (psu.output_on) pin_high(PIN_LED_OUT); else pin_low(PIN_LED_OUT);
        if (psu.cc_active) pin_high(PIN_LED_CC);  else pin_low(PIN_LED_CC);
        return;
    }

    uint8_t code   = (uint8_t)psu.fault - 1;         /* 0..6 */
    uint8_t blinks = (code < 4) ? 2 : 3;
    uint32_t phase = tick_ms % 2000;

    bool hb = false;
    for (uint8_t i = 0; i < blinks; i++)
        if (phase >= i * 300u && phase < i * 300u + 150u) hb = true;

    if (psu.fault == FAULT_ADC_RANGE) hb = true;     /* solid: firmware fault */
    if (hb) pin_high(PIN_LED_HB); else pin_low(PIN_LED_HB);

    if (code & 1) pin_high(PIN_LED_OUT); else pin_low(PIN_LED_OUT);
    if (code & 2) pin_high(PIN_LED_CC);  else pin_low(PIN_LED_CC);
}

void ui_poll(uint32_t tick_ms)
{
    poll_encoder();
    poll_keys();
    update_leds(tick_ms);
}
