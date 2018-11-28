cmake_minimum_required(VERSION 3.13)

project(amazon-freertos VERSION 1.4.4)

# Import global configurations.
include("cmake/afr.cmake")

# -------------------------------------------------------------------------------------------------
# Configure target board
# -------------------------------------------------------------------------------------------------
# Get list of supported boards.
afr_get_boards(AFR_SUPPORTED_BOARDS)

set(AFR_BOARD "vendor.board" CACHE STRING "Target board chosen by the user at configure time")
set_property(CACHE AFR_BOARD PROPERTY STRINGS ${AFR_SUPPORTED_BOARDS})

string(REGEX MATCH [[(.+)\.(.+)]] __match_result ${AFR_BOARD})
set(AFR_VENDOR_NAME ${CMAKE_MATCH_1} CACHE INTERNAL "MCU vendor name")
set(AFR_BOARD_NAME ${CMAKE_MATCH_2} CACHE INTERNAL "MCU board name")

# Abort if the target board is not supported, i.e., corresponding folder is not present.
if(NOT AFR_BOARD IN_LIST AFR_SUPPORTED_BOARDS)
    message(FATAL_ERROR "Board is not supported: ${AFR_BOARD}")
endif()

# Import board CMake build.
add_subdirectory("cmake/vendors/${AFR_VENDOR_NAME}/${AFR_BOARD_NAME}")

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS modules
# -------------------------------------------------------------------------------------------------
# Initialize all modules.
add_subdirectory("lib")
add_subdirectory("demos")
add_subdirectory("tests")

# Resolve dependencies.
afr_resolve_dependencies()

# -------------------------------------------------------------------------------------------------
# Summary
# -------------------------------------------------------------------------------------------------
message("")
message("====================Configuration for Amazon FreeRTOS====================")
message("  Version:                 ${AFR_VERSION}")
message("  Git version:             ${AFR_VERSION_VCS}")

# ================ Target microcontroller =================
message("")
message("Target microcontroller:")

afr_get_board_metadata(vendor_name VENDOR_NAME)
afr_get_board_metadata(board_name  DISPLAY_NAME)
afr_get_board_metadata(description DESCRIPTION)
afr_get_board_metadata(family      FAMILY_NAME)
afr_get_board_metadata(data_ram    DATA_RAM_MEMORY)
afr_get_board_metadata(program_mem PROGRAM_MEMORY)

message("  vendor:                  ${vendor_name}")
message("  board:                   ${board_name}")
message("  description:             ${description}")
message("  family:                  ${family}")
message("  data ram size:           ${data_ram}")
message("  program memory size:     ${program_mem}")

# ===================== Host platform =====================
message("")
message("Host platform:")

message("  OS:                      ${CMAKE_HOST_SYSTEM}")
message("  Toolchain:               ${AFR_TOOLCHAIN}")
message("  Toolchain path:          ${CMAKE_FIND_ROOT_PATH}")
message("  CMake generator:         ${CMAKE_GENERATOR}")

# ================ Amazon FreeRTOS modules ================
message("")
message("Amazon FreeRTOS modules:")

list(JOIN AFR_MODULES_BUILD ", " AFR_STATUS_MODULES_TO_BUILD)
list(JOIN AFR_MODULES_DISABLED_USER ", " AFR_STATUS_MODULES_DISABLED_USER)
list(JOIN AFR_MODULES_DISABLED_DEPS ", " AFR_STATUS_MODULES_DISABLED_DEPS)
list(JOIN AFR_DEMOS_ENABLED ", " AFR_STATUS_DEMOS_ENABLED)
list(JOIN AFR_TESTS_ENABLED ", " AFR_STATUS_TESTS_ENABLED)

message("  Modules to build:        ${AFR_STATUS_MODULES_TO_BUILD}")
message("  Disabled by user:        ${AFR_STATUS_MODULES_DISABLED_USER}")
message("  Disabled by dependency:  ${AFR_STATUS_MODULES_DISABLED_DEPS}")

message("")
message("  Available demos:         ${AFR_STATUS_DEMOS_ENABLED}")
message("  Available tests:         ${AFR_STATUS_TESTS_ENABLED}")

message("=========================================================================")
message("")

# Abort if we're not even able to build the kernel.
if(NOT kernel IN_LIST AFR_MODULES_BUILD)
    message(FATAL_ERROR "Unable to build kernel due to missing dependencies.")
endif()

# -------------------------------------------------------------------------------------------------
# Demos and tests
# -------------------------------------------------------------------------------------------------
# Enable us to use targets defined in vendor CMake files.
cmake_policy(SET CMP0079 NEW)

if(TARGET aws_demos)
    list(TRANSFORM AFR_DEMOS_ENABLED PREPEND "AFR::")
    target_link_libraries(aws_demos PRIVATE ${AFR_DEMOS_ENABLED})
endif()

if(TARGET aws_tests)
    list(TRANSFORM AFR_TESTS_ENABLED PREPEND "AFR::")
    target_link_libraries(aws_tests PRIVATE ${AFR_TESTS_ENABLED})
endif()
