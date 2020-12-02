# FreeRTOS coreHTTP demos
afr_demo_module(core_http)

# Set the default demo that is enabled in the downloaded package.
afr_set_demo_metadata(ID "CORE_HTTP_MUTUAL_AUTH_DEMO")
afr_set_demo_metadata(DESCRIPTION "Examples that demonstrate the HTTP library")
afr_set_demo_metadata(DISPLAY_NAME "coreHTTP Demos")

# Add the CMakeLists.txt file of module to metadata list.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/http_demo_mutual_auth.c"
        "${CMAKE_CURRENT_LIST_DIR}/http_demo_s3_download.c"
        "${CMAKE_CURRENT_LIST_DIR}/http_demo_s3_download_multithreaded.c"
        "${CMAKE_CURRENT_LIST_DIR}/http_demo_s3_upload.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        ${AFR_DEMOS_DIR}/common/http_demo_helpers
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::http_demo_helpers
        AFR::core_http
        AFR::backoff_algorithm
        AFR::transport_interface_secure_sockets
        AFR::secure_sockets
        # Add dependency on the core_http_demo_dependencies metadata module 
        # so that FreeRTOS console shows this demo ONLY when the core HTTP library
        # is selected on the console.
        AFR::core_http_demo_dependencies
)
