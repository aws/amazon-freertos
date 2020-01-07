/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/**
 * Andy McClelland (AWM) March 2015
 * This module provides interface functions to the LPC40x8 Ethernet peripheral
 * for the FreeRTOS+ implementation of a TCP/IP stack.
 * It makes extensive use of the NXP provided LPCOpen chip/board support packages.
 * It is quite probable that this driver will work for the LPC17xx devices too
 * with little or no modification.
 */

/**
 * Peter Wilks (PW) October 2018
 * Updated for current FreeRTOS-TCP
 * The inclusion of the FreeRTOS-TCP copyright notice above is intended to indicate that this
 * software may be used under the same terms and conditions as the FreeRTOS-TCP software.
 */

/**
 * Ulysses Fonseca January 2020
 * Inclusion of the FreeRTOSConfig.h to get the defines of lpc.
 * Changed the way to check the PHY and set FreeRTOS_NetworkDown or FreeRTOS_NetworkUp, for do this faster.
 * Inclusion of the create the task ethernet by configSUPPORT_DYNAMIC_ALLOCATION too.
 * Enabled broadcast frames on driver for DHCP.
 */

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"
#include "FreeRTOSConfig.h"

/* LPCOpen includes. */
#include "chip.h"
#include "board.h"

#ifdef __CODE_RED
#include <cr_section_macros.h>
#endif

/*****************************************************************************
 * Physical interface
 ****************************************************************************/
/* The PHY address connected the to MII/RMII */
#define ENET_PHYDEF_PHYADDR      1

static void prvDelay(uint32_t ulMilliSeconds);
static BaseType_t xPhyConfigure(void);
static BaseType_t xPhyCheckLinkStatus();

/*****************************************************************************
 *
 ****************************************************************************/

/** @brief Receive group interrupts */
#define RXINTGROUP (ENET_INT_RXOVERRUN | ENET_INT_RXERROR | ENET_INT_RXDONE | ENET_INT_RXFINISHED)

/** @brief Transmit group interrupts */
#define TXINTGROUP (ENET_INT_TXUNDERRUN | ENET_INT_TXERROR | ENET_INT_TXDONE)

/* Some configuration defaults for when values have not been defined */
#ifndef iplpcUSE_RMII
   #define iplpcUSE_RMII 1
#endif

#ifndef iplpcINIT_PORT_PINS
   #define iplpcINIT_PORT_PINS 0
#endif

#if (iplpcNUM_RX_DESCRIPTORS + iplpcNUM_TX_DESCRIPTORS) > ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS
#error "(iplpcNUM_RX_DESCRIPTORS + iplpcNUM_TX_DESCRIPTORS) > ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS"
#endif

/* If ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES is set to 1, then the Ethernet
 * driver will filter incoming packets and only pass the stack those packets it
 * considers need processing. */
/* AWM Note - This driver sets up and enables the HW filtering capabilities of the MAC,
 * so SW filtering should be unnecessary.  Perhaps an ipconfigXXXXXX define could be
 * implemented to select HW and/or SW filtering. */

/* LPC EMAC driver data structure */
/* AWM - A comment in the original LPCOpen code example from which this was developed, stated:
 * "The Ethernet Block can only access Peripheral SRAM and External Memory. In this example,
 * Peripheral SRAM is selected for storing descriptors, status arrays and send/receive buffers." */
typedef struct
{
   ENET_RXSTAT_T RxStatus[iplpcNUM_RX_DESCRIPTORS];       /* RX status list */
   ENET_RXDESC_T RxDescriptor[iplpcNUM_RX_DESCRIPTORS];   /* RX descriptor list */
   ENET_TXSTAT_T TxStatus[iplpcNUM_TX_DESCRIPTORS];       /* TX status list */
   ENET_TXDESC_T TxDescriptor[iplpcNUM_TX_DESCRIPTORS];   /* TX descriptor list */
} EnetData_t;
EnetData_t VacEnetData __attribute__ ((aligned(8))) __BSS(RAM2);
typedef struct
{
   uint8_t RxBuffer[ENET_ETH_MAX_FLEN];
} EnetRxBuffer_t;
EnetRxBuffer_t EnetRxBuffer[iplpcNUM_RX_DESCRIPTORS] __attribute__ ((aligned(8))) __BSS(RAM2);

