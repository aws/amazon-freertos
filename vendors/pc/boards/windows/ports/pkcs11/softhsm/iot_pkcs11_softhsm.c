/*
 * Amazon FreeRTOS PKCS#11 V1.0.8
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
 * @file iot_pkcs11_softhsm.c
 * @brief Wrapper module for using https://github.com/opendnssec/SoftHSMv2 with
 * Amazon FreeRTOS. This file deviates from the FreeRTOS style standard for
 * some function names and data types in order to maintain compliance with
 * the PKCS #11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "iot_pkcs11.h"

typedef CK_RV ( * PFN_GET_FUNCTION_LIST )( CK_FUNCTION_LIST_PTR_PTR ppFunctionList );

/**
 * @brief Obtain a pointer to the PKCS #11 module function pointer structure.
 * All other PKCS #11 functions must be invoked using the pointers in the
 * returned structure.
 *
 * \warn Do not overwrite the function list.
 *
 * \param[in] ppxFunctionList       Pointer to the location where the function
 *                                  list pointer will be placed.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = 0;
    HMODULE hModule = NULL;
    PFN_GET_FUNCTION_LIST pfnGetFunctionList = NULL;

    /* Check the caller argument.*/
    if( NULL == ppxFunctionList )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* Load the PKCS #11 module DLL. This will fail if the binary isn't in the
     * execution path of the simulator. */
    if( 0 == xResult )
    {
        hModule = LoadLibraryA( "softhsm2.dll" );

        if( NULL == hModule )
        {
            xResult = GetLastError();
        }
    }

    /* Use the Windows loader to get the DLL export routine for querying the
     * PKCS #11 function list for this module. */
    if( 0 == xResult )
    {
        pfnGetFunctionList = ( PFN_GET_FUNCTION_LIST ) GetProcAddress( hModule, "C_GetFunctionList" );

        if( NULL == pfnGetFunctionList )
        {
            xResult = GetLastError();
        }
    }

    /* Call the first routine. This will fail on WIN32 if structure packing
     * isn't set to one-byte alignment. */
    if( 0 == xResult )
    {
        xResult = pfnGetFunctionList( ppxFunctionList );
    }

    return xResult;
}
