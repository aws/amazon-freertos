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



#ifndef __ETH_DCPT_LISTS_H_
#define __ETH_DCPT_LISTS_H_

#include <sys/kmem.h>
#include <stdint.h>

// *****************************************************************************
/*  Packet Descriptor

  Summary:
    Descriptor of a packet accepted by the TX/RX Ethernet engine.

  Description:
    A packet handled by the Ethernet TX/RX engine is a list of buffer
    descriptors.  A packet consists of multiple buffers and each buffer needs a
    descriptor.  Although the number of buffers per packet is not limited, note
    that the hardware overhead is higher when many buffers have to be handled
    for one packet.  The list ends when the next field is NULL or when the pBuff
    is NULL.
*/

typedef struct _tag_DRV_ETHMAC_PKT_DCPT
{
    // Next descriptor in chain. NULL to end
    struct _tag_DRV_ETHMAC_PKT_DCPT  *next;

    // Buffer to be transmitted
    void                   *pBuff;

    // Number of bytes in the buffer 0-2047 allowed
    uint16_t                nBytes;

} /*DOM-IGNORE-BEGIN*/ __attribute__ ((__packed__)) /*DOM-IGNORE-END*/ DRV_ETHMAC_PKT_DCPT;


// *****************************************************************************
/*  Transmitted Packet Status

  Summary:
    Status of a transmitted packet.

  Description:
    This structure contains the status of a transmitted packet.

  Notes:
    Status is always two "words" (64-bits) long.
*/

typedef union
{
    struct
    {
        // Total bytes transmitted
        uint64_t        totTxBytes   :16;

        // Control frame transmitted
        uint64_t         txCtrl      : 1;

        // Pause control frame transmitted
        uint64_t         txPause     : 1;

        // Transmit backpressure applied
        uint64_t         txBPres     : 1;

        // Transmit VLAN tagged frame
        uint64_t         txVLAN      : 1;

        uint64_t                     : 12;

        // Transmit bytes count
        uint64_t         bCount      :16;

        // Transmit collision count
        uint64_t        collCount   : 4;

        // Transmit CRC error
        uint64_t         crcError    : 1;

        // Transmit length check error
        uint64_t         lenError    : 1;

        // Transmit length out of range
        uint64_t         lenRange    : 1;

        // Transmit done
        uint64_t         txDone      : 1;

        // Transmit multicast
        uint64_t         mcast       : 1;

        // Transmit broadcast
        uint64_t         bcast       : 1;

        // Transmit packet defer
        uint64_t         defer       : 1;

        // Transmit excessive packet defer
        uint64_t         excDefer    : 1;

        // Transmit maximum collision
        uint64_t         maxColl     : 1;

        // Transmit late collision
        uint64_t         lateColl    : 1;

        // Transmit giant frame (set when pktSz>MaxFrameSz && HugeFrameEn==0)
        uint64_t         giant       : 1;

        // Transmit underrun
        uint64_t         underrun    : 1;

    } __attribute__ ((__packed__));

    // Status is 2 words always
    uint64_t            w;

} DRV_ETHMAC_PKT_STAT_TX;


// *****************************************************************************
/*  Received Packet Status

  Summary:
    Status of a received packet.

  Description:
    This structure contains the status of a received packet.

  Notes:
    Status is always two "words" (64-bits) long.
*/

typedef union
{
    struct
    {
        // Packet payload checksum
        uint64_t        pktChecksum     :16;

        uint64_t                        : 8;

        // Runt packet received
        uint64_t        runtPkt         : 1;

        // Unicast, not me packet,
        uint64_t        notMeUcast      : 1;

        // Hash table match
        uint64_t        htMatch         : 1;

        // Magic packet match
        uint64_t        magicMatch      : 1;

        // Pattern match match
        uint64_t        pmMatch         : 1;

        // Unicast match
        uint64_t        uMatch          : 1;

        // Broadcast match
        uint64_t        bMatch          : 1;

        // Multicast match
        uint64_t        mMatch          : 1;

        // Received bytes
        uint64_t        rxBytes         :16;

        // Packet previously ignored
        uint64_t        prevIgnore      : 1;

        // RX data valid event previously seen
        uint64_t        prevDV          : 1;

        // Carrier event previously seen
        uint64_t        prevCarrier     : 1;

        // RX code violation
        uint64_t        rxCodeViol      : 1;

        // CRC error in packet
        uint64_t        crcError        : 1;

        // Receive length check error
        uint64_t        lenError        : 1;

        // Receive length out of range
        uint64_t        lenRange        : 1;

        // Receive OK
        uint64_t        rxOk            : 1;

        // Multicast packet
        uint64_t        mcast           : 1;

        // Broadcast packet
        uint64_t        bcast           : 1;

        // Dribble nibble
        uint64_t        dribble         : 1;

        // Control frame received
        uint64_t        rxCtrl          : 1;

        // Pause control frame received
        uint64_t        rxPause         : 1;

        // Received unsupported code
        uint64_t        rxCodeErr       : 1;

        // Received VLAN tagged frame
        uint64_t        rxVLAN          : 1;

        uint64_t                        : 1;

    }__attribute__ ((__packed__));

    // Status is 2 words always
    uint64_t            w;

} DRV_ETHMAC_PKT_STAT_RX;


