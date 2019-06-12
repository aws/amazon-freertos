AMAZON_FREERTOS_SDK_DIR := ../../../../../../../../libraries/c_sdk
AMAZON_FREERTOS_3RD_PARTY_DIR := ../../../../../../../../libraries/3rdparty
AMAZON_FREERTOS_ABSTRACTIONS_DIR := ../../../../../../../../libraries/abstractions
AMAZON_FREERTOS_ARF_PLUS_DIR := ../../../../../../../../libraries/freertos_plus
AMAZON_FREERTOS_ARF_KERNEL := ../../../../../../../../freertos_kernel
AMAZON_FREERTOS_ARF_PORTS := ../../../../../../../../vendors/espressif/boards/esp32/ports
AMAZON_FREERTOS_DEMOS_DIR := ../../../../../../../../demos

COMPONENT_SRCDIRS := $(AMAZON_FREERTOS_SDK_DIR)/standard/mqtt/src \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/src \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/common \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/common/logging \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/common/taskpool \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/src \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/src/services/device_information \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/src/services/mqtt_ble \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/src/services/wifi_provisioning \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/serializer/src \
        $(AMAZON_FREERTOS_SDK_DIR)/aws/defender/src \
        $(AMAZON_FREERTOS_SDK_DIR)/aws/shadow/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/greengrass/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/tls/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/crypto/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_tcp/source \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_tcp/source/portable/BufferManagement \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_tcp/source/portable/NetworkInterface/esp32 \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/provisioning/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/utils/src \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/pkcs11/mbedtls \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/platform/freertos \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/secure_sockets/freertos_plus_tcp \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/jsmn \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/tinycbor \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/pkcs11 \
        $(AMAZON_FREERTOS_ARF_PORTS)/ble \
        $(AMAZON_FREERTOS_ARF_PORTS)/ota \
        $(AMAZON_FREERTOS_ARF_PORTS)/pkcs11 \
        $(AMAZON_FREERTOS_ARF_PORTS)/wifi 
        
COMPONENT_ADD_INCLUDEDIRS := $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_tcp/include \
                             $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_tcp/source/portable/Compiler/GCC \
                             $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/include \
                             $(AMAZON_FREERTOS_3RD_PARTY_DIR)/jsmn \
                             $(AMAZON_FREERTOS_3RD_PARTY_DIR)/tinycbor \
                             $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/platform/freertos/include \
                             $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/platform/include \
                             $(AMAZON_FREERTOS_3RD_PARTY_DIR)/pkcs11 \
                             $(AMAZON_FREERTOS_SDK_DIR)/standard/serializer/include \
                             $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/ble_hal/include \
                             $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/provisioning/include \
                             $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/src \
                             $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/pkcs11/mbedtls 

COMPONENT_OBJEXCLUDE := $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_1.o

ifndef AMAZON_FREERTOS_ENABLE_UNIT_TEST
COMPONENT_SRCDIRS += ../.. \
    $(AMAZON_FREERTOS_DEMOS_DIR)/demo_runner \
    $(AMAZON_FREERTOS_DEMOS_DIR)/shadow \
    $(AMAZON_FREERTOS_DEMOS_DIR)/defender \
    $(AMAZON_FREERTOS_DEMOS_DIR)/mqtt \
    $(AMAZON_FREERTOS_DEMOS_DIR)/mqtt_v4 \
    $(AMAZON_FREERTOS_DEMOS_DIR)/mqtt_v4/network/bluetooth \
    $(AMAZON_FREERTOS_DEMOS_DIR)/greengrass_connectivity \
    $(AMAZON_FREERTOS_DEMOS_DIR)/ota \
    $(AMAZON_FREERTOS_DEMOS_DIR)/ble \
    $(AMAZON_FREERTOS_DEMOS_DIR)/network_manager \
    $(AMAZON_FREERTOS_DEMOS_DIR)/wifi_provisioning \
    $(AMAZON_FREERTOS_DEMOS_DIR)/tcp

COMPONENT_ADD_INCLUDEDIRS += $(AMAZON_FREERTOS_DEMOS_DIR)/include \
                             $(AMAZON_FREERTOS_DEMOS_DIR)/network_manager
else
    $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/test/aws_test_ota_cbor.o \
endif

COMPONENT_PRIV_INCLUDEDIRS := $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/pkcs11 \
                              $(AMAZON_FREERTOS_ARF_PORTS)/ota \
                              $(AMAZON_FREERTOS_ARF_PORTS)/posix \
                              $(AMAZON_FREERTOS_ARF_PORTS)/ble

lib/greengrass/aws_greengrass_discovery.o: CFLAGS+=-Wno-format
lib/common/aws_logging_task_dynamic_buffers.o: CFLAGS+=-Wno-format -Wno-uninitialized

ifndef AMAZON_FREERTOS_ENABLE_UNIT_TEST
        $(AMAZON_FREERTOS_TESTS_DIR)/common \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/common/test \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/ble/test \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/test/access \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/test/system \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/test/unit \
	$(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/test 

        # todo
        $(AMAZON_FREERTOS_TESTS_DIR)/common/memory_leak \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/secure_sockets \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/defender \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/shadow \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/greengrass \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/crypto \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/pkcs11 \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/tls \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/utils/platform \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/ota \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/wifi \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/posix \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/ble \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/wifi_provisioning \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/unity/src \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/unity/extras/fixture/src \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/mqtt \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/mqtt/unit \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/mqtt/system \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/shadow/unit \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/shadow/system

COMPONENT_ADD_INCLUDEDIRS += $(AMAZON_FREERTOS_TESTS_DIR)/include \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/unity/src 

COMPONENT_PRIV_INCLUDEDIRS += $(AMAZON_FREERTOS_3RD_PARTY_DIR)/unity/extras/fixture/src ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/test/access

# Define the board to pass the SOCKETS_Socket_InvalidTooManySockets test.
CFLAGS += -DESP32

tests/common/secure_sockets/aws_test_tcp.o: CFLAGS+=-Wno-uninitialized
tests/common/wifi/aws_test_wifi.o: CFLAGS+=-Wno-uninitialized
tests/common/ota/aws_test_ota_pal.o: CFLAGS+=-Wno-pointer-sign -Wno-sizeof-pointer-memaccess
tests/common/ota/aws_test_ota_agent.o: CFLAGS+=-Wno-pointer-sign
else

demos/common/tcp/aws_tcp_echo_client_single_task.o: CFLAGS+=-Wno-format
endif

