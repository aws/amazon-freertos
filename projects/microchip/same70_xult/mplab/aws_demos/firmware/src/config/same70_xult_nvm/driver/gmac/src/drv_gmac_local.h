/*******************************************************************************
  Ethernet MAC Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gmac_local.h

  Summary:
    Ethernet MAC driver local declarations and definitions.

  Description:
    This file contains the Ethernet MAC driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

//DOM-IGNORE-END

#ifndef _DRV_GMAC_LOCAL_H
#define _DRV_GMAC_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "system_config.h"
#include "system/system.h"
#include "system/int/sys_int.h"
#include "system/time/sys_time.h"
#include "tcpip/tcpip_mac_object.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************
/** MAC address definition.  The MAC address must be unique on the network. */
#define GMAC_CONF_ETHADDR0                        0x00
#define GMAC_CONF_ETHADDR1                        0x04
#define GMAC_CONF_ETHADDR2                        0x25
#define GMAC_CONF_ETHADDR3                        0x1C
#define GMAC_CONF_ETHADDR4                        0xA0
#define GMAC_CONF_ETHADDR5                        0x02

// *****************************************************************************
/* Ethernet Event Flags

  Summary:
    Ethernet event flags

  Description:
    This enumeration defines flags for the possible Ethernet events that can
    cause interrupts.
*/
typedef enum{
    // All events
	GMAC_EV_NONE
    /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
	// Management Frame Sent
	GMAC_EV_MGMTFRAMESENT
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_MFS_Msk /*DOM-IGNORE-END*/,
	
	// Receive Complete
	GMAC_EV_RXCOMPLETE
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_RCOMP_Msk /*DOM-IGNORE-END*/,
	
	// RX Used Bit Read
	GMAC_EV_RXUSEDBITREAD
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_RXUBR_Msk /*DOM-IGNORE-END*/,
	
	// TX Used Bit Read
	GMAC_EV_TXUSEDBITREAD
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_TXUBR_Msk /*DOM-IGNORE-END*/,
	
	// Transmit Underrun
	GMAC_EV_TXUNDERRUN
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_TUR_Msk /*DOM-IGNORE-END*/,
	
	// Retry Limit Exceeded
	GMAC_EV_RETRYLIMITEXCEED
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_RLEX_Msk /*DOM-IGNORE-END*/,
	
	// Transmit Frame Corruption
	GMAC_EV_TXFRAMECORRUPT
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_TFC_Msk /*DOM-IGNORE-END*/,
	
	// Transmit Complete
	GMAC_EV_TXCOMPLETE
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_TCOMP_Msk /*DOM-IGNORE-END*/,

	// Receive Overrun
	GMAC_EV_RXOVERRUN
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_ROVR_Msk /*DOM-IGNORE-END*/,

	// HRESP Not OK
	GMAC_EV_HRESPNOTOK
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_HRESP_Msk /*DOM-IGNORE-END*/,

	// Pause Frame with Non-zero Pause Quantum Received
	GMAC_EV_PAUSEFRAMENZRECV
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_PFNZ_Msk /*DOM-IGNORE-END*/,

	// Pause Time Zero
	GMAC_EV_PAUSETIMEZERO
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_PTZ_Msk /*DOM-IGNORE-END*/,

	// Pause Frame Transmitted
	GMAC_EV_PAUSEFRAMETX
	/*DOM-IGNORE-BEGIN*/ = GMAC_ISR_PFTR_Msk /*DOM-IGNORE-END*/,
	// All events
	GMAC_EV_ALL
	/*DOM-IGNORE-BEGIN*/= (	GMAC_EV_MGMTFRAMESENT | GMAC_EV_RXCOMPLETE |  GMAC_EV_RXUSEDBITREAD |
	GMAC_EV_TXUNDERRUN | GMAC_EV_RETRYLIMITEXCEED |	GMAC_EV_TXFRAMECORRUPT |
	GMAC_EV_TXCOMPLETE | GMAC_EV_RXOVERRUN | GMAC_EV_HRESPNOTOK |
	GMAC_EV_PAUSEFRAMENZRECV | GMAC_EV_PAUSETIMEZERO | GMAC_EV_PAUSEFRAMETX ) /*DOM-IGNORE-END*/
            
} GMAC_EVENTS;
/* TCPIP Stack Event Descriptor

  Summary:

  Description:

  Remarks:
    None
*/
typedef struct
{
    TCPIP_MAC_EVENT             _TcpEnabledEvents;          // group enabled notification events
    volatile TCPIP_MAC_EVENT    _TcpPendingEvents;          // group notification events that are set, waiting to be re-acknowledged
    GMAC_EVENTS             _EthEnabledEvents;          // copy in GMAC_EVENTS space
    volatile GMAC_EVENTS    _EthPendingEvents;          // copy in GMAC_EVENTS space
    TCPIP_MAC_EventF            _TcpNotifyFnc;              // group notification handler
    const void*                 _TcpNotifyParam;            // notification parameter
}DRV_GMAC_EVENT_DCPT;   // event descriptor per group

