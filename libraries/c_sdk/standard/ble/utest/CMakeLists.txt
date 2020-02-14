project ("c_sdk ble cmock unit test")
cmake_minimum_required (VERSION 3.13)

set(3rdparty_dir "${AFR_ROOT_DIR}/libraries/3rdparty")
set(freertos_plus_dir "${AFR_ROOT_DIR}/libraries/freertos_plus")
set(abstraction_dir "${AFR_ROOT_DIR}/libraries/abstractions")
set(c_sdk_dir ${AFR_ROOT_DIR}/libraries/c_sdk)
set(kernel_dir "${AFR_ROOT_DIR}/freertos_kernel")
set(common_dir "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common")


# Mock library
list(APPEND mock_list
        ${kernel_dir}/include/portable.h
        ${c_sdk_dir}/standard/ble/include/iot_ble.h
        ${common_dir}/include/private/iot_logging.h
        ${abstraction_dir}/platform/include/platform/iot_threads.h
        )
create_mock_list(iot_ble_data_transfer_mock
        "${mock_list}"
        )

target_include_directories(iot_ble_data_transfer_mock PUBLIC
        config
        ${abstraction_dir}/ble_hal/include
        ${AFR_ROOT_DIR}/tests/include
        ${abstraction_dir}/platform/freertos/include
        ${abstraction_dir}/platform/include
        ${abstraction_dir}/platform/include/types
        ${c_sdk_dir}/standard/common/include
        )

target_compile_definitions(iot_ble_data_transfer_mock PUBLIC
        portHAS_STACK_OVERFLOW_CHECKING=1
        portUSING_MPU_WRAPPERS=1
        MPU_WRAPPERS_INCLUDED_FROM_API_FILE
        IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE
        )

add_dependencies(iot_ble_data_transfer_mock
                 cmock
                 unity
                 utils
                )

# Real libraries
add_library(iot_ble_data_transfer_real STATIC
        ../src/services/mqtt_ble/iot_ble_data_transfer.c
            )
target_include_directories(iot_ble_data_transfer_real PUBLIC
        .
        config
        ../include
        ${AFR_ROOT_DIR}/tests/include
        ${abstraction_dir}/ble_hal/include
        ${abstraction_dir}/platform/include
        ${abstraction_dir}/platform/freertos/include
        ${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include
        ${AFR_ROOT_DIR}/freertos_kernel/include/
        ${CMAKE_CURRENT_BINARY_DIR}/mocks
        )

set_target_properties(iot_ble_data_transfer_real PROPERTIES
        COMPILE_FLAGS "-Wall -fPIC -ggdb3 -Og \
                    -fprofile-arcs -ftest-coverage -fprofile-generate \
                    -include portableDefs.h -Wno-unused-but-set-variable"
        LINK_FLAGS "-fPIC -fprofile-arcs -ftest-coverage \
                    -fprofile-generate -ggdb3 -Og"
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
        )

add_dependencies(iot_ble_data_transfer_real iot_ble_data_transfer_mock)
target_link_libraries(iot_ble_data_transfer_real PUBLIC
        -liot_ble_data_transfer_mock
        -lgcov
        )
list(APPEND link_list
        -liot_ble_data_transfer_mock
        libiot_ble_data_transfer_real.a
        libutils.a
        )
list(APPEND dep_list
        iot_ble_data_transfer_real
        )

# Unit test build
create_test(iot_ble_data_transfer_utest
        iot_ble_data_transfer_utest.c
        "${link_list}"
        "${dep_list}"
        )

target_include_directories(iot_ble_data_transfer_utest PUBLIC
        ../include
        config
        ${AFR_ROOT_DIR}/tests/include
        ${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include
        ${abstraction_dir}/ble_hal/include
        ${abstraction_dir}/platform/freertos/include
        ${abstraction_dir}/platform/include
        ${abstraction_dir}/platform/include/platform
        )
