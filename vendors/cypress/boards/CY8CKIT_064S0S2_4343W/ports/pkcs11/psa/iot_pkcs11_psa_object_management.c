/*
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2019 Arm Limited. All Rights Reserved.
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

/*
 * This file is derivative of amazon-freertos\libraries\3rdparty\mbedtls\library\
 * pkparse.c(amazon-freertos commit 74875b1d2)
 */

/**
 * @file iot_pkcs11_psa_object_management.c
 * @brief Functions of managing the objects stored in TF-M.
 */

#include <string.h>
#include "iot_pkcs11_psa_object_management.h"
#include "iot_pkcs11_psa_input_format.h"

P11KeyConfig_t P11KeyConfig =
{.uxDeviceCertificate = 5U,
 .uxJitpCertificate = 6U,
 .uxRootCertificate = 7U,
};

/**
* @brief Writes a file to local storage.
*
* Port-specific file write for crytographic information.
*
* @param[in] pxClass	   Class of the object to be saved.
* @param[in] pxLabel	   Label of the object to be saved.
* @param[in] pucData	   Data buffer to be written to file
* @param[in] ulDataSize	Size (in bytes) of data to be saved.
* @param[in] pvContext	 Context of the key.
*
* @return The file handle of the object that was stored.
*/
CK_OBJECT_HANDLE PKCS11_PSA_SaveObject( CK_ATTRIBUTE_PTR pxClass,
	CK_ATTRIBUTE_PTR pxLabel,
	uint8_t * pucData,
	uint32_t ulDataSize,
	mbedtls_pk_context *pvContext )
{
	CK_OBJECT_HANDLE xHandle = eInvalidHandle;
	psa_key_handle_t uxKeyHandle;
	CK_ULONG uxKeyType;
	psa_key_policy_t policy = psa_key_policy_init();
	psa_algorithm_t uxAlgorithm;
	CK_RV xBytesWritten = 0;
	CK_RV xReturn;
	uint32_t ulPresentMark = ( pkcs11OBJECT_PRESENT_MAGIC | ( ulDataSize ) );
	uint8_t * pemBuffer;
	size_t pemLength;
	const psa_ps_create_flags_t uxFlags = PSA_PS_FLAG_NONE;
	psa_ps_status_t uxStatus;
	psa_ecc_curve_t curve_id;
	const mbedtls_ecp_keypair *ec;
	unsigned char pcPrivateKeyRaw[MBEDTLS_ECP_MAX_BYTES];
	size_t xPrivateKeySizeRaw = 0;
	unsigned char *pcPublicKeyUncompressedData = NULL;
	size_t xPublicKeySizeUncompressed = 0;
	uint8_t * pucKeyData = NULL;
	uint32_t ulKeyDataSize = 0;
	mbedtls_pk_type_t uxPrivateKeyTypePKCS11 = 0;

	if( ulDataSize <= pkcs11OBJECT_MAX_SIZE )
	{
		/*
		 * write client certificate.
		 */
		if( ( *( ( uint32_t * )pxClass->pValue ) == CKO_CERTIFICATE ) &&
			( strcmp( pxLabel->pValue,
					pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) == 0 ) )
		{
			uxStatus = psa_ps_set( P11KeyConfig.uxDeviceCertificate,
								   ulDataSize, pucData, uxFlags );

			if( uxStatus == PSA_PS_SUCCESS )
			{
				xHandle = eAwsDeviceCertificate;

				/*change sst written mark'*/
				P11KeyConfig.ulDeviceCertificateMark = ulPresentMark;
			}
		}

		/*
		 * write Jitp certificate.
		 */
		else if( ( *( ( uint32_t * )pxClass->pValue ) == CKO_CERTIFICATE ) &&
			( strcmp( pxLabel->pValue,
					pkcs11configLABEL_JITP_CERTIFICATE ) == 0 ) )
		{
			uxStatus = psa_ps_set( P11KeyConfig.uxJitpCertificate,
								   ulDataSize, pucData, uxFlags);

			if( uxStatus == PSA_PS_SUCCESS )
			{
				xHandle = eAwsDeviceCertificate;

				/*change sst written mark'*/
				P11KeyConfig.ulJitpCertificateMark = ulPresentMark;
			}
		}

		/*
		 * write Root certificate.
		 */
		else if( ( *( ( uint32_t * )pxClass->pValue ) == CKO_CERTIFICATE ) &&
			( strcmp( pxLabel->pValue,
					pkcs11configLABEL_ROOT_CERTIFICATE ) == 0 ) )
		{
			uxStatus = psa_ps_set( P11KeyConfig.uxRootCertificate,
								   ulDataSize, pucData, uxFlags);

			if( uxStatus == PSA_PS_SUCCESS )
			{
				xHandle = eAwsDeviceCertificate;

				/*change sst written mark'*/
				P11KeyConfig.ulRootCertificateMark = ulPresentMark;
			}
		}

		/*
		 * Store device private key by crypto service of PSA.
		 */

		else if( ( *( ( uint32_t * )pxClass->pValue ) == CKO_PRIVATE_KEY ) &&
			( strcmp( pxLabel->pValue,
					pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) == 0 ) )
		{
			/*
			 * Currently TF-M does not support persistent key APIs, so volatile
			 * key is used here.
			 */
			/*
			 * TODO: Replace the volatile key APIs with persistent key APIs
			 * after TF-M support that.
			 */
			uxStatus = psa_allocate_key( &uxKeyHandle );

			/*
			 * Device private key is only used to make a signature,
			 * only support RSA alg, only support MBEDTLS_RSA_PKCS_V15
			 * padding mode, only support SHA256 md alg.
			 */
			if ( uxStatus == PSA_SUCCESS )
			{
				uxPrivateKeyTypePKCS11 = mbedtls_pk_get_type( pvContext );
				switch ( uxPrivateKeyTypePKCS11 )
				{
					case MBEDTLS_PK_RSA:
						uxKeyType = PSA_KEY_TYPE_RSA_KEYPAIR;
						pucKeyData = pucData;
						ulKeyDataSize = ulDataSize;
						switch ( ( ( mbedtls_rsa_context * ) ( pvContext->pk_ctx ) )->padding )
						{
							case MBEDTLS_RSA_PKCS_V15:
								uxAlgorithm = PSA_ALG_RSA_PKCS1V15_SIGN( PSA_ALG_SHA_256 );
								break;

							case MBEDTLS_RSA_PKCS_V21:
								uxAlgorithm = PSA_ALG_RSA_PSS( PSA_ALG_SHA_256 );
								break;

							default:
								uxAlgorithm = 0;
								break;
						}
						break;

					case MBEDTLS_PK_ECKEY:
					case MBEDTLS_PK_ECDSA:
						ec = mbedtls_pk_ec(*pvContext);;
						curve_id = mbedtls_ecp_curve_info_from_grp_id( ec->grp.id )->tls_id;
						uxKeyType = PSA_KEY_TYPE_ECC_KEYPAIR(curve_id);
						uxAlgorithm = PSA_ALG_ECDSA( PSA_ALG_SHA_256 );
						xPrivateKeySizeRaw = ( ec->grp.nbits + 7 ) / 8;
						if( mbedtls_mpi_write_binary( &ec->d, pcPrivateKeyRaw, xPrivateKeySizeRaw ) != 0 )
						{
							uxStatus = PSA_ERROR_GENERIC_ERROR;
						}
						pucKeyData = pcPrivateKeyRaw;
						ulKeyDataSize = xPrivateKeySizeRaw;
						break;
					default:
						uxAlgorithm = 0;
						uxKeyType = 0;
						break;
				}
			}
			if ( uxStatus == PSA_SUCCESS )
			{
				psa_key_policy_set_usage( &policy,
										PSA_KEY_USAGE_SIGN,
										uxAlgorithm );

				uxStatus = psa_set_key_policy( uxKeyHandle, &policy );
			}
			if ( uxStatus == PSA_SUCCESS )
			{
				uxStatus = psa_import_key( uxKeyHandle,
										   uxKeyType,
										   ( const uint8_t * )pucKeyData,
										   ulKeyDataSize );

			}
			if ( uxStatus == PSA_SUCCESS )
			{
				xHandle = eAwsDevicePrivateKey;
				P11KeyConfig.uxDevicePrivateKey = uxKeyHandle;

				/*change key present mark'*/
				P11KeyConfig.ulDevicePrivateKeyMark = ulPresentMark;
			}
		}
		else if( ( *( ( uint32_t * )pxClass->pValue ) == CKO_PUBLIC_KEY ) &&
			( strcmp( pxLabel->pValue,
					pkcs11configLABEL_CODE_VERIFICATION_KEY ) == 0 ) )
		{
			uxStatus = psa_allocate_key( &uxKeyHandle );

			/*
			 * Code verify key is only used to make a verify,
			 * only SHA256 md alg.
			 */
			if ( uxStatus == PSA_SUCCESS )
			{
				switch ( mbedtls_pk_get_type( pvContext ) )
				{
					case MBEDTLS_PK_RSA:
						/**
						 * The RSA private key should contains the public key. So it should not goes here.
						 */
						break;

					case MBEDTLS_PK_ECKEY:
					case MBEDTLS_PK_ECDSA:
						ec = (mbedtls_ecp_keypair *) (pvContext->pk_ctx );
						curve_id = mbedtls_ecp_curve_info_from_grp_id( ec->grp.id )->tls_id;
						uxKeyType = PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve_id);
						uxAlgorithm = PSA_ALG_ECDSA( PSA_ALG_SHA_256 );
						if( 0 !=get_public_key_ECPoint( pucData,
														ulDataSize,
														&pcPublicKeyUncompressedData,
														&xPublicKeySizeUncompressed ) )
						{
							uxStatus = PSA_ERROR_GENERIC_ERROR;
						}

						pucKeyData = pcPublicKeyUncompressedData;
						ulKeyDataSize = xPublicKeySizeUncompressed;
						break;
					default:
						uxAlgorithm = 0;
						break;
				}
			}
			if ( uxStatus == PSA_SUCCESS )
			{
				psa_key_policy_set_usage( &policy,
										  PSA_KEY_USAGE_VERIFY,
										  uxAlgorithm );

				uxStatus = psa_set_key_policy( uxKeyHandle, &policy );
			}
			if ( uxStatus == PSA_SUCCESS )
			{
				uxStatus = psa_import_key( uxKeyHandle,
											uxKeyType,
											( const uint8_t * )pucKeyData,
											ulKeyDataSize );

			}

			if ( uxStatus == PSA_SUCCESS )
			{
				xHandle = eAwsCodeSigningKey;
				P11KeyConfig.uxCodeVerifyKey = uxKeyHandle;

				/*change key present mark'*/
				P11KeyConfig.ulCodeVerifyKeyMark = ulPresentMark;
			}
		}
		else if( ( *( ( uint32_t * )pxClass->pValue ) == CKO_PUBLIC_KEY ) &&
			( strcmp( pxLabel->pValue,
					pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) == 0 ) )
		{
			uxStatus = psa_allocate_key( &uxKeyHandle );

			/*
			 * Code verify key is only used to make a verify,
			 * only SHA256 md alg.
			 */
			if ( uxStatus == PSA_SUCCESS )
			{
				switch ( mbedtls_pk_get_type( pvContext ) )
				{
					case MBEDTLS_PK_RSA:
						break;

					case MBEDTLS_PK_ECKEY:
					case MBEDTLS_PK_ECDSA:
						ec = (mbedtls_ecp_keypair *) (pvContext->pk_ctx );
						curve_id = mbedtls_ecp_curve_info_from_grp_id( ec->grp.id )->tls_id;
						uxKeyType = PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve_id);
						uxAlgorithm = PSA_ALG_ECDSA( PSA_ALG_SHA_256 );
						if( 0 !=get_public_key_ECPoint( pucData,
														ulDataSize,
														&pcPublicKeyUncompressedData,
														&xPublicKeySizeUncompressed ) )
						{
							uxStatus = PSA_ERROR_GENERIC_ERROR;
						}
						pucKeyData = pcPublicKeyUncompressedData;
						ulKeyDataSize = xPublicKeySizeUncompressed;
						break;
					default:
						uxAlgorithm = 0;
						break;
				}
			}
			if( uxStatus == PSA_SUCCESS )
			{
				psa_key_policy_set_usage( &policy,
										  PSA_KEY_USAGE_VERIFY,
										  uxAlgorithm );

				uxStatus = psa_set_key_policy( uxKeyHandle, &policy );
			}
			if ( uxStatus == PSA_SUCCESS )
			{
				uxStatus = psa_import_key( uxKeyHandle,
										   uxKeyType,
										   ( const uint8_t * )pucKeyData,
										   ulKeyDataSize );

			}

			if ( uxStatus == PSA_SUCCESS )
			{
				xHandle = eAwsDevicePublicKey;
				P11KeyConfig.uxDevicePublicKey = uxKeyHandle;

				/*change key present mark'*/
				P11KeyConfig.ulDevicePublicKeyMark = ulPresentMark;
			}
		}
	}

	return xHandle;
}

