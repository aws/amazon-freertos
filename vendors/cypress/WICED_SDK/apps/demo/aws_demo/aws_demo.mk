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

NAME := aws_demo
GLOBAL_DEFINES := WICED_AMAZON_FREERTOS_SDK

#GLOBAL_DEFINES += WLAN_FIRMWARE_PRNG_SEED

export AMAZON_FREERTOS_PATH := ../../../../../../
export AFR_LIBRARIES_PATH := ../../../../../../libraries/
export AFR_THIRDPARTY_PATH := ../../../../../../libraries/3rdparty/
export AFR_C_SDK_STANDARD_PATH := ../../../../../../libraries/c_sdk/standard/
export AFR_C_SDK_AWS_PATH := ../../../../../../libraries/c_sdk/aws/
export AFR_FREERTOS_PLUS_STANDARD_PATH = ../../../../../../libraries/freertos_plus/standard/
export AFR_FREERTOS_PLUS_AWS_PATH = ../../../../../../libraries/freertos_plus/aws/
export AFR_ABSTRACTIONS_PATH := ../../../../../../libraries/abstractions/

GLOBAL_INCLUDES +=  $(AMAZON_FREERTOS_PATH)demos/include \
                    $(AMAZON_FREERTOS_PATH)demos/common/http_demo_helpers \
                    $(AMAZON_FREERTOS_PATH)demos/common/mqtt_demo_helpers \
                    $(AMAZON_FREERTOS_PATH)demos/common/pkcs11_helpers \
                    $(AMAZON_FREERTOS_PATH)demos/device_defender_for_aws \
                    $(AMAZON_FREERTOS_PATH)demos/device_defender_for_aws/metrics_collector/lwip \
                    $(AMAZON_FREERTOS_PATH)demos/coreMQTT_Agent \
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
                    $(AFR_ABSTRACTIONS_PATH)wifi/include  \
                    $(AFR_THIRDPARTY_PATH)jsmn \
                    $(AFR_THIRDPARTY_PATH)pkcs11 \
                    $(AFR_THIRDPARTY_PATH)lwip/src/include/lwip \
                    $(AFR_THIRDPARTY_PATH)mbedtls_config \
                    $(AFR_THIRDPARTY_PATH)mbedtls/include \
                    $(AFR_THIRDPARTY_PATH)mbedtls_utils \
                    $(AMAZON_FREERTOS_PATH)freertos_kernel/include \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_demos/config_files \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/cypress/$(PLATFORM) \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/arch \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_demos/application_code/cypress_code/include \
                    $(AMAZON_FREERTOS_PATH)demos/dev_mode_key_provisioning/include \
                    $(AFR_ABSTRACTIONS_PATH)pkcs11/corePKCS11/source/include  \
                    $(AFR_ABSTRACTIONS_PATH)secure_sockets/include  \
                    $(AMAZON_FREERTOS_PATH)demos/network_manager \
                    $(AFR_C_SDK_STANDARD_PATH)mqtt/include \
                    $(AFR_C_SDK_STANDARD_PATH)mqtt/include/types \
                    $(AFR_LIBRARIES_PATH)coreMQTT/source/include \
                    $(AFR_LIBRARIES_PATH)coreMQTT/source/interface \
                    $(AFR_LIBRARIES_PATH)coreMQTT-Agent/source/include \
                    $(AFR_ABSTRACTIONS_PATH)mqtt_agent/include  \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/include \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/interface \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/dependency/3rdparty/http_parser \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/include \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)freertos_plus_tcp/portable/Compiler/GCC \
                    $(AFR_C_SDK_AWS_PATH)shadow/include \
                    $(AFR_C_SDK_AWS_PATH)shadow/include/types \
                    $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/include     \
                    $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/src \
                    $(AFR_C_SDK_AWS_PATH)defender/include \
                    $(AFR_C_SDK_AWS_PATH)defender/src \
                    $(AFR_C_SDK_AWS_PATH)defender/src/private \
                    $(AFR_C_SDK_STANDARD_PATH)serializer/include \
                    $(AFR_THIRDPARTY_PATH)tinycbor/src \
                    $(AFR_C_SDK_STANDARD_PATH)https/include \
                    $(AFR_C_SDK_STANDARD_PATH)https/include/types \
                    $(AFR_THIRDPARTY_PATH)http_parser \
                    $(AFR_LIBRARIES_PATH)coreJSON/source/include \
                    $(AFR_LIBRARIES_PATH)device_shadow_for_aws/source/include \
                    $(AFR_LIBRARIES_PATH)device_defender_for_aws/source/include \

