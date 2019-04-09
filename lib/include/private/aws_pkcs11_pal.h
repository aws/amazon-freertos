#ifndef AWS_PKCS11_PAL
#define AWS_PKCS11_PAL
/*-----------------------------------------------------------*/
/*------------ Port Specific File Access API ----------------*/
/*--------- See aws_pkcs11_pal.c for definitions ------------*/
/*-----------------------------------------------------------*/

/**
 *  @brief Save an object to storage.
 */
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize );

/**
 * @brief Delete an object from NVM.
 */
CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle );

/**
 *   @brief Look up an object handle using it's attributes
 */
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
                                        uint8_t usLength );

/**
 *   @brief Get the value of an object.
 *   @note  Buffers may be allocated by this call, and should be
 *          freed up by calling PKCS11_PAL_GetObjectValueCleanup().
 */
BaseType_t PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                      uint8_t ** ppucData,
                                      uint32_t * pulDataSize,
                                      CK_BBOOL * xIsPrivate );

/**
 *  @brief Free the buffer allocated in PKCS11_PAL_GetObjectValue() (see PAL).
 */
void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucBuffer,
                                       uint32_t ulBufferSize );

#endif /* AWS_PKCS11_PAL include guard. */
