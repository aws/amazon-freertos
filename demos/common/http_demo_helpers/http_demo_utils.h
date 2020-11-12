/*
 * FreeRTOS V202011.00
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
 */

#ifndef HTTP_DEMO_UTILS_H
#define HTTP_DEMO_UTILS_H

/* Standard includes. */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/* Kernel includes. */
#include "FreeRTOS.h"

/* HTTP API header. */
#include "core_http_client.h"

/**
 * @brief Function pointer for establishing connection to a server.
 *
 * @param[out] pxNetworkContext Implementation-defined network context.
 *
 * @return pdFAIL on failure; pdPASS on successful connection.
 */
typedef BaseType_t ( * TransportConnect_t )( NetworkContext_t * pxNetworkContext );

/**
 * @brief Connect to a server with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[in] connectFunction Function pointer for establishing connection to a server.
 * @param[out] pxNetworkContext Implementation-defined network context.
 *
 * @return pdFAIL on failure; pdPASS on successful connection.
 */
BaseType_t connectToServerWithBackoffRetries( TransportConnect_t connectFunction,
                                              NetworkContext_t * pxNetworkContext );

/**
 * @brief Retrieve the path from the input URL.
 *
 * This function retrieves the location and length of the path from within the
 * input the URL. The query is not included in the length returned.
 *
 * The URL MUST start with "http://" or "https://" to find the path.
 *
 * For example, if pUrl is:
 * "https://www.somewebsite.com/path/to/item.txt?optionalquery=stuff"
 *
 * Then pPath and pPathLen will be the following:
 * *pPath = "/path/to/item.txt?optionalquery=stuff"
 * *pPathLen = 17
 *
 * @param[in] pUrl URL string to parse.
 * @param[in] urlLen The length of the URL string input.
 * @param[out] pPath pointer within input url that the path starts at.
 * @param[out] pPathLen Length of the path.
 *
 * @return The status of the parsing attempt:
 * HTTPSuccess if the path was successfully parsed,
 * HTTPParserInternalError if there was an error parsing the URL,
 * or HTTPNoResponse if the path was not found.
 */
HTTPStatus_t getUrlPath( const char * pUrl,
                         size_t urlLen,
                         const char ** pPath,
                         size_t * pPathLen );

/**
 * @brief Retrieve the Address from the input URL.
 *
 * This function retrieves the location and length of the address from within
 * the input URL. The path and query are not included in the length returned.
 *
 * The URL MUST start with "http://" or "https://" to find the address.
 *
 * For example, if pUrl is:
 * "https://www.somewebsite.com/path/to/item.txt?optionalquery=stuff"
 *
 * Then pAddress and pAddressLen will be the following:
 * *pAddress = "www.somewebsite.com/path/to/item.txt?optionalquery=stuff"
 * *pAddressLen = 19
 *
 * @param[in] pUrl URL string to parse.
 * @param[in] urlLen The length of the URL string input.
 * @param[out] pAddress pointer within input url that the address starts at.
 * @param[out] pAddressLen Length of the address.
 *
 * @return The status of the parsing attempt:
 * HTTPSuccess if the path was successfully parsed,
 * HTTPParserInternalError if there was an error parsing the URL,
 * or HTTPNoResponse if the path was not found.
 */
HTTPStatus_t getUrlAddress( const char * pUrl,
                            size_t urlLen,
                            const char ** pAddress,
                            size_t * pAddressLen );

#endif /* ifndef HTTP_DEMO_UTILS_H */
