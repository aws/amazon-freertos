/*******************************************************************************
  Ethernet Driver Library Definition

  Company:
    Microchip Technology Inc.
    
  FIle Name:
    drv_gmac_lib.h

  Summary:
    This file contains the Application Program Interface (API) definition  for
    the Ethernet Driver library.

  Description:
    This library provides a low-level abstraction of the Ethernet module
    on Microchip PIC32C family microcontrollers with a convenient C language
    interface.  It can be used to simplify low-level access to the module
    without the necessity of interacting directly with the module's registers,
    thus hiding differences from one microcontroller variant to another.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2017-2018 Microchip Technology Inc. and its subsidiaries.

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


#ifndef _DRV_PIC32CGMAC_LIB_H_
#define _DRV_PIC32CGMAC_LIB_H_

#include <stdlib.h>
#include "system_config.h"
#include "system/debug/sys_debug.h"
#include "system/command/sys_command.h"
#include "system/time/sys_time.h"
#include "system/sys_time_h2_adapter.h"
#include "tcpip/tcpip_ethernet.h"
#include "driver/gmac/src/dynamic/_gmac_dcpt_lists.h"
#include "driver/gmac/src/drv_gmac_local.h"
#include "device.h"
/******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Ethernet Descriptor Types

  Summary:
    Enumeration defining the Ethernet descriptor types.

  Description:
    This enumeration defines the Ethernet descriptor types.

  Remarks:
    Most descriptor operations support just one type, but multiple flags can be
    set.
*/

typedef enum
{
    // TX descriptor
    DRV_GMAC_DCPT_TYPE_RX    /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // RX descriptor
    DRV_GMAC_DCPT_TYPE_TX    /*DOM-IGNORE-BEGIN*/ = 0x2 /*DOM-IGNORE-END*/,

    // All (both) types.  Some descriptor operations support multiple types.
    DRV_GMAC_DCPT_TYPE_ALL   /*DOM-IGNORE-BEGIN*/ = (DRV_GMAC_DCPT_TYPE_RX|DRV_GMAC_DCPT_TYPE_TX) /*DOM-IGNORE-END*/

} DRV_GMAC_DCPT_TYPE;      // descriptor types
/*------------------------------------------------------------------------------
                            Definitions
------------------------------------------------------------------------------
*/
/// The buffer addresses written into the descriptors must be aligned so the
/// last few bits are zero.  These bits have special meaning for the GMAC
/// peripheral and cannot be used as part of the address.
#define GMAC_ADDRESS_MASK   ((unsigned int)0xFFFFFFFC)
#define GMAC_LENGTH_FRAME   ((unsigned int)0x3FFF)    /// Length of frame mask

// receive buffer descriptor bits
#define GMAC_RX_OWNERSHIP_BIT   (1u <<  0)
#define GMAC_RX_WRAP_BIT        (1u <<  1)
#define GMAC_RX_SOF_BIT         (1u << 14)
#define GMAC_RX_EOF_BIT         (1u << 15)

// Transmit buffer descriptor bits
#define GMAC_TX_LAST_BUFFER_BIT (1u << 15)
#define GMAC_TX_WRAP_BIT        (1u << 30)
#define GMAC_TX_USED_BIT        (1u << 31)
#define GMAC_TX_RLE_BIT         (1u << 29) /// Retry Limit Exceeded
#define GMAC_TX_UND_BIT         (1u << 28) /// Tx Buffer Under-run
#define GMAC_TX_ERR_BIT         (1u << 27) /// Exhausted in mid-frame
#define GMAC_TX_ERR_BITS  \
                           (GMAC_TX_RLE_BIT | GMAC_TX_UND_BIT | GMAC_TX_ERR_BIT)

// Interrupt bits
// All interrupts
#define GMAC_INT_ALL 0xFFFFFFFF
// RX Interrupts
#define GMAC_INT_RX_BITS  \
	(GMAC_IER_RCOMP_Msk  | GMAC_IER_RXUBR_Msk  | GMAC_IER_ROVR_Msk )
// TX err interrupts
#define GMAC_INT_TX_ERR_BITS  \
	(GMAC_IER_TUR_Msk  | GMAC_IER_RLEX_Msk  | GMAC_IER_TFC_Msk  | GMAC_IER_HRESP_Msk )
// TX interrupts
#define GMAC_INT_TX_BITS  \
	(GMAC_INT_TX_ERR_BITS | GMAC_IER_TCOMP_Msk )
// Interrupt Status bits
#define GMAC_INT_RX_STATUS_BITS  \
	(GMAC_ISR_RCOMP_Msk  | GMAC_ISR_RXUBR_Msk  | GMAC_ISR_ROVR_Msk )
