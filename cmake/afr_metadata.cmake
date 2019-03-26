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

    set(__allowed_values BOARD LIB)
    if(NOT ${arg_metadata_type} IN_LIST __allowed_values)
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
endfunction()

# Convenient function to set board metadata.
function(afr_set_board_metadata arg_metadata_name arg_metadata_val)
    afr_set_metadata(BOARD ${arg_metadata_name} "${arg_metadata_val}" ${ARGN})
    set(__metadata_file "${AFR_METADATA_OUTPUT_DIR}/board.txt")
    file(APPEND "${__metadata_file}" "${arg_metadata_name}: ${arg_metadata_val}\n")
endfunction()

# Convenient function to set library metadata.
function(afr_set_lib_metadata arg_metadata_name arg_metadata_val)
    afr_set_metadata(LIB ${arg_metadata_name} "${arg_metadata_val}" ${ARGN})
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

afr_define_metadata(LIB NAME "Library name.")
afr_define_metadata(LIB DESCRIPTION "Library description.")
afr_define_metadata(LIB CATEGORY "Library category.")
afr_define_metadata(LIB VERSION "Version of this library.")
afr_define_metadata(LIB LOCATION "Folder location of this library." INHERITED)
