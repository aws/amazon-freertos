/*******************************************************************************
 System Interrupt Source File

  File Name:
    sys_interrupt_a.S

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the 
    interrupt sub-system.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END
/*
*********************************************************************************************************
*                                           INCLUDES
*********************************************************************************************************
*/
#include <xc.h>



#include "ISR_Support.h"


/* External Interrupt Instance 0 Interrupt */
   .extern  IntHandlerExternalInterruptInstance0

   .section	.vector_13,code, keep
   .equ     __vector_dispatch_13, IntVectorExternalInterruptInstance0
   .global  __vector_dispatch_13
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorExternalInterruptInstance0

IntVectorExternalInterruptInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerExternalInterruptInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorExternalInterruptInstance0


 

/* TMR Instance 0 Interrupt */
   .extern  IntHandlerDrvTmrInstance0

   .section	.vector_9,code, keep
   .equ     __vector_dispatch_9, IntVectorDrvTmrInstance0
   .global  __vector_dispatch_9
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvTmrInstance0

IntVectorDrvTmrInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvTmrInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorDrvTmrInstance0


/* SPI Instance 0 Interrupt */
   .extern  IntHandlerSPIRxInstance0

   .section	.vector_143,code, keep
   .equ     __vector_dispatch_143, IntVectorSPIRxInstance0
   .global  __vector_dispatch_143
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorSPIRxInstance0

IntVectorSPIRxInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerSPIRxInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorSPIRxInstance0


   .extern  IntHandlerSPITxInstance0

   .section	.vector_144,code, keep
   .equ     __vector_dispatch_144, IntVectorSPITxInstance0
   .global  __vector_dispatch_144
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorSPITxInstance0

IntVectorSPITxInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerSPITxInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorSPITxInstance0


   .extern  IntHandlerSPIFaultInstance0

   .section	.vector_142,code, keep
   .equ     __vector_dispatch_142, IntVectorSPIFaultInstance0
   .global  __vector_dispatch_142
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorSPIFaultInstance0

IntVectorSPIFaultInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerSPIFaultInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorSPIFaultInstance0


/* USART Instance 0 Interrupt */

   .extern  IntHandlerDrvUsartReceiveInstance0

   .section	.vector_113,code, keep
   .equ     __vector_dispatch_113, IntVectorDrvUsartReceiveInstance0
   .global  __vector_dispatch_113
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvUsartReceiveInstance0

IntVectorDrvUsartReceiveInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvUsartReceiveInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorDrvUsartReceiveInstance0



/* Sytem DMA Instance 1 Interrupt */
   .extern  IntHandlerSysDmaInstance1

   .section	.vector_134,code, keep
   .equ     __vector_dispatch_134, IntVectorSysDmaInstance1
   .global  __vector_dispatch_134
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorSysDmaInstance1

IntVectorSysDmaInstance1:
    portSAVE_CONTEXT
    la    s6,  IntHandlerSysDmaInstance1
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorSysDmaInstance1


/* Sytem DMA Instance 2 Interrupt */
   .extern  IntHandlerSysDmaInstance2

   .section	.vector_135,code, keep
   .equ     __vector_dispatch_135, IntVectorSysDmaInstance2
   .global  __vector_dispatch_135
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorSysDmaInstance2

IntVectorSysDmaInstance2:
    portSAVE_CONTEXT
    la    s6,  IntHandlerSysDmaInstance2
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorSysDmaInstance2



   .extern  IntHandlerDrvUsartTransmitInstance0

   .section	.vector_114,code, keep
   .equ     __vector_dispatch_114, IntVectorDrvUsartTransmitInstance0
   .global  __vector_dispatch_114
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvUsartTransmitInstance0

IntVectorDrvUsartTransmitInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvUsartTransmitInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorDrvUsartTransmitInstance0


   .extern  IntHandlerDrvUsartErrorInstance0

   .section	.vector_112,code, keep
   .equ     __vector_dispatch_112, IntVectorDrvUsartErrorInstance0
   .global  __vector_dispatch_112
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvUsartErrorInstance0

IntVectorDrvUsartErrorInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvUsartErrorInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVectorDrvUsartErrorInstance0


/* ETHMAC Interrupt */
   .extern  IntHandler_ETHMAC

   .section	.vector_153,code, keep
   .equ     __vector_dispatch_153, IntVector_ETHMAC
   .global  __vector_dispatch_153
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVector_ETHMAC

IntVector_ETHMAC:
    portSAVE_CONTEXT
    la    s6,  IntHandler_ETHMAC
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	IntVector_ETHMAC



/*******************************************************************************
 End of File
 */

