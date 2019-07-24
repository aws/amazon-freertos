/* Standard includes. */
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

/* Credentials includes. */
#include "aws_clientcredential.h"

/* Demo includes. */
#include "aws_demo_config.h"
#include "aws_key_provisioning_demo.h"
#include "aws_dev_mode_key_provisioning.h"
#include <stdint.h>

/* Crypto includes. */
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"
#include "aws_dev_mode_key_provisioning.h"

/* mbedTLS includes. */
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/oid.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_csr.h"
#include "mbedtls/x509_crt.h"

#include "iot_tls.h"



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

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */


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

/* GLOBAL VARIABLES */
CK_SESSION_HANDLE xGlobalSession;
CK_RV xResult;

CK_OBJECT_HANDLE xPrivateKeyHandle = CK_INVALID_HANDLE;
CK_OBJECT_HANDLE xPublicKeyHandle = CK_INVALID_HANDLE;
CK_FUNCTION_LIST_PTR pxGlobalFunctionList;


static int prvRNG ( void * pkcs_session,
                    unsigned char * pucRandom,
                    size_t xRandomLength )
{
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


    if( xResult != 0 )
    {
        configPRINTF( ( "ERROR: Failure in signing callback: %d \r\n", xResult ) );
        lFinalResult = TLS_ERROR_SIGN;
    }

    return lFinalResult;
}

static void vProvisionDevice ( void )
{
    /* Provisioning Device Certificate */
    CK_OBJECT_HANDLE xObject = 0;
    xResult = xProvisionCertificate( xGlobalSession,
                                     keyCLIENT_CERTIFICATE_PEM,
                                     sizeof(keyCLIENT_CERTIFICATE_PEM),
                                     ( uint8_t * ) pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                     &xObject );

    if( ( xResult != CKR_OK ) || ( xObject == CK_INVALID_HANDLE ) )
    {
        configPRINTF( ( "ERROR: Failed to provision device certificate. %d \r\n", xResult ) );
    }

    /* Provisioning JITR CA Certificate */
    if( xResult == CKR_OK )
    {
        if( sizeof(keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM) != 0 )
        {
            xResult = xProvisionCertificate( xGlobalSession,
                                             keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM,
                                             sizeof(keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM),
                                             ( uint8_t * ) pkcs11configLABEL_JITP_CERTIFICATE,
                                             &xObject );

            xResult = CKR_OK;
        }
    }

    if( xResult == CKR_OK )
    {
        configPRINTF( ( "Device credential provisioning succeeded.\r\n" ) );
    }
    else
    {
        configPRINTF( ( "Device credential provisioning failed.\r\n" ) );
    }
}

static void vDevModeDeviceKeyProvisioning( void )
{

    xResult = xInitializePKCS11();
    //TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to initialize PKCS #11 module." );
    xResult = xInitializePkcs11Session( &xGlobalSession );
    //TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to open PKCS #11 session." );

    xResult = xDestroyCredentials( xGlobalSession );

    
    xResult = C_GetFunctionList( &pxGlobalFunctionList );



    #define SHA256_DIGEST_SIZE      32
    #define ECDSA_SIGNATURE_SIZE    64
    #define RSA_SIGNATURE_SIZE      256
    #define MBEDTLS_X509_CSR_H
    #define MBEDTLS_X509_CSR_WRITE_C
    #define MBEDTLS_PEM_WRITE_C
    
    CredentialsProvisioned_t xCurrentCredentials = eStateUnknown;
 
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

    mbedtls_pk_context pk_cont; 
    mbedtls_pk_init( &pk_cont );
    int ret = mbedtls_pk_setup(&pk_cont, header_copy);
    pk_cont.pk_ctx = &xEcdsaContext;


    //////////////////////////////////////////////////////////////////////////////////////////////

    // mbedtls_ecp_group_free( &xEcdsaContext.grp );
    // mbedtls_ecdsa_free( &xEcdsaContext );

    ///////////////////////////////////////////// CSR WRITING /////////////////////////////////////////////////// 
    
    mbedtls_x509write_csr my_csr;
    mbedtls_x509write_csr_init(&my_csr);
    ret = mbedtls_x509write_csr_set_subject_name(&my_csr, "CN=ThingName");
    mbedtls_x509write_csr_set_key(&my_csr, &pk_cont);
    mbedtls_x509write_csr_set_md_alg(&my_csr, MBEDTLS_MD_SHA256);
    ret = mbedtls_x509write_csr_set_key_usage(&my_csr, MBEDTLS_X509_KU_DIGITAL_SIGNATURE);
    ret = mbedtls_x509write_csr_set_ns_cert_type(&my_csr, MBEDTLS_X509_NS_CERT_TYPE_SSL_CLIENT);

    unsigned char *final_csr = pvPortMalloc(2000); 
    size_t len_buf = (size_t)2000;

    ret = mbedtls_x509write_csr_pem( &my_csr, final_csr, len_buf, &prvRNG, &xGlobalSession );

    unsigned char *csr_message = "1) PLEASE COPY THE FOLLOWING CERTIFICATE REQUEST INTO tools/create_certs/device_cert.csr :";
    unsigned char *script_message = "2) ONCE YOU'VE COPIED THE CERTIFICATE REQUEST, PLEASE RUN THE SCRIPT NAMED \"device_cert.h\" LOCATED IN tools/create_certs";
    unsigned char *cert_message = "3) ONCE YOU HAVE COMPLETED RUNNING THE SCRIPT, OPEN \"aws_clientcrediental_keys.h\" AND:"\
                                    "\n\ta) FORMAT THE TWO CERTIFICATES LOCATED IN tools/create_certs/deviceCertAndCACert.crt USING tools/certificate_configuration/PEMfileToCString.html"\
                                    "\n\tb) PASTE THE RESULTING TWO C STRINGS INTO keyCLIENT_CERTIFICATE_PEM AND keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM, RESPECTIVELY, IN \"aws_clientcrediental_keys.h\"";

    configPRINTF( ("\n\n%s\n\n%s\n\n%s\n\n%s\r\n\n", csr_message, final_csr, script_message, cert_message) );
    

    //configPRINTF( ("\n\nONCE YOU'VE COPIED THE CERTIFICATE REQUEST, PLEASE RUN THE SCRIPT NAMED \"device_cert.h\" LOCATED IN tools/create_certs\r\n\n") );
    //configPRINTF( ("\n\n%s\r\n\n", cert_message) );
    
    vProvisionDevice( ); 

    return;

}


void vStartKeyProvisioningDemo( void )
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