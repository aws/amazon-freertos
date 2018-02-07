/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifdef INCLUDE_FROM_OTA_AGENT

#include "aws_ota_types.h"
#include "aws_ota_agent.h"


/** 
 * @brief Abort an OTA transfer. 
 * 
 * Aborts access to an existing open file represented by the OTA file context C. This is only valid
 * for jobs that started successfully.
 * 
 * @param[in] C OTA file context information.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent 
 * error codes information in aws_ota_agent.h.
 */
static OTA_Err_t prvAbort(OTA_FileContext_t * const C);

/** 
 * @brief Create a new receive file for the data chunks as they come in. 
 * 
 * @note Opens the file indicated in the OTA file context in the MCU file system.
 * 
 * @param[in] C OTA file context information.
 * 
 * @return pdTRUE if succeeded, pdFALSE otherwise.
 */
static bool_t prvCreateFileForRx(OTA_FileContext_t * const C);

/* @brief Authenticate and close the underlying receive file in the specified OTA context.
 * 
 * @param[in] C OTA file context information.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent 
 * error codes information in aws_ota_agent.h.
 */
static OTA_Err_t prvCloseFile(OTA_FileContext_t * const C);

/** 
 * @brief Authenticate the received file using its signature and signing certificate in the specified OTA context.
 * 
 * @note Optional on some platforms.
 * 
 * @param[in] C OTA file context information.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent 
 * error codes information in aws_ota_agent.h.
 */
static OTA_Err_t prvCheckFileSignature(OTA_FileContext_t * const C);

/**
 * @brief Write a block of data to the specified file at the given offset.
 * 
 * @param[in] C OTA file context information.
 * @param[in] iOffset Byte offset to write to from the beginning of the file.
 * @param[in] pacData Pointer to the byte array of data to write.
 * @param[in] iBlockSize The number of bytes to write.
 * 
 * @return The number of bytes written on a success, or a negative error code from the MCU abstraction layer. 
 */
static int16_t prvWriteBlock(OTA_FileContext_t * const C, int32_t iOffset, uint8_t * const pacData, uint32_t iBlockSize);

/** 
 * @brief Activate the newest MCU image received via OTA.
 * 
 * This function shall reset the MCU and cause a reboot of the system to execute the newly updated 
 * firmware.
 * 
 * @note This function MAY not return.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent 
 * error codes information in aws_ota_agent.h.
 */
static OTA_Err_t prvActivateNewImage(void);

/**
 * @brief sets the state of the OTA Image in the MCU file system.
 * 
 * Do whatever is required by the platform to Accept/Reject the last firmware image (or bundle).
 * Refer to the platform implementation to determine what happens on your platform.
 * 
 * @param[in] eState The state of the OTA Image.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent 
 * error codes information in aws_ota_agent.h.
 */
static OTA_Err_t prvSetImageState (OTA_ImageState_t eState);

/**
 * @brief Read and return the signer certificate.
 * 
 * Read the specified signer certificate from the file system
 * and return it to the caller. 
 * 
 * @note Optional on some platforms.
 * 
 * @param[in] pucCertName The name of the signer certifiate.
 * @param[in] lSignerCertSize The size of the signer certifiate.
 * 
 * @return A pointer to the byte array of the signer certificate from the system.
 */
static uint8_t * prvReadAndAssumeCertificate(const uint8_t * const pucCertName, int32_t * const lSignerCertSize);

#endif	/* INCLUDE_FROM_OTA_AGENT */
