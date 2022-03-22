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

NAME := aws

export AMAZON_FREERTOS_PATH := ../../../../../
export AFR_LIBRARIES_PATH := ../../../../../libraries/
export AFR_THIRDPARTY_PATH := ../../../../../libraries/3rdparty/
export AFR_C_SDK_STANDARD_PATH := ../../../../../libraries/c_sdk/standard/
export AFR_ABSTRACTIONS_PATH := ../../../../../libraries/abstractions/
export AFR_FREERTOS_PLUS_STANDARD_PATH = ../../../../../libraries/freertos_plus/standard/
export AFR_FREERTOS_PLUS_AWS_PATH = ../../../../../libraries/freertos_plus/aws/

$(NAME)_SOURCES :=  $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/src/aws_greengrass_discovery.c        \
                    $(AFR_FREERTOS_PLUS_AWS_PATH)greengrass/src/aws_helper_secure_connect.c       \
                    $(AFR_THIRDPARTY_PATH)jsmn/jsmn.c                               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/aes.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/aesni.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/arc4.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/aria.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/asn1parse.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/asn1write.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/base64.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/bignum.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/blowfish.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/camellia.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ccm.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/certs.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/chacha20.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/chachapoly.c              \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/cipher.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/cipher_wrap.c             \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/cmac.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/constant_time.c           \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ctr_drbg.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/debug.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/des.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/dhm.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ecdh.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ecdsa.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ecjpake.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ecp.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ecp_curves.c              \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/entropy.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/entropy_poll.c            \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/error.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/gcm.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/havege.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/hkdf.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/hmac_drbg.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md.c                      \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md2.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md4.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md5.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/memory_buffer_alloc.c     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/mps_reader.c              \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/mps_trace.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/net_sockets.c             \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/nist_kw.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/oid.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/padlock.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pem.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pk.c                      \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pk_wrap.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkcs11.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkcs12.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkcs5.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkparse.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkwrite.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/platform.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/platform_util.c           \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/poly1305.c                             \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto.c                           \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_aead.c                      \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_cipher.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_client.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_driver_wrappers.c           \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_ecp.c                       \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_hash.c                      \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_mac.c                       \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_rsa.c                       \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_se.c                        \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_slot_management.c           \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_crypto_storage.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/psa_its_file.c            \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ripemd160.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/rsa.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/rsa_internal.c            \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/sha1.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/sha256.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/sha512.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_cache.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_ciphersuites.c        \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_cli.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_cookie.c              \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_msg.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_srv.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_ticket.c              \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_tls.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_tls13_keys.c          \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/threading.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/timing.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/version.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/version_features.c        \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/xtea.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/x509.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/x509_create.c             \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/x509_crl.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/x509_crt.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/x509_csr.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/x509write_crt.c           \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/x509write_csr.c           \
                    $(AFR_THIRDPARTY_PATH)mbedtls_utils/mbedtls_error.c             \
                    $(AFR_LIBRARIES_PATH)coreMQTT/source/core_mqtt.c                                                \
                    $(AFR_LIBRARIES_PATH)coreMQTT/source/core_mqtt_state.c                                          \
                    $(AFR_LIBRARIES_PATH)coreMQTT/source/core_mqtt_serializer.c                                     \
                    $(AFR_LIBRARIES_PATH)coreMQTT-Agent/source/core_mqtt_agent.c                                    \
                    $(AFR_LIBRARIES_PATH)coreMQTT-Agent/source/core_mqtt_agent_command_functions.c                  \
                    $(AFR_LIBRARIES_PATH)coreJSON/source/core_json.c                                                \
                    $(AFR_LIBRARIES_PATH)device_shadow_for_aws/source/shadow.c                     \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/core_http_client.c                                         \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/dependency/3rdparty/llhttp/src/api.c                       \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/dependency/3rdparty/llhttp/src/http.c                      \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/dependency/3rdparty/llhttp/src/llhttp.c                    \
                    $(AFR_LIBRARIES_PATH)coreJSON/source/core_json.c                                                \
                    $(AFR_LIBRARIES_PATH)device_defender_for_aws/source/defender.c                                  \
                    $(AFR_LIBRARIES_PATH)jobs_for_aws/source/jobs.c                                                 \
                    $(AFR_ABSTRACTIONS_PATH)secure_sockets/lwip/iot_secure_sockets.c                                \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)tls/src/iot_tls.c                                                     \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)utils/src/iot_system_init.c                                           \
                    $(AFR_ABSTRACTIONS_PATH)platform/freertos/iot_threads_freertos.c                                     \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/ports/wifi/iot_wifi.c           \
                    $(AFR_ABSTRACTIONS_PATH)pkcs11/corePKCS11/source/portable/mbedtls/core_pkcs11_mbedtls.c                              \
                    $(AFR_ABSTRACTIONS_PATH)pkcs11/corePKCS11/source/core_pki_utils.c                              \
                    $(AFR_ABSTRACTIONS_PATH)pkcs11/corePKCS11/source/core_pkcs11.c                              \
                    $(AFR_ABSTRACTIONS_PATH)mqtt_agent/freertos_command_pool.c                                      \
                    $(AFR_ABSTRACTIONS_PATH)mqtt_agent/freertos_agent_message.c                                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls_utils/mbedtls_utils.c                              \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)crypto/src/iot_crypto.c                                               \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/ports/pkcs11/core_pkcs11_pal.c   \
                    $(AMAZON_FREERTOS_PATH)vendors/cypress/boards/$(PLATFORM)/ports/pkcs11/hw_poll.c

