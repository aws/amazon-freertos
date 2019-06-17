# Enable assembly if we're cross compiling.
if(CMAKE_CROSSCOMPILING)
    enable_language(ASM)
endif()

# Do not prefix the output library file.
set(CMAKE_STATIC_LIBRARY_PREFIX "")

# Set some global path variables.
get_filename_component(__root_dir "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(AFR_ROOT_DIR ${__root_dir} CACHE INTERNAL "Amazon FreeRTOS source root.")
set(AFR_KERNEL_DIR "${AFR_ROOT_DIR}/freertos_kernel" CACHE INTERNAL "FreeRTOS kernel root.")
set(AFR_MODULES_DIR "${AFR_ROOT_DIR}/libraries" CACHE INTERNAL "Amazon FreeRTOS modules root.")
set(AFR_MODULES_C_SDK_DIR "${AFR_MODULES_DIR}/c_sdk" CACHE INTERNAL "C-SDK libraries root.")
set(AFR_MODULES_FREERTOS_PLUS_DIR "${AFR_MODULES_DIR}/freertos_plus" CACHE INTERNAL "FreeRTOS-Plus libraries root.")
set(AFR_MODULES_ABSTRACTIONS_DIR "${AFR_MODULES_DIR}/abstractions" CACHE INTERNAL "Abstractions layers root.")
set(AFR_DEMOS_DIR "${AFR_ROOT_DIR}/demos" CACHE INTERNAL "Amazon FreeRTOS demos root.")
set(AFR_TESTS_DIR "${AFR_ROOT_DIR}/tests" CACHE INTERNAL "Amazon FreeRTOS common tests and framework root.")
set(AFR_VENDORS_DIR "${AFR_ROOT_DIR}/vendors" CACHE INTERNAL "vendors content root.")
set(AFR_3RDPARTY_DIR "${AFR_MODULES_DIR}/3rdparty" CACHE INTERNAL "3rdparty libraries root.")

# TODO, Network manager will temporarily be built into the demo executables (remove later).
set(NETWORK_MANAGER_SOURCES
    "${AFR_DEMOS_DIR}/network_manager/aws_iot_demo_network.c"
    "${AFR_DEMOS_DIR}/network_manager/aws_iot_network_manager.c"
    CACHE INTERNAL
    "Network manager common source files."
)

# Set regular version and Git commit version.
set(AFR_VERSION "${PROJECT_VERSION}")
set(AFR_VERSION_VCS "Unknown" CACHE INTERNAL "")
# Check if we're in a Git repository.
find_package(Git)
if(Git_FOUND AND EXISTS "${AFR_ROOT_DIR}/.git")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" "describe" "--always" WORKING_DIRECTORY "${AFR_ROOT_DIR}"
        OUTPUT_VARIABLE __version OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(AFR_VERSION_VCS "${__version}" CACHE INTERNAL "")
endif()

# If we're cross compiling to a board, set "AFR_TOOLCHAIN" to the file name of CMAKE_TOOLCHAIN_FILE,
# otherwise, set it to compiler id in lower case.
if(CMAKE_TOOLCHAIN_FILE)
    get_filename_component(__toolchain "${CMAKE_TOOLCHAIN_FILE}" NAME_WE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" CACHE INTERNAL "")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(__toolchain "gcc")
else()
    set(__toolchain "${CMAKE_C_COMPILER_ID}")
    string(TOLOWER "${__toolchain}" __toolchain)
endif()
# This variable can be passed from outside to overwrite the default value. Required by IDE project
# generator and Amazon FreeRTOS console.
if(NOT DEFINED CACHE{AFR_TOOLCHAIN})
    set(AFR_TOOLCHAIN ${__toolchain} CACHE INTERNAL "Toolchain to build Amazon FreeRTOS.")
endif()

# Provide an option to enable tests. Also set an helper variable to use in generator expression.
option(AFR_ENABLE_TESTS "Build tests for Amazon FreeRTOS. Requires recompiling whole library." OFF)
if(AFR_ENABLE_TESTS)
    add_compile_definitions(AMAZON_FREERTOS_ENABLE_UNIT_TESTS)
    add_compile_definitions(IOT_BUILD_TESTS=1)
    set(AFR_IS_TESTING 1 CACHE INTERNAL "")
else()
    set(AFR_IS_TESTING 0 CACHE INTERNAL "")
endif()

# Enable debug mode for CMake files
option(AFR_DEBUG_CMAKE "Turn on additional checks and messages.")
mark_as_advanced(AFR_DEBUG_CMAKE)

# Enable metadata mode. This is for internal use only to generate metadata information for a board.
if(NOT CACHE{AFR_METADATA_MODE})
    set(AFR_METADATA_MODE 0 CACHE INTERNAL "Turn off build and generate metadata information.")
endif()

# Import other CMake files.
include("${CMAKE_CURRENT_LIST_DIR}/afr_utils.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/afr_module.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/afr_metadata.cmake")
