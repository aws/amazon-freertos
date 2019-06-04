/*******************************************************************************
  System Definitions

  File Name:
    system_definitions.h

  Summary:
    MPLAB Harmony project system definitions.

  Description:
    This file contains the system-wide prototypes and definitions for an MPLAB
    Harmony project.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
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
//DOM-IGNORE-END
#ifndef _SYS_DEFINITIONS_H
#define _SYS_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "system/common/sys_common.h"
#include "system/common/sys_module.h"
#include "system/devcon/sys_devcon.h"
#include "system/clk/sys_clk.h"
#include "system/int/sys_int.h"
#include "system/console/sys_console.h"
#include "system/random/sys_random.h"
#include "system/tmr/sys_tmr.h"
#include "driver/tmr/drv_tmr.h"
#include "driver/usart/drv_usart.h"
#include "system/ports/sys_ports.h"
#include "system/debug/sys_debug.h"
#include "system/command/sys_command.h"
#include "system/dma/sys_dma.h"
#include "driver/ethmac/drv_ethmac.h"
#include "driver/miim/drv_miim.h"
#include "driver/spi/drv_spi.h"
#include "tcpip/tcpip.h"
#include "osal/osal.h"
#include "driver/wifi/wilc1000/include/wdrv_wilc1000_api.h"
extern void SYS_RESET_SoftwareReset(void);
#include "FreeRTOS.h"
#include "task.h"
//#include "app.h"


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Objects

  Summary:
    Structure holding the system's object handles

  Description:
    This structure contains the object handles for all objects in the
    MPLAB Harmony project's system configuration.

  Remarks:
    These handles are returned from the "Initialize" functions for each module
    and must be passed into the "Tasks" function for each module.
*/

typedef struct
{
    SYS_MODULE_OBJ  sysTmr;
    SYS_MODULE_OBJ  drvTmr0;
    SYS_MODULE_OBJ  drvNvm;
    SYS_MODULE_OBJ  drvUsart0;
    SYS_MODULE_OBJ  sysDebug;
    SYS_MODULE_OBJ  sysConsole0;
    SYS_MODULE_OBJ  sysDma;
    /*** SPI Object for Index 0 ***/
    SYS_MODULE_OBJ	spiObjectIdx0;
    SYS_MODULE_OBJ  tcpip;
    SYS_MODULE_OBJ  drvMiim;

} SYSTEM_OBJECTS;

// *****************************************************************************
// *****************************************************************************
// Section: extern declarations
// *****************************************************************************
// *****************************************************************************

extern SYSTEM_OBJECTS sysObj;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _SYS_DEFINITIONS_H */
/*******************************************************************************
 End of File
*/

