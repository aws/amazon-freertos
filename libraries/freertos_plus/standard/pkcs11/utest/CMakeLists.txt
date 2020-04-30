    project ("freertos_plus pkcs11 cmock unit test")
    cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
        set(project_name "iot_pkcs11")

# =====================  Create your mock here  (edit)  ========================

#  list of files to preprocess
    list(APPEND preprocessed_mock_list
            "${3rdparty_dir}/pkcs11/pkcs11.h"
        )

    set(preprocess_commands
            "-I ${3rdparty_dir}/pkcs11/ -D'CK_PTR=*' -D'NULL_PTR=0' \
            -D'CK_DEFINE_FUNCTION( returnType, name )=returnType name'\
            -D'CK_DECLARE_FUNCTION( returnType, name )=returnType name'\
            -D'CK_DECLARE_FUNCTION_POINTER( returnType, name )=returnType( CK_PTR name )'\
            -D'CK_CALLBACK_FUNCTION( returnType, name )=returnType( CK_PTR name )'"
        )

# list the files to mock here
    list(APPEND mock_list
                "${kernel_dir}/include/portable.h"
                "${3rdparty_dir}/pkcs11/pkcs11.h"
        )

# list the directories your mocks need
    list(APPEND mock_include_list
            "."
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
                    ${standard_dir}/pkcs11/src/iot_pkcs11.c
                )

# list the directories the module under test includes
        list(APPEND real_include_directories
            ${3rdparty_dir}/pkcs11/
            ${standard_dir}/pkcs11/include
            ${CMAKE_CURRENT_BINARY_DIR}/mocks
            )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
    list(APPEND test_include_directories
            ${CMAKE_CURRENT_BINARY_DIR}/mocks
            ${standard_dir}/pkcs11/include
            ${3rdparty_dir}/pkcs11
        )

# =============================  (end edit)  ===================================

    set(mock_name "${project_name}_mock")
    set(real_name "${project_name}_real")
    set(pre_mock_name  "pre_${mock_name}")

    create_mock_list(${mock_name}
            "${mock_list}"
            "${CMAKE_SOURCE_DIR}/tools/cmock/project.yml"
            "${mock_include_list}"
            "${mock_define_list}"
            )


    separate_arguments(unix_flags UNIX_COMMAND "${preprocess_commands}")
    preprocess_mock_list(${mock_name}
                ${preprocessed_mock_list}
                "${unix_flags}"
            )

    add_dependencies(${mock_name}
                    ${pre_mock_name}
        )

    create_real_library(${real_name}
                    "${real_source_files}"
                    "${real_include_directories}"
                    "${mock_name}"
            )

    list(APPEND utest_link_list
            -l${mock_name}
            lib${real_name}.a
            libutil.so
        )

    list(APPEND utest_dep_list
            ${real_name}
        )

    set(utest_name "${project_name}_utest")
    set(utest_source "${project_name}_utest.c")

# Unit test build
    create_test(${utest_name}
            "${utest_source}"
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
        )

