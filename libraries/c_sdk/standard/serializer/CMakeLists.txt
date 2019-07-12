afr_module(INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/cbor/iot_serializer_tinycbor_decoder.c"
        "${src_dir}/cbor/iot_serializer_tinycbor_encoder.c"
        "${src_dir}/json/iot_serializer_json_decoder.c"
        "${src_dir}/json/iot_serializer_json_encoder.c"
        "${src_dir}/iot_serializer_static_memory.c"
        "${inc_dir}/iot_serializer.h"
        "${src_dir}/iot_json_utils.c"
        "${inc_dir}/iot_json_utils.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::common
        3rdparty::tinycbor
        3rdparty::mbedtls
)

# Serializer test
afr_test_module()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_tests_serializer_cbor.c"
        "${test_dir}/iot_tests_serializer_json.c"
	"${test_dir}/iot_tests_deserializer_json.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::serializer
)
