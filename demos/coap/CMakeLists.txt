# C SDK COAP demo
afr_demo_module(coap)

afr_set_demo_metadata(ID "COAP_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates COAP")
afr_set_demo_metadata(DISPLAY_NAME "COAP Hello World")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/iot_demo_coap.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::coap
)



