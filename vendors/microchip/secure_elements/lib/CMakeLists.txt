cmake_minimum_required(VERSION 2.6.4)
project(cryptoauth)

# Various Options for Build
option(ATCA_HAL_KIT_HID "Include the HID HAL Driver")
option(ATCA_HAL_I2C "Include the I2C Hal Driver - Linux & MCU only" ON)
option(ATCA_HAL_CUSTOM "Include support for Custom/Plug-in Hal Driver" ON)
option(ATCA_PRINTF "Enable Debug print statements in library")
option(ATCA_PKCS11 "Build PKCS11 Library")
option(ATCA_MBEDTLS "Integrate with mbedtls")
option(ATCA_BUILD_SHARED_LIBS "Build CryptoAuthLib as shared library" ON)
option(PKCS11_DEBUG_ENABLE "Enable PKCS11 Debug Output" OFF)

set(BUILD_SHARED_LIBS ${ATCA_BUILD_SHARED_LIBS})

# Collect Library Sources
file(GLOB LIB_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB ATCACERT_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "atcacert/*.c")
file(GLOB BASIC_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "basic/*.c")
file(GLOB_RECURSE CRYPTO_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "crypto/*.c")
file(GLOB HOST_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "host/*.c")
file(GLOB JWT_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "jwt/*.c")
file(GLOB PKCS11_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "pkcs11/*.c")
file(GLOB MBEDTLS_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "mbedtls/*.c")
file(GLOB TNG_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../app/tng/*.c")

# Add the basic sources to the library
set(CRYPTOAUTH_SRC ${LIB_SRC} ${ATCACERT_SRC} ${BASIC_SRC} ${CRYPTO_SRC} ${HOST_SRC} ${JWT_SRC} hal/atca_hal.c ${TNG_SRC})

if(WIN32)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_windows.c)
set(HID_SRC ../third_party/hidapi/windows/hid.c)
elseif(APPLE)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_linux.c)
set(HID_SRC ../third_party/hidapi/mac/hid.c)
elseif(UNIX)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_linux.c)
set(TWI_SRC hal/hal_linux_i2c_userspace.c)
set(LINUX TRUE)
endif()

if(LINUX AND ATCA_HAL_KIT_HID)
find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_path(LIBUDEV_INCLUDE_DIR NAMES libudev.h PATH_SUFFIXES "include")
find_library(HAS_LIBUSB usb-1.0)
find_library(HAS_LIBUDEV udev)
if(HAS_LIBUSB AND LIBUSB_INCLUDE_DIR)
set(HID_SRC ../third_party/hidapi/libusb/hid.c)
elseif(HAS_LIBUDEV AND LIBUDEV_INCLUDE_DIR)
set(LIBUSB_INCLUDE_DIR ${LIBUDEV_INCLUDE_DIR})
set(HID_SRC ../third_party/hidapi/linux/hid.c)
else()
message(FATAL_ERROR, "Missing Build Dependencies for ATCA_HAL_KIT_HID - install libusb-1.0-0-dev or libudev-dev")
endif()
else()
set(LIBUSB_INCLUDE_DIR "")
endif()

if(ATCA_HAL_KIT_HID)
add_definitions(-DATCA_HAL_KIT_HID)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/kit_protocol.c hal/hal_all_platforms_kit_hidapi.c ${HID_SRC})
endif(ATCA_HAL_KIT_HID)

if(LINUX AND ATCA_HAL_I2C)
add_definitions(-DATCA_HAL_I2C)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${TWI_SRC})
endif(LINUX AND ATCA_HAL_I2C)

if(ATCA_HAL_CUSTOM)
add_definitions(-DATCA_HAL_CUSTOM)
endif()

# Add Remaining Sources depending on target library type
if(ATCA_MBEDTLS)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${MBEDTLS_SRC})
endif()

if(ATCA_PKCS11)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${PKCS11_SRC})
add_definitions(-DATCA_LIBRARY_CONF="${DEFAULT_CONF_PATH}/${DEFAULT_CONF_FILE_NAME}")
if(PKCS11_DEBUG_ENABLE)
add_definitions(-DPKCS11_DEBUG_ENABLE)
endif(PKCS11_DEBUG_ENABLE)
endif()

add_library(cryptoauth ${CRYPTOAUTH_SRC} ${ATCACERT_DEF_SRC})

if(ATCA_PRINTF)
add_definitions(-DATCAPRINTF)
endif(ATCA_PRINTF)

include_directories(cryptoauth PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../third_party/hidapi/hidapi ${LIBUSB_INCLUDE_DIR})

if(WIN32)
set_target_properties(cryptoauth PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
target_link_libraries(cryptoauth setupapi.lib)
endif(WIN32)

if(APPLE)
find_library(IO_KIT_LIB IOKit)
find_library(CORE_LIB CoreFoundation)
target_link_libraries(cryptoauth ${IO_KIT_LIB} ${CORE_LIB})
endif()

if(LINUX)
add_definitions(-DATCA_USE_SHARED_MUTEX)
if(HAS_LIBUSB AND ATCA_HAL_KIT_HID)
target_link_libraries(cryptoauth usb-1.0)
elseif(HAS_UDEV AND ATCA_HAL_KIT_HID)
target_link_libraries(cryptoauth udev)
endif()
target_link_libraries(cryptoauth rt)
endif()


if(DEFAULT_LIB_PATH)
install(TARGETS ${PROJECT_NAME} DESTINATION ${DEFAULT_LIB_PATH})
endif()
