# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS metadata
# -------------------------------------------------------------------------------------------------
# Clean the output metadata files from the build folder.
set(AFR_METADATA_OUTPUT_DIR "${CMAKE_BINARY_DIR}/metadata" CACHE INTERNAL "")
file(REMOVE_RECURSE "${AFR_METADATA_OUTPUT_DIR}")

# =============== Helper functions ===============
# Define a metadata property. This will define a CMake target property. If INHERITED option is
# given, will also define a directory property, get_property() command will chain up to the
# directory when the requested property is not set in the target.
function(afr_define_metadata arg_type arg_name arg_description)
    cmake_parse_arguments(
        PARSE_ARGV 3
        "ARG"               # Prefix of parsed results.
        "INHERITED"         # Option arguments.
        ""                  # One value arguments.
        ""                  # Multi value arguments.
    )

    set(__metadata_prefix AFR_METADATA_${arg_type})

    if(ARG_INHERITED)
        set(__inherited INHERITED)
        set(__scopes DIRECTORY TARGET)
    else()
        set(__inherited "")
        set(__scopes TARGET)
    endif()

    set(__prop_name ${__metadata_prefix}_${arg_name})

    foreach(__scope IN LISTS __scopes)
        define_property(
            ${__scope} PROPERTY ${__prop_name} ${__inherited}
            BRIEF_DOCS "${arg_description}"
            FULL_DOCS "${arg_description}"
        )
    endforeach()

    afr_cache_append(${__metadata_prefix} ${arg_name})
endfunction()

# If DIRECTORY option is given, will set the metadata property for ${CMAKE_CURRENT_LIST_DIR}.
# Otherwise, will set the metadata property for corresponding metadata target. The target name
# is inferred from the current CMake file name and its directory name.
function(afr_set_metadata arg_metadata_type arg_metadata_name arg_metadata_val)
    cmake_parse_arguments(
        PARSE_ARGV 3
        "ARG"               # Prefix of parsed results.
        "DIRECTORY"         # Option arguments.
        ""                  # One value arguments.
        ""                  # Multi value arguments.
    )

    set(__allowed_values BOARD LIB DEMO)
    if(NOT "${arg_metadata_type}" IN_LIST __allowed_values)
        message(FATAL_ERROR "Invalid metadata type: ${arg_metadata_type}.")
    endif()

    if(ARG_DIRECTORY)
        set(__scope_name "${CMAKE_CURRENT_LIST_DIR}")
        set(__scope DIRECTORY)
    else()
        get_filename_component(__cmake_file_dir "${CMAKE_CURRENT_LIST_DIR}" NAME)
        get_filename_component(__cmake_file_name "${CMAKE_CURRENT_LIST_FILE}" NAME)
        if("${__cmake_file_name}" STREQUAL "CMakeLists.txt")
            set(__scope_name AFR_metadata::${arg_metadata_type}::${__cmake_file_dir})
        else()
            get_filename_component(__cmake_file_name "${__cmake_file_name}" NAME_WE)
            set(__scope_name AFR_metadata::${arg_metadata_type}::${__cmake_file_name})
        endif()
        set(__scope TARGET)
    endif()

    if("${__scope}" STREQUAL "TARGET" AND NOT TARGET ${__scope_name})
        add_library(${__scope_name} UNKNOWN IMPORTED GLOBAL)
    endif()

    set(__prop_name AFR_METADATA_${arg_metadata_type}_${arg_metadata_name})
    set_property(${__scope} "${__scope_name}" PROPERTY ${__prop_name} "${arg_metadata_val}")

    string(TOLOWER "${arg_metadata_type}" arg_metadata_type)
    set(__metadata_file "${AFR_METADATA_OUTPUT_DIR}/ocw/${arg_metadata_type}.txt")
    get_filename_component(__cmake_file_dir "${CMAKE_CURRENT_LIST_DIR}" NAME)
    file(APPEND "${__metadata_file}" "${__cmake_file_dir}###${arg_metadata_name}:::${arg_metadata_val}\n")
endfunction()

