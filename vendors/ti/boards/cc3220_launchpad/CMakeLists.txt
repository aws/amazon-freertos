set(simplelink_dir "${AFR_VENDORS_DIR}/ti/SimpleLink_CC32xx/v2_10_00_04")
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests/common")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
else()
    set(board_dir "${board_demos_dir}")
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
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
afr_set_board_metadata(IDE_CCS_PROJECT_LOCATION "${AFR_ROOT_DIR}/demos/ti/cc3220_launchpad/ccs")
afr_set_board_metadata(IDE_IAR_PROJECT_LOCATION "${AFR_ROOT_DIR}/demos/ti/cc3220_launchpad/iar")
afr_set_board_metadata(DEMO_COMMON_LOCATION "${AFR_ROOT_DIR}/demos/ti/cc3220_launchpad/common")
afr_set_board_metadata(THIRD_PARTY_LIB_LOCATION "${AFR_ROOT_DIR}/lib/third_party/mcu_vendor/ti")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE CC3220sf
)
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        -mv7M4 --code_state=16 --float_support=vfplib -me --diag_warning=225
)
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
        --heap_size=0x0 --stack_size=0x518
        --warn_sections --rom_model --reread_libs
        --diag_suppress=10063
)
target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE
        "${simplelink_dir}/source/ti/devices/cc32xx/driverlib/ccs/Release/driverlib.a"
        "${simplelink_dir}/source/ti/drivers/lib/drivers_cc32xx.aem4"
        "${simplelink_dir}/source/ti/drivers/net/wifi/ccs/rtos/simplelink.a"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Normally the portable layer for kernel should be vendor's driver code.
afr_mcu_port(kernel)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${simplelink_dir}/kernel/freertos/dpl/ClockP_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/DebugP_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/HwiPCC32XX_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/MutexP_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/PowerCC32XX_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/SemaphoreP_freertos.c"
        "${simplelink_dir}/kernel/freertos/dpl/SystemP_freertos.c"
        "${simplelink_dir}/kernel/freertos/startup/startup_cc32xx_ccs.c"
        "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3/port.c"
        "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3/portasm.asm"
        "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3/portmacro.h"
        "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3/portmacro.h"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
)
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${simplelink_dir}/source"
        "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3"
        "${board_dir}/config_files"
        "${board_dir}/application_code/ti_code"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TEST_DIR},${AFR_DEMOS_DIR}>/include"
)
target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE AFR::posix
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
    INTERFACE "${afr_ports_dir}/wifi/aws_wifi.c"
)
# WiFi on TI requires POSIX types
target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
        AFR::posix::mcu_port
        AFR::secure_sockets
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
    INTERFACE "${afr_ports_dir}/secure_sockets/aws_secure_sockets.c"
)

# OTA
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE "${afr_ports_dir}/ota/aws_ota_pal.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".out")

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/ti_code")
afr_glob_src(network_manager_src DIRECTORY "${AFR_DEMOS_DIR}/network_manager")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

add_executable(
    ${exe_target}
    ${board_code_src}
    ${config_files}
    "${board_dir}/application_code/main.c"
    ${network_manager_src}
    "${board_dir}/application_code/ti_code/CC3220SF_LAUNCHXL_FREERTOS.cmd"
)
target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
        AFR::posix::mcu_port
        --xml_link_info="${exe_target}_linkInfo.xml"
        "${board_dir}/application_code/ti_code/CC3220SF_LAUNCHXL_FREERTOS.cmd"
)
add_custom_command(
    TARGET ${exe_target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}"
)
