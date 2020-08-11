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

#ifndef __AWS_CELLULAR_DEMO__H__
#define __AWS_CELLULAR_DEMO__H__

/*
 * @brief Access Point Name (APN) for your cellular network.
 * @todo set the corresponding APN according to your network provider.
 */
#define configCELLULAR_APN                   ""

/*
 * @brief PDN context id for cellular network.
 */
#define configCELLULAR_PDN_CONTEXT_ID        ( CELLULAR_PDN_CONTEXT_ID_MIN )

/*
 * @brief PDN connect timeout.
 */
#define confgCELLULAR_PDN_CONNECT_TIMEOUT    ( 20000UL )

/*
 * @brief DNS server address for cellular network socket service.
 * @todo Set the preferred DNS server address.
 */
#define configCELLULAR_DNS_SERVER            ""

/**
 * @brief the default Cellular comm interface in system.
 */
extern CellularCommInterface_t CellularCommInterface;

#endif /* ifndef __AWS_CELLULAR_DEMO__H__ */
