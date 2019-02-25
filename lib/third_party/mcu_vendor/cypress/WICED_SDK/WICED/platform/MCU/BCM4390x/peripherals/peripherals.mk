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

NAME = 43909_Peripheral_Drivers

$(eval $(call PLATFORM_LOCAL_DEFINES_INCLUDES_43909, ..))

$(NAME)_SOURCES := platform_watchdog.c        \
                   platform_chipcontrol.c     \
                   platform_m2m.c             \
                   platform_pinmux.c          \
                   platform_gpio.c            \
                   platform_spi_i2c.c         \
                   platform_mcu_powersave.c   \
                   platform_cores_powersave.c \
                   platform_backplane.c       \
                   platform_rtc.c             \
                   platform_hib.c             \
                   platform_otp.c             \
                   platform_8021as_clock.c    \
                   platform_audio_timer.c     \
                   platform_spi_flash.c       \
                   platform_ascu.c

ifndef WICED_AMAZON_FREERTOS_SDK
$(NAME)_SOURCES += platform_adc.c
endif
ifeq (,$(PLATFORM_NO_I2S))
$(NAME)_SOURCES += platform_i2s.c
else
GLOBAL_DEFINES  += PLATFORM_NO_I2S=1
endif

ifeq (,$(PLATFORM_NO_PWM))
$(NAME)_SOURCES += platform_pwm.c
else
GLOBAL_DEFINES  += PLATFORM_NO_PWM=1
endif

ifeq (,$(PLATFORM_NO_DDR))
$(NAME)_SOURCES += platform_ddr.c
$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/tlsf
else
GLOBAL_DEFINES  += PLATFORM_NO_DDR=1
endif

ifneq (,$(PLATFORM_NO_SOCSRAM_POWERDOWN))
GLOBAL_DEFINES  += PLATFORM_NO_SOCSRAM_POWERDOWN=1
endif

$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/spi_flash
$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/uart
$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/shared
$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/crypto/tiny_crypto

ifeq (,$(PLATFORM_NO_USB))
ifeq ($(RTOS),ThreadX)
ifneq ($(WICED_USB_SUPPORT),)
GLOBAL_DEFINES += WICED_USB_SUPPORT=1

$(NAME)_COMPONENTS += drivers/USB/USBX
$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/usb_host
$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/usb_device
endif
endif
endif

ifneq ($(WICED_SDIO_SUPPORT),)
$(NAME)_SOURCES    += platform_sdio_host.c
$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/sdio_host
endif

ifeq (,$(APP_WWD_ONLY)$(NS_WWD_ONLY)$(RTOS_WWD_ONLY)$(PLATFORM_NO_GMAC))
$(NAME)_COMPONENTS += MCU/BCM4390x/peripherals/ethernet
else
GLOBAL_DEFINES     += PLATFORM_NO_GMAC=1
endif
