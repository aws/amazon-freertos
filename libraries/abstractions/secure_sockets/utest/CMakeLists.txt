project ("secure sockets unit test")
cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
set(project_name "secure_sockets")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
list(APPEND mock_list
            "${kernel_dir}/include/task.h"
            "${kernel_dir}/include/portable.h"
            "${AFR_MODULES_DIR}/logging/include/iot_logging_task.h"
            "${freertos_plus_dir}/standard/tls/include/iot_tls.h"
            "${3rdparty_dir}/lwip/src/include/lwip/sockets.h"
            "${3rdparty_dir}/lwip/src/include/lwip/ip_addr.h"
            "${3rdparty_dir}/lwip/src/include/lwip/dns.h"
        )
        
# list the directories your mocks need
list(APPEND mock_include_list
            "${CMAKE_CURRENT_LIST_DIR}/include"
            "${3rdparty_dir}/lwip/src/include"
            "${3rdparty_dir}/lwip/src/include/lwip"
            "${3rdparty_dir}/lwip/contrib/ports/unix/port/include"
        )
#list the definitions of your mocks to control what to be included
list(APPEND mock_define_list
            LWIP_COMPAT_SOCKETS=1
            portHAS_STACK_OVERFLOW_CHECKING=1
            portUSING_MPU_WRAPPERS=1
            MPU_WRAPPERS_INCLUDED_FROM_API_FILE
       )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
list(APPEND real_source_files
            "../lwip/iot_secure_sockets.c"
        )
# list the directories the module under test includes
list(APPEND real_include_directories
            .
            ../include
            "${CMAKE_CURRENT_LIST_DIR}/include"
            "${3rdparty_dir}/lwip/src/include/lwip"
            "${3rdparty_dir}/lwip/src/include"
            "${3rdparty_dir}/lwip/contrib/ports/unix/lib"
            "${3rdparty_dir}/lwip/contrib/ports/unix/port/include"
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
            "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/tls/include"
            "${AFR_ROOT_DIR}/freertos_kernel/include/"
            "${CMAKE_CURRENT_BINARY_DIR}/mocks"
        )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
list(APPEND test_include_directories
            ../include
            "${3rdparty_dir}/lwip/src/include/lwip"
            "${3rdparty_dir}/lwip/src/include"
            "${3rdparty_dir}/lwip/contrib/ports/unix/lib"
            "${3rdparty_dir}/lwip/contrib/ports/unix/port/include"
            "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/tls/include"
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
            "${CMAKE_CURRENT_LIST_DIR}/include"
            "${AFR_MODULES_DIR}/logging/include"
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
