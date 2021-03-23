/*
 * FreeRTOS OTA V1.2.1
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

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "unity_fixture.h"
#include "unity.h"

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

#include "logging_levels.h"

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "TEST"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"

/* The logging configuration macros are defined above to ensure they are not
 * superceded by definitions in the following header files. */

#include "ota_platform_interface.h"
#include "ota.h"

#if ( otatestpalREAD_CERTIFICATE_FROM_NVM_WITH_PKCS11 == 1 )
    #include "core_pkcs11_config.h"
    #include "core_pkcs11.h"
    #include "aws_dev_mode_key_provisioning.h"
#endif

#include "aws_ota_codesigner_certificate.h"

//#include "aws_test_ota_pal_rsa_sha1_signature.h"

/* The Texas Instruments CC3220SF has special requirements on its file system security.
 * We enable code specially for testing the OTA PAL layer for this device. */
#ifdef CC3220sf
    #include <ti/drivers/net/wifi/simplelink.h>
#endif

/* For the otaPal_WriteBlock_WriteManyBlocks test this is the number of blocks of
 * ucDummyData to write to the non-volatile memory. */
#define testotapalNUM_WRITE_BLOCKS         10

/* For the otaPal_WriteBlock_WriteManyBlocks test this the delay time in ms following
 * the block write loop. */
#define testotapalWRITE_BLOCKS_DELAY_MS    5000

#if otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
    OtaPalStatus_t test_prvPAL_CheckFileSignature( OtaFileContext_t * const C )
    {
        return prvPAL_CheckFileSignature( C );
    }
#endif

/*-----------------------------------------------------------*/
#if otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
    uint8_t * test_prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                    uint32_t * const ulSignerCertSize )

    {
        return otaPal_ReadAndAssumeCertificate( pucCertName, ulSignerCertSize );
    }
#endif

/*
 * @brief: This dummy data is prepended by a SHA1 hash generated from the rsa-sha1-signer
 * certificate and keys in tests/common/ota/test_files.
 *
 * The RSA SHA256 signature and ECDSA 256 signature are generated from this entire data
 * block as is.
 */
static uint8_t ucDummyData[] =
{
    0x83, 0x0b, 0xf0, 0x6a, 0x81, 0xd6, 0xca, 0xd7, 0x08, 0x22, 0x0d, 0x6a,
    0x33, 0xfa, 0x31, 0x9f, 0xa9, 0x5f, 0xb5, 0x26, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0c, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
    0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
};

/* Global static OTA file context used in every test. This context is reset to all zeros
 * before every test. */
static OtaFileContext_t xOtaFile;

#ifdef CC3220sf

/**
 * @brief In the Texas Instruments CC3220(SF) device, we need to restore
 * mcuflashimage.bin to factory following tests that increase the number
 * of security alerts.
 */
    static void prvResetTiCc3220ToFactoryImage()
    {
        int32_t lResult, lRetVal, lExtendedError;
        SlFsRetToFactoryCommand_t xRetToFactoryCommand;

        xRetToFactoryCommand.Operation = SL_FS_FACTORY_RET_TO_IMAGE;
        lResult = sl_FsCtl( ( SlFsCtl_e ) SL_FS_CTL_RESTORE, 0, NULL, ( _u8 * ) &xRetToFactoryCommand, sizeof( SlFsRetToFactoryCommand_t ), NULL, 0, NULL );

        if( ( _i32 ) lResult < 0 )
        {
            /*Pay attention, for this function the lResult is composed from Signed lRetVal & extended error */
            lRetVal = ( _i16 ) lResult >> 16;
            lExtendedError = ( _u16 ) lResult & 0xFFFF;
            LogError( ( "Error SL_FS_FACTORY_RET_TO_IMAGE, 5d, %d", lRetVal, lExtendedError ) );
            return;
        }

        /* Reset the network processor. */
        sl_Stop( 0 );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
        sl_Start( NULL, NULL, NULL );
    }
#endif /* ifdef CC3220sf */

/**
 * @brief Test group definition.
 */
TEST_GROUP( Full_OTA_PAL );

TEST_SETUP( Full_OTA_PAL )
{
    /* Always reset the OTA file context before each test. */
    memset( &xOtaFile, 0, sizeof( xOtaFile ) );
}

TEST_TEAR_DOWN( Full_OTA_PAL )
{
    #ifdef CC3220sf
        prvResetTiCc3220ToFactoryImage();
    #else
        OtaPalStatus_t xOtaStatus;

        /* We want to abort the OTA file after every test. This closes the OtaFile. */
        xOtaStatus = otaPal_Abort( &xOtaFile );

        if( OtaPalSuccess != xOtaStatus )
        {
            LogError( ( "Failed to abort xOtaFile: OtaPalStatus_t=%d", xOtaStatus ) );
        }
    #endif /* ifdef CC3220sf */
}

