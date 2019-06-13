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
endif

COMPONENT_PRIV_INCLUDEDIRS := $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/pkcs11 \
                              $(AMAZON_FREERTOS_ARF_PORTS)/ota \
                              $(AMAZON_FREERTOS_ARF_PORTS)/posix \
                              $(AMAZON_FREERTOS_ARF_PORTS)/ble

lib/greengrass/aws_greengrass_discovery.o: CFLAGS+=-Wno-format
lib/common/aws_logging_task_dynamic_buffers.o: CFLAGS+=-Wno-format -Wno-uninitialized
demos/common/tcp/aws_tcp_echo_client_single_task.o: CFLAGS+=-Wno-format
