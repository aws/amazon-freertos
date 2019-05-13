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

.PHONY: bootloader download_bootloader download_XIP_bin no_dct download_dct download ota2_bootloader
.PHONY: package package_bootloader package_dct

ifneq ($(BOARD_REVISION),)
PLATFORM_BUS := $(PLATFORM)-$(BOARD_REVISION)-$(BUS)
else
PLATFORM_BUS := $(PLATFORM)-$(BUS)
endif

# use the ota2 bootloader
ifeq (1, $(OTA2_SUPPORT))
BOOTLOADER_TARGET := waf.ota2_bootloader-NoOS-NoNS-$(PLATFORM_BUS)
OTA2_FAILSAFE_TARGET := waf.ota2_failsafe-$(PLATFORM_BUS)
OTA2_FAILSAFE_TARGET_FILE:= $(BUILD_DIR)/$(OTA2_FAILSAFE_TARGET)/binary/$(OTA2_FAILSAFE_TARGET).stripped.elf
else
#NO_TINY_BOOTLOADER_REQUIRED:=1	#temp
BOOTLOADER_TARGET := waf.bootloader-NoOS-NoNS-$(PLATFORM_BUS)
endif # OTA2_SUPPORT
BOOTLOADER_TARGET_FILE := $(BUILD_DIR)/$(BOOTLOADER_TARGET)/binary/$(BOOTLOADER_TARGET)
BOOTLOADER_LINK_FILE := $(BUILD_DIR)/$(BOOTLOADER_TARGET)/binary/$(BOOTLOADER_TARGET)$(LINK_OUTPUT_SUFFIX)
BOOTLOADER_OUT_FILE  := $(BOOTLOADER_LINK_FILE:$(LINK_OUTPUT_SUFFIX)=$(FINAL_OUTPUT_SUFFIX))
BOOTLOADER_TRX_FILE  := $(BOOTLOADER_LINK_FILE:$(LINK_OUTPUT_SUFFIX)=.trx$(FINAL_OUTPUT_SUFFIX))
BOOTLOADER_LOG_FILE  ?= $(BUILD_DIR)/bootloader.log
SFLASH_LOG_FILE      ?= $(BUILD_DIR)/sflash_writer.log
GENERATED_MAC_FILE   := $(SOURCE_ROOT)generated_mac_address.txt
TRX_CREATOR          := $(SOURCE_ROOT)WICED/platform/MCU/BCM4390x/make_trx.pl
MAC_GENERATOR        := $(TOOLS_ROOT)/mac_generator/mac_generator.pl
AES128_CBC_ENCRYPTOR := $(COMMON_TOOLS_PATH)aes_cbc_128$(TOOLCHAIN_SUFFIX)
HMAC_SHA256_SIGNER   := $(COMMON_TOOLS_PATH)hmac_sha256$(TOOLCHAIN_SUFFIX)
RSA_SIGNER           := $(SOURCE_ROOT)tools/secureboot/rsa/rsa_pkcs1_sign.pl
SECURE_TRX_CREATOR   := $(SOURCE_ROOT)WICED/platform/MCU/BCM4390x/make_secure_trx.pl
KEYSDIR              := $(SOURCE_ROOT)WICED/platform/MCU/BCM4390x/keys
SECTOR_NUMBER_SCRIPT := $(TOOLS_ROOT)/text_to_c/sector_number.pl
SECTOR_COUNT_SCRIPT  := $(TOOLS_ROOT)/text_to_c/sector_count.pl
FILE_SIZE_OVERFLOW_SCRIPT := $(TOOLS_ROOT)/text_to_c/file_size_overflow.pl
ABS_DIFFERENCE_SCRIPT := $(TOOLS_ROOT)/text_to_c/abs_difference.pl

# Key Location
KEYS ?= NULL
ENCRYPTOR            := $(AES128_CBC_ENCRYPTOR)
ENCRYPTION_KEY       := $(KEYSDIR)/$(KEYS)/boot_aes.key

ifeq (RSA, $(SECURE_BOOT_AUTH))

SIGNER               := $(RSA_SIGNER)
SIGNING_KEY          := $(KEYSDIR)/$(KEYS)/rsa_key
else
SIGNER               := $(HMAC_SHA256_SIGNER)
SIGNING_KEY          := $(KEYSDIR)/$(KEYS)/boot_sha.key
endif

ifeq (1, $(SECURE_BOOT))
BOOTLOADER_FINAL_TRX_FILE := $(BOOTLOADER_TARGET_FILE).sig.enc.trx$(FINAL_OUTPUT_SUFFIX)
else
BOOTLOADER_FINAL_TRX_FILE := $(BOOTLOADER_TRX_FILE)
endif

ifeq (1, $(SECURE_SFLASH))
SFLASH_ENCRYPTION_KEY := $(KEYSDIR)/$(KEYS)/boot_aes.key
SFLASH_SIGNING_KEY    := $(KEYSDIR)/$(KEYS)/boot_sha.key
BLOCK_SIZE            := 4096
PAD_FILE_IF_NEEDED   := 0
DO_NOT_PAD_FILE      := 1
endif


TINY_BOOTLOADER_TARGET := waf.tiny_bootloader-NoOS-NoNS-$(PLATFORM_BUS)
TINY_BOOTLOADER_LOG_FILE ?= $(BUILD_DIR)/tiny_bootloader.log
TINY_BOOTLOADER_BIN_FILE := $(BUILD_DIR)/$(TINY_BOOTLOADER_TARGET)/binary/$(TINY_BOOTLOADER_TARGET).bin
TINY_BOOTLOADER_BIN2C_FILE := $(OUTPUT_DIR)/tiny_bootloader_bin2c.c
TINY_BOOTLOADER_BIN2C_OBJ  := $(OUTPUT_DIR)/tiny_bootloader_bin2c.o
TINY_BOOTLOADER_BIN2C_ARRAY_NAME := tinybl_bin

