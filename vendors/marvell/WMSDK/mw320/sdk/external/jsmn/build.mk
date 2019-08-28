# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawsjsmn

libawsjsmn-objs-y := \
		../../../../../../../libraries/3rdparty/jsmn/jsmn.c

libawsjsmn-supported-toolchain-y := arm_gcc iar

