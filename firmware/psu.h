/* Shared state and the calls that cross module boundaries. */

#ifndef PSU_H
#define PSU_H

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

typedef enum { MODE_SUPPLY, MODE_BATTERY } mode_t;

typedef enum {
    FAULT_NONE = 0,
    FAULT_OVERCURRENT,
    FAULT_OVERVOLTAGE,
    FAULT_OVERTEMP,
    FAULT_TRACKING,          /* pre-regulator lost the output        */
    FAULT_PD_FAILED,
    FAULT_LEGACY_CLAMP,      /* 5 V port, range reduced              */
    FAULT_ADC_RANGE
} fault_t;

struct psu {
    mode_t   mode;
    bool     output_on;
    fault_t  fault;

    uint16_t v_set;          /* mV */
    uint16_t i_limit;        /* mA */

    uint16_t bat_v_oc;       /* battery: open-circuit mV      */
    uint16_t bat_r_int;      /* battery: milliohms            */

    uint16_t v_out, v_pre, v_bus, i_out;   /* measured */

    bool     cc_active;
    int32_t  pi_integral;
};

extern struct psu psu;

static inline void pin_high(uint32_t port, int n) { BSRR(port) = 1u << n; }
static inline void pin_low (uint32_t port, int n) { BSRR(port) = 1u << (n + 16); }
static inline bool pin_read(uint32_t port, int n) { return !(IDR(port) & (1u << n)); }  /* active low */

static inline int32_t clamp(int32_t v, int32_t lo, int32_t hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

void gpio_mode_output(uint32_t port, int n);
void gpio_mode_input_pullup(uint32_t port, int n);
void gpio_mode_analog(uint32_t port, int n);
void gpio_mode_af(uint32_t port, int n, int af);

void adc_init(void);
void measure(void);

#endif
