# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

libs-y += libmdev

libmdev-objs-y                    := mdev.c
libmdev-objs-$(CONFIG_MDEV_TESTS) += mdev_test.c

libmdev-supported-toolchain-y := arm_gcc iar
