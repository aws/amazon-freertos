#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behavior is entirely configurable,
# please read the ESP-IDF documents if you need to do this.

AMAZON_FREERTOS_SDK_DIR := ../../../../../../../../libraries/c_sdk
AMAZON_FREERTOS_3RD_PARTY_DIR := ../../../../../../../../libraries/3rdparty
AMAZON_FREERTOS_ABSTRACTIONS_DIR := ../../../../../../../../libraries/abstractions
AMAZON_FREERTOS_ARF_PLUS_DIR := ../../../../../../../../libraries/freertos_plus
AMAZON_FREERTOS_ARF_KERNEL := ../../../../../../../../freertos_kernel
AMAZON_FREERTOS_ARF_PORTS := ../../../../../../../../vendors/espressif/boards/esp32/ports
AMAZON_FREERTOS_TESTS_DIR := ../../../../../../../../tests
AMAZON_FREERTOS_DEMOS_DIR := ../../../../../../../../demos

ifndef AMAZON_FREERTOS_ENABLE_UNIT_TEST
AMAZON_FREERTOS_ESP32 := ../../../../../../../../vendors/espressif/boards/esp32/aws_demos
else
AMAZON_FREERTOS_ESP32 := ../../../../../../../../vendors/espressif/boards/esp32/aws_tests
endif

#--------------------------------------------------------------------------------------
# FreeRTOS Libraries
#--------------------------------------------------------------------------------------
COMPONENT_SRCDIRS := $(AMAZON_FREERTOS_SDK_DIR)/standard/mqtt/src \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/src \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/common \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/common/logging \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/common/taskpool \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/src \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/src/services/device_information \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/src/services/mqtt_ble \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/src/services/wifi_provisioning \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/https/src \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/serializer/src \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/serializer/src/cbor \
        $(AMAZON_FREERTOS_SDK_DIR)/standard/serializer/src/json \
        $(AMAZON_FREERTOS_SDK_DIR)/aws/defender/src \
        $(AMAZON_FREERTOS_SDK_DIR)/aws/shadow/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/greengrass/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/src/mqtt \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/src/http \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/tls/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/pkcs11/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/crypto/src \
        $(AMAZON_FREERTOS_DEMOS_DIR)/dev_mode_key_provisioning/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/utils/src \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/pkcs11/mbedtls \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/platform/freertos \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/http_parser \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/jsmn \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/tinycbor/src \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/pkcs11 \
        $(AMAZON_FREERTOS_ARF_PORTS)/ble \
        $(AMAZON_FREERTOS_ARF_PORTS)/ota \
        $(AMAZON_FREERTOS_ARF_PORTS)/pkcs11 \
        $(AMAZON_FREERTOS_ARF_PORTS)/wifi

COMPONENT_ADD_INCLUDEDIRS := $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/include \
                             ${AMAZON_FREERTOS_ABSTRACTIONS_DIR}/posix/include \
                             $(AMAZON_FREERTOS_SDK_DIR)/standard/ble/include \
                             $(AMAZON_FREERTOS_SDK_DIR)/standard/https/include \
                             $(AMAZON_FREERTOS_3RD_PARTY_DIR)/http_parser \
                             $(AMAZON_FREERTOS_3RD_PARTY_DIR)/mbedtls_utils \
                             $(AMAZON_FREERTOS_3RD_PARTY_DIR)/jsmn \
                             $(AMAZON_FREERTOS_3RD_PARTY_DIR)/tinycbor/src \
                             $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/platform/freertos/include \
                             $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/platform/include \
                             $(AMAZON_FREERTOS_3RD_PARTY_DIR)/pkcs11 \
                             $(AMAZON_FREERTOS_SDK_DIR)/standard/serializer/include \
                             $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/ble_hal/include \
                             $(AMAZON_FREERTOS_DEMOS_DIR)/dev_mode_key_provisioning/include \
                             $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/src \
                             $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/pkcs11/mbedtls

COMPONENT_OBJEXCLUDE := $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source/FreeRTOS_POSIX_mqueue.o \
                        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source/FreeRTOS_POSIX_pthread.o \
                        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source/FreeRTOS_POSIX_pthread_barrier.o \
                        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source/FreeRTOS_POSIX_pthread_cond.o \
                        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source/FreeRTOS_POSIX_pthread_mutex.o \
                        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source/FreeRTOS_POSIX_sched.o \
                        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source/FreeRTOS_POSIX_semaphore.o \
                        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source/FreeRTOS_POSIX_unistd.o

COMPONENT_PRIV_INCLUDEDIRS := $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/pkcs11 \
                              $(AMAZON_FREERTOS_ARF_PORTS)/ota \
                              $(AMAZON_FREERTOS_ARF_PORTS)/posix \
                              $(AMAZON_FREERTOS_ARF_PORTS)/ble

lib/greengrass/aws_greengrass_discovery.o: CFLAGS+=-Wno-format
lib/common/aws_logging_task_dynamic_buffers.o: CFLAGS+=-Wno-format -Wno-uninitialized
libraries/c_sdk/aws/defender/src/aws_iot_defender_api.o: CFLAGS+=-Wno-unused-but-set-variable

