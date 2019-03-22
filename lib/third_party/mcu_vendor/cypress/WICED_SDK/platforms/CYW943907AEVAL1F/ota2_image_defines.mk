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
#   | Boot loader Area						| 32k  Size of build/waf.ota2_bootloader-<platform>/binary/waf.ota2_bootloader-<platform>.stripped.elf
#   +---------------------------------------+
#   | Factory Reset OTA Image				| 2MB - 100k for Failsafe App (Adjust this size to fit build/<your_application>-<platform>/OTA2_factory_reset_file.bin)
#   +---------------------------------------+
#   | OTA2 Failsafe App						| 100k  Size of build/waf.ota2_failsafe-<platform>/binary/waf.ota2_failsafe-<platform>.stripped.elf
#   +---------------------------------------+
#   | DCT Save area (when updating)			| 16k  (same size as normal DCT)
#   +--                                   --+
#   | Application Lookup Table (LUT)		|  4k
#   +--                                   --+
#   | DCT Copy 1                      		| 16k   DCT 1 & 2 must be contiguous
#   +--                                   --+
#   | DCT Copy 2                      		| 16k
#   +--                                   --+
#   | OTA2 Extractor Application      		| 308/316k  Size of build/snip.ota2_extract-<platform>/binary/snip.ota2_extract-<platform>.stripped.elf
#   +--                                   --+
#   | File system                      		| ???   Adjust this size to fit future update size of build/<app><platform>/filesystem.bin
#   +--                                   --+
#   | Current Application      				| ??? --\
#   +--                                   --+        >-- total 2.77MB
#   | Expansion area 		  				| ??? --/
#   +---------------------------------------+
#   | OTA2 Staging area (downloaded image)  | 2MB  Adjust this size to max. expectation of update build/<your_application>-<platform>/OTA2_image_file.bin
#	|										|      (Will probably be larger than the Factory Reset Image in Update builds)
#   +---------------------------------------+
#
#  LAST KNOWN GOOD Not supported

# OTA2 FLASH Area                        Location     Size
OTA2_IMAGE_FLASH_BASE                := 0x00000000
OTA2_IMAGE_DO_NOT_WRITE_AREA_START	 := 0x00000000  #########  DO NOT WRITE TO THE FLASH BELOW THIS AFTER MANUFACTURE  ########
OTA2_IMAGE_BOOTLOADER_START          := 0x00000000	#  32k  0x00008000

# DO NOT CHANGE THESE LOCATIONS AFTER A PRODUCT HAS SHIPPED
# DO NOT CHANGE THESE FOR AN UPDATE TO ANOTHER SDK
# (The Bootloader needs these to always be the same - bootloader does not get updated and would not know of changes between SDKs)
# New for this SDK, use new layout
# 0x00008000 + ( 0x00200000 - 0x00019000 ) = 0x001EF000
OTA2_IMAGE_FACTORY_RESET_AREA_BASE   := 0x00008000	#  2M - 100k  0x001E7000
# OTA2 Failsafe Application Storage
OTA2_IMAGE_FAILSAFE_APP_AREA_BASE	 := 0x001EF000  #  100k  0x00019000
OTA2_IMAGE_DO_NOT_WRITE_AREA_END	 := 0x00208000  #########  DO NOT WRITE TO THE FLASH ABOVE THIS AFTER MANUFACTURE  ########

OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE    := 0x00208000  #  16k  0x00004000
OTA2_IMAGE_CURR_LUT_AREA_BASE        := 0x0020c000  #   4k  0x00001000
OTA2_IMAGE_CURR_DCT_1_AREA_BASE      := 0x0020d000	#  16k  0x00004000
OTA2_IMAGE_CURR_DCT_2_AREA_BASE      := 0x00211000	#  16k  0x00004000
# DO NOT CHANGE THE ABOVE LOCATIONS AFTER A PRODUCT HAS SHIPPED
# DO NOT CHANGE THE ABOVE LOCATIONS FOR AN UPDATE TO ANOTHER SDK

# OTA2 Extraction Application Storage (start on 4k boundary)
OTA2_IMAGE_CURR_OTA_APP_AREA_BASE    := 0x00215000	# 308k 0x0004D000 was 300k, For SECURE_SFLASH 316K 0x0004F000

# File system (start on 4k boundary)
ifeq (1, $(SECURE_SFLASH))
OTA2_IMAGE_CURR_FS_AREA_BASE         := 0x00264000	# 852k  0x000D5000
else
OTA2_IMAGE_CURR_FS_AREA_BASE         := 0x00262000	# 860k  0x000D7000
endif

#App0 - Application (start on 4k boundary)
#In non-XIP case, the space of APP0 , 0x0339000 - 0x00600000 is 2.777M(0x002C7000).
#For XIP case the 2.777M space is shared with APP0_XIP binary.
#Currently APP0_XIP base is set to 0x400000. It can be adjusted according to use case.
OTA2_IMAGE_CURR_APP0_AREA_BASE       := 0x00339000
ifeq (1, $(XIP_SUPPORT))
OTA2_IMAGE_APP0_XIP_AREA_BASE		 := 0x00400000
endif #ifeq (1, $(XIP_SUPPORT))

# unused area is after APP0 and before The Staging Area

# Staging Area for OTA2 updates
# DO NOT CHANGE THIS LOCATION AFTER A PRODUCT HAS SHIPPED
# DO NOT CHANGE THIS LOCATION FOR AN UPDATE TO ANOTHER SDK
# (The Bootloader needs this to always be the same - bootloader does not get updated and would not know of changes between SDKs)
OTA2_IMAGE_STAGING_AREA_BASE         := 0x00600000	#   2M  0x00200000
OTA2_IMAGE_STAGING_AREA_SIZE         := 0x00200000
