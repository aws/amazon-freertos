set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(exe_target aws_tests)
    set(xilinx_xsdk_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests/xsdk")
    set(xilinx_config_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests/config_files")
else()
    set(board_dir "${board_demos_dir}")
    set(exe_target aws_demos)
    set(xilinx_xsdk_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos/xsdk")
    set(xilinx_config_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")
endif()



# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "Xilinx-Avnet-MicroZed")
afr_set_board_metadata(DISPLAY_NAME "Microzed Industrial IoT Bundle")
afr_set_board_metadata(DESCRIPTION "MicroZed development board built on Xilinx Zynq-7000 SoC")
afr_set_board_metadata(VENDOR_NAME "Xilinx")
afr_set_board_metadata(FAMILY_NAME "Zynq-7000")
afr_set_board_metadata(DATA_RAM_MEMORY "1GB")
afr_set_board_metadata(PROGRAM_MEMORY "128MB")
afr_set_board_metadata(CODE_SIGNER "null")
afr_set_board_metadata(SUPPORTED_IDE "XSDK")
afr_set_board_metadata(RECOMMENDED_IDE "XSDK")
afr_set_board_metadata(IDE_XSDK_NAME "Xilinx SDK")
afr_set_board_metadata(IDE_XSDK_COMPILER "GCC")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")

afr_set_board_metadata(IDE_XSDK_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/xilinx/microzed/xsdk/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION ${xilinx_config_dir})

#

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

set(compiler_flags -fmessage-length=0 -Wall -O0 -g3 -g -x c -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP)
set(assembler_flags -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP -fmessage-length=0 -Wall -O0 -g3)
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
if(AFR_IS_TESTING)
    set(linker_flags -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -Wl,-build-id=none
        -specs=${board_dir}/Xilinx.spec
        -Wl,-T -Wl,${board_dir}/lscript.ld
        -L${AFR_VENDORS_DIR}/xilinx/aws_bsp/ps7_cortexa9_0/lib)
else()
    set(linker_flags -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -Wl,-build-id=none
        -specs=${board_dir}/Xilinx.spec
        -Wl,-T -Wl,${board_dir}/lscript.ld
        -L${AFR_VENDORS_DIR}/xilinx/aws_bsp/ps7_cortexa9_0/lib)
endif()

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
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        "${AFR_KERNEL_DIR}/portable/GCC/ARM_CA9/port.c"
        "${AFR_KERNEL_DIR}/portable/GCC/ARM_CA9/portASM.S"
        ${compiler_specific_src}
)
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${board_dir}/config_files"
        "${board_dir}/application_code/xilinx_code"
        "${AFR_VENDORS_DIR}/xilinx/aws_bsp/ps7_cortexa9_0/include"
        "${AFR_KERNEL_DIR}/portable/GCC/ARM_CA9"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/GCC"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/tls/include"
        "${AFR_3RDPARTY_DIR}/pkcs11"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/wifi/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
        ${compiler_specific_include}
)
target_link_libraries(
    AFR::kernel::mcu_port
)


# PKCS11
afr_mcu_port(pkcs11_implementation)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/iot_pkcs11_pal.c"
)
target_link_libraries(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        AFR::pkcs11_mbedtls
)


# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_2.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/Zynq/NetworkInterface.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/Zynq/x_emacpsif_dma.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/Zynq/x_emacpsif_physpeed.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/Zynq/x_emacpsif_hw.c"
)

target_include_directories(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/GCC"
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE AFR::secure_sockets_freertos_plus_tcp
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/xilinx_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

# The kernel is the root dependency, linking the library flags like this ensures it adds the flags last in the command line
set(link_extra_flags -Wl,--start-group,-lxilffs,-lxil,-lgcc,-lc,-lrsa,--end-group)

target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE
        ${link_extra_flags}
)

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(
        ${exe_target}
        ${board_code_src}
        ${config_files}
        "${board_dir}/application_code/xilinx_code/FreeRTOS_tick_config.c"
        "${board_dir}/application_code/main.c"
    )
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::utils
    )

    set(bootgen  "${AFR_COMPILER_DIR}/../../../../../bin/bootgen.bat")

    if(AFR_IS_TESTING)
        add_custom_command(
            TARGET ${exe_target} POST_BUILD
            COMMAND "echo" "Running Post-build step"
            COMMAND "${bootgen}" -w -image "${board_dir}/aws_tests.bif" -arch zynq -o BOOT.bin
        )
    else()
        add_custom_command(
            TARGET ${exe_target} POST_BUILD
            COMMAND "echo" "Running Post-build step"
            COMMAND "${bootgen}" -w -image "${board_dir}/aws_demos.bif" -arch zynq -o BOOT.bin
        )
    endif()
endif()

if(NOT AFR_METADATA_MODE)
    message(FATAL_ERROR "CMake support for nxp is not complete yet.")
endif()
