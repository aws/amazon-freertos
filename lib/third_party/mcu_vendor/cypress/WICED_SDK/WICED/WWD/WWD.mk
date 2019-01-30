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

NAME := WWD_for_$(subst /,_,$(BUS))_$(RTOS)

GLOBAL_INCLUDES := . \
                   include \
                   include/network \
                   include/RTOS \
                   internal/bus_protocols/$(subst .,/,$(BUS)) \
                   internal/chips/$(WLAN_CHIP_FAMILY) \
                   ../../libraries/utilities/linked_list \
                   ../RTOS/$(RTOS)/WICED

$(NAME)_SOURCES := internal/wwd_thread.c \
                   internal/wwd_ap_common.c \
                   internal/wwd_thread_internal.c \
                   internal/wwd_sdpcm.c \
                   internal/wwd_internal.c \
                   internal/wwd_management.c \
                   internal/wwd_wifi.c \
                   internal/wwd_wifi_sleep.c \
                   internal/wwd_wifi_chip_common.c \
                   internal/wwd_rtos_interface.c \
                   internal/wwd_logging.c \
                   internal/wwd_debug.c \
                   internal/wwd_eapol.c \
                   internal/bus_protocols/wwd_bus_common.c \
                   internal/bus_protocols/$(subst .,/,$(BUS))/wwd_bus_protocol.c
ifdef NO_WICED_API
$(NAME)_SOURCES += ../internal/wiced_crypto.c # Pull in wiced_crypto_get_random() implementation from WICED/internal
endif


$(NAME)_CHECK_HEADERS := \
                         internal/wwd_ap.h \
                         internal/wwd_ap_common.h \
                         internal/wwd_bcmendian.h \
                         internal/wwd_internal.h \
                         internal/wwd_logging.h \
                         internal/wwd_sdpcm.h \
                         internal/wwd_thread.h \
                         internal/wwd_thread_internal.h \
                         internal/bus_protocols/wwd_bus_protocol_interface.h \
                         internal/bus_protocols/$(subst .,/,$(BUS))/wwd_bus_protocol.h \
                         internal/chips/$(WLAN_CHIP_FAMILY)/chip_constants.h \
                         include/wwd_assert.h \
                         include/wwd_constants.h \
                         include/wwd_debug.h \
                         include/wwd_events.h \
                         include/wwd_management.h \
                         include/wwd_poll.h \
                         include/wwd_structures.h \
                         include/wwd_wifi.h \
                         include/wwd_wifi_sleep.h \
                         include/wwd_wifi_chip_common.h \
                         include/wwd_wlioctl.h \
                         include/Network/wwd_buffer_interface.h \
                         include/Network/wwd_network_constants.h \
                         include/Network/wwd_network_interface.h \
                         include/platform/wwd_bus_interface.h \
                         include/platform/wwd_platform_interface.h \
                         include/platform/wwd_resource_interface.h \
                         include/platform/wwd_sdio_interface.h \
                         include/platform/wwd_spi_interface.h \
                         include/RTOS/wwd_rtos_interface.h


ifdef WICED_USE_WIFI_P2P_INTERFACE
GLOBAL_DEFINES += WICED_USE_WIFI_P2P_INTERFACE
ifeq ($(WLAN_CHIP),43362)
WLAN_CHIP_BIN_TYPE ?=-p2p
else
WLAN_CHIP_BIN_TYPE ?=
endif
else
WLAN_CHIP_BIN_TYPE ?=
endif

#Enable special FW with ampdu dump
ifdef WICED_ENABLE_AMPDU_TINY_DUMP_FW
WLAN_CHIP_BIN_TYPE :=-ampdutinydmp
endif

ifndef NO_WIFI_FIRMWARE
# WIFI_FIRMWARE_LOCATION (defaults to WIFI_FIRMWARE_IN_RESOURCES)
#   WIFI_FIRMWARE_IN_RESOURCES
#     WIFI_FIRMWARE_BIN (defaults to firmware/$(WLAN_CHIP)/$(WLAN_CHIP)$(WLAN_CHIP_REVISION)$(WLAN_CHIP_BIN_TYPE).bin)
#     WIFI_FIRMWARE_CLM_BLOB (defaults to $(WIFI_FIRMWARE).clm_blob)
#   WIFI_FIRMWARE_IN_MULTI_APP
#     Wifi firmware is put in the WIFI_FIRMWARE LUT index ... outside of this makefile
#     WIFI_FIRMWARE_CLM_BLOB - not supported
#   WIFI_FIRMWARE_IN_EXTERNAL_STORAGE
#     Wifi firmware ?
#     WIFI_FIRMWARE_CLM_BLOB - not supported
#
#  Note: These make variables (and c define) are not used by WWD but rather higher level
#        parent components and their adaptation code for WWD.  Thus they are not used here.
#           NO_WIFI
#
#$(info WIFI_FIRMWARE_LOCATION $(WIFI_FIRMWARE_LOCATION))
#$(info WIFI_FIRMWARE_BIN $(WIFI_FIRMWARE_BIN))
#$(info WIFI_FIRMWARE_CLM_BLOB $(WIFI_FIRMWARE_CLM_BLOB))
#
# Make wifi firmware a resource by default
WIFI_FIRMWARE_LOCATION ?= WIFI_FIRMWARE_IN_RESOURCES
ifeq ($(WIFI_FIRMWARE_LOCATION), WIFI_FIRMWARE_IN_RESOURCES)

