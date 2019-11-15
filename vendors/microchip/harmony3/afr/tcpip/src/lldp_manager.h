/*******************************************************************************
  LLDP manager private stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    lldp_manager.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef __LLDP_MANAGER_H_
#define __LLDP_MANAGER_H_

/*****************************************************************************
  Function:
    bool TCPIP_LLDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_LLDP_MODULE_CONFIG* pLLDPConfig);

  Summary:
    Resets the LLDP client module.

  Description:
    Initialization of the LLDP module

  Precondition:
    None

  Parameters:
    stackCtrl   - pointer to stack structure specifying the interface to initialize
    pLLDPConfig - pointer to LLDP configuration data

  Returns:
    None

  Remarks:
    None
 */
bool        TCPIP_LLDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_LLDP_MODULE_CONFIG* pLLDPConfig);


/*****************************************************************************
  Function:
    bool TCPIP_LLDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);

  Summary:
    Turns off the LLDP module for the specified interface.

  Description:
    Deinitialization of the LLDP module.

  Precondition:
    None

  Parameters:
    stackData - pointer to stack structure specifying the interface to deinitialize

  Returns:
    None

  Remarks:
 */
void        TCPIP_LLDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);



// current LLDPDU interface MAC address
const uint8_t* TCPIP_LLDP_LocalIfAddressGet(void);

// current destination MAC address
const TCPIP_MAC_ADDR* TCPIP_LLDP_MacDestAddressGet(void);

// forward declaration
struct lldp_per_port_t;
const struct lldp_per_port_t*      TCPIP_LLDP_PortGet(void);

// set the allocated power
void    TCPIP_LLDP_AllocatedPowerSet(uint16_t allocatedPower);

typedef union
{
    uint8_t val;
    struct
    {
        unsigned fixTlvSize             :1;
        unsigned poeEnabledPair         :1;
        unsigned powerAllocated         :1;
        unsigned uPoeEnabledPower       :1;
        unsigned poePlusEnabledPower    :1;
        unsigned poeEnabledMinPower     :1;
        unsigned reserved               :2;
    };
}TCPIP_LLDP_ORG_FLAGS;

bool    TCPIP_LLDP_OrgProcessFlagsGet(TCPIP_LLDP_ORG_FLAGS* pFlags);


#endif  // __LLDP_MANAGER_H_
