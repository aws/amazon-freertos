#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := aws_tests

ifndef AMAZON_FREERTOS_PATH
export AMAZON_FREERTOS_PATH := $(CURDIR)/../../../..
endif

ifndef IDF_PATH
export IDF_PATH := $(AMAZON_FREERTOS_PATH)/lib/third_party/mcu_vendor/espressif/esp-idf
endif

export AMAZON_FREERTOS_ENABLE_UNIT_TEST := 1

EXTRA_COMPONENT_DIRS := $(AMAZON_FREERTOS_PATH)/tests/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code

CFLAGS += -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS -DUNITY_INCLUDE_CONFIG_H

include $(IDF_PATH)/make/project.mk

