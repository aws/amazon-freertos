# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawstinycbor

libawstinycbor-objs-y := \
		../../../../../../../libraries/3rdparty/tinycbor/src/cborencoder.c \
		../../../../../../../libraries/3rdparty/tinycbor/src/cborparser.c \
		../../../../../../../libraries/3rdparty/tinycbor/src/cborencoder_close_container_checked.c

libawsjsmn-supported-toolchain-y := arm_gcc iar