#define GMAC_INT_TX_STATUS_ERR_BITS  \
	(GMAC_ISR_TUR_Msk  | GMAC_ISR_RLEX_Msk  | GMAC_ISR_TFC_Msk  | GMAC_ISR_HRESP_Msk )

// *****************************************************************************
/* Ethernet Receive Filter Flags

  Summary:
    Defines the receive filter flags

  Description:
    This enumeration defines the receive filter flags.

  Remarks:
    Multiple values can be OR-ed together.

    The values in this enumeration are displayed in the order of priority
    that the receive filter state machine works, with the highest priority first.
    Once a filter accepts or rejects a packet, further filters are not tried.
    If a packet isn't rejected/accepted after all filters are tried, it will be rejected by
    default!
*/
typedef enum{
	// Frames with wrong Preamble are accepted
	GMAC_FILT_PREAMBLE_ERR_ACCEPT
	/*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_RXBP_Msk /*DOM-IGNORE-END*/,

	// Frames with wrong CRC are accepted
	GMAC_FILT_CRC_ERR_ACCEPT
	/*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_IRXFCS_Msk /*DOM-IGNORE-END*/,

	// Frames with Maximum frame size accepted
	GMAC_FILT_MAXFRAME_ACCEPT
	/*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_MAXFS_Msk /*DOM-IGNORE-END*/,

	// Frames with Unicast Hash match accepted
	GMAC_FILT_UCASTHASH_ACCEPT
	/*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_UNIHEN_Msk /*DOM-IGNORE-END*/,
	
	// Frames with Multicast Hash match accepted
	GMAC_FILT_MCASTHASH_ACCEPT
	/*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_MTIHEN_Msk /*DOM-IGNORE-END*/,
	
	// All Broadcast rejected
	GMAC_FILT_BCAST_REJECT
	/*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_NBC_Msk /*DOM-IGNORE-END*/,
	
	// All valid frames accepted
	GMAC_FILT_ALLFRAME_ACCEPT
	/*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_CAF_Msk /*DOM-IGNORE-END*/,
	
	// Jumbo frames accepted
	GMAC_FILT_JUMBOFRAME_ACCEPT
	/*DOM-IGNORE-BEGIN*/ = GMAC_NCFGR_JFRAME_Msk /*DOM-IGNORE-END*/,	
	
	GMAC_FILT_ALL_FILTERS
	/*DOM-IGNORE-BEGIN*/    =	GMAC_FILT_PREAMBLE_ERR_ACCEPT	|	GMAC_FILT_CRC_ERR_ACCEPT	|
								GMAC_FILT_MAXFRAME_ACCEPT		|	GMAC_FILT_UCASTHASH_ACCEPT	|
								GMAC_FILT_MCASTHASH_ACCEPT		|	GMAC_FILT_BCAST_REJECT		|
								GMAC_FILT_ALLFRAME_ACCEPT		|	GMAC_FILT_ALLFRAME_ACCEPT	|
								GMAC_FILT_JUMBOFRAME_ACCEPT	/*DOM-IGNORE-END*/
} GMAC_RX_FILTERS;

/**************************************************************************
  Summary:
    Defines the possible results of Ethernet operations that can succeed or
    fail
  Description:
    Ethernet Operation Result Codes
    
    This enumeration defines the possible results of any of the Ethernet
    library operations that have the possibility of failing. This result
    should be checked to ensure that the operation achieved the desired
    result.                                                                
**************************************************************************/

