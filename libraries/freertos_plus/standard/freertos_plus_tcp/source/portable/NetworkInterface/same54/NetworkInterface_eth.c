/*******************************************************************************
*  Network Interface file
*
*  Summary:
*   Network Interface file for FreeRTOS-Plus-TCP stack
*
*  Description:
*   - Interfaces PIC32 to the FreeRTOS TCP/IP stack
*******************************************************************************/

/*******************************************************************************
*  File Name:  pic32_NetworkInterface.c
*  Copyright 2017 Microchip Technology Incorporated and its subsidiaries.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy of
*  this software and associated documentation files (the "Software"), to deal in
*  the Software without restriction, including without limitation the rights to
*  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
*  of the Software, and to permit persons to whom the Software is furnished to do
*  so, subject to the following conditions:
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE
*******************************************************************************/

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"


#include "NetworkInterface.h"
#include "NetworkConfig.h"



#include "system_config.h"
#include "system/console/sys_console.h"
#include "system/debug/sys_debug.h"
#include "system/command/sys_command.h"

#include "tcpip/tcpip_mac.h"
#include "driver/gmac/drv_gmac.h"
#include "driver/miim/drv_miim.h"

#include "tcpip/tcpip.h"
#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/link_list.h"

#define TCPIP_STACK_USE_EXTERNAL_HEAP

#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX0				"00:04:25:1C:A0:02"

#define TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX0			"192.168.100.11"
#define TCPIP_NETWORK_DEFAULT_IP_MASK_IDX0			"255.255.255.0"
#define TCPIP_NETWORK_DEFAULT_GATEWAY_IDX0			"192.168.100.1"
#define TCPIP_NETWORK_DEFAULT_DNS_IDX0				"192.168.100.1"
#define TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX0			"0.0.0.0"
#define TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0			"full"

#define TCPIP_GMAC_RX_BUFF_COUNT_QUE0				   		12
#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0				1
#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0					1


const DRV_MIIM_INIT drvMiimInitData =
{
    .ethphyId = DRV_MIIM_ETH_MODULE_ID,
};

#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0	"GMAC"
#define TCPIP_IF_GMAC

#define TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX0				"MCHPBOARD_C"
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX0				"00:04:25:1C:A0:02"

#define TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX0			\
													TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON |\
													TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON |\
													TCPIP_NETWORK_CONFIG_IP_STATIC
													
#define TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0			DRV_GMAC_Object


const TCPIP_NETWORK_CONFIG __attribute__((unused))  TCPIP_HOSTS_CONFIGURATION[] =
{
	/*** Network Configuration Index 0 ***/
    {
        TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0,       // interface
        TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX0,            // hostName
        TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX0,             // macAddr
        TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX0,           // ipAddr
        TCPIP_NETWORK_DEFAULT_IP_MASK_IDX0,              // ipMask
        TCPIP_NETWORK_DEFAULT_GATEWAY_IDX0,              // gateway
        TCPIP_NETWORK_DEFAULT_DNS_IDX0,                  // priDNS
        TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX0,           // secondDNS
        TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0,           // powerMode
        TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX0,      // startFlags
       &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0,           // pMacObject
    },
};
const DRV_ETHPHY_INIT tcpipPhyInitData =
{    
    .ethphyId               = TCPIP_INTMAC_MODULE_ID,
    .phyAddress             = TCPIP_INTMAC_PHY_ADDRESS,
    .phyFlags               = TCPIP_INTMAC_PHY_CONFIG_FLAGS,
    .pPhyObject             = &DRV_ETHPHY_OBJECT_KSZ8091,
    .resetFunction          = 0,
    .pMiimObject            = &DRV_MIIM_OBJECT_BASE_Default,
    .pMiimInit              = &drvMiimInitData,
    .miimIndex              = DRV_MIIM_DRIVER_INDEX,

};

/*** GMAC MAC Initialization Data ***/
const TCPIP_MODULE_MAC_PIC32C_CONFIG tcpipMACPIC32CINTInitData =
{ 
	/** QUEUE 0 Intialization**/
		.gmac_queue_config[0].queueEnable	= true,
	.gmac_queue_config[0].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0,
	.gmac_queue_config[0].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0,
	.gmac_queue_config[0].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE0,
	.gmac_queue_config[0].txBufferSize	= TCPIP_GMAC_TX_BUFF_SIZE_QUE0,
	.gmac_queue_config[0].nRxBuffCount	= TCPIP_GMAC_RX_BUFF_COUNT_QUE0,
	.gmac_queue_config[0].nRxBuffCntThres	= TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0,
	.gmac_queue_config[0].nRxBuffAllocCnt	= TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0,

	
	.ethFlags               = TCPIP_GMAC_ETH_OPEN_FLAGS,	
	.linkInitDelay          = TCPIP_INTMAC_PHY_LINK_INIT_DELAY,
    .ethModuleId            = TCPIP_INTMAC_MODULE_ID,
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_Default,
    .pPhyInit               = &tcpipPhyInitData,
};


