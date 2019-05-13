# Set variables.
set(xmc4800_dir "${AFR_VENDORS_DIR}/infineon/boards/xmc4800_iotkit")
set(xmc4800_ports_dir "${xmc4800_dir}/ports")
set(xmclib_dir "${AFR_VENDORS_DIR}/infineon/XMCLib/2.1.20")

if(AFR_IS_TESTING)
    set(xmc4800_aws_dir "${xmc4800_dir}/aws_tests")
else()
    set(xmc4800_aws_dir "${xmc4800_dir}/aws_demos")
endif()

# Include IDE specific cmake file.
if(${AFR_TOOLCHAIN} STREQUAL "arm-gcc")
    include("xmc4800_gcc.cmake")
elseif(NOT AFR_METADATA_MODE)
    message( FATAL_ERROR "The toolchain is not supported." )
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------

afr_set_board_metadata(ID "Infineon-XMC4800-IoT-Kit")
afr_set_board_metadata(DISPLAY_NAME "XMC4800 IoT Connectivity Kit")
afr_set_board_metadata(DESCRIPTION "Development kit for ARM® Cortex®-M4 based XMC4800 MCU")
afr_set_board_metadata(VENDOR_NAME "Infineon")
afr_set_board_metadata(FAMILY_NAME "XMC4x00")
afr_set_board_metadata(CODE_SIGNER "null")
afr_set_board_metadata(DATA_RAM_MEMORY "353KB")
afr_set_board_metadata(PROGRAM_MEMORY "2MB")
afr_set_board_metadata(SUPPORTED_IDE "DAVE")
afr_set_board_metadata(RECOMMENDED_IDE "DAVE")
afr_set_board_metadata(IDE_DAVE_NAME "DAVE")
afr_set_board_metadata(IDE_DAVE_COMPILER "GCC")

afr_set_board_metadata(IDE_DAVE_PROJECT_LOCATION "${AFR_ROOT_DIR}/vendors/infineon/boards/xmc4800_iotkit/aws_demos/dave4")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${AFR_ROOT_DIR}/vendors/infineon/boards/xmc4800_iotkit/aws_demos/common/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)

# Compiler defined symbols.
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE
	${compiler_defined_symbols}>
)

# Compiler flags.
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${compiler_flags}>
)

# Assembler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:ASM>:${assembler_flags}>
)

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
        ${linker_flags}
)

target_link_directories(
    AFR::compiler::mcu_port
    INTERFACE
         ${link_directories}
)

target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE
        ${linker_libs}
)

target_include_directories(
    AFR::compiler::mcu_port
    INTERFACE
        "${xmc4800_aws_dir}/common/config_files"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------


# Kernel
afr_mcu_port(kernel)

afr_glob_src(drivers_src DIRECTORY "${xmclib_dir}/drivers/src")
afr_glob_src(esp_at_lib  RECURSE DIRECTORY "${xmclib_dir}/third_party/ESP_AT_Lib/src")
afr_glob_src(newlib DIRECTORY "${xmclib_dir}/Newlib")

set( xmclib_includes
     "${xmclib_dir}/CMSIS/Include"
	 "${xmclib_dir}/drivers/inc"
	 "${xmclib_dir}/third_party/ESP_AT_Lib/src/include"
)

target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
		${driver_src}
		${esp_at_lib}
		${newlib}
        ${compiler_specific_src}
)

set(kernel_inc_dirs
    "${AFR_KERNEL_DIR}/include"
    "${xmc4800_aws_dir}/common/config_files"
    "${xmc4800_aws_dir}/common/application_code/infineon_code"
    "$<IF:${AFR_IS_TESTING},${AFR_TEST_DIR},${AFR_DEMOS_DIR}>/include"
	${xmclib_includes}
    ${compiler_specific_include}
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
)

# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${xmc4800_ports_dir}/wifi/aws_wifi.c"
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${xmc4800_ports_dir}/pkcs11/aws_pkcs11_pal.c"
)

target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE
        3rdparty::mbedtls
        AFR::pkcs11_mbedtls
        AFR::crypto
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE
        AFR::tls
        AFR::wifi
		AFR::pkcs11
)

target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${xmc4800_ports_dir}/secure_sockets/aws_secure_sockets.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ${executable_suffix})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_glob_src(board_src DIRECTORY "${xmc4800_aws_dir}/common/application_code/infineon_code")
afr_glob_src(config_files_src DIRECTORY "${xmc4800_aws_dir}/common/config_files")

# TODO, remove network manager src.
afr_glob_src(network_manager_src DIRECTORY "${AFR_DEMOS_DIR}/network_manager")

add_executable(${exe_target}
    ${xmc4800_board_src}
    ${config_files_src}
    "${xmc4800_aws_dir}/common/application_code/main.c"
    $<$<NOT:${AFR_IS_TESTING}>:${network_manager_src}>
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
)

# Disable device defender
afr_module_dependencies(
    defender
    PRIVATE
        3rdparty::dummy
)