# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawstls

libawstls-objs-y := \
		../../../../../../../libraries/freertos_plus/standard/tls/src/iot_tls.c

libawstls-supported-toolchain-y := arm_gcc iar

