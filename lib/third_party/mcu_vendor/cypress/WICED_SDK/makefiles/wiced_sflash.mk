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

include $(MAKEFILES_PATH)/wiced_toolchain_common.mk

CONFIG_FILE := build/$(FRAPP)/config.mk

include $(CONFIG_FILE)

OUTPUT_DIR :=./build/$(FRAPP)/
OUTPUT_DIR_CONVD := $(call CONV_SLASHES,$(OUTPUT_DIR))

SFLASHWRITER_TGT:=waf.sflash_write-NoOS-$(PLATFORM)-$(BUS)
SFLASHWRITER_TGT_DIR:=$(SFLASHWRITER_TGT)

.PHONY: sflash sflash_download sflash_writer_app

sflash_writer_app:
	$(QUIET)$(ECHO) Building the Serial Flash Writer App $(SFLASHWRITER_TGT) $(FRAPP)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) $(SILENT) -f $(SOURCE_ROOT)Makefile -s $(SFLASHWRITER_TGT) NO_BUILD_BOOTLOADER=1
	$(QUIET)$(ECHO) Done
	$(QUIET)$(ECHO_BLANK_LINE)

./build/$(SFLASHWRITER_TGT_DIR)/config.mk: sflash_writer_app

sflash_download: sflash_writer_app sflash
	$(QUIET)$(ECHO) Downloading Serial Flash image
	$(QUIET)$(ECHO_BLANK_LINE)
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OUTPUT_DIR)sflash.bin 0x0 $(PLATFORM)-$(BUS) 1 0" -c shutdown
	$(QUIET)$(ECHO_BLANK_LINE)

#pad_dct:
#	$(QUIET)$(PERL) $(TOOLS_ROOT)/create_dct/pad_dct.pl $(DCT)

sflash: $(SFLASH_INCLUDE)
	$(QUIET)$(ECHO) Concatenating the binaries into an image for the serial-flash chip $(OTA_APP)
	$(QUIET)$(ECHO_BLANK_LINE)
	rm -rf $(OUTPUT_DIR_CONVD)sflash.bin
ifneq ($(strip $(FR_APP)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(FR_APP)) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif
ifneq ($(strip $(DCT_IMAGE)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(DCT_IMAGE)) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif
ifneq ($(strip $(OTA_APP)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(OTA_APP)) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif
ifneq ($(strip $(APP0)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(APP0)) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif