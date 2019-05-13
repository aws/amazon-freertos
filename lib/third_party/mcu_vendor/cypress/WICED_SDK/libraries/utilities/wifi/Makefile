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
SOURCE_ROOT :=../../../

ifneq ($(MAKECMDGOALS),wipe_source_for_test)
ifeq ($(RTOS),)
$(error Must define RTOS)
endif
ifeq ($(NETWORK),)
$(error Must define NETWORK)
endif
endif

NAME :=wifi_utils

# General WICED includes
CFLAGS += -I$(SOURCE_ROOT)include
CFLAGS += -I$(SOURCE_ROOT)tools/ARM_GNU/include
CFLAGS += -I$(SOURCE_ROOT)WICED
CFLAGS += -I$(SOURCE_ROOT)WICED/WWD/include
CFLAGS += -I$(SOURCE_ROOT)WICED/WWD
CFLAGS += -I$(SOURCE_ROOT)WICED/platform/include
CFLAGS += -I$(SOURCE_ROOT)WICED/platform/MCU
CFLAGS += -I$(SOURCE_ROOT)WICED/platform/MCU/BCM4390x/peripherals/include
CFLAGS += -I$(SOURCE_ROOT)WICED/network/$(NETWORK)/WWD
CFLAGS += -I$(SOURCE_ROOT)WICED/WWD/internal/chips/4390x
CFLAGS += -I$(SOURCE_ROOT)WICED/RTOS/$(RTOS)/WICED
CFLAGS += -I$(SOURCE_ROOT)WICED/RTOS/$(RTOS)/WWD
CFLAGS += -I$(SOURCE_ROOT)WICED/WWD/internal/bus_protocols/SDIO
CFLAGS += -I$(SOURCE_ROOT)WICED/WWD/include/network
CFLAGS += -I$(SOURCE_ROOT)WICED/network/$(NETWORK)/WICED
CFLAGS += -I$(SOURCE_ROOT)WICED/security/BESL/crypto
CFLAGS += -I$(SOURCE_ROOT)WICED/security/BESL/crypto_open
CFLAGS += -I$(SOURCE_ROOT)libraries/utilities/base64
CFLAGS += -I$(SOURCE_ROOT)libraries/utilities/TLV
CFLAGS += -I$(SOURCE_ROOT)WICED/security/BESL/include
CFLAGS += -I$(SOURCE_ROOT)libraries/utilities/ring_buffer
CFLAGS += -I$(SOURCE_ROOT)libraries/utilities/linked_list
CFLAGS += -I$(SOURCE_ROOT)libraries/filesystems/wicedfs
CFLAGS += $(addprefix -I$(SOURCE_ROOT)WICED/RTOS/$(RTOS)/WWD/, $(GLOBAL_INCLUDES))
CFLAGS += -I$(SOURCE_ROOT)WICED/platform/$(HOST_ARCH)

include wifi_utils.mk

SOURCES := $($(NAME)_SOURCES)
CFLAGS += $(addprefix -I,$(GLOBAL_INCLUDES)) $(addprefix -D,$(GLOBAL_DEFINES)) $(addprefix -I,$($(NAME)_INCLUDES)) $(addprefix -D,$($(NAME)_DEFINES)) $($(NAME)_CFLAGS)

KEEP_LIST_DIR := $(SOURCE_ROOT)libraries/utilities/wifi_utils

include $(SOURCE_ROOT)tools/release/wiced_library.mk
