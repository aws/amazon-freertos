project ("abstractions pkcs11 cmock unit test")
cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
    set(project_name "iot_pkcs11_mbedtls")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
    list(APPEND mock_list
                "${abstraction_dir}/pkcs11/include/iot_pkcs11_pal.h"
                "${kernel_dir}/include/portable.h"
                "${kernel_dir}/include/queue.h"
                "${common_dir}/include/iot_logging_task.h"
                "${standard_dir}/crypto/include/iot_crypto.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/platform_util.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/ctr_drbg.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/sha256.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/base64.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/bignum.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/entropy.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/md.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/rsa.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/ecp.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/ecdsa.h"
                "${3rdparty_dir}/mbedtls_utils/mbedtls_error.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/pk.h"
                "${3rdparty_dir}/mbedtls/include/mbedtls/x509_crt.h"
                "${standard_dir}/utils/include/iot_pki_utils.h"
        )

# list the directories your mocks need
    list(APPEND mock_include_list
                "${standard_dir}/crypto/include"
                "${common_dir}/include"
                "${3rdparty_dir}/mbedtls/include/mbedtls"
                "${3rdparty_dir}/mbedtls_utils"
                "${kernel_dir}/include"
                "${standard_dir}/utils/include"
                "${AFR_ROOT_DIR}/tests/unit_test/linux/config_files"
                "${abstraction_dir}/pkcs11/include"
                "${standard_dir}/pkcs11/include"
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
                ${3rdparty_dir}/mbedtls/library/pk_wrap.c
                ${abstraction_dir}/pkcs11/mbedtls/iot_pkcs11_mbedtls.c
        )

# list the directories the module under test includes
    list(APPEND real_include_directories
                "${3rdparty_dir}/pkcs11" # should come first
                "${standard_dir}/pkcs11/include"
                "${standard_dir}/utils/include"
                "${common_dir}/include/"
                "${standard_dir}/utils/include"
                "${abstraction_dir}/pkcs11/include"
                "${AFR_ROOT_DIR}/tests/unit_test/linux/config_files"
                "${3rdparty_dir}/mbedtls/include"
                "${3rdparty_dir}/mbedtls_utils"
                "${standard_dir}/crypto/include"
                "${AFR_ROOT_DIR}/tests/include"
                "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include/private"
                "${kernel_dir}/include"
                "${CMAKE_CURRENT_BINARY_DIR}/mocks"
        )
# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
    list(APPEND test_include_directories
                "${3rdparty_dir}/pkcs11" # should come first
                ${standard_dir}/pkcs11/include
                ${common_dir}/include
                ${standard_dir}/utils/include
                ${abstraction_dir}/pkcs11/include
                ${3rdparty_dir}/mbedtls/include
                ${3rdparty_dir}/mbedtls/include/mbedtls
                ${AFR_ROOT_DIR}/tests/include
                ${standard_dir}/crypto/include
                ${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include/private
                ${3rdparty_dir}/mbedtls_utils
                ${kernel_dir}/include
                ${CMAKE_CURRENT_BINARY_DIR}/mocks
        )
# =============================  (end edit)  ===================================

    set(mock_name "${project_name}_mock")
    set(real_name "${project_name}_real")

    create_mock_list(${mock_name}
                "${mock_list}"
                "${abstraction_dir}/pkcs11/utest/project.yml"
                "${mock_include_list}"
                "${mock_define_list}"
        )
# PKCS #11 header files must always come before the mbedTLS header files
    target_include_directories(${mock_name} BEFORE PUBLIC
                "${3rdparty_dir}/pkcs11"
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