TEST_GROUP_RUNNER( Full_OTA_PAL )
{
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_CloseFile_ValidSignature );
    /* RUN_TEST_CASE( Full_OTA_PAL, otaPal_CloseFile_NullParameters ); */ /* Not supported yet. */
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_CloseFile_InvalidSignatureBlockWritten );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_CloseFile_InvalidSignatureNoBlockWritten );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_CloseFile_NonexistingCodeSignerCertificate );

    RUN_TEST_CASE( Full_OTA_PAL, otaPal_CreateFileForRx_CreateAnyFile );
    /* RUN_TEST_CASE( Full_OTA_PAL, otaPal_CreateFileForRx_NullParameters ) */ /* Not supported yet. */

    /* Other failure case testing for otaPal_CreateFileForRx() is not possible
     * because the MCU's underlying non-volatile memory will have to be mocked
     * to return failures for creating a new file. */

    /* RUN_TEST_CASE( Full_OTA_PAL, otaPal_Abort_NullFileContext ); */ /* Not supported yet. */
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_Abort_OpenFile );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_Abort_FileWithBlockWritten );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_Abort_NullFileHandle );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_Abort_NonExistentFile );

    /* RUN_TEST_CASE( Full_OTA_PAL, otaPal_WriteBlock_NullParameters ); */ /* Not supported yet. */
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_WriteBlock_WriteSingleByte );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_WriteBlock_WriteManyBlocks );

    /* This test resets the device so it is not valid for an MCU. */
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_ActivateNewImage );

    RUN_TEST_CASE( Full_OTA_PAL, otaPal_SetPlatformImageState_SelfTestImageState );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_SetPlatformImageState_InvalidImageState );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_SetPlatformImageState_UnknownImageState );
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_SetPlatformImageState_RejectImageState );
    /* This test is not supported on WinSim because we simply record the image state passed in into a file. */
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_SetPlatformImageState_AcceptedImageStateButImageNotClosed );

    /* The successful setting of an image with the accepted state is not supported because that requires
     * an image that was written, verified, and rebooted. */


    RUN_TEST_CASE( Full_OTA_PAL, otaPal_GetPlatformImageState_InvalidImageStateFromFileCloseFailure );

    RUN_TEST_CASE( Full_OTA_PAL, prvPAL_ReadAndAssumeCertificate_ExistingFile );
    RUN_TEST_CASE( Full_OTA_PAL, prvPAL_ReadAndAssumeCertificate_NonexistentFile );
    /* RUN_TEST_CASE( Full_OTA_PAL, prvPAL_ReadAndAssumeCertificate_NullParameters ); */ /* Not supported yet. */

    RUN_TEST_CASE( Full_OTA_PAL, prvPAL_CheckFileSignature_ValidSignature );
    RUN_TEST_CASE( Full_OTA_PAL, prvPAL_CheckFileSignature_InvalidSignatureBlockWritten );
    RUN_TEST_CASE( Full_OTA_PAL, prvPAL_CheckFileSignature_InvalidSignatureNoBlockWritten );
    RUN_TEST_CASE( Full_OTA_PAL, prvPAL_CheckFileSignature_NonexistingCodeSignerCertificate );
    /* RUN_TEST_CASE( Full_OTA_PAL, prvPAL_CheckFileSignature_NullParameters ); */ /* Not supported yet. */

    /* This test must run last.  It provisions the code sign certificate,
     * and other tests exercise the non-flash version.*/
    RUN_TEST_CASE( Full_OTA_PAL, otaPal_CloseFile_ValidSignatureKeyInFlash );
}

/**
 * @brief Test otaPal_CloseFile with a valid signature and signature verification
 * certificate. Verify the success.
 */
