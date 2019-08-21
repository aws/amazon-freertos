# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libpkcs11

global-cflags-y += -I$(d)/../../../../../../../../third_party/pkcs11 \
		-I$(d)/../../src/incl/platform/os/freertos
libpkcs11-objs-y := \
		../../../../../../../../pkcs11/portable/marvell/mw300_rd/iot_pkcs11_pal.c \
		../../../../../../../../pkcs11/mbedtls/iot_pkcs11_mbedtls.c

libpkcs11-supported-toolchain-y := arm_gcc iar

