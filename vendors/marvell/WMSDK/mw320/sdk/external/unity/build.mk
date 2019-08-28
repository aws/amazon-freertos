# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libunity

global-cflags-y += \
		-DUNITY_INCLUDE_CONFIG_H=1 \
	        -I$(d)/../../../../../../../libraries/3rdparty/unity/src  \
		-I$(d)/../../../../../../../libraries/3rdparty/unity/extras/fixture/src

libunity-objs-y := \
		../../../../../../../libraries/3rdparty/unity/src/unity.c   \
		../../../../../../../libraries/3rdparty/unity/extras/fixture/src/unity_fixture.c
libunity-supported-toolchain-y := arm_gcc iar

