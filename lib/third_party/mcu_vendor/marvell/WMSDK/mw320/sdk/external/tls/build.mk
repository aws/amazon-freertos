# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawstls

libawstls-objs-y := \
		../../../../../../../../tls/aws_tls.c

libawstls-supported-toolchain-y := arm_gcc iar

