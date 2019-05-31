set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
set(demos_uvision_dir ${CMAKE_CURRENT_LIST_DIR}/aws_demos/uvision/)
set(tests_uvision_dir ${CMAKE_CURRENT_LIST_DIR}/aws_tests/uvision/)

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(board_uvision_dir "${demos_uvision_dir}")
    set(exe_target aws_tests)
else()
    set(board_dir "${board_demos_dir}")
    set(board_uvision_dir "${tests_uvision_dir}")
    set(exe_target aws_demos)
endif()

#Ensure Defender is not included in build
set(AFR_MODULE_defender 0 CACHE INTERNAL "")

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "MT7697Hx-Dev-Kit")
afr_set_board_metadata(DISPLAY_NAME "MT7697Hx-Dev-Kit")
afr_set_board_metadata(DESCRIPTION "Development kit for USI MT7697Hx")
afr_set_board_metadata(VENDOR_NAME "MediaTek")
afr_set_board_metadata(FAMILY_NAME "MT7697Hx")
afr_set_board_metadata(DATA_RAM_MEMORY "352KB")
afr_set_board_metadata(PROGRAM_MEMORY "4MB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "uVision")
afr_set_board_metadata(RECOMMENDED_IDE "uVision")
afr_set_board_metadata(IDE_uVision_NAME "Keil uVision")
afr_set_board_metadata(IDE_uVision_COMPILER "ARMCC")
afr_set_board_metadata(IS_ACTIVE "TRUE")

afr_set_board_metadata(IDE_uVision_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/mediatek/mt7697hx-dev-kit/uvision")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
    -D__MICROLIB
    -D_REENT_SMALL
    -DPRODUCT_VERSION=7687
    -DMTK_BLE_SMTCN_ENABLE
    -DMTK_BT_LWIP_ENABLE
    -DMTK_BLE_BQB_TEST_ENABLE
    -DMTK_BLE_CLI_ENABLE
    -DMTK_BLE_BQB_CLI_ENABLE
    -DMTK_HCI_CONSOLE_MIX_ENABLE
    -DMTK_MINISUPP_ENABLE
    -DMTK_BSPEXT_ENABLE
    -DMTK_HAL_LOWPOWER_ENABLE
    -DMTK_LWIP_CLI_ENABLE
    -DMTK_HTTPCLIENT_SSL_ENABLE
    -DMTK_IPERF_ENABLE
    -DMTK_SMTCN_ENABLE
    -DMTK_CLI_TEST_MODE_ENABLE
    -DMTK_WIFI_REPEATER_ENABLE
    -DCONFIG_REPEATER
    -DMTK_DEBUG_LEVEL_NONE
    -DMTK_WIFI_PRIVILEGE_ENABLE
    -DSUPPORT_MBEDTLS
    -D__MTK_BLE_ONLY_ENABLE__
    -DMTK_SINGLE_SKU_SUPPORT
    -DHAL_DFS_MODULE_ENABLED
    -DLWIP_NO_STDINT_H=1
    -DLWIP_TIMEVAL_PRIVATE=1
)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:ASM>:${assembler_defined_symbols}>
)

set (compiler_flags --c99 -c --cpu Cortex-M4.fp  -g -O0 --apcs=interwork --split_sections )
set (assembler_flags --cpu Cortex-M4.fp -g --apcs=interwork --pd "__MICROLIB SETA 1" --predefine "__UVISION_VERSION SETA 527")

target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${compiler_flags}>
)
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:ASM>:${assembler_flags}>
)

# Linker flags
set(linker_flags --cpu Cortex-M4.fp --library_type=microlib --diag_suppress 6340,6329,6314 --strict --scatter "${AFR_ROOT_DIR}/projects/mediatek/mt7697hx-dev-kit/uvision/aws_tests/flash.sct" --entry Reset_Handler --first __isr_vector --summary_stderr --info summarysizes,sizes,totals,unused,veneers --map --xref --callgraph --symbols )

target_link_options(
    AFR::compiler::mcu_port
    INTERFACE ${linker_flags}
)

