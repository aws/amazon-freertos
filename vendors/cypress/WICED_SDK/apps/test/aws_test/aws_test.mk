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

ifeq ($(FREERTOS_ENABLE_UNIT_TESTS), 1)
GLOBAL_DEFINES += FREERTOS_ENABLE_UNIT_TESTS \
                  IOT_BUILD_TESTS \
                  UNITY_EXCLUDE_MATH_H \
                  UNITY_INCLUDE_CONFIG_H
endif

#GLOBAL_DEFINES += WLAN_FIRMWARE_PRNG_SEED

export AMAZON_FREERTOS_PATH := ../../../../../../
export AFR_LIBRARIES_PATH := ../../../../../../libraries/
export AFR_THIRDPARTY_PATH := ../../../../../../libraries/3rdparty/
export AFR_C_SDK_STANDARD_PATH := ../../../../../../libraries/c_sdk/standard/
export AFR_ABSTRACTIONS_PATH := ../../../../../../libraries/abstractions/
export AFR_FREERTOS_PLUS_STANDARD_PATH = ../../../../../../libraries/freertos_plus/standard/
export AFR_FREERTOS_PLUS_AWS_PATH = ../../../../../../libraries/freertos_plus/aws/

GLOBAL_INCLUDES +=  $(AMAZON_FREERTOS_PATH)tests/include \
                    $(AFR_C_SDK_STANDARD_PATH)common/include \
                    $(AFR_LIBRARIES_PATH)logging/include \
                    $(AFR_ABSTRACTIONS_PATH)platform/include  \
                    $(AFR_ABSTRACTIONS_PATH)platform/freertos/include \
                    $(AFR_ABSTRACTIONS_PATH)backoff_algorithm/source/include \
                    $(AFR_ABSTRACTIONS_PATH)transport/secure_sockets \
                    $(AFR_C_SDK_STANDARD_PATH)common/taskpool/private \
                    $(AFR_C_SDK_STANDARD_PATH)common/include/private \
                    $(AFR_C_SDK_STANDARD_PATH)common/include/types \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)utils/include       \
                    $(AFR_THIRDPARTY_PATH)jsmn \
                    $(AFR_THIRDPARTY_PATH)pkcs11 \
                    $(AFR_THIRDPARTY_PATH)lwip/src/include/lwip \
                    $(AFR_THIRDPARTY_PATH)mbedtls_config \
                    $(AFR_THIRDPARTY_PATH)mbedtls/include \
                    $(AFR_THIRDPARTY_PATH)mbedtls_utils \
                    $(AMAZON_FREERTOS_PATH)freertos_kernel/include \
                    $(AFR_THIRDPARTY_PATH)unity/src \
                    $(AFR_THIRDPARTY_PATH)unity/extras/fixture/src \
                    $(AFR_THIRDPARTY_PATH)tinycbor/src \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)tls/test \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)crypto/include \
                    $(AFR_ABSTRACTIONS_PATH)pkcs11/corePKCS11/source/include \
                    $(AFR_ABSTRACTIONS_PATH)secure_sockets/include \
                    $(AFR_PORTS_MODULES_PATH)mbedtls/include \
                    $(AFR_ABSTRACTIONS_PATH)wifi/include \
                    $(AFR_ABSTRACTIONS_PATH)wifi/test \
                    $(AFR_ABSTRACTIONS_PATH)posix/include \
                    $(AMAZON_FREERTOS_PATH)demos/dev_mode_key_provisioning/include \
                    $(AFR_LIBRARIES_PATH)coreMQTT/source/include \
                    $(AFR_LIBRARIES_PATH)coreMQTT/source/interface \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/include \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/interface \
                    $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/include     \
                    $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/src \
                    $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/test \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_posix/include \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/include \
                    $(AMAZON_FREERTOS_PATH)tests/integration_test \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/portable/Compiler/GCC/ \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_posix/include/portable/cypress/$(PLATFORM) \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_tests/config_files \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/cypress/$(PLATFORM) \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/arch \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_tests/application_code/cypress_code/include \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/dependency/3rdparty/http_parser \
                    $(AFR_LIBRARIES_PATH)device-shadow-for-aws-iot-embedded-sdk/source/include \

