/*
Amazon FreeRTOS OTA Agent V0.9.3
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/


/* OTA agent includes. */
#define INCLUDE_FROM_OTA_AGENT
#include "aws_ota_pal.h"
#include "aws_ota_agent.h"
#include "event_groups.h"
#include "aws_rsprintf.h"
#include "aws_clientcredential.h"
#include <string.h>
#include <stdlib.h>
#include "aws_ota_cbor.h"
#include "aws_application_version.h"

/* FreeRTOS includes. */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"

/* MQTT includes. */
#include "aws_mqtt_agent.h"

/* Included for unit tests. */
#include "aws_ota_agent_internal.h"

/* JSON job document parser includes. */
#include "jsmn.h"
#include "mbedtls/base64.h"


/* Macro to get the number of elements in a static type. */
#define NUM_ELEM(x) (sizeof(x)/sizeof(*x))

/* Returns the byte offset of the element 'e' in the typedef structure 't'. */
#define OFFSET_OF(t,e) ((u32)&((t*)0)->e)

/* Set OTA_PRINT_DATA_BLOCKS to 1 to include the debug dump of received file data blocks. */
#define OTA_PRINT_DATA_BLOCKS    0

/* Convenience macros for calculating log base 2 of 8, 16, or 32 bit values. ONLY USE FOR CONSTANTS! */
#define LOG2_8BIT( v )       ( 8 - 90 / ( ( ( v ) / 4 + 14 ) | 1 ) - 2 / ( ( v ) / 2 + 1 ) )
#define LOG2_16BIT( v )      ( 8 * ( ( v ) > 255 ) + LOG2_8BIT( ( v ) >> 8 * ( ( v ) > 255 ) ) )
#define LOG2_32BIT( v )      ( 16 * ( ( v ) > 65535L ) + LOG2_16BIT( ( v ) * 1L >> 16 * ( ( v ) > 65535L ) ) )

#define CONST_STRLEN( s )    ( sizeof( s ) - 1 )    /* Evaluates to the length of a constant string defined like 'static const char str[]= "xyz"; */
/* Macros to allow quoting strings. Double nesting is required. Use the QQ macro when quoting strings in a macro. */
#define Q( x )               # x
#define QQ( x )              Q( x )

/* General constants. */
#define kOTA_FileRequestWait_ms			2000            /* Time in milliseconds to wait before requesting an update if nothing is happening. */
#define kmOTA_ErasedBlocksVal			0xff            /* The starting state of a group of erased blocks in the Rx block bitmap. */

#define kmOTA_EvtMsgReady				0x00000001		/* OTA MQTT message ready event flag. */
#define kmOTA_EvtShutdown				0x00000002		/* Event flag to request OTA shutdown. */
#define kmOTA_EvtReqTimeout				0x00000004		/* Event flag indicating the request timer has timed out. */
#define kmOTA_AllEvents					( kmOTA_EvtMsgReady | kmOTA_EvtShutdown | kmOTA_EvtReqTimeout )

#define kOTA_MaxConcurrentFiles			1				/* [MUST REMAIN 1! Future support.] Maximum number of concurrent OTA files. */
#define kOTA_MaxSignatureSize			256				/* Max number of bytes allowed for a file signature (e.g. 2048 bit RSA encrypted SHA1). */
#define kOTA_MaxCertPathname			127				/* Max number of bytes allowed for the signer certificate path name. */
#define kOTA_NumMsgQueueEntries			6				/* Maximum number of entries in the OTA message queue. */
#define kOTA_StackSize					( 7 * configMINIMAL_STACK_SIZE )
#define kOTA_SubscribeWaitInTicks		pdMS_TO_TICKS(30000UL)
#define kOTA_UnSubscribeWaitInTicks		pdMS_TO_TICKS(1000UL)
#define kOTA_PublishWaitInTicks			pdMS_TO_TICKS(10000UL)
#define kOTA_HalfSecondDelay				pdMS_TO_TICKS(500UL)
#define kOTA_MaxStreamRequestMomentum			32				/* Max number of stream requests allowed without a response before we abort. */

/* Stream GET message constants. */

#define kOTA_Client_Token               "rdy"			/* Client token sent in the stream "GET" message. */
#define kOTA_MaxBlockBitmapSize         128             /* Max allowed number of bytes to track all blocks of an OTA file. Adjust block size if more range is needed. */
#define kOTA_RequestMsg_MaxSize         ( 3 * kOTA_MaxBlockBitmapSize )

/* Job status message constants. */

#define kOTA_StatusMsg_MaxSize          128				/* Max length of a job status message to the service. */
#define kOTA_UpdateStatusFrequency		64				/* Update the job status every 64 unique blocks received. */

/* Job document parser constants. */

#define kOTA_MaxJSONTokens				64				/* Number of JSON tokens supported in a single parser call. */
#define kOTA_MaxDynamicTopicNameLen		256				/* Max length of a dynamically generated topic string (usually on the stack). */

/* The OTA job document contains parameters that are required for us to build the
 * stream request message and manage the OTA process. Including info like file name,
 * size, attributes, etc. The following value specifies the number of parameters
 * that are included in the job document model although some may be optional. */

#define kOTA_NumJobParams 16
/* We need the following string to match in a couple places in the code so use a #define. */
#define OTA_JSON_UPDATED_BY_KEY "updatedBy"

static const char acOTA_JSON_ClientTokenKey[] = "clientToken";
static const char acOTA_JSON_ExecutionKey[] = "execution";
static const char acOTA_JSON_JobIDKey[] = "jobId";
static const char acOTA_JSON_StatusDetailsKey[] = "statusDetails";
static const char acOTA_JSON_SelfTestKey[] = "self_test";
static const char acOTA_JSON_UpdatedByKey[] = OTA_JSON_UPDATED_BY_KEY;
static const char acOTA_JSON_JobDocKey[] = "jobDocument";
static const char acOTA_JSON_OTAUnitKey[] = "ts_ota";
static const char acOTA_JSON_FileGroupKey[] = "files";
static const char acOTA_JSON_StreamNameKey[] = "streamname";
static const char acOTA_JSON_FilePathKey[] = "filepath";
static const char acOTA_JSON_FileSizeKey[] = "filesize";
static const char acOTA_JSON_FileIDKey[] = "fileid";
static const char acOTA_JSON_FileAttributeKey[] = "attr";
static const char acOTA_JSON_FileCertNameKey[] = "certfile";

/* The OTA signature algorithm is specified by the PAL (Platform Abstraction Layer) file. */
static const char acOTA_JSON_FileSignatureKey[32];

typedef enum {
	eJobReason_Receiving = 0,		/* Update progress status. */
	eJobReason_SigCheckPassed,		/* Set status details to Self Test Ready. */
	eJobReason_SelfTestActive,		/* Set status details to Self Test Active. */
	eJobReason_Accepted,			/* Set job state to Succeeded. */
	eJobReason_Rejected,			/* Set job state to Failed. */
	eJobReason_Aborted,			/* Set job state to Failed. */
	eNumJobReasons
} OTA_JobReason_t;

/* These are the associated statusDetails 'reason' codes that go along with
 * the above enums during the OTA update process. The 'Receiving' state is
 * updated with transfer progress as #blocks received of #total.
 */
const char *acOTA_JobReason_Strings[eNumJobReasons] = { "", "ready", "active", "accepted", "rejected", "aborted" };


typedef enum {
	eJobStatus_InProgress = 0,
	eJobStatus_Failed,
	eJobStatus_Succeeded,
	eJobStatus_Rejected,
	eJobStatus_FailedWithVal,					/* This shows a numeric value for the reason code. */
	eJobStatusLast = eJobStatus_FailedWithVal,
	eNumJobStatusMappings,
} OTA_JobStatus_t;

/* We map all of the above status cases to one of these 4 status strings. */

const char acOTA_String_InProgress[] = "IN_PROGRESS";
const char acOTA_String_Failed[] = "FAILED";
const char acOTA_String_Succeeded[] = "SUCCEEDED";
const char acOTA_String_Rejected[] = "REJECTED";

const char *acOTA_JobStatus_Strings[eNumJobStatusMappings] = {
		acOTA_String_InProgress,
		acOTA_String_Failed,
		acOTA_String_Succeeded,
		acOTA_String_Rejected,
		acOTA_String_Failed						/* eJobStatus_FailedWithVal */
};

/* The OTA job document parameter types used by our job model. */

typedef enum {
	eJobParamType_StringCopy,
	eJobParamType_StringInJobDoc,
	eJobParamType_Object,
	eJobParamType_Array,
	eJobParamType_UInt32,
	eJobParamType_Base64,
	eJobParamType_Ident,
} JobParamType_t;


/* OTA job document parser error codes. */

typedef enum {
	eOTA_JobParseErr_None = 0,
	eOTA_JobParseErr_OutOfMemory,			/* We failed to allocate enough memory for a field. */
	eOTA_JobParseErr_FieldTooLarge,			/* The field is larger than we expect. */
	eOTA_JobParseErr_FieldTypeMismatch,		/* The field type parsed does not match the document model. */
	eOTA_JobParseErr_Base64Decode,			/* There was an error decoding the base64 job data. */
	eOTA_JobParseErr_InvalidNumChar,		/* There was an invalid character in a numeric value field. */
	eOTA_JobParseErr_DuplicatesNotAllowed,  /* A duplicate parameter was found in the job document. */
	eOTA_JobParseErr_BusyWithExistingJob,	/* We're busy with a job but received a new job document. */
	eOTA_JobParseErr_MalformedJobDoc,		/* The job document wasn't perfect. */
	eOTA_JobParseErr_ZeroFileSize			/* Job document specified a zero sized file. This is not allowed. */
} OTA_JobParseErr_t;


/* This is a job parameter structure used by the job document model.
 * It determines the type of parameter specified by the key name and
 * where to store the parameter locally when it is extracted from the
 * job document. It also contains the expected Jasmine type of the
 * value field for validation. */

typedef struct {
	const char * pcSrcKey;
	const bool_t bRequired;
	const uint32_t ulDestOffset;
	const JobParamType_t eModelParamType;
	const jsmntype_t eJasmineType;
} JobParamDeterminer_t;

#define kOTA_ParamRequired		pdTRUE
#define kOTA_ParamOptional		pdFALSE
#define OTA_JOB_PARAM_NOT_STORED 0xffffffff

/* Topic strings used by the OTA process. */

