# USB-C PD Bench PSU - firmware build
#
#   make            build main.elf / main.bin
#   make flash      flash over SWD with OpenOCD
#   make clean

TARGET  = main
MCU     = cortex-m0plus

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

SRCS    = $(wildcard *.c)

CFLAGS  = -mcpu=$(MCU) -mthumb -std=c11 \
          -Wall -Wextra -Os -ffreestanding -fno-builtin \
          -ffunction-sections -fdata-sections
# -nostdlib but keep -lgcc: the M0+ has no hardware divide, so the compiler
# emits calls to __aeabi_uidivmod.
LDFLAGS = -mcpu=$(MCU) -mthumb -nostdlib -T stm32g030.ld \
          -Wl,--gc-sections -Wl,-Map=$(TARGET).map
LDLIBS  = -lgcc

all: $(TARGET).bin

$(TARGET).elf: $(SRCS) stm32g030.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $@ $(LDLIBS)
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32g0x.cfg \
	        -c "program $< verify reset exit"

clean:
	rm -f $(TARGET).elf $(TARGET).bin $(TARGET).map

.PHONY: all flash clean
