# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

libs-y += libwmtime
libwmtime-objs-y := wmtime.c wmtime_cli.c

ifeq (y,$(CONFIG_HW_RTC))
  libwmtime-objs-y += hwrtc.c
else
  libwmtime-objs-y += swrtc.c
endif

global-cflags-$(tc-iar-env-y) += -D_NO_DEFINITIONS_IN_HEADER_FILES

libwmtime-supported-toolchain-y := arm_gcc iar
