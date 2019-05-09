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
export AFR_THIRDPARTY_PATH := ../../../../../../modules/libraries/3rdparty/
export AFR_STANDARD_PATH := ../../../../../../modules/libraries/standard/
export AFR_AWS_PATH := ../../../../../../modules/libraries/aws/
export AFR_PORTS_MODULES_PATH := ../../../../../../modules/ports/

GLOBAL_INCLUDES +=  $(AMAZON_FREERTOS_PATH)demos/include \
                    $(AFR_STANDARD_PATH)common/include \
                    $(AFR_STANDARD_PATH)common/include/platform \
                    $(AFR_STANDARD_PATH)common/taskpool/private \
                    $(AFR_STANDARD_PATH)common/include/private \
                    $(AFR_STANDARD_PATH)common/include/types \
                    $(AFR_STANDARD_PATH)utils/include       \
                    $(AFR_PORTS_MODULES_PATH)wifi/include  \
                    $(AFR_THIRDPARTY_PATH)jsmn \
                    $(AFR_THIRDPARTY_PATH)pkcs11 \
                    $(AFR_THIRDPARTY_PATH)lwip/src/include/lwip \
                    $(AFR_THIRDPARTY_PATH)mbedtls/include \
                    $(AMAZON_FREERTOS_PATH)freertos_kernel/include \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_demos/common/config_files \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/cypress/$(PLATFORM) \
                    $(AFR_THIRDPARTY_PATH)lwip/src/portable/arch \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_demos/common/application_code/cypress_code/include \
                    $(AFR_STANDARD_PATH)provisioning/include \
                    $(AFR_PORTS_MODULES_PATH)pkcs11/include  \
                    $(AFR_PORTS_MODULES_PATH)pkcs11/mbedtls  \
                    $(AFR_PORTS_MODULES_PATH)secure_sockets/include  \
                    $(AMAZON_FREERTOS_PATH)demos/network_manager \
                    $(AFR_STANDARD_PATH)mqtt/include \
                    $(AFR_STANDARD_PATH)mqtt/include/types \
                    $(AFR_STANDARD_PATH)freertos_plus_tcp/include \
                    $(AFR_STANDARD_PATH)freertos_plus_tcp/source/portable/Compiler/GCC \
                    $(AFR_AWS_PATH)shadow/include \
                    $(AFR_AWS_PATH)shadow/include/types \
                    $(AFR_AWS_PATH)greengrass/include     \
                    $(AFR_AWS_PATH)greengrass/src \


#$(info $(AMAZON_FREERTOS_PATH))
$(NAME)_SOURCES    := $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_demos/common/application_code/main.c \
                      $(AFR_STANDARD_PATH)common/logging/aws_logging_task_dynamic_buffers.c \
                      $(AFR_STANDARD_PATH)common/logging/iot_logging.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/iot_demo_runner.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/iot_demo_afr.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/aws_demo.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/aws_demo_network_addr.c \
                      $(AMAZON_FREERTOS_PATH)demos/demo_runner/aws_demo_version.c \
                      $(AMAZON_FREERTOS_PATH)demos/mqtt/aws_mqtt_ble_hello_world.c \
                      $(AMAZON_FREERTOS_PATH)demos/mqtt/iot_demo_mqtt.c \
                      $(AMAZON_FREERTOS_PATH)demos/shadow/aws_iot_demo_shadow.c \
                      $(AFR_STANDARD_PATH)provisioning/src/aws_dev_mode_key_provisioning.c \
                      $(AMAZON_FREERTOS_PATH)demos/greengrass_connectivity/aws_greengrass_discovery_demo.c \
                      $(AMAZON_FREERTOS_PATH)demos/network_manager/aws_iot_demo_network.c \
                      $(AMAZON_FREERTOS_PATH)demos/network_manager/aws_iot_network_manager.c \
                      $(AFR_STANDARD_PATH)common/taskpool/iot_taskpool.c \
                      $(AFR_STANDARD_PATH)common/platform/iot_clock_afr.c \
                      $(AFR_STANDARD_PATH)common/platform/iot_network_afr.c \
                      $(AFR_STANDARD_PATH)common/platform/iot_threads_afr.c \
                      $(AFR_STANDARD_PATH)common/iot_init.c \

$(NAME)_COMPONENTS += utilities/wifi
$(NAME)_COMPONENTS += aws

APPLICATION_DCT := $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/aws_demos/common/application_code/cypress_code/app_dct.c

WICED_AMAZON_FREERTOS_SDK := 1
