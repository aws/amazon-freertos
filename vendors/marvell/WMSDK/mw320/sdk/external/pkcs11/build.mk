# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libpkcs11

global-cflags-y += -I$(d)/../../../../../../../../third_party/pkcs11

libpkcs11-objs-y := \
		../../../../../../../vendors/marvell/boards/mw300_rd/ports/pkcs11/core_pkcs11_pal.c \
		../../../../../../../libraries/abstractions/pkcs11/mbedtls/core_pkcs11_mbedtls.c \
		../../../../../../../libraries/freertos_plus/standard/pkcs11/src/core_pkcs11.c

libpkcs11-supported-toolchain-y := arm_gcc iar

