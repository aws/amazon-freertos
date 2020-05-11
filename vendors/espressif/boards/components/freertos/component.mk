AMAZON_FREERTOS_LIB_DIR := ../../../../../libraries
AMAZON_FREERTOS_KERNEL_DIR := ../../../../../freertos_kernel
ifndef AMAZON_FREERTOS_ENABLE_UNIT_TEST
AMAZON_FREERTOS_ESP32 := ../../../../../vendors/espressif/boards/esp32/aws_demos
else
AMAZON_FREERTOS_ESP32 := ../../../../../vendors/espressif/boards/esp32/aws_tests
endif


COMPONENT_ADD_INCLUDEDIRS := include \
                            ${AMAZON_FREERTOS_ESP32}/config_files \
                            ${AMAZON_FREERTOS_ESP32}/config_files/default_pkcs11_config \
                            ${AMAZON_FREERTOS_KERNEL_DIR}/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/c_sdk/standard/common/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/freertos_plus/standard/crypto/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/c_sdk/aws/defender/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/freertos_plus/aws/greengrass/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/c_sdk/standard/mqtt/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/freertos_plus/aws/ota/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/freertos_plus/standard/pkcs11/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/abstractions/pkcs11/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/abstractions/secure_sockets/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/c_sdk/aws/shadow/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/freertos_plus/standard/utils/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/freertos_plus/standard/tls/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/abstractions/wifi/include \
                            $(AMAZON_FREERTOS_LIB_DIR)/c_sdk/standard/common/include/private \
                            ${AMAZON_FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Xtensa_ESP32/include

ifdef AMAZON_FREERTOS_ENABLE_UNIT_TEST
COMPONENT_ADD_INCLUDEDIRS += $(AMAZON_FREERTOS_LIB_DIR)/freertos_plus/standard/freertos_plus_posix/include
endif

COMPONENT_SRCDIRS := . ${AMAZON_FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Xtensa_ESP32 $(AMAZON_FREERTOS_KERNEL_DIR)
COMPONENT_ADD_LDFLAGS += -Wl,--undefined=uxTopUsedPriority
COMPONENT_ADD_LDFRAGMENTS += linker.lf
