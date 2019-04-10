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

NAME := aws_test

GLOBAL_DEFINES := WICED_AMAZON_FREERTOS_SDK

ifeq ($(AMAZON_FREERTOS_ENABLE_UNIT_TESTS), 1)
GLOBAL_DEFINES += AMAZON_FREERTOS_ENABLE_UNIT_TESTS \
                  UNITY_EXCLUDE_MATH_H \
                  UNITY_INCLUDE_CONFIG_H
endif

#GLOBAL_DEFINES += WLAN_FIRMWARE_PRNG_SEED

export AMAZON_FREERTOS_PATH := ../../../../../../../../
export AMAZON_FREERTOS_LIB_PATH := ../../../../../../../
export AFR_THIRDPARTY_PATH := ../../../../../../

GLOBAL_INCLUDES +=  $(AMAZON_FREERTOS_PATH)tests/common/include/ \
                    $(AFR_THIRDPARTY_PATH)jsmn \
                    $(AFR_THIRDPARTY_PATH)pkcs11 \
                    $(AFR_THIRDPARTY_PATH)lwip/src/include/lwip \
                    $(AFR_THIRDPARTY_PATH)mbedtls/include \
                    $(AMAZON_FREERTOS_PATH)lib/include/ \
                    $(AMAZON_FREERTOS_PATH)lib/include/private \
                    $(AFR_THIRDPARTY_PATH)unity/src \
                    $(AFR_THIRDPARTY_PATH)unity/extras/fixture/src \
                    $(AMAZON_FREERTOS_PATH)tests/cypress/$(PLATFORM)/common/config_files \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/cypress/$(PLATFORM) \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/arch \
                    $(AMAZON_FREERTOS_PATH)tests/common/include/ \
                    $(AMAZON_FREERTOS_PATH)tests/cypress/$(PLATFORM)/common/config_files \
                    $(AMAZON_FREERTOS_PATH)tests/cypress/$(PLATFORM)/common/application_code/cypress_code/include

#$(info $(AMAZON_FREERTOS_PATH))
$(NAME)_SOURCES    := $(AMAZON_FREERTOS_PATH)tests/cypress/$(PLATFORM)/common/application_code/main.c \
                      $(AMAZON_FREERTOS_PATH)demos/common/logging/aws_logging_task_dynamic_buffers.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/test_runner/aws_test_runner.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/framework/aws_test_framework.c \
                      $(AFR_THIRDPARTY_PATH)unity/src/unity.c \
                      $(AFR_THIRDPARTY_PATH)unity/extras/fixture/src/unity_fixture.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/wifi/aws_test_wifi.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/tls/aws_test_tls.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/secure_sockets/aws_test_tcp.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/pkcs11/aws_test_pkcs11.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/mqtt/aws_test_mqtt_agent.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/mqtt/aws_test_mqtt_lib.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/shadow/aws_test_shadow.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/greengrass/aws_test_greengrass_discovery.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/greengrass/aws_test_helper_secure_connect.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/tls/aws_test_tls.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/crypto/aws_test_crypto.c \
                      $(AMAZON_FREERTOS_PATH)demos/common/devmode_key_provisioning/aws_dev_mode_key_provisioning.c
                    
$(NAME)_COMPONENTS += utilities/wifi
$(NAME)_COMPONENTS += aws

APPLICATION_DCT := $(AMAZON_FREERTOS_PATH)tests/cypress/$(PLATFORM)/common/application_code/cypress_code/app_dct.c

WICED_AMAZON_FREERTOS_SDK := 1