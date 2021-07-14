# -------------------------------------------------------------------------------------------------
# FreeRTOS metadata
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

    set(metadata_prefix AFR_METADATA_${arg_type})

    if(ARG_INHERITED)
        set(inherited INHERITED)
        set(scopes DIRECTORY TARGET)
    else()
        set(inherited "")
        set(scopes TARGET)
    endif()

    set(prop_name ${metadata_prefix}_${arg_name})

    foreach(scope IN LISTS scopes)
        define_property(
            ${scope} PROPERTY ${prop_name} ${inherited}
            BRIEF_DOCS "${arg_description}"
            FULL_DOCS "${arg_description}"
        )
    endforeach()

    afr_cache_append(${metadata_prefix} ${arg_name})
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

    set(allowed_values BOARD LIB DEMO)
    if(NOT "${arg_metadata_type}" IN_LIST allowed_values)
        message(FATAL_ERROR "Invalid metadata type: ${arg_metadata_type}.")
    endif()

    get_filename_component(cmake_file_dir "${CMAKE_CURRENT_LIST_DIR}" NAME)
    get_filename_component(cmake_file_name "${CMAKE_CURRENT_LIST_FILE}" NAME)
    if(ARG_DIRECTORY)
        set(scope_name "${CMAKE_CURRENT_LIST_DIR}")
        set(scope DIRECTORY)
    else()
        if("${cmake_file_name}" STREQUAL "CMakeLists.txt")
            set(scope_name AFR_metadata::${arg_metadata_type}::${cmake_file_dir})
        else()
            get_filename_component(cmake_file_name "${cmake_file_name}" NAME_WE)
            set(scope_name AFR_metadata::${arg_metadata_type}::${cmake_file_name})
        endif()
        set(scope TARGET)
    endif()

    if("${scope}" STREQUAL "TARGET" AND NOT TARGET ${scope_name})
        add_library(${scope_name} UNKNOWN IMPORTED GLOBAL)
    endif()

    set(prop_name AFR_METADATA_${arg_metadata_type}_${arg_metadata_name})
    set_property(${scope} "${scope_name}" PROPERTY ${prop_name} "${arg_metadata_val}")

    string(TOLOWER "${arg_metadata_type}" arg_metadata_type)
    set(metadata_file "${AFR_METADATA_OUTPUT_DIR}/console/${arg_metadata_type}.txt")
    if(AFR_METADATA_MODE)
        if("${cmake_file_name}" STREQUAL "CMakeLists.txt")
            if (NOT "${AFR_CURRENT_MODULE}" STREQUAL "")
                string(REPLACE "demo_" "" AFR_CURRENT_MODULE "${AFR_CURRENT_MODULE}")
                file(APPEND "${metadata_file}" "${AFR_CURRENT_MODULE}###${arg_metadata_name}:::${arg_metadata_val}\n")
            else()
                file(APPEND "${metadata_file}" "${cmake_file_dir}###${arg_metadata_name}:::${arg_metadata_val}\n")
            endif()
        else()
            file(APPEND "${metadata_file}" "${cmake_file_name}###${arg_metadata_name}:::${arg_metadata_val}\n")
        endif()
    endif()
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
    set(target_name AFR_metadata::${arg_metadata_type}::${AFR_BOARD_NAME})
    set(prop_name AFR_METADATA_${arg_metadata_type}_${arg_metadata_name})

    if(TARGET ${target_name})
        get_target_property(prop_val ${target_name} ${prop_name})
    endif()

    if(NOT prop_val)
        set(prop_val "UNKNOWN")
    endif()

    set(${arg_out_var} ${prop_val} PARENT_SCOPE)
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

# TODO, we need to keep track of CMakeLists.txt files for AFR modules.
set(AFR_METADATA_CMAKE_FILES "" CACHE INTERNAL "")
function(afr_add_subdirectory module_name)
  add_subdirectory("${module_name}")
  afr_cache_append(AFR_METADATA_CMAKE_FILES "${module_name}/CMakeLists.txt")
endfunction()

