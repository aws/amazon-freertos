/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#include "bt_hal_manager_types.h"
#include "wiced_result.h"
#include "cyabs_rtos.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME         ( "port" )
#include "iot_logging_setup.h"

#define MAX_BT_DEVICE_NAME_LENGTH   26 /* Max. name size set to 26 (Max. ADV packet size 31 - minimum flag size 5) */

typedef struct {
    uint8_t pucDevName[MAX_BT_DEVICE_NAME_LENGTH];
    BTBdname_t xBtName;
    bool bBondable;
    bool bConnectable;
    bool bSecureConnectionOnly;
    BTIOtypes_t xIoCap;
    uint8_t ucAdapterIfCache;
    BTBdaddr_t remote_addr;
}WICEDPropCache_t;

BTStatus_t prvConvertWicedErrorToAfr (wiced_result_t xWicedErrorCode);

extern WICEDPropCache_t xWICEDPropCache;
