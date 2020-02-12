set(mchp_dir "${AFR_VENDORS_DIR}/microchip")
set(harmony_dir "${mchp_dir}/harmony/v2.05")
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TESTS_DIR}/include")
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
endif()

# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
if("${AFR_BOARD_NAME}" STREQUAL "curiosity_pic32mzef")
    include("${CMAKE_CURRENT_LIST_DIR}/curiosity_pic32mzef.cmake")
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
set(linker_flags -Wl,--script=\"${board_dir}/application_code/microchip_code/app_mz.ld\",--defsym=_min_heap_size=170000,--defsym=_min_stack_size=10000,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,--memorysummary,memoryfile.xml,--allow-multiple-definition)

target_link_options(
    AFR::compiler::mcu_port
    INTERFACE -mprocessor=32MZ2048EFM100 -mnewlib-libc -DXPRJ_pic32mz_ef_curiosity=pic32mz_ef_curiosity -no-legacy-libc ${linker_flags}
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
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
    "${AFR_MODULES_C_SDK_DIR}/standard/common/include"
    "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/include"
    "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/tls/include"
    "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/include"
    "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/GCC"
    "${AFR_MODULES_ABSTRACTIONS_DIR}/wifi/include"
    "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
    "${board_dir}/application_code/microchip_code"
    "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ"
    "${harmony_dir}/framework"
    "${harmony_dir}/bsp"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        ${kernel_inc_dirs}
        "${aws_credentials_include}"
        "${board_dir}/config_files"
)
target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE
        3rdparty::jsmn
        3rdparty::pkcs11
        3rdparty::tinycbor
        # Require this for bootloader
        3rdparty::tinycrypt
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
        "${afr_ports_dir}/wifi/iot_wifi.c"
        "${afr_ports_dir}/wifi/aws_wifi_assert.c"
        "${harmony_dir}/framework/system/command/src/sys_command.c"
        "${harmony_dir}/bsp/bsp.c"
        "${harmony_dir}/framework/driver/ethmac/src/dynamic/drv_ethmac.c"
        "${harmony_dir}/framework/driver/ethmac/src/dynamic/drv_ethmac_lib.c"
        "${harmony_dir}/framework/driver/ethphy/src/dynamic/drv_ethphy.c"
        "${harmony_dir}/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.c"
        "${harmony_dir}/framework/driver/flash/src/drv_flash_static.c"
        "${harmony_dir}/framework/driver/miim/src/dynamic/drv_miim.c"
        "${harmony_dir}/framework/driver/spi/dynamic/drv_spi_api.c"
        "${harmony_dir}/framework/driver/spi/dynamic/drv_spi_master_dma_tasks.c"
        "${harmony_dir}/framework/driver/spi/dynamic/drv_spi_master_rm_tasks.c"
        "${harmony_dir}/framework/driver/spi/dynamic/drv_spi_tasks.c"
        "${harmony_dir}/framework/driver/spi/src/dynamic/drv_spi.c"
        "${harmony_dir}/framework/driver/spi/src/dynamic/drv_spi_api.c"
        "${harmony_dir}/framework/driver/spi/src/drv_spi_sys_queue_fifo.c"
        "${harmony_dir}/framework/driver/tmr/src/dynamic/drv_tmr.c"
        "${harmony_dir}/framework/driver/usart/src/dynamic/drv_usart.c"
        "${harmony_dir}/framework/driver/usart/src/dynamic/drv_usart_buffer_queue.c"
        "${harmony_dir}/framework/driver/usart/src/dynamic/drv_usart_buffer_queue_dma.c"
        "${harmony_dir}/framework/driver/usart/src/dynamic/drv_usart_byte_model.c"
        "${harmony_dir}/framework/driver/usart/src/dynamic/drv_usart_dma.c"
        "${harmony_dir}/framework/driver/usart/src/dynamic/drv_usart_read_write.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/dev/console/wdrv_wilc1000_console.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/dev/gpio/wdrv_wilc1000_eint.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/dev/gpio/wdrv_wilc1000_gpio.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/dev/spi/wdrv_wilc1000_spi.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/dev/timer/wdrv_wilc1000_timer.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/osal/wdrv_wilc1000_osal.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_cli.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_config.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_connmgr.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_events.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_iwpriv.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_main.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver/wdrv_wilc1000_scan_helper.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/common/source/nm_common.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/m2m_hif.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/m2m_periph.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/m2m_wifi.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/nmasic.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/nmbus.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/nmdrv.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source/nmspi.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/spi_flash/source/spi_flash.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/wdrvext_wilc1000.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/wilc1000_fw_update.c"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/wilc1000_task.c"
        "${harmony_dir}/framework/osal/src/osal.c"
        "${harmony_dir}/framework/osal/src/osal_freertos.c"
        "${harmony_dir}/framework/peripheral/tmr/src/plib_tmr_pic32.c"
        "${harmony_dir}/framework/system/clk/src/sys_clk_pic32mz.c"
        "${harmony_dir}/framework/system/command/src/sys_command.c"
        "${harmony_dir}/framework/system/common/src/sys_buffer.c"
        "${harmony_dir}/framework/system/common/src/sys_queue.c"
        "${harmony_dir}/framework/system/console/src/sys_console.c"
        "${harmony_dir}/framework/system/console/src/sys_console_uart.c"
        "${harmony_dir}/framework/system/debug/src/sys_debug.c"
        "${harmony_dir}/framework/system/devcon/src/sys_devcon.c"
        "${harmony_dir}/framework/system/devcon/src/sys_devcon_pic32mz.c"
        "${harmony_dir}/framework/system/devcon/src/sys_devcon_cache_pic32mz.S"
        "${harmony_dir}/framework/system/dma/src/sys_dma.c"
        "${harmony_dir}/framework/system/int/src/sys_int_pic32.c"
        "${harmony_dir}/framework/system/ports/src/sys_ports.c"
        "${harmony_dir}/framework/system/ports/src/sys_ports_static.c"
        "${harmony_dir}/framework/system/random/src/sys_random.c"
        "${harmony_dir}/framework/system/reset/src/sys_reset.c"
        "${harmony_dir}/framework/system/tmr/src/sys_tmr.c"
        "${harmony_dir}/framework/tcpip/src/tcpip_heap_alloc.c"
        "${harmony_dir}/framework/tcpip/src/tcpip_heap_external.c"
        "${harmony_dir}/framework/tcpip/src/tcpip_helpers.c"
        "${harmony_dir}/framework/tcpip/src/tcpip_packet.c"
)