#ifdef PIC32_USE_ETHERNET

    /* local definitions and data */

    /* debug messages */
    #if ( PIC32_MAC_DEBUG_MESSAGES != 0 )
        #define PIC32_MAC_DbgPrint   printf
    #else
        #define PIC32_MAC_DbgPrint                  printf  
    #endif /* (PIC32_MAC_DEBUG_MESSAGES != 0) */

    typedef enum
    {
        PIC32_MAC_EVENT_INIT_NONE = 0x000,      /* no event/invalid */

        PIC32_MAC_EVENT_INIT_DONE = 0x001,      /* initialization done event */
        PIC32_MAC_EVENT_TIMEOUT = 0x002,        /* periodic timeout event */
        PIC32_MAC_EVENT_IF_PENDING = 0x004,     /* an interface event signal: RX, TX, errors. etc. */
    } PIC32_MAC_EVENT_TYPE;

    typedef enum
    {
        eMACInit,                               /* Must initialise MAC. */
        eMACPass,                               /* Initialisation was successful. */
        eMACFailed,                             /* Initialisation failed. */
    } eMAC_INIT_STATUS_TYPE;

    static TCPIP_STACK_HEAP_HANDLE macHeapHandle;

    static const TCPIP_MAC_OBJECT * macObject; /* the one and only MAC object; */

    static SYS_MODULE_OBJ macObjHandle;        /* the MAC object instance, obtained at initialization */
    static TCPIP_MAC_HANDLE macCliHandle;      /* client handle */
    static volatile SYS_STATUS macObjStatus;   /* current MAC status */

    static TaskHandle_t macTaskHandle;

    static TimerHandle_t macTmrHandle;

    static bool macLinkStatus;              /* true if link is ON */

    static eMAC_INIT_STATUS_TYPE xMacInitStatus = eMACInit;

    /* local prototypes */
    static bool StartInitMac( void );
    static void StartInitCleanup( void );

    static void SetMacCtrl( TCPIP_MAC_MODULE_CTRL * pMacCtrl );

    static bool MacSyncFunction( void * synchHandle,
                                 TCPIP_MAC_SYNCH_REQUEST req );

    /* the PIC32 MAC task function */
    static void MacHandlerTask( void * params );

    /* MAC interrupt event function */
    static void MAC_EventFunction( TCPIP_MAC_EVENT event,
                                   const void * eventParam );

    /* timer callback for link maintenance, etc; */
    static void MacTmrCallback( TimerHandle_t xTimer );

    /* MAC RX packets functions */
    static void MacRxPackets( void );
    static void MacProcessRxPacket( TCPIP_MAC_PACKET * pRxPkt );


    /* memory allocation mapping to FreeRTOS */
    static void * _malloc( size_t nBytes )
    {
        return pvPortMalloc( nBytes );
    }

    /*-----------------------------------------------------------*/

    static void * _calloc( size_t nElems,
                           size_t elemSize )
    {
        size_t nBytes = nElems * elemSize;

        void * ptr = pvPortMalloc( nBytes );

        if( ptr != 0 )
        {
            memset( ptr, 0, nBytes );
        }

        return ptr;
    }

    /*-----------------------------------------------------------*/

    static void _free( void * pBuff )
    {
        vPortFree( pBuff );
    }

    /* extern references */
    /* */
    /* use the configuration data from the system_init.c */
    extern const TCPIP_NETWORK_CONFIG TCPIP_HOSTS_CONFIGURATION[];

    /* BufferAllocation_2.c:: packet allocation function */
    extern TCPIP_MAC_PACKET * PIC32_MacPacketAllocate( uint16_t pktLen,
                                                       uint16_t segLoadLen,
                                                       TCPIP_MAC_PACKET_FLAGS flags );

    extern void PIC32_MacAssociate( TCPIP_MAC_PACKET * pRxPkt,
                                    NetworkBufferDescriptor_t * pxBufferDescriptor,
                                    size_t pktLength );
    extern void PIC32_MacPacketOrphan( TCPIP_MAC_PACKET * pPkt );

    /* cannot use the system_init.c::tcpipHeapConfig because FreeRTOS does not have a calloc function! */
    /* we build it here! */

    /* make sure we're running with external heap! Redirect to FreeRTOS. */
    #if !defined( TCPIP_STACK_USE_EXTERNAL_HEAP ) || defined( TCPIP_STACK_USE_INTERNAL_HEAP ) || defined( TCPIP_STACK_USE_INTERNAL_HEAP_POOL )
        #error "TCPIP_STACK_USE_EXTERNAL_HEAP should be defined for this project!"
    #endif

    static const TCPIP_STACK_HEAP_EXTERNAL_CONFIG tcpipHeapConfig =
    {
        .heapType   = TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP,
        .heapFlags  = TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED | TCPIP_STACK_HEAP_FLAG_NO_MTHREAD_SYNC,
        .heapUsage  = TCPIP_STACK_HEAP_USE_DEFAULT,
        .malloc_fnc = _malloc,
        .calloc_fnc = _calloc,
        .free_fnc   = _free,
    };

    #if ( PIC32_MAC_DEBUG_COMMANDS != 0 )
        static int _Command_MacInfo( SYS_CMD_DEVICE_NODE * pCmdIO,
                                     int argc,
                                     char ** argv );
        static int _Command_NetInfo( SYS_CMD_DEVICE_NODE * pCmdIO,
                                     int argc,
                                     char ** argv );
        static int _Command_Version(SYS_CMD_DEVICE_NODE* pCmdIO,
                                      int argc,
                                      char** argv );
    #endif /* (PIC32_MAC_DEBUG_COMMANDS != 0) */

#if (PIC32_FLASH_PKCS_COMMANDS != 0)
        static int _Command_Pkcs11(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv );
        static int _Command_Write(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv );
        static int _Command_Erase(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv );
        static int _Command_BlankCheck(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv );
