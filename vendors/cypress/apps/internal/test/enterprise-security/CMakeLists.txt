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
#
# Additional defines
target_compile_definitions(${AFR_TARGET_APP_NAME} PUBLIC
    "-DAPP_VERSION_MAJOR=${APP_VERSION_MAJOR}"
    "-DAPP_VERSION_MINOR=${APP_VERSION_MINOR}"
    "-DAPP_VERSION_BUILD=${APP_VERSION_BUILD}"
    "-DCY_RETARGET_IO_CONVERT_LF_TO_CRLF"
    "-DCY_USE_LWIP"
    "-DAFR"
    )

set(CY_APP_INTERNAL_LIB_DIR "${CY_APP_DIR}/../../../../libraries/internal/" )
set(CY_APP_LIB_DIR "${CY_APP_DIR}/../../../../libraries/" )

target_compile_definitions(afr_3rdparty_mbedtls PUBLIC
    "-DMBEDTLS_USER_CONFIG_FILE=\"mbedtls_user_config_afr.h\""
    )

# Application sources
file(GLOB_RECURSE CY_APP_EXTRA_SOURCES
    "${CY_APP_INTERNAL_LIB_DIR}/command-console/*.c"
    "${CY_APP_INTERNAL_LIB_DIR}/command-console/*.cpp"
    "${CY_APP_LIB_DIR}/enterprise-security/*.c"
    )

target_sources(${AFR_TARGET_APP_NAME} PUBLIC "${CY_APP_EXTRA_SOURCES}" )

# Additional include dirs
target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC
                          "${CY_APP_INTERNAL_LIB_DIR}/command-console/iperf/rtos/AFR"
                          "${CY_APP_INTERNAL_LIB_DIR}/command-console/command_console"
                          "${CY_APP_INTERNAL_LIB_DIR}/command-console/iperf_utility"
                          "${CY_APP_INTERNAL_LIB_DIR}/command-console/test_console_utility"
                          "${CY_APP_INTERNAL_LIB_DIR}/command-console/iperf/include"
                          "${CY_APP_INTERNAL_LIB_DIR}/command-console/command_console/rtos/AFR"
                          "${CY_APP_INTERNAL_LIB_DIR}/utilities"
                          "${CY_APP_LIB_DIR}/enterprise-security"
                          "${CY_APP_LIB_DIR}/enterprise-security/src"
                          "${CY_APP_LIB_DIR}/enterprise-security/src/supplicant_core"
                          "${CY_APP_LIB_DIR}/enterprise-security/src/abstraction/tls"
                          "${CY_APP_LIB_DIR}/enterprise-security/src/abstraction/rtos/afr"
                          "${CY_APP_LIB_DIR}/enterprise-security/src/supplicant_core/supplicant_host"
                          )
set(CY_APP_EXTRA_INCLUDE_DIRS "${CY_APP_EXTRA_INCLUDE_DIRS}" "${CY_APP_LIB_DIR}/enterprise-security" )

# Disable AFR aws_demos Modules
# set(CY_APP_DISABLE_AFR_MODULES "")
set(CY_APP_DISABLE_AFR_MODULES
    "defender"
    "greengrass"
    "mqtt"
    "shadow"
    )
