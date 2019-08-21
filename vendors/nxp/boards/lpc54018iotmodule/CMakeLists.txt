# Set variables.
set(lpc54018_dir "${AFR_VENDORS_DIR}/nxp/LPC54018")
set(lpc54018iotmodule_dir "${AFR_VENDORS_DIR}/nxp/boards/lpc54018iotmodule")

set(lpc54018_ports_dir "${lpc54018iotmodule_dir}/ports")

if(AFR_IS_TESTING)
    set(lpc54018_aws_dir "${lpc54018iotmodule_dir}/aws_tests")
else()
    set(lpc54018_aws_dir "${lpc54018iotmodule_dir}/aws_demos")
endif()

# Include IDE specific cmake file.
if(${AFR_TOOLCHAIN} STREQUAL "arm-gcc")
    include("mcuxpresso.cmake")
elseif(${AFR_TOOLCHAIN} STREQUAL "arm-iar")
    include("iar.cmake")
elseif(NOT AFR_METADATA_MODE)
    message( FATAL_ERROR "The toolchain is not supported." )
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------

afr_set_board_metadata(ID "NXP-LPC54018-IoT-Module")
afr_set_board_metadata(DISPLAY_NAME "LPC54018 IoT Module")
afr_set_board_metadata(DESCRIPTION "Development kit for ARM® Cortex®-M4 based LPC54018 MCU")
afr_set_board_metadata(VENDOR_NAME "NXP")
afr_set_board_metadata(FAMILY_NAME "LPC5401x_54S0xx")
afr_set_board_metadata(CODE_SIGNER "null")
afr_set_board_metadata(DATA_RAM_MEMORY "360KB")
afr_set_board_metadata(PROGRAM_MEMORY "128MB")
afr_set_board_metadata(SUPPORTED_IDE "IAREmbeddedWorkbench;MCUXpresso")
afr_set_board_metadata(RECOMMENDED_IDE "IAREmbeddedWorkbench")
afr_set_board_metadata(IDE_IAREmbeddedWorkbench_NAME "IAR Embedded Workbench")
afr_set_board_metadata(IDE_IAREmbeddedWorkbench_COMPILER "IAR")
afr_set_board_metadata(IDE_MCUXpresso_NAME "MCUXpresso")
afr_set_board_metadata(IDE_MCUXpresso_COMPILER "GCC")

afr_set_board_metadata(IDE_IAREmbeddedWorkbench_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/nxp/lpc54018iotmodule/iar/aws_demos")
afr_set_board_metadata(IDE_MCUXpresso_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/nxp/lpc54018iotmodule/mcuxpresso/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${AFR_ROOT_DIR}/vendors/nxp/boards/lpc54018iotmodule/aws_demos/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)

# Compiler defined symbols.
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${compiler_defined_symbols}>
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
         "${link_directories}"
)

target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE
        "${link_dependent_libs}"
)