#endif  // (PIC32_FLASH_PKCS_COMMANDS != 0)

#if ( PIC32_MAC_DEBUG_COMMANDS != 0 ) || (PIC32_FLASH_PKCS_COMMANDS != 0)
        static const SYS_CMD_DESCRIPTOR macCmdTbl[] =
        {
#if ( PIC32_MAC_DEBUG_COMMANDS != 0 )
            { "macinfo",    _Command_MacInfo,   ": Check MAC statistics" },
            { "netinfo",    _Command_NetInfo,   ":Net info"              },
            { "version",    _Command_Version,   ":Version info"},
#endif  // ( PIC32_MAC_DEBUG_COMMANDS != 0 )
#if (PIC32_FLASH_PKCS_COMMANDS != 0)
            { "pkcs",       _Command_Pkcs11,    ":PKCS11 test"},
            { "write",      _Command_Write,     ":Write pages"},
            { "erase",      _Command_Erase,     ":Erase pages"},
            { "blank",      _Command_BlankCheck,":Blank Check"},
#endif  // (PIC32_FLASH_PKCS_COMMANDS != 0)
        };
#endif  // ( PIC32_MAC_DEBUG_COMMANDS != 0 ) || (PIC32_FLASH_PKCS_COMMANDS != 0)


    /* FreeRTOS implementation functions */
    BaseType_t xNetworkInterfaceInitialise( void )
    {
    BaseType_t xResult;

        if( xMacInitStatus == eMACInit )
        {
			/* This is the first time this function is called. */
            if( StartInitMac() != false )
            {
                /* Indicate that the MAC initialisation succeeded. */
                xMacInitStatus = eMACPass;
            }
            else
            {
                xMacInitStatus = eMACFailed;
            }
        }

        if( xMacInitStatus == eMACPass )
        {
            xResult = xGetPhyLinkStatus();
        }
        else
        {
            xResult = pdFAIL;
        }

    	PIC32_MAC_DbgPrint( "xNetworkInterfaceInitialise: %d %d\r\n", ( int ) xMacInitStatus, ( int ) xResult );

        return xResult;
    }


    /*-----------------------------------------------------------*/

    BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor,
                                        BaseType_t xReleaseAfterSend )
    {
        TCPIP_MAC_RES macRes;
        TCPIP_MAC_PACKET * pTxPkt;

        BaseType_t retRes = pdFALSE;


        if( ( pxDescriptor != 0 ) && ( pxDescriptor->pucEthernetBuffer != 0 ) && ( pxDescriptor->xDataLength != 0 ) )
        {
            TCPIP_MAC_PACKET ** ppkt = ( TCPIP_MAC_PACKET ** ) ( pxDescriptor->pucEthernetBuffer - PIC32C_BUFFER_PKT_PTR_OSSET );
            configASSERT( ( ( uint32_t ) ppkt & ( sizeof( uint32_t ) - 1 ) ) == 0 );
            pTxPkt = *ppkt;
            configASSERT( pTxPkt != 0 );

            /* prepare the packet for transmission */
            /* set the correct data length: */
            configASSERT( pTxPkt->pDSeg->segSize >= pTxPkt->pDSeg->segLen );
            pTxPkt->pDSeg->segLen = pxDescriptor->xDataLength;
            pTxPkt->next = 0; /* unlink it */
            macRes = ( macObject->TCPIP_MAC_PacketTx )( macCliHandle, pTxPkt );

            if( macRes >= 0 )
            {
                retRes = pdTRUE;
                pxDescriptor->pucEthernetBuffer = 0; /* it will be released by the MAC driver once it's transmitted */
                iptraceNETWORK_INTERFACE_TRANSMIT();
            }

            /* else same error occurred; this normally should not happen! But the buffer is left in there so it shold be freed! */

            /* The buffer has been sent so can be released. */
            if( xReleaseAfterSend != pdFALSE )
            {
                vReleaseNetworkBufferAndDescriptor( pxDescriptor );
            }
        }

        return retRes;
    }


    /************************************* Section: helper functions ************************************************** */
    /* */

    /*-----------------------------------------------------------*/

    const void * const PIC32_GetMacConfigData( void )
    {
            extern const TCPIP_MODULE_MAC_PIC32C_CONFIG tcpipMACPIC32CINTInitData;
            return &tcpipMACPIC32CINTInitData;
    }

    /************************************* Section: worker code ************************************************** */
    /* */
  
    static bool StartInitMac( void )
    {
        TCPIP_MAC_MODULE_CTRL macCtrl;
        EventBits_t evBits;


        /* perform some initialization of all variables so that we can cleanup what failed */
        /* if something failed, the routine will be called again and again by FreeRTOS! */
        macHeapHandle = 0;
        macObjHandle = 0;
        macCliHandle = 0;
        macTmrHandle = 0;
        macTaskHandle = 0;
        macObject = TCPIP_HOSTS_CONFIGURATION[ 0 ].pMacObject; /* the MAC object we use */
        macObjStatus = SYS_STATUS_UNINITIALIZED;
        macLinkStatus = false;

        int netUpFail = 0;

        while( true )
        {
            /* start the allocator */
            macHeapHandle = TCPIP_HEAP_Create( ( const TCPIP_STACK_HEAP_CONFIG * ) &tcpipHeapConfig, 0 );

            if( macHeapHandle == 0 )
            {
                netUpFail = 1;
                break;
            }

            if( TCPIP_PKT_Initialize( macHeapHandle, 0, 0 ) == false )
            {
                netUpFail = 2;
                break;
            }

           // moduleInit.sys.powerState = SYS_MODULE_POWER_RUN_FULL;

            /* Initialize the MAC. MAC address is defined to 0x000000000000 in
             * FreeRTOSConfig.h and therefore it will be initialized to the
             * factory programmed MAC address. */
            SetMacCtrl( &macCtrl );
            /* Set the mac address in the FreeRTOS+TCP stack. */
            FreeRTOS_UpdateMACAddress( macCtrl.ifPhyAddress.v );

            TCPIP_MAC_INIT macInit =
            {
                .moduleInit = { 0},
                .macControl = &macCtrl,
                .moduleData = PIC32_GetMacConfigData(),
            };
            

            macObjHandle = ( macObject->TCPIP_MAC_Initialize )( TCPIP_MODULE_MAC_PIC32C, &macInit.moduleInit );

            if( macObjHandle == SYS_MODULE_OBJ_INVALID )
            {
                macObjHandle = 0;
                netUpFail = 4;
                break;
            }

            /* open the MAC */
            macCliHandle = ( macObject->TCPIP_MAC_Open )( TCPIP_MODULE_MAC_PIC32C, DRV_IO_INTENT_READWRITE );

            if( macCliHandle == DRV_HANDLE_INVALID )
            {
                macCliHandle = 0;
                netUpFail = 5;
                break;
            }

            if( !( macObject->TCPIP_MAC_EventMaskSet )( macCliHandle, ( TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_TX_DONE | TCPIP_MAC_EV_RXTX_ERRORS ), true ) )
            {
                netUpFail = 6;
                break;
            }

            /* completed the MAC initialization */
            /* continue the initialization */
            macTmrHandle = xTimerCreate( PIC32_MAC_TIMER_NAME, PIC32_MAC_TIMER_PERIOD, pdTRUE, 0, MacTmrCallback );

            if( ( macTmrHandle == 0 ) || ( xTimerStart( macTmrHandle, 0 ) != pdPASS ) )
            {
                netUpFail = 8;
                break;
            }

            /* spawn the PIC32 MAC task function */
            /* and wait for its event signal */
            macObjStatus = SYS_STATUS_BUSY;

            if( xTaskCreate( MacHandlerTask, PIC32_MAC_TASK_NAME, PIC32_MAC_TASK_STACK_SIZE, xTaskGetCurrentTaskHandle(), PIC32_MAC_TASK_PRI, &macTaskHandle ) != pdPASS )
            { /* failed */
                netUpFail = 9;
                break;
            }

            xTaskNotifyWait( PIC32_MAC_EVENT_INIT_DONE, PIC32_MAC_EVENT_INIT_DONE, &evBits, PIC32_MAC_INIT_TIMEOUT );

            if( ( evBits & PIC32_MAC_EVENT_INIT_DONE ) == 0 )
            { /* timed out */
                netUpFail = 10;
                break;
            }

            if( macObjStatus != SYS_STATUS_READY )
            { /* failed somehow ??? */
                netUpFail = 11;
                break;
            }

            netUpFail = 0;
            break;
        }

        if( netUpFail == 0 )
        {
            PIC32_MAC_DbgPrint( " MAC Init success!\r\n" );

            #if ( PIC32_MAC_DEBUG_COMMANDS != 0 )
                /* create command group */
                if( !SYS_CMD_ADDGRP( macCmdTbl, sizeof( macCmdTbl ) / sizeof( *macCmdTbl ), "mac", ": mac commands" ) )
                {
                    PIC32_MAC_DbgPrint( "Failed to create MAC Commands\r\n" );
                }
            #endif /* (PIC32_MAC_DEBUG_COMMANDS != 0) */

            return true;
        }
        else
        {
            printf( "MAC Init failed: %d macObjStatus = %d!\r\n", netUpFail,macObjStatus );
            StartInitCleanup();
            return false;
        }
    }

    /*-----------------------------------------------------------*/

    static void StartInitCleanup( void )
    {
        if( macHeapHandle != 0 )
        {
            TCPIP_HEAP_Delete( macHeapHandle );
            macHeapHandle = 0;
        }

        if( macObjHandle != 0 )
        {
            ( macObject->TCPIP_MAC_Deinitialize )( macObjHandle );
            macObjHandle = 0;
        }

        if( macTmrHandle != 0 )
        {
            xTimerDelete( macTmrHandle, portMAX_DELAY );
            macTmrHandle = 0;
        }

        if( macTaskHandle != 0 )
        {
            vTaskDelete( macTaskHandle );
            macTaskHandle = 0;
        }
    }

    /*-----------------------------------------------------------*/

    static void SetMacCtrl( TCPIP_MAC_MODULE_CTRL * pMacCtrl )
    {
        TCPIP_MAC_ADDR macAdd;
        uint8_t unsetMACAddr[ 6 ] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };     /* not set MAC address */

        pMacCtrl->nIfs = 1;

        pMacCtrl->mallocF = TCPIP_HEAP_MallocOutline;
        pMacCtrl->callocF = TCPIP_HEAP_CallocOutline;
        pMacCtrl->freeF = TCPIP_HEAP_FreeOutline;
        pMacCtrl->memH = macHeapHandle;


        pMacCtrl->pktAllocF = PIC32_MacPacketAllocate;
        pMacCtrl->pktFreeF = ( TCPIP_MAC_PKT_FreeF ) _TCPIP_PKT_FREE_FNC;
        pMacCtrl->pktAckF = ( TCPIP_MAC_PKT_AckF ) _TCPIP_PKT_ACK_FNC;

        pMacCtrl->synchF = MacSyncFunction;

        pMacCtrl->eventF = MAC_EventFunction;
        pMacCtrl->eventParam = 0;

        pMacCtrl->moduleId = TCPIP_MODULE_MAC_PIC32C;
        pMacCtrl->netIx = 0;
        pMacCtrl->macAction = TCPIP_MAC_ACTION_INIT;
        pMacCtrl->powerMode = TCPIP_MAC_POWER_FULL;

        macAdd.v[ 0 ] = configMAC_ADDR0;
        macAdd.v[ 1 ] = configMAC_ADDR1;
        macAdd.v[ 2 ] = configMAC_ADDR2;
        macAdd.v[ 3 ] = configMAC_ADDR3;
        macAdd.v[ 4 ] = configMAC_ADDR4;
        macAdd.v[ 5 ] = configMAC_ADDR5;

        if( memcmp( macAdd.v, unsetMACAddr, sizeof( unsetMACAddr ) ) == 0 )
        { /* if unspecified we use the system config address */
            TCPIP_Helper_StringToMACAddress(TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX0, pMacCtrl->ifPhyAddress.v);
        }
        else
        { /* use the config suggested one */
            memcpy( pMacCtrl->ifPhyAddress.v, macAdd.v, sizeof( macAdd ) );
        }
    }

    /*-----------------------------------------------------------*/

    static bool MacSyncFunction( void * synchHandle,
                                 TCPIP_MAC_SYNCH_REQUEST req )
    {
        switch( req )
        {
            case TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE:
                vSemaphoreCreateBinary( *( SemaphoreHandle_t * ) synchHandle );

                return ( *( SemaphoreHandle_t * ) synchHandle == NULL ) ? false : true;

            case TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE:
                vSemaphoreDelete( *( SemaphoreHandle_t * ) synchHandle );
                *( SemaphoreHandle_t * ) synchHandle = NULL;

                return true;

            case TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK:

                return ( xSemaphoreTake( *( SemaphoreHandle_t * ) synchHandle, portMAX_DELAY ) == pdTRUE ) ? true : false;

            case TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK:

                return ( xSemaphoreGive( *( SemaphoreHandle_t * ) synchHandle ) == pdTRUE ) ? true : false;

            case TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER:
                vTaskSuspendAll();

                return true;

            case TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE:
                xTaskResumeAll();

                return true;

            default:

                return false;
        }
    }


    /*-----------------------------------------------------------*/

    static void MacHandlerTask( void * params )
    {
        EventBits_t evBits;

        /* perform the MAC initialization */
        while( macObjStatus == SYS_STATUS_BUSY )
        {
            /* process the underlying MAC module tasks */
            ( macObject->TCPIP_MAC_Tasks )( macObjHandle );

            SYS_STATUS macStatus = ( macObject->TCPIP_MAC_Status )( macObjHandle );

            if( macStatus == SYS_STATUS_BUSY )
            { /* still pending */
                vTaskDelay( PIC32_MAC_TASK_INIT_PENDING_DELAY );
            }
            else
            { /* completed ...somehow */
                macObjStatus = macStatus;

                xTaskNotify( ( TaskHandle_t ) params, PIC32_MAC_EVENT_INIT_DONE, eSetBits );

                if( macStatus != SYS_STATUS_READY )
                { /* failed miserably */
                    vTaskDelete( 0 );
                }

                /* done, up and running */
            }
        }

        while( true )
        {
            xTaskNotifyWait( PIC32_MAC_EVENT_TIMEOUT | PIC32_MAC_EVENT_IF_PENDING, PIC32_MAC_EVENT_TIMEOUT | PIC32_MAC_EVENT_IF_PENDING, &evBits, portMAX_DELAY );

            if( ( evBits & PIC32_MAC_EVENT_TIMEOUT ) != 0 )
            {                                                                       /* timeout occurred... */
                ( macObject->TCPIP_MAC_Tasks )( macObjHandle );
                bool linkCurr = ( macObject->TCPIP_MAC_LinkCheck )( macCliHandle ); /* check link status */

                if( macLinkStatus != linkCurr )
                { /* link status changed; some event could ve fired here if needed */
                    PIC32_MAC_DbgPrint( " MAC link: %s!\r\n", linkCurr ? "ON" : "OFF" );
                    macLinkStatus = linkCurr;
                }
            }

            if( ( evBits & PIC32_MAC_EVENT_IF_PENDING ) != 0 )
            { /* IF events signal */
                TCPIP_MAC_EVENT activeEvents = ( macObject->TCPIP_MAC_EventPendingGet )( macCliHandle );

                if( activeEvents != TCPIP_MAC_EV_NONE )
                {
                    /* acknowledge the events */
                    ( macObject->TCPIP_MAC_EventAcknowledge )( macCliHandle, activeEvents );

                    /* check for RX */
                    if( ( activeEvents & ( TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA ) ) != 0 )
                    { /* RX packets available */
                        MacRxPackets();
                    }

                    /* call the driver process function; */
                    /* PIC32 driver requests it through TCPIP_MAC_ParametersGet() which is bypassed here! */
                    ( macObject->TCPIP_MAC_Process )( macCliHandle );
                }
            }

            /* do what you have to do and then wait for another event... */
        }
    }

    /*-----------------------------------------------------------*/

    static void MacTmrCallback( TimerHandle_t xTimer )
    {
        if(macTaskHandle)
        {
            xTaskNotify( macTaskHandle, PIC32_MAC_EVENT_TIMEOUT, eSetBits );
        }
    }

    /* MAC interrupt event function */
    /* MAC signals an event, probably from within ISR */
    /* we care just for RX related events */
    static void MAC_EventFunction( TCPIP_MAC_EVENT event,
                                   const void * eventParam )
    {
        BaseType_t xHigherPriorityTaskWoken;

        if( ( event & ( TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_TX_DONE | TCPIP_MAC_EV_RXTX_ERRORS ) ) != 0 )
        {
            xHigherPriorityTaskWoken = pdFALSE;
            xTaskNotifyFromISR( macTaskHandle, PIC32_MAC_EVENT_IF_PENDING, eSetBits, &xHigherPriorityTaskWoken );

            if( xHigherPriorityTaskWoken )
            {
                portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
            }
        }
    }

    /*-----------------------------------------------------------*/

    BaseType_t xGetPhyLinkStatus( void )
    {
        return macLinkStatus == true ? pdPASS : pdFAIL;
    }


    /* receive packets from the MAC driver */
    static void MacRxPackets( void )
    {
        TCPIP_MAC_PACKET * pRxPkt;

        /* get all the new MAC packets */
        while( ( pRxPkt = ( macObject->TCPIP_MAC_PacketRx )( macCliHandle, 0, 0 ) ) != 0 )
        {
            MacProcessRxPacket( pRxPkt );
        }
    }

    /*-----------------------------------------------------------*/

    static void MacProcessRxPacket( TCPIP_MAC_PACKET * pRxPkt )
    {
        bool pktSuccess, pktLost;
        size_t pktLength;
        TCPIP_MAC_DATA_SEGMENT * pSeg;
        uint8_t * pPktBuff;
        NetworkBufferDescriptor_t * pxBufferDescriptor;
        IPStackEvent_t xRxEvent;

        pxBufferDescriptor = 0;
        pktSuccess = pktLost = false;

        while( true )
        {
            pktLength = 0;
            int nSegs = 0;
            pSeg = pRxPkt->pDSeg;
            pPktBuff = pSeg->segLoad;

            /* calculate the packet size */
            do
            {
                pktLength += pSeg->segLen;
                pSeg = pSeg->next;
                nSegs++;
            } while( pSeg != 0 );

            if( nSegs > 1 )
            { /* no support in FreeRTOS for multi segment packets! */
                break;
            }

            /* sizeof(TCPIP_MAC_ETHERNET_HEADER) is subtracted by the driver */
            /* but FreeRTOS needs the whole frame! */
            pktLength += sizeof( TCPIP_MAC_ETHERNET_HEADER );

            if( eConsiderFrameForProcessing( pPktBuff ) != eProcessBuffer )
            {
                break;
            }

            /* get the network descriptor (no data buffer) to hold this packet */
            pxBufferDescriptor = pxGetNetworkBufferWithDescriptor( 0, 0 );

            if( pxBufferDescriptor == 0 )
            {
                pktLost = true;
                break;
            }

            PIC32_MacAssociate( pRxPkt, pxBufferDescriptor, pktLength );

            xRxEvent.eEventType = eNetworkRxEvent;
            xRxEvent.pvData = ( void * ) pxBufferDescriptor;

            /* Send the data to the TCP/IP stack */
            if( xSendEventStructToIPTask( &xRxEvent, 0 ) == pdFALSE )
            { /* failed */
                pktLost = true;
            }
            else
            { /* success */
                pktSuccess = true;
                iptraceNETWORK_INTERFACE_RECEIVE();
            }

            break;
        }

        if( !pktSuccess )
        { /* smth went wrong; nothing sent to the */
            if( pxBufferDescriptor != 0 )
            {
                pxBufferDescriptor->pucEthernetBuffer = 0;
                vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );
            }

            if( pktLost )
            {
                iptraceETHERNET_RX_EVENT_LOST();
            }

            /* acknowledge the packet to the MAC driver */
            if( pRxPkt->ackFunc )
            {
                ( *pRxPkt->ackFunc )( pRxPkt, pRxPkt->ackParam );
            }
            else
            {
                PIC32_MacPacketOrphan( pRxPkt );
            }
        }
    }

    #if ( PIC32_MAC_DEBUG_COMMANDS != 0 )
        /* */
        static int _Command_MacInfo( SYS_CMD_DEVICE_NODE * pCmdIO,
                                     int argc,
                                     char ** argv )
        {
            TCPIP_MAC_RES macRes;
            TCPIP_MAC_RX_STATISTICS rxStatistics;
            TCPIP_MAC_TX_STATISTICS txStatistics;
            TCPIP_MAC_STATISTICS_REG_ENTRY regEntries[ 8 ];
            TCPIP_MAC_STATISTICS_REG_ENTRY * pRegEntry;
            int jx, hwEntries;
            char entryName[ sizeof( pRegEntry->registerName ) + 1 ];

            const void * cmdIoParam = pCmdIO->cmdIoParam;

            if( argc != 1 )
            {
                ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "Usage: macinfo \r\n" );
                ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "Ex: macinfo \r\n" );

                return false;
            }

            ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Interface: %s driver statistics\r\n", macObject->macName );
            macRes = ( macObject->TCPIP_MAC_StatisticsGet )( macCliHandle, &rxStatistics, &txStatistics );

            if( macRes == TCPIP_MAC_RES_OK )
            {
                ( *pCmdIO->pCmdApi->print )( cmdIoParam, "\tnRxOkPackets: %d, nRxPendBuffers: %d, nRxSchedBuffers: %d, ",
                                             rxStatistics.nRxOkPackets, rxStatistics.nRxPendBuffers, rxStatistics.nRxSchedBuffers );
                ( *pCmdIO->pCmdApi->print )( cmdIoParam, "nRxErrorPackets: %d, nRxFragmentErrors: %d\r\n", rxStatistics.nRxErrorPackets, rxStatistics.nRxFragmentErrors );
                ( *pCmdIO->pCmdApi->print )( cmdIoParam, "\tnTxOkPackets: %d, nTxPendBuffers: %d, nTxErrorPackets: %d, nTxQueueFull: %d\r\n",
                                             txStatistics.nTxOkPackets, txStatistics.nTxPendBuffers, txStatistics.nTxErrorPackets, txStatistics.nTxQueueFull );
            }
            else
            {
                ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "\tnot supported\r\n" );
            }

            ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Interface: %s hardware statistics\r\n", macObject->macName );
            macRes = ( macObject->TCPIP_MAC_RegisterStatisticsGet )( macCliHandle, regEntries, sizeof( regEntries ) / sizeof( *regEntries ), &hwEntries );

            if( macRes == TCPIP_MAC_RES_OK )
            {
                entryName[ sizeof( entryName ) - 1 ] = 0;

                for( jx = 0, pRegEntry = regEntries; jx < hwEntries && jx < sizeof( regEntries ) / sizeof( *regEntries ); jx++, pRegEntry++ )
                {
                    strncpy( entryName, pRegEntry->registerName, sizeof( entryName ) - 1 );
                    ( *pCmdIO->pCmdApi->print )( cmdIoParam, "\t%s: 0x%8x\r\n", entryName, pRegEntry->registerValue );
                }
            }
            else
            {
                ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "\tnot supported\r\n" );
            }

            return true;
        }

        /*-----------------------------------------------------------*/

        static int _Command_NetInfo( SYS_CMD_DEVICE_NODE * pCmdIO,
                                     int argc,
                                     char ** argv )
        {
            const void * cmdIoParam = pCmdIO->cmdIoParam;

            union
            {
                uint32_t ul;
                uint8_t b[ 4 ];
            }
            sUl;

            sUl.ul = FreeRTOS_GetIPAddress();

            bool linkUp = FreeRTOS_IsNetworkUp() == pdTRUE;

            ( *pCmdIO->pCmdApi->print )( cmdIoParam, "IP address: %d.%d.%d.%d\r\n", sUl.b[ 0 ], sUl.b[ 1 ], sUl.b[ 2 ], sUl.b[ 3 ] );
            ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Link is: %s\r\n", linkUp ? "Up" : "Down" );

            return true;
        }

    #include "aws_application_version.h"

    static int _Command_Version(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
    {
        configPRINTF( ( "App version - maj: %d, min: %d, build: %d\r\n",  xAppFirmwareVersion.u.x.ucMajor, xAppFirmwareVersion.u.x.ucMinor, xAppFirmwareVersion.u.x.usBuild) );
        return 0;
    }

    #endif /* (PIC32_MAC_DEBUG_COMMANDS != 0) */

#if (PIC32_FLASH_PKCS_COMMANDS != 0)

uint32_t writeBuff[2048 / 4];
uint32_t readBuff[2048 / 4];

extern BaseType_t prvSaveFile( char * pcFileName, uint8_t * pucData, uint32_t ulDataSize );
extern BaseType_t prvReadFile( char * pcFileName, uint8_t ** ppucData, uint32_t * pulDataSize );

static uint32_t aaXorCount = 0;

static int _Command_Pkcs11(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv )
{
    const void * cmdIoParam = pCmdIO->cmdIoParam;
    if( argc != 2 )
    {
        ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "Usage: pkcs 0/1/2/3 \r\n" );
        return false;
    }


    int certIx = atoi(argv[1]);
    char* certName;
    switch(certIx)
    {
        case 0:
            certName = "FreeRTOS_P11_Certificate.dat";
            break;
        case 1:
            certName = "FreeRTOS_P11_Key.dat";
            break;
        case 2:
            certName = "FreeRTOS_Publisher_Certificate.dat";
            break;
        default:
            certName = "FreeRTOS_Publisher_Key.dat";
            break;
    }


    int ix;
    uint32_t* ptr = writeBuff;
    for(ix = 0; ix < sizeof(writeBuff) / 4; ix++)
    {
        *ptr++ = rand() ^ aaXorCount++;
    }

    // write certificate
    BaseType_t res = prvSaveFile( certName, (uint8_t*)writeBuff, sizeof(writeBuff) );
    ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Write certificate: %d, %s\r\n", certIx, res == pdTRUE ? "success" : "Failed!" );


    // read back
    uint32_t readSize;
    uint8_t* ptrData;
    res = prvReadFile( certName, &ptrData, &readSize );
    ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Read certificate - size: %d, %s\r\n", readSize, res == pdTRUE ? "success" : "Failed!" );

    // compare
    if(res == pdTRUE && readSize == sizeof(readBuff))
    {
        memcpy(readBuff, ptrData, readSize);

        bool matchFail = false;
        uint32_t* ptr1 = writeBuff;
        uint32_t* ptr2 = readBuff;
        for(ix = 0; ix < sizeof(writeBuff) / 4; ix++)
        {
            if(*ptr1++ != *ptr2++)
            {
                matchFail = true;
                break;
            }
        }

        ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Write/Read compare: %s\r\n", matchFail ? "Failed!" : "success" );
    }
    else
    {
        ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "No Write/Read compare because of res/readSize\r\n");
    }


    return true;

}

