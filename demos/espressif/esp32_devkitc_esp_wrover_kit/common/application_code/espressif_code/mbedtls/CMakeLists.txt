set(AMAZON_FREERTOS_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../lib")
set(AMAZON_FREERTOS_DEMOS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../demos")

set(COMPONENT_ADD_INCLUDEDIRS port/include ${AMAZON_FREERTOS_LIB_DIR}/third_party/mbedtls/include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS ${AMAZON_FREERTOS_LIB_DIR}/third_party/mbedtls/library port)

register_component()

target_compile_definitions(${COMPONENT_TARGET} PUBLIC
    -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h"
)