function(afr_write_metadata)
    set(ide_dir "${AFR_METADATA_OUTPUT_DIR}/ide")
    set(console_dir "${AFR_METADATA_OUTPUT_DIR}/console")

    set(afr_version_file "${console_dir}/afr_version.txt")
    set(board_path_file "${console_dir}/vendor_board_path.txt")
    set(cmake_files_file "${AFR_METADATA_OUTPUT_DIR}/console/cmake_files.txt")
    set(module_sources_file "${console_dir}/module_sources.txt")
    set(module_dependencies_file "${console_dir}/module_dependencies.txt")
    set(metadata_file "${console_dir}/modules.txt")
    file(APPEND "${afr_version_file}" "${AFR_VERSION}")
    file(APPEND "${board_path_file}" "vendor_path: ${AFR_VENDOR_PATH}\n")
    file(APPEND "${board_path_file}" "board_path: ${AFR_BOARD_PATH}")
    file(APPEND "${module_dependencies_file}" "public_modules#${AFR_MODULES_PUBLIC}\n")
    file(APPEND "${module_dependencies_file}" "modules#${AFR_MODULES_BUILD}\n")
    file(APPEND "${module_dependencies_file}" "demos#${AFR_DEMOS_ENABLED}\n")
    set(enabledModules ${AFR_MODULES_ENABLED_USER} ${AFR_MODULES_ENABLED_DEPS})
    file(APPEND "${module_dependencies_file}" "enabledModules#${enabledModules}\n")

    set(src_all "")
    set(src_console "")
    set(inc_all "")

    # Write all required cmake files.
    set(
        cmake_files
        "${AFR_ROOT_DIR}/tools/cmake"
        "${AFR_ROOT_DIR}/CMakeLists.txt"
        "${AFR_ROOT_DIR}/PreLoad.cmake"
        "${AFR_ROOT_DIR}/libraries/CMakeLists.txt"
        "${AFR_ROOT_DIR}/libraries/3rdparty/CMakeLists.txt"
        "${AFR_ROOT_DIR}/demos/CMakeLists.txt"
    )
    foreach(cmake_file IN LISTS AFR_METADATA_CMAKE_FILES)
        get_filename_component(module_dir "${cmake_file}" DIRECTORY)
        get_filename_component(module "${module_dir}" NAME)
        if(module IN_LIST AFR_MODULES_ENABLED OR "demo_${module}" IN_LIST AFR_MODULES_ENABLED)
            list(APPEND cmake_files "${cmake_file}")
            list(APPEND src_console "${module_dir}")
        endif()
    endforeach()
    file(APPEND "${cmake_files_file}" "${cmake_files}")

    foreach(module IN LISTS AFR_MODULES_ENABLED)
        # Skip kernel, we already got the metadata from other kernel modules.
        if("${module}" STREQUAL "kernel")
            continue()
        endif()

        # Check if module contains module-specifc cmake files.
        set(prop_var AFR_MODULE_${module}_CMAKE_FILES)
        if(DEFINED ${prop_var})
            # Add cmake files associated with this module to the metadata file 
            # containing list of all cmake files.
            file(APPEND "${cmake_files_file}" ";${${prop_var}}")
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
    list(APPEND src_console ${src_all})
    foreach(3rdparty_name IN LISTS 3RDPARTY_MODULES_ENABLED)
        set(3rdparty_target "3rdparty::${3rdparty_name}")
        list(APPEND src_console "${AFR_3RDPARTY_DIR}/${3rdparty_name}")
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

    if(TARGET 3rdparty::mbedtls)
        list(
            APPEND src_console
            "${AFR_3RDPARTY_DIR}/mbedtls_config"
            "${AFR_3RDPARTY_DIR}/mbedtls_utils"
        )
    endif()

    # Append extra files for FreeRTOS console.
    list(
        APPEND src_console
        "${AFR_ROOT_DIR}/CHANGELOG.md"
        "${AFR_ROOT_DIR}/checksums.json"
        "${AFR_ROOT_DIR}/directories.txt"
        "${AFR_ROOT_DIR}/LICENSE"
        "${AFR_ROOT_DIR}/README.md"
        "${AFR_ROOT_DIR}/CONTRIBUTING.md"
        "${AFR_ROOT_DIR}/CODE_OF_CONDUCT.md"
        "${AFR_ROOT_DIR}/tools/certificate_configuration/PEMfileToCString.html"
        "${AFR_ROOT_DIR}/tools/certificate_configuration/CertificateConfigurator.html"
        "${AFR_ROOT_DIR}/tools/certificate_configuration/js/generator.js"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/SetupAWS.py"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/certs.py"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/thing.py"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/policy.py"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/misc.py"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/configure.json"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/aws_clientcredential.templ"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/aws_clientcredential_keys.templ"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/policy_document.templ"
        "${AFR_ROOT_DIR}/tools/aws_config_quick_start/README.md"
        "${AFR_KERNEL_DIR}"
        "${AFR_TESTS_DIR}"
    )

    # Add any addtional files to ${console_dir}/source_paths.txt
    list(
        APPEND src_console
        ${AFR_FILES_TO_CONSOLE_METADATA}
    )

    # Write all sources and include dirs.
    file(WRITE "${console_dir}/source_paths.txt" "${src_console}")
    file(WRITE "${ide_dir}/source_paths.txt" "${src_all}")
    file(WRITE "${ide_dir}/include_paths.txt" "${inc_all}")
    file(
        GENERATE
        OUTPUT "${console_dir}/source_paths.txt"
        INPUT "${console_dir}/source_paths.txt"
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

# This function is used for any additional files that need
# be included in the ZIP downloaded from FreeRTOS console.
function(afr_files_to_console_metadata)
    set(prop_var AFR_FILES_TO_CONSOLE_METADATA)
    if( NOT DEFINED ${prop_var} )
       set(${prop_var} "" CACHE INTERNAL "")
    endif()
    afr_cache_append(${prop_var} ${ARGN})
endfunction()


# =============== Metadata definition ===============
set(AFR_METADATA_BOARD "" CACHE INTERNAL "List of CMake property names for hardware metadata.")
set(AFR_METADATA_LIB "" CACHE INTERNAL "List of CMake property names for AFR library metadata.")
set(AFR_METADATA_DEMO "" CACHE INTERNAL "List of CMake property names for AFR demo metadata.")

afr_define_metadata(BOARD ID "ID for the board to uniquely identify it.")
afr_define_metadata(BOARD DISPLAY_NAME " Name displayed on the FreeRTOS Console.")
afr_define_metadata(BOARD DESCRIPTION "Short description of the board.")
afr_define_metadata(BOARD VENDOR_NAME "Vendor name." INHERITED)
afr_define_metadata(BOARD FAMILY_NAME "Board family name." INHERITED)
afr_define_metadata(BOARD DATA_RAM_MEMORY "Data RAM size.")
afr_define_metadata(BOARD PROGRAM_MEMORY "Program memory size.")
afr_define_metadata(BOARD CODE_SIGNER "Code signer platform.")
afr_define_metadata(BOARD SUPPORTED_IDE "Supported IDE." INHERITED)
afr_define_metadata(BOARD RECOMMENDED_IDE "Recommended IDE." INHERITED)
afr_define_metadata(BOARD IS_ACTIVE "Is the board Active to be displayed on FreeRTOS Console")
afr_define_metadata(BOARD AWS_DEMOS_CONFIG_FILES_LOCATION "Location of vendor's config files")

afr_define_metadata(LIB ID "Library name.")
afr_define_metadata(LIB DISPLAY_NAME "Library name displayed on the FreeRTOS Console.")
afr_define_metadata(LIB DESCRIPTION "Library description.")
afr_define_metadata(LIB CATEGORY "Library category.")
afr_define_metadata(LIB IS_VISIBLE "boolean to decide if this library is visible on FreeRTOS Console.")
afr_define_metadata(LIB VERSION "Version of this library.")

afr_define_metadata(DEMO ID "Demo name.")
afr_define_metadata(DEMO DISPLAY_NAME "Demo name displayed on the FreeRTOS Console.")
afr_define_metadata(DEMO DESCRIPTION "Demo description.")
