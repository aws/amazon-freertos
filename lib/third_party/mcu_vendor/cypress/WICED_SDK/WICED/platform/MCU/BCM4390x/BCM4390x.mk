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

NAME = MCU_BCM4390x

$(NAME)_SOURCES := vector_table_$(TOOLCHAIN_NAME).S \
                   start_$(TOOLCHAIN_NAME).S \
                   BCM4390x_platform.c \
                   WAF/waf_platform.c \
                   ../platform_resource.c \
                   ../platform_stdio.c \
                   ../wiced_platform_common.c \
                   ../wiced_apps_common.c \
                   ../wiced_waf_common.c \
                   ../wiced_dct_external_common.c \
                   ../wiced_dct_update.c \
                   ../../$(HOST_ARCH)/crt0_$(TOOLCHAIN_NAME).c \
                   ../../$(HOST_ARCH)/platform_cache.c \
                   ../../$(HOST_ARCH)/platform_cache_asm.S \
                   platform_unhandled_isr.c \
                   platform_vector_table.c \
                   platform_filesystem.c \
                   platform_tick.c \
                   platform_chipcommon.c \
                   platform_deep_sleep.c \
                   wwd_platform.c \
                   ../platform_nsclock.c \
                   ../../$(HOST_ARCH)/exception_handlers.c      \
                   platform_audio_info.c \
                   platform_gspi_slave.c

ifdef PLATFORM_SUPPORTS_BUTTONS
$(NAME)_SOURCES += ../platform_button.c
endif

ifndef WICED_AMAZON_FREERTOS_SDK
GLOBAL_DEFINES +=  PLATFORM_HAS_HW_CRYPTO_SUPPORT
$(NAME)_SOURCES += platform_crypto.c
endif

#for DCT with crc checking
$(NAME)_COMPONENTS  += utilities/crc

# include the ota2 specific functions
ifeq (1, $(OTA2_SUPPORT))
$(NAME)_SOURCES += ../wiced_dct_external_ota2.c
$(NAME)_COMPONENTS += filesystems/ota2
endif


HOST_ARCH  := ARM_CR4

# Host MCU alias for OpenOCD
HOST_OPENOCD := BCM4390x

GLOBAL_INCLUDES +=  . \
                    .. \
                    ../../$(HOST_ARCH) \
                    WAF \
                    peripherals \
                    peripherals/include \
                    peripherals/tlsf

ifneq ($(wildcard $(SOURCE_ROOT)WICED/platform/MCU/BCM4390x/$(HOST_MCU_VARIANT)/$(APPS_CHIP_REVISION)/$(HOST_MCU_VARIANT)$(APPS_CHIP_REVISION).mk),)
include $(SOURCE_ROOT)WICED/platform/MCU/BCM4390x/$(HOST_MCU_VARIANT)/$(HOST_MCU_VARIANT).mk
endif # wildcard $(WICED ...)

include $(CURDIR)BCM94390x_common.mk

ifdef	WICED_POWER_LOGGER
$(NAME)_SOURCES += WPL/platform_wpl.c
GLOBAL_DEFINES += WPL_ENABLE_WIFI_LOG
endif

ifeq ($(BUS),SoC.43909)
$(NAME)_SOURCES += wwd_m2m.c
else
$(NAME)_SOURCES += ../wwd_resources.c \
                   wwd_SDIO.c
endif

ifdef BUILD_ROM
$(NAME)_COMPONENTS += WICED/platform/MCU/BCM4390x/ROM_build
else
ifeq ($(IMAGE_TYPE),rom)
$(NAME)_COMPONENTS += WICED/platform/MCU/$(HOST_MCU_FAMILY)/ROM_offload
endif # rom
endif # BUILD_ROM

ifeq (1, $(XIP_SUPPORT))

include $(SOURCE_ROOT)platforms/$(PLATFORM)/platform_xip.mk


