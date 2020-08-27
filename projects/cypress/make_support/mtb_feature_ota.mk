# This file contains the description of sources that are required to enable the OTA
# feature in amazon-freertos.

################################################################################
# Additional Source files and includes needed for OTA support
################################################################################

# OTA / MCUBoot defines
#
# IMPORTANT NOTE: These defines are also used in the building of MCUBOOT
#                 they must EXACTLY match the values added to
#                 mcuboot/boot/cypress/MCUBootApp/MCUBootApp.mk
#
# Must be a multiple of 1024 (must leave __vectors on a 1k boundary)
ifneq ($(CY_TFM_PSA_SUPPORTED),1)
	# Non secure flow
	MCUBOOT_HEADER_SIZE=0x400
	MCUBOOT_IMAGE_NUMBER=1
	MCUBOOT_BOOTLOADER_SIZE=0x00018000
	CY_BOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE)  # Boot loader size defines for mcuboot & app are different, but value is the same
	CY_BOOT_PRIMARY_1_START=$(MCUBOOT_BOOTLOADER_SIZE)	# Primary Slot currently follows Bootloader sequentially
	CY_BOOT_SCRATCH_SIZE=0x00001000

ifeq ($(OTA_USE_EXTERNAL_FLASH),1)
	MCUBOOT_MAX_IMG_SECTORS=3584		# 1.75Mb max app size	# needed for imgtool in non-TFM flow
	CY_BOOT_PRIMARY_1_SIZE=0x001C0000
	CY_BOOT_SECONDARY_1_SIZE=0x001C0000
else
	MCUBOOT_MAX_IMG_SECTORS=2000		# 1Mb max app size	# needed for imgtool in non-TFM flow
	CY_BOOT_PRIMARY_1_SIZE=0x000EE000
	CY_BOOT_SECONDARY_1_SIZE=0x000EE000
endif # OTA_USE_EXTERNAL_FLASH

else
	# Secure flow
	# We need the FLASH areas mapped out with absolute addresses
	# see cy_flash_map.c for usage
	#
	MCUBOOT_HEADER_SIZE=0x400
	MCUBOOT_IMAGE_NUMBER=2
	MCUBOOT_MAX_IMG_SECTORS=2400		# 1.2Mb max app size	# needed for imgtool in non-TFM flow
	MCUBOOT_BOOTLOADER_SIZE=0x00040000
    CY_BOOT_SCRATCH_SIZE=0x00010000
    CY_BOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE) 	# Boot loader size defines for mcuboot & app are different, but value is the same
    # start values are offset from CY_FLASH_BASE
    CY_BOOT_PRIMARY_1_START=0x00050000
    CY_BOOT_PRIMARY_1_SIZE=0x00120000
    CY_BOOT_PRIMARY_2_START=0x00000000
    CY_BOOT_PRIMARY_2_SIZE=0x50000
    # Secondary_1 is same size as Primary_1
    # Secondary_2 is same size as Primary_2
    CY_BOOT_SECONDARY_1_START=0x18000000 # start of external FLASH
    CY_BOOT_SECONDARY_2_START=0x00170000 # offset from CY_FLASH_BASE
endif # CY_TFM_PSA_SUPPORTED

DEFINES+=OTA_SUPPORT=1 \
	MCUBOOT_HEADER_SIZE=$(MCUBOOT_HEADER_SIZE) \
	MCUBOOT_MAX_IMG_SECTORS=$(MCUBOOT_MAX_IMG_SECTORS) \
	CY_BOOT_SCRATCH_SIZE=$(CY_BOOT_SCRATCH_SIZE) \
	MCUBOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE) \
	CY_BOOT_BOOTLOADER_SIZE=$(CY_BOOT_BOOTLOADER_SIZE) \
	CY_BOOT_PRIMARY_1_START=$(CY_BOOT_PRIMARY_1_START) \
	CY_BOOT_PRIMARY_1_SIZE=$(CY_BOOT_PRIMARY_1_SIZE) \
	CY_BOOT_SECONDARY_1_SIZE=$(CY_BOOT_PRIMARY_1_SIZE) \
	MCUBOOT_IMAGE_NUMBER=$(MCUBOOT_IMAGE_NUMBER)

