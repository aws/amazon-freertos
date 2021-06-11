    project ("ble_middleware utest")
    cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
    set(project_name "iot_ble_middleware")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
    list(APPEND mock_list
        ${AFR_ROOT_DIR}/freertos_kernel/include/portable.h
        ${abstraction_dir}/ble_hal/include/bt_hal_manager.h
        ${abstraction_dir}/ble_hal/include/bt_hal_manager_adapter_ble.h
        ${abstraction_dir}/ble_hal/include/bt_hal_gatt_server.h
        ${CMAKE_CURRENT_LIST_DIR}/iot_ble_interface.h
        ${abstraction_dir}/platform/include/platform/iot_threads.h
        ${common_dir}/include/private/iot_logging.h
    )
# list the directories your mocks need
    list(APPEND mock_include_list
        ${CMAKE_CURRENT_LIST_DIR}
        ${abstraction_dir}/ble_hal/include
        ${abstraction_dir}/platform/include
        ${abstraction_dir}/platform/freertos/include
        ${abstraction_dir}/platform/include
        "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
    )
#list the definitions of your mocks to control what to be included
    list(APPEND mock_define_list
		portUSING_MPU_WRAPPERS=1
        MPU_WRAPPERS_INCLUDED_FROM_API_FILE
        portHAS_STACK_OVERFLOW_CHECKING=1
    )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
    list(APPEND real_source_files
            "${AFR_ROOT_DIR}/libraries/ble/src/iot_ble_gap.c"
            "${AFR_ROOT_DIR}/libraries/ble/src/iot_ble_gatt.c"
        )
# list the directories the module under test includes
    list(APPEND real_include_directories
        "${AFR_ROOT_DIR}/libraries/ble/include"
        "${AFR_ROOT_DIR}/freertos_kernel/include/"
        "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
        "${AFR_ROOT_DIR}/libraries/logging/include"
        "${CMAKE_CURRENT_BINARY_DIR}/mocks"
        ${abstraction_dir}/ble_hal/include
        ${AFR_ROOT_DIR}/tests/include
        ${abstraction_dir}/platform/freertos/include
        ${abstraction_dir}/platform/include
    )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
    list(APPEND test_include_directories
            "${CMAKE_CURRENT_LIST_DIR}"
            "${AFR_ROOT_DIR}/libraries/ble/include"
            "${AFR_ROOT_DIR}/freertos_kernel/include/"
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
            "${AFR_ROOT_DIR}/tests/unit_test/linux/logging-stack"
            "${CMAKE_CURRENT_BINARY_DIR}/mocks"
            ${abstraction_dir}/ble_hal/include
            ${abstraction_dir}/platform/freertos/include
            ${abstraction_dir}/platform/include/platform
            ${abstraction_dir}/platform/include/types
            ${abstraction_dir}/platform/include
            ${AFR_ROOT_DIR}/tests/include
	    )

# =============================  (end edit)  ===================================

set(mock_name "${project_name}_mock")
set(real_name "${project_name}_real")

create_mock_list(${mock_name}
            "${mock_list}"
            "${CMAKE_SOURCE_DIR}/tools/cmock/project.yml"
            "${mock_include_list}"
            "${mock_define_list}"
        )

create_real_library(${real_name}
                    "${real_source_files}"
                    "${real_include_directories}"
                    "${mock_name}"
        )

#Turn off External modules as they are not part of test.
target_compile_definitions(${real_name} PUBLIC
        IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE=0
        IOT_BLE_ENABLE_DEVICE_INFO_SERVICE=0
        IOT_BLE_ADD_CUSTOM_SERVICES=0
)


list(APPEND utest_link_list
            -l${mock_name}
            lib${real_name}.a
            libutils.so
        )

list(APPEND utest_dep_list
            ${real_name}
        )

set(utest_name "${project_name}_utest")
set(utest_source "iot_ble_gap_gatt_utest.c")
create_test(${utest_name}
            ${utest_source}
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
        )