SFLASH_APP_TARGET := waf.sflash_write-NoOS-$(PLATFORM_BUS)
SFLASH_APP_OUTFILE := $(BUILD_DIR)/$(SFLASH_APP_TARGET)/binary/$(SFLASH_APP_TARGET)


ifneq ($(filter download, $(MAKECMDGOALS)),)
ifeq ($(filter download_apps, $(MAKECMDGOALS)),)
#Overiding user options for any thing other than the main images
FR_APP  	:=
DCT_IMAGE 	:=
OTA_APP 	:=
WIFI_FIRMWARE :=
APP1		:=
APP2		:=
endif
endif

SFLASH_APP_BCM4390 := 43909
SFLASH_APP_PLATFROM_BUS := $(PLATFORM_BUS)

# this must be zero as defined in the ROM bootloader
SFLASH_BOOTLOADER_LOC := 0x00000000
PACKAGE_DEVTARGET := "4390x"

ifeq ($(JLINK_NATIVE), 1)
JLINK_SFLASH_BASE_LOC := 0x14000000
JLINK_DEVICE	:= CYW43907
JLINK_INTERFACE	:= JTAG
JLINK_SPEED		:= 4000
JLINK_JTAG_CONF	:= -jtagconf -1,-1
JLINK_AUTO_CONNECT	:= 1
JLINK_MAKE_SCRIPT_EXTEND := $(ECHO) w4 0x18020634 0 >> $(JLINK_CMD_SCRIPT)$(LINE_SYMBOL)
JLINK_MAKE_SCRIPT_GO := $(ECHO) go >> $(JLINK_CMD_SCRIPT)$(LINE_SYMBOL)
endif

ifeq (1, $(OTA2_SUPPORT))
include platforms/$(subst .,/,$(PLATFORM))/ota2_image_defines.mk
SFLASH_DCT_LOC:= $(OTA2_IMAGE_CURR_DCT_1_AREA_BASE)
SFLASH_DCT2_LOC:= $(OTA2_IMAGE_CURR_DCT_2_AREA_BASE)
SFLASH_FS_LOC := $(OTA2_IMAGE_CURR_FS_AREA_BASE)
# Current bootloader address for OTA2 image starts at 0x00000000, ends at OTA2_IMAGE_FACTORY_RESET_AREA_BASE
ifeq (1, $(SECURE_BOOT))
# SECURE_BOOT only supports bootloader images that are < 16k after signature and encryption
MAX_SFLASH_BOOTLOADER_SPACE:= 0x0004000
else
MAX_SFLASH_BOOTLOADER_SPACE:= $(OTA2_IMAGE_FACTORY_RESET_AREA_BASE)
endif # SECURE_BOOT
else
include $(SOURCE_ROOT)platforms/$(subst .,/,$(PLATFORM))/normal_image_defines.mk
SFLASH_DCT_LOC:= $(NORMAL_IMAGE_DCT_1_AREA_BASE)
SFLASH_DCT2_LOC:= $(NORMAL_IMAGE_DCT_2_AREA_BASE)
SFLASH_FS_LOC := $(NORMAL_IMAGE_FS_AREA_BASE)
# Current bootloader address for normal image starts at 0x00000000, ends at NORMAL_IMAGE_DCT_1_AREA_BASE
ifeq (1, $(SECURE_BOOT))
# SECURE_BOOT only supports bootloader images that are < 16k after signature and encryption
MAX_SFLASH_BOOTLOADER_SPACE:= 0x0004000
else
MAX_SFLASH_BOOTLOADER_SPACE:= $(NORMAL_IMAGE_DCT_1_AREA_BASE)
endif # SECURE_BOOT
endif # OTA2_SUPPORT

SFLASH_DCT_START_SECTOR:= $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(SFLASH_DCT_LOC) 4096)
SFLASH_DCT2_START_SECTOR:= $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(SFLASH_DCT2_LOC) 4096)

OPENOCD_LOG_FILE ?= build/openocd_log.txt
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)

ifneq ($(VERBOSE),1)
BOOTLOADER_REDIRECT	= > $(BOOTLOADER_LOG_FILE)
TINY_BOOTLOADER_REDIRECT = > $(TINY_BOOTLOADER_LOG_FILE)
SFLASH_REDIRECT	= > $(SFLASH_LOG_FILE)
endif


ifeq (,$(and $(OPENOCD_PATH),$(OPENOCD_FULL_NAME)))
	$(error Path to OpenOCD has not been set using OPENOCD_PATH and OPENOCD_FULL_NAME)
endif

# Include packaging rules
PACKAGE_OUTPUT_DIR := $(OUTPUT_DIR)
-include $(MAKEFILES_PATH)/wiced_package.mk

#APPS LOOK UP TABLE PARAMS
APPS_LUT_DOWNLOAD_DEP :=

NO_BOOTLOADER_REQUIRED:=0

# If the current build string is building the bootloader, don't recurse to build another bootloader
ifneq (,$(findstring waf.bootloader, $(BUILD_STRING)))
NO_BOOTLOADER_REQUIRED:=1
endif

ifneq (,$(findstring waf/ota2_bootloader, $(BUILD_STRING)))
NO_BOOTLOADER_REQUIRED:=1
endif

