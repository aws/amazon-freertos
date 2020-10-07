    project ("transportsecure sockets unit test")
    cmake_minimum_required (VERSION 3.13)

# ====================  Define your project name (edit) ========================
    set(project_name "transport_secure_sockets")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
    list(APPEND mock_list
                "${kernel_dir}/include/task.h"
                "${kernel_dir}/include/portable.h"
                "${common_dir}/include/iot_logging_task.h"
                "${abstraction_dir}/secure_sockets/include/iot_secure_sockets.h"
            )
# list the directories your mocks need
    list(APPEND mock_include_list
                "${CMAKE_CURRENT_LIST_DIR}/include"
            )
#list the definitions of your mocks to control what to be included
    list(APPEND mock_define_list
                LWIP_COMPAT_SOCKETS=1
                portHAS_STACK_OVERFLOW_CHECKING=1
                portUSING_MPU_WRAPPERS=1
                MPU_WRAPPERS_INCLUDED_FROM_API_FILE
           )

# ================= Create the library under test here (edit) ==================

# The single source of truth for the transport_interface.h is in the coreMQTT repository.
# In the FreeRTOS Device Software download page in AWS IoT Core, it is possible
# to choose coreHTTP instead of coreMQTT. In this case, it is necessary to check
# for the existence of the file in the downloaded software package.
if( EXISTS ${AFR_MODULES_DIR}/coreMQTT/source )
    set(transport_interface_dir "${AFR_MODULES_DIR}/coreMQTT/source/portable")
elseif( EXISTS ${AFR_MODULES_DIR}/coreHTTP/source )
    set(transport_interface_dir "${AFR_MODULES_DIR}/coreHTTP/source/portable")
else()
    message( FATAL_ERROR "No transport_interface.h exists for this included interface.")
endif()

# list the files you would like to test here
    list(APPEND real_source_files
                "${AFR_ROOT_DIR}/libraries/abstractions/transport/secure_sockets/transport_secure_sockets.c"
            )
# list the directories the module under test includes
    list(APPEND real_include_directories
                .
                "${transport_interface_dir}"
                "${AFR_ROOT_DIR}/libraries/abstractions/transport/secure_sockets"
                "${AFR_ROOT_DIR}/libraries/abstractions/secure_sockets/include"
                "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
            )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
    list(APPEND test_include_directories
                "${transport_interface_dir}"
                "${AFR_ROOT_DIR}/libraries/abstractions/transport/secure_sockets"
                "${AFR_ROOT_DIR}/libraries/abstractions/secure_sockets/include"
                "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
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

