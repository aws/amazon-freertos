project ("c_sdk ble cmock unit test")
cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
    set(project_name "iot_ble_data_transfer")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
    list(APPEND mock_list
                ${kernel_dir}/include/portable.h
                ${c_sdk_dir}/standard/ble/include/iot_ble.h
                ${common_dir}/include/private/iot_logging.h
                ${abstraction_dir}/platform/include/platform/iot_threads.h
            )

# list the directories your mocks need
    list(APPEND mock_include_list
                ${abstraction_dir}/ble_hal/include
                ${abstraction_dir}/platform/freertos/include
                ${abstraction_dir}/platform/include
                ${abstraction_dir}/platform/include/types
                ${c_sdk_dir}/standard/common/include
            )

#list the definitions of your mocks to control what to be included
    list(APPEND mock_define_list
                portHAS_STACK_OVERFLOW_CHECKING=1
                portUSING_MPU_WRAPPERS=1
                MPU_WRAPPERS_INCLUDED_FROM_API_FILE
                IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE
            )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
    list(APPEND real_source_files
                "../src/services/mqtt_ble/iot_ble_data_transfer.c"
            )
# list the directories the module under test includes
    list(APPEND real_include_directories
            .
            ../include
            ${abstraction_dir}/ble_hal/include
            ${abstraction_dir}/platform/include
            ${abstraction_dir}/platform/freertos/include
            ${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include
            ${AFR_ROOT_DIR}/freertos_kernel/include/
            ${CMAKE_CURRENT_BINARY_DIR}/mocks
        )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
    list(APPEND test_include_directories
                ../include
                ${CMAKE_CURRENT_BINARY_DIR}/mocks
                ${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include
                ${abstraction_dir}/ble_hal/include
                ${abstraction_dir}/platform/freertos/include
                ${abstraction_dir}/platform/include
                ${abstraction_dir}/platform/include/platform
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

    list(APPEND utest_link_list
                -l${mock_name}
                lib${real_name}.a
                libutils.so
            )
    list(APPEND utest_dep_list
                ${real_name}
            )

    set(utest_name "${project_name}_utest")
    set(utest_source "${project_name}_utest.c")

    create_test(${utest_name}
                "${utest_source}"
                "${utest_link_list}"
                "${utest_dep_list}"
                "${test_include_directories}"
            )