# Bootloader is not needed when debugger downloads to RAM
ifneq (download,$(findstring download,$(MAKECMDGOALS)))
# Hmmm. flags by exclusion are never good
ifneq (package,$(findstring package,$(MAKECMDGOALS)))
NO_BOOTLOADER_REQUIRED:=1
endif
endif


#if building an upgrade, don't build the bootloader
ifneq (,$(BOOTLOADER_SDK))
NO_BOOTLOADER_REQUIRED:=1
NO_TINY_BOOTLOADER_REQUIRED:=1
endif

# Bootloader is needed for manufacturing image
ifeq (ota2_manuf_image,$(findstring ota2_manuf_image,$(MAKECMDGOALS)))
NO_BUILD_BOOTLOADER := 0
NO_BOOTLOADER_REQUIRED:=0
endif

# Do not include $(TINY_BOOTLOADER_BIN2C_OBJ) if building bootloader/tiny_bootloader/sflash_write
ifneq (,$(findstring waf.bootloader, $(BUILD_STRING))$(findstring waf.tiny_bootloader, $(BUILD_STRING))$(findstring waf.sflash_write, $(BUILD_STRING)))
NO_TINY_BOOTLOADER_REQUIRED:=1
endif

# Bootloader is not needed when asked from command line
ifeq (no_tinybl,$(findstring no_tinybl,$(MAKECMDGOALS)))
NO_TINY_BOOTLOADER_REQUIRED:=1
endif
no_tinybl:
	@

# Tiny Bootloader Targets
ifneq (1,$(NO_TINY_BOOTLOADER_REQUIRED))
LINK_LIBS += $(TINY_BOOTLOADER_BIN2C_OBJ)
$(TINY_BOOTLOADER_BIN2C_OBJ): $(GENERATED_MAC_FILE)
	$(ECHO) Building Tiny Bootloader $(TINY_BOOTLOADER_TARGET)
	$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(TINY_BOOTLOADER_TARGET) -I$(OUTPUT_DIR) SUB_BUILD=tiny_bootloader $(TINY_BOOTLOADER_REDIRECT)
	$(BIN2C) $(TINY_BOOTLOADER_BIN_FILE) $(TINY_BOOTLOADER_BIN2C_FILE) $(TINY_BOOTLOADER_BIN2C_ARRAY_NAME)
	$(CC) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(TINY_BOOTLOADER_BIN2C_FILE) $(WICED_SDK_DEFINES) $(WICED_SDK_INCLUDES) $(COMPILER_SPECIFIC_DEBUG_CFLAGS)  $(COMPILER_SPECIFIC_STANDARD_CFLAGS) -o $(TINY_BOOTLOADER_BIN2C_OBJ)
	$(ECHO) Finished Building Tiny Bootloader
endif

# Bootloader Targets
ifeq (1,$(NO_BOOTLOADER_REQUIRED))
bootloader:
	@:

download_bootloader:
	@:

copy_bootloader_output_for_eclipse:
	@:

else
ifeq (1,$(NO_BUILD_BOOTLOADER))
bootloader:
	$(QUIET)$(ECHO) Skipping building bootloader due to commandline spec

download_bootloader:
	@:

copy_bootloader_output_for_eclipse:
	@:

else
APPS_LUT_DOWNLOAD_DEP += download_bootloader
APPS_LUT_PACKAGE_DEP += package_bootloader
bootloader:
	$(QUIET)$(ECHO) Building Bootloader $(BOOTLOADER_TARGET)
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(BOOTLOADER_TARGET) -I$(OUTPUT_DIR)  SFLASH= EXTERNAL_WICED_GLOBAL_DEFINES=$(EXTERNAL_WICED_GLOBAL_DEFINES) SUB_BUILD=bootloader $(BOOTLOADER_REDIRECT)
	$(QUIET)$(PERL) $(TRX_CREATOR) $(BOOTLOADER_LINK_FILE) $(BOOTLOADER_TRX_FILE)
ifeq (1, $(SECURE_BOOT))
	$(QUIET)$(ECHO) Signing bootloader...
	$(QUIET) $(SIGNER) sign $(SIGNING_KEY) $(BOOTLOADER_OUT_FILE) $(BOOTLOADER_TARGET_FILE).sig$(FINAL_OUTPUT_SUFFIX)
	$(QUIET)$(ECHO) Encrypting bootloader...
	$(QUIET) $(ENCRYPTOR) enc $(ENCRYPTION_KEY) $(BOOTLOADER_TARGET_FILE).sig$(FINAL_OUTPUT_SUFFIX) $(BOOTLOADER_TARGET_FILE).sig.enc$(FINAL_OUTPUT_SUFFIX)
	$(QUIET)$(ECHO) Generate Encrypted/Signed TRX
	$(QUIET) $(SECURE_TRX_CREATOR) $(BOOTLOADER_TRX_FILE) $(BOOTLOADER_TARGET_FILE).sig.enc$(FINAL_OUTPUT_SUFFIX) $(BOOTLOADER_FINAL_TRX_FILE)
endif
	$(QUIET)$(ECHO) Finished Building Bootloader
	$(QUIET)$(ECHO_BLANK_LINE)

ifeq (1,$(XIP_SUPPORT))
include $(SOURCE_ROOT)platforms/$(PLATFORM)/platform_xip.mk
APPS_LUT_DOWNLOAD_DEP += download_XIP_bin

ifndef XIP_LOAD_ADDRESS
$(error XIP download address is not defined!)
endif
download_XIP_bin: $(XIP_OUTPUT_FILE) display_map_summary download_dct download_bootloader
	$(QUIET)$(ECHO) Downloading XIP bin download to $(XIP_LOAD_ADDRESS)...
