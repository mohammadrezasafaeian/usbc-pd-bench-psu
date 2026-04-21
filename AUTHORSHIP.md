# Authorship and provenance

**Author:** Mohammadreza Safaeian
**Contact:** m.re.safaeian@gmail.com · mohammad.rsafaeian@gmail.com
**Repository:** https://github.com/mohammadrezasafaeian
**Copyright:** © 2026 Mohammadreza Safaeian. All rights reserved.

This is an original design. It is not derived from a reference design, an
application-note schematic, or any downloaded project.

---

## What makes that checkable

Anyone can type a name into a title block. These are the things that are
actually hard to fake, in descending order of usefulness:

### 1. The design evolved through revisions, and the earlier one is included

`docs/earlier-revision.png` is my previous KiCad revision (the earlier revision:
CH32V003 MCU, IR2104 drivers, discrete LM358 current sense). It is bound into
`sch.pdf` as page 4.

Rev C is the redraw that fixes the problems in it:

| the earlier revision (previous) | Rev C (this design) |
|---|---|
| no snubbers on the switch nodes | RC snubbers (4R7 + 2n2) on both |
| +12 V gate rail left as an off-sheet module — **unbuildable** | MT3608 boost on-sheet |
| IR2104 drivers (V_IH out of spec at 3.3 V) | IRS2104 (V_IH 2.5 V) |
| discrete LM358 current sense | INA181A2 + 0R05 shunt |
| CH32V003 — out of I/O once the UI was added | STM32G030C8T6, ~18 pins spare |
| no hardware over-current path | LM2903 trip, < 1 µs, firmware-independent |
| logic LDO fed from VBUS — fails on a legacy 5 V port | fed from +12 V |

Downloaded schematics do not come with the author's own superseded revision
and a defensible list of what was wrong with it.

### 2. The design was verified, not just drawn

Beyond KiCad's own ERC, the design was checked for connectivity and layout
faults:

| Check | Looks for |
|---|---|
| connectivity | dangling wire endpoints, wires crossing pins, shorted two-pin parts |
| netlist | orphan parts, unnamed nets, single-pin nets |
| page fit | items off-page, title-block clashes, overlapping notes |
| peripherals | every analogue net on a real ADC channel |

### 3. Faults found during checking, and how they were fixed

These are recorded because they show the work, not just the result. Each one
passed a visual inspection and was caught by a programmatic check:

- **Instance-path mismatch** — sheet symbols on the root page carried different
  UUIDs from the ones the child sheets quoted, so KiCad silently dropped five
  parts (including L2, the main inductor) from the netlist.
- **Q5 collector shorted to its emitter** — the pass transistor was bypassed
  entirely; `VPRE` and `VOUT` resolved to the same net.
- **Every UI input shorted to GND** — the debounce RC stack is ~19 mm tall but
  rows were pitched 12–16 mm, so each ground stub landed on the next row's
  signal node, chaining six inputs plus GND into one 84-node net.
- **All three keys shorted to GND** — the compact RC leg put the cap *on* the
  row, so the horizontal label wire ran straight through the cap and its
  ground symbol.
- **Encoder B shorted to S1** — a vertical drop crossed another contact's
  horizontal run.
- **LED polarity reversed** — MCU pins were driving cathodes.
- **Duplicate stacked wires** — two identical segments make KiCad drop the
  connection entirely.

### 4. Design decisions I can defend

The reasoning behind these is in `README.md` and in notes on the sheets
themselves. They are the questions worth asking me about:

- Why the current shunt is **high-side** rather than in the output return.
- Why the ground plane is **not split** between the switching and linear
  stages, despite that being a common instinct.
- Why the PD contract is **chosen per setpoint** rather than pinned at 20 V —
  and why 9 V, not 5 V, is the floor.
- Why the pass device is a **PNP Darlington** and not an NPN follower.
- Why the linear headroom is **adaptive** rather than a flat 2 V.
- Why 250 kHz costs 13 % of the period to dead-time, and what 150 kHz buys.

### 5. Commit history

`git log` shows the design being built and corrected over time, with each fix
in its own commit.

---

## Verification status

| Check | Result |
|---|---|
| KiCad ERC errors | 0 |
| KiCad ERC warnings | 5 `lib_symbol_mismatch` (cosmetic — see README) |
| Orphan parts | 0 |
| Dangling / single-pin nets | 0 (18 explicit no-connects excluded) |
| Auto-named `Net-(...)` nets | 0 — all 85 real nets carry names |
| Shorted two-terminal parts | 0 |
| Off-page items / title-block clashes | 0 |

---

## Licence

© 2026 Mohammadreza Safaeian. All rights reserved. See `LICENSE`.

The design is published for portfolio and evaluation purposes. No licence to
manufacture, redistribute, or create derivative works is granted. If you would
like to use any part of it, please get in touch.
