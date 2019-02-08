MINISUPP_SRC = middleware/MTK/minisupp

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/util/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/include
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minisupp/inc

#################################################################################

ifeq ($(MTK_WIFI_WPS_ENABLE),y)
MINISUPP_LIB = libminisupp_wps.a
else
MINISUPP_LIB = libminisupp.a
endif

#################################################################################
ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/minisupp/src_protected/),)
MODULE_PATH += $(MID_MINISUPP_PATH)/src_protected/GCC
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minisupp/src_protected/wpa_supplicant_8.jb4_1/wpa_supplicant
LIBS    += $(OUTPATH)/$(MINISUPP_LIB)

else
LIBS    += $(SOURCE_DIR)/middleware/MTK/minisupp/lib/$(MINISUPP_LIB)
endif

#################################################################################