// descriptors

typedef union
{
    struct
    {
        uint32_t        : 7;
        uint32_t EOWN   : 1;
        uint32_t NPV    : 1;
        uint32_t sticky : 1;    // a receive buffer is sticky
        uint32_t kv0    : 1;    // belongs to k0. else k1
        uint32_t rx_wack: 1;    // RX buffer waiting for acknowledge
        uint32_t rx_nack: 1;    // RX descriptor is not acknowledged
        uint32_t        : 3;
        uint32_t bCount : 11;
        uint32_t        : 3;
        uint32_t EOP    : 1;
        uint32_t SOP    : 1;
    };
    uint32_t            w;
}DRV_ETHMAC_DCPT_HDR;  // descriptor header

#define _SDCPT_HDR_EOWN_MASK_       0x00000080
#define _SDCPT_HDR_NPV_MASK_        0x00000100
#define _SDCPT_HDR_STICKY_MASK_     0x00000200
#define _SDCPT_HDR_KV0_MASK_        0x00000400
#define _SDCPT_HDR_RX_WACK_MASK_    0x00000800
#define _SDCPT_HDR_BCOUNT_MASK_     0x07ff0000
#define _SDCPT_HDR_BCOUNT_POS_      16
#define _SDCPT_HDR_EOP_MASK_        0x40000000
#define _SDCPT_HDR_SOP_MASK_        0x80000000


typedef struct
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    uint8_t*                pEDBuff;    // data buffer address
    volatile uint64_t       stat;       // TX/RX packet status
    uint32_t                next_ed;    // next descriptor (hdr.NPV==1);
}__attribute__ ((__packed__)) DRV_ETHMAC_HW_DCPT;   // hardware TX/RX descriptor (linked).


typedef struct __attribute__ ((__packed__))
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    uint8_t*                pEDBuff;    // data buffer address
    volatile DRV_ETHMAC_PKT_STAT_TX  stat;       // transmit packet status
    uint32_t                next_ed;    // next descriptor (hdr.NPV==1);
} DRV_ETHMAC_HW_DCPT_TX; // hardware TX descriptor (linked).


typedef struct __attribute__ ((__packed__))
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    uint8_t*                pEDBuff;    // data buffer address
    volatile DRV_ETHMAC_PKT_STAT_RX  stat;       // transmit packet status
    uint32_t                next_ed;    // next descriptor (hdr.NPV==1);
} DRV_ETHMAC_HW_DCPT_RX; // hardware Rx descriptor (linked).



