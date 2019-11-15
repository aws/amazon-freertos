/*******************************************************************************
  MAC Module (Microchip PIC32C) for Microchip TCP/IP Stack
*******************************************************************************/

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


#include "driver/gmac/src/dynamic/drv_gmac_lib.h"

/** D E F I N I T I O N S ****************************************************/
//PIC32C internal GMAC peripheral
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_PIC32C

// All pause capabilities our MAC supports
#define DRV_GMAC_PAUSE_CPBL_MASK     (TCPIP_ETH_PAUSE_TYPE_ALL)

/******************************************************************************
 * Prototypes
 ******************************************************************************/
//PHY related Init for GMAC
static SYS_MODULE_OBJ _DRV_GMAC_PHYInitialise(DRV_GMAC_DRIVER *pMACDrv);


#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void     _MacRxFreePacket( DRV_GMAC_DRIVER * pMACDrv);
static void     _MACCleanup(DRV_GMAC_DRIVER * pMACDrv );
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void     _MACDeinit(DRV_GMAC_DRIVER * pMACDrv );

static void             _MACTxAcknowledgeEth(DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx);
static TCPIP_MAC_RES    _MacTxPendingPackets(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx );
static void             _MacTxDiscardQueues(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes);

// MAC interface functions
SYS_MODULE_OBJ          DRV_GMAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
void                    DRV_GMAC_Deinitialize(SYS_MODULE_OBJ object);
void                    DRV_GMAC_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
SYS_STATUS              DRV_GMAC_Status( SYS_MODULE_OBJ object );
void                    DRV_GMAC_Tasks( SYS_MODULE_OBJ object );
DRV_HANDLE              DRV_GMAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
void                    DRV_GMAC_Close( DRV_HANDLE hMac );
bool                    DRV_GMAC_LinkCheck(DRV_HANDLE hMac);
TCPIP_MAC_RES           DRV_GMAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);
bool                    DRV_GMAC_PowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode);
TCPIP_MAC_RES           DRV_GMAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);
TCPIP_MAC_PACKET*       DRV_GMAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, const TCPIP_MAC_PACKET_RX_STAT** ppPktStat);
TCPIP_MAC_RES           DRV_GMAC_Process(DRV_HANDLE hMac);
TCPIP_MAC_RES           DRV_GMAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);
TCPIP_MAC_RES           DRV_GMAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);
TCPIP_MAC_RES           DRV_GMAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);
size_t                  DRV_GMAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);


bool                    DRV_GMAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);
bool                    DRV_GMAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents);
TCPIP_MAC_EVENT         DRV_GMAC_EventPendingGet(DRV_HANDLE hMac);

static TCPIP_MAC_RES    DRV_GMAC_EventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam);
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES    DRV_GMAC_EventDeInit(DRV_HANDLE hMac);
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

typedef void (*_DRV_GMAC_LinkStateF)(DRV_GMAC_DRIVER * pMACDrv);

//Ethernet Link Start up states
static void _DRV_GMAC_LinkStateStartLink(DRV_GMAC_DRIVER * pMACDrv);
static void _DRV_GMAC_LinkStateGetLink(DRV_GMAC_DRIVER * pMACDrv);
static void _DRV_GMAC_LinkStateWaitLinkUp(DRV_GMAC_DRIVER * pMACDrv);
static void _DRV_GMAC_LinkStateNegComplete(DRV_GMAC_DRIVER * pMACDrv);
static void _DRV_GMAC_LinkStateNegResult(DRV_GMAC_DRIVER * pMACDrv);

//GMAC Statistics Register access
static uint32_t _DRV_GMAC_GetTxOctetLow(void);
static uint32_t _DRV_GMAC_GetTxOctetHigh(void);
static uint32_t _DRV_GMAC_GetTxFrameCount(void);
static uint32_t _DRV_GMAC_GetTxBCastFrameCount(void);
static uint32_t _DRV_GMAC_GetTxMCastFrameCount(void);
static uint32_t _DRV_GMAC_GetTxPauseFrameCount(void);
static uint32_t _DRV_GMAC_GetTx64ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetTx127ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetTx255ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetTx511ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetTx1023ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetTx1518ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetTxGT1518ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetTxUnderRunFrameCount(void);
static uint32_t _DRV_GMAC_GetTxSingleCollFrameCount(void);
static uint32_t _DRV_GMAC_GetTxMultiCollFrameCount(void);
static uint32_t _DRV_GMAC_GetTxExcessCollFrameCount(void);
static uint32_t _DRV_GMAC_GetTxLateCollFrameCount(void);
static uint32_t _DRV_GMAC_GetTxDeferFrameCount(void);
static uint32_t _DRV_GMAC_GetTxCSErrorFrameCount(void);
static uint32_t _DRV_GMAC_GetRxOctetLow(void);
static uint32_t _DRV_GMAC_GetRxOctetHigh(void);
static uint32_t _DRV_GMAC_GetRxFrameCount(void);
static uint32_t _DRV_GMAC_GetRxBCastFrameCount(void);
static uint32_t _DRV_GMAC_GetRxMCastFrameCount(void);
static uint32_t _DRV_GMAC_GetRxPauseFrameCount(void);
static uint32_t _DRV_GMAC_GetRx64ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetRx127ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetRx255ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetRx511ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetRx1023ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetRx1518ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetRxGT1518ByteFrameCount(void);
static uint32_t _DRV_GMAC_GetRxUnderSizeFrameCount(void);
static uint32_t _DRV_GMAC_GetRxOverSizeFrameCount(void);
static uint32_t _DRV_GMAC_GetRxJabberFrameCount(void);
static uint32_t _DRV_GMAC_GetRxFCSErrorFrameCount(void);
static uint32_t _DRV_GMAC_GetRxLFErrorFrameCount(void);
static uint32_t _DRV_GMAC_GetRxSymErrorFrameCount(void);
static uint32_t _DRV_GMAC_GetRxAlignErrorFrameCount(void);
static uint32_t _DRV_GMAC_GetRxResErrorFrameCount(void);
static uint32_t _DRV_GMAC_GetRxOverRunFrameCount(void);
static uint32_t _DRV_GMAC_GetRxIPHdrCSErrorFrameCount(void);
static uint32_t _DRV_GMAC_GetRxTCPCSErrorFrameCount(void);
static uint32_t _DRV_GMAC_GetRxUDPCSErrorFrameCount(void);

//Transform TCPIP MAC filters to GMAC filters
static GMAC_RX_FILTERS _DRV_GMAC_MacToEthFilter(TCPIP_MAC_RX_FILTER_TYPE macFilter);

/******************************************************************************
 * PIC32C GMAC object implementation
 ******************************************************************************/
// the embedded PIC32 MAC object
/*static*/ const TCPIP_MAC_OBJECT DRV_GMAC_Object =  
{
    TCPIP_MODULE_MAC_PIC32C,
    "GMAC",   
	DRV_GMAC_Initialize,
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
    DRV_GMAC_Deinitialize,
    DRV_GMAC_Reinitialize, 
#else
    0,
    0,
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
    DRV_GMAC_Status,
    DRV_GMAC_Tasks,
    DRV_GMAC_Open,
    DRV_GMAC_Close,
    DRV_GMAC_LinkCheck,
    DRV_GMAC_RxFilterHashTableEntrySet,
    DRV_GMAC_PowerMode,
    DRV_GMAC_PacketTx,
    DRV_GMAC_PacketRx,
    DRV_GMAC_Process,
    DRV_GMAC_StatisticsGet,
    DRV_GMAC_ParametersGet,
    DRV_GMAC_RegisterStatisticsGet,
    DRV_GMAC_ConfigGet,
    DRV_GMAC_EventMaskSet,
    DRV_GMAC_EventAcknowledge,
    DRV_GMAC_EventPendingGet,
};

// the embedded PIC32 MAC descriptor
// to support multiple instances
// create an array/list of MAC_DCPT structures
// or allocate dynamically
static DRV_GMAC_DRIVER _gmac_drv_dcpt[1] = 
{
	{
		&DRV_GMAC_Object,
		{0},
	}		
    
};

