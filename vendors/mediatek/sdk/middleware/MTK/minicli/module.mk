##############################################################################
#
# External Dependencies
#
#############################################################################


ifeq ($(MTK_FOTA_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/fota/inc/76x7
endif

CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/wifi/inc

CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/iperf/inc

ifeq ($(MTK_SPI_THROUGHPUT), y)
CFLAGS  += -I$(SOURCE_DIR)/project/mt7687_hdk/test_loads/spi_slave_throughput/inc
endif

CFLAGS  += -I$(SOURCE_DIR)/project/common/bsp_ex/inc

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minisupp/inc

ifeq ($(MTK_HOMEKIT_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/homekit/inc
endif

ifeq ($(MTK_HOMEKIT_HAP_MOCK), y)
ifeq ($(MTK_HOMEKIT_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/homekit/inc
endif
endif

ifeq ($(MTK_MDNS_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mDNSResponder/inc
endif

ifeq ($(MTK_HOMEKIT_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/homekit/inc
endif

ifeq ($(MTK_HOMEKIT_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/homekit/inc
endif

ifeq ($(MTK_MPERF_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/protected/mperf/inc
endif

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc

CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc

CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/ping/inc

CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/util/inc

CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/wifi/inc

CFLAGS  += -I$(SOURCE_DIR)//kernel/service/inc

CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/util/inc

ifeq ($(MTK_BLE_CLI_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
endif

ifeq ($(MTK_BLE_BQB_CLI_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
endif

ifeq ($(MTK_SECURE_BOOT_ENABLE), y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/secure_boot/inc
endif

CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/util/inc


##############################################################################
#
# Configurations
#
#############################################################################

MTK_MINICLI_ROOT ?= middleware/MTK/minicli

CFLAGS  += -I$(MTK_MINICLI_ROOT)/inc

C_FILES += $(MTK_MINICLI_ROOT)/src/minicli_api.c

ifneq ($(wildcard $(strip $(SOURCE_DIR))/$(MTK_MINICLI_ROOT)/src_protected/),)
include $(SOURCE_DIR)/$(MTK_MINICLI_ROOT)/src_protected/GCC/module.mk
else
LIBS += $(SOURCE_DIR)/$(MTK_MINICLI_ROOT)/lib/libminicli_CM4_GCC.a
endif

