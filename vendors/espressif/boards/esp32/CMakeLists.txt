
# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
if("${AFR_BOARD_NAME}" STREQUAL "esp32_devkitc")
    include("${CMAKE_CURRENT_LIST_DIR}/esp32_devkitc.cmake")
    set(ECC608_IN_USE FALSE)
endif()
if("${AFR_BOARD_NAME}" STREQUAL "esp32_wrover_kit")
    include("${CMAKE_CURRENT_LIST_DIR}/esp32_wrover_kit.cmake")
    set(ECC608_IN_USE FALSE)
endif()
if("${AFR_BOARD_NAME}" STREQUAL "esp32_plus_ecc608a_devkitc")
    include("${CMAKE_CURRENT_LIST_DIR}/esp32_plus_ecc608a_devkitc.cmake")
    set(ECC608_IN_USE TRUE)
endif()

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
# Mark the python dependencies as checked so that esp-idf does not check them since this is not
# needed when we are only generating metadata and not building the project.
if(AFR_METADATA_MODE)
    set(PYTHON_DEPS_CHECKED 1)
endif()

if(DEFINED ENV{IDF_PATH})
    message("WARNING: IDF_PATH environment variable is not cleared.
    If CMake is generating an error, consider clearing the IDF_PATH environment
    variable, and generating a clean build. This message can be ignored if
    CMake was successful.")
endif()



set(esp_idf_dir "${AFR_VENDORS_DIR}/espressif/esp-idf")
# Provides idf_import_components and idf_link_components
include(${esp_idf_dir}/tools/cmake/idf.cmake)
include(${esp_idf_dir}/tools/cmake/project_utils.cmake)


if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_mcu_port(compiler)

set(linker_flags "-Wl,--gc-sections" "-Wl,--cref" "-Wl,--Map=${exe_target}.map" "-Wl,--undefined=uxTopUsedPriority")

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE ${linker_flags}
)


# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/../ports")
set(extra_components_dir "${CMAKE_CURRENT_LIST_DIR}/../components")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TESTS_DIR}/include")
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
endif()

# mbedTLS include directories
if(NOT AFR_FREERTOS_TCP)
    target_include_directories(
        afr_3rdparty_mbedtls
        PUBLIC
        "${esp_idf_dir}/components/lwip/include/apps"
        "${esp_idf_dir}/components/lwip/include/apps/sntp"
        "${esp_idf_dir}/components/lwip/lwip/src/include"
        "${esp_idf_dir}/components/lwip/port/esp32/include"
        "${esp_idf_dir}/components/lwip/port/esp32/include/arch"
        "${esp_idf_dir}/components/lwip/lwip/src/include/compat"
        )
endif()

# Kernel
afr_mcu_port(kernel)
afr_glob_src(driver_src DIRECTORY "${esp_idf_dir}" RECURSE)

set(
    kernel_inc_dirs
    "${esp_idf_dir}/components/app_update/include/"
    "${esp_idf_dir}/components/bootloader_support/include"
    "${esp_idf_dir}/components/esp32/include"
    "${esp_idf_dir}/components/esp_event/include"
    "${esp_idf_dir}/components/efuse/include"
    "${esp_idf_dir}/components/driver/include"
    "${esp_idf_dir}/components/heap/include"
    "${esp_idf_dir}/components/log/include"
    "${esp_idf_dir}/components/nvs_flash/include"
    "${esp_idf_dir}/components/pthread/include"
    "${esp_idf_dir}/components/newlib/platform_include"
    "${esp_idf_dir}/components/soc/src/esp32/include"
    "${esp_idf_dir}/components/soc/soc/esp32/include"
    "${esp_idf_dir}/components/soc/include"
    "${esp_idf_dir}/components/spi_flash/include"
    "${esp_idf_dir}/components/vfs/include"
    "${esp_idf_dir}/components/esp_ringbuf/include/"
    "${extra_components_dir}/freertos/include"
    "${esp_idf_dir}/components/mbedtls/port/include"
    "${esp_idf_dir}/components/xtensa/esp32/include/xtensa"
    "${esp_idf_dir}/components/xtensa/esp32/include"
    "${esp_idf_dir}/components/xtensa/include"
    "${esp_idf_dir}/components/esp_rom/include"
    "${esp_idf_dir}/components/esp_timer/include"
    "${esp_idf_dir}/components/esp_common/include"
    "${esp_idf_dir}/components/esp_system/include"
)