//GMAC statistics register access 
static const DRV_PIC32CGMAC_HW_REG_DCPT macPIC32CHwRegDcpt[] =
{
	{"Tx Byte Count-lower 32bits(TxOTLO)",    _DRV_GMAC_GetTxOctetLow},
	{"Tx Byte Count-upper 16bits(TxOTHI)",    _DRV_GMAC_GetTxOctetHigh},
	{"Tx Frames w/o error(TxFTCNT)",   _DRV_GMAC_GetTxFrameCount},
	{"Tx Broadcast Frames(TxBCFT)",    _DRV_GMAC_GetTxBCastFrameCount},
	{"Tx Multicast Frames(TxMFT)",     _DRV_GMAC_GetTxMCastFrameCount},
	{"Tx Pause Frames(TxPFT)",     _DRV_GMAC_GetTxPauseFrameCount},
	{"Tx 64-byte Frames(TxBFT64)",   _DRV_GMAC_GetTx64ByteFrameCount},
	{"Tx 65-127 byte Frames(TBFT127)",   _DRV_GMAC_GetTx127ByteFrameCount},
	{"Tx 128-255 byte Frames(TBFT255)",   _DRV_GMAC_GetTx255ByteFrameCount},
	{"Tx 256-511 byte Frames(TBFT511)",   _DRV_GMAC_GetTx511ByteFrameCount},
	{"Tx 512-1023 byte Frames(TBFT1023)",  _DRV_GMAC_GetTx1023ByteFrameCount},
	{"Tx 1024-1518 byte Frames(TBFT1518)",  _DRV_GMAC_GetTx1518ByteFrameCount},
	{"Tx Frames >= 1518 byte(GTBF1518)",  _DRV_GMAC_GetTxGT1518ByteFrameCount},
	{"Tx Underrun Frames not TXed(TxTUR)",     _DRV_GMAC_GetTxUnderRunFrameCount},
	{"Tx Single Collision Frames(TxSCF)",     _DRV_GMAC_GetTxSingleCollFrameCount},
	{"Tx 2-15 Collision Frames(TxMCF)",     _DRV_GMAC_GetTxMultiCollFrameCount},	
	{"Tx 16 Collision Frames(TxECCNT)",   _DRV_GMAC_GetTxExcessCollFrameCount},
	{"Tx late Collision Frames(TxLCCNT)",   _DRV_GMAC_GetTxLateCollFrameCount},
	{"Tx Deferred Frames(TxDTF)",     _DRV_GMAC_GetTxDeferFrameCount},	
	{"Tx Carrier Sense error Frames(TxCSE)",     _DRV_GMAC_GetTxCSErrorFrameCount},		
	{"Rx Byte Count-lower 32bits(RxORLO)",    _DRV_GMAC_GetRxOctetLow},
	{"Rx Byte Count-upper 16bits(RxORHI)",    _DRV_GMAC_GetRxOctetHigh},
	{"Rx Frames w/o error(RxFRCNT)",    _DRV_GMAC_GetRxFrameCount},
	{"Rx Broadcast Frames(RxBCFR)",    _DRV_GMAC_GetRxBCastFrameCount},
	{"Rx Multicast Frames (RxMFR)",     _DRV_GMAC_GetRxMCastFrameCount},
	{"Rx Pause Frames(RxPFR)",     _DRV_GMAC_GetRxPauseFrameCount},
	{"Rx 64-byte Frames(RxBFR64)",   _DRV_GMAC_GetRx64ByteFrameCount},
	{"Rx 65-127 byte Frames(TBFR127)",   _DRV_GMAC_GetRx127ByteFrameCount},
	{"Rx 128-255 byte Frames(TBFR255)",   _DRV_GMAC_GetRx255ByteFrameCount},
	{"Rx 256-511 byte Frames(TBFR511)",   _DRV_GMAC_GetRx511ByteFrameCount},
	{"Rx 512-1023 byte Frames(TBFR1023)",  _DRV_GMAC_GetRx1023ByteFrameCount},
	{"Rx 1024-1518 byte Frames(TBFR1518)",  _DRV_GMAC_GetRx1518ByteFrameCount},
	{"Rx Frames >= 1519 bytes(TMXBFR)",    _DRV_GMAC_GetRxGT1518ByteFrameCount},
	{"Rx Frames < 64 bytes(RxUFR)",     _DRV_GMAC_GetRxUnderSizeFrameCount},
	{"Rx Frames > 1518 bytes(RxOFR)",     _DRV_GMAC_GetRxOverSizeFrameCount},
	{"Rx error Frames > 1518 bytes(RxJR)",      _DRV_GMAC_GetRxJabberFrameCount},
	{"Rx CRC error Frames(RxFCSE)",    _DRV_GMAC_GetRxFCSErrorFrameCount},
	{"Rx Length field error Frames(RxLFFE)",    _DRV_GMAC_GetRxLFErrorFrameCount},
	{"Rx symbol error Frames(RxRSE)",     _DRV_GMAC_GetRxSymErrorFrameCount},
	{"Rx alignment error Frames(RxAE)",      _DRV_GMAC_GetRxAlignErrorFrameCount},
	{"Rx Resource Error (RxRRE)",     _DRV_GMAC_GetRxResErrorFrameCount},		
	{"Rx overrun error Frames(RxROE)",     _DRV_GMAC_GetRxOverRunFrameCount},
	{"Rx IPHdr Chksum error Frames(RxIHCE)",    _DRV_GMAC_GetRxIPHdrCSErrorFrameCount},	
	{"Rx TCP Chksum error Frames(RxTCE)",     _DRV_GMAC_GetRxTCPCSErrorFrameCount},	
	{"Rx UDP Chksum error Frames(RxUCE)",     _DRV_GMAC_GetRxUDPCSErrorFrameCount},
};


//table of different Link startup
static const _DRV_GMAC_LinkStateF _DRV_GMAC_LinkStateTbl[] =
{
	_DRV_GMAC_LinkStateStartLink,     // DRV_PIC32CGMAC_LINK_CHECK_START_LINK
	_DRV_GMAC_LinkStateGetLink,       // DRV_PIC32CGMAC_LINK_CHECK_GET_LINK
	_DRV_GMAC_LinkStateWaitLinkUp,       // DRV_PIC32CGMAC_LINK_CHECK_WAIT_LINK_UP
	_DRV_GMAC_LinkStateNegComplete,   // DRV_PIC32CGMAC_LINK_CHECK_NEG_COMPLETE
	_DRV_GMAC_LinkStateNegResult,     // DRV_PIC32CGMAC_LINK_CHECK_NEG_RESULT
};

//convert mac id to index
static __inline__ int __attribute__((always_inline)) _GmacIdToIndex(TCPIP_MODULE_MAC_ID macId)
{    
	int macIx = macId - TCPIP_MODULE_MAC_PIC32C_0;

    if(macIx >= 0 && macIx < sizeof(_gmac_drv_dcpt)/sizeof(*_gmac_drv_dcpt))
    {
        return macIx;
    }

    return -1;
}



/*
 * interface functions
 *
*/

/****************************************************************************
 * Function:        DRV_GMAC_Initialize
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *                
 * Side Effects:    None
 *
 * Overview:        This function initializes the GMAC controller, the MAC and the associated PHY.
 *                  It should be called to be able to schedule any MAC transmit or receive operation.
 *
 * Note:            Only one client per MAC supported. 
 *****************************************************************************/
SYS_MODULE_OBJ DRV_GMAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
	DRV_GMAC_DRIVER * pMACDrv;	
	int				macIx;
	const TCPIP_MAC_MODULE_CTRL* const macControl = ((TCPIP_MAC_INIT*)init)->macControl;	
    const TCPIP_MODULE_MAC_PIC32C_CONFIG* initData = (const TCPIP_MODULE_MAC_PIC32C_CONFIG*)((TCPIP_MAC_INIT*)init)->moduleData;
	TCPIP_MAC_RES		initRes;	
	GMAC_QUE_LIST		queueIdx;	
	GMAC_RX_FILTERS		gmacRxFilt;
	
	macIx = _GmacIdToIndex(index);
	
	if(macIx < 0 )
	{   
		return SYS_MODULE_OBJ_INVALID;      // no such type supported
	}
   
    pMACDrv = _gmac_drv_dcpt + macIx;

	if(pMACDrv->sGmacData._macFlags._init != 0)
	{   // already initialized	
		return (SYS_MODULE_OBJ)pMACDrv;
	}
	    	
	if(pMACDrv->sGmacData._macFlags._open != 0)
	{
		return SYS_MODULE_OBJ_INVALID;     // have a client connected
	}

	if(macControl->memH == 0)
	{
		return SYS_MODULE_OBJ_INVALID;     // not possible without dynamic memory!
	}
	
	if(initData == 0)
	{
		return SYS_MODULE_OBJ_INVALID;     // not possible without init data!
	}

	if(initData->pPhyBase == 0 || initData->pPhyInit == 0)
	{
		return SYS_MODULE_OBJ_INVALID;     // not possible without a PHY!
	}
	
	// init the MAC object
    memset(&pMACDrv->sGmacData, 0x0, sizeof(pMACDrv->sGmacData)); 
    pMACDrv->sGmacData.hPhyClient = DRV_HANDLE_INVALID;
    pMACDrv->sGmacData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
    pMACDrv->sGmacData.sysStat = SYS_STATUS_UNINITIALIZED;
	
    pMACDrv->sGmacData._macIx = macIx;
    pMACDrv->sGmacData._phyIx = macIx;    // use the same index for the associated PHY
    pMACDrv->sGmacData._macFlags._linkPrev = 0;	

    pMACDrv->sGmacData._macIntSrc = DRV_GMAC_INTERRUPT_SOURCE;// interrupt source for Queue 0; need to implement for queue 1 and 2
	
    // use initialization data
    pMACDrv->sGmacData._AllocH = macControl->memH;
    pMACDrv->sGmacData._callocF = macControl->callocF;
    pMACDrv->sGmacData._freeF = macControl->freeF;


    pMACDrv->sGmacData.pktAllocF = macControl->pktAllocF;
    pMACDrv->sGmacData.pktFreeF = macControl->pktFreeF;
    pMACDrv->sGmacData.pktAckF = macControl->pktAckF;

    pMACDrv->sGmacData._synchF = macControl->synchF;
	
	// copy the configuration data
	pMACDrv->sGmacData.gmacConfig = *initData;	
	
    DRV_GMAC_LibDescriptorsPoolAdd (pMACDrv, DRV_GMAC_DCPT_TYPE_RX);
    DRV_GMAC_LibDescriptorsPoolAdd (pMACDrv, DRV_GMAC_DCPT_TYPE_TX);
    
	for(queueIdx = GMAC_QUE_0; queueIdx < DRV_GMAC_NUMBER_OF_QUEUES; queueIdx++)
	{
		if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt < DRV_GMAC_MIN_TX_DESCRIPTORS)
		{
			return SYS_MODULE_OBJ_INVALID; //return invalid than setting min dscp count
		}
		if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize < DRV_GMAC_MIN_RX_SIZE)
		{
			return SYS_MODULE_OBJ_INVALID; //return invalid than setting min Rx buffer size
		}
	}
	
	//set the MAC address received from system config
	memcpy((pMACDrv->sGmacData.gmacConfig.macAddress.v), (macControl->ifPhyAddress.v),sizeof(macControl->ifPhyAddress));	

	
    if(!_DRV_GMAC_RxCreate(pMACDrv))
    {
        return SYS_MODULE_OBJ_INVALID;     // failed to create synch lock
    }
    if(!_DRV_GMAC_TxCreate(pMACDrv))
    {
        _DRV_GMAC_RxDelete(pMACDrv);
        return SYS_MODULE_OBJ_INVALID;     // failed to create synch lock
    }
    
    initRes = TCPIP_MAC_RES_OK;
   
	while(1)
	{
		uint32_t rxfilter= 0;

		// start the initialization sequence
		SYS_INT_SourceDisable(pMACDrv->sGmacData._macIntSrc);      // stop Eth ints
			
 		initRes = _DRV_GMAC_PHYInitialise(pMACDrv);
 		if(initRes != TCPIP_MAC_RES_OK)
 		{
			// some error occurred
			initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
	 		break;
 		}
		
		//global configurations for gmac
		DRV_PIC32CGMAC_LibInit(pMACDrv);
				        
        //Receive All Multi-cast packets? then set 64-bit hash value to all ones.
        if((TCPIP_GMAC_RX_FILTERS) & TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT)
        {
            DRV_GMAC_HASH hash; 
            
            hash.hash_value = 0xffffffffffffffff; //Set 64-bit Hash value to all 1s, to receive all multi-cast
            hash.calculate_hash = false; // No hash calculation; directly set hash register
            
            DRV_PIC32CGMAC_LibRxFilterHash_Calculate(pMACDrv, &hash);
        }
        // Set Rx Filters 
        gmacRxFilt = _DRV_GMAC_MacToEthFilter(TCPIP_GMAC_RX_FILTERS);
		rxfilter = (uint32_t)(GMAC_REGS->GMAC_NCFGR) & (~GMAC_FILT_ALL_FILTERS); 
		GMAC_REGS->GMAC_NCFGR  = (rxfilter|gmacRxFilt) ;

		if(DRV_PIC32CGMAC_LibRxInit(pMACDrv) != DRV_PIC32CGMAC_RES_OK)
		{			
			initRes = TCPIP_MAC_RES_INIT_FAIL;
			break;		
		}
		
		if(DRV_PIC32CGMAC_LibTxInit(pMACDrv) != DRV_PIC32CGMAC_RES_OK)
		{
			initRes = TCPIP_MAC_RES_INIT_FAIL;
			break;
		}
		
        for(queueIdx = GMAC_QUE_0; queueIdx < DRV_GMAC_NUMBER_OF_QUEUES; queueIdx++)
        {
            //Initialize QUEUES				
            if(DRV_PIC32CGMAC_LibInitTransfer(pMACDrv,queueIdx) != DRV_PIC32CGMAC_RES_OK)
            {
                initRes = TCPIP_MAC_RES_INIT_FAIL;
                break;
            }
        }
		
		SYS_INT_SourceStatusClear(pMACDrv->sGmacData._macIntSrc); // clear any pending interrupt flag
		SYS_INT_SourceEnable(pMACDrv->sGmacData._macIntSrc); // enable interrupt
		
		DRV_PIC32CGMAC_LibTransferEnable(pMACDrv); //enable Transmit and Receive of GMAC
		
        if(DRV_GMAC_EventInit((DRV_HANDLE)pMACDrv, macControl->eventF, macControl->eventParam) != TCPIP_MAC_RES_OK)
		{
			initRes = TCPIP_MAC_RES_EVENT_INIT_FAIL;
			break;
		}
		// end of initialization
		break;
		
	}
	
	if(initRes != TCPIP_MAC_RES_OK)
	{   // failed; release what we've allocated
		_MACDeinit(pMACDrv);
		return SYS_MODULE_OBJ_INVALID;
	}
	
	// success
	// remaining initialization is done by DRV_ETHMAC_PIC32MACTasks	 	
	pMACDrv->sGmacData._macFlags._init = 1;	
	pMACDrv->sGmacData.sysStat = SYS_STATUS_BUSY;
    
    //initialize unimplemented Receive status to -1
    pMACDrv->sGmacData._rxStat.nRxErrorPackets = -1;
    pMACDrv->sGmacData._rxStat.nRxFragmentErrors = -1;
    pMACDrv->sGmacData._rxStat.nRxPendBuffers = -1;
    pMACDrv->sGmacData._rxStat.nRxSchedBuffers = -1;
    
	return (SYS_MODULE_OBJ)pMACDrv;
	
}