typedef enum
{
    /* Everything ok */
    DRV_PIC32CGMAC_RES_OK       /*DOM-IGNORE-BEGIN*/ =  0 /*DOM-IGNORE-END*/,

    /* Ethernet RX, TX, acknowledge packet codes */
    /* No such packet exist */
    DRV_PIC32CGMAC_RES_NO_PACKET,

    /* Packet is queued (not transmitted or received and not processed) */
    DRV_PIC32CGMAC_RES_PACKET_QUEUED,

    /* Not enough descriptors available */
    DRV_PIC32CGMAC_RES_NO_DESCRIPTORS,
            
    /* Not enough nodes in Tx Queues */
    DRV_PIC32CGMAC_RES_NO_TX_QUEUE,   
            
    /* Not enough nodes in Rx Queues */
    DRV_PIC32CGMAC_RES_NO_RX_QUEUE,
            
    /* Errors: Ethernet buffers, descriptors */
	DRV_PIC32CGMAC_RES_DESC_CNT_ERR,
    /* Some memory allocation failed */
    DRV_PIC32CGMAC_RES_OUT_OF_MEMORY        /*DOM-IGNORE-BEGIN*/ =  -1 /*DOM-IGNORE-END*/,

    /* We don't support user space buffers. */
    DRV_PIC32CGMAC_RES_USPACE_ERR           /*DOM-IGNORE-BEGIN*/ =  -2 /*DOM-IGNORE-END*/,

    /* The size of the receive buffers too small */
    DRV_PIC32CGMAC_RES_RX_SIZE_ERR          /*DOM-IGNORE-BEGIN*/ =  -3 /*DOM-IGNORE-END*/,

    /* A received packet spans more buffers/descriptors than supplied */
    DRV_PIC32CGMAC_RES_RX_PKT_SPLIT_ERR     /*DOM-IGNORE-BEGIN*/ =  -4 /*DOM-IGNORE-END*/,
	
	DRV_PIC32CGMAC_RES_EMPTY_BUFFER			/*DOM-IGNORE-BEGIN*/  =  -5 /*DOM-IGNORE-END*/,
    /* Descriptor Queue Error*/        
    DRV_PIC32CGMAC_RES_QUEUE_ERR			/*DOM-IGNORE-BEGIN*/  =  -6 /*DOM-IGNORE-END*/,

} DRV_PIC32CGMAC_RESULT; 


/**************************************************************************
  Summary:
    Defines the different states of RX receive state machine
  Description:
    GMAC RX engine copies frame packet to single/multiple RX buffers and update the 
    status in associated RX descriptors. The RX state machine reads the different
    RX descriptors to detect a valid RX packet
**************************************************************************/
typedef enum  
{
	GMAC_RX_NO_FRAME_STATE,
	GMAC_RX_SOF_DETECTED_STATE,
	GMAC_RX_VALID_FRAME_DETECTED_STATE
} GMAC_RXFRAME_STATE;

/**************************************************************************
  Summary:
     GMAC Rx descriptor data packet attributes
  Description:
	 These attributes used during the search for valid Rx packet,on GMAC Rx Interrupt.
**************************************************************************/
typedef struct
{
	uint16_t startIndex;
	uint16_t endIndex;
	uint16_t buffer_count;	
} DRV_PIC32CGMAC_RX_FRAME_INFO; 

// *****************************************************************************
/* Ethernet Close Flags

  Summary:
    Defines the possible disable codes of Ethernet controller "DRV_PIC32CGMAC_LibClose" call.

  Description:
    This enumeration defines the close capabilities of the Ethernet module.
*/

typedef enum
{
    /* Wait for the current TX/RX operation to finish */
    DRV_PIC32CGMAC_CLOSE_GRACEFUL  /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // Default close options
    DRV_PIC32CGMAC_CLOSE_DEFAULT /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,

} DRV_PIC32CGMAC_CLOSE_FLAGS; 

// *****************************************************************************
// *****************************************************************************
// Section: Ethernet Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

// Section:  Open and Configuration Functions

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibInit (DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    Ethernet controller initialization routine
  Description:
    This function performs the initialization of the Ethernet Controller(GMAC).

  Precondition:
    None.

  Parameters:
    pMACDrv -  GMAC device driver structure.

  Returns:
    None
  Example:
    <code>
    DRV_PIC32CGMAC_LibInit(DRV_GMAC_DRIVER* pMACDrv);
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/

void DRV_PIC32CGMAC_LibInit (DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx)

  Summary:
    GMAC transfer initialization of GMAC queue
  Description:
    This function performs the initialization of DMA and interrupts of GMAC queue.

  Precondition:
    None.

  Parameters:
    pMACDrv -  GMAC device driver structure.
    queueIdx - Queue Index

  Returns:
    DRV_PIC32CGMAC_RES_OK               - success
    DRV_PIC32CGMAC_RES_DESC_CNT_ERR     - descriptor count error
    DRV_PIC32CGMAC_RES_RX_SIZE_ERR      - rx buffer size error
    DRV_PIC32CGMAC_RES_QUEUE_ERR        - Queue Index error
  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Handle error
    }
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx);

