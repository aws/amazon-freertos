cmake_minimum_required(VERSION 3.13)

set(stm32l475_dir "${AFR_3RDPARTY_DIR}/mcu_vendor/st/stm32l475_discovery")
set(board_demos_dir "${AFR_ROOT_DIR}/demos/st/stm32l475_discovery/common")
set(board_tests_dir "${AFR_ROOT_DIR}/tests/st/stm32l475_discovery/common")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
else()
    set(board_dir "${board_demos_dir}")
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(NAME "STM32-B-L475E-IOT01A")
afr_set_board_metadata(DISPLAY_NAME "STM32L4 Discovery kit IoT node")
afr_set_board_metadata(DESCRIPTION "B-L475E-IOT01A Discovery kit with low-power RF, Wi-Fi, multiway sensing")
afr_set_board_metadata(VENDOR_NAME "STMicroelectronics")
afr_set_board_metadata(FAMILY_NAME "STM32L4")
afr_set_board_metadata(DATA_RAM_MEMORY "128KB")
afr_set_board_metadata(PROGRAM_MEMORY "1MB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "SystemWorkbench")
afr_set_board_metadata(IDE_SystemWorkbench_NAME "System Workbench for STM32")
afr_set_board_metadata(IDE_SystemWorkbench_COMPILERS "GCC")

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
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
set(vendor "st")
set(board "stm32l475_discovery")
set(portable_dir "portable/${vendor}/${board}")

# Kernel
afr_mcu_port(kernel)
afr_glob_src(driver_src DIRECTORY "${stm32l475_dir}" RECURSE)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        ${driver_src}
        "${AFR_MODULES_DIR}/FreeRTOS/portable/GCC/ARM_CM4F/port.c"
        "${AFR_MODULES_DIR}/FreeRTOS/portable/GCC/ARM_CM4F/portmacro.h"
        "${AFR_MODULES_DIR}/FreeRTOS/portable/MemMang/$<IF:${AFR_IS_TESTING},heap_4.c,heap_5.c>"
)
set(
    kernel_inc_dirs
    "${AFR_MODULES_DIR}/FreeRTOS/portable/GCC/ARM_CM4F"
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
    "${board_dir}/config_files"
    "${board_dir}/application_code"
    "${board_dir}/application_code/st_code"
    "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
)
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
)

# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${AFR_MODULES_DIR}/wifi/${portable_dir}/aws_wifi.c"
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE "${AFR_MODULES_DIR}/pkcs11/${portable_dir}/aws_pkcs11_pal.c"
)
# Link to AFR::pkcs11_mbedtls if you want to use default implementation based on mbedtls.
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE AFR::pkcs11_mbedtls
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE AFR::tls
)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${AFR_MODULES_DIR}/secure_sockets/${portable_dir}/aws_secure_sockets.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/st_code")
add_executable(
    ${exe_target}
    ${board_code_src}
    "${board_dir}/application_code/main.c"
)
target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
        -T"${board_dir}/../ac6/STM32L475VGTx_FLASH.ld"
)

find_program(gcc_objcopy arm-none-eabi-objcopy)
find_program(gcc_size arm-none-eabi-size)

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
