;********************************************************************************
;* @file      startup.s
;* @version   V1.00
;* @date      1/16/2019
;* @brief     CMSIS Cortex-M3 Core Device Startup File for the W60X
;*
;* @note      Copyright (C) 2019 WinnerMicro Inc. All rights reserved.
;*
;* <h2><center>&copy; COPYRIGHT 2019 WinnerMicro</center></h2>
;*
;********************************************************************************

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN pxCurrentTCB
        EXTERN vTaskSwitchContext
        EXTERN  __iar_program_start
;        EXTERN  SystemInit
        PUBLIC  __vector_table

;*******************************************************************************
; Fill-up the Vector Table entries with the exceptions ISR address
;*******************************************************************************
        DATA

__vector_table

        DCD  sfe(CSTACK)                        ; Top address of Stack
        DCD  Reset_Handler                      ; Reset Handler
        DCD  NMI_Handler                        ; NMI Handler
        DCD  HardFault_Handler                  ; Hard Fault Handler
        DCD  MemManage_Handler                  ; Memory Management Fault Handler
        DCD  BusFault_Handler                   ; Bus Fault Handler
        DCD  UsageFault_Handler                 ; Usage Fault Handler
        DCD  0                                  ; Reserved
        DCD  0                                  ; Reserved
        DCD  0                                  ; Reserved
        DCD  0                                  ; Reserved
        DCD  SVC_Handler                        ; SVC Handler
        DCD  DebugMon_Handler                   ; Debug Monitor Handler
        DCD  0                                  ; Reserved
        DCD  PendSV_Handler                     ; PendSV Handler
        DCD  OS_CPU_SysTickHandler              ; SysTick Handler

        ; External Interrupt Handler
        DCD  SDIO_RX_IRQHandler
        DCD  SDIO_TX_IRQHandler
        DCD  SDIO_RX_CMD_IRQHandler
        DCD  SDIO_TX_CMD_IRQHandler
        DCD  tls_wl_mac_isr
        DCD  0
        DCD  tls_wl_rx_isr
        DCD  tls_wl_mgmt_tx_isr
        DCD  tls_wl_data_tx_isr
        DCD  PMU_TIMER1_IRQHandler
        DCD  PMU_TIMER0_IRQHandler
        DCD  PMU_GPIO_WAKE_IRQHandler
        DCD  PMU_SDIO_WAKE_IRQHandler
        DCD  DMA_Channel0_IRQHandler
        DCD  DMA_Channel1_IRQHandler
        DCD  DMA_Channel2_IRQHandler
        DCD  DMA_Channel3_IRQHandler
        DCD  DMA_Channel4_7_IRQHandler
        DCD  DMA_BRUST_IRQHandler
        DCD  I2C_IRQHandler
        DCD  ADC_IRQHandler
        DCD  SPI_LS_IRQHandler
        DCD  SPI_HS_IRQHandler
        DCD  UART0_IRQHandler
        DCD  UART1_IRQHandler
        DCD  GPIOA_IRQHandler
        DCD  TIM0_IRQHandler
        DCD  TIM1_IRQHandler
        DCD  TIM2_IRQHandler
        DCD  TIM3_IRQHandler
        DCD  TIM4_IRQHandler
        DCD  TIM5_IRQHandler
        DCD  WDG_IRQHandler
        DCD  PMU_IRQHandler
        DCD  FLASH_IRQHandler
        DCD  PWM_IRQHandler
        DCD  I2S_IRQHandler
        DCD  PMU_RTC_IRQHandler
        DCD  RSA_IRQHandler
        DCD  CRYPTION_IRQHandler
        DCD  GPIOB_IRQHandler
        DCD  UART2_IRQHandler
        DCD  0

        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
;        LDR     R0, =SystemInit
;        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        ldr	r3, =pxCurrentTCB
        ldr r1, [r3]
        ldr r0, [r1]
        ldmia r0!, {r4-r11}
        msr psp, r0
        mov r0, #0
        msr	basepri, r0
        orr r14, r14, #0xd
        bx r14

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        CPSID   I
        mrs r0, psp
        ldr	r3, =pxCurrentTCB
        ldr	r2, [r3]
        stmdb r0!, {r4-r11}
        str r0, [r2]
        stmdb sp!, {r3, r14}
        bl vTaskSwitchContext
        ldmia sp!, {r3, r14}
        ldr r1, [r3]
        ldr r0, [r1]
        ldmia r0!, {r4-r11}
        msr psp, r0
        CPSIE I
        bx r14
        nop