const char acOTA_GetNextAccepted_Topic[] = "$aws/things/" clientcredentialIOT_THING_NAME "/jobs/$next/get/accepted";
const char acOTA_NotifyNext_Topic[] = "$aws/things/" clientcredentialIOT_THING_NAME "/jobs/notify-next";
const char acOTA_GetNextJob_Topic[] = "$aws/things/" clientcredentialIOT_THING_NAME "/jobs/$next/get";
const char acOTA_GetNextJob_MsgTemplate[] = "{\"clientToken\":\"%u:%s\"}";
const char acOTA_Request_TopicTemplate[] = "$aws/bin/things/" clientcredentialIOT_THING_NAME "/streams/%s/get";
const char acOTA_RxStream_TopicTemplate[] = "$aws/things/" clientcredentialIOT_THING_NAME "/streams/%s";
const char acOTA_RequestAccepted_TopicTemplate[] = "$aws/bin/things/" clientcredentialIOT_THING_NAME "/streams/%s/get/accepted";
const char acOTA_JobStatus_TopicTemplate[] = "$aws/things/" clientcredentialIOT_THING_NAME "/jobs/%s/update";
const char acOTA_JobStatus_StatusTemplate[] = "{\"status\":\"%s\",\"statusDetails\":{";
const char acOTA_JobStatus_ReceiveDetailsTemplate[] = "\"%s\":\"%u/%u\"}}";
const char acOTA_JobStatus_SelfTestDetailsTemplate[] = "\"%s\":\"%s\",\"" OTA_JSON_UPDATED_BY_KEY "\":\"0x%x\"}}";
const char acOTA_JobStatus_ReasonStrTemplate[] = "\"reason\":\"%s\"}}";
const char acOTA_JobStatus_ReasonValTemplate[] = "\"reason\":\"%d\"}}";
const char acOTA_String_Receive[] = "receive";
const char acOTA_String_SelfTest[] = "self_test";


/* Test a null terminated string against a JSON string of known length and return whether
 * it is the same or not. */

bool_t JSON_IsCStringEqual(const char * pcJSONString, uint32_t lLen, const char * pcCString);

/* OTA agent private function prototypes. */

static void prvOTAUpdateTask(MQTTAgentHandle_t xPubSubClientHandle);

/* Determine if the received MQTT message is from one of our job topics. */

static bool_t prvIsMsgFromJobTopic(MQTTPublishData_t xMsgMetaData);

/* Start a timer to kick-off the OTA update request. Pass it the OTA file context. */

static void prvStartRequestTimer(OTA_FileContext_t* C);

/* Stop the OTA update request timer. */

static void prvStopRequestTimer(OTA_FileContext_t* C);

/* This is the OTA update request timer callback action (upon timeout). */

static void prvRequestTimer_Callback(TimerHandle_t T);

/* Subscribe to the jobs notification topic (i.e. New file version available). */

static bool_t prvSubscribeToJobNotificationTopic(void);

/* UnSubscribe from the jobs notification topic. */

static bool_t prvUnSubscribeFromJobNotificationTopic(void);

/* Subscribe to the firmware update receive topic. */

static bool_t prvSubscribeToDataStream(OTA_FileContext_t * C);

/* UnSubscribe from the firmware update receive topic. */

static bool_t prvUnSubscribeFromDataStream (OTA_FileContext_t * C);

/* Publish a message using the platforms PubSub mechanism. */

static MQTTAgentReturnCode_t prvPublishMessage(void * pvClient, const char * const pacTopic, u16 iTopicLen, char * pacMsg, u32 iMsgSize, MQTTQoS_t eQOS);

/* Called when the OTA agent receives a file data block message. */

static IngestResult_t prvIngestDataBlock(OTA_FileContext_t*, const char *pszMsg, u32 iMsgLen);

/* Called when the OTA agent receives an OTA version message. */

static OTA_FileContext_t* prvProcessOTAJobMsg(const char *pszMsg, u32 iMsgLen);

/* Get an available OTA file context structure or NULL if none available. */

static OTA_FileContext_t *prvGetFreeContext(void);

/* Get the information about the update file from a JSON metadata blob. */

static OTA_FileContext_t *prvParseJobDocFromJSON(const char *pacRawMsg, u32 iMsgLen);

/* Close an open OTA file context and free it. */

static bool_t prvOTA_Close(OTA_FileContext_t * const C);

/* Called when a MQTT message is received on an OTA agent topic of interest. */

static MQTTBool_t prvOTAPublishCallback(void * pvCallbackContext,
	const MQTTPublishData_t * const pxPublishData);

/* Construct a dynamic OTA job status topic name into the specified buffer. */

static uint32_t prvBuildJobStatusTopicName (char *pcNameBuffer, uint32_t ulBufSize, const char *pacJobName);

/* Construct a dynamic OTA data REQUEST topic name into the specified buffer. */

static uint32_t prvBuildDataRequestTopicName (char *pcNameBuffer, uint32_t ulBufSize, OTA_FileContext_t * C);

/* Construct a dynamic OTA data STREAM topic name into the specified buffer. */

static uint32_t prvBuildDataStreamTopicName (char *pcNameBuffer, uint32_t ulBufSize, OTA_FileContext_t * C);

/* Update the job status topic with our progress of the OTA transfer. */

static void prvUpdateJobStatus (OTA_FileContext_t *C, char *pcOTA_DynamicTopic, OTA_JobStatus_t eStatus, int32_t lReason);

/* Construct the "Get Stream" message and publish it to the stream service request topic. */

static OTA_Err_t prvPublishGetStreamMessage (OTA_FileContext_t *C, char *pcOTA_DynamicTopic);

/* The OTA agent is a singleton. Initialization and static structures are protected by critical sections. */

static uint32_t	ulOTA_PacketsQueued;							/* Number of OTA packets queued by the MQTT callback. */
static uint32_t	ulOTA_PacketsProcessed;							/* Number of OTA packets processed by the OTA task. */
static uint32_t	ulOTA_PacketsDropped;							/* Number of OTA packets dropped due to congestion. */
static OTA_State_t eOTA_AgentState = eOTA_AgentState_NotReady;		/* State of the OTA engine. */
static void *pvPubSubClient = NULL;								/* The current publish/subscribe client context (use is determined by the client). */
static OTA_FileContext_t astOTA_Files[kOTA_MaxConcurrentFiles];	/* Static array of OTA file structures. */
EventGroupHandle_t xOTA_EventFlags = NULL;						/* Event group for communicating with the OTA task. */
static pxOTACompleteCallback_t pxOTAJobCompleteCallback = NULL;	/* The user level function to call after the OTA job is complete. */
static OTA_ImageState_t eOTA_FinalImageState = eOTA_ImageState_Unknown;
static bool_t bOTA_Singleton_SelfTestMode = pdFALSE;				/* True when the latest job document status is in self test mode. */
static uint8_t *pacOTA_Singleton_ActiveJobName = NULL;				/* The currently active job name. We only allow one at a time. */
static char *pacClientTokenFromJob = NULL;						/* The update job clientToken field. */

/**
* @brief Handle of the OTA message queue used to pass MQTT messages
* received via the callback to the OTA task.
*/
static QueueHandle_t xOTA_MsgQ = NULL;

/* This is the jobs notification subscription structure. */

static const MQTTAgentSubscribeParams_t stJobsNotificationSubscription =
{
	(const uint8_t*)acOTA_GetNextAccepted_Topic,	/* const char* Topic name. */
	CONST_STRLEN(acOTA_GetNextAccepted_Topic),		/* Length of topic. */
	eMQTTQoS1,										/* MQTTQoS_t xQoS */
	NULL,											/* pvPublishCallbackContext field. */
	prvOTAPublishCallback							/* pxPublishCallback field. */
};


static const MQTTAgentSubscribeParams_t stJobsNotifyNextSubscription =
{
	(const uint8_t*)acOTA_NotifyNext_Topic,		/* const char* Topic name. */
	CONST_STRLEN (acOTA_NotifyNext_Topic),		/* Length of topic. */
	eMQTTQoS1,										/* MQTTQoS_t xQoS */
	NULL,											/* pvPublishCallbackContext field. */
	prvOTAPublishCallback							/* pxPublishCallback field. */
};


/* Public API to initialize the OTA update process. */

OTA_State_t OTA_AgentInit(void *pvClient, pxOTACompleteCallback_t func, TickType_t xTicksToWait) {
	u32 i;
	BaseType_t xReturn;

	/* The actual OTA queue control structure. */
	static StaticQueue_t xStaticQueue;

	/* The array to use as the queue's data area. This is an array of
	MQTT publish message data structures (i.e. the metadata). */
	static MQTTPublishData_t ucQueueData[kOTA_NumMsgQueueEntries];

	/* Set the function to be called after an OTA job is complete. The callback function gets the pass or fail status. */
	pxOTAJobCompleteCallback = func;

	/* Reset our statistics counters. */
	ulOTA_PacketsDropped = 0;
	ulOTA_PacketsQueued = 0;
	ulOTA_PacketsProcessed = 0;
	portENTER_CRITICAL();
	if (eOTA_AgentState == eOTA_AgentState_NotReady) {

		pvPubSubClient = pvClient;							/* Save the current pub/sub client as specified by the user. */

		/* Create the queue used to pass MQTT publish messages to the OTA task. */
		xOTA_MsgQ = xQueueCreateStatic(kOTA_NumMsgQueueEntries, sizeof(MQTTPublishData_t), (uint8_t*)ucQueueData, &xStaticQueue);
		configASSERT(xOTA_MsgQ);

		for (i = 0; i < kOTA_MaxConcurrentFiles; i++) {
			astOTA_Files[i].pacFilepath = NULL;
		}
		xReturn = xTaskCreate(prvOTAUpdateTask, "OTA Task", kOTA_StackSize, NULL, tskIDLE_PRIORITY, NULL);
		portEXIT_CRITICAL();								/* Protected elements are initialized. It's now safe to context switch. */
		if (xReturn == pdPASS)
		{
			/* Wait for the OTA agent to be ready if requested. */
			while ((xTicksToWait-- > 0) && (eOTA_AgentState != eOTA_AgentState_Ready))
			{
				vTaskDelay(1);
			}
		}
		else
		{
			/* Task creation failed so fall through to exit. */
		}
	}
	else
	{
		portEXIT_CRITICAL();
	}
	return eOTA_AgentState;									/* Return status of agent. */
}



/* Public API to shutdown the OTA process. */

