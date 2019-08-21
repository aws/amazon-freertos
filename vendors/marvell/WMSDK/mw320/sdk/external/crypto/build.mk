# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawscrypto

global-cflags-y += -I$(d)/../../../../../../../pkcs11

libawscrypto-objs-y := \
		../../../../../../../../crypto/iot_crypto.c  

libawscrypto-supported-toolchain-y := arm_gcc iar

