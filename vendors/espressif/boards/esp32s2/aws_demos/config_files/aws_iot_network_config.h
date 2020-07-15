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
 * @file aws_iot_network_config.h
 * @brief Configuration file which enables different network types.
 */
#ifndef AWS_IOT_NETWORK_CONFIG_H_
#define AWS_IOT_NETWORK_CONFIG_H_

/**
 * @brief Configuration flag used to specify all supported network types by the board.
 *
 * The configuration is fixed per board and should never be changed.
 * More than one network interfaces can be enabled by using 'OR' operation with flags for
 * each network types supported. Flags for all supported network types can be found
 * in "aws_iot_network.h"
 */

#define configSUPPORTED_NETWORKS    ( AWSIOT_NETWORK_TYPE_WIFI )

/**
 * @brief Configuration flag which is used to enable one or more network interfaces for a board.
 *
 * The configuration can be changed any time to keep one or more network enabled or disabled.
 * More than one network interfaces can be enabled by using 'OR' operation with flags for
 * each network types supported. Flags for all supported network types can be found
 * in "aws_iot_network.h"
 *
 */

#define configENABLED_NETWORKS      ( AWSIOT_NETWORK_TYPE_WIFI )

#endif /* CONFIG_FILES_AWS_IOT_NETWORK_CONFIG_H_ */