OTA_State_t OTA_AgentShutdown( TickType_t xTicksToWait )
{
    if( eOTA_AgentState == eOTA_AgentState_Ready )
    {
        xEventGroupSetBits( xOTA_EventFlags, kmOTA_EvtShutdown );

        /* Wait for the OTA agent to complete shutdown, if requested. */
        while( ( xTicksToWait-- > 0 ) && ( eOTA_AgentState != eOTA_AgentState_NotReady ) )
        {
            vTaskDelay( 1 );
        }
    }

    return eOTA_AgentState;
}


/* Return the current state of the OTA agent. */

OTA_State_t OTA_GetAgentState( void )
{
    return eOTA_AgentState;
}

uint32_t OTA_GetPacketsDropped(void)
{
	return ulOTA_PacketsDropped;
}

uint32_t OTA_GetPacketsQueued(void)
{
	return ulOTA_PacketsQueued;
}

uint32_t OTA_GetPacketsProcessed(void)
{
	return ulOTA_PacketsProcessed;
}


/* Request for the next available OTA job from the job service by publishing
 * a "get next job" message to the job service. */

OTA_Err_t OTA_CheckForUpdate (void)
{
	static uint32_t ulReqCounter = 0;
	MQTTAgentReturnCode_t eResult;
	uint32_t iLen;
	/* The following buffer is big enough to hold a dynamically constructed $next/get job message.
	 * It contains a client token that is used to track how many requests have been made. */
	char acMsg[CONST_STRLEN(acOTA_GetNextJob_MsgTemplate) + kU32_MaxPlaces + CONST_STRLEN(clientcredentialIOT_THING_NAME)];

	OTA_PRINT ("[OTA] Check For Update #%u\r\n", ulReqCounter);
	iLen = rsprintf (acMsg, acOTA_GetNextJob_MsgTemplate, ulReqCounter++, clientcredentialIOT_THING_NAME);
	eResult = prvPublishMessage (
		pvPubSubClient,
		acOTA_GetNextJob_Topic,
		CONST_STRLEN (acOTA_GetNextJob_Topic),
		acMsg,
		iLen,
		eMQTTQoS1);

	if (eResult != eMQTTAgentSuccess) {
		OTA_PRINT ("[OTA] Failed to publish Check For Update message.\r\n");
		return kOTA_Err_PublishFailed;
	}
	else {
		/* Nothing special to do. We succeeded. */
	}
	return kOTA_Err_None;
}


/* Clean up after the OTA process is done. Possibly free memory for re-use. */

void prvAgentShutdown( void )
{
    u32 i;

    /* Mark OTA agent as not ready. */
    eOTA_AgentState = eOTA_AgentState_ShuttingDown;

	/* Un-subscribe from the OTA job notification topic. */
	prvUnSubscribeFromJobNotificationTopic ();

	/* Close any open OTA transfers. */
    for( i = 0; i < kOTA_MaxConcurrentFiles; i++ )
    {
        prvOTA_Close( &astOTA_Files[ i ] );
    }

	/* Free any remaining string memory holding the job name. */
	if (pacOTA_Singleton_ActiveJobName != NULL)
	{
		vPortFree (pacOTA_Singleton_ActiveJobName);
		pacOTA_Singleton_ActiveJobName = NULL;
	}
}


OTA_Err_t OTA_ActivateNewImage(void)
{
	OTA_Err_t eErr = kOTA_Err_None;

	/* Call platform code to activate the image. This may not return. */
	u8 uErr = prvActivateNewImage();

	/* Map internal error to user error. */
	if (uErr != 0)
	{
		eErr = kOTA_Err_ActivateFailed;
	}
	return eErr;
}


/* Get the running image state. */

OTA_ImageState_t OTA_GetImageState (void)
{
	return eOTA_FinalImageState;
}


/* Accept or reject the OTA image transfer. */

OTA_Err_t OTA_SetImageState (OTA_ImageState_t eState)
{
	//ToDo: Can we move the dynamic buffer to a better place to avoid two of these within one task?
	char acOTA_DynamicTopic[kOTA_MaxDynamicTopicNameLen];

	if (( eState > eOTA_ImageState_Unknown ) && ( eState <= eOTA_ImageState_Last ))
	{
		/* Call the platform specific code to set the image state. */
		prvSetImageState (eState);
		eOTA_FinalImageState = eState;

		if (pacOTA_Singleton_ActiveJobName != NULL)
		{
			if (eState == eOTA_ImageState_Testing)
			{	/* We discovered we're ready for test mode, put job status in active. */
				prvUpdateJobStatus (NULL, &acOTA_DynamicTopic[0], eJobStatus_InProgress, eJobReason_SelfTestActive);
			}
			else
			{
				if (eState == eOTA_ImageState_Accepted)
				{	/* Now that we've accepted the firmware update, we can complete the job. */
					prvUpdateJobStatus (NULL, &acOTA_DynamicTopic[0], eJobStatus_Succeeded, eJobReason_Accepted);
				}
				else if (eState == eOTA_ImageState_Aborted)
				{	/* The firmware update was aborted, complete the job as FAILED. */
					prvUpdateJobStatus (NULL, &acOTA_DynamicTopic[0], eJobStatus_Failed, eJobReason_Aborted);
				}
				else
				{	/* The firmware update was rejected, complete the job as FAILED (Job service doesn't allow REJECTED today). */
					prvUpdateJobStatus (NULL, &acOTA_DynamicTopic[0], eJobStatus_Failed, eJobReason_Rejected);
				}
				/* We don't need the job name memory anymore since we're done with this job. */
				vPortFree (pacOTA_Singleton_ActiveJobName);
				pacOTA_Singleton_ActiveJobName = NULL;
				/* We're no longer in self test mode. */
				bOTA_Singleton_SelfTestMode = pdFALSE;
			}
			return kOTA_Err_None;
		}
		else
		{
			return kOTA_Err_NoActiveJob;
		}
	}
	else
	{
		return kOTA_Err_BadFinalState;
	}
}



/* Update the job status with our progress. This may remain in progress or go terminal. */

static void prvUpdateJobStatus (OTA_FileContext_t *C, char *pcOTA_DynamicTopic, OTA_JobStatus_t eStatus, int32_t lReason)
{
	uint32_t ulRequestTopicLen, iNumBlocks, iReceived, iMsgSize;
	MQTTAgentReturnCode_t eResult;
	MQTTQoS_t eQOS;
	char acMsg[kOTA_StatusMsg_MaxSize];

	/* All job state transitions except streaming progress use QOS 1 since it is required to have status in the job document. */
	eQOS = eMQTTQoS1;

	if (eStatus == eJobStatus_InProgress)
	{
		if (lReason == (int32_t) eJobReason_Receiving)
		{
			if (C != NULL)
			{
				iNumBlocks = (C->iFileSize + (kOTA_FileBlockSize - 1)) >> LOG2_16BIT (kOTA_FileBlockSize);
				iReceived = iNumBlocks - C->iBlocksRemaining;
				if ((iReceived % kOTA_UpdateStatusFrequency) == 0)	/* Output a status update once in a while. */
				{
					/* Downgrade Progress updates to QOS 0 to avoid overloading MQTT buffers during active streaming. */
					eQOS = eMQTTQoS0;
					iMsgSize = rsprintf (acMsg, acOTA_JobStatus_StatusTemplate, acOTA_JobStatus_Strings[eStatus]);
					iMsgSize += rsprintf (&acMsg[iMsgSize], acOTA_JobStatus_ReceiveDetailsTemplate, acOTA_String_Receive, iReceived, iNumBlocks);
				}
				else
				{	/* Don't send a status update yet. */
					return;
				}
			}
			else
			{	/* Can't send a status update without data from the OTA context. */
				return;
			}
		}
		else
		{	/* We're no longer receiving but we're still In Progress so it is assumed we're in Self Test mode. */
			iMsgSize = rsprintf (acMsg, acOTA_JobStatus_StatusTemplate, acOTA_JobStatus_Strings[eStatus]);
			iMsgSize += rsprintf (&acMsg[iMsgSize], acOTA_JobStatus_SelfTestDetailsTemplate, acOTA_String_SelfTest,
					acOTA_JobReason_Strings[lReason], xAppFirmwareVersion.u.ulVersion32);
		}
	}
	else
	{
		if (eStatus <= eJobStatusLast)
		{	/* Status updates that are not "IN PROGRESS" use simple string maps for status and reason code
		 	 * except for the FAILED status. FAILED status updates use a numeric reason code since there
		 	 * are many more possibilities and the descriptions are quite long.
		 	 */
			iMsgSize = rsprintf (acMsg, acOTA_JobStatus_StatusTemplate, acOTA_JobStatus_Strings[eStatus]);
			if (eStatus == eJobStatus_FailedWithVal)
			{
				iMsgSize += rsprintf (&acMsg[iMsgSize], acOTA_JobStatus_ReasonValTemplate, lReason);
			}
			else
			{
				iMsgSize += rsprintf (&acMsg[iMsgSize], acOTA_JobStatus_ReasonStrTemplate, acOTA_JobReason_Strings[lReason]);
			}
		}
		else
		{	/* Unknown status code. Just ignore it. */
			return;
		}
	}
	/* Try to build the dynamic job status topic . */
	ulRequestTopicLen = prvBuildJobStatusTopicName (pcOTA_DynamicTopic, kOTA_MaxDynamicTopicNameLen, (char*) pacOTA_Singleton_ActiveJobName);

	/* If the topic name was built, try to publish the status message to it. Use QOS 1 to assure update. */
	if (ulRequestTopicLen > 0)
	{	
		eResult = prvPublishMessage (
			pvPubSubClient,
			pcOTA_DynamicTopic,
			ulRequestTopicLen,
			&acMsg[0],
			iMsgSize,
			eQOS);

		if (eResult != eMQTTAgentSuccess) {
			OTA_PRINT ("[OTA] MQTT failed to publish job status message.\r\n");
		}
		else {
			OTA_PRINT ("[OTA] Published '%s' status to %s\r\n", acOTA_JobStatus_Strings[eStatus], pcOTA_DynamicTopic);
		}
	}
	else
	{
		OTA_PRINT ("[OTA] Failed to publish job status message. Invalid topic name.\r\n");
	}
}


/* Construct the "Get Stream" message and publish it to the stream service request topic. */