/**
* @brief Gets the value of an object in storage, by handle.
*
* Port-specific file access for cryptographic information.
*
* This call dynamically allocates the buffer which object value
* data is copied into.  PKCS11_PSA_GetObjectValueCleanup()
* should be called after each use to free the dynamically allocated
* buffer.
*
* @sa PKCS11_PSA_GetObjectValueCleanup
*
* @param[in] pcFileName	The name of the file to be read.
* @param[out] ppucData	 Pointer to buffer for file data.
* @param[out] pulDataSize  Size (in bytes) of data located in file.
* @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
*						  or exportable (CK_FALSE)
*
* @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
* no such object handle was found, CKR_DEVICE_MEMORY if memory for
* buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
* error.
*/
CK_RV PKCS11_PSA_GetObjectValue( CK_OBJECT_HANDLE xHandle,
	uint8_t * ppucData,
	uint32_t * pulDataSize,
	CK_BBOOL * pIsPrivate )
{
	CK_RV ulReturn = CKR_OBJECT_HANDLE_INVALID;
	uint32_t ulDataSize = 0;
	psa_ps_status_t uxStatus;

	/*
	 * Read client certificate.
	 */
	if( xHandle == eAwsDeviceCertificate )
	{
		/*
		 * return reference and size only if certificates are present in the device.
		 */
		if( ( P11KeyConfig.ulDeviceCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
		{
			ulDataSize = ( uint32_t ) P11KeyConfig.ulDeviceCertificateMark & pkcs11OBJECT_LENGTH_MASK;
			uxStatus = psa_ps_get( P11KeyConfig.uxDeviceCertificate, 0, ulDataSize, ppucData );
			if ( uxStatus == PSA_PS_SUCCESS )
			{
				*pulDataSize = ulDataSize;
				*pIsPrivate = CK_FALSE;
				ulReturn = CKR_OK;
			}
		}
	}

	/*
	 * Read Jitp certificate.
	 */
	if( xHandle == eAwsJitpCertificate )
	{
		/*
		 * return reference and size only if certificates are present in the device.
		 */
		if( ( P11KeyConfig.ulJitpCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
		{
			ulDataSize = ( uint32_t ) P11KeyConfig.ulJitpCertificateMark & pkcs11OBJECT_LENGTH_MASK;
			uxStatus = psa_ps_get( P11KeyConfig.uxJitpCertificate, 0, ulDataSize, ppucData );
			if ( uxStatus == PSA_PS_SUCCESS )
			{
				*pulDataSize = ulDataSize;
				*pIsPrivate = CK_FALSE;
				ulReturn = CKR_OK;
			}
		}
	}

	/*
	 * Read Root certificate.
	 */
	if( xHandle == eAwsRootCertificate )
	{
		/*
		 * return reference and size only if certificates are present in the device.
		 */
		if( ( P11KeyConfig.ulRootCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
		{
			ulDataSize = ( uint32_t ) P11KeyConfig.ulRootCertificateMark & pkcs11OBJECT_LENGTH_MASK;
			uxStatus = psa_ps_get( P11KeyConfig.uxRootCertificate, 0, ulDataSize, ppucData );
			if ( uxStatus == PSA_PS_SUCCESS )
			{
				*pulDataSize = ulDataSize;
				*pIsPrivate = CK_FALSE;
				ulReturn = CKR_OK;
			}
		}
	}

	/*
	 * Read client key.
	 */
	else if( xHandle == eAwsDevicePrivateKey )
	{
		/*
		 * return reference and size only if key is present in the device and is not private
		 */
		if( ( P11KeyConfig.ulDevicePrivateKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
		{
			uxStatus = psa_export_key( P11KeyConfig.uxDevicePrivateKey, ppucData, *pulDataSize, pulDataSize );
			if ( uxStatus == PSA_ERROR_NOT_PERMITTED )
			{
				*pIsPrivate = CK_TRUE;
				ulReturn = CKR_OK;
			}
			else if ( uxStatus == PSA_SUCCESS )
			{
				*pIsPrivate = CK_FALSE;
				ulReturn = CKR_OK;
			}
		}
	}

	else if( xHandle == eAwsDevicePublicKey )
	{
		/*
		 * return reference and size only if key is present in the device and is not private
		 */
		if( ( P11KeyConfig.ulDevicePublicKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
		{
			uxStatus = psa_export_key( P11KeyConfig.uxDevicePublicKey, ppucData, *pulDataSize, pulDataSize );
			if ( uxStatus == PSA_ERROR_NOT_PERMITTED )
			{
				*pIsPrivate = CK_TRUE;
				ulReturn = CKR_OK;
			}
			else if ( uxStatus == PSA_SUCCESS )
			{
				*pIsPrivate = CK_FALSE;
				ulReturn = CKR_OK;
			}
		}
	}

	else if( xHandle == eAwsCodeSigningKey )
	{
		/*
		 * return reference and size only if key is present in the device and is not private
		 */
		if( ( P11KeyConfig.ulCodeVerifyKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
		{
			uxStatus = psa_export_key( P11KeyConfig.uxCodeVerifyKey, ppucData, *pulDataSize, pulDataSize );
			if ( uxStatus == PSA_ERROR_NOT_PERMITTED )
			{
				*pIsPrivate = CK_TRUE;
				ulReturn = CKR_OK;
			}
			else if ( uxStatus == PSA_SUCCESS )
			{
				*pIsPrivate = CK_FALSE;
				ulReturn = CKR_OK;
			}
		}
	}

	return ulReturn;
}

/**
* @brief Translates a PKCS #11 label into an object handle.
*
* Port-specific object handle retrieval.
*
*
* @param[in] pLabel		 Pointer to the label of the object
*						   who's handle should be found.
* @param[in] usLength	   The length of the label, in bytes.
*
* @return The object handle if operation was successful.
* Returns eInvalidHandle if unsuccessful.
*/
CK_OBJECT_HANDLE PKCS11_PSA_FindObject( uint8_t * pLabel,
	uint8_t usLength )
{
	CK_OBJECT_HANDLE xHandle = eInvalidHandle;

	if( ( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, usLength ) ) &&
		( ( P11KeyConfig.ulDeviceCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC ) )
	{
		xHandle = eAwsDeviceCertificate;
	}
	else if( ( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, usLength ) ) &&
			 ( ( P11KeyConfig.ulDevicePrivateKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC ) )
	{
		xHandle = eAwsDevicePrivateKey;
	}
	else if( ( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, usLength ) ) &&
			 ( ( P11KeyConfig.ulDevicePublicKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC ) )
	{
		xHandle = eAwsDevicePublicKey;
	}
	else if( ( 0 == memcmp( pLabel, pkcs11configLABEL_CODE_VERIFICATION_KEY, usLength ) ) &&
			 ( ( P11KeyConfig.ulCodeVerifyKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC ) )
	{
		xHandle = eAwsCodeSigningKey;
	}

	return xHandle;
}

/**
* @brief Cleanup after PKCS11_PSA_GetObjectValue().
*
* @param[in] pucData	   The buffer to free.
*						  (*ppucData from PKCS11_PSA_GetObjectValue())
* @param[in] ulDataSize	The length of the buffer to free.
*						  (*pulDataSize from PKCS11_PSA_GetObjectValue())
*/
void PKCS11_PSA_GetObjectValueCleanup( uint8_t * pucData,
	uint32_t ulDataSize )
{
	/* Unused parameters. */
	( void ) pucData;
	( void ) ulDataSize;

	/* Since no buffer was allocated on heap, there is no cleanup
	 * to be done. */
}
