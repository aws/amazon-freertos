# This file contains the description of sources that are required to enable the BLE
# feature in amazon-freertos.

################################################################################
# Additional Source files and includes needed for BLE support
################################################################################
SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/ble/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/ble_hal/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/services/device_information/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/services/mqtt_ble/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/services/wifi_provisioning/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src/services/data_transfer/*.c)\
	$(wildcard $(CY_AFR_ROOT)/libraries/c_sdk/standard/mqtt/src/*.c)

INCLUDES+=\
	$(CY_AFR_BOARD_PATH)/ports/ble\
	$(CY_AFR_ROOT)/libraries/abstractions/ble_hal\
	$(CY_AFR_ROOT)/libraries/abstractions/ble_hal/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/src

ifeq ($(CY_AFR_IS_TESTING), 1)
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/libraries/abstractions/ble_hal/test/src/*.c)\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/test/iot_test_ble_end_to_end.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/test/iot_mqtt_ble_system_test.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/test/iot_test_wifi_provisioning.c\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/test/iot_test_ble_mqtt_serialize.c

INCLUDES+=\
	$(CY_AFR_ROOT)/libraries/abstractions/ble_hal/test/include\
	$(CY_AFR_ROOT)/libraries/c_sdk/standard/ble/test
else
#Demo code
SOURCES+=\
	$(wildcard $(CY_AFR_ROOT)/demos/ble/mqtt_ble/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/ble/gatt_server/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/ble/numeric_comparison/*.c)\
	$(wildcard $(CY_AFR_ROOT)/demos/ble/shadow_ble/*.c)
endif