/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxInit(DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    GMAC receive side initialization
  Description:
    This function performs the initialization of Rx descriptors and Rx Queue buffer
    pointer for all the GMAC queues

  Precondition:
    DRV_PIC32CGMAC_LibInit must have been called.
    DRV_PIC32CGMAC_LibInitTransfer must have been called.

  Parameters:
    pMACDrv -  GMAC device driver structure.
  
  Returns:
    DRV_PIC32CGMAC_RES_OK               - success
    DRV_PIC32CGMAC_RES_OUT_OF_MEMORY    - out of memory error
 
  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibRxInit(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Handle error
    }
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxInit(DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxInit(DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    GMAC transmit side initialization
  Description:
    This function performs the initialization of TX descriptors and TX Queue buffer
    pointer for all the GMAC queues

  Precondition:
    DRV_PIC32CGMAC_LibInit must have been called.
    DRV_PIC32CGMAC_LibInitTransfer must have been called.

  Parameters:
    pMACDrv -  GMAC device driver structure.
  
  Returns:
    DRV_PIC32CGMAC_RES_OK               - success
  
  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibTxInit(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Handle error
    }
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxInit(DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
      void DRV_PIC32CGMAC_LibTransferEnable (DRV_GMAC_DRIVER* pMACDrv)

  Summary:
    GMAC Transmit and Receive Enable
  Description:

  Precondition:
    DRV_PIC32CGMAC_LibInit must have been called.
    DRV_PIC32CGMAC_LibInitTransfer must have been called.
    DRV_PIC32CGMAC_LibTxInit must have been called.
    DRV_PIC32CGMAC_LibRxInit must have been called.
  Parameters:
    pMACDrv -  GMAC device driver structure.

  Returns:
    None
  Example:
    <code>
    DRV_PIC32CGMAC_LibTransferEnable(DRV_GMAC_DRIVER* pMACDrv);
    </code>
  Remarks:
    This function should be called before the DRV_PIC32CGMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/
void DRV_PIC32CGMAC_LibTransferEnable (DRV_GMAC_DRIVER* pMACDrv);

/*******************************************************************************
  Function:
    void DRV_PIC32CGMAC_LibMACOpen(DRV_GMAC_DRIVER * pMACDrv, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType )

  Summary:
    Initializes and configures the Ethernet MAC.

  Description:
    This function performs the open and configuration of the Ethernet MAC.  It needs
    to be called after the DRV_PIC32CGMAC_LibInit(DRV_GMAC_DRIVER * pMACDrv) was called and the PHY initialized.


  Precondition:
    DRV_PIC32CGMAC_LibInit must have been called.
    DRV_PIC32CGMAC_LibInitTransfer must have been called.
    DRV_PIC32CGMAC_LibTxInit must have been called.
    DRV_PIC32CGMAC_LibRxInit must have been called.
    DRV_PIC32CGMAC_LibTransferEnable must have been called.

  Parameters:
    pMACDrv       - driver instance.
    oFlags      - Open flags
    pauseType   - Pause type to be supported by the MAC


  Returns:
    Ethernet operation result code.

  Example:
    <code>
    // Open and configure the MAC.
    //
    // Since we're not connected to an external PHY there's no negotiation
    // going on and we know all our opening flags
    DRV_PIC32CGMAC_LibMACOpen(DRV_GMAC_DRIVER * pMACDrv, oFlags, pauseType);
    </code>

  Remarks:
    If no auto-negotiation is performed, the DRV_PIC32CGMAC_LibMACOpen() could be called
    immediately after DRV_PIC32CGMAC_LibInit(DRV_GMAC_INSTANCE_DCPT* pMacD) and PHY initialization.  Otherwise the open
    flags and the pause type should be the ones obtained as a result of the
    PHY negotiation procedure!
    <p>Replaces:<p><c><b>void EthMACOpen(TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType)</b></c>
 *****************************************************************************/

void DRV_PIC32CGMAC_LibMACOpen (DRV_GMAC_DRIVER * pMACDrv, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType );


/*******************************************************************************
  Function:
    void DRV_PIC32CGMAC_LibClose(DRV_GMAC_DRIVER * pMACDrv, DRV_PIC32CGMAC_CLOSE_FLAGS cFlags )

  Summary:
    Aborts any transactions and disables the Ethernet controller

  Description:
    This function performs an abort of any current Ethernet transactions, disables
    the Ethernet controller and interrupts.  It should be called after the PHY
    close/reset procedure.

  Precondition:
    None.

  Parameters:
    pMACDrv       - driver instance.
    cFlags - closing flags

  Returns:
    None.

  Example:
    <code>
    DRV_PIC32CGMAC_LibClose(DRV_GMAC_DRIVER * pMACDrv, DRV_PIC32CGMAC_CLOSE_DEFAULT);
    </code>

  Remarks:
    It does not free any allocated memory.
    <p>Replaces:<p><c><b>void EthClose(DRV_PIC32CGMAC_CLOSE_FLAGS cFlags)</b></c>
 *****************************************************************************/

void DRV_PIC32CGMAC_LibClose (DRV_GMAC_DRIVER * pMACDrv, DRV_PIC32CGMAC_CLOSE_FLAGS cFlags );

/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxFilterHash_Calculate(DRV_GMAC_DRIVER* pMACDrv, DRV_GMAC_HASH *hash) 

  Summary:
    Calculates the hash for given MAC address and sets the hash in PIC32C GMAC register

  Description:

  Precondition:
    None.

  Parameters:
    pMACDrv  - driver instance.
    hash  - hash structure

  Returns:
    None.

  Example:
    <code>
    DRV_PIC32CGMAC_LibRxFilterHash_Calculate(pMACDrv, &hash);
    </code>

  Remarks:
    
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxFilterHash_Calculate(DRV_GMAC_DRIVER* pMACDrv, DRV_GMAC_HASH *hash);
/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersAppend(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx, uint16_t start_index, uint16_t nDscCnt)

  Summary:
    Supplies buffers to the receiving process and enables the receiving part of
    the controller.

  Description:
    This function supplies buffers to the receiving process and enables the
    receiving part of the controller.  As soon as the controller starts
    receiving data packets these will be stored into memory at the addresses
    specified by these buffers.  A received packet can consist of multiple
    buffers, split across buffers with the SAME size.  Each buffer needs an associated receive
    descriptor. Therefore, the number of needed receive descriptors should be
    available for this function to succeed.  Once a receive operation is
    scheduled, DRV_PIC32CGMAC_LibRxGetPacket(DRV_GMAC_DRIVER* pMACDrv, ) can be called to get the received packet.

  Precondition:
    DRV_PIC32CGMAC_LibInit and other GMAC initialization routines should have been called.
  
  Parameters:
    pMACDrv       - driver instance.
    queueIdx      - Queue index

    start_index      - Descriptor Start index

    nDscCnt     - number of descriptors

  Returns:
    DRV_PIC32CGMAC_RES_OK               - success

    DRV_PIC32CGMAC_RES_OUT_OF_MEMORY    - no enough memory

  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx,cstart_index,cnDscCnt);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Handle error
    }
    </code>

  Remarks:
    Not multithreaded safe. Don't call from from both ISR -non ISR code or
    multiple ISR's!

    This function enables the Ethernet receiving.

    When a packet is split into multiple buffers, all buffers have the same
    size.

    The append process continues until a NULL buffer pointer is retrieved or
    nBuffs buffers are appended.

    Only RX DRV_PIC32CGMAC_BUFF_FLAGS are relevant for this function.
    <p>Replaces:<p><c><b>DRV_PIC32CGMAC_LibRxBuffersAppend(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx, uint16_t start_index, uint16_t nDscCnt);</b></c>
 *****************************************************************************/

DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersAppend(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx, uint16_t start_index, uint16_t nDscCnt);

// Section:  TX/RX Functions


/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxSendPacket (DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx)

  Summary:
    Schedules the supplied packet for transmission.

  Description:
    This function schedules the supplied packet for transmission.  Each packet
    can consist of multiple buffers, each having a different size, as specified
    in the packet descriptor.  Each buffer will need an associated transmit
    descriptor. Therefore, the number of needed transmit descriptors should be
    available for this function to succeed.  Once a transmission is scheduled
    and done, DRV_PIC32CGMAC_LibTxGetPacketStatus can be called to check the transmission
    result.

  Precondition:
    DRV_PIC32CGMAC_LibMACOpen, DRV_PIC32CGMAC_LibDescriptorsPoolAdd should have been called.
    Valid packet descriptor pointer required.
    Buffers specified should be >0 and < 2048 bytes in size.

  Parameters:
    pMACDrv       - driver instance.
    queueIdx        - Queue Index.

  Returns:
    DRV_PIC32CGMAC_RES_OK              - success

    DRV_PIC32CGMAC_RES_NO_DESCRIPTORS  - no descriptors available

    DRV_PIC32CGMAC_RES_NO_PACKET      -  no packet for transmission

  Example:
    <code>
    ethRes = DRV_PIC32CGMAC_LibTxSendPacket (pMACDrv,queueIdx);
    if ( ethRes != DRV_PIC32CGMAC_RES_OK )
    {
        // Transmission failed
    }
    </code>

  Remarks:
    Not multithreaded safe. Don't call from from both ISR and non-ISR code or
    multiple ISR's!

    This function enables the Ethernet transmission.

    The packet construction from buffers stops when a descriptor with NULL buffer
    or 0 size is retrieved or the list ends.
    <p>Replaces:<p><c><b>DRV_PIC32CGMAC_LibTxSendPacket (DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx)</b></c>
 *****************************************************************************/

DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxSendPacket (DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx);

// *****************************************************************************
// *****************************************************************************
// Section: Descriptors add and remove functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void DRV_GMAC_LibDescriptorsPoolAdd (DRV_GMAC_DRIVER * pMACDrv, DRV_GMAC_DCPT_TYPE dType);

  Summary:
    Adds transmit & receive descriptors.

  Description:
    This function adds the specified number of transmit/receive descriptors to
    the pool of descriptors.  These descriptors are used whenever a new
    transmission or a reception is initiated.  

  Precondition:
    Allocate  RX and TX descriptors in memory.

  Parameters:
    pMACDrv           - driver instance.
  
    dType           - TX/RX descriptors requested


  Returns:
    None

  Example:
    <code>
    DRV_GMAC_LibDescriptorsPoolAdd (pMACDrv, DRV_GMAC_DCPT_TYPE_RX);
    </code>

  Remarks:
    DRV_GMAC_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has
    to be used.   
 *****************************************************************************/
void DRV_GMAC_LibDescriptorsPoolAdd (DRV_GMAC_DRIVER * pMACDrv, DRV_GMAC_DCPT_TYPE dType);

// *****************************************************************************
// *****************************************************************************
// Section: packet acknowledge functions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Ethernet Buffer Acknowledge Callback Function

  Function:
    void <Function Name> ( void *pPktBuff, int buffIx, void* param )

  Summary:
    Pointer to an function called to acknowledge a buffer/packet.

  Description:
    Pointer to a function called to acknowledge the completion of a buffer/packet.

  Precondition:
    A TX or RX packet must have been queued.

  Parameters:
    pPktBuff    - Pointer to the currently acknowledged transmitted or received
                  buffer.

    buffIx      - The 0-based buffer index for a packet that spans multiple
                  buffers.

    param       - extra parameter that will be used by the function call


  Returns:
    None.

  Remarks:
    This function is meant to be used for both RX and TX packets acknowledge.
    It allows the caller to pass an extra parameter that will be used in the
    function call.
*/

typedef void ( *DRV_PIC32CGMAC_BUFF_AckF ) ( void *pPktBuff, int buffIx, void* param );

/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxAckPacket(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx);
 
  Summary:
    Acknowledges a transmitted packet

  Description:
    This function acknowledges a transmitted packet.  The transmission
    of this packet has to have been completed otherwise the call will fail.
    The TX acknowledgment function will be called from this routine and free the
    used TX descriptors

  Precondition:
   DRV_PIC32CGMAC_LibTxSendPacket should be called before

  Parameters:
    pMACDrv       - driver instance.
    queueIdx      - queue Index


  Returns:
    DRV_PIC32CGMAC_RES_OK              - Success

    DRV_PIC32CGMAC_RES_DESC_CNT_ERR   - descriptor error

  Example:
    <code>
    ethRes= DRV_PIC32CGMAC_LibTxAckPacket(pMACDrv,queueIdx);


    </code>

  Remarks:
    Any transmitted packet has to be acknowledged, otherwise the Ethernet library will
    run out of transmission descriptors.

    <p>Replaces:<p><c><b>DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxAckPacket(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx)</b></c>
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxAckPacket(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx);

// *****************************************************************************
// *****************************************************************************
// Section: Packet receive functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket (DRV_GMAC_DRIVER * pMACDrv,  TCPIP_MAC_PACKET **pPkt, int *pnBuffs, 
 *                          DRV_GMAC_RXDCPT_STATUS* pRxStat,GMAC_QUE_LIST queueIdx )

  Summary:
    Provides the layout and the status of the next available received packet.

  Description:
    This function returns the layout and the status of the next available
    received packet.  If there's no available fully received packet the call
    will fail (DRV_PIC32CGMAC_RES_PACKET_QUEUED/DRV_PIC32CGMAC_RES_NO_PACKET).  The supplied pPkt has
    to be large enough to accommodate the packet layout in the case of packets
    split across multiple buffers/descriptors (i.e. when the size of the buffer,
    DRV_PIC32CGMAC_LibRxSetBufferSize(DRV_GMAC_INSTANCE_DCPT* pMacD, ) is less than the packet size) otherwise
    DRV_PIC32CGMAC_RES_RX_PKT_SPLIT_ERR will be returned.  However, if the pPkt is NULL,
    then the caller can use the returned pRxStat to check the total size of the
    packet. Then using the size of the set buffers, the number of buffers in
    pPkt will result.  pnBuffs can be used for the same purpose. If pnBuffs is
    supplied, the numbers of buffers in the packet will be returned.

  Precondition:
    DRV_PIC32CGMAC_LibRxSetBufferSize, DRV_PIC32CGMAC_LibRxBuffersAppend should have been called.

  Parameters:
    pMACDrv       - driver instance.
    pPkt            - Pointer to a packet descriptor: a list of buffer
                      descriptors large enough to receive the packet layout or
                      NULL.

    pnBuffs         - Pointer to store the number of buffers in the packet or
                      NULL.

    pRxStat         - Address to store the pointer to the packet status or
                      NULL.
    queueIdx        - Queue index

  Returns:
    DRV_PIC32CGMAC_RES_OK                  - For success.

    DRV_PIC32CGMAC_RES_NO_PACKET           - No such packet in the receiving queue/
                                  No packets available in the receiving queue.

  Example:
    <code>
    // Wait to receive a packet
    ethRes = DRV_PIC32CGMAC_LibRxGetPacket (pMACDrv, &pRxPkt, &buffsPerRxPkt, &pRxPktStat, GMAC_QUE_0);    
    </code>

  Remarks:
    <p>Replaces:<p><c><b>DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket (DRV_GMAC_DRIVER * pMACDrv,  TCPIP_MAC_PACKET **pPkt, int *pnBuffs, DRV_GMAC_RXDCPT_STATUS* pRxStat,GMAC_QUE_LIST queueIdx )</b></c>
 *****************************************************************************/

DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket (DRV_GMAC_DRIVER * pMACDrv,  TCPIP_MAC_PACKET **pPkt, int *pnBuffs, DRV_GMAC_RXDCPT_STATUS* pRxStat,GMAC_QUE_LIST queueIdx );


/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibSetMacAddr (const uint8_t * pMacAddr)

  Summary:
    Set MAC address for Ethernet controller
  Description:
    This function performs setting of MAC address of the Ethernet Controller(GMAC).

  Precondition:
    None.

  Parameters:
    pMacAddr -  address of MAC Address array.

  Returns:
    DRV_PIC32CGMAC_RESULT

  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibSetMacAddr (const uint8_t * pMacAddr);

/*******************************************************************************
  Function:
      DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibGetMacAddr (uint8_t * pMacAddr)

  Summary:
    Get MAC address for Ethernet controller
  Description:
    This function performs reading of MAC address from the Ethernet Controller(GMAC).

  Precondition:
    None.

  Parameters:
    pMacAddr -  address of MAC Address array.

  Returns:
    DRV_PIC32CGMAC_RESULT

  ************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibGetMacAddr (uint8_t * pMacAddr);

/*******************************************************************************/
//Clear the Tx node in the single linked list
static inline void _Clear_TxNode(DRV_PIC32CGMAC_SGL_LIST_NODE*   pTxNode)
{
	pTxNode->next = NULL;
	pTxNode->data = NULL;
	pTxNode->startIndex = pTxNode->endIndex = 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: Mutex Locks
// *****************************************************************************
// *****************************************************************************
// RX lock functions
#if defined(DRV_GMAC_USE_RX_SEMAPHORE_LOCK)
static __inline__ bool __attribute__((always_inline)) _DRV_GMAC_RxCreate(DRV_GMAC_DRIVER * pMACDrv)
{
	return (pMACDrv->sGmacData._synchF == 0) ? true : (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE);
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxDelete(DRV_GMAC_DRIVER * pMACDrv)
{
	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE);
	}
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxLock(DRV_GMAC_DRIVER * pMACDrv)
{
	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK);
	}
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxUnlock(DRV_GMAC_DRIVER * pMACDrv)
{
	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK);
	}
}

