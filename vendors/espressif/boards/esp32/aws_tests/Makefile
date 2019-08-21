#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := aws_tests

ifndef AMAZON_FREERTOS_PATH
export AMAZON_FREERTOS_PATH := $(CURDIR)/../../../../..
endif

ifndef IDF_PATH
export IDF_PATH := $(AMAZON_FREERTOS_PATH)/vendors/espressif/esp-idf

export AMAZON_FREERTOS_ENABLE_UNIT_TEST := 1

EXTRA_COMPONENT_DIRS := $(AMAZON_FREERTOS_PATH)/vendors/espressif/boards/esp32/aws_tests/application_code/espressif_code

CFLAGS += -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS -DUNITY_INCLUDE_CONFIG_H

include $(IDF_PATH)/make/project.mk

else
$(error ERROR: IDF_PATH is defined in your environement, it will not point to vendors/espressif/esp-idf/esp-idf. To remove the variable run the command "unset IDF_PATH")
endif



