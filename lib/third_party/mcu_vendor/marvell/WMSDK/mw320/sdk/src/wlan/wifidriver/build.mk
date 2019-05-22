# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

libs-y += libwifidriver

global-cflags-y += -I$(d)/../../../src/incl/platform/net/freertos-plus-tcp

libwifidriver-objs-y := \
		wifi-mem.c wifi_pwrmgr.c wifi.c wifi-uap.c \
		wifi-debug.c wifi-sdio.c mlan_uap_ioctl.c \
		mlan_11n.c mlan_11n_rxreorder.c mlan_init.c \
		mlan_cmdevt.c mlan_join.c mlan_cfp.c mlan_glue.c \
		mlan_txrx.c mlan_sta_rx.c mlan_misc.c mlan_shim.c \
		mlan_wmm.c mlan_11n_aggr.c mlan_sta_cmd.c \
		mlan_sta_cmdresp.c mlan_sta_event.c mlan_wmsdk.c \
		mlan_11h.c mlan_scan.c mlan_11d.c \
		mlan_sta_ioctl.c mlan_sdio.c mlan_uap_cmdevent.c
#mlan_glue.c

libwifidriver-cflags-y              := -I$(d) -I$(d)/incl

libwifidriver-supported-toolchain-y := arm_gcc iar