/* Initialises the Tx and Rx descriptors respectively. */
static void prvSetupTxDescriptors(EnetData_t *EnetData);
static void prvSetupRxDescriptors(EnetData_t *EnetData);

/* Semaphore used to indicate that there is received data to be processed */
static SemaphoreHandle_t EthernetRxSem;

/* The task that receives the semaphore and does the processing */
static void prvEMACDeferredInterruptHandlerTask(void *pvParameters);

/* To indicate to the output task that transmission is complete and the buffer can be released */
static SemaphoreHandle_t EthernetTxDoneSem;

static BaseType_t LinkUp = pdFALSE;  // == pdTRUE when the link is up

/* These three functions update the hash table to allow a MAC address. */
static uint32_t prvGenerateCRC32(const uint8_t *ucAddress);
static uint32_t prvGetHashIndex(const uint8_t *ucAddress);
static void prvAddMACAddress(const uint8_t* ucMacAddress);

/* LLMNR is Link Local Multicast Name Resolution, which uses a multicast MAC address of
 * 01-00-5E-00-00-FC
 * See RFC4795: http://tools.ietf.org/html/rfc4795 */
#if( ipconfigUSE_LLMNR == 1 )
static const uint8_t xLLMNR_MACAddress[] = {'\x01', '\x00', '\x5E', '\x00', '\x00', '\xFC'};
#endif  /* ipconfigUSE_LLMNR == 1 */

/* The MAC address, must be defined externally */
extern uint8_t ucMACAddress[6];

/*****************************************************************************
 * Local functions
 ****************************************************************************/

#if ( iplpcINIT_PORT_PINS == 1 )
/**
 * Sets up the Ethernet interface pins for either MII or RMII.
 * I'd rather see it in the board.c module, but the lpc_board_ea_devkit_4088
 * version doesn't have it, so for compatibility, I have put it here.
 */
static void prvBoard_ENET_Init(bool useRMII)
{
   /* Set up those required for both MII and RMII */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 0, (IOCON_FUNC1 | IOCON_MODE_INACT));  /* TXD0 */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 1, (IOCON_FUNC1 | IOCON_MODE_INACT));  /* TXD1 */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 4, (IOCON_FUNC1 | IOCON_MODE_INACT));  /* TXEN */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 8, (IOCON_FUNC1 | IOCON_MODE_INACT));  /* CRS/DV */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 9, (IOCON_FUNC1 | IOCON_MODE_INACT));  /* RXD0 */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 10, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* RXD1 */

   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 14, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN) );   /* RX_ER */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 15, (IOCON_FUNC1 | IOCON_MODE_INACT));                       /* RX_CLK */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 16, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));    /* MDC */
   Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 17, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));    /* MDIO */

   /* If required set up the additional MII pins */
   if (!useRMII)
   {
      Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 2, (IOCON_FUNC1 | IOCON_MODE_INACT));                     /* TXD2 */
      Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 3, (IOCON_FUNC1 | IOCON_MODE_INACT));                     /* TXD3 */
      Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 5, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));  /* TX_ER */
      Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 6, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));  /* TX_CLK */
      Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 7, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));  /* COL */
      Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 11, (IOCON_FUNC1 | IOCON_MODE_INACT));                    /* RXD2 */
      Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 12, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)); /* RXD3 */
      Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 13, (IOCON_FUNC1 | IOCON_MODE_INACT));                    /* RX_DV */
   }
}
#endif // ( iplpcINIT_PORT_PINS == 1 )

/**
 * Receive Interrupt Handling Task
 * Based heavily on the example at:
 *  http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Porting.html
 *  (See prvEMACDeferredInterruptHandlerTask)
 */
