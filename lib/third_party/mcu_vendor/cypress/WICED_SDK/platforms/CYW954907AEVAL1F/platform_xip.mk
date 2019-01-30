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

#If OTA2 is enabled, use OTA2_IMAGE_APP0_XIP_AREA_BASE from ota2_image_defines.mk
ifeq (1,$(OTA2_SUPPORT))
include platforms/$(PLATFORM)/ota2_image_defines.mk

ifeq (,$(OTA2_IMAGE_APP0_XIP_AREA_BASE))
$(error OTA2_IMAGE_APP0_XIP_AREA_BASE is not defined!)
endif # ifeq (,$(OTA2_IMAGE_APP0_XIP_AREA_BASE))

XIP_LOAD_ADDRESS             := $(OTA2_IMAGE_APP0_XIP_AREA_BASE)
else
XIP_LOAD_ADDRESS             := 0x400000
endif #ifeq (1,$(OTA2_SUPPORT))

SFLASH_BASE_ADDRESS			 := 0x14000000
#Link start address = 0x14000000 + $(XIP_LOAD_ADDRESS)
XIP_LINK_START_ADDRESS       = $(shell $(PERL) -le 'print sprintf("0x%X", $(SFLASH_BASE_ADDRESS) + $(XIP_LOAD_ADDRESS))')
#The region length used in generated app_without_rom_xip.ld:  2M
XIP_REGION_LENGTH			 := 0x200000
