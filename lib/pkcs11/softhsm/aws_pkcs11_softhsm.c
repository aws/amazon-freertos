/*
 * Amazon FreeRTOS mbedTLS-based PKCS#11 V1.0.3
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "aws_pkcs11_config.h"
#include "task.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"

#include <windows.h>


/* Threading mutex implementations for SoftHSM */
#include "mbedtls/threading.h"
#include "threading_alt.h"

/**
* @brief Implementation of mbedtls_mutex_init for thread-safety.
*
*/
void aws_mbedtls_mutex_init( mbedtls_threading_mutex_t * mutex )
{
    if ( mutex->is_valid == 0 )
    {
        mutex->mutex = xSemaphoreCreateMutex();

        if ( mutex->mutex != NULL )
        {
            mutex->is_valid = 1;
        }
        else
        {
            //PKCS11_PRINT( ("Failed to initialize mbedTLS mutex.\r\n") );
        }
    }
}

/**
* @brief Implementation of mbedtls_mutex_free for thread-safety.
*
*/
void aws_mbedtls_mutex_free( mbedtls_threading_mutex_t * mutex )
{
    if ( mutex->is_valid == 1 )
    {
        vSemaphoreDelete( mutex->mutex );
        mutex->is_valid = 0;
    }
}

/**
* @brief Implementation of mbedtls_mutex_lock for thread-safety.
*
* @return 0 if successful, MBEDTLS_ERR_THREADING_MUTEX_ERROR if timeout,
* MBEDTLS_ERR_THREADING_BAD_INPUT_DATA if the mutex is not valid.
*/
int aws_mbedtls_mutex_lock( mbedtls_threading_mutex_t * mutex )
{
    int ret = MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;

    if ( mutex->is_valid == 1 )
    {
        if ( xSemaphoreTake( mutex->mutex, portMAX_DELAY ) )
        {
            ret = 0;
        }
        else
        {
            ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
            //PKCS11_PRINT( ("Failed to obtain mbedTLS mutex.\r\n") );
        }
    }

    return ret;
}

/**
* @brief Implementation of mbedtls_mutex_unlock for thread-safety.
*
* @return 0 if successful, MBEDTLS_ERR_THREADING_MUTEX_ERROR if timeout,
* MBEDTLS_ERR_THREADING_BAD_INPUT_DATA if the mutex is not valid.
*/
int aws_mbedtls_mutex_unlock( mbedtls_threading_mutex_t * mutex )
{
    int ret = MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;

    if ( mutex->is_valid == 1 )
    {
        if ( xSemaphoreGive( mutex->mutex ) )
        {
            ret = 0;
        }
        else
        {
            ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
            //PKCS11_PRINT( ("Failed to unlock mbedTLS mutex.\r\n") );
        }
    }

    return ret;
}

typedef CK_RV(__stdcall * getFunctionListDll)(CK_FUNCTION_LIST_PTR_PTR);

CK_DEFINE_FUNCTION(CK_RV, C_GetFunctionList)(CK_FUNCTION_LIST_PTR_PTR ppxFunctionList)
{
    HINSTANCE hDll = LoadLibraryA("softhsm2.dll");

    if (!hDll)
    {
        return CKR_FUNCTION_FAILED;
    }

    getFunctionListDll pFunc = (getFunctionListDll)GetProcAddress(hDll, "C_GetFunctionList");

    if (!pFunc)
    {
        return CKR_FUNCTION_FAILED;
    }

    /* Ensure that the FreeRTOS heap is used. */
    CRYPTO_ConfigureHeap();

    /* Configure mbedtls to use FreeRTOS mutexes. */
    mbedtls_threading_set_alt( aws_mbedtls_mutex_init,
        aws_mbedtls_mutex_free,
        aws_mbedtls_mutex_lock,
        aws_mbedtls_mutex_unlock );

    return pFunc(ppxFunctionList);
}