ifeq ($(JLINK_NATIVE), 1)
	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL,$(XIP_OUTPUT_FILE),$(XIP_LOAD_ADDRESS), $(JLINK_MAKE_SCRIPT_EXTEND))
	$(QUIET)$(call JLINK_EXE_SCRIPT)
else
	$(QUIET)$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(SOURCE_ROOT)apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(XIP_OUTPUT_FILE) $(XIP_LOAD_ADDRESS) $(PLATFORM_BUS) 0 43909" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif
	$(QUIET)$(ECHO) XIP application download done
endif

# Check bootloader size and space before downloading image
download_bootloader: bootloader display_map_summary download_dct
	$(QUIET)$(ECHO) Downloading Bootloader ...
	$(QUIET)$(eval BOOTLOADER_IMAGE_SIZE_OVERFLOW := $(shell $(PERL) $(FILE_SIZE_OVERFLOW_SCRIPT) $(BOOTLOADER_FINAL_TRX_FILE) $(MAX_SFLASH_BOOTLOADER_SPACE)))
	$(QUIET)$(eval $(if $(BOOTLOADER_IMAGE_SIZE_OVERFLOW), $(error Bootloader $(BOOTLOADER_FINAL_TRX_FILE) size $(BOOTLOADER_IMAGE_SIZE_OVERFLOW) overflowed max_size $(MAX_SFLASH_BOOTLOADER_SPACE) !!!)))
ifeq ($(JLINK_NATIVE), 1)
	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL,$(BOOTLOADER_FINAL_TRX_FILE),$(SFLASH_BOOTLOADER_LOC), $(JLINK_MAKE_SCRIPT_EXTEND))
	$(QUIET)$(call JLINK_EXE_SCRIPT)
else
	$(QUIET)$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(SOURCE_ROOT)apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(BOOTLOADER_FINAL_TRX_FILE) $(SFLASH_BOOTLOADER_LOC) $(PLATFORM_BUS) 1 43909" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif
	$(QUIET)$(ECHO) Finished Downloading Bootloader

package_bootloader: bootloader display_map_summary package_dct
	$(call ADD_TO_PACKAGE, $(BOOTLOADER_FINAL_TRX_FILE), sflash, $(SFLASH_BOOTLOADER_LOC))

ifeq (1,$(OTA2_SUPPORT))
APPS_LUT_DOWNLOAD_DEP += ota2_failsafe_download
ota2_failsafe: bootloader
	$(QUIET)$(ECHO) Building OTA2 Failsafe
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(OTA2_FAILSAFE_TARGET) -I$(OUTPUT_DIR)  SFLASH= EXTERNAL_WICED_GLOBAL_DEFINES=$(EXTERNAL_WICED_GLOBAL_DEFINES) SUB_BUILD=bootloader $(BOOTLOADER_REDIRECT)
	$(QUIET)$(ECHO) Finished Building OTA2 Failsafe
	$(QUIET)$(ECHO_BLANK_LINE)
ifeq (1, $(FAILSAFE_APP_SECURE))
	$(QUIET)$(ECHO) Signing OTA_FAILSAFE_APP $(OTA2_FAILSAFE_TARGET_FILE)
	$(HMAC_SHA256_SIGNER) sign $(SFLASH_SIGNING_KEY) $(OTA2_FAILSAFE_TARGET_FILE) $(OTA2_FAILSAFE_TARGET_FILE).sig $(BLOCK_SIZE) $(PAD_FILE_IF_NEEDED)
	$(QUIET)$(ECHO) Encrypting OTA_FAILSAFE_APP $(OTA2_FAILSAFE_TARGET_FILE).sig , OTA2_FAILSAFE_SECTOR_START is $(OTA2_FAILSAFE_SECTOR_START)
	$(call CONV_SLASHES, $(AES128_CBC_ENCRYPTOR) enc $(SFLASH_ENCRYPTION_KEY) $(OTA2_FAILSAFE_TARGET_FILE).sig $(OTA2_FAILSAFE_TARGET_FILE).sig.enc $(BLOCK_SIZE) $(OTA2_FAILSAFE_SECTOR_START))
endif

ota2_failsafe_download: ota2_failsafe download_bootloader $(if $(filter 1,$(XIP_SUPPORT)),download_XIP_bin)
ifeq (1, $(FAILSAFE_APP_SECURE))
	$(QUIET)$(ECHO) Downloading OTA2 Secure Failsafe App ...

	$(QUIET)$(eval MAX_OTA2_FAILSAFE_IMAGE_SPACE := $(shell $(PERL) $(ABS_DIFFERENCE_SCRIPT) $(OTA2_IMAGE_DO_NOT_WRITE_AREA_END) $(OTA2_IMAGE_FAILSAFE_APP_AREA_BASE) ))
	$(QUIET)$(eval OTA2_FAILSAFE_IMAGE_SIZE_OVERFLOW := $(shell $(PERL) $(FILE_SIZE_OVERFLOW_SCRIPT) $(OTA2_FAILSAFE_TARGET_FILE).sig.enc $(MAX_OTA2_FAILSAFE_IMAGE_SPACE) ))
	$(QUIET)$(eval $(if $(OTA2_FAILSAFE_IMAGE_SIZE_OVERFLOW), $(error OTA2 Failsafe $(OTA2_FAILSAFE_TARGET_FILE).sig.enc size $(OTA2_FAILSAFE_IMAGE_SIZE_OVERFLOW) overflowed max_size $(MAX_OTA2_FAILSAFE_IMAGE_SPACE) !!!)))

