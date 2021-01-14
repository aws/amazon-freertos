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

// Forward declaration of the default fault and interrupt handlers.
void resetISR(void);
static void nmiISR(void);
static void faultISR(void);
static void busFaultHandler(void);
static void intDefaultHandler(void);

// FreeRTOS handlers
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

// The reset handler called when processor is started
extern void _c_int00(void);

// The entry point for the application.
extern int main(void);

// Linker variable that marks the top and bottom of the stack.
extern void *__stack;
extern unsigned long _stack_end;

//*****************************************************************************
//
// The vector table in Flash. Note that the proper constructs must be placed
// on this to ensure that it ends up at physical address 0x0000.0000 or at the
// start of the program if located at a start address other than 0.
//
//*****************************************************************************
__attribute__ ((section(".resetVecs"))) __attribute__ ((used))
static void (* const resetVectors[])(void) =
{
    (void (*)(void))((uint32_t)&_stack_end),
    resetISR,                        // The reset handler
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
// The following are arrays of pointers to constructor functions that need to
// be called during startup to initialize global objects.
//
//*****************************************************************************
extern void (*__init_array_start []) (void);
extern void (*__init_array_end []) (void);

//*****************************************************************************
//
// The following global variable is required for C++ support.
//
//*****************************************************************************
void * __dso_handle = (void *) &__dso_handle;

//*****************************************************************************
//
// This is the code that gets called when the processor starts execution
// following a reset.  Only the absolutely necessary steps are performed,
// after which the application supplied entry routine is called.
//
//*****************************************************************************
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;

void localProgramStart(void)
{
    volatile uint32_t* cpacr = (volatile uint32_t*)0xE000ED88;
    uint32_t *vtor = (unsigned long *)0xE000ED08;
    uint32_t * bs;
    uint32_t * be;
    uint32_t * dl;
    uint32_t * ds;
    uint32_t * de;
    uint32_t count;
    uint32_t i;
    uint32_t newBasePri;

    /* enable FPU */
    *cpacr |= (0xF0 << 16);

    /* do final trim of device */
    SetupTrimDevice();

    /* Disable interrupts */
    __asm volatile (
        "mov %0, %1 \n\t"
        "msr basepri, %0 \n\t"
        "isb \n\t"
        "dsb \n\t"
        :"=r" (newBasePri) : "i" (configMAX_SYSCALL_INTERRUPT_PRIORITY) : "memory"
    );

#if configENABLE_ISR_STACK_INIT
    /* Initialize ISR stack to known value for Runtime Object View */
    register uint32_t *top = (uint32_t *)&__stack;
    register uint32_t *end = (uint32_t *)&newBasePri;
    while (top < end) {
        *top++ = (uint32_t)0xa5a5a5a5;
    }
#endif

    /* initiailize .bss to zero */
    bs = & __bss_start__;
    be = & __bss_end__;
    while (bs < be) {
        *bs = 0;
        bs++;
    }

    /* relocate the .data section */
    dl = & __data_load__;
    ds = & __data_start__;
    de = & __data_end__;
    if (dl != ds) {
        while (ds < de) {
            *ds = *dl;
            dl++;
            ds++;
        }
    }

    /* run any constructors */
    count = (uint32_t)(__init_array_end - __init_array_start);
    for (i = 0; i < count; i++) {
        __init_array_start[i]();
    }

    /*
     * set vector table base to point to above vectors in Flash; during
     * driverlib interrupt initialization this table will be copied to RAM
     */
    *vtor = (uint32_t)&resetVectors[0];

    /* call the application's entry point. */
    main();
}

//*****************************************************************************
//
// This is the code that gets called when the processor is reset.
//
//*****************************************************************************
void __attribute__((naked)) resetISR(void)
{
    /*
     *  Some debuggers do not load the stack pointer from the reset vector.
     *  This code ensures that the stack pointer is initialized.  We branch
     *  to localProgramStart() so that nothing is pushed to the stack
     *  before it has been initialized.
     */
    __asm__ __volatile__ (
        " movw r0, #:lower16:resetVectors\n"
        " movt r0, #:upper16:resetVectors\n"
        " ldr r0, [r0]\n"
        " mov sp, r0\n"
        " b localProgramStart"
        );
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

//*****************************************************************************
//
// This function is called by __libc_fini_array which gets called when exit()
// is called. In order to support exit(), an empty _fini() stub function is
// required.
//
//*****************************************************************************
void _fini(void)
{
    /* Function body left empty intentionally */
}
