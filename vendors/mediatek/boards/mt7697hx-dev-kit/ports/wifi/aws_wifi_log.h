/*
 * Amazon FreeRTOS
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
 * @file aws_wifi_log.h
 * @brief Wi-Fi AWS Log Definition.
 */

#ifndef _AWS_WIFI_LOG_H_
#define _AWS_WIFI_LOG_H_

#if 0
#ifndef AWS_WIFI_LOGD
#define AWS_WIFI_LOGD(...) configPRINTF( (__VA_ARGS__) )
#endif

#ifndef AWS_WIFI_LOGI
#define AWS_WIFI_LOGI(...) configPRINTF( (__VA_ARGS__) )
#endif

#ifndef AWS_WIFI_LOGW
#define AWS_WIFI_LOGW(...) configPRINTF( (__VA_ARGS__) )
#endif

#ifndef AWS_WIFI_LOGE
#define AWS_WIFI_LOGE(...) configPRINTF( (__VA_ARGS__) )
#endif

#ifndef AWS_WIFI_LOGF
#define AWS_WIFI_LOGF(...) configPRINTF( (__VA_ARGS__) )
#endif
#else

#ifndef AWS_WIFI_LOGD
#define AWS_WIFI_LOGD(...) 
#endif

#ifndef AWS_WIFI_LOGI
#define AWS_WIFI_LOGI(...) 
#endif

#ifndef AWS_WIFI_LOGW
#define AWS_WIFI_LOGW(...) 
#endif

#ifndef AWS_WIFI_LOGE
#define AWS_WIFI_LOGE(...) 
#endif

#ifndef AWS_WIFI_LOGF
#define AWS_WIFI_LOGF(...) 
#endif

#endif

#endif

