# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawsmqtt

libawsmqtt-objs-y := \
		../../../../../../../../mqtt/iot_mqtt_agent.c \
		../../../../../../../../mqtt/aws_mqtt_lib.c

libawsmqtt-supported-toolchain-y := arm_gcc iar