ifeq ($(MCUBOOT_IMAGE_NUMBER),2)
# extra defines for Primary slot 2 and Secondary Slot 2
# Secondary_1 is same size as Primary_1
# Secondary_2 is same size as Primary_2
DEFINES+=\
	CY_BOOT_PRIMARY_2_START=$(CY_BOOT_PRIMARY_2_START)\
	CY_BOOT_PRIMARY_2_SIZE=$(CY_BOOT_PRIMARY_2_SIZE) \
	CY_BOOT_SECONDARY_2_SIZE=$(CY_BOOT_PRIMARY_2_SIZE) \
	CY_BOOT_SECONDARY_2_START=$(CY_BOOT_SECONDARY_2_START) \

endif

ifeq ($(OTA_USE_EXTERNAL_FLASH),1)
	# non-zero for secondary slot in external FLASH
	CY_FLASH_ERASE_VALUE=1

	DEFINES+= \
		CY_BOOT_USE_EXTERNAL_FLASH=1\
		CY_FLASH_ERASE_VALUE=$(CY_FLASH_ERASE_VALUE)
else
	# zero for secondary slot in internal FLASH
	CY_FLASH_ERASE_VALUE=0

	DEFINES+= \
		CY_FLASH_ERASE_VALUE=$(CY_FLASH_ERASE_VALUE)
endif

# Paths for OTA support

# for if using mcuboot directly
# CY_AFR_MCUBOOT_SCRIPT_FILE_DIR=$(CY_AFR_ROOT)/../mcuboot1.6/scripts
# CY_AFR_MCUBOOT_KEY_DIR=$(CY_AFR_ROOT)/../mcuboot1.6/boot/cypress/keys
# CY_AFR_MCUBOOT_CYFLASH_PAL_DIR=$(CY_AFR_ROOT)/../mcuboot1.6/boot/cypress/cy_flash_pal
# CY_AFR_MCUBOOT_CYFLASH_PAL_DIR=$(CY_AFR_ROOT)/vendors/cypress/common/mcuboot/cy_flash_pal
CY_AFR_OTA_DIR=$(CY_EXTAPP_PATH)/port_support/ota
CY_AFR_MCUBOOT_DIR=$(CY_AFR_OTA_DIR)/mcuboot

ifeq ($(OS),Windows_NT)
	#
	# CygWin/MSYS ?
	#
	CY_WHICH_CYGPATH:=$(shell which cygpath)
ifneq ($(CY_WHICH_CYGPATH),)
	CY_AFR_MCUBOOT_DIR:=$(shell cygpath -m --absolute $(subst \,/,$(CY_AFR_MCUBOOT_DIR)))
	CY_BUILD_LOCATION:=$(shell cygpath -m --absolute $(subst \,/,$(CY_BUILD_LOCATION)))
else
	CY_AFR_MCUBOOT_DIR:=$(subst \,/,$(CY_AFR_MCUBOOT_DIR))
endif
endif

CY_AFR_MCUBOOT_CYFLASH_PAL_DIR=$(CY_AFR_MCUBOOT_DIR)/cy_flash_pal
CY_OUTPUT_FILE_PATH=$(CY_BUILD_LOCATION)/$(CY_AFR_BUILD)/$(TARGET)/$(CONFIG)

# Signing scripts and keys from MCUBoot
IMGTOOL_SCRIPT_NAME=./imgtool.py

# Use "Create" for PSoC 062 instead of "sign", and no key path (use a space " " for keypath to keep batch happy)
# MCUBoot must also be modified to skip checking the signature
#   Comment out and re-build MCUBootApp
#   <mcuboot>/boot/cypress/MCUBootApp/config/mcuboot_config/mcuboot_config.h
#   line 37, 38, 77
# 37: //#define MCUBOOT_SIGN_EC256
# 38: //#define NUM_ECC_BYTES (256 / 8)   // P-256 curve size in bytes, rnok: to make compilable
# 77: //#define MCUBOOT_VALIDATE_PRIMARY_SLOT
ifneq ($(CY_TFM_PSA_SUPPORTED),1)
CY_AFR_MCUBOOT_SCRIPT_FILE_DIR=$(CY_AFR_OTA_DIR)/scripts
CY_AFR_MCUBOOT_KEY_DIR=$(CY_AFR_MCUBOOT_DIR)/keys
CY_AFR_SIGN_SCRIPT_FILE_PATH=$(CY_AFR_MCUBOOT_SCRIPT_FILE_DIR)/sign_script.bash
IMGTOOL_COMMAND_ARG=create
CY_SIGNING_KEY_ARG=" "
else
CY_AFR_MCUBOOT_SCRIPT_FILE_DIR=$(CY_EXTAPP_PATH)/psoc6/psoc64tfm/security
CY_AFR_MCUBOOT_KEY_DIR=$(CY_AFR_MCUBOOT_SCRIPT_FILE_DIR)/keys
CY_AFR_SIGN_SCRIPT_FILE_PATH=$(CY_AFR_OTA_DIR)/scripts/sign_tar.bash
MCUBOOT_KEY_FILE=$(CY_AFR_MCUBOOT_KEY_DIR)/cypress-test-ec-p256.pem
IMGTOOL_COMMAND_ARG=sign
CY_SIGNING_KEY_ARG="-k $(MCUBOOT_KEY_FILE)"
endif

