# Copyright 2008-2018, Marvell International Ltd.

libs-y += libhardfault

libhardfault-objs-y := hardfault.c

libhardfault-cflags-y := -I$(d)

libhardfault-supported-toolchain-y := arm_gcc iar
