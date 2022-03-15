set(stm32l475_dir "${AFR_VENDORS_DIR}/st/stm32l475_discovery")
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
else()
    set(board_dir "${board_demos_dir}")
endif()

# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "STM32-B-L475E-IOT01A")
afr_set_board_metadata(DISPLAY_NAME "STM32L4 Discovery kit IoT node")
afr_set_board_metadata(DESCRIPTION "B-L475E-IOT01A Discovery kit with low-power RF, Wi-Fi, multiway sensing")
afr_set_board_metadata(VENDOR_NAME "STMicroelectronics")
afr_set_board_metadata(FAMILY_NAME "STM32L4")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(DATA_RAM_MEMORY "128KB")
afr_set_board_metadata(PROGRAM_MEMORY "1MB")
afr_set_board_metadata(CODE_SIGNER "null")
afr_set_board_metadata(SUPPORTED_IDE "SystemWorkbench")
afr_set_board_metadata(RECOMMENDED_IDE "SystemWorkbench")
afr_set_board_metadata(IDE_SystemWorkbench_NAME "System Workbench for STM32")
afr_set_board_metadata(IDE_SystemWorkbench_COMPILER "GCC")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")

afr_set_board_metadata(IDE_SystemWorkbench_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/st/stm32l475_discovery/ac6/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)
set(
    defined_symbols
    USE_HAL_DRIVER
    USE_OFFLOAD_SSL
    STM32L475xx
    MQTTCLIENT_PLATFORM_HEADER=MQTTCMSIS.h
    ENABLE_IOT_INFO
    ENABLE_IOT_ERROR
    SENSOR
    RFU
)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${defined_symbols}>
)

set(common_flags "-mcpu=cortex-m4" "-mthumb" "-mfloat-abi=hard" "-mfpu=fpv4-sp-d16")
set(c_flags "-Wall" "-fmessage-length=0" "-ffunction-sections")
set(linker_flags "-specs=nosys.specs" "-Wl,-Map=output.map,--gc-sections,-zmuldefs" "-lm")

# Compiler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        ${common_flags}
        $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${c_flags}>
)

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE ${common_flags} ${linker_flags}
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Kernel
afr_mcu_port(kernel)
afr_glob_src(driver_src DIRECTORY "${stm32l475_dir}" RECURSE)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        ${driver_src}
        "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c"
        "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/portmacro.h"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_5.c"
)
set(
    kernel_inc_dirs
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F"
    "${stm32l475_dir}/BSP/B-L475E-IOT01"
    "${stm32l475_dir}/BSP/Components/Common"
    "${stm32l475_dir}/BSP/Components/es_wifi"
    "${stm32l475_dir}/BSP/Components/hts221"
    "${stm32l475_dir}/BSP/Components/lis3mdl"
    "${stm32l475_dir}/BSP/Components/lps22hb"
    "${stm32l475_dir}/BSP/Components/lsm6dsl"
    "${stm32l475_dir}/BSP/Components/mx25r6435f"
    "${stm32l475_dir}/BSP/Components/vl53l0x"
    "${stm32l475_dir}/CMSIS/Include"
    "${stm32l475_dir}/CMSIS/Device/ST/STM32L4xx/Include"
    "${stm32l475_dir}/STM32L4xx_HAL_Driver/Inc"
    "${stm32l475_dir}/STM32L4xx_HAL_Driver/Inc/Legacy"
    "${board_dir}/application_code"
    "${board_dir}/application_code/st_code"
    "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
        "${board_dir}/config_files"
)

# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${afr_ports_dir}/wifi/iot_wifi.c"
)

# PKCS11
afr_mcu_port(pkcs11_implementation)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE "${afr_ports_dir}/pkcs11/core_pkcs11_pal.c"
)
# Link to AFR::pkcs11_mbedtls if you want to use default implementation based on mbedtls.
target_link_libraries(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        AFR::pkcs11_mbedtls
        AFR::wifi
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE
        AFR::tls
        AFR::wifi
)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${afr_ports_dir}/secure_sockets/iot_secure_sockets.c"
)

# Common I/O
afr_mcu_port(common_io)
target_sources(
    AFR::common_io::mcu_port
    INTERFACE 
        "${afr_ports_dir}/common_io/iot_uart.c"
        "${afr_ports_dir}/common_io/iot_i2c.c"
        "${afr_ports_dir}/common_io/iot_spi.c"
        # test code
        $<${AFR_IS_TESTING}:${afr_ports_dir}/common_io/iot_test_common_io_internal.c>
)

target_include_directories(
    AFR::common_io::mcu_port
    INTERFACE
        "${AFR_MODULES_ABSTRACTIONS_DIR}/platform/freertos/include"
        # test code
        $<${AFR_IS_TESTING}:${AFR_MODULES_ABSTRACTIONS_DIR}/common_io/test>
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/st_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(
        ${exe_target}
        ${board_code_src}
        ${config_files}
        "${board_dir}/application_code/main.c"
    )
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::wifi
            AFR::utils
            # TODO: common_io must explicitly linked because nothing else depends on it. 
            # Once this is a demo, this can be removed
            AFR::common_io
            -T"${board_dir}/STM32L475VGTx_FLASH.ld"
    )

    if(AFR_METADATA_MODE)
        return()
    endif()

    find_program(gcc_objcopy arm-none-eabi-objcopy)
    find_program(gcc_size arm-none-eabi-size)
    if(NOT gcc_objcopy)
        message(FATAL_ERROR "Cannot find arm-none-eabi-objcopy.")
    endif()

    set(output_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.hex")
    add_custom_command(
        TARGET ${exe_target} POST_BUILD
        COMMAND "${gcc_objcopy}" -O ihex "$<TARGET_FILE:${exe_target}>" "${output_file}"
        COMMAND "${gcc_size}" "$<TARGET_FILE:${exe_target}>"
    )
    add_custom_command(
        TARGET ${exe_target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "${output_file}" "${CMAKE_BINARY_DIR}"
    )
endif()
