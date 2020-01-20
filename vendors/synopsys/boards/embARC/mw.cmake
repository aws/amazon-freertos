if(NOT DEFINED embARC_map_name)
    set(embARC_map_name "embARC_${embARC_BOARD}_${embARC_CUR_CORE}")
endif()

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

# message("mw.cmake: ${CMAKE_BINARY_DIR}/build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}")
set(arg_dir
    ${CMAKE_BINARY_DIR}/build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}/embARC_generated/ccac.arg
    )

string(TOUPPER BOARD_${embARC_BOARD} BOARD_MACRO)
set(compiler_defined_symbols
    __MW__
    ${BOARD_MACRO}
    HW_VERSION=${embARC_BOARD_VER}
    LIB_CLIB
    LIB_CONSOLE
    EMBARC_TCF_GENERATED
)

set(basic_flags
    @${arg_dir}
    -Hnoccm
    -Hnosdata
    -Wincompatible-pointer-types
    -Hnocopyr
    -Hpurge
    -O2
    -g
)

set(extra_flags_c
    -Hnocplus
)

set(extra_flags_asm
    -Hasmcpp
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
    ${CMAKE_BINARY_DIR}/build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}/libembarc.a
)

set(linker_flags
    @${arg_dir}
    -Hpurge
    -Hnocopyr
    -Hnosdata
    -Hnocrt
    -Hldopt=-Coutput=${embARC_map_name}.map
    -Hldopt=-Csections
    -Hldopt=-Ccrossfunc
    -Hldopt=-Csize
    -zstdout
    -Hldopt=-Bgrouplib
    ${CMAKE_BINARY_DIR}/build/linker_mw.ldf
)


# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(compiler_specific_src
    ""
)

set(compiler_specific_include
    ""
)