static void prvEMACDeferredInterruptHandlerTask(void *pvParameters)
{
   TickType_t xPollPeriod = iplpcLINK_RECEIVE_PERIOD;
   NetworkBufferDescriptor_t *pxNetworkDescriptor;
   /* Set up the event type (an Rx event) */
   IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
   uint16_t ConsumeIdx;
   BaseType_t PhyLinkStatus;
   size_t xBytesReceived;
   char *buffer;

   /* Just to prevent compiler warnings about unused parameters. */
   (void) pvParameters;

   while (1)           // A FreeRTOS task should never exit.
   {
      /* Wait for the semaphore to be given by the IRQ handler.
       * If not given within the timeout then check link status. */
      if (xSemaphoreTake(EthernetRxSem, xPollPeriod) == pdPASS)
      {
         // AWM - TODO  handle OVERRUN errors, which need a soft reset of the Rx channel

         /* Check the buffer status, i.e. make sure that it is not empty */
         if (!Chip_ENET_IsRxEmpty(LPC_ETHERNET))
         {
            /* Get a new network buffer ready for the next frame to be received by HW */
            ConsumeIdx = Chip_ENET_GetRXConsumeIndex(LPC_ETHERNET);
            xBytesReceived = ENET_RINFO_SIZE(VacEnetData.RxStatus[ConsumeIdx].StatusInfo);
            pxNetworkDescriptor = pxGetNetworkBufferWithDescriptor(xBytesReceived, 0);

            /* Check it */
            if (pxNetworkDescriptor != NULL)
            {
               /* Check the status of the next DMA frame to be consumed
                * for either errors or zero size */
               if (!(VacEnetData.RxStatus[ConsumeIdx].StatusInfo & ENET_RINFO_ERR_MASK))
               {
                  // TODO ipCONSIDER_FRAME_FOR_PROCESSING (),
                  // probably not be necessary if HW filtering is used.

                  /* If the link isn't up, we shouldn't process the packet in the IP task */
                  if (LinkUp == pdTRUE)
                  {
                     /* Xfer the information from the DMA descriptor to the network buffer descriptor */
                     memcpy(pxNetworkDescriptor->pucEthernetBuffer, (void*)VacEnetData.RxDescriptor[ConsumeIdx].Packet, xBytesReceived);;
                     pxNetworkDescriptor->xDataLength = xBytesReceived;

                     /* pvData is used to point to the network buffer descriptor that now references the received data. */
                     xRxEvent.pvData = (void *) pxNetworkDescriptor;
                     /* and send it to the IP task.  10-tick timeout.  TODO determine if a non-zero timeout is desirable */
                     if (xSendEventStructToIPTask(&xRxEvent, 10) == pdFAIL)
                     {
                        /* The buffer could not be sent to the IP task so the buffer must be released. */
                        vReleaseNetworkBufferAndDescriptor(pxNetworkDescriptor);
                        /* Make a call to the standard trace macro to log the occurrence. */
                        iptraceETHERNET_RX_EVENT_LOST();
                     }
                     else
                     {
                        /* The message was successfully sent to the TCP/IP stack.
                           Call the standard trace macro to log the occurrence. */
                        iptraceNETWORK_INTERFACE_RECEIVE();
                     }
                  }
                  else
                  {
                     /* Link is down, so release the buffer */
                     vReleaseNetworkBufferAndDescriptor(pxNetworkDescriptor);
                     /* and log the event */
                     iptraceNETWORK_DOWN();
                  }   // end if (LinkUp == pdTRUE)
               }
               else
               {
                  /* The frame contained errors, no processing was done, so release the buffer */
                  vReleaseNetworkBufferAndDescriptor(pxNetworkDescriptor);
                  //DEBUGOUT("prvEMACDeferredInterruptHandlerTask: iptraceNETWORK_EVENT_ERRORS() %08X\r\n", VacEnetData.RxStatus[ConsumeIdx].StatusInfo);
               }   // end if (!( (VacEnetData.prxs[ConsumeIdx].StatusInfo ...
            }
            else
            {
               /* Descriptor pointer was NULL, i.e. one was not allocated */
               iptraceFAILED_TO_OBTAIN_NETWORK_BUFFER();
               iptraceETHERNET_RX_EVENT_LOST();
               //DEBUGSTR("prvEMACDeferredInterruptHandlerTask: DIDN'T GET A BUFFER!\r\n");
            }   // end if (pxNetworkDescriptor != NULL)

            /* Update the receive consumer DMA descriptor index, freeing it up. */
            ConsumeIdx = Chip_ENET_IncRXConsumeIndex(LPC_ETHERNET);
         }
         else
         {
            /* DMA buffer was empty, we didn't need to allocate a network buffer. */
            //DEBUGSTR("prvEMACDeferredInterruptHandlerTask: iptraceNETWORK_EVENT_EMPTY()\r\n");
         }   // end if (!Chip_ENET_IsRxEmpty (LPC_ETHERNET))

         xPollPeriod = iplpcLINK_RECEIVE_PERIOD;
      }
      else
      {
         /* Timed out waiting for a packet to come in, so check the link status. */
         PhyLinkStatus = xPhyCheckLinkStatus();

         /* Any kind of change probably requires a restart, but if it hasn't changed,
          * then we shouldn't have to call FreeRTOS_NetworkDown() again.
          * However, it could be that when +TCP calls xNetworkInterfaceInitialise(),
          * the status will change and cause this task to call FreeRTOS_NetworkDown() again.
          * One to watch out for perhaps. */
         if((PhyLinkStatus & PHY_LINK_CHANGED)){
        	 if (!(PhyLinkStatus & PHY_LINK_CONNECTED))
        	 {
        		 /* Guard using LinkUp state. */
        		 if (LinkUp == pdTRUE)
        		 {
        			 LinkUp = pdFALSE;
        			 /* FreeRTOS_NetworkDown() will:
        			  * a) Call vApplicationIPNetworkEventHook(eNetworkDown)
        			  * b) Call xNetworkInterfaceInitialise() */
        			 FreeRTOS_NetworkDown();
        		 }
        		 /*  Decrease the polling rate (semaphore timeout) now we know the link is down */
        		 xPollPeriod = iplpcLINK_RECEIVE_PERIOD;
        	 }
        	 else
        	 {
        		 /* Increase the polling rate (semaphore timeout) to speed things up
        		  * until we either receive a packet or the link is confirmed as down */
        		 xPollPeriod = iplpcLINK_POLL_PERIOD;
        	 }
         }
      }   // End if (xSemaphoreTake ...)
   }   // End while (1)
}

