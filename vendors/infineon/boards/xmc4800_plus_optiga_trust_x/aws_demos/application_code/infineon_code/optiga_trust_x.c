#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "optiga/optiga_util.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"
#include "mbedtls/base64.h"

/* PKCS#11 includes. */
#include "aws_pkcs11.h"

/* Key provisioning includes. */
#include "aws_dev_mode_key_provisioning.h"

/* OPTIGA Trust X defines. */
#define mainTrustX_TASK_STACK_SIZE     		( configMINIMAL_STACK_SIZE * 8 )

char CLIENT_CERTIFICATE_PEM[1124];
uint32_t CLIENT_CERTIFICATE_LENGTH;

// Not used, but required for mbedTLS...
const char CLIENT_PRIVATE_KEY_PEM[] =
"-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIH84O6hf145Awun9lH6GbesjSmwJ5w1ODaYhp7xLZRA3oAoGCCqGSM49\n"
"AwEHoUQDQgAEb5ERgUCa0019j5JWdEiH0JyldJF9cv/MW2Lx94n9HVJ9FiQcooFi\n"
"KegulIrIuCLE1a7zsv8Izs/6gY9rxOdHHA==\n"
"-----END EC PRIVATE KEY-----\n";

/*
 * Length of device private key included from aws_clientcredential_keys.h .
 */
const uint32_t CLIENT_PRIVATE_KEY_LENGTH = sizeof( CLIENT_PRIVATE_KEY_PEM );


optiga_comms_t optiga_comms = {(void*)&ifx_i2c_context_0,NULL,NULL, OPTIGA_COMMS_SUCCESS};
static TimerHandle_t xTrustXInitTimer;
SemaphoreHandle_t xTrustXSemaphoreHandle; /**< OPTIGA™ Trust X module semaphore. */
const TickType_t xTrustXSemaphoreWaitTicks = pdMS_TO_TICKS( 60000 );

extern optiga_lib_status_t  example_authenticate_chip(void);

static void read_ifx_cert(void)
{
		uint8_t ifx_cert_hex[800];
		uint16_t  ifx_cert_hex_len = sizeof(ifx_cert_hex);
		size_t  ifx_cert_b64_len = 0;
		uint8_t ifx_cert_b64_temp[1124];
		uint16_t offset_to_read = 0;
		uint16_t offset_to_write = 0;
		uint16_t size_to_copy = 0;
		optiga_lib_status_t status;

		// IFX_CHECK: eDEVICE_PUBKEY_CERT_IFX or eDEVICE_PUBKEY_CERT_PRJSPC_1
		while ( (status = optiga_util_read_data(eDEVICE_PUBKEY_CERT_PRJSPC_1, 0, ifx_cert_hex, &ifx_cert_hex_len)) != OPTIGA_LIB_SUCCESS);

		mbedtls_base64_encode((unsigned char *)ifx_cert_b64_temp, sizeof(ifx_cert_b64_temp),
								&ifx_cert_b64_len,
								ifx_cert_hex + 9, ifx_cert_hex_len - 9);

		memcpy(CLIENT_CERTIFICATE_PEM, "-----BEGIN CERTIFICATE-----\n", 28);
		offset_to_write += 28;

		//Properly copy certificate and format it as pkcs expects
		for (offset_to_read = 0; offset_to_read < ifx_cert_b64_len;)
		{
			// The last block of data usually is less than 64, thus we need to find the leftover
			if ((offset_to_read + 64) >= ifx_cert_b64_len)
				size_to_copy = ifx_cert_b64_len - offset_to_read;
			else
				size_to_copy = 64;
			memcpy(CLIENT_CERTIFICATE_PEM + offset_to_write, ifx_cert_b64_temp + offset_to_read, size_to_copy);
			offset_to_write += size_to_copy;
			offset_to_read += size_to_copy;
			CLIENT_CERTIFICATE_PEM[offset_to_write] = '\n';
			offset_to_write++;
		}

		memcpy(CLIENT_CERTIFICATE_PEM + offset_to_write, "-----END CERTIFICATE-----\n\0", 27);

		CLIENT_CERTIFICATE_LENGTH = offset_to_write + 27;
}

void vTrustXInitCallback( TimerHandle_t xTimer )
{
	xSemaphoreGive(xTrustXSemaphoreHandle);
}

void vTrustXTaskCallbackHandler( void * pvParameters )
{
	optiga_lib_status_t status = OPTIGA_LIB_ERROR;

	if ( xSemaphoreTake(xTrustXSemaphoreHandle, xTrustXSemaphoreWaitTicks) == pdTRUE )
	{
		pal_os_event_init();

		status = optiga_util_open_application(&optiga_comms);

		status = example_authenticate_chip();

		read_ifx_cert();

	    ProvisioningParams_t xParams;

	    xParams.ulClientPrivateKeyType = CKK_EC;
	    xParams.pcClientPrivateKey = ( uint8_t * ) CLIENT_PRIVATE_KEY_PEM;
	    xParams.ulClientPrivateKeyLength = CLIENT_PRIVATE_KEY_LENGTH;
	    xParams.pcClientCertificate = ( uint8_t * ) CLIENT_CERTIFICATE_PEM;
	    xParams.ulClientCertificateLength = CLIENT_CERTIFICATE_LENGTH;

	    vAlternateKeyProvisioning( &xParams );

        DEMO_RUNNER_RunDemos();
	}

	vTaskDelete(NULL);
}

void OPTIGA_TRUST_X_Init(void)
{
	/* Create the handler for the callbacks. */
	xTaskCreate( vTrustXTaskCallbackHandler,       /* Function that implements the task. */
				"TrstXHndlr",          /* Text name for the task. */
				configMINIMAL_STACK_SIZE*8,      /* Stack size in words, not bytes. */
				NULL,    /* Parameter passed into the task. */
				tskIDLE_PRIORITY,/* Priority at which the task is created. */
				NULL );      /* Used to pass out the created task's handle. */

	xTrustXSemaphoreHandle = xSemaphoreCreateBinary();

	xTrustXInitTimer = xTimerCreate("TrustX_init_timer",        /* Just a text name, not used by the kernel. */
									pdMS_TO_TICKS(1),    /* The timer period in ticks. */
									pdFALSE,         /* The timers will auto-reload themselves when they expire. */
									( void * )NULL,   /* Assign each timer a unique id equal to its array index. */
									vTrustXInitCallback  /* Each timer calls the same callback when it expires. */
									);
	if( xTrustXInitTimer == NULL )
	{
		// The timer was not created.
	}
	else
	{
		// Start the timer.  No block time is specified, and even if one was
		// it would be ignored because the scheduler has not yet been
		// started.
		if( xTimerStart( xTrustXInitTimer, 0 ) != pdPASS )
		{
		    // The timer could not be set into the Active state.
		}
	}
}

