# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

tc-src-dir-$(tc-iar-env-y) := IAR
tc-src-dir-$(tc-arm_gcc-env-y) := GCC

global-cflags-y += \
                -I$(d)/../../../../../../../../include/

#global-cflags-$(tc-cortex-m3-y) += \
#		-I$(d)/portable/$(tc-src-dir-y)/ARM_CM3

global-cflags-$(tc-cortex-m4-y) += \
		-I$(d)/../../../../../../../../FreeRTOS/portable/$(tc-src-dir-y)/ARM_CM4F

-include $(d)/build.freertos.mk

