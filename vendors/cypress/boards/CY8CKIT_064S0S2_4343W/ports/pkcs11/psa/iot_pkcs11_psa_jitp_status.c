/*
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file iot_pkcs11_psa_jitp_status.c
 * @brief PSA based PKCS#11 implementation.
 */

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* PKCS#11 includes. */
#include "iot_pkcs11_config.h"
#include "iot_pkcs11_psa_jitp_status.h"

/* PSA includes. */
#include "psa/protected_storage.h"

#define PKCS11_PRINT( X )    vLoggingPrintf X

#if pkcs11configVENDOR_DEVICE_CERTIFICATE_SUPPORTED
#define AWS_IOT_JITP_STATUS  "_AWSIoT_JITP_"

/**
 * @brief Check if JITP is done.
 *
 * @return 1 - if AWS IoT JITP is completed.
 * `       0 -  otherwise.
 */
int GetAwsIoTJITPStatus( void )
{
    psa_status_t status;
    psa_storage_uid_t uid = pkcs11configVENDOR_JITP_STATUS_UID;
    char buff[sizeof(AWS_IOT_JITP_STATUS) + 1];
    size_t buff_size = sizeof(AWS_IOT_JITP_STATUS) + 1;

    status = psa_ps_get( uid, 0, buff_size, buff, &buff_size);
    if ((status == PSA_SUCCESS) &&
        (strncmp(buff, AWS_IOT_JITP_STATUS, buff_size) == 0)) {
       return 1;
    }
    return 0;
}

/**
 * @brief Set device certificate regsiter status
 *
 * @return  0 - if successfully set
 *         -1 -`otherwise.
 */
int SetAwsIoTJITPStatus( void )
{
    psa_status_t status;
    psa_storage_uid_t uid = pkcs11configVENDOR_JITP_STATUS_UID;

    status = psa_ps_set( uid,
                         sizeof(AWS_IOT_JITP_STATUS),
                         AWS_IOT_JITP_STATUS,
                         PSA_STORAGE_FLAG_NONE);
    if (status == PSA_SUCCESS)
       return -1;

    return 0;
}

#endif /* pkcs11configVENDOR_DEVICE_CERTIFICATE_SUPPORTED */
