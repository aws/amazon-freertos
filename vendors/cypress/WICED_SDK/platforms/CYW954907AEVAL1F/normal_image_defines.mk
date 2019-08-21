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
#
# Constant sector size FLASH
#

SECTOR_SIZE 						 := 4096

#
#  Sample Layout for 8MB FLASH
#
#   Sizes are on sector boundaries for this platform
#
#   +---------------------------------------+
#   | Boot Loader Area						| 32k  ---< build/waf.bootloader-<platform>/binary/waf.bootloader-<platform>.trx.bin
#   +---------------------------------------+
#   | DCT 1                 				| 16k  ---< build/<your_application>-<platform>/DCT.bin
#   +---------------------------------------+
#   | DCT 2                 				| 16k  ---< For the backup of one single DCT area
#   +---------------------------------------+
#   | Application Lookup Table (LUT)		|  4k  ---< build/<your_application>-<platform>/APPS.bin
#   +---------------------------------------+
#   | APPS - Filesystem	                    |  ??  ---< build/<your_application>-<platform>/filesystem.bin
#   +--                                   --+
#   | APPS - ??? (if existed)	            |
#   +--                                   --+
#   | APPS - ??? (if existed)	            |
#   +--                                   --+
#   | APPS - APP0	                        |  ??  ---< build/<your_application>-<platform>/binary/<your_application>-<platform>.stripped.elf
#   +--                                   --+
#   | APPS - APP1 (if existed)	            |
#   +--                                   --+
#   | APPS - APP2 (if existed)	            |
#   +--                                   --+
#   | 	                      				|
#   +---------------------------------------+
#
#--------------------------------------------------------------------------------------
# Normal Image FLASH Area                  Location     Size
#--------------------------------------------------------------------------------------
NORMAL_IMAGE_FLASH_BASE                 := 0x00000000

# Boot Loader
NORMAL_IMAGE_BOOTLOADER_START           := 0x00000000	#  32k  (0x00008000)

# DCT1
NORMAL_IMAGE_DCT_1_AREA_BASE            := 0x00008000	#  16k  (0x00004000)

# DCT2
NORMAL_IMAGE_DCT_2_AREA_BASE            := 0x0000C000	#  16k  (0x00004000)

# LUT
NORMAL_IMAGE_LUT_AREA_BASE              := 0x00010000	#   4k  (0x00001000)

# APPS
# For normal image, APPS including: FR_APP DCT_IMAGE OTA_APP FILESYSTEM_IMAGE WIFI_FIRMWARE APP0 APP1 APP2
# Given the start sector number, all APPS image will be arranged automatically in "wiced_apps.mk"
# See "BUILD_APPS_RULES" for more detail
NORMAL_IMAGE_APPS_AREA_BASE             := 0x00011000

# Filesystem
# Should be an obsolete define
NORMAL_IMAGE_FS_AREA_BASE               := $(NORMAL_IMAGE_APPS_AREA_BASE)



