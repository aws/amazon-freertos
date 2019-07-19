/* Standard includes. */
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

/* MQTT includes. */
//#include "aws_mqtt_agent.h"

/* Credentials includes. */
#include "aws_clientcredential.h"

/* Demo includes. */
#include "aws_demo_config.h"
#include "aws_key_provisioning_demo.h"
#include "aws_dev_mode_key_provisioning.h"
#include <stdint.h>

/* Crypto includes. */
//#include "aws_crypto.h"
#include "aws_clientcredential.h"
//#include "aws_default_root_certificates.h"
#include "iot_pkcs11.h" //used to be aws
//#include "aws_pkcs11_mbedtls.h"
#include "aws_dev_mode_key_provisioning.h"
//#include "aws_test_pkcs11_config.h"
#include "mbedtls/x509_crt.h"

//#include "aws_pkcs11_config.h"


/* mbedTLS includes. */
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/oid.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_csr.h"

#include "iot_tls.h"

/* Test includes. */
//#include "unity_fixture.h"
//#include <unity.h>



/**
 * @brief Gives users the option to generate keys externally or internally
 * 0 if keys are to be imported
 * 1 if keys are to be generated on device
 */
#define GENERATE_KEYS_ON_DEVICE 1

/**
 * @brief Gives users the option to reprovision each time the demo is run
 * 0 if no reprovisioning is required
 * 1 if user wants to reprovision the device everytime
 */
//#define REPROVISION_EACH_TIME 0


#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H


#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEApyQJJUNHySqmv62Lf8bDby2fcGHGRycph4TaFzXum9fi5pvG\n"\
"g7kV2Yn+0ZKqDdgqqekB3C9wM8EX9reEL55jXrofW8HTJo1VlIFo0QVOfFURJ5qw\n"\
"r1Dtf9y/KoTRX2ZZiLuR/4/gQjVqtK75WT/yIwtgqtnG+gB1ItU4ntI00PSh0j87\n"\
"drEzofEerD+G3McQATYg8qTizTjdMrOTRrJFpUJvOw9Guges596AtaPA6/x/2SRD\n"\
"rDtKKn3LJpZEXWhy4QhJqG9+4FijS5APutNmE7yOFv0WEyz6BmDp37AvXlkwqKCU\n"\
"J2XqH5NiQP1V4pZZtOJu2if68i2bXT1Vi/ETAQIDAQABAoIBAQCmyKHLqkCW79Wn\n"\
"TdD3N2tLpPYF2btWwzCAx8QtBydMIfiMcAN+7BFum3JIxJam6/9ev2GYy0mmi1eT\n"\
"XPst3VqBay7hjB9cyAoO/7eNZEM2dFiC8ay/tPzZ63hjQFgBX4uWkr2UjIiW6ODK\n"\
"3hN5yQKhj7ftpRUV4aEBmk0+xrnni6g5kPoT1beDLdZTo0TM22hcdSkeLD0WUeRR\n"\
"X3S024KDDU0YNw8dGJlZPhXD8ERAAIt85Xl0CpY92B1j6EbGiu5dFAjmUSmDZTk7\n"\
"Vpiy95eWv1DMCwJqUWrE44WvKDLIVu6Frxlus/RTMPOITvNXkW2VPr8+crJaAU/N\n"\
"aJ9NBTtlAoGBANgbFm26y76kImzPefc3wLxSh1b7q40tgC/poRSgqX58uzO1fRp6\n"\
"SxuxMTS5Q3smx2YL2paHdtWYZVh4IJSb9rknH/V5l0/bSh+8aRhcPrAjr8otWy88\n"\
"pc4/do11lh/YEtdSiJPluwcQ3XuZppoBW122B7vx/EO1w6C/3NMnQXZDAoGBAMX+\n"\
"6pBLucHqrfWVEQgvWTl5lnAq4inIW8MytJOTFUWSmwQd+MSgUUuF/u5NfEfKCO1T\n"\
"srHQydEy9W9vrO4RdXbZliGTA4K28V0atenyutwYbckMD8oLkSMZ5iCK6f58fFHH\n"\
"X+MsEsZwBnasRu6mRzR62bHfagVEFHpiB9yM/vdrAoGAGw1pBVt43eiWFqDyV6w3\n"\
"CFwcbRscIpQrk9PfQkHKdfcPSC3T91iPxmusCTmX5Pa5x2B8gJN8oMhvB2AiU/gi\n"\
"LpNjQMtz0Z9V7Bsd3NrPTiVpgBmlbIhfCS6QnKy3mkzmLuY6CqmbfAr0buCRJn48\n"\
"/cInvbNRLPi4AIuEDTrAyxkCgYBI+z7A83jENtnqhQjjKHjOC7hdHiGBXz4bxGv1\n"\
"XajgnTqQmO+ioql0mi6u7lbaK2mA+XILwzcw6oS3j8WBd2QB7BA/ze3kV8S+GeNm\n"\
"Gbew+zw0eEK1qe/UrW47HplD2eUFS+VXWl6NUKvBQJ1gyF/Ew2fM/doZmM+nkMzv\n"\
"CfXNgwKBgClyO3M87x9JXNKBuLyVGYwbWqN5lTHXS6J4HioBkQuVDcdrqwGACN8h\n"\
"0q2aGlI5SkSb0fvj7IIE4aN/OD/N7uoaCW3DqONsxcGiYjLuV9NysV8sfkzkkJQm\n"\
"vWlt5Hh8hv3giCt/Q8sgve3FQSzJDjwWNPano39dDJrXGkFtZI2V\n"\
"-----END RSA PRIVATE KEY-----\n"

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */

