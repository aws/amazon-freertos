# This file contains the description of the source that is provided by amazon not specific
# to cypress.

################################################################################
# freertos_kernel
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/freertos_kernel/*.c)\
	$(wildcard $(CY_AFR_ROOT)/freertos_kernel/portable/$(CY_AFR_TOOLCHAIN)/ARM_CM4F/*.c)\
	$(wildcard $(CY_AFR_ROOT)/freertos_kernel/portable/$(CY_AFR_TOOLCHAIN)/ARM_CM4F/*.s)\
	$(CY_AFR_ROOT)/freertos_kernel/portable/MemMang/heap_3.c

INCLUDES+=\
	$(CY_AFR_ROOT)/freertos_kernel\
	$(CY_AFR_ROOT)/freertos_kernel/include\
	$(CY_AFR_ROOT)/freertos_kernel/portable/$(CY_AFR_TOOLCHAIN)/ARM_CM4F

################################################################################
# demos or tests
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/demos/dev_mode_key_provisioning/src/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/demos/dev_mode_key_provisioning/include

ifeq ($(CY_AFR_IS_TESTING), 0)
SOURCES+=\
	$(CY_AFR_ROOT)/demos/demo_runner/aws_demo.c\
	$(CY_AFR_ROOT)/demos/demo_runner/aws_demo_version.c\
	$(CY_AFR_ROOT)/demos/demo_runner/iot_demo_freertos.c\
	$(CY_AFR_ROOT)/demos/demo_runner/iot_demo_runner.c\
	$(CY_AFR_ROOT)/demos/common/http_demo_helpers/http_demo_utils.c\
	$(CY_AFR_ROOT)/demos/common/mqtt_demo_helpers/mqtt_demo_helpers.c\
	$(CY_AFR_ROOT)/demos/common/pkcs11_helpers/pkcs11_helpers.c\
	$(wildcard $(CY_AFR_ROOT)/demos/greengrass_connectivity/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/device_defender_for_aws/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/device_defender_for_aws/metrics_collector/lwip/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/https/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/mqtt/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/coreMQTT/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/coreHTTP/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/network_manager/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/tcp/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/shadow/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/device_shadow_for_aws/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/jobs_for_aws/*.c)

INCLUDES+=\
	$(CY_EXTAPP_PATH)/common\
	$(CY_AFR_ROOT)/demos/https\
	$(CY_AFR_ROOT)/demos/include\
	$(CY_AFR_ROOT)/demos/network_manager\
	$(CY_AFR_ROOT)/demos/tcp\
	$(CY_AFR_ROOT)/demos/common/http_demo_helpers\
	$(CY_AFR_ROOT)/demos/common/mqtt_demo_helpers\
	$(CY_AFR_ROOT)/demos/common/pkcs11_helpers\
	$(CY_AFR_ROOT)/demos/device_defender_for_aws\
	$(CY_AFR_ROOT)/demos/device_defender_for_aws/metrics_collector/lwip
else

SOURCES+=\
	$(CY_AFR_ROOT)/tests/common/aws_test_framework.c\
	$(CY_AFR_ROOT)/tests/common/aws_test_runner.c\
	$(CY_AFR_ROOT)/tests/common/aws_test.c\
	$(CY_AFR_ROOT)/tests/common/iot_test_freertos.c\
	$(CY_AFR_ROOT)/tests/common/iot_tests_network.c

INCLUDES+=\
	$(CY_EXTAPP_PATH)/common\
	$(CY_AFR_ROOT)/tests/include

endif
################################################################################
# libraries (3rd party)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/api/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/ipv4/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/ipv6/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/ppp/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/ppp/polarssl/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/portable/arch/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip_osal/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/library/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/mbedtls_utils/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/tinycbor/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/unity/extras/fixture/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/unity/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/jsmn/*.c)

ifneq ($(CY_USE_ALL_NETIF),)
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/*.c)
else
SOURCES+=\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/ethernet.c
endif

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/3rdparty/pkcs11\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip_osal/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/include/mbedtls\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls_config/\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls_utils/\
	$(CY_AFR_ROOT)/libraries/3rdparty/tinycbor/src/\
	$(CY_AFR_ROOT)/libraries/3rdparty/unity/extras/fixture/src\
	$(CY_AFR_ROOT)/libraries/3rdparty/unity/src\
	$(CY_AFR_ROOT)/libraries/3rdparty/jsmn\
	$(CY_AFR_VENDOR_PATH)/lwip


################################################################################
# libraries (abstractions)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/backoff_algorithm/source/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/platform/freertos/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/secure_sockets/lwip/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/pkcs11/corePKCS11/source/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/pkcs11/psa/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/transport/secure_sockets/*.c)
	

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/abstractions/backoff_algorithm/source/include\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11/corePKCS11/source/include\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11/psa\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11/corePKCS11/source/portable/mbedtls/include\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11/include\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11/mbedtls\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/freertos\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/freertos/include\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/freertos/include/platform\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/include\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/include/platform\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/include/types\
	$(CY_AFR_ROOT)/libraries/abstractions/secure_sockets\
	$(CY_AFR_ROOT)/libraries/abstractions/secure_sockets/include\
	$(CY_AFR_ROOT)/libraries/abstractions/wifi\
	$(CY_AFR_ROOT)/libraries/abstractions/wifi/include\
	$(CY_AFR_ROOT)/libraries/abstractions/retry_utils\
	$(CY_AFR_ROOT)/libraries/abstractions/transport/secure_sockets

ifneq ($(CY_TFM_PSA_SUPPORTED),1)
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/pkcs11/mbedtls/*c)
endif

ifeq ($(CY_AFR_IS_TESTING), 1)
# Test code
SOURCES+=\
	$(CY_AFR_ROOT)/libraries/abstractions/wifi/test/iot_test_wifi.c\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/pkcs11/test/*.c)\
	$(CY_AFR_ROOT)/libraries/abstractions/secure_sockets/test/iot_test_tcp.c\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/platform/test/*.c)\
	$(CY_AFR_ROOT)/libraries/abstractions/common_io/test/iot_test_common_io.c

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/abstractions/common_io/include\
	$(CY_AFR_ROOT)/libraries/abstractions/common_io/test
endif

################################################################################
# libraries (c_sdk)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/logging/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/taskpool/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/https/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer/src/cbor/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer/src/json/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/aws/defender/src/*c)

# MQTT without ble
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/common\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/common/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/common/include/private\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/common/include/types\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/https\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/https/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/https/include/types\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/https/src\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/https/src/private\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/include/types\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/private\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/include/types\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/defender/include

ifeq ($(CY_AFR_IS_TESTING), 1)
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/test/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/https/test/unit/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/https/test/system/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer/test/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/test/unit/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/test/unit/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/test/system/*.c)\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/test/system/aws_iot_tests_shadow_system.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/test/aws_test_shadow.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/defender/test/unit/aws_iot_tests_defender_unit.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/defender/test/system/aws_iot_tests_defender_system.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/test/mock/iot_tests_mqtt_mock.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/test/iot_test_mqtt_agent.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_serializer_deserializer_wrapper.c\
	$(CY_AFR_ROOT)/tests/integration_test/core_mqtt_system_test.c\
	$(CY_AFR_ROOT)/tests/integration_test/core_http_system_test.c\
	$(CY_AFR_ROOT)/tests/integration_test/shadow_system_test.c

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/https/test/access\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/test/mock\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/test/access\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/src/private\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/src\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/defender/src/private\
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/defender/src\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/include

endif

################################################################################
# libraries (freertos_plus)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/pkcs11/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/aws/greengrass/src/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_posix\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_posix/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/pkcs11\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/pkcs11/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/greengrass/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/greengrass/src

ifeq ($(CY_AFR_IS_TESTING), 1)
# Test code
SOURCES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/test/iot_test_crypto.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls/test/iot_test_tls.c\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/aws/greengrass/test/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/greengrass/test
endif

ifneq ($(CY_USE_FREERTOS_PLUS_TCP),)
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/*.c)\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_2.c\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/NetworkInterface/board_family/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/$(CY_AFR_TOOLCHAIN)/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/pkcs11/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils/src/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/$(CY_AFR_TOOLCHAIN)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/$(CY_AFR_TOOLCHAIN)

endif

################################################################################
# core MQTT
################################################################################
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/coreMQTT/source/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/coreMQTT/source/include\
	$(CY_AFR_ROOT)/libraries/coreMQTT/portable/

################################################################################
# core HTTP
################################################################################
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/coreHTTP/source/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/coreHTTP/source/dependency/3rdparty/http_parser/http_parser.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/coreHTTP/source/include\
	$(CY_AFR_ROOT)/libraries/coreHTTP/source/interface\
	$(CY_AFR_ROOT)/libraries/coreHTTP/source/dependency/3rdparty/http_parser

################################################################################
# Logging
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/logging/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/logging/include


################################################################################
# Json
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/coreJSON/source/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/coreJSON/source/include

################################################################################
# Jobs for aws
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/jobs_for_aws/source/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/jobs_for_aws/source/include

################################################################################
# Shadow
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/device_shadow_for_aws/source/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/device_shadow_for_aws/source/include


################################################################################
# Defender
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/device_defender_for_aws/source/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/device_defender_for_aws/source/include