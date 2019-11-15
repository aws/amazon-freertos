/*******************************************************************************
  TCP/IP modules manager file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_module_manager.h

  Summary:
    Internal TCP/IP stack module manager file
    
  Description:
    This header file contains the function prototypes and definitions of the 
    TCP/IP stack manager services
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef __TCPIP_MODULE_MANAGER_H_
#define __TCPIP_MODULE_MANAGER_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// definitions
// 


// ************* stack supported modules and their attached functionality **************
// 

// ******* table with TCPIP stack modules **************
// We use directly the functions names (pointers) rather than providing registration functions
// so that we can create this table in const space
//

// initialization function
// if the module has initialization to do, this function
// will be called. It should return a result to indicate
// if the initialization was successful. If not, the
// interface will not be completed.
typedef bool    (*tcpipModuleInitFunc)(const TCPIP_STACK_MODULE_CTRL* const, const void* );

// de-initialization function
// if the module needs to clean up when the module is
// brought down, this function will be called. It should
// return a result to indicate that everything has been
// cleaned up.
typedef void    (*tcpipModuleDeInitFunc)(const TCPIP_STACK_MODULE_CTRL * const);


// descriptor of an TCPIP stack module entry
// module that's part of the stack
// each module has an ID and init/deinit functions
// 
typedef struct
{
    TCPIP_STACK_MODULE       moduleId;           // module identification
    tcpipModuleInitFunc      initFunc;           // initialization function
#if (TCPIP_STACK_DOWN_OPERATION != 0)
    tcpipModuleDeInitFunc    deInitFunc;         // deinitialization function
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
}TCPIP_STACK_MODULE_ENTRY;



// *********** a stack module exposes an signal handler ****************


typedef struct
{
    tcpipModuleSignalHandler    signalHandler;      // signal handler; regular ModuleTask() function
                                                    // if NULL, the corresponding entry is not used
    TCPIP_MODULE_SIGNAL_FUNC    userSignalF;        // external user signal function                                                    

    uint16_t                    signalVal;          // TCPIP_MODULE_SIGNAL: current signal value;
    int16_t                     asyncTmo;           // module required timeout, msec; 
                                                    // the stack manager checks that the module reached its timeout
    int16_t                     currTmo;            // current module timeout, msec; maintained by the stack manager
    uint16_t                    signalParam;        // some signals have parameters
                                                    // for TCPIP_MODULE_SIGNAL_INTERFACE_CHANGE
                                                    // this is the interface mask: 1 << ifx 

}TCPIP_MODULE_SIGNAL_ENTRY;


// Connection event handler definition.
// The stack calls the handler when a new connection event occurs.
// Note that this call will carry only connection events!
typedef void    (*tcpipModuleConnHandler)(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent);


// function for getting the MAC module of an interface
static __inline__ const TCPIP_MAC_OBJECT* __attribute__((always_inline))  _TCPIP_STACK_GetMacObject(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? pNetIf->pMacObj : 0;
}

// function for getting the MAC handle of an interface
static __inline__ SYS_MODULE_OBJ __attribute__((always_inline))  _TCPIP_STACK_GetMacObjectHandle(TCPIP_NET_IF* pNetIf)
{
    return (pNetIf && pNetIf->pMacObj) ? pNetIf->macObjHandle : 0;
}

// function for getting the MAC client handle of an interface
static __inline__ TCPIP_MAC_HANDLE __attribute__((always_inline))  _TCPIP_STACK_GetMacClientHandle(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? pNetIf->hIfMac : 0;
}

#endif //  __TCPIP_MODULE_MANAGER_H_








