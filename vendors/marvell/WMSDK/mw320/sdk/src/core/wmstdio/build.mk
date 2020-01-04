# Copyright 2008-2019, Marvell International Ltd.

libs-y += libwmstdio

libwmstdio-objs-y :=  wmstdio.c console.c wmassert.c

libwmstdio-supported-toolchain-y := arm_gcc iar
