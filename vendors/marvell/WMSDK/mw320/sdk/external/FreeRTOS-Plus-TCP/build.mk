# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libfreertosplustcp
global-cflags-y +=  \
		-I$(d)/../../../../../../../../FreeRTOS-Plus-TCP/include  \
		-I$(d)/../../../../../../../../FreeRTOS-Plus-TCP/source \
		-I$(d)/../../../../../../../../FreeRTOS-Plus-TCP/source/portable/Compiler/GCC
#		-I$(d)/../../src/wlan/wifidriver/incl

libfreertosplustcp-objs-y := \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_ARP.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_DHCP.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_IP.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_Sockets.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_Stream_Buffer.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_IP.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/FreeRTOS_UDP_IP.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/portable/NetworkInterface/mw300_rd/NetworkInterface.c  \
	../../../../../../../../FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_2.c
	
libfreertosplustcp-supported-toolchain-y := arm_gcc iar

