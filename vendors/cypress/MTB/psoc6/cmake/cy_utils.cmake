cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0025 NEW)

# extensions we care about...
set(CY_HDR_EXT "h")
set(CY_SRC_EXT "c;cpp;s;S;asm")
set(CY_LIB_EXT "o;a;ar")
set(CY_IGNORE_EXT "cyignore")

# extensions in shell glob form
set(CY_HDR_GLOB ${CY_HDR_EXT})
list(TRANSFORM CY_HDR_GLOB PREPEND "*.")

set(CY_SRC_GLOB ${CY_SRC_EXT})
list(TRANSFORM CY_SRC_GLOB PREPEND "*.")

set(CY_LIB_GLOB ${CY_LIB_EXT})
list(TRANSFORM CY_LIB_GLOB PREPEND  "*.")

set(CY_IGNORE_GLOB ${CY_IGNORE_EXT})
list(TRANSFORM CY_IGNORE_GLOB PREPEND "*.")

set(CY_ALL_GLOB ${CY_HDR_GLOB} ${CY_SRC_GLOB} ${CY_LIB_GLOB} ${CY_IGNORE_GLOB})

# extensions in regex form
set(CY_HDR_RE ${CY_HDR_EXT})
list(JOIN CY_HDR_RE "|" CY_HDR_RE)
set(CY_HDR_RE ".*\\.(${CY_HDR_RE})$")

set(CY_SRC_RE ${CY_SRC_EXT})
list(JOIN CY_SRC_RE "|" CY_SRC_RE)
set(CY_SRC_RE ".*\\.(${CY_SRC_RE})$")

set(CY_LIB_RE ${CY_LIB_EXT})
list(JOIN CY_LIB_RE "|" CY_LIB_RE)
set(CY_LIB_RE ".*\\.(${CY_LIB_RE})$")

set(CY_IGNORE_RE ${CY_IGNORE_EXT})
list(JOIN CY_IGNORE_RE "|" CY_IGNORE_RE)
set(CY_IGNORE_RE ".*\\.(${CY_IGNORE_RE})$")

function(cy_cfg_env)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "TARGET;DEVICE;TOOLCHAIN;LINKER_PATH;LINKER_SCRIPT;COMPONENTS;ARCH_DIR"
    ""
    )

    if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "Building directly in the source folder not supported -- see cmake -S and -B arguments.")
    endif()

    if(NOT DEFINED ARG_TARGET)
    message(FATAL_ERROR "Missing TARGET argument.")
    endif()

    if(NOT DEFINED ARG_DEVICE)
    message(FATAL_ERROR "Missing DEVICE argument.")
    endif()

    if(NOT DEFINED ARG_TOOLCHAIN)
    message(FATAL_ERROR "Missing TOOLCHAIN argument.")
    endif()

    if(NOT DEFINED ARG_ARCH_DIR)
    message(FATAL_ERROR "Missing ARCH_DIR argument.")
    endif()
    
    if(NOT DEFINED ARG_LINKER_SCRIPT)
    message(FATAL_ERROR "Missing LINKER_SCRIPT argument.")
    endif()

    # COMPONENTS is optional

    #####################################
    # set CY_TARGET
    set(ENV{CY_TARGET} "${ARG_TARGET}")

    #####################################
    # set CY_DEVICE
    set(ENV{CY_DEVICE} "${ARG_DEVICE}")

    #####################################
    # set CY_CORE
    if ("${CORE}" STREQUAL "")
    set(ENV{CY_CORE} "CM4")
    else()
    set(ENV{CY_CORE} "${CORE}")
    endif()

    #####################################
    # set CY_TOOLCHAIN
    # map AFR toolchain names to cypress toolchain names
    # TODO make this support multiple toolchain and flash memory sizes
    if ("${ARG_TOOLCHAIN}" STREQUAL "arm-gcc")
      set(ARG_TOOLCHAIN GCC_ARM)
    elseif("${ARG_TOOLCHAIN}" STREQUAL "arm-armclang")
      set(ARG_TOOLCHAIN ARM)
    elseif("${ARG_TOOLCHAIN}" STREQUAL "arm-iar")
      set(ARG_TOOLCHAIN IAR)
    endif()
    set(ENV{CY_TOOLCHAIN} "${ARG_TOOLCHAIN}")
    
    #####################################
    # set CY_LINKER_PATH
    set(ENV{CY_LINKER_PATH} "${ARG_LINKER_PATH}")
    
    #####################################
    # set CY_LINKER_SCRIPT
    set(ENV{CY_LINKER_SCRIPT} "${ARG_LINKER_SCRIPT}")
    
    #####################################
    # set CY_COMPONENTS
    set(ENV{CY_COMPONENTS} "")
    cy_cfg_components(ADD "${ARG_COMPONENTS}") # added by the BSP
    cy_cfg_components(ADD "$ENV{CY_CORE}")
    cy_cfg_components(ADD "${COMPONENTS}")     # added by the user via cmake -DCOMPONENTS="C1;C2"

    #####################################
    # set CY_ARCH_DIR
    set(ENV{CY_ARCH_DIR} "${ARG_ARCH_DIR}")

    cy_cfg_toolchain()
