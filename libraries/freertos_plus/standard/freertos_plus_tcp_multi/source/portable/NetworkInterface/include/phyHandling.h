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

 /*
 * Handling of Ethernet PHY's
 * PHY's communicate with an EMAC either through
 * a Media-Independent Interface (MII), or a Reduced Media-Independent Interface (RMII).
 * The EMAC can poll for PHY ports on 32 different addresses. Each of the PHY ports
 * shall be treated independently.
 * 
 */

#ifndef PHYHANDLING_H

#define PHYHANDLING_H

#ifdef __cplusplus
extern "C" {
#endif


#ifndef ipconfigPHY_MAX_PORTS
	/* There can be at most 32 PHY ports, but in most cases there are 4 or less. */
	#define	ipconfigPHY_MAX_PORTS	4
#endif

/* A generic user-provided function that reads from the PHY-port at 'xAddress'( 0-based ). A 16-bit value shall be stored in
  '*pusValue'. xRegister is the register number ( 0 .. 31 ). In fact all PHY registers are 16-bit.
  Return non-zero in case the action failed. */
typedef BaseType_t ( *xApplicationPhyReadHook_t )( BaseType_t xAddress, BaseType_t xRegister, uint32_t *pulValue );

/* A generic user-provided function that writes 'usValue' to the
   PHY-port at 'xAddress' ( 0-based ). xRegister is the register number ( 0 .. 31 ).
   Return non-zero in case the action failed. */
typedef BaseType_t ( *xApplicationPhyWriteHook_t )( BaseType_t xAddress, BaseType_t xRegister, uint32_t ulValue );

typedef struct xPhyProperties
{
	uint8_t ucSpeed;
	uint8_t ucMDI_X;		/* MDI-X : Medium Dependent Interface - Crossover */
	uint8_t ucDuplex;
	uint8_t ucSpare;
} PhyProperties_t;

typedef struct xEthernetPhy
{
	xApplicationPhyReadHook_t fnPhyRead;
	xApplicationPhyWriteHook_t fnPhyWrite;
	uint32_t ulPhyIDs[ ipconfigPHY_MAX_PORTS ];
	uint8_t ucPhyIndexes[ ipconfigPHY_MAX_PORTS ];
	TimeOut_t xLinkStatusTimer;
	TickType_t xLinkStatusRemaining;
	BaseType_t xPortCount;
	uint32_t ulBCRValue;
	uint32_t ulACRValue;
	uint32_t ulLinkStatusMask;
	PhyProperties_t xPhyPreferences;
	PhyProperties_t xPhyProperties;
} EthernetPhy_t;

/* Some defines used internally here to indicate preferences about speed, MDIX
(wired direct or crossed), and duplex (half or full). */

/* Values for PhyProperties_t::ucSpeed : */
#define	PHY_SPEED_10		1
#define	PHY_SPEED_100		2
#define	PHY_SPEED_AUTO		3

/* Values for PhyProperties_t::ucMDI_X : */
#define	PHY_MDIX_DIRECT		1
#define	PHY_MDIX_CROSSED	2
#define	PHY_MDIX_AUTO		3

/* Values for PhyProperties_t::ucDuplex : */
#define	PHY_DUPLEX_HALF		1
#define	PHY_DUPLEX_FULL		2
#define	PHY_DUPLEX_AUTO		3

/* ID's of supported PHY's : */
#define PHY_ID_LAN8742A		0x0007c130
#define PHY_ID_LAN8720		0x0007c0f0

#define PHY_ID_KSZ8041		0x000010A1
#define PHY_ID_KSZ8051		0x000010A1
#define PHY_ID_KSZ8081		0x000010A1

#define PHY_ID_KSZ8863		0x00221430
#define PHY_ID_KSZ8081MNXIA 0x00221560

#define PHY_ID_DP83848I		0x20005C90


/* Initialise the struct and assign a PHY-read and -write function. */
void vPhyInitialise( EthernetPhy_t *pxPhyObject, xApplicationPhyReadHook_t fnPhyRead, xApplicationPhyWriteHook_t fnPhyWrite );

/* Discover all PHY's connected by polling 32 indexes ( zero-based ) */
BaseType_t xPhyDiscover( EthernetPhy_t *pxPhyObject );

/* Send a reset commando to the connected PHY ports and send configuration. */
BaseType_t xPhyConfigure( EthernetPhy_t *pxPhyObject, const PhyProperties_t *pxPhyProperties );

/* Give a commando to start auto negotiation on a set of PHY port's. */
BaseType_t xPhyStartAutoNegotiation( EthernetPhy_t *pxPhyObject, uint32_t ulPhyMask );

/* Do not use auto negotiation but use predefined values from 'pxPhyObject->xPhyPreferences'. */
BaseType_t xPhyFixedValue( EthernetPhy_t *pxPhyObject, uint32_t ulPhyMask );

/* Check the current Link Status.
'xHadReception' : make this true if a packet has been received since the
last call to this function. */
BaseType_t xPhyCheckLinkStatus( EthernetPhy_t *pxPhyObject, BaseType_t xHadReception );

static __inline uint32_t xPhyGetMask( EthernetPhy_t *pxPhyObject )
{
	return ( ( ( uint32_t ) 1u ) << pxPhyObject-> xPortCount ) - 1;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