static OTA_Err_t prvPublishGetStreamMessage(OTA_FileContext_t *C, char *pcOTA_DynamicTopic)
{
	size_t iMsgSize;
	uint32_t iNumBlocks, iBitmapLen, ulRequestTopicLen;
	int16_t iResult;
	OTA_Err_t eErr = kOTA_Err_None;
	char acMsg[kOTA_RequestMsg_MaxSize];

	if (C != NULL)
	{
		if ( C->ulRequestMomentum < kOTA_MaxStreamRequestMomentum )
		{
			iNumBlocks = (C->iFileSize + (kOTA_FileBlockSize - 1)) >> LOG2_16BIT (kOTA_FileBlockSize);
			iBitmapLen = (iNumBlocks + (kBitsPerByte - 1)) >> LOG2_8BIT (kBitsPerByte);
			iResult = -1;	/* Start by assuming error code. */

			if (pdTRUE == OTA_CBOR_Encode_GetStreamRequestMessage (
				(uint8_t *)acMsg,
				sizeof (acMsg),
				&iMsgSize,
				kOTA_Client_Token,
				C->ulServerFileID,
				kOTA_FileBlockSize,
				0,
				C->pacRxBlockBitmap,
				iBitmapLen))
			{
				/* Bump the request momentum counter for all publish attempts. */
				C->ulRequestMomentum++;

				/* Try to build the dynamic data REQUEST topic and subscribe to it. */
				ulRequestTopicLen = prvBuildDataRequestTopicName (pcOTA_DynamicTopic, kOTA_MaxDynamicTopicNameLen, C);
				if (ulRequestTopicLen > 0)
				{
					iResult = prvPublishMessage (
						pvPubSubClient,
						pcOTA_DynamicTopic,
						ulRequestTopicLen,
						&acMsg[0],
						iMsgSize,
						eMQTTQoS0);
				}
				if (iResult < 0) {
					OTA_PRINT ("[OTA] Failed to publish message on request topic.\r\n");
					/* Don't return an error. Let max momentum catch it since this may be intermittent. */
				}
				else {
					OTA_PRINT ("[OTA] Published file request to %s\r\n", pcOTA_DynamicTopic);
					/* Restart the request timer to retry if we don't complete the update. */
					prvStartRequestTimer (C);
				}
			}
			else
			{
				OTA_PRINT ("[OTA] Failed to CBOR encode GetStream message.\r\n");
				eErr = kOTA_Err_SoftwareBug;
			}
		}
		else
		{
			/* Too many requests have been sent without a response. Abort. */
			eErr = kOTA_Err_MomentumAbort;
		}
	}
	else
	{
		/* Defensive programming. */
	}
	return eErr;
}


/* This function is called whenever we receive a MQTT publish message on one of our OTA topics. */

static MQTTBool_t prvOTAPublishCallback(void * pvCallbackContext,
	const MQTTPublishData_t * const pxPublishData)
{
	MQTTBool_t xTakeOwnership = eMQTTFalse;
	BaseType_t xReturn;

	(void)pvCallbackContext;

	/* If we're running the OTA task, send publish messages to it for processing. */
	if (xOTA_EventFlags != NULL)
	{
		xReturn = xQueueSendToBack(xOTA_MsgQ, pxPublishData, (TickType_t)0);
		if (xReturn == pdPASS)
		{
			ulOTA_PacketsQueued++;
			xEventGroupSetBits(xOTA_EventFlags, kmOTA_EvtMsgReady);
			/* Take ownership of the MQTT buffer. */
			xTakeOwnership = eMQTTTrue;
		}
		else
		{
			ulOTA_PacketsDropped++;
		}
	}
	else
	{
		/* This should never occur unless we accidentally subscribed to an OTA
		 * topic before the agent was initialized. Drop any MQTT messages that
		 * arrive here until OTA is initialized. */
		OTA_PRINT (("[OTA] Warning: Received MQTT message before agent is ready.\r\n"));
		ulOTA_PacketsDropped++;
	}
	return xTakeOwnership;
}


/* Check if the MQTT message is from one of our job topics. */

static bool_t prvIsMsgFromJobTopic(MQTTPublishData_t xMsgMetaData)
{
	const char * pszTopic = acOTA_GetNextAccepted_Topic;
	uint16_t iLen = (uint16_t)strlen(pszTopic);
	if ( (iLen == xMsgMetaData.usTopicLength) && (memcmp(pszTopic, xMsgMetaData.pucTopic, iLen) == 0) )
	{
		return pdTRUE;
	}
	else
	{
		pszTopic = acOTA_NotifyNext_Topic;
		iLen = (uint16_t)strlen(pszTopic);
		if ( (iLen == xMsgMetaData.usTopicLength) && (memcmp(pszTopic, xMsgMetaData.pucTopic, iLen) == 0) )
		{
			return pdTRUE;
		}
		else return pdFALSE;
	}
}


/* NOTE: This implementation only supports 1 OTA context. Concurrent OTA is not supported. */

static void prvOTAUpdateTask( MQTTAgentHandle_t xPubSubClientHandle )
{
	EventBits_t uxBits;
	OTA_FileContext_t* C = NULL;
	uint16_t iLen;
	OTA_Err_t eErr;
	char acOTA_DynamicTopic[kOTA_MaxDynamicTopicNameLen];

	(void)xPubSubClientHandle;
	MQTTPublishData_t xMsgMetaData;

	/* Subscribe to the OTA job notification topic. */
	if (prvSubscribeToJobNotificationTopic() == pdTRUE)
	{
		xOTA_EventFlags = xEventGroupCreate ();
		if (xOTA_EventFlags != NULL)
		{
			/* Start by sending a request to the job service for the next available job. */
			OTA_CheckForUpdate ();

			/* Put the OTA agent in the ready state. */
			eOTA_AgentState = eOTA_AgentState_Ready;
			while (pdTRUE) {

				uxBits = xEventGroupWaitBits(
					xOTA_EventFlags,		/* The event group being tested. */
					kmOTA_AllEvents,		/* The bits within the event group to wait for. */
					pdTRUE,					/* Bits should be cleared before returning. */
					pdFALSE,				/* Any bit set will do. */
					(TickType_t)~0);		/* Wait forever. */
				(void)uxBits;

				/* Check for the shutdown event. */
				if (uxBits & kmOTA_EvtShutdown)
				{
					/* Shut down and kill this task. */
					prvAgentShutdown();		/* Free up all resources. */
					break;					/* Break, so we stop all OTA processing. */
				}
				/* On OTA request timer timeout, publish the stream request if we have context. */
				if ( ( uxBits & kmOTA_EvtReqTimeout) && ( C != NULL ) )
				{
					eErr = prvPublishGetStreamMessage (C, &acOTA_DynamicTopic[0]);
					if (eErr != kOTA_Err_None)
					{	/* Abort the current OTA. */
						OTA_SetImageState( eOTA_ImageState_Aborted );
						prvOTA_Close(C);
						C = NULL;
					}
				}

				/* Check if a MQTT message is ready for us to process. */
				if (uxBits & kmOTA_EvtMsgReady)
				{
					xMsgMetaData.xBuffer = NULL;
					if (eOTA_AgentState == eOTA_AgentState_Ready)
					{
						while (xQueueReceive(xOTA_MsgQ, &xMsgMetaData, 0) != pdFALSE)
						{
							if (prvIsMsgFromJobTopic(xMsgMetaData) == pdTRUE)
							{
								if (C != NULL)
								{
									prvOTA_Close(C);		/* Abort the existing OTA. */
								}
								C = prvProcessOTAJobMsg((char*)xMsgMetaData.pvData, xMsgMetaData.ulDataLength);
								if ((C == NULL) && (OTA_GetImageState () == eOTA_ImageState_Testing))
								{
									pxOTAJobCompleteCallback (eOTA_ImageState_Testing);
								}
							}
							else {
								/* It's not a job message, maybe it's a data stream message... */
								if (C != NULL)
								{
									iLen = prvBuildDataStreamTopicName (acOTA_DynamicTopic, kOTA_MaxDynamicTopicNameLen, C);
									if ((iLen == xMsgMetaData.usTopicLength) && (memcmp (acOTA_DynamicTopic, xMsgMetaData.pucTopic, iLen) == 0))
									{
										IngestResult_t result = prvIngestDataBlock (C, (const char*)xMsgMetaData.pvData, xMsgMetaData.ulDataLength);
										if (result < eIngest_Result_Continue)
										{
											/* Negative result codes mean we should stop the OTA process
											   because we are either done or in an unrecoverable error state.
											   We don't want to hang on to the resources. */

											if (result == eIngest_Result_FileComplete)
											{
												prvUpdateJobStatus (C, &acOTA_DynamicTopic[0], eJobStatus_InProgress,
													(int32_t) eJobReason_SigCheckPassed);
											}
											else
											{
						                        OTA_PRINT( "[OTA] Aborting due to prvIngestDataBlock error %d\r\n", (int32_t) result );
												prvUpdateJobStatus (C, &acOTA_DynamicTopic[0], eJobStatus_FailedWithVal, (int32_t) result);
											}
											/* Un-subscribe from the data stream topic (errors or not). */
											prvUnSubscribeFromDataStream (C);

											/* Release all remaining resources of the OTA file. */
											prvOTA_Close (C);
											C = NULL;

											/* Let main application know of our result. */
											pxOTAJobCompleteCallback ((result == eIngest_Result_FileComplete) ? eOTA_ImageState_Accepted : eOTA_ImageState_Rejected);

											/* Free any remaining string memory holding the job name since this job is done. */
											if (pacOTA_Singleton_ActiveJobName != NULL)
											{
												vPortFree (pacOTA_Singleton_ActiveJobName);
												pacOTA_Singleton_ActiveJobName = NULL;
											}
										}
										else
										{	/* We're actively receiving a file so update the job status as needed. */
											/* First reset the momentum counter since we received a good block. */
											C->ulRequestMomentum = 0;
											prvUpdateJobStatus (C, &acOTA_DynamicTopic[0], eJobStatus_InProgress, eJobReason_Receiving);
										}
									}
								}
								else
								{	/* We received an unidentified message but it's on our topic so let's check if an update is available. */
									OTA_CheckForUpdate ();
								}
							}
							if (xMsgMetaData.xBuffer != NULL)
							{
								ulOTA_PacketsProcessed++;
								/* Return the MQTT buffer since we're done with it (even if we ignored the message). */
								if ((MQTT_AGENT_ReturnBuffer(pvPubSubClient, xMsgMetaData.xBuffer)) == eMQTTAgentSuccess)
								{
									OTA_PRINT(("[OTA] Returned buffer to MQTT Client.\r\n"));
								}
								else
								{
									OTA_PRINT(("[OTA] MQTT_AGENT_ReturnBuffer() Failed.\r\n"));
									break;
								}
							}
						}
					}
				}
			}
			vEventGroupDelete(xOTA_EventFlags);
		}
	}
	/* We've shut down so update the state and delete resources. */
	eOTA_AgentState = eOTA_AgentState_NotReady;
	vTaskDelete(NULL);
}


