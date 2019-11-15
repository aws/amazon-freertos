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
void vPortSVCHandler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DebugMonitor_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void xPortPendSVHandler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void xPortSysTickHandler        ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PM_Handler                 ( void ) __attribute__((weak, alias("Dummy_Handler")));
void MCLK_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void OSCCTRL_XOSC0_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void OSCCTRL_XOSC1_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void OSCCTRL_DFLL_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void OSCCTRL_DPLL0_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void OSCCTRL_DPLL1_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void OSC32KCTRL_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SUPC_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SUPC_BODDET_Handler        ( void ) __attribute__((weak, alias("Dummy_Handler")));
void WDT_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void RTC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_0_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_1_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_2_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_3_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_4_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_5_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_6_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_7_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_8_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_9_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_10_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_11_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_12_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_13_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_14_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EIC_EXTINT_15_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void FREQM_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void NVMCTRL_0_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void NVMCTRL_1_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DMAC_0_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DMAC_1_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DMAC_2_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DMAC_3_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DMAC_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EVSYS_0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EVSYS_1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EVSYS_2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EVSYS_3_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
void EVSYS_OTHER_Handler        ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PAC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void RAMECC_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM0_0_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM0_1_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM0_2_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM0_OTHER_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM1_0_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM1_1_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM1_2_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM1_OTHER_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM2_USART_InterruptHandler ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM3_0_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM3_1_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM3_2_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM3_OTHER_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM4_0_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM4_1_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM4_2_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM4_OTHER_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM5_0_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM5_1_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM5_2_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM5_OTHER_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM6_0_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM6_1_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM6_2_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM6_OTHER_Handler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM7_USART_InterruptHandler ( void ) __attribute__((weak, alias("Dummy_Handler")));
void CAN0_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void CAN1_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void USB_OTHER_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void USB_SOF_HSOF_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void USB_TRCPT0_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void USB_TRCPT1_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void GMAC_InterruptHandler      ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC0_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC0_MC0_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC0_MC1_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC0_MC2_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC0_MC3_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC0_MC4_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC0_MC5_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC1_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC1_MC0_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC1_MC1_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC1_MC2_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC1_MC3_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC2_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC2_MC0_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC2_MC1_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC2_MC2_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC3_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC3_MC0_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC3_MC1_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC4_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC4_MC0_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TCC4_MC1_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC0_TimerInterruptHandler  ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC1_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC2_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC3_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC4_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC5_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC6_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TC7_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PDEC_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PDEC_MC0_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PDEC_MC1_Handler           ( void ) __attribute__((weak, alias("Dummy_Handler")));
void ADC0_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void ADC0_RESRDY_Handler        ( void ) __attribute__((weak, alias("Dummy_Handler")));
void ADC1_OTHER_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
void ADC1_RESRDY_Handler        ( void ) __attribute__((weak, alias("Dummy_Handler")));
void AC_Handler                 ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DAC_OTHER_Handler          ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DAC_EMPTY_0_Handler        ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DAC_EMPTY_1_Handler        ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DAC_RESRDY_0_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void DAC_RESRDY_1_Handler       ( void ) __attribute__((weak, alias("Dummy_Handler")));
void I2S_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PCC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void AES_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void TRNG_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void ICM_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
void PUKCC_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void QSPI_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SDHC0_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
void SDHC1_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));



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
    .pfnSVCall_Handler             = ( void * ) vPortSVCHandler,
    .pfnDebugMonitor_Handler       = ( void * ) DebugMonitor_Handler,
    .pfnPendSV_Handler             = ( void * ) xPortPendSVHandler,
    .pfnSysTick_Handler            = ( void * ) xPortSysTickHandler,
    .pfnPM_Handler                 = ( void * ) PM_Handler,
    .pfnMCLK_Handler               = ( void * ) MCLK_Handler,
    .pfnOSCCTRL_XOSC0_Handler      = ( void * ) OSCCTRL_XOSC0_Handler,
    .pfnOSCCTRL_XOSC1_Handler      = ( void * ) OSCCTRL_XOSC1_Handler,
    .pfnOSCCTRL_DFLL_Handler       = ( void * ) OSCCTRL_DFLL_Handler,
    .pfnOSCCTRL_DPLL0_Handler      = ( void * ) OSCCTRL_DPLL0_Handler,
    .pfnOSCCTRL_DPLL1_Handler      = ( void * ) OSCCTRL_DPLL1_Handler,
    .pfnOSC32KCTRL_Handler         = ( void * ) OSC32KCTRL_Handler,
    .pfnSUPC_OTHER_Handler         = ( void * ) SUPC_OTHER_Handler,
    .pfnSUPC_BODDET_Handler        = ( void * ) SUPC_BODDET_Handler,
    .pfnWDT_Handler                = ( void * ) WDT_Handler,
    .pfnRTC_Handler                = ( void * ) RTC_Handler,
    .pfnEIC_EXTINT_0_Handler       = ( void * ) EIC_EXTINT_0_Handler,
    .pfnEIC_EXTINT_1_Handler       = ( void * ) EIC_EXTINT_1_Handler,
    .pfnEIC_EXTINT_2_Handler       = ( void * ) EIC_EXTINT_2_Handler,
    .pfnEIC_EXTINT_3_Handler       = ( void * ) EIC_EXTINT_3_Handler,
    .pfnEIC_EXTINT_4_Handler       = ( void * ) EIC_EXTINT_4_Handler,
    .pfnEIC_EXTINT_5_Handler       = ( void * ) EIC_EXTINT_5_Handler,
    .pfnEIC_EXTINT_6_Handler       = ( void * ) EIC_EXTINT_6_Handler,
    .pfnEIC_EXTINT_7_Handler       = ( void * ) EIC_EXTINT_7_Handler,
    .pfnEIC_EXTINT_8_Handler       = ( void * ) EIC_EXTINT_8_Handler,
    .pfnEIC_EXTINT_9_Handler       = ( void * ) EIC_EXTINT_9_Handler,
    .pfnEIC_EXTINT_10_Handler      = ( void * ) EIC_EXTINT_10_Handler,
    .pfnEIC_EXTINT_11_Handler      = ( void * ) EIC_EXTINT_11_Handler,
    .pfnEIC_EXTINT_12_Handler      = ( void * ) EIC_EXTINT_12_Handler,
    .pfnEIC_EXTINT_13_Handler      = ( void * ) EIC_EXTINT_13_Handler,
    .pfnEIC_EXTINT_14_Handler      = ( void * ) EIC_EXTINT_14_Handler,
    .pfnEIC_EXTINT_15_Handler      = ( void * ) EIC_EXTINT_15_Handler,
    .pfnFREQM_Handler              = ( void * ) FREQM_Handler,
    .pfnNVMCTRL_0_Handler          = ( void * ) NVMCTRL_0_Handler,
    .pfnNVMCTRL_1_Handler          = ( void * ) NVMCTRL_1_Handler,
    .pfnDMAC_0_Handler             = ( void * ) DMAC_0_Handler,
    .pfnDMAC_1_Handler             = ( void * ) DMAC_1_Handler,
    .pfnDMAC_2_Handler             = ( void * ) DMAC_2_Handler,
    .pfnDMAC_3_Handler             = ( void * ) DMAC_3_Handler,
    .pfnDMAC_OTHER_Handler         = ( void * ) DMAC_OTHER_Handler,
    .pfnEVSYS_0_Handler            = ( void * ) EVSYS_0_Handler,
    .pfnEVSYS_1_Handler            = ( void * ) EVSYS_1_Handler,
    .pfnEVSYS_2_Handler            = ( void * ) EVSYS_2_Handler,
    .pfnEVSYS_3_Handler            = ( void * ) EVSYS_3_Handler,
    .pfnEVSYS_OTHER_Handler        = ( void * ) EVSYS_OTHER_Handler,
    .pfnPAC_Handler                = ( void * ) PAC_Handler,
    .pfnRAMECC_Handler             = ( void * ) RAMECC_Handler,
    .pfnSERCOM0_0_Handler          = ( void * ) SERCOM0_0_Handler,
    .pfnSERCOM0_1_Handler          = ( void * ) SERCOM0_1_Handler,
    .pfnSERCOM0_2_Handler          = ( void * ) SERCOM0_2_Handler,
    .pfnSERCOM0_OTHER_Handler      = ( void * ) SERCOM0_OTHER_Handler,
    .pfnSERCOM1_0_Handler          = ( void * ) SERCOM1_0_Handler,
    .pfnSERCOM1_1_Handler          = ( void * ) SERCOM1_1_Handler,
    .pfnSERCOM1_2_Handler          = ( void * ) SERCOM1_2_Handler,
    .pfnSERCOM1_OTHER_Handler      = ( void * ) SERCOM1_OTHER_Handler,
    .pfnSERCOM2_0_Handler          = ( void * ) SERCOM2_USART_InterruptHandler,
    .pfnSERCOM2_1_Handler          = ( void * ) SERCOM2_USART_InterruptHandler,
    .pfnSERCOM2_2_Handler          = ( void * ) SERCOM2_USART_InterruptHandler,
    .pfnSERCOM2_OTHER_Handler      = ( void * ) SERCOM2_USART_InterruptHandler,
    .pfnSERCOM3_0_Handler          = ( void * ) SERCOM3_0_Handler,
    .pfnSERCOM3_1_Handler          = ( void * ) SERCOM3_1_Handler,
    .pfnSERCOM3_2_Handler          = ( void * ) SERCOM3_2_Handler,
    .pfnSERCOM3_OTHER_Handler      = ( void * ) SERCOM3_OTHER_Handler,
    .pfnSERCOM4_0_Handler          = ( void * ) SERCOM4_0_Handler,
    .pfnSERCOM4_1_Handler          = ( void * ) SERCOM4_1_Handler,
    .pfnSERCOM4_2_Handler          = ( void * ) SERCOM4_2_Handler,
    .pfnSERCOM4_OTHER_Handler      = ( void * ) SERCOM4_OTHER_Handler,
    .pfnSERCOM5_0_Handler          = ( void * ) SERCOM5_0_Handler,
    .pfnSERCOM5_1_Handler          = ( void * ) SERCOM5_1_Handler,
    .pfnSERCOM5_2_Handler          = ( void * ) SERCOM5_2_Handler,
    .pfnSERCOM5_OTHER_Handler      = ( void * ) SERCOM5_OTHER_Handler,
    .pfnSERCOM6_0_Handler          = ( void * ) SERCOM6_0_Handler,
    .pfnSERCOM6_1_Handler          = ( void * ) SERCOM6_1_Handler,
    .pfnSERCOM6_2_Handler          = ( void * ) SERCOM6_2_Handler,
    .pfnSERCOM6_OTHER_Handler      = ( void * ) SERCOM6_OTHER_Handler,
    .pfnSERCOM7_0_Handler          = ( void * ) SERCOM7_USART_InterruptHandler,
    .pfnSERCOM7_1_Handler          = ( void * ) SERCOM7_USART_InterruptHandler,
    .pfnSERCOM7_2_Handler          = ( void * ) SERCOM7_USART_InterruptHandler,
    .pfnSERCOM7_OTHER_Handler      = ( void * ) SERCOM7_USART_InterruptHandler,
    .pfnCAN0_Handler               = ( void * ) CAN0_Handler,
    .pfnCAN1_Handler               = ( void * ) CAN1_Handler,
    .pfnUSB_OTHER_Handler          = ( void * ) USB_OTHER_Handler,
    .pfnUSB_SOF_HSOF_Handler       = ( void * ) USB_SOF_HSOF_Handler,
    .pfnUSB_TRCPT0_Handler         = ( void * ) USB_TRCPT0_Handler,
    .pfnUSB_TRCPT1_Handler         = ( void * ) USB_TRCPT1_Handler,
    .pfnGMAC_Handler               = ( void * ) GMAC_InterruptHandler,
    .pfnTCC0_OTHER_Handler         = ( void * ) TCC0_OTHER_Handler,
    .pfnTCC0_MC0_Handler           = ( void * ) TCC0_MC0_Handler,
    .pfnTCC0_MC1_Handler           = ( void * ) TCC0_MC1_Handler,
    .pfnTCC0_MC2_Handler           = ( void * ) TCC0_MC2_Handler,
    .pfnTCC0_MC3_Handler           = ( void * ) TCC0_MC3_Handler,
    .pfnTCC0_MC4_Handler           = ( void * ) TCC0_MC4_Handler,
    .pfnTCC0_MC5_Handler           = ( void * ) TCC0_MC5_Handler,
    .pfnTCC1_OTHER_Handler         = ( void * ) TCC1_OTHER_Handler,
    .pfnTCC1_MC0_Handler           = ( void * ) TCC1_MC0_Handler,
    .pfnTCC1_MC1_Handler           = ( void * ) TCC1_MC1_Handler,
    .pfnTCC1_MC2_Handler           = ( void * ) TCC1_MC2_Handler,
    .pfnTCC1_MC3_Handler           = ( void * ) TCC1_MC3_Handler,
    .pfnTCC2_OTHER_Handler         = ( void * ) TCC2_OTHER_Handler,
    .pfnTCC2_MC0_Handler           = ( void * ) TCC2_MC0_Handler,
    .pfnTCC2_MC1_Handler           = ( void * ) TCC2_MC1_Handler,
    .pfnTCC2_MC2_Handler           = ( void * ) TCC2_MC2_Handler,
    .pfnTCC3_OTHER_Handler         = ( void * ) TCC3_OTHER_Handler,
    .pfnTCC3_MC0_Handler           = ( void * ) TCC3_MC0_Handler,
    .pfnTCC3_MC1_Handler           = ( void * ) TCC3_MC1_Handler,
    .pfnTCC4_OTHER_Handler         = ( void * ) TCC4_OTHER_Handler,
    .pfnTCC4_MC0_Handler           = ( void * ) TCC4_MC0_Handler,
    .pfnTCC4_MC1_Handler           = ( void * ) TCC4_MC1_Handler,
    .pfnTC0_Handler                = ( void * ) TC0_TimerInterruptHandler,
    .pfnTC1_Handler                = ( void * ) TC1_Handler,
    .pfnTC2_Handler                = ( void * ) TC2_Handler,
    .pfnTC3_Handler                = ( void * ) TC3_Handler,
    .pfnTC4_Handler                = ( void * ) TC4_Handler,
    .pfnTC5_Handler                = ( void * ) TC5_Handler,
    .pfnTC6_Handler                = ( void * ) TC6_Handler,
    .pfnTC7_Handler                = ( void * ) TC7_Handler,
    .pfnPDEC_OTHER_Handler         = ( void * ) PDEC_OTHER_Handler,
    .pfnPDEC_MC0_Handler           = ( void * ) PDEC_MC0_Handler,
    .pfnPDEC_MC1_Handler           = ( void * ) PDEC_MC1_Handler,
    .pfnADC0_OTHER_Handler         = ( void * ) ADC0_OTHER_Handler,
    .pfnADC0_RESRDY_Handler        = ( void * ) ADC0_RESRDY_Handler,
    .pfnADC1_OTHER_Handler         = ( void * ) ADC1_OTHER_Handler,
    .pfnADC1_RESRDY_Handler        = ( void * ) ADC1_RESRDY_Handler,
    .pfnAC_Handler                 = ( void * ) AC_Handler,
    .pfnDAC_OTHER_Handler          = ( void * ) DAC_OTHER_Handler,
    .pfnDAC_EMPTY_0_Handler        = ( void * ) DAC_EMPTY_0_Handler,
    .pfnDAC_EMPTY_1_Handler        = ( void * ) DAC_EMPTY_1_Handler,
    .pfnDAC_RESRDY_0_Handler       = ( void * ) DAC_RESRDY_0_Handler,
    .pfnDAC_RESRDY_1_Handler       = ( void * ) DAC_RESRDY_1_Handler,
    .pfnI2S_Handler                = ( void * ) I2S_Handler,
    .pfnPCC_Handler                = ( void * ) PCC_Handler,
    .pfnAES_Handler                = ( void * ) AES_Handler,
    .pfnTRNG_Handler               = ( void * ) TRNG_Handler,
    .pfnICM_Handler                = ( void * ) ICM_Handler,
    .pfnPUKCC_Handler              = ( void * ) PUKCC_Handler,
    .pfnQSPI_Handler               = ( void * ) QSPI_Handler,
    .pfnSDHC0_Handler              = ( void * ) SDHC0_Handler,
    .pfnSDHC1_Handler              = ( void * ) SDHC1_Handler,


};

/*******************************************************************************
 End of File
*/