# Convenient function to set board metadata.
function(afr_set_board_metadata arg_metadata_name arg_metadata_val)
    afr_set_metadata(BOARD ${arg_metadata_name} "${arg_metadata_val}" ${ARGN})
endfunction()

# Convenient function to set library metadata.
function(afr_set_lib_metadata arg_metadata_name arg_metadata_val)
    afr_set_metadata(LIB ${arg_metadata_name} "${arg_metadata_val}" ${ARGN})
endfunction()

# Convenient function to set demo metadata.
function(afr_set_demo_metadata arg_metadata_name arg_metadata_val)
    afr_set_metadata(DEMO ${arg_metadata_name} "${arg_metadata_val}" ${ARGN})
endfunction()

# Get metadata value.
function(afr_get_metadata arg_out_var arg_metadata_type arg_metadata_name)
    set(__target_name AFR_metadata::${arg_metadata_type}::${AFR_BOARD_NAME})
    set(__prop_name AFR_METADATA_${arg_metadata_type}_${arg_metadata_name})

    if(TARGET ${__target_name})
        get_target_property(__prop_val ${__target_name} ${__prop_name})
    endif()

    if(NOT __prop_val)
        set(__prop_val "UNKNOWN")
    endif()

    set(${arg_out_var} ${__prop_val} PARENT_SCOPE)
endfunction()

# Convenient function to get board metadata.
function(afr_get_board_metadata arg_out_var arg_metadata_name)
    afr_get_metadata(${arg_out_var} BOARD ${arg_metadata_name})
    set(${arg_out_var} "${${arg_out_var}}" PARENT_SCOPE)
endfunction()

# Convenient function to get library metadata.
function(afr_get_lib_metadata arg_out_var arg_metadata_name)
    afr_get_metadata(${arg_out_var} LIB ${arg_metadata_name})
    set(${arg_out_var} "${${arg_out_var}}" PARENT_SCOPE)
endfunction()

# Convenient function to get demo metadata.
function(afr_get_demo_metadata arg_out_var arg_metadata_name)
    afr_get_metadata(${arg_out_var} DEMO ${arg_metadata_name})
    set(${arg_out_var} "${${arg_out_var}}" PARENT_SCOPE)
endfunction()

# TODO, this wrapper function is needed because we need to keep track of all CMakeListst.txt files.
set(AFR_METADATA_CMAKE_FILES "${AFR_METADATA_OUTPUT_DIR}/ocw/_cmake_files.txt")
function(afr_add_subdirectory module_name)
  add_subdirectory("${module_name}")
  file(APPEND "${AFR_METADATA_CMAKE_FILES}" "${module_name}/CMakeLists.txt;")
endfunction()

