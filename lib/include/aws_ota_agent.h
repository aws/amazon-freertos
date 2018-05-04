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

/**
 * @file aws_ota_agent.h
 * @brief OTA Agent Interface
 */

#ifndef AWS_TS_OTA_AGENT_H_
#define AWS_TS_OTA_AGENT_H_

/* Type definitions for OTA Agent */
#include "aws_ota_types.h"

/* Includes required by the FreeRTOS timers structure. */
#include "FreeRTOS.h"
#include "timers.h"

/**
 * @brief Special OTA Agent printing definition.
 */
#define OTA_PRINT vLoggingPrintf

/** 
 * @brief OTA Agent states.
 * 
 * The current state of the OTA Task (OTA Agent). 
 * 
 * @note There is currently support only for a single OTA context.
 */
typedef enum {
	eOTA_AgentState_NotReady = 0,	/*!< The OTA agent task is not running. */
	eOTA_AgentState_Ready,		  	/*!< The OTA agent task is currently running. */
	eOTA_AgentState_ShuttingDown,	/*!< The OTA agent task is performing shut down activities. */
} OTA_State_t;

/**
 * @brief OTA Error type.
 */
typedef uint32_t OTA_Err_t;

/**
 * @defgroup OTA Error code operation helpers.
 * @brief Helper constants for extracting the error code from the OTA error returned.
 *  
 * The OTA error codes consist of an agent code in the upper 8 bits of a 32 bit word and sometimes 
 * merged with a platform specific code in the lower 24 bits. You must refer to the platform PAL 
 * layer in use to determine the meaning of the lower 24 bits.
 */
#define kOTA_PAL_ErrMask 0xffffff		/*!< The PAL layer uses the signed low 24 bits of the OTA error code. */
#define kOTA_MainErrShiftDownBits 24	/*!< The OTA Agent error code is the highest 8 bits of the word. */

/**
 * @defgroup OTA Agent error codes.
 * @brief Error codes returned by OTA agent API.
 * 
 * @note OTA agent error codes are in the upper 8 bits of the 32 bit OTA error word, OTA_Err_t.
 */
#define kOTA_Err_None					0x00000000
#define kOTA_Err_SignatureCheckFailed	0x01000000		/*!< The signature check failed for the specified file. */
#define kOTA_Err_BadSignerCert			0x02000000		/*!< The signer certificate was not readable or zero length. */
#define kOTA_Err_OutOfMemory			0x03000000		/*!< General out of memory error. */
#define kOTA_Err_ActivateFailed			0x04000000		/*!< The activation of the new OTA image failed. */
#define kOTA_Err_CommitFailed			0x05000000		/*!< The acceptance commit of the new OTA image failed. */
#define kOTA_Err_PublishFailed			0x06000000		/*!< Attempt to publish a MQTT message failed. */
#define kOTA_Err_BadFinalState			0x07000000		/*!< The specified final OTA image state was out of range. */
#define kOTA_Err_NoActiveJob			0x08000000		/*!< Attempt to set final image state without an active job. */
#define kOTA_Err_FileAbort				0x10000000		/*!< Error in low level file abort. */
#define kOTA_Err_FileClose				0x11000000		/*!< Error in low level file close. */
#define kOTA_Err_NullFilePtr			0x20000000		/*!< Attempt to use a null file pointer. */
#define kOTA_Err_SoftwareBug			0x21000000		/*!< An unexpected software error occurred. */
#define kOTA_Err_MomentumAbort          0x22000000		/*!< Too many OTA stream requests without any response. */

/**
 * @defgroup OTA operator flags.
 * @brief OTA operator flags indicating special active file transfer characteristics.
 *  
 * These are the OTA operator (user) flags to indicate specific characteristics of the active file 
 * transfer. For example, it may indicate that the file is the MCU firmware image and should be made
 * active when successfully transferred.
 */
#define kOTA_Flags_MCUFirmware		0x00000001		/*!< The file is the MCU firmware. */
#define kOTA_Flags_Signature		0x00000002		/*!< The file is the signature of the firmware image. */
#define kOTA_Flags_Bundle			0x00000004		/*!< The file is one of several in a bundle. All must pass or none are kept. */
#define kOTA_Flags_Secure			0x00000008		/*!< This file should be a secure file in the file system. */

