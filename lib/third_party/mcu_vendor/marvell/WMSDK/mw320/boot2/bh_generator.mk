# Copyright (C) 2018 Marvell International Ltd.
# All Rights Reserved.

OS ?= $(shell $(t_uname))

t_bh_generator := sdk/tools/bin/$(os_dir)/$(arch_name-y)_bh_generator$(file_ext)

# Path of boot2 src specific to platform
# $(arch_name-y) can be mc200 or mw300 as of now
#
boot2-path := $(d)/src/$(arch_name-y)

bh_generator-cflags-y := \
		-Wall \
		-I$(d)/../sdk/src/incl/sdk/ \
		-I$(d)/src/$(arch_name-y) \
		-I$(d)/src/utils\
		-O2

# This is  for locking JTAG
# The password is passed by boot2 creation make command
# Command used is :
# make boot2 JTAG_LOCK_PASSWORD=<password>
#
ifneq ($(JTAG_LOCK_PASSWORD),)
bh_generator-cflags-$(CONFIG_CPU_MC200) +=-DJTAG_LOCK_PASSWORD=$(JTAG_LOCK_PASSWORD)
endif

ifeq ($(findstring Darwin,$(OS)),)
bh_generator-cflags-y += -fdata-sections -ffunction-sections
bh_generator-lflags-y += -s -Wl,--gc-sections
endif

ifneq ($(or $(findstring MINGW,$(OS)), $(findstring windows,$(OS))),)
bh_generator-cflags-y += -DBUILD_MINGW -mno-ms-bitfields
endif

bh_generator-cflags-$(CONFIG_CPU_MC200) += -DCONFIG_CPU_MC200
bh_generator-cflags-$(CONFIG_CPU_MW300) += -DCONFIG_CPU_MW300

bh_generator-objs-y := $(boot2-path)/bootrom_header/bootrom_header.c
bh_generator-objs-$(CONFIG_CPU_MC200) += $(boot2-path)/../utils/soft_crc32.c

bh_generator-objs-$(CONFIG_CPU_MW300) += $(boot2-path)/../utils/soft_crc16.c

# bh_generator is used to generate bootheader

$(t_bh_generator): $(bh_generator-objs-y)
	@echo " [$(notdir $@)]"
	@echo " [host][ld] $(bh_generator-objs-y)"
	$(AT) $(HOST_CC) $(bh_generator-cflags-y) $(bh_generator-lflags-y) $(bh_generator-objs-y) -o $@

$(t_bh_generator).clean:
	@echo " [clean] $(notdir $(t_bh_generator))"
	$(AT) $(t_rm) -f $(t_bh_generator)

$(notdir $(t_bh_generator)): $(t_bh_generator)
$(notdir $(t_bh_generator)).clean: $(t_bh_generator).clean

tools.install: $(notdir $(t_bh_generator))
tools.clean: $(notdir $(t_bh_generator)).clean
