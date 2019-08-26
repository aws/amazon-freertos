/**
  ******************************************************************************
  * @file    hci.h
  * @author  AMS - VMA RF Application team
  * @version V1.0.0
  * @date    21-Sept-2016
  * @brief   Header file for BlueNRG-1's HCI APIs
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  ******************************************************************************
  */
#ifndef __HCI_H_
#define __HCI_H_

#include "hal_types.h"
#include "link_layer.h"
#include <listBlueNRG.h>


#define HCI_READ_PACKET_SIZE				        128 //71

/**
 * Maximum payload of HCI commands that can be sent. Change this value if needed.
 * This value can be up to 255.
 */
#define HCI_MAX_PAYLOAD_SIZE 128

/*** Data types ***/

/* structure used to read received data */
typedef struct _tHciDataPacket
{
    tListNode currentNode;
	uint8_t dataBuff[HCI_READ_PACKET_SIZE];
    uint8_t data_len;
}tHciDataPacket;

struct hci_request {
  uint16_t ogf;
  uint16_t ocf;
  int      event;
  void     *cparam;
  int      clen;
  void     *rparam;
  int      rlen;
};

/**
 * Initialization function. Must be done before any data can be received from
 * BLE controller.
 */
uint8_t BlueNRG_Stack_Initialization(void);

/**
 * Callback used to pass events to application.
 *
 * @param[in] pckt    The event.
 *
 */
extern void HCI_Event_CB(void *pckt);

/**
 * Processing function that must be called after an event is received from
 * HCI interface. Must be called outside ISR. It will call HCI_Event_CB if
 * necessary.
*/
void BTLE_StackTick(void);


/**
 * @brief Check if queue of HCI event is empty or not.
 * @note This funtion can be used to check if the event queue from BlueNRG is empty. This
 *        is useful when checking if it is safe to go to sleep. 
 * @return TRUE if event queue is empty. FALSE otherwhise.
 */
BOOL HCI_Queue_Empty(void);

/**
 * Interrupt service routine that must be called when the BlueNRG 
 * reports a packet received or an event to the host through the 
 * BlueNRG interrupt line.
 */
void HCI_Isr(void);
void HCI_Isr_Uart(void);
int HCI_verify(const tHciDataPacket * hciReadPacket);
int hci_send_req(struct hci_request *r, BOOL async);

extern tListNode hciReadPktPool;
extern tListNode hciReadPktRxQueue;

#endif /* __HCI_H_ */
