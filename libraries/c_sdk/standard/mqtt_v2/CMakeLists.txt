afr_module()

afr_set_lib_metadata(ID "mqttv2")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT v2 protocol that enables \
communication with AWS IoT. MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "MQTTv2")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/utest")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
         "${src_dir}/mqtt.c"
         "${src_dir}/mqtt_lightweight.c"
         "${src_dir}/mqtt_state.c"
         "${src_dir}"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
)


