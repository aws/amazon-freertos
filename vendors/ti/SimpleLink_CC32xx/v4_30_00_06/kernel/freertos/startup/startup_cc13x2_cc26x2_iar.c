/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <string.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/setup.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)

#include <FreeRTOSConfig.h>

/* IAR includes */
#include <intrinsics.h>

// Forward declaration of the default fault and interrupt handlers.
static void nmiISR(void);
static void faultISR(void);
static void busFaultHandler(void);
static void intDefaultHandler(void);

// FreeRTOS handlers
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

//*****************************************************************************
//
// The entry point for the application startup code.
//
//*****************************************************************************
extern void __iar_program_start(void);

//*****************************************************************************
//
// Linker variable that marks the top of the stack.
//
//*****************************************************************************
extern const void* STACK_TOP;

// Place something in the CSTACK segment to get the stack check feature
// to work as expected
__root static void* dummy_stack @ ".stack";

//*****************************************************************************
//
// The vector table in Flash. Note that the proper constructs must be placed
// on this to ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__root void (* const __vector_table[])(void) @ ".intvec" =
{
    (void (*)(void))&STACK_TOP,      // The initial stack pointer
    __iar_program_start,             // The reset handler
    nmiISR,                          // The NMI handler
    faultISR,                        // The hard fault handler
    intDefaultHandler,               // The MPU fault handler
    busFaultHandler,                 // The bus fault handler
    intDefaultHandler,               // The usage fault handler
    intDefaultHandler,               // Reserved
    intDefaultHandler,               // Reserved
    intDefaultHandler,               // Reserved
    intDefaultHandler,               // Reserved
    vPortSVCHandler,                 // SVCall handler
    intDefaultHandler,               // Debug monitor handler
    intDefaultHandler,               // Reserved
    xPortPendSVHandler,              // The PendSV handler
    xPortSysTickHandler,             // The SysTick handler
    intDefaultHandler,               // 16 AON edge detect
    intDefaultHandler,               // 17 I2C
    intDefaultHandler,               // 18 RF Core Command & Packet Engine 1
    intDefaultHandler,               // 19 PKA Interrupt event
    intDefaultHandler,               // 20 AON RTC
    intDefaultHandler,               // 21 UART0 Rx and Tx
    intDefaultHandler,               // 22 AUX software event 0
    intDefaultHandler,               // 23 SSI0 Rx and Tx
    intDefaultHandler,               // 24 SSI1 Rx and Tx
    intDefaultHandler,               // 25 RF Core Command & Packet Engine 0
    intDefaultHandler,               // 26 RF Core Hardware
    intDefaultHandler,               // 27 RF Core Command Acknowledge
    intDefaultHandler,               // 28 I2S
    intDefaultHandler,               // 29 AUX software event 1
    intDefaultHandler,               // 30 Watchdog timer
    intDefaultHandler,               // 31 Timer 0 subtimer A
    intDefaultHandler,               // 32 Timer 0 subtimer B
    intDefaultHandler,               // 33 Timer 1 subtimer A
    intDefaultHandler,               // 34 Timer 1 subtimer B
    intDefaultHandler,               // 35 Timer 2 subtimer A
    intDefaultHandler,               // 36 Timer 2 subtimer B
    intDefaultHandler,               // 37 Timer 3 subtimer A
    intDefaultHandler,               // 38 Timer 3 subtimer B
    intDefaultHandler,               // 39 Crypto Core Result available
    intDefaultHandler,               // 40 uDMA Software
    intDefaultHandler,               // 41 uDMA Error
    intDefaultHandler,               // 42 Flash controller
    intDefaultHandler,               // 43 Software Event 0
    intDefaultHandler,               // 44 AUX combined event
    intDefaultHandler,               // 45 AON programmable 0
    intDefaultHandler,               // 46 Dynamic source (Default: PRCM)
    intDefaultHandler,               // 47 AUX Comparator A
    intDefaultHandler,               // 48 AUX ADC interrupts
    intDefaultHandler,               // 49 TRNG event
    intDefaultHandler,               // 50 Combined event from Osc control
    intDefaultHandler,               // 51 AUX Timer2 event 0
    intDefaultHandler,               // 52 UART1 combined interrupt
    intDefaultHandler                // 53 Combined event from battery monitor
};

//*****************************************************************************
//
// This function is called by __iar_program_start() early in the boot sequence.
//
//*****************************************************************************
int __low_level_init(void)
{
    unsigned long *vtor = (unsigned long *)0xE000ED08;

    /* do final trim of device */
    SetupTrimDevice();

    /* disable interrupts */
    __set_BASEPRI( configMAX_SYSCALL_INTERRUPT_PRIORITY );
    __DSB();
    __ISB();

    /*
     * set vector table base to point to above vectors in Flash; during
     * driverlib interrupt initialization this table will be copied to RAM
     */
    *vtor = (unsigned long)&__vector_table[0];

    return 1;
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void nmiISR(void)
{
    /* Enter an infinite loop. */
    while(1) {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void faultISR(void)
{
    /* Enter an infinite loop. */
    while(1) {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a bus fault.
// This simply enters an infinite loop, preserving the system state for
// examination by a debugger.
//
//*****************************************************************************
static void busFaultHandler(void)
{
    /* Enter an infinite loop. */
    while(1) {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void intDefaultHandler(void)
{
    /* Enter an infinite loop. */
    while(1) {
    }
}