#$(info $(AMAZON_FREERTOS_PATH))
$(NAME)_SOURCES    := $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_demos/application_code/main.c \
                      $(AFR_LIBRARIES_PATH)logging/iot_logging_task_dynamic_buffers.c \
                      $(AFR_LIBRARIES_PATH)logging/iot_logging.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/iot_demo_runner.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/iot_demo_freertos.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/aws_demo.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/aws_demo_network_addr.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/aws_demo_version.c \
                      $(AMAZON_FREERTOS_PATH)demos/common/http_demo_helpers/http_demo_utils.c \
                      $(AMAZON_FREERTOS_PATH)demos/common/mqtt_demo_helpers/mqtt_demo_helpers.c \
                      $(AMAZON_FREERTOS_PATH)demos/common/pkcs11_helpers/pkcs11_helpers.c \
                      $(AMAZON_FREERTOS_PATH)demos/coreHTTP/http_demo_mutual_auth.c \
                      $(AMAZON_FREERTOS_PATH)demos/coreHTTP/http_demo_s3_download.c \
                      $(AMAZON_FREERTOS_PATH)demos/coreHTTP/http_demo_s3_download_multithreaded.c \
                      $(AMAZON_FREERTOS_PATH)demos/coreHTTP/http_demo_s3_upload.c \
                      $(AMAZON_FREERTOS_PATH)demos/coreMQTT/mqtt_demo_mutual_auth.c \
                      $(AMAZON_FREERTOS_PATH)demos/coreMQTT_Agent/mqtt_agent_task.c \
                      $(AMAZON_FREERTOS_PATH)demos/coreMQTT_Agent/simple_sub_pub_demo.c \
                      $(AMAZON_FREERTOS_PATH)demos/coreMQTT_Agent/subscription_manager.c \
                      $(AMAZON_FREERTOS_PATH)demos/device_shadow_for_aws/shadow_demo_main.c \
                      $(AMAZON_FREERTOS_PATH)demos/device_defender_for_aws/defender_demo.c \
                      $(AMAZON_FREERTOS_PATH)demos/device_defender_for_aws/report_builder.c \
                      $(AMAZON_FREERTOS_PATH)demos/device_defender_for_aws/metrics_collector/lwip/metrics_collector.c \
                      $(AMAZON_FREERTOS_PATH)demos/jobs_for_aws/jobs_demo.c \
                      $(AMAZON_FREERTOS_PATH)demos/tcp/aws_tcp_echo_client_single_task.c \
                      $(AMAZON_FREERTOS_PATH)demos/dev_mode_key_provisioning/src/aws_dev_mode_key_provisioning.c \
                      $(AMAZON_FREERTOS_PATH)demos/greengrass_connectivity/aws_greengrass_discovery_demo.c \
                      $(AMAZON_FREERTOS_PATH)demos/network_manager/aws_iot_demo_network.c \
                      $(AMAZON_FREERTOS_PATH)demos/network_manager/aws_iot_network_manager.c \
                      $(AFR_C_SDK_STANDARD_PATH)common/taskpool/iot_taskpool.c \
                      $(AFR_ABSTRACTIONS_PATH)platform/freertos/iot_clock_freertos.c \
                      $(AFR_ABSTRACTIONS_PATH)platform/freertos/iot_network_freertos.c \
                      $(AFR_ABSTRACTIONS_PATH)platform/freertos/iot_threads_freertos.c \
                      $(AFR_ABSTRACTIONS_PATH)backoff_algorithm/source/backoff_algorithm.c \
                      $(AFR_ABSTRACTIONS_PATH)transport/secure_sockets/transport_secure_sockets.c \
                      $(AFR_ABSTRACTIONS_PATH)mqtt_agent/freertos_agent_message.c \
                      $(AFR_ABSTRACTIONS_PATH)mqtt_agent/freertos_command_pool.c \
                      $(AFR_C_SDK_STANDARD_PATH)common/iot_init.c \
                      $(AFR_C_SDK_STANDARD_PATH)common/iot_device_metrics.c \
                      $(AFR_C_SDK_STANDARD_PATH)serializer/src/iot_json_utils.c \
                      $(AFR_C_SDK_AWS_PATH)defender/src/aws_iot_defender_api.c \
                      $(AFR_C_SDK_AWS_PATH)defender/src/aws_iot_defender_collector.c \
                      $(AFR_C_SDK_AWS_PATH)defender/src/aws_iot_defender_mqtt.c \
                      $(AFR_C_SDK_AWS_PATH)defender/src/aws_iot_defender_v1.c \
                      $(AFR_ABSTRACTIONS_PATH)platform/freertos/iot_metrics.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborencoder.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborencoder_close_container_checked.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborerrorstrings.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborparser.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborparser_dup_string.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborpretty.c \
                      $(AFR_THIRDPARTY_PATH)tinycbor/src/cborpretty_stdio.c \
                      $(AFR_C_SDK_STANDARD_PATH)serializer/src/cbor/iot_serializer_tinycbor_decoder.c \
                      $(AFR_C_SDK_STANDARD_PATH)serializer/src/cbor/iot_serializer_tinycbor_encoder.c \
                      $(AFR_C_SDK_STANDARD_PATH)serializer/src/json/iot_serializer_json_decoder.c \
                      $(AFR_C_SDK_STANDARD_PATH)serializer/src/json/iot_serializer_json_encoder.c \
                      $(AFR_C_SDK_STANDARD_PATH)https/src/iot_https_client.c \
                      $(AFR_C_SDK_STANDARD_PATH)https/src/iot_https_utils.c \

$(NAME)_COMPONENTS += utilities/wifi
$(NAME)_COMPONENTS += aws

APPLICATION_DCT := $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_demos/application_code/cypress_code/app_dct.c

WICED_AMAZON_FREERTOS_SDK := 1
