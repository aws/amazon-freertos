# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libmbedtls
global-cflags-y += \
	-I$(d)/../../../../../../../mbedtls/include

mbedtls_src_dir := $(d)/../../../../../../../libraries/3rdparty/mbedtls

libmbedtls-objs-y := $(wildcard $(mbedtls_src_dir)/library/*.c)
libmbedtls-objs-y := $(libmbedtls-objs-y:$(d)/%=%)

libmbedtls-supported-toolchain-y := arm_gcc

