# Copyright 2008-2018, Marvell International Ltd.

libs-y += libpart

libpart-objs-y :=  partition.c

libpart-supported-toolchain-y := arm_gcc iar
