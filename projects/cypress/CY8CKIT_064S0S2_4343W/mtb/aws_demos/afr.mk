################################################################################
# \file afr.mk
# \version 1.0
#
# \brief
# Configuration file for adding/removing aFR files. Modify this file
# to suit the application needs.
#
################################################################################
# \copyright
# Copyright 2019 Cypress Semiconductor Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################


################################################################################
# Global settings
################################################################################

CY_DEVICESUPPORT_PATH=$(CY_AFR_ROOT)/vendors/cypress/MTB/psoc6/psoc6pdl

MCUBOOT_CYFLASH_PAL_DIR=$(CY_AFR_ROOT)/vendors/cypress/MTB/ota/mcuboot/cy_flash_pal

# Artifact locations for launch configs
CY_AFR_APPLOC=$(notdir $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST))))))
CY_SYM_FILE=\$$\{cy_prj_path\}/$(CY_BUILD_RELATIVE_LOCATION)/$(CY_AFR_APPLOC)/$(TARGET)/$(CONFIG)/$(APPNAME).elf
CY_PROG_FILE=\$$\{cy_prj_path\}/$(CY_BUILD_RELATIVE_LOCATION)/$(CY_AFR_APPLOC)/$(TARGET)/$(CONFIG)/$(APPNAME).hex

# Resolve toolchain name
ifeq ($(TOOLCHAIN),GCC_ARM)
CY_AFR_TOOLCHAIN=GCC
CY_AFR_TOOLCHAIN_LS_EXT=ld
else ifeq ($(TOOLCHAIN),IAR)
CY_AFR_TOOLCHAIN=$(TOOLCHAIN)
CY_AFR_TOOLCHAIN_LS_EXT=icf
else ifeq ($(TOOLCHAIN),ARM)
# armclang is more compatible with GCC than RVDS
CY_AFR_TOOLCHAIN=GCC
CY_AFR_TOOLCHAIN_LS_EXT=sct
endif

# Resolve target name and path
CY_AFR_TARGET=$(subst -,_,$(TARGET))

# Explicitly add the BSP makefile
CY_EXTRA_INCLUDES=$(CY_AFR_BOARD_APP_PATH)/$(TARGET).mk

# Explicitly set the linker script
LINKER_SCRIPT=$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.$(CY_AFR_TOOLCHAIN_LS_EXT))