ifeq ($(JLINK_NATIVE), 1)
	$(QUIET)$(shell cp $(OTA2_FAILSAFE_TARGET_FILE).sig.enc $(OTA2_FAILSAFE_TARGET_FILE).sig.enc.bin)
	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL,$(OTA2_FAILSAFE_TARGET_FILE).sig.enc.bin,$(OTA2_FAILSAFE_APP_ADDRESS), $(JLINK_MAKE_SCRIPT_EXTEND))
	$(QUIET)$(call JLINK_EXE_SCRIPT)
else
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OTA2_FAILSAFE_TARGET_FILE).sig.enc $(OTA2_FAILSAFE_APP_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif
	$(QUIET)$(ECHO) Finished Downloading OTA2 Failsafe App
else
	$(QUIET)$(ECHO) Downloading OTA2 Failsafe App ...

	$(QUIET)$(eval MAX_OTA2_FAILSAFE_IMAGE_SPACE := $(shell $(PERL) $(ABS_DIFFERENCE_SCRIPT) $(OTA2_IMAGE_DO_NOT_WRITE_AREA_END) $(OTA2_IMAGE_FAILSAFE_APP_AREA_BASE) ))
	$(QUIET)$(eval OTA2_FAILSAFE_IMAGE_SIZE_OVERFLOW := $(shell $(PERL) $(FILE_SIZE_OVERFLOW_SCRIPT) $(OTA2_FAILSAFE_TARGET_FILE) $(MAX_OTA2_FAILSAFE_IMAGE_SPACE) ))
	$(QUIET)$(eval $(if $(OTA2_FAILSAFE_IMAGE_SIZE_OVERFLOW), $(error OTA2 Failsafe $(OTA2_FAILSAFE_TARGET_FILE) size $(OTA2_FAILSAFE_IMAGE_SIZE_OVERFLOW) overflowed max_size $(MAX_OTA2_FAILSAFE_IMAGE_SPACE) !!!)))

ifeq ($(JLINK_NATIVE), 1)
	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL,$(OTA2_FAILSAFE_TARGET_FILE),$(OTA2_FAILSAFE_APP_ADDRESS), $(JLINK_MAKE_SCRIPT_EXTEND))
	$(QUIET)$(call JLINK_EXE_SCRIPT)
else
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(SOURCE_ROOT)apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OTA2_FAILSAFE_TARGET_FILE) $(OTA2_FAILSAFE_APP_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif
	$(QUIET)$(ECHO) Finished Downloading OTA2 Failsafe App
endif
endif

copy_bootloader_output_for_eclipse: build_done $(BUILD_DIR)/eclipse_debug
	$(QUIET)$(CP) $(BOOTLOADER_LINK_FILE) $(BUILD_DIR)/eclipse_debug/last_bootloader.elf

endif
endif

$(BUILD_DIR)/eclipse_debug:
	$(QUIET)$(call MKDIR, $(BUILD_DIR)/eclipse_debug/)

ifeq (1, $(DCT_IMAGE_SECURE))
DCT_IMAGE_PLATFORM    := $(FINAL_DCT_FILE).secure
DCT2_IMAGE_PLATFORM   := $(DCT_IMAGE_PLATFORM)_2
$(DCT_IMAGE_PLATFORM): $(FINAL_DCT_FILE)
	$(QUIET)$(ECHO) Padding DCT
	$(QUIET)$(CP) $(FINAL_DCT_FILE) $(FINAL_DCT_FILE).pad
	$(QUIET)$(PERL) $(TOOLS_ROOT)/create_dct/pad_dct.pl $(FINAL_DCT_FILE).pad
	$(QUIET)$(ECHO) Signing and Encrypting DCT, SFLASH_DCT2_START_SECTOR=$(SFLASH_DCT2_START_SECTOR) SFLASH_DCT_START_SECTOR=$(SFLASH_DCT_START_SECTOR)
	$(HMAC_SHA256_SIGNER) sign $(SFLASH_SIGNING_KEY) $(FINAL_DCT_FILE).pad $(FINAL_DCT_FILE).sige $(BLOCK_SIZE) $(DO_NOT_PAD_FILE)
	$(AES128_CBC_ENCRYPTOR) enc $(SFLASH_ENCRYPTION_KEY) $(FINAL_DCT_FILE).sige $(FINAL_DCT_FILE).secure_2 $(BLOCK_SIZE) $(SFLASH_DCT2_START_SECTOR)
	$(AES128_CBC_ENCRYPTOR) enc $(SFLASH_ENCRYPTION_KEY) $(FINAL_DCT_FILE).sige $(FINAL_DCT_FILE).secure $(BLOCK_SIZE) $(SFLASH_DCT_START_SECTOR)
else
DCT_IMAGE_PLATFORM             := $(FINAL_DCT_FILE)
endif

# DCT Targets
ifneq (no_dct,$(findstring no_dct,$(MAKECMDGOALS)))
APPS_LUT_DOWNLOAD_DEP += download_dct
download_dct: $(DCT_IMAGE_PLATFORM) display_map_summary sflash_write_app
	$(QUIET)$(ECHO) Downloading DCT ... $(DCT_IMAGE_PLATFORM) @ SFLASH_DCT_LOC=$(SFLASH_DCT_LOC)
ifeq ($(JLINK_NATIVE), 1)
	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL,$(DCT_IMAGE_PLATFORM),$(SFLASH_DCT_LOC), $(JLINK_MAKE_SCRIPT_EXTEND))
	$(QUIET)$(call JLINK_EXE_SCRIPT)