/****************************************************************************
 * Function:        DRV_GMAC_Deinitialize
 *
 * PreCondition:    None
 *
 * Input:           stackData - standard stack initialization structure
 *
 * Output:          TCPIP_MAC_RES
 *                
 * Side Effects:    None
 *
 * Overview:        This function de-initializes the Eth MAC controller.
 *
 * Note:            None 
 *****************************************************************************/
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
void DRV_GMAC_Deinitialize(SYS_MODULE_OBJ object)
{
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER *)object;
	
	if(pMACDrv->sGmacData._macFlags._init != 0)
	{
		_MACDeinit(pMACDrv);
	}

}

void DRV_GMAC_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init)
{
    // not supported
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

SYS_STATUS DRV_GMAC_Status (SYS_MODULE_OBJ object)
{
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER *)object;
	if(pMACDrv->sGmacData._macFlags._init != 0)
	{
		return pMACDrv->sGmacData.sysStat;
	}
	return SYS_STATUS_ERROR;
}

void DRV_GMAC_Tasks(SYS_MODULE_OBJ object)
{
	TCPIP_ETH_PAUSE_TYPE        pauseType;
	DRV_ETHPHY_CLIENT_STATUS    phyStat;
	DRV_HANDLE                  hPhyClient;
	DRV_ETHPHY_RESULT           phyInitRes;
	const DRV_ETHPHY_OBJECT_BASE* pPhyBase;
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER *)object;
	static bool discard_flag = false;
    
    if( (pMACDrv->sGmacData._negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) == 0 )
	{
        if(discard_flag == true)
        {            
            _MacTxDiscardQueues(pMACDrv,0, TCPIP_MAC_PKT_ACK_LINK_DOWN);
            discard_flag = false;
        }
    }
    else
    {
        discard_flag = true;
    }
    
    
	if(pMACDrv->sGmacData._macFlags._init == 0)
	{   // nothing to do
		return;
	}
	
	pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
	
	pPhyBase->DRV_ETHPHY_Tasks(pMACDrv->sGmacData.hPhySysObject);
	
	switch(pMACDrv->sGmacData.sysStat)
	{
		case SYS_STATUS_BUSY:
		// perform the PHY initialization

		hPhyClient = pMACDrv->sGmacData.hPhyClient;
		phyStat = pPhyBase->DRV_ETHPHY_ClientStatus(hPhyClient);
		if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
		{   // wait some more
			return;
		}

		phyInitRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(hPhyClient);

		if ( phyInitRes != DRV_ETHPHY_RES_OK)
		{
			_MACDeinit(pMACDrv);
			// keep the error status though
			pMACDrv->sGmacData._macFlags._init = 1;
			pMACDrv->sGmacData.sysStat = SYS_STATUS_ERROR;
			SYS_ERROR_PRINT(SYS_ERROR_ERROR, "DRV PHY init failed: %d\r\n", phyInitRes);
			break;
		}

		// PHY was detected properly
		pMACDrv->sGmacData._macFlags._linkPresent = 1;
		if((pMACDrv->sGmacData.gmacConfig.ethFlags & TCPIP_ETH_OPEN_AUTO) != 0)
		{	// we'll just wait for the negotiation to be done
			pMACDrv->sGmacData._macFlags._linkNegotiation = 1;	// performing the negotiation
		}
		else
		{	// no need of negotiation results; just update the MAC
			pauseType = (pMACDrv->sGmacData.gmacConfig.ethFlags & TCPIP_ETH_OPEN_FDUPLEX) ? DRV_GMAC_PAUSE_CPBL_MASK : TCPIP_ETH_PAUSE_TYPE_NONE;
			DRV_PIC32CGMAC_LibMACOpen(pMACDrv, pMACDrv->sGmacData._linkResFlags, pauseType);
		}

		pMACDrv->sGmacData._linkUpTick = SYS_TMR_TickCountGet();     // the initialization time
		pMACDrv->sGmacData._linkWaitTick = ((SYS_TMR_TickCounterFrequencyGet() * pMACDrv->sGmacData.gmacConfig.linkInitDelay) + 999 ) / 1000;
		pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;

		pMACDrv->sGmacData.sysStat = SYS_STATUS_READY;
		break;

		default:    // SYS_STATUS_READY
		break;
	}

}


size_t DRV_GMAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize) 
{	
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	
	if(pConfigSize)
	{
		*pConfigSize =  sizeof(TCPIP_MODULE_MAC_PIC32C_CONFIG);
	}

	if(configBuff && buffSize >= sizeof(TCPIP_MODULE_MAC_PIC32C_CONFIG))
	{   // can copy the data
		TCPIP_MODULE_MAC_PIC32C_CONFIG* pMacConfig = (TCPIP_MODULE_MAC_PIC32C_CONFIG*)configBuff;

		*pMacConfig = pMACDrv->sGmacData.gmacConfig;
		return sizeof(TCPIP_MODULE_MAC_PIC32C_CONFIG);
	}
    return 0;

}

DRV_HANDLE DRV_GMAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent) 
{
    int                 macIx;
    DRV_GMAC_DRIVER * pMACDrv;
    DRV_HANDLE    hMac = DRV_HANDLE_INVALID;
	
	macIx = _GmacIdToIndex(drvIndex);
	
	if(macIx >= 0 )
	{
		pMACDrv = _gmac_drv_dcpt + macIx;
		if(pMACDrv->sGmacData._macFlags._init == 1)
		{
			if(pMACDrv->sGmacData._macFlags._open == 0)
			{   // only one client for now
				pMACDrv->sGmacData._macFlags._open = 1;
				hMac = (DRV_HANDLE)pMACDrv;
			}
#if (DRV_GMAC_CLIENTS_NUMBER > 1)
			else
			{   // allow multiple clients
				hMac = (DRV_HANDLE)pMACDrv;
			}
#endif
		}
	}

    return hMac;
}


void DRV_GMAC_Close( DRV_HANDLE hMac ) 
{
	DRV_GMAC_DRIVER * pMACDrv;
	pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	
	if(pMACDrv->sGmacData._macFlags._init == 1)	
	{
		pMACDrv->sGmacData._macFlags._open = 0;
	}

}

/**************************
 * TX functions
 ***********************************************/

TCPIP_MAC_RES DRV_GMAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket)  
{
	TCPIP_MAC_RES       macRes = TCPIP_MAC_RES_PACKET_ERR;
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	DRV_PIC32CGMAC_SGL_LIST_NODE*   pTxQueueNode;
	GMAC_QUE_LIST queueIdx = GMAC_QUE_0;	
	
	_DRV_GMAC_TxLock(pMACDrv);
    
	//new packet for transmission
	while(ptrPacket)
	{
		//valid packet data
		if (ptrPacket->pDSeg != NULL)
		{		
			if((pTxQueueNode = DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxNewQueue))!= NULL)
			{						
				pTxQueueNode->data = (TCPIP_MAC_PACKET*) ptrPacket;
				DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxStartQueue, (DRV_PIC32CGMAC_SGL_LIST_NODE*)pTxQueueNode);
				
				ptrPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
				ptrPacket = ptrPacket->next;
				macRes = TCPIP_MAC_RES_OK;
			}
			else
			{
				_DRV_GMAC_TxUnlock(pMACDrv);
				return TCPIP_MAC_RES_QUEUE_TX_FULL; //return error
			}			
		}
		else
		{
			_DRV_GMAC_TxUnlock(pMACDrv);
			return TCPIP_MAC_RES_PACKET_ERR; //return error			
		}
	}
	
	_MacTxPendingPackets(pMACDrv,queueIdx);
	_MACTxAcknowledgeEth(pMACDrv,queueIdx);	

	_DRV_GMAC_TxUnlock(pMACDrv);

	return macRes;
}



