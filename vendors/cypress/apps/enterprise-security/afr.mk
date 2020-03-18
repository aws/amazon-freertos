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

CY_DEVICESUPPORT_PATH=$(CY_AFR_ROOT)/vendors/cypress/psoc6/psoc6pdl

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
CY_AFR_TOOLCHAIN=$(TOOLCHAIN)
CY_AFR_TOOLCHAIN_LS_EXT=sct
endif

# Resolve target name and path
CY_AFR_TARGET=$(subst -,_,$(TARGET))

# Explicitly add the BSP makefile
CY_EXTRA_INCLUDES=$(CY_AFR_BOARD_APP_PATH)/$(TARGET).mk

# Explicitly set the linker script
LINKER_SCRIPT=$(wildcard $(CY_AFR_BOARD_APP_PATH)/CORE_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.$(CY_AFR_TOOLCHAIN_LS_EXT))

# Use auto-discovery for cypress files and set the paths
CY_EXTAPP_PATH=$(CY_AFR_ROOT)/vendors/cypress
CY_AFR_BOARD_PATH=$(CY_EXTAPP_PATH)/boards/$(CY_AFR_TARGET)
CY_AFR_BOARD_APP_PATH=$(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/application_code/cy_code


################################################################################
# vendors/cypress
################################################################################

# Cypress-specific directories and files to ignore
CY_IGNORE+=\
    $(CY_EXTAPP_PATH)/boards\
    $(CY_EXTAPP_PATH)/WICED_SDK\
    $(CY_EXTAPP_PATH)/apps\
    $(CY_EXTAPP_PATH)/libraries\
    
SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/GeneratedSource/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.S)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.s)\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/pkcs11/*.c)\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/wifi/*.c)\

INCLUDES+=\
	$(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/application_code\
	$(CY_AFR_BOARD_APP_PATH)\
	$(CY_AFR_BOARD_APP_PATH)/GeneratedSource\
	$(CY_AFR_BOARD_APP_PATH)/startup\
	$(CY_AFR_BOARD_PATH)/ports/pkcs11\
	$(CY_AFR_BOARD_PATH)/ports/wifi


################################################################################
# freertos_kernel
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/freertos_kernel/*.c)\
	$(wildcard $(CY_AFR_ROOT)/freertos_kernel/portable/$(CY_AFR_TOOLCHAIN)/ARM_CM4F/*.c)\
	$(CY_AFR_ROOT)/freertos_kernel/portable/MemMang/heap_4.c

INCLUDES+=\
	$(CY_AFR_ROOT)/freertos_kernel\
	$(CY_AFR_ROOT)/freertos_kernel/include\
	$(CY_AFR_ROOT)/freertos_kernel/portable/$(CY_AFR_TOOLCHAIN)/ARM_CM4F


################################################################################
# libraries (3rd party)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/http_parser/http_parser.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/api/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/ipv4/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/core/ipv6/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/ppp/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/netif/ppp/polarssl/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/portable/arch/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/library/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/utils/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/tinycbor/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/unity/extras/fixture/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/3rdparty/unity/src/*c)

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/3rdparty/http_parser\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/compat/posix\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/compat/posix/arpa\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/compat/posix/net\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/compat/posix/sys\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/compat/stdc\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/lwip\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/lwip/apps\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/lwip/priv\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/lwip/prot\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/netif\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/netif/ppp\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/include/netif/ppp/polarssl\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/portable\
	$(CY_AFR_ROOT)/libraries/3rdparty/lwip/src/portable/arch\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/include\
	$(CY_AFR_ROOT)/libraries/3rdparty/mbedtls/include/mbedtls\
	$(CY_AFR_ROOT)/libraries/3rdparty/pkcs11\
	$(CY_AFR_ROOT)/libraries/3rdparty/tinycbor\
	$(CY_AFR_ROOT)/libraries/3rdparty/unity/extras/fixture/src\
	$(CY_AFR_ROOT)/libraries/3rdparty/unity/src


################################################################################
# libraries (abstractions)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/pkcs11/mbedtls/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/platform/freertos/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/secure_sockets/lwip/*c)

INCLUDES+=\
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
	$(CY_AFR_ROOT)/libraries/abstractions/wifi/include


################################################################################
# libraries (c_sdk)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/logging/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/common/taskpool/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/https/src/*c)\

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

################################################################################
# libraries (freertos_plus)
################################################################################

SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/src/*c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/*c)\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_2.c\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/NetworkInterface/board_family/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/$(CY_AFR_TOOLCHAIN)/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/pkcs11/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils/src/*.c)\

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_posix\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_posix/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/$(CY_AFR_TOOLCHAIN)\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/pkcs11\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/pkcs11/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/tls/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils\
	$(CY_AFR_ROOT)/libraries/freertos_plus/standard/utils/include

################################################################################
# libraries (Enterprise Security)
################################################################################
SOURCES+=\
    $(wildcard $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/*.c)\
    $(wildcard $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/abstraction/tls/*.c)\
     $(wildcard $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/abstraction/rtos/afr/cy_enterprise_security_interface.c)\
    $(wildcard $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/abstraction/rtos/afr/cy_rtos_abstraction.c)\
    $(wildcard $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/abstraction/rtos/afr/cy_wifi_abstraction.c)\
    $(wildcard $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/supplicant_core/*.c)\
    $(wildcard $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/supplicant_core/supplicant_host/*.c)

INCLUDES+=\
    $(CY_EXTAPP_PATH)/libraries/enterprise-security/src\
    $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/abstraction/tls\
    $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/abstraction/rtos/afr\
    $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/supplicant_core\
    $(CY_EXTAPP_PATH)/libraries/enterprise-security/src/supplicant_core/supplicant_host\
    $(CY_EXTAPP_PATH)/libraries/internal/utilities