#else
// use critical sections
static __inline__ bool __attribute__((always_inline)) _DRV_GMAC_RxCreate(DRV_GMAC_DRIVER * pMACDrv)
{
	return true;
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxDelete(DRV_GMAC_DRIVER * pMACDrv)
{
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxLock(DRV_GMAC_DRIVER * pMACDrv)
{
	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER);
	}
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_RxUnlock(DRV_GMAC_DRIVER * pMACDrv)
{
	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE);
	}
}
#endif  // defined(DRV_GMAC_USE_RX_SEMAPHORE_LOCK)


// TX lock functions
#if defined(DRV_GMAC_USE_TX_SEMAPHORE_LOCK)
static __inline__ bool __attribute__((always_inline)) _DRV_GMAC_TxCreate(DRV_GMAC_DRIVER * pMACDrv)
{
	return (pMACDrv->sGmacData._synchF == 0) ? true : (*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE);
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxDelete(DRV_GMAC_DRIVER * pMACDrv)
{

	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE);
	}
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxLock(DRV_GMAC_DRIVER * pMACDrv)
{
	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK);
	}
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxUnlock(DRV_GMAC_DRIVER * pMACDrv)
{

	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK);
	}
}
#else
// use critical sections
static __inline__ bool __attribute__((always_inline)) _DRV_GMAC_TxCreate(DRV_GMAC_DRIVER * pMACDrv)
{
	return true;
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxDelete(DRV_GMAC_DRIVER * pMACDrv)
{
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxLock(DRV_GMAC_DRIVER * pMACDrv)
{
	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER);
	}
}