if(AFR_ESP_FREERTOS_TCP)
    list(APPEND kernel_inc_dirs
    "${extra_components_dir}/freertos_tcpip/ethernet/include"
    "${extra_components_dir}/freertos_tcpip/smartconfig_ack/include"
    "${extra_components_dir}/freertos_tcpip/tcpip_adapter/include"
    "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/Compiler/GCC"
    "${esp_idf_dir}/components/xtensa"
    "${esp_idf_dir}/components/esp_wifi/include"
    "${esp_idf_dir}/components/esp_netif/include"
    "${esp_idf_dir}/components/esp_eth/include"
    )
else()
    list(APPEND kernel_inc_dirs
    "${esp_idf_dir}/components/tcpip_adapter/include"
    )
endif()

if(ECC608_IN_USE)
    set(mchp_dir "${AFR_VENDORS_DIR}/microchip")
    set(ecc608a_dir "${mchp_dir}/secure_elements")
    list(APPEND kernel_inc_dirs
        "${esp_idf_dir}/components/esp_ringbuf/include"
        "${board_dir}/config_files/ecc608a_pkcs11_config"
    )
else()
    list(APPEND kernel_inc_dirs
        "${board_dir}/config_files/default_pkcs11_config"
    )
endif()

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        ${kernel_inc_dirs}
        "${AFR_KERNEL_DIR}/portable/ThirdParty/GCC/Xtensa_ESP32/include/"
        "${aws_credentials_include}"
        "${board_dir}/config_files"
        "$<$<NOT:${AFR_METADATA_MODE}>:${CMAKE_BINARY_DIR}/config>"
)

# WiFi
afr_mcu_port(wifi)

if(AFR_ESP_FREERTOS_TCP)
target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
    AFR::freertos_plus_tcp
)
else()
target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
        "${esp_idf_dir}/components/lwip/include/apps"
        "${esp_idf_dir}/components/lwip/include/apps/sntp"
        "${esp_idf_dir}/components/lwip/lwip/src/include"
        "${esp_idf_dir}/components/lwip/port/esp32/include"
        "${esp_idf_dir}/components/lwip/port/esp32/include/arch"
        "${esp_idf_dir}/components/protocomm/include/common"
        "${esp_idf_dir}/components/protocomm/include/security"
        "${esp_idf_dir}/components/protocomm/include/transports"
        "${esp_idf_dir}/components/wifi_provisioning/include"
        "${esp_idf_dir}/components/esp_wifi/include"
        "${esp_idf_dir}/components/esp_netif/include"
        "${esp_idf_dir}/components/esp_eth/include"
        "${esp_idf_dir}/components/soc/soc/include"
)
endif()

target_sources(
    AFR::wifi::mcu_port
    INTERFACE 
        "${afr_ports_dir}/wifi/iot_wifi.c"
)

