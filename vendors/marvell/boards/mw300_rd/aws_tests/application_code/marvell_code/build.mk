# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libcommontests

global-cflags-y += -I$(d)/../../../../../common/include \
		-I$(d)/../../config_files \
		-I$(d)/../../../../../common/ota \
		-I$(d)/../../../../../../lib/third_party/mbedtls/include \
		-I$(d)/../../../../../../demos/common/include



libcommontests-objs-y := \
		../../../../../common/pkcs11/iot_test_pkcs11.c \
		../../../../../common/framework/aws_test_framework.c \
		../../../../../common/memory_leak/iot_memory_leak.c \
		../../../../../common/test_runner/aws_test_runner.c \
		../../../../../common/freertos_tcp/iot_test_freertos_tcp.c \
		../../../../../common/tls/iot_test_tls.c \
		../../../../../common/wifi/iot_test_wifi.c \
		../../../../../common/secure_sockets/iot_test_tcp.c \
		../../../../../common/mqtt/iot_test_mqtt_agent.c \
		../../../../../common/mqtt/aws_test_mqtt_lib.c \
                ../../../../../common/ota/aws_test_ota_agent.c \
		../../../../../common/ota/aws_test_ota_pal.c \
		../../../../../common/crypto/iot_test_crypto.c


libcommontests-supported-toolchain-y := arm_gcc iar

