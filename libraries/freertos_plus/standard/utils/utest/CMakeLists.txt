project ("utils pki cmock unit test")
cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
    set(project_name "iot_pki_utils")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
    list(APPEND mock_list
                "${kernel_dir}/include/portable.h"
        )

# list the directories your mocks need
    list(APPEND mock_include_list
                "${kernel_dir}/include"
        )

#list the definitions of your mocks to control what to be included
    list(APPEND mock_define_list
                portHAS_STACK_OVERFLOW_CHECKING=1
                portUSING_MPU_WRAPPERS=1
                MPU_WRAPPERS_INCLUDED_FROM_API_FILE
        )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
    list(APPEND real_source_files
                ${standard_dir}/utils/src/iot_pki_utils.c
        )

# list the directories the module under test includes
    list(APPEND real_include_directories
                "${standard_dir}/utils/include"
                "${AFR_ROOT_DIR}/tests/unit_test/linux/config_files"
                "${kernel_dir}/include"
                "${CMAKE_CURRENT_BINARY_DIR}/mocks"
        )
# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
    list(APPEND test_include_directories
                ${standard_dir}/utils/include
                ${kernel_dir}/include
                ${CMAKE_CURRENT_BINARY_DIR}/mocks
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

# Unit test build
    set(utest_name "${project_name}_utest")
    set(utest_source "${project_name}_utest.c")

    create_test(${utest_name}
                "${utest_source}"
                "${utest_link_list}"
                "${utest_dep_list}"
                "${test_include_directories}"
        )
