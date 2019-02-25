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

NAME := WWD_FreeRTOS_Interface_$(PLATFORM)

GLOBAL_INCLUDES := . \
                   ./$(PLATFORM_DIRECTORY)

$(NAME)_SOURCES := wwd_rtos.c

$(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)

$(NAME)_CHECK_HEADERS := \
                         wwd_FreeRTOS_systick.h \
                         wwd_rtos.h

$(NAME)_ARM_CM3_SOURCES  := ARM_CM3/low_level_init.c \
                            ARM_CM3/low_power.c
$(NAME)_ARM_CM3_INCLUDES := ./ARM_CM3

$(NAME)_ARM_CM7_SOURCES  := ARM_CM7/low_level_init.c \
                            ARM_CM7/low_power.c
$(NAME)_ARM_CM7_INCLUDES := ./ARM_CM7

$(NAME)_ARM_CM4_SOURCES  := $($(NAME)_ARM_CM3_SOURCES)
$(NAME)_ARM_CM4_INCLUDES := $($(NAME)_ARM_CM3_INCLUDES)

$(NAME)_ARM_CR4_SOURCES  := ARM_CR4/low_level_init.c \
                            ARM_CR4/low_power.c
$(NAME)_ARM_CR4_INCLUDES := ./ARM_CR4

$(NAME)_Win32_x86_SOURCES    :=
$(NAME)_Win32_x86_INCLUDES   := ./Win32_x86

$(NAME)_SOURCES += $($(NAME)_$(HOST_ARCH)_SOURCES)
GLOBAL_INCLUDES += $($(NAME)_$(HOST_ARCH)_INCLUDES)
