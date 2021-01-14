# These FreeRTOS related global variables are available to use.
# AFR_ROOT_DIR                  FreeRTOS source root.
# AFR_KERNEL_DIR                FreeRTOS kernel root.
# AFR_MODULES_DIR               FreeRTOS modules root.
# AFR_MODULES_C_SDK_DIR         C-SDK libraries root.
# AFR_MODULES_FREERTOS_PLUS_DIR FreeRTOS-Plus libraries root.
# AFR_MODULES_ABSTRACTIONS_DIR  Abstractions layers root.
# AFR_DEMOS_DIR                 FreeRTOS demos root.
# AFR_TESTS_DIR                 FreeRTOS common tests and framework root.
# AFR_VENDORS_DIR               vendors content root.
# AFR_3RDPARTY_DIR              3rdparty libraries root.
# AFR_VENDOR_NAME               Folder name for vendor.
# AFR_BOARD_NAME                Folder name for this board
# AFR_TOOLCHAIN                 Compiler chosen by the user. Should be one of
#                               the file names under ${AFR_ROOT_DIR}/tools/cmake/toolchains
# AFR_IS_TESTING                1 if testing enabled, otherwise, 0.

# You may also use these 2 functions we defined to glob files when needed. However, we recommend
# to specify your source files explicitly to avoid unexpected behavior unless you're 100% sure.
# CMake reference link: https://cmake.org/cmake/help/latest/command/file.html#filesystem
# afr_glob_files(<out_var> [RECURSE] <DIRECTORY> <directory> [<GLOBS> <glob-expressions>...])
# afr_glob_src(<out_var> [RECURSE] <DIRECTORY> <directory> [<EXTENSIONS> <file-extensions>...])

# If you don't specify GLOBS or EXTENSIONS parameters,
# afr_glob_files: glob all files including hidden files in the specified directory.
# afr_glob_src:   glob all files ending with either .c, .h, .s or .asm

# Use RECURSE if you want to recursively search all subdirectories.

# Example usage,
# afr_glob_src(board_code DIRECTORY "${board_dir}/application_code/${vendor}_code")
# afr_glob_src(driver_code RECURSE DIRECTORY "${driver_path}")
# afr_glob_src(headers DIRECTORY "${some_path}" EXTENSIONS h)

# -------------------------------------------------------------------------------------------------
# Common variables setup
# -------------------------------------------------------------------------------------------------

# Set common variables
set(simplelink_sdk_dir "${AFR_VENDORS_DIR}/ti/SimpleLink_CC32xx/v4_30_00_06")
set(simplelink_common_src_dir "${AFR_VENDORS_DIR}/ti/simplelink_common/")
set(portable_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(exe_target aws_tests)
else()
    set(board_dir "${board_demos_dir}")
    set(exe_target aws_demos)
endif()

# Include IDE specific cmake file.
if(${AFR_TOOLCHAIN} STREQUAL "arm-gcc")
    include("${CMAKE_CURRENT_LIST_DIR}/gcc.cmake")
    set(toolchain "gcc")
elseif(${AFR_TOOLCHAIN} STREQUAL "arm-ti")
    include("${CMAKE_CURRENT_LIST_DIR}/ccs.cmake")
    set(toolchain "ccs")
elseif(${AFR_TOOLCHAIN} STREQUAL "arm-iar")
    include("${CMAKE_CURRENT_LIST_DIR}/iar.cmake")
    set(toolchain "iar")
    if(NOT AFR_METADATA_MODE)
        message(FATAL_ERROR "CMake support for TI using IAR is not complete yet.")
endif()
elseif(NOT AFR_METADATA_MODE)
    message( FATAL_ERROR "The toolchain is not supported." )
endif()

# These files were generated from the Sysconfig file found in ${board_dir}/application_code/ti_code.
# To regenerate them, the bash/bat script provided with Sysconfig can be executed like so
# ${sysconfig_sh} --compiler ${compiler_str} --product ${simplelink_sdk_dir}/.metadata/product.json --output ${sysconfig_out}
# in order to generate new driver files from the modified Sysconfig.
set(sysconfig_out "${board_dir}/application_code/ti_code/${toolchain}")
set(sysconfig_files_wifi
    "${sysconfig_out}/ti_drivers_net_wifi_config.c"
    "${sysconfig_out}/ti_net_config.c"
)
set(sysconfig_files_drivers
    "${sysconfig_out}/ti_drivers_config.c"
)
set(sysconfig_files_all
    "${sysconfig_files_wifi}"
    "${sysconfig_files_drivers}"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------

afr_set_board_metadata(ID "TI-CC3220SF-LAUNCHXL")
afr_set_board_metadata(DISPLAY_NAME "CC3220SF-LAUNCHXL")
afr_set_board_metadata(DESCRIPTION "CC3220SF SimpleLink™ Wi-Fi® LaunchPad™ Development Kit")
afr_set_board_metadata(VENDOR_NAME "Texas Instruments")
afr_set_board_metadata(FAMILY_NAME "CC3220")
afr_set_board_metadata(DATA_RAM_MEMORY "256KB")
afr_set_board_metadata(PROGRAM_MEMORY "1MB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-TI-CC3220SF")
afr_set_board_metadata(SUPPORTED_IDE "CCS")
afr_set_board_metadata(RECOMMENDED_IDE "CCS")
afr_set_board_metadata(IDE_CCS_NAME "Code Composer Studio")
afr_set_board_metadata(IDE_CCS_COMPILER "GCC")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")

afr_set_board_metadata(IDE_CCS_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/ti/cc3220_launchpad/ccs/aws_demos")
afr_set_board_metadata(IDE_IAR_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/ti/cc3220_launchpad/iar/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

afr_mcu_port(compiler)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:ASM>:${assembler_defined_symbols}>
)
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
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE ${linker_flags}
)
target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE ${link_dependent_libs}
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

# Normally the portable layer for kernel should be vendor's driver code.
afr_mcu_port(kernel)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${simplelink_sdk_dir}/kernel/freertos/dpl/ClockP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/DebugP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/HwiPCC32XX_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/MutexP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/PowerCC32XX_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/QueueP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/SemaphoreP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/SwiP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/SystemP_freertos.c"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        ${compiler_specific_src}
)
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${simplelink_sdk_dir}/source"
        ${sysconfig_out}
        "${simplelink_common_src_dir}/utils/uart_term"
        "${board_dir}/config_files"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
        ${compiler_specific_include}
)