extern bool AWS_FlashPagesErase(const uint32_t* ptrFlash, int nPages);
extern bool AWS_FlashPagesWrite(const uint32_t* ptrFlash, const uint32_t* pageData, int nPages);
extern bool AWS_FlashRegionLock(const uint32_t* ptrFlash);
extern bool AWS_FlashRegionUnlock(const uint32_t* ptrFlash);
extern bool AWS_FlashRegionProtect(int regionNo, uint32_t regionSize, const void* flashAddress, bool enable);

#define FLASH_PAGE_SIZE         512     // size of flash page
#define FLASH_PAGES_NO          32      // 32 pages assigned to our test
#define FLASH_MPU_REGION        12      // assigned MPU region
#define FLASH_MPU_REGION_SIZE   (FLASH_PAGES_NO * FLASH_PAGE_SIZE)      // region size


uint32_t* writeFlashAddr = (uint32_t*)0x5fc000;


uint32_t writePageBuff[4 * FLASH_PAGE_SIZE / 4];

static int _Command_Write(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv )
{
    const void * cmdIoParam = pCmdIO->cmdIoParam;

    int ix;
    uint32_t* ptr = writePageBuff;
    for(ix = 0; ix < sizeof(writePageBuff) / 4; ix++)
    {
        *ptr++ = rand() ^ aaXorCount++;
    }

    AWS_FlashRegionProtect(FLASH_MPU_REGION, FLASH_MPU_REGION_SIZE, writeFlashAddr, false);
    AWS_FlashRegionUnlock(writeFlashAddr);

    AWS_FlashPagesErase(writeFlashAddr, FLASH_PAGES_NO);

    int nPages = sizeof(writePageBuff) / FLASH_PAGE_SIZE;
    // start writing
    AWS_FlashPagesWrite(writeFlashAddr, writePageBuff, nPages);

    AWS_FlashRegionLock(writeFlashAddr);
    AWS_FlashRegionProtect(FLASH_MPU_REGION, FLASH_MPU_REGION_SIZE, writeFlashAddr, true);

    // read back and compare
    bool matchFail = false;
    uint32_t* ptrW = writePageBuff;
    uint32_t* ptrR = writeFlashAddr;
    for(ix = 0; ix < sizeof(writePageBuff) / 4; ix++)
    {
        if(*ptrR++ != *ptrW++)
        {   // failed
            matchFail = true;
            break;
        }
    }

    if(matchFail == false)
    {
        ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "Write successful!\r\n");
    }
    else
    {
        ptrW--; ptrR--;
        ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Write FAILED! mismatch at address: 0x%08x\r\n", ptrR);
        ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Expected: 0x%08x, got: 0x%08x\r\n", *ptrW, *ptrR);

    }



    return true;
}