// synchronization for the RX flow
// The RX packets are allocated by the MAC and
// passed to the stack in manager context but
// acknowledged by the stack (user threads)
// when contents is processed
// Synchronization is needed
// If a semaphore is used then the packet RX flow (manager)
// could be blocked waiting for user threads.
// For better performance this should be avoided.
// When not using a semaphore lock, a critical section
// will be used.
//#define   DRV_GMAC_USE_RX_SEMAPHORE_LOCK
#define   DRV_GMAC_USE_TX_SEMAPHORE_LOCK


#define DRV_GMAC_MIN_RX_SIZE           64     // minimum RX buffer size
                                                // less than this creates excessive fragmentation
                                                // Keep it always multiple of 16!

#define DRV_GMAC_MIN_TX_DESCRIPTORS    1       // minimum number of TX descriptors
                                                 // needed to accomodate zero copy and TCP traffic
                                                 
// *****************************************************************************
/* Ethernet Driver Module Link check states

  Summary:
    List of the states to read the current link status

  Description:
    List of the states to read the current link status

  Remarks:
    None
*/

typedef enum
{
    DRV_GMAC_LINK_CHECK_START_LINK  = 0,
    DRV_GMAC_LINK_CHECK_GET_LINK,
    DRV_GMAC_LINK_CHECK_WAIT_LINK_UP,
    DRV_GMAC_LINK_CHECK_NEG_COMPLETE,
    DRV_GMAC_LINK_CHECK_NEG_RESULT,
}DRV_GMAC_LINK_CHECK_STATE;

/**
 * Configuration Structure for Queues in GMAC.
 */
typedef struct
{	
	/** Pointer to allocated RX buffer */
	TCPIP_MAC_PACKET *pRxPckt[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0]; //normally QUE0 will have maximum RX descriptors
                                                                  // So that many number of Rx packet pointers for all Queues
	/** Pointer to Rx TDs (must be 8-byte aligned) */
	DRV_PIC32CGMAC_HW_RXDCPT *pRxDesc;
    // Rx queues for RX packet buffers
	DRV_PIC32CGMAC_SGL_LIST _RxBuffNewQueue;
    DRV_PIC32CGMAC_SGL_LIST _RxBuffAckQueue;
	
	/** Pointer to Tx TDs (must be 8-byte aligned) */
	DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc;

	DRV_PIC32CGMAC_SGL_LIST _TxNewQueue;		//Pool of New Queue list
	DRV_PIC32CGMAC_SGL_LIST _TxStartQueue;		//Queue list of for transmission
	DRV_PIC32CGMAC_SGL_LIST _TxAckQueue;        // TX acknowledgement queue; stores TX packets that need to be acknowledged

	/** RX index of current processing Descriptor */
	uint16_t nRxDescIndex;

	/** Circular buffer head pointer by upper layer (buffer to be sent) */
	uint16_t nTxDescHead;
	/** Circular buffer tail pointer incremented by handlers (buffer sent) */
	uint16_t nTxDescTail;	

} DRV_GMAC_QUEUE;
// *****************************************************************************
/* GMAC driver data.

  Summary: Data for GMAC driver structure.

  Description: All the data related to GMAC driver
*/
typedef struct {	
	
	unsigned int        _macIx;             // index of the MAC, for multiple MAC's support
	unsigned int        _phyIx;             // index of the associated PHY
	SYS_MODULE_OBJ      hPhySysObject;      // PHY object handle
	SYS_MODULE_OBJ      hPhyClient;         // PHY handle
	SYS_STATUS          sysStat;            // driver status
	union
	{
		uint16_t        val;
		struct
		{
			uint16_t    _init               : 1;    // the corresponding MAC is initialized
			uint16_t    _open               : 1;    // the corresponding MAC is opened
			uint16_t    _linkPresent        : 1;    // lif connection to the PHY properly detected : on/off
			uint16_t    _linkNegotiation    : 1;    // if an auto-negotiation is in effect : on/off
			uint16_t	_linkPrev           : 1;    // last value of the link status: on/off
			uint16_t	_linkUpDone       : 1;      // the link up sequence done
			// add another flags here
		};
	}_macFlags;										// corresponding MAC flags
		
	// general stuff
	const void*                     _AllocH ;        // allocation handle
	TCPIP_MAC_HEAP_CallocF          _callocF;        // allocation functions
	TCPIP_MAC_HEAP_FreeF            _freeF;
    
	// packet allocation functions
	TCPIP_MAC_PKT_AllocF            pktAllocF;
	TCPIP_MAC_PKT_FreeF             pktFreeF;
	TCPIP_MAC_PKT_AckF              pktAckF;
	
	// synchronization
	TCPIP_MAC_SynchReqF             _synchF;
	// timing and link status maintenance
	TCPIP_ETH_OPEN_FLAGS            _linkResFlags;        // resulted link flags
	uint32_t                        _linkUpTick;				// tick value when the link up sequence was started
	uint32_t                        _linkWaitTick;				// tick value to wait for
	DRV_ETHPHY_NEGOTIATION_RESULT   _negResult;		// negotiation result storage
	DRV_GMAC_LINK_CHECK_STATE       _linkCheckState;    // link state machine current status
	INT_SOURCE                      _macIntSrc;							// this MAC interrupt source
	
	DRV_GMAC_EVENT_DCPT             _pic32c_ev_group_dcpt;
	uintptr_t                       _syncRxH;          // synch object handle for RX operations
	uintptr_t                       _syncTxH;          // synch object handle for TX operations
	
	// debug: run time statistics
	TCPIP_MAC_RX_STATISTICS         _rxStat;
	TCPIP_MAC_TX_STATISTICS         _txStat;
	
    // GMAC queues
	DRV_GMAC_QUEUE                      gmac_queue[DRV_GMAC_NUMBER_OF_QUEUES];	
	TCPIP_MODULE_MAC_PIC32C_CONFIG    gmacConfig;  // configuration parameters
	
} DRV_GMAC_INSTANCE;


