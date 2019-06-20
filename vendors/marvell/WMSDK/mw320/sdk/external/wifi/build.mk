# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.


libs-y += libawswifi

libawswifi-objs-y := ../../../../../../../../wifi/portable/marvell/mw300_rd/aws_wifi.c

libdrv-supported-toolchain-y := arm_gcc

