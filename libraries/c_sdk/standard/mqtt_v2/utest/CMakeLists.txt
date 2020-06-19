include("../mqttFilePaths.cmake")
project ("mqtt unit test")
cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
set(project_name "mqtt")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
list(APPEND mock_list
            "${MODULES_DIR}/standard/mqtt/include/mqtt_lightweight.h"
            "${MODULES_DIR}/standard/mqtt/include/mqtt_state.h"
        )
# list the directories your mocks need
list(APPEND mock_include_list
            .
            ${MQTT_INCLUDE_PUBLIC_DIRS}
            ${LOGGING_INCLUDE_DIRS}
        )
#list the definitions of your mocks to control what to be included
list(APPEND mock_define_list
            ""
       )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
list(APPEND real_source_files
            ${MQTT_SOURCES}
        )
# list the directories the module under test includes
list(APPEND real_include_directories
            .
            ${MQTT_INCLUDE_PUBLIC_DIRS}
            ${MQTT_INCLUDE_PRIVATE_DIRS}
            ${LOGGING_INCLUDE_DIRS}
        )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
list(APPEND test_include_directories
            .
            ${MQTT_INCLUDE_PUBLIC_DIRS}
            ${MQTT_INCLUDE_PRIVATE_DIRS}
            ${LOGGING_INCLUDE_DIRS}
        )

# =============================  (end edit)  ===================================

set(mock_name "${project_name}_mock")
set(real_name "${project_name}_real")

create_mock_list(${mock_name}
                "${mock_list}"
                "${ROOT_DIR}/tools/cmock/project.yml"
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

# need to redefine because the tests below don't use any mocks
set(utest_link_list "")
list(APPEND utest_link_list
                lib${real_name}.a
        )

# mqtt_state_utest
set(utest_name "${project_name}_state_utest")
set(utest_source "${project_name}_state_utest.c")

create_test(${utest_name}
            ${utest_source}
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
        )

# mqtt_lightweight_utest
set(utest_name "${project_name}_lightweight_utest")
set(utest_source "${project_name}_lightweight_utest.c")

set(utest_link_list "")
list(APPEND utest_link_list
            lib${real_name}.a
        )

create_test(${utest_name}
            ${utest_source}
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
        )
