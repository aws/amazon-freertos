
mtk_sdk_mk: all

##############################################################################

MTK_C_FLAGS        += -DMTK_HAL_LOWPOWER_ENABLE

##############################################################################

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/chip/inc
MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/chip/mt7687/inc

MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_adc.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_aes.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_cache.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_cache_internal.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_des.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_dfs.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_eint.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_flash.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_gdma.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_gpio.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_gpt.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_i2c_master.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_i2s.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_irtx.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_log.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_md5.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_misc.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_nvic.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_nvic_internal.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_pwm.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_rtc.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_sha.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_spi_master.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_spi_slave.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_uart.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_wdt.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/hal_file.c

##############################################################################

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/chip/mt7687/src/common/include

MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/top.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/cos_api.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/dma.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/flash_sfc.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/gpt.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_gdma_internal.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_gpc.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_gpio_7687.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_I2C.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_i2s_internal.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_irrx.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_lp.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_pcnt.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_pdma_internal.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_pinmux.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_spim.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_sys.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_trng.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/hal_uid.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/i2c.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/ir_tx.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/low_hal_gpio.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/low_hal_irrx.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/low_hal_irtx.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/low_hal_pwm.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/low_hal_rtc.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/low_hal_wdt.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/nvic.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/pinmux.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/pwm.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/sfc.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/spi_flash.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/spim.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/timer.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/top.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/common/uart.c

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/chip/mt7687/src/sdio_gen3/include

MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/sdio_gen3/connsys_bus.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/sdio_gen3/connsys_driver.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/sdio_gen3/connsys_util.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/chip/mt7687/src/sdio_gen3/context_dump.c

##############################################################################

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/CMSIS/Include
MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/CMSIS/Device/MTK/mt7687/Include

MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/external_flash/src/bsp_flash_config.c
MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/board/mt76x7_hdk/external_flash/inc

MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src/wifi_init.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src/os.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src/wifi_os_mem.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src/wifi_os_api.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src/wifi_profile.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src/wifi_default_config.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src/misc.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src/get_profile_string.c

ifeq ($(wildcard $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src_protected/GCC/module.mk),)
MTK_LIBS           += -lwifi_CM4_GCC
else
include $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/src_protected/GCC/module.mk
MTK_C_FILES        += $(MTK_WIFI_C_FILES)
endif

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/board/mt76x7_hdk/wifi/inc

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/board/mt76x7_hdk/util/inc
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/util/src/io_def.c

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/driver/board/mt76x7_hdk/ept/inc
MTK_C_FILES        +=   $(MTK_SDK_DIR)/driver/board/mt76x7_hdk/ept/src/bsp_gpio_ept_config.c

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/kernel/service/inc
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/service/src/context_info_save.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/service/src/exception_handler.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/service/src/memory_regions.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/service/src/os_port_callback.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/service/src/os_trace_callback.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/service/src/toi.c

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/kernel/extend/inc
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/extend/src/wrap_heap.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/extend/src/GCC/port.c
MTK_C_FILES        +=   $(MTK_SDK_DIR)/kernel/extend/src/GCC/port_tick.c

MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/middleware/MTK/minisupp/inc
#MTK_C_FILES        +=   $(MTK_SDK_DIR)/middleware/MTK/minisupp/src/

#MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/middleware/third_party/lwip/ports/include
#MTK_C_FLAGS        += -I$(MTK_SDK_DIR)/middleware/third_party/lwip/src/include



##############################################################################
#
# DHCPD
#
##############################################################################

_DHCPD_DIR_         =   $(MTK_SDK_DIR)/middleware/MTK/dhcpd

MTK_C_FILES        +=   $(_DHCPD_DIR_)/src/dhcpd.c

MTK_C_FLAGS        += -I$(_DHCPD_DIR_)/inc

##############################################################################
#
# PING - Depends on LwIP
#
##############################################################################

_PING_DIR_          =   $(MTK_SDK_DIR)/middleware/third_party/ping

MTK_C_FILES        +=   $(_PING_DIR_)/src/ping.c

MTK_C_FLAGS        += -I$(_PING_DIR_)/inc

##############################################################################
#
# MiniCLI
#
##############################################################################

_CLI_DIR_           =   $(MTK_SDK_DIR)/middleware/MTK/minicli

MTK_C_FILES        +=   $(_CLI_DIR_)/src/minicli_api.c

MTK_C_FLAGS        += -I$(_CLI_DIR_)/inc

ifeq ($(MTK_MINICLI_ENABLE),y)
MTK_C_FLAGS        += -DMTK_MINICLI_ENABLE
endif

##############################################################################
#
# WPA Suppplicant
#
##############################################################################

ifeq ($(MTK_MINISUPP_ENABLE),y)

ifeq ($(wildcard $(MTK_SDK_DIR)/middleware/MTK/minisupp/src_protected/GCC/module.mk),)
MTK_LIBS           += -lminisupp_CM4_GCC
else
include $(MTK_SDK_DIR)/middleware/MTK/minisupp/src_protected/GCC/module.mk
MTK_C_FILES        += $(MTK_MINISUPP_C_FILES)
endif

MTK_C_FLAGS        += -DMTK_MINISUPP_ENABLE

MTK_C_FLAGS        += -Imiddleware/MTK/minisupp/inc
MTK_C_FLAGS        += -Imiddleware/MTK/minisupp/src/wpa_supplicant_8.jb4_1/src
MTK_C_FLAGS        += -Imiddleware/MTK/minisupp/src/wpa_supplicant_8.jb4_1/src/drivers
MTK_C_FLAGS        += -Imiddleware/MTK/minisupp/src/wpa_supplicant_8.jb4_1/src/utils

endif

##############################################################################

MTK_LIBS      += -L$(MTK_PREBUILT_DIR)/gcc
MTK_LIBS      += -lhal_core_CM4_GCC
MTK_LIBS      += -lhal_protected_CM4_GCC
MTK_LIBS      += -lminicli_CM4_GCC

# Do not use kservice because it is not binary compatible with FreeRTOS V10.the
#MTK_LIBS      += -lkservice_CM4_MT7697_GCC