# Common I/O, currently only fully supported for GCC
if(${AFR_TOOLCHAIN} STREQUAL "arm-gcc")
afr_mcu_port(common_io)
target_sources(
  AFR::common_io::mcu_port
  INTERFACE
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_uart.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_spi.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_i2c.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_gpio.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_timer.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_flash.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_pwm.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_rtc.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_watchdog.c"
      "$<IF:${AFR_IS_TESTING},${simplelink_common_src_dir}/ports/common_io/test_internals/${AFR_BOARD_NAME}/iot_test_common_io_internal.c,>"
)

target_include_directories(
    AFR::common_io::mcu_port
    INTERFACE
      ${compiler_posix_includes}
      ${compiler_specific_include}
      "${sysconfig_out}"
      "$<IF:${AFR_IS_TESTING},${AFR_MODULES_ABSTRACTIONS_DIR}/common_io/test,>"
)
endif()

# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE
        "${portable_dir}/wifi/iot_wifi.c"
        "${simplelink_common_src_dir}/utils/network_if/${AFR_BOARD_NAME}/network_if.c"
        "${sysconfig_files_wifi}"

        # Use TI POSIX instead of FreeRTOS_POSIX for compatibility with TI
        # network stack.
        "${simplelink_sdk_dir}/source/ti/posix/freertos/clock.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/mqueue.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread_barrier.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread_cond.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread_mutex.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread_rwlock.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/sched.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/sleep.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/semaphore.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/timer.c"
)

# WiFi on TI requires POSIX types
target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
      "${compiler_posix_includes}"
      "${simplelink_sdk_dir}/source/ti/net"
      "${simplelink_sdk_dir}/source/ti/drivers/net/wifi"
      "${simplelink_common_src_dir}/utils/network_if/${AFR_BOARD_NAME}"
)

target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
        AFR::secure_sockets
)

# PKCS11
afr_mcu_port(pkcs11_implementation)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${portable_dir}/pkcs11/core_pkcs11_pal.c"
)
target_link_libraries(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        AFR::pkcs11_mbedtls
        3rdparty::mbedtls
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${portable_dir}/secure_sockets/iot_secure_sockets.c"
)

# OTA
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE "${portable_dir}/ota/aws_ota_pal.c"
)
target_link_libraries(
    AFR::ota::mcu_port
    INTERFACE
        AFR::ota_mqtt
        AFR::ota_http
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

set(CMAKE_EXECUTABLE_SUFFIX ".out")

afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(
        ${exe_target}
        ${sysconfig_files_drivers}
        ${board_code_src}
        ${config_files}
        "${simplelink_common_src_dir}/utils/uart_term/uart_term.c"
        "${board_dir}/application_code/main.c"
        # Memory.c contains a malloc to be used for FreeRTOS port.
        # This resolves issue of functions using "malloc" calling into the
        # stdlib malloc, when we actually want to use pvPortMalloc.
        # It must be in this artifact to avoid confusing the linker.
        "${simplelink_sdk_dir}/source/ti/posix/freertos/memory.c"
    )
    target_include_directories(
        ${exe_target}
        PRIVATE
            "${sysconfig_out}"
    )
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::wifi
             $<$<TARGET_EXISTS:AFR::common_io::mcu_port>:AFR::common_io>
            AFR::utils
            ${link_extra_flags}
    )
    add_custom_command(
        TARGET ${exe_target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}"
    )

    # Enable binary generation as a post-build action
    include("${simplelink_common_src_dir}/cmake/ti_object_2_binary.cmake")
endif()
