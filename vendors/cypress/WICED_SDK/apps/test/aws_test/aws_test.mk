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
                  IOT_BUILD_TESTS \
                  UNITY_EXCLUDE_MATH_H \
                  UNITY_INCLUDE_CONFIG_H
endif

#GLOBAL_DEFINES += WLAN_FIRMWARE_PRNG_SEED

export AMAZON_FREERTOS_PATH := ../../../../../../
export AFR_THIRDPARTY_PATH := ../../../../../../modules/libraries/3rdparty/
export AFR_STANDARD_PATH := ../../../../../../modules/libraries/standard/
export AFR_AWS_PATH := ../../../../../../modules/libraries/aws/
export AFR_PORTS_MODULES_PATH := ../../../../../../modules/ports/

GLOBAL_INCLUDES +=  $(AMAZON_FREERTOS_PATH)test/include \
                    $(AFR_STANDARD_PATH)common/include \
                    $(AFR_STANDARD_PATH)common/include/platform \
                    $(AFR_STANDARD_PATH)common/taskpool/private \
                    $(AFR_STANDARD_PATH)common/include/private \
                    $(AFR_STANDARD_PATH)common/include/types \
                    $(AFR_STANDARD_PATH)utils/include       \
                    $(AFR_THIRDPARTY_PATH)jsmn \
                    $(AFR_THIRDPARTY_PATH)pkcs11 \
                    $(AFR_THIRDPARTY_PATH)lwip/src/include/lwip \
                    $(AFR_THIRDPARTY_PATH)mbedtls/include \
                    $(AMAZON_FREERTOS_PATH)freertos_kernel/include \
                    $(AFR_THIRDPARTY_PATH)unity/src \
                    $(AFR_THIRDPARTY_PATH)unity/extras/fixture/src \
                    $(AFR_STANDARD_PATH)tls/test \
                    $(AFR_STANDARD_PATH)crypto/include \
                    $(AFR_PORTS_MODULES_PATH)pkcs11/include \
                    $(AFR_PORTS_MODULES_PATH)pkcs11/mbedtls  \
                    $(AFR_PORTS_MODULES_PATH)secure_sockets/include \
                    $(AFR_PORTS_MODULES_PATH)mbedtls/include \
                    $(AFR_PORTS_MODULES_PATH)wifi/include \
                    $(AFR_PORTS_MODULES_PATH)wifi/test \
                    $(AFR_STANDARD_PATH)provisioning/include \
                    $(AFR_STANDARD_PATH)mqtt/include \
                    $(AFR_STANDARD_PATH)mqtt/include/types \
                    $(AFR_STANDARD_PATH)mqtt/src \
                    $(AFR_STANDARD_PATH)mqtt/src/private \
                    $(AFR_STANDARD_PATH)mqtt/test/access \
                    $(AFR_AWS_PATH)shadow/include \
                    $(AFR_AWS_PATH)shadow/include/types \
                    $(AFR_AWS_PATH)shadow/src \
                    $(AFR_AWS_PATH)shadow/src/private \
                    $(AFR_AWS_PATH)greengrass/include     \
                    $(AFR_AWS_PATH)greengrass/src \
                    $(AFR_AWS_PATH)greengrass/test \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_tests/common/config_files \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/cypress/$(PLATFORM) \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/arch \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_tests/common/application_code/cypress_code/include \

#$(info $(AMAZON_FREERTOS_PATH))
$(NAME)_SOURCES    := $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_tests/common/application_code/main.c \
                      $(AFR_STANDARD_PATH)common/logging/aws_logging_task_dynamic_buffers.c \
                      $(AFR_STANDARD_PATH)common/logging/iot_logging.c \
                      $(AMAZON_FREERTOS_PATH)test/src/aws_test_runner.c \
                      $(AMAZON_FREERTOS_PATH)test/src/aws_test_framework.c \
                      $(AMAZON_FREERTOS_PATH)test/src/aws_test.c \
                      $(AMAZON_FREERTOS_PATH)test/src/iot_test_afr.c \
                      $(AMAZON_FREERTOS_PATH)test/src/iot_tests_network.c \
                      $(AFR_THIRDPARTY_PATH)unity/src/unity.c \
                      $(AFR_THIRDPARTY_PATH)unity/extras/fixture/src/unity_fixture.c \
                      $(AFR_PORTS_MODULES_PATH)wifi/test/aws_test_wifi.c \
                      $(AFR_STANDARD_PATH)tls/test/aws_test_tls.c \
                      $(AFR_PORTS_MODULES_PATH)secure_sockets/test/aws_test_tcp.c \
                      $(AFR_PORTS_MODULES_PATH)pkcs11/test/aws_test_pkcs11.c \
                      $(AFR_STANDARD_PATH)mqtt/test/aws_test_mqtt_agent.c \
                      $(AFR_STANDARD_PATH)mqtt/test/system/iot_tests_mqtt_system.c \
                      $(AFR_STANDARD_PATH)mqtt/src/iot_mqtt_api.c \
                      $(AFR_STANDARD_PATH)mqtt/test/unit/iot_tests_mqtt_api.c \
                      $(AFR_STANDARD_PATH)mqtt/test/unit/iot_tests_mqtt_receive.c \
                      $(AFR_STANDARD_PATH)mqtt/test/unit/iot_tests_mqtt_subscription.c \
                      $(AFR_STANDARD_PATH)mqtt/test/unit/iot_tests_mqtt_validate.c \
                      $(AFR_AWS_PATH)shadow/test/aws_test_shadow.c \
                      $(AFR_AWS_PATH)shadow/test/unit/aws_iot_tests_shadow_api.c \
                      $(AFR_AWS_PATH)shadow/test/unit/aws_iot_tests_shadow_parser.c \
                      $(AFR_AWS_PATH)shadow/test/system/aws_iot_tests_shadow_system.c \
                      $(AFR_AWS_PATH)greengrass/test/aws_test_greengrass_discovery.c \
                      $(AFR_AWS_PATH)greengrass/test/aws_test_helper_secure_connect.c \
                      $(AFR_STANDARD_PATH)tls/test/aws_test_tls.c \
                      $(AFR_STANDARD_PATH)crypto/test/aws_test_crypto.c \
                      $(AFR_STANDARD_PATH)provisioning/src/aws_dev_mode_key_provisioning.c \
                      $(AFR_STANDARD_PATH)common/taskpool/iot_taskpool.c \
                      $(AFR_STANDARD_PATH)common/platform/iot_clock_afr.c \
                      $(AFR_STANDARD_PATH)common/platform/iot_network_afr.c \
                      $(AFR_STANDARD_PATH)common/platform/iot_threads_afr.c \
                      $(AFR_STANDARD_PATH)common/test/iot_test_platform_clock.c \
                      $(AFR_STANDARD_PATH)common/test/iot_test_platform_threads.c \
                      $(AFR_STANDARD_PATH)common/iot_init.c \
                    
$(NAME)_COMPONENTS += utilities/wifi
$(NAME)_COMPONENTS += aws

APPLICATION_DCT := $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_tests/common/application_code/cypress_code/app_dct.c

WICED_AMAZON_FREERTOS_SDK := 1

# AFR_STANDARD_PATH)mqtt/test/unit/aws_iot_tests_mqtt_serialize_ble.c \