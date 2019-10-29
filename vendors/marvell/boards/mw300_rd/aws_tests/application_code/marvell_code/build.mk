# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libcommontests

global-cflags-y += -I$(d)/../../../../../common/include \
		-I$(d)/../../config_files \
		-I$(d)/../../../../../common/ota \
		-I$(d)/../../../../../../lib/third_party/mbedtls/include \
		-I$(d)/../../../../../../demos/common/include



libcommontests-objs-y := \
		../../../../../../../libraries/abstractions/pkcs11/test/iot_test_pkcs11.c \
		../../../../../../../tests/common/aws_test_framework.c \
		../../../../../../../libraries/c_sdk/standard/common/test/iot_memory_leak.c \
		../../../../../../../tests/common/aws_test_runner.c \
		../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/test/iot_test_freertos_tcp.c \
		../../../../../../../libraries/freertos_plus/standard/tls/test/iot_test_tls.c \
		../../../../../../../libraries/abstractions/wifi/test/iot_test_wifi.c \
		../../../../../../../libraries/abstractions/secure_sockets/test/iot_test_tcp.c \
		../../../../../../../libraries/c_sdk/standard/mqtt/test/iot_test_mqtt_agent.c \
		../../../../../../../libraries/freertos_plus/standard/crypto/test/iot_test_crypto.c \
		../../../../../../../tests/common/iot_test_freertos.c \
		../../../../../../../libraries/abstractions/platform/freertos/iot_threads_freertos.c

libcommontests-supported-toolchain-y := arm_gcc iar

