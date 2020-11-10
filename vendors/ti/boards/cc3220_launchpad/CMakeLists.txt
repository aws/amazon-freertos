set(simplelink_dir "${AFR_VENDORS_DIR}/ti/SimpleLink_CC32xx/v2_10_00_04")
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
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
if(${AFR_TOOLCHAIN} STREQUAL "arm-ti")
    include("${CMAKE_CURRENT_LIST_DIR}/ccs.cmake")
elseif(${AFR_TOOLCHAIN} STREQUAL "arm-iar")
    include("${CMAKE_CURRENT_LIST_DIR}/iar.cmake")
    if(NOT AFR_METADATA_MODE)
        message(FATAL_ERROR "CMake support for TI using IAR is not complete yet.")
    endif()
elseif(NOT AFR_METADATA_MODE)
    message( FATAL_ERROR "The toolchain is not supported." )
endif()

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
afr_set_board_metadata(SUPPORTED_IDE "CCS;IAR")
afr_set_board_metadata(RECOMMENDED_IDE "CCS")
afr_set_board_metadata(IDE_CCS_NAME "Code Composer Studio")
afr_set_board_metadata(IDE_CCS_COMPILER "TI-ARM")
afr_set_board_metadata(IDE_IAR_NAME "IAR Embedded Workbench")
afr_set_board_metadata(IDE_IAR_COMPILER "IAR-ARM")
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
        "${CMAKE_CURRENT_LIST_DIR}/CC3220SF.ccxml"
        "${simplelink_dir}/kernel/freertos/dpl/ClockP_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/DebugP_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/HwiPCC32XX_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/MutexP_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/PowerCC32XX_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/SemaphoreP_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/SystemP_freertos.c"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        ${compiler_specific_src}
)
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${simplelink_dir}/source"
        "${board_dir}/config_files"
        "${board_dir}/application_code/ti_code"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
        ${compiler_specific_include}
)
target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE
        AFR::posix
        AFR::posix_headers
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

# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${afr_ports_dir}/wifi/iot_wifi.c"
)
# WiFi on TI requires POSIX types
target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
        AFR::posix::mcu_port
        AFR::secure_sockets
)

# PKCS11
afr_mcu_port(pkcs11_implementation)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/core_pkcs11_pal.c"
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
    INTERFACE "${afr_ports_dir}/secure_sockets/iot_secure_sockets.c"
)

# OTA
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE "${afr_ports_dir}/ota/aws_ota_pal.c"
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

afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/ti_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(
        ${exe_target}
        ${board_code_src}
        ${config_files}
        "${board_dir}/application_code/main.c"
        "${board_dir}/application_code/ti_code/CC3220SF_LAUNCHXL_FREERTOS.cmd"
    )
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::wifi
            AFR::utils
            AFR::posix::mcu_port
            ${link_extra_flags}
    )
    add_custom_command(
        TARGET ${exe_target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}"
    )

    find_program(tiobj2bin tiobj2bin PATHS "${TI_CCS_PATH}/utils/tiobj2bin")
    find_program(mkhex4bin mkhex4bin PATHS "${TI_CCS_PATH}/utils/tiobj2bin")
    find_program(armofd armofd PATHS "${AFR_COMPILER_DIR}")
    find_program(armhex armhex PATHS "${AFR_COMPILER_DIR}")
    if(tiobj2bin)
        set(output_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.hex")
        add_custom_command(
            TARGET ${exe_target} POST_BUILD
            COMMAND "${tiobj2bin}" "${CMAKE_BINARY_DIR}/${exe_target}.out" "${CMAKE_BINARY_DIR}/${exe_target}.bin"
                    "${armofd}" "${armhex}" "${mkhex4bin}"
        )
    endif()
endif()
