AMAZON_FREERTOS_LIB_DIR := ../../../../../../../../libraries

COMPONENT_SRCDIRS := $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/library port \
                     $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/utils

COMPONENT_ADD_INCLUDEDIRS := $(AMAZON_FREERTOS_LIB_DIR)/3rdparty/mbedtls/include port/include

