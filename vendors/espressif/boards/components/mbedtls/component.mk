AMAZON_FREERTOS_LIB_DIR := ../../../../../libraries
MBEDTLS_DIR := ../../../esp-idf/components/mbedtls

COMPONENT_SRCDIRS := $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/library $(MBEDTLS_DIR)/port \
                      $(MBEDTLS_DIR)/port/esp32 $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls_utils

COMPONENT_ADD_INCLUDEDIRS := $(MBEDTLS_DIR)/port/include $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/include $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/library

libraries/3rdparty/mbedtls/library/hmac_drbg.o: CFLAGS += -Wno-maybe-uninitialized
libraries/3rdparty/mbedtls/library/x509_crt.o: CFLAGS += -Wno-maybe-uninitialized
libraries/3rdparty/mbedtls/library/ecp.o: CFLAGS += -Wno-maybe-uninitialized
libraries/3rdparty/mbedtls/library/ssl_tls.o: CFLAGS += -Wno-maybe-uninitialized
