project ("freertos_plus pkcs11 cmock unit test")
cmake_minimum_required (VERSION 3.13)
set(3rdparty_dir "${AFR_ROOT_DIR}/libraries/3rdparty")
set(standard_dir "${AFR_ROOT_DIR}/libraries/freertos_plus/standard")
set(kernel_dir "${AFR_ROOT_DIR}/freertos_kernel")

set(preprocess_commands "-I ${3rdparty_dir}/pkcs11/ -D'CK_PTR=*' -D'NULL_PTR=0' \
                         -D'CK_DEFINE_FUNCTION( returnType, name )=returnType name'\ 
                         -D'CK_DECLARE_FUNCTION( returnType, name )=returnType name'\ 
                         -D'CK_DECLARE_FUNCTION_POINTER( returnType, name )=returnType( CK_PTR name )'\
                         -D'CK_CALLBACK_FUNCTION( returnType, name )=returnType( CK_PTR name )'")

separate_arguments(unix_flags UNIX_COMMAND "${preprocess_commands}")

# Mock library
list(APPEND mock_list
    "${kernel_dir}/include/portable.h"
    "${3rdparty_dir}/pkcs11/pkcs11.h"
    )

list(APPEND preprocessed_mock_list
    "${3rdparty_dir}/pkcs11/pkcs11.h"
    )


create_mock_list(pkcs11_mock "${mock_list}")
preprocess_mock_list(pkcs11_mock ${preprocessed_mock_list} "${unix_flags}")


target_compile_definitions(pkcs11_mock PUBLIC
        portHAS_STACK_OVERFLOW_CHECKING=1
        portUSING_MPU_WRAPPERS=1
        MPU_WRAPPERS_INCLUDED_FROM_API_FILE
        )

add_dependencies(pkcs11_mock
                 cmock
                 unity
                 pre_pkcs11_mock
                 )

# Real libraries
add_library(pkcs11_real STATIC
        ${standard_dir}/pkcs11/src/iot_pkcs11.c
        )

target_include_directories(pkcs11_real PUBLIC
        ${3rdparty_dir}/pkcs11/
        ${standard_dir}/pkcs11/include
        ${CMAKE_CURRENT_BINARY_DIR}/mocks
        )

set_target_properties(pkcs11_real PROPERTIES
        COMPILE_FLAGS "-Wall  \
                    -fprofile-arcs -ftest-coverage -fprofile-generate \
                    -include portableDefs.h -Wno-unused-but-set-variable"
        LINK_FLAGS "-fprofile-arcs -ftest-coverage \
                    -fprofile-generate"
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
        )

add_dependencies(pkcs11_real pkcs11_mock)
target_link_libraries(pkcs11_real PUBLIC
        -lpkcs11_mock
        -lgcov
    )

list(APPEND link_list
        -lpkcs11_mock
        libunity.a
        libcmock.a
        libpkcs11_real.a
    )

list(APPEND dep_list
        pkcs11_real
    )

# Unit test build
create_test(iot_pkcs11_utest
        iot_pkcs11_utest.c
        "${link_list}"
        "${dep_list}"
        )

target_include_directories(iot_pkcs11_utest PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/mocks
        ${standard_dir}/pkcs11/include
        ${3rdparty_dir}/pkcs11
        )
