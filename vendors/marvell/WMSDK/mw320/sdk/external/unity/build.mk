# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

libs-y += libunity

global-cflags-y += \
		-DUNITY_INCLUDE_CONFIG_H=1 \
	        -I$(d)/../../../../../../../unity/src  \
		-I$(d)/../../../../../../../unity/extras/fixture/src

libunity-objs-y := \
		../../../../../../../unity/src/unity.c   \
		../../../../../../../unity/extras/fixture/src/unity_fixture.c 

libunity-supported-toolchain-y := arm_gcc iar

