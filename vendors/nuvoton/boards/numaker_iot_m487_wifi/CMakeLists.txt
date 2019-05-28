set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests/common")
set(demos_uvision_dir ${CMAKE_CURRENT_LIST_DIR}/aws_demos/keil/)
set(tests_uvision_dir ${CMAKE_CURRENT_LIST_DIR}/aws_tests/keil/)

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(board_uvision_dir "${tests_uvision_dir}")
    set(exe_target aws_tests)
else()
    set(board_dir "${board_demos_dir}")
    set(board_uvision_dir "${demos_uvision_dir}")
    set(exe_target aws_demos)
endif()

#Ensure Defender is not included in build
set (AFR_MODULE_defender 0)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "numaker_iot_m487_wifi")
afr_set_board_metadata(DISPLAY_NAME "numaker_iot_m487_wifi Development Kit")
afr_set_board_metadata(DESCRIPTION "numaker_iot_m487_wifi Development Kit")
afr_set_board_metadata(VENDOR_NAME "Nuvoton")
afr_set_board_metadata(FAMILY_NAME "numaker_iot_m487_wifi Series")
afr_set_board_metadata(DATA_RAM_MEMORY "352KB")
afr_set_board_metadata(PROGRAM_MEMORY "4MB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "uVision")
afr_set_board_metadata(RECOMMENDED_IDE "uVision")
afr_set_board_metadata(IDE_uVision_NAME "Keil uVision")
afr_set_board_metadata(IDE_uVision_COMPILER "ARMCC")
afr_set_board_metadata(IS_ACTIVE "TRUE")

afr_set_board_metadata(IDE_uVision_PROJECT_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/keil")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
    -D__MICROLIB
    -D_REENT_SMALL 
    -DPRODUCT_VERSION=m487 
    -DCONFIG_REPEATER 
    -DSUPPORT_MBEDTLS 
    -DHAL_DFS_MODULE_ENABLED 
    -DLWIP_NO_STDINT_H=1 
    -DLWIP_TIMEVAL_PRIVATE=1
)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:ASM>:${assembler_defined_symbols}>
)

set (compiler_flags --c99 -c --cpu Cortex-M4.fp  -g -O0 --apcs=interwork --split_sections )
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
set(linker_flags --cpu Cortex-M4.fp --library_type=microlib --diag_suppress 550,177,C4017,111,2770,223 --strict --summary_stderr --info summarysizes,sizes,totals,unused,veneers --map --xref --callgraph --symbols )

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
        # Nuvoton SDK sources (driver)		
        "${AFR_VENDORS_DIR}/nuvoton/sdk/middleware/wifi_esp8266/esp8266_wifi.c"

        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/clk.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/crypto.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/fmc.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/sys.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/uart.c"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/src/retarget.c"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        "${AFR_KERNEL_DIR}/portable/RVDS/ARM_CM4F/port.c"
        "${board_dir}/application_code/nuvoton_code/entropy_hardware_poll.c"
 
        ${compiler_specific_src}
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/RVDS/ARM_CM4F"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/CMSIS/Include"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/Device/Nuvoton/numaker_iot_m487_wifi/Include"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/middleware/wifi_esp8266"
        "${AFR_VENDORS_DIR}/nuvoton/sdk/StdDriver/inc"
        "${board_dir}/config_files"
        "${board_dir}/application_code/nuvoton_code"
        "${board_dir}/application_code/nuvoton_code/include"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TEST_DIR},${AFR_DEMOS_DIR}>/include"
		
		
        "${AFR_MODULES_STANDARD_DIR}/mqtt/include"
        "${AFR_MODULES_STANDARD_DIR}/common/include"
        
        "${AFR_3RDPARTY_DIR}/lwip/src/include"
        "${AFR_3RDPARTY_DIR}/lwip/test/fuzz"
        "${AFR_3RDPARTY_DIR}/lwip/src/portable"
        "${AFR_3RDPARTY_DIR}/lwip/src/portable/arch"
		
        "${AFR_3RDPARTY_DIR}/pkcs11"
        "${AFR_ROOT_DIR}/modules/ports/secure_sockets/include"
        "${AFR_MODULES_STANDARD_DIR}/common/include/private"
        "${AFR_MODULES_STANDARD_DIR}/utils/include"
        "${AFR_MODULES_STANDARD_DIR}/provisioning/include"
        "${AFR_MODULES_PORTS_DIR}/pkcs11/include"
        "${AFR_MODULES_PORTS_DIR}/wifi/include"
        "${board_dir}/config_files"
        "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/Include"
        ${compiler_specific_include}
)


# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${afr_ports_dir}/wifi/aws_wifi.c"

    # LWIP files
    
    
    # Nuvoton SDK Wifi Files

)

target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
        "${AFR_VENDORS_DIR}/nuvoton/sdk/middleware/wifi_esp8266"        
        #"${AFR_3RDPARTY_DIR}/lwip/src/portable"
        #"${AFR_3RDPARTY_DIR}/lwip/src/include/lwip"
)

# WiFi on MTK requires Secure Sockets 
target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
        AFR::secure_sockets::mcu_port
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/aws_pkcs11_pal.c"
        "${AFR_MODULES_PORTS_DIR}/pkcs11/mbedtls/aws_pkcs11_mbedtls.c"
        "${AFR_MODULES_STANDARD_DIR}/crypto/src/aws_crypto.c"
)
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE 3rdparty::mbedtls
)

target_include_directories(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${AFR_MODULES_STANDARD_DIR}/crypto/include/"
        #"${board_dir}/application_code/mediatek_code/include"
)

#Secure sockets
afr_mcu_port(secure_sockets)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE 
        "${afr_ports_dir}/secure_sockets/aws_secure_sockets.c"
        "${AFR_ROOT_DIR}/modules/libraries/standard/tls/src/aws_tls.c"
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
    AFR::secure_sockets::mcu_port
    INTERFACE
        AFR::tls
)


# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".axf")

# TODO, remove network manager src.
afr_glob_src(network_manager_src DIRECTORY "${AFR_DEMOS_DIR}/network_manager")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

add_executable(
    ${exe_target}
    ${board_code_src}
    ${config_files}
    $<IF:${AFR_IS_TESTING}, ,${network_manager_src}>
    "${AFR_VENDORS_DIR}/nuvoton/sdk/Device/Nuvoton/numaker_iot_m487_wifi/Source/ARM/startup_M480.s"
	"${AFR_VENDORS_DIR}/nuvoton/sdk/Device/Nuvoton/numaker_iot_m487_wifi/Source/system_M480.c"
    "${board_dir}/application_code/main.c"	
	
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi::mcu_port
        AFR::utils       
        ${link_extra_flags}
        AFR::crypto
        AFR::pkcs11
)

# Convert afx file to bin file
afr_find_compiler(ARM_KEIL_FROMELF fromelf.exe)

add_custom_command(
    TARGET ${exe_target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}"
	COMMAND "${ARM_KEIL_FROMELF}" --bin "${exe_target}.axf" --output "${exe_target}.bin"
	COMMAND "${ARM_KEIL_FROMELF}" --text -c "${exe_target}.axf" --output "${exe_target}.txt"
)

