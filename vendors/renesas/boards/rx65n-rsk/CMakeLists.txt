# Set variables.
set(renesas_dir "${AFR_VENDORS_DIR}/renesas")
set(rx65nrsk_dir "${AFR_VENDORS_DIR}/renesas/boards/rx65n-rsk")
set(rx65nrsk_ports_dir "${rx65nrsk_dir}/ports")
set(rx_driver_package_dir "${AFR_VENDORS_DIR}/renesas/rx_driver_package/v125")

if(AFR_IS_TESTING)
    set(rx65nrsk_aws_dir "${rx65nrsk_dir}/aws_tests")
    set(exe_target aws_tests)
else()
    set(rx65nrsk_aws_dir "${rx65nrsk_dir}/aws_demos")
    set(exe_target aws_demos)
endif()

# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------

afr_set_board_metadata(ID "Renesas-RX65N-RSK")
afr_set_board_metadata(DISPLAY_NAME "Renesas Starter Kit+ for RX65N-2MB")
afr_set_board_metadata(DESCRIPTION "The Renesas Starter Kit+ for RX65N-2MB connects to AWS IoT Cloud using Ethernet")
afr_set_board_metadata(VENDOR_NAME "Renesas")
afr_set_board_metadata(FAMILY_NAME "RX65N")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(DATA_RAM_MEMORY "640KB")
afr_set_board_metadata(PROGRAM_MEMORY "2MB")
afr_set_board_metadata(SUPPORTED_IDE "E2Studio")
afr_set_board_metadata(RECOMMENDED_IDE "E2Studio")
afr_set_board_metadata(IDE_E2Studio_NAME "e2 studio")
afr_set_board_metadata(IDE_E2Studio_COMPILER "CCRX")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")

afr_set_board_metadata(IDE_E2Studio_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/renesas/rx65n-rsk/e2studio/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")


# -------------------------------------------------------------------------------------------------
# Compiler settings
#
# Note1: Cmake doesn't support cc-rx compiler. The settings are commented out.
# Note2: The settings below are NOT test yet. Some flags might be wrong and some might be missing.
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)

#set(compiler_flags
#    -isa=rxv2 -fpu -branch=32 -preinclude="implicitlyinclude.h"
#    -nomessage=11174,21644,20010,23034,23035,20177,23033 -output=obj  -debug -nologo
#    -obj_path=${workspace_loc:/${ProjName}/${ConfigName}}
#)

# Compiler flags.
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${compiler_flags}>
)

#set(assembler_flags
#    -isa=rxv2 -fpu -debug -nologo
#    -output=${workspace_loc:/${ProjName}/${ConfigName}}
#)

# Assembler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:ASM>:${assembler_flags}>
)

#set(linker_flags
#    -noprelink -form=absolute -nomessage -vect=_undefined_interrupt_source_isr -list=${exe_target}.map
#    -nooptimize -rom=D=R,D_1=R_1,D_2=R_2 -nologo
#)

target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
        ${linker_flags}
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(renesas_amazon_freertos_common_dir "${renesas_dir}/amazon_freertos_common")
afr_glob_src(afr_common_src DIRECTORY "${renesas_amazon_freertos_common_dir}")
afr_glob_src(afr_common_compiler_src DIRECTORY "${renesas_amazon_freertos_common_dir}/compiler_support/ccrx")
afr_glob_src(afr_common_network_src DIRECTORY "${renesas_amazon_freertos_common_dir}/network_support/onchip_rx_ether")

set(afr_common_include
    "${renesas_amazon_freertos_common_dir}"
    "${renesas_amazon_freertos_common_dir}/compiler_support/ccrx"
    "${renesas_amazon_freertos_common_dir}/network_support/onchip_rx_ether"
)

# r_cmt_rx, r_riic_rx, r_sci_iic_rx are excluded
afr_glob_src(rbyteq_src DIRECTORY "${rx_driver_package_dir}/r_byteq" RECURSE)
afr_glob_src(retherrx_src DIRECTORY "${rx_driver_package_dir}/r_ether_rx" RECURSE)
afr_glob_src(rflashrx_src DIRECTORY "${rx_driver_package_dir}/r_flash_rx" RECURSE)
afr_glob_src(rs12adrx_src DIRECTORY "${rx_driver_package_dir}/r_s12ad_rx" RECURSE)

# select only RX65 for r_bsp
afr_glob_src(rbsp_mcu_src DIRECTORY "${rx_driver_package_dir}/r_bsp/mcu")
afr_glob_src(rbsp_board_genericrx65n_src DIRECTORY "${rx_driver_package_dir}/r_bsp/board/generic_rx65n")
afr_glob_src(rbsp_mcu_all_src DIRECTORY "${rx_driver_package_dir}/r_bsp/mcu/all")
afr_glob_src(rbsp_mcu_rx65n_src DIRECTORY "${rx_driver_package_dir}/r_bsp/mcu/rx65n")

set(rbsp_mcu_src
    "${rx_driver_package_dir}/r_bsp/mcu/all/dbsct.c"
    "${rx_driver_package_dir}/r_bsp/mcu/all/lowlvl.c"
    "${rx_driver_package_dir}/r_bsp/mcu/all/lowsrc.c"
    "${rx_driver_package_dir}/r_bsp/mcu/all/resetprg.c"
    "${rx_driver_package_dir}/r_bsp/mcu/all/sbrk.c"
)