endfunction()

function(cy_cfg_components)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "ADD;REMOVE"
    ""
    )
    
    # cmake list chokes on lists in environment variables. :-/
    set(comps "$ENV{CY_COMPONENTS}")

    if(DEFINED ARG_ADD)
    list(APPEND comps "${ARG_ADD}")
    endif()

    if(DEFINED ARG_REMOVE)
    foreach(to_remove IN LISTS ARG_REMOVE)
        list(REMOVE_ITEM comps "${to_remove}")
    endforeach()
    endif()
 
    # make sure components excluded by the user via cmake -DEXCLUDE_COMPONENTS="C1;C2" aren't added
    if(DEFINED EXCLUDE_COMPONENTS)
    foreach(to_remove IN LISTS EXCLUDE_COMPONENTS)
        list(REMOVE_ITEM comps "${to_remove}")
    endforeach()
    endif()

    # remove empties, dups, and sort
    list(FILTER comps EXCLUDE REGEX "^[ \t\r\n]*$")
    list(SORT comps)
    list(REMOVE_DUPLICATES comps)

    set(ENV{CY_COMPONENTS} "${comps}")
endfunction()


function(cy_filter_dirs arg_files)
    cmake_parse_arguments(
    PARSE_ARGV 1
    "ARG"
    ""
    "PREFIX;VALUES;ITEMS"
    ""
    )

    if(NOT DEFINED ARG_PREFIX)
    message(FATAL_ERROR "Missing PREFIX argument.")
    endif()

    if(NOT DEFINED ARG_ITEMS)
    message(FATAL_ERROR "Missing ITEMS argument.")
    endif()

    set(rslt "")

    if(DEFINED ARG_VALUES)
    foreach(value IN LISTS ARG_VALUES)
        set(tmp "${ARG_ITEMS}")
        list(FILTER tmp INCLUDE REGEX "^${ARG_PREFIX}_${value}/")
        list(APPEND rslt "${tmp}")
        
        set(tmp "${ARG_ITEMS}")
        list(FILTER tmp INCLUDE REGEX "/${ARG_PREFIX}_${value}/")
        list(APPEND rslt "${tmp}")
    endforeach()
    endif()

    set(tmp "${ARG_ITEMS}")
    list(FILTER tmp EXCLUDE REGEX "^${ARG_PREFIX}_[^/]*/")
    list(FILTER tmp EXCLUDE REGEX "/${ARG_PREFIX}_[^/]*/")
    list(APPEND rslt "${tmp}")

    set(${arg_files} "${rslt}" PARENT_SCOPE)
endfunction()

