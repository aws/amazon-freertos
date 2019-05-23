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
afr_set_board_metadata(ID "Xilinx-Avnet-MicroZed")
afr_set_board_metadata(DISPLAY_NAME "Microzed Industrial IoT Bundle")
afr_set_board_metadata(DESCRIPTION "MicroZed development board built on Xilinx Zynq-7000 SoC")
afr_set_board_metadata(VENDOR_NAME "Xilinx")
afr_set_board_metadata(FAMILY_NAME "Zynq-7000")
afr_set_board_metadata(DATA_RAM_MEMORY "1GB")
afr_set_board_metadata(PROGRAM_MEMORY "128MB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "XSDK")
afr_set_board_metadata(RECOMMENDED_IDE "XSDK")
afr_set_board_metadata(IDE_XSDK_NAME "Xilinx SDK")
afr_set_board_metadata(IDE_XSDK_COMPILER "GCC")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(IDE_XSDK_PROJECT_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/xsdk")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common/config_files")

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
set(linker_flags -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -Wl,-build-id=none -specs=${CMAKE_CURRENT_LIST_DIR}/aws_demos/xsdk/aws_demos/src/Xilinx.spec -Wl,-T -Wl,${CMAKE_CURRENT_LIST_DIR}/aws_demos/xsdk/aws_demos/src/lscript.ld -L${AFR_VENDORS_DIR}/xilinx/aws_bsp/ps7_cortexa9_0/lib   )

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
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/include"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/Compiler/GCC"
        "${AFR_MODULES_STANDARD_DIR}/tls/include"
        "${AFR_3RDPARTY_DIR}/pkcs11"
        "${AFR_MODULES_PORTS_DIR}/pkcs11/include"
        "${AFR_MODULES_PORTS_DIR}/wifi/include"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface"
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
        "${AFR_MODULES_PORTS_DIR}/pkcs11/mbedtls/aws_pkcs11_mbedtls.c"
        "${AFR_MODULES_PORTS_DIR}/pkcs11/mbedtls/threading_alt.h"
)
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE 3rdparty::mbedtls
)

target_include_directories(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${AFR_MODULES_STANDARD_DIR}/crypto/include/"
)


# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_2.c"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/Zynq/NetworkInterface.c"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/Zynq/x_emacpsif_dma.c"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/Zynq/x_emacpsif_physpeed.c"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/Zynq/x_emacpsif_hw.c"
)


target_include_directories(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/Compiler/GCC"
)



# Secure sockets
afr_mcu_port(secure_sockets)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE 
        "${AFR_MODULES_PORTS_DIR}/secure_sockets/freertos_plus_tcp/aws_secure_sockets.c"
        "${AFR_MODULES_STANDARD_DIR}/tls/src/aws_tls.c"
        "${AFR_MODULES_STANDARD_DIR}/crypto/src/aws_crypto.c"
)

target_include_directories(
    AFR::secure_sockets::mcu_port
    INTERFACE
       "${AFR_3RDPARTY_DIR}/lwip/src/include/lwip"
       "${AFR_3RDPARTY_DIR}/lwip/src/portable"
       "${AFR_ROOT_DIR}/modules/ports/wifi/include"
       "${AFR_MODULES_STANDARD_DIR}/common/include"
       "${AFR_MODULES_STANDARD_DIR}/tls/include"
       "${AFR_3RDPARTY_DIR}/mbedtls/include"
       "${AFR_MODULES_STANDARD_DIR}/crypto/include"
       "${AFR_MODULES_PORTS_DIR}/pkcs11/mbedtls"
)

afr_module_dependencies(
    secure_sockets_freertos_tcp
    INTERFACE
        AFR::freertos_plus_tcp
        AFR::tls
        AFR::crypto
        AFR::pkcs11
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# TODO, remove network manager src.
afr_glob_src(network_manager_src DIRECTORY "${AFR_DEMOS_DIR}/network_manager")
afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/xilinx_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

add_executable(
    ${exe_target}
    ${board_code_src}
    "${board_dir}/application_code/xilinx_code/FreeRTOS_Tick_config.c"
    ${config_files}
    "${board_dir}/application_code/main.c"
    $<$<NOT:${AFR_IS_TESTING}>:${network_manager_src}>
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::utils
        AFR::freertos_plus_tcp
)

# The kernel is the root dependency, linking the library flags like this ensures it adds the flags last in the command line

set(link_extra_flags -Wl,--start-group,-lxilffs,-lxil,-lgcc,-lc,-lrsa,--end-group)

target_link_libraries(
    AFR::kernel::mcu_port
        INTERFACE ${link_extra_flags}
)