/***********************************************
 * RX functions
 ***********************************************/

// returns a pending RX packet if exists
TCPIP_MAC_PACKET* DRV_GMAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, const TCPIP_MAC_PACKET_RX_STAT** ppPktStat)  
{
	DRV_PIC32CGMAC_RESULT	ethRes;	
	TCPIP_MAC_RES			mRes;	
	TCPIP_MAC_PACKET		*pRxPkt;

	DRV_GMAC_RXDCPT_STATUS	pRxPktStat;
	int                     buffsPerRxPkt = 0;
	DRV_GMAC_DRIVER			*pMACDrv = (DRV_GMAC_DRIVER*)hMac;
    
	_DRV_GMAC_RxLock(pMACDrv);	
	//get Rx packet from Queue 0
	ethRes = DRV_PIC32CGMAC_LibRxGetPacket (pMACDrv, &pRxPkt, &buffsPerRxPkt, &pRxPktStat, GMAC_QUE_0);
	_DRV_GMAC_RxUnlock(pMACDrv);

	if(buffsPerRxPkt > 1)
    {
        pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_SPLIT;
    } 
	if(ethRes == DRV_PIC32CGMAC_RES_PACKET_QUEUED || ethRes == DRV_PIC32CGMAC_RES_NO_PACKET)
	{   // done, no more packets
		mRes = TCPIP_MAC_RES_PENDING;
	}
	else if(ethRes == DRV_PIC32CGMAC_RES_OK)
	{   
		mRes = TCPIP_MAC_RES_OK;
		pMACDrv->sGmacData._rxStat.nRxOkPackets++;		
	}
	else
	{   
		mRes = TCPIP_MAC_RES_PACKET_ERR;
		pMACDrv->sGmacData._rxStat.nRxErrorPackets++;
	}

	if(pRes)
	{
		*pRes = mRes;
	}
	if(ppPktStat)
	{
		*ppPktStat = 0;
	}
	
	if(mRes == TCPIP_MAC_RES_OK)
	{   // valid ethernet packet;
		// format properly and return the packet pointer
		// restore packet the buffer belongs to
        
        //Ethernet MAC header(14 bytes) stored in data packet (or in first data buffer for multi-buffer data).
        //Reduce header length to get data segment length
        pRxPkt->pDSeg->segLen = pRxPkt->pDSeg->segLen - sizeof(TCPIP_MAC_ETHERNET_HEADER);
		// Note: re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet!
		pRxPkt->pMacLayer = pRxPkt->pDSeg->segLoad;
		pRxPkt->pNetLayer = pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);

		pRxPkt->tStamp = SYS_TMR_TickCountGet();
		pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;


		pRxPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_CAST_MASK;
		if(pRxPktStat.bm.bBroadcastDetected)
		{
			pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_BCAST;
		}
		else if(pRxPktStat.bm.bMultiHashMatch)
		{
			pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
		}
		else if(pRxPktStat.bm.bUniHashMatch)
		{
			pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_UNICAST;
		}

		if(ppPktStat)
		{					
			*ppPktStat = (TCPIP_MAC_PACKET_RX_STAT*)&pRxPktStat;
		}

		// success
		return pRxPkt;
	}
	
    return 0;

}


// verify the link status

bool DRV_GMAC_LinkCheck(DRV_HANDLE hMac) 
{
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	
    if(pMACDrv->sGmacData._macFlags._linkPresent == 0)
    {
	    return false;
    }

    (*_DRV_GMAC_LinkStateTbl[pMACDrv->sGmacData._linkCheckState])(pMACDrv);

    return pMACDrv->sGmacData._macFlags._linkPrev == 1;	

}

static void _DRV_GMAC_LinkStateDown(DRV_GMAC_DRIVER * pMACDrv) 
{
	// link is down; reset state flags, initialization time
	pMACDrv->sGmacData._macFlags._linkPrev = 0;
	pMACDrv->sGmacData._macFlags._linkUpDone = 0;
	pMACDrv->sGmacData._linkUpTick = SYS_TMR_TickCountGet();
	pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;
}

static void _DRV_GMAC_LinkStateStartLink(DRV_GMAC_DRIVER * pMACDrv) 
{
    DRV_ETHPHY_RESULT phyRes;    
   
	phyRes = pMACDrv->sGmacData.gmacConfig.pPhyBase->DRV_ETHPHY_LinkStatusGet(pMACDrv->sGmacData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &pMACDrv->sGmacData._negResult.linkStatus, false);
    if(phyRes < 0)
    {   // some error occurred
	    _DRV_GMAC_LinkStateDown(pMACDrv);
	    return;
    }

    pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_GET_LINK;
}

static void _DRV_GMAC_LinkStateGetLink(DRV_GMAC_DRIVER * pMACDrv) 
{
	DRV_ETHPHY_CLIENT_STATUS phyStat;
	DRV_ETHPHY_RESULT phyRes;
	const DRV_ETHPHY_OBJECT_BASE* pPhyBase;
	bool linkCurrUp;

	
	pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
	phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMACDrv->sGmacData.hPhyClient );
	if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
	{   // not done yet
		return;
	}

	phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMACDrv->sGmacData.hPhyClient);
	if(phyRes != DRV_ETHPHY_RES_OK)
	{   // some error occurred
		_DRV_GMAC_LinkStateDown(pMACDrv);
		return;
	}

	linkCurrUp = (pMACDrv->sGmacData._negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) != 0;

	if(linkCurrUp == false)
	{
		_DRV_GMAC_LinkStateDown(pMACDrv);
		return;
	}

	// link up
	if( (bool) pMACDrv->sGmacData._macFlags._linkPrev == linkCurrUp )
	{   // PHY state not changed
		pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;
	}
	else
	{
		pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_WAIT_LINK_UP;
	}
	
}

// ok, the link is coming up
static void _DRV_GMAC_LinkStateWaitLinkUp(DRV_GMAC_DRIVER * pMACDrv) 
{
	DRV_ETHPHY_RESULT   phyRes;

	if(pMACDrv->sGmacData._macFlags._linkUpDone == 0)
	{   // wait the init tmo before checking the link up
		uint32_t currTick = SYS_TMR_TickCountGet();
		if(currTick - pMACDrv->sGmacData._linkUpTick < pMACDrv->sGmacData._linkWaitTick)
		{
			return;
		}
		pMACDrv->sGmacData._macFlags._linkUpDone = 1;
	}

	// initialization delay passed
	if(pMACDrv->sGmacData._macFlags._linkNegotiation == 0)
	{   // no auto negotiation taking place; this is the link status
		pMACDrv->sGmacData._macFlags._linkPrev = true;
		pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;
		return;
	}

	// auto negotiation is turned on
	// we have to re-negotiate but the link is actually up only
	// after the re-negotiation is done
	// wait for negotiation complete
	phyRes = pMACDrv->sGmacData.gmacConfig.pPhyBase->DRV_ETHPHY_NegotiationIsComplete(pMACDrv->sGmacData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, false);
	if(phyRes < 0)
	{   // some error occurred
		_DRV_GMAC_LinkStateDown(pMACDrv);
	}
	else
	{   // advance
		pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_NEG_COMPLETE;
	}
   
}

static void _DRV_GMAC_LinkStateNegComplete(DRV_GMAC_DRIVER * pMACDrv) 
{
	DRV_ETHPHY_CLIENT_STATUS phyStat;
	DRV_ETHPHY_RESULT phyRes;
	const DRV_ETHPHY_OBJECT_BASE* pPhyBase;

	pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
	phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMACDrv->sGmacData.hPhyClient );
	if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
	{   // not done yet
		return;
	}

	phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMACDrv->sGmacData.hPhyClient);
	if(phyRes != DRV_ETHPHY_RES_OK)
	{   // some error occurred
		_DRV_GMAC_LinkStateDown(pMACDrv);
		return;
	}

	phyRes = pPhyBase->DRV_ETHPHY_NegotiationResultGet(pMACDrv->sGmacData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &pMACDrv->sGmacData._negResult);

	if(phyRes < 0)
	{   // some error occurred
		_DRV_GMAC_LinkStateDown(pMACDrv);
	}
	else
	{   // advance; get negotiation result
		pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_NEG_RESULT;
	}
   
}


static void _DRV_GMAC_LinkStateNegResult(DRV_GMAC_DRIVER * pMACDrv)  
{
	DRV_ETHPHY_CLIENT_STATUS phyStat;
	DRV_ETHPHY_RESULT phyRes;
	const DRV_ETHPHY_OBJECT_BASE* pPhyBase;
	TCPIP_ETH_PAUSE_TYPE        pauseType;
	DRV_ETHPHY_CONFIG_FLAGS phyCfgFlags = 0;

	pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;
	phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMACDrv->sGmacData.hPhyClient );
	if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
	{   // not done yet
		return;
	}

	phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMACDrv->sGmacData.hPhyClient);
	if(phyRes != DRV_ETHPHY_RES_OK)
	{   // some error occurred
		_DRV_GMAC_LinkStateDown(pMACDrv);
		return;
	}

	
	if( (pMACDrv->sGmacData._negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) != 0 )
	{   // negotiation succeeded; properly update the MAC
		pPhyBase->DRV_ETHPHY_HWConfigFlagsGet(pMACDrv->sGmacData.hPhyClient, &phyCfgFlags);
		pMACDrv->sGmacData._negResult.linkFlags |= (phyCfgFlags & DRV_ETHPHY_CFG_RMII) ? TCPIP_ETH_OPEN_RMII : TCPIP_ETH_OPEN_MII ;
		pauseType = pMACDrv->sGmacData._negResult.pauseType;
		DRV_PIC32CGMAC_LibMACOpen(pMACDrv, pMACDrv->sGmacData._negResult.linkFlags, pauseType);
        
		pMACDrv->sGmacData._macFlags._linkPrev = 1;
		pMACDrv->sGmacData._linkCheckState = DRV_GMAC_LINK_CHECK_START_LINK;
	}
	else
	{
		_DRV_GMAC_LinkStateDown(pMACDrv);
	}
    
}

