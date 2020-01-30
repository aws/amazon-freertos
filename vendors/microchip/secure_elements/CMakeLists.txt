cmake_minimum_required(VERSION 2.6.4)
project (cryptoauthlib)

# Set the current release version
set(VERSION "2018.10.26")
set(VERSION_MAJOR 2028)
set(VERSION_MINOR 10)
set(VERSION_PATCH 26)

# Build Options
option(BUILD_TESTS "Create Test Application with library" OFF)
set(ATCA_PKCS11 ON CACHE INTERNAL "")

# Default install root which is normally /usr/local/
set(CMAKE_INSTALL_PREFIX "/" CACHE INTERNAL "")

# If including certificate definitions into the library then include them as ATCACERT_DEF_SRC
#file(GLOB ATCACERT_DEF_SRC ABSOLUTE "app/*.c")

if(WIN32)
string(REPLACE "\\" "/" LOCAL_APP_DATA "$ENV{LOCALAPPDATA}/Microchip")
set(DEFAULT_LIB_PATH "${LOCAL_APP_DATA}" CACHE
    STRING "The default absolute library path")
set(DEFAULT_CONF_PATH "${LOCAL_APP_DATA}" CACHE
    STRING "The default location of ${PROJECT_NAME}.conf")
set(DEFAULT_STORE_PATH "${LOCAL_APP_DATA}/${PROJECT_NAME}" CACHE
    STRING "The default location of the filestore directory")
else()
include(GNUInstallDirs)
set(DEFAULT_LIB_PATH "${CMAKE_INSTALL_FULL_LIBDIR}" CACHE
    STRING "The default absolute library path")
set(DEFAULT_CONF_PATH "${CMAKE_INSTALL_FULL_SYSCONFDIR}/${PROJECT_NAME}" CACHE
    STRING "The default location of ${PROJECT_NAME}.conf")
set(DEFAULT_STORE_PATH "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/lib/${PROJECT_NAME}" CACHE
    STRING "The default location of the filestore directory")
endif()

set(DEFAULT_CONF_FILE_NAME "${PROJECT_NAME}.conf" CACHE 
    STRING "The default file for library configuration")

if(NOT CMAKE_BUILD_TYPE)
if(WIN32)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Default build type" FORCE)
else()
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Default build type" FORCE)
endif()
endif()

# Set up a default configuration file to install
configure_file(${PROJECT_SOURCE_DIR}/app/pkcs11/cryptoauthlib.conf.in ${PROJECT_BINARY_DIR}/${DEFAULT_CONF_FILE_NAME})

message(STATUS "Building Configuration: ${CMAKE_BUILD_TYPE}")

# Packaging
set(CPACK_PACKAGE_VENDOR "Microchip Technology Inc")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "build/*;\\.git/*")

include(CPack)

add_subdirectory(lib)

# Tests
if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)

# Installation
install(DIRECTORY DESTINATION ${DEFAULT_CONF_PATH})
install(CODE "
        if(NOT EXISTS ${DEFAULT_CONF_PATH}/${DEFAULT_CONF_FILE_NAME})
            file(INSTALL ${PROJECT_BINARY_DIR}/${DEFAULT_CONF_FILE_NAME}
                 DESTINATION ${DEFAULT_CONF_PATH})
        endif()
        ")
install(DIRECTORY DESTINATION ${DEFAULT_STORE_PATH})
install(CODE "execute_process(COMMAND chmod 1777 ${DEFAULT_STORE_PATH})")
install(CODE "
        if(NOT EXISTS ${DEFAULT_STORE_PATH}/slot.conf.tmpl)
            file(INSTALL ${PROJECT_SOURCE_DIR}/app/pkcs11/slot.conf.tmpl
                 DESTINATION ${DEFAULT_STORE_PATH})
        endif()
        ")