target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE ${link_dependent_libs}
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Normally the portable layer for kernel should be vendor's driver code.
afr_mcu_port(kernel)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        # Mediatek SDK sources (driver)
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/ept/src/bsp_gpio_ept_config.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_lp.c"

        "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/extend/src/wrap_heap.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/extend/src/RVDS/port.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/extend/src/RVDS/port_tick.c"

        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_adc.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_aes.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_cache.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_cache_internal.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_des.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_dfs.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_eint.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_file.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_flash.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_gdma.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_gpio.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_gpt.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_i2c_master.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_i2s.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_irtx.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_log.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_md5.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_misc.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_nvic.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_nvic_internal.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_pwm.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_rtc.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_sha.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_spi_master.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_spi_slave.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_uart.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/hal_wdt.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/cos_api.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/dma.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/flash_sfc.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/gpt.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_gdma_internal.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_gpc.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_gpio_7687.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_I2C.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_i2s_internal.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_irrx.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_lp.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_pcnt.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_pdma_internal.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_pinmux.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_spim.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_sys.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_trng.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/hal_uid.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/i2c.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/ir_tx.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/low_hal_gpio.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/low_hal_irrx.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/low_hal_irtx.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/low_hal_pwm.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/low_hal_rtc.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/low_hal_wdt.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/nvic.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/pinmux.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/pwm.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/sfc.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/spi_flash.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/spim.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/timer.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/top.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/uart.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/sdio_gen3/connsys_bus.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/sdio_gen3/connsys_driver.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/sdio_gen3/connsys_util.c"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/sdio_gen3/context_dump.c"

        # Tracelyzer recorder files
        $<IF:${AFR_IS_TESTING}, "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/trcKernelPort.c" , >
        $<IF:${AFR_IS_TESTING}, "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/trcSnapshotRecorder.c" , >
        $<IF:${AFR_IS_TESTING}, "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/trcStreamingRecorder.c" , >
        ${compiler_specific_src}
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/RVDS/ARM_CM4F"
        "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/service/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/CMSIS/Device/MTK/mt7687/Include"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/CMSIS/Include"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/src/common/include"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/ept/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/util/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/external_flash/inc"
        "${board_dir}/config_files"
        "${board_dir}/application_code/mediatek_code"
        "${board_dir}/application_code/mediatek_code/include"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include"

        "${AFR_3RDPARTY_DIR}/lwip/src/portable/mediatek/include"
        "${AFR_3RDPARTY_DIR}/lwip/src/include"
        "${AFR_3RDPARTY_DIR}/lwip/src/portable"
        "${AFR_3RDPARTY_DIR}/pkcs11"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/utils/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/provisioning/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
        "${board_dir}/config_files"
        "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/Include"
        ${compiler_specific_include}
)


# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${afr_ports_dir}/wifi/aws_wifi.c"

    # LWIP files
    "${AFR_3RDPARTY_DIR}/lwip/src/portable/mediatek/netif/ethernetif.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/portable/arch/sys_arch.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/portable/mediatek/lwip_log.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/api/api_lib.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/api/api_msg.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/api/err.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/api/netbuf.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/api/netdb.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/api/netifapi.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/api/sockets.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/api/tcpip.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/def.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/dns.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/inet_chksum.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/init.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/mem.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/memp.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/netif.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/raw.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/stats.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/sys.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/tcp.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/tcp_in.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/timeouts.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/udp.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/pbuf.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/tcp_out.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ip.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv4/autoip.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv4/dhcp.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv4/icmp.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv4/igmp.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv4/ip4.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv4/ip4_addr.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv4/ip4_frag.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv4/etharp.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/dhcp6.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/ethip6.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/icmp6.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/inet6.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/ip6.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/ip6_addr.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/ip6_frag.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/mld6.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/core/ipv6/nd6.c"
    "${AFR_3RDPARTY_DIR}/lwip/src/netif/ethernet.c"

    # MediaTek SDK Wifi Files
    "${AFR_VENDORS_DIR}/mediatek/sdk/middleware/MTK/dhcpd/src/dhcpd.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/middleware/third_party/ping/src/ping.c"

    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/util/src/io_def.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/src/get_profile_string.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/src/misc.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/src/os.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/src/wifi_default_config.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/src/wifi_init.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/src/wifi_os_api.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/src/wifi_profile.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/src/wifi_os_mem.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/ept/src/bsp_gpio_ept_config.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/external_flash/src/bsp_flash_config.c"

)

