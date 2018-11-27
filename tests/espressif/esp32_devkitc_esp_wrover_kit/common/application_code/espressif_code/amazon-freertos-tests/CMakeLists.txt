set(AMAZON_FREERTOS_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../lib")
set(AMAZON_FREERTOS_DEMOS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../demos")
set(AMAZON_FREERTOS_TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../tests")

set(COMPONENT_ADD_INCLUDEDIRS ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/include
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/jsmn
                ${AMAZON_FREERTOS_TESTS_DIR}/common/include
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/unity/src
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/include/portable/espressif/esp32_devkitc_esp_wrover_kit
                ${AMAZON_FREERTOS_TESTS_DIR}/common/ota
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/tinycbor
   )

set(COMPONENT_PRIV_INCLUDEDIRS ${AMAZON_FREERTOS_LIB_DIR}/third_party/pkcs11
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/unity/extras/fixture/src
                ${AMAZON_FREERTOS_LIB_DIR}/ota/portable/espressif/esp32_devkitc_esp_wrover_kit
    ) 

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES mbedtls)

set(COMPONENT_PRIV_REQUIRES spiffs bootloader_support app_update newlib pthread nvs_flash)

set(COMPONENT_SRCDIRS ../..
                ${AMAZON_FREERTOS_LIB_DIR}/mqtt
                ${AMAZON_FREERTOS_LIB_DIR}/shadow
                ${AMAZON_FREERTOS_LIB_DIR}/greengrass
                ${AMAZON_FREERTOS_LIB_DIR}/utils
                ${AMAZON_FREERTOS_LIB_DIR}/bufferpool
                ${AMAZON_FREERTOS_LIB_DIR}/tls
                ${AMAZON_FREERTOS_LIB_DIR}/ota
                ${AMAZON_FREERTOS_LIB_DIR}/ota/portable/espressif/esp32_devkitc_esp_wrover_kit
                ${AMAZON_FREERTOS_LIB_DIR}/crypto
                ${AMAZON_FREERTOS_LIB_DIR}/wifi/portable/espressif/esp32_devkitc_esp_wrover_kit
                ${AMAZON_FREERTOS_LIB_DIR}/secure_sockets/portable/freertos_plus_tcp
                ${AMAZON_FREERTOS_LIB_DIR}/pkcs11/portable/espressif/esp32_devkitc_esp_wrover_kit
                ${AMAZON_FREERTOS_LIB_DIR}/pkcs11/mbedtls
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/logging
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/devmode_key_provisioning
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/BufferManagement
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/esp32
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/jsmn
                ${AMAZON_FREERTOS_TESTS_DIR}/common/test_runner
                ${AMAZON_FREERTOS_TESTS_DIR}/common/framework
                ${AMAZON_FREERTOS_TESTS_DIR}/common/memory_leak
                ${AMAZON_FREERTOS_TESTS_DIR}/common/secure_sockets
                ${AMAZON_FREERTOS_TESTS_DIR}/common/mqtt
                ${AMAZON_FREERTOS_TESTS_DIR}/common/shadow
                ${AMAZON_FREERTOS_TESTS_DIR}/common/greengrass
                ${AMAZON_FREERTOS_TESTS_DIR}/common/crypto
                ${AMAZON_FREERTOS_TESTS_DIR}/common/pkcs11
                ${AMAZON_FREERTOS_TESTS_DIR}/common/tls
                ${AMAZON_FREERTOS_TESTS_DIR}/common/wifi
                ${AMAZON_FREERTOS_TESTS_DIR}/common/ota
                ${AMAZON_FREERTOS_TESTS_DIR}/common/posix
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/tinycbor

                ${AMAZON_FREERTOS_LIB_DIR}/third_party/unity/src
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/unity/extras/fixture/src
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/source
     )

set(COMPONENT_SRCEXCLUDE ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_1.c
        ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c
        ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c
        ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c
        ${AMAZON_FREERTOS_TESTS_DIR}/common/ota/aws_test_ota_cbor.c
        )

register_component()

component_compile_definitions(AMAZON_FREERTOS_ENABLE_UNIT_TESTS UNITY_INCLUDE_CONFIG_H)

set_source_files_properties( ${AMAZON_FREERTOS_TESTS_DIR}/common/secure_sockets/aws_test_tcp.c
 ${AMAZON_FREERTOS_TESTS_DIR}/common/wifi/aws_test_wifi.c
    PROPERTIES COMPILE_FLAGS
    -Wno-format
)

set_source_files_properties( ${AMAZON_FREERTOS_DEMOS_DIR}/common/logging/aws_logging_task_dynamic_buffers.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format -Wno-uninitialized"
)

set_source_files_properties( ${AMAZON_FREERTOS_TESTS_DIR}/common/ota/aws_test_ota_pal.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign -Wno-sizeof-pointer-memaccess"
)

set_source_files_properties( ${AMAZON_FREERTOS_TESTS_DIR}/common/ota/aws_test_ota_agent.c
    PROPERTIES COMPILE_FLAGS
    -Wno-pointer-sign
)
