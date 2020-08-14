board_list=("CY8CKIT_062_WIFI_BT" "CY8CKIT_062S2_43012" "CY8CPROTO_062_4343W" "CY8CKIT_064S0S2_4343W")
project_list=("aws_demos aws_tests")

# The exclude_list and the sync_list need to stay in sync with each other as each
# value in the exclude_list corresponds to the files to be excluded from the diff of the
# directory or file at the same index in the sync_list.
sync_list=('$project_name/application_code/main.c' '$project_name/config_files/' 'ports/')
exclude_list=("" "-x FreeRTOSConfig.h" "-x ota -x psa -x iot_pkcs11_pal.c")

boards_dir="$(dirname "${BASH_SOURCE[0]}")/../../vendors/cypress/boards"
secure_ota_port_dir="$(dirname "${BASH_SOURCE[0]}")/../../vendors/cypress/MTB/port_support/ota/ports/CY8CKIT_064S0S2_4343W"
