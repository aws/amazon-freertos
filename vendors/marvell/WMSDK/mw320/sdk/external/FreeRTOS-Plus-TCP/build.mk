# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libfreertosplustcp
global-cflags-y +=  \
		-I$(d)/../../../../../../../tests/integration_test/ \
		-I$(d)/../../../../../../../libraries/freertos_plus/standard/tls/include/

libfreertosplustcp-objs-y := \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_ARP.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_DHCP.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_DNS.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_IP.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_Sockets.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_Stream_Buffer.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_TCP_IP.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_TCP_WIN.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/FreeRTOS_UDP_IP.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/portable/NetworkInterface/mw300_rd/NetworkInterface.c  \
	../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/portable/BufferManagement/BufferAllocation_2.c
	
libfreertosplustcp-supported-toolchain-y := arm_gcc iar

