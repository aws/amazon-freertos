/*
 * FreeRTOS V202012.00
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
 * @file  ota_pal_test.h
 * @brief Function declarations for the OTA PAL functions required for the
 *        qualification tests. Also includes example certificates that are
 *        used during testing. Which certificate is used depends on the
 *        configuration in the ota test config file.
 */

#ifndef OTA_PAL_TEST_H_
#define OTA_PAL_TEST_H_

#include "ota.h"
#include "aws_test_ota_config.h"

/**
 * @brief Abort an OTA transfer.
 *
 * Aborts access to an existing open file represented by the OTA file context pFileContext. This is
 * only valid for jobs that started successfully.
 *
 * @note The input OtaFileContext_t pFileContext is checked for NULL by the OTA agent before this
 * function is called.
 * This function may be called before the file is opened, so the file pointer pFileContext->fileHandle
 * may be NULL when this function is called.
 *
 * @param[in] pFileContext OTA file context information.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in ota.h.
 *
 * The file pointer will be set to NULL after this function returns.
 * OtaPalSuccess is returned when aborting access to the open file was successful.
 * OtaPalFileAbort is returned when aborting access to the open file context was unsuccessful.
 */
OtaPalStatus_t otaPal_Abort( OtaFileContext_t * const C );

/**
 * @brief Create a new receive file for the data chunks as they come in.
 *
 * @note Opens the file indicated in the OTA file context in the MCU file system.
 *
 * @note The previous image may be present in the designated image download partition or file, so the
 * partition or file must be completely erased or overwritten in this routine.
 *
 * @note The input OtaFileContext_t pFileContext is checked for NULL by the OTA agent before this
 * function is called.
 * The device file path is a required field in the OTA job document, so pFileContext->pFilePath is
 * checked for NULL by the OTA agent before this function is called.
 *
 * @param[in] pFileContext OTA file context information.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in ota.h.
 *
 * OtaPalSuccess is returned when file creation is successful.
 * OtaPalRxFileTooLarge is returned if the file to be created exceeds the device's non-volatile memory size constraints.
 * OtaPalBootInfoCreateFailed is returned if the bootloader information file creation fails.
 * OtaPalRxFileCreateFailed is returned for other errors creating the file in the device's non-volatile memory.
 */
OtaPalStatus_t otaPal_CreateFileForRx( OtaFileContext_t * const C );

/**
 * @brief Authenticate and close the underlying receive file in the specified OTA context.
 *
 * @note The input OtaFileContext_t pFileContext is checked for NULL by the OTA agent before this
 * function is called. This function is called only at the end of block ingestion.
 * prvPAL_CreateFileForRx() must succeed before this function is reached, so
 * pFileContext->fileHandle(or pFileContext->pFile) is never NULL.
 * The certificate path on the device is a required job document field in the OTA Agent,
 * so pFileContext->pCertFilepath is never NULL.
 * The file signature key is required job document field in the OTA Agent, so pFileContext->pSignature will
 * never be NULL.
 *
 * If the signature verification fails, file close should still be attempted.
 *
 * @param[in] pFileContext OTA file context information.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in ota.h.
 *
 * OtaPalSuccess is returned on success.
 * OtaPalSignatureCheckFailed is returned when cryptographic signature verification fails.
 * OtaPalBadSignerCert is returned for errors in the certificate itself.
 * OtaPalFileClose is returned when closing the file fails.
 */
OtaPalStatus_t otaPal_CloseFile( OtaFileContext_t * const C );

/**
 * @brief Write a block of data to the specified file at the given offset.
 *
 * @note The input OtaFileContext_t pFileContext is checked for NULL by the OTA agent before this
 * function is called.
 * The file pointer/handle pFileContext->pFile, is checked for NULL by the OTA agent before this
 * function is called.
 * pData is checked for NULL by the OTA agent before this function is called.
 * blockSize is validated for range by the OTA agent before this function is called.
 * offset is validated by the OTA agent before this function is called.
 *
 * @param[in] pFileContext OTA file context information.
 * @param[in] offset Byte offset to write to from the beginning of the file.
 * @param[in] pData Pointer to the byte array of data to write.
 * @param[in] blockSize The number of bytes to write.
 *
 * @return The number of bytes written on a success, or a negative error code from the platform
 * abstraction layer.
 */
int16_t otaPal_WriteBlock( OtaFileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pcData,
                           uint32_t ulBlockSize );

/**
 * @brief Activate the newest MCU image received via OTA.
 *
 * This function shall do whatever is necessary to activate the newest MCU
 * firmware received via OTA. It is typically just a reset of the device.
 *
 * @note This function SHOULD not return. If it does, the platform does not support
 * an automatic reset or an error occurred.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in ota.h.
 */
OtaPalStatus_t otaPal_ActivateNewImage( OtaFileContext_t * const C );

/**
 * @brief Reset the device.
 *
 * This function shall reset the MCU and cause a reboot of the system.
 *
 * @note This function SHOULD not return. If it does, the platform does not support
 * an automatic reset or an error occurred.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in ota.h.
 */
