AMAZON_FREERTOS_LIB_DIR := ../../../../../../libraries

COMPONENT_SRCDIRS := $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/library port \
                     $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/utils

COMPONENT_ADD_INCLUDEDIRS := port/include $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/include

libraries/3rdparty/mbedtls/library/ecp.o: CFLAGS += -Wno-maybe-uninitialized
libraries/3rdparty/mbedtls/library/ssl_tls.o: CFLAGS += -Wno-maybe-uninitialized
