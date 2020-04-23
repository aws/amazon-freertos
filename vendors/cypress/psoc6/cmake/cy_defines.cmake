#--------------------------------------------------------------------
# Defines that may be used in application-specific CMakeLists.txt
#
# This file is included in multiple Cypress cmake files
#
#--------------------------------------------------------------------

#--------------------------------------------------------------------
# copied from afr.cmake, be aware they may change between AFR versions
#--------------------------------------------------------------------
if(NOT AFR_ROOT_DIR)
    get_filename_component(__root_dir "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    set(AFR_ROOT_DIR ${__root_dir})
    set(AFR_MODULES_DIR "${AFR_ROOT_DIR}/libraries")
    set(AFR_MODULES_C_SDK_DIR "${AFR_MODULES_DIR}/c_sdk")
    set(AFR_MODULES_FREERTOS_PLUS_DIR "${AFR_MODULES_DIR}/freertos_plus")
    set(AFR_DEMOS_DIR "${AFR_ROOT_DIR}/demos")
    set(AFR_TESTS_DIR "${AFR_ROOT_DIR}/tests")
    set(AFR_VENDORS_DIR "${AFR_ROOT_DIR}/vendors")
endif()

#--------------------------------------------------------------------
# common directories
set(cy_psoc6_dir    "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/psoc6")
set(cy_common_dir   "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/common")
set(cy_whd_dir      "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/whd")
set(cy_capsense_dir "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/capsense")
set(cy_clib_dir     "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/clib_support")
set(cy_board_dir    "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/boards/${AFR_BOARD_NAME}")
set(cy_libraries_dir "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/libraries/")
set(afr_ports_dir   "${cy_board_dir}/ports")

#--------------------------------------------------------------------
# Set initial info for the application
#
# Pass argument to cmake "-DAPP=<path_to_application_directory>"
#      Application directory needs to have a CMakeLists.txt file
# We include the file here to get some app-specific sources and settings
#
# Define these on the cmake command line if you wish something different from 1.0.0
# "-DAPP_VERSION_MAJOR=x"
# "-DAPP_VERSION_MINOR=y"
# "-DAPP_VERSION_BUILD=z"
# So that the version # will be x.y.z

    # -------------------------------------------------------------------------------------------------
    # Amazon FreeRTOS demos and tests
    # -------------------------------------------------------------------------------------------------
    # We require you to define at least demos and tests executable targets. Available demos and tests
    # will be automatically enabled by us. You need to provide other project settings such as linker
    # scripts and post build commands.
    # ==================== Example ====================

if(APP)
    set(AFR_TARGET_APP_DIR ${APP})
    set(CY_ALTERNATE_APP    "1")
    get_filename_component(AFR_TARGET_APP_NAME "${AFR_TARGET_APP_DIR}" NAME)
elseif(AFR_IS_TESTING)
    set(AFR_TARGET_APP_NAME aws_tests)
    set(AFR_TARGET_APP_DIR "${cy_board_dir}/aws_tests/application_code")
else()
    set(AFR_TARGET_APP_NAME aws_demos)
    set(AFR_TARGET_APP_DIR "${cy_board_dir}/aws_demos/application_code")
endif()

#----------------------------------------------------------------
# Create the output executable
# We do this early so that we can use these cmake functions to add elements to the executable:
#
# target_sources($[AFR_TARGET_APP_NAME} ... )
# target_compile_definitions(${AFR_TARGET_APP_NAME} PUBLIC ... )
# target_include_directories($[AFR_TARGET_APP_NAME} PUBLIC ... )
# target_link_options(${AFR_TARGET_APP_NAME} PUBLIC ... )
#
add_executable( ${AFR_TARGET_APP_NAME}
            "${AFR_TARGET_APP_DIR}/main.c"
            )


#--------------------------------------------------------------------
# generate compiler MAP file
#--------------------------------------------------------------------
if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
    target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "-Wl,--Map=${AFR_TARGET_APP_NAME}.map")
elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
    target_link_options(${AFR_TARGET_APP_NAME} PUBLIC --info sizes)
    target_link_options(${AFR_TARGET_APP_NAME} PUBLIC --callgraph)
    target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "--map")
elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
    target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "--map=${AFR_TARGET_APP_NAME}.map")
endif()

#--------------------------------------------------------------------
# Location of mcuboot for sources & keys to sign HEX output
#
# <dev_root>
#   |-- amazon-freertos
#   |-- mcuboot
#
# You can change this as a command line arg "-DMCUBOOT_DIR=<dir>"
#
if((NOT MCUBOOT_DIR) OR ("${MCUBOOT_DIR}" STREQUAL ""))
    set(MCUBOOT_DIR             "${cy_common_dir}/mcuboot")
else()
    get_filename_component(MCUBOOT_DIR "${MCUBOOT_DIR}" ABSOLUTE)
    message("MCUBoot directory: ${MCUBOOT_DIR}")
endif()

# signing scripts and keys from MCUBoot
set(IMGTOOL_SCRIPT_NAME         "./imgtool.py")
set(MCUBOOT_SCRIPT_FILE_DIR     "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/tools/scripts")
set(MCUBOOT_KEY_DIR             "${cy_common_dir}/mcuboot/keys")
# look for mcuboot_logging.h locally before mcuboot repo
set(MCUBOOT_CYFLASH_PAL_DIR     "${cy_common_dir}/mcuboot/cy_flash_pal")

#--------------------------------------------------------------------
# Allow application to add elements of the build
#
if(CY_ALTERNATE_APP)
    include("${AFR_TARGET_APP_DIR}/CMakeLists.txt")

    #--------------------------------------------------------------------
    # check for version #s
    set(VERSION_CHECK "${INTERFACE_COMPILE_DEFINITIONS}")
    list(FILTER VERSION_CHECK_WORK INCLUDE REGEX "APP_VERSION_MAJOR")
    if(VERSION_CHECK_WORK STREQUAL "")
        message("WARNING: APP_VERSION_MAJOR not set - using 1")
        target_compile_definitions(${AFR_TARGET_APP_NAME} PUBLIC "-DAPP_VERSION_MAJOR=1")
    endif()

    set(VERSION_CHECK "${INTERFACE_COMPILE_DEFINITIONS}")
    list(FILTER VERSION_CHECK_WORK INCLUDE REGEX "APP_VERSION_MINOR")
    if(VERSION_CHECK_WORK STREQUAL "")
        message("WARNING: APP_VERSION_MINOR not set - using 0")
        target_compile_definitions(${AFR_TARGET_APP_NAME} PUBLIC "-DAPP_VERSION_MINOR=0")
    endif()

    set(VERSION_CHECK "${INTERFACE_COMPILE_DEFINITIONS}")
    list(FILTER VERSION_CHECK_WORK INCLUDE REGEX "APP_VERSION_BUILD")
    if(VERSION_CHECK_WORK STREQUAL "")
        message("WARNING: APP_VERSION_BUILD not set - using 0")
        target_compile_definitions(${AFR_TARGET_APP_NAME} PUBLIC "-DAPP_VERSION_BUILD=0")
    endif()

    #--------------------------------------------------------------------
    # Disable aws_demos and aws_tests for alternate builds
    #
    set(AFR_ENABLE_DEMOS 0 CACHE BOOL "Build demos for Amazon FreeRTOS." FORCE)
    set(AFR_ENABLE_TESTS 0 CACHE BOOL "Build demos for Amazon FreeRTOS." FORCE)

endif(CY_ALTERNATE_APP)

#--------------------------------------------------------------------
# Common libraries we need for all apps
#
target_link_libraries(${AFR_TARGET_APP_NAME} PUBLIC
    "AFR::common"
    )