static __inline__ void __attribute__((always_inline)) _DRV_GMAC_TxUnlock(DRV_GMAC_DRIVER * pMACDrv)
{
	if(pMACDrv->sGmacData._synchF != 0)
	{
		(*pMACDrv->sGmacData._synchF)(&pMACDrv->sGmacData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE);
	}
}
#endif  // defined(DRV_GMAC_USE_TX_SEMAPHORE_LOCK)


/****************************************************************************
 * Function:        _DRV_GMAC_HashValueSet
 * Summary: Set Hash Value in GMAC register
 *****************************************************************************/
static __inline__ void __attribute__((always_inline)) _DRV_GMAC_HashValueSet(uint64_t hash_value)
{
    GMAC_REGS->GMAC_HRB = hash_value & 0xffffffff;
    GMAC_REGS->GMAC_HRT = (hash_value >> 32) & 0xffffffff;    
}

/****************************************************************************
 * Function:        _DRV_GMAC_HashValueGet
 * Summary: Read Hash Value in GMAC register
 *****************************************************************************/
static __inline__ uint64_t __attribute__((always_inline)) _DRV_GMAC_HashValueGet(void)
{
    uint64_t hash_value = 0;    
    hash_value = GMAC_REGS->GMAC_HRT;
    hash_value = (hash_value << 32) | GMAC_REGS->GMAC_HRB;
    return hash_value;
}
// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros
// *****************************************************************************
// *****************************************************************************


