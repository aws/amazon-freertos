# -------------------------------------------------------------------------------------------------
# Utilities
# -------------------------------------------------------------------------------------------------
function(afr_get_boards arg_boards)
    set(vendors_dir "${CMAKE_CURRENT_LIST_DIR}/cmake/vendors")
    file(GLOB __vendors RELATIVE "${vendors_dir}" "${vendors_dir}/*")
    foreach(__vendor IN LISTS __vendors)
        file(GLOB __boards RELATIVE "${vendors_dir}/${__vendor}" "${vendors_dir}/${__vendor}/*")
        foreach(__board IN LISTS __boards)
            list(APPEND ${arg_boards} "${__vendor}.${__board}")
        endforeach()
    endforeach()

    set(${arg_boards} "${${arg_boards}}" PARENT_SCOPE)
endfunction()

function(afr_cache_append arg_cache_var)
    if(AFR_DEBUG_CMAKE)
        # Verify ${arg_cache_var} is indeed a cache entry.
        get_property(__type CACHE ${arg_cache_var} PROPERTY TYPE)
        if(NOT __type)
            message(FATAL_ERROR "${arg_cache_var} is not a cache entry.")
        endif()

        # Check duplicate.
        foreach(__item IN ITEMS ${ARGN})
            if(${__item} IN_LIST ${arg_cache_var})
                message(WARNING "Item ${__item} already in list ${arg_cache_var}.")
            endif()
        endforeach()
    endif()

    list(APPEND ${arg_cache_var} ${ARGN})
    get_property(__docstring CACHE ${arg_cache_var} PROPERTY HELPSTRING)
    set(${arg_cache_var} "${${arg_cache_var}}" CACHE INTERNAL "${__docstring}")
endfunction()

function(afr_cache_remove arg_cache_var)
    if(AFR_DEBUG_CMAKE)
        # Verify ${arg_cache_var} is indeed a cache entry.
        get_property(__type CACHE ${arg_cache_var} PROPERTY TYPE)
        if(NOT __type)
            message(FATAL_ERROR "${arg_cache_var} is not a cache entry.")
        endif()
    endif()

    if(ARGN)
        list(REMOVE_ITEM ${arg_cache_var} ${ARGN})
        get_property(__docstring CACHE ${arg_cache_var} PROPERTY HELPSTRING)
        set(${arg_cache_var} "${${arg_cache_var}}" CACHE INTERNAL "${__docstring}")
    endif()
endfunction()

# Gather files under a folder.
function(afr_glob_files arg_files)
    cmake_parse_arguments(
        PARSE_ARGV 1
        "ARG"               # Prefix results with "ARG_".
        "RECURSE"           # Whether to traverse all subdirectories.
        "DIRECTORY;GLOBS"   # Specify folder location and semicolon separated glob expressions.
        ""                  # No multi-value arguments.
    )

    if(NOT DEFINED ARG_DIRECTORY)
        message(FATAL_ERROR "Missing DIRECTORY arguments.")
    endif()

    if(ARG_RECURSE)
        set(__glob_mode GLOB_RECURSE)
    else()
        set(__glob_mode GLOB)
    endif()

    if(NOT DEFINED ARG_GLOBS)
        set(__glob_list "*;.*")
    else()
        set(__glob_list "${ARG_GLOBS}")
    endif()

    set(__file_list "")
    foreach(_glob_exp IN LISTS __glob_list)
        file(
            ${__glob_mode} glob_result
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            "${ARG_DIRECTORY}/${_glob_exp}"
        )
        list(APPEND __file_list ${glob_result})
    endforeach()

    # Set output variable.
    set(${arg_files} "${__file_list}" PARENT_SCOPE)
endfunction()

# Gather source files under a folder.
function(afr_glob_src arg_files)
    cmake_parse_arguments(
        PARSE_ARGV 1
        "ARG"                   # Prefix results with "ARG_".
        "RECURSE"               # Whether to traverse all subdirectories.
        "DIRECTORY;EXTENSIONS"  # Specify folder location and semicolon separated extensions.
        ""                      # No multi-value arguments.
    )

    # Default to C and assembly files if no extension is specified.
    if("${ARG_EXTENSIONS}" STREQUAL "")
        set(__extensions "c;C;h;H;s;S;asm;ASM")
    else()
        set(__extensions "${ARG_EXTENSIONS}")
    endif()

    set(__glob_list)
    foreach(_ext IN LISTS __extensions)
        list(APPEND __glob_list "*.${_ext}")
    endforeach()

    if(ARG_RECURSE)
        afr_glob_files(${arg_files} RECURSE DIRECTORY ${ARG_DIRECTORY} GLOBS "${__glob_list}")
    else()
        afr_glob_files(${arg_files} DIRECTORY ${ARG_DIRECTORY} GLOBS "${__glob_list}")
    endif()

    # Set output variable.
    set(${arg_files} "${${arg_files}}" PARENT_SCOPE)
endfunction()

# Create a target to print the generator expression.
function(afr_print_generator_expr target_name expression)
    add_custom_target(${target_name}
        ${CMAKE_COMMAND} -E echo "\"${expression}\""
    )
endfunction()