// *****************************************************************************
/* GMAC driver structure.

  Summary: Driver Apis and Data for GMAC driver.

  Description:

*/
typedef struct
{
	const TCPIP_MAC_OBJECT* pObj;		// safe cast to TCPIP_MAC_DCPT
	DRV_GMAC_INSTANCE   sGmacData;		// specific PIC32C MAC data
} DRV_GMAC_DRIVER;

/* Function prototype for accessing Statistics registers */
typedef uint32_t (*DRV_PIC32CGMAC_HW_REG_FUNC)(void);

// *****************************************************************************
/* PIC32 MAC Hardware statistics register access structure

  Summary:

  Description:
    Abstraction of the HW statistics registers

  Remarks:
*/

typedef struct
{
    char                    regName[36];     // hardware name
    DRV_PIC32CGMAC_HW_REG_FUNC  regFunc;    // register access function
} DRV_PIC32CGMAC_HW_REG_DCPT;

// *****************************************************************************
/* Strutcure for PIC32C GMAC Hash Calculation

  Summary:

  Description:
    Structure supports hash value calculation and setting in PIC32C GMAC register

  Remarks:
*/
typedef struct
{
    uint64_t hash_value;            // 64- bit Hash value to Set
    const TCPIP_MAC_ADDR* DestMACAddr;    // MAC address pointer for Hash value calculation
    uint8_t calculate_hash;        // Selects hash calculation or direct hash value
                                    // Clear to 0 for direct loading of hash value to register
                                    // Set to 1for Calculating the Hash from Mac address and load

}DRV_GMAC_HASH; 

typedef union
  {
      uint8_t index;
      struct __attribute__((packed))
      {
          uint8_t b0:1;
          uint8_t b1:1;
          uint8_t b2:1;
          uint8_t b3:1;
          uint8_t b4:1;
          uint8_t b5:1;
          uint8_t b6:1;
          uint8_t b7:1;
      } bits;
  }DRV_GMAC_HASH_INDEX;  

  typedef union
  {
      uint8_t addr_val;
      struct __attribute__((packed))
      {
          uint8_t b0:1;
          uint8_t b1:1;
          uint8_t b2:1;
          uint8_t b3:1;
          uint8_t b4:1;
          uint8_t b5:1;
          uint8_t b6:1;
          uint8_t b7:1;
      } bits;
  }DRV_GMAC_MAC_ADDR;
#endif //#ifndef _DRV_GMAC_LOCAL_H

/*******************************************************************************
 End of File
*/