# Use auto-discovery for cypress files and set the paths
CY_AFR_VENDOR_PATH=$(CY_AFR_ROOT)/vendors/cypress
CY_EXTAPP_PATH=$(CY_AFR_VENDOR_PATH)/MTB
CY_AFR_BOARD_PATH=$(CY_AFR_VENDOR_PATH)/boards/$(CY_AFR_TARGET)
CY_AFR_BOARD_APP_PATH=$(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/application_code/cy_code
MCUBOOT_DIR=$(CY_EXTAPP_PATH)/ota/mcuboot

# project directory
CY_PROJECT_DIR:=.

ifeq ($(TOOLCHAIN),IAR)
CFLAGS+=--dlib_config=full
CXXFLAGS+=--dlib_config=full --guard_calls
LDFLAGS+=--threaded_lib
DEFINES+=LWIP_ERRNO_INCLUDE=\"cy_lwip_errno_iar.h\"
SOURCES+=$(CY_AFR_ROOT)/freertos_kernel/portable/IAR/ARM_CM4F/portasm.s
endif

ifeq ($(TOOLCHAIN),ARM)
DEFINES+=LWIP_ERRNO_INCLUDE=\"cy_lwip_errno_armcc.h\"
# As of ARM Compiler 6.12, <assert.h> does not define static_assert (ISO/IEC 9899:2011).
# This define may interfere with <cassert> or future versions of the ARM C library.
DEFINES+=static_assert=_Static_assert
endif

################################################################################
# vendors/cypress
################################################################################

# Cypress-specific directories and files to ignore
CY_IGNORE+=\
	$(CY_EXTAPP_PATH)/ota\
	$(CY_EXTAPP_PATH)/common\
	$(CY_EXTAPP_PATH)/bluetooth\
	$(CY_AFR_VENDOR_PATH)/boards\
	$(CY_AFR_VENDOR_PATH)/lwip\
	$(CY_AFR_VENDOR_PATH)/WICED_SDK\
	$(CY_AFR_VENDOR_PATH)/freertos_thirdparty_port


CY_CONFIG_MODUS_FILE=./$(CY_AFR_BOARD_APP_PATH)/design.modus

SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/application_code/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/GeneratedSource/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.S)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.s)\
	$(wildcard $(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/config_files/*.c)\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/wifi/*.c)\

INCLUDES+=\
	$(CY_PROJECT_DIR)/\
	$(CY_PROJECT_DIR)/include\
	$(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/application_code\
	$(CY_AFR_BOARD_APP_PATH)\
	$(CY_AFR_BOARD_APP_PATH)/GeneratedSource\
	$(CY_AFR_BOARD_APP_PATH)/startup\
	$(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/config_files\
	$(CY_AFR_BOARD_PATH)/ports/wifi

ifneq ($(CY_TFM_PSA_SUPPORTED),)
SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/pkcs11/*.c)

INCLUDES+=\
	$(CY_AFR_BOARD_PATH)/ports/pkcs11
else
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/pkcs11/psa/*.c)\
	$(CY_AFR_BOARD_PATH)/ports/pkcs11/hw_poll.c

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11/psa\
	$(CY_EXTAPP_PATH)/psoc6/psoc64tfm/COMPONENT_TFM_NS_INTERFACE/include
endif

# SDIO_HOST sources and includes
ifneq ($(filter $(TARGET),CY8CKIT-062-WIFI-BT CYW943012P6EVB-01),)
SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/SDIO_HOST/*.c)

INCLUDES+=\
	$(CY_AFR_BOARD_APP_PATH)/SDIO_HOST
endif

# enable TFM
CY_TFM_ENABLED=true

################################################################################
# freertos_kernel
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/freertos_kernel/*.c)\
	$(wildcard $(CY_AFR_ROOT)/freertos_kernel/portable/$(CY_AFR_TOOLCHAIN)/ARM_CM4F/*.c)\
	$(wildcard $(CY_AFR_ROOT)/freertos_kernel/portable/$(CY_AFR_TOOLCHAIN)/ARM_CM4F/*.s)\
	$(CY_AFR_ROOT)/freertos_kernel/portable/MemMang/heap_4.c

INCLUDES+=\
	$(CY_AFR_ROOT)/freertos_kernel\
	$(CY_AFR_ROOT)/freertos_kernel/include\
	$(CY_AFR_ROOT)/freertos_kernel/portable/$(CY_AFR_TOOLCHAIN)/ARM_CM4F

################################################################################
# demos
################################################################################

SOURCES+=\
	$(CY_AFR_ROOT)/demos/demo_runner/aws_demo.c\
	$(CY_AFR_ROOT)/demos/demo_runner/aws_demo_version.c\
	$(CY_AFR_ROOT)/demos/demo_runner/iot_demo_freertos.c\
	$(CY_AFR_ROOT)/demos/demo_runner/iot_demo_runner.c\
	$(CY_AFR_ROOT)/demos/common/http_demo_helpers/http_demo_utils.c\
	$(CY_AFR_ROOT)/demos/common/mqtt_demo_helpers/mqtt_demo_helpers.c\
	$(wildcard $(CY_AFR_ROOT)/demos/dev_mode_key_provisioning/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/greengrass_connectivity/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/defender/*.c)\
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
	$(CY_AFR_ROOT)/demos/dev_mode_key_provisioning\
	$(CY_AFR_ROOT)/demos/dev_mode_key_provisioning/include\
	$(CY_AFR_ROOT)/demos/https\
	$(CY_AFR_ROOT)/demos/include\
	$(CY_AFR_ROOT)/demos/network_manager\
	$(CY_AFR_ROOT)/demos/tcp\
	$(CY_AFR_ROOT)/demos/common/http_demo_helpers\
	$(CY_AFR_ROOT)/demos/common/mqtt_demo_helpers\

################################################################################
# libraries (3rd party)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/api/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/ipv4/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/ipv6/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/ppp/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/ppp/polarssl/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/portable/arch/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip_osal/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/library/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/mbedtls_utils/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/tinycbor/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/unity/extras/fixture/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/unity/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/jsmn/*.c)

ifneq ($(CY_USE_ALL_NETIF),)
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/*c)
else
SOURCES+=\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/ethernet.c
endif

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/3rdparty/pkcs11\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip_osal/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls_config\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/include/mbedtls\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls_utils\
	$(CY_AFR_ROOT)/libraries/3rdparty/tinycbor/src\
	$(CY_AFR_ROOT)/libraries/3rdparty/unity/extras/fixture/src\
	$(CY_AFR_ROOT)/libraries/3rdparty/unity/src\
	$(CY_AFR_ROOT)/libraries/3rdparty/jsmn\
	$(CY_AFR_VENDOR_PATH)/lwip


################################################################################
# libraries (abstractions)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/platform/freertos/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/secure_sockets/lwip/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/pkcs11/corePKCS11/source/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/transport/secure_sockets/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/retry_utils/freertos/*.c)\

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11/corePKCS11/source/include\
	$(CY_AFR_ROOT)/libraries/abstractions/pkcs11/corePKCS11/source/portable/mbedtls/include\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/freertos\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/freertos/include\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/freertos/include/platform\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/include\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/include/platform\
	$(CY_AFR_ROOT)/libraries/abstractions/platform/include/types\
	$(CY_AFR_ROOT)/libraries/abstractions/retry_utils\
	$(CY_AFR_ROOT)/libraries/abstractions/secure_sockets\
	$(CY_AFR_ROOT)/libraries/abstractions/secure_sockets/include\
	$(CY_AFR_ROOT)/libraries/abstractions/transport/secure_sockets\
	$(CY_AFR_ROOT)/libraries/abstractions/wifi\
	$(CY_AFR_ROOT)/libraries/abstractions/wifi/include

ifneq ($(CY_TFM_PSA_SUPPORTED),)
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/pkcs11/corePKCS11/source/portable/mbedtls/*.c)

endif

################################################################################
# libraries (c_sdk)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/logging/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/taskpool/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/https/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer/src/cbor/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/serializer/src/json/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/aws/shadow/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/aws/defender/src/*c)

# MQTT without ble
SOURCES+=\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_agent.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_api.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_network.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_operation.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_static_memory.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_subscription.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_validate.c \
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_context_connection.c \
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_serializer_deserializer_wrapper.c \
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_managed_function_wrapper.c \
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_subscription_container.c \
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_mutex_wrapper.c \
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_publish_duplicates.c \
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_validate.c

# Device Shadow, Device Defender, Jobs, coreJSON, coreMQTT and coreHTTP
SOURCES+=\
	$(CY_AFR_ROOT)/libraries/device_shadow_for_aws/source/shadow.c\
	$(CY_AFR_ROOT)/libraries/coreJSON/source/core_json.c \
	$(CY_AFR_ROOT)/libraries/device_defender_for_aws/source/defender.c\
	$(CY_AFR_ROOT)/libraries/jobs_for_aws/source/jobs.c\
	$(CY_AFR_ROOT)/libraries/coreMQTT/source/core_mqtt_serializer.c\
	$(CY_AFR_ROOT)/libraries/coreMQTT/source/core_mqtt_state.c\
	$(CY_AFR_ROOT)/libraries/coreMQTT/source/core_mqtt.c\
	$(CY_AFR_ROOT)/libraries/coreHTTP/source/core_http_client.c\
	$(CY_AFR_ROOT)/libraries/coreHTTP/source/dependency/3rdparty/http_parser/http_parser.c

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/common\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/common/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/common/include/private\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/common/include/types\
	$(CY_AFR_ROOT)/libraries/logging/include\
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
	$(CY_AFR_ROOT)/libraries/c_sdk/aws/defender/include\
	$(CY_AFR_ROOT)/libraries/coreMQTT/source/include\
	$(CY_AFR_ROOT)/libraries/coreMQTT/source/interface\
	$(CY_AFR_ROOT)/libraries/coreHTTP/source/include\
	$(CY_AFR_ROOT)/libraries/coreHTTP/source/interface\
	$(CY_AFR_ROOT)/libraries/coreHTTP/source/dependency/3rdparty/http_parser\
	$(CY_AFR_ROOT)/libraries/device_shadow_for_aws/source/include\
	$(CY_AFR_ROOT)/libraries/coreJSON/source/include\
	$(CY_AFR_ROOT)/libraries/device_defender_for_aws/source/include\
	$(CY_AFR_ROOT)/libraries/jobs_for_aws/source/include

################################################################################
# libraries (freertos_plus)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/aws/greengrass/src/*.c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_posix\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_posix/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/greengrass/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/greengrass/src


################################################################################
# Additional Source files and includes needed for BLE support
################################################################################

ifneq ($(BLE_SUPPORT),)
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/ble_hal/*c)\
	$(wildcard $(CY_AFR_ROOT)/demos/ble/gatt_server/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/ble/mqtt_ble/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/ble/shadow_ble/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/ble/numeric_comparison/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/services/device_information/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/services/mqtt_ble/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/services/data_transfer/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/services/wifi_provisioning/*c)\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/ble/*c)\
	$(wildcard $(CY_EXTAPP_PATH)/bluetooth/psoc6/cyosal/src/*.c)\
	$(wildcard $(CY_EXTAPP_PATH)/bluetooth/psoc6/cyhal/src/*.c)

INCLUDES+=\
	$(CY_AFR_BOARD_PATH)/ports/ble\
	$(CY_EXTAPP_PATH)/bluetooth/include\
	$(CY_EXTAPP_PATH)/bluetooth/include/stackHeaders\
	$(CY_EXTAPP_PATH)/bluetooth/psoc6/cyhal/include\
	$(CY_EXTAPP_PATH)/bluetooth/psoc6/cyosal/include\
	$(CY_AFR_ROOT)/libraries/abstractions/ble_hal\
	$(CY_AFR_ROOT)/libraries/abstractions/ble_hal/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src
	

LDLIBS=\
	$(CY_EXTAPP_PATH)/bluetooth/bluetooth.FreeRTOS.ARM_CM4.release.a\
	$(CY_EXTAPP_PATH)/bluetooth/shim.FreeRTOS.ARM_CM4.release.a
endif

################################################################################
# Additional Source files and includes needed for OTA support
################################################################################

ifeq ($(OTA_SUPPORT),1)
DEFINES+=CY_BOOT_USE_EXTERNAL_FLASH

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/demos/ota/*.c)\
	$(wildcard $(CY_EXTAPP_PATH)/ota/ports/$(CY_AFR_TARGET)/*.c)\
	$(CY_AFR_ROOT)/demos/demo_runner/aws_demo_version.c\
	$(CY_AFR_ROOT)/demos/demo_runner/iot_demo_freertos.c\
	$(CY_AFR_ROOT)/demos/demo_runner/iot_demo_runner.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/aws_iot_ota_agent.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/aws_iot_ota_interface.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/http/aws_iot_ota_http.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/mqtt/aws_iot_ota_mqtt.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/mqtt/aws_iot_ota_cbor.c\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/http/*.c)\
	$(CY_AFR_ROOT)/libraries/3rdparty/jsmn/jsmn.c\
	$(CY_EXTAPP_PATH)/common/utilities/JSON_parser/JSON.c\
	$(CY_EXTAPP_PATH)/common/utilities/untar/untar.c\
	$(MCUBOOT_CYFLASH_PAL_DIR)/cy_flash_map.c\
	$(MCUBOOT_CYFLASH_PAL_DIR)/flash_qspi/flash_qspi.c\
	$(MCUBOOT_CYFLASH_PAL_DIR)/cy_flash_psoc6.c\
	$(MCUBOOT_CYFLASH_PAL_DIR)/cy_smif_psoc6.c\
	$(MCUBOOT_DIR)/bootutil/src/bootutil_misc.c
	
INCLUDES+=\
	$(MCUBOOT_DIR)\
	$(MCUBOOT_DIR)/mcuboot_header\
	$(MCUBOOT_DIR)/bootutil/include\
	$(MCUBOOT_DIR)/sysflash\
	$(MCUBOOT_CYFLASH_PAL_DIR)\
	$(MCUBOOT_CYFLASH_PAL_DIR)/include\
	$(MCUBOOT_CYFLASH_PAL_DIR)/include/flash_map_backend\
	$(MCUBOOT_CYFLASH_PAL_DIR)/flash_qspi\
	$(CY_EXTAPP_PATH)/common/utilities/JSON_parser\
	$(CY_EXTAPP_PATH)/common/utilities/untar\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/jsmn\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/test\
	$(CY_AFR_ROOT)/libraries/abstractions/wifi/include
else
INCLUDES+=\
	$(MCUBOOT_DIR)/sysflash
endif