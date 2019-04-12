afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/cbor/iot_serializer_tinycbor_decoder.c"
        "${src_dir}/cbor/iot_serializer_tinycbor_encoder.c"
        "${src_dir}/json/aws_iot_serializer_json_decoder.c"
        "${src_dir}/json/aws_iot_serializer_json_encoder.c"
        "${inc_dir}/iot_serializer.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        3rdparty::tinycbor
        3rdparty::mbedtls
)