set(rbsp_src
    ${rbsp_mcu_src}
    ${rbsp_board_genericrx65n_src}
    ${rbsp_mcu_all_src}
    ${rbsp_mcu_rx65n_src}
)

# select only RX65 for r_sci_rx
afr_glob_src(rscirx_target_rx65n_src DIRECTORY "${rx_driver_package_dir}/r_sci_rx/src/targets/rx65n")

set(rscirx_src
    "${rx_driver_package_dir}/r_sci_rx/src/r_sci_rx.c"
    ${rscirx_target_rx65n_src}
)

set(rx_driver_package_include
    "${rx_driver_package_dir}"
    "${rx_driver_package_dir}/r_bsp"
    "${rx_driver_package_dir}/r_bsp/board/generic_rx65n"
    "${rx_driver_package_dir}/r_ether_rx"
    "${rx_driver_package_dir}/r_flash_rx"
    "${rx_driver_package_dir}/r_flash_rx/src"
    "${rx_driver_package_dir}/r_flash_rx/src/flash_type_1"
    "${rx_driver_package_dir}/r_flash_rx/src/flash_type_3"
    "${rx_driver_package_dir}/r_flash_rx/src/flash_type_4"
    "${rx_driver_package_dir}/r_flash_rx/src/targets"
    "${rx_driver_package_dir}/r_byteq"
    "${rx_driver_package_dir}/r_byteq/src"
    "${rx_driver_package_dir}/r_sci_rx"
    "${rx_driver_package_dir}/r_sci_rx/src"
    "${rx_driver_package_dir}/r_s12ad_rx"
    "${rx_driver_package_dir}/r_s12ad_rx/src"
    "${rx_driver_package_dir}/r_s12ad_rx/src/targets/rx65n"
)

afr_glob_src(smc_gen_src DIRECTORY "${rx65nrsk_aws_dir}/src/smc_gen" RECURSE)

set(smc_gen_include
    "${rx65nrsk_aws_dir}/src/smc_gen/r_config"
    "${rx65nrsk_aws_dir}/src/smc_gen/general"
    "${rx65nrsk_aws_dir}/src/smc_gen/r_pincfg"
)

# Kernel
afr_mcu_port(kernel)

target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        ${afr_common_src}
        ${afr_common_compiler_src}
        ${afr_common_network_src}
        ${rbsp_src}
        ${rbyteq_src}
        ${retherrx_src}
        ${rflashrx_src}
        ${rs12adrx_src}
        ${rscirx_src}
        ${smc_gen_src}
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        "${AFR_KERNEL_DIR}/portable/Renesas/RX600v2/port_asm.src"
        "${AFR_KERNEL_DIR}/portable/Renesas/RX600v2/port.c"
        "${AFR_KERNEL_DIR}/portable/Renesas/RX600v2/portmacro.h"
)

set(kernel_inc_dirs
    ${afr_common_include}
    ${rx_driver_package_include}
    ${smc_gen_include}
    "${AFR_KERNEL_DIR}/portable/Renesas/RX600v2"
    "${rx65nrsk_aws_dir}/config_files"
    "${rx65nrsk_aws_dir}/application_code/renesas_code"
    "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
)

# PKCS11
afr_mcu_port(pkcs11_implementation)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${rx65nrsk_ports_dir}/pkcs11/core_pkcs11_pal.c"
)

target_link_libraries(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        3rdparty::mbedtls
        AFR::pkcs11_mbedtls
)

# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/BufferManagement/BufferAllocation_2.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/NetworkInterface/RX/ether_callback.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/NetworkInterface/RX/NetworkInterface.c"
        # Header files added to the target so that these are available in code downloaded from the FreeRTOS console.
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/Compiler/Renesas/pack_struct_end.h"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/Compiler/Renesas/pack_struct_start.h"
)

target_include_directories(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/Compiler/Renesas"
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE AFR::secure_sockets_freertos_plus_tcp
)

# Over-the-air Updates
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE
        "${rx65nrsk_ports_dir}/ota_pal_for_aws/ota_pal.c"
        "${rx65nrsk_ports_dir}/ota_pal_for_aws/ota_pal.h"
)

target_include_directories(
    AFR::ota::mcu_port
    INTERFACE "${rx65nrsk_ports_dir}/ota_pal_for_aws"
)

target_link_libraries(
    AFR::ota::mcu_port
    INTERFACE
        AFR::crypto
        AFR::ota
        AFR::pkcs11
)

if(AFR_ENABLE_TESTS)
    target_include_directories(
        AFR::ota::mcu_port
        INTERFACE "${PROJECT_SOURCE_DIR}/tests/integration_test/ota_pal"
    )
endif()

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX "abs")

afr_glob_src(board_code_src DIRECTORY "${rx65nrsk_aws_dir}/application_code/renesas_code")
afr_glob_src(config_files_src DIRECTORY "${rx65nrsk_aws_dir}/config_files")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(${exe_target}
        ${board_code_src}
        ${config_files_src}
        "${rx65nrsk_aws_dir}/application_code/main.c"
    )
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::freertos_plus_tcp
            AFR::utils
    )
endif()

if(NOT AFR_METADATA_MODE)
    message(FATAL_ERROR "CMake support for nxp is not complete yet.")
endif()
