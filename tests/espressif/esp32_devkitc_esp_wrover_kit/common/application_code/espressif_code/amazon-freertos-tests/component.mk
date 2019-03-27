#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behavior is entirely configurable,
# please read the ESP-IDF documents if you need to do this.

AMAZON_FREERTOS_ROOT_DIR := ../../../../../../..
AMAZON_FREERTOS_LIB_DIR := $(AMAZON_FREERTOS_ROOT_DIR)/lib
AMAZON_FREERTOS_TESTS_DIR := $(AMAZON_FREERTOS_ROOT_DIR)/tests
AMAZON_FREERTOS_DEMOS_DIR := $(AMAZON_FREERTOS_ROOT_DIR)/demos


#--------------------------------------------------------------------------------------
# Amazon FreeRTOS Libraries
#--------------------------------------------------------------------------------------
COMPONENT_SRCDIRS := $(AMAZON_FREERTOS_LIB_DIR)/mqtt \
        $(AMAZON_FREERTOS_LIB_DIR)/shadow \
        $(AMAZON_FREERTOS_LIB_DIR)/greengrass \
        $(AMAZON_FREERTOS_LIB_DIR)/utils \
        $(AMAZON_FREERTOS_LIB_DIR)/utils/taskpool \
        $(AMAZON_FREERTOS_LIB_DIR)/common \
        $(AMAZON_FREERTOS_LIB_DIR)/utils/platform \
        $(AMAZON_FREERTOS_LIB_DIR)/bufferpool \
        $(AMAZON_FREERTOS_LIB_DIR)/tls \
        $(AMAZON_FREERTOS_LIB_DIR)/ota \
        $(AMAZON_FREERTOS_LIB_DIR)/ota/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/crypto \
        $(AMAZON_FREERTOS_LIB_DIR)/wifi/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/secure_sockets \
        $(AMAZON_FREERTOS_LIB_DIR)/secure_sockets/portable/freertos_plus_tcp \
        $(AMAZON_FREERTOS_LIB_DIR)/pkcs11/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/pkcs11/mbedtls \
        $(AMAZON_FREERTOS_ROOT_DIR)/demos/common/logging \
        $(AMAZON_FREERTOS_ROOT_DIR)/demos/common/devmode_key_provisioning \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/BufferManagement \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/esp32 \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/jsmn \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/tinycbor \
        $(AMAZON_FREERTOS_LIB_DIR)/bluetooth_low_energy \
        $(AMAZON_FREERTOS_LIB_DIR)/bluetooth_low_energy/services/wifi_provisioning \
        $(AMAZON_FREERTOS_LIB_DIR)/bluetooth_low_energy/services/device_information \
        $(AMAZON_FREERTOS_LIB_DIR)/bluetooth_low_energy/services/mqtt_ble \
        $(AMAZON_FREERTOS_LIB_DIR)/bluetooth_low_energy/portable/espressif \
        $(AMAZON_FREERTOS_LIB_DIR)/metrics \
        $(AMAZON_FREERTOS_LIB_DIR)/serializer/cbor \
        $(AMAZON_FREERTOS_LIB_DIR)/serializer/json

COMPONENT_ADD_INCLUDEDIRS := $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/include \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/jsmn \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/include/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/tinycbor \
	$(AMAZON_FREERTOS_LIB_DIR)/include/bluetooth_low_energy \
	$(AMAZON_FREERTOS_LIB_DIR)/include/bluetooth_low_energy/private \
	$(AMAZON_FREERTOS_LIB_DIR)/bluetooth_low_energy/portable/espressif

COMPONENT_OBJEXCLUDE := $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_1.o \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.o \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.o \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.o \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.o \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.o \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/ota/aws_test_ota_cbor.o \

COMPONENT_PRIV_INCLUDEDIRS := $(AMAZON_FREERTOS_LIB_DIR)/third_party/pkcs11 $(AMAZON_FREERTOS_LIB_DIR)/ota/portable/espressif/esp32_devkitc_esp_wrover_kit

lib/greengrass/aws_greengrass_discovery.o: CFLAGS+=-Wno-format
demos/common/logging/aws_logging_task_dynamic_buffers.o: CFLAGS+=-Wno-format -Wno-uninitialized

#--------------------------------------------------------------------------------------
# Amazon FreeRTOS Tests
#--------------------------------------------------------------------------------------
COMPONENT_SRCDIRS += ../.. \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/test_runner \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/framework \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/memory_leak \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/secure_sockets \
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
        $(AMAZON_FREERTOS_DEMOS_DIR)/common/ota \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/unity/src \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/unity/extras/fixture/src \
        $(AMAZON_FREERTOS_DEMOS_DIR)/common/ota \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/mqtt \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/mqtt/unit \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/mqtt/system \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/shadow/unit \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/shadow/system

COMPONENT_ADD_INCLUDEDIRS += $(AMAZON_FREERTOS_TESTS_DIR)/common/include \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/unity/src \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/ota \
        $(AMAZON_FREERTOS_DEMOS_DIR)/common/include \
        $(AMAZON_FREERTOS_TESTS_DIR)/common/mqtt/access

COMPONENT_PRIV_INCLUDEDIRS += $(AMAZON_FREERTOS_LIB_DIR)/third_party/unity/extras/fixture/src $(AMAZON_FREERTOS_LIB_DIR)/third_party/pkcs11

# Define the board to pass the SOCKETS_Socket_InvalidTooManySockets test.
CFLAGS += -DIOT_CONFIG_FILE="\"iot_test_config.h\""

tests/common/secure_sockets/aws_test_tcp.o: CFLAGS+=-Wno-uninitialized
tests/common/wifi/aws_test_wifi.o: CFLAGS+=-Wno-uninitialized
tests/common/ota/aws_test_ota_pal.o: CFLAGS+=-Wno-pointer-sign -Wno-sizeof-pointer-memaccess
tests/common/ota/aws_test_ota_agent.o: CFLAGS+=-Wno-pointer-sign
