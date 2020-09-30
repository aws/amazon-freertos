# This file contains the description for the sources that are in the vendors/cypress
# directory.

################################################################################
# vendors/cypress
################################################################################

# Cypress-specific directories and files to ignore
CY_IGNORE+=\
	$(CY_AFR_BOARD_PATH)\
	$(CY_EXTAPP_PATH)/port_support/ota\
	$(CY_EXTAPP_PATH)/port_support/untar\
	$(CY_EXTAPP_PATH)/libraries/connectivity-utilities\
	$(CY_EXTAPP_PATH)/libraries/wifi-host-driver/docs\
	$(CY_EXTAPP_PATH)/libraries/wifi-host-driver/External\
	$(CY_EXTAPP_PATH)/libraries/lpa\

ifneq ($(BLE_SUPPORT),1)
CY_IGNORE+=\
	$(CY_EXTAPP_PATH)/libraries/bluetooth\
	$(CY_EXTAPP_PATH)/port_support/bluetooth
endif

CY_CONFIG_MODUS_FILE=./$(CY_AFR_BOARD_APP_PATH)/design.modus

SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/application_code/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/GeneratedSource/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/*.c)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.S)\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/*.s)\
	$(wildcard $(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/config_files/*.c)\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/wifi/*.c)\

INCLUDES+=\
	$(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/application_code\
	$(CY_AFR_BOARD_APP_PATH)\
	$(CY_AFR_BOARD_APP_PATH)/GeneratedSource\
	$(CY_AFR_BOARD_APP_PATH)/startup\
	$(CY_AFR_BOARD_PATH)/$(CY_AFR_BUILD)/config_files\
	$(CY_AFR_BOARD_PATH)/ports/pkcs11\
	$(CY_AFR_BOARD_PATH)/ports/wifi\

ifneq ($(CY_TFM_PSA_SUPPORTED), 1)
SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/pkcs11/*.c)

INCLUDES+=\
	$(CY_AFR_BOARD_PATH)/ports/pkcs11
else
SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_PATH)/ports/pkcs11/psa/*.c)\
	$(CY_AFR_BOARD_PATH)/ports/pkcs11/hw_poll.c

INCLUDES+=\
	$(CY_AFR_BOARD_PATH)/ports/pkcs11/psa/\
	$(CY_EXTAPP_PATH)/psoc6/psoc64tfm/COMPONENT_TFM_NS_INTERFACE/include
endif
