idf_component_register()

set(FREERTOS_COMPONENT_DIR "${CMAKE_CURRENT_LIST_DIR}/../freertos")
set(MBEDTLS_DIR "${AFR_VENDORS_DIR}/espressif/esp-idf/components/mbedtls")

string(FIND "${AFR_BOARD_NAME}" "esp32s2" SOC_NAME_ESP32S2)
string(FIND "${AFR_BOARD_NAME}" "esp32" SOC_NAME_ESP32)

if (NOT(${SOC_NAME_ESP32S2} EQUAL -1))
    set(SOC_NAME "esp32s2")
elseif(NOT($SOC_NAME_ESP32) EQUAL -1)
    set(SOC_NAME "esp32")
endif()

target_compile_options(
    afr_3rdparty_mbedtls
    PUBLIC
    ${IDF_COMPILE_OPTIONS}
    $<$<COMPILE_LANGUAGE:C>:${IDF_C_COMPILE_OPTIONS}>
    $<$<COMPILE_LANGUAGE:CXX>:${IDF_CXX_COMPILE_OPTIONS}>
    "-Wno-maybe-uninitialized"
)

set(
    include_dirs
    ${IDF_INCLUDE_DIRECTORIES}
    ${MBEDTLS_DIR}/port/include
    "${AFR_VENDORS_DIR}/espressif/esp-idf/components/xtensa/include"
    "${AFR_VENDORS_DIR}/espressif/esp-idf/components/soc/soc/include"
)

if("${SOC_NAME}" STREQUAL "esp32s2")
    list(APPEND include_dirs
        "${AFR_VENDORS_DIR}/espressif/esp-idf/components/soc/src/esp32s2/include"
        "${AFR_VENDORS_DIR}/espressif/esp-idf/components/esp32s2/include"
    )
else()
    list(APPEND include_dirs
        "${AFR_VENDORS_DIR}/espressif/esp-idf/components/soc/src/esp32/include"
    )
endif()

target_include_directories(
    afr_3rdparty_mbedtls
    BEFORE
    PRIVATE
    "${AFR_VENDORS_DIR}/espressif/esp-idf/components/vfs/include"
    "${AFR_VENDORS_DIR}/espressif/esp-idf/components/soc/soc/include/soc"
    ${FREERTOS_COMPONENT_DIR}/include
    PUBLIC
    ${include_dirs}
)

set(
    mbedtls_srcs
    "${MBEDTLS_DIR}/port/esp_bignum.c"
    "${MBEDTLS_DIR}/port/esp_mem.c"
    "${MBEDTLS_DIR}/port/esp_hardware.c"
    "${MBEDTLS_DIR}/port/mbedtls_debug.c"
    "${MBEDTLS_DIR}/port/esp_aes_xts.c"
)

if("${SOC_NAME}" STREQUAL "esp32s2")
    list(APPEND mbedtls_srcs
    "${MBEDTLS_DIR}/port/esp32s2/aes.c"
    "${MBEDTLS_DIR}/port/esp32s2/sha.c"
    "${MBEDTLS_DIR}/port/esp32s2/bignum.c"
    "${MBEDTLS_DIR}/port/esp32s2/esp_sha1.c"
    "${MBEDTLS_DIR}/port/esp32s2/esp_sha512.c"
    "${MBEDTLS_DIR}/port/esp32s2/esp_sha256.c"
    "${AFR_VENDORS_DIR}/espressif/esp-idf/components/esp32s2/esp_crypto_lock.c"
    )
else()
    list(APPEND mbedtls_srcs
    "${MBEDTLS_DIR}/port/esp32/aes.c"
    "${MBEDTLS_DIR}/port/esp32/sha.c"
    "${MBEDTLS_DIR}/port/esp32/bignum.c"
    "${MBEDTLS_DIR}/port/esp32/esp_sha1.c"
    "${MBEDTLS_DIR}/port/esp32/esp_sha512.c"
    "${MBEDTLS_DIR}/port/esp32/esp_sha256.c"
    )
endif()

target_sources(
    afr_3rdparty_mbedtls
    PUBLIC
    ${mbedtls_srcs}
)

target_compile_definitions(
    afr_3rdparty_mbedtls
    PUBLIC
    ${IDF_COMPILE_DEFINITIONS}
    -DMBEDTLS_CONFIG_FILE="${MBEDTLS_DIR}/port/include/mbedtls/esp_config.h"
)

target_link_libraries(
    ${COMPONENT_TARGET}
    INTERFACE
    afr_3rdparty_mbedtls
)
