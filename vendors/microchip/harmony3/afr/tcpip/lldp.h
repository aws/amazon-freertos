/*******************************************************************************
  LLDP Module API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    lldp.h

  Summary:
    LLDP Module API definitions.

  Description:
    The LLDP module implements the Link Local Discovery Protocol.
********************************************************************************/
//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef __LLDP_H
#define __LLDP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// LLDP Module Configuration
typedef struct
{
    void*   reserved;
} TCPIP_LLDP_MODULE_CONFIG;

void TCPIP_LLDP_RxEnable(void);
void TCPIP_LLDP_TxEnable(void);
void TCPIP_LLDP_RxTxEnable(void);
void TCPIP_LLDP_PortDisable(void);

void TCPIP_LLDP_FastTxCounterSet(uint8_t n);

void TCPIP_LLDP_MacDestAddressSet(TCPIP_MAC_ADDR* pAddr);

// returns the current allocated power
uint16_t TCPIP_LLDP_AllocatedPowerGet(void);

void     TCPIP_LLDP_DesiredPowerSet(uint16_t desiredPower);

bool TCPIP_LLDP_UPoePowerIsEnabled(void);

bool TCPIP_LLDP_PoePlusPowerIsEnabled(void);
bool TCPIP_LLDP_PoeMinPowerIsEnabled(void);

// *****************************************************************************
// *****************************************************************************
// Section: Task Function
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void  TCPIP_LLDP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs LLDP module tasks in the TCP/IP stack.

  Precondition:
    The LLDP module should have been initialized

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_LLDP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // __LLDP_H

