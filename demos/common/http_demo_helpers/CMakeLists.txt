# Module for demo helper utilities based on coreHTTP library. 
afr_module(INTERNAL NAME http_demo_helpers)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/http_demo_utils.c
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
    )

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_http
        AFR::backoff_algorithm
        AFR::transport_interface_secure_sockets
        AFR::pkcs11_helpers
)
