# -------------------------------------------------------------------------------------------------
# Tests
# -------------------------------------------------------------------------------------------------
# POSIX test
afr_test_module(test_posix)
afr_module_sources(
    test_posix
    INTERFACE
        "${AFR_TESTS_DIR}/posix/aws_test_posix_clock.c"
        "${AFR_TESTS_DIR}/posix/aws_test_posix_mqueue.c"
        "${AFR_TESTS_DIR}/posix/aws_test_posix_pthread.c"
        "${AFR_TESTS_DIR}/posix/aws_test_posix_semaphore.c"
        "${AFR_TESTS_DIR}/posix/aws_test_posix_stress.c"
        "${AFR_TESTS_DIR}/posix/aws_test_posix_timer.c"
        "${AFR_TESTS_DIR}/posix/aws_test_posix_utils.c"
)
afr_module_dependencies(
    test_posix
    INTERFACE AFR::posix
)

# C SDK platform layer test
afr_test_module(test_platform)
afr_module_sources(
    test_platform
    INTERFACE
        "${AFR_TESTS_DIR}/utils/platform/iot_test_platform_clock.c"
        "${AFR_TESTS_DIR}/utils/platform/iot_test_platform_threads.c"
)
afr_module_dependencies(
    test_platform
    INTERFACE
        AFR::sdk_platform
)

# Crypto test
afr_test_module(test_crypto)
afr_module_sources(
    test_crypto
    INTERFACE
        "${AFR_TESTS_DIR}/crypto/aws_test_crypto.c"
)
afr_module_dependencies(
    test_crypto
    INTERFACE AFR::crypto
)

# Greengrass test
afr_test_module(test_greengrass)
afr_module_sources(
    test_greengrass
    INTERFACE
        "${AFR_TESTS_DIR}/greengrass/aws_test_greengrass_discovery.c"
        "${AFR_TESTS_DIR}/greengrass/aws_test_helper_secure_connect.c"
)
afr_module_dependencies(
    test_greengrass
    INTERFACE
        AFR::greengrass
        3rdparty::jsmn
)

# MQTT test
afr_test_module(test_mqtt)

# Enable tests for BLE-over-MQTT serializers on supported boards.
if(${BLE_SUPPORTED})
    set(extra_test_mqtt_sources "${AFR_TESTS_DIR}/mqtt/unit/aws_iot_tests_mqtt_serialize_ble.c")
endif()

afr_module_sources(
    test_mqtt
    INTERFACE
        "${AFR_TESTS_DIR}/mqtt/aws_test_mqtt_agent.c"
        "${AFR_TESTS_DIR}/mqtt/system/iot_tests_mqtt_system.c"
        "${AFR_TESTS_DIR}/mqtt/unit/iot_tests_mqtt_api.c"
        "${AFR_TESTS_DIR}/mqtt/unit/iot_tests_mqtt_receive.c"
        "${AFR_TESTS_DIR}/mqtt/unit/iot_tests_mqtt_subscription.c"
        "${AFR_TESTS_DIR}/mqtt/unit/iot_tests_mqtt_validate.c"
        ${extra_test_mqtt_sources}
)
afr_module_include_dirs(
    test_mqtt
    INTERFACE
        "${AFR_TESTS_DIR}/mqtt/access"
)
afr_module_dependencies(
    test_mqtt
    INTERFACE AFR::mqtt
)

# OTA test
afr_test_module(test_ota)
afr_module_sources(
    test_ota
    INTERFACE
        "${AFR_TESTS_DIR}/ota/aws_test_ota_agent.c"
        "${AFR_TESTS_DIR}/ota/aws_test_ota_pal.c"

        # The OTA end-to-end tests use code from the demo.
        "${AFR_DEMOS_DIR}/ota/aws_ota_update_demo.c"
)
afr_module_include_dirs(
    test_ota
    INTERFACE
        "${AFR_TESTS_DIR}/ota"

        # The OTA end-to-end tests use code from the demo.
        "${AFR_DEMOS_DIR}/include"
)
afr_module_dependencies(
    test_ota
    INTERFACE AFR::ota
)

# PKCS11 test
afr_test_module(test_pkcs11)
afr_module_sources(
    test_pkcs11
    INTERFACE
        "${AFR_TESTS_DIR}/pkcs11/aws_test_pkcs11.c"
)
afr_module_dependencies(
    test_pkcs11
    INTERFACE
        AFR::pkcs11
        AFR::crypto
)

# Secure sockets test
afr_test_module(test_secure_sockets)
afr_module_sources(
    test_secure_sockets
    INTERFACE
        "${AFR_TESTS_DIR}/secure_sockets/aws_test_tcp.c"
        "${AFR_TESTS_DIR}/include/aws_test_tcp.h"
)
afr_module_dependencies(
    test_secure_sockets
    INTERFACE AFR::secure_sockets
)

# TLS test
afr_test_module(test_tls)
afr_module_sources(
    test_tls
    INTERFACE
        "${AFR_TESTS_DIR}/tls/aws_test_tls.c"
)
afr_module_dependencies(
    test_tls
    INTERFACE
        AFR::secure_sockets
)

# Shadow test
afr_test_module(test_shadow)
afr_module_sources(
    test_shadow
    INTERFACE
        "${AFR_TESTS_DIR}/shadow/aws_test_shadow.c"
        "${AFR_TESTS_DIR}/shadow/system/aws_iot_tests_shadow_system.c"
        "${AFR_TESTS_DIR}/shadow/unit/aws_iot_tests_shadow_api.c"
        "${AFR_TESTS_DIR}/shadow/unit/aws_iot_tests_shadow_parser.c"
)
afr_module_include_dirs(
    test_shadow
    INTERFACE
        "${AFR_TESTS_DIR}/mqtt/access"
)
afr_module_dependencies(
    test_shadow
    INTERFACE
        AFR::shadow
)

# WiFi test
afr_test_module(test_wifi)
afr_module_sources(
    test_wifi
    INTERFACE
        "${AFR_TESTS_DIR}/wifi/aws_test_wifi.c"
)
afr_module_dependencies(
    test_wifi
    INTERFACE
        AFR::wifi
)

# BLE test
# Only build on boards that support BLE.
if(${BLE_SUPPORTED})
    afr_test_module(test_ble)
    afr_module_sources(
        test_ble
        INTERFACE
            "${AFR_TESTS_DIR}/ble/aws_test_ble.c"
            "${AFR_TESTS_DIR}/ble/aws_test_ble_end_to_end.c"
    )
endif()

# Memory test
afr_test_module(test_memory)
afr_module_sources(
    test_memory
    INTERFACE
        "${AFR_TESTS_DIR}/memory_leak/aws_memory_leak.c"
)

# Base tests target
afr_test_module(test_base)
afr_module_sources(
    test_base
    INTERFACE
        "${AFR_TESTS_DIR}/framework/aws_test_framework.c"
        "${AFR_TESTS_DIR}/test_runner/aws_test_runner.c"
        "${AFR_TESTS_DIR}/include/aws_test_framework.h"
        "${AFR_TESTS_DIR}/include/aws_test_runner.h"
        "${AFR_TESTS_DIR}/include/aws_clientcredential.h"
        "${AFR_TESTS_DIR}/include/aws_clientcredential_keys.h"
)
afr_module_include_dirs(
    test_base
    INTERFACE "${AFR_TESTS_DIR}/include"
)
afr_module_dependencies(
    test_base
    INTERFACE
        AFR::kernel
        AFR::provisioning
        AFR::sdk_common
        3rdparty::unity
)
