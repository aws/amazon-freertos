# Wifi Provisioning demo
afr_demo_module(wifi_provisioning)

afr_set_demo_metadata(ID "wifi_provisioning")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates WiFi provisioning over BLE")
afr_set_demo_metadata(DISPLAY_NAME "BLE Wi-Fi Provisioning")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_wifi_connect_task.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::wifi
        AFR::ble
)