/******************************************************************************
 * Function:        TCPIP_MAC_RES DRV_GMAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, TCPIP_MAC_ADDR* DestMACAddr)
 *
 * PreCondition:    DRV_GMAC_Initialize() should have been called.
 *
 * Input:           DestMACAddr: 6 byte group destination MAC address to allow 
 *                  through the Hash Table Filter.  If DestMACAddr 
 *                  is set to 00-00-00-00-00-00, then the hash 
 *                  table will be cleared of all entries and the 
 *                  filter will be disabled.
 *
 * Output:          Sets the appropriate bit in the GMAC_HRB/GMAC_HRT registers to allow 
 *                  packets sent to DestMACAddr to be received and enabled the 
 *                  Hash Table receive filter (if not already).
 *
 *                  Returns TCPIP_MAC_RES_OK if success,
 *                  an eror code otherwise
 *
 * Side Effects:    None
 *
 *
 * Note:            There is no way to individually unset destination MAC 
 *                  addresses from the hash table since it is possible to have 
 *                  a hash collision and therefore multiple MAC addresses 
 *                  relying on the same hash table bit.  The stack would have 
 *                  to individually store each 6 byte MAC address to support 
 *                  this feature, which would waste a lot of RAM and be 
 *                  unnecessary in most applications.  As a simple compromise, 
 *                  you can call DRV_GMAC_RxFilterHashTableEntrySet() using a 
 *                  00-00-00-00-00-00 destination MAC address, which will clear 
 *                  the entire hash table and disable the hash table filter.  
 *                  This will allow you to then read the necessary destination 
 *                  addresses.
 *****************************************************************************/
TCPIP_MAC_RES DRV_GMAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr) 
{ 

    
    DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
    DRV_GMAC_HASH hash;
    uint8_t nullMACAddr[6] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    // Clear the Hash Table bits and disable the Hash Table Filter if a special 
    // 00-00-00-00-00-00 destination MAC address is provided.
    if( DestMACAddr == 0 || memcmp(DestMACAddr->v, nullMACAddr, sizeof(nullMACAddr))==0 )
    {
        hash.hash_value = 0x0; //Clear to all 0 for 64-bit Hash value to reject all hash match
        hash.calculate_hash = false; // No hash calculation; directly set hash register
    }
    else
    {
        hash.DestMACAddr = DestMACAddr;    
        hash.calculate_hash = true; // calculate hash and set hash register
    }
    
    DRV_PIC32CGMAC_LibRxFilterHash_Calculate(pMACDrv, &hash);
      
    return TCPIP_MAC_RES_OK;
}

bool DRV_GMAC_PowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode) 
{   // not supported
    return true;
}


TCPIP_MAC_RES DRV_GMAC_Process(DRV_HANDLE hMac)  
{
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	GMAC_QUE_LIST queueIdx = GMAC_QUE_0; //currently, only checking Queue 0
	TCPIP_MAC_RES macRes;
    
	_DRV_GMAC_TxLock(pMACDrv);   

	_MACTxAcknowledgeEth(pMACDrv,queueIdx);  

	macRes = _MacTxPendingPackets(pMACDrv,queueIdx);   

	_DRV_GMAC_TxUnlock(pMACDrv);

	return macRes;

}

TCPIP_MAC_RES DRV_GMAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics) 
{
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
    
	if(pRxStatistics)
	{
		*pRxStatistics = pMACDrv->sGmacData._rxStat;
	}
	if(pTxStatistics)
	{
		_DRV_GMAC_TxLock(pMACDrv);
		pMACDrv->sGmacData._txStat.nTxPendBuffers = 0; //clear the previous pending status
		for(uint16_t index = 0; index < DRV_GMAC_NUMBER_OF_QUEUES; index++)
		{
			pMACDrv->sGmacData._txStat.nTxPendBuffers += DRV_PIC32CGMAC_SingleListCount(&(pMACDrv->sGmacData.gmac_queue[index]._TxAckQueue));
			
		}		
		_DRV_GMAC_TxUnlock(pMACDrv);
		*pTxStatistics = pMACDrv->sGmacData._txStat;
	}

	return TCPIP_MAC_RES_OK;

}

TCPIP_MAC_RES DRV_GMAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries)  
{
	const DRV_PIC32CGMAC_HW_REG_DCPT*   pHwRegDcpt;
	
	int nHwEntries = sizeof(macPIC32CHwRegDcpt)/sizeof(*macPIC32CHwRegDcpt);

	if(pHwEntries)
	{
		*pHwEntries = nHwEntries;
	}

	if(pRegEntries)
	{   // fill data
		unsigned int ix, regLim;

		regLim = nEntries > nHwEntries ? nHwEntries : nEntries;
		pHwRegDcpt = macPIC32CHwRegDcpt;
		for(ix = 0; ix < regLim; ix++, pRegEntries++, pHwRegDcpt++)
		{
			strncpy(pRegEntries->registerName, pHwRegDcpt->regName, sizeof(pRegEntries->registerName));
			pRegEntries->registerValue = (*pHwRegDcpt->regFunc)();
		}
	}

	return TCPIP_MAC_RES_OK;

}

TCPIP_MAC_RES DRV_GMAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams)
{
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	if(pMACDrv->sGmacData.sysStat == SYS_STATUS_READY)
	{
		if(pMacParams)
		{		
            DRV_PIC32CGMAC_LibGetMacAddr(pMacParams->ifPhyAddress.v);
			
			pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAG_RX | TCPIP_MAC_PROCESS_FLAG_TX);
			pMacParams->macType = TCPIP_MAC_TYPE_ETH;
			pMacParams->linkMtu = TCPIP_MAC_LINK_MTU_ETH;
		}

		return TCPIP_MAC_RES_OK;
	}

	return TCPIP_MAC_RES_IS_BUSY;
    
}

/**********************************************
 * local functions and helpers
 ***********************************************/

// acknowledge the ETHC packets
static void _MACTxAcknowledgeEth(DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx)  
{
	//Clear the TXCOMP transmit status, if set
	GMAC_REGS->GMAC_TSR = GMAC_TSR_TXCOMP_Msk;
	DRV_PIC32CGMAC_LibTxAckPacket(pMACDrv,queueIdx);
}


// transmits pending packets, if any
// if the link is down the TX queued packets are discarded
// returns:
//      - TCPIP_MAC_RES_OK if the queue is empty and another packet can be scheduled for TX
//      - TCPIP_MAC_RES_PENDING if MAC descriptor queue is full and no packet can be scheduled for TX

static TCPIP_MAC_RES _MacTxPendingPackets(DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx ) 
{
	TCPIP_MAC_PACKET* pPkt = (TCPIP_MAC_PACKET*)(pMACDrv->sGmacData.gmac_queue[queueIdx]._TxStartQueue.head->data); 
	TCPIP_MAC_RES     pktRes;
	
	DRV_PIC32CGMAC_RESULT ethRes = DRV_PIC32CGMAC_RES_NO_PACKET;
	

    //packet in queue for transmission
	while(((pMACDrv->sGmacData.gmac_queue[queueIdx]._TxStartQueue.nNodes) != 0) && (ethRes != DRV_PIC32CGMAC_RES_NO_DESCRIPTORS))
	{
		ethRes = DRV_PIC32CGMAC_LibTxSendPacket(pMACDrv, queueIdx);
        
	}
		
	if((ethRes == DRV_PIC32CGMAC_RES_OK)||(ethRes == DRV_PIC32CGMAC_RES_NO_PACKET))
	{
		pktRes =  TCPIP_MAC_RES_OK;
	}
	else if(ethRes == DRV_PIC32CGMAC_RES_NO_DESCRIPTORS)
	{   // not enough room in the hw queue
		pktRes =  TCPIP_MAC_RES_PENDING;
		pMACDrv->sGmacData._txStat.nTxQueueFull++;
	}
	else
		pktRes = TCPIP_MAC_RES_PACKET_ERR; 
		
		
	if(pktRes == TCPIP_MAC_RES_PACKET_ERR)
	{   // not transmitted	
		pMACDrv->sGmacData._txStat.nTxErrorPackets++;
		(*pMACDrv->sGmacData.pktAckF)(pPkt, TCPIP_MAC_PKT_ACK_BUFFER_ERR,TCPIP_THIS_MODULE_ID);		
	}

	return pktRes;

}


#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static void _MACDeinit(DRV_GMAC_DRIVER * pMACDrv )
{
	 SYS_INT_SourceDisable(pMACDrv->sGmacData._macIntSrc);      // stop Eth ints
	 DRV_PIC32CGMAC_LibClose(pMACDrv, DRV_PIC32CGMAC_CLOSE_DEFAULT);
	 SYS_INT_SourceStatusClear(pMACDrv->sGmacData._macIntSrc); // clear any pending interrupt flag
	 DRV_GMAC_EventDeInit((DRV_HANDLE)pMACDrv);
	 
	 const DRV_ETHPHY_OBJECT_BASE* pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase;

	 if(pMACDrv->sGmacData.hPhyClient != DRV_HANDLE_INVALID)
	 {
		 pPhyBase->DRV_ETHPHY_Close(pMACDrv->sGmacData.hPhyClient);
		 pMACDrv->sGmacData.hPhyClient = DRV_HANDLE_INVALID;
	 }
	 if(pMACDrv->sGmacData.hPhySysObject != SYS_MODULE_OBJ_INVALID)
	 {
		 pPhyBase->DRV_ETHPHY_Deinitialize(pMACDrv->sGmacData.hPhySysObject);
		 pMACDrv->sGmacData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
		 
	 }
	 
	_MACCleanup(pMACDrv);
	pMACDrv->sGmacData.sysStat = SYS_STATUS_UNINITIALIZED;
	pMACDrv->sGmacData._macFlags.val = 0;

}

