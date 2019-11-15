/*******************************************************************************
  Ethernet Driver Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    drv_eth_pic32_lib.h

  Summary:
    Defines the Ethernet Driver Library Interface.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Ethernet
    Driver Library for Microchip microcontrollers.  The definitions in this 
    file are for the Ethernet module.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2008-2018 Microchip Technology Inc. and its subsidiaries.

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
// DOM-IGNORE-END

#ifndef _DRV_ETH_LIB_H_
#define _DRV_ETH_LIB_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************

// layout of the Ethernet registers        
typedef struct __attribute__ ((packed , aligned(4)))
{
 __ETHCON1bits_t	ETHCON1;
volatile uint32_t        ETHCON1CLR ;
volatile uint32_t        ETHCON1SET ;
volatile uint32_t        ETHCON1INV ;

 __ETHCON2bits_t	ETHCON2;
 volatile uint32_t        ETHCON2CLR ;
 volatile uint32_t        ETHCON2SET ;
 volatile uint32_t        ETHCON2INV ;

 __ETHTXSTbits_t		ETHTXST ;
 volatile uint32_t        ETHTXSTCLR ;
 volatile uint32_t        ETHTXSTSET ;
 volatile uint32_t        ETHTXSTINV ;
 
 __ETHRXSTbits_t		ETHRXST ;
 volatile uint32_t        ETHRXSTCLR ;
 volatile uint32_t        ETHRXSTSET ;
 volatile uint32_t        ETHRXSTINV ;

 __ETHHT0bits_t        ETHHT0 ;
 volatile uint32_t        ETHHT0CLR ;
 volatile uint32_t        ETHHT0SET ;
 volatile uint32_t        ETHHT0INV ;

 __ETHHT1bits_t        ETHHT1 ;
 volatile uint32_t        ETHHT1CLR ;
 volatile uint32_t        ETHHT1SET ;
 volatile uint32_t        ETHHT1INV ;

 __ETHPMM0bits_t 	  ETHPMM0 ;
 volatile uint32_t        ETHPMM0CLR ;
 volatile uint32_t        ETHPMM0SET ;
 volatile uint32_t        ETHPMM0INV ;

 __ETHPMM1bits_t 	  ETHPMM1 ;
 volatile uint32_t        ETHPMM1CLR ;
 volatile uint32_t        ETHPMM1SET ;
 volatile uint32_t        ETHPMM1INV ;

 __ETHPMCSbits_t      ETHPMCS ;
 volatile uint32_t        ETHPMCSCLR ;
 volatile uint32_t        ETHPMCSSET ;
 volatile uint32_t        ETHPMCSINV ;

 __ETHPMObits_t        ETHPMO ;
 volatile uint32_t        ETHPMOCLR ;
 volatile uint32_t        ETHPMOSET ;
 volatile uint32_t        ETHPMOINV ;
 
 __ETHRXFCbits_t	   ETHRXFC ;
 volatile uint32_t        ETHRXFCCLR ;
 volatile uint32_t        ETHRXFCSET ;
 volatile uint32_t        ETHRXFCINV ;

 __ETHRXWMbits_t         ETHRXWM ;
 volatile uint32_t        ETHRXWMCLR ;
 volatile uint32_t        ETHRXWMSET ;
 volatile uint32_t        ETHRXWMINV ;

 __ETHIENbits_t         ETHIEN ;
 volatile uint32_t        ETHIENCLR ;
 volatile uint32_t        ETHIENSET ;
 volatile uint32_t        ETHIENINV ;

 __ETHIRQbits_t         ETHIRQ ;
 volatile uint32_t        ETHIRQCLR ;
 volatile uint32_t        ETHIRQSET ;
 volatile uint32_t        ETHIRQINV ;

 __ETHSTATbits_t         ETHSTAT ;

 volatile uint32_t   	  addroffset[7];
 
 __ETHRXOVFLOWbits_t		ETHRXOVFLOW ;
 volatile uint32_t        ETHRXOVFLOWCLR ;
 volatile uint32_t        ETHRXOVFLOWSET ;
 volatile uint32_t        ETHRXOVFLOWINV ;

 __ETHFRMTXOKbits_t		ETHFRMTXOK ;
 volatile uint32_t        ETHFRMTXOKCLR ;
 volatile uint32_t        ETHFRMTXOKSET ;
 volatile uint32_t        ETHFRMTXOKINV ;

 __ETHSCOLFRMbits_t		ETHSCOLFRM ;
 volatile uint32_t        ETHSCOLFRMCLR ;
 volatile uint32_t        ETHSCOLFRMSET ;
 volatile uint32_t        ETHSCOLFRMINV ;

 __ETHMCOLFRMbits_t		ETHMCOLFRM ;
 volatile uint32_t        ETHMCOLFRMCLR ;
 volatile uint32_t        ETHMCOLFRMSET ;
 volatile uint32_t        ETHMCOLFRMINV ;

 __ETHFRMRXOKbits_t      ETHFRMRXOK ;
 volatile uint32_t        ETHFRMRXOKCLR ;
 volatile uint32_t        ETHFRMRXOKSET ;
 volatile uint32_t        ETHFRMRXOKINV ;

 __ETHFCSERRbits_t       ETHFCSERR ;
 volatile uint32_t        ETHFCSERRCLR ;
 volatile uint32_t        ETHFCSERRSET ;
 volatile uint32_t        ETHFCSERRINV ;

 __ETHALGNERRbits_t      ETHALGNERR ;
  volatile uint32_t        ETHALGNERRCLR ;
 volatile uint32_t        ETHALGNERRSET ;
 volatile uint32_t        ETHALGNERRINV ;

 volatile uint32_t   	  addroffset1[36];
 
 __EMACxCFG1bits_t       EMACxCFG1 ;
 volatile uint32_t        EMACxCFG1CLR ;
 volatile uint32_t        EMACxCFG1SET ;
 volatile uint32_t        EMACxCFG1INV ;

 __EMACxCFG2bits_t       EMACxCFG2 ;
 volatile uint32_t        EMACxCFG2CLR ;
 volatile uint32_t        EMACxCFG2SET ;
 volatile uint32_t        EMACxCFG2INV ;

 __EMACxIPGTbits_t       EMACxIPGT ;
 volatile uint32_t        EMACxIPGTCLR ;
 volatile uint32_t        EMACxIPGTSET ;
 volatile uint32_t        EMACxIPGTINV ;

 __EMACxIPGRbits_t       EMACxIPGR ;
 volatile uint32_t        EMACxIPGRCLR ;
 volatile uint32_t        EMACxIPGRSET ;
 volatile uint32_t        EMACxIPGRINV ;

 __EMACxCLRTbits_t       EMACxCLRT ;
 volatile uint32_t        EMACxCLRTCLR ;
 volatile uint32_t        EMACxCLRTSET ;
 volatile uint32_t        EMACxCLRTINV ;

 __EMACxMAXFbits_t       EMACxMAXF ;
 volatile uint32_t        EMACxMAXFCLR ;
 volatile uint32_t        EMACxMAXFSET ;
 volatile uint32_t        EMACxMAXFINV ;

 __EMACxSUPPbits_t       EMACxSUPP ;
 volatile uint32_t        EMACxSUPPCLR ;
 volatile uint32_t        EMACxSUPPSET ;
 volatile uint32_t        EMACxSUPPINV ;

 __EMACxTESTbits_t       EMACxTEST ;
 volatile uint32_t        EMACxTESTCLR ;
 volatile uint32_t        EMACxTESTSET ;
 volatile uint32_t        EMACxTESTINV ;

 __EMACxMCFGbits_t       EMACxMCFG ;
 volatile uint32_t        EMACxMCFGCLR ;
 volatile uint32_t        EMACxMCFGSET ;
 volatile uint32_t        EMACxMCFGINV ;

 __EMACxMCMDbits_t       EMACxMCMD ;
 volatile uint32_t        EMACxMCMDCLR ;
 volatile uint32_t        EMACxMCMDSET ;
 volatile uint32_t        EMACxMCMDINV ;

 __EMACxMADRbits_t       EMACxMADR ;
 volatile uint32_t        EMACxMADRCLR ;
 volatile uint32_t        EMACxMADRSET ;
 volatile uint32_t        EMACxMADRINV ;


 __EMACxMWTDbits_t  	EMACxMWTD ;
 volatile uint32_t        EMACxMWTDCLR ;
 volatile uint32_t        EMACxMWTDSET ;
 volatile uint32_t        EMACxMWTDINV ;

 __EMACxMRDDbits_t       EMACxMRDD ;
 volatile uint32_t        EMACxMRDDCLR ;
 volatile uint32_t        EMACxMRDDSET ;
 volatile uint32_t        EMACxMRDDINV ;

 __EMACxMINDbits_t       EMACxMIND ;
 volatile uint32_t        EMACxMINDCLR ;
 volatile uint32_t        EMACxMINDSET ;
 volatile uint32_t        EMACxMINDINV ;
 
 volatile uint32_t  addroffset2[8];	

 __EMACxSA0bits_t        EMACxSA0 ;
 volatile uint32_t        EMACxSA0CLR ;
 volatile uint32_t        EMACxSA0SET ;
 volatile uint32_t        EMACxSA0INV ;

 __EMACxSA1bits_t        EMACxSA1 ;
 volatile uint32_t        EMACxSA1CLR ;
 volatile uint32_t        EMACxSA1SET ;
 volatile uint32_t        EMACxSA1INV ;

 __EMACxSA2bits_t        EMACxSA2 ;
 volatile uint32_t        EMACxSA2CLR ;
 volatile uint32_t        EMACxSA2SET ;
 volatile uint32_t        EMACxSA2INV ;
 
} DRV_ETHERNET_REGISTERS;

//*****************************************************************************
/* RMII Speed Selection

  Summary:
    Lists the possible speed selection for the Reduced Media Independent 
    Interface (RMII).

  Description:
    This enumeration lists the possible speed selection for RMII.
    The body contains only two states: RMII_10Mbps or RMII_100Mbps.

  Remarks:
    This enumeration is processor specific and is defined in the processor-
    specific header files (see processor.h).
*/
typedef enum {

    DRV_ETH_RMII_10Mbps  = 0x00,
    DRV_ETH_RMII_100Mps  = 0x01,

} DRV_ETH_RMII_SPEED;


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

