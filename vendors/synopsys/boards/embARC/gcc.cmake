if(NOT DEFINED embARC_map_name)
    set(embARC_map_name "embARC_${embARC_BOARD}_${embARC_CUR_CORE}")
endif()

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

if(${AFR_TOOLCHAIN} STREQUAL "nsim")
    set(LIBGROSS_MACRO -U_HAVE_LIBGLOSS_)
else()
    set(LIBGROSS_MACRO _HAVE_LIBGLOSS_)
endif()

# message("gcc.cmake: ${CMAKE_BINARY_DIR}/build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}")
set(arg_dir
    ${CMAKE_BINARY_DIR}/build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}/embARC_generated/gcc.arg
    )

string(TOUPPER BOARD_${embARC_BOARD} BOARD_MACRO)
set(compiler_defined_symbols
    __GNU__
    ${BOARD_MACRO}
    HW_VERSION=${embARC_BOARD_VER}
    ${LIBGROSS_MACRO}
    LIB_CLIB
    LIB_CONSOLE
    EMBARC_TCF_GENERATED
)

# set(assembler_defined_symbols
#     __GNU__
#     ${BOARD_MACRO}
#     HW_VERSION=${embARC_BOARD_VER}
#     EMBARC_TCF_GENERATED
# )

# Get the directory of compiler out of the full path
# get_filename_component(compiler_dir ${CMAKE_C_COMPILER} DIRECTORY)

set(basic_flags
    @${arg_dir}
    -ffunction-sections
    -fdata-sections
    -mno-sdata
    -O2
    -g
)

set(extra_flags_c
    -std=gnu99
)

set(extra_flags_asm
    -x assembler-with-cpp
)

set(c_flags
    ${basic_flags}
    ${extra_flags_c}
)

set(cxx_flags
    ${basic_flags}
)

set(assembler_flags
    ${basic_flags}
    ${extra_flags_asm}
)

set(link_dependent_libs
    -Wl,--start-group ${CMAKE_BINARY_DIR}/build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}/libembarc.a -lm -lc -lgcc -Wl,--end-group
)

set(linker_flags
    @${arg_dir}
    -Wl,--gc-sections
    -mno-sdata
    -nostartfiles
    -Wl,-M,-Map=${embARC_map_name}.map
    -lm
    -Wl,--script=${CMAKE_BINARY_DIR}/build/linker_gnu.ldf
    # -Wl,--start-group
    # ${link_dependent_libs}
    # -lm -lc -lgcc
    # -Wl,--end-group
)

# unset(link_dependent_libs)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(compiler_specific_src
    ""
)

set(compiler_specific_include
    ""
)