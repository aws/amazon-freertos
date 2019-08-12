/*
 * FreeRTOS+TCP Multi Interface Labs Build 180222
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Authors include Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   This is a version of FreeRTOS+TCP that supports multiple network      ***
 ***   interfaces, and includes basic IPv6 functionality.  Unlike the base   ***
 ***   version of FreeRTOS+TCP, THE MULTIPLE INTERFACE VERSION IS STILL IN   ***
 ***   THE LAB.  While it is functional and has been used in commercial      ***
 ***   products we are still refining its design, the source code does not   ***
 ***   yet quite conform to the strict coding and style standards, and the   ***
 ***   documentation and testing is not complete.                            ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact                                  ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
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

#ifndef FREERTOS_DHCP_H
#define FREERTOS_DHCP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Application level configuration options. */
#include "FreeRTOSIPConfig.h"
#include "IPTraceMacroDefaults.h"

/* Used in the DHCP callback if ipconfigUSE_DHCP_HOOK is set to 1. */
typedef enum eDHCP_PHASE
{
	eDHCPPhasePreDiscover,	/* Driver is about to send a DHCP discovery. */
	eDHCPPhasePreRequest	/* Driver is about to request DHCP an IP address. */
} eDHCPCallbackPhase_t;

/* Used in the DHCP callback if ipconfigUSE_DHCP_HOOK is set to 1. */
typedef enum eDHCP_ANSWERS
{
	eDHCPContinue,			/* Continue the DHCP process */
	eDHCPUseDefaults,		/* Stop DHCP and use the static defaults. */
	eDHCPStopNoChanges,		/* Stop DHCP and continue with current settings. */
} eDHCPCallbackAnswer_t;

/* DHCP state machine states. */
typedef enum
{
	eWaitingSendFirstDiscover = 0,	/* Initial state.  Send a discover the first time it is called, and reset all timers. */
	eWaitingOffer,					/* Either resend the discover, or, if the offer is forthcoming, send a request. */
	eWaitingAcknowledge,			/* Either resend the request. */
	#if( ipconfigDHCP_FALL_BACK_AUTO_IP != 0 )
		eGetLinkLayerAddress,		/* When DHCP didn't respond, try to obtain a LinkLayer address 168.254.x.x. */
	#endif
	eLeasedAddress,					/* Resend the request at the appropriate time to renew the lease. */
	eNotUsingLeasedAddress			/* DHCP failed, and a default IP address is being used. */
} eDHCPState_t;

/* Hold information in between steps in the DHCP state machine. */
struct xDHCP_DATA
{
	uint32_t ulTransactionId;
	uint32_t ulOfferedIPAddress;
	uint32_t ulDHCPServerAddress;
	uint32_t ulLeaseTime;
	/* Hold information on the current timer state. */
	TickType_t xDHCPTxTime;
	TickType_t xDHCPTxPeriod;
	/* Try both without and with the broadcast flag */
	BaseType_t xUseBroadcast;
	/* Maintains the DHCP state machine state. */
	eDHCPState_t eDHCPState;
	Socket_t xDHCPSocket;
};

typedef struct xDHCP_DATA DHCPData_t;

/*
 * NOT A PUBLIC API FUNCTION.
 */
void vDHCPProcess( BaseType_t xReset, struct xNetworkEndPoint *pxEndPoint );

/* Internal call: returns true if socket is the current DHCP socket */
BaseType_t xIsDHCPSocket( Socket_t xSocket );

/* Prototype of the hook (or callback) function that must be provided by the
application if ipconfigUSE_DHCP_HOOK is set to 1.  See the following URL for
usage information:
http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_IP_Configuration.html#ipconfigUSE_DHCP_HOOK
*/
eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress );

#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif /* FREERTOS_DHCP_H */