# Path to the linker script to use (if empty, use the default linker script).
# Resolve toolchain name
ifeq ($(TOOLCHAIN),GCC_ARM)
	# for ELF -> HEX conversion
	CY_ELF_TO_HEX=$(CY_CROSSPATH)/bin/arm-none-eabi-objcopy
	CY_ELF_TO_HEX_OPTIONS="-O ihex"
	CY_ELF_TO_HEX_FILE_ORDER="elf_first"

else ifeq ($(TOOLCHAIN),IAR)
	# for ELF -> HEX conversion
	CY_ELF_TO_HEX=$(CY_CROSSPATH)/bin/ielftool
	CY_ELF_TO_HEX_OPTIONS="--ihex"
	CY_ELF_TO_HEX_FILE_ORDER="elf_first"

else ifeq ($(TOOLCHAIN),ARM)
	# for ELF -> HEX conversion
	CY_ELF_TO_HEX=$(CY_CROSSPATH)/bin/fromelf
	CY_ELF_TO_HEX_OPTIONS="--i32 --output"
	CY_ELF_TO_HEX_FILE_ORDER="hex_first"

endif

# Define CY_TEST_APP_VERSION_IN_TAR in Application Makefile
# to test application version in TAR archive at start of OTA image download.
# NOTE: This requires that the user set the version numbers in the Makefile and
#          in aws_application_version.h and that they MATCH.
# NOTE: This will create compile warnings such as
#		'warning: "APP_VERSION_MAJOR" redefined'
#
ifneq ($(CY_TEST_APP_VERSION_IN_TAR),)
DEFINES+=\
	CY_TEST_APP_VERSION_IN_TAR=1\
	APP_VERSION_MAJOR=$(APP_VERSION_MAJOR)\
	APP_VERSION_MINOR=$(APP_VERSION_MINOR)\
	APP_VERSION_BUILD=$(APP_VERSION_BUILD)

CY_BUILD_VERSION=$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR).$(APP_VERSION_BUILD)

else

# Default value for scripts if CY_TEST_APP_VERSION_IN_TAR not defined
CY_BUILD_VERSION=0.9.0

endif

# Hex file to BIN conversion
# Toolchain path will always point to MTB toolchain
# Once we have a HEX file, we can make a bin - compiler option not important
CY_OBJ_COPY=$(CY_COMPILER_GCC_ARM_DIR)/bin/arm-none-eabi-objcopy

ifneq ($(CY_TFM_PSA_SUPPORTED),1)
POSTBUILD+=$(CY_AFR_SIGN_SCRIPT_FILE_PATH) $(CY_OUTPUT_FILE_PATH) $(CY_AFR_BUILD)\
	$(CY_ELF_TO_HEX) $(CY_ELF_TO_HEX_OPTIONS) $(CY_ELF_TO_HEX_FILE_ORDER)\
	$(CY_AFR_MCUBOOT_SCRIPT_FILE_DIR) $(IMGTOOL_SCRIPT_NAME) $(IMGTOOL_COMMAND_ARG) $(CY_FLASH_ERASE_VALUE) $(MCUBOOT_HEADER_SIZE)\
	$(MCUBOOT_MAX_IMG_SECTORS) $(CY_BUILD_VERSION) $(CY_BOOT_PRIMARY_1_START) $(CY_BOOT_PRIMARY_1_SIZE)\
	$(CY_SIGNING_KEY_ARG) $(CY_OBJ_COPY)
