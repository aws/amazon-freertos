################################################################################
# \file CY8CKIT-064S0S2-4343W.mk
#
# \brief
# Define the CY8CKIT-064S0S2-4343W target.
#
################################################################################
# \copyright
# Copyright 2018-2020 Cypress Semiconductor Corporation
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

ifeq ($(WHICHFILE),true)
$(info Processing $(lastword $(MAKEFILE_LIST)))
endif

# MCU device selection
DEVICE:=CYB0644ABZI-S2D44
# Additional devices on the board
ADDITIONAL_DEVICES:=CYW4343WKUBG
# Default target core to CM4 if not already set
CORE?=CM4
# Define default type of bootloading method [single, dual]
# single -> CM4 only, dual -> CM0 and CM4
# multi is similar to dual, but cybootloader checks both images
SECURE_CORE_MODE=multi

ifeq ($(CORE),CM4)
# Additional components supported by the target
COMPONENTS+=PSOC6HAL
# Use CyHAL
DEFINES+=CY_USING_HAL

ifeq ($(SECURE_CORE_MODE),single)
CY_LINKERSCRIPT_SUFFIX=cm4
CY_SECURE_POLICY_NAME=policy_single_stage_CM4
else ifeq ($(SECURE_CORE_MODE),dual)
CY_LINKERSCRIPT_SUFFIX=cm4_dual
CY_SECURE_POLICY_NAME=policy_dual_stage_CM0p_CM4
else
CY_LINKERSCRIPT_SUFFIX=cm4_dual
CY_SECURE_POLICY_NAME=policy_multi_img_CM0p_CM4
endif

else
CY_SECURE_POLICY_NAME=policy_dual_stage_CM0p_CM4
endif

# Python path definition
ifeq ($(OS),Windows_NT)
CY_PYTHON_PATH?=python
else
CY_PYTHON_PATH?=python3
endif

ifneq ($(CY_TFM_ENABLED), true)
# BSP-specific post-build action
CY_BSP_POSTBUILD=$(CY_PYTHON_PATH) -c "import cysecuretools; \
	tools = cysecuretools.CySecureTools('cy8ckit-064s0s2-4343w', 'policy/$(CY_SECURE_POLICY_NAME).json'); \
	tools.sign_image('$(CY_CONFIG_DIR)/$(APPNAME).hex');"
else
# TFM enabled
TFM_PSOC64_PATH=$(CY_EXTAPP_PATH)/psoc6/psoc64tfm
TFM_PSOC64_SECURE_PATH=$(TFM_PSOC64_PATH)/security
TFM_POLICY_FILE=$(TFM_PSOC64_SECURE_PATH)/$(CY_SECURE_POLICY_NAME)_debug_2M.json
TFM_SIGN_SCRIPT=$(TFM_PSOC64_SECURE_PATH)/sign.py
TFM_DEVICE_NAME=cy8ckit-064b0s2-4343w
TFM_CM0_HEX=$(CY_CONFIG_DIR)/cm0.hex
TFM_CM4_ELF=$(CY_CONFIG_DIR)/cm4.elf
TFM_CM4_HEX=$(CY_CONFIG_DIR)/cm4.hex
TFM_CM0_SIGNED=$(CY_CONFIG_DIR)/cm0_signed.hex
TFM_CM4_SIGNED=$(CY_CONFIG_DIR)/cm4_signed.hex

ifeq ($(TOOLCHAIN),GCC_ARM)
CY_BSP_POSTBUILD=$(CY_CROSSPATH)/arm-none-eabi-objcopy -R .cy_sflash_user_data -R .cy_toc_part2 $(CY_CONFIG_DIR)/$(APPNAME).elf $(TFM_CM4_ELF);
CY_BSP_POSTBUILD+=$(CY_CROSSPATH)/arm-none-eabi-objcopy -O ihex $(TFM_CM4_ELF) $(TFM_CM4_HEX);
else ifeq ($(TOOLCHAIN),IAR)
CY_BSP_POSTBUILD=${CY_CROSSPATH}/ielftool --ihex $(CY_CONFIG_DIR)/$(APPNAME).elf $(TFM_CM4_HEX);
else ifeq ($(TOOLCHAIN),ARM)
CY_BSP_POSTBUILD=$(CY_CROSSPATH)/fromelf --i32 --output=$(TFM_CM4_HEX) $(CY_CONFIG_DIR)/$(APPNAME).elf;
endif

