afr_module(ota)

afr_module_sources(
    ota PRIVATE
        "${AFR_MODULES_DIR}/ota/aws_ota_agent.c"
        "${AFR_MODULES_DIR}/ota/aws_ota_cbor.c"
        "${AFR_MODULES_DIR}/include/aws_ota_agent.h"
        "${AFR_MODULES_DIR}/include/private/aws_ota_cbor.h"
        "${AFR_MODULES_DIR}/include/private/aws_ota_pal.h"
        "${AFR_MODULES_DIR}/include/private/aws_ota_types.h"
        "${AFR_MODULES_DIR}/include/private/aws_ota_agent_internal.h"
        "${AFR_MODULES_DIR}/include/private/aws_ota_cbor_internal.h"
)

afr_module_include_dirs(
    ota
    PUBLIC
        "${AFR_MODULES_DIR}/include"
        "${AFR_MODULES_DIR}/include/private"
)

afr_module_dependencies(
    ota
    PUBLIC
        AFR::demo_logging
    PRIVATE
        AFR::ota::mcu_port
        AFR::mqtt
        3rdparty::tinycbor
        3rdparty::jsmn
)

# OTA depends on only 1 file from mbedtls
afr_module_sources(
    ota PRIVATE
    "${AFR_3RDPARTY_DIR}/mbedtls/library/base64.c"
)
afr_module_include_dirs(
    ota PRIVATE
    "${AFR_3RDPARTY_DIR}/mbedtls/include"
)

# Test dependencies
if(AFR_IS_TESTING)
    afr_module_include_dirs(
        ota
        PRIVATE
            "${AFR_TESTS_DIR}/ota"
    )
endif()
