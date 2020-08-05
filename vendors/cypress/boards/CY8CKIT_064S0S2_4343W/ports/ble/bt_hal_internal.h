/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#include "bt_hal_manager_types.h"
#include "wiced_result.h"
#include "wiced_rtos.h"

#define MAX_BT_DEVICE_NAME_LENGTH   26 /* Max. name size set to 26 (Max. ADV packet size 31 - minimum flag size 5) */

typedef struct {
    uint8_t pucDevName[MAX_BT_DEVICE_NAME_LENGTH];
    BTBdname_t xBtName;
    bool bBondable;
    bool bConnectable;
    bool bSecureConnectionOnly;
    BTIOtypes_t xIoCap;
    uint8_t ucAdapterIfCache;
}WICEDPropCache_t;

BTStatus_t prvConvertWicedErrorToAfr (wiced_result_t xWicedErrorCode);
extern wiced_worker_thread_t btStack_worker_thread;
