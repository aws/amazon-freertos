register_component()

target_compile_options(
    afr_3rdparty_mbedtls
    PUBLIC
    ${IDF_COMPILE_OPTIONS}
    $<$<COMPILE_LANGUAGE:C>:${IDF_C_COMPILE_OPTIONS}>
    $<$<COMPILE_LANGUAGE:CXX>:${IDF_CXX_COMPILE_OPTIONS}>
)

target_include_directories(
    afr_3rdparty_mbedtls
    PRIVATE
    "${AFR_VENDORS_DIR}/espressif/esp-idf/components/vfs/include"
    PUBLIC
    ${IDF_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_LIST_DIR}/port/include
)

target_sources(
    afr_3rdparty_mbedtls
    PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/port/esp_bignum.c"
    "${CMAKE_CURRENT_LIST_DIR}/port/esp_mem.c"
    "${CMAKE_CURRENT_LIST_DIR}/port/esp_sha256.c"
    "${CMAKE_CURRENT_LIST_DIR}/port/esp_hardware.c"
    "${CMAKE_CURRENT_LIST_DIR}/port/esp_sha1.c"
    "${CMAKE_CURRENT_LIST_DIR}/port/esp_sha512.c"
    "${CMAKE_CURRENT_LIST_DIR}/port/mbedtls_debug.c"
)

target_compile_definitions(
    afr_3rdparty_mbedtls
    PUBLIC
    ${IDF_COMPILE_DEFINITIONS}
    -DMBEDTLS_CONFIG_FILE="${CMAKE_CURRENT_LIST_DIR}/port/include/mbedtls/esp_config.h"
)

target_link_libraries(
    ${COMPONENT_TARGET}
    INTERFACE
    afr_3rdparty_mbedtls
)
