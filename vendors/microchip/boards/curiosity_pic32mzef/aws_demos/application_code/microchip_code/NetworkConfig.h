/*******************************************************************************
  Network Interface configuration file

  Summary:
    Network Configuration file for FreeRTOS-Plus-TCP stack
    
  Description:
    - PIC32 configuration definitions for the FreeRTOS TCP/IP stack
*******************************************************************************/

/*******************************************************************************
File Name:  pic32_NetworkConfig.h
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/
#ifndef _PIC32_NETWORKCONFIG_H_
#define _PIC32_NETWORKCONFIG_H_


// FreeRTOS environment configuration

#define PIC32_MAC_TASK_NAME           "MacTask"
#define PIC32_MAC_TASK_STACK_SIZE     ( 2 * configMINIMAL_STACK_SIZE )

#define PIC32_MAC_TASK_PRI            (configMAX_PRIORITIES - 1)

// initialization done timeout 
#define PIC32_MAC_INIT_TIMEOUT        (pdMS_TO_TICKS(1000))


// delay to sleep while waiting for initialization to complete
#define PIC32_MAC_TASK_INIT_PENDING_DELAY  (pdMS_TO_TICKS(10))

#define PIC32_MAC_TIMER_NAME           "MacTmr"
#define PIC32_MAC_TIMER_PERIOD         (pdMS_TO_TICKS(50))

// describes the offset in an allocated packet buffer 
// where the pointer to the belonging packet is stored
// (this pointer is needed to restore the original packet
// to which this buffer belongs to once the MAC is done with it).
// Currently the structure of a network buffer that can be used by both:
//  - the FreeRTOSIP (NetworkBufferDescriptor_t->pucEthernetBuffer)
//  - the Harmony MAC driver: TCPIP_MAC_PACKET 
// from the beginning of the buffer:
//      - 4 bytes pointer to the network descriptor (FreeRTOS)
//      - 4 bytes pointer to the MAC packet (pic32_NetworkInterface.c)
//      - 2 bytes offset from the MAC packet (Harmony MAC driver: segLoadOffset)
// NOTE: the ipBUFFER_PADDING should be set to 10 to match the TCPIP_MAC_FRAME_OFFSET value!
#define PIC32_BUFFER_PKT_PTR_OSSET    6



// standard PIC32 MAC driver configuration
// taken from system_config.h !


// debug console commands
#define PIC32_MAC_DEBUG_COMMANDS    0

// debug start up messages
#define PIC32_MAC_DEBUG_MESSAGES    0

#endif  // _PIC32_NETWORKCONFIG_H_