TEST( Full_OTA_PAL, otaPal_CloseFile_ValidSignature )
{
    OtaPalStatus_t xOtaStatus;
    Sig256_t xSig = { 0 };

    /* We use a dummy file name here because closing the system designated bootable
     * image with content that is not runnable may cause issues. */
    xOtaFile.pFilePath = ( uint8_t * ) ( "test_happy_path_image.bin" );
    xOtaFile.fileSize = sizeof( ucDummyData );
    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails somewhere here. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

        xOtaFile.pSignature = &xSig;
        xOtaFile.pSignature->size = ucValidSignatureLength;
        memcpy( xOtaFile.pSignature->data, ucValidSignature, ucValidSignatureLength );
        xOtaFile.pCertFilepath = ( uint8_t * ) otatestpalCERTIFICATE_FILE;

        xOtaStatus = otaPal_CloseFile( &xOtaFile );
        TEST_ASSERT_EQUAL_INT( OtaPalSuccess, xOtaStatus );
    }
}
#if ( otatestpalREAD_CERTIFICATE_FROM_NVM_WITH_PKCS11 == 1 )
    CK_RV prvImportCodeSigningCertificate( const uint8_t * pucCertificate,
                                           size_t xCertificateLength,
                                           uint8_t * pucLabel )
    {
        /* Find the certificate */
        CK_OBJECT_HANDLE xHandle;
        CK_RV xResult;
        CK_FUNCTION_LIST_PTR xFunctionList;
        CK_SLOT_ID xSlotId;
        CK_ULONG xCount = 1;
        CK_SESSION_HANDLE xSession;
        CK_BBOOL xSessionOpen = CK_FALSE;

        xResult = C_GetFunctionList( &xFunctionList );

        if( CKR_OK == xResult )
        {
            xResult = xInitializePKCS11();
        }

        if( ( CKR_OK == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
        {
            xResult = xFunctionList->C_GetSlotList( CK_TRUE, &xSlotId, &xCount );
        }

        if( CKR_OK == xResult )
        {
            xResult = xFunctionList->C_OpenSession( xSlotId, CKF_SERIAL_SESSION, NULL, NULL, &xSession );
        }

        if( CKR_OK == xResult )
        {
            xSessionOpen = CK_TRUE;
            xResult = xProvisionCertificate( xSession,
                                             ( uint8_t * ) pucCertificate,
                                             xCertificateLength,
                                             pucLabel,
                                             &xHandle );
        }

        if( xSessionOpen == CK_TRUE )
        {
            xResult = xFunctionList->C_CloseSession( xSession );
        }

        return xResult;
    }

#endif /* if ( otatestpalREAD_CERTIFICATE_FROM_NVM_WITH_PKCS11 == 1 ) */

/**
 * @brief Test otaPal_CloseFile with a valid signature and signature verification
 * certificate when the code verification key has already been imported to flash.
 * Verify the success.
 */
TEST( Full_OTA_PAL, otaPal_CloseFile_ValidSignatureKeyInFlash )
{
    #if ( otatestpalREAD_CERTIFICATE_FROM_NVM_WITH_PKCS11 == 1 )
        OtaPalStatus_t xOtaStatus;
        Sig256_t xSig = { 0 };
        CK_RV xResult;

        /* Import the code signing certificate into NVM using the PKCS #11 module. */
        xResult = prvImportCodeSigningCertificate( ( const uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM,
                                                   sizeof( signingcredentialSIGNING_CERTIFICATE_PEM ),
                                                   ( uint8_t * ) pkcs11configLABEL_CODE_VERIFICATION_KEY );
        TEST_ASSERT_EQUAL( CKR_OK, xResult );

        /* We use a dummy file name here because closing the system designated bootable
         * image with content that is not runnable may cause issues. */
        /* xOtaFile.pFilePath = otatestpalFIRMWARE_FILE; */
        xOtaFile.pFilePath = ( uint8_t * ) ( "test_happy_path_image.bin" );
        xOtaFile.fileSize = sizeof( ucDummyData );
        xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
        TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

        /* We still want to close the file if the test fails somewhere here. */
        if( TEST_PROTECT() )
        {
            /* Write data to the file. */
            xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                            0,
                                            ucDummyData,
                                            sizeof( ucDummyData ) );
            TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

            xOtaFile.pSignature = &xSig;
            xOtaFile.pSignature->size = ucValidSignatureLength;
            memcpy( xOtaFile.pSignature->data, ucValidSignature, ucValidSignatureLength );
            /* The PKCS #11 label is used to retrieve the code signing certificate. */
            xOtaFile.pCertFilepath = ( uint8_t * ) pkcs11configLABEL_CODE_VERIFICATION_KEY;

            xOtaStatus = otaPal_CloseFile( &xOtaFile );
            TEST_ASSERT_EQUAL_INT( OtaPalSuccess, xOtaStatus );
        }
    #endif /* if ( otatestpalREAD_CERTIFICATE_FROM_NVM_WITH_PKCS11 == 1 ) */
}


/**
 * @brief Call otaPal_CloseFile with an invalid signature in the file context.
 * The close is called after we have a written a block of dummy data to the file.
 * Verify the correct OTA Agent level error code is returned from otaPal_CloseFile.
 */
TEST( Full_OTA_PAL, otaPal_CloseFile_InvalidSignatureBlockWritten )
{
    OtaPalStatus_t xOtaStatus;
    Sig256_t xSig = { 0 };

    /* Create a local file using the PAL. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData );

    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails somewhere here. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

        /* Fill out an incorrect signature. */
        xOtaFile.pSignature = &xSig;
        xOtaFile.pSignature->size = ucInvalidSignatureLength;
        memcpy( xOtaFile.pSignature->data, ucInvalidSignature, ucInvalidSignatureLength );
        xOtaFile.pCertFilepath = ( uint8_t * ) otatestpalCERTIFICATE_FILE;

        /* Try to close the file. */
        xOtaStatus = otaPal_CloseFile( &xOtaFile );

        if( ( OtaPalBadSignerCert != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
            ( OtaPalSignatureCheckFailed != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
            ( OtaPalFileClose != OTA_PAL_MAIN_ERR( xOtaStatus ) ) )
        {
            TEST_ASSERT_TRUE( 0 );
        }
    }
}

/**
 * @brief Call otaPal_CloseFile with an invalid signature in the file context.
 * The close is called when no blocks have been written to the file.
 * Verify the correct OTA Agent level error code is returned from otaPal_CloseFile.
 */
TEST( Full_OTA_PAL, otaPal_CloseFile_InvalidSignatureNoBlockWritten )
{
    OtaPalStatus_t xOtaStatus;
    Sig256_t xSig = { 0 };

    /* Create a local file using the PAL. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* Fill out an incorrect signature. */
    xOtaFile.pSignature = &xSig;
    xOtaFile.pSignature->size = ucInvalidSignatureLength;
    memcpy( xOtaFile.pSignature->data, ucInvalidSignature, ucInvalidSignatureLength );
    xOtaFile.pCertFilepath = ( uint8_t * ) otatestpalCERTIFICATE_FILE;

    /* We still want to close the file if the test fails somewhere here. */
    if( TEST_PROTECT() )
    {
        /* Try to close the file. */
        xOtaStatus = otaPal_CloseFile( &xOtaFile );

        if( ( OtaPalBadSignerCert != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
            ( OtaPalSignatureCheckFailed != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
            ( OtaPalFileClose != OTA_PAL_MAIN_ERR( xOtaStatus ) ) )
        {
            TEST_ASSERT_TRUE( 0 );
        }
    }
}

/**
 * @brief Call otaPal_CloseFile with a signature verification certificate path does
 * not exist in the system. Verify the correct OTA Agent level error code is returned
 * from otaPal_CloseFile.
 *
 * @note This test is only valid if your device uses a file system in your non-volatile memory.
 * Some devices may revert to using aws_codesigner_certificate.h if a file is not found, but
 * that option is not being enforced.
 */
TEST( Full_OTA_PAL, otaPal_CloseFile_NonexistingCodeSignerCertificate )
{
    #if ( otatestpalUSE_FILE_SYSTEM == 1 )
        OtaPalStatus_t xOtaStatus;
        Sig256_t xSig = { 0 };

        memset( &xOtaFile, 0, sizeof( xOtaFile ) );

        /* Create a local file using the PAL. */
        xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
        xOtaFile.fileSize = sizeof( ucDummyData );

        xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
        TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

        /* We still want to close the file if the test fails somewhere here. */
        if( TEST_PROTECT() )
        {
            /* Write data to the file. */
            xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                            0,
                                            ucDummyData,
                                            sizeof( ucDummyData ) );
            TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

            /* Check the signature (not expected to be valid in this case). */
            xOtaFile.pSignature = &xSig;
            xOtaFile.pSignature->size = ucValidSignatureLength;
            memcpy( xOtaFile.pSignature->data, ucValidSignature, ucValidSignatureLength );
            xOtaFile.pCertFilepath = ( uint8_t * ) ( "nonexistingfile.crt" );

            xOtaStatus = otaPal_CloseFile( &xOtaFile );

            if( ( OtaPalBadSignerCert != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
                ( OtaPalSignatureCheckFailed != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
                ( OtaPalFileClose != OTA_PAL_MAIN_ERR( xOtaStatus ) ) )
            {
                TEST_ASSERT_TRUE( 0 );
            }
        }
    #endif /* if ( otatestpalUSE_FILE_SYSTEM == 1 ) */
}

/**
 * @brief Create a file with any name, and verify success.
 */
TEST( Full_OTA_PAL, otaPal_CreateFileForRx_CreateAnyFile )
{
    OtaPalStatus_t xOtaStatus;

    memset( &xOtaFile, 0, sizeof( OtaFileContext_t ) );

    /* TEST: Create a local file using the PAL. Verify error in close. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );
}

/**
 * @brief Abort on an open file. Verify success.
 */
TEST( Full_OTA_PAL, otaPal_Abort_OpenFile )
{
    OtaPalStatus_t xOtaStatus;

    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;

    /* Create a local file using the PAL. */
    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* Signal that the download is being aborted. */
    xOtaStatus = otaPal_Abort( &xOtaFile );
    TEST_ASSERT_EQUAL_INT( OtaPalSuccess, xOtaStatus );

    /* Verify that the file handle is null. */
    TEST_ASSERT_EQUAL_INT( NULL, xOtaFile.pFile );
}

/**
 * @brief Abort after writing a block to an open file. Verify success.
 */
TEST( Full_OTA_PAL, otaPal_Abort_FileWithBlockWritten )
{
    OtaPalStatus_t xOtaStatus;

    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData );

    /* Create a local file again using the PAL. */
    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails here. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );
    }

    /* Signal that the download is being aborted. */
    xOtaStatus = otaPal_Abort( &xOtaFile );
    TEST_ASSERT_EQUAL_INT( OtaPalSuccess, xOtaStatus );

    /* Verify that the file handle is null. */
    TEST_ASSERT_EQUAL_INT( NULL, xOtaFile.pFile );
}

/**
 * @brief Call otaPal_Abort on a NULL file handle. Verify there is no error.
 */
TEST( Full_OTA_PAL, otaPal_Abort_NullFileHandle )
{
    OtaPalStatus_t xOtaStatus;

    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;

    xOtaFile.pFile = 0;
    xOtaStatus = otaPal_Abort( &xOtaFile );
    TEST_ASSERT_EQUAL_INT( OtaPalSuccess, xOtaStatus );
}

/**
 * @brief Verify there is no error for aborting a file that doesn't exist.
 */
TEST( Full_OTA_PAL, otaPal_Abort_NonExistentFile )
{
    OtaPalStatus_t xOtaStatus;

    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;

    xOtaFile.pFilePath = ( uint8_t * ) ( "nonexistingfile.bin" );
    xOtaStatus = otaPal_Abort( &xOtaFile );
    TEST_ASSERT_EQUAL_INT( OtaPalSuccess, OTA_PAL_MAIN_ERR( xOtaStatus ) );
}

/**
 * Write one byte of data and verify success.
 */
TEST( Full_OTA_PAL, otaPal_WriteBlock_WriteSingleByte )
{
    OtaPalStatus_t xOtaStatus;
    int16_t sNumBytesWritten;
    uint8_t ucData = 0xAA;

    /* TEST: Write a byte of data. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    if( TEST_PROTECT() )
    {
        sNumBytesWritten = otaPal_WriteBlock( &xOtaFile, 0, &ucData, 1 );
        TEST_ASSERT_EQUAL_INT( 1, sNumBytesWritten );
    }
}

/**
 * @brief Write many blocks of data to a file opened in the device. Verify success.
 */
TEST( Full_OTA_PAL, otaPal_WriteBlock_WriteManyBlocks )
{
    OtaPalStatus_t xOtaStatus;
    int16_t sNumBytesWritten;

    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData ) * testotapalNUM_WRITE_BLOCKS;
    /* TEST: Write many bytes of data. */

    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    if( TEST_PROTECT() )
    {
        int lIndex = 0;

        for( lIndex = 0; lIndex < testotapalNUM_WRITE_BLOCKS; lIndex++ )
        {
            sNumBytesWritten = otaPal_WriteBlock( &xOtaFile, lIndex * sizeof( ucDummyData ), ucDummyData, sizeof( ucDummyData ) );
            TEST_ASSERT_EQUAL_INT( sizeof( ucDummyData ), sNumBytesWritten );
        }

        /* Sufficient delay for flash write to complete. */
        vTaskDelay( pdMS_TO_TICKS( testotapalWRITE_BLOCKS_DELAY_MS ) );
    }
}

/**
 * Call otaPal_ActivateNewImage() and verify success. This function is expected to
 * reset the device, so this test is only supported on the Windows Simulator environment.
 * The Windows Simulator environment does not reset.
 */
TEST( Full_OTA_PAL, otaPal_ActivateNewImage )
{
    #ifdef WIN32
        OtaPalStatus_t xOtaStatus = otaPal_ActivateNewImage(&xOtaFile);

        TEST_ASSERT_EQUAL_INT( OtaPalSuccess, xOtaStatus );
    #endif
}

/**
 * @brief Set the platform state to self-test and verify success.
 */
TEST( Full_OTA_PAL, otaPal_SetPlatformImageState_SelfTestImageState )
{
    OtaPalStatus_t xOtaStatus;

    OtaImageState_t eImageState = OtaImageStateUnknown;

    /* Create a local file again using the PAL. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData );

    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

        /* Set the image state. */
        eImageState = OtaImageStateTesting;
        xOtaStatus = otaPal_SetPlatformImageState( eImageState );
        TEST_ASSERT_EQUAL_INT( OtaPalSuccess, xOtaStatus );

        /* Verify that image state was saved correctly. */

        /* [**]All platforms need a reboot of a successfully close image in order to return
         * OtaPalImageStatePendingCommit from otaPal_GetPlatformImageState(). So this cannot be tested.
         */

        /*ePalImageState = otaPal_GetPlatformImageState();
         * TEST_ASSERT_EQUAL_INT( OtaPalImageStatePendingCommit, ePalImageState ); */
    }
}

/**
 * @brief Set an invalid platform image state exceeding the range and verify success.
 */
TEST( Full_OTA_PAL, otaPal_SetPlatformImageState_InvalidImageState )
{
    OtaPalStatus_t xOtaStatus;
    OtaImageState_t eImageState = OtaImageStateUnknown;

    /* Create a local file again using the PAL. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData );

    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

        /* Try to set an invalid image state. */
        eImageState = ( OtaImageState_t ) ( OtaLastImageState + 1 );
        xOtaStatus = otaPal_SetPlatformImageState( &xOtaFile, eImageState );
        TEST_ASSERT_EQUAL( OtaPalBadImageState, OTA_PAL_MAIN_ERR( xOtaStatus ) );
    }
}

/**
 * @brief Set the image state to unknown and verify a failure.
 */
TEST( Full_OTA_PAL, otaPal_SetPlatformImageState_UnknownImageState )
{
    OtaPalStatus_t xOtaStatus;
    OtaImageState_t eImageState = OtaImageStateUnknown;

    /* Create a local file again using the PAL. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData );

    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

        /* Try to set an invalid image state. */
        eImageState = OtaImageStateUnknown;
        xOtaStatus = otaPal_SetPlatformImageState( &xOtaFile, eImageState );
        TEST_ASSERT_EQUAL( OtaPalBadImageState, OTA_PAL_MAIN_ERR( xOtaStatus ) );
    }
}

/**
 * @brief Set the image state to accepted with an image that was never closed. We are
 * expecting an error in this case.
 * Verify the correct OTA Agent level error code is returned from otaPal_SetPlatformImageState.
 */
TEST( Full_OTA_PAL, otaPal_SetPlatformImageState_AcceptedImageStateButImageNotClosed )
{
    #ifndef WIN32
        OtaPalStatus_t xOtaStatus;
        OtaImageState_t eImageState = OtaImageStateUnknown;

        /* Create a local file again using the PAL. */
        xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
        xOtaFile.fileSize = sizeof( ucDummyData );

        xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
        TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

        /* We still want to close the file if the test fails. */
        if( TEST_PROTECT() )
        {
            /* Write data to the file. */
            xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                            0,
                                            ucDummyData,
                                            sizeof( ucDummyData ) );
            TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

            eImageState = OtaImageStateAccepted;
            xOtaStatus = otaPal_SetPlatformImageState( &xOtaFile, eImageState );
            TEST_ASSERT_EQUAL_INT( OtaPalCommitFailed, OTA_PAL_MAIN_ERR( xOtaStatus ) );
        }
    #endif /* ifndef WIN32 */
}

