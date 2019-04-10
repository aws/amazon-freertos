#
# Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # 
 # This software, associated documentation and materials ("Software")
 # is owned by Cypress Semiconductor Corporation,
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#

default: Help

export WICED_AMAZON_FREERTOS_SDK := 1

ifndef AMAZON_FREERTOS_PATH
export AMAZON_FREERTOS_PATH := $(CURDIR)/../../../../..
endif

ifndef WICED_SDK_PATH
export WICED_SDK_PATH := $(AMAZON_FREERTOS_PATH)/lib/third_party/mcu_vendor/cypress/WICED_SDK
endif

export AMAZON_FREERTOS_ENABLE_UNIT_TESTS := 1

export SOURCE_ROOT:=./
export MAKEFILES_PATH := $(SOURCE_ROOT)/makefiles
export TOOLS_ROOT:=$(WICED_STUDIO_SDK_PATH)/43xxx_Wi-Fi/tools
MAKEFILE_TARGETS := clean test testlist iar_project release

# Detect -iar style toolchain selection string
# This definition can be override by command line option IAR=1
export IAR := $(if $(findstring -iar, $(MAKECMDGOALS)),1)
export XIP_SUPPORT := $(if $(findstring -xip, $(MAKECMDGOALS)),1)

include $(MAKEFILES_PATH)/wiced_toolchain_common.mk

define USAGE_TEXT
Aborting due to invalid targets

