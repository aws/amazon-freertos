cmake_minimum_required(VERSION 3.1.0)
project(cryptoauth C)

include(CMakeDependentOption)
include(CheckSymbolExists)

# HAL Selection
option(ATCA_HAL_KIT_HID "Include the HID HAL Driver")
option(ATCA_HAL_KIT_BRIDGE "General purpose kit protocol (Packet and Stream)")
option(ATCA_HAL_I2C "Include the I2C Hal Driver - Linux & MCU only")
option(ATCA_HAL_SPI "Include the SPI HAL Driver - Linux & MCU only")
option(ATCA_HAL_CUSTOM "Include support for Custom/Plug-in Hal Driver")

# Library Options
option(ATCA_PRINTF "Enable Debug print statements in library")
option(ATCA_PKCS11 "Build PKCS11 Library")
option(ATCA_BUILD_SHARED_LIBS "Build CryptoAuthLib as shared library" ON)
option(ATCA_NO_HEAP "Do not use dynamic (heap) allocation functions" OFF)
option(ATCA_USE_ATCAB_FUNCTIONS "Build the atcab_ api functions rather than using macros" OFF)
option(ATCA_ENABLE_DEPRECATED "Enable the use of older APIs that that been replaced" OFF)

# Software Cryptographic backend for host crypto abstractions
option(ATCA_MBEDTLS "Integrate with mbedtls" OFF)
option(ATCA_WOLFSSL "Integrate with WolfSSL" OFF)
option(ATCA_OPENSSL "Integration with OpenSSL" OFF)

# Trust Platform Options
option(ATCA_TNGTLS_SUPPORT "Include Trust & Go TLS Certificates")
option(ATCA_TNGLORA_SUPPORT "Include Trust & Go LORA Certificates")
option(ATCA_TFLEX_SUPPORT "Include Trust Flex Certificates")
option(ATCA_TNG_LEGACY_SUPPORT "Include previous version of Trust & Go Certificates")

# Device enablement
option(ATCA_ATSHA204A_SUPPORT "Include support for ATSHA204A device" ON)
option(ATCA_ATSHA206A_SUPPORT "Include support for ATSHA206A device" ON)
option(ATCA_ATECC108A_SUPPORT "Include support for ATECC108A device" ON)
option(ATCA_ATECC508A_SUPPORT "Include support for ATECC508A device" ON)
option(ATCA_ATECC608_SUPPORT "Include support for ATECC608 device" ON)
option(ATCA_TA100_SUPPORT "Include support for TA100 device" OFF)
option(ATCA_ECC204_SUPPORT "Include support for ECC204 device" ON)


# TA100 Dependent Configuration options
cmake_dependent_option(ATCA_TA100_AES_AUTH_SUPPORT "Include Encrypted (GCM) and CMAC Auth session support" ON "ATCA_TA100_SUPPORT" OFF)
cmake_dependent_option(ATCA_TA100_FCE_SUPPORT "Include FCE Support for the TA100" ON "ATCA_TA100_SUPPORT" OFF)

# Check Options
if (ATCA_MBEDTLS AND (ATCA_WOLFSSL OR ATCA_OPENSSL))
message(FATAL_ERROR "Only one external SSL/TLS library can be supported")
elseif (ATCA_WOLFSSL AND (ATCA_MBEDTLS OR ATCA_OPENSSL))
message(FATAL_ERROR "Only one external SSL/TLS library can be supported")
endif()

if (ATCA_TA100_SUPPORT AND ATCA_TA100_AES_AUTH_SUPPORT AND NOT (ATCA_MBEDTLS OR ATCA_WOLFSSL OR ATCA_OPENSSL))
message(FATAL_ERROR "Encrypted (GCM) and CMAC uthorization sessions with the TA100 require a linked crypto library")
endif()

# Check Platform Information
check_symbol_exists(malloc "stdlib.h" HAS_MALLOC)
check_symbol_exists(free "stdlib.h" HAS_FREE)


set(BUILD_SHARED_LIBS ${ATCA_BUILD_SHARED_LIBS})

