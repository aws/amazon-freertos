/*
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2019-2020 Arm Limited. All Rights Reserved.
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
 */

#ifndef __IOT_PKCS11_PSA_JITP_STATUS_H__
#define __IOT_PKCS11_PSA_JITP_STATUS_H__

/**
 * @brief Check if JITP is done.
 *
 * @return 1 - if AWS IoT JITP is completed.
 * `       0 -  otherwise.
 */
int GetAwsIoTJITPStatus( void );

/**
 * @brief Set device certificate regsiter status
 *
 * @return  0 - if successfully set
 *         -1 -`otherwise.
 */
int SetAwsIoTJITPStatus( void );

#endif /* __IOT_PKCS11_PSA_JITP_STATUS_H__ */
