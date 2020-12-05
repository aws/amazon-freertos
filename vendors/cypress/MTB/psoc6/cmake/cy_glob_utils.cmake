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

    if(DEFINED ARG_ITEMS)
    foreach(path IN LISTS ARG_ITEMS)
        set(prefix_dir_list "")
        string(REPLACE "/" ";" prefix_dir_list ${path})
        list(FILTER prefix_dir_list INCLUDE REGEX "^${ARG_PREFIX}_[^/]*")
        set(is_match TRUE)

        foreach(prefixed_val IN LISTS prefix_dir_list)
            string(REGEX MATCH "${ARG_PREFIX}_([^/]*)" match "${prefixed_val}")
            list(FIND ARG_VALUES ${CMAKE_MATCH_1} _index)
            if (${_index} EQUAL -1)
                set(is_match FALSE)
                break()
            endif()
        endforeach()

        if(is_match)
            list(APPEND rslt "${path}")
        endif()
    endforeach()
    endif()

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
