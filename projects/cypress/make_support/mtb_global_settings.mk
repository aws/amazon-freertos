
# General configuration
COMPONENTS+=FREERTOS
DEFINES+=CYBSP_WIFI_CAPABLE CY_RTOS_AWARE CY_RETARGET_IO_CONVERT_LF_TO_CRLF CY_USE_LWIP MBEDTLS_CONFIG_FILE=\"aws_mbedtls_config.h\"

# Configuration specific to TFM
ifeq ($(CY_TFM_PSA_SUPPORTED),1)
# Additional defines
CY_TFM_ENABLED=true

COMPONENTS+=TFM_NS_FREERTOS_WRAPPER TFM_NS_INTERFACE
# Add Mbedtls defines
DEFINES+=CONFIG_MEDTLS_USE_AFR_MEMORY
# Add TFM defines
DEFINES+=CY_TFM_PSA_SUPPORTED TFM_MULTI_CORE_MULTI_CLIENT_CALL
endif

# Configuration specific to BLE
ifeq ($(BLE_SUPPORT),1)
DEFINES+=BLE_SUPPORTED
COMPONENTS+=WICED_BLE
endif

# Configuration specific to aws_tests project
ifeq ($(CY_AFR_BUILD), aws_tests)
DEFINES+=AMAZON_FREERTOS_ENABLE_UNIT_TESTS IOT_BUILD_TESTS=1 UNITY_INCLUDE_CONFIG_H
CY_AFR_IS_TESTING=1
else
CY_AFR_IS_TESTING=0
endif

# Resolve toolchain name and add linker flag for Freertos OpenOCD debug
ifeq ($(TOOLCHAIN),GCC_ARM)

CY_AFR_TOOLCHAIN=GCC
CY_AFR_TOOLCHAIN_LS_EXT=ld
LDFLAGS+=-Wl,--undefined=uxTopUsedPriority

else ifeq ($(TOOLCHAIN),IAR)

CY_AFR_TOOLCHAIN=$(TOOLCHAIN)
CY_AFR_TOOLCHAIN_LS_EXT=icf
LDFLAGS+=--keep uxTopUsedPriority

else ifeq ($(TOOLCHAIN),ARM)

CY_AFR_TOOLCHAIN=GCC
CY_AFR_TOOLCHAIN_LS_EXT=sct
LDFLAGS+=--undefined=uxTopUsedPriority

endif #GCC_ARM

ifeq ($(OTA_SUPPORT),1)

# Additional / custom linker flags.
ifeq ($(TOOLCHAIN),GCC_ARM)
LDFLAGS+="-Wl,--defsym,MCUBOOT_HEADER_SIZE=$(MCUBOOT_HEADER_SIZE),--defsym,MCUBOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE),--defsym,CY_BOOT_PRIMARY_1_SIZE=$(CY_BOOT_PRIMARY_1_SIZE)"
else
ifeq ($(TOOLCHAIN),IAR)
    LDFLAGS+=--config_def MCUBOOT_HEADER_SIZE=$(MCUBOOT_HEADER_SIZE) --config_def MCUBOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE) --config_def CY_BOOT_PRIMARY_1_SIZE=$(CY_BOOT_PRIMARY_1_SIZE)
else
ifeq ($(TOOLCHAIN),ARM)
LDFLAGS+=--pd=-DMCUBOOT_HEADER_SIZE=$(MCUBOOT_HEADER_SIZE) --pd=-DMCUBOOT_BOOTLOADER_SIZE=$(MCUBOOT_BOOTLOADER_SIZE) --pd=-DCY_BOOT_PRIMARY_1_SIZE=$(CY_BOOT_PRIMARY_1_SIZE)
else
LDFLAGS+=
endif #ARM
endif #IAR
endif #GCC_ARM

# OTA specific linker scripts
LINKER_SCRIPT=$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/ota/*_ota_int.$(CY_AFR_TOOLCHAIN_LS_EXT))

else

# Explicitly set the linker script
LINKER_SCRIPT=$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.$(CY_AFR_TOOLCHAIN_LS_EXT))

endif #OTA_SUPPORT

ifeq ($(TOOLCHAIN),IAR)
CFLAGS+=--dlib_config=full
CXXFLAGS+=--dlib_config=full --guard_calls
LDFLAGS+=--threaded_lib
DEFINES+=LWIP_ERRNO_INCLUDE=\"cy_lwip_errno_iar.h\"
SOURCES+=$(CY_AFR_ROOT)/freertos_kernel/portable/IAR/ARM_CM4F/portasm.s
endif

ifeq ($(TOOLCHAIN),ARM)
DEFINES+=LWIP_ERRNO_INCLUDE=\"cy_lwip_errno_armcc.h\"
# As of ARM Compiler 6.12, <assert.h> does not define static_assert (ISO/IEC 9899:2011).
# This define may interfere with <cassert> or future versions of the ARM C library.
DEFINES+=static_assert=_Static_assert
endif