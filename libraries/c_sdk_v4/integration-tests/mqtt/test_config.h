/*
 * AWS IoT Device SDK for Embedded C V202009.00
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

#ifndef TEST_CONFIG_H_
#define TEST_CONFIG_H_

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

#include "logging_levels.h"

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "TEST"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"

/************ End of logging configuration ****************/

/**
 * @brief MQTT server host name.
 *
 * #define BROKER_ENDPOINT               "...insert here..."
 */

/**
 * @brief MQTT server port number.
 *
 * In general, port 8883 is for secured MQTT connections.
 */
#define BROKER_PORT    ( 8883 )

/**
 * @brief Path of the file containing the client certificate.
 *
 * #define CLIENT_CERT_PATH    "...insert here..."
 */

/**
 * @brief Path of the file containing the client's private key.
 *
 * #define CLIENT_PRIVATE_KEY_PATH    "...insert here..."
 */

/**
 * @brief Path of the file containing the server's root CA certificate.
 *
 * This certificate should be PEM-encoded.
 *
 * * #define SERVER_ROOT_CA_CERT_PATH    "...insert here..."
 */

#endif /* ifndef TEST_CONFIG_H_ */
