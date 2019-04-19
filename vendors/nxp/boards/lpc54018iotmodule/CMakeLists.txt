set(lpc54018_dir "${AFR_VENDORS_DIR}/nxp/LPC54018")
set(lpc54018iotmodule_dir "${AFR_VENDORS_DIR}/nxp/boards/lpc54018iotmodule")

set(lpc54018_ports_dir "${lpc54018iotmodule_dir}/ports")
set(lpc54018_demos_common_dir "${lpc54018iotmodule_dir}/aws_demos/common")
set(lpc54018_tests_common_dir "${lpc54018iotmodule_dir}/aws_tests/common")

if(AFR_IS_TESTING)
    set(lpc54018_app_dir "${lpc54018_tests_common_dir}")
else()
    set(lpc54018_app_dir "${lpc54018_demos_common_dir}")
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
afr_set_board_metadata(IDE_IAREmbeddedWorkbench_NAME "IAR Embedded Workbench")
afr_set_board_metadata(IDE_IAREmbeddedWorkbench_COMPILER "IAR")
afr_set_board_metadata(IDE_IAREmbeddedWorkbench_RECOMMENDED "true")
afr_set_board_metadata(IDE_MCUXpresso_NAME "MCUXpresso")
afr_set_board_metadata(IDE_MCUXpresso_COMPILER "GCC")
afr_set_board_metadata(IDE_MCUXpresso_RECOMMENDED "false")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)

# Defined symbols
set(defined_symbols
    __REDLIB__
    MXL12835F
    __USE_CMSIS
    BOARD_DEBUG_UART_TYPE=DEBUG_CONSOLE_DEVICE_TYPE_USBCDC
    IMG_BAUDRATE=96000000
    XIP_IMAGE
    BOARD_USE_VIRTUALCOM
    USB_STACK_USE_DEDICATED_RAM=1
    SDK_DEBUGCONSOLE=1
    CPU_LPC54018JET180=1
    PRINTF_FLOAT_ENABLE=0
    USB_STACK_FREERTOS
    USE_RTOS=1
    FSL_RTOS_FREE_RTOS
    A_LITTLE_ENDIAN
    CR_INTEGER_PRINTF
    USB_DEVICE_CONFIG_LPCIP3511HS=1
    __MCUXPRESSO
    DEBUG
    SDK_OS_FREE_RTOS
    CPU_LPC54018JET180_cm4
    CPU_LPC54018JET180
)

target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${defined_symbols}>
)

set(common_flags "-mcpu=cortex-m4" "-mfpu=fpv4-sp-d16" "-mfloat-abi=hard" "-mthumb")

set(c_flags "-std=gnu99" "-O0" "-fno-common" "-g" "-Wall" "-c" "-ffunction-sections" "-fdata-sections" "-ffreestanding" "-fno-builtin" "-fomit-frame-pointer")

set(asm_flags "-c" "-x" "assembler-with-cpp")

# Compiler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        ${common_flags}
        $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${c_flags}>
        -specs=redlib.specs -MMD -MP
)

# Assembler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        ${common_flags}
        $<$<COMPILE_LANGUAGE:ASM>:${asm_flags}>
)

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
        -nostdlib
        "SHELL:-Xlinker -print-memory-usage" "SHELL:-Xlinker --gc-sections" "SHELL:-Xlinker -Map=aws_demos.map"
        ${common_flags}
)

target_link_directories(
    AFR::compiler::mcu_port
    INTERFACE
         "${lpc54018_dir}/mcuxpresso"
)

target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE
        "${lpc54018_dir}/mcuxpresso/libpower_hardabi.a"
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

set(mcuxpresso_src
    ${lpc54018_dir}/mcuxpresso/startup_lpc54018.c
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
        ${mcuxpresso_src}
        ${usb_device_src}
        ${usb_osa_src}
        ${wifi_qca_src}
        ${utilities_src}
        "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c"
        "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/portmacro.h"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
)

set(kernel_inc_dirs
    ${lpc54018_includes}
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F"
    "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3"
    "${AFR_KERNEL_DIR}/include"
    "${lpc54018_app_dir}/config_files"
    "${lpc54018_app_dir}/application_code/nxp_code"
    "$<IF:${AFR_IS_TESTING},${AFR_TEST_DIR},${AFR_DEMOS_DIR}>/include"

)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
)


# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${lpc54018_ports_dir}/wifi/aws_wifi.c"
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${lpc54018_ports_dir}/pkcs11/aws_pkcs11_pal.c"
        "${lpc54018_ports_dir}/pkcs11/hw_poll.c"
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
)

target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${lpc54018_ports_dir}/secure_sockets/aws_secure_sockets.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".axf")

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_glob_src(board_code_src DIRECTORY "${lpc54018_app_dir}/application_code/nxp_code")
afr_glob_src(config_files_src DIRECTORY "${lpc54018_app_dir}/config_files")

add_executable(${exe_target}
    ${board_code_src}
    ${config_files_src}
    "${lpc54018_app_dir}/application_code/main.c"
    # TODO, It is unknown why hw_poll.c has to be included here
    "${lpc54018_ports_dir}/pkcs11/hw_poll.c"
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
)

# TODO, Figure out how to generate those linker scripts.
# file(COPY "${lpc54018_dir}/mcuxpresso/aws_demos_Debug.ld" DESTINATION . )
# file(COPY "${lpc54018_dir}/mcuxpresso/aws_demos_Debug_memory.ld" DESTINATION . )
# file(COPY "${lpc54018_dir}/mcuxpresso/aws_demos_Debug_library.ld" DESTINATION . )
# target_link_options(
#     ${exe_target}
#     PRIVATE
#         -T "aws_demos_Debug.ld"
# )

# find_program(gcc_objcopy arm-none-eabi-objcopy)

# if(NOT gcc_objcopy)
#     message(FATAL_ERROR "Cannot find arm-none-eabi-objcopy.")
# endif()

# set(output_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.hex")
# add_custom_command(
#     TARGET ${exe_target} POST_BUILD
#     COMMAND "${gcc_objcopy}" -v -O ihex "$<TARGET_FILE:${exe_target}>" "${output_file}"
# )
