# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

libs-y += libpsm

libpsm-objs-y := \
		psm-v2.c \
		psm-utils.c

libpsm-objs-$(CONFIG_SECURE_PSM) += psm-v2-secure.c
global-cflags-$(CONFIG_SECURE_PSM) += -DCONFIG_SECURE_PSM

libpsm-supported-toolchain-y := arm_gcc iar
