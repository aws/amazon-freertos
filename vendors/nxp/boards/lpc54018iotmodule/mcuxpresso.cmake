# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

set(compiler_defined_symbols
    __NEWLIB__
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

set(common_flags "-mcpu=cortex-m4" "-mfpu=fpv4-sp-d16" "-mfloat-abi=hard" "-mthumb")

set(c_flags "-std=gnu99" "-O0" "-fno-common" "-g" "-Wall" "-c" "-ffunction-sections" "-fdata-sections" "-ffreestanding" "-fno-builtin" "-fomit-frame-pointer" "-specs=newlib.specs" "-MMD" "-MP")

set(asm_flags "-c" "-x" "assembler-with-cpp")

set(compiler_flags
    ${common_flags}
    ${c_flags}
)

set(assembler_flags
    ${common_flags}
    ${asm_flags}
)

set(linker_flags
    -nostdlib
    "SHELL:-Xlinker -print-memory-usage" "SHELL:-Xlinker --gc-sections" "SHELL:-Xlinker -Map=aws_demos.map"
    ${common_flags}
)

set(link_directories
    "${lpc54018_dir}/mcuxpresso"
)

set(link_dependent_libs
    "${lpc54018_dir}/mcuxpresso/libpower_hardabi.a"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(compiler_specific_src
    ${lpc54018_dir}/mcuxpresso/startup_lpc54018.c
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c"
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/portmacro.h"
    # Add two auxiliary link scripts 
    ${lpc54018_dir}/mcuxpresso/linkscripts/data.ldt
    ${lpc54018_dir}/mcuxpresso/linkscripts/main_text.ldt
)

set(compiler_specific_include
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

set(executable_suffix ".axf")


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