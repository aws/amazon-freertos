set(AMAZON_FREERTOS_ABSTRACTIONS_DIR "${AFR_MODULES_ABSTRACTIONS_DIR}")
set(AMAZON_FREERTOS_3RDPARTY_DIR "${AFR_3RDPARTY_DIR}")

set(
    COMPONENT_ADD_INCLUDEDIRS
    port/include
    ${AMAZON_FREERTOS_ABSTRACTIONS_DIR}/pkcs11/mbedtls
    ${AMAZON_FREERTOS_3RDPARTY_DIR}/mbedtls/include
)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS ${AMAZON_FREERTOS_3RDPARTY_DIR}/mbedtls/library port)

register_component()

target_compile_definitions(${COMPONENT_TARGET} PUBLIC
    -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h"
)