typedef enum
{
    // Frames with wrong CRC are accepted
    DRV_ETH_FILT_CRC_ERR_ACCEPT = _ETHRXFC_CRCERREN_MASK,

    // Runt frames accepted
    DRV_ETH_FILT_RUNT_ACCEPT = _ETHRXFC_RUNTERREN_MASK,

    // Frames with wrong CRC are rejected
    DRV_ETH_FILT_CRC_ERR_REJECT = _ETHRXFC_CRCOKEN_MASK,

    // Runt frames rejected
    DRV_ETH_FILT_RUNT_REJECT = _ETHRXFC_RUNTEN_MASK,

    // Me unicast accepted
    DRV_ETH_FILT_ME_UCAST_ACCEPT = _ETHRXFC_UCEN_MASK,

    // Not me unicast accepted
    DRV_ETH_FILT_NOTME_UCAST_ACCEPT = _ETHRXFC_NOTMEEN_MASK,

    // Multicast accepted
    DRV_ETH_FILT_MCAST_ACCEPT = _ETHRXFC_MCEN_MASK,

    // Broadcast accepted
    DRV_ETH_FILT_BCAST_ACCEPT = _ETHRXFC_BCEN_MASK,

    // Hash table matches destination address accepted
    DRV_ETH_FILT_HTBL_ACCEPT = _ETHRXFC_HTEN_MASK,

    // Magic packet accepted
    DRV_ETH_FILT_MAGICP_ACCEPT = _ETHRXFC_MPEN_MASK,


    // All Filters
    DRV_ETH_FILT_ALL_FILTERS = DRV_ETH_FILT_CRC_ERR_ACCEPT  | DRV_ETH_FILT_RUNT_ACCEPT        |
                               DRV_ETH_FILT_CRC_ERR_REJECT  | DRV_ETH_FILT_RUNT_REJECT        |
                               DRV_ETH_FILT_ME_UCAST_ACCEPT | DRV_ETH_FILT_NOTME_UCAST_ACCEPT |
                               DRV_ETH_FILT_MCAST_ACCEPT    | DRV_ETH_FILT_BCAST_ACCEPT       |
                               DRV_ETH_FILT_HTBL_ACCEPT     | DRV_ETH_FILT_MAGICP_ACCEPT 
} DRV_ETH_RX_FILTERS;