else
	$(QUIET)$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(SOURCE_ROOT)apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(DCT_IMAGE_PLATFORM) $(SFLASH_DCT_LOC) $(PLATFORM_BUS) 0 43909" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

package_dct: $(DCT_IMAGE_PLATFORM) display_map_summary
	$(QUIET)$(ECHO) Packaging DCT ... $(DCT_IMAGE_PLATFORM) @ SFLASH_DCT_LOC=$(SFLASH_DCT_LOC)
	$(call ADD_TO_PACKAGE, $(DCT_IMAGE_PLATFORM), sflash, $(SFLASH_DCT_LOC))
ifeq (1, $(DCT_IMAGE_SECURE))
	$(QUIET)$(ECHO) Downloading DCT2 ... $(DCT2_IMAGE_PLATFORM) @ SFLASH_DCT_LOC=$(SFLASH_DCT2_LOC)
ifeq ($(JLINK_NATIVE), 1)
	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL,$(DCT2_IMAGE_PLATFORM),$(SFLASH_DCT2_LOC), $(JLINK_MAKE_SCRIPT_EXTEND))
	$(QUIET)$(call JLINK_EXE_SCRIPT)
else
	$(QUIET)$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(SOURCE_ROOT)apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(DCT2_IMAGE_PLATFORM) $(SFLASH_DCT2_LOC) $(PLATFORM_BUS) 0 43909" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif
endif

else
download_dct:
	@:

no_dct:
	$(QUIET)$(ECHO) DCT unmodified

endif


run: $(SHOULD_I_WAIT_FOR_DOWNLOAD)
	$(QUIET)$(ECHO) Resetting target
ifeq ($(JLINK_NATIVE), 1)
	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL,,,$(JLINK_MAKE_SCRIPT_GO))
	$(QUIET)$(call JLINK_EXE_SCRIPT)
else
	$(QUIET)$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg  -f $(OPENOCD_PATH)$(HOST_OPENOCD)_gdb_jtag.cfg -c "resume" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Target running
endif

copy_output_for_eclipse: build_done $(BUILD_DIR)/eclipse_debug
	$(QUIET)$(CP) $(LINK_OUTPUT_FILE) $(BUILD_DIR)/eclipse_debug/last_built.elf



debug: $(BUILD_STRING) $(SHOULD_I_WAIT_FOR_DOWNLOAD)
	$(QUIET)$(GDB_COMMAND) $(LINK_OUTPUT_FILE) -x .gdbinit_attach


$(GENERATED_MAC_FILE): $(MAC_GENERATOR)
	$(QUIET)$(PERL) $<  > $@

EXTRA_PRE_BUILD_TARGETS  += $(GENERATED_MAC_FILE) $(SFLASH_APP_DEPENDENCY) bootloader
EXTRA_POST_BUILD_TARGETS += copy_output_for_eclipse $(FS_IMAGE)  .gdbinit_platform

STAGING_DIR := $(OUTPUT_DIR)/resources/Staging/
FS_IMAGE    := $(OUTPUT_DIR)/filesystem.bin

ifeq (1, $(FILESYSTEM_IMAGE_SECURE))
FILESYSTEM_IMAGE := $(FS_IMAGE).sig
SECURE_FS_IMAGE := $(FS_IMAGE).sig.enc

$(FS_IMAGE).sig : $(FS_IMAGE)
	$(QUIET)$(ECHO) Signing Filesystem
	$(HMAC_SHA256_SIGNER) sign $(SFLASH_SIGNING_KEY) $(FS_IMAGE) $(FS_IMAGE).sig $(BLOCK_SIZE) $(PAD_FILE_IF_NEEDED)

$(SECURE_FS_IMAGE): $(FS_IMAGE).sig APPS_LOOKUP_TABLE_RULES
	$(QUIET)$(ECHO) Encrypting Filesystem $(FILESYSTEM_IMAGE) FILESYSTEM_IMAGE_SECTOR_START is $(FILESYSTEM_IMAGE_SECTOR_START)
	$(call CONV_SLASHES, $(AES128_CBC_ENCRYPTOR) enc $(SFLASH_ENCRYPTION_KEY) $(FILESYSTEM_IMAGE) $(FILESYSTEM_IMAGE).enc $(BLOCK_SIZE) $(FILESYSTEM_IMAGE_SECTOR_START))

else
FILESYSTEM_IMAGE := $(FS_IMAGE)
endif

$(FS_IMAGE): $(STRIPPED_LINK_OUTPUT_FILE) display_map_summary $(STAGING_DIR).d
	$(QUIET)$(ECHO) Creating Filesystem BCM94390x_targets.mk ...
ifeq (1, $(BOOTLOADER_LOAD_MAIN_APP_FROM_FILESYSTEM))
	$(QUIET)$(CP) $(STRIPPED_LINK_OUTPUT_FILE) $(STAGING_DIR)app.elf
else
	$(QUIET)$(RM) $(STAGING_DIR)app.elf
endif
	$(QUIET)$(ECHO) $(COMMON_TOOLS_PATH)mk_wicedfs32 $(FS_IMAGE) $(STAGING_DIR)
	$(QUIET)$(COMMON_TOOLS_PATH)mk_wicedfs32 $(FS_IMAGE) $(STAGING_DIR)
	$(QUIET)$(ECHO) Creating Filesystem Done

