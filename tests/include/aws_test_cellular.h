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

#ifndef __AWS_TEST_CELLULAR_H__
#define __AWS_TEST_CELLULAR_H__

/* The number of times to loop in the CELLULARConnectionLoop test. */
#define testCELLULARCONNECTION_LOOP_TIMES                      ( 3U )

/* APN for the test network. */
#define testCELLULAR_APN                                       ""

/* PDN context id for cellular network. */
#define testCELLULAR_PDN_CONTEXT_ID                            ( CELLULAR_PDN_CONTEXT_ID_MIN )

/* DNS server address. */
#define testCELLULAR_DNS_SERVER_ADDRESS                        "8.8.8.8"

/* Host name to resolve. The host name should only has one IP address. */
#define testCELLULAR_HOST_NAME                                 "ec2-34-209-68-37.us-west-2.compute.amazonaws.com"

/* Host name resolved address. The resolved address should be the IP address of
 * testCELLULAR_HOST_NAME. */
#define testCELLULAR_HOST_NAME_ADDRESS                         "34.209.68.37"

/* Repeat echo server address for EDRX echo times test. */
#define testCELLULAR_EDRX_ECHO_SERVER_ADDRESS                  "34.209.68.37"

/* Repeat echo server port for EDRX echo times test. */
#define testCELLULAR_EDRX_ECHO_SERVER_PORT                     ( 8885 )

/* Repeat echo server send interfal for EDRX echo times test. */
#define testCELLULAR_EDRX_ECHO_SERVER_DATA_SEND_INTERVAL_MS    ( 30000 )

/*
 * 2 GSM
 * 3 UTRAN
 * 4 LTE Cat M1
 * 5 LTE Cat NB1
 */
#define testCELLULAR_EDRX_RAT                                  ( 4 )


#endif /* ifndef __AWS_TEST_CELLULAR_H__ */
