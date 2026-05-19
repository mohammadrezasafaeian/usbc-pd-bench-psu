# Firmware

Bare-metal C for the STM32G030C8T6. No HAL, no RTOS.

```
firmware/
  main.c          init and the 5 kHz / 100 Hz loop
  hw.c            GPIO, ADC, PWM  — the only file that touches registers
  control.c       PI loop, current limit, battery emulation
  pd.c            PDO selection and renegotiation
  ui.c            encoder, buttons, LED codes
  psu.h           shared state and cross-module declarations
  board.h         pin map, divider ratios, limits
  stm32g030.h     register map
  startup.c       vector table, .data / .bss setup
  stm32g030.ld    64 KB flash, 8 KB RAM
  Makefile
```

`board.h` is the only file that changes if the wiring changes.

## Build

```bash
cd firmware
make            # -> main.elf, main.bin
make flash      # OpenOCD over SWD
```

Needs `gcc-arm-none-eabi`. **3.0 KB flash (4.7 %)**, **40 bytes RAM (0.5 %)**.

---

## How the regulation is split

| Layer | Does | Speed |
|---|---|---|
| LM358 + PNP Darlington | holds V_OUT at the reference | analogue |
| Comparator + SHDN_N | kills the drivers on over-current | < 1 µs, no MCU |
| Firmware PI loop | keeps V_PRE ≈ V_OUT + headroom | 5 kHz |
| Firmware supervisor | limits, PDO choice, UI, diagnostics | 100 Hz |

Firmware never regulates the output directly. It sets the reference through a
PWM DAC and keeps the pre-regulator a sensible distance above the output:

```c
V_head = clamp(2.0 V + 0.35 Ω × I_out, 2.0 V, 3.0 V)
```

A flat 2 V is thin at 3 A once Darlington saturation, the shunt drop and the
switcher's droop are added up.

`SHDN_N` is open drain and wired-AND with the comparator, so firmware can only
*assert* shutdown — never override the hardware trip.

---

## Modes

**Supply** — constant voltage with a current limit. At the limit the firmware
walks the reference down and lights CC; the analogue loop does the limiting.

**Battery** — a cell sags under load, and that is the whole model:

```c
V_out = V_oc − I_load × R_internal
```

Both adjustable from the panel — 3.7 V / 100 mΩ for a Li-ion cell, 12.6 V /
20 mΩ for a small lead-acid pack.

Source side only: a real battery accepts charge, and the PNP cannot sink. That
needs a two-quadrant output stage — the next hardware revision.

---

## PDO selection

The buck-boost decouples input from output, so **any contract reaches any
output voltage**. What a contract limits is *current*.

| Setpoint | Requests | Why |
|---|---|---|
| 0 – 4 V | 9 V | keeps duty off the dead-time floor |
| 4 – 9 V | 12 V | |
| 9 – 13 V | 15 V | |
| 13 – 36 V | 20 V | least step-up ratio |

On a legacy 5 V-only port the range clamps to 4 V and `FAULT_LEGACY_CLAMP`
lights — the supply still works, with a smaller envelope. The output is
dropped across a renegotiation, because VBUS collapses briefly.

---

## Diagnostic codes

Three LEDs are on GPIO; the other seven are wired to hardware signals, so the
fault lamp still works when the MCU is dead.

| HB | OUT | CC | Meaning |
|---|---|---|---|
| 1 Hz heartbeat | output live | in CC | normal |
| 2 blinks | · | · | over-current |
| 2 blinks | on | · | over-voltage |
| 2 blinks | · | on | over-temperature |
| 2 blinks | on | on | tracking — pre-reg lost the output |
| 3 blinks | · | · | PD negotiation failed |
| 3 blinks | on | · | legacy 5 V port, range clamped |
| solid | on | on | firmware fault |

Press **OUTPUT** to clear a fault and toggle the output, or hold **BACK** for
about a second to clear it without touching the output.
