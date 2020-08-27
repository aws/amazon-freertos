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
# Paths
################################################################################
CY_AFR_TARGET=$(subst -,_,$(TARGET))

CY_AFR_MAKE_SUPPORT_PATH=$(CY_AFR_ROOT)/projects/cypress/make_support
CY_DEVICESUPPORT_PATH=$(CY_AFR_ROOT)/vendors/cypress/MTB/psoc6/psoc6pdl

# Use auto-discovery for cypress files and set the paths
CY_AFR_VENDOR_PATH=$(CY_AFR_ROOT)/vendors/cypress
CY_EXTAPP_PATH=$(CY_AFR_VENDOR_PATH)/MTB
CY_AFR_BOARD_PATH=$(CY_AFR_VENDOR_PATH)/boards/$(CY_AFR_TARGET)
CY_AFR_BOARD_APP_PATH=$(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/application_code/cy_code

# Build Artifact locations for launch configs
CY_AFR_APPLOC=$(notdir $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST))))))
CY_SYM_FILE=\$$\{cy_prj_path\}/$(CY_BUILD_RELATIVE_LOCATION)/$(CY_AFR_APPLOC)/$(TARGET)/$(CONFIG)/$(APPNAME).elf
CY_PROG_FILE=\$$\{cy_prj_path\}/$(CY_BUILD_RELATIVE_LOCATION)/$(CY_AFR_APPLOC)/$(TARGET)/$(CONFIG)/$(APPNAME).hex

# Explicitly add the BSP makefile
CY_EXTRA_INCLUDES=$(CY_AFR_BOARD_APP_PATH)/$(TARGET).mk

################################################################################
# Global settings
################################################################################
include $(CY_AFR_MAKE_SUPPORT_PATH)/mtb_global_settings.mk

################################################################################
# Include source from vendors/cypress
################################################################################
include $(CY_AFR_MAKE_SUPPORT_PATH)/mtb_cypress_source.mk

################################################################################
# Include amazon-freertos source
################################################################################
include $(CY_AFR_MAKE_SUPPORT_PATH)/mtb_afr_source.mk

################################################################################
# Add secure signing specific steps.
################################################################################
ifeq ($(CY_TFM_ENABLED), true)
include $(CY_AFR_MAKE_SUPPORT_PATH)/mtb_secure_sign.mk
endif

################################################################################
# Additional Source files and includes needed for BLE support
################################################################################
ifeq ($(BLE_SUPPORT),1)
include $(CY_AFR_MAKE_SUPPORT_PATH)/mtb_feature_ble.mk
endif

################################################################################
# Additional Source files and includes needed for OTA support
################################################################################
ifeq ($(OTA_SUPPORT),1)
include $(CY_AFR_MAKE_SUPPORT_PATH)/mtb_feature_ota.mk
endif