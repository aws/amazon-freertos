/*
 * Amazon FreeRTOS CELLULAR Preview Release
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

#ifndef __CELLULAR_CONFIG_DEFAULTS_H__
#define __CELLULAR_CONFIG_DEFAULTS_H__

#ifndef CELLULAR_MCC_MAX_SIZE
    #define CELLULAR_MCC_MAX_SIZE    ( 3U )
#endif

#ifndef CELLULAR_MNC_MAX_SIZE
    #define CELLULAR_MNC_MAX_SIZE    ( 3U )
#endif

#ifndef CELLULAR_ICCID_MAX_SIZE
    #define CELLULAR_ICCID_MAX_SIZE    ( 20U )
#endif

#ifndef CELLULAR_IMSI_MAX_SIZE
    #define CELLULAR_IMSI_MAX_SIZE    ( 15U )
#endif

#ifndef CELLULAR_FW_VERSION_MAX_SIZE
    #define CELLULAR_FW_VERSION_MAX_SIZE    ( 32U )
#endif

#ifndef CELLULAR_HW_VERSION_MAX_SIZE
    #define CELLULAR_HW_VERSION_MAX_SIZE    ( 12U )
#endif

#ifndef CELLULAR_SERIAL_NUM_MAX_SIZE
    #define CELLULAR_SERIAL_NUM_MAX_SIZE    ( 12U )
#endif

#ifndef CELLULAR_IMEI_MAX_SIZE
    #define CELLULAR_IMEI_MAX_SIZE    ( 15U )
#endif

#ifndef CELLULAR_NETWORK_NAME_MAX_SIZE
    #define CELLULAR_NETWORK_NAME_MAX_SIZE    ( 32U )
#endif

#ifndef CELLULAR_APN_MAX_SIZE
    #define CELLULAR_APN_MAX_SIZE    ( 64U )
#endif

#ifndef CELLULAR_PDN_USERNAME_MAX_SIZE
    #define CELLULAR_PDN_USERNAME_MAX_SIZE    ( 32U )
#endif

#ifndef CELLULAR_PDN_PASSWORD_MAX_SIZE
    #define CELLULAR_PDN_PASSWORD_MAX_SIZE    ( 32u )
#endif

#ifndef CELLULAR_IP_ADDRESS_MAX_SIZE
    #define CELLULAR_IP_ADDRESS_MAX_SIZE    ( 40U )
#endif

#ifndef CELLULAR_AT_CMD_MAX_SIZE
    #define CELLULAR_AT_CMD_MAX_SIZE    ( 200U )
#endif

#ifndef CELLULAR_NUM_SOCKET_MAX
    #define CELLULAR_NUM_SOCKET_MAX    ( 12U )
#endif

#ifndef CELLULAR_MANUFACTURE_ID_MAX_SIZE
    #define CELLULAR_MANUFACTURE_ID_MAX_SIZE    ( 20U )
#endif

#ifndef CELLULAR_MODEL_ID_MAX_SIZE
    #define CELLULAR_MODEL_ID_MAX_SIZE    ( 10U )
#endif

#ifndef CELLULAR_EDRX_LIST_MAX_SIZE
    #define CELLULAR_EDRX_LIST_MAX_SIZE    ( 4U )
#endif

#ifndef CELLULAR_PDN_CONTEXT_ID_MIN
    #define CELLULAR_PDN_CONTEXT_ID_MIN    ( 1U )
#endif

#ifndef CELLULAR_PDN_CONTEXT_ID_MAX
    #define CELLULAR_PDN_CONTEXT_ID_MAX    ( 16U )
#endif

#ifndef CELLULAR_MAX_RAT_PRIORITY_COUNT
    #define CELLULAR_MAX_RAT_PRIORITY_COUNT    ( 3U )
#endif

#ifndef CELLULAR_MAX_SEND_DATA_LEN
    #define CELLULAR_MAX_SEND_DATA_LEN    ( 1460U )
#endif

#ifndef CELLULAR_MAX_RECV_DATA_LEN
    #define CELLULAR_MAX_RECV_DATA_LEN    ( 1500U )
#endif

#ifndef CELLULAR_SUPPORT_GETHOSTBYNAME
    #define CELLULAR_SUPPORT_GETHOSTBYNAME    ( 1U )
#endif

#ifndef CELLULAR_COMM_IF_SEND_TIMEOUT_MS
    #define CELLULAR_COMM_IF_SEND_TIMEOUT_MS    ( 1000U )
#endif

#ifndef CELLULAR_COMM_IF_RECV_TIMEOUT_MS
    #define CELLULAR_COMM_IF_RECV_TIMEOUT_MS    ( 1000U )
#endif

#endif /* __CELLULAR_CONFIG_DEFAULTS_H__ */
