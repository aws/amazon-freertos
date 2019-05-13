# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawsbufferpool

libawsbufferpool-objs-y := \
		../../../../../../../../bufferpool/aws_bufferpool_static_thread_safe.c

libawsbufferpool-supported-toolchain-y := arm_gcc iar

