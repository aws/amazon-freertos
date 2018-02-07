/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*
 * aws_rsprintf.h
 *
 * Author: ricyoung
 */


#ifndef AWS_RSPRINTF_H_
#define AWS_RSPRINTF_H_

#include <stdint.h>
/* #include <unistd.h> */
#include <stdarg.h>
#include "aws_ota_types.h"

#define kH32_MaxPlaces 8							/* Maximum number of output digits of the szH32 function. */
#define kU32_MaxPlaces 10							/* Maximum number of output digits of the szU32 function. */
#define kS32_MaxPlaces 11							/* Maximum number of output digits of the szS32 function (includes space for minus). */
#define kH32_MaxTextSize (kH32_MaxPlaces + 1)		/* Maximum output size of the szH32 function including zero terminator. */
#define kU32_MaxTextSize (kU32_MaxPlaces + 1)		/* Maximum output size of the szU32 function including zero terminator. */
#define kS32_MaxTextSize (kS32_MaxPlaces + 1)		/* Maximum output size of the szS32 function including zero terminator. */

char *szU32(char* dest, u32 val, u8 iFieldWidth);
char *szS32(char* dest, s32 val, u8 iFieldWidth);
char *szH32(char* dest, u32 val, u8 iFieldWidth);
u32 rsprintf(char *dest, const char *fmt, ...);
u32 rvsprintf(char *dest, const char *fmt, va_list va);
u32 cprintf(const char *fmt, ...);
char *stpcpy(register char *dest, register const char *src);

#endif /* AWS_RSPRINTF_H_ */