/* When the OTA request timer expires, signal the OTA task to request the file. */

static void prvRequestTimer_Callback( TimerHandle_t T )
{
    ( void ) T;

    if( xOTA_EventFlags != NULL )
    {
        /* ToDo: Send message with context pointer related to this timeout.	C = (OTA_FileContext_t*) pvTimerGetTimerID(T);
		 * Or, have the timeout code run through all context's and include a timer in the context. */
        xEventGroupSetBits( xOTA_EventFlags, kmOTA_EvtReqTimeout );
    }
}


/* Create and/or start the OTA request timer to kick off the process if needed. */

void prvStartRequestTimer( OTA_FileContext_t * C )
{
    BaseType_t lTimerStarted = false;

    if( C->pvRequestTimer == NULL )
    {
        C->pvRequestTimer = xTimerCreate( "OTA_FW", pdMS_TO_TICKS( kOTA_FileRequestWait_ms ), pdFALSE, ( void * ) C, prvRequestTimer_Callback );

        if( C->pvRequestTimer != NULL )
        {
            lTimerStarted = xTimerStart( C->pvRequestTimer, 0 );
        }
    }
    else
    {
        lTimerStarted = xTimerReset( C->pvRequestTimer, portMAX_DELAY );
    }

    if( lTimerStarted == false )
    {
        OTA_PRINT( "[OTA] ERROR: RTOS call failed to reset or start the file request timer.\r\n" );
    }
}


/* Stop the FW request timer if it is running. */

void prvStopRequestTimer( OTA_FileContext_t * C )
{
    if( C->pvRequestTimer != NULL )
    {
        xTimerStop( C->pvRequestTimer, 0 );
        OTA_PRINT( "[OTA] Stopping the file request timer.\r\n" );
    }
}


/* Close an existing OTA context. */

bool_t prvOTA_Close(OTA_FileContext_t * const C)
{
	/* Stop and delete any existing transfer request timer. */
	if (C->pvRequestTimer != NULL) {

		xTimerStop(C->pvRequestTimer, 0);
		xTimerDelete(C->pvRequestTimer, 0);
		C->pvRequestTimer = NULL;
	}
	if (C->pacStreamName != NULL)
	{
		prvUnSubscribeFromDataStream (C);			/* Unsubscribe from the data stream if needed. */
		vPortFree (C->pacStreamName);				/* Free any previously allocated stream name memory. */
		C->pacStreamName = NULL;
	}
	if (C->pacJobName != NULL)
	{
		vPortFree (C->pacJobName);					/* Free the job name memory. */
		C->pacJobName = NULL;
	}
	if (C->pacRxBlockBitmap != NULL)
	{
		vPortFree(C->pacRxBlockBitmap);				/* Free any previously allocated block bitmap. */
		C->pacRxBlockBitmap = NULL;
	}
	if (C->pacSignature != NULL)
	{
		vPortFree(C->pacSignature);					/* Free the signature memory. */
		C->pacSignature = NULL;
	}
	if (C->pacFilepath != NULL)
	{
		vPortFree(C->pacFilepath);					/* Free the file path name string memory. */
		C->pacFilepath = NULL;
	}
	if (C->pacCertFilepath != NULL)
	{
		vPortFree(C->pacCertFilepath);				/* Free the certificate path name string memory. */
		C->pacCertFilepath = NULL;
	}
	/* Abort any existing file and release file resource, if needed. */
	prvAbort(C);
	memset(C, 0, sizeof(OTA_FileContext_t));		/* Clear the entire structure now that it is free. */
	return pdTRUE;
}


/* Find an available OTA transfer context structure. */

OTA_FileContext_t *prvGetFreeContext(void)
{
	uint32_t i;
	OTA_FileContext_t *C = NULL;

	for (i = 0; i < kOTA_MaxConcurrentFiles && (astOTA_Files[i].pacFilepath != NULL); i++);

	if (i != kOTA_MaxConcurrentFiles) {
		C = memset(&astOTA_Files[i], 0, sizeof(OTA_FileContext_t));
	}
	else
	{
		/* Not able to support this request so we'll return NULL. */
	}
	return C;
}



/* This is the job document model describing the parameters, their types and and where to store them. */

static const JobParamDeterminer_t xJobDocModel[kOTA_NumJobParams] = {
	{ acOTA_JSON_ClientTokenKey, kOTA_ParamOptional, (uint32_t) &pacClientTokenFromJob, eJobParamType_StringInJobDoc, JSMN_STRING },
	{ acOTA_JSON_ExecutionKey, kOTA_ParamRequired, OTA_JOB_PARAM_NOT_STORED, eJobParamType_Object, JSMN_OBJECT },
	{ acOTA_JSON_JobIDKey, kOTA_ParamRequired, OFFSET_OF (OTA_FileContext_t, pacJobName), eJobParamType_StringCopy, JSMN_STRING },
	{ acOTA_JSON_StatusDetailsKey, kOTA_ParamOptional, OTA_JOB_PARAM_NOT_STORED, eJobParamType_Object, JSMN_OBJECT },
	{ acOTA_JSON_SelfTestKey, kOTA_ParamOptional, (uint32_t) &bOTA_Singleton_SelfTestMode, eJobParamType_Ident, JSMN_STRING },
	{ acOTA_JSON_UpdatedByKey, kOTA_ParamOptional,  OFFSET_OF (OTA_FileContext_t, ulUpdaterVersion), eJobParamType_UInt32, JSMN_STRING },
	{ acOTA_JSON_JobDocKey, kOTA_ParamRequired, OTA_JOB_PARAM_NOT_STORED, eJobParamType_Object, JSMN_OBJECT },
	{ acOTA_JSON_OTAUnitKey, kOTA_ParamRequired, OTA_JOB_PARAM_NOT_STORED, eJobParamType_Object, JSMN_OBJECT },
	{ acOTA_JSON_FileGroupKey, kOTA_ParamRequired, OTA_JOB_PARAM_NOT_STORED, eJobParamType_Array, JSMN_ARRAY },
	{ acOTA_JSON_StreamNameKey, kOTA_ParamRequired, OFFSET_OF (OTA_FileContext_t, pacStreamName), eJobParamType_StringCopy, JSMN_STRING },
	{ acOTA_JSON_FileCertNameKey, kOTA_ParamRequired, OFFSET_OF(OTA_FileContext_t, pacCertFilepath), eJobParamType_StringCopy, JSMN_STRING },
	{ acOTA_JSON_FilePathKey, kOTA_ParamRequired, OFFSET_OF(OTA_FileContext_t, pacFilepath), eJobParamType_StringCopy, JSMN_STRING },
	{ acOTA_JSON_FileSizeKey, kOTA_ParamRequired, OFFSET_OF(OTA_FileContext_t, iFileSize), eJobParamType_UInt32, JSMN_PRIMITIVE },
	{ acOTA_JSON_FileSignatureKey, kOTA_ParamRequired, OFFSET_OF(OTA_FileContext_t, pacSignature), eJobParamType_Base64, JSMN_STRING },
	{ acOTA_JSON_FileIDKey, kOTA_ParamRequired, OFFSET_OF(OTA_FileContext_t, ulServerFileID), eJobParamType_UInt32, JSMN_PRIMITIVE },
	{ acOTA_JSON_FileAttributeKey, kOTA_ParamRequired, OFFSET_OF(OTA_FileContext_t, ulFileAttributes), eJobParamType_UInt32, JSMN_PRIMITIVE },
};


bool_t JSON_IsCStringEqual(const char * pcJSONString, uint32_t lLen, const char * pcCString)
{
	if ( ( strncmp(pcJSONString, pcCString, lLen) == 0 ) &&
		( pcCString[lLen] == 0 ) )
	{
		return pdTRUE;
	}
	else
	{
		/* JSON string and argument C string are not the same. */
		return pdFALSE;
	}
}


/* Extract the OTA file metadata from the JSON format job document. */

