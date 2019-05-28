# -------------------------------------------------------------------------------------------------
# Tests
# -------------------------------------------------------------------------------------------------
# Base test target
afr_test_module(base)

set(src_dir "${AFR_TESTS_DIR}/common/src")
set(inc_dir "${AFR_TESTS_DIR}/common/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/aws_test_framework.c"
        "${src_dir}/aws_test_runner.c"
        "${src_dir}/aws_test.c"
        "${src_dir}/iot_test_afr.c"
        "${src_dir}/iot_tests_network.c"
        "${inc_dir}/aws_test_framework.h"
        "${inc_dir}/aws_test_runner.h"
        "${inc_dir}/aws_clientcredential.h"
        "${inc_dir}/aws_clientcredential_keys.h"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${inc_dir}"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::kernel
        AFR::provisioning
        AFR::common
        3rdparty::unity
)