;        PUBWEAK SysTick_Handler
;        SECTION .text:CODE:REORDER:NOROOT(1)
;SysTick_Handler
;        B SysTick_Handler

;        PUBWEAK PendSV_Handler
        PUBWEAK OS_CPU_SysTickHandler
        PUBWEAK SDIO_RX_IRQHandler
        PUBWEAK SDIO_TX_IRQHandler
        PUBWEAK SDIO_RX_CMD_IRQHandler
        PUBWEAK SDIO_TX_CMD_IRQHandler
        PUBWEAK tls_wl_mac_isr
        PUBWEAK tls_wl_rx_isr
        PUBWEAK tls_wl_data_tx_isr
        PUBWEAK tls_wl_mgmt_tx_isr
        PUBWEAK RSV_IRQHandler
        PUBWEAK PMU_RTC_IRQHandler
        PUBWEAK PMU_TIMER1_IRQHandler
        PUBWEAK PMU_TIMER0_IRQHandler
        PUBWEAK PMU_GPIO_WAKE_IRQHandler
        PUBWEAK PMU_SDIO_WAKE_IRQHandler
        PUBWEAK DMA_Channel0_IRQHandler
        PUBWEAK DMA_Channel1_IRQHandler
        PUBWEAK DMA_Channel2_IRQHandler
        PUBWEAK DMA_Channel3_IRQHandler
        PUBWEAK DMA_Channel4_7_IRQHandler
        PUBWEAK DMA_BRUST_IRQHandler
        PUBWEAK I2C_IRQHandler
        PUBWEAK ADC_IRQHandler
        PUBWEAK SPI_LS_IRQHandler
        PUBWEAK SPI_HS_IRQHandler
        PUBWEAK UART0_IRQHandler
        PUBWEAK UART1_IRQHandler
        PUBWEAK GPIOA_IRQHandler
        PUBWEAK TIM0_IRQHandler
        PUBWEAK TIM1_IRQHandler
        PUBWEAK TIM2_IRQHandler
        PUBWEAK TIM3_IRQHandler
        PUBWEAK TIM4_IRQHandler
        PUBWEAK TIM5_IRQHandler
        PUBWEAK WDG_IRQHandler
        PUBWEAK PMU_IRQHandler
        PUBWEAK FLASH_IRQHandler
        PUBWEAK PWM_IRQHandler
        PUBWEAK I2S_IRQHandler
        PUBWEAK PMU_6IRQHandler
        PUBWEAK RSA_IRQHandler
        PUBWEAK CRYPTION_IRQHandler
        PUBWEAK GPIOB_IRQHandler
        PUBWEAK UART2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
;PendSV_Handler
OS_CPU_SysTickHandler
SDIO_RX_IRQHandler
SDIO_TX_IRQHandler
SDIO_RX_CMD_IRQHandler
SDIO_TX_CMD_IRQHandler
tls_wl_mac_isr
tls_wl_rx_isr
tls_wl_data_tx_isr
tls_wl_mgmt_tx_isr          
RSV_IRQHandler
;SEC_RX_IRQHandler          
;SEC_TX_MNGT_IRQHandler          
;SEC_TX_DAT_IRQHandler          
PMU_RTC_IRQHandler          
PMU_TIMER1_IRQHandler       
PMU_TIMER0_IRQHandler       
PMU_GPIO_WAKE_IRQHandler   
PMU_SDIO_WAKE_IRQHandler
DMA_Channel0_IRQHandler
DMA_Channel1_IRQHandler
DMA_Channel2_IRQHandler
DMA_Channel3_IRQHandler
DMA_Channel4_7_IRQHandler
DMA_BRUST_IRQHandler
I2C_IRQHandler
ADC_IRQHandler
SPI_LS_IRQHandler
SPI_HS_IRQHandler
UART0_IRQHandler         
UART1_IRQHandler        
GPIOA_IRQHandler         
TIM0_IRQHandler    
TIM1_IRQHandler       
TIM2_IRQHandler          
TIM3_IRQHandler           
TIM4_IRQHandler            
TIM5_IRQHandler        
WDG_IRQHandler         
PMU_IRQHandler        
FLASH_IRQHandler        
PWM_IRQHandler            
I2S_IRQHandler           
PMU_6IRQHandler      
RSA_IRQHandler
CRYPTION_IRQHandler  
GPIOB_IRQHandler        
UART2_IRQHandler
        B .

        END