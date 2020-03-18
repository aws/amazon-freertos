################################################################################
# \file Makefile
# \version 1.0
#
# \brief
# Top-level application make file.
#
################################################################################
# \copyright
# Copyright 2018-2019 Cypress Semiconductor Corporation
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

# Application directory name
CY_AFR_BUILD=ota

# Root location of AFR directory
CY_AFR_ROOT=../../../../..

# Build artifact location
CY_BUILD_RELATIVE_LOCATION=$(CY_AFR_ROOT)/build/cy
CY_BUILD_LOCATION=$(abspath $(CY_BUILD_RELATIVE_LOCATION))


################################################################################
# Basic Configuration
################################################################################

# Target board/hardware
TARGET=CY8CPROTO-062-4343W
# Underscore needed for directory
TARGET_UNDERSCORE=$(subst -,_,$(TARGET))

# Core processor
CORE?=CM4

# Name of application (used to derive name of final linked file).
APPNAME=$(CY_AFR_BUILD)

# Name of toolchain to use. Options include:
#
# GCC_ARM -- GCC 7.2.1, provided with ModusToolbox IDE
# ARM     -- ARM Compiler (must be installed separately)
# IAR     -- IAR Compiler (must be installed separately)
#
# See also: CY_COMPILER_PATH below
TOOLCHAIN=GCC_ARM

# Default build configuration. Options include:
#
# Debug   -- build with minimal optimizations, focus on debugging.
# Release -- build with full optimizations
CONFIG=Debug

# If set to "true" or "1", display full command-lines when building.
VERBOSE=

# Set to 1 to add OTA defines, sources, and libraries (must be used with MCUBoot)
# NOTE: Extra code must be called from your app to initialize AFR OTA Agent
OTA_SUPPORT=1

################################################################################
# Advanced Configuration
################################################################################

# Enable optional code that is ordinarily disabled by default.
#
# Available components depend on the specific targeted hardware and firmware
# in use. In general, if you have
#
#    COMPONENTS=foo bar
#
# ... then code in directories named COMPONENT_foo and COMPONENT_bar will be 
# added to the build
#
COMPONENTS=FREERTOS 4343W CY8CKIT_062_4343W

# Like COMPONENTS, but disable optional code that was enabled by default.
DISABLE_COMPONENTS=

# By default the build system automatically looks in the Makefile's directory
# tree for source code and builds it. The SOURCES variable can be used to
# manually add source code to the build process from a location not searched
# by default, or otherwise not found by the build system.
SOURCES=

# Like SOURCES, but for include directories. Value should be paths to
# directories (without a leading -I).
INCLUDES=

# Add additional defines to the build process (without a leading -D).
DEFINES=CYBSP_WIFI_CAPABLE CY_RTOS_AWARE CY_RETARGET_IO_CONVERT_LF_TO_CRLF CY_USE_LWIP

# Select softfp or hardfp floating point. Default is softfp. 
VFP_SELECT=hardfp

# Additional / custom C compiler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
CFLAGS=

# Additional / custom C++ compiler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
CXXFLAGS=

# Additional / custom assembler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
ASFLAGS=

# Additional / custom linker flags.
LDFLAGS=

# Additional / custom libraries to link in to the application.
LDLIBS=

# Custom pre-build commands to run.
PREBUILD=

# Custom post-build commands to run.
POSTBUILD=