typedef struct _tag_DRV_ETHMAC_DCPT_NODE
{
    struct _tag_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT          hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE; // Ethernet descriptor node: generic linked descriptor for both TX/RX.


typedef struct
{
    struct _tag_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT_TX            hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE_TX;   // TX linked descriptor


typedef struct
{
    struct _tag_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT_RX            hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE_RX;   // Rx linked descriptor

// although there could be unlinked TX and RX descriptors (hdr.NPV==0), we don't use them in this implementation


#if defined(__PIC32MZ__)
// for PIC32MZ these lists have to be aligned on a cache line
#define DRV_ETHMAC_DCPT_LIST_ALIGN      16
typedef struct __attribute__((aligned(16)))
{
    uint64_t    pad;
    DRV_ETHMAC_DCPT_NODE*  head;   // list head
    DRV_ETHMAC_DCPT_NODE*  tail;
}DRV_ETHMAC_DCPT_LIST;  // single linked list
#else
// for PIC32MX alignment is 1 byte
#define DRV_ETHMAC_DCPT_LIST_ALIGN      1
typedef struct
{
    DRV_ETHMAC_DCPT_NODE*  head;   // list head
    DRV_ETHMAC_DCPT_NODE*  tail;
}DRV_ETHMAC_DCPT_LIST;  // single linked list
#endif  // defined(__PIC32MZ__)

/////  single linked lists manipulation ///////////
//
#define     DRV_ETHMAC_LIB_ListIsEmpty(pL)       ((pL)->head==0)

extern __inline__ int __attribute__((always_inline)) DRV_ETHMAC_LIB_ListCount(DRV_ETHMAC_DCPT_LIST* pL)
{

    DRV_ETHMAC_DCPT_NODE*  pN;
    int     nNodes=0;
    for(pN=pL->head; pN!=0; pN=pN->next)
    {
        nNodes++;
    }
    return nNodes;
}


extern __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAddHead(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_NODE* pN)
{
    pN->next=pL->head;
    pN->hwDcpt.next_ed=KVA_TO_PA(&pL->head->hwDcpt);
    pL->head=pN;
    if(pL->tail==0)
    {  // empty list
        pL->tail=pN;
    }

}


extern __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAddTail(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_NODE* pN)
{
    pN->next=0;
    pN->hwDcpt.next_ed = 0;
    if(pL->tail==0)
    {
        pL->head=pL->tail=pN;
    }
    else
    {
        pL->tail->next=pN;
        pL->tail->hwDcpt.next_ed=KVA_TO_PA(&pN->hwDcpt);
        pL->tail=pN;
    }

}


// insertion in the middle, not head or tail
#define     SlAddMid(pL, pN, after) do{ (pN)->next=(after)->next; (pN)->next_ed=(after)->next_ed; \
                            (after)->next=(pN); (after)->next_ed=KVA_TO_PA(&pN->hwDcpt); \
                        }while(0)


extern __inline__ DRV_ETHMAC_DCPT_NODE* __attribute__((always_inline)) DRV_ETHMAC_LIB_ListRemoveHead(DRV_ETHMAC_DCPT_LIST* pL)
{
    DRV_ETHMAC_DCPT_NODE* pN=pL->head;
    if(pL->head==pL->tail)
    {
        pL->head=pL->tail=0;
    }
    else
    {
        pL->head=pN->next;
    }

    return pN;
}


extern __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAppendTail(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_LIST* pAList)
{
    DRV_ETHMAC_DCPT_NODE* pN;
    while((pN=DRV_ETHMAC_LIB_ListRemoveHead(pAList)))
    {
        DRV_ETHMAC_LIB_ListAddTail(pL, pN);
    }
}


#if defined(__PIC32MZ__)
// flushes a data cache line/address
extern __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_DataLineFlush(void* address)
{   // issue a hit-writeback invalidate order
    __asm__ __volatile__ ("cache 0x15, 0(%0)" ::"r"(address));
    __asm__ __volatile__ ("sync");
}
#endif  // defined(__PIC32MZ__)


extern __inline__ DRV_ETHMAC_DCPT_LIST* __attribute__((always_inline)) DRV_ETHMAC_LIB_ListInit(DRV_ETHMAC_DCPT_LIST* pL)
{
#if defined(__PIC32MZ__)
    DRV_ETHMAC_LIB_DataLineFlush(pL);
    DRV_ETHMAC_DCPT_LIST* pNL = KVA0_TO_KVA1(pL);
#else
    DRV_ETHMAC_DCPT_LIST* pNL = pL;
#endif

    pNL->head = pNL->tail = 0;
    return pNL;
}

/////  generic single linked lists manipulation ///////////
//
//
typedef struct _TAG_DRV_ETHMAC_SGL_LIST_NODE
{
	struct _TAG_DRV_ETHMAC_SGL_LIST_NODE*	next;		// next node in list
    void*                                   data[0];    // generic payload    
}DRV_ETHMAC_SGL_LIST_NODE;	// generic linked list node definition


typedef struct
{
	DRV_ETHMAC_SGL_LIST_NODE*	head;	// list head
	DRV_ETHMAC_SGL_LIST_NODE*	tail;
    int             nNodes; // number of nodes in the list

}DRV_ETHMAC_SGL_LIST;	// single linked list

extern __inline__ void  __attribute__((always_inline)) DRV_ETHMAC_SingleListInitialize(DRV_ETHMAC_SGL_LIST* pL)
{
    pL->head = pL->tail = 0;
    pL->nNodes = 0;
}



extern __inline__ int __attribute__((always_inline)) DRV_ETHMAC_SingleListCount(DRV_ETHMAC_SGL_LIST* pL)
{
    return pL->nNodes;
}

// removes the head node
DRV_ETHMAC_SGL_LIST_NODE*  DRV_ETHMAC_SingleListHeadRemove(DRV_ETHMAC_SGL_LIST* pL);

void  DRV_ETHMAC_SingleListTailAdd(DRV_ETHMAC_SGL_LIST* pL, DRV_ETHMAC_SGL_LIST_NODE* pN);

#endif //  __ETH_DCPT_LISTS_H_
