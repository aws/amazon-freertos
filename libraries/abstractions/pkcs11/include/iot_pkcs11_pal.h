/*
 * FreeRTOS PKCS #11 V2.0.3
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

#ifndef IOT_PKCS11_PAL
#define IOT_PKCS11_PAL
/*-----------------------------------------------------------*/
/*------------ Port Specific File Access API ----------------*/
/*--------- See iot_pkcs11_pal.c for definitions ------------*/
/*-----------------------------------------------------------*/

/*------------------------ PKCS #11 pal functions -------------------------*/

/**
 * @functions_page{pkcs11_pal,PKCS #11 PAL, PKCS #11 PAL}
 * @functions_brief{PKCS #11 PAL Layer}
 * - @function_name{pkcs11_pal_function_saveobject}
 * @function_brief{pkcs11_pal_function_saveobject}
 * - @function_name{pkcs11_pal_function_findobject}
 * @function_brief{pkcs11_pal_function_findobject}
 * - @function_name{pkcs11_pal_function_getobjectvalue}
 * @function_brief{pkcs11_pal_function_getobjectvalue}
 * - @function_name{pkcs11_pal_function_getobjectvaluecleanup}
 * @function_brief{pkcs11_pal_function_getobjectvaluecleanup}
 */

/**
 * @function_page{PKCS11_PAL_SaveObject,pkcs11_pal,saveobject}
 * @function_snippet{pkcs11_pal,saveobject,this}
 * @copydoc PKCS11_PAL_SaveObject
 * @function_page{PKCS11_PAL_FindObject,pkcs11_pal,findobject}
 * @function_snippet{pkcs11_pal,findobject,this}
 * @copydoc PKCS11_PAL_FindObject
 * @function_page{PKCS11_PAL_GetObjectValue,pkcs11_pal,getobjectvalue}
 * @function_snippet{pkcs11_pal,getobjectvalue,this}
 * @copydoc PKCS11_PAL_GetObjectValue
 * @function_page{PKCS11_PAL_GetObjectValueCleanup,pkcs11_pal,getobjectvaluecleanup}
 * @function_snippet{pkcs11_pal,getobjectvaluecleanup,this}
 * @copydoc PKCS11_PAL_GetObjectValueCleanup
 */

/**
 *  @brief Save an object to storage.
 */
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize );
 /* @[declare pkcs11_pal_saveobject] */

/**
 * @brief Delete an object from NVM.
 */
 /* @[declare pkcs11_pal_destroyobject] */
CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle );
 /* @[declare pkcs11_pal_destroyobject] */

/**
 *   @brief Look up an object handle using it's attributes
 */
 /* @[declare pkcs11_pal_findobject] */
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
                                        uint8_t usLength );
 /* @[declare pkcs11_pal_findobject] */


/**
 *   @brief Get the value of an object.
 *   @note  Buffers may be allocated by this call, and should be
 *          freed up by calling PKCS11_PAL_GetObjectValueCleanup().
 */
 /* @[declare pkcs11_pal_getobjectvalue] */
BaseType_t PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                      uint8_t ** ppucData,
                                      uint32_t * pulDataSize,
                                      CK_BBOOL * xIsPrivate );
 /* @[declare pkcs11_pal_getobjectvalue] */

/**
 *  @brief Free the buffer allocated in PKCS11_PAL_GetObjectValue() (see PAL).
 */
 /* @[declare pkcs11_pal_getobjectvaluecleanup] */
void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucBuffer,
                                       uint32_t ulBufferSize );
 /* @[declare pkcs11_pal_getobjectvaluecleanup] */

#endif /* IOT_PKCS11_PAL include guard. */