else
POSTBUILD+=$(CY_AFR_SIGN_SCRIPT_FILE_PATH) $(CY_OUTPUT_FILE_PATH) $(CY_AFR_BUILD) $(CY_OBJ_COPY)\
	$(CY_AFR_MCUBOOT_SCRIPT_FILE_DIR) $(IMGTOOL_SCRIPT_NAME) $(IMGTOOL_COMMAND_ARG) $(CY_FLASH_ERASE_VALUE) $(MCUBOOT_HEADER_SIZE)\
	$(CY_BUILD_VERSION) $(CY_BOOT_PRIMARY_1_START) $(CY_BOOT_PRIMARY_1_SIZE)\
	$(CY_BOOT_PRIMARY_2_SIZE) $(CY_BOOT_SECONDARY_1_START)\
	$(CY_AFR_MCUBOOT_KEY_DIR) $(CY_SIGNING_KEY_ARG)
endif

# MCUBoot location
SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/ota/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/http/*.c)\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/aws_iot_ota_agent.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/http/aws_iot_ota_http.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/mqtt/aws_iot_ota_mqtt.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/mqtt/aws_iot_ota_cbor.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/aws_iot_ota_interface.c\
    $(CY_AFR_ROOT)/libraries/3rdparty/jsmn/jsmn.c\
	$(CY_AFR_MCUBOOT_CYFLASH_PAL_DIR)/cy_flash_map.c\
	$(CY_AFR_MCUBOOT_CYFLASH_PAL_DIR)/cy_flash_psoc6.c\
    $(CY_AFR_MCUBOOT_DIR)/bootutil/src/bootutil_misc.c\
	$(CY_EXTAPP_PATH)/libraries/connectivity-utilities/JSON_parser/cy_json_parser.c\
	$(CY_EXTAPP_PATH)/port_support/untar/untar.c\

ifneq ($(CY_TFM_PSA_SUPPORTED),1)
SOURCES+=\
	$(CY_AFR_BOARD_PATH)/ports/ota/aws_ota_pal.c
else
SOURCES+=\
	$(wildcard $(CY_AFR_OTA_DIR)/ports/$(CY_AFR_TARGET)/*.c)
endif

ifeq ($(OTA_USE_EXTERNAL_FLASH),1)
SOURCES+=\
	$(CY_AFR_MCUBOOT_CYFLASH_PAL_DIR)/cy_smif_psoc6.c\
	$(CY_AFR_MCUBOOT_CYFLASH_PAL_DIR)/flash_qspi/flash_qspi.c
endif

INCLUDES+=\
    $(CY_AFR_MCUBOOT_DIR)\
    $(CY_AFR_MCUBOOT_DIR)/config\
    $(CY_AFR_MCUBOOT_DIR)/mcuboot_header\
    $(CY_AFR_MCUBOOT_DIR)/bootutil/include\
    $(CY_AFR_MCUBOOT_DIR)/sysflash\
    $(CY_AFR_MCUBOOT_CYFLASH_PAL_DIR)\
    $(CY_AFR_MCUBOOT_CYFLASH_PAL_DIR)/include\
    $(CY_AFR_MCUBOOT_CYFLASH_PAL_DIR)/include/flash_map_backend\
    $(CY_AFR_MCUBOOT_CYFLASH_PAL_DIR)/flash_qspi\
    $(CY_EXTAPP_PATH)/libraries/connectivity-utilities/JSON_parser\
    $(CY_EXTAPP_PATH)/port_support/untar\
    $(CY_EXTAPP_PATH)/libraries/connectivity-utilities\
	$(CY_AFR_BOARD_PATH)/ports/ota\
    $(CY_AFR_ROOT)/libraries/freertos_plus/standard/crypto/include\
    $(CY_AFR_ROOT)/libraries/3rdparty/jsmn\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/include\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src\
    $(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/src/http\
    $(CY_AFR_ROOT)/libraries/abstractions/wifi/include

ifeq ($(CY_AFR_IS_TESTING), 1)
# Test code
SOURCES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/test/aws_test_ota_agent.c\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/test/aws_test_ota_pal.c

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/freertos_plus/aws/ota/test
else
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/demos/ota/*.c)
endif