// *****************************************************************************
/* Ethernet Pattern Match Modes

  Summary:
    Defines pattern match modes

  Description:
    This enumeration defines the Ethernet pattern match modes.

  Remarks:
    These are mutually exclusive modes, not flags.  However,
    DRV_ETH_FILT_PMATCH_INVERT act as a flag and can be applied to any value.
*/

typedef enum
{
    // Simple Pattern Match accepted
    DRV_ETH_FILT_PMATCH_ACCEPT = 1,

    // Pattern Match AND destination==me
    DRV_ETH_FILT_PMATCH_ME_UCAST_ACCEPT,

    // Pattern Match AND destination!=me
    DRV_ETH_FILT_PMATCH_NOTME_UCAST_ACCEPT,

    // Pattern Match AND destination!=unicast
    DRV_ETH_FILT_PMATCH_MCAST_ACCEPT,

    // Pattern Match AND destination==unicast
    DRV_ETH_FILT_PMATCH_NOT_MCAST_ACCEPT,

    // Pattern Match AND destination==broadcast
    DRV_ETH_FILT_PMATCH_BCAST_ACCEPT,

    // Pattern Match AND destination!=broadcast
    DRV_ETH_FILT_PMATCH_NOT_BCAST_ACCEPT,

    // Pattern Match AND hash table filter match (regardless of the
    // DRV_ETH_FILT_HTBL_ACCEPT setting)
    DRV_ETH_FILT_PMATCH_HTBL_ACCEPT,

    // Pattern Match AND packet ==magic packet
    DRV_ETH_FILT_PMATCH_MAGICP_ACCEPT,

    // If set, the pattern must NOT match for a successful Pattern Match to occur!
    DRV_ETH_FILT_PMATCH_INVERT = 0x80000000

} DRV_ETH_PMATCH_MODE;


// *****************************************************************************
/* Ethernet Event Flags

  Summary:
    Ethernet event flags

  Description:
    This enumeration defines flags for the possible Ethernet events that can
    cause interrupts.
*/

typedef enum
{
    // RX FIFO overflow
    DRV_ETH_EV_RXOVFLOW = _ETHIRQ_RXOVFLW_MASK, 

    // RX buffer not available (descriptor overrun)
    DRV_ETH_EV_RXBUFNA = _ETHIRQ_RXBUFNA_MASK,

    // TX abort condition
    DRV_ETH_EV_TXABORT = _ETHIRQ_TXABORT_MASK,

    // TX done
    DRV_ETH_EV_TXDONE = _ETHIRQ_TXDONE_MASK, 

    // RX activity
    DRV_ETH_EV_RXACT = _ETHIRQ_RXACT_MASK, 

    // RX packet pending
    DRV_ETH_EV_PKTPEND = _ETHIRQ_PKTPEND_MASK,

    // RX packet successfully received
    DRV_ETH_EV_RXDONE = _ETHIRQ_RXDONE_MASK, 

    // Full watermark reached
    DRV_ETH_EV_FWMARK = _ETHIRQ_FWMARK_MASK, 

    // Empty watermark reached
    DRV_ETH_EV_EWMARK = _ETHIRQ_EWMARK_MASK,

    // RX bus error
    DRV_ETH_EV_RXBUSERR = _ETHIRQ_RXBUSE_MASK,

    // TX bus error
    DRV_ETH_EV_TXBUSERR = _ETHIRQ_TXBUSE_MASK,

    // All events
    DRV_ETH_EV_ALL = (DRV_ETH_EV_RXOVFLOW | DRV_ETH_EV_RXBUFNA | DRV_ETH_EV_TXABORT |
                           DRV_ETH_EV_TXDONE   | DRV_ETH_EV_RXACT   | DRV_ETH_EV_PKTPEND |
                           DRV_ETH_EV_RXDONE   | DRV_ETH_EV_FWMARK  | DRV_ETH_EV_EWMARK  |
                           DRV_ETH_EV_RXBUSERR | DRV_ETH_EV_TXBUSERR )
} DRV_ETH_EVENTS;


//*****************************************************************************
/* MII Clock Selection

  Summary:
    Lists the possible speed selection for the Reduced Media Independent 
    Interface (RMII).

  Description:
    This enumeration lists the possible speed selection for RMII.
    The body contains only two states: RMII_10Mbps or RMII_100Mbps.

  Remarks:
    This enumeration is processor specific and is defined in the processor-
    specific header files (see processor.h).
*/
typedef enum {

    ETH_MIIM_SYSCLK_DIV_BY_4   /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_RSVD   /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_6   /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_8   /*DOM-IGNORE-BEGIN*/ = 0x03 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_10  /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_14  /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_20  /*DOM-IGNORE-BEGIN*/ = 0x06 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_28  /*DOM-IGNORE-BEGIN*/ = 0x07 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_40  /*DOM-IGNORE-BEGIN*/ = 0x08 /*DOM-IGNORE-END*/

} ETH_MIIM_CLK;
// *****************************************************************************
// *****************************************************************************
// Section: Driver Library Interface Routines
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void DRV_ETH_PauseTimerSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t PauseTimerValue)

  Summary:
    Sets the Pause Timer value used for flow control.

  Description:
    This function sets the Pause Timer value used for flow control.

  Precondition:
    Write to the Pause Timer register before enabling the receiver. Call this 
	function before calling DRV_ETH_ReceiveEnable.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    PauseTimerValue - used for Flow Control

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_PauseTimerSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t PauseTimerValue);


//******************************************************************************
/* Function:
    void DRV_ETH_Enable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables the Ethernet module.

  Description:
    This function enables the Ethernet module.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_Enable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_Disable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Disables the Ethernet module.

  Description:
    This function disables the Ethernet module.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_Disable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_TxRTSEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables the Ethernet transmit request to send.

  Description:
    This function enables the Ethernet request to send. Transmit logic is activated
    and any packets defined in the Ethernet descriptor table are transmitted.

  Precondition:
    The TX descriptor list and TX DMA must be initialized using 
    DRV_ETH_TransmitPacketDescStartAddrSet.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    This status is cleared by hardware when the transmission is complete.

    None
*/

