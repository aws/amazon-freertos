# -------------------------------------------------------------------------------------------------
# Demos
# -------------------------------------------------------------------------------------------------
# Key provisioning
afr_demo_module(demo_key_provisioning)
afr_module_sources(
    demo_key_provisioning
    INTERFACE
        "${AFR_DEMOS_DIR}/devmode_key_provisioning/aws_dev_mode_key_provisioning.c"
        "${AFR_DEMOS_DIR}/include/aws_dev_mode_key_provisioning.h"
)
afr_module_dependencies(
    demo_key_provisioning
    INTERFACE
        AFR::pkcs11
        3rdparty::mbedtls
)

# Logging
afr_demo_module(demo_logging)
afr_module_sources(
    demo_logging
    INTERFACE
        "${AFR_DEMOS_DIR}/logging/aws_logging_task_dynamic_buffers.c"
        "${AFR_DEMOS_DIR}/include/aws_logging_task.h"
)

# MQTT hello world demo
afr_demo_module(demo_mqtt_hello_world)
afr_module_sources(
    demo_mqtt_hello_world
    INTERFACE
        "${AFR_DEMOS_DIR}/mqtt/aws_hello_world.c"
        "${AFR_DEMOS_DIR}/include/aws_hello_world.h"
)
afr_module_dependencies(
    demo_mqtt_hello_world
    INTERFACE
        AFR::mqtt
)

# MQTT publish subscribe demo
afr_demo_module(demo_mqtt_pubsub)
afr_module_sources(
    demo_mqtt_pubsub
    INTERFACE
        "${AFR_DEMOS_DIR}/mqtt/aws_subscribe_publish_loop.c"
        "${AFR_DEMOS_DIR}/include/aws_subscribe_publish_loop.h"
)
afr_module_dependencies(
    demo_mqtt_pubsub
    INTERFACE
        AFR::mqtt
)

# TCP echo demo
# TODO, tcp echo client separate tasks is not supported in NXP
afr_demo_module(demo_tcp)
afr_module_sources(
    demo_tcp
    INTERFACE
        #"${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_separate_tasks.c"
        "${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_single_task.c"
        "${AFR_DEMOS_DIR}/include/aws_tcp_echo_client_single_tasks.h"
)
afr_module_dependencies(
    demo_tcp
    INTERFACE
        AFR::secure_sockets
)

# Shadow demo
afr_demo_module(demo_shadow)
afr_module_sources(
    demo_shadow
    INTERFACE
        "${AFR_DEMOS_DIR}/shadow/aws_shadow_lightbulb_on_off.c"
        "${AFR_DEMOS_DIR}/include/aws_shadow_lightbulb_on_off.h"
)
afr_module_dependencies(
    demo_shadow
    INTERFACE
        AFR::shadow
        3rdparty::jsmn
)

# Greengrass demo
afr_demo_module(demo_greengrass)
afr_module_sources(
    demo_greengrass
    INTERFACE
        "${AFR_DEMOS_DIR}/greengrass_connectivity/aws_greengrass_discovery_demo.c"
        "${AFR_DEMOS_DIR}/include/aws_greengrass_discovery_demo.h"
)
afr_module_dependencies(
    demo_greengrass
    INTERFACE
        AFR::greengrass
)

# OTA demo
afr_demo_module(demo_ota)
afr_module_sources(
    demo_ota
    INTERFACE
        "${AFR_DEMOS_DIR}/ota/aws_ota_update_demo.c"
        "${AFR_DEMOS_DIR}/include/aws_ota_update_demo.h"
)
afr_module_dependencies(
    demo_ota
    INTERFACE
        AFR::ota
)

# Base demo target
afr_demo_module(demo_base)
afr_module_sources(
    demo_base
    INTERFACE
        "${AFR_DEMOS_DIR}/demo_runner/aws_demo_runner.c"
        "${AFR_DEMOS_DIR}/include/aws_demo.h"
        "${AFR_DEMOS_DIR}/include/aws_demo_runner.h"
        "${AFR_DEMOS_DIR}/include/aws_clientcredential.h"
        "${AFR_DEMOS_DIR}/include/aws_clientcredential_keys.h"
        "${AFR_DEMOS_DIR}/include/aws_application_version.h"
)
afr_module_include_dirs(
    demo_base
    INTERFACE
        "${AFR_DEMOS_DIR}/include"
)
afr_module_dependencies(demo_base INTERFACE AFR::kernel)
