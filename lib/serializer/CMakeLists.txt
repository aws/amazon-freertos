afr_module(serializer)

afr_module_sources(
    serializer
    PRIVATE
        "${AFR_MODULES_DIR}/serializer/cbor/iot_serializer_tinycbor_decoder.c"
        "${AFR_MODULES_DIR}/serializer/cbor/iot_serializer_tinycbor_encoder.c"
        "${AFR_MODULES_DIR}/serializer/json/aws_iot_serializer_json_decoder.c"
        "${AFR_MODULES_DIR}/serializer/json/aws_iot_serializer_json_encoder.c"
)

afr_module_include_dirs(
    serializer
    PUBLIC
        "${AFR_MODULES_DIR}/include"
)

afr_module_dependencies(
    serializer
    PUBLIC
        3rdparty::tinycbor
        3rdparty::mbedtls
)
