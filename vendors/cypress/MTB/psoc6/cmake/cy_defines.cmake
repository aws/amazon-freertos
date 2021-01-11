#--------------------------------------------------------------------
# Defines that may be used in application-specific CMakeLists.txt
#
# This file is included in multiple Cypress cmake files
#
#--------------------------------------------------------------------

#--------------------------------------------------------------------
# common directories
#--------------------------------------------------------------------
set(cy_mtb_dir                  "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/MTB")

set(cy_port_support_dir         "${cy_mtb_dir}/port_support")

set(cy_libraries_dir            "${cy_mtb_dir}/libraries")
set(cy_libraries_clib_dir       "${cy_libraries_dir}/clib-support")
set(cy_libraries_whd_dir        "${cy_libraries_dir}/wifi-host-driver")
set(cy_libraries_capsense_dir   "${cy_libraries_dir}/capsense")
set(cy_libararies_bt_dir        "${cy_libraries_dir}/bluetooth")

set(cy_psoc6_dir                "${cy_mtb_dir}/psoc6")
set(cy_psoc6_cmake_dir          "${cy_psoc6_dir}/cmake")

set(cy_board_dir                "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/boards/${AFR_BOARD_NAME}")
set(afr_ports_dir               "${cy_board_dir}/ports")

if(AFR_IS_TESTING)
    set(app_dir                 "${cy_board_dir}/aws_tests")
    set(iot_common_dir          "${AFR_TESTS_DIR}/include")
else()
    set(app_dir                 "${cy_board_dir}/aws_demos")
    set(iot_common_dir          "${AFR_DEMOS_DIR}/include")
endif()

if (DEFINED LIB_MODE)
    if (DEFINED CY_INCLUDE_DIRS)
        set(include_dirs ${CY_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "CY_INCLUDE_DIRS must be defined to include headers for AWS config, FreeRTOS config, and BSP config")
    endif()
else()
    set(include_dirs "${app_dir}/config_files;${iot_common_dir}")
endif()


#--------------------------------------------------------------------
# MCUBOOT directory used for inclusion of C files and Header directories
#
# You can change this as a command line arg "-DMCUBOOT_DIR=<dir>"
#
if((NOT MCUBOOT_DIR) OR ("${MCUBOOT_DIR}" STREQUAL ""))
    set(MCUBOOT_DIR             "${cy_port_support_dir}/ota/mcuboot")
else()
    get_filename_component(MCUBOOT_DIR "${MCUBOOT_DIR}" ABSOLUTE)
    message("DEFINES SETTING MCUBoot directory: ${MCUBOOT_DIR}")
endif()
#
# You can change this as a command line arg "-DMCUBOOT_CYFLASH_PAL_DIR=<dir>"
#
if((NOT MCUBOOT_CYFLASH_PAL_DIR) OR ("${MCUBOOT_CYFLASH_PAL_DIR}" STREQUAL ""))
    set(MCUBOOT_CYFLASH_PAL_DIR     "${MCUBOOT_DIR}/cy_flash_pal")
else()
    get_filename_component(MCUBOOT_CYFLASH_PAL_DIR "${MCUBOOT_CYFLASH_PAL_DIR}" ABSOLUTE)
    message("DEFINES SETTING MCUBoot directory: ${MCUBOOT_CYFLASH_PAL_DIR}")
endif()

include("${cy_psoc6_cmake_dir}/cy_glob_utils.cmake")