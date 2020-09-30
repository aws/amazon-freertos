# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

set(compiler_defined_symbols
    DeviceFamily_CC3220
)

#set(assembler_defined_symbols ${compiler_defined_symbols})

set(compiler_flags
    -mcpu=cortex-m4 -march=armv7e-m -mthumb -std=c99 -mfloat-abi=soft -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall
)

set(assembler_flags ${compiler_flags})

# The start-group flag is needed because some of the link_dependent_libs depend
# on the CMake built kernel, so the link order can't be configured.
set(linker_flags
    -march=armv7e-m -mthumb -mfloat-abi=soft -mfpu=fpv4-sp-d16 -nostartfiles -static -Wl,--gc-sections -lgcc -lc -lm -lnosys --specs=nano.specs --specs=nosys.specs -Wl,--start-group
)

#set(linker_flags
#    --heap_size=0x0 --stack_size=0x518
#    --warn_sections --rom_model --reread_libs
#   --diag_suppress=10063
#)

set(link_dependent_libs
    "${simplelink_sdk_dir}/source/ti/devices/cc32xx/driverlib/gcc/Release/driverlib.a"
    "${simplelink_sdk_dir}/source/ti/drivers/lib/drivers_cc32xx.am4g"
    "${simplelink_sdk_dir}/source/ti/drivers/net/wifi/gcc/rtos/simplelink.a"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(compiler_specific_src
    "${simplelink_sdk_dir}/kernel/freertos/startup/startup_cc32xx_gcc.c"
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM3/port.c"
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM3/portmacro.h"
)

set(compiler_specific_include
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM3"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

set(additional_linker_file_and_flags
    "-T${board_dir}/application_code/ti_code/cc32xxsf_freertos.lds"
)