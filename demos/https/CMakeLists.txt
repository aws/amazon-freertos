# AFR HTTPS demo
afr_demo_module(https)

afr_set_demo_metadata(ID "HTTPS_SYNC_DOWNLOAD_DEMO")
afr_set_demo_metadata(DESCRIPTION "Examples that demonstrate the HTTPS Client")
afr_set_demo_metadata(DISPLAY_NAME "HTTPS Client Demos")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/iot_demo_https_s3_download_sync.c"
        "${CMAKE_CURRENT_LIST_DIR}/iot_demo_https_s3_download_async.c"
        "${CMAKE_CURRENT_LIST_DIR}/iot_demo_https_s3_upload_sync.c"
        "${CMAKE_CURRENT_LIST_DIR}/iot_demo_https_s3_upload_async.c"
        "${CMAKE_CURRENT_LIST_DIR}/iot_demo_https_common.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::https
)
