/*
 * FreeRTOS V1.4.8
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
 * @file aws_ota_agent_config.h
 * @brief OTA user configurable settings.
 */

#ifndef _AWS_OTA_AGENT_CONFIG_H_
#define _AWS_OTA_AGENT_CONFIG_H_

/**
 * @brief The number of words allocated to the stack for the OTA agent.
 */
#define otaconfigSTACK_SIZE                     630U    /* FIX ME. */

/**
 * @brief Log base 2 of the size of the file data block message (excluding the header).
 *
 * 10 bits yields a data block size of 1KB.
 */
#define otaconfigLOG2_FILE_BLOCK_SIZE           10UL    /* FIX ME. */

/**
 * @brief Milliseconds to wait for the self test phase to succeed before we force reset.
 */
#define otaconfigSELF_TEST_RESPONSE_WAIT_MS     16000U  /* FIX ME. */

/**
 * @brief Milliseconds to wait before requesting data blocks from the OTA service if nothing is happening.
 *
 * The wait timer is reset whenever a data block is received from the OTA service so we will only send
 * the request message after being idle for this amount of time.
 */
#define otaconfigFILE_REQUEST_WAIT_MS           2500U   /* FIX ME. */

/**
 * @brief The OTA agent task priority. Normally it runs at a low priority.
 */
#define otaconfigAGENT_PRIORITY                 tskIDLE_PRIORITY    /* FIX ME. */

/**
 * @brief The maximum allowed length of the thing name used by the OTA agent.
 *
 * AWS IoT requires Thing names to be unique for each device that connects to the broker.
 * Likewise, the OTA agent requires the developer to construct and pass in the Thing name when
 * initializing the OTA agent. The agent uses this size to allocate static storage for the
 * Thing name used in all OTA base topics. Namely $aws/things/<thingName>
 */
#define otaconfigMAX_THINGNAME_LEN              64  /* FIX ME. */

/**
 * @brief The maximum number of data blocks requested per request from the OTA
 * streaming service.
 *
 * This configuration parameter is sent with data requests and represents the
 * maximum number of data blocks the service will send in a single response.
 * The amount of data requested and therefore received in a single response
 * (in bytes) is equal to:
 *
 * (otaconfigMAX_NUM_BLOCKS_REQUEST * ( pow(2,otaconfigLOG2_FILE_BLOCK_SIZE) )
 *
 * @note If the device has a incoming TLS I/O buffer set to be less than the
 * standard default size of 16KB, then the amount of bytes being requested has
 * to be less than the the size of the incoming TLS I/O buffer.
 * <br><br>
 *
 * For example, if the device has otaconfigLOG2_FILE_BLOCK_SIZE set to '12',
 * then each data block will be 4096 bytes large. If the device also has a
 * incoming TLS I/O bufferthat is 8KB large, then the
 * otaconfigMAX_NUM_BLOCKS_REQUEST must be set to '1' so that the data received
 * in one response doesn't surpass the size of the incoming TLS I/O buffer.
 *
 * @note If the device has a incoming TLS I/O buffer set to be equal to the
 * standard default size of 16Kb, then the maximum limit for this must be
 * calculated from the maximum data response limit (128 KB from service)
 * divided by the block size. For example if block size is set as 1 KB then the
 * maximum number of data blocks that we can request is 128/1 = 128 blocks.
 * Configure this parameter to this maximum limit or lower based on how many
 * data blocks response is expected for each data requests.
 *
 * @note This must be set to a value larger than zero.
 */
#define otaconfigMAX_NUM_BLOCKS_REQUEST         4U

#endif /* _AWS_OTA_AGENT_CONFIG_H_ */