static void prvDelay(uint32_t ulMilliSeconds)
{
   /* Ensure the scheduler was started before attempting to use the scheduler to create a delay. */
   configASSERT(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);
   if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
   {
      vTaskDelay(pdMS_TO_TICKS(ulMilliSeconds));
   }
}

/* Set up the transmit channel DMA descriptors and point the Ethernet peripheral to them */
static void prvSetupTxDescriptors(EnetData_t *pEnetData)
{
   int32_t idx;

   /* Build TX descriptors for local buffers */
   for (idx = 0; idx < iplpcNUM_TX_DESCRIPTORS; idx++)
   {
      pEnetData->TxDescriptor[idx].Packet = (uint32_t) NULL;
      pEnetData->TxDescriptor[idx].Control = 0;
      pEnetData->TxStatus[idx].StatusInfo = 0xFFFFFFFF;
   }

   /* Setup pointers to TX structures */
   Chip_ENET_InitTxDescriptors(LPC_ETHERNET, pEnetData->TxDescriptor, pEnetData->TxStatus, iplpcNUM_TX_DESCRIPTORS);
}

/* Set up the receive channel DMA descriptors and point the Ethernet peripheral to them */
static void prvSetupRxDescriptors(EnetData_t *pEnetData)
{
   int32_t idx;

   /* Initialise them */
   for (idx = 0; idx < iplpcNUM_RX_DESCRIPTORS; idx++)
   {
      /* Get a buffer address and allocate it to a descriptor */
      pEnetData->RxDescriptor[idx].Packet = (uint32_t)EnetRxBuffer[idx].RxBuffer;
      /* Set the control word for the maximum expected number of bytes and to generate an interrupt  */
      pEnetData->RxDescriptor[idx].Control = ENET_RCTRL_SIZE(ENET_ETH_MAX_FLEN) | ENET_RCTRL_INT;
      /* Clear the status words */
      pEnetData->RxStatus[idx].StatusInfo = 0xFFFFFFFF;
      pEnetData->RxStatus[idx].StatusHashCRC = 0xFFFFFFFF;
   }

   Chip_ENET_InitRxDescriptors(LPC_ETHERNET, pEnetData->RxDescriptor, pEnetData->RxStatus, iplpcNUM_RX_DESCRIPTORS);
}

