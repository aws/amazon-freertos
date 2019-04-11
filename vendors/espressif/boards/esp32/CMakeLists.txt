# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
if("${AFR_BOARD_NAME}" STREQUAL "esp32_devkit_c")
    include("esp32_devkit_c.cmake")
elseif("${AFR_BOARD_NAME}" STREQUAL "esp32_wrover_kit")
    include("esp32_wrover_kit.cmake")
endif()

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)

set(linker_flags "-Wl,--gc-sections" "-Wl,--cref" "-Wl,--Map=output.map" "-Wl,--undefined=uxTopUsedPriority")

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE ${linker_flags}
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
set(vendor "espressif")
set(board "esp32_devkitc_esp_wrover_kit")
set(portable_dir "portable/${vendor}/${board}")

set(espressif_dir "${AFR_3RDPARTY_DIR}/mcu_vendor/espressif/esp-idf")
set(board_demos_dir "${AFR_ROOT_DIR}/demos/espressif/esp32_devkitc_esp_wrover_kit/common")
set(board_tests_dir "${AFR_ROOT_DIR}/tests/espressif/esp32_devkitc_esp_wrover_kit/common")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TESTS_DIR}/include")
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
endif()

# Kernel
afr_mcu_port(kernel)
afr_glob_src(driver_src DIRECTORY "${espressif_dir}" RECURSE)

set(
    kernel_inc_dirs
    "${espressif_dir}/components/app_update/include/"
    "${espressif_dir}/components/bootloader_support/include"
    "${espressif_dir}/components/esp32/include"
    "${espressif_dir}/components/driver/include"
    "${espressif_dir}/components/heap/include"
    "${espressif_dir}/components/log/include"
    "${espressif_dir}/components/nvs_flash/include"
    "${espressif_dir}/components/pthread/include"
    "${espressif_dir}/components/newlib/include"
    "${espressif_dir}/components/soc/esp32/include"
    "${espressif_dir}/components/soc/include"
    "${espressif_dir}/components/spi_flash/include"
    "${board_dir}/application_code/espressif_code/freertos/include"
    "${board_dir}/application_code/espressif_code/ethernet/include"
    "${board_dir}/application_code/espressif_code/mbedtls/port/include"
    "${board_dir}/application_code/espressif_code/smartconfig_ack/include"
    "${board_dir}/application_code/espressif_code/tcpip_adapter/include"
    "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC"
)
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        ${kernel_inc_dirs}
        "${board_dir}/config_files"
        "${AFR_MODULES_DIR}/FreeRTOS/portable/ThirdParty/GCC/Xtensa_ESP32/include/"
        "${aws_credentials_include}"
        "${CMAKE_BINARY_DIR}/config"
)

# WiFi
afr_mcu_port(wifi)
target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
    AFR::freertos_plus_tcp
)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${AFR_MODULES_DIR}/wifi/${portable_dir}/aws_wifi.c"
)

# BLE
set(BLE_SUPPORTED 1 CACHE INTERNAL "BLE is supported on this platform.")

afr_mcu_port(ble)
target_sources(
    AFR::ble::mcu_port
    INTERFACE
        "${AFR_MODULES_DIR}/bluetooth_low_energy/portable/espressif/aws_ble_hal_common_gap.c"
        "${AFR_MODULES_DIR}/bluetooth_low_energy/portable/espressif/aws_ble_hal_gap.c"
        "${AFR_MODULES_DIR}/bluetooth_low_energy/portable/espressif/aws_ble_hal_gatt_server.c"
)
target_include_directories(
    AFR::ble::mcu_port
    INTERFACE
        "${espressif_dir}/components/bt/bluedroid/api/include/api"
        "${espressif_dir}/components/bt/include"
        "${AFR_MODULES_DIR}/bluetooth_low_energy/portable/espressif/"
)

# PKCS11
afr_mcu_port(pkcs11)
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE AFR::pkcs11_mbedtls
)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE "${AFR_MODULES_DIR}/pkcs11/${portable_dir}/aws_pkcs11_pal.c"
)

# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_2.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/esp32/NetworkInterface.c"
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE
        AFR::freertos_plus_tcp
        AFR::pkcs11
        AFR::tls
)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${AFR_MODULES_DIR}/secure_sockets/portable/freertos_plus_tcp/aws_secure_sockets.c"
)