/** ISO/IEC 14882:2003(E) - 5.6 Multiplicative operators:
 * The binary / operator yields the quotient, and the binary % operator yields
 * the remainder from the division of the first expression by the second.
 * If the second operand of / or % is zero the behaviour is undefined; otherwise
 *  (a/b)*b + a%b is equal to a.
 * If both operands are non-negative then the remainder is non-negative;
 * if not, the sign of the remainder is implementation-defined 74).
 */
__STATIC_INLINE int fixed_mod(int a, int b)
{
	int rem = a % b;

	while (rem < 0)
		rem += b;

	return rem;
}

/** Return count in buffer */
#define GCIRC_CNT(head,tail,size)  fixed_mod((head) - (tail), (size))

/** Return space available, 0..size-1. always leave one free char as a
    completely full buffer has head == tail, which is the same as empty */
#define GCIRC_SPACE(head,tail,size) GCIRC_CNT((tail),((head)+1),(size))

#define GCIRC_BUFFCNT(head,tail,size) (size - GCIRC_SPACE(head,tail,size))
/** Return count up to the end of the buffer. Carefully avoid accessing head
    and tail more than once, so they can change underneath us without returning
    inconsistent results */
#define GCIRC_CNT_TO_END(head,tail,size) \
	({int end = (size) - (tail); \
		int n = fixed_mod((head) + end, (size));    \
		n < end ? n : end;})

/** Return space available up to the end of the buffer */
#define GCIRC_SPACE_TO_END(head,tail,size) \
	({int end = (size) - 1 - (head); \
		int n = fixed_mod(end + (tail), (size));    \
		n <= end ? n : end+1;})

/** Increment head or tail */
#define GCIRC_INC(headortail,size) \
	headortail++;             \
	if (headortail >= size) {  \
		headortail = 0;       \
	}

/** Decrement head or tail */
#define GCIRC_DEC(headortail,size) \
	headortail = headortail;	\
	if(headortail == 0)	{			\
		headortail = size - 1;	\
	}					\
	else				\
	{					\
		headortail--;	\
	}

/** Circular buffer is empty ? */
#define GCIRC_EMPTY(head, tail)     (head == tail)

/** Clear circular buffer */
#define GCIRC_CLEAR(head, tail)  (head = tail = 0)

#endif  // _DRV_PIC32CGMAC_LIB_H_

