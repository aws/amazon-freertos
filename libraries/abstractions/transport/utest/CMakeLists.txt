project ("transportsecure sockets unit test")
cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
set(project_name "transport_secure_sockets")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
list(APPEND mock_list
            "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include/iot_secure_sockets.h"
        )

#list the definitions of your mocks to control what to be included
list(APPEND mock_include_list
            ""
       )

#list the definitions of your mocks to control what to be included
list(APPEND mock_define_list
            ""
       )

# ================= Create the library under test here (edit) ==================

# Include filepaths for source and include.
include( ${AFR_MODULES_ABSTRACTIONS_DIR}/transport/transport_interface.cmake )

# list the files you would like to test here
list(APPEND real_source_files
            "${AFR_MODULES_ABSTRACTIONS_DIR}/transport/secure_sockets/transport_secure_sockets.c"
        )

# list the directories the module under test includes
list(APPEND real_include_directories
            .
            "${transport_interface_dir}"
            "${AFR_MODULES_ABSTRACTIONS_DIR}/transport/secure_sockets"
            "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include"
            "${AFR_TESTS_DIR}/unit_test/linux/logging-stack"
        )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
list(APPEND test_include_directories
            "${transport_interface_dir}"
            "${AFR_MODULES_ABSTRACTIONS_DIR}/transport/secure_sockets"
            "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include"
            "${AFR_TESTS_DIR}/unit_test/linux/config_files"
        )

# =============================  (end edit)  ===================================

set(mock_name "${project_name}_mock")
set(real_name "${project_name}_real")

create_mock_list(${mock_name}
            "${mock_list}"
            "${CMAKE_CURRENT_LIST_DIR}/project.yml"
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