OTA_FileContext_t *prvParseJobDocFromJSON(const char *pacRawMsg, u32 iMsgLen) {

	OTA_FileContext_t *C, *pxFinalFile;
	jsmn_parser xParser;
	jsmntok_t *pxTokens, *pxValTok;
	uint32_t ulNumTokens, ulTokenLen, ulValueDest;
	uint32_t a, i;
	uint32_t ulParamsReceivedBitmap, ulParamsRequiredBitmap;
	OTA_JobParseErr_t xErr = eOTA_JobParseErr_None;
	const JobParamDeterminer_t *pxJobDocModel = xJobDocModel;

	pxFinalFile = NULL;
	C = prvGetFreeContext();
	if (C != NULL)
	{
		/* Reset the Jasmine tokenizer. */
		jsmn_init(&xParser);

		/* Count the total number of tokens in our JSON document. */
		ulNumTokens = jsmn_parse(&xParser, pacRawMsg, iMsgLen, NULL, 1);
		if (ulNumTokens > 0)
		{	/* If the document isn't too big for our token array... */
			if (ulNumTokens <= kOTA_MaxJSONTokens)
			{
				/* Allocate space for the document JSON tokens. */
				pxTokens = (jsmntok_t*)pvPortMalloc(ulNumTokens * sizeof(jsmntok_t));
				if (pxTokens != NULL)
				{
					/* Reset Jasmine again and tokenize the document for real. */
					jsmn_init(&xParser);
					i = jsmn_parse(&xParser, pacRawMsg, iMsgLen, pxTokens, ulNumTokens);
					if (i == ulNumTokens)
					{
						ulParamsReceivedBitmap = 0;
						ulParamsRequiredBitmap = 0;
						for (a = 0; a < kOTA_NumJobParams; a++)
						{
							if (pxJobDocModel[a].bRequired == pdTRUE)
							{
								/* Add parameter to the required bitmap. */
								ulParamsRequiredBitmap |= (1 << a);
							}
						}
						/* Examine each token, searching for job parameters based on our document model. */
						for (i = 0; ( xErr == eOTA_JobParseErr_None ) && ( i < ulNumTokens ); i++)
						{
							/* All parameter keys are JSON strings. */
							if (pxTokens[i].type == JSMN_STRING)
							{
								/* Search our job model for a key match. */
								for (a = 0; a < kOTA_NumJobParams; a++)
								{
									ulTokenLen = pxTokens[i].end - pxTokens[i].start;
									if (JSON_IsCStringEqual(&pacRawMsg[pxTokens[i].start], ulTokenLen,
										pxJobDocModel[a].pcSrcKey) == pdTRUE)
									{
										/* Per Security, don't allow multiple entries of the same parameter. */
										if ((ulParamsReceivedBitmap & (1 << a)) != 0)
										{
											xErr = eOTA_JobParseErr_DuplicatesNotAllowed;
										}
										else
										{
											/* Mark parameter as received in the bitmap. */
											ulParamsReceivedBitmap |= (1 << a);
										}
										break;
									}
								}
								if (xErr == eOTA_JobParseErr_None)
								{
									if (a == kOTA_NumJobParams)
									{	/* We didn't understand this parameter key so skip its children. */
										int iRoot = i++;
										while (i < ulNumTokens && (pxTokens[i].parent >= iRoot))
										{
											i++;	/* Skip over children of the unknown parent. */
										}
										--i;		/* Adjust for outer loop increment. */
										continue;	/* Continue with token loop. */
									}
									else
									{	/* We found a parameter key match so let's process the value field. */

										/* Get the value token. */
										pxValTok = &pxTokens[i + 1];
										/* Verify the field type is what we expect for this parameter. */
										if (pxValTok->type != pxJobDocModel[a].eJasmineType)
										{
											OTA_PRINT ("[OTA] Job doc parameter type mismatch [ %s : type %u, expected %u ]\r\n",
												pxJobDocModel[a].pcSrcKey, pxValTok->type, pxJobDocModel[a].eJasmineType);
											xErr = eOTA_JobParseErr_FieldTypeMismatch;
											break;
										}
										else if (OTA_JOB_PARAM_NOT_STORED == pxJobDocModel[a].ulDestOffset)
										{
											/* Nothing to do with this parameter since we're not storing it. */
											continue;
										}
										else
										{
											/* Get offset of parameter storage location. */
											ulValueDest = (uint32_t)pxJobDocModel[a].ulDestOffset;

											/* If it's within the file context structure, add in the context instance base address. */
											if (ulValueDest < sizeof (OTA_FileContext_t))
											{
												ulValueDest += (uint32_t)C;
											}
											else
											{
												/* It's a raw pointer so keep it as is. */
											}

											if (eJobParamType_StringCopy == pxJobDocModel[a].eModelParamType)
											{
												/* Malloc memory for a copy of the value string plus a zero terminator. */
												ulTokenLen = pxValTok->end - pxValTok->start;
												*(char**)ulValueDest = pvPortMalloc (ulTokenLen + 1);
												if (*(char**)ulValueDest != NULL)
												{
													/* Copy parameter string into newly allocated memory. */
													memcpy ((void*)*(char**)ulValueDest, (char*) &pacRawMsg[pxValTok->start], ulTokenLen);
													/* Zero terminate the new string. */
													(*(char**)ulValueDest)[ulTokenLen] = 0;
													OTA_PRINT ("[OTA] Set job doc parameter [ %s: %s ]\r\n", pxJobDocModel[a].pcSrcKey, *(char**)ulValueDest);
												}
												else
												{	/* Stop processing on error. */
													xErr = eOTA_JobParseErr_OutOfMemory;
													break;
												}
											}
											if (eJobParamType_StringInJobDoc == pxJobDocModel[a].eModelParamType)
											{
												/* Set pointer to parameter string. */
												*(char**)ulValueDest = (char*) &pacRawMsg[pxValTok->start];
												if (*(char**)ulValueDest != NULL)
												{
													ulTokenLen = pxValTok->end - pxValTok->start;
													OTA_PRINT ("[OTA] Set job doc parameter [ %s: %.*s ]\r\n", pxJobDocModel[a].pcSrcKey,
															ulTokenLen, *(char**)ulValueDest);
												}
												else
												{	/* Ignore. */
												}
											}
											else if (eJobParamType_UInt32 == pxJobDocModel[a].eModelParamType)
											{
												char *pEnd;
												const char *pStart = &pacRawMsg[pxValTok->start];
												*(uint32_t*)ulValueDest = strtoul (pStart, &pEnd, 0);
												if (pEnd == &pacRawMsg[pxValTok->end])
												{
													OTA_PRINT ("[OTA] Set job doc parameter [ %s: %u ]\r\n", pxJobDocModel[a].pcSrcKey, *(uint32_t*)ulValueDest);
												}
												else
												{
													xErr = eOTA_JobParseErr_InvalidNumChar;
												}
											}
											else if (eJobParamType_Base64 == pxJobDocModel[a].eModelParamType)
											{
												/* ToDo: Support for various signature types and sizes including just a filename. */
												/* Allocate space for and decode the base64 signature. */
												*(char**)ulValueDest = pvPortMalloc (kOTA_MaxSignatureSize);
												if (*(char**)ulValueDest != NULL)
												{
													size_t xActualLen;
													ulTokenLen = pxValTok->end - pxValTok->start;
													if (mbedtls_base64_decode (*(u8**)ulValueDest, kOTA_MaxSignatureSize, &xActualLen,
														(const u8*)&pacRawMsg[pxValTok->start], ulTokenLen) != 0)
													{	/* Stop processing on error. */
														OTA_PRINT ("[OTA] mbedtls_base64_decode failed.\r\n");
														xErr = eOTA_JobParseErr_Base64Decode;
														break;
													}
													C->usSigSize = (uint16_t)xActualLen;
													OTA_PRINT ("[OTA] Set job doc parameter [ %s: %.32s ]\r\n", pxJobDocModel[a].pcSrcKey, &pacRawMsg[pxValTok->start]);
												}
												else
												{
													/* We failed to allocate needed memory. Everything will be freed below upon failure. */
													xErr = eOTA_JobParseErr_OutOfMemory;
												}
											}
											else if (eJobParamType_Ident == pxJobDocModel[a].eModelParamType)
											{
												OTA_PRINT ("[OTA] Identified job doc parameter [ %s ]\r\n", pxJobDocModel[a].pcSrcKey);
												*(bool_t*)ulValueDest = pdTRUE;
											}
										}
									}
								}
							}
						}
						/* Free the token memory. */
						vPortFree(pxTokens);
						if (xErr == eOTA_JobParseErr_None)
						{
							uint32_t ulMissingParams = (ulParamsReceivedBitmap & ulParamsRequiredBitmap) ^ ulParamsRequiredBitmap;
							if (ulMissingParams != 0)
							{
								/* Error! Missing one or more required parameters! */
								for (a = 0; a < kOTA_NumJobParams; a++)
								{
									if ((ulMissingParams & (1 << a)) != 0)
									{
										OTA_PRINT ("[OTA] Missing job parameter: %s\r\n", pxJobDocModel[a].pcSrcKey);
									}
								}
								xErr = eOTA_JobParseErr_MalformedJobDoc;
							}
							else
							{	
								if (C->iFileSize == 0)
								{
									OTA_PRINT ("[OTA] Zero file size is not allowed!\r\n");
									xErr = eOTA_JobParseErr_ZeroFileSize;
								}
								/* If there's an active job, verify that it's the same as what's being reported now. */
								/* We already checked for missing parameters so we SHOULD have a job name in the context. */
								else if (pacOTA_Singleton_ActiveJobName != NULL)
								{
									if (C->pacJobName != NULL)
									{
										if (strcmp ((char*)pacOTA_Singleton_ActiveJobName, (char*)C->pacJobName) != 0)
										{
											OTA_PRINT ("[OTA] Busy with existing job. Ignoring.\r\n");
											xErr = eOTA_JobParseErr_BusyWithExistingJob;
										}
										else
										{	/* The same job is being reported so free the duplicate job name from the context. */
											OTA_PRINT ("[OTA] Superfluous report of current job.\r\n");
											vPortFree (C->pacJobName);
											C->pacJobName = NULL;
										}
									}
								}
								else
								{	/* Assume control of the job name from the context. */
									pacOTA_Singleton_ActiveJobName = C->pacJobName;
									C->pacJobName = NULL;
								}
								if (xErr == eOTA_JobParseErr_None)
								{
									/* If we're in self test mode, don't start the transfer and instead
									 * set the image state to testing if we're newer than the updater
									 * who put this job in self test mode. If we're older, reject the
									 * job since something unexpected has happened. If it's from ourself,
									 * just ignore it and wait for reboot to start the new firmware.
									 * If, for some reason, it's the same version after a reboot, just
									 * reject the job since the update was probably rejected by the
									 * bootloader or the same image as the one currently running was
									 * used as the update image and it's useless. */
									if (bOTA_Singleton_SelfTestMode == pdTRUE)
									{
										OTA_PRINT ("[OTA] Job is ready for self test.\r\n");
										if (C->ulUpdaterVersion < xAppFirmwareVersion.u.ulVersion32)
										{
											/* Start test mode. */
											OTA_SetImageState (eOTA_ImageState_Testing);
										}
										else if (C->ulUpdaterVersion > xAppFirmwareVersion.u.ulVersion32)
										{
											/* Running firmware is older than the updater version so reject the job. */
											OTA_PRINT ("[OTA] Rejecting job. Job updater is from the future.\r\n");
											OTA_SetImageState (eOTA_ImageState_Rejected);
										}
										else
										{
											if ( ( pacClientTokenFromJob == NULL ) ||
													( ( pacClientTokenFromJob != NULL ) && ( strtoul (pacClientTokenFromJob, NULL, 0) == 0) ) )
											{
												/* We're noted as the updater after a reboot, reject the job. */
												OTA_PRINT ("[OTA] Rejecting job. We rebooted and it is no different.\r\n");
												OTA_SetImageState (eOTA_ImageState_Rejected);
											}
											else
											{
												OTA_PRINT ("[OTA] Ignoring job. Device must be rebooted first.\r\n");
											}
										}
									}
									else
									{
										/* Everything looks OK. Set final context structure to start OTA. */
										OTA_PRINT ("[OTA] Job was accepted. Attempting to start transfer.\r\n");
										pxFinalFile = C;
									}
								}
								else
								{
									OTA_PRINT ("[OTA] Error (%u) parsing job document.\r\n", (uint32_t)xErr);
								}
							}
						}
						else
						{
							OTA_PRINT ("[OTA] Error (%u) parsing job document.\r\n", (uint32_t)xErr);
						}
					}
					else
					{
						OTA_PRINT("[OTA] jsmn_parse didn't match token count when parsing.\r\n");
					}
				}
				else
				{
					OTA_PRINT("[OTA] No memory for JSON tokens.\r\n");
				}
			}
			else
			{
				OTA_PRINT("[OTA] Job document has too many keys.\r\n");
			}
		}
		else
		{
			OTA_PRINT("[OTA] Invalid job document.\r\n");
		}
	}
	/* If job parsing failed and there's a job ID, update the job state to REJECTED. */
	if ((xErr != eOTA_JobParseErr_None) && (C->pacJobName != NULL))
	{
		/* Assume control of the job name from the context. */
		pacOTA_Singleton_ActiveJobName = C->pacJobName;
		C->pacJobName = NULL;
		OTA_PRINT ("[OTA] Rejecting job.\r\n");
		OTA_SetImageState (eOTA_ImageState_Rejected);
	}
	/* If we failed, free the reserved file context (C) to make it available again. */
	if (pxFinalFile == NULL)
	{
		prvOTA_Close(C);
	}
	/* Return pointer to populated file context or NULL if it failed. */
	return pxFinalFile;
}