CY_BSP_POSTBUILD+=cp $(TFM_PSOC64_PATH)/COMPONENT_TFM_S_FW/tfm_s_unsigned.hex $(TFM_CM0_HEX);
CY_BSP_POSTBUILD+=$(CY_PYTHON_PATH) $(TFM_SIGN_SCRIPT) -s $(TFM_CM0_HEX) -n $(TFM_CM4_HEX) -p $(TFM_POLICY_FILE) -d $(TFM_DEVICE_NAME);

###########################################################################
#
# OTA Support
#
ifeq ($(OTA_SUPPORT),1)
    # OTA / MCUBoot defines
    #
    # IMPORTANT NOTE: These defines are also used in the building of MCUBOOT
    #                 they must EXACTLY match the values added to
    #                 mcuboot/boot/cypress/MCUBootApp/MCUBootApp.mk
    #
    # Must be a multiple of 1024 (must leave __vectors on a 1k boundary)
    MCUBOOT_HEADER_SIZE=0x400
    MCUBOOT_MAX_IMG_SECTORS=2000
    CY_BOOT_SCRATCH_SIZE=0x00010000
    # Boot loader size defines for mcuboot & app are different, but value is the same
    MCUBOOT_BOOTLOADER_SIZE=0x00040000
    CY_BOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE)
    # Primary Slot Currently follows Bootloader sequentially
    CY_BOOT_PRIMARY_1_START=0x10040000
    CY_BOOT_PRIMARY_1_SIZE=0x00120000
    CY_BOOT_SECONDARY_1_SIZE=0x00120000
    CY_BOOT_PRIMARY_2_SIZE=0x50000
    CY_BOOT_SECONDARY_1_START=0x00170000

    # Change to non-zero if stored in external FLASH
    CY_FLASH_ERASE_VALUE=0

    # MCUBoot location
    CY_COMMON_DIR=$(CY_EXTAPP_PATH)/common

    ifeq ($(MCUBOOT_DIR),)
    MCUBOOT_DIR=$(CY_COMMON_DIR)/mcuboot
    endif
    MCUBOOT_DIR_ABSOLUTE=$(abspath $(MCUBOOT_DIR))

    ifeq ($(OS),Windows_NT)
        CY_WHICH_CYGPATH:=$(shell which cygpath)
        #
        # CygWin/MSYS ?
        #
        ifneq ($(CY_WHICH_CYGPATH),)
            MCUBOOT_DIR:=$(shell cygpath -m --absolute $(subst \,/,$(MCUBOOT_DIR_ABSOLUTE)))
            CY_BUILD_LOCATION:=$(shell cygpath -m --absolute $(subst \,/,$(CY_BUILD_LOCATION)))
        else
            MCUBOOT_DIR:=$(subst \,/,$(MCUBOOT_DIR_ABSOLUTE))
        endif
    endif

    # signing scripts and keys from MCUBoot
    IMGTOOL_SCRIPT_NAME=./imgtool.py
    MCUBOOT_SCRIPT_FILE_DIR=$(CY_EXTAPP_PATH)/psoc6/psoc64tfm/security
    MCUBOOT_KEY_DIR=$(MCUBOOT_SCRIPT_FILE_DIR)/keys
    MCUBOOT_CYFLASH_PAL_DIR=$(CY_COMMON_DIR)/mcuboot/cy_flash_pal

    DEFINES+=OTA_SUPPORT=1 \
        MCUBOOT_HEADER_SIZE=$(MCUBOOT_HEADER_SIZE) \
        MCUBOOT_MAX_IMG_SECTORS=$(MCUBOOT_MAX_IMG_SECTORS) \
        CY_BOOT_SCRATCH_SIZE=$(CY_BOOT_SCRATCH_SIZE) \
        MCUBOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE) \
        CY_BOOT_BOOTLOADER_SIZE=$(CY_BOOT_BOOTLOADER_SIZE) \
        CY_BOOT_PRIMARY_1_START=$(CY_BOOT_PRIMARY_1_START) \
        CY_BOOT_PRIMARY_1_SIZE=$(CY_BOOT_PRIMARY_1_SIZE) \
        CY_BOOT_SECONDARY_1_SIZE=$(CY_BOOT_SECONDARY_1_SIZE) \
        CY_FLASH_ERASE_VALUE=$(CY_FLASH_ERASE_VALUE)\
        APP_VERSION_MAJOR=$(APP_VERSION_MAJOR)\
        APP_VERSION_MINOR=$(APP_VERSION_MINOR)\
        APP_VERSION_BUILD=$(APP_VERSION_BUILD)

    CY_OBJ_COPY=$(CY_CROSSPATH)/arm-none-eabi-objcopy

    # Custom post-build commands to run.
    MCUBOOT_KEY_FILE=$(MCUBOOT_KEY_DIR)/cypress-test-ec-p256.pem
    CY_OUTPUT_FILE_PATH=$(CY_BUILD_LOCATION)/$(CY_AFR_BUILD)/$(TARGET)/$(CONFIG)

    # For signing, use "sign", and key path
    IMGTOOL_COMMAND_ARG=sign
    CY_SIGNING_KEY_ARG="-k $(MCUBOOT_KEY_FILE)"
    SIGN_SCRIPT_FILE_PATH=$(CY_AFR_ROOT)/vendors/cypress/psoc6/psoc6make/make/scripts/sign_tar.bash

    CY_BUILD_VERSION=$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR).$(APP_VERSION_BUILD)

    CY_BSP_POSTBUILD+=$(SIGN_SCRIPT_FILE_PATH) $(CY_OUTPUT_FILE_PATH) $(CY_AFR_BUILD) $(CY_OBJ_COPY)\
        $(MCUBOOT_SCRIPT_FILE_DIR) $(IMGTOOL_SCRIPT_NAME) $(IMGTOOL_COMMAND_ARG) $(CY_FLASH_ERASE_VALUE) $(MCUBOOT_HEADER_SIZE)\
        $(MCUBOOT_MAX_IMG_SECTORS) $(CY_BUILD_VERSION) $(CY_BOOT_PRIMARY_1_START) $(CY_BOOT_PRIMARY_1_SIZE)\
        $(MCUBOOT_KEY_DIR) $(CY_SIGNING_KEY_ARG)
