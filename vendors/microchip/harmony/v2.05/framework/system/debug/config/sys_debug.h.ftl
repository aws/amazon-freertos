<#--
/*******************************************************************************
  Debug System Service Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_debug.h.ftl

  Summary:
    Debug System Service Freemarker Template File

  Description:

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2014-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
-->
<#if CONFIG_USE_SYS_DEBUG == true>

/*** Debug System Service Configuration ***/
#define SYS_DEBUG_ENABLE
#define DEBUG_PRINT_BUFFER_SIZE       ${CONFIG_DEBUG_PRINT_BUFFER_SIZE}
<#if (CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true) && CONFIG_USB_DEVICE_USE_CDC_NEEDED == true>
#define SYS_DEBUG_BUFFER_DMA_READY        __attribute__((coherent)) __attribute__((aligned(16)))
<#elseif (CONFIG_DSTBDPIC32CZ == true || CONFIG_PIC32WK == true) && CONFIG_USB_DEVICE_USE_CDC_NEEDED == true>
#define SYS_DEBUG_BUFFER_DMA_READY        __attribute__((aligned(16)))
<#else>
#define SYS_DEBUG_BUFFER_DMA_READY
</#if>
</#if>
<#if CONFIG_USE_SYS_CONSOLE == true>
#define SYS_DEBUG_USE_CONSOLE
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