/* Valid RSA private key. */
static const char cValidRSAPrivateKey[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIIEpAIBAAKCAQEAsIqRecRxLz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7o\n"
    "swihe7+26yx8medpNvX1ym9jphty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx\n"
    "/x8LMdXFoBAkRXIL8WS5YKafC87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N10\n"
    "0XL5ykLCa4xJBOqlIvbDvJ+bKty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+g\n"
    "Ujinr7h6e2uQeipWyiIw9NKWbvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOz\n"
    "BNl1YZ4lIW5sJLATES9+Z8nHi7yRDLw6x/kcVQIDAQABAoIBADd+h3ZIeu/HtT8I\n"
    "vNuSSK0bwpj+wV1O9VcbMLfp760bEAd+J5XHu8NDo4NPi6dxZ9CABpBo7WEUtdNU\n"
    "2Ie11W4B8WpwvXpPIvOxLMJf85/ie5EjDNuObZ1vvlyvVkeCLyDlcaRhHBPBIC/+\n"
    "SpPY/1qNTSzwd6+55zkM69YajD60tFv8WuCsgkAteCoDjcqwDcowyAy4pILhOYaW\n"
    "1x+0ZPMUqwtld+06ct/yqBPB8C9IH7ZIeJr5e58R9SxytbuTwTN4jceOoeD5MBbG\n"
    "A+A0WxGdQ8N+kwWkz77qDbZfP4G8wNxeUXobnfqfDGqb0O5zeEaU7EI+mlEQH58Z\n"
    "B1edj6ECgYEA3rldciCQc4t2qYKiZSffnl7Pg7L+ojzH/Eam4Dlk27+DqOo70MnX\n"
    "LVWUWkLOMQ27dRSBQsUDUaqVRZLkcFKc6C8k2cIpPBMpA9WdZVd9kFawZ8lJ7jko\n"
    "qTbJxnDxvhdHrZRvLRjEenbdNXdAGy2EuqvThUJgPEybLAWg6sE3LB0CgYEAyurT\n"
    "14h4BGEGBpl2KevoPQ4PPS+IoDXMS29mtfcascVkYcxxW1v8wOQVyD4VrsRMErck\n"
    "ZMpu2evd+aQSPSrAod/sQ20C+wCCA7ipBlhAUeuS/FpqFIZWkHzZnVccp8O3nOFO\n"
    "KNeAmw4udq8PyjVVouey/6F386itJdxWt/d8i5kCgYA3Aru045wqHck6RvzLVVTj\n"
    "LfG9Sqmf8rlGc0DmYuapbB0dzHTnteLC3L9ep997uDOT0HO4xSZztllWLNjlcVI1\n"
    "+ub0LgO3Rdg8jTdp/3kQ/IhnqgzrnQyQ9upRbDYZSHC4y8/F6LcmtFMg0Ipx7AU7\n"
    "ghMld+aDHjy5W86KDR0OdQKBgQCAZoPSONqo+rQTbPwmns6AA+uErhVoO2KgwUdf\n"
    "EZPktaFFeVapltWjQTC/WvnhcvkoRpdS5/2pC+WUWEvqRKlMRSN9rvdZ2QJsVGcw\n"
    "Spu4urZx1MyXXEJef4I8W6kYR3JiZPdORL9uXlTsaO425/Tednr/4y7CEhQuhvSg\n"
    "yIwY0QKBgQC2NtKDOwcgFykKRYqtHuo6VpSeLmgm1DjlcAuaGJsblX7C07ZH8Tjm\n"
    "IHQb01oThNEa4tC0vO3518PkQwvyi/TWGHm9SLYdXvpVnBwkk5yRioKPgPmrs4Xi\n"
    "ERIYrvveGGtQ3vSknLWUJ/0BgmuYj5U6aJBZPv8COM2eKIbTQbtQaQ==\n"
    "-----END RSA PRIVATE KEY-----\n";


/* The constants above are set to const char * pointers defined in aws_demo_runner.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS 
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#define pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS       "Device Priv TLS Key"
#define pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS       pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS

#define pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS        "Device Pub TLS Key"
#define pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS        pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS

typedef enum
{
    eNone,                /* Device is not provisioned.  All credentials have been destroyed. */
    eRsaTest,             /* Provisioned using the RSA test credentials located in this file. */
    eEllipticCurveTest,   /* Provisioned using EC test credentials located in this file. */
    eClientCredential,    /* Provisioned using the credentials in aws_clientcredential_keys. */
    eGeneratedEc,         /* Provisioned using elliptic curve generated on device.  Private key unknown.  No corresponding certificate. */
    eGeneratedRsa,
    eDeliberatelyInvalid, /* Provisioned using credentials that are meant to trigger an error condition. */
    eStateUnknown         /* State of the credentials is unknown. */
} CredentialsProvisioned_t;

