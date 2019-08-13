# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libutils

libutils-objs-y := \
	           ../../../../../../../libraries/freertos_plus/standard/utils/src/aws_system_init.c

libutils-supported-toolchain-y := arm_gcc iar