void DRV_ETH_TxRTSEnable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_TxRTSDisable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Aborts an Ethernet module transmission.

  Description:
    This function aborts an Ethernet module transmission and disables the 
    transmitter after the current packet has completed.

  Precondition
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    When disabled by software, transmission stops after the current packet
    has been completed.

    None
*/

void DRV_ETH_TxRTSDisable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_RxEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables the Ethernet receive logic.

  Description:
    This function enables the Ethernet receive logic. Packets are received and
    stored in the receive buffer as controlled by the filter configuration.

  Precondition:
    All receive registers must be configured before calling this function.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_RxEnable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_RxDisable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Disables the Ethernet module receive logic.

  Description:
    This function disables the Ethernet receive logic.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    Disabling Ethernet receive is not recommended for making changes to any
    receive-related registers. After the receiver has been enabled,
    the Ethernet module must be reinitialized to implement changes.

    None
*/

void DRV_ETH_RxDisable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_AutoFlowControlEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables the Ethernet Automatic Flow Control logic.

  Description:
    This function enables Ethernet Automatic Flow Control logic.
	This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    The full and empty watermarks are used to automatically enable and disable
    flow control, respectively. When the number of received buffers rises to
    the full watermark, flow control is automatically enabled.
    When the receive buffer count falls to the empty watermark, flow control is
    automatically disabled.

    None
*/

