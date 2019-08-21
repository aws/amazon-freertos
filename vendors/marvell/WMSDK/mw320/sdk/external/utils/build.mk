# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

libs-y += libutils

libutils-objs-y := \
	           ../../../../../../../../utils/iot_system_init.c

libutils-supported-toolchain-y := arm_gcc iar

