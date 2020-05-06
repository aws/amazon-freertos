set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
set(demos_uvision_dir "${AFR_ROOT_DIR}/projects/nuvoton/numaker_iot_m487_wifi/uvision/aws_demos")
set(tests_uvision_dir "${AFR_ROOT_DIR}/projects/nuvoton/numaker_iot_m487_wifi/uvision/aws_tests")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(board_uvision_dir "${tests_uvision_dir}")
    set(exe_target aws_tests)
else()
    set(board_dir "${board_demos_dir}")
    set(board_uvision_dir "${demos_uvision_dir}")
    set(exe_target aws_demos)
endif()

if(AFR_ENABLE_ETH)
    set(extra_define -DM487_ETH_DEMO)
else()
    set(extra_define "")
endif()

# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "NuMaker-IoT-M487")
afr_set_board_metadata(DISPLAY_NAME "NuMaker-IoT-M487")
afr_set_board_metadata(DESCRIPTION "NuMaker-IoT-M487 Development Board")
afr_set_board_metadata(VENDOR_NAME "Nuvoton")
afr_set_board_metadata(FAMILY_NAME "NuMicro M487 Series")
afr_set_board_metadata(DATA_RAM_MEMORY "160KB")
afr_set_board_metadata(PROGRAM_MEMORY "512KB")
afr_set_board_metadata(CODE_SIGNER "null")
afr_set_board_metadata(SUPPORTED_IDE "uVision")
afr_set_board_metadata(RECOMMENDED_IDE "uVision")
afr_set_board_metadata(IDE_uVision_NAME "Keil uVision")
afr_set_board_metadata(IDE_uVision_COMPILER "ARMCC")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(IDE_uVision_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/nuvoton/numaker_iot_m487_wifi/uvision/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
    -DPRODUCT_VERSION=m487 
    -DCONFIG_REPEATER 
    -DSUPPORT_MBEDTLS 
    -DHAL_DFS_MODULE_ENABLED 
    -DLWIP_NO_STDINT_H=1 
    -DLWIP_TIMEVAL_PRIVATE=1
    -D__little_endian__=1
    -DNDEBUG
    -DUNITY_INCLUDE_CONFIG_H 
    -DRVDS_ARMCM4_NUC4xx 
    -D__LITTLE_ENDIAN__ 
    -DM487 
    ${extra_define}
)
if(AFR_IS_TESTING)
    target_compile_definitions(
        AFR::compiler::mcu_port
        INTERFACE -DMBEDTLS_USER_CONFIG_FILE=\"mbedtls_user_config.h\"
    )
endif()
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:ASM>:${assembler_defined_symbols}>
)

# --split_sections is to make one ELF section per function. In IDE project file, it is disabled.
# So it is not included in the compiler flags. If enabling it, the code size is smaller because it removes unused functions.
# Reference: http://www.keil.com/support/docs/3738.htm
set (compiler_flags --c99 -c --cpu Cortex-M4.fp  -g -O0 --apcs=interwork)
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
set(linker_flags --cpu Cortex-M4.fp --diag_suppress 6340,6329,6314 --entry Reset_Handler --library_type=microlib --strict --scatter "${CMAKE_CURRENT_LIST_DIR}/m487_flash.sct" --summary_stderr --info summarysizes,sizes,totals,unused,veneers --map --xref --callgraph --symbols )

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
        # Nuvoton SDK sources (driver)		
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/clk.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/crypto.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/fmc.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/sys.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/uart.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/retarget.c"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        "${AFR_KERNEL_DIR}/portable/RVDS/ARM_CM4F/port.c"
        "${board_dir}/application_code/nuvoton_code/entropy_hardware_poll.c"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/RVDS/ARM_CM4F"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/CMSIS/Include"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/Device/Nuvoton/numaker_iot_m487_wifi/Include"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/middleware/wifi_esp8266"        
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/inc"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/Keil"
        "${board_dir}/config_files"
        "${board_dir}/application_code/nuvoton_code"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
        "${board_dir}/config_files"
)

# PKCS11
afr_mcu_port(pkcs11_implementation DEPENDS AFR::pkcs11_mbedtls)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/iot_pkcs11_pal.c"
)


if(AFR_ENABLE_ETH)

    # FreeRTOS Plus TCP
    afr_mcu_port(freertos_plus_tcp)
    target_sources(
        AFR::freertos_plus_tcp::mcu_port
        INTERFACE
            "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_1.c"
            "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/M487/m480_eth.c"
            "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/M487/NetworkInterface.c"
    )

    target_include_directories(
        AFR::freertos_plus_tcp::mcu_port
        INTERFACE
            "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/M487"
            "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/inc"
    )

    # Secure sockets for ETH
    afr_mcu_port(secure_sockets)
    target_link_libraries(
        AFR::secure_sockets::mcu_port
        INTERFACE AFR::secure_sockets_freertos_plus_tcp
    )

else()

    # WiFi
    afr_mcu_port(wifi)
    target_sources(
        AFR::wifi::mcu_port
        INTERFACE 
            "${afr_ports_dir}/wifi/iot_wifi.c"
            "${AFR_VENDORS_DIR}/nuvoton/sdk/middleware/wifi_esp8266/esp8266_wifi.c"
    )

    target_include_directories(
        AFR::wifi::mcu_port
        INTERFACE
            "${AFR_VENDORS_DIR}/nuvoton/sdk/middleware/wifi_esp8266"        
    )

    # Secure sockets for WiFi
    afr_mcu_port(secure_sockets)

    target_sources(
        AFR::secure_sockets::mcu_port
        INTERFACE 
            "${afr_ports_dir}/secure_sockets/iot_secure_sockets.c"
    )

    target_link_libraries(
        AFR::secure_sockets::mcu_port
        INTERFACE
            AFR::tls
            AFR::wifi
    )
endif()


# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".axf")

afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(
        ${exe_target}
        ${board_code_src}
        ${config_files}
        "${board_dir}/application_code/main.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/Device/Nuvoton/numaker_iot_m487_wifi/Source/ARM/startup_M480.s"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/Device/Nuvoton/numaker_iot_m487_wifi/Source/system_M480.c"
        $<IF:${AFR_IS_TESTING}, ,${network_manager_src}>
    )

    if(AFR_ENABLE_ETH)
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::freertos_plus_tcp
            AFR::utils
    )
    else()
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::wifi
            AFR::utils
    )
    endif()

    if(NOT AFR_METADATA_MODE)
        # Convert afx file to bin file
        find_program(ARM_KEIL_FROMELF fromelf.exe PATHS "${AFR_COMPILER_DIR}")
    endif()

    add_custom_command(
        TARGET ${exe_target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}"
        COMMAND "${ARM_KEIL_FROMELF}" --bin "${exe_target}.axf" --output "${exe_target}.bin"
        COMMAND "${ARM_KEIL_FROMELF}" --text -c "${exe_target}.axf" --output "${exe_target}.txt"
    )
endif()
