# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libdemoscommon

global-cflags-y += -I$(d)/../../../../../../../lib/include/private \
		   -I$(d)/../../../../../../../lib/include
libdemoscommon-objs-y := \
	../../../../../../../libraries/c_sdk/standard/common/logging/iot_logging_task_dynamic_buffers.c \
	../../../../../../../demos/dev_mode_key_provisioning/src/aws_dev_mode_key_provisioning.c

libdemoscommon-supported-toolchain-y := arm_gcc iar
