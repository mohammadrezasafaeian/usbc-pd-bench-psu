/* Minimal startup for STM32G030C8T6 (Cortex-M0+):
 * copy .data, zero .bss, call main. */

#include <stdint.h>

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;

int  main(void);
void Reset_Handler(void);
static void Default_Handler(void) { for (;;) ; }

void Reset_Handler(void)
{
    uint32_t *src = &_sidata, *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;
    for (dst = &_sbss; dst < &_ebss; ) *dst++ = 0;
    main();
    for (;;) ;
}

/* Core vectors only - no peripheral interrupt is used anywhere. */
__attribute__((section(".isr_vector"), used))
void (* const vectors[])(void) = {
    (void (*)(void))&_estack,
    Reset_Handler,
    Default_Handler,   /* NMI       */
    Default_Handler,   /* HardFault */
    0, 0, 0, 0, 0, 0, 0,
    Default_Handler,   /* SVC       */
    0, 0,
    Default_Handler,   /* PendSV    */
    Default_Handler,   /* SysTick   */
};