ifdef AMAZON_FREERTOS_ENABLE_UNIT_TEST
COMPONENT_SRCDIRS += ../.. \
        $(AMAZON_FREERTOS_ESP32)/common \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/common/test \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/ble/test \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/https/test/system \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/https/test/unit \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/test \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/test/system \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/test/unit \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/serializer/test \
        ${AMAZON_FREERTOS_SDK_DIR}/aws/defender/test \
        ${AMAZON_FREERTOS_SDK_DIR}/aws/shadow/test \
        ${AMAZON_FREERTOS_SDK_DIR}/aws/shadow/test/unit \
        ${AMAZON_FREERTOS_SDK_DIR}/aws/shadow/test/system \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/ble_hal/test \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/pkcs11/test \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/platform/test \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/secure_sockets/test \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/wifi/test \
        ${AMAZON_FREERTOS_ARF_PLUS_DIR}/aws/greengrass/test \
        ${AMAZON_FREERTOS_ARF_PLUS_DIR}/aws/ota/test \
        ${AMAZON_FREERTOS_ARF_PLUS_DIR}/standard/crypto/test \
        ${AMAZON_FREERTOS_ARF_PLUS_DIR}/standard/tls/test \
        ${AMAZON_FREERTOS_3RD_PARTY_DIR}/unity/src \
        ${AMAZON_FREERTOS_3RD_PARTY_DIR}/unity/extras/fixture/src \
        $(AMAZON_FREERTOS_ARF_PLUS_DIR)/standard/freertos_plus_posix/source \
        ${AMAZON_FREERTOS_TESTS_DIR}/common


COMPONENT_ADD_INCLUDEDIRS += $(AMAZON_FREERTOS_TESTS_DIR)/include \
        $(AMAZON_FREERTOS_3RD_PARTY_DIR)/unity/src \
        $(AMAZON_FREERTOS_DEMOS_DIR)/include \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/ble/test \
        ${AMAZON_FREERTOS_ARF_PLUS_DIR}/aws/greengrass/test \
        ${AMAZON_FREERTOS_ARF_PLUS_DIR}/aws/ota/test \
        $(AMAZON_FREERTOS_ABSTRACTIONS_DIR)/wifi/test \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/https/src \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/src \
        ${AMAZON_FREERTOS_SDK_DIR}/aws/defender/src/private \
        ${AMAZON_FREERTOS_SDK_DIR}/aws/shadow/src \
        ${AMAZON_FREERTOS_ARF_PLUS_DIR}/aws/greengrass/src

COMPONENT_OBJEXCLUDE += $(AMAZON_FREERTOS_ARF_PLUS_DIR)/aws/ota/test/aws_test_ota_cbor.o \

COMPONENT_PRIV_INCLUDEDIRS += $(AMAZON_FREERTOS_3RD_PARTY_DIR)/unity/extras/fixture/src \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/mqtt/test/access \
        ${AMAZON_FREERTOS_SDK_DIR}/standard/https/test/access

# Define the board to pass the SOCKETS_Socket_InvalidTooManySockets test.
CFLAGS += -DESP32

tests/common/secure_sockets/aws_test_tcp.o: CFLAGS+=-Wno-uninitialized
tests/common/wifi/aws_test_wifi.o: CFLAGS+=-Wno-uninitialized
tests/common/ota/aws_test_ota_pal.o: CFLAGS+=-Wno-pointer-sign -Wno-sizeof-pointer-memaccess
tests/common/ota/aws_test_ota_agent.o: CFLAGS+=-Wno-pointer-sign
else

COMPONENT_SRCDIRS += ../.. \
    $(AMAZON_FREERTOS_DEMOS_DIR)/shadow \
    $(AMAZON_FREERTOS_DEMOS_DIR)/defender \
    $(AMAZON_FREERTOS_DEMOS_DIR)/https \
    $(AMAZON_FREERTOS_DEMOS_DIR)/mqtt \
    $(AMAZON_FREERTOS_DEMOS_DIR)/mqtt_v4 \
    $(AMAZON_FREERTOS_DEMOS_DIR)/mqtt_v4/network/bluetooth \
    $(AMAZON_FREERTOS_DEMOS_DIR)/greengrass_connectivity \
    $(AMAZON_FREERTOS_DEMOS_DIR)/ota \
    $(AMAZON_FREERTOS_DEMOS_DIR)/ble \
    $(AMAZON_FREERTOS_DEMOS_DIR)/network_manager \
    $(AMAZON_FREERTOS_DEMOS_DIR)/wifi_provisioning \
    $(AMAZON_FREERTOS_DEMOS_DIR)/demo_runner \

COMPONENT_ADD_INCLUDEDIRS += $(AMAZON_FREERTOS_DEMOS_DIR)/include \
                             $(AMAZON_FREERTOS_DEMOS_DIR)/network_manager

demos/common/tcp/aws_tcp_echo_client_single_task.o: CFLAGS+=-Wno-format
endif

ifndef AFR_ESP_FREERTOS_TCP
COMPONENT_OBJEXCLUDE += $(AMAZON_FREERTOS_DEMOS_DIR)/demo_runner/aws_demo_network_addr.o
endif
