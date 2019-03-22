/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

    .syntax unified
    .arch armv7-m

/* Memory Model
   The HEAP starts at the end of the DATA section and grows upward.
   
   The STACK starts at the end of the RAM and grows downward.
   
   The HEAP and stack STACK are only checked at compile time:
   (DATA_SIZE + HEAP_SIZE + STACK_SIZE) < RAM_SIZE
   
   This is just a check for the bare minimum for the Heap+Stack area before
   aborting compilation, it is not the run time limit:
   Heap_Size + Stack_Size = 0x80 + 0x80 = 0x100
 */
    .section .stack
    .align 3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0xc00
#endif
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size __StackLimit, . - __StackLimit
__StackTop:
    .size __StackTop, . - __StackTop
    
    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    .long    __StackTop            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .long    NMI_Handler           /* NMI Handler */
    .long    HardFault_Handler     /* Hard Fault Handler */
    .long    MemManage_Handler     /* MPU Fault Handler */
    .long    BusFault_Handler      /* Bus Fault Handler */
    .long    UsageFault_Handler    /* Usage Fault Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    SvcCall_Main          /* SVCall Handler */
    .long    DebugMon_Handler      /* Debug Monitor Handler */
    .long    0                     /* Reserved */
    .long    PendSV_Main           /* PendSV Handler */
    .long    SysTick_Main          /* SysTick Handler */

    /* External interrupts */
    .long   isrC_main         /* 16: UART1  */
    .long   isrC_main         /* 17: DMA */
    .long   isrC_main         /* 18: HIF */   
    .long   isrC_main         /* 19: I2C1 */
    .long   isrC_main         /* 20: I2C2 */
    .long   isrC_main         /* 21: UART2 */
    .long   isrC_main         /* 22: MTK_CRYPTO */
    .long   isrC_main         /* 23: SF */
    .long   isrC_main         /* 24: EINT */
    .long   isrC_main         /* 25: BTIF */
    .long   isrC_main         /* 26: WDT */
    .long   isrC_main         /* 27: reserved */
    .long   isrC_main         /* 28: SPI_SLAVE */
    .long   isrC_main         /* 29:  WDT_N9 */
    .long   isrC_main         /* 30:  ADC */
    .long   isrC_main         /* 31:  IRDA_TX */
    .long   isrC_main         /* 32:  IRDA_RX */
    .long   isrC_main         /* 33:  USB_VBUS_ON */
    .long   isrC_main         /* 34:  USB_VBUS_OFF */
    .long   isrC_main         /* 35: timer_hit */
    .long   isrC_main         /* 36: GPT3 */
    .long   isrC_main         /* 37: alarm_hit */
    .long   isrC_main         /* 38:  AUDIO */
    .long   isrC_main         /* 39: n9_cm4_sw_irq */
    .long   isrC_main         /* 40: GPT4 */
    .long   isrC_main         /* 41: adc_comp_irq */
    .long   isrC_main         /* 42: reserved */
    .long   isrC_main         /* 43: SPIM */
    .long   isrC_main         /* 44:  USB */
    .long   isrC_main         /* 45: UDMA */
    .long   isrC_main         /* 46: TRNG */
    .long   isrC_main         /* 47: reserved */
    .long   isrC_main         /* 48: configurable */
    .long   isrC_main         /* 49: configurable */
    .long   isrC_main         /* 50: configurable */
    .long   isrC_main         /* 51: configurable */
    .long   isrC_main         /* 52: configurable */
    .long   isrC_main         /* 53: configurable */
    .long   isrC_main         /* 54: configurable */
    .long   isrC_main         /* 55: configurable */
    .long   isrC_main         /* 56: configurable */
    .long   isrC_main         /* 57: configurable */
    .long   isrC_main         /* 58: configurable */
    .long   isrC_main         /* 59: configurable */
    .long   isrC_main         /* 60: configurable */
    .long   isrC_main         /* 61: configurable */
    .long   isrC_main         /* 62: configurable */
    .long   isrC_main         /* 63: configurable */
    .long   isrC_main         /* 64: configurable */
    .long   isrC_main         /* 65: configurable */
    .long   isrC_main         /* 66: configurable */
    .long   isrC_main         /* 67: configurable */
    .long   isrC_main         /* 68: configurable */
    .long   isrC_main         /* 69: configurable */
    .long   isrC_main         /* 70: configurable */
    .long   isrC_main         /* 71: configurable */
    .long   isrC_main         /* 72: configurable */
    .long   isrC_main         /* 73: configurable */
    .long   isrC_main         /* 74: configurable */
    .long   isrC_main         /* 75: configurable */
    .long   isrC_main         /* 76: configurable */
    .long   isrC_main         /* 77: configurable */
    .long   isrC_main         /* 78: configurable */
    .long   isrC_main         /* 79: configurable */
    .long   isrC_main         /* 80: configurable */
    .long   isrC_main         /* 81: configurable */
    .long   isrC_main         /* 82: configurable */
    .long   isrC_main         /* 83: configurable */
    .long   isrC_main         /* 84: configurable */
    .long   isrC_main         /* 85: configurable */
    .long   isrC_main         /* 86: configurable */
    .long   isrC_main         /* 87: configurable */
    .long   isrC_main         /* 88: configurable */
    .long   isrC_main         /* 89: configurable */
    .long   isrC_main         /* 90: configurable */
    .long   isrC_main         /* 91: configurable */
    .long   isrC_main         /* 92: configurable */
    .long   isrC_main         /* 93: configurable */
    .long   isrC_main         /* 94: configurable */
    .long   isrC_main         /* 95: configurable */
    .long   isrC_main         /* 96: configurable */
    .long   isrC_main         /* 97: configurable */
    .long   isrC_main         /* 98: configurable */
    .long   isrC_main         /* 99: configurable */
    .long   isrC_main         /* 100: configurable */
    .long   isrC_main         /* 101: configurable */
    .long   isrC_main         /* 102: configurable */
    .long   isrC_main         /* 103: configurable */
    .long   isrC_main         /* 104: configurable */
    .long   isrC_main         /* 105: configurable */
    .long   isrC_main         /* 106: configurable */
    .long   isrC_main         /* 107: configurable */
    .long   isrC_main         /* 108: configurable */
    .long   isrC_main         /* 109: configurable */
    .long   isrC_main         /* 110: configurable */
    .long   isrC_main         /* 111: configurable */   

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb
    .thumb_func
    .section .init
    .align 2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in 
 *      linker script.
 *      _etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */
    ldr    sp, =__StackTop    		 /* set stack pointer */

    cpsid i

    ldr    r0, =hal_lp_get_wic_status
    blx    r0
    cbz    r0, Normal_boot
    /* deep sleep wakeup
    ** 1. TCM/SYSRAM keep power on during deep sleep, no need initialization
    ** 2. jump to the deep sleep wakeup handler
    */
    @jump to sleep return address
    ldr     r0, =DEEPSLEEP_WAKEUP_LR
    ldr     r0,[r0]
    ldr     r0,[r0]
    bx      r0