/* prvProcessOTAJobMsg
*
* We received new OTA update job message from the job notification service.
* Process the message and prepare for receiving the file as needed. */

OTA_FileContext_t* prvProcessOTAJobMsg(const char *pacRawMsg, u32 iMsgLen) {

	u32		i;
	u32		iNumBlocks;												/* How many data pages are in the expected update image. */
	u32		iBitmapLen;												/* Length of the file block bitmap in bytes. */
	OTA_FileContext_t *pstUpdateFile = NULL;						/* Pointer to an OTA update file. */


	/* Get the file metadata from the OTA job document. */
	pstUpdateFile = prvParseJobDocFromJSON (pacRawMsg, iMsgLen);

	if (pstUpdateFile != NULL) {

		if (pstUpdateFile->pacRxBlockBitmap != NULL) {
			vPortFree(pstUpdateFile->pacRxBlockBitmap);				/* Free any previously allocated bitmap. */
			pstUpdateFile->pacRxBlockBitmap = NULL;
		}
		/* Calculate how many bytes we need in our bitmap for tracking received blocks.
		The below calculation requires power of 2 page sizes. */

		iNumBlocks = (pstUpdateFile->iFileSize + (kOTA_FileBlockSize - 1)) >> LOG2_16BIT(kOTA_FileBlockSize);
		iBitmapLen = (iNumBlocks + (kBitsPerByte - 1)) >> LOG2_8BIT(kBitsPerByte);
		pstUpdateFile->pacRxBlockBitmap = pvPortMalloc(iBitmapLen);
		if (pstUpdateFile->pacRxBlockBitmap) {

			if (prvSubscribeToDataStream(pstUpdateFile) == pdTRUE) {

				/* Set all bits in the bitmap to the erased state (we use 1 for erased just like flash memory). */
				memset(pstUpdateFile->pacRxBlockBitmap, kmOTA_ErasedBlocksVal, iBitmapLen);

				/* Mark as used any pages in the bitmap that are out of range, based on the file size.
				This keeps us from requesting those pages during retry processing or if using a windowed
				block request. It also avoids erroneously accepting an out of range data block should it
				get past any safety checks.
				Files aren't always a multiple of 8 pages (8 bits/pages per byte) so some bits of the
				last byte may be out of range and those are the bits we want to clear. */

				u32 bit = 1 << (kBitsPerByte - 1);
				u32 n = (iBitmapLen * kBitsPerByte) - iNumBlocks;
				for (i = 0; i < n; i++) {
					pstUpdateFile->pacRxBlockBitmap[iBitmapLen - 1] &= ~bit;
					bit >>= 1;
				}
				pstUpdateFile->iBlocksRemaining = iNumBlocks;		/* Initialize our blocks remaining counter. */
				prvStartRequestTimer(pstUpdateFile);

				/* Create/Open the OTA file on the file system. */
				/* ToDo: Set mFlags in context struct to support secure, bundle, etc. */
				if ( prvCreateFileForRx(pstUpdateFile) == pdFALSE)
				{
					OTA_SetImageState (eOTA_ImageState_Rejected);
					prvOTA_Close(pstUpdateFile);
					pstUpdateFile = NULL;
				}
			}
			else {
				/* Can't receive the image without a subscription. */
				prvOTA_Close(pstUpdateFile);
				pstUpdateFile = NULL;
			}
		}
		else {
			/* Can't receive the image without enough memory. */
			prvOTA_Close(pstUpdateFile);
			pstUpdateFile = NULL;
		}
	}
	return pstUpdateFile;                                           /* Return the OTA file context. */
}


/* prvIngestDataBlock
 *
 * A block of file data was received by the application via some configured communication protocol.
 * If it looks like it is in range, write it to persistent storage. If it's the last block we're
 * expecting, perform the final signature check on the overall file and prepare it for use if the
 * signature check passes. If the signature check fails, abort that file transfer.
 */
IngestResult_t prvIngestDataBlock( OTA_FileContext_t * C,
                                   const char * pacRawMsg,
                                   u32 iMsgSize )
{
    IngestResult_t eIngestResult = eIngest_Result_Continue;
    int lFileId = 0;
    int lBlockSize = 0;
    int lBlockIndex = 0;
    uint8_t *pucPayload = NULL;
    size_t xPayloadSize = 0;

    if( C != NULL )
    {
        /* If we have a block bitmap available then process the message. */
        if( C->pacRxBlockBitmap && ( C->iBlocksRemaining > 0 ) )
        {
            /* Reset or start the firmware request timer. */
            prvStartRequestTimer( C );

			/* Decode the CBOR content. */
            if( pdFALSE == OTA_CBOR_Decode_GetStreamResponseMessage(
                ( uint8_t * ) pacRawMsg,
                iMsgSize,
                &lFileId,
                &lBlockIndex,
                &lBlockSize,
                &pucPayload,
                &xPayloadSize ) )
            {
                eIngestResult = eIngest_Result_BadData;
            }
            else
			{
                /* Validate the block index and size. */
                /* If it is NOT the last block, it MUST be equal to a full block size. */
                /* If it IS the last block, it MUST be equal to the expected remainder. */
                /* If the block ID is out of range, that's an error. */
				u32 iLastBlock = ( ( C->iFileSize + ( kOTA_FileBlockSize - 1 ) ) >> LOG2_16BIT( kOTA_FileBlockSize ) ) - 1;

                if( ( ( ( u32 )lBlockIndex < iLastBlock ) && ( lBlockSize == kOTA_FileBlockSize ) ) ||
                    ( ( ( u32 )lBlockIndex == iLastBlock ) && ( ( u32 )lBlockSize == ( C->iFileSize - ( iLastBlock * kOTA_FileBlockSize ) ) ) ) )
                {
                    OTA_PRINT( "[OTA] Received file block %u, size %u\r\n", lBlockIndex, lBlockSize );

                    u32 mBit = 1 << ( lBlockIndex % kBitsPerByte );      /* Create bit mask for use in our bitmap. */
                    u32 byte = lBlockIndex >> LOG2_8BIT( kBitsPerByte ); /* Calculate byte offset into bitmap. */

                    if( ( C->pacRxBlockBitmap[byte] & mBit ) == 0 )    /* If we've already received this block... */
                    {
                        OTA_PRINT( "[OTA] block %u is a DUPLICATE\r\n", lBlockIndex );
                    }
                    else /* Otherwise, process it normally... */
                    {
                        if( C->iFileHandle != ( s32 )NULL )
                        {
                            i32 iBytesWritten = prvWriteBlock( C, ( lBlockIndex * kOTA_FileBlockSize ), pucPayload, ( u32 )lBlockSize );

                            if( iBytesWritten < 0 )
                            {
                                OTA_PRINT( "[OTA] Error (%d) writing file block\r\n", iBytesWritten );
                                eIngestResult = eIngest_Result_WriteBlockFailed;
                            }
                            else
                            {
                                C->pacRxBlockBitmap[byte] &= ~mBit; /* Mark this block as received in our bitmap. */
                                C->iBlocksRemaining--;
                            }
                        }
                        else
                        {
                            OTA_PRINT( "[OTA] Error: Illegal file handle (%d) therefore unable to write block.\r\n", ( s32 )C->iFileHandle );
                            eIngestResult = eIngest_Result_BadFileHandle;
                        }

                        if( C->iBlocksRemaining == 0 )
                        {
                            prvStopRequestTimer( C );         /* Don't request any more since we're done. */
                            vPortFree( C->pacRxBlockBitmap ); /* Free the bitmap now that we're done with the download. */
                            C->pacRxBlockBitmap = NULL;

                            if( C->iFileHandle != ( s32 )NULL )
                            {
                                s32 iCloseResult = prvCloseFile( C );

                                if( iCloseResult == 0 )
                                {
                                    OTA_PRINT( "[OTA] File receive complete and signature is valid.\r\n" );
                                    eIngestResult = eIngest_Result_FileComplete;
                                }
                                else
                                {
                                    OTA_PRINT( "[OTA] Error (%u:%d) closing OTA file.\r\n", iCloseResult >> kOTA_MainErrShiftDownBits, iCloseResult & kOTA_PAL_ErrMask );
                                    eIngestResult = eIngest_Result_SigCheckFail;
                                }
                                C->iFileHandle = ( s32 )NULL; /* File is now closed so clear the file handle in the context. */
                            }
                            else
                            {
                                OTA_PRINT( "[OTA] Error: Illegal file handle (%d) after last block received.\r\n", ( s32 )C->iFileHandle );
                                eIngestResult = eIngest_Result_BadFileHandle;
                            }
                        }
                        else
                        {
                            OTA_PRINT( "[OTA] Remaining: %u\r\n", C->iBlocksRemaining );
                        }
                    }
#if OTA_PRINT_DATA_BLOCKS
                    prvPrintDataBlock( pxDataMsg->acData, pxDataMsg->iBlockSize );
#endif
                }
                else
                {
                    OTA_PRINT( "[OTA] Error! Block %u out of expected range!\r\n", lBlockIndex );
                    eIngestResult = eIngest_Result_BlockOutOfRange;
                }
            }
        }
        else
        {
            eIngestResult = eIngest_Result_UnexpectedBlock;
        }
    }
    else
    {
        eIngestResult = eIngest_Result_NullContext;
    }
	if( NULL != pucPayload )
    {
        vPortFree( pucPayload );
    }
    return eIngestResult;
}



