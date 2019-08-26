# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

set(compiler_defined_symbols
    DEBUG
    XIP_IMAGE
    CPU_LPC54018
    SDK_DEBUGCONSOLE=1
    BOARD_USE_VIRTUALCOM
    USB_STACK_FREERTOS
    USB_STACK_USE_DEDICATED_RAM=1
    BOARD_DEBUG_UART_TYPE=DEBUG_CONSOLE_DEVICE_TYPE_USBCDC
    CPU_LPC54018JET180=1
    USE_RTOS=1
    FSL_RTOS_FREE_RTOS
    A_LITTLE_ENDIAN
    MXL12835F
    IMG_BAUDRATE=96000000
    # tinycbor cbor.h references "FILE"
    _DLIB_FILE_DESCRIPTOR=1
)

# Get the directory of compiler out of the full path
get_filename_component(compiler_dir ${CMAKE_C_COMPILER} DIRECTORY)


set(compiler_flags
    "--cpu=Cortex-M4" "--fpu=VFPv4_sp"
    "--diag_suppress" "Pa082,Pa050" "--no_cse" "--no_unroll" "--no_inline" "--no_code_motion"
    "--no_tbaa" "--no_clustering" "--no_scheduling" "--endian=little" "-e"
    "--dlib_config" "${compiler_dir}/../inc/c/DLib_Config_Normal.h"
)

set(assembler_flags "-s+" "-M{}" "-w-" "-j" "-DDEBUG" "-DMXL12835F" "--cpu" "Cortex-M4" "--fpu" "VFPv4_sp")

set(linker_flags
    "--config_def" "XIP_IMAGE=1"
    "--config" "${lpc54018_dir}/iar/LPC54018_spifi_flash_data.icf"
    "--semihosting" "--entry" "__iar_program_start" "--vfe" 
    "--text_out" "locale"
)

set(link_directories
    "${lpc54018_dir}/iar" 
)

set(link_dependent_libs
    "${lpc54018_dir}/iar/iar_lib_power.a"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(compiler_specific_src
    ${lpc54018_dir}/iar/startup_LPC54018.s
    "${AFR_KERNEL_DIR}/portable/IAR/ARM_CM4F/port.c"
    "${AFR_KERNEL_DIR}/portable/IAR/ARM_CM4F/portmacro.h"
    "${AFR_KERNEL_DIR}/portable/IAR/ARM_CM4F/portasm.s"
)

set(compiler_specific_include
    "${AFR_KERNEL_DIR}/portable/IAR/ARM_CM4F"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

set(executable_suffix ".out")