/* Taken from LPC18xx/NetworkInterface.c */
static uint32_t prvGenerateCRC32(const uint8_t *ucAddress)
{
   unsigned int j;
   const uint32_t Polynomial = 0xEDB88320;
   uint32_t crc = ~0ul;
   const uint8_t *pucCurrent = (const uint8_t *) ucAddress;
   const uint8_t *pucLast = pucCurrent + 6;

   /* Calculate  normal CRC32 */
   while (pucCurrent < pucLast)
   {
      crc ^= *(pucCurrent++);
      for (j = 0; j < 8; j++)
      {
         if ((crc & 1) != 0)
         {
            crc = (crc >> 1) ^ Polynomial;
         }
         else
         {
            crc >>= 1;
         }
      }
   }
   return ~crc;
}

/* Taken from LPC18xx/NetworkInterface.c */
static uint32_t prvGetHashIndex(const uint8_t *ucAddress)
{
   uint32_t ulCrc = prvGenerateCRC32(ucAddress);
   uint32_t ulIndex = 0ul;
   BaseType_t xCount = 6;

   /* Take the lowest 6 bits of the CRC32 and reverse them */
   while (xCount--)
   {
      ulIndex <<= 1;
      ulIndex |= (ulCrc & 1);
      ulCrc >>= 1;
   }

   /* This is the hash value of 'ucAddress' */
   return ulIndex;
}

/* Taken from LPC18xx/NetworkInterface.c */
static void prvAddMACAddress(const uint8_t* ucMacAddress)
{
   BaseType_t xIndex;

   xIndex = prvGetHashIndex(ucMacAddress);
   if (xIndex >= 32)
   {
      LPC_ETHERNET->RXFILTER.HashFilterH |= (1u << (xIndex - 32));
   }
   else
   {
      LPC_ETHERNET->RXFILTER.HashFilterL |= (1u << xIndex);
   }
}

/*****************************************************************************
 * Physical interface
 * This is a very thin wrapper around the functions in the NXP library.
 ****************************************************************************/

static BaseType_t xPhyConfigure(void)
{
   if (lpc_phy_init(iplpcUSE_RMII, prvDelay) == SUCCESS)
   {
      return pdPASS;
   }
   else
   {
      return pdFAIL;
   }
}

static BaseType_t xPhyCheckLinkStatus(void)
{
   return lpcPHYStsPoll();
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief   EMAC interrupt handler
 * @return  Nothing
 * @note    This function handles the transmit, receive, and error interrupts of
 *          the LPC17xx/40xx Ethernet peripheral.
 */
void ETH_IRQHandler(void)
{
   BaseType_t xRxTaskWoken = pdFALSE;
   BaseType_t xTxTaskWoken = pdFALSE;
   uint32_t ulInterrupts;

   /* Interrupts are of 2? groups - transmit or receive. Based on the
    interrupt, kick off the receive and/or transmit (cleanup) task.
    Doesn't distinguish between 'good' and 'bad' interrupts, that is
    left to the deferred tasks. */

   /* Get pending interrupts */
   ulInterrupts = Chip_ENET_GetIntStatus(LPC_ETHERNET);

   /* RX group interrupt(s) */
   if (ulInterrupts & RXINTGROUP)
   {
      /* Give semaphore to wakeup RX receive task.
       * Note the FreeRTOS semaphore method is used at present. */
      xSemaphoreGiveFromISR(EthernetRxSem, &xRxTaskWoken);
   }

   /* TX group interrupt(s) */
   if (ulInterrupts & TXINTGROUP)
   {
      /* Give semaphore to let transmit function know that it is complete.
       * Note the FreeRTOS semaphore method is used at present. */
      xSemaphoreGiveFromISR(EthernetTxDoneSem, &xTxTaskWoken);
   }

   // TODO Handle other interrupts

   /* Clear pending interrupts */
   Chip_ENET_ClearIntStatus(LPC_ETHERNET, ulInterrupts);

   /* Context switch needed? */
   portEND_SWITCHING_ISR(xRxTaskWoken || xTxTaskWoken);
}

#if 0 //PW:  Commented out, as only used by BufferAllocation_1.c
/* First statically allocate the buffers, ensuring an additional ipBUFFER_PADDING
bytes are allocated to each buffer.  This example makes no effort to align
the start of the buffers, but most hardware will have an alignment requirement.
If an alignment is required then the size of each buffer must be adjusted to
ensure it also ends on an alignment boundary.  Below shows an example assuming
the buffers must also end on an 8-byte boundary. */
#define BUFFER_SIZE ( ipTOTAL_ETHERNET_FRAME_SIZE + ipBUFFER_PADDING )
#define BUFFER_SIZE_ROUNDED_UP ( ( BUFFER_SIZE + 7 ) & ~0x07UL )
static uint8_t ucBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ][ BUFFER_SIZE_ROUNDED_UP ] __attribute__ ((aligned(4))) __BSS(RAM2);