/* Construct the dynamic OTA job status topic name into into the buffer supplied
* by the caller. If there is an error, the buffer is not modified and the
* returned topic length shall be zero, otherwise we return the actual size
* of the constructed topic.
*/
static uint32_t prvBuildJobStatusTopicName (char *pcNameBuffer, uint32_t ulBufSize, const char *pacJobName)
{
	uint32_t ulTopicLen = 0;

	/* Calculate the length of the topic and verify it is not too big for the supplied buffer. We
	* should subtract 2 from the length to accomodate the extra '%s' characters in the template
	* but we also need to account for the zero terminator so only subtract 1. */

	if (pacJobName != NULL)
	{
		ulTopicLen = strlen (pacJobName) + CONST_STRLEN (acOTA_JobStatus_TopicTemplate) - 1;
	}
	if (ulTopicLen <= ulBufSize)
	{
		ulTopicLen = rsprintf (pcNameBuffer, acOTA_JobStatus_TopicTemplate, pacJobName);
	}
	else
	{	/* On buffer overflow, set topic length to zero. */
		ulTopicLen = 0;
	}
	return ulTopicLen;
}


/* Construct the dynamic OTA data REQUEST topic name into into the buffer supplied
 * by the caller. If there is an error, the buffer is not modified and the
 * returned topic length shall be zero, otherwise we return the actual size
 * of the constructed topic.
 */
static uint32_t prvBuildDataRequestTopicName (char *pcNameBuffer, uint32_t ulBufSize, OTA_FileContext_t * C)
{
	/* Calculate the length of the topic and verify it is not too big for the supplied buffer. We
	* should subtract 2 from the length to accomodate the extra '%s' characters in the template
	* but we also need to account for the zero terminator so only subtract 1. */

	uint32_t ulTopicLen = strlen ( (const char *) C->pacStreamName) + CONST_STRLEN (acOTA_Request_TopicTemplate) - 1;
	if (ulTopicLen <= ulBufSize)
	{
		ulTopicLen = rsprintf (pcNameBuffer, acOTA_Request_TopicTemplate, C->pacStreamName);
	}
	else
	{	/* On error, set topic length to zero. */
		ulTopicLen = 0;
	}
	return ulTopicLen;
}


/* Construct the dynamic OTA data stream topic name into the buffer supplied
 * by the caller. If there is an error, the buffer is not modified and the
 * returned topic length shall be zero, otherwise we return the actual size
 * of the constructed topic.
 */
static uint32_t prvBuildDataStreamTopicName (char *pcNameBuffer, uint32_t ulBufSize, OTA_FileContext_t * C)
{
	/* Calculate the length of the topic and verify it is not too big for the supplied buffer. We
	 * should subtract 2 from the length to accomodate the extra '%s' characters in the template
	 * but we also need to account for the zero terminator so only subtract 1. */

	uint32_t ulTopicLen = strlen ( (const char *) C->pacStreamName) + CONST_STRLEN (acOTA_RxStream_TopicTemplate) - 1;
	if (ulTopicLen <= ulBufSize)
	{
		ulTopicLen = rsprintf (pcNameBuffer, acOTA_RxStream_TopicTemplate, C->pacStreamName);
	}
	else
	{	/* On error, set topic length to zero. */
		ulTopicLen = 0;
	}
	/* Return length of topic not including the zero terminator added by rsprintf. */
	return ulTopicLen;
}


/* Subscribe to the job notification topic (i.e. New file version available). */

static bool_t prvSubscribeToJobNotificationTopic (void)
{
	bool_t bResult = pdFALSE;

	if (MQTT_AGENT_Subscribe (pvPubSubClient, &stJobsNotificationSubscription, (TickType_t)kOTA_SubscribeWaitInTicks) != eMQTTAgentSuccess)
	{
		OTA_PRINT ("[OTA] Error when subscribing to topic: %s\n\r", stJobsNotificationSubscription.pucTopic);
		MQTT_AGENT_Disconnect (pvPubSubClient, (TickType_t)0);
	}
	else
	{
		OTA_PRINT ("[OTA] Subscribed to topic: %s\n\r", stJobsNotificationSubscription.pucTopic);
		bResult = pdTRUE;
	}
	if (MQTT_AGENT_Subscribe (pvPubSubClient, &stJobsNotifyNextSubscription, (TickType_t)kOTA_SubscribeWaitInTicks) != eMQTTAgentSuccess)
	{
		OTA_PRINT ("[OTA] Error when subscribing to topic: %s\n\r", stJobsNotifyNextSubscription.pucTopic);
		MQTT_AGENT_Disconnect (pvPubSubClient, (TickType_t)0);
	}
	else
	{
		OTA_PRINT ("[OTA] Subscribed to topic: %s\n\r", stJobsNotifyNextSubscription.pucTopic);
		bResult = pdTRUE;
	}
	return bResult;
}


/* Subscribe to the OTA data stream topic. */

static bool_t prvSubscribeToDataStream(OTA_FileContext_t * C)
{
	bool_t bResult = pdFALSE;
	char acOTA_RxStreamTopic[kOTA_MaxDynamicTopicNameLen];
	MQTTAgentSubscribeParams_t stOTAUpdateDataSubscription;

	memset (&stOTAUpdateDataSubscription, 0, sizeof (stOTAUpdateDataSubscription));
	stOTAUpdateDataSubscription.xQoS = eMQTTQoS1;
	stOTAUpdateDataSubscription.pucTopic = (const uint8_t*)acOTA_RxStreamTopic;
	stOTAUpdateDataSubscription.usTopicLength = prvBuildDataStreamTopicName (acOTA_RxStreamTopic, kOTA_MaxDynamicTopicNameLen, C);
	stOTAUpdateDataSubscription.pxPublishCallback = prvOTAPublishCallback;

	if (stOTAUpdateDataSubscription.usTopicLength > 0)
	{
		if (MQTT_AGENT_Subscribe (pvPubSubClient, &stOTAUpdateDataSubscription, (TickType_t)kOTA_SubscribeWaitInTicks) != eMQTTAgentSuccess)
		{
			OTA_PRINT ("[OTA] Error when subscribing to topic: %s\n\r", stOTAUpdateDataSubscription.pucTopic);
			MQTT_AGENT_Disconnect (pvPubSubClient, (TickType_t)0);
		}
		else
		{
			OTA_PRINT ("[OTA] Subscribed to topic: %s\n\r", stOTAUpdateDataSubscription.pucTopic);
			bResult = pdTRUE;
		}
	}
    return bResult;
}


/* UnSubscribe from the OTA data stream topic. */

static bool_t prvUnSubscribeFromDataStream(OTA_FileContext_t * C)
{
    MQTTAgentUnsubscribeParams_t stUnSub;
    bool_t bResult = pdFALSE;
	char acOTA_RxStreamTopic[kOTA_MaxDynamicTopicNameLen];

	if (C != NULL)
	{
		/* Try to build the dynamic data stream topic and un-subscribe from it. */

		stUnSub.usTopicLength = prvBuildDataStreamTopicName (acOTA_RxStreamTopic, kOTA_MaxDynamicTopicNameLen, C);

		if (stUnSub.usTopicLength > 0)
		{
			stUnSub.pucTopic = (const uint8_t *)acOTA_RxStreamTopic;
			if (MQTT_AGENT_Unsubscribe (pvPubSubClient, &stUnSub, kOTA_UnSubscribeWaitInTicks) != eMQTTAgentSuccess)
			{
				OTA_PRINT ("[OTA] Error when un-subscribing from topic: %s\n\r", acOTA_RxStreamTopic);
				MQTT_AGENT_Disconnect (pvPubSubClient, (TickType_t)0);
			}
			else
			{
				OTA_PRINT ("[OTA] Un-subscribed from topic: %s\n\r", acOTA_RxStreamTopic);
				bResult = pdTRUE;
			}
		}
	}
    return bResult;
}


/* UnSubscribe from the OTA job notification topic. */

static bool_t prvUnSubscribeFromJobNotificationTopic( void )
{
    MQTTAgentUnsubscribeParams_t stUnSub;
    bool_t bResult = pdFALSE;

    stUnSub.pucTopic = stJobsNotificationSubscription.pucTopic;
    stUnSub.usTopicLength = stJobsNotificationSubscription.usTopicLength;

    if( MQTT_AGENT_Unsubscribe( pvPubSubClient, &stUnSub, kOTA_UnSubscribeWaitInTicks ) != eMQTTAgentSuccess )
    {
        OTA_PRINT( "[OTA] Error when un-subscribing from topic: %s\n\r", stJobsNotificationSubscription.pucTopic );
        MQTT_AGENT_Disconnect( pvPubSubClient, ( TickType_t ) 0 );
    }
    else
    {
        OTA_PRINT( "[OTA] Un-subscribed from topic: %s\n\r", stJobsNotificationSubscription.pucTopic );
        bResult = pdTRUE;
    }
	return bResult;
}


/* Publish a message to the specified client/topic at the given QOS. */

static MQTTAgentReturnCode_t prvPublishMessage( void * const pvClient,
                              const char * const pacTopic,
                              u16 usTopicLen,
                              char * pacMsg,
                              u32 ulMsgSize,
							  MQTTQoS_t eQOS )
{
    MQTTAgentPublishParams_t xPublishParams;

    xPublishParams.pucTopic = ( const uint8_t * ) pacTopic;
    xPublishParams.usTopicLength = usTopicLen;
    xPublishParams.xQoS = eQOS;
    xPublishParams.pvData = pacMsg;
    xPublishParams.ulDataLength = ulMsgSize;
    return MQTT_AGENT_Publish( pvClient, &xPublishParams, ( TickType_t )kOTA_PublishWaitInTicks);
}


#if OTA_PRINT_DATA_BLOCKS
/* Print out the received data block to the debug terminal. */

    static void prvPrintDataBlock( u8 * pacData,
                                   u32 iSize )
    {
        u32 i;

        for( i = 0; i < iSize; i++ )
        {
            OTA_PRINT( "%02x ", *pacData++ );
        }

        OTA_PRINT( "\r\n" );
    }
#endif /* if OTA_PRINT_DATA_BLOCKS */

/*-----------------------------------------------------------*/
/* Private OTA abstractions. These are statics so the implementation is included in-line. */
#include "aws_ota_pal.c"

/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_agent_internal.c"
#endif
