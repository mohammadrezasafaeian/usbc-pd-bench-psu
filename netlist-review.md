# Netlist review

Checks run against the exported netlist rather than the drawing, because
most of these are invisible on a sheet that looks correct.

| Check | Result |
|---|---|
| every part has a footprint | pass |
| no two-terminal part shorted end to end | pass |
| no net with a single node | pass |
| no two labels differing only by case or a character | pass |
| every symbol inside a sheet frame | pass |
| every MCU pin either assigned or explicitly no-connect | pass |
| analogue nets land on pins that have an ADC channel | **fail** |
| feedback divider reaches the error amplifier | **fail** |

## Analogue nets on pins with no ADC

`ADC_VBUS`, `ADC_VPRE`, `ADC_VOUT` and `ISNS_OUT` sit on PD2, PD3, PC6 and
PC7.  None of those has an ADC channel on this part, so not one of the four
measurements can be taken.  They need to move to PA0, PA1, PA4 and PA5.

## Feedback divider does not reach the error amplifier

`VOUT_FB` reaches only an MCU analogue input.  The error amplifier's
inverting input is on a separate net that sees only its compensation
capacitor, so the loop has no DC path around it.  Both need to be one node.

The first is fixed in the next revision.  The second is deeper than a pin
move and is carried forward.