static void _MACCleanup(DRV_GMAC_DRIVER * pMACDrv )
{
    GMAC_QUE_LIST queueIdx;
	// TX clean up
    for(queueIdx = GMAC_QUE_0; queueIdx < DRV_GMAC_NUMBER_OF_QUEUES; queueIdx++)
	{
        _MacTxDiscardQueues(pMACDrv,queueIdx, TCPIP_MAC_PKT_ACK_NET_DOWN);
    }
	// RX clean up
	_MacRxFreePacket(pMACDrv);

	_DRV_GMAC_RxDelete(pMACDrv);
	_DRV_GMAC_TxDelete(pMACDrv);

}

static void _MacRxFreePacket( DRV_GMAC_DRIVER * pMACDrv)
{
	DRV_PIC32CGMAC_SGL_LIST_NODE*   pRxBuffQueueNode;
    TCPIP_MAC_PACKET* pRxPkt;
    GMAC_QUE_LIST queueIdx;
	uint8_t index = 0;
    
	//free all the Rx packets linked to New and Ack Queues.
	for(queueIdx = GMAC_QUE_0; queueIdx < DRV_GMAC_NUMBER_OF_QUEUES; queueIdx++)
	{	
        //Free all Rx packets and queue nodes in Ack Queue
        while((pRxBuffQueueNode = DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._RxBuffAckQueue))!= NULL)
        {
            // Get Rx Packet in the queue node
            pRxPkt = (TCPIP_MAC_PACKET*)(pRxBuffQueueNode->data);
            // Free the Rx packet
            (*pMACDrv->sGmacData.pktFreeF)(pRxPkt);            
            //Free the queue node
            (*pMACDrv->sGmacData._freeF)(pMACDrv->sGmacData._AllocH, pRxBuffQueueNode);
        }
        
        //Free all Rx packet and queue node in New Queue
        while((pRxBuffQueueNode = DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._RxBuffNewQueue))!= NULL)
        {
            // Get Rx Packet in the queue node
            pRxPkt = (TCPIP_MAC_PACKET*)(pRxBuffQueueNode->data);
            // Free the Rx packet
            (*pMACDrv->sGmacData.pktFreeF)(pRxPkt);            
            //Free the queue node
            (*pMACDrv->sGmacData._freeF)(pMACDrv->sGmacData._AllocH, pRxBuffQueueNode);            
        }        
        //set rx descriptors to default
        for(index = 0; index < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt; index++ )
		{
            pMACDrv->sGmacData.gmac_queue[queueIdx].nRxDescIndex = 0;
			pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[index] = 0;
			pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[index].rx_desc_buffaddr.val = 0;
			pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[index].rx_desc_status.val = 0;
		}
        __DMB();
	}
	
}

#else
static void _MACDeinit(DRV_GMAC_DRIVER * pMACDrv )
{

}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void _MacTxDiscardQueues(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes)  
{
	TCPIP_MAC_PACKET* pPkt;
	DRV_PIC32CGMAC_SGL_LIST_NODE* tx_queue_node;
    uint8_t desc_idx;

    
	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk; 
	//remove the packets queued for transmission
    if((pMACDrv->sGmacData.gmac_queue[queueIdx]._TxStartQueue.nNodes) != 0)
    {
        while( (tx_queue_node = DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxStartQueue)) != 0) 
        {   // acknowledge the packet
            pPkt = tx_queue_node->data;
            (*pMACDrv->sGmacData.pktAckF)(pPkt, ackRes,TCPIP_THIS_MODULE_ID); //Remove the Tx Packets
            //clean the current Node
            _Clear_TxNode(tx_queue_node);
            // add the node to Free queue
            DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxNewQueue, tx_queue_node);
        }
	}
	//remove the packets queued for Acknowledgement
    if((pMACDrv->sGmacData.gmac_queue[queueIdx]._TxAckQueue.nNodes) != 0) 
    {
        while( (tx_queue_node = DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxAckQueue)) != 0)
        {   // acknowledge the packet
            pPkt = tx_queue_node->data;
            (*pMACDrv->sGmacData.pktAckF)(pPkt, ackRes,TCPIP_THIS_MODULE_ID); //Remove the Tx Packets
            //clean the current Node
            _Clear_TxNode(tx_queue_node);
            // add the node to Free queue
            DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxNewQueue,tx_queue_node);
        }
    }
    
    pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead = 0;
    pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail = 0;
    for(desc_idx=0; desc_idx < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt; desc_idx++)
    {    
        pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc[desc_idx].tx_desc_buffaddr = 0;
        pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc[desc_idx].tx_desc_status.val = GMAC_TX_USED_BIT | GMAC_TX_LAST_BUFFER_BIT;
    }
    pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc[desc_idx-1].tx_desc_status.val |= GMAC_TX_WRAP_BIT;

    GMAC_REGS->GMAC_TBQB = GMAC_TBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc);
    GMAC_REGS->GMAC_NCR |= GMAC_NCR_TXEN_Msk; 
}

/*************************
 * local data
 *************************/

static DRV_HANDLE         _hEventMac;     // the MAC we belong to


/*********************************
 *  local proto 
 ******************************************/

void DRV_GMAC_Tasks_ISR( SYS_MODULE_OBJ object );

/****************************************************************************
 * Function:        _XtlEventsTcp2Eth
 *
 * PreCondition:    None
 *
 * Input:           tcpEv - the TcpIp events to be translated
 *
 * Output:          Ethernet events
 *
 * Side Effects:    None
 *
 * Overview:        This function translates from  events to Ethernet events
 *
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events! 
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation the CONN_LOST/CONN_ESTABLISHED event is lost!
 ******************************************************************************/
/*static __inline__*/static  GMAC_EVENTS /*__attribute__((always_inline))*/ _XtlEventsTcp2Eth(TCPIP_MAC_EVENT tcpEv)
{

	GMAC_EVENTS  eEvents;
	eEvents = (tcpEv&(TCPIP_MAC_EV_TX_DONE))? GMAC_EV_TXCOMPLETE:0;	
	eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_DONE))? GMAC_EV_RXCOMPLETE:0;	
	eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_OVFLOW))? GMAC_EV_RXOVERRUN:0;
	eEvents |= (tcpEv&(TCPIP_MAC_EV_TX_BUSERR))? GMAC_EV_TXFRAMECORRUPT:0;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_TX_ABORT))? GMAC_EV_TXUNDERRUN:0;
	eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_BUFNA))? GMAC_EV_RXUSEDBITREAD:0; // mapping RX Used bit interrupt to BUFNA;
                                                                         // reason is no GMAC interrupt bit for BUFNA
                                                                         // and RXUBR interrupt will trigger with BUFNA
    
	return eEvents;
	
}

/****************************************************************************
 * Function:        _XtlEventsEth2Tcp
 *
 * PreCondition:    None
 *
 * Input:           eEvents - the Eth events to be translated
 *
 * Output:          TcpIp events
 *
 * Side Effects:    None
 *
 * Overview:        This function translates from Ethernet events to TcpIp events
 *
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events! 
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation a CONN_LOST/CONN_ESTABLISHED event cannot be generated!
 ******************************************************************************/
/*static __inline__*/static  TCPIP_MAC_EVENT /*__attribute__((always_inline))*/ _XtlEventsEth2Tcp(GMAC_EVENTS eEvents)
{
	TCPIP_MAC_EVENT tcpEv;

    tcpEv = (eEvents&(GMAC_EV_TXCOMPLETE))? TCPIP_MAC_EV_TX_DONE:0;
	tcpEv |= (eEvents&(GMAC_EV_RXCOMPLETE))? TCPIP_MAC_EV_RX_DONE:0;
	tcpEv |= (eEvents&(GMAC_EV_RXOVERRUN))? TCPIP_MAC_EV_RX_OVFLOW:0;
	tcpEv |= (eEvents&(GMAC_EV_TXFRAMECORRUPT))? TCPIP_MAC_EV_TX_BUSERR:0;
	tcpEv |= (eEvents&(GMAC_EV_RXUSEDBITREAD))? TCPIP_MAC_EV_RX_BUFNA:0; // mapping RX Used bit interrupt to BUFNA;
                                                                         // reason is no GMAC interrupt bit for BUFNA
                                                                         // and RXUBR interrupt will trigger with BUFNA
    
    return tcpEv;
}

/*********************************
 *  implementation
 ******************************************/

/*******************************************************************************
  Function:
    TCPIP_MAC_RES DRV_GMAC_EventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam)

  Summary:
    Initializes the ethernet event notification.

  Description:
     This function initializes the ethernet event notification.
     It performs any resource allocation that may be needed.

  Precondition:
     None.

  Parameters:
    hMac        - parameter identifying the intended MAC  
    eventF      - notification function
    eventParam  - notification parameter
    
  Returns:
    TCPIP_MAC_RES_OK  if initialization succeeded,
    an error code otherwise

  Example:
    <code>
    DRV_GMAC_EventInit( hMac, eventF, param);
    </code>

  Remarks:

    Not multi-threaded safe.
*****************************************************************************/
static TCPIP_MAC_RES DRV_GMAC_EventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam)
{
	DRV_GMAC_EVENT_DCPT*    pDcpt;
	DRV_GMAC_DRIVER * pMACDrv;
	
	if(_hEventMac == hMac)
	{   // re-init the same MAC
		return TCPIP_MAC_RES_OK;
	}
	else if(_hEventMac != 0)
	{   // already opened
		return TCPIP_MAC_RES_IS_BUSY;
	}

	pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	SYS_INT_SourceDisable(pMACDrv->sGmacData._macIntSrc);      // stop Eth ints
	SYS_INT_SourceStatusClear(pMACDrv->sGmacData._macIntSrc);

	pDcpt = &pMACDrv->sGmacData._pic32c_ev_group_dcpt;
	pDcpt->_TcpEnabledEvents = pDcpt->_TcpPendingEvents = TCPIP_MAC_EV_NONE;
	pDcpt->_EthEnabledEvents = pDcpt->_EthPendingEvents = 0;
	pDcpt->_TcpNotifyFnc = eventF;
	pDcpt->_TcpNotifyParam = eventParam;

	_hEventMac = hMac;
	return TCPIP_MAC_RES_OK;

}

