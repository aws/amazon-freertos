/***************************************************************************//**
* \file cy_capsense_csd.c
* \version 2.0
*
* \brief
* This file defines the data structure global variables and provides
* implementation for the low-level functions of the CSD part of
* the Sensing module. The file contains the functions used for the CSD HW block
* initialization, calibration, and scanning.
*
********************************************************************************
* \copyright
* Copyright 2018-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include <stddef.h>
#include "cy_device_headers.h"
#include "cy_sysclk.h"
#include "cy_syslib.h"
#include "cy_gpio.h"
#include "cy_csd.h"

#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"
#include "cy_capsense_sensing.h"
#include "cy_capsense_csd.h"

#if defined(CY_IP_MXCSDV2)


/*******************************************************************************
* Local definition
*******************************************************************************/
/* CY_ID285392, CY_ID515 */
#define CY_CAPSENSE_FILTER_DELAY_MAX                        (CY_CAPSENSE_CSD_CONFIG_FILTER_DELAY_4_CYCLES)
#define CY_CAPSENSE_EXTRA_COUNTS_MAX                        (CY_CAPSENSE_FILTER_DELAY_MAX + 5u + 20u)
#define CY_CAPSENSE_16_BIT_RESOLUTION                       (16uL)

/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/

__STATIC_INLINE void Cy_CapSense_CSDChangeClkFreq(uint32_t channelIndex, cy_stc_capsense_context_t * context);

static void Cy_CapSense_CSDInitNextSnsScan(cy_stc_capsense_context_t * context);
static void Cy_CapSense_CSDInitNextChScan(cy_stc_capsense_context_t * context);

static void Cy_CapSense_CSDEnableShieldElectrodes(const cy_stc_capsense_context_t * context);
static void Cy_CapSense_CSDCmodPrecharge(cy_stc_capsense_context_t * context);
static void Cy_CapSense_CSDTriggerScan(cy_stc_capsense_context_t * context);

static void Cy_CapSense_CSDDischargeCmod(cy_stc_capsense_context_t * context);
static void Cy_CapSense_CSDCalibrate(uint32_t widgetId, uint32_t target, 
                                                            cy_stc_capsense_context_t * context);
static uint32_t Cy_CapSense_CSDSwitchIdacGain(cy_stc_capsense_context_t * context);
static void Cy_CapSense_CSDNormalizeIdac(
                cy_stc_capsense_widget_config_t const * ptrWdConfig, 
                uint32_t target,
                const cy_stc_capsense_context_t * context);
/** \} \endcond */

/*******************************************************************************
* Function Name: Cy_CapSense_CSDDisableMode
****************************************************************************//**
*
* This function disables CSD mode.
*
* To disable CSD mode the following tasks are performed:
* 1. Disconnect Cmod from AMUXBUS-A.
* 2. Disconnect previous CSX electrode if it has been connected.
* 3. Disconnect Csh from AMUXBUS-B.
* 4. Disable Shield Electrodes.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDDisableMode(cy_stc_capsense_context_t * context)
{
    /* Cmod pin to default state */
    Cy_CapSense_SsConfigPinRegisters(context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod, CY_GPIO_DM_ANALOG, CY_CAPSENSE_HSIOM_SEL_GPIO);

    if (0u != context->ptrCommonConfig->csdShieldEn)
    {
        /* Disconnect shields */
        Cy_CapSense_CSDDisableShieldElectrodes(context);
        
        if (0u != context->ptrCommonConfig->csdCTankShieldEn)
        {
            /* Csh pin to default state */
            Cy_CapSense_SsConfigPinRegisters(context->ptrCommonConfig->portCsh, (uint32_t)context->ptrCommonConfig->pinCsh, CY_GPIO_DM_ANALOG, CY_CAPSENSE_HSIOM_SEL_GPIO);
        }
    }
    
    /* Disconnect IDACA, IDACB and RefGen input */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_REFGEN_SEL, 0u);

    /* Disconnect previous CSD sensor if it has been connected */
    Cy_CapSense_CSDSnsStateCheck(context);
}
    

/*******************************************************************************
* Function Name: Cy_CapSense_CSDEnableShieldElectrodes
****************************************************************************//**
*
* This internal function initializes Shield Electrodes.
*
* The function sets the bit in the HSIOM register which enables the shield electrode
* functionality on the pin. The port and pin configurations are stored in
* the cy_capsense_shieldIoList structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSDEnableShieldElectrodes(const cy_stc_capsense_context_t * context)
{
    uint32_t shieldIndex;
    const cy_stc_capsense_pin_config_t * ptrShieldPin = context->ptrShieldPinConfig;

    for (shieldIndex = 0u; shieldIndex < context->ptrCommonConfig->csdShieldNumPin; shieldIndex++)
    {
        Cy_CapSense_SsConfigPinRegisters(ptrShieldPin->pcPtr, (uint32_t)ptrShieldPin->pinNumber, CY_GPIO_DM_STRONG_IN_OFF, CY_CAPSENSE_HSIOM_SEL_CSD_SHIELD);
        ptrShieldPin++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDDisableShieldElectrodes
****************************************************************************//**
*
* This internal function disables Shield Electrodes.
*
* The function resets the bit in the HSIOM register which disables the shield
* electrode functionality on the pin. The port and pin configurations are
* stored in  the cy_capsense_shieldIoList structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDDisableShieldElectrodes(const cy_stc_capsense_context_t * context)
{
    uint32_t shieldIndex;
    uint32_t interruptState;
    const cy_stc_capsense_pin_config_t * ptrShieldPin = context->ptrShieldPinConfig;

    for (shieldIndex = 0u; shieldIndex < context->ptrCommonConfig->csdShieldNumPin; shieldIndex++)
    {
        Cy_CapSense_SsConfigPinRegisters(ptrShieldPin->pcPtr, (uint32_t)ptrShieldPin->pinNumber, CY_GPIO_DM_STRONG, CY_CAPSENSE_HSIOM_SEL_GPIO);

        interruptState = Cy_SysLib_EnterCriticalSection();
        Cy_GPIO_Clr(ptrShieldPin->pcPtr, (uint32_t)ptrShieldPin->pinNumber);
        Cy_SysLib_ExitCriticalSection(interruptState);

        ptrShieldPin++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDInitialize
****************************************************************************//**
*
* This function initializes the CSD module.
*
* The function performs the following steps:
* 1) Sets GPIO output to "0" for all sensor pins;
* 2) Connects CMOD to AMUXBUS-A and to CSDBUS-A;
* 3) Connects CMOD to (sense path) to CSDCOMP;
* 4) Connects Csh_tank to AMUXBUS-B and to CSDBUS-B;
* 5) Connects VREF to REFGEN;
* 6) Configures REFGEN and sets the reference voltage;
* 7) Connects VREF to CSDCOMP and HSCOMP;
* 8) Configures IDAC and connect to CSDBUS-A (to drive CMOD);
* 9) Configures ModClk;
* 10) Configure SnsClk source;
* 11) Sets other CSD configurations (Csd Auto Zero time,
* Sample Init period, interrupts,
* CMOD and Csh_tank/shield initialization switch resistance).
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDInitialize(cy_stc_capsense_context_t * context)
{
    uint32_t interruptState;
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;
    
    /* Set all the sensors to inactive state */
    Cy_CapSense_CSDClearSensors(context);
    Cy_CapSense_DischargeExtCapacitors(context);

    if (0u != context->ptrCommonConfig->csdShieldEn)
    {
        /* Connect shields to AMUX-B bus (config HSIOM regs) */
        Cy_CapSense_CSDEnableShieldElectrodes(context);
        
        if (0u != context->ptrCommonConfig->csdCTankShieldEn)
        {
            Cy_CapSense_SsConfigPinRegisters(context->ptrCommonConfig->portCsh, (uint32_t)context->ptrCommonConfig->pinCsh, CY_GPIO_DM_ANALOG, CY_CAPSENSE_HSIOM_SEL_AMUXB);
        }
    }
    
    /* Initialize the unused CSD IP registers to default state */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SENSE_DUTY, CY_CAPSENSE_CSD_SENSE_DUTY_SENSE_POL_PHI_HIGH);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR_SET, CY_CAPSENSE_DEFAULT_CSD_INTR_SET);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_DSI_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_DSI_SEL);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_ADC_CTL, CY_CAPSENSE_DEFAULT_CSD_ADC_CTL);
    
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_SHIELD_SEL, 0u);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_TANK_SEL, context->ptrInternalContext->csdRegSwFwTankSelScan);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_AMUXBUF_SEL, context->ptrInternalContext->csdRegSwAmuxbufSel);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_P_SEL, context->ptrInternalContext->csdRegSwHsPSelScan);
    
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_N_SEL, CY_CAPSENSE_DEFAULT_SW_HS_N_SEL);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_HSCMP, context->ptrInternalContext->csdRegHscmpScan);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IO_SEL, context->ptrInternalContext->csdRegIoSel);
    
    /* Connection AMUXBUS-A to CSDBUS-A / AMUXBUS-B to CSDBUS-B */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_BYP_SEL, context->ptrInternalContext->csdRegSwBypSel);

    /* Connect CMOD to AMUXBUS-A */
    interruptState = Cy_SysLib_EnterCriticalSection();
    Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod, CY_GPIO_DM_ANALOG);
    Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod, CY_CAPSENSE_HSIOM_SEL_AMUXA);
    Cy_SysLib_ExitCriticalSection(interruptState);

    /* Set Csd Auto Zero time (set AZ_TIME bitmask) */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_TIME, 0u);
    /* Select CMOD and Csh_tank/shield initialization switch resistance */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_RES, context->ptrInternalContext->csdRegSwResScan);
    /* Set the number of dummy fine initialization cycles */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_INIT_CNT, (uint32_t)context->ptrCommonConfig->csdFineInitTime);

    /* Connect CMOD to (sense path) to CSDCOMP */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_CMP_P_SEL, context->ptrInternalContext->csdRegSwCmpPSel);

    /* Configure VREF */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_REFGEN_SEL, 
        context->ptrInternalContext->regSwRefGenSel | CY_CAPSENSE_CSD_SW_REFGEN_SEL_SW_IAIB_MSK);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_AMBUF, context->ptrInternalContext->csdRegAmuxbufInit);

    /* Connect VREFHI to HSCOMP */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_N_SEL, CY_CAPSENSE_CSD_SW_HS_N_SEL_SW_HCRH_STATIC_CLOSE);

    /* Connect VREFHI (from RefGen) to CSDCOMP when Vdda >= 2 V */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_CMP_N_SEL, CY_CAPSENSE_CSD_SW_CMP_N_SEL_SW_SCRH_STATIC_CLOSE);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_REFGEN, context->ptrInternalContext->csdRegRefgen);

    /* Configure IDACs mode */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IDACA, context->ptrInternalContext->csdIdacAConfig);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IDACB, context->ptrInternalContext->csdIdacBConfig);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CONFIG, context->ptrInternalContext->csdRegConfig);

    /* Configure ModClk */
    Cy_CapSense_SetClkDivider((uint32_t)context->ptrCommonContext->modCsdClk - 1u, context);
    
    /* Setup ISR handler to single-sensor scan function */
    if(NULL != ptrFptrCfg->fptrCSDScanISR)
    {
        context->ptrActiveScanSns->ptrISRCallback = ptrFptrCfg->fptrCSDScanISR;
    }

    context->ptrActiveScanSns->mfsChannelIndex = 0u;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDSnsStateCheck
****************************************************************************//**
*
* Checks if electrodes were previously connected using
* Cy_CapSense_CSDSetupWidgetExt() and if yes disconnects them.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDSnsStateCheck(cy_stc_capsense_context_t * context)
{
    if (CY_CAPSENSE_SNS_CONNECTED == context->ptrActiveScanSns->connectedSnsState)
    {
        Cy_CapSense_CSDDisconnectSnsExt(context);
    }
}

/*******************************************************************************
* Function Name: Cy_CapSense_CSDSetUpIdacs
****************************************************************************//**
*
* This function writes both IDACs available in CSD method into corresponding 
* the CSD HW block registers.
*
* Modulator IDAC is taken from widget context structure and Compensation
* IDAC is taken from sensor context structure.
* The cy_stc_active_scan_sns_t structure must be updated to the current 
* widget/sensor by Cy_CapSense_InitActivePtr() before calling this function.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDSetUpIdacs(cy_stc_capsense_context_t * context)
{
    uint32_t regValue;

    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;

    const uint32_t idacaBitsToWrite = CSD_IDACA_VAL_Msk |
                                      CSD_IDACA_RANGE_Msk |
                                      CSD_IDACA_LEG1_EN_Msk |
                                      CSD_IDACA_LEG2_EN_Msk;

    const uint32_t idacbBitsToWrite = CSD_IDACB_VAL_Msk |
                                      CSD_IDACB_RANGE_Msk |
                                      CSD_IDACB_LEG1_EN_Msk |
                                      CSD_IDACB_LEG2_EN_Msk |
                                      CSD_IDACB_LEG3_EN_Msk;

    /* IDAC A Code */
    if (ptrActive->ptrWdConfig->numCols > ptrActive->sensorIndex)
    {
        regValue = ptrActive->ptrWdContext->idacMod[ptrActive->mfsChannelIndex];
    }
    else
    {
        regValue = ptrActive->ptrWdContext->rowIdacMod[ptrActive->mfsChannelIndex];
    }

    regValue |= context->ptrCommonConfig->idacGainTable[ptrActive->ptrWdContext->idacGainIndex].gainReg;

    Cy_CSD_WriteBits(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IDACA, idacaBitsToWrite, regValue);

    if (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csdIdacCompEn)
    {
        /* IDAC B Code */
        regValue = ptrActive->ptrSnsContext->idacComp;
        regValue |= context->ptrCommonConfig->idacGainTable[ptrActive->ptrWdContext->idacGainIndex].gainReg;
        
        Cy_CSD_WriteBits(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IDACB, idacbBitsToWrite, regValue);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDGetNumberOfConversions
****************************************************************************//**
*
* This function calculate number of sub-conversions.
*
* \param snsClkDivider 
* The divider value of the sense clock.
*
* \param resolution
* The widget resolution.
*
* \param snsClkSrc
* The widget Sense Clock Source.
*
* \return 
* Returns the number of sub-conversions.
*
*******************************************************************************/
uint32_t Cy_CapSense_CSDGetNumberOfConversions(uint32_t snsClkDivider, uint32_t resolution, uint32_t snsClkSrc)
{
    /* CY_ID304273 */
    uint32_t conversionsNum = 1uL << resolution;
    uint32_t extraCounts = 0u;
    
    if (0u == snsClkDivider)
    {
        snsClkDivider = 1u;
    }
    
    if ((CY_CAPSENSE_CLK_SOURCE_PRS8 == snsClkSrc) || (CY_CAPSENSE_CLK_SOURCE_PRS12 == snsClkSrc))
    {
        snsClkDivider <<= 1u;
    }
    
    if(CY_CAPSENSE_16_BIT_RESOLUTION <= resolution)
    {
        /* CY_ID285392 */
        extraCounts = CY_CAPSENSE_EXTRA_COUNTS_MAX;
    }
    
    conversionsNum = (conversionsNum - extraCounts) / snsClkDivider;

    return((conversionsNum > 0u) ? (conversionsNum) : 1u);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDConfigClock
****************************************************************************//**
*
* This function configure sense clock for different modes.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDConfigClock(cy_stc_capsense_context_t * context)
{
    const cy_stc_capsense_widget_context_t * ptrWdCxt = context->ptrActiveScanSns->ptrWdContext;
    uint32_t snsClkDivider;
    uint32_t snsClkSrc = (uint32_t)ptrWdCxt->snsClkSource & ((uint32_t)~(uint32_t)CY_CAPSENSE_CLK_SOURCE_AUTO_MASK);
    uint32_t regConfig = 0u;

    /* Getting row clock divider for matrix buttons or touchpad widgets */
    if (context->ptrActiveScanSns->ptrWdConfig->numCols <= context->ptrActiveScanSns->sensorIndex)
    {
        snsClkDivider = ptrWdCxt->rowSnsClk;
    }
    else
    {
        snsClkDivider = ptrWdCxt->snsClk;
    }
    
    /* Configuring PRS SEL_BIT  and decreasing divider in case of PRS */
    if ((CY_CAPSENSE_CLK_SOURCE_PRS8 == snsClkSrc) || (CY_CAPSENSE_CLK_SOURCE_PRS12 == snsClkSrc))
    {
        regConfig = CY_CAPSENSE_CSD_SENSE_PERIOD_SEL_LFSR_MSB_MSK;
        snsClkDivider >>= 1u;
    }
    
    /* Check divider value */
    if (0u == snsClkDivider)
    {
        snsClkDivider = 1u;
    }

    regConfig |= ((snsClkSrc << CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_SIZE_POS) | (snsClkDivider - 1u));
    
    /* Update reg value with divider and configuration */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SENSE_PERIOD, regConfig);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDCalculateScanDuration
****************************************************************************//**
*
* Calculates Scan Duration which is defined by scan resolution
*
* The function calculates the number of conversions and updates
* SEQ_NORM_CNT register. The number of conversions depends on resolution and
* snsClk divider.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDCalculateScanDuration(cy_stc_capsense_context_t * context)
{
    uint32_t subConv;
    cy_stc_capsense_widget_context_t * ptrWdCxt = context->ptrActiveScanSns->ptrWdContext;
    uint32_t divider = ptrWdCxt->snsClk;
    
    if (context->ptrActiveScanSns->ptrWdConfig->numCols <= context->ptrActiveScanSns->sensorIndex)
    {
        divider = ptrWdCxt->rowSnsClk;
    }

    subConv = Cy_CapSense_CSDGetNumberOfConversions(divider, (uint32_t)ptrWdCxt->resolution, 
                ((uint32_t)ptrWdCxt->snsClkSource & ((uint32_t)~(uint32_t)CY_CAPSENSE_CLK_SOURCE_AUTO_MASK)));

    /* Write number of sub-conversions into the CSD HW block register */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_NORM_CNT, (subConv & CY_CAPSENSE_CSD_SEQ_NORM_CNT_CONV_CNT_MSK));
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDSetupWidget
****************************************************************************//**
*
* Performs the initialization required to scan the specified CSD widget.
*
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_SetupWidget() function should be used instead.
*
* This function prepares the middleware to scan all the sensors in the 
* specified CSD widget by executing the following tasks:
* 1. Configure the CSD HW block if it is not configured to perform the
* CSD sensing method used by the specified widget.
* 2. Initialize the CSD HW block with specific sensing configuration (e.g.
* sensor clock, scan resolution) used by the widget.
* 3. Disconnect all previously connected electrodes, if the electrodes
* connected by the Cy_CapSense_CSDSetupWidgetExt(),
* Cy_CapSense_CSXSetupWidgetExt() functions are not disconnected.
*
* This function does not start sensor scanning. The Cy_CapSense_CSDScan()
* function must be called to start the scan sensors in the widget. If this
* function is called more than once, it does not break the middleware 
* operation, but only the last initialized widget is in effect.
* Calling this function directly from the application program is not
* recommended. This function is used to implement only the user's specific
* use cases (for example faster execution).
*
* The status of a sensor scan must be checked using the Cy_CapSense_IsBusy() 
* function prior to starting a next scan or setting up another widget.
* 
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDSetupWidget(uint32_t widgetId, cy_stc_capsense_context_t * context)
{
    /* Setup new scanning mode */
    Cy_CapSense_SwitchSensingMode((uint8_t)CY_CAPSENSE_SENSE_METHOD_CSD_E, context);

    /* Disconnect previous sensors if they have been connected */
    Cy_CapSense_CSDSnsStateCheck(context);

    /* Save widget Id to have assess to it after scanning  */
    Cy_CapSense_InitActivePtr(widgetId, 0u, context);

    /* Set up scanning resolution (Number of conversion) */
    Cy_CapSense_CSDCalculateScanDuration(context);
    Cy_CapSense_CSDConfigClock(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDSetupWidgetExt
****************************************************************************//**
*
* Performs extended initialization required to scan a specified sensor in 
* a widget using CSD sensing method.
* 
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_SetupWidgetExt() function should be used instead.
*
* This function performs the same tasks of Cy_CapSense_CSDSetupWidget() and 
* also connects and configures specified sensor for scan. Hence this 
* function, along with Cy_CapSense_CSDScanExt() function, can be used to 
* scan a specific sensor in the widget.
* 
* This function should be called for widget that is configured to use 
* CSD sensing method, using this function on a non-CSD sensing widget 
* would cause unexpected result.
* 
* This function requires using the Cy_CapSense_CSDScanExt() function to 
* initiate a scan.
* 
* Calling this function directly from the application layer is not 
* recommended. This function is used to implement only the user's 
* specific use cases (for example faster execution).
* 
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param sensorId
* Specifies the ID number of the sensor within the widget. A macro for the 
* sensor ID within a specified widget can be found in the cycfg_capsense.h 
* file defined as CY_CAPSENSE_<WIDGET_NAME>_SNS<SENSOR_NUMBER>_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDSetupWidgetExt(uint32_t widgetId, uint32_t sensorId, cy_stc_capsense_context_t * context)
{
    /* Switch Mode if needed */
    Cy_CapSense_SwitchSensingMode((uint8_t)CY_CAPSENSE_SENSE_METHOD_CSD_E, context);

    /* Disconnect previous sensors if they have been connected */
    Cy_CapSense_CSDSnsStateCheck(context);

    /* Setup new widget/sensor */
    Cy_CapSense_InitActivePtr(widgetId, sensorId, context);
    Cy_CapSense_CSDConnectSnsExt(context);
    Cy_CapSense_CSDSetUpIdacs(context);
    Cy_CapSense_CSDCalculateScanDuration(context);
    Cy_CapSense_CSDConfigClock(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDStartSample
****************************************************************************//**
*
* Starts the CSD conversion.
*
* This function assumes that the CSD HW block is already set up using
* the Cy_CapSense_CSDSetupWidget() and the sensor port-pin is connected to the CSD
* block using Cy_CapSense_CSDConnectSns().
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDStartSample(cy_stc_capsense_context_t * context)
{
    if(NULL != context->ptrCommonContext->ptrSSCallback)
    {
        context->ptrCommonContext->ptrSSCallback(context->ptrActiveScanSns);
    }

    /* Precharging Cmod and Csh */
    Cy_CapSense_CSDCmodPrecharge(context);

    /* Trigger Scan */
    Cy_CapSense_CSDTriggerScan(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDScanExt
****************************************************************************//**
*
* Starts the CSD conversion on the preconfigured sensor. 
* 
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_ScanExt() function should be used instead.
*
* This function performs scanning of a specific sensor in the widget 
* previously initialized using the Cy_CapSense_CSDSetupWidgetExt() function. 
* 
* This function is called when no scanning is in progress. 
* I.e. Cy_CapSense_IsBusy() returns a non-busy status and the widget must 
* be preconfigured using Cy_CapSense_CSDSetupWidgetExt() function prior 
* to calling this function. Calling this function directly from 
* the application program is not recommended. This function is used to 
* implement only the user's specific use cases (for example faster execution).
* 
* This function does not disconnect sensor GPIOs from CSD HW block at the 
* end of a scan, therefore making subsequent scan of the same sensor is faster. 
* If sensor needs to be disconnected after the scan, 
* Cy_CapSense_CSDDisconnectSns() function can be used.
* 
* Calling Cy_CapSense_SetupWidget(), Cy_CapSense_CSDSetupWidget(), 
* Cy_CapSense_ScanAllWidgets(), or if Cy_CapSense_RunTuner() returns 
* CY_CAPSENSE_STATUS_RESTART_DONE status invalidated initialization 
* made by this function.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDScanExt(cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;

    /* Set MFS channel index to 0 */
    ptrActive->mfsChannelIndex = 0u;

    /* Initialize the Active Context pointer with the CH0 context */
    ptrActive->ptrSnsContext = &ptrActive->ptrWdConfig->ptrSnsContext[ptrActive->sensorIndex];

    /* Set Start of sensor scan flag */
    Cy_CapSense_SetBusyFlags(context);

    /* Set scope flag */
    ptrActive->scanScopeSns = CY_CAPSENSE_SCAN_SCOPE_SGL_SNS;

    /* Initiate scan */
    Cy_CapSense_CSDStartSample(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDScan
****************************************************************************//**
*
* This function initiates a scan for the sensors of the widget initialized 
* by the Cy_CapSense_CSDSetupWidget() function.
*
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_Scan() function should be used instead.
*
* This function does the following tasks:
* 1. Connects the first sensor of the widget.
* 2. Configures the IDAC value.
* 3. Starts scanning for the first sensor in the widget.
*
* This function is called by the Cy_CapSense_Scan() if the given
* widget uses the CSD sensing method.
*
* Calling this function directly from the application program is not
* recommended. This function is used to implement only the user's specific
* use cases (for example faster execution).
*
* This function is called when no scanning is in progress. I.e.
* Cy_CapSense_IsBusy() returns a non-busy status and the widget must be
* preconfigured using the Cy_CapSense_CSDSetupWidget() function prior
* to calling this function.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDScan(cy_stc_capsense_context_t * context)
{
    Cy_CapSense_InitActivePtrSns(0u, context);
        
    /* Setup Idac Value */
    Cy_CapSense_CSDSetUpIdacs(context);
    Cy_CapSense_CSDConnectSnsExt(context);

    /* Set Start of sensor scan flag */
    Cy_CapSense_SetBusyFlags(context);
    /* Set scope flag */
    context->ptrActiveScanSns->scanScopeSns = CY_CAPSENSE_SCAN_SCOPE_ALL_SNS;
    /* Initiate scan */
    Cy_CapSense_CSDStartSample(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDConnectSnsExt
****************************************************************************//**
*
* Connects a ganged sensor port-pin to the CSD HW block via the AMUX bus.
*
* The function gets the IO configuration registers addresses, their shifts and
* masks from the cy_stc_capsense_pin_config_t object. Based on this data, it
* updates the HSIOM and PC registers.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDConnectSnsExt(cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;
    const cy_stc_capsense_electrode_config_t * ptrActiveSns = ptrActive->ptrEltdConfig;
    const cy_stc_capsense_pin_config_t * ptrActivePin = ptrActiveSns->ptrPin;
    uint32_t i;

    /* Connect all pins of current sensors */
    for(i = 0u; i < ptrActiveSns->numPins; i++)
    {
        Cy_CapSense_CSDConnectSns(ptrActivePin, context);
        ptrActivePin++;
    }
    ptrActive->connectedSnsState = CY_CAPSENSE_SNS_CONNECTED;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDDisconnectSnsExt
****************************************************************************//**
*
* Disconnects a ganged sensor port-pin from the CSD HW block and AMUX bus.
* Sets the default state of the un-scanned sensor.
*
* The function gets the IO configuration registers addresses, their shifts and
* masks from the cy_stc_capsense_pin_config_t object. Based on this data and
* the Inactive sensor connection parameter, it updates the HSIOM, PC, and DR registers.
* The HSIOM register is updated only when the Inactive sensor connection parameter
* is set to Shield.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDDisconnectSnsExt(cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;
    const cy_stc_capsense_electrode_config_t * ptrActiveSns = ptrActive->ptrEltdConfig;
    const cy_stc_capsense_pin_config_t * ptrActivePin = ptrActiveSns->ptrPin;
    
    uint32_t i;

    /* Disconnect all pins of current sensors */
    for(i = 0u; i < ptrActiveSns->numPins; i++)
    {
        Cy_CapSense_CSDDisconnectSns(ptrActivePin, context);
        ptrActivePin++;
    }
    ptrActive->connectedSnsState = CY_CAPSENSE_SNS_DISCONNECTED;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDConnectSns
****************************************************************************//**
*
* Connects port pin to the CSD HW block using AMUX bus.
* 
* This function can be used to customize the default sensor connection 
* by connecting one or more pins to an existing sensor prior to initiating 
* scan of the sensor.
* 
* The function ignores whether the sensor is a ganged sensor and 
* connects only a specified port pin to the CSD HW block. This function can 
* only use GPIOs that are already assigned to CapSense middleware.
* 
* The functions that perform a setup and scan of a sensor/widget do not 
* take into account changes in the design made by 
* the Cy_CapSense_CSDConnectSns() function, hence all GPIOs connected 
* using this function must be disconnected using 
* the Cy_CapSense_CSDDisconnectSns() function prior to initializing 
* new widgets. Use this function in StartSample 
* callback (see the \ref group_capsense_callbacks section for details) 
* or with low-level functions that perform a single-sensor scanning.
* 
* Scanning should be completed before calling this function.
*
* \param snsAddrPtr
* Specifies the pointer to the cy_stc_capsense_pin_config_t object belonging to
* a sensor that is to be connected to the CSD HW block.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \funcusage
* 
* An example of using the function to perform port pin re-connection: 
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_CSDConnect
* 
*******************************************************************************/
void Cy_CapSense_CSDConnectSns(
                const cy_stc_capsense_pin_config_t * snsAddrPtr, 
                cy_stc_capsense_context_t * context)
{
    Cy_CapSense_SsConfigPinRegisters(snsAddrPtr->pcPtr, (uint32_t)snsAddrPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF, CY_CAPSENSE_HSIOM_SEL_CSD_SENSE);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDDisconnectSns
****************************************************************************//**
*
* Disconnects port pin from the CSD HW block by disconnecting it from AMUX bus.
* 
* This function can be used to disconnect a sensor connected 
* using the Cy_CapSense_CSDConnectSns() function. In addition, this 
* function can be used to customize a default sensor connection by 
* disconnecting one or more already connected sensors prior to 
* initiating a scan of the sensor.
* 
* This function works identically to the Cy_CapSense_CSDConnectSns() function 
* except it disconnects the specified port pin used by the sensor.
*
* Scanning should be completed before calling this function.
*
* \param  snsAddrPtr
* Specifies the pointer to the cy_stc_capsense_pin_config_t object belonging to
* a sensor that should be disconnected from the CSD HW block.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \funcusage
* 
* An example of using the function to perform port pin re-connection: 
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_CSDConnect
* 
*******************************************************************************/
void Cy_CapSense_CSDDisconnectSns(
                const cy_stc_capsense_pin_config_t * snsAddrPtr, 
                const cy_stc_capsense_context_t * context)
{
    uint32_t interruptState;
    
    Cy_CapSense_SsConfigPinRegisters(snsAddrPtr->pcPtr, (uint32_t)snsAddrPtr->pinNumber, 
                                     context->ptrInternalContext->csdInactiveSnsDm, 
                                     context->ptrInternalContext->csdInactiveSnsHsiom);

    interruptState = Cy_SysLib_EnterCriticalSection();
    Cy_GPIO_Clr(snsAddrPtr->pcPtr, (uint32_t)snsAddrPtr->pinNumber);
    Cy_SysLib_ExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDClearSensors
****************************************************************************//**
*
* Resets all the CSD sensors to the non-sampling state by sequentially
* disconnecting all the sensors from the Analog MUX bus and putting them to
* an inactive state.
*
* The function goes through all the widgets and updates appropriate bits in
* the IO HSIOM, PC and DR registers depending on the Inactive sensor connection
* parameter. DR register bits are set to zero when the Inactive sensor
* connection is Ground or Hi-Z.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDClearSensors(const cy_stc_capsense_context_t * context)
{
    uint32_t wdgtIndex;
    uint32_t snsIndex;
    uint32_t pinIndex;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;
    const cy_stc_capsense_electrode_config_t * ptrSnsCfg;
    
    /* Go through all CSD widgets and configure each sensor/pin to the defined state */
    for (wdgtIndex = 0u; wdgtIndex < context->ptrCommonConfig->numWd; wdgtIndex++)
    {
        ptrWdCfg = &context->ptrWdConfig[wdgtIndex];
        if ((uint8_t)CY_CAPSENSE_SENSE_METHOD_CSD_E == ptrWdCfg->senseMethod)
        {
            /* Go through all sensors in widget */
            for (snsIndex = 0u; snsIndex < ptrWdCfg->numSns; snsIndex++)
            {
                ptrSnsCfg = ptrWdCfg->ptrEltdConfig;
                /* Go through all pins in sensor */
                for(pinIndex = 0u; pinIndex < ptrSnsCfg->numPins; pinIndex++)
                {
                    /* Set CSD pin default state */
                    Cy_CapSense_CSDDisconnectSns(ptrSnsCfg->ptrPin, context);
                }
            }
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDSetWidgetSenseClkSrc
****************************************************************************//**
*
* Sets a source for the sense clock for a widget.
*
* \param ptrWdConfig
* The pointer to the widget context structure.
*
*******************************************************************************/
void Cy_CapSense_CSDSetWidgetSenseClkSrc(const cy_stc_capsense_widget_config_t * ptrWdConfig)
{
    uint32_t lfsrSize;
    uint32_t conversionsNum;
    uint32_t rowLfsrSize;
    uint8_t widgetType = ptrWdConfig->wdType;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;
    
    if (0u != (ptrWdCxt->snsClkSource & CY_CAPSENSE_CLK_SOURCE_AUTO_MASK))
    {
        conversionsNum = Cy_CapSense_CSDGetNumberOfConversions((uint32_t)ptrWdCxt->snsClk, (uint32_t)ptrWdCxt->resolution, CY_CAPSENSE_CLK_SOURCE_DIRECT);
        lfsrSize = Cy_CapSense_SsCalcLfsrSize((uint32_t)ptrWdCxt->snsClk, conversionsNum);
        if (CY_CAPSENSE_CLK_SOURCE_DIRECT == lfsrSize)
        {
            lfsrSize = Cy_CapSense_CSDCalcPrsSize((uint32_t)ptrWdCxt->snsClk << 1uL, (uint32_t)ptrWdCxt->resolution);
        }
        
        if (((uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E == widgetType) || ((uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E == widgetType))
        {
            if (ptrWdCxt->rowSnsClk != ptrWdCxt->snsClk)
            {
                conversionsNum = Cy_CapSense_CSDGetNumberOfConversions((uint32_t)ptrWdCxt->rowSnsClk, (uint32_t)ptrWdCxt->resolution, CY_CAPSENSE_CLK_SOURCE_DIRECT);
                rowLfsrSize = Cy_CapSense_SsCalcLfsrSize((uint32_t)ptrWdCxt->rowSnsClk, conversionsNum);
                if (CY_CAPSENSE_CLK_SOURCE_DIRECT == rowLfsrSize)
                {
                    rowLfsrSize = Cy_CapSense_CSDCalcPrsSize((uint32_t)ptrWdCxt->rowSnsClk << 1uL, (uint32_t)ptrWdCxt->resolution);
                }
                /* Select sense clock source based on both dimensions */
                if (rowLfsrSize != lfsrSize)
                {
                    lfsrSize = CY_CAPSENSE_CLK_SOURCE_DIRECT;
                }
            }
        }
        ptrWdCxt->snsClkSource = (uint8_t)lfsrSize | CY_CAPSENSE_CLK_SOURCE_AUTO_MASK;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDCalcPrsSize
****************************************************************************//**
*
* The function finds PRS polynomial size when clock source is set to Auto mode.
*
* The PRS polynomial size in the Auto mode is found based on the following
* requirements:
* - at least one full spread spectrum polynomial should pass during scan time.
*
* \param snsClkDivider
* The divider value for the sense clock.
*
* \param resolution
* The widget resolution.
*
* \return 
* Returns the size of PRS value for SENSE_PERIOD register.
*
*******************************************************************************/
uint32_t Cy_CapSense_CSDCalcPrsSize(uint32_t snsClkDivider, uint32_t resolution)
{
    uint32_t prsSize;

    if ((snsClkDivider * CY_CAPSENSE_PRS_LENGTH_12_BITS) <= ((0x00000001uL << resolution) - 1u))
    {
        /* Set PRS12 mode */
        prsSize = CY_CAPSENSE_CLK_SOURCE_PRS12;
    }
    else if ((snsClkDivider * CY_CAPSENSE_PRS_LENGTH_8_BITS) <= ((0x00000001uL << resolution) - 1u))
    {
        /* Set PRS8 mode */
        prsSize = CY_CAPSENSE_CLK_SOURCE_PRS8;
    }
    else
    {
        prsSize = CY_CAPSENSE_CLK_SOURCE_DIRECT;
    }

    return prsSize;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDDischargeCmod
****************************************************************************//**
*
* Discharges Cmod capacitor related to CSD sensing method and restores 
* it state.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSDDischargeCmod(cy_stc_capsense_context_t * context)
{
    en_hsiom_sel_t hsiomReg;
    uint32_t pcReg;
    uint32_t  interruptState;
    
    /* Disconnect Ext Cap from AMUXBUS-A / AMUXBUSB using HSIOM registers */
    interruptState = Cy_SysLib_EnterCriticalSection();
    hsiomReg = Cy_GPIO_GetHSIOM(context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod);
    Cy_GPIO_SetHSIOM(context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod,
                                                            CY_CAPSENSE_HSIOM_SEL_GPIO);
    Cy_SysLib_ExitCriticalSection(interruptState);

    /* Set port configuration register (drive mode) to STRONG mode */
    interruptState = Cy_SysLib_EnterCriticalSection();
    pcReg = Cy_GPIO_GetDrivemode(context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod);
    Cy_GPIO_Clr(context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod);
    Cy_GPIO_SetDrivemode(context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod, 
                                                                            CY_GPIO_DM_STRONG_IN_OFF);
    Cy_SysLib_ExitCriticalSection(interruptState);

    /* Now external CSD-related capacitors discharging */
    Cy_SysLib_DelayUs(CY_CAPSENSE_EXT_CAP_DISCHARGE_TIME);

    /* Restore Ext Cap settings */
    interruptState = Cy_SysLib_EnterCriticalSection();
    Cy_GPIO_SetDrivemode(context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod, pcReg);
    Cy_GPIO_SetHSIOM(context->ptrCommonConfig->portCmod, (uint32_t)context->ptrCommonConfig->pinCmod, hsiomReg);
    Cy_SysLib_ExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Cy_CapSense_SsCSDSwitchIdacGain
****************************************************************************//**
*
* Switches to the lower IDAC gain is possible.
*
* This internal function switches to the lower IDAC gain is possible.
* Conditions of switching to the lower IDAC gains:
* 1. The current IDAC gain is not the lowest one.
* 2. The maximum IDAC at gain switching will not be out of range.
* 3. The minimum IDAC is still below the acceptable range.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return Returns the status of the operation:
* - Zero     - Gain switching is not needed.
* - Non-Zero - Gain was switched to the lower one.
*
*******************************************************************************/
static uint32_t Cy_CapSense_CSDSwitchIdacGain(cy_stc_capsense_context_t * context)
{
    uint32_t ratio;
    uint32_t maxIdac;
    uint32_t minIdac;
    uint32_t swStatus = 0u;
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt;
    uint32_t gainIndex = ptrActive->ptrWdContext->idacGainIndex;
    uint32_t snsIndex;
    uint32_t idacVal;
    uint8_t freqChIndex;
    uint8_t freqChNumber;

    maxIdac = 0u;
    minIdac = CY_CAPSENSE_CAL_IDAC_MAX_VALUE;
        
    freqChNumber = (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn) ? 3u : 1u;
    for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
    {        
        /* Find maximum and minimum IDACs */
        idacVal = ptrActive->ptrWdContext->idacMod[freqChIndex];
        if (maxIdac < idacVal)
        {
            maxIdac = idacVal;
        }
        if (minIdac > idacVal)
        {
            minIdac = idacVal;
        }
        /* Check for sensorIndex >= numCols added and choose rowIdac if needed */
        if ((((uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E == ptrActive->ptrWdConfig->wdType) || 
             ((uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrActive->ptrWdConfig->wdType)) &&
             (ptrActive->ptrWdConfig->numCols <= ptrActive->sensorIndex))
        {
            idacVal = ptrActive->ptrWdContext->rowIdacMod[freqChIndex];
            if (maxIdac < idacVal)
            {
                maxIdac = idacVal;
            }
            if (minIdac > idacVal)
            {
                minIdac = idacVal;
            }
        }
        
        /* Set value for iDac comp */
        if (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csdIdacCompEn)
        {    
            for(snsIndex = 0u; snsIndex < ptrActive->ptrWdConfig->numSns; snsIndex++)
            {
                ptrSnsCxt = &ptrActive->ptrWdConfig->ptrSnsContext[snsIndex];
                idacVal = ptrSnsCxt[freqChIndex * context->ptrCommonConfig->numSns].idacComp;
                if (minIdac > idacVal)
                {
                    minIdac = idacVal;
                }
            }
        }
    }
        
    /* Check gain switch conditions */
    if (gainIndex != 0u)
    {
        if (minIdac < context->ptrCommonConfig->csdIdacMin)
        {
            ratio = context->ptrCommonConfig->idacGainTable[gainIndex].gainValue / 
                    context->ptrCommonConfig->idacGainTable[gainIndex - 1u].gainValue;
            if ((maxIdac * ratio) < CY_CAPSENSE_CAL_IDAC_MAX_VALUE)
            {
                /* Switch to lower idac gain */
                ptrActive->ptrWdContext->idacGainIndex--;
                swStatus = 1u;
            }
        }
    }
    return (swStatus);
}
    

/*******************************************************************************
* Function Name: Cy_CapSense_SsCSDNormalizeIdac
****************************************************************************//**
*
* This function normalizes compensation IDAC.
*
* \param ptrWdConfig 
* Specifies the pointer to a widget configuration structure.
*
* \param target
* Raw count target in percentage.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSDNormalizeIdac(
                cy_stc_capsense_widget_config_t const * ptrWdConfig, 
                uint32_t target,
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t maxIdac;
    uint32_t minIdac;
    uint32_t minRaw;
    uint32_t rawLevel;
    uint32_t iMod;

    uint8_t freqChIndex;
    uint8_t freqChNumber;
    
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;
    uint32_t maxRawLevel = ptrWdCxt->maxRawCount;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt;
    
    freqChNumber = (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn) ? 3u : 1u;
    for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
    {
        /* Find maximum and minimum IDACs */
        maxIdac = ptrWdConfig->ptrWdContext->idacMod[freqChIndex];
        minIdac = maxIdac;
        ptrSnsCxt = &ptrWdConfig->ptrSnsContext[freqChIndex * context->ptrCommonConfig->numSns];
        minRaw = ptrSnsCxt->raw;
        for(snsIndex = 0u; snsIndex < ptrWdConfig->numCols; snsIndex++)
        {
            if (minIdac > ptrSnsCxt->idacComp)
            {
                minIdac = ptrSnsCxt->idacComp;
                minRaw = ptrSnsCxt->raw;
            }
            ptrSnsCxt++;
        }
        
        /* Define new modulator IDAC */
        rawLevel = ((minRaw * CY_CAPSENSE_PERCENTAGE_100) / maxRawLevel) + CY_CAPSENSE_PERCENTAGE_100;
        iMod = (rawLevel * minIdac) / target;
        
        if (iMod > maxIdac)
        {
            iMod = maxIdac;
        }
        ptrWdCxt->idacMod[freqChIndex] = (uint8_t)iMod;
        
        /* Re-calculate compensation IDAC */
        ptrSnsCxt = &ptrWdConfig->ptrSnsContext[freqChIndex * context->ptrCommonConfig->numSns];
        for(snsIndex = 0u; snsIndex < ptrWdConfig->numCols; snsIndex++)
        {
            rawLevel = ((((uint32_t)ptrSnsCxt->raw * CY_CAPSENSE_PERCENTAGE_100) / maxRawLevel) + 
                CY_CAPSENSE_PERCENTAGE_100) * ptrSnsCxt->idacComp;
            if (rawLevel < (iMod * target))
            {
                ptrSnsCxt->idacComp = 0u;
            }
            else
            {
                ptrSnsCxt->idacComp = (uint8_t)(((rawLevel - (iMod * target)) + 
                    (CY_CAPSENSE_PERCENTAGE_100 >> 1u)) / CY_CAPSENSE_PERCENTAGE_100);
            }
            ptrSnsCxt++;
        }
        /* IDAC Normalization is performed separately for row and column  */
        if (((uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdConfig->wdType) || 
            ((uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrWdConfig->wdType))
        {
            /* Find maximum and minimum IDACs */
            maxIdac = ptrWdCxt->rowIdacMod[freqChIndex];
            minIdac = ptrWdCxt->rowIdacMod[freqChIndex];
            ptrSnsCxt = &(ptrWdConfig->ptrSnsContext[(freqChIndex * context->ptrCommonConfig->numSns) + ptrWdConfig->numCols]);
            minRaw = ptrSnsCxt->raw;
            for(snsIndex = ptrWdConfig->numCols; snsIndex < ptrWdConfig->numSns; snsIndex++)
            {
                if (minIdac > ptrSnsCxt->idacComp)
                {
                    minIdac = ptrSnsCxt ->idacComp;
                    minRaw = ptrSnsCxt->raw;
                }
                ptrSnsCxt++;
            }
            
            /* Define new modulator IDAC */
            rawLevel = ((minRaw * CY_CAPSENSE_PERCENTAGE_100) / maxRawLevel) + CY_CAPSENSE_PERCENTAGE_100;
            iMod = (rawLevel * minIdac) / target;
            
            if (iMod > maxIdac)
            {
                iMod = maxIdac;
            }
            ptrWdCxt->rowIdacMod[freqChIndex] = (uint8_t)iMod;
            
            /* Re-calculate compensation IDAC */
            ptrSnsCxt = &(ptrWdConfig->ptrSnsContext[(freqChIndex * context->ptrCommonConfig->numSns) + ptrWdConfig->numCols]);
            for(snsIndex = 0u; snsIndex < ptrWdConfig->numCols; snsIndex++)
            {
                rawLevel = ((((uint32_t)ptrSnsCxt->raw * CY_CAPSENSE_PERCENTAGE_100) / maxRawLevel) + 
                    CY_CAPSENSE_PERCENTAGE_100) * ptrSnsCxt->idacComp;
                if (rawLevel < (iMod * target))
                {
                    ptrSnsCxt->idacComp = 0u;
                }
                else
                {
                    ptrSnsCxt->idacComp = (uint8_t)(((rawLevel - (iMod * target)) + 
                        (CY_CAPSENSE_PERCENTAGE_100 >> 1u)) / CY_CAPSENSE_PERCENTAGE_100);
                }
                ptrSnsCxt++;
            }
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_SsCSDCalibrate
****************************************************************************//**
*
* Implements IDAC calibration for a desired widget using successive 
* approximation algorithm.
*
* It supports any type of CSD widgets, and works 
* with multi-frequency scan and compensation IDAC features enabled.
*
* As result of function operation, the modulator IDAC that corresponds to the 
* sensor with the highest capacitance (the biggest modulator IDAC) is stored
* into widget data structure. If it is dual-axis widget type (touchpad or matrix 
* buttons) or if multi-frequency scan feature is enabled then the maximum 
* modulator IDAC found separately for each multi-frequency channel and for 
* rows/columns.
*
* If compensation IDAC is enabled, then it preserves IDAC value of 
* single-sensor calibration. In dual IDAC mode each sensor was calibrated with 
* equal values of modulator and compensation IDAC.
*
* After IDACs were found each sensor scanned again to get real raw count stored 
* in sensor structure.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param target
* Specifies the calibration target in percentages of the maximum raw count.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSDCalibrate(
                uint32_t widgetId, 
                uint32_t target, 
                cy_stc_capsense_context_t * context)
{
    uint32_t freqChIndex;
    uint32_t freqChNumber;
    uint32_t snsIndex;
    uint32_t rawTarget;
    uint32_t tmpVal;
    uint32_t cpuFreqMHz;
    uint32_t watchdogCounter;

    uint8_t calMask;
    uint8_t * ptrIdacMod;
    uint8_t * ptrIdacMax;
    uint8_t maxColIdac[CY_CAPSENSE_FREQ_CHANNELS_NUM] = {0u, 0u, 0u};
    uint8_t maxRowIdac[CY_CAPSENSE_FREQ_CHANNELS_NUM] = {0u, 0u, 0u};

    cy_stc_capsense_widget_config_t const * ptrWdCfg = &context->ptrWdConfig[widgetId];
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdCfg->ptrWdContext;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt;

    /* Approximate duration of Wait For Init loop */
    const uint32_t isBusyLoopDuration = 5uL;

    /* Wait For Init watchdog timeout in microseconds */
    const uint32_t isBusyWatchdogTimeUs = 200000uL;

    rawTarget = ((uint32_t)ptrWdCxt->maxRawCount * target) / CY_CAPSENSE_PERCENTAGE_100;
    freqChNumber = (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn) ? 3u : 1u;
        
    for(snsIndex = 0u; snsIndex < (uint32_t)ptrWdCfg->numSns; snsIndex++)
    {
        /* Set default IDAC code */
        calMask = (uint8_t)CY_CAPSENSE_CAL_MIDDLE_VALUE;
        ptrSnsCxt = &ptrWdCfg->ptrSnsContext[snsIndex];

        
        if (ptrWdCfg->numCols > snsIndex)
        {
            ptrIdacMod = ptrWdCxt->idacMod;
            ptrIdacMax = maxColIdac;
        }
        else
        {
            ptrIdacMod = ptrWdCxt->rowIdacMod;
            ptrIdacMax = maxRowIdac;
        }

        for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
        {
            ptrIdacMod[freqChIndex] = calMask;
            if (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csdIdacCompEn)
            {
                ptrSnsCxt[freqChIndex * context->ptrCommonConfig->numSns].idacComp = calMask;
            }
        }
        
        do
        {
            /* Need to configure the CSD HW block with each IDAC change */
            Cy_CapSense_CSDSetupWidgetExt(widgetId, snsIndex, context);
            /* Need to discharge Cmod capacitor */
            Cy_CapSense_CSDDischargeCmod(context);
            /* Scan the sensor */
            Cy_CapSense_CSDScanExt(context);

            cpuFreqMHz = context->ptrCommonConfig->cpuClkHz / CY_CAPSENSE_CONVERSION_MEGA;
            watchdogCounter = Cy_CapSense_WatchdogCyclesNum(isBusyWatchdogTimeUs, cpuFreqMHz, isBusyLoopDuration);
            
            /* Wait for EOS */
            while (CY_CAPSENSE_BUSY  == (context->ptrCommonContext->status & CY_CAPSENSE_BUSY))
            {
                if(0uL == watchdogCounter)
                {
                    break;
                }
                watchdogCounter--;
            }

            /* Switch to the lower IDAC mask */
            calMask >>= 1u;
            for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
            {
                /* Update IDAC based on scan result */
                if (ptrSnsCxt[freqChIndex * context->ptrCommonConfig->numSns].raw < rawTarget)
                {
                    ptrIdacMod[freqChIndex] &= (uint8_t)(~(uint8_t)(calMask << 1u));
                }

                ptrIdacMod[freqChIndex] |= (uint8_t)calMask;

                if(0u == ptrIdacMod[freqChIndex])
                {
                    ptrIdacMod[freqChIndex] = 1u;
                }
                if (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csdIdacCompEn)
                {
                    ptrSnsCxt[freqChIndex * context->ptrCommonConfig->numSns].idacComp = ptrIdacMod[freqChIndex];
                }
            }
            
        }
        while(calMask != 0u);

        for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
        {
            /* Set the max Idac value */
            if (ptrIdacMax[freqChIndex] < ptrIdacMod[freqChIndex])
            {
                ptrIdacMax[freqChIndex] = ptrIdacMod[freqChIndex];
            }
        }
        
        /* Perform scan again to get real raw count if IDAC was changed last iteration */
        Cy_CapSense_CSDSetupWidgetExt(widgetId, snsIndex, context);
        Cy_CapSense_CSDScanExt(context);

        cpuFreqMHz = context->ptrCommonConfig->cpuClkHz / CY_CAPSENSE_CONVERSION_MEGA;
        watchdogCounter = Cy_CapSense_WatchdogCyclesNum(isBusyWatchdogTimeUs, cpuFreqMHz, isBusyLoopDuration);

        /* Wait for EOS */
        while (CY_CAPSENSE_BUSY  == (context->ptrCommonContext->status & CY_CAPSENSE_BUSY))
        {
            if(0uL == watchdogCounter)
            {
                break;
            }
            watchdogCounter--;
        }
    }

    for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
    {
        ptrWdCxt->idacMod[freqChIndex] = maxColIdac[freqChIndex];

        if (((uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType) || 
            ((uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrWdCfg->wdType))
        {
            ptrWdCxt->rowIdacMod[freqChIndex] = maxRowIdac[freqChIndex];

            if (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csdIdacRowColAlignEn)
            {
                if (((uint32_t)ptrWdCxt->idacMod[freqChIndex] * ptrWdCxt->snsClk) <
                    ((uint32_t)ptrWdCxt->rowIdacMod[freqChIndex] * ptrWdCxt->rowSnsClk))
                {
                    tmpVal = ((uint32_t)ptrWdCxt->rowIdacMod[freqChIndex] * ptrWdCxt->rowSnsClk) / ptrWdCxt->snsClk;
                    if (tmpVal > CY_CAPSENSE_CAL_IDAC_MAX_VALUE)
                    {
                        tmpVal = CY_CAPSENSE_CAL_IDAC_MAX_VALUE;
                    }
                    ptrWdCxt->idacMod[freqChIndex] = (uint8_t)tmpVal;
                }
                else
                {
                    tmpVal = ((uint32_t)ptrWdCxt->idacMod[freqChIndex] * ptrWdCxt->snsClk) / ptrWdCxt->rowSnsClk;
                    if (tmpVal > CY_CAPSENSE_CAL_IDAC_MAX_VALUE)
                    {
                        tmpVal = CY_CAPSENSE_CAL_IDAC_MAX_VALUE;
                    }
                    ptrWdCxt->rowIdacMod[freqChIndex] = (uint8_t)tmpVal;
                }
            }
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDCalibrateWidget
****************************************************************************//**
*
* Executes the IDAC calibration for all the sensors in the widget specified in
* the input.
*
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_CalibrateWidget() function should be used instead.
*
* Performs a successive approximation search algorithm to find appropriate
* modulator and compensation IDAC (if enabled) values for all sensors in 
* the specified widget that provide the raw count to the level 
* specified by the target parameter.
*
* Calibration returns CYRET_BAD_DATA if the achieved raw count is outside 
* of the range specified by the target and acceptable calibration deviation
* parameters.
*
* This function could be used when the CSD Enable IDAC auto-calibration
* parameter is enabled. Do not use this function when 
* the SmartSense auto-tuning mode is configured.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param target
* Specifies the calibration target in percentages of the maximum raw count.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified widget calibration:
* - CYRET_SUCCESS       - The operation is successfully completed.
* - CYRET_BAD_PARAM     - The input parameter is invalid.
* - CYRET_BAD_DATA      - The calibration failed and CapSense may not operate
*                         as expected.
* - CYRET_INVALID_STATE - The previous scanning is not completed, and 
*                         the CapSense middleware is busy.
*
*******************************************************************************/
cy_status Cy_CapSense_CSDCalibrateWidget(
                uint32_t widgetId, 
                uint32_t target, 
                cy_stc_capsense_context_t * context)
{
    cy_status calibrateStatus = CYRET_SUCCESS;
    uint32_t gainSwitch;

    uint32_t cpuFreqMHz;
    uint32_t watchdogCounter;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;

    /* Approximate duration of Wait For Init loop */
    const uint32_t isBusyLoopDuration = 5uL;

    /* Wait For Init watchdog timeout in microseconds */
    const uint32_t isBusyWatchdogTimeUs = 200000uL;

    if((context->ptrCommonConfig->numWd <= widgetId) ||
       (CY_CAPSENSE_DISABLE == context->ptrCommonConfig->csdIdacAutocalEn))
    {
        calibrateStatus = CY_RET_BAD_PARAM;
    }

    if((uint8_t)CY_CAPSENSE_SENSE_METHOD_CSD_E != context->ptrWdConfig[widgetId].senseMethod)
    {
        calibrateStatus = CY_RET_BAD_PARAM;
    }

    if(CY_CAPSENSE_BUSY  == (context->ptrCommonContext->status & CY_CAPSENSE_BUSY))
    {
        /* Previous widget is being scanned, return error */
        calibrateStatus = CYRET_INVALID_STATE;
    }

    if(CY_RET_SUCCESS == calibrateStatus)
    {
        ptrWdCfg = &context->ptrWdConfig[widgetId];
        ptrWdCfg->ptrWdContext->idacGainIndex = context->ptrCommonConfig->csdIdacGainInitIndex;

        /* Perform calibration */
        if (CY_CAPSENSE_ENABLE != context->ptrCommonConfig->csdIdacAutoGainEn)
        {
            Cy_CapSense_CSDCalibrate(widgetId, target, context);
        }
        else
        {
            do
            {
                Cy_CapSense_CSDCalibrate(widgetId, target, context);
                gainSwitch = Cy_CapSense_CSDSwitchIdacGain(context);
            } while(0u != gainSwitch);
        }

        if (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csdIdacCompEn)
        {
            /* IDAC Normalization in Dual IDAC mode */
            Cy_CapSense_CSDNormalizeIdac(ptrWdCfg, target, context);
        }
        
        /* Perform specified widget scan to check calibration result */
        Cy_CapSense_CSDSetupWidget_Call(widgetId, context);
        Cy_CapSense_CSDScan_Call(context);

        cpuFreqMHz = context->ptrCommonConfig->cpuClkHz / CY_CAPSENSE_CONVERSION_MEGA;
        watchdogCounter = Cy_CapSense_WatchdogCyclesNum(isBusyWatchdogTimeUs, cpuFreqMHz, isBusyLoopDuration);
        
        while (CY_CAPSENSE_BUSY  == (context->ptrCommonContext->status & CY_CAPSENSE_BUSY))
        {
            if(0uL == watchdogCounter)
            {
                break;
            }
            watchdogCounter--;
        }
        
        /* Verification of calibration result */

        calibrateStatus = Cy_CapSense_CalibrateCheck(widgetId, target, (uint32_t)CY_CAPSENSE_SENSE_METHOD_CSD_E, context);
    }
    
    return calibrateStatus;
}


    
/*******************************************************************************
* Function Name: Cy_CapSense_CSDCmodPrecharge
****************************************************************************//**
*
* Initializes the Cmod charging to Vref.
*
* This function performs coarse initialization for Cmod and Csh.
* The coarse initialization is performed by HSCOMP.
* The HSCOMP monitors the Cmod voltage via Cmod sense path
* and charges the Cmod using HCAV switch via CSDBUS-A, AMUXBUS-A
* and static connection of Cmod to AMUXBUS-A.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSDCmodPrecharge(cy_stc_capsense_context_t * context)
{
    uint32_t cpuFreqMHz;
    uint32_t watchdogCounter;

    /* Approximate duration of Wait For Init loop */
    const uint32_t intrInitLoopDuration = 5uL;

    /* Wait For Init watchdog timeout in microseconds */
    const uint32_t initWatchdogTimeUs = 200000uL;

    /* Disable INIT interrupt */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR_MASK, CY_CAPSENSE_CSD_INTR_MASK_CLEAR_MSK);

    /* Clear all interrupt pending requests */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR, CY_CAPSENSE_CSD_INTR_ALL_MSK);
    (void)Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR);

    /* Enable power to sub-blocks */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_HSCMP, context->ptrInternalContext->csdRegHscmpInit);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CSDCMP, CY_CAPSENSE_CSD_CSDCMP_CSDCMP_EN_MSK);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_MOD_SEL, CY_CAPSENSE_CSD_SW_FW_MOD_SEL_INIT);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_TANK_SEL, context->ptrInternalContext->csdRegSwFwTankSelInit);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_SHIELD_SEL, context->ptrInternalContext->csdRegSwShieldSelInit);

    /* Connect CMOD to (sense path) to HSCOMP: HMPM or HMPS or HMPT switches depend on Cmod connects to certain pad */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_P_SEL, context->ptrInternalContext->csdRegSwHsPSelInit);

    if (0u != context->ptrCommonConfig->csdShieldEn)
    {
        if (0u != context->ptrCommonConfig->csdCTankShieldEn)
        {
            Cy_CapSense_SsConfigPinRegisters(context->ptrCommonConfig->portCsh, (uint32_t)context->ptrCommonConfig->pinCsh, 
                                            CY_GPIO_DM_ANALOG, CY_CAPSENSE_HSIOM_SEL_AMUXA);
        }
    }

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_RES, context->ptrInternalContext->csdRegSwResInit);

    /* Start SEQUENCER for coarse initialization for Cmod */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START, CY_CAPSENSE_CSD_SEQ_START_SEQ_MODE_MSK | 
                                                                     CY_CAPSENSE_CSD_SEQ_START_START_MSK);

    /* Init Watchdog Counter to prevent a hang */
    cpuFreqMHz = context->ptrCommonConfig->cpuClkHz / CY_CAPSENSE_CONVERSION_MEGA;
    watchdogCounter = Cy_CapSense_WatchdogCyclesNum(initWatchdogTimeUs, cpuFreqMHz, intrInitLoopDuration);

    /* Approximate duration of Wait For Init loop */
    while((0u != (CY_CAPSENSE_CSD_SEQ_START_START_MSK &
        Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START))))
    {
        if(0uL == watchdogCounter)
        {
            break;
        }

        watchdogCounter--;
    }

    if (0u == watchdogCounter)
    {
        /* Set sequencer to idle state if coarse initialization fails */
        Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START, CY_CAPSENSE_CSD_SEQ_START_ABORT_MSK);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDTriggerScan
****************************************************************************//**
*
* Triggers the scanning.
*
* This function trigger the fine initialization (scan some dummy cycles) and 
* start sampling.
* For the fine initialization and sampling, Cmod is statically connected to 
* AMUXBUS-A and in every conversion (one cycle of SnsClk), the sensor 
* capacitance is charged from Cmod and discharged to ground using the 
* switches in GPIO cell. The CSDCOMP monitors voltage on Cmod using the 
* sense path and charges Cmod back to Vref using IDACs by connecting IDAC 
* to CSDBUS-A and then the AMUXBUS-A.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSDTriggerScan(cy_stc_capsense_context_t * context)
{
    /* Clear previous interrupts */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR, CY_CAPSENSE_CSD_INTR_ALL_MSK);
    (void)Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR);

    /* Enable SAMPLE interrupt */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR_MASK, CY_CAPSENSE_CSD_INTR_MASK_SAMPLE_MSK);


    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_P_SEL, context->ptrInternalContext->csdRegSwHsPSelScan);

    /* Set scanning configuration for switches */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_MOD_SEL, CY_CAPSENSE_CSD_SW_FW_MOD_SEL_SCAN);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_TANK_SEL, context->ptrInternalContext->csdRegSwFwTankSelScan);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_SHIELD_SEL, context->ptrInternalContext->csdRegSwShieldSelScan);

    if (0u != context->ptrCommonConfig->csdShieldEn)
    {
        if (0u != context->ptrCommonConfig->csdCTankShieldEn)
        {
            Cy_CapSense_SsConfigPinRegisters(context->ptrCommonConfig->portCsh, (uint32_t)context->ptrCommonConfig->pinCsh,
                                             CY_GPIO_DM_ANALOG, CY_CAPSENSE_HSIOM_SEL_AMUXB);
        }
    }
    
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_RES, context->ptrInternalContext->csdRegSwResScan);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CSDCMP, CY_CAPSENSE_CSD_CSDCMP_CSDCMP_EN_MSK);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_HSCMP, context->ptrInternalContext->csdRegHscmpScan);

    /* Force the LFSR to it's initial state (all ones) */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SENSE_PERIOD, 
            Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SENSE_PERIOD) | 
            CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_CLEAR_MSK | CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_BITS_MSK);

    /* Start SEQUENCER for fine initialization scan for Cmod and then for normal scan */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START, CY_CAPSENSE_CSD_SEQ_START_AZ0_SKIP_MSK | 
                                                                     CY_CAPSENSE_CSD_SEQ_START_AZ1_SKIP_MSK | 
                                                                     CY_CAPSENSE_CSD_SEQ_START_START_MSK);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDScanISR
****************************************************************************//**
*
* This is an internal ISR function to handle the CSD sensing method operation.
*
* This handler covers the following functionality:
* - Read the result of the measurement and store it into the corresponding 
*   register of the data structure.
* - If the Noise Metric functionality is enabled, then check the number of bad
*   conversions and repeat the scan of the current sensor of the number of bad
*   conversions is greater than the Noise Metric Threshold.
* - Initiate the scan of the next sensor for multiple sensor scanning mode.
* - Update the status register in the data structure.
* - Switch the CSD HW block to the default state if scanning of all the sensors is
*   completed.
*
* This is an internal ISR function for the single-sensor scanning implementation.
*
* \param capsenseContext
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSDScanISR(void * capsenseContext)
{
    uint32_t rawData;
    uint32_t maxCount;
    cy_stc_capsense_context_t * cxt = (cy_stc_capsense_context_t *)capsenseContext;
    cy_stc_active_scan_sns_t * ptrActive = cxt->ptrActiveScanSns;
    
    /* Clear pending interrupt */
    Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR, CY_CAPSENSE_CSD_INTR_ALL_MSK);
    (void)Cy_CSD_ReadReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR);

    /* Open HCBV and HCBG switches */
    Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_SHIELD_SEL, 0u);

    /* Save raw count */
    maxCount = (1uL << ptrActive->ptrWdContext->resolution) - 1uL;
    rawData = Cy_CSD_ReadReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_RESULT_VAL1) & CY_CAPSENSE_CSD_RESULT_VAL1_VALUE_MSK;
    
    if(rawData > maxCount)
    {
        rawData = maxCount;
    }
    ptrActive->ptrSnsContext->raw = (uint16_t)rawData;

    /* Either complete scan or initiate new one */
    if((CY_CAPSENSE_ENABLE == cxt->ptrCommonConfig->mfsEn) &&
       (ptrActive->mfsChannelIndex < CY_CAPSENSE_MFS_CH2_INDEX))
    {
        Cy_CapSense_CSDInitNextChScan(cxt);
    }
    else if ((CY_CAPSENSE_SCAN_SCOPE_SGL_WD == ptrActive->scanScopeAll) &&
        (CY_CAPSENSE_SCAN_SCOPE_SGL_SNS == ptrActive->scanScopeSns))
    {
        Cy_CapSense_ClrBusyFlags(cxt);
    }
    else
    {
        /* Disable sensor */
        Cy_CapSense_CSDDisconnectSnsExt(cxt);
        /* Scan the next sensor */
        Cy_CapSense_CSDInitNextSnsScan(cxt);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDInitNextSnsScan
****************************************************************************//**
*
* This function initializes the next sensor scan.
*
* The function increments the sensor index, updates sense clock for matrix
* or touchpad widgets only, sets up Compensation IDAC, enables the sensor and
* scans it. When all the sensors are scanned it continues to set up the next 
* widget until all the widgets are scanned.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSDInitNextSnsScan(cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;
    uint32_t sensorId = ptrActive->sensorIndex + 1uL;
    
    /* Check if all the sensors are scanned in widget */
    if (ptrActive->ptrWdConfig->numSns > sensorId)
    {
        /* Switch to the next sensor */
        Cy_CapSense_InitActivePtrSns(sensorId, context);

        Cy_CapSense_CSDCalculateScanDuration(context);
        Cy_CapSense_CSDConfigClock(context);
        Cy_CapSense_CSDConnectSnsExt(context);
        Cy_CapSense_CSDSetUpIdacs(context);

        Cy_CapSense_CSDStartSample(context);
    }
    else
    {
        if (CY_CAPSENSE_SCAN_SCOPE_ALL_WD == ptrActive->scanScopeAll)
        {
            /* Configure and begin scanning next widget */
            Cy_CapSense_SsPostAllWidgetsScan(context);
        }
        else
        {
            /* All the widgets are scanned */
            Cy_CapSense_ClrBusyFlags(context);
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDInitNextChScan
****************************************************************************//**
*
* This function initializes the next frequency of the sensor scan.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSDInitNextChScan(cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;

    CY_ASSERT(ptrActive->mfsChannelIndex < CY_CAPSENSE_MFS_CH2_INDEX);

    ptrActive->mfsChannelIndex++;
    ptrActive->ptrSnsContext += context->ptrCommonConfig->numSns;

    Cy_CapSense_CSDChangeClkFreq((uint32_t)ptrActive->mfsChannelIndex, context);
    Cy_CapSense_CSDSetUpIdacs(context);
    Cy_CapSense_CSDStartSample(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDChangeClkFreq
****************************************************************************//**
*
*  This function changes the sensor clock frequency by configuring
*  the corresponding divider.
*
* \param channelIndex
*  The frequency channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CapSense_CSDChangeClkFreq(uint32_t channelIndex, cy_stc_capsense_context_t * context)
{
    const cy_stc_capsense_widget_context_t * ptrWdCxt = context->ptrActiveScanSns->ptrWdContext;
    uint32_t snsClkDivider;
    uint32_t snsClkSrc = (uint32_t)ptrWdCxt->snsClkSource & ((uint32_t)~(uint32_t)CY_CAPSENSE_CLK_SOURCE_AUTO_MASK);
    uint32_t regConfig = 0u;
    uint32_t freqOffset;
    uint32_t conversionsNum;

    /* Getting row clock divider for matrix buttons or touchpad widgets */
    if (context->ptrActiveScanSns->ptrWdConfig->numCols <= context->ptrActiveScanSns->sensorIndex)
    {
        snsClkDivider = ptrWdCxt->rowSnsClk;
    }
    else
    {
        snsClkDivider = ptrWdCxt->snsClk;
    }

    /* Change the divider based on the chId */
    switch (channelIndex)
    {
        case CY_CAPSENSE_MFS_CH1_INDEX:
        {
            freqOffset = context->ptrCommonConfig->csdMfsDividerOffsetF1;
            break;
        }
        case CY_CAPSENSE_MFS_CH2_INDEX:
        {
            freqOffset = context->ptrCommonConfig->csdMfsDividerOffsetF2;
            break;
        }
        default:
        {
            freqOffset = 0u;
            break;
        }
    }
    
    if ((CY_CAPSENSE_CLK_SOURCE_PRS8 == snsClkSrc) || (CY_CAPSENSE_CLK_SOURCE_PRS12 == snsClkSrc))
    {
        freqOffset <<= 1uL;
    }

    snsClkDivider += freqOffset;

    /* Set Number Of Conversions based on scanning resolution */
    conversionsNum = Cy_CapSense_CSDGetNumberOfConversions(snsClkDivider, (uint32_t)ptrWdCxt->resolution, (uint32_t)snsClkSrc);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_NORM_CNT, (conversionsNum & CY_CAPSENSE_CSD_SEQ_NORM_CNT_CONV_CNT_MSK));

    /* Configuring PRS SEL_BIT  and decreasing divider in case of PRS */
    if ((CY_CAPSENSE_CLK_SOURCE_PRS8 == snsClkSrc) || (CY_CAPSENSE_CLK_SOURCE_PRS12 == snsClkSrc))
    {
        regConfig = CY_CAPSENSE_CSD_SENSE_PERIOD_SEL_LFSR_MSB_MSK;
        snsClkDivider >>= 1u;
    }
    
    /* Check divider value */
    if (0u == snsClkDivider)
    {
        snsClkDivider = 1u;
    }

    regConfig |= ((snsClkSrc << CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_SIZE_POS) | (snsClkDivider - 1u));
    
    /* Update reg value with divider and configuration */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SENSE_PERIOD, regConfig);
}

#endif /* CY_IP_MXCSDV2 */


/* [] END OF FILE */
