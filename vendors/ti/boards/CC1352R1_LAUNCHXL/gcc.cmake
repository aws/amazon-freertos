# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

set(compiler_defined_symbols
    DeviceFamily_CC13X2_CC26X2
    DeviceFamily_CC13X2
)

set(compiler_flags
    -mcpu=cortex-m4 -march=armv7e-m -mthumb -std=c99 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall
)

# The start-group flag is needed because some of the link_dependent_libs depend
# on the CMake built kernel, so the link order can't be configured.
set(linker_flags
    -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostartfiles -static -Wl,--gc-sections -lgcc -lc -lm -lnosys --specs=nano.specs --specs=nosys.specs -Wl,--start-group
)

set(link_dependent_libs
    ":source/ti/drivers/lib/drivers_cc13x2.am4fg"
    ":source/ti/drivers/rf/lib/rf_multiMode_cc13x2.am4fg"
    ":source/ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(compiler_specific_src
    "${simplelink_sdk_dir}/kernel/freertos/startup/startup_cc13x2_cc26x2_gcc.c"
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c"
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/portmacro.h"
)

set(compiler_specific_include
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

set(additional_linker_file_and_flags
    "-T${board_dir}/application_code/ti_code/cc13x2_cc26x2_freertos.lds"
)