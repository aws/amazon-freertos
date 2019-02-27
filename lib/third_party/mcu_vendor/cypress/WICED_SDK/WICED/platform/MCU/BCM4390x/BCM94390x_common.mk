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

SECTOR_NUMBER_SCRIPT  := $(TOOLS_ROOT)/text_to_c/sector_number.pl

ifdef BUILD_ROM
$(NAME)_SOURCES += ROM_build/reference.S
$(NAME)_COMPONENTS += BESL
$(NAME)_COMPONENTS += WICED/RTOS/ThreadX/WICED
RTOS:=ThreadX
NETWORK:=NetX_Duo

endif

VALID_OSNS_COMBOS := ThreadX-NetX ThreadX-NetX_Duo NoOS FreeRTOS-LwIP

# BCM94390x-specific make targets
EXTRA_TARGET_MAKEFILES += $(SOURCE_ROOT)WICED/platform/MCU/BCM4390x/BCM94390x_targets.mk

GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)

ifndef USES_BOOTLOADER_OTA
USES_BOOTLOADER_OTA :=1
endif

INTERNAL_MEMORY_RESOURCES =

$(NAME)_COMPONENTS += filesystems/wicedfs

ifneq ($(WICED_SDMMC_SUPPORT),)
WICED_SDIO_SUPPORT := yes
GLOBAL_DEFINES += WICED_SDMMC_SUPPORT=1
endif

ifneq ($(WICED_SDIO_SUPPORT),)
GLOBAL_DEFINES += WICED_SDIO_SUPPORT=1
endif

# Uses spi_flash interface but implements own functions
GLOBAL_INCLUDES += $(SOURCE_ROOT)Library/drivers/spi_flash

#GLOBAL_DEFINES += PLATFORM_HAS_OTP

ifndef VALID_BUSES
VALID_BUSES := SoC.43909
endif

ifndef BUS
BUS:=SoC.43909
endif

GLOBAL_INCLUDES  += $(SOURCE_ROOT)platforms/$(PLATFORM_DIRECTORY)
GLOBAL_INCLUDES  += $(PLATFORM_SOURCES)
GLOBAL_INCLUDES  += $(SOURCE_ROOT)WICED/platform/include
GLOBAL_INCLUDES  += $(SOURCE_ROOT)libraries/bluetooth/include

$(NAME)_LINK_FILES :=

GLOBAL_DEFINES += WICED_DISABLE_CONFIG_TLS
## FIXME: need to uncomment these with a proper solution post-43909 Alpha.
#GLOBAL_DEFINES += PLATFORM_BCM94390X
#GLOBAL_DEFINES += WICED_BT_USE_RESET_PIN
#GLOBAL_DEFINES += GSIO_UART

WIFI_IMAGE_DOWNLOAD := buffered

ifneq ($(BUS),SoC.43909)
ifneq ($(BUS),SDIO)
$(error This platform only supports "$(VALID_BUSES)" bus protocol. Currently set to "$(BUS)")
endif
endif

ifeq (1, $(SECURE_SFLASH))
ifeq (1, $(OTP_SECUREKEYS_OFFSETS_DEFINED))
$(info SecureSflash Enabled)
GLOBAL_DEFINES += PLATFORM_SECURESFLASH_ENABLED=1

APP0_SECURE := 1
FILESYSTEM_IMAGE_SECURE := 1
DCT_IMAGE_SECURE := 1
OTA_APP_SECURE := 1
FAILSAFE_APP_SECURE := 1
FR_APP_SECURE := 1

ifeq (1, $(DCT_IMAGE_SECURE))
GLOBAL_DEFINES += PLATFORM_SECUREDCT_ENABLED=1
endif
else
$(error Securesflash not enabled, OTP KEY offsets not defined)
endif # ifeq (1, $(OTP_SECUREKEYS_OFFSETS_DEFINED))
else
GLOBAL_DEFINES += PLATFORM_SECURESFLASH_ENABLED=0
GLOBAL_DEFINES += PLATFORM_SECUREDCT_ENABLED=0
endif # ifeq (1, $(SECURE_SFLASH))

