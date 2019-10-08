# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libawsmqtt

libawsmqtt-objs-y := \
		../../../../../../../libraries/c_sdk/standard/mqtt/src/iot_mqtt_agent.c

libawsmqtt-supported-toolchain-y := arm_gcc iar

