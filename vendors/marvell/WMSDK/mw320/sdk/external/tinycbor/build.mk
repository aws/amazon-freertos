# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawstinycbor

libawstinycbor-objs-y := \
		../../../../../../../libraries/3rdparty/tinycbor/cborencoder.c \
		../../../../../../../libraries/3rdparty/tinycbor/cborparser.c \
		../../../../../../../libraries/3rdparty/tinycbor/cborencoder_close_container_checked.c

libawsjsmn-supported-toolchain-y := arm_gcc iar

