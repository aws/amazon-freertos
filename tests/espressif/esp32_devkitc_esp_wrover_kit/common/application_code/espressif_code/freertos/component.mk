AMAZON_FREERTOS_LIB_DIR := ../../../../../../../lib

COMPONENT_ADD_INCLUDEDIRS := include ../../../config_files $(AMAZON_FREERTOS_LIB_DIR)/include $(AMAZON_FREERTOS_LIB_DIR)/include/private $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS/portable/ThirdParty/GCC/Xtensa_ESP32/include
COMPONENT_SRCDIRS := . $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS/portable/ThirdParty/GCC/Xtensa_ESP32 $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS
COMPONENT_ADD_LDFLAGS += -Wl,--undefined=uxTopUsedPriority