target_include_directories(
    AFR::compiler::mcu_port
    INTERFACE
        "${lpc54018_aws_dir}/config_files"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------


afr_glob_src(lpc54018_src DIRECTORY "${lpc54018_dir}")

afr_glob_src(usb_device_src DIRECTORY "${lpc54018_dir}/middleware/usb/device")

# All sources files under wifi_qca are needed
afr_glob_src(wifi_qca_src DIRECTORY "${lpc54018_dir}/middleware/wifi_qca" RECURSE)

set(usb_osa_src
    ${lpc54018_dir}/middleware/usb/osa/usb_osa_freertos.c
)

set(utilities_src
    ${lpc54018_dir}/utilities/str/fsl_str.c
    ${lpc54018_dir}/utilities/log/fsl_log.c
    ${lpc54018_dir}/utilities/io/swo/fsl_swo.c
    ${lpc54018_dir}/utilities/io/fsl_io.c
    ${lpc54018_dir}/utilities/fsl_assert.c
    ${lpc54018_dir}/utilities/fsl_debug_console.c
    ${lpc54018_dir}/utilities/usb_device_cdc_acm.c
    ${lpc54018_dir}/utilities/usb_device_ch9.c
    ${lpc54018_dir}/utilities/usb_device_descriptor.c
    ${lpc54018_dir}/utilities/virtual_com.c
)

set(drivers_src
    ${lpc54018_dir}/drivers/fsl_clock.c
    ${lpc54018_dir}/drivers/fsl_common.c
    ${lpc54018_dir}/drivers/fsl_dma.c
    ${lpc54018_dir}/drivers/fsl_emc.c
    ${lpc54018_dir}/drivers/fsl_flexcomm.c
    ${lpc54018_dir}/drivers/fsl_gpio.c
    ${lpc54018_dir}/drivers/fsl_inputmux.c
    ${lpc54018_dir}/drivers/fsl_pint.c
    ${lpc54018_dir}/drivers/fsl_power.c
    ${lpc54018_dir}/drivers/fsl_reset.c
    ${lpc54018_dir}/drivers/fsl_sha.c
    ${lpc54018_dir}/drivers/fsl_spi.c
    ${lpc54018_dir}/drivers/fsl_spi_dma.c
    ${lpc54018_dir}/drivers/fsl_spifi.c
    ${lpc54018_dir}/drivers/fsl_spifi_dma.c
    ${lpc54018_dir}/drivers/fsl_usart.c
    ${lpc54018_dir}/drivers/mflash_drv.c
    ${lpc54018_dir}/drivers/mflash_file.c
)

set(lpc54018_includes
    "${lpc54018_dir}"
    "${lpc54018_dir}/drivers"
    "${lpc54018_dir}/cmsis_drivers"
    "${lpc54018_dir}/middleware/wifi_qca"
    "${lpc54018_dir}/middleware/wifi_qca/common_src/hcd"
    "${lpc54018_dir}/middleware/wifi_qca/common_src/include"
    "${lpc54018_dir}/middleware/wifi_qca/common_src/stack_common"
    "${lpc54018_dir}/middleware/wifi_qca/common_src/wmi"
    "${lpc54018_dir}/middleware/wifi_qca/custom_src/include"
    "${lpc54018_dir}/middleware/wifi_qca/custom_src/stack_custom"
    "${lpc54018_dir}/middleware/wifi_qca/include"
    "${lpc54018_dir}/middleware/wifi_qca/include/AR6002"
    "${lpc54018_dir}/middleware/wifi_qca/include/AR6002/hw2.0/hw"
    "${lpc54018_dir}/middleware/wifi_qca/port"
    "${lpc54018_dir}/middleware/wifi_qca/port/boards/lpc54018iotmodule/freertos"
    "${lpc54018_dir}/middleware/wifi_qca/port/boards/lpc54018iotmodule/freertos/gt202"
    "${lpc54018_dir}/middleware/wifi_qca/port/drivers/flexcomm_freertos"
    "${lpc54018_dir}/middleware/wifi_qca/port/env/freertos"
    "${lpc54018_dir}/middleware/usb/device"
    "${lpc54018_dir}/middleware/usb/include"
    "${lpc54018_dir}/middleware/usb/osa"
    "${lpc54018_dir}/utilities"
    "${lpc54018_dir}/utilities/io"
    "${lpc54018_dir}/utilities/log"
    "${lpc54018_dir}/utilities/str"
    "${lpc54018_dir}/CMSIS/Include"
)

# Kernel
afr_mcu_port(kernel)

target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        ${lpc54018_src}
        ${drivers_src}
        ${usb_device_src}
        ${usb_osa_src}
        ${wifi_qca_src}
        ${utilities_src}
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        # Compiler specific source files.
        ${compiler_specific_src}
)

set(kernel_inc_dirs
    ${lpc54018_includes}
    "${AFR_KERNEL_DIR}/include"
    "${lpc54018_aws_dir}/config_files"
    "${lpc54018_aws_dir}/application_code/nxp_code"
    "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
    # Compiler specific includes.
    ${compiler_specific_include}
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
)

# POSIX

# TODO: Compile error with mcuxpresso tool chain
# "time_t" is already defined in the redlib
if(${AFR_TOOLCHAIN} STREQUAL "arm-iar")

afr_mcu_port(posix)

target_sources(
    AFR::posix::mcu_port
    INTERFACE "${lpc54018_ports_dir}/posix/FreeRTOS_POSIX_portable.h"
)

target_include_directories(
    AFR::posix::mcu_port
    INTERFACE "${lpc54018_ports_dir}/posix"
)

target_link_libraries(
    AFR::posix::mcu_port
    INTERFACE AFR::freertos_plus_posix
)

endif()

# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${lpc54018_ports_dir}/wifi/iot_wifi.c"
)

# PKCS11
afr_mcu_port(pkcs11_implementation)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${lpc54018_ports_dir}/pkcs11/iot_pkcs11_pal.c"
        "${lpc54018_ports_dir}/pkcs11/hw_poll.c"
)

target_link_libraries(
    AFR::pkcs11_implementation::mcu_port
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
)

target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${lpc54018_ports_dir}/secure_sockets/iot_secure_sockets.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ${executable_suffix})

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_glob_src(board_code_src DIRECTORY "${lpc54018_aws_dir}/application_code/nxp_code")
afr_glob_src(config_files_src DIRECTORY "${lpc54018_aws_dir}/config_files")

# TODO, remove network manager src.
afr_glob_src(network_manager_src DIRECTORY "${AFR_DEMOS_DIR}/network_manager")

add_executable(${exe_target}
    ${board_code_src}
    ${config_files_src}
    "${lpc54018_aws_dir}/application_code/main.c"
    # TODO, It is unknown why hw_poll.c has to be included here
    "${lpc54018_ports_dir}/pkcs11/hw_poll.c"
    $<$<NOT:${AFR_IS_TESTING}>:${network_manager_src}>
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
)

if(NOT AFR_METADATA_MODE)
    message(FATAL_ERROR "CMake support for nxp is not complete yet.")
endif()
