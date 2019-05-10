set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests/common")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(exe_target aws_tests)
else()
    set(board_dir "${board_demos_dir}")
    set(exe_target aws_demos)
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "microzed")
afr_set_board_metadata(DISPLAY_NAME "Xilinx MicroZEed")
afr_set_board_metadata(DESCRIPTION "Xilinx Avnet MicroZed Industrial IoT Kit")
afr_set_board_metadata(VENDOR_NAME "Xilinx")
afr_set_board_metadata(FAMILY_NAME "Zynq-7000 SoC")
afr_set_board_metadata(DATA_RAM_MEMORY "1GB")
afr_set_board_metadata(PROGRAM_MEMORY "QSPI OR MicroSD")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "XSDK")
afr_set_board_metadata(RECOMMENDED_IDE "XSDK")
afr_set_board_metadata(IS_ACTIVE "TRUE")

afr_set_board_metadata(IDE_XSDK_PROJECT_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/xsdk")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common/config_files")

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
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Normally the portable layer for kernel should be vendor's driver code.
afr_mcu_port(kernel)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        ${compiler_specific_src}
)
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${board_dir}/config_files"
        "${board_dir}/application_code/xilinx_code"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TEST_DIR},${AFR_DEMOS_DIR}>/include"
        ${compiler_specific_include}
)
target_link_libraries(
    AFR::kernel::mcu_port
)


# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/aws_pkcs11_pal.c"
        "${AFR_MODULES_PORTS_DIR}/pkcs11/mbedtls/threading_alt.h"
)
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE 3rdparty::mbedtls
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${AFR_MODULES_PORTS_DIR}/secure_sockets/freertos_plus_tcp/aws_secure_sockets.c"
)



# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".out")

# TODO, remove network manager src.
afr_glob_src(network_manager_src DIRECTORY "${AFR_DEMOS_DIR}/network_manager")
afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/xilinx_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

add_executable(
    ${exe_target}
    ${board_code_src}
    ${config_files}
    "${board_dir}/application_code/main.c"
    $<$<NOT:${AFR_IS_TESTING}>:${network_manager_src}>
)
target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::utils
        ${link_extra_flags}
)


