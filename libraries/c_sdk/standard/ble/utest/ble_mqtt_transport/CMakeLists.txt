    project ("ble_mqtt_transport utest")
    cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
    set(project_name "iot_ble_mqtt_transport")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
    list(APPEND mock_list
        ${AFR_ROOT_DIR}/freertos_kernel/include/portable.h
        ${AFR_ROOT_DIR}/freertos_kernel/include/stream_buffer.h
        ${AFR_ROOT_DIR}/libraries/c_sdk/standard/ble/include/iot_ble_data_transfer.h
        ${AFR_ROOT_DIR}/libraries/c_sdk/standard/ble/include/iot_ble_mqtt_serialize.h
        ${AFR_ROOT_DIR}/libraries/coreMQTT/source/include/core_mqtt_serializer.h
    )
# list the directories your mocks need
    list(APPEND mock_include_list
        ../../include
        ${AFR_ROOT_DIR}/freertos_kernel/include/
        ${AFR_ROOT_DIR}/libraries/coreMQTT/source/include
        ${AFR_ROOT_DIR}/tests/include
        "${AFR_ROOT_DIR}/libraries/coreMQTT/source/interface"
        ${abstraction_dir}/platform/include
        ${abstraction_dir}/platform/include/platform
        ${abstraction_dir}/platform/include/types
    )
#list the definitions of your mocks to control what to be included
    list(APPEND mock_define_list
		portUSING_MPU_WRAPPERS=1
        MPU_WRAPPERS_INCLUDED_FROM_API_FILE
        portHAS_STACK_OVERFLOW_CHECKING=1
        MQTT_DO_NOT_USE_CUSTOM_CONFIG=1
    )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
    list(APPEND real_source_files
            "../../src/services/mqtt_ble/iot_ble_mqtt_transport.c"
        )
# list the directories the module under test includes
    list(APPEND real_include_directories
        .
        "${AFR_ROOT_DIR}/libraries/c_sdk/standard/ble/include"
        "${AFR_ROOT_DIR}/freertos_kernel/include/"
        "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
        "${AFR_ROOT_DIR}/libraries/logging/include"
        "${AFR_ROOT_DIR}/libraries/coreMQTT/source/include"
        "${CMAKE_CURRENT_BINARY_DIR}/mocks"
        ${abstraction_dir}/ble_hal/include
        ${abstraction_dir}/platform/freertos/include
        ${abstraction_dir}/platform/include/platform
        ${abstraction_dir}/platform/include/types
        ${abstraction_dir}/platform/include
        ${AFR_ROOT_DIR}/tests/include
    )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
    list(APPEND test_include_directories
            .
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/ble/include"
            "${AFR_ROOT_DIR}/freertos_kernel/include/"
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
            "${AFR_ROOT_DIR}/libraries/coreMQTT/source/include"
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
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/ble/utest/ble_mqtt_transport/project.yml"
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
            ${utest_source}
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
        )

