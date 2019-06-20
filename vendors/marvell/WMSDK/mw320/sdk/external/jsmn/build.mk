# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawsjsmn

libawsjsmn-objs-y := \
		../../../../../../../jsmn/jsmn.c

libawsjsmn-supported-toolchain-y := arm_gcc iar

