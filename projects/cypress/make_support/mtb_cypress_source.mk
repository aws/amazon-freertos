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
	$(CY_EXTAPP_PATH)/libraries/lpa\
	$(CY_EXTAPP_PATH)/libraries/enterprise-security\
	$(CY_EXTAPP_PATH)/libraries/utilities\

ifeq ($(BLE_SUPPORT),)
CY_IGNORE+=\
	$(CY_EXTAPP_PATH)/libraries/bluetooth
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

ifeq ($(CY_TFM_PSA_SUPPORTED),)
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

# SDIO_HOST sources and includes
ifneq ($(filter $(TARGET),CY8CKIT-062-WIFI-BT CYW943012P6EVB-01),)
SOURCES+=\
	$(wildcard $(CY_AFR_BOARD_APP_PATH)/SDIO_HOST/*.c)

INCLUDES+=\
	$(CY_AFR_BOARD_APP_PATH)/SDIO_HOST
endif