ifndef XIP_LINK_START_ADDRESS
$(error NOT defined XIP_LINK_START_ADDRESS!! )
endif

ifndef XIP_REGION_LENGTH
$(error NOT defined XIP_REGION_LENGTH!! )
endif

GLOBAL_DEFINES  += XIP_INSTRUCTION_START_ADDRESS=$(XIP_LINK_START_ADDRESS)
GLOBAL_DEFINES  += XIP_INSTRUCTION_REGION_LENGTH=$(XIP_REGION_LENGTH)
endif # XIP_SUPPORT

# $(1) is the relative path to the platform directory
define PLATFORM_LOCAL_DEFINES_INCLUDES_43909
$(NAME)_INCLUDES += $(1)/peripherals/include

$(NAME)_DEFINES += BCMDRIVER \
                   BCM_WICED \
                   BCM_CPU_PREFETCH \
                   BCMCHIPID=BCM43909_CHIP_ID \
                   UNRELEASEDCHIP \
                   BCMM2MDEV_ENABLED \
                   CFG_GMAC

ifeq ($(APPS_CHIP_REVISION),A0)
$(NAME)_DEFINES += BCMCHIPREV=0
endif
ifeq ($(APPS_CHIP_REVISION),B0)
$(NAME)_DEFINES += BCMCHIPREV=1
endif
ifeq ($(APPS_CHIP_REVISION),B1)
$(NAME)_DEFINES += BCMCHIPREV=2
endif

$(NAME)_CFLAGS += -Wundef
endef

ifneq ($(APPS_CHIP_REVISION),A0)
ifneq ($(APPS_CHIP_REVISION),B0)
GLOBAL_DEFINES += PLATFORM_ALP_CLOCK_RES_FIXUP=0
endif
endif

# USB require some fixup to use ALP clock. Necessary for A0/B0/B1 chips
ifeq ($(filter $(APPS_CHIP_REVISION),A0 B0 B1),)
GLOBAL_DEFINES += PLATFORM_USB_ALP_CLOCK_RES_FIXUP=0
endif

$(eval $(call PLATFORM_LOCAL_DEFINES_INCLUDES_43909, .))

$(NAME)_COMPONENTS  += $(TOOLCHAIN_NAME)
$(NAME)_COMPONENTS  += MCU/BCM4390x/peripherals
$(NAME)_COMPONENTS  += utilities/ring_buffer

GLOBAL_CFLAGS   += $$(CPU_CFLAGS)    $$(ENDIAN_CFLAGS_LITTLE)
GLOBAL_CXXFLAGS += $$(CPU_CXXFLAGS)  $$(ENDIAN_CXXFLAGS_LITTLE)
GLOBAL_ASMFLAGS += $$(CPU_ASMFLAGS)  $$(ENDIAN_ASMFLAGS_LITTLE)
GLOBAL_LDFLAGS  += $$(CPU_LDFLAGS)   $$(ENDIAN_LDFLAGS_LITTLE)

ifndef $(RTOS)_SYS_STACK
$(RTOS)_SYS_STACK=0
endif
ifndef $(RTOS)_FIQ_STACK
$(RTOS)_FIQ_STACK=0
endif
ifndef $(RTOS)_IRQ_STACK
$(RTOS)_IRQ_STACK=1024
endif

ifeq ($(TOOLCHAIN_NAME),GCC)
GLOBAL_LDFLAGS  += -nostartfiles
GLOBAL_LDFLAGS  += -Wl,--defsym,START_STACK_SIZE=$($(RTOS)_START_STACK) \
                   -Wl,--defsym,FIQ_STACK_SIZE=$($(RTOS)_FIQ_STACK) \
                   -Wl,--defsym,IRQ_STACK_SIZE=$($(RTOS)_IRQ_STACK) \
                   -Wl,--defsym,SYS_STACK_SIZE=$($(RTOS)_SYS_STACK)