/*******************************************************************************
  Function:
    TCPIP_MAC_RES    DRV_GMAC_EventDeInit(DRV_HANDLE hMac )

  Summary:
    De-initializes the ethernet event notification.

  Description:
     This function de-initializes the ethernet event notification.
     It performs any resource clean-up that may be needed.

  Precondition:
     None.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    
  Returns:
    TCPIP_MAC_RES_OK  always

  Example:
    <code>
    DRV_GMAC_EventDeInit( hMac );
    </code>

  Remarks:

    Not multi-threaded safe.
*****************************************************************************/
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES DRV_GMAC_EventDeInit(DRV_HANDLE hMac) 
{
	DRV_GMAC_EVENT_DCPT*    pDcpt;
	DRV_GMAC_DRIVER * pMACDrv;

	if(_hEventMac != hMac)
	{   // not us?
		return TCPIP_MAC_RES_IS_BUSY;
	}

	pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	SYS_INT_SourceDisable(pMACDrv->sGmacData._macIntSrc);      // stop Eth ints
	SYS_INT_SourceStatusClear(pMACDrv->sGmacData._macIntSrc);

	pDcpt = &pMACDrv->sGmacData._pic32c_ev_group_dcpt;
	pDcpt->_TcpNotifyFnc = 0;
	pDcpt->_TcpEnabledEvents = pDcpt->_TcpPendingEvents = TCPIP_MAC_EV_NONE;
	pDcpt->_EthEnabledEvents = pDcpt->_EthPendingEvents = 0;

	_hEventMac = 0;
	return TCPIP_MAC_RES_OK;
    
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)


/*******************************************************************************
  Function:
    bool    DRV_GMAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable)

  Summary:
    Enables the MAC events.

  Description:
     This function sets the enabled events.
     Multiple events can be orr-ed together.
     All events that are set will be added to the notification process. The old events will be disabled.
     The stack (or stack user) has to catch the events that are notified and process them:
         - The stack should process the TCPIP_MAC_EV_RX_PKTPEND/TCPIP_MAC_EV_RX_DONE, TCPIP_MAC_EV_TX_DONE transfer events
         - Process the specific condition and acknowledge them calling DRV_GMAC_EventAcknowledge() so that they can be re-enabled.

  Precondition:
   DRV_GMAC_EventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    macEvMask - events the user of the stack wants to add/delete for notification
    enable    - if true, the events will  be enabled, else disabled
    
  Returns:
    true  if operation succeeded,
    an error code otherwise

  Example:
    <code>
    DRV_GMAC_EventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    The event notification system enables the user of the TCPIP stack to call into the stack
    for processing only when there are relevant events rather than being forced to periodically call
    from within a loop.
    
    If the notification events are nill the interrupt processing will be disabled.
    Otherwise the event notification will be enabled and the interrupts relating to the requested events will be enabled.
    
    Note that once an event has been caught by the stack ISR (and reported if a notification handler is in place)
    it will be disabled until the DRV_GMAC_EventAcknowledge() is called.

*****************************************************************************/
bool DRV_GMAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable) 
{
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	DRV_GMAC_EVENT_DCPT*  pDcpt = &pMACDrv->sGmacData._pic32c_ev_group_dcpt;	
	
	if(enable)
	{
		GMAC_EVENTS  ethSetEvents;
        
		ethSetEvents = _XtlEventsTcp2Eth(macEvMask);

		if(pDcpt->_TcpEnabledEvents != 0)
		{   // already have some active
			SYS_INT_SourceDisable(pMACDrv->sGmacData._macIntSrc);      // stop ints for a while
		}

		pDcpt->_TcpEnabledEvents |= macEvMask;        // add more
		pDcpt->_EthEnabledEvents |= ethSetEvents;

		if(pDcpt->_TcpEnabledEvents != 0)
		{
			ethSetEvents &= ~pDcpt->_EthPendingEvents;		// keep just the new un-ack events			
			GMAC_REGS->GMAC_ISR;		//Read ISR register to clear the interrupt status			
			GMAC_REGS->GMAC_IER = ethSetEvents;
			SYS_INT_SourceEnable(pMACDrv->sGmacData._macIntSrc);       // enable
		}
	}
	else
	{   // disable some events
		GMAC_EVENTS  ethClrEvents;
		int         ethILev = 0;

		macEvMask &= pDcpt->_TcpEnabledEvents;                  // keep just the enabled ones
		ethClrEvents = _XtlEventsTcp2Eth(macEvMask);

		if(pDcpt->_TcpEnabledEvents != 0)
		{   // already have some active
			ethILev = SYS_INT_SourceDisable(pMACDrv->sGmacData._macIntSrc);      // stop ints for a while
		}

		pDcpt->_TcpEnabledEvents &= ~macEvMask;     // clear some of them
		pDcpt->_EthEnabledEvents &= ~ethClrEvents;

		pDcpt->_TcpPendingEvents &= ~macEvMask;     // remove them from un-ack list
		pDcpt->_EthPendingEvents &= ~ethClrEvents;
		
		GMAC_REGS->GMAC_IDR = ethClrEvents;		
		GMAC_REGS->GMAC_ISR;		//Read ISR register to clear the interrupt status

		if(pDcpt->_TcpEnabledEvents != 0)
		{
			SYS_INT_SourceRestore(pMACDrv->sGmacData._macIntSrc, ethILev);   // re-enable  
		}
	}

	return true;

}


/*******************************************************************************
  Function:
    bool    DRV_GMAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv)

  Summary:
    Acknowledges and re-enables processed events.

  Description:
    This function acknowledges and re-enables processed events.
    Multiple events can be orr-ed together as they are processed together.
    The events acknowledged by this function should be the events that have been retrieved from the stack
    by calling DRV_GMAC_EventPendingGet() or have been passed to the user by the stack using the notification handler
    (PIC32MACEventSetNotifyHandler()) and have been processed and have to be re-enabled.


  Precondition:
   DRV_GMAC_EventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    macEvents - the events that the user processed and need to be re-enabled
    
  Returns:
    true if events acknowledged
    false if no events to be acknowledged

  Example:
    <code>
    DRV_GMAC_EventAcknowledge( hMac, stackNewEvents );
    </code>

  Remarks:   
    All events should be acknowledged, in order to be re-enabled.

    Some events are fatal errors and should not be acknowledged (TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR).
    Stack re-initialization is needed under such circumstances.

    Some events are just system/application behavior and they are intended only as simple info (TCPIP_MAC_EV_RX_OVFLOW,
    TCPIP_MAC_EV_RX_BUFNA, TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT).

    The TCPIP_MAC_EV_RX_FWMARK and TCPIP_MAC_EV_RX_EWMARK events are part of the normal flow control operation (if auto flow control was enabled).
    They should be enabled alternatively, if needed.

    The events are persistent. They shouldn't be re-enabled unless they have been processed and the condition that generated them was removed.
    Re-enabling them immediately without proper processing will have dramatic effects on system performance.

*****************************************************************************/
bool DRV_GMAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv) 
{
	int                   ethILev;
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	DRV_GMAC_EVENT_DCPT*  pDcpt = &pMACDrv->sGmacData._pic32c_ev_group_dcpt;
	
	if(pDcpt->_TcpEnabledEvents != 0)
	{   // already have some active
		GMAC_EVENTS  ethAckEv;

		ethAckEv=_XtlEventsTcp2Eth(tcpAckEv);

		ethILev = SYS_INT_SourceDisable(pMACDrv->sGmacData._macIntSrc);  // stop ints for a while

		pDcpt->_TcpPendingEvents &= ~tcpAckEv;         // no longer pending

		pDcpt->_EthPendingEvents &= ~ethAckEv;         // no longer pending
		
		GMAC_REGS->GMAC_ISR;		//Read ISR register to clear the interrupt status		
		GMAC_REGS->GMAC_IER = ethAckEv;

		SYS_INT_SourceRestore(pMACDrv->sGmacData._macIntSrc, ethILev);   // re-enable 
		return true;
	}

	return false;
	
}


/*******************************************************************************
  Function:
    TCPIP_MAC_EVENT DRV_GMAC_EventPendingGet(DRV_HANDLE hMac)

  Summary:
    Returns the currently pending events.

  Description:
    This function returns the currently pending events belonging to a group.
    Multiple events can be orr-ed together as they accumulate.
    The stack should be called for processing whenever a stack managed event (TCPIP_MAC_EV_RX_PKTPEND, TCPIP_MAC_EV_TX_DONE) is present.
    The other, non critical events, may not be managed by the stack and passed to an user.
    They will have to be eventually acknowledged if re-enabling is needed.

  Precondition:
   hMac      - parameter identifying the intended MAC  
   DRV_GMAC_EventInit should have been called.

  Parameters:
    
  Returns:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = DRV_GMAC_EventPendingGet( hMac);
    </code>

  Remarks:   
    This is the preferred method to get the current pending MAC events.
    The stack maintains a proper image of the events from their occurrence to their acknowledgement.
    
    Even with a notification handler in place it's better to use this function to get the current pending events
    rather than using the events passed by the notification handler which could be stale.
    
    The events are persistent. They shouldn't be re-enabled unless they have been processed and
    the condition that generated them was removed.
    Re-enabling them immediately without proper processing will have dramatic effects on system performance.

    The returned value is just a momentary value. The pending events can change any time.
*****************************************************************************/
TCPIP_MAC_EVENT DRV_GMAC_EventPendingGet(DRV_HANDLE hMac) 
{
	DRV_GMAC_DRIVER * pMACDrv = (DRV_GMAC_DRIVER*)hMac;
	return pMACDrv->sGmacData._pic32c_ev_group_dcpt._TcpPendingEvents;
}


/**************************************
 * local functions
 ****************************************/
