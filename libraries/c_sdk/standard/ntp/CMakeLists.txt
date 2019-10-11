afr_module()

afr_set_lib_metadata(ID "ntp")
afr_set_lib_metadata(DESCRIPTION "This library implements the NTP protocol that enables \
fetching and synchronizing wallclock time from NTP server.")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/iot_sntp_client.c"
        "${src_dir}/iot_sntp_date_time.c"
        "${inc_dir}/iot_sntp_client.h"
        "${inc_dir}/iot_sntp_config.h"
        "${inc_dir}/iot_sntp_date_time.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::secure_sockets
)

# test for NTP module
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_sntp.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::ntp
)