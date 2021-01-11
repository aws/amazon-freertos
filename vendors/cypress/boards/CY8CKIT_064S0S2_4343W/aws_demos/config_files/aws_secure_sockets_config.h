/*
 * Amazon FreeRTOS V1.4.7
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_secure_sockets_config.h
 * @brief Secure sockets configuration options.
 */

#ifndef _AWS_SECURE_SOCKETS_CONFIG_H_
#define _AWS_SECURE_SOCKETS_CONFIG_H_

/**
 * @brief Byte order of the target MCU.
 *
 * Valid values are pdLITTLE_ENDIAN and pdBIG_ENDIAN.
 */
#define socketsconfigBYTE_ORDER              pdLITTLE_ENDIAN

/**
 * @brief Default socket send timeout.
 */
#define socketsconfigDEFAULT_SEND_TIMEOUT    ( 3000 )

/**
 * @brief Default socket receive timeout.
 */
#define socketsconfigDEFAULT_RECV_TIMEOUT    ( 3000 )

/**
 * @brief Default max socket number support
 */
#define socketsconfigDEFAULT_MAX_NUM_SECURE_SOCKETS     5

/**
 * @brief Stack depth for the task that runs the receive callback function
 *
 * When SOCKETS_SetSockOpt() is called with SOCKETS_SO_WAKEUP_CALLBACK and
 * a function pointer, a task is created to run the callback each time the
 * socket becomes ready.  This is the number of words (not bytes!) to allocate
 * for use as the task’s stack.
 */
#define socketsconfigRECEIVE_CALLBACK_TASK_STACK_DEPTH      512u

/**
 * @brief Enable metrics of secure socket.
 */
#define AWS_IOT_SECURE_SOCKETS_METRICS_ENABLED    ( 1 )

#endif /* _AWS_SECURE_SOCKETS_CONFIG_H_ */