OtaPalStatus_t otaPal_ResetDevice( OtaFileContext_t * const C );

/**
 * @brief Attempt to set the state of the OTA update image.
 *
 * Do whatever is required by the platform to Accept/Reject the OTA update image (or bundle).
 * Refer to the PAL implementation to determine what happens on your platform.
 *
 * @param[in] pFileContext File context of type OtaFileContext_t.
 *
 * @param[in] eState The desired state of the OTA update image.
 *
 * @return The OtaPalStatus_t error code combined with the MCU specific error code. See ota.h for
 *         OTA major error codes and your specific PAL implementation for the sub error code.
 *
 * Major error codes returned are:
 *
 *   OtaPalSuccess on success.
 *   OtaPalBadImageState: if you specify an invalid OtaImageState_t. No sub error code.
 *   OtaPalAbortFailed: failed to roll back the update image as requested by OtaImageStateAborted.
 *   OtaPalRejectFailed: failed to roll back the update image as requested by OtaImageStateRejected.
 *   OtaPalCommitFailed: failed to make the update image permanent as requested by OtaImageStateAccepted.
 */
OtaPalStatus_t otaPal_SetPlatformImageState( OtaFileContext_t * const C,
                                             OtaImageState_t eState );

/**
 * @brief Get the state of the OTA update image.
 *
 * We read this at OTA_Init time and when the latest OTA job reports itself in self
 * test. If the update image is in the "pending commit" state, we start a self test
 * timer to assure that we can successfully connect to the OTA services and accept
 * the OTA update image within a reasonable amount of time (user configurable). If
 * we don't satisfy that requirement, we assume there is something wrong with the
 * firmware and automatically reset the device, causing it to roll back to the
 * previously known working code.
 *
 * If the update image state is not in "pending commit," the self test timer is
 * not started.
 *
 * @param[in] pFileContext File context of type OtaFileContext_t.
 *
 * @return An OtaPalImageState_t. One of the following:
 *   OtaPalImageStatePendingCommit (the new firmware image is in the self test phase)
 *   OtaPalImageStateValid         (the new firmware image is already committed)
 *   OtaPalImageStateInvalid       (the new firmware image is invalid or non-existent)
 *
 *   NOTE: OtaPalImageStateUnknown should NEVER be returned and indicates an implementation error.
 */
OtaPalImageState_t otaPal_GetPlatformImageState( OtaFileContext_t * const C );

