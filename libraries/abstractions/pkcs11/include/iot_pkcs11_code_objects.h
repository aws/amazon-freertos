/*
 * Amazon FreeRTOS PKCS #11 V2.0.0
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

#ifndef IOT_PKCS11_CODE_OBJECTS
#define IOT_PKCS11_CODE_OBJECTS

CK_OBJECT_HANDLE PKCS11_Code_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                         uint8_t * pucData,
                                         uint32_t ulDataSize );

CK_OBJECT_HANDLE PKCS11_Code_FindObject( uint8_t * pLabel,
                                         uint8_t usLength );

CK_RV PKCS11_Code_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                  uint8_t ** ppucData,
                                  uint32_t * pulDataSize,
                                  CK_BBOOL * pIsPrivate );

void PKCS11_Code_GetObjectValueCleanup( uint8_t * pucData,
                                        uint32_t ulDataSize );

CK_RV PKCS11_Code_DestroyObject( CK_OBJECT_HANDLE xHandle );

#endif /* ifndef IOT_PKCS11_CODE_OBJECTS */