#
# Set BLINK_FREQ_UPDATE_OTA as 0/1 to change LED blink frequency.
DEFINES+= BLINK_FREQ_UPDATE_OTA=0
# Check for default Version values
APP_VERSION_MAJOR?=1
APP_VERSION_MINOR?=0
APP_VERSION_BUILD?=0

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
    MCUBOOT_BOOTLOADER_SIZE=0x00012000
    CY_BOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE)
    # Primary Slot Currently follows Bootloader sequentially
    CY_BOOT_PRIMARY_1_START=0x00012000
    CY_BOOT_PRIMARY_1_SIZE=0x000EE000
    CY_BOOT_SECONDARY_1_SIZE=0x000EE000
    
    # Change to non-zero if stored in external FLASH
    CY_FLASH_ERASE_VALUE=0
    
    # MCUBoot location
    # This is used in afr.mk and sign_script.bash
    CY_COMMON_DIR=$(CY_AFR_ROOT)/vendors/cypress/common
    
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
    MCUBOOT_SCRIPT_FILE_DIR=$(CY_AFR_ROOT)/vendors/cypress/tools/scripts
    MCUBOOT_KEY_DIR=$(CY_COMMON_DIR)/mcuboot/keys
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
    	
    #NOTE this is still aws_demos for cy_code inclusion
    CY_EXTAPP_PATH=$(CY_AFR_ROOT)/vendors/cypress
    CY_AFR_BOARD_PATH=$(CY_EXTAPP_PATH)/boards/$(TARGET_UNDERSCORE)
    CY_AFR_BOARD_APP_PATH=$(CY_AFR_BOARD_PATH)/aws_demos/application_code/cy_code
    
    # Path to the linker script to use (if empty, use the default linker script).
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
    
    # Custom post-build commands to run.
    MCUBOOT_KEY_FILE=$(MCUBOOT_KEY_DIR)/cypress-test-ec-p256.pem
    CY_OUTPUT_FILE_PATH=$(CY_BUILD_LOCATION)/$(CY_AFR_BUILD)/$(TARGET)/$(CONFIG)

    # For signing, use "sign", and key path
	IMGTOOL_COMMAND_ARG=sign
	CY_SIGNING_KEY_ARG="-k $(MCUBOOT_KEY_FILE)"
    SIGN_SCRIPT_FILE_PATH=$(CY_AFR_ROOT)/vendors/cypress/psoc6/psoc6make/make/scripts/sign_script.bash
    # Use "Create" for PSoC 062 instead of "sign", and no key path (use a space " " for keypath to keep batch happy)
    # MCUBoot must also be modified to skip checking the signature
    #   Comment out and re-build MCUBootApp
    #   <mcuboot>/boot/cypress/MCUBootApp/config/mcuboot_config/mcuboot_config.h
    #   line 37, 38, 77
    # 37: //#define MCUBOOT_SIGN_EC256
    # 38: //#define NUM_ECC_BYTES (256 / 8)   // P-256 curve size in bytes, rnok: to make compilable
    # 77: //#define MCUBOOT_VALIDATE_PRIMARY_SLOT
    SECURE_TEST=$(shell echo $(TARGET) | grep "062")
	ifneq ($(SECURE_TEST),)
	    IMGTOOL_COMMAND_ARG=create
        CY_SIGNING_KEY_ARG=" "
    endif
    
    CY_BUILD_VERSION=$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR).$(APP_VERSION_BUILD)
    
    POSTBUILD=$(SIGN_SCRIPT_FILE_PATH) $(CY_OUTPUT_FILE_PATH) $(CY_AFR_BUILD)\
    	$(CY_ELF_TO_HEX) $(CY_ELF_TO_HEX_OPTIONS) $(CY_ELF_TO_HEX_FILE_ORDER)\
    	$(MCUBOOT_SCRIPT_FILE_DIR) $(IMGTOOL_SCRIPT_NAME) $(IMGTOOL_COMMAND_ARG) $(CY_FLASH_ERASE_VALUE) $(MCUBOOT_HEADER_SIZE)\
    	$(MCUBOOT_MAX_IMG_SECTORS) $(CY_BUILD_VERSION) $(CY_BOOT_PRIMARY_1_START) $(CY_BOOT_PRIMARY_1_SIZE)\
    	$(CY_SIGNING_KEY_ARG)

endif # OTA Support




################################################################################
# Paths
################################################################################

# Relative path to the project directory (default is the Makefile's directory).
#
# This controls where automatic source code discovery looks for code.
CY_APP_PATH=

# Relative path to the "base" library. It provides the core makefile build 
# infrastructure.
CY_BASELIB_PATH=$(CY_AFR_ROOT)/vendors/cypress/psoc6/psoc6make

# Absolute path to the compiler's "bin" directory. 
#
# The default depends on the selected TOOLCHAIN (GCC_ARM uses the ModusToolbox
# IDE provided compiler by default).
CY_COMPILER_PATH=

# Include aFR configuration make file 
include ./afr.mk


################################################################################
# Tools path
################################################################################

# Locate ModusToolbox IDE helper tools folders in default installation
# locations for Windows, Linux, and macOS.
CY_WIN_HOME=$(subst \,/,$(USERPROFILE))
CY_TOOLS_PATHS ?= $(wildcard \
    $(CY_WIN_HOME)/ModusToolbox/tools_* \
    $(HOME)/ModusToolbox/tools_* \
    /Applications/ModusToolbox/tools_*)

# If you install ModusToolbox IDE in a custom location, add the path to its
# "tools_X.Y" folder (where X and Y are the version number of the tools
# folder).
CY_TOOLS_PATHS+=

# Default to the newest installed tools folder, or the users override (if it's
# found).
CY_TOOLS_DIR=$(lastword $(sort $(wildcard $(CY_TOOLS_PATHS))))

ifeq ($(CY_TOOLS_DIR),)
$(error Unable to find any of the available CY_TOOLS_PATHS -- $(CY_TOOLS_PATHS))
endif

$(info Tools Directory: $(CY_TOOLS_DIR))
include $(CY_TOOLS_DIR)/make/start.mk