Usage: make <target> [download] [run | debug] [JTAG=xxx] [no_dct]
       make run

  <target>
    One each of the following mandatory [and optional] components separated by '-'
      * Application (apps in sub-directories are referenced by subdir.appname)
      * Hardware Platform ($(filter-out common  include README.txt,$(notdir $(wildcard WICED/platforms/*))))
      * [RTOS] ($(notdir $(wildcard WICED/RTOS/*)))
      * [Network Stack] ($(notdir $(wildcard WICED/network/*)))
      * [MCU-WLAN Interface Bus] (SDIO SPI)

  [download]
    Download firmware image to target platform

  [run]
    Reset and run an application on the target hardware (no download)

  [debug]
    Connect to the target platform and run the debugger

  [JTAG=xxx]
    JTAG interface configuration file from the tools/OpenOCD dirctory
    Default option is CYW9WCD1EVAL1, direct JTAG option is jlink

  [no_dct]
    DCT downloading is disabled and the DCT remains unmodified.
    Only valid when the 'download' option is present

  [factory_reset_dct]
    Generates a factory reset DCT

  [VERBOSE=1]
    Shows the commands as they are being executed

  [JOBS=x]
    Sets the maximum number of parallel build threads (default=4)

  [IAR=1]
    Build with IAR toolchain
    Do NOT use together with [JOBS=x]. No parallel build support for IAR

  [ALWAYS=1]
    Always download all components (DCT, apps, etc) (default is to only download when changed)

  Notes
    * Component names are case sensitive
    * 'WICED', 'SDIO', 'SPI' and 'debug' are reserved component names
    * Component names MUST NOT include space or '-' characters
    * Building for release is assumed unless '-debug' is appended to the target
    * Some platforms may only support a single interface bus option

  Example Usage
    Build for Release
      $> make snip.scan-BCM943362WCD4
      $> make snip.scan-BCM943362WCD4-ThreadX-NetX_Duo-SDIO

    Build, Download and Run using the default USB-JTAG programming interface
      $> make snip.scan-BCM943362WCD4 download run

    Build for Debug
      $> make snip.scan-BCM943362WCD4-debug

    Build, Download and Debug using command line GDB
      $> make snip.scan-BCM943362WCD4-debug download debug

    Reset and run an application on the target hardware
      $> make run

    Clean output directory
      $> make clean

    Build a Factory Reset image for the SPI serial flash
      $> make snip.ping-BCM943362WCD4  OTA=waf.ota_upgrade  SFLASH=app-dct-ota-download

         where: [OTA]=<OTA application build string>
                         Build string options are App name, RTOS and TCP stack ONLY
                <SFLASH>=<app>-[dct]-[ota]-[download]
                         where:
                            app      : Add the application to the image
                            dct      : Add the DCT to the image
                            ota      : Add the OTA application to the image
                            download : Download the image after creation

	Working with OTA2 Support - This is an alternate to OTA support - do not mix them
	  First, build the extraction application that is part of the OTA2 system
	         This only needs to be built once after you "make clean" (unless you are modifying the OTA2 libraries)
	  $> make snip.ota2_extract-<platform>

	  Now you can build the Application
	  $> make snip.ota2_example-<platform> download download_apps run [ota2_image | ota2_download] [ota2_factory_image | ota2_factory_download]

		where:	ota2_image             : Build an OTA2 Upgrade Image, save in build/<application_dir>/OTA2_image_file.bin
										 This is an OTA2 Image file suitable for storage on an update server for downloading.
                ota2_download          : Build an OTA2 Upgrade Image + write to FLASH
										 This uses the sflash_write utility to write the OTA2_image_file.bin to the Staging Area
										 - This is used to test the OTA2 image file by putting the file in the area it would be during
										   an update sequence. In the example code, use the command line ">update_reboot" and reboot the
										   system to see the bootloader extract the image.
                ota2_factory_image     : Build an OTA2 Factory Reset Image
										 This is an OTA2 Image file suitable for storage in FLASH at manufacturing as the Factory Reset Image
										 - This is the OTA2 Image used when you hold the "factory reset" button during boot.
                ota2_factory_download  : Build an OTA2 Factory Reset Image + write to FLASH
										 - This is used to load the OTA2 Factory Reset Image file by putting the file in the area it would
										   be during manufacture. In the example code, hold the "factory reset" button during reboot and the
										   bootloader will extract the Image.

    Build an Over The Air 2 (OTA2) in-field upgrade for an older SDK (SDK-3.6.3 and up)
    	The Bootloader is never updated. Match parts of the DCT with the older SDK Bootloader

      $> make snip.ping-BCM943362WCD4 UPDATE_FROM_SDK=X_X_X <APP_USED_BT=1> <APP_USED_P2P=1> <APP_USED_OTA2=1>
      		Where X_X_X is the SDK you are creating an update application for
      			Valid SDKs: 3_0_1
                            3_1_0   3_1_1   3_1_2
                            3_3_0   3_3_1
                            3_4_0
                            3_5_1   3_5_2
                            3_6_0   3_6_1   3_6_2
                            3_7_0
            Optional additions - Must match original app compilation to match these DCT sections
            	APP_USED_BT=1		If Original Applciation used Bluetooth
            						 That is, this was added to <application>.mk file:
										GLOBAL_DEFINES += WICED_DCT_INCLUDE_BT_CONFIG
            	APP_USED_P2P=1		If Original Applciation used P2P
            						 That is, this was added to <application>.mk file:
										GLOBAL_DEFINES += WICED_DCT_INCLUDE_P2P_CONFIG
            	APP_USED_OTA2=1		If Original Applciation used OTA2
            						 That is, if the command line build included "ota2_xxx" as described for OTA2 builds above

			Define the Application Version for the OTA2 Image File by adding these to the build command
				APP_VERSION_FOR_OTA2_MAJOR=X
				APP_VERSION_FOR_OTA2_MINOR=Y

			For an initial product release:
				<application>-<platform> ota2_factory_download download download_apps run APP_VERSION_FOR_OTA2_MAJOR=1 APP_VERSION_FOR_OTA2_MINOR=0

			For an update OTA2 Image suitable for placing on an update server:
				<application>-<platform> ota2_image APP_VERSION_FOR_OTA2_MAJOR=1 APP_VERSION_FOR_OTA2_MINOR=2

            For a manufacturing image
                <application>-<platform> ota2_manuf_image APP_VERSION_FOR_OTA2_MAJOR=1 APP_VERSION_FOR_OTA2_MINOR=2

            Build manufacturing image and FLASH as one (testing)
                <application>-<platform> ota2_manuf_download APP_VERSION_FOR_OTA2_MAJOR=1 APP_VERSION_FOR_OTA2_MINOR=2
endef

############################
# Extra options:
#                LINT=1 : Sends source files for parsing by LINT
#                CHECK_HEADERS=1 : builds header files to test for their completeness
############################

OPENOCD_LOG_FILE ?= build/openocd_log.txt
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)

BOOTLOADER_LOG_FILE ?= build/bootloader.log
export HOST_OS
export VERBOSE
export SUB_BUILD
export OPENOCD_LOG_FILE
export EXTERNAL_WICED_GLOBAL_DEFINES

# We look for a command goal of ota2_image or ota2_download or ota2_factory_image or ota2_factory_download
# So that we include the ota_bootloader in *any* Application build
# and so we build to correct FLASH positioning
override OTA2_SUPPORT := $(if $(findstring ota2_, $(MAKECMDGOALS)),1)
export OTA2_SUPPORT
ifeq (1, $(OTA2_SUPPORT))
$(info MAKEFILE MAKECMDGOALS=$(MAKECMDGOALS) OTA2_SUPPORT is enabled)
else
$(info MAKEFILE MAKECMDGOALS=$(MAKECMDGOALS) OTA2_SUPPORT is disabled)
endif

ifeq (1, $(IAR))
$(info Building with IAR toolchain...)
export DOT_TOOLCHAIN_TYPE := .iar
export TOOLCHAIN_NAME := IAR
else
export TOOLCHAIN_NAME := GCC
export GCC_VERSION := 7.2.1
export GCC_VERSION_GREATER_THAN_5 := 1
endif


# This section is for setting DCT information for Upgrade between SDKs
# Starting with SDK-3.6.3 we will support an in-the-field upgade - also known as Over The Air (OTA or OTA2)
# upgrade - between SDKs. The Bootloader code is not updated, but the DCT layout has changed. These defines allow
# the new application to address the crucial part of the DCT so that the Bootloader and the Application are in
# agreement as to the layout of platform_dc_header_t. And the DCT can be updated to a new DCT layout.
BOOTLOADER_SDK:=
BOOTLOADER_SDK_VER:=
BOOTLOADER_SDK_CFLAGS:=
ORIGINAL_APP_SDK_USED_BT_CONFIG_D:=
ORIGINAL_APP_SDK_USED_P2P_CONFIG_D:=
ORIGINAL_APP_SDK_USED_OTA2_CONFIG_D:=
ifneq ($(UPDATE_FROM_SDK),)
BOOTLOADER_SDK_VER:=BOOTLOADER_SDK_$(UPDATE_FROM_SDK)
BOOTLOADER_SDK_VER_D:=-D$(BOOTLOADER_SDK_VER)
ifeq ($(APP_USED_BT),1)
ORIGINAL_APP_SDK_USED_BT_CONFIG:=ORIGINAL_APP_SDK_USED_BT_CONFIG
ORIGINAL_APP_SDK_USED_BT_CONFIG_D:= -DORIGINAL_APP_SDK_USED_BT_CONFIG
endif
ifeq ($(APP_USED_P2P),1)
ORIGINAL_APP_SDK_USED_P2P_CONFIG:=ORIGINAL_APP_SDK_USED_P2P_CONFIG
ORIGINAL_APP_SDK_USED_P2P_CONFIG_D := -DORIGINAL_APP_SDK_USED_P2P_CONFIG
endif
ifeq ($(APP_USED_OTA2),1)
ORIGINAL_APP_SDK_USED_OTA2_CONFIG:=ORIGINAL_APP_SDK_USED_OTA2_CONFIG
ORIGINAL_APP_SDK_USED_OTA2_CONFIG_D := -DORIGINAL_APP_SDK_USED_OTA2_CONFIG
endif
BOOTLOADER_SDK := $(BOOTLOADER_SDK_VER) $(ORIGINAL_APP_SDK_USED_BT_CONFIG) $(ORIGINAL_APP_SDK_USED_P2P_CONFIG) $(ORIGINAL_APP_SDK_USED_OTA2_CONFIG)
BOOTLOADER_SDK_CFLAGS:= $(BOOTLOADER_SDK_VER_D) $(ORIGINAL_APP_SDK_USED_BT_CONFIG_D) $(ORIGINAL_APP_SDK_USED_P2P_CONFIG_D) $(ORIGINAL_APP_SDK_USED_OTA2_CONFIG_D)
$(info Building an Upgrade Application using:: $(BOOTLOADER_SDK))
#always export it
export BOOTLOADER_SDK BOOTLOADER_SDK_CFLAGS UPDATE_FROM_SDK
endif      # $(UPDATE_FROM_SDK)
# End of SDK upgrade section

ALWAYS_DOWNLOAD_COMPONENTS :=
ifeq ($(ALWAYS),1)
ALWAYS_DOWNLOAD_COMPONENTS := 1
endif


.PHONY: $(BUILD_STRING) main_app bootloader download_only test testlist clean iar_project Help download no_dct download_dct download_work copy_elf_for_eclipse run debug download_bootloader sflash_image .gdbinit factory_reset_dct sflash

Help: $(TOOLCHAIN_HOOK_TARGETS)
	$(error $(USAGE_TEXT))

IAR_PROJECT_BUILDER = $(TOOLS_ROOT)/IAR/wiced_sdk_project/build_iar_project.exe

iar_project:
	$(QUIET)$(ECHO) Generating Wiced_SDK IAR project file...
	$(QUIET) $(RM) $(TOOLS_ROOT)/IAR/wiced_sdk_project/wiced_sdk_project.ewp
	$(QUIET) cd $(TOOLS_ROOT)/IAR/wiced_sdk_project
	$(QUIET) $(IAR_PROJECT_BUILDER) $(subst /,\,$(CURDIR))
	$(QUIET)$(ECHO) Generated. Now opening.... This may take several seconds
	$(QUIET)cmd /C start "$(IAR_WORKBENCH_EXECUTABLE)" "$(TOOLS_ROOT)/IAR/wiced_sdk_project/wiced_sdk_workspace.eww"



clean:
	$(QUIET)$(ECHO) Cleaning...
	$(QUIET)$(CLEAN_COMMAND)
	$(QUIET)$(RM) -rf .gdbinit ../../demos ../../lib ../../tests ./lwip ./jsmn ./mbedtls ./unity
	$(QUIET)$(ECHO) Done

ifneq (,$(filter test,$(MAKECMDGOALS)))
BUILD_STRING :=
test:
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/../release/wiced_test$(DOT_TOOLCHAIN_TYPE).mk $(filter-out test,$(MAKECMDGOALS))

endif

ifneq (,$(filter release,$(MAKECMDGOALS)))
BUILD_STRING :=
release:
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/../release/release$(DOT_TOOLCHAIN_TYPE).mk $(filter-out release,$(MAKECMDGOALS))

endif


# Processing of factory_reset_dct
ifneq (,$(findstring factory_reset_dct,$(MAKECMDGOALS)))
$(filter-out factory_reset_dct, $(MAKECMDGOALS)):
	@:
FACTORY_RESET_TARGET := $(BUILD_STRING)
BUILD_STRING :=
endif

ifneq ($(BUILD_STRING),)
-include build/$(CLEANED_BUILD_STRING)/config.mk
# Now we know the target architecture - include all toolchain makefiles and check one of them can handle the architecture
ifeq ($(IAR),1)
include $(MAKEFILES_PATH)/wiced_toolchain_IAR.mk
else
include $(MAKEFILES_PATH)/wiced_toolchain_ARM_GNU.mk
endif

build/$(CLEANED_BUILD_STRING)/config.mk: Makefile $(MAKEFILES_PATH)/wiced_config.mk $(MAKEFILES_PATH)/wiced_toolchain_common.mk $(MAKEFILES_PATH)/wiced_toolchain_ARM_GNU.mk $(WICED_SDK_MAKEFILES)
	$(QUIET)$(ECHO) $(if $(WICED_SDK_MAKEFILES),Applying changes made to: $?,Making config file for first time)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/wiced_config.mk $(CLEANED_BUILD_STRING)
endif

ifeq ($(IAR),1)
#IAR cannot support multi threaded build
JOBS=1
endif

JOBS ?=4
ifeq (,$(SUB_BUILD))
JOBSNO := $(if $(findstring 1,$(LINT)), , -j$(JOBS) )
endif

PASSDOWN_TARGETS := $(strip $(filter-out $(MAKEFILE_TARGETS) $(BUILD_STRING),$(MAKECMDGOALS)))

$(BUILD_STRING): main_app $(if $(SFLASH),sflash_image) copy_elf_for_eclipse  $(if $(SUB_BUILD),,.gdbinit)

$(PASSDOWN_TARGETS):
	@:

main_app: build/$(CLEANED_BUILD_STRING)/config.mk $(WICED_SDK_PRE_APP_BUILDS) $(MAKEFILES_PATH)/wiced_elf.mk
	$(QUIET)$(COMMON_TOOLS_PATH)mkdir -p $(OUTPUT_DIR)/binary $(OUTPUT_DIR)/modules $(OUTPUT_DIR)/libraries $(OUTPUT_DIR)/resources
	$(QUIET)$(MAKE) -r $(JOBSNO) $(SILENT) -f $(MAKEFILES_PATH)/wiced_elf.mk $(CLEANED_BUILD_STRING) $(PASSDOWN_TARGETS)
	$(QUIET)$(ECHO) Build complete

ifeq ($(SUB_BUILD),)
.gdbinit: build/$(CLEANED_BUILD_STRING)/config.mk $(MAKEFILES_PATH)/wiced_toolchain_common.mk main_app
	$(QUIET)$(ECHO) Making $@
	$(QUIET)$(ECHO) set remotetimeout 20 > $@
	$(QUIET)$(ECHO) $(GDBINIT_STRING) >> $@
ifneq ($(WAIT_FOR_GDBSERVER_STRING),)
	$(QUIET)$(ECHO) $(WAIT_FOR_GDBSERVER_STRING) >> $@
endif
endif

# The 'add-sym' command in .gdbinit_platform won't work here because it runs
# before Eclipse executes the 'symbol-file' or 'load' command
# What's more, executing the commands in this order causes GDB to lose the symbols for the
# file that you are debugging
# TODO: find a better way to do this - possibly via a function that is called from the Eclipse commands window
#	$(if $(wildcard .gdbinit_platform),$(QUIET)$(ECHO) source .gdbinit_platform >> $@)


ifneq ($(SFLASH),)
sflash_image: main_app
	$(QUIET)$(ECHO) Building Serial Flash Image
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/mfg_image.mk $(SFLASH) FRAPP=$(CLEANED_BUILD_STRING) SFLASH=
endif


sflash: main_app
	$(QUIET)$(ECHO) Building Serial Flash Image $@
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/wiced_sflash.mk FRAPP=$(CLEANED_BUILD_STRING) $@

sflash_download: main_app sflash
	$(QUIET)$(ECHO) Downloading Serial Flash Image $@
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/wiced_sflash.mk FRAPP=$(CLEANED_BUILD_STRING) $@

factory_reset_dct: $(MAKEFILES_PATH)/wiced_factory_reset.mk Makefile
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/wiced_factory_reset.mk $(FACTORY_RESET_TARGET)