/**
 * @brief OTA Image states.
 *  
 * When an OTA MCU image receive is complete, it is technically moved to the Self Test state, 
 * pending final acceptance. After the image is activated and tested, the user should put it into 
 * either the Accepted or Rejected state. If the image is accepted, it becomes the main image to be 
 * booted from then on. If it is rejected, the image is no longer valid and shall not be used, 
 * effectively reverting to the last known good image.
 */
typedef enum {
	eOTA_ImageState_Unknown = 0,	/*!< The initial state of the OTA MCU Image. */
	eOTA_ImageState_Testing,		/*!< The state of the OTA MCU Image post successful download and reboot. */
	eOTA_ImageState_Accepted,		/*!< The state of the OTA MCU Image post successful download and successful self_test. */
	eOTA_ImageState_Rejected,		/*!< The state of the OTA MCU Image when the job has been rejected. */
	eOTA_ImageState_Aborted,		/*!< The state of the OTA MCU Image after a timeout publish to the stream request fails. 
									 * Also if the OTA MCU image is aborted in the middle of a stream. */
	eOTA_ImageState_Last = eOTA_ImageState_Aborted
} OTA_ImageState_t;

/**
 * @brief OTA File Context Information.
 * 
 * Information about an OTA Update file that is to be streamed. This structure is filled in from a 
 * job notification MQTT message. Currently only one file context can be streamed at time. 
 */
typedef struct {

	u8		   *pacFilepath;		/*!< Local file pathname. */
	union {
									
		s32		iFileHandle;		/*!< Device internal file pointer or handle.
		                             * File type is handle after file is open for write. 
									 */
#if WIN32
		FILE   *pstFile;			/*!< File type is stdio FILE structure after file is open for write. */
#endif
		u8*		pucFile;			/*!< File type is RAM/Flash image pointer after file is open for write. */
	};
	TimerHandle_t pvRequestTimer;	/*!< The request timer associated with this OTA context. */
	u32			iFileSize;			/*!< The size of the file in bytes. */
    u32			iBlocksRemaining;	/*!< How many blocks remain to be received (a code optimization). */
	u16			usSigSize;			/*!< Size, in bytes, of the file signature (it may vary). */
	u32			ulFileAttributes;	/*!< Flags specific to the file being received (e.g. secure, bundle, archive). */
	u32			ulServerFileID;		/*!< The file is referenced by this numeric ID in the OTA job. */
	u32			ulRequestMomentum;	/*!< The number of stream requests published before a response was received. */
	u8		   *pacJobName;			/*!< The job name associated with this file from the job service. */
	u8		   *pacStreamName;		/*!< The stream associated with this file from the OTA service. */
	u8		   *pacSignature;		/*!< Pointer to the file's signature in binary form. */
	u8		   *pacRxBlockBitmap;	/*!< Bitmap to track the blocks we've received (for de-duping). */
	u8		   *pacCertFilepath;	/*!< Pathname of the certificate file used to validate the receive file. */
	u32			ulUpdaterVersion;	/*!< Used by OTA self-test detection, the version of FW that did the update. */

} OTA_FileContext_t;

/**
 * @brief OTA Image stream completion callback function signature.
 * 
 * The user must register a call back to be notified of when the OTA job is complete in order to
 * call OTA_ActivateNewImage() and reboot the MCU system or if the job failed.
 * 
 * @note: 
 * 
 * The callback function is called with the input parameter result set to eOTA_ImageState_Accepted when 
 * the image has been fully downloaded and the signature has been verified. At this point the job manager
 * has just been notified that the signature verification has passed. The OTA job will next be in 
 * self_test mode and waiting for the system to reboot.
 * 
 * The callback function is called with a result of eOTA_ImageState_Testing when the system
 * has rebooted and it is verified that the current image is an acceptable image.
 * 
 * The callback function is called with a result of eOTA_ImageState_Rejected when there has
 * been a problem in the download of the image.
 * 
 * @param[in] result The state of the OTA image that is being streamed.
 */
typedef void (*pxOTACompleteCallback_t)(OTA_ImageState_t result);

/*---------------------------------------------------------------------------*/
/*								Public API									 */
/*---------------------------------------------------------------------------*/

