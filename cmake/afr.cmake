# Enable assembly.
enable_language(ASM)

# Do not prefix the output library file.
set(CMAKE_STATIC_LIBRARY_PREFIX "")

# Set some global path variables.
get_filename_component(__root_dir "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(AFR_ROOT_DIR ${__root_dir} CACHE INTERNAL "Amazon FreeRTOS source root.")

set(AFR_MODULES_DIR "${AFR_ROOT_DIR}/lib" CACHE INTERNAL "Amazon FreeRTOS modules root.")
set(AFR_DEMOS_DIR "${AFR_ROOT_DIR}/demos/common" CACHE INTERNAL "Amazon FreeRTOS demos root.")
set(AFR_TESTS_DIR "${AFR_ROOT_DIR}/tests/common" CACHE INTERNAL "Amazon FreeRTOS tests root.")
set(AFR_3RDPARTY_DIR "${AFR_MODULES_DIR}/third_party" CACHE INTERNAL "3rdparty libraries root.")

# If we're cross compiling to a board, set "AFR_TOOLCHAIN" to the file name of CMAKE_TOOLCHAIN_FILE,
# otherwise, set it to compiler id in lower case.
if(CMAKE_TOOLCHAIN_FILE)
    get_filename_component(__toolchain "${CMAKE_TOOLCHAIN_FILE}" NAME_WE)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(__toolchain "gcc")
else()
    set(__toolchain "${CMAKE_C_COMPILER_ID}")
    string(TOLOWER "${__toolchain}" __toolchain)
endif()
set(AFR_TOOLCHAIN ${__toolchain} CACHE STRING "Toolchain to build Amazon FreeRTOS.")

# Provide an option to enable tests. Also set an helper variable to use in generator expression.
option(AFR_ENABLE_TESTS "Build tests for Amazon FreeRTOS. Requires recompiling whole library." OFF)
if(AFR_ENABLE_TESTS)
    add_compile_definitions(AMAZON_FREERTOS_ENABLE_UNIT_TESTS)
    set(AFR_IS_TESTING 1 CACHE INTERNAL "")
else()
    set(AFR_IS_TESTING 0 CACHE INTERNAL "")
endif()

# Enable debug mode for CMake files
option(AFR_DEBUG_CMAKE "Turn on additional checks and messages.")
mark_as_advanced(AFR_DEBUG_CMAKE)

# Import other CMake files.
include("${CMAKE_CURRENT_LIST_DIR}/afr_utils.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/afr_module.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/afr_metadata.cmake")