void DRV_ETH_AutoFlowControlEnable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_RxBufferCountDecrement(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Causes the Receive Descriptor Buffer Counter to decrement by 1.

  Description:
    This function causes the Receive Descriptor Buffer Counter to decrement by 1.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    Hardware increments the receive buffer counter and software decrements it.
    If the receive buffer counter is incremented by the receive logic
    at the same time, the receive buffer counter will appear unchanged.

    Always reads as '0', so there is no get value routine.

    None
*/

void DRV_ETH_RxBufferCountDecrement(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    int DRV_ETH_RxSetBufferSize(DRV_ETHERNET_REGISTERS* pEthReg, int rxBuffSize)

  Summary:
    Sets the Ethernet module receive buffer size.

  Description:
   This function sets the required buffer size for the receive operation.
    In this implementation, all receive descriptors use the same buffer 
	size (unlike the transmission flow where each descriptor can have 
	a different buffer size)

  Precondition:
    0 < rxBuffSize <= 2032

  Parameters:
    pEthReg - pointer to the Ethernet registers
    ReceiveBufferSize - In units of 16 bytes, must be a value of 0x01 to 0x7F, 
                        0x00 is invalid
  
  Returns:
    None.

  Example:

  Remarks:
    None
	This function should be part of the initialization process and should
	NOT be called when the rx process is active! The receive buffer size 
	is always TRUNCATED to a multiple of 16 bytes.
*/
int DRV_ETH_RxSetBufferSize(DRV_ETHERNET_REGISTERS* pEthReg, int rxBuffSize);


//******************************************************************************
/* Function:
    void DRV_ETH_TxPacketDescAddrSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *txPacketDescStartAddr)

  Summary:
    Sets the Ethernet module transmit packet descriptor start address.

  Description:
    This function sets the Ethernet transmit packet descriptor start address.

  Precondition:
    No transmit, receive, or DMA operations should be in progress when this
    function is called. Call this function before enabling transmit and
    receive.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    txPacketDescStartAddr - This address must be 4-byte aligned. (The least 
                            significant 2 bits must be '00'.)

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_TxPacketDescAddrSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *txPacketDescStartAddr);


//******************************************************************************
/* Function:
    uint8_t *DRV_ETH_TxPacketDescAddrGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the address of the next descriptor to be transmitted.

  Description:
    This function gets the address of the next transmit descriptor.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    Transmit Packet Descriptor Start Address.

  Example:

  Remarks:
    None
*/

uint8_t *DRV_ETH_TxPacketDescAddrGet(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_RxPacketDescAddrSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *rxPacketDescStartAddr)

   Summary:
    Sets the Ethernet module receive packet descriptor start address.

  Description:
    This function sets the Ethernet receive packet descriptor start address.

  Precondition:
    No transmit, receive, or DMA operations should be in progress when this
    function is called. Call this function before enabling transmit and
    receive.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    rxPacketDescStartAddr - This address must be 4-byte aligned. (The least 
                            significant 2 bits must be '00'.)

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_RxPacketDescAddrSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *rxPacketDescStartAddr);



//******************************************************************************
/* Function:
    uint8_t *DRV_ETH_RxPacketDescAddrGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the address of the next receive descriptor.

  Description:
    This function gets the address of the next receive descriptor to be used.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - ReceivePacketDescriptorAddress - receive packet descriptor address

  Example:

  Remarks:
    None
*/

uint8_t * DRV_ETH_RxPacketDescAddrGet(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void  DRV_ETH_RxFiltersHTSet ( DRV_ETHERNET_REGISTERS* pEthReg, uint64_t htable )

  Summary:
    Sets the hash table for the hash table RX filter.

  Description:
     This function sets the hash table for the hash table RX filter.

  Precondition:
    This function should not be called when Ethernet RX operation is enabled and the
    hash filter is active.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    htable      - The hash table itself

  Returns:
    None.

  Example:

  Remarks:
    Properly set the hash table with this function before enabling the Hash
    Table filter.
*/
void  DRV_ETH_RxFiltersHTSet ( DRV_ETHERNET_REGISTERS* pEthReg, uint64_t htable );

//******************************************************************************
/* Function:
    void DRV_ETH_RxFiltersSet (DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RX_FILTERS rxFilters )

  Summary:
    Sets the acceptance/rejection filters for the Ethernet receive.

  Description:
    This function sets the acceptance/rejection filters for the Ethernet receive
    mechanism.  Multiple filters can be OR-ed together.  All filter specified
    in rxFilters will be set.

  Precondition:
   This function should not be called when Ethernet RX operation is enabled.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    rxFilters   - RX filters that are to be set

  Returns:
    None.

  Example:

  Remarks:
    All filters except Pattern Match can be set. Use DRV_ETH_RxFiltersPMSet().
    Before enabling the Hash Table filter, the hash table values should be set
    using DRV_ETH_RxFiltersHTSet().
    See the definition of the DRV_ETH_RX_FILTERS for the priority of the RX filters.
*/
void DRV_ETH_RxFiltersSet (DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RX_FILTERS rxFilters );

//******************************************************************************
/* Function:
    void DRV_ETH_RxFiltersClr (DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RX_FILTERS rxFilters )

  Summary:
     Clears the acceptance/rejection filters for the Ethernet receive.

  Description:
    This function clears the acceptance/rejection filters for the Ethernet receive
    mechanism.  Multiple filters can be OR-ed together.  All filters specified
    in rxFilters will be cleared.

  Precondition:
    This function should not be called when Ethernet RX operation is enabled.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    rxFilters   - RX filters that are to be cleared

  Returns:
    None.

  Example:

  Remarks:
    All filters except Pattern Match can be cleared. Use DRV_ETH_RxFiltersPMClr().
*/
void DRV_ETH_RxFiltersClr (DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RX_FILTERS rxFilters );


//******************************************************************************
/* Function:
    void DRV_ETH_RxFullWmarkSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t watermarkValue)

  Summary:
    Sets the Ethernet module to receive a full watermark.

  Description:
    This function sets the Ethernet to receive a full watermark with a new value.

    The software controlled RX Buffer Full Watermark (Wmark) Pointer is compared
    against the receive buffer count to determine the full watermark
    condition for the full watermark interrupt and for enabling flow control
    if Auto Flow Control is enabled.

    The Full Wmark value should always be greater than the Empty Wmark value.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    watermarkValue - Full watermark value

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_RxFullWmarkSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t watermarkValue);


//******************************************************************************
/* Function:
    void DRV_ETH_RxEmptyWmarkSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t watermarkValue)

  Summary:
    Sets the Ethernet module receive empty water mark.

  Description:
    This function sets the Ethernet receive empty water mark with a new value.
    The software controlled Receive Buffer Empty Wmark is compared against
    the receive buffer count to determine the empty watermark condition
    for the empty watermark interrupt and for disabling flow control
    if Auto Flow Control is enabled.

    The Full Wmark value should always be greater than the Empty Wmark value.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    watermarkValue - Empty watermark value

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_RxEmptyWmarkSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t watermarkValue);


//******************************************************************************
/* Function:
    void DRV_ETH_EventsEnableSet ( DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents )

  Summary:
    Enables the events that will generate interrupts for the Ethernet controller.

  Description:
    The function enables the events that will generate interrupts for the Ethernet
    controller.  Multiple events can be OR-ed together.  Any event that is set
    in the eEvents will be enabled , the other events won't be touched.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    eEvents  - Events with the significance described in the DRV_ETH_EVENTS definition.

  Returns:
    None.

  Example:

  Remarks:
 */

void DRV_ETH_EventsEnableSet ( DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents );


//*******************************************************************************
/*  Function:
    void DRV_ETH_EventsEnableClr ( DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents )

  Summary:
    Disables the events that will generate interrupts.

  Description:
    The function disables the events that will generate interrupts for the Ethernet
    controller.  Multiple events can be OR-ed together.  Any event that is set
    in the eEvents will be disabled , the other events won't be touched.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    eEvents  - Events with the significance described in the DRV_ETH_EVENTS definition.

  Returns:
    None.

  Example:

  Remarks:
*/

void DRV_ETH_EventsEnableClr ( DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents );




//******************************************************************************
/* Function:
    void DRV_ETH_MIIResetEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables the EMAC MIIM Soft reset.

  Description:
    This function enables the EMAC MIIM soft reset.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_MIIResetEnable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_MIIResetDisable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Disables the EMAC Soft reset.

  Description:
    This function disables the EMAC MIIM Soft reset.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_MIIResetDisable(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_MaxFrameLengthSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t MaxFrameLength)

  Summary:
    Sets the EMAC maximum frame length.

  Description:
    This function sets the EMAC maximum frame length.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    MaxFrameLength - Maximum frame length

  Returns:
    None.

  Example:

  Remarks:
    This field resets to 0x05EE, which represents a maximum receive
    frame of 1518 bytes.
    An untagged maximum size Ethernet frame is 1518 bytes.
    A tagged frame adds four octets for a total of 1522 bytes.
    If a shorter/longer maximum length restriction is desired, program
    this 16-bit field.

    If a proprietary header is allowed, this field should be adjusted 
    accordingly. For example, if 4-byte headers are prepended to frames, 
    MACMAXF could be set to 1527 bytes. This would allow the maximum VLAN 
    tagged frame plus the 4-byte header.

    None
*/

void DRV_ETH_MaxFrameLengthSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t MaxFrameLength);

//******************************************************************************
/* Function:
    uint8_t DRV_ETH_EthernetIsBusy(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the status value of the Ethernet logic busy.

  Description:
    This function sets the value of the Ethernet logic busy. A request indicates 
    that the module has just been turned ON or is completing a transaction after 
    being turned OFF.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - true  - Ethernet logic has been turned ON or is completing a transaction
    - false - Ethernet logic is idle

  Example:

  Remarks:
    None
*/

bool DRV_ETH_EthernetIsBusy(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    uint8_t DRV_ETH_RxPacketCountGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the value of the receive packet buffer count.

  Description:
    This function gets the value of the receive packet buffer count.
    When a packet has been successfully received, this value is incremented
    by hardware. Software decrements the counter after a packet has been read.
    Call  DRV_ETH_ReceiveBufferCountDecrement(DRV_ETHERNET_REGISTERS* pEthReg) to decrement.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - RxPacketCount - Number of received packets in memory

  Example:

  Remarks:
    Receive Packet Buffer Counter cannot be decremented below 0x00
    Cleared when the Ethernet module is reset.

    The Receive Packet Buffer Count is not set to '0' when the Ethernet module
    is turned OFF. This allows software to continue to utilize and decrement
    the count.

    None
*/

uint8_t DRV_ETH_RxPacketCountGet(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_EventsClear(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS evmask)

  Summary:
    Clears the Ethernet module event/interrupt source status as a group, using a mask.

  Description:
    This function clears the Ethernet module interrupt source status using a mask.
    Logically 'OR' the masks together. Any masks not ORed with the others will 
    be disabled.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    intmask - Members of DRV_ETH_EVENTS logically ORed together

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_EventsClear(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS evmask);

//******************************************************************************
/* Function:
    bool DRV_ETH_TransmitIsBusy(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the status value of the Ethernet transmit logic busy status

  Description:
    This function gets the value of the Ethernet transmit logic busy status.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - true  - Transmit logic is sending data
    - false - Transmit logic is idle

  Example:

  Remarks:
    None
*/

bool DRV_ETH_TransmitIsBusy(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    bool DRV_ETH_ReceiveIsBusy(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the Ethernet receive logic busy status.

  Description:
    This function gets the Ethernet receive logic busy status.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - true  - Receive logic is receiving data
    - false - Receive logic is idle

  Example:

  Remarks:
    None
*/

bool DRV_ETH_ReceiveIsBusy(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_BackToBackIPGSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t backToBackIPGValue)

  Summary:
    Sets the EMAC back-to-back interpacket gap.

  Description:
    This function sets the EMAC back-to-back interpacket gap.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    backToBackIPGValue - Back-to-back interpacket gap

  Returns:
    None.

  Example:

  Remarks:
    In Full-Duplex mode, the register value should be the desired period
    in nibble times minus 3. In Full-Duplex, the recommended setting is 0x15.

    In Half-Duplex mode, the register value should be the desired period
    in nibble times minus 6. n Half-Duplex, the recommended setting is 0x12.

    None
*/

void DRV_ETH_BackToBackIPGSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t backToBackIPGValue);

//******************************************************************************
/* Function:
    void DRV_ETH_NonBackToBackIPG1Set(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t nonBackToBackIPGValue)

  Summary:
    Sets the EMAC non-back-to-back interpacket gap register 1.

  Description:
    This function sets the EMAC non-back-to-back interpacket gap register 1.
    A value of 0x0C is recommended.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    nonBackToBackIPGValue - Non-back-to-back interpacket gap

  Returns:
    None.

  Example:

  Remarks:
    This is a programmable field representing the optional carrierSense window
    referenced in the IEEE 802.3 Specification.
    If a carrier is detected during the timing of IPGR1, the MAC defers to the carrier.
    If, however, the carrier comes after IPGR1, the MAC continues timing IPGR2 and
    transmits, knowingly causing a collision, thereby ensuring fair access to the medium.
    Its range of values is 0x0 to IPGR2. Its recommended value is 0xC (12d).

    None
*/

void DRV_ETH_NonBackToBackIPG1Set(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t nonBackToBackIPGValue);

//******************************************************************************
/* Function:
    void DRV_ETH_NonBackToBackIPG2Set(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t nonBackToBackIPGValue)

  Summary:
    Sets the EMAC non-back-to-back interpacket gap register 2.

  Description:
    This function sets the EMAC non-back-to-back interpacket gap register 2.
    A value of 0x12 is recommended.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    nonBackToBackIPGValue - Non-back-to-back interpacket gap

  Returns:
    None.

  Example:

  Remarks:
    This is a programmable field representing the non-back-to-back
    Inter-Packet-Gap. Its recommended value is 0x12 (18d), which represents
    the minimum IPG of 0.96 us (in 100 Mbps) or 9.6 us (in 10 Mbps).

    None
*/

void DRV_ETH_NonBackToBackIPG2Set(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t nonBackToBackIPGValue);

//******************************************************************************
/* Function:
    void DRV_ETH_CollisionWindowSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t collisionWindowValue)

  Summary:
    Sets the EMAC collision window.

  Description:
    This function sets the EMAC collision window.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    collisionWindowValue - Collision window

  Returns:
    None.

  Example:

  Remarks:
    This is a programmable field representing the slot time or collision window
    during which collisions occur in properly configured networks. Since the
    collision window starts at the beginning of transmission, the preamble
    and SFD is included. Its default of 0x37 (55d) corresponds to the count of
    frame bytes at the end of the window.

    None
*/

void DRV_ETH_CollisionWindowSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t collisionWindowValue);

//******************************************************************************
/* Function:
    void DRV_ETH_ReTxMaxSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t retransmitMax)

  Summary:
    Sets the EMAC maximum retransmissions.

  Description:
    This function sets the EMAC maximum retransmissions.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    retransmitMax - Maximum number of retransmissions

  Returns:
    None.

  Example:

  Remarks:
    This is a programmable field specifying the number of retransmission
    attempts following a collision before aborting the packet due to excessive
    collisions. The IEEE 802.3 Specification standard specifies the maximum
    number of attempts (attemptLimit) to be 0xF (15d). Its default is 000.

    None
*/

void DRV_ETH_ReTxMaxSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t retransmitMax);

//******************************************************************************
/* Function:
    void DRV_ETH_RMIIResetEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables EMAC Reset RMII.

  Description:
    This function enables EMAC Reset RMII.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_RMIIResetEnable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_RMIIResetDisable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Disables EMAC Reset RMII.

  Description:
    This function disables EMAC Reset RMII for normal operation.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_RMIIResetDisable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_RMIISpeedSet(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RMII_SPEED RMIISpeed)

  Summary:
    Sets EMAC RMII Speed.

  Description:
    This function sets EMAC RMII speed. RMII speed can be either RMII_100Mbps or 
    RMII_10Mbps.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    RMIISpeed - RMII_100Mbps or RMII_10Mbps of type DRV_ETH_RMII_SPEED

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_RMIISpeedSet(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RMII_SPEED RMIISpeed);

//******************************************************************************
/* Function:
    void DRV_ETH_TxPauseEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables the transmission Pause frames.

  Description:
    This function enables the transmission Pause frames. The Pause frames are 
    allowed for transmission.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_TxPauseEnable(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_RxPauseEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables the EMAC receive flow control.

  Description:
    This function enables the EMAC receive flow control. The EMAC will act upon 
    the received Pause frames.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    None
*/

void DRV_ETH_RxPauseEnable(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
     void DRV_ETH_MACSetAddress (DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *bAddress )

  Summary:
    Sets the MAC address.

  Description:
    This function sets the selected MAC Station Address.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    bAddress    - standard MAC address, 6 bytes, Network order!

  Returns:
    None.

  Example:

  Remarks:
    On a reset, this register is loaded with the factory preprogrammed
    station address.
*/

void DRV_ETH_MACSetAddress (DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *bAddress );

//******************************************************************************
/* Function:
    void DRV_ETH_MACGetAddress (DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *bAddress )

  Summary:
    Returns the current MAC address.

  Description:
    This function returns the current MAC address.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    bAddress    - Address to store a standard MAC address, 6 bytes, Network order!

  Returns:
    None.

  Example:

  Remarks:
    On a reset, this register is loaded with the factory preprogrammed
    station address.

    None
*/
void DRV_ETH_MACGetAddress (DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *bAddress );

//******************************************************************************
/* Function:
    uint16_t DRV_ETH_FramesTxdOkCountGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the count of Ethernet Control Frames transmitted successfully.

  Description:
    This function gets the count of Ethernet Control Frames transmitted successfully.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - count - count of control frames transmitted correctly

  Example:

  Remarks:
    None
*/

uint16_t DRV_ETH_FramesTxdOkCountGet(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    uint16_t DRV_ETH_FramesRxdOkCountGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the count of the frames frames received successfully.

  Description:
    This function gets the count of the frames received successfully.
    Increment count for frames received successfully by the RX Filter.
    This count will not be incremented if there is a Frame Check Sequence (FCS)
    or Alignment error.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - value - Count of frames received correctly

  Example:

  Remarks:
    None
*/

uint16_t DRV_ETH_FramesRxdOkCountGet(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    uint16_t DRV_ETH_RxOverflowCountGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the count of the dropped Ethernet receive frames.

  Description:
    This function gets the count of the dropped Ethernet receive frames.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - count - uint16_t receiver overflow counts

  Example:

  Remarks:
    None
*/

uint16_t DRV_ETH_RxOverflowCountGet(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    uint16_t DRV_ETH_FCSErrorCountGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the count of the frame check sequence error.

  Description:
    This function gets the count of the frame check sequence error.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - value - Count of FCS Errors

  Example:

  Remarks:
    None
*/

uint16_t DRV_ETH_FCSErrorCountGet(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    uint16_t DRV_ETH_AlignErrorCountGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the count of Ethernet alignment errors.

  Description:
    This function gets the count of Ethernet alignment errors.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - value - Count of alignment errors

  Example:

  Remarks:
    None
*/

uint16_t DRV_ETH_AlignErrorCountGet(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    uint16_t DRV_ETH_SingleCollisionCountGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the count of the frames transmitted successfully on a second attempt.

  Description:
    This function gets the count of the frames transmitted successfully on
    a second attempt.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - count - Count of frames transmitted successfully on second attempt

  Example:

  Remarks:
    None
*/

uint16_t DRV_ETH_SingleCollisionCountGet(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    uint16_t DRV_ETH_MultipleCollisionCountGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the count of the frames transmitted successfully after there was more 
    than one collision.

  Description:
    This function gets the count of the frames transmitted successfully after 
    there was more than one collision.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - count - Count of multiple collision frames

  Example:

  Remarks:
    None
*/

uint16_t DRV_ETH_MultipleCollisionCountGet(DRV_ETHERNET_REGISTERS* pEthReg);

//*******************************************************************************
/*  Function:
    void DRV_ETH_EventsClr ( DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents )

  Summary:
    Clears the selected events for the Ethernet controller.

  Description:
    The function clears the selected events for the Ethernet controller.  Multiple
    events can be OR-ed together. Any event that is set in the eEvents will be
    cleared, the other events won't be touched.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    eEvents  - Events with the significance described in the DRV_ETH_EVENTS definition.

  Returns:
    None.

  Example:

  Remarks:
    The DRV_ETH_EV_FWMARK cannot be cleared directly. It is cleared indirectly by
    DRV_ETH_RxAcknowledgePacket/DRV_ETH_RxAcknowledgeBuffer.

    The DRV_ETH_EV_EWMARK cannot be cleared directly. It is cleared by hardware when
    receiving a new packet.

    The DRV_ETH_EV_PKTPEND cannot be cleared directly. It is cleared indirectly by
    DRV_ETH_RxAcknowledgePacket/DRV_ETH_RxAcknowledgeBuffer.

*/

void DRV_ETH_EventsClr( DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents );

/*******************************************************************************
  Function:
    DRV_ETH_EVENTS DRV_ETH_EventsGet ( DRV_ETHERNET_REGISTERS* pEthReg )

  Summary:
    Returns the active events for the Ethernet controller.

  Description:
    This function returns the active events for the Ethernet controller.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    Events with the significance described in the DRV_ETH_EVENTS definition.

  Example:

  Remarks:
 *****************************************************************************/

DRV_ETH_EVENTS DRV_ETH_EventsGet ( DRV_ETHERNET_REGISTERS* pEthReg );


//******************************************************************************
/* Function: 
    bool DRV_ETH_IsEnabled(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the Ethernet receive logic busy status.

  Description:
    This function gets the Ethernet receive logic busy status.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - true  - Receive logic is receiving data
    - false - Receive logic is idle

  Example:

  Remarks:
    None
*/

//******************************************************************************
/* Function:
    bool DRV_ETH_IsEnabled(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the Ethernet module enable status.

  Description:
    This function returns the Ethernet module enable status.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - true  - Ethernet module is enabled
    - false - Ethernet module is disabled

  Example:

  Remarks:
*/
bool DRV_ETH_IsEnabled(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_MIIMNoPreEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables EMAC MIIM No Preamble (suppresses preamble).

  Description:
    This function enables EMAC MIIM No Preamble (suppresses preamble). The MII 
    Management will perform read/write cycles without the 32-bit preamble field.
    Some PHYs support suppressed preamble.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMNoPreEnable(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_MIIMNoPreDisable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Disables EMAC No Preamble (allows preamble).

  Description:
    This function disables EMAC No preamble (allows preamble). Normal read/write 
    cycles are performed.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMNoPreDisable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_MIIMScanIncrEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables EMAC MIIM Scan Increment.

  Description:
    This function enables EMAC MIIM Scan Increment. The MII Management module will 
    perform read cycles across a range of PHYs. The read cycles will start from 
    address 1 through the value set in the PHY address register.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    The read cycles will start at PHY address 1 and continue through the value
    set for as the PHY address register.

*/
void DRV_ETH_MIIMScanIncrEnable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_MIIMScanIncrDisable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Disables the EMAC MIIM Scan Increment.

  Description:
    This function disables the EMAC MIIM Scan Increment. Allows continuous reads 
    of the same PHY.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMScanIncrDisable(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    bool DRV_ETH_MIIMIsBusy(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the MII management busy status.

  Description:
    This function returns the MII management busy status.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - true  - The MII Management module is currently performing an MII Management 
              read or write cycle
    - false - The MII Management is free

  Example:

  Remarks:
*/
bool DRV_ETH_MIIMIsBusy(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_PHYAddressSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t phyAddr)

  Summary:
    Sets the EMAC MIIM PHY address.

  Description:
    This function sets the EMAC MIIM PHY address. This field represents the 5-bit 
    PHY Address field of Management cycles. Up to 31 PHYs can be addressed (0 is reserved).

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    phyAddr - A 5-bit address of the PHY

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_PHYAddressSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t phyAddr);

//******************************************************************************
/* Function:
    void DRV_ETH_RegisterAddressSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t regAddr)

  Summary:
    Sets EMAC MIIM register address.

  Description:
    This function sets the EMAC MIIM register address. Up to 32 registers may 
    be accessed.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    regAddr - The (5-bit) address of the MII Registers.

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_RegisterAddressSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t regAddr);

//******************************************************************************
/* Function:
    void DRV_ETH_ClearDataValid(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Clears the EMAC MIND register data valid indicator

  Description:
    This function clears the EMAC MIND register data valid indicator. 

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Remarks:
*/
void DRV_ETH_ClearDataValid(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_MIIMScanModeEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables MIIM scan mode.

  Description:
    This function enables MIIM scan mode. The MII Management module will perform 
    read cycles continuously. (Useful for monitoring the Link Fail.)

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMScanModeEnable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_MIIMScanModeDisable(DRV_ETHERNET_REGISTERS* pEthReg)
  Summary:
    Disables MIIM scan mode.

  Description:
    This function disables MIIM scan mode. Scan is disabled for Normal operation.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMScanModeDisable(DRV_ETHERNET_REGISTERS* pEthReg);


//******************************************************************************
/* Function:
    void DRV_ETH_MIIMWriteDataSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t writeData)

  Summary:
    Sets the EMAC MIIM write data before initiating an MII write cycle.

  Description:
    This function sets the EMAC MIIM write data before initiating write cycle.

  Precondition:
    Prior to a call to this routine, the PHY and Register addresses
    should be set using DRV_ETH_MIIPHYAddressSet and DRV_ETH_MIIRegisterAddressSet.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    writeData - MII write data

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMWriteDataSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t writeData);

//******************************************************************************
/* Function:
    void DRV_ETH_MIIMWriteStart(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Initiates an MII management write command.

  Description:
    This function initiates an MII management read command. The MII Management module 
    will perform a write cycle.

  Precondition:
    The PHY address and MII register address must be configured before a write
    using DRV_ETH_MIIPHYAddressSet(MY_ETH_INSTANCE, phyAddr)   and
    DRV_ETH_MIIRegisterAddressSet(MY_ETH_INSTANCE, regAddr)

    Data to be written must be first loaded into the MII write register using
    DRV_ETH_MIIMWriteDataSet(MY_ETH_INSTANCE, writeData)

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMWriteStart(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_MIIMReadStart(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Initiates an MII management read command.

  Description:
    This function initiates an MII read command. The MII Management module will 
    perform a single read cycle. To get data, use DRV_ETH_MIIMReadDataGet.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMReadStart(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    uint16_t DRV_ETH_MIIMReadDataGet(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets EMAC MIIM management read data after a MII read cycle has completed.

  Description:
    This function gets EMAC MIIM management read data after a MII read cycle
    has completed.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - readData - MII read data

  Example:

  Remarks:
*/
uint16_t DRV_ETH_MIIMReadDataGet(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    bool DRV_ETH_DataNotValid(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Gets the MII management read data not valid status.

  Description:
    This function gets the MII management read data not valid status.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    - true  - The MII Management read cycle has not completed and the read data
              is not yet valid
    - false - The MII Management read cycle is complete and the read data is valid

  Example:

  Remarks:
*/
bool DRV_ETH_DataNotValid(DRV_ETHERNET_REGISTERS* pEthReg); 


//******************************************************************************
/* Function:
    void DRV_ETH_MIIMResetEnable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Enables EMAC Reset Media Independent Interface (MII) Management.

  Description:
    This function enables EMAC Reset MII Management and holds the MII Management 
    module in reset while enabled.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
    MII Management held in Reset after This function is called. Disable ResetMIIanagement 
    to return to normal operation.

*/
void DRV_ETH_MIIMResetEnable(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_MIIMResetDisable(DRV_ETHERNET_REGISTERS* pEthReg)

  Summary:
    Disables EMAC Reset MII Management.

  Description:
    This function disables EMAC Reset MII Management.
    EMAC will resume normal operation.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers

  Returns:
    None.

  Example:

  Remarks:
*/
void DRV_ETH_MIIMResetDisable(DRV_ETHERNET_REGISTERS* pEthReg);

//******************************************************************************
/* Function:
    void DRV_ETH_MIIMClockSet(DRV_ETHERNET_REGISTERS* pEthReg, ETH_MIIM_CLK MIIMClock)

  Summary:
    Sets the EMAC MIM clock selection.

  Description:
    This function sets the EMAC MIIM clock selection.

  Precondition:
    None.

  Parameters:
    pEthReg - pointer to the Ethernet registers
    MIIMClock - of type ETH_MIIM_CLK - the system clock divisor for MII

  Returns:
    None.

  Example:

  Remarks:
    This field is used by the clock divide logic in creating the MII Management 
    Clock (MDC), which the IEEE 802.3 Specification defines to be no faster than 
    2.5 MHz. Some PHYs support clock rates up to 12.5 MHz.

*/
void DRV_ETH_MIIMClockSet(DRV_ETHERNET_REGISTERS* pEthReg , ETH_MIIM_CLK MIIMClock );

#ifdef __cplusplus
}
#endif


#endif //_DRV_ETH_LIB_H_

//******************************************************************************
/* End of File
*/
