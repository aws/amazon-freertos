# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

CROSS_COMPILE := arm-none-eabi-

AS    := $(CROSS_COMPILE)gcc
CC    := $(CROSS_COMPILE)gcc
CPP   := $(CROSS_COMPILE)g++
LD    := $(CROSS_COMPILE)gcc
AR    := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
STRIP := $(CROSS_COMPILE)strip
HOST_CC := gcc


######### Common Linker File Handling
# This can be overriden from the apps
global-linkerscript-y := build/toolchains/arm_gcc/$(arch_name-y).ld

# Toolchain specific global cflags-y and lflags-y
global-cflags-y :=
global-lflags-y :=

######### XIP Handling
ifeq ($(XIP), 1)
  global-linkerscript-y := build/toolchains/arm_gcc/$(arch_name-y)-xip.ld
  global-linkerscript-$(CONFIG_ENABLE_MCU_PM3) := build/toolchains/arm_gcc/$(arch_name-y)-xip-pm3.ld
endif

# FORCE option for execution
tc-force-opt := FORCE
# Compiler environment variable
tc-env := arm_gcc
tc-arm_gcc-env-y := y

tc-install-dir-y := for_arm_gcc
tc-install-dir-$(USE_EXTD_LIBC) := for_extd

# define disable-lto-for empty
disable-lto-for :=

# file include option
tc-include-opt := -include

# library strip options
tc-strip-opt := --strip-debug

# gcc specific extra linker flags
tc-lflags-y := \
		-Xlinker --undefined \
		-Xlinker uxTopUsedPriority \
		-nostartfiles \
		-Xlinker --cref \
		-Xlinker --gc-sections

# Linker flags
tc-lflags-$(tc-lto-y) += -Xlinker -flto
tc-lflags-$(tc-cortex-m4-y) += \
		-Xlinker --defsym=_rom_data=64 \
		-mthumb -g -Os \
		-fdata-sections \
		-ffunction-sections \
		-ffreestanding \
		-MMD -Wall \
		-fno-strict-aliasing \
		-fno-common \
		-mfloat-abi=softfp \
		-mfpu=fpv4-sp-d16 \
		-D__FPU_PRESENT

# Defining t_date here as devhost_*.mk are not included yet.
OS := $(shell python sdk/tools/bin/get_os.py -s)
ifneq ($(findstring windows,${OS}),)
  t_date ?= sdk/tools/bin/GnuWin32/bin/date.exe
else
  t_which ?= which
  t_date ?= $(shell $(t_which) date | tail -1)
endif

_mrvl_build_time ?= $(shell $(t_date) +%s)

# Linker flag to add timestamp to build axf. It is used only during linking but
# is not saved in command file.
tc-lflag-ts-y := -Xlinker --defsym=_latest_time=$(_mrvl_build_time)

tc-lflags-$(tc-cortex-m3-y) += -mcpu=cortex-m3
tc-lflags-$(tc-cortex-m4-y) += -mcpu=cortex-m4

global-cflags-y += \
		-mthumb -g -Os \
		-fdata-sections \
		-ffunction-sections \
		-ffreestanding \
		-MMD -Wall \
		-fno-strict-aliasing \
		-fno-common \
		-std=c99

global-cflags-y += \
		-Wextra \
		-Wno-unused-parameter \
		-Wno-empty-body \
		-Wno-missing-field-initializers \
		-Wno-sign-compare \
		-Wno-type-limits \

global-cflags-$(tc-cortex-m4-y) += \
		-mfloat-abi=softfp \
		-mfpu=fpv4-sp-d16 \
		-D__FPU_PRESENT

global-cflags-$(tc-cortex-m3-y) += -mcpu=cortex-m3
global-cflags-$(tc-cortex-m4-y) += -mcpu=cortex-m4

# cpp specific compiler and linker flags
ifeq ($(CONFIG_ENABLE_CPP_SUPPORT),y)
global-cpp-cflags-y := \
		-D_Bool=bool \
		-std=c++1y \
		--specs=nosys.specs
endif

tc-lflags-y += \
		-Wl,--wrap,malloc -Wl,--wrap,free -Wl,--wrap,calloc \
		-Wl,--wrap,realloc -Wl,--wrap,printf -Wl,--wrap,fprintf

global-c-cflags-y := -fgnu89-inline

##############################################
## ARM_GCC Tololchain specific rules

# The command for converting a .c/.cc/.cpp/.S/.s to .o
# arg1 the .c/.cc/.cpp/.S/.s filename
# arg2 the object filename
#
# This file has the default rule that maps an object file from the standard
# build output directory to the corresponding .c/.cc/.cpp/.S/.s file in the src directory
#
define b-cmd-c-to-o
$(CC) $(b-trgt-cflags-y) $(tc-include-opt) $(global-preinclude-y) $(b-trgt-c-cflags-y) $(global-cflags-y) $(global-c-cflags-y) -o $(2) -c $(1) -MMD
endef

define b-cmd-s-to-o
$(AS) $(b-trgt-cflags-y) $(tc-include-opt) $(global-preinclude-y) $(b-trgt-c-cflags-y) $(global-cflags-y) $(global-c-cflags-y) -o $(2) -c $(1) -MMD
endef

ifneq ($(CONFIG_ENABLE_CPP_SUPPORT),)
define b-cmd-cpp-to-o
$(CPP) $(b-trgt-cflags-y) $(tc-include-opt) $(global-preinclude-y) $(b-trgt-cpp-cflags-y) $(global-cflags-y) $(global-cpp-cflags-y) -o $(2) -c $(1) -MMD
endef
endif

define b-cmd-axf
$($(1)-LD) -o $(2) $($(1)-objs-y) $($(1)-lflags-y) -Xlinker --start-group $($(1)-prebuilt-libs-y) $($(1)-libs-paths-y) $(global-prebuilt-libs-y) -Xlinker --end-group -T $($(1)-linkerscript-y) -Xlinker -M -Xlinker -Map -Xlinker $(2:%.axf=%.map) $(tc-lflags-y) $(global-cflags-y) $(global-lflags-y)
endef


define b-cmd-archive
$(AR) cru $(2) $($(1)-objs-y)
endef
##############################################
ifeq ($(XIP),1)
subdir-y += sdk/src/middleware/cache_profile
endif

