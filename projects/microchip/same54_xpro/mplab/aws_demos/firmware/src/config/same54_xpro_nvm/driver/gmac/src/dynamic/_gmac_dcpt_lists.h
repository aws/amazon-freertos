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



#ifndef __GMAC_DCPT_LISTS_H_
#define __GMAC_DCPT_LISTS_H_

// *****************************************************************************
/*  GMAC Queue List
 
    Summary: List of Multiple Queues supported by GMAC    
*/
typedef enum  
{
	GMAC_QUE_0 = 0,
	GMAC_QUE_1 = 1,
	GMAC_QUE_2 = 2,
    GMAC_QUE_3 = 3,
	GMAC_QUE_4 = 4,
	GMAC_QUE_5 = 5
} GMAC_QUE_LIST;

// *****************************************************************************
/*  TX Packet Descriptor Status Word

  Summary:
    Status word with each Transmit Descriptor

  Description:
    The transmit descriptor consist of two 32-bit word; one is Transmit Status
    word. The status of a packet transmission is stored in this word by GMAC
*/

typedef union
{
	uint32_t val;                   /**< 32-Bit access */
	struct _GmacTxStatusBM 
	{
		uint32_t len: 11,           /**< Length of buffer */
					reserved: 4,
					bLastBuffer: 1, /**< Last buffer (in the current frame) */
					bNoCRC: 1,      /**< No CRC */
					reserved1: 10,
					bExhausted: 1,  /**< Buffer exhausted in mid frame */
					bUnderrun: 1,   /**< Transmit under run */
					bError: 1,      /**< Retry limit exceeded, error detected */
					bWrap: 1,       /**< Marks last descriptor in TD list */
					bUsed: 1;       /**< User clear, GMAC sets this once a frame
                                        has been successfully transmitted */
	} bm;
} DRV_GMAC_TXDCPT_STATUS;

// *****************************************************************************
/*  RX Packet Descriptor Address Word

  Summary:
    Address word with each Receive Descriptor

  Description:
    The Receive descriptor consist of two 32-bit word; one is Receive address
    word. The Rx buffer address , the Ownership bit and Wrap bit stored in this 
    word
*/
typedef union
{
	uint32_t val;
	struct _GmacRxAddrBM 
	{
		uint32_t    bOwnership: 1, /**< User clear, GMAC set this to one once
                                    it has successfully written a frame to
                                        memory */
                    bWrap: 1,       /**< Marks last descriptor in receive buffer */
					addrDW: 30;     /**< Address in number of DW */
	} bm;
} DRV_GMAC_RXDCPT_ADDR;             /**< Address, Wrap & Ownership */

// *****************************************************************************
/*  RX Packet Descriptor Status Word

  Summary:
    Status word with each Receive Descriptor

  Description:
    The Receive descriptor consist of two 32-bit word; one is Receive Status
    word. Different receive status after receiving a packet is stored in this
    32-bit field by GMAC engine
*/
typedef union  
{
	uint32_t val;
	struct _GmacRxStatusBM 
	{
		uint32_t	len: 13,				/** Length of frame including FCS */
					offset: 1,              /** Receive buffer offset,
                                            bits 13:12 of frame length for jumbo
                                            frame */
					bSof: 1,				/** Start of frame */
					bEof: 1,				/** End of frame */
					bCFI: 1,				/** Concatenation Format Indicator */
					vlanPriority: 3,		/** VLAN priority (if VLAN detected) */
					bPriorityDetected: 1,	/** Priority tag detected */
					bVlanDetected: 1,		/**< VLAN tag detected */
					bTypeIDMatch: 2,		/**< Type ID match */
					bTypeIDMatchfound: 1,	/**< Type ID match found*/
					bAddrMatch: 2,			/**< Specific Address register 1, 2, 3, 4 match */
					bAddrMatchfound: 1,     /**< Specific Address match found */
					reserved: 1,				
					bUniHashMatch: 1,       /**< Unicast hash match */
					bMultiHashMatch: 1,     /**< Multicast hash match */
					bBroadcastDetected: 1;  /**< Global all ones broadcast
                                                address detected */
	} bm;
} DRV_GMAC_RXDCPT_STATUS;


// *****************************************************************************
/*  Receive Descriptor

  Summary:
    RX descriptor for control and status of GMAC RX block

  Description:
    The Receive descriptor consist of two 32-bit word; one is Receive buffer 
    address and other for Rx status
*/
typedef struct
{
	volatile DRV_GMAC_RXDCPT_ADDR rx_desc_buffaddr;
	volatile DRV_GMAC_RXDCPT_STATUS rx_desc_status;
	
} DRV_PIC32CGMAC_HW_RXDCPT; 

// *****************************************************************************
/*  Transmit Descriptor

  Summary:
    TX descriptor for control and status of GMAC Transmit block

  Description:
    The Transmit descriptor consist of two 32-bit word; one is Transmit buffer
    address  and other for Tx status
*/
typedef struct
{
	volatile uint32_t tx_desc_buffaddr;
	volatile DRV_GMAC_TXDCPT_STATUS tx_desc_status;	
} DRV_PIC32CGMAC_HW_TXDCPT;   


////////////////////////////////////////////////////////////////////////////////
//////////////  GMAC Single linked lists manipulation //////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct _TAG_DRV_PIC32CGMAC_SGL_LIST_NODE
{
	struct _TAG_DRV_PIC32CGMAC_SGL_LIST_NODE*	next;       /**< next linked list  */      
	void*                                       data;       /**< data with this node */
	uint16_t                                    startIndex; /**< Start index of Tx/Rx descriptor*/
	uint16_t                                    endIndex;   /**< Start index of Tx/Rx descriptor */
	GMAC_QUE_LIST                               queueId;    /**< Queue index */
}DRV_PIC32CGMAC_SGL_LIST_NODE;                  /**< Single list for GMAC packet */ 


typedef struct
{
	DRV_PIC32CGMAC_SGL_LIST_NODE*	head;	/**< Linked list head  */ 
	DRV_PIC32CGMAC_SGL_LIST_NODE*	tail;   /**< Linked list tail  */ 
    int             nNodes;                 /**< number of nodes in the list */ 

}DRV_PIC32CGMAC_SGL_LIST;	// single linked list

//Initialize Single List
static  __inline__ void  __attribute__((always_inline)) DRV_PIC32CGMAC_SingleListInitialize(DRV_PIC32CGMAC_SGL_LIST* pL)
{
    pL->head = pL->tail = 0;
    pL->nNodes = 0;
}

// Return number of nodes in list
static  __inline__ int __attribute__((always_inline)) DRV_PIC32CGMAC_SingleListCount(DRV_PIC32CGMAC_SGL_LIST* pL)
{
    return pL->nNodes;
}

// Removes the head node from list
DRV_PIC32CGMAC_SGL_LIST_NODE*  DRV_PIC32CGMAC_SingleListHeadRemove(DRV_PIC32CGMAC_SGL_LIST* pL);

// Add a node to the tail of list
void  DRV_PIC32CGMAC_SingleListTailAdd(DRV_PIC32CGMAC_SGL_LIST* pL, DRV_PIC32CGMAC_SGL_LIST_NODE* pN);

// Add a node to the head of list
void  DRV_PIC32CGMAC_SingleListHeadAdd(DRV_PIC32CGMAC_SGL_LIST* pL, DRV_PIC32CGMAC_SGL_LIST_NODE* pN);
#endif //  __GMAC_DCPT_LISTS_H_
