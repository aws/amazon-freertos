# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

set(compiler_defined_symbols
    _DLIB_FILE_DESCRIPTOR
)

set(assembler_defined_symbols
    __ASSEMBLER__
)

# Get the directory of compiler out of the full path
get_filename_component(compiler_dir ${CMAKE_C_COMPILER} DIRECTORY)

set(basic_flags
    --no_cse  --no_unroll --no_inline --no_code_motion --no_tbaa --no_clustering --no_scheduling --debug --endian=little --cpu=Cortex-M4
    --dlib_config "${compiler_dir}/../inc/c/DLib_Config_Normal.h"
)

set(extra_flags
    --debug
    --silent
    -e
    --aeabi
    --thumb
    --diag_suppress=Pa050
    --fpu=None
)

set(compiler_flags
    ${basic_flags}
    ${extra_flags}
)

# "-M<>" is the default option to specify the left and right quotes of macro argument
# -r is automatically added to generate debug information
# Ref: http://netstorage.iar.com/SuppDB/Public/UPDINFO/012120/arm/doc/EWARM_AssemblerReference.ENU.pdf
set(assembler_flags "-s+" -M{} "-w-" "-j" "--cpu" "Cortex-M4" "--fpu" "None")

set(linker_flags
    --config "${simplelink_dir}/source/ti/boards/CC3220SF_LAUNCHXL/CC3220SF_LAUNCHXL_FREERTOS.icf"
    --semihosting --entry __iar_program_start --vfe --text_out locale
)

set(link_dependent_libs
    "${simplelink_dir}/source/ti/devices/cc32xx/driverlib/iar/Release/driverlib.a"
    "${simplelink_dir}/source/ti/drivers/lib/drivers_cc32xx.arm4"
    "${simplelink_dir}/source/ti/drivers/net/wifi/iar/rtos/simplelink.a"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(compiler_specific_src
    "${simplelink_dir}/kernel/freertos/startup/startup_cc32xx_iar.c"
    "${AFR_KERNEL_DIR}/portable/IAR/ARM_CM3/port.c"
    "${AFR_KERNEL_DIR}/portable/IAR/ARM_CM3/portasm.s"
    "${AFR_KERNEL_DIR}/portable/IAR/ARM_CM3/portmacro.h"
)

set(compiler_specific_include
    "${AFR_KERNEL_DIR}/portable/IAR/ARM_CM3"
)