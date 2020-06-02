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
# Additional defines
target_compile_definitions(${AFR_TARGET_APP_NAME} PUBLIC
    "-DAPP_VERSION_MAJOR=1"
    "-DAPP_VERSION_MINOR=0"
    "-DAPP_VERSION_BUILD=0"
    "-DCY_RETARGET_IO_CONVERT_LF_TO_CRLF"
    )

# Application sources
# ( main.c already included in vendors/cypress/psoc6/cmake/cy_defines.cmake)
# target_sources(${AFR_TARGET_APP_NAME} PUBLIC "" )

# Additional include dirs
# target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC "" )

# AWS Configuration directory (set if not using default: vendors/cypress/apps/<appname>/config_files)
# set(CY_APP_CONFIG_DIR "")

# AWS IOT Configuration directory (set if not using default: vendors/cypress/apps/<appname>/include)
set(CY_APP_IOT_CONFIG_DIR "${AFR_DEMOS_DIR}/include")

# Additional libs or link options we need
# target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "<lib_name>" )

# Extra linker options
# target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "" )


#--------------------------------------------------------------------
# Disable AFR aws_demos Modules
# These AFR aws_demos modules are not needed for the wifi scanner to build
set(CY_APP_DISABLE_AFR_MODULES
    "defender"
    "greengrass"
    "shadow"
    )