// Rx Filter for PIC32C
static GMAC_RX_FILTERS _DRV_GMAC_MacToEthFilter(TCPIP_MAC_RX_FILTER_TYPE macFilter) 
{
	GMAC_RX_FILTERS rxFilter = 0;
    
	rxFilter = (macFilter & TCPIP_MAC_RX_FILTER_TYPE_FRAMEERROR_ACCEPT) ? GMAC_FILT_PREAMBLE_ERR_ACCEPT: 0;
	
	rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT) ? GMAC_FILT_CRC_ERR_ACCEPT:0;
	
	rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_MAXFRAME_ACCEPT) ? GMAC_FILT_MAXFRAME_ACCEPT:0;
	
	rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_UCAST_HASH_ACCEPT) ? GMAC_FILT_UCASTHASH_ACCEPT:0;
	
	rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT) ? GMAC_FILT_MCASTHASH_ACCEPT:0;
    
    rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_MCAST_HASH_ACCEPT) ? GMAC_FILT_MCASTHASH_ACCEPT:0;
	
	rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT) ? 0:GMAC_FILT_BCAST_REJECT;

	rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_ALL_ACCEPT) ? GMAC_FILT_ALLFRAME_ACCEPT:0;
	
	rxFilter |= (macFilter & TCPIP_MAC_RX_FILTER_TYPE_JUMBOFRAME_ACCEPT) ? GMAC_FILT_JUMBOFRAME_ACCEPT:0;
	
	return rxFilter;

}

void GMAC_InterruptHandler(void)
{
	DRV_GMAC_Tasks_ISR((SYS_MODULE_OBJ)0);
}

/****************************************************************************
 * Function:        DRV_GMAC_Tasks_ISR
 *
 * PreCondition:    DRV_GMAC_EventInit, DRV_GMAC_EventMaskSet should have been called.
 *
 * Input:           macIndex - PIC32 MAC object index
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function processes the Ethernet interrupts and reports the events back to the user.
 *
 * Note:            None
 ******************************************************************************/

void DRV_GMAC_Tasks_ISR( SYS_MODULE_OBJ macIndex ) 
{
	GMAC_EVENTS  currEthEvents = (GMAC_EVENTS)GMAC_EV_NONE;
    GMAC_EVENTS currGroupEvents = (GMAC_EVENTS)GMAC_EV_NONE;
	DRV_GMAC_EVENT_DCPT* pDcpt;	
	DRV_GMAC_DRIVER * pMACDrv = &_gmac_drv_dcpt[macIndex];		
	
	currEthEvents = (GMAC_EVENTS)GMAC_REGS->GMAC_ISR;
    __DMB();
	// process interrupts
	pDcpt = &pMACDrv->sGmacData._pic32c_ev_group_dcpt;
	currGroupEvents = currEthEvents & pDcpt->_EthEnabledEvents;     //  keep just the relevant ones

	if(currGroupEvents)
	{
		pDcpt->_EthPendingEvents |= currGroupEvents;                    // add the new events
		pDcpt->_TcpPendingEvents |= _XtlEventsEth2Tcp(currGroupEvents);


		{			
			GMAC_REGS->GMAC_IDR = currGroupEvents;					
		}


		if(pDcpt->_TcpNotifyFnc)
		{
			(*pDcpt->_TcpNotifyFnc)(pDcpt->_TcpPendingEvents, pDcpt->_TcpNotifyParam);     // let the user know
		}
	}

}

/*****************************************************************************
 ********************* GMAC Statistics Register Access routines **************
 *****************************************************************************/

static uint32_t _DRV_GMAC_GetTxOctetLow(void)
{
		
	return GMAC_REGS->GMAC_OTLO;	
}

static uint32_t _DRV_GMAC_GetTxOctetHigh(void)
{
	
	return GMAC_REGS->GMAC_OTHI;
}

static uint32_t _DRV_GMAC_GetTxFrameCount(void)
{
	
	return GMAC_REGS->GMAC_FT;
}

static uint32_t _DRV_GMAC_GetTxBCastFrameCount(void)
{
	
	return GMAC_REGS->GMAC_BCFT;
}

static uint32_t _DRV_GMAC_GetTxMCastFrameCount(void)
{
	
	return GMAC_REGS->GMAC_MFT;
}

static uint32_t _DRV_GMAC_GetTxPauseFrameCount(void)
{
	
	return GMAC_REGS->GMAC_PFT;
}

static uint32_t _DRV_GMAC_GetTx64ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_BFT64;
}

static uint32_t _DRV_GMAC_GetTx127ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFT127;
}

static uint32_t _DRV_GMAC_GetTx255ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFT255;
}

static uint32_t _DRV_GMAC_GetTx511ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFT511;
}

static uint32_t _DRV_GMAC_GetTx1023ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFT1023;
}

static uint32_t _DRV_GMAC_GetTx1518ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFT1518;
}

static uint32_t _DRV_GMAC_GetTxGT1518ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_GTBFT1518;
}

static uint32_t _DRV_GMAC_GetTxUnderRunFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TUR;
}

static uint32_t _DRV_GMAC_GetTxSingleCollFrameCount(void)
{
	
	return GMAC_REGS->GMAC_SCF;
}

static uint32_t _DRV_GMAC_GetTxMultiCollFrameCount(void)
{
	
	return GMAC_REGS->GMAC_MCF;
}

static uint32_t _DRV_GMAC_GetTxExcessCollFrameCount(void)
{
	
	return GMAC_REGS->GMAC_EC;
}

static uint32_t _DRV_GMAC_GetTxLateCollFrameCount(void)
{
	
	return GMAC_REGS->GMAC_LC;
}

static uint32_t _DRV_GMAC_GetTxDeferFrameCount(void)
{
	
	return GMAC_REGS->GMAC_DTF;
}

static uint32_t _DRV_GMAC_GetTxCSErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_CSE;
}

static uint32_t _DRV_GMAC_GetRxOctetLow(void)
{
	
	return GMAC_REGS->GMAC_ORLO;
}

static uint32_t _DRV_GMAC_GetRxOctetHigh(void)
{
	
	return GMAC_REGS->GMAC_ORHI;
}

static uint32_t _DRV_GMAC_GetRxFrameCount(void)
{
	
	return GMAC_REGS->GMAC_FR;
}

static uint32_t _DRV_GMAC_GetRxBCastFrameCount(void)
{
	
	return GMAC_REGS->GMAC_BCFR;
}

static uint32_t _DRV_GMAC_GetRxMCastFrameCount(void)
{
	
	return GMAC_REGS->GMAC_MFR;
}

static uint32_t _DRV_GMAC_GetRxPauseFrameCount(void)
{
	
	return GMAC_REGS->GMAC_PFR;
}

static uint32_t _DRV_GMAC_GetRx64ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_BFR64;
}

static uint32_t _DRV_GMAC_GetRx127ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFR127;
}

static uint32_t _DRV_GMAC_GetRx255ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFR255;
}

static uint32_t _DRV_GMAC_GetRx511ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFR511;
}

static uint32_t _DRV_GMAC_GetRx1023ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFR1023;
}

static uint32_t _DRV_GMAC_GetRx1518ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TBFR1518;
}

static uint32_t _DRV_GMAC_GetRxGT1518ByteFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TMXBFR;
}

static uint32_t _DRV_GMAC_GetRxUnderSizeFrameCount(void)
{
	
	return GMAC_REGS->GMAC_UFR;
}

static uint32_t _DRV_GMAC_GetRxOverSizeFrameCount(void)
{
	
	return GMAC_REGS->GMAC_OFR;
}

static uint32_t _DRV_GMAC_GetRxJabberFrameCount(void)
{
	
	return GMAC_REGS->GMAC_JR;
}

static uint32_t _DRV_GMAC_GetRxFCSErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_FCSE;
}

static uint32_t _DRV_GMAC_GetRxLFErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_LFFE;
}

static uint32_t _DRV_GMAC_GetRxSymErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_RSE;
}

static uint32_t _DRV_GMAC_GetRxAlignErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_AE;
}

static uint32_t _DRV_GMAC_GetRxResErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_RRE;
}

static uint32_t _DRV_GMAC_GetRxOverRunFrameCount(void)
{
	
	return GMAC_REGS->GMAC_ROE;
}

static uint32_t _DRV_GMAC_GetRxIPHdrCSErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_IHCE;
}

static uint32_t _DRV_GMAC_GetRxTCPCSErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_TCE;
}

static uint32_t _DRV_GMAC_GetRxUDPCSErrorFrameCount(void)
{
	
	return GMAC_REGS->GMAC_UCE;
}

/****************************************************************************
 * Function:        _DRV_GMAC_PHYInitialise
 * Summary: PHY initialization 
 *****************************************************************************/
static SYS_MODULE_OBJ _DRV_GMAC_PHYInitialise(DRV_GMAC_DRIVER *pMACDrv) 
{
	SYS_MODULE_OBJ hPhySysObject;
	DRV_HANDLE     hPhyClient;	
	DRV_ETHPHY_SETUP phySetup;
	const DRV_ETHPHY_OBJECT_BASE* pPhyBase;	
	int phyIx = pMACDrv->sGmacData._phyIx;
	TCPIP_MAC_RES   initRes;
	DRV_ETHPHY_RESULT   phyInitRes;	

	pPhyBase =  pMACDrv->sGmacData.gmacConfig.pPhyBase; 

	hPhySysObject = pPhyBase->DRV_ETHPHY_Initialize(phyIx,  (SYS_MODULE_INIT *)pMACDrv->sGmacData.gmacConfig.pPhyInit);
	if ( hPhySysObject == SYS_MODULE_OBJ_INVALID )
	{
		initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
		return initRes;
	}

	pMACDrv->sGmacData.hPhySysObject = hPhySysObject;
	hPhyClient = pPhyBase->DRV_ETHPHY_Open(phyIx, DRV_IO_INTENT_READWRITE);
	if ( DRV_HANDLE_INVALID == hPhyClient )
	{
		initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
		return initRes;
	}
	
	pMACDrv->sGmacData.hPhyClient = hPhyClient;
	phySetup.phyAddress = pMACDrv->sGmacData.gmacConfig.pPhyInit->phyAddress;
	phySetup.openFlags = pMACDrv->sGmacData.gmacConfig.ethFlags;
	phySetup.configFlags = pMACDrv->sGmacData.gmacConfig.pPhyInit->phyFlags;
	phySetup.macPauseType = DRV_GMAC_PAUSE_CPBL_MASK;
	phySetup.resetFunction = pMACDrv->sGmacData.gmacConfig.pPhyInit->resetFunction;
		
	phyInitRes = pPhyBase->DRV_ETHPHY_Setup(hPhyClient, &phySetup, &pMACDrv->sGmacData._linkResFlags);
	
	if(phyInitRes < 0)
	{   // some error occurred
		initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
		return initRes;
	}
	initRes = TCPIP_MAC_RES_OK;
	return initRes;
}

