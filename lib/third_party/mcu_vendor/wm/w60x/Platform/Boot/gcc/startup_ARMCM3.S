/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
/* Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
	.syntax	unified
	.arch	armv7-m

	.section .stack
	.align	3
#ifdef __STACK_SIZE
	.equ	Stack_Size, __STACK_SIZE
#else
	.equ	Stack_Size, 0xc00
#endif
	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	3
#ifdef __HEAP_SIZE
	.equ	Heap_Size, __HEAP_SIZE
#else
	.equ	Heap_Size, 0x1A000
#endif
	.globl	__heap_base
	.globl	__HeapLimit
__heap_base:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__heap_base, . - __heap_base
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit

	.section .isr_vector
	.align	2
	.globl	__isr_vector
__isr_vector:
	.long	__StackTop            /* Top of Stack */
	.long	Reset_Handler         /* Reset Handler */
	.long	NMI_Handler           /* NMI Handler */
	.long	HardFault_Handler     /* Hard Fault Handler */
	.long	MemManage_Handler     /* MPU Fault Handler */
	.long	BusFault_Handler      /* Bus Fault Handler */
	.long	UsageFault_Handler    /* Usage Fault Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	SVC_Handler           /* SVCall Handler */
	.long	DebugMon_Handler      /* Debug Monitor Handler */
	.long	0                     /* Reserved */
	.long	PendSV_Handler        /* PendSV Handler */
	.long	OS_CPU_SysTickHandler /* SysTick Handler */

	/* External interrupts */
	.long   SDIO_RX_IRQHandler            /* Window Watchdog */
    .long   SDIO_TX_IRQHandler             /* PVD through EXTI Line detect */
    .long   SDIO_RX_CMD_IRQHandler          /* Tamper */
    .long   SDIO_TX_CMD_IRQHandler             /* RTC */
    .long   tls_wl_mac_isr           /* Flash */
    .long   Default_Handler             /* RCC */
    .long   tls_wl_rx_isr           /* EXTI Line 0 */
    .long   tls_wl_mgmt_tx_isr           /* EXTI Line 1 */
    .long   tls_wl_data_tx_isr           /* EXTI Line 2 */
    .long   PMU_TIMER1_IRQHandler           /* EXTI Line 3 */
    .long   PMU_TIMER0_IRQHandler           /* EXTI Line 4 */
    .long   PMU_GPIO_WAKE_IRQHandler   /* DMA1 Channel 1 */
    .long   PMU_SDIO_WAKE_IRQHandler   /* DMA1 Channel 2 */
    .long   DMA_Channel0_IRQHandler   /* DMA1 Channel 3 */
    .long   DMA_Channel1_IRQHandler   /* DMA1 Channel 4 */
    .long   DMA_Channel2_IRQHandler   /* DMA1 Channel 5 */
    .long   DMA_Channel3_IRQHandler   /* DMA1 Channel 6 */
    .long   DMA_Channel4_7_IRQHandler   /* DMA1 Channel 7 */
    .long   DMA_BRUST_IRQHandler          /* ADC1 & ADC2 */
    .long   I2C_IRQHandler  /* USB High Priority or CAN1 TX */
    .long   ADC_IRQHandler /* USB Low  Priority or CAN1 RX0 */
    .long   SPI_LS_IRQHandler        /* CAN1 RX1 */
    .long   SPI_HS_IRQHandler        /* CAN1 SCE */
    .long   UART0_IRQHandler         /* EXTI Line 9..5 */
    .long   UART1_IRQHandler        /* TIM1 Break */
    .long   GPIOA_IRQHandler         /* TIM1 Update */
    .long   TIM0_IRQHandler    /* TIM1 Trigger and Commutation */
    .long   TIM1_IRQHandler         /* TIM1 Capture Compare */
    .long   TIM2_IRQHandler            /* TIM2 */
    .long   TIM3_IRQHandler            /* TIM3 */
    .long   TIM4_IRQHandler            /* TIM4 */
    .long   TIM5_IRQHandler         /* I2C1 Event */
    .long   WDG_IRQHandler         /* I2C1 Error */
    .long   PMU_IRQHandler         /* I2C2 Event */
    .long   FLASH_IRQHandler         /* I2C2 Error */
    .long   PWM_IRQHandler            /* SPI1 */
    .long   I2S_IRQHandler            /* SPI2 */
    .long   PMU_RTC_IRQHandler          /* USART1 */
    .long   RSA_IRQHandler          /* USART2 */
    .long   CRYPTION_IRQHandler          /* USART3 */
    .long   GPIOB_IRQHandler       /* EXTI Line 15..10 */
    .long   UART2_IRQHandler        /* RTC Alarm through EXTI Line */	

	.size	__isr_vector, . - __isr_vector

	.text
	.thumb
	.thumb_func
	.align	2
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:

/*  Firstly it copies data from read only memory to RAM.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

.L_loop1:
	cmp	r2, r3
	ittt	lt
	ldrlt	r0, [r1], #4
	strlt	r0, [r2], #4
	blt	.L_loop1

/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0
.L_loop3:
	cmp	r1, r2
	itt	lt
	strlt	r0, [r1], #4
	blt	.L_loop3

#ifndef __NO_SYSTEM_INIT
	 bl	SystemInit 
#endif

	bl	main

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler
	def_irq_handler	HardFault_Handler
	def_irq_handler	MemManage_Handler
	def_irq_handler	BusFault_Handler
	def_irq_handler	UsageFault_Handler
	def_irq_handler	SVC_Handler
	def_irq_handler	DebugMon_Handler
	def_irq_handler	PendSV_Handler
	def_irq_handler	OS_CPU_SysTickHandler
	
	def_irq_handler SDIO_RX_IRQHandler            /* Window Watchdog */
    def_irq_handler SDIO_TX_IRQHandler             /* PVD through EXTI Line detect */
    def_irq_handler SDIO_RX_CMD_IRQHandler          /* Tamper */
    def_irq_handler SDIO_TX_CMD_IRQHandler             /* RTC */
    def_irq_handler tls_wl_mac_isr           /* Flash */
    def_irq_handler tls_wl_rx_isr           /* EXTI Line 0 */
    def_irq_handler tls_wl_mgmt_tx_isr           /* EXTI Line 1 */
    def_irq_handler tls_wl_data_tx_isr           /* EXTI Line 2 */
    /*def_irq_handler PMU_TIMER1_IRQHandler*/          /* EXTI Line 3 */
    /*def_irq_handler PMU_TIMER0_IRQHandler*/          /* EXTI Line 4 */
    /*def_irq_handler PMU_GPIO_WAKE_IRQHandler*/   /* DMA1 Channel 1 */
    def_irq_handler PMU_SDIO_WAKE_IRQHandler   /* DMA1 Channel 2 */
    def_irq_handler DMA_Channel0_IRQHandler   /* DMA1 Channel 3 */
    def_irq_handler DMA_Channel1_IRQHandler   /* DMA1 Channel 4 */
    def_irq_handler DMA_Channel2_IRQHandler   /* DMA1 Channel 5 */
    def_irq_handler DMA_Channel3_IRQHandler   /* DMA1 Channel 6 */
    def_irq_handler DMA_Channel4_7_IRQHandler   /* DMA1 Channel 7 */
    def_irq_handler DMA_BRUST_IRQHandler          /* ADC1 & ADC2 */
    /*def_irq_handler I2C_IRQHandler*/  /* USB High Priority or CAN1 TX */
    def_irq_handler ADC_IRQHandler /* USB Low  Priority or CAN1 RX0 */
    def_irq_handler SPI_LS_IRQHandler        /* CAN1 RX1 */
    def_irq_handler SPI_HS_IRQHandler        /* CAN1 SCE */
    def_irq_handler UART0_IRQHandler         /* EXTI Line 9..5 */
    def_irq_handler UART1_IRQHandler        /* TIM1 Break */
    def_irq_handler GPIOA_IRQHandler         /* TIM1 Update */
    def_irq_handler TIM0_IRQHandler    /* TIM1 Trigger and Commutation */
    def_irq_handler TIM1_IRQHandler         /* TIM1 Capture Compare */
    def_irq_handler TIM2_IRQHandler            /* TIM2 */
    def_irq_handler TIM3_IRQHandler            /* TIM3 */
    def_irq_handler TIM4_IRQHandler            /* TIM4 */
    def_irq_handler TIM5_IRQHandler         /* I2C1 Event */
    def_irq_handler WDG_IRQHandler         /* I2C1 Error */
    def_irq_handler PMU_IRQHandler         /* I2C2 Event */
    def_irq_handler FLASH_IRQHandler         /* I2C2 Error */
    /*def_irq_handler PWM_IRQHandler*/            /* SPI1 */
    /*def_irq_handler I2S_IRQHandler*/            /* SPI2 */
    /*def_irq_handler PMU_RTC_IRQHandler*/          /* USART1 */
    def_irq_handler RSA_IRQHandler          /* USART2 */
    def_irq_handler CRYPTION_IRQHandler          /* USART3 */
    def_irq_handler GPIOB_IRQHandler       /* EXTI Line 15..10 */
    def_irq_handler UART2_IRQHandler        /* RTC Alarm through EXTI Line */

	.end
