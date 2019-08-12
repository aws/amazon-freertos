# Copyright (C) 2008-2019, Marvell International Ltd.
# All Rights Reserved.

libs-y += libcommontests

global-cflags-y += -I$(d)/../../../../../common/include \
		-I$(d)/../../config_files \
		-I$(d)/../../../../../common/ota \
		-I$(d)/../../../../../../lib/third_party/mbedtls/include \
		-I$(d)/../../../../../../demos/common/include



libcommontests-objs-y := \
		../../../../../../../libraries/abstractions/pkcs11/test/aws_test_pkcs11.c \
		../../../../../../../tests/common/aws_test_framework.c \
		../../../../../../../libraries/c_sdk/standard/common/test/aws_memory_leak.c \
		../../../../../../../tests/common/aws_test_runner.c \
		../../../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/test/aws_test_freertos_tcp.c \
		../../../../../../../libraries/freertos_plus/standard/tls/test/aws_test_tls.c \
		../../../../../../../libraries/abstractions/wifi/test/aws_test_wifi.c \
		../../../../../../../libraries/abstractions/secure_sockets/test/aws_test_tcp.c \
		../../../../../../../libraries/c_sdk/standard/mqtt/test/aws_test_mqtt_agent.c \
		../../../../../../../libraries/freertos_plus/standard/crypto/test/aws_test_crypto.c


libcommontests-supported-toolchain-y := arm_gcc iar

