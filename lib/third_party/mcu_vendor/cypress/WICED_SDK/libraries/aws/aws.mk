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

export AFR_THIRDPARTY_PATH := ../../../../../

$(NAME)_SOURCES :=  $(AMAZON_FREERTOS_LIB_PATH)lib/bufferpool/aws_bufferpool_static_thread_safe.c        \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/greengrass/aws_greengrass_discovery.c                 \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/greengrass/aws_helper_secure_connect.c                \
                    $(AFR_THIRDPARTY_PATH)jsmn/jsmn.c                               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/aes.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/aesni.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/arc4.c                    \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/asn1parse.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/asn1write.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/base64.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/bignum.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/blowfish.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/camellia.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ccm.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/certs.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/cipher.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/cipher_wrap.c             \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/cmac.c                    \
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
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/hmac_drbg.c               \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md.c                      \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md2.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md4.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md5.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/md_wrap.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/memory_buffer_alloc.c     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/net_sockets.c             \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/oid.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/padlock.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pem.c                     \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pk.c                      \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pk_wrap.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkcs12.c                  \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkcs5.c                   \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkparse.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/pkwrite.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/platform.c                \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/platform_util.c           \
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
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_srv.c                 \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_ticket.c              \
                    $(AFR_THIRDPARTY_PATH)mbedtls/library/ssl_tls.c                 \
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
                    $(AMAZON_FREERTOS_LIB_PATH)lib/mqtt/aws_mqtt_agent.c                                 \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/mqtt/aws_mqtt_lib.c                                   \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/secure_sockets/portable/lwip/aws_secure_sockets.c \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/shadow/aws_shadow.c                                   \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/shadow/aws_shadow_json.c                              \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/tls/aws_tls.c                                         \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/utils/aws_system_init.c                               \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/wifi/portable/cypress/$(PLATFORM)/aws_wifi.c     \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/pkcs11/mbedtls/aws_pkcs11_mbedtls.c \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/crypto/aws_crypto.c \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/pkcs11/portable/cypress/$(PLATFORM)/aws_pkcs11_pal.c \
                    $(AMAZON_FREERTOS_LIB_PATH)lib/pkcs11/portable/cypress/$(PLATFORM)/hw_poll.c

$(NAME)_INCLUDES := $(AMAZON_FREERTOS_LIB_PATH)lib/include/ \
                    $(AFR_THIRDPARTY_PATH)jsmn \
                    $(AFR_THIRDPARTY_PATH)mbedtls/include \
                    $(AFR_THIRDPARTY_PATH)unity/src