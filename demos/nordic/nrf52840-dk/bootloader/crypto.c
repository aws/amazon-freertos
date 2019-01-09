#include "crypto.h"
#include "nrf_crypto_init.h"
#include "nrf_crypto_ecdsa.h"
#include "nrf_crypto_hash.h"
#include "bootloader.h"
#include "asn1utility.h"

extern unsigned char the_public_key[ 91 ];

ret_code_t xCryptoInit()
{
    return nrf_crypto_init();
}

ret_code_t xCryptoUnInit()
{
    return nrf_crypto_uninit();
}

ret_code_t xComputeSHA256Hash( void * pvMessage,
                               size_t ulMessageSize,
                               void * pvHashDigest,
                               size_t * pulDigestSize )
{
    /* We are computing the hash of memory stored on the flash so we need to first move it to ram to use CC310
     * That's why we are computing the hash by chunks */
    ret_code_t err_code;
    static nrf_crypto_hash_context_t hash_context;

    err_code = nrf_crypto_hash_init( &hash_context, &g_nrf_crypto_hash_sha256_info );
    uint8_t pulMemBuffer[ 256 ];
    size_t ulConsumed = 0;

    while( err_code == NRF_SUCCESS && ulConsumed < ulMessageSize )
    {
        size_t ulToConsume = MIN( ulMessageSize - ulConsumed, 256 );
        memcpy( pulMemBuffer, pvMessage + ulConsumed, ulToConsume );
        err_code = nrf_crypto_hash_update( &hash_context, pulMemBuffer, ulToConsume );
        ulConsumed += ulToConsume;
    }

    if( err_code == NRF_SUCCESS )
    {
        err_code = nrf_crypto_hash_finalize( &hash_context, pvHashDigest, pulDigestSize );
    }

    return err_code;
}

/*-----------------------------------------------------------*/

ret_code_t xVerifySignature( uint8_t * pusHash,
                             size_t ulHashSize,
                             uint8_t * pusSignature,
                             size_t ulSignatureSize )
{
    volatile ret_code_t xErrCode;
    nrf_crypto_ecdsa_verify_context_t xVerifyContext;
    nrf_crypto_ecc_public_key_t xPublicKey;
    nrf_crypto_ecdsa_signature_t xSignature;

    memset( &xPublicKey, 0, sizeof( xPublicKey ) );
    memset( &xSignature, 0, sizeof( xSignature ) );
    /* Copy the public key from flash to memory to allow itto be used by CC310*/
    uint8_t pusPublicKey[ sizeof( the_public_key ) ];

    /* The public key comes in the ASN.1 DER format,  and so we need everything
     * except the DER metadata which fortunately in this case is containded in the front part of buffer */
    memcpy( pusPublicKey, the_public_key + ( sizeof( the_public_key ) - NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE ), sizeof( the_public_key ) );
    /* Convert the extracted public key to the NRF5 representation */
    xErrCode = nrf_crypto_ecc_public_key_from_raw( &g_nrf_crypto_ecc_secp256r1_curve_info, &xPublicKey, pusPublicKey, NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE );
    /* The signature is also ASN.1 DER encoded, but here we need to decode it properly */
    xErrCode = asn1_decodeSignature( xSignature, pusSignature, pusSignature + ulSignatureSize );
    /* Aand... Verify! */
    xErrCode = nrf_crypto_ecdsa_verify( &xVerifyContext,                             /* Context */
                                        &xPublicKey,                                 /* Public key */
                                        pusHash,                                     /* Message hash */
                                        ulHashSize,                                  /* Hash size */
                                        xSignature,                                  /* Signature */
                                        NRF_CRYPTO_ECDSA_SECP256R1_SIGNATURE_SIZE ); /* Signature size */
    return xErrCode;
}

ret_code_t xVerifyImageSignature( uint8_t * pusImageStart )
{
    nrf_crypto_hash_sha256_digest_t xHash;
    size_t ulHashLength = sizeof( xHash );
    volatile ret_code_t xErrCode = NRF_SUCCESS;

    ImageDescriptor_t * descriptor = ( ImageDescriptor_t * ) ( pusImageStart );

    xErrCode = xComputeSHA256Hash( ( uint8_t * ) descriptor->ulStartAddress, descriptor->ulEndAddress - descriptor->ulStartAddress, &xHash, &ulHashLength );
    if (xErrCode == NRF_SUCCESS)
    {
        xErrCode = xVerifySignature( ( uint8_t * ) &xHash, ulHashLength, descriptor->pSignature, descriptor->ulSignatureSize );
    }

    if( xErrCode != NRF_SUCCESS )
    {
        return NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE;
    }

    return NRF_SUCCESS;
}
