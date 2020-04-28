project ("abstractions pkcs11 cmock unit test")
cmake_minimum_required (VERSION 3.13)
set(3rdparty_dir "${AFR_ROOT_DIR}/libraries/3rdparty")
set(abstractions_dir "${AFR_ROOT_DIR}/libraries/abstractions")
set(standard_dir "${AFR_ROOT_DIR}/libraries/freertos_plus/standard")
set(kernel_dir "${AFR_ROOT_DIR}/freertos_kernel")
set(common_dir "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common")

# Mock library
list(APPEND mock_list
    "${abstractions_dir}/pkcs11/include/iot_pkcs11_pal.h"
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
    "${standard_dir}/utils/include/iot_pki_utils.h"
    )

create_mock_list(pkcs11_mbedtls_mock 
                 "${mock_list}"
                 "${abstractions_dir}/pkcs11/utest/project.yml"
                 )

target_compile_definitions(pkcs11_mbedtls_mock PUBLIC
        portHAS_STACK_OVERFLOW_CHECKING=1
        portUSING_MPU_WRAPPERS=1
        MPU_WRAPPERS_INCLUDED_FROM_API_FILE
        )

target_include_directories(pkcs11_mbedtls_mock PUBLIC
        "${standard_dir}/crypto/include"
        "${common_dir}/include"
        "${3rdparty_dir}/mbedtls/include/mbedtls"
        "${3rdparty_dir}/mbedtls_utils"
        "${kernel_dir}/include"
        "${standard_dir}/utils/include"
        "${AFR_ROOT_DIR}/tests/unit_test/linux/config_files"
        "${abstractions_dir}/pkcs11/include"
        "${standard_dir}/pkcs11/include"
        )

# PKCS #11 header files must always come before the mbedTLS header files
target_include_directories(pkcs11_mbedtls_mock BEFORE PUBLIC
        "${3rdparty_dir}/pkcs11"
        )

add_dependencies(pkcs11_mbedtls_mock
                 cmock
                 unity
                 )

# Real libraries
add_library(pkcs11_mbedtls_real STATIC
        ${3rdparty_dir}/mbedtls/library/pk_wrap.c
        ${abstractions_dir}/pkcs11/mbedtls/iot_pkcs11_mbedtls.c
        )

target_include_directories(pkcs11_mbedtls_real PUBLIC
       "${standard_dir}/pkcs11/include"
       "${standard_dir}/utils/include"
       "${common_dir}/include/"
       "${standard_dir}/utils/include"
       "${abstractions_dir}/pkcs11/include"
       "${AFR_ROOT_DIR}/tests/unit_test/linux/config_files"
       "${3rdparty_dir}/mbedtls/include"
       "${3rdparty_dir}/mbedtls_utils"
       "${standard_dir}/crypto/include"
       "${AFR_ROOT_DIR}/tests/include"
       "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include/private"
       "${kernel_dir}/include"
       "${CMAKE_CURRENT_BINARY_DIR}/mocks"
        )

# PKCS #11 header files must always come before the mbedTLS header files
target_include_directories(pkcs11_mbedtls_real BEFORE PUBLIC
        "${3rdparty_dir}/pkcs11"
        )

set_target_properties(pkcs11_mbedtls_real PROPERTIES
        COMPILE_FLAGS "-Wall \
                    -fprofile-arcs -ftest-coverage -fprofile-generate \
                    -include portableDefs.h -Wno-unused-but-set-variable"
        LINK_FLAGS "-fprofile-arcs -ftest-coverage \
                    -fprofile-generate"
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
        )

add_dependencies(pkcs11_mbedtls_real pkcs11_mbedtls_mock)
target_link_libraries(pkcs11_mbedtls_real PUBLIC
        -lpkcs11_mbedtls_mock
        -lgcov
    )

list(APPEND link_list
        -lpkcs11_mbedtls_mock
        libunity.a
        libcmock.a
        libpkcs11_mbedtls_real.a
    )

list(APPEND dep_list
        pkcs11_mbedtls_real
    )

# Unit test build
create_test(iot_pkcs11_mbedtls_utest
        iot_pkcs11_mbedtls_utest.c
        "${link_list}"
        "${dep_list}"
        )

target_include_directories(iot_pkcs11_mbedtls_utest PUBLIC
        ${standard_dir}/pkcs11/include
        ${common_dir}/include
        ${standard_dir}/utils/include
        ${abstractions_dir}/pkcs11/include
        ${3rdparty_dir}/mbedtls/include
        ${3rdparty_dir}/mbedtls/include/mbedtls
        ${AFR_ROOT_DIR}/tests/include
        ${standard_dir}/crypto/include
        ${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include/private
        ${3rdparty_dir}/mbedtls_utils
        ${kernel_dir}/include
        ${CMAKE_CURRENT_BINARY_DIR}/mocks
        )

# PKCS #11 header files must always come before the mbedTLS header files
target_include_directories(iot_pkcs11_mbedtls_utest BEFORE PUBLIC
        "${3rdparty_dir}/pkcs11"
        )