//// GLOBAL VARIABLES ////
CK_SESSION_HANDLE xGlobalSession;
CK_RV xResult;

CK_OBJECT_HANDLE xPrivateKeyHandle = CK_INVALID_HANDLE;
CK_OBJECT_HANDLE xPublicKeyHandle = CK_INVALID_HANDLE;
CK_OBJECT_HANDLE xFoundPrivateKeyHandle = CK_INVALID_HANDLE;
CK_OBJECT_HANDLE xFoundPublicKeyHandle = CK_INVALID_HANDLE;
CK_FUNCTION_LIST_PTR pxGlobalFunctionList;


static int prvRNG ( void * pkcs_session,
                    unsigned char * pucRandom,
                    size_t xRandomLength )
{
   // TLSContext_t * pxCtx = ( TLSContext_t * ) pkcs_session; /*lint !e9087 !e9079 Allow casting void* to other types. */
    BaseType_t xResult;
    CK_FUNCTION_LIST_PTR pxP11FunctionList;
    xResult = C_GetFunctionList( &pxP11FunctionList );


    xResult = pxP11FunctionList->C_GenerateRandom( (*((CK_SESSION_HANDLE*)pkcs_session)), pucRandom, xRandomLength );

    if( xResult != 0 )
    {
        configPRINTF( ( "ERROR: Failed to generate random bytes %d \r\n", xResult ) );
        xResult = TLS_ERROR_RNG;
    }

    return xResult;
}