ifeq (1, $(APP0_SECURE))
APP0 := $(STRIPPED_LINK_OUTPUT_FILE).sig
SECURE_APP0 :=$(STRIPPED_LINK_OUTPUT_FILE:$(SECURE_OUTPUT_SUFFIX)=.sig.enc$(SECURE_OUTPUT_SUFFIX))

$(STRIPPED_LINK_OUTPUT_FILE).sig : $(STRIPPED_LINK_OUTPUT_FILE)
	$(QUIET)$(ECHO) Signing APP0
	$(HMAC_SHA256_SIGNER) sign $(SFLASH_SIGNING_KEY) $(STRIPPED_LINK_OUTPUT_FILE) $(STRIPPED_LINK_OUTPUT_FILE).sig $(BLOCK_SIZE) $(PAD_FILE_IF_NEEDED)

$(SECURE_APP0): $(STRIPPED_LINK_OUTPUT_FILE).sig APPS_LOOKUP_TABLE_RULES
	$(QUIET)$(ECHO) Encrypting APP0 $(APP0) , APP0_SECTOR_START is $(APP0_SECTOR_START)
	$(call CONV_SLASHES, $(AES128_CBC_ENCRYPTOR) enc $(SFLASH_ENCRYPTION_KEY) $(STRIPPED_LINK_OUTPUT_FILE).sig $(STRIPPED_LINK_OUTPUT_FILE).sig.enc $(BLOCK_SIZE) $(APP0_SECTOR_START))
else
APP0                    := $(STRIPPED_LINK_OUTPUT_FILE)
endif #APP0_SECURE

ifeq (1, $(FR_APP_SECURE))
SECURE_OTA_APP := $(OTA_APP).sig.enc

$(OTA_APP).sig : $(OTA_APP) APPS_LOOKUP_TABLE_RULES
	$(QUIET)$(ECHO) Signing OTA_APP $(OTA_APP)
	$(HMAC_SHA256_SIGNER) sign $(SFLASH_SIGNING_KEY) $(OTA_APP) $(OTA_APP).sig $(BLOCK_SIZE) $(PAD_FILE_IF_NEEDED)

$(SECURE_OTA_APP): $(OTA_APP).sig
	$(QUIET)$(ECHO) Encrypting OTA_APP $(OTA_APP).sig , OTA_APP_SECTOR_START is $(OTA_APP_SECTOR_START)
	$(call CONV_SLASHES, $(AES128_CBC_ENCRYPTOR) enc $(SFLASH_ENCRYPTION_KEY) $(OTA_APP).sig $(OTA_APP).sig.enc $(BLOCK_SIZE) $(OTA_APP_SECTOR_START))
endif

ifdef BUILD_ROM

BESL_PREBUILT_FILE            := $(SOURCE_ROOT)WICED/security/BESL/BESL.ARM_CR4.release.a
ROM_LINK_OUTPUT_FILE          := $(SOURCE_ROOT)build/rom.elf
ROM_BINARY_FILE               := $(SOURCE_ROOT)build/rom.bin
ROM_LINK_C_FILE               := $(SOURCE_ROOT)build/rom.c
ROM_USE_NM_FILE               := $(SOURCE_ROOT)build/rom_use.nm
ROM_USE_LD_FILE               := $(SOURCE_ROOT)build/rom_use.ld

ROM_MAP_OUTPUT_FILE           :=$(ROM_LINK_OUTPUT_FILE:.elf=.map)
ROMLINK_OPTS_FILE             :=$(ROM_LINK_OUTPUT_FILE:.elf=.opts)
ROM_LINK_SCRIPT               := platforms/BCM43909/ROM_Build/rom.ld
ROM_LINK_LIBS := $(OUTPUT_DIR)/modules/platforms/BCM43909/ROM_build/reference.o \
                 $(OUTPUT_DIR)/libraries/NetX_Duo.a  \
                 $(OUTPUT_DIR)/libraries/ThreadX.a \
                 $(BESL_PREBUILT_FILE) \
                 $(TOOLS_ROOT)/ARM_GNU/lib/libc.a \
                 $(TOOLS_ROOT)/ARM_GNU/lib/libgcc.a

  #Supplicant_besl.a
#ROM_LINK_LIBS := NetX_Duo.a  ThreadX.a
#ROM_LINK_STDLIBS := $(TOOLS_ROOT)/ARM_GNU/lib/libc.a \
#                    $(TOOLS_ROOT)/ARM_GNU/lib/libgcc.a
#ROM_LINK_LIBS := $(addprefix $(OUTPUT_DIR)/libraries/, $(ROM_LINK_LIBS))
#ROM_LINK_LIBS += $(BESL_PREBUILT_FILE)
#ROM_BOOT_SRC := reference.S
#boot.c jumps.c


#ROM_BOOT_OUTPUT := $(addprefix $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,WICED),$(ROM_BOOT_SRC:.S=.o))


EXTRA_POST_BUILD_TARGETS += $(ROM_LINK_OUTPUT_FILE) $(ROM_USE_LD_FILE) $(ROM_LINK_C_FILE)


ROM_LDFLAGS := -Wl,--cref -Wl,-O3 -mcpu=cortex-r4 -mlittle-endian -nostartfiles -nostdlib
WHOLE_ARCHIVE := -Wl,--whole-archive
NO_WHOLE_ARCHIVE := -Wl,--no-whole-archive

$(ROMLINK_OPTS_FILE): $(MAKEFILES_PATH)/wiced_elf.mk
	$(QUIET)$(call WRITE_FILE_CREATE, $@ ,$(ROM_LDFLAGS) $(call COMPILER_SPECIFIC_LINK_SCRIPT,$(ROM_LINK_SCRIPT)) $(call COMPILER_SPECIFIC_LINK_MAP,$(ROM_MAP_OUTPUT_FILE))  $(NO_WHOLE_ARCHIVE) $(ROM_LINK_LIBS) )



