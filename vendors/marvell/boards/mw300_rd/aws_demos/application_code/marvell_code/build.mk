# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libdemoscommon

global-cflags-y += -I$(d)/../../../../../../../lib/include/private \
		   -I$(d)/../../../../../../../lib/include
libdemoscommon-objs-y := \
	../../../../../common/logging/iot_logging_task_dynamic_buffers.c \
	../../../../../common/devmode_key_provisioning/aws_dev_mode_key_provisioning.c \
	../../../../../common/demo_runner/aws_demo_runner.c \
	../../../../../common/mqtt/aws_hello_world.c \
	../../../../../common/mqtt/aws_subscribe_publish_loop.c \
        ../../../../../common/ota/aws_ota_update_demo.c

libdemoscommon-supported-toolchain-y := arm_gcc iar
