# -------------------------------------------------------------------------------------------------
# Tests
# -------------------------------------------------------------------------------------------------
# Base test target
afr_test_module(base)

set(src_dir "${AFR_TESTS_DIR}/common")
set(inc_dir "${AFR_TESTS_DIR}/include")
 
if(AFR_ENABLE_TESTS OR FREERTOS_ENABLE_UNIT_TESTS)
    add_subdirectory(integration_test)
endif()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/aws_test_framework.c"
        "${src_dir}/aws_test_runner.c"
        "${src_dir}/aws_test.c"
        "${src_dir}/iot_test_freertos.c"
        "${src_dir}/iot_tests_network.c"
        "${inc_dir}/aws_clientcredential.h"
        "${inc_dir}/aws_clientcredential_keys.h"
        "${inc_dir}/aws_test_runner.h"
        "${inc_dir}/aws_test_framework.h"
        "${inc_dir}/aws_test_tcp.h"
        "${inc_dir}/aws_test_utils.h"
        "${inc_dir}/aws_unity_config.h"
        "${inc_dir}/iot_config_common.h"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE 
        "${inc_dir}"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::kernel
        AFR::common
        3rdparty::unity
)

if(TARGET AFR::common_io::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            AFR::freertos_cli_plus_uart
            AFR::common_io
    )
endif()

if(TARGET AFR::pkcs11_implementation::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            AFR::dev_mode_key_provisioning
    )
endif()
