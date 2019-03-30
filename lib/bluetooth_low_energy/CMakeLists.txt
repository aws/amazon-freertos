# This module only builds for boards with BLE support.
if(${BLE_SUPPORTED})

    afr_module(ble)

    afr_module_sources(
        ble
        PRIVATE
        "${AFR_MODULES_DIR}/bluetooth_low_energy/iot_ble_gap.c"
        "${AFR_MODULES_DIR}/bluetooth_low_energy/iot_ble_gatt.c"

        "${AFR_MODULES_DIR}/bluetooth_low_energy/services/device_information/iot_ble_device_information.c"
        "${AFR_MODULES_DIR}/bluetooth_low_energy/services/mqtt_ble/iot_ble_mqtt.c"
        "${AFR_MODULES_DIR}/bluetooth_low_energy/services/wifi_provisioning/iot_ble_wifi_provisioning.c"
    )

    afr_module_include_dirs(
        ble
        PUBLIC
            ${BLE_LIBRARY_INCLUDE_DIR}
    )

    afr_module_dependencies(
        ble
        PUBLIC
            AFR::wifi
            AFR::ble::mcu_port
    )
endif()
