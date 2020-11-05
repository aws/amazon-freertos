/*
 * FreeRTOS
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file cellular_setup.c
 * @brief Setup cellular connectivity for board with cellular module.
 */

/* FreeRTOS include. */
#include <FreeRTOS.h>
#include "task.h"

#include <stdbool.h>

/* Test runner includes. */
#include "aws_test_runner.h"
#include "iot_system_init.h"
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_types.h"
#include "cellular_api.h"
#include "cellular_comm_interface.h"
#include "aws_test_cellular.h"
#include "FreeRTOSIPConfig.h"

/* Secure socket needs application provide the cellular handle and pdn context id. */
/* User of secure sockets cellular should provide this variable. */
/* coverity[misra_c_2012_rule_8_6_violation] */
CellularHandle_t CellularHandle = NULL;
/* User of secure sockets cellular should provide this variable. */
/* coverity[misra_c_2012_rule_8_6_violation] */
uint8_t CellularSocketPdnContextId = testCELLULAR_PDN_CONTEXT_ID;

#define CELLULAR_SIM_CARD_WAIT_INTERVAL_MS (500UL)
#define CELLULAR_SIM_CARD_WAIT_TIMEOUT_MS (60000UL)

#define CELLULAR_MAX_SIM_RETRY (5U)

/**
 * @brief the default Cellular comm interface in system.
 */
extern CellularCommInterface_t CellularCommInterface;

bool setupCellular(void)
{
    bool cellularRet = true;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularSimCardStatus_t simStatus = {0};
    CellularServiceStatus_t serviceStatus = {0};
    CellularCommInterface_t *pCommIntf = &CellularCommInterface;
    uint8_t tries = 0;
    CellularPdnConfig_t pdnConfig = {CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, testCELLULAR_APN, "", ""};
    CellularPdnStatus_t PdnStatusBuffers = {0};
    char localIP[CELLULAR_IP_ADDRESS_MAX_SIZE] = {'\0'};
    uint32_t timeoutCountLimit = (CELLULAR_SIM_CARD_WAIT_TIMEOUT_MS / CELLULAR_SIM_CARD_WAIT_INTERVAL_MS) + 1U;
    uint32_t timeoutCount = 0;
    uint8_t NumStatus = 1;

    /* Initialize Cellular Comm Interface. */
    cellularStatus = Cellular_Init(&CellularHandle, pCommIntf);

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        /* wait until SIM is ready */
        for (tries = 0; tries < CELLULAR_MAX_SIM_RETRY; tries++)
        {
            cellularStatus = Cellular_GetSimCardStatus(CellularHandle, &simStatus);

            if ((cellularStatus == CELLULAR_SUCCESS) &&
                ((simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED) &&
                 (simStatus.simCardLockState == CELLULAR_SIM_CARD_READY)))
            {
                FreeRTOS_printf((">>>  Cellular SIM okay  <<<\r\n"));
                break;
            }
            else
            {
                FreeRTOS_printf((">>>  Cellular SIM card state %d, Lock State  <<<\r\n",
                                 simStatus.simCardState,
                                 simStatus.simCardLockState));
            }

            vTaskDelay(pdMS_TO_TICKS(CELLULAR_SIM_CARD_WAIT_INTERVAL_MS));
        }
    }

    /* Setup the PDN config. */
    if (cellularStatus == CELLULAR_SUCCESS)
    {
        cellularStatus = Cellular_SetPdnConfig(CellularHandle, testCELLULAR_PDN_CONTEXT_ID, &pdnConfig);
    }
    else
    {
        /* TODO : consider to loop here? */
        FreeRTOS_printf((">>>  Cellular SIM failure  <<<\r\n"));
    }

    /* Rescan network. */
    if (cellularStatus == CELLULAR_SUCCESS)
    {
        cellularStatus = Cellular_RfOff(CellularHandle);
    }
    if (cellularStatus == CELLULAR_SUCCESS)
    {
        cellularStatus = Cellular_RfOn(CellularHandle);
    }

    /* Get service status. */
    if (cellularStatus == CELLULAR_SUCCESS)
    {
        while (timeoutCount < timeoutCountLimit)
        {
            cellularStatus = Cellular_GetServiceStatus(CellularHandle, &serviceStatus);

            if ((cellularStatus == CELLULAR_SUCCESS) &&
                ((serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME) ||
                 (serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING)))
            {
                FreeRTOS_printf((">>>  Cellular module registered  <<<\r\n"));
                break;
            }
            else
            {
                FreeRTOS_printf((">>>  Cellular GetServiceStatus failed %d, ps registration status %d  <<<\r\n",
                                 cellularStatus, serviceStatus.psRegistrationStatus));
            }

            timeoutCount++;

            if (timeoutCount >= timeoutCountLimit)
            {
                /* TODO : consider to loop here? */
                FreeRTOS_printf((">>>  Cellular module can't be registered  <<<\r\n"));
            }

            vTaskDelay(pdMS_TO_TICKS(CELLULAR_SIM_CARD_WAIT_INTERVAL_MS * 2));
        }
    }

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        cellularStatus = Cellular_ActivatePdn(CellularHandle, CellularSocketPdnContextId);
    }

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        cellularStatus = Cellular_GetIPAddress(CellularHandle, CellularSocketPdnContextId, localIP, sizeof(localIP));
    }

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        cellularStatus = Cellular_GetPdnStatus(CellularHandle, &PdnStatusBuffers, CellularSocketPdnContextId, &NumStatus);
    }

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        cellularStatus = Cellular_SetDns(CellularHandle, CellularSocketPdnContextId, testCELLULAR_DNS_SERVER_ADDRESS);
    }

    if ((cellularStatus == CELLULAR_SUCCESS) && (PdnStatusBuffers.state == 1))
    {
        cellularRet = true;
    }
    else
    {
        cellularRet = false;
    }

    return cellularRet;
}