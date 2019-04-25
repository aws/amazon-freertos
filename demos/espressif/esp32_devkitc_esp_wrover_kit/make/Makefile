#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
# To use your own main.c, clone this demo directory and modify the
# AMAZON_FREERTOS_PATH below or pass it to make on the command line

PROJECT_NAME := aws_demos

ifndef AMAZON_FREERTOS_PATH
export AMAZON_FREERTOS_PATH := $(CURDIR)/../../../..
endif

# Amazon FreeRTOS relies on the bundled ESP-IDF. If you are sure you need to override the ESP-IDF,
# also set the variable CUSTOM_IDF_PATH
ifndef IDF_PATH
	export IDF_PATH := $(AMAZON_FREERTOS_PATH)/lib/third_party/mcu_vendor/espressif/esp-idf
else
	ifndef CUSTOM_IDF_PATH
	$(error "IDF path is set. Amazon FreeRTOS expects a specific ESP-IDF. Pleae unset IDF_PATH or if you are sure, set CUSTOM_IDF_PATH and rerun.")
	endif
endif

EXTRA_COMPONENT_DIRS := $(AMAZON_FREERTOS_PATH)/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code

include $(IDF_PATH)/make/project.mk
