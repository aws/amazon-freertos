# Copyright 2008-2015 Marvell International Ltd.
# All Rights Reserved.

OS := $(shell uname)

ifneq ($(NOISY),1)
AT=@
else
AT=
endif

TARGET ?= axf2firmware
CC ?= gcc
SDK_PATH ?= $(CURDIR)/../../../../..

ifeq ($(TOOLCHAIN),iar)
  CFLAGS := -DAXF_BUILT_BY_IAR
else
  CFLAGS := -DAXF_BUILT_BY_ARM_GCC
endif

CFLAGS += -O2 -Wall -fdata-sections -ffunction-sections

ifneq ($(OS), Darwin)
LDFLAGS = -static -s -Wl,--gc-sections
endif

all: $(TARGET)

$(TARGET): $(SDK_PATH)/sdk/src/core/util/crc/soft_crc32.c axf2firmware.c
	@echo " [host][ld] $@"
	$(AT)$(CC) -O2 -Wall $(CFLAGS) $(LDFLAGS) -I$(SDK_PATH)/sdk/src/incl/sdk -I$(SDK_PATH)/sdk/src/incl/libc/arm_gcc -o $@ $^

clean:
	$(AT)rm -f $(TARGET) $(wildcard *.o)
