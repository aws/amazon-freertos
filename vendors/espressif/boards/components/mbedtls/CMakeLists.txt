idf_component_register()

set(FREERTOS_COMPONENT_DIR "${CMAKE_CURRENT_LIST_DIR}/../freertos")
set(MBEDTLS_DIR "${AFR_VENDORS_DIR}/espressif/esp-idf/components/mbedtls")

set(ESP_HW_SUPPORT_DIR "${AFR_VENDORS_DIR}/espressif/esp-idf/components/esp_hw_support")
set(ESP_PM_DIR "${AFR_VENDORS_DIR}/espressif/esp-idf/components/esp_pm")

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
)

if("${SOC_NAME}" STRGREATER "esp32")
    list(APPEND include_dirs
        "${ESP_HW_SUPPORT_DIR}/include/soc/${SOC_NAME}"
        "${ESP_PM_DIR}/include"
        "${ESP_PM_DIR}/include/${SOC_NAME}"
    )
endif()

if("${SOC_NAME}" STREQUAL "esp32")
    set(AES_PERIPHERAL_TYPE "block")
    set(SHA_PERIPHERAL_TYPE "parallel_engine")
else()
    set(AES_PERIPHERAL_TYPE "dma")
    set(SHA_PERIPHERAL_TYPE "dma")
endif()

if(AES_PERIPHERAL_TYPE STREQUAL "dma")
    list(APPEND include_dirs
        "${MBEDTLS_DIR}/port/aes/dma/include"
    )
endif()

if(SHA_PERIPHERAL_TYPE STREQUAL "dma")
    list(APPEND include_dirs
        "${MBEDTLS_DIR}/port/sha/dma/include"
    )
endif()

target_include_directories(
    afr_3rdparty_mbedtls
    BEFORE
    PRIVATE
    "${AFR_VENDORS_DIR}/espressif/esp-idf/components/vfs/include"
    "${AFR_VENDORS_DIR}/espressif/esp-idf/components/soc/${SOC_NAME}/include"
    ${FREERTOS_COMPONENT_DIR}/include
    PUBLIC
    ${include_dirs}
)

set(
    mbedtls_srcs
    "${MBEDTLS_DIR}/port/esp_hardware.c"
    "${MBEDTLS_DIR}/port/esp_mem.c"
    "${MBEDTLS_DIR}/port/esp_timing.c"

    "${MBEDTLS_DIR}/port/aes/esp_aes_xts.c"
    "${MBEDTLS_DIR}/port/aes/esp_aes_common.c"
    "${MBEDTLS_DIR}/port/aes/${AES_PERIPHERAL_TYPE}/esp_aes.c"

    "${MBEDTLS_DIR}/port/sha/esp_sha.c"
    "${MBEDTLS_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/sha.c"
    "${MBEDTLS_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha1.c"
    "${MBEDTLS_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha512.c"
    "${MBEDTLS_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha256.c"

    "${MBEDTLS_DIR}/port/mbedtls_debug.c"
    "${esp_idf_dir}/components/mbedtls/port/net_sockets.c"
)

if(AES_PERIPHERAL_TYPE STREQUAL "dma")

    if("${SOC_NAME}" STREQUAL "esp32s2")
        set(AES_DMA_SRCS "${MBEDTLS_DIR}/port/aes/dma/esp_aes_crypto_dma_impl.c")
    else()
        set(AES_DMA_SRCS "${MBEDTLS_DIR}/port/aes/dma/esp_aes_gdma_impl.c")
    endif()

    list(APPEND mbedtls_srcs "${AES_DMA_SRCS}")
endif()

if(SHA_PERIPHERAL_TYPE STREQUAL "dma")

    if("${SOC_NAME}" STREQUAL "esp32s2")
        set(SHA_DMA_SRCS "${MBEDTLS_DIR}/port/sha/dma/esp_sha_crypto_dma_impl.c")
    else()
        set(SHA_DMA_SRCS "${MBEDTLS_DIR}/port/sha/dma/esp_sha_gdma_impl.c"
                         "${MBEDTLS_DIR}/port/crypto_shared_gdma/esp_crypto_shared_gdma.c")
    endif()

    list(APPEND mbedtls_srcs "${SHA_DMA_SRCS}")
endif()

if("${SOC_NAME}" STRGREATER "esp32")
    list(APPEND mbedtls_srcs
        "${ESP_HW_SUPPORT_DIR}/port/${SOC_NAME}/esp_crypto_lock.c"
    )
endif()

if(CONFIG_MBEDTLS_HARDWARE_MPI)
    list(APPEND mbedtls_srcs
        "${MBEDTLS_DIR}/port/esp_bignum.c"
        "${MBEDTLS_DIR}/port/${SOC_NAME}/bignum.c"
    )
endif()

if(CONFIG_MBEDTLS_DYNAMIC_BUFFER)
    list(APPEND mbedtls_srcs
        "${MBEDTLS_DIR}/port/dynamic/esp_mbedtls_dynamic_impl.c"
        "${MBEDTLS_DIR}/port/dynamic/esp_ssl_cli.c"
        "${MBEDTLS_DIR}/port/dynamic/esp_ssl_srv.c"
        "${MBEDTLS_DIR}/port/dynamic/esp_ssl_tls.c"
    )

    set(WRAP_FUNCTIONS
        mbedtls_ssl_handshake_client_step
        mbedtls_ssl_handshake_server_step
        mbedtls_ssl_read
        mbedtls_ssl_write
        mbedtls_ssl_session_reset
        mbedtls_ssl_free
        mbedtls_ssl_setup
        mbedtls_ssl_send_alert_message
        mbedtls_ssl_close_notify)

    foreach(wrap ${WRAP_FUNCTIONS})
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=${wrap}")
    endforeach()
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