# Uncomment BCM4390X_WL_UART_ENABLED if the WLAN firmware is utilizing the Slow UART for WL console/debug logging
# BCM4390X_WL_UART_ENABLED := yes
ifneq ($(BCM4390X_WL_UART_ENABLED),)
GLOBAL_DEFINES += PLATFORM_WL_UART_ENABLED
GLOBAL_DEFINES += BCM4390X_UART_SLOW_POLL_MODE
GLOBAL_DEFINES += BCM4390X_UART_FAST_POLL_MODE
GLOBAL_DEFINES += BCM4390X_UART_GCI_POLL_MODE
endif

#PRE_APP_BUILDS := generated_mac_address.txt

# WICED APPS
# APP0 and FILESYSTEM_IMAGE are reserved main app and resources file system
# FR_APP :=
# DCT_IMAGE :=
# OTA_APP :=
# FILESYSTEM_IMAGE :=
# WIFI_FIRMWARE :=
# APP0 :=
# APP1 :=
# APP2 :=

# WICED APPS LOOKUP TABLE

# include the correct FLASH positioning
ifeq (1, $(OTA2_SUPPORT))
include $(SOURCE_ROOT)platforms/$(subst .,/,$(PLATFORM))/ota2_image_defines.mk
#OTA2_IMAGE_CURR_LUT_AREA_BASE
APPS_LUT_HEADER_LOC := $(OTA2_IMAGE_CURR_LUT_AREA_BASE)
# The start of the Filesystem, actually
APPS_START_SECTOR := $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(OTA2_IMAGE_CURR_FS_AREA_BASE) 4096)
else
# include the normal image flash positioning
include $(SOURCE_ROOT)platforms/$(subst .,/,$(PLATFORM))/normal_image_defines.mk
APPS_LUT_HEADER_LOC := $(NORMAL_IMAGE_LUT_AREA_BASE)
APPS_START_SECTOR := $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(NORMAL_IMAGE_APPS_AREA_BASE) $(SECTOR_SIZE))
# define for the usage in codes
GLOBAL_DEFINES += NORMAL_IMAGE_DCT_1_AREA_BASE=$(NORMAL_IMAGE_DCT_1_AREA_BASE)
endif # OTA2_SUPPORT

ifneq ($(APP),ota2_bootloader)
ifneq ($(APP),bootloader)
ifneq ($(APP),tiny_bootloader)
ifneq ($(APP),sflash_write)
ifneq ($(APP),rom_test)


# Platform specific MIN MAX range for WM8533 DAC in decibels
GLOBAL_DEFINES += MIN_WM8533_DB_LEVEL=-53.0
GLOBAL_DEFINES += MAX_WM8533_DB_LEVEL=6.0

ifneq ($(MAIN_COMPONENT_PROCESSING),1)
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | IMPORTANT NOTES                                                                                     | )
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | Wi-Fi MAC Address                                                                                   | )
$(info |    The target Wi-Fi MAC address is defined in <WICED-SDK>/generated_mac_address.txt                 | )
$(info |    Ensure each target device has a unique address.                                                  | )
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | MCU & Wi-Fi Power Save                                                                              | )
$(info |    It is *critical* that applications using WICED Powersave API functions connect an accurate 32kHz | )
$(info |    reference clock to the sleep clock input pin of the WLAN chip. Please read the WICED Powersave   | )
$(info |    Application Note located in the documentation directory if you plan to use powersave features.   | )
$(info +-----------------------------------------------------------------------------------------------------+ )
endif  # ($(MAIN_COMPONENT_PROCESSING),1)

ifeq ($(BEATS_AUDIO),1)
GLOBAL_DEFINES += $(BEATS_GLOBAL_DEFINES)
$(NAME)_COMPONENTS += $(BEATS_COMPONENTS)
endif  # ($(BEATS_AUDIO),1)

endif  # ($(APP),rom_test)
endif  # ($(APP),sflash_write)
endif  # ($(APP),tiny_bootloader)
endif  # ($(APP),bootloader)
endif  # ($(APP),ota2_bootloader)

# uncomment for non-bootloader apps to avoid adding a DCT
# NODCT := 1


# GLOBAL_DEFINES += STOP_MODE_SUPPORT PRINTF_BLOCKING
