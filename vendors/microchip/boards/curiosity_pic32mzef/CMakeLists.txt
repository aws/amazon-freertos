# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
if("${AFR_BOARD_NAME}" STREQUAL "curiosity_pic32mzef")
    include("curiosity_pic32mzef.cmake")
endif()

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)

# Compiler flags
set(c_flags -g -x c -ffunction-sections -O1 -mprocessor=32MZ2048EFM100 -mnewlib-libc -std=gnu99 -fgnu89-inline)
set(asm_flags -mprocessor=32MZ2048EFM100 -MMD -MF  -Wa$<COMMA>--gdwarf-2)
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
    $<IF:$<COMPILE_LANGUAGE:C>,${c_flags},${asm_flags}>
)


# Linker flags
set(linker_flags -Wl,--script=\"${AFR_ROOT_DIR}/vendors/microchip/boards/curiosity_pic32mzef/aws_demos/common/application_code/microchip_code/app_mz.ld\",--defsym=_min_heap_size=170000,--defsym=_min_stack_size=10000,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,--memorysummary,memoryfile.xml,--allow-multiple-definition)

target_link_options(
    AFR::compiler::mcu_port
    INTERFACE -mprocessor=32MZ2048EFM100 -mnewlib-libc -DXPRJ_pic32mz_ef_curiosity=pic32mz_ef_curiosity -no-legacy-libc ${linker_flags}  
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests/common")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TEST_DIR}/include")
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
endif()

# Kernel
afr_mcu_port(kernel)
afr_glob_src(driver_src RECURSE DIRECTORY "${AFR_ROOT_DIR}/lib/third_party/mcu_vendor/microchip/harmony/v2.05/framework")
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ/port.c"
        "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ/port_asm.S"
        "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ/portmacro.h"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
)

set(
    kernel_inc_dirs
    "${AFR_MODULES_STANDARD_DIR}/common/include"
    "${AFR_ROOT_DIR}/modules/libraries/3rdparty/jsmn"
    "${AFR_ROOT_DIR}/modules/libraries/3rdparty/pkcs11"
    "${AFR_ROOT_DIR}/modules/libraries/3rdparty/tinycbor"
    "${AFR_MODULES_STANDARD_DIR}/crypto/include"
    "${AFR_MODULES_STANDARD_DIR}/tls/include"
    "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/include"
    "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/Compiler/GCC"
    "${AFR_ROOT_DIR}/modules/ports/wifi/include"
    "${AFR_ROOT_DIR}/modules/ports/pkcs11/include"
    "${board_dir}/application_code/microchip_code"
    "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/bsp"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        ${kernel_inc_dirs}
        "${aws_credentials_include}"
        "${board_dir}/config_files"
)


# WiFi
afr_mcu_port(wifi)
target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
    AFR::freertos_plus_tcp
)


target_sources(
    AFR::wifi::mcu_port
    INTERFACE
        "${afr_ports_dir}/wifi/aws_wifi.c"
        "${afr_ports_dir}/wifi/aws_wifi_assert.c"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/system/command/src/sys_command.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/bsp/bsp.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/ethmac/src/dynamic/drv_ethmac.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/ethmac/src/dynamic/drv_ethmac_lib.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/ethphy/src/dynamic/drv_ethphy.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/flash/src/drv_flash_static.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/miim/src/dynamic/drv_miim.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/spi/dynamic/drv_spi_api.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/spi/dynamic/drv_spi_master_dma_tasks.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/spi/dynamic/drv_spi_master_rm_tasks.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/spi/dynamic/drv_spi_tasks.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/spi/src/dynamic/drv_spi.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/spi/src/dynamic/drv_spi_api.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/spi/src/drv_spi_sys_queue_fifo.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/tmr/src/dynamic/drv_tmr.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/usart/src/dynamic/drv_usart.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/usart/src/dynamic/drv_usart_buffer_queue.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/usart/src/dynamic/drv_usart_buffer_queue_dma.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/usart/src/dynamic/drv_usart_byte_model.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/usart/src/dynamic/drv_usart_dma.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/usart/src/dynamic/drv_usart_read_write.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/dev/console/wdrv_wilc1000_console.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/dev/gpio/wdrv_wilc1000_eint.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/dev/gpio/wdrv_wilc1000_gpio.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/dev/spi/wdrv_wilc1000_spi.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/dev/timer/wdrv_wilc1000_timer.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/osal/wdrv_wilc1000_osal.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_cli.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_config.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_connmgr.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_events.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_iwpriv.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_main.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_scan_helper.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/common/source/nm_common.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/m2m_hif.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/m2m_periph.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/m2m_wifi.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/nmasic.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/nmbus.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/nmdrv.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/nmspi.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/spi_flash/source/spi_flash.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/wdrvext_wilc1000.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/wilc1000_fw_update.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/wilc1000_task.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/osal/src/osal.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/osal/src/osal_freertos.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/peripheral/tmr/src/plib_tmr_pic32.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/clk/src/sys_clk_pic32mz.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/command/src/sys_command.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/common/src/sys_buffer.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/common/src/sys_queue.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/console/src/sys_console.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/console/src/sys_console_uart.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/debug/src/sys_debug.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/devcon/src/sys_devcon.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/devcon/src/sys_devcon_pic32mz.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/devcon/src/sys_devcon_cache_pic32mz.S"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/dma/src/sys_dma.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/int/src/sys_int_pic32.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/ports/src/sys_ports.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/ports/src/sys_ports_static.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/random/src/sys_random.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/reset/src/sys_reset.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/system/tmr/src/sys_tmr.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/tcpip/src/tcpip_heap_alloc.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/tcpip/src/tcpip_heap_external.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/tcpip/src/tcpip_helpers.c"
		"${AFR_ROOT_DIR}/vendors//microchip/harmony/v2.05/framework/tcpip/src/tcpip_packet.c"
)

