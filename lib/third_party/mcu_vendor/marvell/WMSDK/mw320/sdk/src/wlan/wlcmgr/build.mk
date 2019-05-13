# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

libs-y += libwlcmgr

global-cflags-y += -I$(d)/../../../src/incl/platform/net/freertos-plus-tcp

libwlcmgr-objs-y := \
		wlan.c \
		wlan_smc.c \
		wlan_pdn_handlers.c \
	#	wlan_tests.c \
	#	wlan_basic_cli.c \
		iw.c \
		uaputl.c \

#fw_heartbeat.c \

libwlcmgr-objs-$(CONFIG_WLAN_BRIDGE) += wlan_bridge_cli.c

WLAN_DRV_VERSION_INTERNAL := 1.0
WLAN_DRV_VERSION := \"$(WLAN_DRV_VERSION_INTERNAL)\"
libwlcmgr-cflags-y := -DWLAN_DRV_VERSION=$(WLAN_DRV_VERSION)

libwlcmgr-supported-toolchain-y := arm_gcc iar