/*
 * PEM-encoded code signer certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
#if ( otatestSIG_METHOD == otatestSIG_SHA256_ECDSA )
    static const char signingcredentialSIGNING_CERTIFICATE_PEM[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIBXDCCAQOgAwIBAgIJAPMhJT8l0C6AMAoGCCqGSM49BAMCMCExHzAdBgNVBAMM\n"
        "FnRlc3Rfc2lnbmVyQGFtYXpvbi5jb20wHhcNMTgwNjI3MjAwNDQyWhcNMTkwNjI3\n"
        "MjAwNDQyWjAhMR8wHQYDVQQDDBZ0ZXN0X3NpZ25lckBhbWF6b24uY29tMFkwEwYH\n"
        "KoZIzj0CAQYIKoZIzj0DAQcDQgAEyza/tGLVbVxhL41iYtC8D6tGEvAHu498gNtq\n"
        "DtPsKaoR3t5xQx+6zdWiCi32fgFT2vkeVAmX3pf/Gl8nIP48ZqMkMCIwCwYDVR0P\n"
        "BAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUFBwMDMAoGCCqGSM49BAMCA0cAMEQCIDkf\n"
        "83Oq8sOXhSyJCWAN63gc4vp9//RFCXh/hUXPYcTWAiBgmQ5JV2MZH01Upi2lMflN\n"
        "YLbC+lYscwcSlB2tECUbJA==\n"
        "-----END CERTIFICATE-----\n";
#elif ( otatestSIG_METHOD == otatestSIG_SHA256_RSA )
    static const char signingcredentialSIGNING_CERTIFICATE_PEM[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIC6TCCAdGgAwIBAgIJAJmG5mbNWlukMA0GCSqGSIb3DQEBCwUAMCExHzAdBgNV\n"
        "BAMMFnRlc3Rfc2lnbmVyQGFtYXpvbi5jb20wHhcNMTgwNjI3MjAwMDIyWhcNMTkw\n"
        "NjI3MjAwMDIyWjAhMR8wHQYDVQQDDBZ0ZXN0X3NpZ25lckBhbWF6b24uY29tMIIB\n"
        "IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt8SVb/B4pRHAsxtK5Xcm1iBK\n"
        "BIpiUc/ovUPzhGVlvkeZJfV2hrHc8PJx+XMyxp/MQGI1nr0AfHKkmAbisfU15/ta\n"
        "foY1D9Skomlp+QKCK4UvJCKRl/vgdgKtZ6+vEls4tguBmt0pwQsEhNP2TDUA47uq\n"
        "n7wcvgEwrrDLa9cmP7bfxRbVTWCKdchllOnRgLc05lQpRGxyMaB8zsgmx27AJlJI\n"
        "rXFmnSiOKKGo6Di9/UQTLTzC+t6YfajMSpq+Fe+td7hv/vSsZT+5yLSpJDwmHTp5\n"
        "66AljEEv0vfcJ72dStacEoOKM/qX4dfQfQOF3nyeP6ofgakc+zEjncQPCoCLUQID\n"
        "AQABoyQwIjALBgNVHQ8EBAMCB4AwEwYDVR0lBAwwCgYIKwYBBQUHAwMwDQYJKoZI\n"
        "hvcNAQELBQADggEBAHjgDAj+U4SpHIaxfvKuNxxPiGnyUSWyXS2h5HiPixL85psS\n"
        "k5hvbZaFJwRICSX1dU9EAf2sQN+s2MCLKPnm8vzKWLkyEUzWScr7C/mVXw8cEmdI\n"
        "rd3uzsN4IThHxHiwhPXu2IjjoIjSXLHcTKyZR2uHaifXbVEEwmUimg/49of02eIa\n"
        "ouLySI/+zcLgNFtTklRdSUxxGVK9y47ufiVb+sErzfDBpYx7CwZg0d+wPgfbuYE2\n"
        "8GMrCwFUbsFGMvhscPwG51rFLNonWh+ThZIwHrBwnVXmsDXayBlr6wCA50zGfBQb\n"
        "SXVRXIv/SKpj1k1slnf9IJLfArH6F0Ctjcef/Aw=\n"
        "-----END CERTIFICATE-----";
#elif ( otatestSIG_METHOD == otatestSIG_SHA1_RSA )
    static const char signingcredentialSIGNING_CERTIFICATE_PEM[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDPDCCAiSgAwIBAgIBATANBgkqhkiG9w0BAQUFADB8MQswCQYDVQQGEwJJTDEP\n"
        "MA0GA1UECAwGU2hhcm9uMRAwDgYDVQQHDAdSYWFuYW5hMR8wHQYDVQQKDBZUZXhh\n"
        "cyBJbnN0cnVtZW50cyBJbmMuMQwwCgYDVQQLDANSTkQxGzAZBgNVBAMMEmR1bW15\n"
        "LXJvb3QtY2EtY2VydDAeFw0xODA2MjcxOTI4NTNaFw0xOTA2MjcxOTI4NTNaMCEx\n"
        "HzAdBgNVBAMMFnRlc3Rfc2lnbmVyQGFtYXpvbi5jb20wggEiMA0GCSqGSIb3DQEB\n"
        "AQUAA4IBDwAwggEKAoIBAQC5craIexo46oq3OTj+3fLz17MCtw/icXku8sJ16el0\n"
        "29Cibq4u8bdJeDnGlc42UySw0UlodWnzd5I52tXoXcQZs++m6uQ3JvvRHFXpNuV6\n"
        "Cyo//AdQ2MoqdKK8/pZqu4X71/xXHcIMBb3kS1RSkunJB65mPuPOfjcem6IXDtRk\n"
        "E346aN6wMfeIlmwOoFK4RBoWse459ELB5aq0tSwUmHv7UcyHYQf4Cxkz5HjWfx0y\n"
        "NYKCR4tPoudjg8E9R6uqik7DFlqrijNW2XdXS67d/cVToDJFn1SnYcHStARAoZUK\n"
        "gP0JwXTapO31QThi9q+q63FETBcnEQ29rfmeN1/pBOI/AgMBAAGjJDAiMAsGA1Ud\n"
        "DwQEAwIHgDATBgNVHSUEDDAKBggrBgEFBQcDAzANBgkqhkiG9w0BAQUFAAOCAQEA\n"
        "W29xvDV66B7NYAOSqXIG11JH+Pk2GFzZl5gNehkcDPPMHumBRNHHZqVAO4mah3Ej\n"
        "StqWmTXnIs5YKX9Ro7ch3NfKHMt4/JFvR7v9VolVIVwZZB+AQU1BD8OAGlHNEq28\n"
        "TbOdkpdv9Q69W4TVsqXAkVhONekLkEJQTZyhW7db28nb/LizftfN4ps+uuE2Xl9c\n"
        "YHmgWb/xqi9NIcsyQL08urJVCnyGuLQgj+GfitELFsCfc3ohhacNENsXupRIOz08\n"
        "NCa9WuCyk5uwoo6mn6JIErBMLqLTBcs82vq9d7WIFHf4QpgTs2FuelY/Hyw7HRFo\n"
        "Ml3tXnR4B4lqeJy/BP6/GA==\n"
        "-----END CERTIFICATE-----";
#else /* if ( otatestSIG_METHOD == otatestSIG_SHA256_ECDSA ) */
    static const char signingcredentialSIGNING_CERTIFICATE_PEM[] = "Paste code signing certificate here.";
#endif /* if ( otatestSIG_METHOD == otatestSIG_SHA256_ECDSA ) */



#endif /* ifndef OTA_PAL_TEST_H_ */
