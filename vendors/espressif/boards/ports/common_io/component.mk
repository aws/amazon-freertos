AMAZON_FREERTOS_COMMON_IO_DIR := ../../../../../../libraries/abstractions/common_io

COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS := $(AMAZON_FREERTOS_COMMON_IO_DIR)/include \
							 $(AMAZON_FREERTOS_COMMON_IO_DIR)/test \
							 ./include

CFLAGS += -Wno-format -Wno-maybe-uninitialized -Wno-pointer-sign -Wno-incompatible-pointer-types -Wno-unused-but-set-variable