# SoftAP Provisioning - Available ONLY when using LWIP stack
if( NOT AFR_ESP_FREERTOS_TCP )
    target_include_directories(
        AFR::wifi::mcu_port
        INTERFACE
            "${AFR_ROOT_DIR}/libraries/coreHTTP/source/dependency/3rdparty/http_parser"
            "${AFR_ROOT_DIR}/libraries/3rdparty/mbedtls/include"
            "${esp_idf_dir}/components/protobuf-c/protobuf-c"
            "${esp_idf_dir}/components/protocomm/proto-c"
            "${esp_idf_dir}/components/protocomm/src/common"
            "${esp_idf_dir}/components/wifi_provisioning/proto-c"
            "${esp_idf_dir}/components/esp_http_server/include"
    )
    target_sources(
        AFR::wifi::mcu_port
        INTERFACE
            "${afr_ports_dir}/wifi/iot_softap_wifi_provisioning.c"
            "${AFR_ROOT_DIR}/libraries/coreHTTP/source/dependency/3rdparty/http_parser/http_parser.c"
            "${AFR_ROOT_DIR}/libraries/coreHTTP/source/dependency/3rdparty/http_parser/http_parser.h"
            "${esp_idf_dir}/components/protobuf-c/protobuf-c/protobuf-c/protobuf-c.c"
            "${esp_idf_dir}/components/protocomm/proto-c/session.pb-c.c"
            "${esp_idf_dir}/components/protocomm/proto-c/sec0.pb-c.c"
            "${esp_idf_dir}/components/protocomm/proto-c/sec1.pb-c.c"
            "${esp_idf_dir}/components/protocomm/proto-c/constants.pb-c.c"
            "${esp_idf_dir}/components/protocomm/src/common/protocomm.c"
            "${esp_idf_dir}/components/protocomm/src/security/security1.c"
            "${esp_idf_dir}/components/protocomm/src/transports/protocomm_httpd.c"
            "${esp_idf_dir}/components/wifi_provisioning/proto-c/wifi_config.pb-c.c"
            "${esp_idf_dir}/components/wifi_provisioning/proto-c/wifi_constants.pb-c.c"
            "${esp_idf_dir}/components/wifi_provisioning/src/wifi_config.c"
    )
    target_link_libraries(
        AFR::wifi::mcu_port
        INTERFACE
            3rdparty::mbedtls
    )
endif()

# BLE
set(BLE_SUPPORTED 1 CACHE INTERNAL "BLE is supported on this platform.")

afr_mcu_port(ble_hal)

# Include Bluedroid HAL files as header files.
afr_glob_src( bluedroid_src DIRECTORY ${afr_ports_dir}/ble/bluedroid  RECURSE )
set_source_files_properties( ${bluedroid_src} PROPERTIES HEADER_FILE_ONLY TRUE )

# Include Nimble HAL files as header files.
afr_glob_src( nimble_src DIRECTORY ${afr_ports_dir}/ble/nimble  RECURSE )
set_source_files_properties( ${nimble_src} PROPERTIES HEADER_FILE_ONLY TRUE )

target_sources(
    AFR::ble_hal::mcu_port
    INTERFACE
        "${afr_ports_dir}/ble/iot_ble_hal_common_gap.c"
        "${afr_ports_dir}/ble/iot_ble_hal_gap.c"
        "${afr_ports_dir}/ble/iot_ble_hal_gatt_server.c"
        ${bluedroid_src}
        ${nimble_src}
)

target_include_directories(
    AFR::ble_hal::mcu_port
    INTERFACE
        "${esp_idf_dir}/components/bt/host/bluedroid/api/include/api"
        "${esp_idf_dir}/components/bt/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/porting/nimble/include"
        "${esp_idf_dir}/components/bt/host/nimble/port/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/services/ans/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/services/bas/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/services/gap/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/services/gatt/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/services/ias/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/services/lls/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/services/tps/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/util/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/store/ram/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/nimble/host/store/config/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/porting/npl/freertos/include"
        "${esp_idf_dir}/components/bt/host/nimble/nimble/ext/tinycrypt/include"
        "${esp_idf_dir}/components/bt/host/nimble/esp-hci/include"

        "${afr_ports_dir}/ble"
        "${afr_ports_dir}/ble/bluedroid"
        "${afr_ports_dir}/ble/nimble"
)

if (NOT AFR_METADATA_MODE)
    target_link_libraries(
        AFR::ble_hal::mcu_port
        INTERFACE
            idf::bt
    )
endif()

