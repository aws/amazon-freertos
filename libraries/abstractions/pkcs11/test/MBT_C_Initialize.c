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


//------------------------------------------------------------------------------
//
// This code was auto-generated by a tool.
//
// Changes to this file may cause incorrect behavior and will be
// lost if the code is regenerated.
//
//-----------------------------------------------------------------------------

#include "MBT_C_Initialize.h"

void C_Initialize_normal_behavior()
{
    CK_VOID_PTR pInitArgs = NULL_PTR;

    CK_RV rv = pxGlobalFunctionList->C_Initialize(pInitArgs);

    TEST_ASSERT_EQUAL(CKR_OK, rv);
}

void C_Initialize_exceptional_behavior_0()
{
    CK_VOID_PTR pInitArgs = NULL_PTR;

    CK_RV rv = pxGlobalFunctionList->C_Initialize(pInitArgs);

    TEST_ASSERT_EQUAL(CKR_CRYPTOKI_ALREADY_INITIALIZED, rv);
}
