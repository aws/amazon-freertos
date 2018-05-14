#
# Basic Makefile for FreeRTOS based project
# (C) 2018 Sergey Shcherbakov <shchers@gmail.com>
#

ifeq ($(shell which avr-gcc),)
$(error Please install avr-gcc package)
endif

ifeq ($(shell which avrdude),)
$(error Please install avrdude package)
endif

# Change MCU type accordingly to installed in your board
MCU := atmega328p

# Frequency of MCU in Hz
F_CPU := 16000000

# Fuses
EFUSE := 0xfd
HFUSE := 0xd3
LFUSE := 0xff

# Firmware name
OUT_NAME := blink

CFLAGS := -DF_CPU=$(F_CPU)UL
# Compiler options
CFLAGS += -mmcu=$(MCU) -gdwarf-2 -std=gnu99
# Compiler optimization
CFLAGS += -Os
# C Dialect options
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
# Warning options
CFLAGS += -Wall -Wstrict-prototypes
# Includes
CFLAGS += -I./
CFLAGS += -I../../../lib/include
CFLAGS += -I../../../lib/include/private
CFLAGS += -I../../../lib/FreeRTOS/portable/GCC/ATMega328P

C_SRC := \
    main.c
OBJS := $(C_SRC:.c=.o)

PORT_PATH := ../../../lib/FreeRTOS/portable/GCC/ATMega328P
PORT_C_SRC := \
    port.c
OBJS += $(addprefix $(PORT_PATH)/,$(PORT_C_SRC:.c=.o))

FREERTOS_PATH := ../../../lib/FreeRTOS
FREERTOS_C_SRC := \
    portable/MemMang/heap_1.c \
    list.c \
    tasks.c
OBJS += $(addprefix $(FREERTOS_PATH)/,$(FREERTOS_C_SRC:.c=.o))

# Libs
LDFLAGS := -lm

# --- Programmer options ---

# ISP bitclock
BITCLOCK := 4MHz

# --- Targets ---

.PHONY: all firmware flash fuses clean

all: firmware

clean:
	@echo "\\033[1;37;42m--- Clean-up output ---\\033[0m"
	@rm -v $(OBJS) $(OUT_NAME).elf $(OUT_NAME).hex $(OUT_NAME).eep $(OUT_NAME).map

fuses:
	@dialog --yesno "Do you really want to flash FUSES?" 0 0
	@avrdude -p $(MCU) -c avrispmkII -P usb -B $(BITCLOCK) -U efuse:w:$(EFUSE):m -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

%.o : %.c
	@echo "\\033[33m--- Building $< ---\\033[0m"
	@avr-gcc $(CFLAGS) -c $< -o $@

$(OUT_NAME).elf: $(OBJS)
	@echo "\\033[1;37;42m--- Linking $(@) ---\\033[0m"
	@avr-gcc $(CFLAGS) $(OBJS) --output $(OUT_NAME).elf -Wl,-Map=$(OUT_NAME).map,--cref $(LDFLAGS)

$(OUT_NAME).hex: $(OUT_NAME).elf
	@echo "\\033[1;37;42m--- Generating $(@) ---\\033[0m"
# Creating load file for Flash
	@avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature $(OUT_NAME).elf $(OUT_NAME).hex
# Creating load file for EEPROM
	@avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex $(OUT_NAME).elf $(OUT_NAME).eep

firmware: $(OUT_NAME).hex
# Just print size
	@avr-size --format=avr --mcu=$(MCU) -C $(OUT_NAME).elf

flash: firmware
	@echo "\\033[1;37;42m--- Flashing $(OUT_NAME).hex to device ---\\033[0m"
	@avrdude -p $(MCU) -c avrispmkII -P usb -B $(BITCLOCK) -U flash:w:$(OUT_NAME).hex