# PKCS11
if(ECC608_IN_USE)
    afr_mcu_port(pkcs11_implementation DEPENDS AFR::pkcs11_ecc608a)
    set(
        ecc608a_hal_src
            "${ecc608a_dir}/lib/hal/atca_hal.c"
            "${ecc608a_dir}/lib/hal/hal_freertos.c"
            "${ecc608a_dir}/lib/hal/hal_esp32_i2c.c"
            "${ecc608a_dir}/lib/hal/hal_esp32_timer.c"
            "${ecc608a_dir}/lib/hal/atca_hal.h"
            "${ecc608a_dir}/lib/hal/atca_start_config.h"
            "${ecc608a_dir}/lib/hal/atca_start_iface.h"
    )
    target_include_directories(
        AFR::pkcs11_implementation::mcu_port
        INTERFACE
            "${board_dir}/config_files/ecc608a_pkcs11_config"
    )
    target_sources(
        AFR::pkcs11_implementation::mcu_port
        INTERFACE
            "${ecc608a_hal_src}"
    )
else()
    afr_mcu_port(pkcs11_implementation DEPENDS AFR::pkcs11_mbedtls)
    target_include_directories(
        AFR::pkcs11_implementation::mcu_port
        INTERFACE
            "${board_dir}/config_files/default_pkcs11_config/"
    )
    target_sources(
        AFR::pkcs11_implementation::mcu_port
        INTERFACE
            "${afr_ports_dir}/pkcs11/core_pkcs11_pal.c"
    )
endif()

target_include_directories(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${esp_idf_dir}/components/soc/soc/include"
)

if(AFR_ESP_FREERTOS_TCP)
# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/BufferManagement/BufferAllocation_2.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/NetworkInterface/esp32/NetworkInterface.c"
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE AFR::secure_sockets_freertos_plus_tcp
)
else()

# Secure sockets
afr_mcu_port(secure_sockets)

target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE
        "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/lwip/iot_secure_sockets.c"
)

target_include_directories(
    AFR::secure_sockets::mcu_port
    INTERFACE
        "${esp_idf_dir}/components/lwip/include/apps"
        "${esp_idf_dir}/components/lwip/include/apps/sntp"
        "${esp_idf_dir}/components/lwip/lwip/src/include"
        "${esp_idf_dir}/components/lwip/port/esp32/include"
        "${esp_idf_dir}/components/lwip/port/esp32/include/arch"
        "${esp_idf_dir}/components/lwip/include"
)

target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE
        AFR::tls
        AFR::wifi
)

endif()

# Common I/O
afr_mcu_port(common_io)
target_sources(
    AFR::common_io::mcu_port
    INTERFACE
        "${afr_ports_dir}/common_io/iot_i2c.c"
        "${afr_ports_dir}/common_io/iot_spi.c"
        "${afr_ports_dir}/common_io/iot_uart.c"
        $<${AFR_IS_TESTING}:${afr_ports_dir}/common_io/iot_test_common_io_internal.c>
        # Add the header file to generate metadata for it so that
        # it is present in the code downloaded from FreeRTOS console.
        "${afr_ports_dir}/common_io/include/iot_board_gpio.h"
)
target_include_directories(
    AFR::common_io::mcu_port
    INTERFACE
        "${afr_ports_dir}/common_io/include"
        "${AFR_ROOT_DIR}/libraries/abstractions/common_io/test"
        "${esp_idf_dir}/components/soc/soc/include"
)


target_compile_options(
    AFR::common_io::mcu_port
    INTERFACE
    -Wno-implicit-function-declaration
    -Wno-format -Wno-maybe-uninitialized
    -Wno-pointer-sign
    -Wno-unused-but-set-variable
    -Wno-incompatible-pointer-types
)

if(AFR_IS_TESTING)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
    -DESP32
)
endif()

