# Module to manage MQTT subscriptions for demos. 
afr_module(INTERNAL NAME mqtt_subscription_manager)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/mqtt_subscription_manager.c
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
    )

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_mqtt
)
