/*
 * FreeRTOS V202010.00
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 */

#ifndef HTTP_DEMO_MUTUAL_AUTH_CONFIG_H
#define HTTP_DEMO_MUTUAL_AUTH_CONFIG_H

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

/* Include header that defines log levels. */
#include "logging_levels.h"

/* Logging configuration for the demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "HTTPDemo"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

/**
 * To use this demo, please configure the client's certificate and private key
 * in demos/include/aws_clientcredential_keys.h.
 *
 * To connect to AWS IoT, refer to the AWS documentation below for details
 * regarding client authentication.
 * https://docs.aws.amazon.com/iot/latest/developerguide/client-authentication.html
 */

/**
 * @brief Your AWS IoT Core endpoint.
 *
 * @note Your AWS IoT Core endpoint can be found in the AWS IoT console under
 * Settings/Custom Endpoint, or using the describe-endpoint REST API (with AWS
 * CLI command line tool).
 *
 * #define democonfigAWS_IOT_ENDPOINT                 "...insert here..."
 */

/**
 * @brief AWS IoT Core server port number for HTTPS connections.
 *
 * For this demo, an X.509 certificate is used to verify the client.
 *
 * @note Port 443 requires use of the ALPN TLS extension with the ALPN protocol
 * name being x-amzn-http-ca. When using port 8443, ALPN is not required.
 */
#ifndef democonfigAWS_HTTP_PORT
    #define democonfigAWS_HTTP_PORT    8443
#endif

/**
 * @brief Server's root CA certificate for TLS authentication to AWS IoT Core.
 *
 * This certificate is used to identify the AWS IoT server and is publicly
 * available. Refer to the AWS documentation available in the link below for
 * information about server root CAs.
 * https://docs.aws.amazon.com/iot/latest/developerguide/server-authentication.html#server-authentication-certs
 *
 * @note The TI C3220 Launchpad board requires that the root CA have its
 * certificate self-signed. As specified in the link above, the Amazon Root CAs
 * are cross-signed by the Starfield Root CA. Thus, ONLY the Starfield Root CA
 * can be used to connect to the ATS endpoints on AWS IoT, for the TI board.
 *
 * @note This certificate should be PEM-encoded.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define democonfigROOT_CA_PEM                       tlsSTARFIELD_ROOT_CERTIFICATE_PEM

/**
 * @brief This endpoint can be used to publish a message to a topic named topic
 * on AWS IoT Core.
 *
 * Each client certificate has an associated policy document that must be
 * configured to support the path below for this demo to work correctly.
 *
 * @note QoS=1 implies the message is delivered to all subscribers of the topic
 * at least once.
 */
#define democonfigPOST_PATH                         "/topics/topic?qos=1"

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 5000 )

/**
 * @brief The length in bytes of the user buffer.
 */
#define democonfigUSER_BUFFER_LENGTH                ( 2048 )

/**
 * @brief Request body to send for POST requests in this demo.
 */
#define democonfigREQUEST_BODY                      "{ \"message\": \"Hello, world\" }"

#endif /* ifndef HTTP_DEMO_MUTUAL_AUTH_CONFIG_H */