# OTA
# Need to get this validated
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE
        "${afr_ports_dir}/ota/aws_ota_pal.c"
        "${afr_ports_dir}/ota/aws_esp_ota_ops.c"
        "${afr_ports_dir}/ota/aws_esp_ota_ops.h"
)
target_include_directories(
    AFR::ota::mcu_port
    INTERFACE
        "${esp_idf_dir}/components/soc/soc/include"
)
target_link_libraries(
    AFR::ota::mcu_port
    INTERFACE
        AFR::crypto
        AFR::pkcs11
        AFR::ota_mqtt
        AFR::ota_http
)

# Add extra components to metadata
afr_files_to_console_metadata(
    "${extra_components_dir}"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

# If external project is set do not link IDF components to aws target
if (NOT IDF_PROJECT_EXECUTABLE)
    set(IDF_PROJECT_EXECUTABLE ${exe_target})
endif()

if (NOT IDF_EXECUTABLE_SRCS)
    set(IDF_EXECUTABLE_SRCS "${board_dir}/application_code/main.c" ${extra_exe_sources})
endif()

add_executable(
    ${IDF_PROJECT_EXECUTABLE}
    ${IDF_EXECUTABLE_SRCS}
)

target_include_directories(
    ${IDF_PROJECT_EXECUTABLE}
    PUBLIC
        "${esp_idf_dir}/components/esp_ringbuf/include"
        $<TARGET_PROPERTY:AFR::kernel,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:AFR::ble_hal::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:AFR::wifi::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(
    ${IDF_PROJECT_EXECUTABLE}
    PRIVATE
        AFR::wifi
        AFR::utils
        AFR::ble
        AFR::common_io
)

if(AFR_METADATA_MODE)
    return()
endif()


# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------

# TODO, workaround for permission issue in FreeRTOS console.
if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    execute_process(COMMAND chmod +x "${esp_idf_dir}/components/esptool_py/esptool/esptool.py")
endif()

if(ECC608_IN_USE)
set_source_files_properties(
    ${ecc608a_dir}/lib/pkcs11/pkcs11_token.c
    ${ecc608a_dir}/lib/pkcs11/pkcs11_object.c
    ${ecc608a_dir}/lib/basic/atca_helpers.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-error=pointer-sign -Wno-error=char-subscripts"
)
endif()
set_source_files_properties(
    ${AFR_MODULES_C_SDK_DIR}/aws/greengrass/aws_greengrass_discovery.c
    ${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_single_task.c
    ${AFR_DEMOS_DIR}/secure_sockets/iot_test_tcp.c
    ${AFR_DEMOS_DIR}/wifi/iot_test_wifi.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format"
)

set_source_files_properties(${AFR_DEMOS_DIR}/logging/iot_logging_task_dynamic_buffers.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format -Wno-uninitialized"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_pal.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign -Wno-sizeof-pointer-memaccess"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_agent.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign"
)

set_source_files_properties(${AFR_DEMOS_DIR}/posix/iot_test_posix_pthread.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-int-conversion"
)

set_source_files_properties(${AFR_ROOT_DIR}/libraries/c_sdk/standard/mqtt/test/unit/iot_tests_mqtt_subscription.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-restrict")

set(IDF_TARGET esp32)
set(ENV{IDF_PATH} ${esp_idf_dir})

# If external project has set sdkconfig.defaults do not overwrite
if (NOT IDF_SDKCONFIG_DEFAULTS)
    # Fetch sdkconfig.defaults and modify the custom partition table csv path
    file(READ "${board_dir}/sdkconfig.defaults" file_sdkconfig_default)
    string(REGEX REPLACE "partition-table.csv" "${board_dir}/partition-table.csv" file_sdkconfig_default "${file_sdkconfig_default}")
    file(WRITE "${CMAKE_BINARY_DIR}/sdkconfig.defaults" "${file_sdkconfig_default}")
    set(IDF_SDKCONFIG_DEFAULTS "${CMAKE_BINARY_DIR}/sdkconfig.defaults")
else()
    set(ABS_SDKCONFIG_DEFAULTS)
    foreach(DEFAULTS ${IDF_SDKCONFIG_DEFAULTS})
        get_filename_component(ABS_DEFAULTS ${DEFAULTS} ABSOLUTE)
        list(APPEND ABS_SDKCONFIG_DEFAULTS ${ABS_DEFAULTS})
    endforeach()
    set(IDF_SDKCONFIG_DEFAULTS ${ABS_SDKCONFIG_DEFAULTS})
endif()

# Set sdkconfig generation path inside build
set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")

# Do some configuration for idf_import_components. This enables creation of artifacts (which might not be
# needed) for some projects
set(IDF_BUILD_ARTIFACTS ON)
set(IDF_BUILD_ARTIFACTS_DIR ${CMAKE_BINARY_DIR})

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")

set_property(GLOBAL PROPERTY IDF_PROJECT_EXECUTABLE ${IDF_PROJECT_EXECUTABLE})

get_filename_component(
    ABS_EXTRA_COMPONENT_DIRS
    "${extra_components_dir}/freertos" ABSOLUTE
)

idf_build_component(${ABS_EXTRA_COMPONENT_DIRS})

get_filename_component(
    ABS_EXTRA_COMPONENT_DIRS
    "${extra_components_dir}/mbedtls" ABSOLUTE
)

idf_build_component(${ABS_EXTRA_COMPONENT_DIRS})

if(AFR_ESP_FREERTOS_TCP)
get_filename_component(
    ABS_NW_EXTRA_COMPONENT_DIRS
    "${extra_components_dir}/freertos_tcpip" ABSOLUTE
)

idf_build_component(${ABS_EXTRA_COMPONENT_DIRS})
endif()

idf_build_process(esp32
                    SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig
                    SDKCONFIG_DEFAULTS ${IDF_SDKCONFIG_DEFAULTS}
                    BUILD_DIR ${CMAKE_BINARY_DIR})

idf_build_get_property(idf_compile_options "COMPILE_OPTIONS")
idf_build_get_property(idf_c_compile_options "C_COMPILE_OPTIONS")
idf_build_get_property(idf_cxx_compile_options "CXX_COMPILE_OPTIONS")
idf_build_get_property(idf_compile_definitions "COMPILE_DEFINITIONS")

# Compiler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${idf_c_compile_options}>
    $<$<COMPILE_LANGUAGE:CXX>:${idf_cxx_compile_options}>
    -Wno-stringop-overflow
    -Wno-bool-operation
    -Wno-sizeof-pointer-div
    -Wno-nonnull
)

# Compiler definitions/macros
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE
    ${idf_compile_definitions}
    -DESP_PLATFORM
)

if(NOT AFR_ESP_FREERTOS_TCP)
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
    -DAFR_ESP_LWIP
)
endif()

target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
    ${idf_compile_options}
)

idf_build_set_property(PROJECT_NAME ${IDF_PROJECT_EXECUTABLE})
idf_build_set_property(EXECUTABLE_NAME ${IDF_PROJECT_EXECUTABLE})
idf_build_set_property(PROJECT_DIR ${AFR_ROOT_DIR})
idf_build_set_property(EXECUTABLE ${IDF_PROJECT_EXECUTABLE})
idf_project_generate_description_file(${CMAKE_BINARY_DIR}/project_description.json)
idf_build_executable(${IDF_PROJECT_EXECUTABLE})
idf_build_set_property(EXECUTABLE_DIR ${CMAKE_BINARY_DIR})

# This is a hack to have IDF build system use PRIVATE keyword when
# calling target_link_libraries() on aws_demos target. This is necessary
# as CMake doesn't allow mixing target_link_libraries() call signature
# for the same target.
function(target_link_libraries)
    set(_args ARGV)
    get_property(exe_target GLOBAL PROPERTY IDF_PROJECT_EXECUTABLE)
    if (${ARGV0} STREQUAL ${exe_target})
        list(INSERT ${_args} 1 PRIVATE)
    endif()
    _target_link_libraries(${${_args}})
endfunction()

# Override IDF's native toolchain file
set(IDF_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE})