target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/wifi/inc/"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/board/mt76x7_hdk/util/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/middleware/MTK/minisupp/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/middleware/third_party/ping/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/middleware/MTK/dhcpd/inc"
        "${AFR_3RDPARTY_DIR}/lwip/src/portable"
        "${AFR_3RDPARTY_DIR}/lwip/src/include/lwip"
        "${board_dir}/application_code/mediatek_code/include"
)

# WiFi on MTK requires Secure Sockets
target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
        AFR::secure_sockets::mcu_port
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/aws_pkcs11_pal.c"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/mbedtls/aws_pkcs11_mbedtls.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/src/aws_crypto.c"
)
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE 3rdparty::mbedtls
)

target_include_directories(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/include/"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/mt7687/inc"
        "${AFR_VENDORS_DIR}/mediatek/sdk/driver/chip/inc"
        "${board_dir}/application_code/mediatek_code/include"
)

#Secure sockets
afr_mcu_port(secure_sockets)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE
        "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/lwip/aws_secure_sockets.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/tls/src/aws_tls.c"
)
target_include_directories(
    AFR::secure_sockets::mcu_port
    INTERFACE
       "${AFR_3RDPARTY_DIR}/lwip/src/include/lwip"
       "${AFR_3RDPARTY_DIR}/lwip/src/portable"
       "${AFR_MODULES_ABSTRACTIONS_DIR}/wifi/include"
       "${AFR_MODULES_C_SDK_DIR}/standard/common/include"
       "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/tls/include"
       "${AFR_3RDPARTY_DIR}/mbedtls/include"
       "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/include"
       "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/mbedtls"
)

 afr_module_dependencies(
    AFR::secure_sockets::mcu_port
     INTERFACE
         AFR::tls
 )




# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".axf")

# TODO, remove network manager src.
afr_glob_src(network_manager_src DIRECTORY "${AFR_DEMOS_DIR}/network_manager")
afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/mediatek_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

add_executable(
    ${exe_target}
    ${board_code_src}
    ${config_files}
    $<IF:${AFR_IS_TESTING}, ,${network_manager_src}>
    "${AFR_ROOT_DIR}/projects/mediatek/mt7697hx-dev-kit/uvision/aws_tests/startup_mt7687.s"
    "${board_dir}/application_code/main.c"
    "${board_dir}/application_code/lwipopts.c"
    "${board_dir}/application_code/mediatek_code/source/sys_init.c"
    "${board_dir}/application_code/mediatek_code/source/system_mt7687.c"
    "${board_dir}/application_code/mediatek_code/source/ept_gpio_var.c"
    "${board_dir}/application_code/mediatek_code/source/ept_eint_var.c"

    "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/service/src/exception_handler.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/service/src/toi.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/service/src/os_trace_callback.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/service/src/os_port_callback.c"
    "${AFR_VENDORS_DIR}/mediatek/sdk/kernel/service/src/memory_regions.c"
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi::mcu_port
        AFR::utils
        "${AFR_VENDORS_DIR}/mediatek/sdk/prebuilt/keil/libwifi_CM4_Keil.lib"
        "${AFR_VENDORS_DIR}/mediatek/sdk/prebuilt/keil/libhal_core_CM4_Keil.lib"
        "${AFR_VENDORS_DIR}/mediatek/sdk/prebuilt/keil/libhal_protected_CM4_Keil.lib"
        "${AFR_VENDORS_DIR}/mediatek/sdk/prebuilt/keil/libkservice_CM4_MT7687_Keil.lib"
        "${AFR_VENDORS_DIR}/mediatek/sdk/prebuilt/keil/libminicli_CM4_Keil.lib"
        "${AFR_VENDORS_DIR}/mediatek/sdk/prebuilt/keil/libminisupp_CM4_Keil.lib"
        ${link_extra_flags}
        AFR::crypto
        AFR::pkcs11
)

add_custom_command(
    TARGET ${exe_target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}"
)