#$(info $(AMAZON_FREERTOS_PATH))
$(NAME)_SOURCES    := $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_tests/application_code/main.c \
                      $(AFR_LIBRARIES_PATH)logging/iot_logging_task_dynamic_buffers.c \
                      $(AFR_LIBRARIES_PATH)logging/iot_logging.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/aws_test_runner.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/aws_test_framework.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/aws_test.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/iot_test_freertos.c \
                      $(AMAZON_FREERTOS_PATH)tests/common/iot_tests_network.c \
                      $(AFR_THIRDPARTY_PATH)unity/src/unity.c \
                      $(AFR_THIRDPARTY_PATH)unity/extras/fixture/src/unity_fixture.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborencoder.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborencoder_close_container_checked.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborerrorstrings.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborparser.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborparser_dup_string.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborpretty.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborpretty_stdio.c \
                      $(AFR_ABSTRACTIONS_PATH)wifi/test/iot_test_wifi.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)tls/test/iot_test_tls.c \
                      $(AFR_ABSTRACTIONS_PATH)secure_sockets/test/iot_test_tcp.c \
                      $(AFR_ABSTRACTIONS_PATH)pkcs11/test/iot_test_pkcs11.c \
                      $(AMAZON_FREERTOS_PATH)tests/integration_test/core_mqtt_system_test.c \
                      $(AMAZON_FREERTOS_PATH)tests/integration_test/core_http_system_test.c \
                      $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/test/aws_test_ggd_system.c \
                      $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/test/aws_test_ggd_unit.c \
                      $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/test/aws_test_helper_secure_connect.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)crypto/test/iot_test_crypto.c \
                      $(AMAZON_FREERTOS_PATH)demos/dev_mode_key_provisioning/src/aws_dev_mode_key_provisioning.c \
                      $(AFR_C_SDK_STANDARD_PATH)common/taskpool/iot_taskpool.c \
                      $(AFR_ABSTRACTIONS_PATH)platform/freertos/iot_clock_freertos.c \
                      $(AFR_ABSTRACTIONS_PATH)platform/freertos/iot_threads_freertos.c \
                      $(AFR_ABSTRACTIONS_PATH)backoff_algorithm/source/backoff_algorithm.c \
                      $(AFR_ABSTRACTIONS_PATH)transport/secure_sockets/transport_secure_sockets.c \
                      $(AFR_ABSTRACTIONS_PATH)platform/test/iot_test_platform_clock.c \
                      $(AFR_ABSTRACTIONS_PATH)platform/test/iot_test_platform_threads.c \
                      $(AFR_C_SDK_STANDARD_PATH)common/test/iot_memory_leak.c \
                      $(AFR_C_SDK_STANDARD_PATH)common/test/iot_tests_taskpool.c \
                      $(AFR_C_SDK_STANDARD_PATH)common/iot_init.c \
                      $(AFR_C_SDK_STANDARD_PATH)common/iot_device_metrics.c \
                      $(AMAZON_FREERTOS_PATH)tests/integration_test/test_freertos_tcp.c \
                      $(AMAZON_FREERTOS_PATH)tests/integration_test/freertos_tcp_test_access_dns_define.h \
                      $(AMAZON_FREERTOS_PATH)tests/integration_test/freertos_tcp_test_access_tcp_define.h \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_UDP_IP.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_Sockets.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_ARP.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_DHCP.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_DNS.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_Stream_Buffer.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_IP.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_TCP_IP.c \
                      $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/FreeRTOS_TCP_WIN.c \
                      $(AMAZON_FREERTOS_PATH)tests/integration_test/shadow_system_test.c \


$(NAME)_COMPONENTS += utilities/wifi
$(NAME)_COMPONENTS += aws

APPLICATION_DCT := $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_tests/application_code/cypress_code/app_dct.c

WICED_AMAZON_FREERTOS_SDK := 1