GLOBAL_ASMFLAGS += --defsym SYS_STACK_SIZE=$($(RTOS)_SYS_STACK) \
                   --defsym FIQ_STACK_SIZE=$($(RTOS)_FIQ_STACK) \
                   --defsym IRQ_STACK_SIZE=$($(RTOS)_IRQ_STACK)

# Pick-up MCU variant linker-scripts.
GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)WICED/platform/MCU/BCM4390x/$(HOST_MCU_VARIANT)

# Let linker script include other generic linker scripts.
GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)WICED/platform/MCU/BCM4390x
endif # GCC

ifdef NO_WIFI
$(NAME)_COMPONENTS += WICED/WWD
ifndef NETWORK
NETWORK := NoNS
$(NAME)_COMPONENTS += WICED/network/NoNS
endif
endif

# These need to be forced into the final ELF since they are not referenced otherwise
$(NAME)_LINK_FILES := ../../$(HOST_ARCH)/crt0_$(TOOLCHAIN_NAME).o \
                      vector_table_$(TOOLCHAIN_NAME).o \
                      start_$(TOOLCHAIN_NAME).o

#                      ../../$(HOST_ARCH)/hardfault_handler.o

$(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS) -I$(SOURCE_ROOT)./

# Add maximum and default watchdog timeouts to definitions. Warning: Do not change MAX_WATCHDOG_TIMEOUT_SECONDS
MAX_WATCHDOG_TIMEOUT_SECONDS    = 22
MAX_WAKE_FROM_SLEEP_DELAY_TICKS = 500
GLOBAL_DEFINES += MAX_WATCHDOG_TIMEOUT_SECONDS=$(MAX_WATCHDOG_TIMEOUT_SECONDS) MAX_WAKE_FROM_SLEEP_DELAY_TICKS=$(MAX_WAKE_FROM_SLEEP_DELAY_TICKS)

# Tell that platform has data cache and set cache line size
GLOBAL_DEFINES += PLATFORM_L1_CACHE_SHIFT=5

# Tell that platform supports Low power modes (Deep Sleep / Sleep)
GLOBAL_DEFINES += PLATFORM_SUPPORTS_LOW_POWER_MODES

# Tell that platform has special WLAN features
GLOBAL_DEFINES += BCM43909

# ARM DSP/SIMD instruction set support:
# use this define to embed specific instructions
# related to the ARM DSP set.
# If further defines are needed for specific sub-set (CR4, floating point)
# make sure you protect them under this global define.
GLOBAL_DEFINES += ENABLE_ARM_DSP_INSTRUCTIONS

# Use packet release hook to refill DMA
GLOBAL_DEFINES += PLAT_NOTIFY_FREE

# DCT linker script
DCT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/dct$(LINK_SCRIPT_SUFFIX)

# Dont Link with ROM symbols except for Bootloader/Tiny-Bootloader
LINK_WITH_ROM_SYMBOLS := FALSE

ifeq ($(APP),ota2_bootloader)
####################################################################################
# Building OTA2 bootloader
####################################################################################
DEFAULT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/ota2_bootloader$(LINK_SCRIPT_SUFFIX)

ifeq (1, $(SECURE_SFLASH))
# Link Bootloader/Tiny-Bootloader with ROM symbols to reduce their SRAM memory
# size.
LINK_WITH_ROM_SYMBOLS := TRUE
endif

else
ifeq ($(APP),bootloader)
####################################################################################
# Building bootloader
####################################################################################
DEFAULT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/bootloader$(LINK_SCRIPT_SUFFIX)

ifeq (1, $(SECURE_SFLASH))
# Link Bootloader/Tiny-Bootloader with ROM symbols to reduce their SRAM memory
# size.
LINK_WITH_ROM_SYMBOLS := TRUE
endif

else
ifeq ($(APP),tiny_bootloader)
####################################################################################
# Building tiny bootloader
####################################################################################