# wifi firmware processing/setup
# Use default chip firwmare if not specified
WIFI_FIRMWARE_BIN ?= firmware/$(WLAN_CHIP)/$(WLAN_CHIP)$(WLAN_CHIP_REVISION)$(WLAN_CHIP_BIN_TYPE).bin
#$(info WIFI_FIRMWARE_BIN $(WIFI_FIRMWARE_BIN))
# Create resource for it
$(NAME)_RESOURCES += $(WIFI_FIRMWARE_BIN)
# The wifi firmware resource variable named based on resource file system specification
WIFI_FIRMWARE_RESOURCE_VARIABLE_NAME = $(addprefix resources_,$(subst /,_DIR_,$(subst -,_,$(subst .,_,$(subst $(SOURCE_ROOT)resources/,,$(WIFI_FIRMWARE_BIN))))))
# Add a c define such that the fixed symbol, wifi_firmwage_image, becomes the actual resource variable name.
GLOBAL_DEFINES += wifi_firmware_image=$(WIFI_FIRMWARE_RESOURCE_VARIABLE_NAME)

# wifi firmware clm blob processing/setup

# clm blob file name is same as firmware but wiht .clm_blob suffix ... unless specified
WIFI_FIRMWARE_CLM_BLOB ?= $(subst .bin,.clm_blob,$(WIFI_FIRMWARE_BIN))
#$(info WIFI_FIRMWARE_CLM_BLOB $(WIFI_FIRMWARE_CLM_BLOB))
# if the clm blob file exists then enable the feature to use it
ifneq ($(wildcard $(addprefix $(SOURCE_ROOT)resources/,$(WIFI_FIRMWARE_CLM_BLOB))),)
# Create resource for it
$(NAME)_RESOURCES += $(WIFI_FIRMWARE_CLM_BLOB)
# The clm blob resource variable named based on resource file system specification
WIFI_FIRMWARE__CLM_BLOB_RESOURCE_VARIABLE_NAME = $(addprefix resources_,$(subst /,_DIR_,$(subst -,_,$(subst .,_,$(subst $(SOURCE_ROOT)resources/,,$(WIFI_FIRMWARE_CLM_BLOB))))))
# Add a c define such that the fixed symbol, wifi_firmwage_clm_blob, becomes the actual resource variable name.
$(NAME)_DEFINES += wifi_firmware_clm_blob=$(WIFI_FIRMWARE__CLM_BLOB_RESOURCE_VARIABLE_NAME)
# Add a c define to enable using/downloading the resource at WLAN power up.
$(NAME)_DEFINES += WWD_DOWNLOAD_CLM_BLOB
$(NAME)_SOURCES += internal/wwd_clm.c
endif
endif

ifeq ($(WLAN_CHIP),43362)
GLOBAL_DEFINES += FIRMWARE_WITH_PMK_CALC_SUPPORT
endif

endif

ifeq ($(WLAN_CHIP_FAMILY),4343x)
ifeq ($(BUS),SPI)
GLOBAL_DEFINES += WWD_SPI_IRQ_FALLING_EDGE
endif
# Uncomment to configure max number of associated STA for SoftAP
#GLOBAL_DEFINES += WWD_WIFI_CONFIG_AP_MAX_ASSOC=5
endif

ifeq ($(WLAN_CHIP_FAMILY),4334x)
ifeq ($(BUS),SPI)
GLOBAL_DEFINES += WWD_DISABLE_SAVE_RESTORE
endif
# Uncomment to configure max number of associated STA for SoftAP
#GLOBAL_DEFINES += WWD_WIFI_CONFIG_AP_MAX_ASSOC=5
endif

ifeq ($(WLAN_CHIP_FAMILY),43012)
# Uncomment to configure max number of associated STA for SoftAP
#GLOBAL_DEFINES += WWD_WIFI_CONFIG_AP_MAX_ASSOC=5
endif

ifeq ($(WLAN_CHIP_FAMILY),4334)
# Uncomment to configure max number of associated STA for SoftAP
#GLOBAL_DEFINES += WWD_WIFI_CONFIG_AP_MAX_ASSOC=5
endif

ifeq ($(WLAN_CHIP_FAMILY),43362)
# Uncomment to configure max number of associated STA for SoftAP
#GLOBAL_DEFINES += WWD_WIFI_CONFIG_AP_MAX_ASSOC=5
endif

ifeq ($(WLAN_CHIP_FAMILY),43455)
# Uncomment to configure max number of associated STA for SoftAP
#GLOBAL_DEFINES += WWD_WIFI_CONFIG_AP_MAX_ASSOC=5
endif

ifeq ($(WLAN_CHIP_FAMILY),4390)
# Uncomment to configure max number of associated STA for SoftAP
#GLOBAL_DEFINES += WWD_WIFI_CONFIG_AP_MAX_ASSOC=5
endif

ifeq ($(WLAN_CHIP_FAMILY),4390x)
# Uncomment to configure max number of associated STA for SoftAP
#GLOBAL_DEFINES += WWD_WIFI_CONFIG_AP_MAX_ASSOC=5
endif

ifeq ($(WLAN_CHIP),)
$(error ERROR: WLAN_CHIP must be defined in your platform makefile)
endif

ifeq ($(WLAN_CHIP_REVISION),)
$(error ERROR: WLAN_CHIP_REVISION must be defined in your platform makefile)
endif

ifeq ($(WLAN_CHIP_FAMILY),)
$(error ERROR: WLAN_CHIP_FAMILY must be defined in your platform makefile)
endif

ifeq ($(HOST_OPENOCD),)
$(error ERROR: HOST_OPENOCD must be defined in your platform makefile)
endif

$(NAME)_SOURCES += internal/chips/$(WLAN_CHIP_FAMILY)/wwd_ap.c \
                   internal/chips/$(WLAN_CHIP_FAMILY)/wwd_chip_specific_functions.c

$(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)

$(NAME)_COMPONENTS += utilities/TLV
