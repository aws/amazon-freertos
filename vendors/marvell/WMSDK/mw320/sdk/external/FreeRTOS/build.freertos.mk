# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

libs-y += libfreertos

disable-lto-for += libfreertos

libfreertos-objs-y := \
		../../../../../../../../FreeRTOS/list.c \
		../../../../../../../../FreeRTOS/queue.c \
		../../../../../../../../FreeRTOS/tasks.c \
		../../../../../../../../FreeRTOS/event_groups.c \
		../../../../../../../../FreeRTOS/timers.c \
		../../../../../../../../FreeRTOS/stream_buffer.c \
		portable/MemMang/heap_marvell.c

libfreertos-objs-$(tc-cortex-m4-y) += \
		../../../../../../../../FreeRTOS/portable/$(tc-src-dir-y)/ARM_CM4F/port.c

libfreertos-objs-$(tc-iar-env-$(tc-cortex-m4-y)) += \
		../../../../../../../../FreeRTOS/portable/$(tc-src-dir-y)/ARM_CM4F/portasm.s

libfreertos-cflags-$(DEBUG_HEAP) += -DDEBUG_HEAP

libfreertos-cflags-$(CONFIG_ENABLE_STACK_OVERFLOW_CHECK) += -DCONFIG_ENABLE_STACK_OVERFLOW_CHECK

libfreertos-cflags-$(CONFIG_ENABLE_ASSERTS) += -DCONFIG_ENABLE_ASSERT

libfreertos-cflags-$(tc-iar-env-y) += --fpu VFPv4_SP

libfreertos-supported-toolchain-y := arm_gcc iar
