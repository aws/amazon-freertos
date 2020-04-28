# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

set(compiler_defined_symbols
    CC3220sf
)

set(assembler_defined_symbols ${compiler_defined_symbols})

set(compiler_flags
    -mv7M4 --code_state=16 --float_support=vfplib -me --diag_warning=225
)

set(assembler_flags ${compiler_flags})

set(linker_flags
    --heap_size=0x0 --stack_size=0x518
    --warn_sections --rom_model --reread_libs
    --diag_suppress=10063
)

set(link_dependent_libs
    "${simplelink_dir}/source/ti/devices/cc32xx/driverlib/ccs/Release/driverlib.a"
    "${simplelink_dir}/source/ti/drivers/lib/drivers_cc32xx.aem4"
    "${simplelink_dir}/source/ti/drivers/net/wifi/ccs/rtos/simplelink.a"
)

# Force the use of response file to avoid "command line too long" error.
# These are needed to be put in this file as opposed to arm-ti.cmake because
# in that case the archiver starts to function incorrectly for mbedtls. The
# archiver starts to interpret the response file as object file and puts it in
# the archive as opposed to the actual object files written in that response
# file. The problem arises because for the TI archiver the response file flag is
# "@" while for the TI linker it is nothing (empty string).
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)

# Default response file flag is "@". TI's compiler requires it to be
# space (" "). Note that the empty string does not work.
set(CMAKE_C_RESPONSE_FILE_LINK_FLAG " ")
set(CMAKE_CXX_RESPONSE_FILE_LINK_FLAG " ")

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(compiler_specific_src
    "${simplelink_dir}/kernel/freertos/startup/startup_cc32xx_ccs.c"
    "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3/port.c"
    "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3/portasm.asm"
    "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3/portmacro.h"
)

set(compiler_specific_include
    "${AFR_KERNEL_DIR}/portable/CCS/ARM_CM3"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

set(link_extra_flags 
    --xml_link_info="${exe_target}_linkInfo.xml"
    "${board_dir}/application_code/ti_code/CC3220SF_LAUNCHXL_FREERTOS.cmd"
)