static int _Command_Erase(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv )
{
    const void * cmdIoParam = pCmdIO->cmdIoParam;

    AWS_FlashRegionProtect(FLASH_MPU_REGION, FLASH_MPU_REGION_SIZE, writeFlashAddr, false);
    AWS_FlashRegionUnlock(writeFlashAddr);

    bool eraseRes = AWS_FlashPagesErase(writeFlashAddr, FLASH_PAGES_NO);

    AWS_FlashRegionLock(writeFlashAddr);
    AWS_FlashRegionProtect(FLASH_MPU_REGION, FLASH_MPU_REGION_SIZE, writeFlashAddr, true);

    ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Erase res: %d\r\n", eraseRes);
    return true;
}

static int _Command_BlankCheck(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv )
{
    const void * cmdIoParam = pCmdIO->cmdIoParam;


    // check the erase result
    int ix;
    uint32_t* ptr = writeFlashAddr;
    bool eraseMatch = true;
    for(ix = 0; ix < FLASH_MPU_REGION_SIZE / 4; ix++)
    {
        if(*ptr++ != 0xffffffff)
        {
            eraseMatch = false;
            break;
        }
    }

    if(eraseMatch)
    {
        ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "Blank check successful! All 0xff's.\r\n");
    }
    else
    {
        uint32_t* pSamplePtr[3]; // take some data samples

        pSamplePtr[0] = --ptr;
        pSamplePtr[1] = ((ptr + 8)  < writeFlashAddr + FLASH_MPU_REGION_SIZE / 4) ? ptr + 8 : 0;
        pSamplePtr[2] = ((ptr + 16) < writeFlashAddr + FLASH_MPU_REGION_SIZE / 4) ? ptr + 16 : 0;

        ( *pCmdIO->pCmdApi->msg )( cmdIoParam, "Blank check FAILED!\r\n");

        for(ix = 0; ix < sizeof(pSamplePtr) / sizeof(*pSamplePtr); ix++)
        {
            if((ptr = pSamplePtr[ix]) != 0)
            {
                ( *pCmdIO->pCmdApi->print )( cmdIoParam, "Blank address: 0x%08x found: 0x%08x\r\n", ptr, *ptr );
            }
        }
    }

    return true;
}

#endif  // (PIC32_FLASH_PKCS_COMMANDS != 0)

#endif /* #ifdef PIC32_USE_ETHERNET */