function(afr_write_metadata)
    set(ide_dir "${AFR_METADATA_OUTPUT_DIR}/ide")
    set(ocw_dir "${AFR_METADATA_OUTPUT_DIR}/ocw")

    set(afr_version_file "${ocw_dir}/afr_version.txt")
    set(module_sources_file "${ocw_dir}/module_sources.txt")
    set(module_dependencies_file "${ocw_dir}/module_dependencies.txt")
    set(metadata_file "${ocw_dir}/modules.txt")
    file(APPEND "${afr_version_file}" "${AFR_VERSION}")
    file(APPEND "${module_dependencies_file}" "public_modules#${AFR_MODULES_PUBLIC}\n")
    file(APPEND "${module_dependencies_file}" "modules#${AFR_STATUS_MODULES_TO_BUILD}\n")
    file(APPEND "${module_dependencies_file}" "demos#${AFR_STATUS_DEMOS_ENABLED}\n")
    string(CONCAT disabledModules ${AFR_STATUS_MODULES_DISABLED_USER} , ${AFR_STATUS_MODULES_DISABLED_DEPS})
    file(APPEND "${module_dependencies_file}" "disabledModules#${disabledModules}\n")

    set(3rdparty_list "")
    set(src_all "")
    set(src_ocw "")
    set(inc_all "")

    foreach(module IN LISTS AFR_MODULES_ENABLED)
        # Skip kernel, we already got the metadata from other kernel modules.
        if("${module}" STREQUAL "kernel")
            continue()
        endif()
        string(FIND ${module} ::mcu_port __idx)
        if(__idx EQUAL -1)
            set(dependencies_list ${AFR_MODULE_${module}_DEPENDS_ALL})
            set(src_list ${AFR_MODULE_${module}_SOURCES_ALL})
            set(inc_list ${AFR_MODULE_${module}_INCLUDES_ALL})
        else()
            get_target_property(dependencies_list AFR::${module} INTERFACE_LINK_LIBRARIES)
            if(NOT dependencies_list)
                set(dependencies_list "")
            endif()
            list(FILTER dependencies_list INCLUDE REGEX AFR::)
            get_target_property(src_list AFR::${module} INTERFACE_SOURCES)
            get_target_property(inc_list AFR::${module} INTERFACE_INCLUDE_DIRECTORIES)
            if(NOT src_list)
                set(src_list "")
            endif()
            if(NOT inc_list)
                set(inc_list "")
            else()
                string(REGEX REPLACE [[\$<NOT:\$<COMPILE_LANGUAGE:[A-Z]+>>]] 1 inc_list "${inc_list}")
                string(REGEX REPLACE [[\$<COMPILE_LANGUAGE:[A-Z]+>]] 1 inc_list "${inc_list}")
            endif()
        endif()
        foreach(dep IN LISTS dependencies_list)
            string(FIND ${dep} 3rdparty:: __idx)
            if(__idx EQUAL 0 AND NOT ${dep} IN_LIST 3rdparty_list)
                list(APPEND 3rdparty_list ${dep})
            endif()
        endforeach()
        list(REMOVE_DUPLICATES 3rdparty_list)

        list(APPEND src_all ${src_list})
        list(APPEND inc_all ${inc_list})

        # Write module metadata
        list(TRANSFORM src_list REPLACE "${AFR_ROOT_DIR}/" "")
        if(NOT "${dependencies_list}" STREQUAL "")
            file(APPEND "${module_dependencies_file}" "${module}#")
            file(APPEND "${module_dependencies_file}" "${dependencies_list}\n")
        endif()

        if(NOT "${src_list}" STREQUAL "")
            file(APPEND "${module_sources_file}" "${module}#")
            file(APPEND "${module_sources_file}" "${src_list}\n")
        endif()

        # write module metadata readable way
        list(JOIN src_list "\n    - " src_list)
        list(JOIN dependencies_list "\n    - " dependencies_list)

        file(APPEND "${metadata_file}" "${module}:\n")
        file(APPEND "${metadata_file}" "  dependencies:\n")
        if(NOT "${dependencies_list}" STREQUAL "")
            file(APPEND "${metadata_file}" "    - ${dependencies_list}\n")
        endif()
        file(APPEND "${metadata_file}" "  sources:\n")
        if(NOT "${src_list}" STREQUAL "")
            file(APPEND "${metadata_file}" "    - ${src_list}\n")
        endif()
    endforeach()

    # Write demo or test project metadata
    if(AFR_IS_TESTING)
        set(exe_target aws_tests)
    else()
        set(exe_target aws_demos)
    endif()

    get_target_property(exe_src ${exe_target} SOURCES)
    get_target_property(exe_inc ${exe_target} INCLUDE_DIRECTORIES)
    list(APPEND src_all ${exe_src})
    list(TRANSFORM exe_src REPLACE "${AFR_ROOT_DIR}/" "")
    list(JOIN exe_src "\n    - " exe_src)
    file(APPEND "${metadata_file}" "${exe_target}:\n")
    file(APPEND "${metadata_file}" "  sources:\n")
    file(APPEND "${metadata_file}" "    - ${exe_src}\n")
    if(exe_inc)
        list(APPEND inc_all ${exe_inc})
    endif()

    # Add third party data
    set(src_ocw ${src_all})
    foreach(3rdparty_target IN LISTS 3rdparty_list)
        string(LENGTH "3rdparty::" len)
        string(SUBSTRING "${3rdparty_target}" ${len} -1 3rdparty_name)
        list(APPEND src_ocw "${AFR_3RDPARTY_DIR}/${3rdparty_name}")
        get_target_property(lib_type ${3rdparty_target} TYPE)
        if("${lib_type}" STREQUAL "INTERFACE_LIBRARY")
            set(prop_prefix "INTERFACE_")
        else()
            set(prop_prefix "")
        endif()
        get_target_property(3rdparty_src ${3rdparty_target} ${prop_prefix}SOURCES)
        get_target_property(3rdparty_inc ${3rdparty_target} ${prop_prefix}INCLUDE_DIRECTORIES)
        if(3rdparty_src)
            list(APPEND src_all ${3rdparty_src})
        endif()
        if(3rdparty_inc)
            list(APPEND inc_all ${3rdparty_inc})
        endif()
    endforeach()

    # Append CMake files for OCW.
    file(READ "${ocw_dir}/_cmake_files.txt" cmake_files_list)
    list(APPEND src_ocw ${cmake_files_list})

    # Write all sources and include dirs.
    string(REPLACE ";" "\n" src_ocw "${src_ocw}")
    string(REPLACE ";" "\n" src_all "${src_all}")
    string(REPLACE ";" "\n" inc_all "${inc_all}")
    file(WRITE "${ocw_dir}/source_paths.txt" "${src_ocw}")
    file(WRITE "${ide_dir}/source_paths.txt" "${src_all}")
    file(WRITE "${ide_dir}/include_paths.txt" "${inc_all}")
    file(
        GENERATE
        OUTPUT "${ocw_dir}/source_paths.txt"
        INPUT "${ocw_dir}/source_paths.txt"
    )
    file(
        GENERATE
        OUTPUT "${ide_dir}/source_paths.txt"
        INPUT "${ide_dir}/source_paths.txt"
    )
    file(
        GENERATE
        OUTPUT "${ide_dir}/include_paths.txt"
        INPUT "${ide_dir}/include_paths.txt"
    )