$(BESL_PREBUILT_FILE):
	$(QUIET)$(MAKE) -C WICED/security/BESL/ HOST_ARCH=ARM_CR4

$(ROM_LINK_OUTPUT_FILE): build_done $(ROM_LINK_SCRIPT) $(ROMLINK_OPTS_FILE) $(BESL_PREBUILT_FILE)
	$(QUIET)$(ECHO) Making $@
	$(QUIET)$(LINKER) -o  $@ $(OPTIONS_IN_FILE_OPTION)$(ROMLINK_OPTS_FILE)

$(ROM_BINARY_FILE): $(ROM_LINK_OUTPUT_FILE)
	$(QUIET)$(OBJCOPY) -O binary -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes $< $@

$(ROM_LINK_C_FILE): $(ROM_BINARY_FILE)
	$(QUIET)$(PERL) $(SOURCE_ROOT)tools/text_to_c/bin_to_c.pl apps_rom_image $< > $@


$(ROM_USE_NM_FILE): $(ROM_LINK_OUTPUT_FILE)
	$(QUIET)"$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)nm$(EXECUTABLE_SUFFIX)"  $< > $@


$(ROM_USE_LD_FILE): $(ROM_USE_NM_FILE)
	$(QUIET)$(PERL) $(SOURCE_ROOT)platforms/BCM43909/nm_to_ld.pl $(ROM_USE_NM_FILE) > $(ROM_USE_LD_FILE)

endif

jlink_test:
ifeq ($(JLINK_NATIVE), 1)
ifeq ($(JLINK_PATH),)
	$(info ************************************************)
	$(info Please assign path (JLINK_PATH) of J-Link tool.)
	$(info ex: test.console-CYW943907AEVAL1F JLINK_NATIVE=1 JLINK_PATH="../../../../Program\ Files\ \(x86\)/SEGGER/JLink_V630c/" JLINK_EXE="Jlink.exe" download run)
	$(error ************************************************)
endif
ifeq ($(JLINK_EXE),)
	$(info ************************************************)
	$(info Please assign execute file (JLINK_EXE) of J-Link tool.)
	$(info ex: test.console-CYW943907AEVAL1F JLINK_NATIVE=1 JLINK_PATH="../../../../Program\ Files\ \(x86\)/SEGGER/JLink_V630c/" JLINK_EXE="Jlink.exe" download run)
	$(error ************************************************)
endif

	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL)
ifeq ($(findstring J-Link,$(ECHO) $(shell $(call JLINK_EXE_SCRIPT))),)
	$(info ************************************************)
	$(info WICED can't execute J-Link properly, please recheck JLINK_PATH & JLINK_EXE and make sure it can work properly.)
	$(error ************************************************)
endif
endif

download: jlink_test APPS_LUT_DOWNLOAD $(STRIPPED_LINK_OUTPUT_FILE) display_map_summary download_bootloader $(if $(filter 1,$(XIP_SUPPORT)),download_XIP_bin)  $(if $(findstring no_dct,$(MAKECMDGOALS)),,download_dct)
package: $(RELEASE_PACKAGE)
	$(QUIET)$(ECHO) Created package successfully

$(RELEASE_PACKAGE): create_package_descriptor APPS_LUT_PACKAGE \
		$(STRIPPED_LINK_OUTPUT_FILE) display_map_summary \
		package_bootloader package_dct

download_apps: APPS_LUT_DOWNLOAD
package_apps: APPS_LUT_PACKAGE

ifneq (no_dct,$(findstring no_dct,$(MAKECMDGOALS)))
FS_DEP := download_dct
else
FS_DEP := download_bootloader
endif

download_filesystem: $(FS_IMAGE) display_map_summary  $(FS_DEP)
	$(QUIET)$(ECHO) Downloading filesystem ...

ifeq ($(JLINK_NATIVE), 1)
	$(QUIET)$(call JLINK_MAKE_SCRIPT_ALL,$(FS_IMAGE),$(SFLASH_FS_LOC), $(JLINK_MAKE_SCRIPT_EXTEND))
	$(QUIET)$(call JLINK_EXE_SCRIPT)
else
	$(QUIET)$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(SOURCE_ROOT)apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FS_IMAGE) $(SFLASH_FS_LOC) $(PLATFORM_BUS) 0 43909" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

.gdbinit_platform:
ifeq ($(SUB_BUILD),)
ifneq ($(wildcard WICED/platform/MCU/BCM4390x/common/$(APPS_CHIP_REVISION)/apps_rom/$(APPS_CHIP_REVISION)flashbl.elf),)
	$(QUIET)$(ECHO) add-sym WICED/platform/MCU/BCM4390x/common/$(APPS_CHIP_REVISION)/apps_rom/$(APPS_CHIP_REVISION)flashbl.elf 0x0000000 >> $@
	ifneq($(FLASHBL_SRC_PATH),)
	$(QUIET)$(ECHO) set substitute-path $(FLASHBL_SRC_PATH) ./WICED/platform/MCU/BCM4390x/common/$(APPS_CHIP_REVISION)/apps_rom/$(APPS_ROM_BUILD_REV) >> $@
	endif
endif
ifneq (1,$(NO_BOOTLOADER_REQUIRED))
	$(QUIET)$(ECHO) add-sym $(BOOTLOADER_LINK_FILE) 0x00000000 >> $@
endif
endif
