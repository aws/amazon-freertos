# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawsota

global-cflags-y += -I$(d)/../../../../../../../../third_party/jsmn \
		   -I$(d)/../../../../../../../../third_party/tinycbor/src

libawsota-objs-y := \
		../../../../../../../../ota/portable/marvell/mw300_rd/aws_ota_pal.c \
		../../../../../../../../ota/aws_ota_agent.c \
		../../../../../../../../ota/aws_ota_cbor.c 

libawsota-supported-toolchain-y := arm_gcc iar