endfunction()

# =============== Metadata definition ===============
set(AFR_METADATA_BOARD "" CACHE INTERNAL "List of CMake property names for hardware metadata.")
set(AFR_METADATA_LIB "" CACHE INTERNAL "List of CMake property names for AFR library metadata.")

afr_define_metadata(BOARD ID "ID for the board to uniquely identify it.")
afr_define_metadata(BOARD DISPLAY_NAME " Name displayed on the Amazon FreeRTOS Console.")
afr_define_metadata(BOARD DESCRIPTION "Short description of the board.")
afr_define_metadata(BOARD VENDOR_NAME "Vendor name." INHERITED)
afr_define_metadata(BOARD FAMILY_NAME "Board family name." INHERITED)
afr_define_metadata(BOARD DATA_RAM_MEMORY "Data RAM size.")
afr_define_metadata(BOARD PROGRAM_MEMORY "Program memory size.")
afr_define_metadata(BOARD CODE_SIGNER "Code signer platform.")
afr_define_metadata(BOARD SUPPORTED_IDE "Supported IDE." INHERITED)
afr_define_metadata(BOARD IS_ACTIVE "Is the board Active to be displayed on Amazon FreeRTOS Console")
afr_define_metadata(BOARD DEMO_COMMON_LOCATION "Location of vendor's commons in demos")
afr_define_metadata(BOARD THIRD_PARTY_LIB_LOCATION "Location of third party library files")

afr_define_metadata(LIB ID "Library name.")
afr_define_metadata(LIB DISPLAY_NAME "Library name displayed on the Amazon FreeRTOS Console.")
afr_define_metadata(LIB DESCRIPTION "Library description.")
afr_define_metadata(LIB CATEGORY "Library category.")
afr_define_metadata(LIB IS_VISIBLE "boolean to decide if this library is visible on Amazon FreeRTOS Console.")
afr_define_metadata(LIB VERSION "Version of this library.")

afr_define_metadata(DEMO ID "Demo name.")
afr_define_metadata(DEMO DISPLAY_NAME "Demo name displayed on the Amazon FreeRTOS Console.")
afr_define_metadata(DEMO DESCRIPTION "Demo description.")
