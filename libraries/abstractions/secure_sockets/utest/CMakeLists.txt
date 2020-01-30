#afr_module(secure_socket_ut)
    project ("secure socket cmock unit test")
    cmake_minimum_required (VERSION 3.13)

    set(3rdparty_dir "${AFR_ROOT_DIR}/libraries/3rdparty")
    set(freertos_plus_dir "${AFR_ROOT_DIR}/libraries/freertos_plus")
    set(abstraction_dir "${AFR_ROOT_DIR}/libraries/abstractions")
    set(kernel_dir "${AFR_ROOT_DIR}/freertos_kernel")
    set(common_dir "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common")

    list(APPEND mock_list
                "${kernel_dir}/include/task.h"
                "${kernel_dir}/include/portable.h"
                "${common_dir}/include/iot_logging_task.h"
                "${abstraction_dir}/wifi/include/iot_wifi.h"
                "${freertos_plus_dir}/standard/tls/include/iot_tls.h"
                "${3rdparty_dir}/lwip/src/include/lwip/sockets.h"
            )
    create_mock_list(secure_sockets_mock "${mock_list}"
            )
    add_dependencies(secure_sockets_mock libcmock libunity
            )
    target_include_directories(secure_sockets_mock PUBLIC
                "${3rdparty_dir}/lwip/src/include"
                "${3rdparty_dir}/lwip/src/include/lwip"
                "${3rdparty_dir}/lwip/contrib/ports/unix/lib"
                "${3rdparty_dir}/lwip/contrib/ports/unix/port/include"
            )

    target_compile_definitions(secure_sockets_mock PUBLIC
                LWIP_COMPAT_SOCKETS=1
            )
    target_compile_definitions(secure_sockets_mock PUBLIC
                portHAS_STACK_OVERFLOW_CHECKING=1
            )
    target_compile_definitions(secure_sockets_mock PUBLIC
                portUSING_MPU_WRAPPERS=1
            )
    target_compile_definitions(secure_sockets_mock PUBLIC
                MPU_WRAPPERS_INCLUDED_FROM_API_FILE
            )

    add_library(secure_sockets_real STATIC
                "../lwip/iot_secure_sockets.c"
            )
    target_include_directories(secure_sockets_real PUBLIC
                .
                ../include
                "${3rdparty_dir}/lwip/src/include"
                "${3rdparty_dir}/lwip/contrib/ports/unix/lib"
                "${3rdparty_dir}/lwip/contrib/ports/unix/port/include"
                "${AFR_ROOT_DIR}/libraries/abstractions/wifi/include"
                "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
                "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/tls/include"
                "${AFR_ROOT_DIR}/freertos_kernel/include/"
                "${CMAKE_CURRENT_BINARY_DIR}/mocks"
            )

    set_target_properties(secure_sockets_real PROPERTIES
                COMPILE_FLAGS "-Wall -fPIC -ggdb3 -O0  -pg \
                    -fprofile-arcs -ftest-coverage -fprofile-generate \
                    -fsanitize=address \
                    -fsanitize=leak \
                    -include /usr/include/errno.h "
                LINK_FLAGS "-fPIC -fprofile-arcs -ftest-coverage \
                    -fprofile-generate -ggdb3 -O0 -pg \
                    -fsanitize=address \
                    -fsanitize=address \
                    -fsanitize=leak"
                ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
            )

    add_dependencies(secure_sockets_real secure_sockets_mock)
    target_link_libraries(secure_sockets_real PUBLIC
                          -lsecure_sockets_mock
                          -lgcov
            )
    list(APPEND link_list
                -lsecure_sockets_mock
                libsecure_sockets_real.a
                libutils.a
            )
    list(APPEND dep_list
                secure_sockets_real
            )
    create_test(secure_sockets_utest
                secure_sockets_utest.c
                "${link_list}"
                "${dep_list}"
            )
    set_target_properties(secure_sockets_utest PROPERTIES
                COMPILE_FLAGS "-include /usr/include/errno.h"
            )
    target_include_directories(secure_sockets_utest PUBLIC
                ../include
                "${3rdparty_dir}/lwip/src/include/lwip"
                "${3rdparty_dir}/lwip/src/include"
                "${3rdparty_dir}/lwip/contrib/ports/unix/lib"
                "${3rdparty_dir}/lwip/contrib/ports/unix/port/include"
                "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/tls/include"
                "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
                "${AFR_ROOT_DIR}/libraries/abstractions/wifi/include"
            )