/**
 * @brief OTA Agent initialization function.
 * 
 * Initialize the OTA engine by starting the OTA Agent ("OTA Task") in the system. This funtion must 
 * be called with the MQTT messaging client context before calling OTA_CheckForUpdate(). Only one 
 * task/context is currently supported at a time.
 * 
 * @param[in] pvPubSubClient The messaging protocol client context (e.g. an MQTT context).
 * @param[in] func Static callback function for when an OTA job is complete. This function will have
 * input of the state of the OTA imave after download and during self-test.
 * @param[in] xTicksToWait The number of ticks to wait until the OTA Task signals that it is ready.
 * If this is set to zero, then the function will return immediately after creating the OTA task,
 * but the OTA task may not be ready to operate yet.
 * 
 * @return The state of the OTA Agent. If the task was not successfully created or there already
 * exists a task, the state will be eOTA_AgentState_Ready. If there is no OTA task then the state 
 * will be eOTA_AgentState_NotReady.
 */
OTA_State_t OTA_AgentInit(void *pvPubSubClient, pxOTACompleteCallback_t func, TickType_t xTicksToWait);

/**
 * @brief Signal to the OTA Agent to shut down.
 * 
 * Signals the OTA agent task to shut down. The OTA agent will unsubscribe from all MQTT job 
 * notification topics, stop in progress OTA jobs, if any, and clear all resources.
 * 
 * @param[in] xTicksToWait The number of ticks to wait until the OTA Agent signals move to the not
 * ready state (no task running). If this is set to zero, then the function will return immediately,
 * but regardless of the OTA agent state it may not be fully shut down yet.
 * 
 * @return The OTA agent state. eOTA_AgentState_ShuttingDown if the agent is already in the process
 * of shutting down and eOTA_AgentState_NotReady if the agent is already shut down.
 */
OTA_State_t OTA_AgentShutdown(TickType_t xTicksToWait);

/**
 * @brief Get the current state of the OTA agent. 
 * 
 * @return The current state of the OTA agent.
 */
OTA_State_t OTA_GetAgentState(void);

/**
 * @brief Activate the newest MCU image received via OTA.
 * 
 * This function shall reset the MCU and cause a reboot of the system to execute the newly updated 
 * firmware. It should be called via user code in response to the OTA Job Complete Callback when the 
 * status is reported as eOTA_ImageState_Accepted. Refer to the OTA_RegisterJobCompleteCallback() 
 * API to configure the user callback handler.
 * 
 * @return kOTA_Err_None if successful, otherwise an error code explaining the error that is 
 * returned.
 */
OTA_Err_t OTA_ActivateNewImage(void);

/**
 * @brief Set the state of the current MCU image.
 * 
 * The states are eOTA_ImageState_Testing, eOTA_ImageState_Accepted, eOTA_ImageState_Aborted or 
 * eOTA_ImageState_Rejected; see OTA_ImageState_t documentation. This will update the status of the 
 * current image and publish to the job status topic, if there is an active job.
 * 
 * @param[in] The state to set of the OTA image.
 * 
 * @return kOTA_Err_None if successful, otherwise returns an error code explaining the error that is
 * returned.
 */
OTA_Err_t OTA_SetImageState(OTA_ImageState_t);

/** 
 * @brief Get the state of the currently running MCU image.
 * 
 * The states are eOTA_ImageState_Testing, eOTA_ImageState_Accepted, eOTA_ImageState_Aborted or 
 * eOTA_ImageState_Rejected; see OTA_ImageState_t documentation.
 * 
 * @return The state of the current context's OTA image.
 */
OTA_ImageState_t OTA_GetImageState(void);

/* Request for the next available OTA job from the job service. */
OTA_Err_t OTA_CheckForUpdate(void);

/*---------------------------------------------------------------------------*/
/*							Statistics API									 */
/*---------------------------------------------------------------------------*/

/** 
 * @brief Get the number of OTA message packets queued by the OTA agent.
 * 
 * @note Calling OTA_AgentInit() will reset this statistic.
 * 
 * @return The number of OTA packets that have been queued.
 */
uint32_t OTA_GetPacketsQueued(void);

/**
 * @brief Get the number of OTA message packets processed by the OTA agent.
 * 
 * @note Calling OTA_AgentInit() will reset this statistic.
 * 
 * @return the number of OTA packets that have been processed.
 * 
 */
uint32_t OTA_GetPacketsProcessed(void);

/** 
 * @brief Get the number of OTA message packets dropped by the OTA agent. 
 * 
 * @note Calling OTA_AgentInit() will reset this statistic.
 * 
 * @return the number of OTA packets that have been dropped.
 */
uint32_t OTA_GetPacketsDropped(void);

/* AWS_TS_OTA_AGENT_H_ */
#endif