/* Next provide the vNetworkInterfaceAllocateRAMToBuffers() function, which
simply fills in the pucEthernetBuffer member of each descriptor. */
void vNetworkInterfaceAllocateRAMToBuffers(
    NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{
   BaseType_t x;

   for (x = 0; x < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; x++)
   {
      /* pucEthernetBuffer is set to point ipBUFFER_PADDING bytes in from the
         beginning of the allocated buffer. */
      pxNetworkBuffers[x].pucEthernetBuffer = &(ucBuffers[x][ipBUFFER_PADDING]);

      /* The following line is also required, but will not be required in
         future versions. */
      *((uint32_t *)&ucBuffers[x][0]) = (uint32_t)&(pxNetworkBuffers[x]);
   }
}
#endif

/* See https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Porting.html
 *
 * xNetworkInterfaceInitialise() must prepare the Ethernet MAC to send and receive data.
 * In most cases this will just involve calling whichever initialise function is provided with
 * the Ethernet MAC peripheral drivers - which will in turn ensure the MAC hardware is enabled
 * and clocked, as well as configure the MAC peripheral's DMA descriptors.
 *
 * xNetworkInterfaceInitialise() does not take any parameters, returns pdPASS if the initialisation was successful,
 * and returns pdFAIL if the initialisation fails.
 */
BaseType_t xNetworkInterfaceInitialise(void)
{
   BaseType_t xReturn = pdFAIL;
   static bool RxTaskCreated = pdFALSE; /* Guard against creating more than one receive task */
   static bool HwInitialised = pdFALSE;
   BaseType_t LinkStatus = 0;
   BaseType_t RxFilterControl = 0;
   TickType_t xLastWakeTime = xTaskGetTickCount(); /* Initialise with the current time. */

   if (HwInitialised == pdFALSE)
   {
#if ( iplpcINIT_PORT_PINS == 1 )
      /* Start by setting up the pins for the desired MAC-PHY interface type. */
      prvBoard_ENET_Init(configUSE_RMII);
#endif // ( iplpcINIT_PORT_PINS == 1 )

      /* Initialise the peripheral using MII or RMII according to configuration */
      Chip_ENET_Init(LPC_ETHERNET, iplpcUSE_RMII);

      /* Initialise the PHY clock for 2.5MHz and address */
      Chip_ENET_SetupMII(LPC_ETHERNET,
                         Chip_ENET_FindMIIDiv(LPC_ETHERNET, 2500000),
                         ENET_PHYDEF_PHYADDR);

      /* Initialise the external PHY */
      if (xPhyConfigure() == pdPASS)
      {
         /* Save MAC address */
         Chip_ENET_SetADDR(LPC_ETHERNET, ucMACAddress);

         /* Clear all MAC address hash entries. */
         LPC_ETHERNET->RXFILTER.HashFilterH = 0;
         LPC_ETHERNET->RXFILTER.HashFilterL = 0;

         /* Add the station MAC address to the filter hash table */
         prvAddMACAddress(ucMACAddress);

#if (ipconfigUSE_LLMNR == 1)
         /* Add the LLMNR MAC address to the filter hash table */
         prvAddMACAddress (xLLMNR_MACAddress);
#endif /* ipconfigUSE_LLMNR == 1 */

         /* Enable packet reception according to configuration requirements. */
         // AWM - for testing, we'll accept anything. TODO sort out config #defines
         RxFilterControl = ENET_RXFILTERCTRL_APE; // Enable "perfect frames" as standard
         //        RxFilterControl |= ENET_RXFILTERCTRL_AUE;        // Enable unicast frames
         //        RxFilterControl |= ENET_RXFILTERCTRL_ABE;        // Enable broadcast frames
         //        RxFilterControl |= ENET_RXFILTERCTRL_AME;        // Enable multicast frames
         /* If the hash filters are non-zero, we are planning to use them, so enable
          * multi- and unicast frames */
         if ((LPC_ETHERNET->RXFILTER.HashFilterH != 0) || (LPC_ETHERNET->RXFILTER.HashFilterL != 0))
         {
            RxFilterControl |= ENET_RXFILTERCTRL_AMHE | ENET_RXFILTERCTRL_AUHE | ENET_RXFILTERCTRL_ABE;
         }

#if (ipconfigUSE_LLMNR)
         RxFilterControl |= ENET_RXFILTERCTRL_AMHE;     // Multicast hash accept
#endif
         Chip_ENET_EnableRXFilter(LPC_ETHERNET, RxFilterControl);

         /* Setup transmit and receive descriptors */
         prvSetupTxDescriptors(&VacEnetData);
         prvSetupRxDescriptors(&VacEnetData);

         /* Hardware successfully initialised */
         HwInitialised = pdTRUE;
      }
   }   // end if (HwInitialised)

   if (HwInitialised == pdTRUE)
   {
      /* Create the Ethernet receive semaphore and start up the Ethernet receive handling task
       *  if it hasn't already been started */
      if (RxTaskCreated != pdTRUE)
      {
#if (configSUPPORT_STATIC_ALLOCATION==1)
         static StackType_t xStack[configMINIMAL_STACK_SIZE];
         static StaticTask_t xTaskBuffer;
#endif

         EthernetRxSem = xSemaphoreCreateCounting(iplpcNUM_RX_DESCRIPTORS, 0);
         vQueueAddToRegistry(EthernetRxSem, "EnetRx");

#if (configSUPPORT_STATIC_ALLOCATION==1)
         xTaskCreateStatic(prvEMACDeferredInterruptHandlerTask,
                           "Eth_Rx",
                           configMINIMAL_STACK_SIZE,
                           NULL,
                           iplpcETH_RX_TASK_PRIORITY,
                           xStack,
                           &xTaskBuffer);
#elif (configSUPPORT_DYNAMIC_ALLOCATION==1)
         xTaskCreate(prvEMACDeferredInterruptHandlerTask,
                                    "Eth_Rx",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    iplpcETH_RX_TASK_PRIORITY,
                                    NULL);
#endif

         RxTaskCreated = pdTRUE;

         EthernetTxDoneSem = xSemaphoreCreateCounting(iplpcNUM_TX_DESCRIPTORS, 0);
         vQueueAddToRegistry(EthernetTxDoneSem, "EnetTxDone");
      }   // End if (RxTaskCreated != pdTRUE)

      /* Clear and enable RX and TX interrupts, note NVIC not enabled yet */
      Chip_ENET_EnableInt(LPC_ETHERNET, RXINTGROUP | TXINTGROUP);

      /* Enable RX and TX channels */
      Chip_ENET_TXEnable(LPC_ETHERNET);
      Chip_ENET_RXEnable(LPC_ETHERNET);

      /* Enable Ethernet interrupts once the tasks and semaphores have been created.
       * This function is called from the +TCP IP_Task, so that should be ready. */
      NVIC_SetPriority(ETHERNET_IRQn, config_ETHERNET_INTERRUPT_PRIORITY);
      NVIC_EnableIRQ(ETHERNET_IRQn);

      /* Initialisation over, loop, polling for the link status until it is "up",
       *  'sleeping' between polls. */
      while (LinkUp != pdTRUE)
      {
         LinkStatus = xPhyCheckLinkStatus();
         if (LinkStatus & PHY_LINK_CONNECTED)
         {
            /* Set MAC interface speed and duplicity according to PHY negotiation */
            if (LinkStatus & PHY_LINK_SPEED100)
            {
               Chip_ENET_Set100Mbps(LPC_ETHERNET);
            }
            else
            {
               Chip_ENET_Set10Mbps(LPC_ETHERNET);
            }
            if (LinkStatus & PHY_LINK_FULLDUPLX)
            {
               Chip_ENET_SetFullDuplex(LPC_ETHERNET);
            }
            else
            {
               Chip_ENET_SetHalfDuplex(LPC_ETHERNET);
            }
            //Board_LED_Set (0, true);
            LinkUp = pdTRUE;                  // Break out of while() loop
         }
         else
         {
            //Board_LED_Set (0, false);
            vTaskDelayUntil(&xLastWakeTime, iplpcLINK_POLL_PERIOD); // Wait before trying again
         }
      }   // End while (LinkUp != pdTRUE)

      xReturn = pdPASS;           // Signal to caller that all is OK
   }   // End if (xReturn == pdPASS)

   return xReturn;
}

/* See https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Porting.html
 *
 * The TCP/IP stack calls xNetworkInterfaceOutput() whenever a network buffer is ready to be transmitted.
 *
 * The buffer to transmit is described by the descriptor passed into the function using the function's
 * pxDescriptor parameter. If xReleaseAfterSend does not equal pdFALSE then both the buffer and the buffer's
 * descriptor must be released (returned) back to the embedded TCP/IP stack by the driver code when they are
 * no longer required. If xReleaseAfterSend is pdFALSE then both the network buffer and the buffer's
 * descriptor will be released by the TCP/IP stack itself (in which case the driver does not need to release them).
 *
 * Note that, at the time of writing, the value returned from xNetworkInterfaceOutput() is ignored. The embedded
 * TCP/IP stack will NOT call xNetworkInterfaceOutput() for the same network buffer twice, even if the first call
 * to xNetworkInterfaceOutput() could not send the network buffer onto the network.
 */
BaseType_t xNetworkInterfaceOutput(NetworkBufferDescriptor_t * const pxNetworkBuffer, BaseType_t bReleaseAfterSend)
{
   BaseType_t xReturn = pdFAIL;
   int16_t ProduceIdx;
   int16_t ConsumeIdx;

   /* Only send if the link is up, i.e. connected */
   if (LinkUp)
   {
      /* Find out the index that the hardware is using */
      ConsumeIdx = Chip_ENET_GetTXConsumeIndex(LPC_ETHERNET);

      /* Check on the descriptor buffer status, we can't send if it is full. */
      if (!Chip_ENET_IsTxFull(LPC_ETHERNET))
      {
         ProduceIdx = Chip_ENET_GetTXProduceIndex(LPC_ETHERNET);
         /* A DMA descriptor is available, so assign it to the network descriptor */
         VacEnetData.TxDescriptor[ProduceIdx].Packet = (uint32_t)pxNetworkBuffer->pucEthernetBuffer;
         /* The ENET_TCTRL_LAST flag tells the DMA engine that the whole frame is described by this descriptor,
          * the ENET_TCTRL_INT flag will generate an interrupt when the frame has been sent. */
         VacEnetData.TxDescriptor[ProduceIdx].Control = ENET_TCTRL_SIZE(pxNetworkBuffer->xDataLength) | ENET_TCTRL_LAST | ENET_TCTRL_INT;
#if (ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 1)
         /* The ENET_TCTRL_CRC flag causes a hardware CRC to be appended to the frame */
         VacEnetData.TxDescriptor[ProduceIdx].Control |= ENET_TCTRL_CRC;
#endif

         /* Send the data by updating the Tx producer index register */
         ProduceIdx = Chip_ENET_IncTXProduceIndex(LPC_ETHERNET);

         /* Call the standard trace macro to log the send event. */
         iptraceNETWORK_INTERFACE_TRANSMIT();

         /* Wait for a TX Done interrupt */
         if (xSemaphoreTake(EthernetTxDoneSem, portMAX_DELAY) == pdPASS)
         {
            /* Check the status held */
            // TODO Check user manual to find out which index should be used,
            // I'm pretty sure that it is the consumer that reflects the packet
            // that has just consumed by the DMA engine.
            xReturn = Chip_ENET_GetIntStatus(LPC_ETHERNET);
            if (xReturn)
            {
               DEBUGOUT(" IS:%X", xReturn);
            }

            xReturn = VacEnetData.TxStatus[ConsumeIdx].StatusInfo;
            if (xReturn)
            {
               DEBUGOUT(" DS:%X", xReturn);
            }

            // AWM - TODO Check for and handle underrun errors

            /* The network buffer has been sent so its descriptor can be released. */
            if (bReleaseAfterSend != pdFALSE)
            {
               /* Release the descriptor. */
               vReleaseNetworkBufferAndDescriptor(pxNetworkBuffer);
            }
         }
         xReturn = pdPASS;
      }
      // else the buffer was full  TODO a wait and retry mechanism, for now return pdFAIL

   }   // end if (LinkUp)

   return xReturn;
}