GLOBAL_INCLUDES :=  $(AFR_LIBRARIES_PATH)coreMQTT/source/include \
                    $(AFR_LIBRARIES_PATH)coreMQTT/source/interface \
                    $(AFR_LIBRARIES_PATH)coreMQTT-Agent/source/include \
                    $(AFR_LIBRARIES_PATH)coreJSON/source/include \
                    $(AFR_LIBRARIES_PATH)device_shadow_for_aws/source/include \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/include \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/interface \
                    $(AFR_LIBRARIES_PATH)coreHTTP/source/dependency/3rdparty/llhttp/include \
                    $(AFR_LIBRARIES_PATH)coreJSON/source/include \
                    $(AFR_LIBRARIES_PATH)device_defender_for_aws/source/include \
                    $(AFR_LIBRARIES_PATH)jobs_for_aws/source/include \
                    $(AFR_FREERTOS_PLUS_AWS_PATH)/greengrass/include     \
                    $(AFR_FREERTOS_PLUS_AWS_PATH)/greengrass/src         \
                    $(AMAZON_FREERTOS_PATH)demos/dev_mode_key_provisioning/include \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)tls/include \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)utils/include \
                    $(AFR_FREERTOS_PLUS_STANDARD_PATH)crypto/include \
                    $(AFR_ABSTRACTIONS_PATH)pkcs11/corePKCS11/source/include \
                    $(AFR_C_SDK_STANDARD_PATH)common/include \
                    $(AFR_ABSTRACTIONS_PATH)platform/include/platform \
                    $(AFR_ABSTRACTIONS_PATH)platform/freertos/include/platform \
                    $(AFR_ABSTRACTIONS_PATH)transport/secure_sockets \
                    $(AFR_ABSTRACTIONS_PATH)backoff_algorithm \
                    $(AFR_ABSTRACTIONS_PATH)pkcs11/include \
                    $(AFR_ABSTRACTIONS_PATH)mqtt_agent/include \
                    $(AFR_THIRDPARTY_PATH)jsmn \
                    $(AFR_THIRDPARTY_PATH)mbedtls_config \
                    $(AFR_THIRDPARTY_PATH)mbedtls/include \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library \
                    $(AFR_THIRDPARTY_PATH)mbedtls_utlils \
                    $(AFR_THIRDPARTY_PATH)unity/src

GLOBAL_DEFINES := MBEDTLS_CONFIG_FILE="<aws_mbedtls_config.h>" \
                  CONFIG_MEDTLS_USE_AFR_MEMORY
