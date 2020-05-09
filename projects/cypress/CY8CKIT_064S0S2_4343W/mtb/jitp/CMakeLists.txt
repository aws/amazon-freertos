#
# Application specific CMakeLists.txt file
#   Included from vendors/cypress/psoc6/cmake/cy_kit_utils.cmake
#
# Add app specific sources, defines, includes here
#
#--------------------------------------------------------------------
# This app dir
get_filename_component(CY_APP_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

#--------------------------------------------------------------------
#
# JITP
#
set(OTA_SUPPORT "1")

#--------------------------------------------------------------------
# App version for aws_demo_version.h
#
# You can over-ride from command line when running cmake
# ex: "-DAPP_VERSION_MAJOR=1"
#
if(NOT "${APP_VERSION_MAJOR}" STREQUAL "")
    set(APP_VERSION_MAJOR     "${APP_VERSION_MAJOR}")
else()
    set(APP_VERSION_MAJOR     "0")
endif()

if(NOT "${APP_VERSION_MINOR}" STREQUAL "")
    set(APP_VERSION_MINOR     "${APP_VERSION_MINOR}")
else()
    set(APP_VERSION_MINOR     "0")
endif()

if(NOT "${APP_VERSION_BUILD}" STREQUAL "")
    set(APP_VERSION_BUILD     "${APP_VERSION_BUILD}")
else()
    set(APP_VERSION_BUILD     "0")
endif()

#--------------------------------------------------------------------
# Additional defines
target_compile_definitions(${AFR_TARGET_APP_NAME} PUBLIC
    "-DAPP_VERSION_MAJOR=${APP_VERSION_MAJOR}"
    "-DAPP_VERSION_MINOR=${APP_VERSION_MINOR}"
    "-DAPP_VERSION_BUILD=${APP_VERSION_BUILD}"
    "-DCY_RETARGET_IO_CONVERT_LF_TO_CRLF"
    )

# Application sources
# ( main.c already included in vendors/cypress/psoc6/cmake/cy_defines.cmake)
# target_sources(${AFR_TARGET_APP_NAME} PUBLIC "" )

# Additional include dirs
# target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC "<include_dir>" )

# AWS Configuration directory (set if not using default: vendors/cypress/apps/<appname>/config_files)
# set(CY_APP_CONFIG_DIR "")

# AWS IOT Configuration directory (set if not using default: vendors/cypress/apps/<appname>/include)
# set(CY_APP_IOT_CONFIG_DIR "")

# Additional libs we need
# target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "<lib_name>" )

# Extra linker options
# target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "" )


#--------------------------------------------------------------------
# Disable AFR aws_demos Modules
# These AFR aws_demos modules are not needed
set(CY_APP_DISABLE_AFR_MODULES
    "defender"
    "greengrass"
    "shadow"
    )

target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC
    "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/include"
    "${AFR_DEMOS_DIR}/network_manager"
    "${cy_board_dir}/ports/pkcs11/psa"
    )