endif # OTA Support

# BSP programming flow
CY_BSP_PROGRAM=true
CY_OPENOCD_PROGRAM_ERASE= -s ${CY_OPENOCD_DIR}/scripts \
                          -f interface/kitprog3.cfg \
                          -f target/psoc6_2m_secure.cfg \
                          -c "init; reset init" \
                          -c "flash erase_address 0x101c0000 0x10000" \
                          -c "shutdown"

CY_OPENOCD_PROGRAM_TFM_S= -s $(CY_OPENOCD_DIR)/scripts \
                         -f interface/kitprog3.cfg \
                         -c "set ENABLE_ACQUIRE 0" \
                         -f target/psoc6_2m_secure.cfg \
                         -c "init; reset init; flash write_image erase $(TFM_CM0_SIGNED)" \
                         -c "resume; reset; exit"

CY_OPENOCD_PROGRAM_TFM_NS= -s $(CY_OPENOCD_DIR)/scripts \
                         -f interface/kitprog3.cfg \
                         -c "set ENABLE_ACQUIRE 0" \
                         -f target/psoc6_2m_secure.cfg \
                         -c "init; reset init; flash write_image erase $(TFM_CM4_SIGNED)" \
                         -c "resume; reset; exit"

program: build qprogram
	@echo;\
	echo "Programming PSoC64 ... ";\
	$(CY_OPENOCD_DIR)/bin/openocd $(CY_OPENOCD_PROGRAM_ERASE)
	$(CY_OPENOCD_DIR)/bin/openocd $(CY_OPENOCD_PROGRAM_TFM_S)
	$(CY_OPENOCD_DIR)/bin/openocd $(CY_OPENOCD_PROGRAM_TFM_NS)


endif