target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
        "${AFR_ROOT_DIR}/modules/ports/secure_sockets/include"
        "${AFR_MODULES_STANDARD_DIR}/common/include/private"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/system/common"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/include"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/common/include"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/include"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/include"
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/aws_pkcs11_pal.c"
        "${afr_ports_dir}/pkcs11/pkcs11_nvm.c"
)


# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/pic32mzef/BufferAllocation_2.c"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/pic32mzef/NetworkInterface_wifi.c"
        "${AFR_ROOT_DIR}/modules/ports/pkcs11/mbedtls/aws_pkcs11_mbedtls.c"
)

target_include_directories(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension"
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/include"  
        "${AFR_ROOT_DIR}/modules/libraries/3rdparty/mbedtls/include"
        "${AFR_ROOT_DIR}/modules/ports/pkcs11/mbedtls"
)   

# Secure sockets
afr_mcu_port(secure_sockets)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${AFR_ROOT_DIR}/modules/ports/secure_sockets/freertos_plus_tcp/aws_secure_sockets.c"
)

target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE
        AFR::secure_sockets_freertos_tcp
        AFR::tls
)


# OTA
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE "${afr_ports_dir}/ota/aws_ota_pal.c"
    INTERFACE "${afr_ports_dir}/ota/aws_nvm.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
    set(extra_exe_sources "${AFR_TESTS_DIR}/src/iot_tests_network.c")
else()
    set(exe_target aws_demos)
    set(
        extra_exe_sources
        ${NETWORK_MANAGER_SOURCES}
    )
endif()

afr_glob_src(microchip_code RECURSE DIRECTORY "${AFR_ROOT_DIR}/vendors/microchip/boards/curiosity_pic32mzef/aws_demos/common/application_code/microchip_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")
add_executable(
    ${exe_target}
    ${config_files}
    "${board_dir}/application_code/main.c"
    ${microchip_code}
    ${extra_exe_sources}
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/bin/framework/peripheral/PIC32MZ2048EFM100_peripherals.a"
)

target_include_directories(
    ${exe_target}
    PUBLIC
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/system/common"
)


if(AFR_NON_BUILD_MODE)
    return()
endif()


# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")


set_source_files_properties(${AFR_MODULES_DIR}/greengrass/aws_greengrass_discovery.c
    ${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_single_task.c
    ${AFR_DEMOS_DIR}/secure_sockets/aws_test_tcp.c
    ${AFR_DEMOS_DIR}/wifi/aws_test_wifi.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format"
)

set_source_files_properties(${AFR_DEMOS_DIR}/logging/aws_logging_task_dynamic_buffers.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format -Wno-uninitialized"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_pal.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign -Wno-sizeof-pointer-memaccess"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_agent.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign"
)

set_source_files_properties(${AFR_DEMOS_DIR}/posix/aws_test_posix_pthread.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-int-conversion"
)

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")


set(xc32_bin2hex ${XC32_BIN}/xc32-bin2hex)
set(xc32_objcopy ${XC32_BIN}/xc32-objcopy)
set(output_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.hex")

# These locations have to be generalized through the toolchain
# We may have to re-build the bootloader hex file?
set(hexmate_dir /Applications/microchip/xc8/v2.05/pic/bin) 
set(bl_hex_file ${AFR_ROOT_DIR}/vendors/microchip/boards/curiosity_pic32mzef/bootloader/aws_bootloader.X/dist/pic32mz_ef_curiosity/production/aws_bootloader.X.production.hex)

add_custom_command(
    TARGET ${exe_target} POST_BUILD
    COMMAND "echo" "Running Post-build step" 
    COMMAND "${xc32_bin2hex}" "$<TARGET_FILE:${exe_target}>"
    COMMAND "echo" "Running xc32-objcopy" 
    COMMAND "echo" ${XC32_BIN}/xc32_objcopy
    COMMAND "${xc32_objcopy}" -I ihex $<TARGET_FILE_DIR:${exe_target}>/${exe_target}.hex -O binary $<TARGET_FILE_DIR:${exe_target}>/${exe_target}.intermediate.bin
    COMMAND "echo" "Creating binary image"
    COMMAND "python" ${AFR_DEMOS_DIR}/ota/bootloader/utility/ota_image_generator.py -b $<TARGET_FILE_DIR:${exe_target}>/${exe_target}.intermediate.bin -p MCHP-Curiosity-PIC32MZEF
    COMMAND "echo" "Running Hexmate to combine bootloader with image"
#    COMMAND "${hexmate_dir}/hexmate" $<TARGET_FILE_DIR:${exe_target}>/${exe_target}.hex  ${bl_hex_file} -O${exe_target}_image.hex
)
