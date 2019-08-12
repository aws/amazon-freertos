# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libsecuresocket

libsecuresocket-objs-y := \
		../../../../../../../libraries/abstractions/secure_sockets/freertos_plus_tcp/aws_secure_sockets.c \
		../../../../../../../libraries/abstractions/platform/freertos/iot_metrics.c

libsecuresocket-supported-toolchain-y := arm_gcc iar