Normal_boot:
    /* preinit cache to accelerate region init progress */
    bl CachePreInit

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

.Lflash_to_ram_loop:
    cmp     r2, r3
    ittt    lo
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo    .Lflash_to_ram_loop

    ldr    r1, =__ramtext_load__
    ldr    r2, =__ramtext_start__
    ldr    r3, =__ramtext_end__

.Lflash_to_tcm_loop:
    cmp     r2, r3
    ittt    lo
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo    .Lflash_to_tcm_loop


    ldr    r2, =__tcmbss_start__
    ldr    r3, =__tcmbss_end__

.Lbss_to_tcm_loop:
    cmp     r2, r3
    ittt    lo
    movlo   r0, #0
    strlo   r0, [r2], #4
    blo    .Lbss_to_tcm_loop

    ldr    r2, =__bss_start__
    ldr    r3, =__bss_end__

.Lbss_to_ram_loop:
    cmp     r2, r3
    ittt    lo
    movlo   r0, #0
    strlo   r0, [r2], #4
    blo    .Lbss_to_ram_loop

    ldr    r0, =SystemInit
    blx    r0

    ldr    r0, =main
    bx     r0
    .pool
    .size Reset_Handler, . - Reset_Handler

DEEPSLEEP_WAKEUP_LR:
   .global  deepsleep_wakeup_address
   .long    deepsleep_wakeup_address

/**
 * @brief  This is the code for system exception handling(svcCall,systick,pendSV), must suspend 
           flash before fetch code from flash
 * @param  None
 * @retval None
*/
    .section  .ramTEXT,"ax",%progbits
    .type  SvcCall_Main, %function
    .global  SvcCall_Main
SvcCall_Main:
    cpsid    i
    push     {lr}
    blx Flash_ReturnReady
    pop      {lr}
    cpsie    i
    b    SVC_Handler
    .size  SvcCall_Main, .-SvcCall_Main
  
    .type  PendSV_Main, %function
    .global  PendSV_Main
PendSV_Main:
    cpsid     i
    push     {lr}
    blx Flash_ReturnReady
    pop      {lr}
    cpsie    i
    b PendSV_Handler
    .size  PendSV_Main, .-PendSV_Main  

    .type  SysTick_Main, %function
    .global  SysTick_Main
SysTick_Main:
    cpsid    i
    push     {lr}
    blx Flash_ReturnReady
    cpsie    i
    blx SysTick_Handler
    pop     {lr}
    bx      lr
  .size  SysTick_Main, .-SysTick_Main

    .text
/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_default_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .size    \handler_name, . - \handler_name
    .endm

    def_default_handler    NMI_Handler
    def_default_handler    HardFault_Handler
    def_default_handler    MemManage_Handler
    def_default_handler    BusFault_Handler
    def_default_handler    UsageFault_Handler
    def_default_handler    SVC_Handler
    def_default_handler    DebugMon_Handler
    def_default_handler    PendSV_Handler
    def_default_handler    SysTick_Handler
    def_default_handler    Default_Handler

    .end


