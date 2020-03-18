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
    "-DCY_RETARGET_IO_CONVERT_LF_TO_CRLF=1"
    "-DCY_USE_LWIP=1"
    )

set(CY_APP_INTERNAL_LIB_DIR "${CY_APP_DIR}/../../libraries/internal/" )
set(CY_APP_LIB_DIR "${CY_APP_DIR}/../../libraries/" )

target_compile_definitions(afr_3rdparty_mbedtls PUBLIC
    "-DMBEDTLS_USER_CONFIG_FILE=\"mbedtls_user_config_afr.h\""
    )

# Application sources
file(GLOB_RECURSE CY_APP_EXTRA_SOURCES
    "${CY_APP_LIB_DIR}/enterprise-security/src/cy_enterprise_security.c"
    "${CY_APP_LIB_DIR}/enterprise-security/src/abstraction/tls/*.c"
    "${CY_APP_LIB_DIR}/enterprise-security/src/supplicant_core/*.c"
    "${CY_APP_LIB_DIR}/enterprise-security/src/abstraction/rtos/afr/cy_enterprise_security_interface.c"
    "${CY_APP_LIB_DIR}/enterprise-security/src/abstraction/rtos/afr/cy_rtos_abstraction.c"
    "${CY_APP_LIB_DIR}/enterprise-security/src/abstraction/rtos/afr/cy_wifi_abstraction.c"
    )

target_sources(${AFR_TARGET_APP_NAME} PUBLIC "${CY_APP_EXTRA_SOURCES}" )

# Additional include dirs
target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC
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
    "serializer"
    )