target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
        "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
        "${harmony_dir}/framework/system/common"
        "${harmony_dir}/framework/driver/wifi/wilc1000/include"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/common/include"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/include"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver/include"
)

# PKCS11
afr_mcu_port(pkcs11_implementation DEPENDS AFR::pkcs11_mbedtls)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/iot_pkcs11_pal.c"
        "${afr_ports_dir}/pkcs11/pkcs11_nvm.c"
        "${afr_ports_dir}/pkcs11/pkcs11_nvm.h"
)

# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/pic32mzef/BufferAllocation_2.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/pic32mzef/NetworkInterface_wifi.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/pic32mzef/NetworkInterface_eth.c"
)

target_include_directories(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension"
        "${harmony_dir}/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/include"
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE AFR::secure_sockets_freertos_plus_tcp
)

# POSIX
afr_mcu_port(posix)
target_sources(
    AFR::posix::mcu_port
    INTERFACE "${afr_ports_dir}/posix/FreeRTOS_POSIX_portable.h"
)
target_include_directories(
    AFR::posix::mcu_port
    INTERFACE "${afr_ports_dir}/posix"
)
target_link_libraries(
    AFR::posix::mcu_port
    INTERFACE AFR::freertos_plus_posix
)


# OTA
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE "${afr_ports_dir}/ota/aws_ota_pal.c"
    INTERFACE "${afr_ports_dir}/ota/aws_nvm.c"
    INTERFACE "${afr_ports_dir}/ota/aws_nvm.h"
)
target_link_libraries(
    AFR::ota::mcu_port
    INTERFACE
        AFR::pkcs11
        AFR::ota_mqtt
        AFR::ota_http
)

# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------
set_source_files_properties(${AFR_MODULES_C_SDK_DIR}/aws/greengrass/aws_greengrass_discovery.c
    ${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_single_task.c
    ${AFR_DEMOS_DIR}/secure_sockets/iot_test_tcp.c
    ${AFR_DEMOS_DIR}/wifi/iot_test_wifi.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format"
)

set_source_files_properties(${AFR_MODULES_C_SDK_DIR}/common/logging/iot_logging_task_dynamic_buffers.c
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

set_source_files_properties(${AFR_DEMOS_DIR}/posix/iot_test_posix_pthread.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-int-conversion"
)

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

afr_glob_src(microchip_code RECURSE DIRECTORY "${board_dir}/application_code/microchip_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
    set(extra_exe_sources "${AFR_TESTS_DIR}/common/iot_tests_network.c")
else()
    set(exe_target aws_demos)
endif()

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(
        ${exe_target}
        ${config_files}
        "${board_dir}/application_code/main.c"
        ${microchip_code}
        ${extra_exe_sources}
        "${board_dir}/application_code/microchip_code/app_mz.ld"
    )

    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::wifi
            AFR::utils
            "${harmony_dir}/bin/framework/peripheral/PIC32MZ2048EFM100_peripherals.a"
    )

    target_include_directories(
        ${exe_target}
        PUBLIC
            "${harmony_dir}/framework/system/common"
    )

    if(AFR_METADATA_MODE)
        return()
    endif()

    find_program(xc32_bin2hex xc32-bin2hex PATHS "${AFR_COMPILER_DIR}")
    find_program(xc32_objcopy xc32-objcopy PATHS "${AFR_COMPILER_DIR}")

    # Locate microchip hexmate.
    set(MCHP_HEXMATE_PATH "" CACHE STRING "Path to microchip hexmate, usually this should be mplab's path")
    find_program(hexmate_path hexmate PATHS "${MCHP_HEXMATE_PATH}" PATH_SUFFIXES bin)
    if (NOT AFR_METADATA_MODE)
        if(NOT hexmate_path)
            message(FATAL_ERROR "Cannot find Microchip's hexmate tool. Set MCHP_HEXMATE_PATH to the hexmate tool that comes with MPLAB. Try MPLABX\{VERSION}\mplab_platform\bin or /Applications/microchip/mplabx/{VERSION}/mplab_ide.app/Contents/Resources/mplab_ide/bin.")
        endif()
    endif()

    # These locations have to be generalized through the toolchain
    # We may have to re-build the bootloader hex file?
    set(bl_hex_file ${CMAKE_CURRENT_LIST_DIR}/bootloader/aws_bootloader.X.production.hex)

    set(output_hex_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.hex")
    set(output_bin_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.production.bin")
    add_custom_command(
        TARGET ${exe_target} POST_BUILD
        COMMAND "echo" "Running Post-build step"
        COMMAND "${xc32_bin2hex}" "$<TARGET_FILE:${exe_target}>"
        COMMAND "echo" "Running Hexmate to combine bootloader with image"
        COMMAND "${hexmate_path}" ${output_hex_file}  ${bl_hex_file} -O${exe_target}.production.unified.hex
    )
    if(NOT AFR_IS_TESTING)
        set(ota_image_generator "${CMAKE_CURRENT_LIST_DIR}/bootloader/bootloader/utility/ota_image_generator.py")
        add_custom_command(
            TARGET ${exe_target} POST_BUILD
            COMMAND "echo" "Running xc32-objcopy"
            COMMAND "${xc32_objcopy}" -I ihex ${output_hex_file} -O binary "${output_bin_file}"
            COMMAND "echo" "Creating binary image"
            COMMAND "python" "${ota_image_generator}" -b "${output_bin_file}" -p MCHP-Curiosity-PIC32MZEF
        )
    endif()
endif()