# Collect Library Sources
file(GLOB LIB_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB LIB_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
file(GLOB ATCACERT_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "atcacert/*.c")
file(GLOB ATCACERT_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "atcacert/*.h")
file(GLOB CALIB_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "calib/*.c")
file(GLOB CALIB_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "calib/*.h")
file(GLOB TALIB_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "talib/*.c")
file(GLOB TALIB_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "talib/*.h")
file(GLOB_RECURSE CRYPTO_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "crypto/*.c")
file(GLOB CRYPTO_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "crypto/*.h")
file(GLOB CRYPTO_HASHES_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "crypto/hashes/*.h")
file(GLOB HOST_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "host/*.c")
file(GLOB HOST_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "host/*.h")
file(GLOB JWT_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "jwt/*.c")
file(GLOB JWT_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "jwt/*.h")
file(GLOB PKCS11_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "pkcs11/*.c")
file(GLOB PKCS11_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "pkcs11/*.h")
file(GLOB TNG_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../app/tng/*.c")
file(GLOB TNG_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../app/tng/*.h")
file(GLOB SHA206_API_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../app/api_206a/*.c")
file(GLOB SHA206_API_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../app/api_206a/*.h")

if(ATCA_PKCS11 AND (ATCA_TNGTLS_SUPPORT OR ATCA_TNGLORA_SUPPORT OR ATCA_TFLEX_SUPPORT))
SET(TNG_SRC ${TNG_SRC} ../app/pkcs11/trust_pkcs11_config.c)
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ATCACERT_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CALIB_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TALIB_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CRYPTO_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HOST_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${JWT_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PKCS11_SRC})
source_group("App/Tng" FILES ${TNG_SRC})
endif()

if (ATCA_MBEDTLS)
if (NOT TARGET mbedtls)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/CMakeLists-mbedtls.txt.in ${CMAKE_BINARY_DIR}/${DEPENDENCY_DIR}/mbedtls_downloader/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${DEPENDENCY_DIR}/mbedtls_downloader/)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${DEPENDENCY_DIR}/mbedtls_downloader/)

file(GLOB MBEDTLS_LIB_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../third_party/mbedtls/library/*.c")
add_library(mbedtls STATIC ${MBEDTLS_LIB_SRC})
target_compile_definitions(mbedtls PUBLIC -DMBEDTLS_CMAC_C)
if(NOT WIN32)
target_compile_options(mbedtls PRIVATE -fPIC)
endif()
include_directories(mbedtls PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/mbedtls/include)
endif()

file(GLOB MBEDTLS_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "mbedtls/*.c")
if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MBEDTLS_SRC})
endif()
endif(ATCA_MBEDTLS)

if (ATCA_WOLFSSL)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/CMakeLists-wolfssl.txt.in ${CMAKE_BINARY_DIR}/${DEPENDENCY_DIR}/wolfssl_downloader/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${DEPENDENCY_DIR}/wolfssl_downloader/)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${DEPENDENCY_DIR}/wolfssl_downloader/)

#file(GLOB WOLFSSL_LIB_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../third_party/wolfssl/wolfcrypt/src/*.c")

set(WOLFSSL_LIB_SRC ../third_party/wolfssl/wolfcrypt/src/aes.c
                    ../third_party/wolfssl/wolfcrypt/src/arc4.c
                    ../third_party/wolfssl/wolfcrypt/src/asn.c
                    ../third_party/wolfssl/wolfcrypt/src/cmac.c
                    ../third_party/wolfssl/wolfcrypt/src/coding.c
                    ../third_party/wolfssl/wolfcrypt/src/des3.c
		    ../third_party/wolfssl/wolfcrypt/src/ecc.c
                    ../third_party/wolfssl/wolfcrypt/src/hash.c
                    ../third_party/wolfssl/wolfcrypt/src/hmac.c
                    ../third_party/wolfssl/wolfcrypt/src/integer.c
                    ../third_party/wolfssl/wolfcrypt/src/memory.c
                    ../third_party/wolfssl/wolfcrypt/src/pwdbased.c
                    ../third_party/wolfssl/wolfcrypt/src/random.c
                    ../third_party/wolfssl/wolfcrypt/src/rsa.c
                    ../third_party/wolfssl/wolfcrypt/src/sha.c
                    ../third_party/wolfssl/wolfcrypt/src/sha256.c
                    ../third_party/wolfssl/wolfcrypt/src/tfm.c
                    ../third_party/wolfssl/wolfcrypt/src/wc_encrypt.c
                    ../third_party/wolfssl/wolfcrypt/src/wc_port.c
                    ../third_party/wolfssl/wolfcrypt/src/wolfmath.c
                    )


add_library(wolfssl STATIC ${WOLFSSL_LIB_SRC})
target_compile_definitions(wolfssl PRIVATE -DWC_NO_HARDEN -DHAVE_ECC -DHAVE_ALL_CURVES -DWOLFCRYPT_ONLY -DWOLFSSL_PEM_TO_DER -DWOLFSSL_VALIDATE_ECC_IMPORT -DWOLFSSL_AES_DIRECT -DHAVE_AESGCM -DWOLFSSL_CMAC -DNO_OLD_TLS -DNO_MD5)
include_directories(wolfssl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/wolfssl)


file(GLOB WOLFSSL_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "wolfssl/*.c")
if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${WOLFSSL_SRC})
endif()
endif(ATCA_WOLFSSL)

if (ATCA_OPENSSL)
file(GLOB OPENSSL_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "openssl/*.c")
if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${OPENSSL_SRC})
endif()
find_package(OpenSSL REQUIRED)
endif(ATCA_OPENSSL)

if (ATCA_ATSHA204A_SUPPORT OR ATCA_ATSHA206A_SUPPORT OR
    ATCA_ATECC108A_SUPPORT OR ATCA_ATECC508A_SUPPORT OR
    ATCA_ATECC608_SUPPORT OR ATCA_ECC204_SUPPORT)
set(LIB_SRC ${LIB_SRC} ${CALIB_SRC} ${HOST_SRC})
endif()

if (ATCA_ATSHA206A_SUPPORT)
set(LIB_SRC ${LIB_SRC} ${SHA206_API_SRC})
endif()

if (ATCA_TA100_SUPPORT)
set(LIB_SRC ${LIB_SRC} ${TALIB_SRC})
endif()

# Add the basic sources to the library
set(CRYPTOAUTH_SRC ${LIB_SRC}
                   ${ATCACERT_SRC}
                   ${CRYPTO_SRC}
                   ${JWT_SRC}
                   ${TNG_SRC}
                   ${MBEDTLS_SRC}
                   ${WOLFSSL_SRC}
                   ${OPENSSL_SRC}
                   hal/atca_hal.c )

set(HAL_INC hal/atca_hal.h)

if(ATCA_HAL_KIT_HID OR ATCA_HAL_KIT_CDC)
set(NEED_USB true)
endif()

if(WIN32)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_windows.c)
set(HID_SRC ../third_party/hidapi/windows/hid.c)
set(CDC_SRC hal/hal_win_kit_cdc.c)
elseif(APPLE)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_linux.c)
set(HID_SRC ../third_party/hidapi/mac/hid.c)
elseif(UNIX)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_linux.c)
set(TWI_SRC hal/hal_linux_i2c_userspace.c)
set(SPI_SRC hal/hal_linux_spi_userspace.c)
set(LINUX TRUE)
endif()

if(LINUX AND NEED_USB)
find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_path(LIBUDEV_INCLUDE_DIR NAMES libudev.h PATH_SUFFIXES "include")
find_library(HAS_LIBUSB usb-1.0)
find_library(HAS_LIBUDEV udev)

if(HAS_LIBUSB AND LIBUSB_INCLUDE_DIR)
set(LIBUSB_GOOD TRUE)
endif()

if(HAS_LIBUDEV AND LIBUDEV_INCLUDE_DIR)
set(LIBUDEV_GOOD TRUE)
endif()

if(LIBUDEV_GOOD)
set(USE_UDEV TRUE)
elseif(LIBUSB_GOOD AND NOT ATCA_HAL_KIT_CDC)
set(USE_LIBUSB TRUE)
else()
message(FATAL_ERROR, "Missing Build Dependencies for USB - install libusb-1.0-0-dev or libudev-dev")
endif()

endif(LINUX AND NEED_USB)

if(USE_UDEV)
set(USB_INCLUDE_DIR ${LIBUDEV_INCLUDE_DIR})
set(HID_SRC ../third_party/hidapi/linux/hid.c)
set(CDC_SRC hal/hal_linux_kit_cdc.c)
endif(USE_UDEV)

if(USE_LIBUSB)
set(USB_INCLUDE_DIR ${LIBUSB_INCLUDE_DIR})
set(HID_SRC ../third_party/hidapi/libusb/hid.c)
endif(USE_LIBUSB)

if(NEED_USB)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/kit_protocol.c)
endif()

if(ATCA_HAL_KIT_HID)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${HID_SRC} hal/hal_all_platforms_kit_hidapi.c)
endif(ATCA_HAL_KIT_HID)

if(ATCA_HAL_KIT_CDC)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${CDC_SRC})
endif(ATCA_HAL_KIT_CDC)

if(ATCA_HAL_I2C)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${TWI_SRC})
endif(ATCA_HAL_I2C)

if(ATCA_HAL_SPI)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${SPI_SRC})
endif(ATCA_HAL_SPI)

if(ATCA_HAL_KIT_BRIDGE)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_kit_bridge.c)
endif(ATCA_HAL_KIT_BRIDGE)

# Add Remaining Sources depending on target library type
if(ATCA_MBEDTLS)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${MBEDTLS_SRC})
endif()

if(ATCA_PKCS11)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${PKCS11_SRC})
set(ATCA_LIBRARY_CONF ${DEFAULT_CONF_PATH}/${DEFAULT_CONF_FILE_NAME} CACHE STRING "" FORCE)
if(PKCS11_DEBUG_ENABLE)
add_definitions(-DPKCS11_DEBUG_ENABLE)
endif(PKCS11_DEBUG_ENABLE)
endif()

if(ATCA_BUILD_SHARED_LIBS)
add_definitions(-DATCA_BUILD_SHARED_LIBS)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} atca_utils_sizes.c)
endif(ATCA_BUILD_SHARED_LIBS)

add_library(cryptoauth ${CRYPTOAUTH_SRC} ${ATCACERT_DEF_SRC})

set_property(TARGET cryptoauth PROPERTY C_STANDARD 99)

if(HAS_MALLOC)
set(ATCA_PLATFORM_MALLOC malloc CACHE STRING "" FORCE)
endif(HAS_MALLOC)

if(HAS_FREE)
set(ATCA_PLATFORM_FREE free CACHE STRING "" FORCE)
endif(HAS_FREE)

if(BUILD_TESTS)
set(ATCA_TESTS_ENABLED ON CACHE INTERNAL "")
endif(BUILD_TESTS)

configure_file(atca_config.h.in atca_config.h @ONLY)
set(LIB_INC ${LIB_INC} ${CMAKE_CURRENT_BINARY_DIR}/atca_config.h)

if(ATCA_PKCS11)
configure_file(pkcs11/pkcs11_config.h.in pkcs11_config.h @ONLY)
set(PKCS11_INC ${PKCS11_INC} ${CMAKE_CURRENT_BINARY_DIR}/pkcs11_config.h)
endif()

include_directories(cryptoauth PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ../app/tng ../third_party/hidapi/hidapi ${USB_INCLUDE_DIR})

if(ATCA_MBEDTLS)
target_link_libraries(cryptoauth mbedtls)
endif()

if(ATCA_WOLFSSL)
target_link_libraries(cryptoauth wolfssl)
endif()

if(ATCA_OPENSSL)
include_directories(cryptoauth PUBLIC ${OPENSSL_INCLUDE_DIR})
target_link_libraries(cryptoauth ${OPENSSL_CRYPTO_LIBRARY})
endif()

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
if(USE_LIBUSB)
target_link_libraries(cryptoauth usb-1.0)
elseif(USE_UDEV)
target_link_libraries(cryptoauth udev)
endif()
target_link_libraries(cryptoauth rt)
endif(LINUX)

if(DEFAULT_LIB_PATH)
if(${CMAKE_VERSION} VERSION_GREATER "3.12.0")
install(TARGETS ${PROJECT_NAME}
        LIBRARY
          DESTINATION ${DEFAULT_LIB_PATH}
          COMPONENT Libraries
          NAMELINK_COMPONENT Development)
else()
install(TARGETS ${PROJECT_NAME}
        LIBRARY
          DESTINATION ${DEFAULT_LIB_PATH}
          COMPONENT Libraries)
endif()
endif(DEFAULT_LIB_PATH)

if(DEFAULT_INC_PATH)
install(FILES ${LIB_INC} DESTINATION ${DEFAULT_INC_PATH} COMPONENT Development)
install(FILES ${HAL_INC} DESTINATION ${DEFAULT_INC_PATH}/hal COMPONENT Development)
install(FILES ${ATCACERT_INC} DESTINATION ${DEFAULT_INC_PATH}/atcacert COMPONENT Development)
install(FILES ${CALIB_INC} DESTINATION ${DEFAULT_INC_PATH}/calib COMPONENT Development)
install(FILES ${TALIB_INC} DESTINATION ${DEFAULT_INC_PATH}/talib COMPONENT Development)
install(FILES ${CRYPTO_INC} DESTINATION ${DEFAULT_INC_PATH}/crypto COMPONENT Development)
install(FILES ${CRYPTO_HASHES_INC} DESTINATION ${DEFAULT_INC_PATH}/crypto/hashes COMPONENT Development)
install(FILES ${HOST_INC} DESTINATION ${DEFAULT_INC_PATH}/host COMPONENT Development)
install(FILES ${JWT_INC} DESTINATION ${DEFAULT_INC_PATH}/jwt COMPONENT Development)
install(FILES ${PKCS11_INC} DESTINATION ${DEFAULT_INC_PATH}/pkcs11 COMPONENT Development)
install(FILES ${TNG_INC} DESTINATION ${DEFAULT_INC_PATH}/app/tng COMPONENT Development)
install(FILES ${SHA206_API_INC} DESTINATION ${DEFAULT_INC_PATH}/app/api_206a COMPONENT Development)
endif(DEFAULT_INC_PATH)
