set(AMAZON_FREERTOS_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../lib")
set(AMAZON_FREERTOS_DEMOS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../demos")

set(COMPONENT_ADD_INCLUDEDIRS ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/include
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/jsmn
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/include
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/tinycbor
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/include/portable/espressif/esp32_devkitc_esp_wrover_kit
   )

set (COMPONENT_PRIV_INCLUDEDIRS ${AMAZON_FREERTOS_LIB_DIR}/third_party/pkcs11
                                ${AMAZON_FREERTOS_LIB_DIR}/ota/portable/espressif/esp32_devkitc_esp_wrover_kit
    )

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES mbedtls)

set(COMPONENT_PRIV_REQUIRES spiffs bootloader_support app_update nvs_flash)

set(COMPONENT_SRCDIRS ../..
                ${AMAZON_FREERTOS_LIB_DIR}/mqtt
                ${AMAZON_FREERTOS_LIB_DIR}/shadow
                ${AMAZON_FREERTOS_LIB_DIR}/greengrass
                ${AMAZON_FREERTOS_LIB_DIR}/utils
                ${AMAZON_FREERTOS_LIB_DIR}/bufferpool
                ${AMAZON_FREERTOS_LIB_DIR}/tls
                ${AMAZON_FREERTOS_LIB_DIR}/crypto
                ${AMAZON_FREERTOS_LIB_DIR}/wifi/portable/espressif/esp32_devkitc_esp_wrover_kit
                ${AMAZON_FREERTOS_LIB_DIR}/secure_sockets/portable/freertos_plus_tcp
                ${AMAZON_FREERTOS_LIB_DIR}/pkcs11/portable/espressif/esp32_devkitc_esp_wrover_kit
                ${AMAZON_FREERTOS_LIB_DIR}/pkcs11/mbedtls
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/BufferManagement
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/esp32
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/jsmn
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/logging
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/devmode_key_provisioning
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/demo_runner
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/shadow
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/mqtt
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/greengrass_connectivity
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/tcp
                ${AMAZON_FREERTOS_LIB_DIR}/ota
                ${AMAZON_FREERTOS_LIB_DIR}/ota/portable/espressif/esp32_devkitc_esp_wrover_kit
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/source
                ${AMAZON_FREERTOS_DEMOS_DIR}/common/ota
                ${AMAZON_FREERTOS_LIB_DIR}/third_party/tinycbor
   )

set(COMPONENT_SRCEXCLUDE ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_1.c
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c
                ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c
    )

register_component()

set_source_files_properties( ${AMAZON_FREERTOS_LIB_DIR}/greengrass/aws_greengrass_discovery.c
    ${AMAZON_FREERTOS_DEMOS_DIR}/common/tcp/aws_tcp_echo_client_single_task.c
    PROPERTIES COMPILE_FLAGS
    -Wno-format
)

set_source_files_properties( ${AMAZON_FREERTOS_DEMOS_DIR}/common/logging/aws_logging_task_dynamic_buffers.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format -Wno-uninitialized"
)