static int prvPrivateKeySigningCallback( void * pvContext,
                                         mbedtls_md_type_t xMdAlg,
                                         const unsigned char * pucHash,
                                         size_t xHashLen,
                                         unsigned char * pucSig,
                                         size_t * pxSigLen,
                                         int ( *piRng )( void *,
                                                         unsigned char *,
                                                         size_t ), /*lint !e955 This parameter is unused. */
                                         void * pvRng )
{
    CK_RV xResult = 0;
    int lFinalResult = 0;
    //TLSContext_t * pxTLSContext = ( TLSContext_t * ) pvContext;
    CK_MECHANISM xMech = { 0 };
    CK_BYTE xToBeSigned[ 256 ];
    uint8_t ucTemp[ 64 ] = { 0 }; /* A temporary buffer for the pre-formatted signature. */
    CK_ULONG xToBeSignedLen = sizeof( xToBeSigned );
    

    /* Unreferenced parameters. */
    ( void ) ( piRng );
    ( void ) ( pvRng );
    ( void ) ( xMdAlg );

    /* Sanity check buffer length. */
    if( xHashLen > sizeof( xToBeSigned ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* If applicable, format the hash data to be signed. */
    // if( CKK_RSA == pxTLSContext->xKeyType )
    // {
    //     xMech.mechanism = CKM_RSA_PKCS;
    //     vAppendSHA256AlgorithmIdentifierSequence( ( uint8_t * ) pucHash, xToBeSigned );
    //     xToBeSignedLen = pkcs11RSA_SIGNATURE_INPUT_LENGTH;
    // }
    // else if( CKK_EC == pxTLSContext->xKeyType )
    // {
    //     xMech.mechanism = CKM_ECDSA;
    //     memcpy( xToBeSigned, pucHash, xHashLen );
    //     xToBeSignedLen = xHashLen;
    // }
    // else
    // {
    //     xResult = CKR_ARGUMENTS_BAD;
    // }
    xMech.mechanism = CKM_ECDSA;
    memcpy( xToBeSigned, pucHash, xHashLen );
    xToBeSignedLen = xHashLen;

    if( 0 == xResult )
    {
        /* Use the PKCS#11 module to sign. */
        xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession,
                                                               &xMech,
                                                               xPrivateKeyHandle );
    }

    if( 0 == xResult )
    {
        *pxSigLen = sizeof( xToBeSigned );
        xResult = pxGlobalFunctionList->C_Sign( ( CK_SESSION_HANDLE ) xGlobalSession,
                                                           xToBeSigned,
                                                           xToBeSignedLen,
                                                           pucSig,
                                                           ( CK_ULONG_PTR ) pxSigLen );
    }

    // if( CKK_EC == pxTLSContext->xKeyType )
    // {
        uint8_t * pucSigPtr;

        /* PKCS #11 for P256 returns a 64-byte signature with 32 bytes for R and 32 bytes for S.
         * This must be converted to an ASN1 encoded array. */
        configASSERT( *pxSigLen == 64 );
        memcpy( ucTemp, pucSig, *pxSigLen );

        pucSig[ 0 ] = 0x30; /* Sequence. */
        pucSig[ 1 ] = 0x44; /* The minimum length the signature could be. */
        pucSig[ 2 ] = 0x02; /* Integer. */

        if( ucTemp[ 0 ] & 0x80 )
        {
            pucSig[ 1 ]++;
            pucSig[ 3 ] = 0x21;
            pucSig[ 4 ] = 0x0;
            memcpy( &pucSig[ 5 ], ucTemp, 32 );
            pucSigPtr = pucSig + 33 + 4;
        }
        else
        {
            pucSig[ 3 ] = 0x20;
            memcpy( &pucSig[ 4 ], ucTemp, 32 );
            pucSigPtr = pucSig + 32 + 4;
        }

        pucSigPtr[ 0 ] = 0x02; /* Integer. */
        pucSigPtr++;

        if( ucTemp[ 32 ] & 0x80 )
        {
            pucSig[ 1 ]++;
            pucSigPtr[ 0 ] = 0x21;
            pucSigPtr[ 1 ] = 0x00;
            pucSigPtr += 2;

            memcpy( pucSigPtr, &ucTemp[ 32 ], 32 );
        }
        else
        {
            pucSigPtr[ 0 ] = 0x20;
            pucSigPtr++;
            memcpy( pucSigPtr, &ucTemp[ 32 ], 32 );
        }

        *pxSigLen = ( CK_ULONG ) pucSig[ 1 ] + 2;
    //}

    if( xResult != 0 )
    {
        configPRINTF( ( "ERROR: Failure in signing callback: %d \r\n", xResult ) );
        lFinalResult = TLS_ERROR_SIGN;
    }

    return lFinalResult;
}


int write_certificate_request( mbedtls_x509write_csr *req, const char *output_file,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng )
{
    int ret;
    FILE *f;
    unsigned char output_buf[4096];
    size_t len = 0;

    memset( output_buf, 0, 4096 );
    if( ( ret = mbedtls_x509write_csr_pem( req, output_buf, 4096, f_rng, p_rng ) ) < 0 )
        return( ret );

    len = strlen( (char *) output_buf );

    char test[] = "testing";

    if( ( f = fopen( output_file, "w" ) ) == NULL )
        return( -1 );
    
    fwrite(test , 1 , sizeof(test) , f );


    if( fwrite( output_buf, 1, len, f ) != len )
    {
        fclose( f );
        return( -1 );
    }

    fclose( f );

    return( 0 );
}

static void vDevModeDeviceKeyProvisioning( void )
{

    xResult = xInitializePKCS11();
    //TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to initialize PKCS #11 module." );
    xResult = xInitializePkcs11Session( &xGlobalSession );
    //TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to open PKCS #11 session." );
    // CK_OBJECT_HANDLE xPrivateKeyHandle;
    // CK_OBJECT_HANDLE xPublicKeyHandle;
    xResult = xDestroyCredentials( xGlobalSession );

    
    xResult = C_GetFunctionList( &pxGlobalFunctionList );



    // xResult = xProvisionGenerateKeyPairEC( xGlobalSession,
    //                                         ( uint8_t * ) pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
    //                                         ( uint8_t * ) pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
    //                                         &xPrivateKeyHandle,
    //                                         &xPublicKeyHandle );

    // CK_ATTRIBUTE xTemplate;
    // xTemplate.type = CKA_EC_POINT;

    // CK_BYTE xEC[ 4 ] = { 0 };
    // xTemplate.pValue = xEC;
    // xTemplate.ulValueLen = sizeof( xEC );
    // xResult = pxFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );


    // configPRINTF((cValidRSAPrivateKey));


    #define SHA256_DIGEST_SIZE      32
    #define ECDSA_SIGNATURE_SIZE    64
    #define RSA_SIGNATURE_SIZE      256
    #define MBEDTLS_X509_CSR_H
    #define MBEDTLS_X509_CSR_WRITE_C
    #define MBEDTLS_PEM_WRITE_C
    
    CredentialsProvisioned_t xCurrentCredentials = eStateUnknown;

    //CK_RV xResult;
 
    /* Note that mbedTLS does not permit a signature on all 0's. */
    CK_BYTE xHashedMessage[ SHA256_DIGEST_SIZE ] = { 0xab };
    CK_MECHANISM xMechanism;
    CK_BYTE xSignature[ RSA_SIGNATURE_SIZE ] = { 0 };
    CK_BYTE xEcPoint[ 256 ] = { 0 };
    CK_BYTE xEcParams[ 11 ] = { 0 };
    CK_KEY_TYPE xKeyType;
    CK_ULONG xSignatureLength;
    CK_ATTRIBUTE xTemplate;
    CK_OBJECT_CLASS xClass;
    /* mbedTLS structures for verification. */
    int lMbedTLSResult;
    mbedtls_ecdsa_context xEcdsaContext;
    uint8_t ucSecp256r1Oid[] = pkcs11DER_ENCODED_OID_P256; /*"\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1; */

    /* An ECDSA signature is comprised of 2 components - R & S. */
    mbedtls_mpi xR;
    mbedtls_mpi xS;

    xResult = xDestroyCredentials( xGlobalSession );
    //TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to destroy credentials before Generating Key Pair" );
    xCurrentCredentials = eNone;

    xResult = xProvisionGenerateKeyPairEC( xGlobalSession,
                                           ( uint8_t * ) pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                           ( uint8_t * ) pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                           &xPrivateKeyHandle,
                                           &xPublicKeyHandle );

    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Generating EC key pair failed." );
    // TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, xPrivateKeyHandle, "Invalid private key handle generated by GenerateKeyPair" );
    // TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, xPublicKeyHandle, "Invalid public key handle generated by GenerateKeyPair" );

    /* Call GetAttributeValue to retrieve information about the keypair stored. */

    /* Check that correct object class retrieved. */
    xTemplate.type = CKA_CLASS;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPublicKeyHandle, &xTemplate, 1 );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "GetAttributeValue for length of public EC key class failed." );
    // TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_OBJECT_CLASS ), xTemplate.ulValueLen, "Incorrect object class length returned from GetAttributeValue." );

    xTemplate.pValue = &xClass;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "GetAttributeValue for private EC key class failed." );
    // TEST_ASSERT_EQUAL_MESSAGE( CKO_PRIVATE_KEY, xClass, "Incorrect object class returned from GetAttributeValue." );

    xTemplate.pValue = &xClass;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPublicKeyHandle, &xTemplate, 1 );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "GetAttributeValue for public EC key class failed." );
    // TEST_ASSERT_EQUAL_MESSAGE( CKO_PUBLIC_KEY, xClass, "Incorrect object class returned from GetAttributeValue." );


    /* Check that both keys are stored as EC Keys. */
    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = &xKeyType;
    xTemplate.ulValueLen = sizeof( CK_KEY_TYPE );
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error getting attribute value of EC key type." );
    // TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen, "Length of key type incorrect in GetAttributeValue" );
    // TEST_ASSERT_EQUAL_MESSAGE( CKK_EC, xKeyType, "Incorrect key type for private key" );

    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPublicKeyHandle, &xTemplate, 1 );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error getting attribute value of EC key type." );
    // TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen, "Length of key type incorrect in GetAttributeValue" );
    // TEST_ASSERT_EQUAL_MESSAGE( CKK_EC, xKeyType, "Incorrect key type for public key" );

    /* Check that correct curve retrieved for private key. */
    // xTemplate.type = CKA_EC_PARAMS;
    // xTemplate.pValue = xEcParams;
    // xTemplate.ulValueLen = sizeof( xEcParams );
    // xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error getting attribute value of EC Parameters." );
    // TEST_ASSERT_EQUAL_MESSAGE( sizeof( ucSecp256r1Oid ), xTemplate.ulValueLen, "Length of ECParameters identifier incorrect in GetAttributeValue" );
    // TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE( ucSecp256r1Oid, xEcParams, xTemplate.ulValueLen, "EcParameters did not match P256 OID." );

    /* Check that public key point can be retrieved for public key. */
    xTemplate.type = CKA_EC_POINT;
    xTemplate.pValue = xEcPoint;
    xTemplate.ulValueLen = sizeof( xEcPoint );
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPublicKeyHandle, &xTemplate, 1 );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to retrieve EC Point." );

    /* Perform a sign with the generated private key. */
    xMechanism.mechanism = CKM_ECDSA;
    xMechanism.pParameter = NULL;
    xMechanism.ulParameterLen = 0;
    xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession, &xMechanism, xPrivateKeyHandle );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to SignInit ECDSA." );

    xSignatureLength = sizeof( xSignature );
    xResult = pxGlobalFunctionList->C_Sign( xGlobalSession, xHashedMessage, SHA256_DIGEST_SIZE, xSignature, &xSignatureLength );
    // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to ECDSA Sign." );

    /* Verify the signature with mbedTLS */
    mbedtls_ecdsa_init( &xEcdsaContext );
    mbedtls_ecp_group_init( &xEcdsaContext.grp );


    lMbedTLSResult = mbedtls_ecp_group_load( &xEcdsaContext.grp, MBEDTLS_ECP_DP_SECP256R1 );
    // TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed in setup for signature verification." );
    /* The first 2 bytes are for ASN1 type/length encoding. */
    lMbedTLSResult = mbedtls_ecp_point_read_binary( &xEcdsaContext.grp, &xEcdsaContext.Q, &xEcPoint[ 2 ], xTemplate.ulValueLen - 2 );
    // TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed in setup for signature verification." );

    /* C_Sign returns the R & S components one after another- import these into a format that mbedTLS can work with. */
    // mbedtls_mpi_init( &xR );
    // mbedtls_mpi_init( &xS );
    // lMbedTLSResult = mbedtls_mpi_read_binary( &xR, &xSignature[ 0 ], 32 );
    // // TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed in setup for signature verification." );
    // lMbedTLSResult = mbedtls_mpi_read_binary( &xS, &xSignature[ 32 ], 32 );
    // // TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed in setup for signature verification." );

    // /* Verify using mbedTLS & exported public key. */
    // lMbedTLSResult = mbedtls_ecdsa_verify( &xEcdsaContext.grp, xHashedMessage, sizeof( xHashedMessage ), &xEcdsaContext.Q, &xR, &xS );
    // // TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed to verify signature." );

    // /* Verify the signature with the generated public key. */
    // xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession, &xMechanism, xPublicKeyHandle );
    // // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to VerifyInit ECDSA." );
    // xResult = pxGlobalFunctionList->C_Verify( xGlobalSession, xHashedMessage, SHA256_DIGEST_SIZE, xSignature, xSignatureLength );
    // // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to Verify ECDSA." );


    //////////////////////////////////////////////////////////////////////////////////////////////
    mbedtls_pk_type_t type_key = MBEDTLS_PK_ECKEY;
    const mbedtls_pk_info_t *header = mbedtls_pk_info_from_type(type_key);

    mbedtls_pk_info_t *header_copy = pvPortMalloc(sizeof(mbedtls_pk_info_t));
    memcpy(header_copy, header, sizeof(struct mbedtls_pk_info_t));

    header_copy->sign_func = &prvPrivateKeySigningCallback;

    // mbedtls_ecp_keypair keypair = pk_cont.pk_ctx;
    // void *key_stuff = mbedtls_ecdsa_context();

    // mbedtls_ecdsa_context ecdsa;
    // mbedtls_ecdsa_init( &ecdsa );
    // mbedtls_ecdsa_from_keypair(&ecdsa, &xPublicKeyHandle);

    mbedtls_pk_context pk_cont; // = pvPortMalloc(sizeof(struct mbedtls_pk_context));
    mbedtls_pk_init( &pk_cont );
    int ret = mbedtls_pk_setup(&pk_cont, header_copy);
    pk_cont.pk_ctx = &xEcdsaContext;

    //mbedtls_pk_parse_keyfile( &pk_cont, (const char*) (&xPrivateKeyHandle), NULL );

    //////////////////////////////////////////////////////////////////////////////////////////////
    
    // mbedtls_mpi_free( &xR );
    // mbedtls_mpi_free( &xS );
    // mbedtls_ecp_group_free( &xEcdsaContext.grp );
    // mbedtls_ecdsa_free( &xEcdsaContext );


    // /* Check that FindObject works on Generated Key Pairs. */
    // xResult = xFindObjectWithLabelAndClass( xGlobalSession, pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, CKO_PRIVATE_KEY, &xFoundPrivateKeyHandle );
    // // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error finding generated private key." );
    // // TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, xFoundPrivateKeyHandle, "Invalid private key handle found." );

    // xResult = xFindObjectWithLabelAndClass( xGlobalSession, pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, CKO_PUBLIC_KEY, &xFoundPublicKeyHandle );
    // // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error finding generated public key." );
    // // TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, xFoundPrivateKeyHandle, "Invalid public key handle found." );

    // /* Close & reopen the session.  Make sure you can still find the keys. */
    // xResult = pxGlobalFunctionList->C_CloseSession( xGlobalSession );
    // // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error closing session after generating key pair." );
    // xResult = xInitializePkcs11Session( &xGlobalSession );
    // // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error re-opening session after generating key pair." );

    // xResult = xFindObjectWithLabelAndClass( xGlobalSession, pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, CKO_PRIVATE_KEY, &xFoundPrivateKeyHandle );
    // // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error finding generated private key." );
    // // TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, xFoundPrivateKeyHandle, "Invalid private key handle found." );

    // xResult = xFindObjectWithLabelAndClass( xGlobalSession, pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, CKO_PUBLIC_KEY, &xFoundPublicKeyHandle );
    // // TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error finding generated public key." );
    // // TEST_ASSERT_NOT_EQUAL_MESSAGE( CK_INVALID_HANDLE, xFoundPrivateKeyHandle, "Invalid public key handle found." );


    ///////////////////////////////////////////// CSR WRITING /////////////////////////////////////////////////// 
    
    // mbedtls_asn1_buf oid = {MBEDTLS_ASN1_UTF8_STRING, 3, "idk"};
    // mbedtls_asn1_buf thing_info = {MBEDTLS_ASN1_UTF8_STRING, 9, "ThingName"};
    // mbedtls_asn1_named_data subj = {oid, thing_info, NULL, 0};
    // mbedtls_pk_context key;
    // mbedtls_pk_init( &key );
    // const char *test_key = &cValidRSAPrivateKey[0];
    // int ret = mbedtls_pk_parse_keyfile( &key, test_key, NULL );
    // if( ret != 0 )
    // {
    //     configPRINTF(( " failed\n  !  mbedtls_pk_parse_keyfile returned %d", ret ));
  
    // }


   
    mbedtls_x509write_csr my_csr; // = pvPortMalloc(sizeof(struct mbedtls_x509write_csr));
    mbedtls_x509write_csr_init(&my_csr);
    ret = mbedtls_x509write_csr_set_subject_name(&my_csr, "CN=ThingName");
    mbedtls_x509write_csr_set_key(&my_csr, &pk_cont); //pk_cont
    mbedtls_x509write_csr_set_md_alg(&my_csr, MBEDTLS_MD_SHA256);
    ret = mbedtls_x509write_csr_set_key_usage(&my_csr, MBEDTLS_X509_KU_DIGITAL_SIGNATURE);
    ret = mbedtls_x509write_csr_set_ns_cert_type(&my_csr, MBEDTLS_X509_NS_CERT_TYPE_SSL_CLIENT); //do we need this?

    unsigned char *final_csr = pvPortMalloc(4096) ; 
    
    //const char* filename = "csr.txt";
    size_t len_buf = (size_t)4096;

    ret = mbedtls_x509write_csr_pem( &my_csr, final_csr, len_buf, &prvRNG, &xGlobalSession );
    // write_certificate_request( &my_csr, filename,
    //                             &prvRNG,
    //                             &xGlobalSession );

    configPRINTF((final_csr));

}

void vStartKeyProvisioningDemo( void )
                                // (bool awsIotMqttMode,
                                // const char * pIdentifier,
                                // void * pNetworkServerInfo,
                                // void * pNetworkCredentialInfo,
                                // const IotNetworkInterface_t * pNetworkInterface)
{
    #if(!GENERATE_KEYS_ON_DEVICE)
    {
        configPRINTF( ( "Keys are Being Imported\r\n" ) );
        configPRINTF( ( "Starting Key Provisioning\r\n" ) );
    
        (void) vDevModeKeyProvisioning( );

        configPRINTF( ( "Ending Key Provisioning\r\n" ) );
    }
    #else
    {
        configPRINTF( ( "Keys Are Being Generated on Device\r\n" ) );
        configPRINTF( ( "Starting Key Provisioning\r\n" ) );

        (void) vDevModeDeviceKeyProvisioning( );
        
        configPRINTF( ( "Ending Key Provisioning\r\n" ) );


    }
    #endif
}