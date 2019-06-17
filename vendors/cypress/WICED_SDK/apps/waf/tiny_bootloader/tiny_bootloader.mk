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

NAME := App_WICED_tiny_Bootloader_$(PLATFORM)

$(NAME)_SOURCES    := tiny_bootloader.c

$(NAME)_LINK_FILES += tiny_bootloader.o

NoOS_START_STACK   := 4000

APP_WWD_ONLY       := 1
NO_WIFI            := 1
NO_WIFI_FIRMWARE   := YES

GLOBAL_DEFINES     := WICED_NO_WIFI \
                      NO_WIFI_FIRMWARE \
                      WICED_DISABLE_STDIO \
                      WICED_DISABLE_MCU_POWERSAVE \
                      WICED_DCACHE_WTHROUGH \
                      WICED_NO_VECTORS \
                      TINY_BOOTLOADER \
                      PLATFORM_NO_SFLASH_WRITE=1
GLOBAL_INCLUDES    += .
GLOBAL_INCLUDES    += ../../waf/bootloader $(GENERATED_MAC_FILE)
GLOBAL_INCLUDES    += ../../../libraries/utilities/linked_list

VALID_OSNS_COMBOS  := NoOS-NoNS
VALID_BUILD_TYPES  := release

VALID_PLATFORMS    := BCM943909* BCM943907* BCM943903* CYW943907* Quicksilver* CYW954907*