$(NAME)_SOURCES := $(filter-out vector_table_$(TOOLCHAIN_NAME).S, $($(NAME)_SOURCES))
$(NAME)_LINK_FILES := $(filter-out vector_table_$(TOOLCHAIN_NAME).o, $($(NAME)_LINK_FILES))
ifeq (1, $(OTA2_SUPPORT))
DEFAULT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/ota2_tiny_bootloader$(LINK_SCRIPT_SUFFIX)	# temp
else
DEFAULT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/tiny_bootloader$(LINK_SCRIPT_SUFFIX)
endif
GLOBAL_DEFINES      += TINY_BOOTLOADER
ifeq (1, $(SECURE_SFLASH))
# Link Bootloader/Tiny-Bootloader with ROM symbols to reduce their SRAM memory
# size.
LINK_WITH_ROM_SYMBOLS := TRUE
endif

else
ifneq ($(filter ota_upgrade sflash_write, $(APP)),)
####################################################################################
# Building sflash_write OR ota_upgrade
####################################################################################

PRE_APP_BUILDS      += bootloader
WIFI_IMAGE_DOWNLOAD := buffered
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_without_rom$(LINK_SCRIPT_SUFFIX)
GLOBAL_DEFINES      += WICED_DISABLE_BOOTLOADER
GLOBAL_DEFINES      += __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=32768
ifeq ($(TOOLCHAIN_NAME),IAR)
GLOBAL_LDFLAGS      += --config_def __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384
endif

else

####################################################################################
# Building a stand-alone application
####################################################################################
ifeq (1, $(APP_IN_DDR))
ifeq (1, $(PLATFORM_NO_DDR))
$(error misconfiguration: APP_IN_DDR cannot be set in platform that does not have ddr)
else
ifeq ($(XIP_SUPPORT),1)
$(error XIP and DDR are exclusive, it's not allowed to have both enabled!)
endif # ifeq ($(XIP_SUPPORT),1)
DDR_SUFFIX := _ddr
endif # ifeq (1, $(PLATFORM_NO_DDR))
endif # ifeq (1, $(APP_IN_DDR))

ifneq ($(IMAGE_TYPE),rom)
ifeq (1, $(OTA2_SUPPORT))

ifneq ($(XIP_SUPPORT),1)
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/ota2_app_without_rom$(DDR_SUFFIX)$(LINK_SCRIPT_SUFFIX)
else
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_without_rom_with_xip$(LINK_SCRIPT_SUFFIX)
endif #XIP_SUPPORT == 0

else
ifneq ($(XIP_SUPPORT),1)
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_without_rom$(DDR_SUFFIX)$(LINK_SCRIPT_SUFFIX)
else
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_without_rom_with_xip$(LINK_SCRIPT_SUFFIX)
endif

endif # OTA2_SUPPORT
else
ifeq (1, $(OTA2_SUPPORT))
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/ota2_app_with_rom$(LINK_SCRIPT_SUFFIX)
else
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_with_rom$(LINK_SCRIPT_SUFFIX)
endif # OTA2_SUPPORT
endif # IMAGE_TYPE

endif # APP=ota_upgrade OR sflash_write
endif # APP=tiny_bootloader
endif # APP=bootloader
endif # APP=ota2_bootloader

ifeq ($(LINK_WITH_ROM_SYMBOLS),TRUE)
GLOBAL_LDFLAGS += -L $(SOURCE_ROOT)WICED/platform/MCU/$(HOST_MCU_FAMILY)/common/$(ROM_OFFLOAD_CHIP)/rom_offload
# The Linker script for rom symbols should be placed before the app linker script
# to ensure that symbols are linked from ROM.
DEFAULT_LINK_SCRIPT := common/$(ROM_OFFLOAD_CHIP)/rom_offload/GCC_rom_bootloader_symbols.ld $(DEFAULT_LINK_SCRIPT)
endif
