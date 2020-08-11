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

#ifndef _TEST_CELLULAR_API_H_
#define _TEST_CELLULAR_API_H_

/* Second set of valid CELLULAR credentials for testing the connection loop. */

/* The number of times to loop in the CELLULARConnectionLoop test. */
#define testCELLULARCONNECTION_LOOP_TIMES       3

/* Retry until SIM is ready. */
#define CELLULAR_MAX_SIM_RETRY                  ( 5U )

/* APN for the test network. */
#define testCELLULAR_APN                        ""
#define testINVALID_CELLULAR_APN                "VZWINTERNETVZWINTERNETVZWINTERNETVZWINTERNETVZWINTERNETVZWINTERN"
#define testHotSpotENABLE_CONFIGURE_AP_TESTS    0

/**
 * @brief the default Cellular comm interface in system.
 */
extern CellularCommInterface_t CellularCommInterface;

#endif /* ifndef _TEST_CELLULAR_API_H_ */