# OTA
# Need to get this validated
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE
        "${AFR_MODULES_DIR}/ota/portable/espressif/esp32_devkitc_esp_wrover_kit/aws_ota_pal.c"
        "${AFR_MODULES_DIR}/ota/portable/espressif/esp32_devkitc_esp_wrover_kit/aws_esp_ota_ops.c"
)
target_link_libraries(
    AFR::ota::mcu_port
    INTERFACE 3rdparty::pkcs11
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
    set(extra_exe_sources "${AFR_TESTS_DIR}/test_runner/iot_tests_network.c")
else()
    set(exe_target aws_demos)
    set(
        extra_exe_sources
        "${AFR_DEMOS_DIR}/ble/iot_ble_numericComparison.c"
        ${NETWORK_MANAGER_SOURCES}
    )
endif()

add_executable(
    ${exe_target}
    "${board_dir}/application_code/main.c"
    ${extra_exe_sources}
)
target_include_directories(
    ${exe_target}
    PUBLIC
        $<TARGET_PROPERTY:AFR::kernel,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:AFR::ble::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>
)
target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
        AFR::ble
)

if(AFR_NON_BUILD_MODE)
    return()
endif()


# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------

set_source_files_properties(${AFR_MODULES_DIR}/greengrass/aws_greengrass_discovery.c
    ${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_single_task.c
    ${AFR_DEMOS_DIR}/secure_sockets/aws_test_tcp.c
    ${AFR_DEMOS_DIR}/wifi/aws_test_wifi.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format"
)

set_source_files_properties(${AFR_DEMOS_DIR}/logging/aws_logging_task_dynamic_buffers.c
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

set_source_files_properties(${AFR_DEMOS_DIR}/posix/aws_test_posix_pthread.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-int-conversion"
)

set(IDF_TARGET esp32)
set(ENV{IDF_PATH} ${espressif_dir})

# Fetch sdkconfig.defaults and modify the custom partition table csv path
file(READ "${board_dir}/../make/sdkconfig.defaults" file_sdkconfig_default)
string(REGEX REPLACE "partitions_example.csv" "${board_dir}/../make/partitions_example.csv" file_sdkconfig_default "${file_sdkconfig_default}")
file(WRITE "${CMAKE_BINARY_DIR}/sdkconfig.defaults" "${file_sdkconfig_default}")
set(IDF_SDKCONFIG_DEFAULTS "${CMAKE_BINARY_DIR}/sdkconfig.defaults")

# Do some configuration for idf_import_components. This enables creation of artifacts (which might not be
# needed) for some projects
set(IDF_BUILD_ARTIFACTS ON)
set(IDF_BUILD_ARTIFACTS_DIR ${CMAKE_BINARY_DIR})

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")

# This is a hack to have IDF build system use PRIVATE keyword when
# calling target_link_libraries() on aws_demos target. This is necessary
# as CMake doesn't allow mixing target_link_libraries() call signature
# for the same target.
function(target_link_libraries)
    set(_args ARGV)
    if ((${ARGV0} STREQUAL aws_tests) OR (${ARGV0} STREQUAL aws_demos))
        list(INSERT ${_args} 1 PRIVATE)
    endif()
    _target_link_libraries(${${_args}})
endfunction()

# Override IDF's native toolchain file
set(IDF_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE})

# Provides idf_import_components and idf_link_components
include(${espressif_dir}/tools/cmake/idf_functions.cmake)

get_filename_component(
    ABS_EXTRA_COMPONENT_DIRS
    "${board_dir}/application_code/espressif_code" ABSOLUTE
)

set(IDF_EXTRA_COMPONENT_DIRS ${ABS_EXTRA_COMPONENT_DIRS})
set(IDF_PROJECT_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/${exe_target}")

# Wraps add_subdirectory() to create library targets for components, and then `return` them using the given variable.
# In this case the variable is named `component`
idf_import_components(components ${espressif_dir} esp-idf)

# Wraps target_link_libraries() to link processed components by idf_import_components to target
idf_link_components(${exe_target} "${components}")
