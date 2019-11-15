/*******************************************************************************
 System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt.c

  Summary:
    Interrupt vectors mapping

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************

extern uint32_t _stack;

/* Brief default interrupt handler for unused IRQs.*/
void __attribute__((optimize("-O1"),section(".text.Dummy_Handler"),long_call))Dummy_Handler(void)
{
#if defined(__DEBUG) || defined(__DEBUG_D) && defined(__XC32)
    __builtin_software_breakpoint();
#endif
    while (1)
    {
    }
}
/* Device vectors list dummy definition*/
void Reset_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void NonMaskableInt_Handler     ( void ) __attribute__((weak, alias("Dummy_Handler")));
void HardFault_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void MemoryManagement_Handler   ( void ) __attribute__((weak, alias("Dummy_Handler")));
void BusFault_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void UsageFault_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SVCall_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DebugMonitor_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PendSV_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SysTick_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SUPC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void RSTC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void RTC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void RTT_InterruptHandler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void WDT_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PMC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EFC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void UART0_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void UART1_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PIOA_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PIOB_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PIOC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void USART0_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void USART1_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void USART2_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PIOD_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PIOE_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void HSMCI_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TWIHS0_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TWIHS1_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SPI0_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SSC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC0_CH0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC0_CH1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC0_CH2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC1_CH0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC1_CH1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC1_CH2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void AFEC0_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DACC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PWM0_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void ICM_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void ACC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void USBHS_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void MCAN0_INT0_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void MCAN0_INT1_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void MCAN1_INT0_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void MCAN1_INT1_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void GMAC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void AFEC1_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TWIHS2_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SPI1_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void QSPI_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void UART2_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void UART3_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void UART4_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC2_CH0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC2_CH1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC2_CH2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC3_CH0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC3_CH1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC3_CH2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void AES_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TRNG_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void XDMAC_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void ISI_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PWM1_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void FPU_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SDRAMC_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void RSWDT_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void CCW_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void CCF_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void GMAC_Q1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void GMAC_Q2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void IXC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void I2SC0_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void I2SC1_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void GMAC_Q3_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void GMAC_Q4_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void GMAC_Q5_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));



/* Mutiple handlers for vector */



