# Set your compiler path here if it's not in the PATH environment variable.
set(AFR_TOOLCHAIN_PATH "" CACHE INTERNAL "")

# If VENDOR or BOARD is specified, try to find a match.
if(DEFINED VENDOR OR DEFINED BOARD)
    include("${CMAKE_CURRENT_LIST_DIR}/tools/cmake/afr_utils.cmake")

    set(matched_boards)
    afr_get_boards(all_boards)
    foreach(board IN LISTS all_boards)
        string(REGEX MATCH "${VENDOR}.*\\.${BOARD}.*" match "${board}")
        if(match)
            list(APPEND matched_boards "${match}")
        endif()
    endforeach()

    list(LENGTH matched_boards size)
    if(size EQUAL 0)
        message(FATAL_ERROR "No matching board found, please check your VENDOR and BOARD value.")
    endif()
    if(NOT size EQUAL 1)
        list(JOIN matched_boards ", " matched_boards)
        message(FATAL_ERROR "Multiple matching boards found: ${matched_boards}")
    else()
        set(AFR_BOARD "${matched_boards}" CACHE INTERNAL "")
    endif()
endif()

# If COMPILER is specified, set toolchain file to ${COMPILER}.cmake.
if(DEFINED COMPILER)
    if(DEFINED CMAKE_TOOLCHAIN_FILE)
        get_filename_component(toolchain "${CMAKE_TOOLCHAIN_FILE}" NAME_WE)
        if(NOT "${COMPILER}" STREQUAL "${toolchain}")
            message(WARNING "CMAKE_TOOLCHAIN_FILE is already defined to ${toolchain}.cmake, you\
                need to delete cache and reconfigure if you want to switch compiler.")
        endif()
    else()
        set(toolchain_dir "${CMAKE_CURRENT_LIST_DIR}/tools/cmake/toolchains")
        set(toolchain_file "${toolchain_dir}/${COMPILER}.cmake")
        if(EXISTS "${toolchain_file}")
            set(CMAKE_TOOLCHAIN_FILE "${toolchain_file}" CACHE INTERNAL "")
        else()
            message(FATAL_ERROR "Toolchain file \"${COMPILER}.cmake\" does not exist, please\
                select one from \"cmake/toolchains\" folder.")
        endif()
    endif()
endif()

# Disable compiler checks when only outputing metadata.
if(AFR_METADATA_MODE)
    set(CMAKE_C_COMPILER_FORCED TRUE CACHE INTERNAL "")
    set(CMAKE_CXX_COMPILER_FORCED TRUE CACHE INTERNAL "")
endif()

# Remove these helper variables from CMake cache.
unset(VENDOR CACHE)
unset(BOARD CACHE)
unset(COMPILER CACHE)
