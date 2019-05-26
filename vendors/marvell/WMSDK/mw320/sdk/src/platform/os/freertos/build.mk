# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libos
libos-objs-y := os.c os_cli.c

libos-objs-$(CONFIG_HEAP_TESTS)      += heap-alloc-test.c
libos-objs-$(CONFIG_SEMAPHORE_DEBUG) += semdbg.c

libos-supported-toolchain-y := arm_gcc iar