__attribute__ ((section(".vectors")))
const DeviceVectors exception_table=
{
    /* Configure Initial Stack Pointer, using linker-generated symbols */
    .pvStack = (void*) (&_stack),

    .pfnReset_Handler              = ( void * ) Reset_Handler,
    .pfnNonMaskableInt_Handler     = ( void * ) NonMaskableInt_Handler,
    .pfnHardFault_Handler          = ( void * ) HardFault_Handler,
    .pfnMemoryManagement_Handler   = ( void * ) MemoryManagement_Handler,
    .pfnBusFault_Handler           = ( void * ) BusFault_Handler,
    .pfnUsageFault_Handler         = ( void * ) UsageFault_Handler,
    .pfnSVCall_Handler             = ( void * ) SVCall_Handler,
    .pfnDebugMonitor_Handler       = ( void * ) DebugMonitor_Handler,
    .pfnPendSV_Handler             = ( void * ) PendSV_Handler,
    .pfnSysTick_Handler            = ( void * ) SysTick_Handler,
    .pfnSUPC_Handler               = ( void * ) SUPC_Handler,
    .pfnRSTC_Handler               = ( void * ) RSTC_Handler,
    .pfnRTC_Handler                = ( void * ) RTC_Handler,
    .pfnRTT_Handler                = ( void * ) RTT_InterruptHandler,
    .pfnWDT_Handler                = ( void * ) WDT_Handler,
    .pfnPMC_Handler                = ( void * ) PMC_Handler,
    .pfnEFC_Handler                = ( void * ) EFC_Handler,
    .pfnUART0_Handler              = ( void * ) UART0_Handler,
    .pfnUART1_Handler              = ( void * ) UART1_Handler,
    .pfnPIOA_Handler               = ( void * ) PIOA_Handler,
    .pfnPIOB_Handler               = ( void * ) PIOB_Handler,
    .pfnPIOC_Handler               = ( void * ) PIOC_Handler,
    .pfnUSART0_Handler             = ( void * ) USART0_Handler,
    .pfnUSART1_Handler             = ( void * ) USART1_Handler,
    .pfnUSART2_Handler             = ( void * ) USART2_Handler,
    .pfnPIOD_Handler               = ( void * ) PIOD_Handler,
    .pfnPIOE_Handler               = ( void * ) PIOE_Handler,
    .pfnHSMCI_Handler              = ( void * ) HSMCI_Handler,
    .pfnTWIHS0_Handler             = ( void * ) TWIHS0_Handler,
    .pfnTWIHS1_Handler             = ( void * ) TWIHS1_Handler,
    .pfnSPI0_Handler               = ( void * ) SPI0_Handler,
    .pfnSSC_Handler                = ( void * ) SSC_Handler,
    .pfnTC0_CH0_Handler            = ( void * ) TC0_CH0_Handler,
    .pfnTC0_CH1_Handler            = ( void * ) TC0_CH1_Handler,
    .pfnTC0_CH2_Handler            = ( void * ) TC0_CH2_Handler,
    .pfnTC1_CH0_Handler            = ( void * ) TC1_CH0_Handler,
    .pfnTC1_CH1_Handler            = ( void * ) TC1_CH1_Handler,
    .pfnTC1_CH2_Handler            = ( void * ) TC1_CH2_Handler,
    .pfnAFEC0_Handler              = ( void * ) AFEC0_Handler,
    .pfnDACC_Handler               = ( void * ) DACC_Handler,
    .pfnPWM0_Handler               = ( void * ) PWM0_Handler,
    .pfnICM_Handler                = ( void * ) ICM_Handler,
    .pfnACC_Handler                = ( void * ) ACC_Handler,
    .pfnUSBHS_Handler              = ( void * ) USBHS_Handler,
    .pfnMCAN0_INT0_Handler         = ( void * ) MCAN0_INT0_Handler,
    .pfnMCAN0_INT1_Handler         = ( void * ) MCAN0_INT1_Handler,
    .pfnMCAN1_INT0_Handler         = ( void * ) MCAN1_INT0_Handler,
    .pfnMCAN1_INT1_Handler         = ( void * ) MCAN1_INT1_Handler,
    .pfnGMAC_Handler               = ( void * ) GMAC_Handler,
    .pfnAFEC1_Handler              = ( void * ) AFEC1_Handler,
    .pfnTWIHS2_Handler             = ( void * ) TWIHS2_Handler,
    .pfnSPI1_Handler               = ( void * ) SPI1_Handler,
    .pfnQSPI_Handler               = ( void * ) QSPI_Handler,
    .pfnUART2_Handler              = ( void * ) UART2_Handler,
    .pfnUART3_Handler              = ( void * ) UART3_Handler,
    .pfnUART4_Handler              = ( void * ) UART4_Handler,
    .pfnTC2_CH0_Handler            = ( void * ) TC2_CH0_Handler,
    .pfnTC2_CH1_Handler            = ( void * ) TC2_CH1_Handler,
    .pfnTC2_CH2_Handler            = ( void * ) TC2_CH2_Handler,
    .pfnTC3_CH0_Handler            = ( void * ) TC3_CH0_Handler,
    .pfnTC3_CH1_Handler            = ( void * ) TC3_CH1_Handler,
    .pfnTC3_CH2_Handler            = ( void * ) TC3_CH2_Handler,
    .pfnAES_Handler                = ( void * ) AES_Handler,
    .pfnTRNG_Handler               = ( void * ) TRNG_Handler,
    .pfnXDMAC_Handler              = ( void * ) XDMAC_Handler,
    .pfnISI_Handler                = ( void * ) ISI_Handler,
    .pfnPWM1_Handler               = ( void * ) PWM1_Handler,
    .pfnFPU_Handler                = ( void * ) FPU_Handler,
    .pfnSDRAMC_Handler             = ( void * ) SDRAMC_Handler,
    .pfnRSWDT_Handler              = ( void * ) RSWDT_Handler,
    .pfnCCW_Handler                = ( void * ) CCW_Handler,
    .pfnCCF_Handler                = ( void * ) CCF_Handler,
    .pfnGMAC_Q1_Handler            = ( void * ) GMAC_Q1_Handler,
    .pfnGMAC_Q2_Handler            = ( void * ) GMAC_Q2_Handler,
    .pfnIXC_Handler                = ( void * ) IXC_Handler,
    .pfnI2SC0_Handler              = ( void * ) I2SC0_Handler,
    .pfnI2SC1_Handler              = ( void * ) I2SC1_Handler,
    .pfnGMAC_Q3_Handler            = ( void * ) GMAC_Q3_Handler,
    .pfnGMAC_Q4_Handler            = ( void * ) GMAC_Q4_Handler,
    .pfnGMAC_Q5_Handler            = ( void * ) GMAC_Q5_Handler,


};

/*******************************************************************************
 End of File
*/
