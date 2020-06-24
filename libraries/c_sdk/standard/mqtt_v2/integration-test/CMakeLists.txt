include("../mqttFilePaths.cmake")
project ("mqtt system test")
cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
set(project_name "mqtt_system")

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
            # Header path for the TLS server auth utility.
            ${ROOT_DIR}/demos/transport
        )

# =============================  (end edit)  ===================================
set(real_name "${project_name}_real")

create_real_library(${real_name}
                    "${real_source_files}"
                    "${real_include_directories}"
                    # Empty mock name as create_real_library needs the 4th argument.
                    ""
        )

list(APPEND stest_link_list
            lib${real_name}.a
        )

list(APPEND stest_dep_list
            ${real_name}
            transport_utils
        )

set(stest_name "${project_name}_test")
set(stest_source "${project_name}_test.c")
create_test(${stest_name}
            ${stest_source}
            "${stest_link_list}"
            "${stest_dep_list}"
            "${test_include_directories}"
        )
