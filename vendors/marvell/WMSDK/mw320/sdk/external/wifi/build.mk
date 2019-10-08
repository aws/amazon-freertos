# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.


libs-y += libawswifi

libawswifi-objs-y := ../../../../../../../vendors/marvell/boards/mw300_rd/ports/wifi/iot_wifi.c

libdrv-supported-toolchain-y := arm_gcc

