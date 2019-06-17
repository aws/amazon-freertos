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
OTP_SECUREKEYS_OFFSETS_DEFINED := 1
GLOBAL_DEFINES += OTP_WORD_NUM_SHA_KEY=258
GLOBAL_DEFINES += OTP_WORD_NUM_SHA_KEY_R=242
GLOBAL_DEFINES += OTP_WORD_NUM_AES_KEY=282
GLOBAL_DEFINES += OTP_WORD_NUM_AES_KEY_R=274
GLOBAL_DEFINES += SECUREBOOT_SHA_KEY_SIZE=32
GLOBAL_DEFINES += GSIO_I2C_REPEATED_START_NEEDS_GPIO=1
GLOBAL_DEFINES += PLATFORM_HIB_WAKE_CTRL_FREQ_BITS_FLIPPED

# Link Bootloader/Tiny-Bootloader with ROM symbols to reduce their SRAM memory
# size.
ifeq ($(LINK_WITH_ROM_SYMBOLS),TRUE)
$(NAME)_SOURCES += $(SOURCE_ROOT)common/B1/rom_offload/bootloader_rom_symbols.S
endif