function(cy_filter_ignored arg_files)
    cmake_parse_arguments(
    PARSE_ARGV 1
    "ARG"
    ""
    "ITEMS"
    ""
    )
    if(NOT DEFINED ARG_ITEMS)
    message(FATAL_ERROR "Missing ITEMS argument.")
    endif()

    set(dot_ignores "${ARG_ITEMS}")
    list(FILTER dot_ignores INCLUDE REGEX ${CY_IGNORE_RE})

    set(ignore_list "")
    foreach(ignore_file IN LISTS dot_ignores)
        get_filename_component(cyignore_dir ${ignore_file} DIRECTORY)

        file(STRINGS ${ignore_file} contents)
        list(FILTER contents EXCLUDE REGEX "^[ \t\r\n]*$")
        string(REPLACE "\$CY_APP_PATH" "${CMAKE_SOURCE_DIR}" contents "${contents}")
        list(TRANSFORM contents PREPEND ${cyignore_dir}/)

        if(IS_DIRECTORY contents)
            list(APPEND ignore_list "${contents}/*")
        else()
            list(APPEND ignore_list "${contents}")
        endif()
    endforeach()
    list(FILTER ignore_list EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")
    list(FILTER ignore_list EXCLUDE REGEX "^[ \t\r\n]*$")

    foreach(to_ignore IN LISTS ignore_list)
        list(FILTER ARG_ITEMS EXCLUDE REGEX "${to_ignore}.*")
    endforeach()

    set(${arg_files} "${ARG_ITEMS}" PARENT_SCOPE)
endfunction()

function(cy_find_files arg_files)
    cmake_parse_arguments(
    PARSE_ARGV 1
    "ARG"
    ""
    "DIRECTORY"
    ""  
    )

    if(NOT DEFINED ARG_DIRECTORY)
    message(FATAL_ERROR "Missing DIRECTORY argument.")
    endif()

    set(file_list "")
    
    foreach(glob_exp ${CY_ALL_GLOB})
        foreach(dir_exp IN LISTS ARG_DIRECTORY)
            file(
                GLOB_RECURSE
                glob_result
                LIST_DIRECTORIES false
                CONFIGURE_DEPENDS
                "${dir_exp}/${glob_exp}"
            )
        list(APPEND file_list "${glob_result}")
        endforeach()
    endforeach()
    list(REMOVE_DUPLICATES file_list)

    list(FILTER file_list EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")
    list(FILTER file_list EXCLUDE REGEX "^[ \t\r\n]*$")
    list(SORT file_list)

    cy_filter_dirs(file_list PREFIX "TARGET" VALUES "$ENV{CY_TARGET}" ITEMS "${file_list}")
    cy_filter_dirs(file_list PREFIX "TOOLCHAIN" VALUES "$ENV{CY_TOOLCHAIN}" ITEMS "${file_list}")
    cy_filter_dirs(file_list PREFIX "COMPONENT" VALUES "$ENV{CY_COMPONENTS}" ITEMS "${file_list}")
    cy_filter_ignored(file_list ITEMS "${file_list}")

    set(${arg_files} "${file_list}" PARENT_SCOPE)
endfunction()

function(cy_get_src arg_files)
    cmake_parse_arguments(
    PARSE_ARGV 1
    "ARG"
    ""
    "ITEMS"
    ""
    )

    if(NOT DEFINED ARG_ITEMS)
    message(FATAL_ERROR "Missing ITEMS argument.")
    endif()

    set(src "${ARG_ITEMS}")
    list(FILTER src INCLUDE REGEX ${CY_SRC_RE})

    set(hdr "${ARG_ITEMS}")
    list(FILTER hdr INCLUDE REGEX ${CY_HDR_RE})

    set(rslt ${src} ${hdr})
    list(FILTER rslt EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")
    list(FILTER rslt EXCLUDE REGEX "^[ \t\r\n]*$")
    list(SORT rslt)
    
    set(${arg_files} "${rslt}" PARENT_SCOPE)
endfunction()

function(cy_get_includes arg_files)
    cmake_parse_arguments(
    PARSE_ARGV 1
    "ARG"
    "IMPLICIT_ROOT"
    "ROOT;ITEMS"
    ""
    )

    if(NOT DEFINED ARG_ROOT)
    set(ARG_ROOT "${CMAKE_SOURCE_DIR}")
    endif()

    if(NOT DEFINED ARG_ITEMS)
    message(FATAL_ERROR "Missing ITEMS argument.")
    endif()

    set(to_search "${ARG_ITEMS}")
    list(FILTER to_search INCLUDE REGEX ${CY_HDR_RE})

    set(includes "")
    foreach(header IN LISTS to_search)
        set(dir "${header}")
        get_filename_component(dir "${dir}" DIRECTORY)
    
        list(APPEND includes "${dir}")
        while(NOT "${dir}" IN_LIST ARG_ROOT)
            get_filename_component(dir "${dir}" DIRECTORY)
            list(APPEND includes "${dir}")
        endwhile()
    endforeach()

    if(${ARG_IMPLICIT_ROOT})
    list(APPEND includes "${ARG_ROOT}")
    endif()

    list(REMOVE_DUPLICATES includes)
    list(FILTER includes EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")
    list(FILTER includes EXCLUDE REGEX "^[ \t\r\n]*$")
    list(SORT includes)

    set(${arg_files} "${includes}" PARENT_SCOPE)
endfunction()

function(cy_get_libs arg_files)
    cmake_parse_arguments(
    PARSE_ARGV 1
    "ARG"
    ""
    "ITEMS"
    ""
    )

    if (NOT DEFINED ARG_ITEMS)
    message(FATAL_ERROR "Missing ITEMS argument.")
    endif()

    set(libs "${ARG_ITEMS}")
    list(FILTER libs INCLUDE REGEX ${CY_LIB_RE})
    list(FILTER libs EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")
    list(FILTER libs EXCLUDE REGEX "^[ \t\r\n]*$")

    set(${arg_files} "${libs}" PARENT_SCOPE)
endfunction()

function(cy_add_std_defines target mode)
    string(REGEX REPLACE - _ tgt_def $ENV{CY_TARGET})
    string(REGEX REPLACE - _ tgt_mpn $ENV{CY_DEVICE})
    target_compile_definitions(
    ${target}
    ${mode}
    ${tgt_mpn}
    TARGET_${tgt_def}
    )

    foreach(comp IN LISTS CY_COMPONENTS)
        target_compile_definitions(
            ${target}
            ${mode}
            COMPONENT_${comp}
        )
    endforeach()
endfunction()

function(cy_add_args_to_target target mode)
    cmake_parse_arguments(
    PARSE_ARGV 2
    "ARG"
    ""
    "OPTIMIZATION;DEBUG_FLAG;DEFINE_FLAGS;COMMON_FLAGS;VFP_FLAGS;CORE_FLAGS;CFLAGS;CXXFLAGS;ASFLAGS;LDFLAGS"
    ""
    )

    set(_all_cflags ${ARG_CORE_FLAGS} ${ARG_OPTIMIZATION} ${ARG_VFP_FLAGS} ${ARG_COMMON_FLAGS})
    target_compile_options(
        ${target}
        ${mode}
        $<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:CXX>>:${_all_cflags}>
    )

    target_compile_options(
        ${target}
        ${mode}
        $<$<COMPILE_LANGUAGE:ASM>:${ARG_ASFLAGS}>
    )

    if("${CMAKE_ASM_COMPILER}" STREQUAL "${CMAKE_C_COMPILER}")
        # If using compiler frontend for assembler then pass cflags when assembling
        target_compile_options(
            ${target}
            ${mode}
            $<$<COMPILE_LANGUAGE:ASM>:${_all_cflags}>
        )
    endif()

    target_compile_definitions(
        ${target}
        ${mode}
        $<$<CONFIG:Debug>:${ARG_DEBUG_FLAG}>
        ${ARG_DEFINE_FLAGS}
    )

    if("${LINKER_SCRIPT}" STREQUAL "")
    set(linker_script "$ENV{CY_LINKER_PATH}/COMPONENT_$ENV{CY_CORE}/TOOLCHAIN_$ENV{CY_TOOLCHAIN}/$ENV{CY_LINKER_SCRIPT}$ENV{CY_LINKERSCRIPT_EXT}")
    else()
    set(linker_script "${LINKER_SCRIPT}")
    endif()

    if("$ENV{CY_TOOLCHAIN}" STREQUAL "IAR")
        # IAR has a separate linker command that does not use the same options as the compiler.
        target_link_options(
            ${target}
            ${mode}
            ${ARG_CORE_FLAGS}
            ${ARG_VFP_FLAGS}
            ${ARG_LDFLAGS}
            --config ${linker_script}
        )
    elseif("$ENV{CY_TOOLCHAIN}" STREQUAL "ARM")
        # ARM has a separate linker command that does not use the same options as the compiler.
        target_link_options(
            ${target}
            ${mode}
            ${ARG_LDFLAGS}
            --scatter=${linker_script}
        )
    else()
        # GNU based toolchain
        target_link_options(
            ${target}
            ${mode}
            ${ARG_CORE_FLAGS}
            ${ARG_VFP_FLAGS}
            ${ARG_COMMON_FLAGS}
            ${ARG_LDFLAGS}
            -T${linker_script}
        )
    endif()
endfunction()

function(cy_cfg_mtb_target)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    "PUBLIC;INTERFACE;PRIVATE"
    "CREATE;CONFIG;OPTIMIZATION;DEBUG_FLAG;DEFINE_FLAGS;COMMON_FLAGS;VFP_FLAGS;CORE_FLAGS;LDFLAGS"
    ""
    )

    if ((DEFINED ARG_CREATE) AND (DEFINED ARG_CONFIG))
    message(FATAL_ERROR "CREATE <name> and CONFIG <name> are mutually excusive")
    endif()

    if ((NOT DEFINED ARG_CREATE) AND (NOT DEFINED ARG_CONFIG))
    message(FATAL_ERROR "Missing argument CREATE <name> or CONFIG <name>.")
    endif()

    if(DEFINED ARG_CREATE)
    set(name ${ARG_CREATE})
    elseif(DEFINED ARG_CONFIG)
    set(name ${ARG_CONFIG})
    endif()

    if(${ARG_PUBLIC})
    set(mode PUBLIC)
    elseif(${ARG_PRIVATE})
    set(mode PRIVATE)
    else()
    set(mode INTERFACE)
    endif()
    
    cy_find_files(mtb_files DIRECTORY "$ENV{CY_ARCH_DIR}")
    cy_get_src(mtb_src ITEMS "${mtb_files}")
    cy_get_includes(mtb_inc ITEMS "${mtb_files}" ROOT "$ENV{CY_ARCH_DIR}")
    cy_get_libs(mtb_libs ITEMS "${mtb_files}")

    if(DEFINED ARG_CREATE)
    add_library(${name} OBJECT ${mtb_src})
    endif()

    cy_add_std_defines(${name} ${mode})

    if((DEFINED mtb_inc) AND (NOT "${mtb_inc}" STREQUAL ""))
    target_include_directories(${name} ${mode} ${mtb_inc})
    endif()

    if ((DEFINED mtb_libs) AND (NOT "${mtb_libs}" STREQUAL ""))
    target_link_libraries(${name} ${mode} ${mtb_libs})
    
    foreach(lib IN LISTS mtb_libs)
        get_filename_component(dir "${lib}" DIRECTORY)
        target_link_directories(
        "${name}"
        "${mode}"
        "${dir}"
        )
    endforeach()
    endif()

    cy_add_args_to_target(${name} ${mode}
    OPTIMIZATION "$ENV{OPTIMIZATION}"
    DEBUG_FLAG "$ENV{DEBUG_FLAG}"
    DEFINE_FLAGS "$ENV{DEFINE_FLAGS}"
    COMMON_FLAGS "$ENV{COMMON_FLAGS}"
    VFP_FLAGS "$ENV{VFP_FLAGS}"
    CORE_FLAGS "$ENV{CORE_FLAGS}"
    ASFLAGS "$ENV{ASFLAGS}"
    LDFLAGS "$ENV{LDFLAGS}")
endfunction()

