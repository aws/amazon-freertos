# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libsecuresocket

libsecuresocket-objs-y := \
		../../../../../../../../secure_sockets/portable/freertos_plus_tcp/iot_secure_sockets.c

libsecuresocket-supported-toolchain-y := arm_gcc iar