/**
 * @brief Set platform image state to rejected and verify success.
 * We cannot test a reject failed without mocking the underlying non volatile memory.
 */
TEST( Full_OTA_PAL, otaPal_SetPlatformImageState_RejectImageState )
{
    OtaPalStatus_t xOtaStatus;
    OtaImageState_t eImageState = OtaImageStateUnknown;

    /* Create a local file again using the PAL. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData );

    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

        eImageState = OtaImageStateRejected;
        xOtaStatus = otaPal_SetPlatformImageState( &xOtaFile, eImageState );
        TEST_ASSERT_EQUAL_INT( OtaPalSuccess, OTA_PAL_MAIN_ERR( xOtaStatus ) );
    }
}

/**
 * @brief Set the platform image state to aborted.
 * We cannot test a abort failed without mocking the underlying non-volatile memory.
 */
TEST( Full_OTA_PAL, otaPal_SetPlatformImageState_AbortImageState )
{
    OtaPalStatus_t xOtaStatus;
    OtaImageState_t eImageState = OtaImageStateUnknown;

    /* Create a local file again using the PAL. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData );

    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

        eImageState = OtaImageStateAborted;
        xOtaStatus = otaPal_SetPlatformImageState( &xOtaFile, eImageState );
        TEST_ASSERT_EQUAL_INT( OtaPalSuccess, OTA_PAL_MAIN_ERR( xOtaStatus ) );
    }
}

/**
 * @brief Verify that the current image received is in the invalid state after a
 * failure to close the file because of a bad signature.
 */
TEST( Full_OTA_PAL, otaPal_GetPlatformImageState_InvalidImageStateFromFileCloseFailure )
{
    OtaPalStatus_t xOtaStatus;
    Sig256_t xSig = { 0 };
    OtaPalImageState_t ePalImageState = OtaPalImageStateUnknown;

    /* TEST: Invalid image returned from otaPal_GetPlatformImageState(). Using a failure to close. */
    /* Create a local file again using the PAL. */
    xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
    xOtaFile.fileSize = sizeof( ucDummyData );

    xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
    TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

    /* We still want to close the file if the test fails. */
    if( TEST_PROTECT() )
    {
        /* Write data to the file. */
        xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                        0,
                                        ucDummyData,
                                        sizeof( ucDummyData ) );
        TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

        /* Check the signature. */
        xOtaFile.pSignature = &xSig;
        xOtaFile.pSignature->size = ucInvalidSignatureLength;
        memcpy( xOtaFile.pSignature->data, ucInvalidSignature, ucInvalidSignatureLength );
        xOtaFile.pCertFilepath = ( uint8_t * ) otatestpalCERTIFICATE_FILE;

        xOtaStatus = otaPal_CloseFile( &xOtaFile );

        if( ( OtaPalBadSignerCert != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
            ( OtaPalSignatureCheckFailed != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
            ( OtaPalFileClose != OTA_PAL_MAIN_ERR( xOtaStatus ) ) )
        {
            TEST_ASSERT_TRUE( 0 );
        }

        /* The file failed to close, so it is invalid or in an unknown state. */
        ePalImageState = otaPal_GetPlatformImageState( &xOtaFile );
        TEST_ASSERT( ( OtaPalImageStateInvalid == ePalImageState ) || ( OtaPalImageStateUnknown == ePalImageState ) );
    }
}



/**
 * @brief Call prvPAL_ReadAndAssumeCertificate with an existing certificate file
 * path. Some OTA PAL implementations may use aws_codesigner_certificate.h, in
 * which case this test should still pass.
 */
TEST( Full_OTA_PAL, prvPAL_ReadAndAssumeCertificate_ExistingFile )
{
    #if ( otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED == 1 )
        uint8_t * pucSignerCert = NULL;
        uint8_t * pucCertName = ( uint8_t * ) otatestpalCERTIFICATE_FILE;
        uint32_t ulSignerCertSize = 0;

        pucSignerCert = test_prvPAL_ReadAndAssumeCertificate( pucCertName, &ulSignerCertSize );

        /* Verify that the signer certificate returned is not NULL. */
        TEST_ASSERT_MESSAGE( pucSignerCert != NULL, "The returned certificate is NULL." );

        /* Verify the signer certificate size. The certificates are one of the expected certificates
         * in test/common/ota/test_files. */
        TEST_ASSERT_GREATER_THAN( 0, ulSignerCertSize );

        /* Free the certificate file memory. */
        vPortFree( pucSignerCert );
    #endif /* if ( otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED == 1 ) */
}

/**
 * @brief Call test_prvPAL_ReadAndAssumeCertificate with a non existing certificate file
 * path. This test is valid only for devices that abstract their non-volatile memory
 * with a file system.
 */
TEST( Full_OTA_PAL, prvPAL_ReadAndAssumeCertificate_NonexistentFile )
{
    #if ( ( otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED == 1 ) && ( otatestpalUSE_FILE_SYSTEM == 1 ) )
        uint8_t * pucSignerCert = NULL;
        uint8_t * pucCertName = ( uint8_t * ) ( "non-existing-file.pem" );
        uint32_t ulSignerCertSize = 0;

        pucSignerCert = test_prvPAL_ReadAndAssumeCertificate( pucCertName, &ulSignerCertSize );

        /* Verify that the signer certificate returned is NULL. */
        TEST_ASSERT_MESSAGE( pucSignerCert == NULL, "The returned certificate is NULL." );
        /* Verify the signer certificate size is zero. */
        TEST_ASSERT_EQUAL_INT( 0, ulSignerCertSize );
    #endif /* if ( otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED == 1 ) */
}


/**
 * @brief Call prvPAL_CheckFileSignature with a valid signature on the data block written and
 * expect no errors are returned.
 */
TEST( Full_OTA_PAL, prvPAL_CheckFileSignature_ValidSignature )
{
    #if ( otatestpalCHECK_FILE_SIGNATURE_SUPPORTED == 1 )
        OtaPalStatus_t xOtaStatus;
        Sig256_t xSig = { 0 };

        /* Create a local file using the PAL. */
        xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
        xOtaFile.fileSize = sizeof( ucDummyData );

        xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
        TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

        /* We still want to close the file if the test fails. */
        if( TEST_PROTECT() )
        {
            /* Write data to the file. */
            xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                            0,
                                            ucDummyData,
                                            sizeof( ucDummyData ) );
            TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

            xOtaFile.pSignature = &xSig;
            xOtaFile.pSignature->size = ucValidSignatureLength;
            memcpy( xOtaFile.pSignature->data, ucValidSignature, ucValidSignatureLength );
            xOtaFile.pCertFilepath = ( uint8_t * ) otatestpalCERTIFICATE_FILE;

            xOtaStatus = test_prvPAL_CheckFileSignature( &xOtaFile );
            TEST_ASSERT_EQUAL_INT( OtaPalSuccess, xOtaStatus );
        }
    #endif /* if ( otatestpalCHECK_FILE_SIGNATURE_SUPPORTED == 1 ) */
}

/**
 * @brief prvPAL_CheckFileSignature with an invalid signature on the data block
 * written. Verify the signature check fails.
 */
TEST( Full_OTA_PAL, prvPAL_CheckFileSignature_InvalidSignatureBlockWritten )
{
    #if ( otatestpalCHECK_FILE_SIGNATURE_SUPPORTED == 1 )
        OtaPalStatus_t xOtaStatus;
        Sig256_t xSig = { 0 };

        /* Create a local file using the PAL. */
        xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
        xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
        TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

        /* We still want to close the file if the test fails. */
        if( TEST_PROTECT() )
        {
            /* Write data to the file. */
            xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                            0,
                                            ucDummyData,
                                            sizeof( ucDummyData ) );
            TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

            /* Fill out an incorrect signature. */
            xOtaFile.pSignature = &xSig;
            xOtaFile.pSignature->size = ucInvalidSignatureLength;
            memcpy( xOtaFile.pSignature->data, ucInvalidSignature, ucInvalidSignatureLength );
            xOtaFile.pCertFilepath = ( uint8_t * ) otatestpalCERTIFICATE_FILE;

            /* Check the signature. */
            xOtaStatus = test_prvPAL_CheckFileSignature( &xOtaFile );

            if( ( OtaPalBadSignerCert != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
                ( OtaPalSignatureCheckFailed != OTA_PAL_MAIN_ERR( xOtaStatus ) ) )
            {
                TEST_ASSERT_TRUE( 0 );
            }
        }
    #endif /* if ( otatestpalCHECK_FILE_SIGNATURE_SUPPORTED == 1 ) */
}

TEST( Full_OTA_PAL, prvPAL_CheckFileSignature_InvalidSignatureNoBlockWritten )
{
    #if ( otatestpalCHECK_FILE_SIGNATURE_SUPPORTED == 1 )
        OtaPalStatus_t xOtaStatus;
        Sig256_t xSig = { 0 };

        /* Create a local file using the PAL. */
        xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
        xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
        TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

        /* We still want to close the file if the test fails. */
        if( TEST_PROTECT() )
        {
            /* Fill out an incorrect signature. */
            xOtaFile.pSignature = &xSig;
            xOtaFile.pSignature->size = ucInvalidSignatureLength;
            memcpy( xOtaFile.pSignature->data, ucInvalidSignature, ucInvalidSignatureLength );
            xOtaFile.pCertFilepath = ( uint8_t * ) otatestpalCERTIFICATE_FILE;

            /* Try to close the file. */
            xOtaStatus = otaPal_CloseFile( &xOtaFile );

            if( ( OtaPalBadSignerCert != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
                ( OtaPalSignatureCheckFailed != OTA_PAL_MAIN_ERR( xOtaStatus ) ) )
            {
                TEST_ASSERT_TRUE( 0 );
            }
        }
    #endif /* if ( otatestpalCHECK_FILE_SIGNATURE_SUPPORTED == 1 ) */
}

TEST( Full_OTA_PAL, prvPAL_CheckFileSignature_NonexistingCodeSignerCertificate )
{
    #if ( ( otatestpalCHECK_FILE_SIGNATURE_SUPPORTED == 1 ) && ( otatestpalUSE_FILE_SYSTEM == 1 ) )
        OtaPalStatus_t xOtaStatus;
        Sig256_t xSig = { 0 };

        /* Create a local file using the PAL. */
        xOtaFile.pFilePath = ( uint8_t * ) otatestpalFIRMWARE_FILE;
        xOtaStatus = otaPal_CreateFileForRx( &xOtaFile );
        TEST_ASSERT_EQUAL( OtaPalSuccess, xOtaStatus );

        /* We still want to close the file if the test fails. */
        if( TEST_PROTECT() )
        {
            /* Write data to the file. */
            xOtaStatus = otaPal_WriteBlock( &xOtaFile,
                                            0,
                                            ucDummyData,
                                            sizeof( ucDummyData ) );
            TEST_ASSERT_EQUAL( sizeof( ucDummyData ), xOtaStatus );

            /* Check the signature (not expected to be valid in this case). */
            xOtaFile.pSignature = &xSig;
            xOtaFile.pSignature->size = ucValidSignatureLength;
            memcpy( xOtaFile.pSignature->data, ucValidSignature, ucValidSignatureLength );
            xOtaFile.pCertFilepath = ( uint8_t * ) ( "nonexistingfile.crt" );

            xOtaStatus = test_prvPAL_CheckFileSignature( &xOtaFile );

            if( ( OtaPalBadSignerCert != OTA_PAL_MAIN_ERR( xOtaStatus ) ) &&
                ( OtaPalSignatureCheckFailed != OTA_PAL_MAIN_ERR( xOtaStatus ) ) )
            {
                TEST_ASSERT_TRUE( 0 );
            }
        }
    #endif /* if ( ( otatestpalCHECK_FILE_SIGNATURE_SUPPORTED == 1 ) && ( otatestpalUSE_FILE_SYSTEM == 1 ) ) */
}
