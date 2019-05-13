# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawstinycbor

libawstinycbor-objs-y := \
		../../../../../../../tinycbor/cborencoder.c \
		../../../../../../../tinycbor/cborparser.c \
		../../../../../../../tinycbor/cborencoder_close_container_checked.c

libawsjsmn-supported-toolchain-y := arm_gcc iar

