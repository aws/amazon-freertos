afr_module(mqtt)

afr_module_sources(
    mqtt
    PRIVATE
        "${AFR_MODULES_DIR}/mqtt/aws_mqtt_agent.c"
        "${AFR_MODULES_DIR}/mqtt/aws_mqtt_lib.c"
        "${AFR_MODULES_DIR}/include/aws_mqtt_agent.h"
        "${AFR_MODULES_DIR}/include/aws_mqtt_lib.h"
        "${AFR_MODULES_DIR}/include/private/aws_mqtt_buffer.h"
        "${AFR_MODULES_DIR}/include/private/aws_mqtt_config_defaults.h"
        "${AFR_MODULES_DIR}/include/private/aws_mqtt_agent_config_defaults.h"
        "${AFR_MODULES_DIR}/include/private/aws_doubly_linked_list.h"
)

afr_module_include_dirs(
    mqtt
    PUBLIC
        "${AFR_MODULES_DIR}/include"
        "${AFR_MODULES_DIR}/include/private"
)

afr_module_dependencies(
    mqtt
    PUBLIC
        AFR::secure_sockets
    PRIVATE
        AFR::bufferpool
)
