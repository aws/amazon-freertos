/***************************************************************************//**
* \file cy_capsense_csx.c
* \version 2.0
*
* \brief
* This file defines the data structure global variables and provides
* implementation for the low-level functions of the CSX part of
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
#include "cy_syslib.h"
#include "cy_sysclk.h"
#include "cy_gpio.h"
#include "cy_csd.h"

#include "cy_device_headers.h"
#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"
#include "cy_capsense_csx.h"
#include "cy_capsense_sensing.h"

#if defined(CY_IP_MXCSDV2)


/*******************************************************************************
* Local function declarations
*******************************************************************************/

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/

static void Cy_CapSense_CSXChangeClkFreq(uint32_t channelIndex, cy_stc_capsense_context_t * context);

static void Cy_CapSense_CSXStartSample(cy_stc_capsense_context_t * context);
static void Cy_CapSense_CSXInitNextChScan(cy_stc_capsense_context_t * context);
static void Cy_CapSense_CSXInitNextScan(cy_stc_capsense_context_t * context);

__STATIC_INLINE void Cy_CapSense_CSXStartSampleExt(cy_stc_capsense_context_t * context);

/** \} \endcond */

/*******************************************************************************
* Function Name: Cy_CapSense_CSXInitialize
****************************************************************************//**
*
* Performs hardware and firmware initialization required for the CSX operation
* of the CapSense middleware.
*
* This function initializes hardware to perform the CSX sensing operation. 
* If both CSX and CSD sensing methods are used in the 
* middleware, this function is called by the Cy_CapSense_SetupWidget() to 
* change hardware configured for CSD sensing method to re-initialize for the 
* CSX sensing method.
*
* If the CSD and CSX widgets are used in the middleware, do not
* mix the CSD widgets between the CSX widgets. Instead, place all 
* CSX widgets in the required scanning order and then place the CSD widgets 
* in the CapSense Configurator tool.
* For the middleware, this action will eliminate the need for changing
* the CSD HW block configuration for every widget scan and will increase the 
* execution speed in the Cy_CapSense_ScanAllWidgets() when the function is 
* called.
*
* Similarly, set up and scan all the CSX widgets in such
* a sequence that the Cy_CapSense_SetupWidget() does not need to perform 
* hardware sensing-configuration switches.
*
* Do not call this function directly from 
* the application program.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXInitialize(cy_stc_capsense_context_t * context)
{
    uint32_t interruptState;
    uint32_t tmpRegVal;
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    Cy_CapSense_DischargeExtCapacitors(context);

    interruptState = Cy_SysLib_EnterCriticalSection();
    Cy_GPIO_SetDrivemode(context->ptrCommonConfig->portCintA, (uint32_t)context->ptrCommonConfig->pinCintA, CY_GPIO_DM_ANALOG);
    Cy_GPIO_SetDrivemode(context->ptrCommonConfig->portCintB, (uint32_t)context->ptrCommonConfig->pinCintB, CY_GPIO_DM_ANALOG);
    Cy_SysLib_ExitCriticalSection(interruptState);

    /* Clear all pending interrupts of the CSD HW block */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR, CY_CAPSENSE_CSD_INTR_ALL_MSK);

    /* Enable the End Of Scan interrupt */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR_MASK, CY_CAPSENSE_DEFAULT_CSD_INTR_MASK_CFG);
    
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_HSCMP, CY_CAPSENSE_DEFAULT_CSD_HSCMP_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_AMBUF, CY_CAPSENSE_DEFAULT_CSD_AMBUF_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_REFGEN, context->ptrInternalContext->csxRegRefgen );
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CSDCMP, CY_CAPSENSE_DEFAULT_CSD_CSDCMP_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IDACA, CY_CAPSENSE_DEFAULT_CSD_IDACA_CFG);

    if (0u != context->ptrCommonConfig->csdIdacCompEn)
    {
        Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IDACB, CY_CAPSENSE_DEFAULT_CSD_IDACB_CFG);
    }

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_RES,context->ptrInternalContext->csxRegSwResInit);
    
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SENSE_DUTY, CY_CAPSENSE_DEFAULT_CSD_SENSE_DUTY_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_P_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_HS_P_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_N_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_HS_N_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_SHIELD_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_SHIELD_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_AMUXBUF_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_AMUXBUF_SEL_CFG);

    tmpRegVal = Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_BYP_SEL);
    tmpRegVal &= ~(CY_CAPSENSE_CSD_SW_BYP_SEL_SW_BYA_MSK);
    tmpRegVal |= CY_CAPSENSE_DEFAULT_CSD_SW_BYP_SEL_CFG;
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_BYP_SEL, tmpRegVal);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_CMP_P_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_CMP_P_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_CMP_N_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_CMP_N_SEL_CFG);

    tmpRegVal = Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_REFGEN_SEL);
    tmpRegVal &= ~CY_CAPSENSE_DEFAULT_CSD_SW_REFGEN_SEL_MSK;
    tmpRegVal |= context->ptrInternalContext->regSwRefGenSel ;
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_REFGEN_SEL, tmpRegVal);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_MOD_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_FW_MOD_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_TANK_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_FW_TANK_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_DSI_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_DSI_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IO_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_IO_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_TIME, 0u);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_INIT_CNT, (uint32_t)context->ptrCommonConfig->csxFineInitTime);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_NORM_CNT, CY_CAPSENSE_DEFAULT_CSD_SEQ_NORM_CNT_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_ADC_CTL, CY_CAPSENSE_DEFAULT_CSD_ADC_CTL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START, CY_CAPSENSE_DEFAULT_CSD_SEQ_START_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CONFIG, context->ptrInternalContext->csxRegConfigInit);

    Cy_CapSense_SetClkDivider((uint32_t)context->ptrCommonContext->modCsxClk - 1u, context);

    /* Set all IO states to the default state */
    Cy_CapSense_SetIOsInDefaultState(context);

    /* Enable the CSD HW block interrupt and set interrupt vector to CSX sensing method */
    if(NULL != ptrFptrCfg->fptrCSXScanISR)
    {
        context->ptrActiveScanSns->ptrISRCallback = ptrFptrCfg->fptrCSXScanISR;
    }

    context->ptrActiveScanSns->mfsChannelIndex = 0u;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXElectrodeCheck
****************************************************************************//**
*
* Check whether electrodes were previously connected using
* the Cy_CapSense_CSXSetupWidgetExt() function and if yes, disconnect them.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXElectrodeCheck(cy_stc_capsense_context_t * context)
{
    if (CY_CAPSENSE_SNS_CONNECTED == context->ptrActiveScanSns->connectedSnsState)
    {
        /* Disconnect all Tx pins */
        Cy_CapSense_CSXDisconnectTxExt(context);
        /* Disconnect all Rx pins */
        Cy_CapSense_CSXDisconnectRxExt(context);
        /* Mark the current sensor as disconnected */
        context->ptrActiveScanSns->connectedSnsState = CY_CAPSENSE_SNS_DISCONNECTED;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXDisableMode
****************************************************************************//**
*
* This function disables CSX mode.
*
* To disable CSX mode, the following tasks are performed:
* 1. Disconnect previous CSX electrode if it has been connected.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXDisableMode(cy_stc_capsense_context_t * context)
{
    /* Disconnect previous CSX electrode if it has been connected */
    Cy_CapSense_CSXElectrodeCheck(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXSetupWidget
****************************************************************************//**
*
* Performs the initialization required to scan the specified CSX widget.
*
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_SetupWidget() function should be used instead.
*
* This function prepares the middleware to scan all the sensors in the 
* specified CSX widget by executing the following tasks:
* 1. Configure the CSD HW block if it is not configured to perform the
* CSX sensing method used by the specified widget.
* 2. Initialize the CSD HW block with specific sensing configuration (e.g.
* sensor clock, scan resolution) used by the widget.
* 3. Disconnect all previously connected electrodes, if the electrodes
* connected by the Cy_CapSense_CSDSetupWidgetExt(),
* Cy_CapSense_CSXSetupWidgetExt() functions are not disconnected.
*
* This function does not start sensor scanning. The Cy_CapSense_CSXScan()
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
void Cy_CapSense_CSXSetupWidget(uint32_t widgetId, cy_stc_capsense_context_t * context)
{
    /* variable to access widget details */
    uint32_t snsClkDivider;
    uint32_t snsClkSrc;
    uint32_t tmpRegVal;

    Cy_CapSense_SwitchSensingMode((uint8_t)CY_CAPSENSE_SENSE_METHOD_CSX_E, context);
    /*
    * Check whether CSX electrodes were previously connected using
    * Cy_CapSense_CSXSetupWidgetExt and if yes, disconnect them
    */
    Cy_CapSense_CSXElectrodeCheck(context);
    /* Set up widget and its first sensor IDs and pointers to have assess to them after scanning  */
    Cy_CapSense_InitActivePtr(widgetId, 0u, context);
    /* Number of conversion and maxRawCount setup */
    tmpRegVal = (uint32_t)context->ptrActiveScanSns->ptrWdContext->resolution;    
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_NORM_CNT, tmpRegVal);
    context->ptrActiveScanSns->ptrWdContext->maxRawCount = (uint16_t)tmpRegVal * 
        (context->ptrActiveScanSns->ptrWdContext->snsClk - CY_CAPSENSE_CSX_DEADBAND_CYCLES_NUMBER);

    /* SnsClk setup */
    snsClkDivider = (uint32_t) context->ptrActiveScanSns->ptrWdContext->snsClk;
    
    /* Check divider value */
    if (0u == snsClkDivider)
    {
        snsClkDivider = 1u;
    }
    snsClkSrc = (uint32_t)context->ptrActiveScanSns->ptrWdContext->snsClkSource & (uint32_t)~((uint32_t)CY_CAPSENSE_CLK_SOURCE_AUTO_MASK);
    tmpRegVal = ((snsClkSrc << CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_SIZE_POS) | (snsClkDivider - 1u) |
            CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_CLEAR_MSK | CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_BITS_MSK);
    /* Update reg value with divider and configuration */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SENSE_PERIOD, tmpRegVal);
}

/*******************************************************************************
* Function Name: Cy_CapSense_CSXSetupWidgetExt
****************************************************************************//**
*
* Performs extended initialization required to scan a specified sensor in 
* a widget using CSX sensing method.
* 
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_SetupWidgetExt() function should be used instead.
*
* This function performs the same tasks of Cy_CapSense_CSXSetupWidget() and 
* also connects and configures specified sensor for scan. Hence this 
* function, along with Cy_CapSense_CSXScanExt() function, can be used to 
* scan a specific sensor in the widget.
* 
* This function should be called for a widget that is configured to use 
* CSX sensing method. Using this function on a non-CSX sensing widget 
* would cause an unexpected result.
* 
* This function requires using the Cy_CapSense_CSXScanExt() function to 
* initiate a scan.
* 
* Calling this function directly from the application program is not 
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
void Cy_CapSense_CSXSetupWidgetExt(
                uint32_t widgetId, 
                uint32_t sensorId, 
                cy_stc_capsense_context_t * context)
{
    /* Initialize widget */
    Cy_CapSense_CSXSetupWidget_Call(widgetId, context);

    /* Initialize sensor data structure pointer to appropriate address */
    Cy_CapSense_InitActivePtrSns(sensorId, context);

    /* Connect current sensor`s Tx and Rx IOs for scan 
    * and set flag to indicate that IOs should be disconnected */
    Cy_CapSense_CSXConnectTxExt(context);
    Cy_CapSense_CSXConnectRxExt(context);
}

/*******************************************************************************
* Function Name: Cy_CapSense_CSXScan
****************************************************************************//**
*
* This function initiates a scan for the sensors of the widget initialized 
* by the Cy_CapSense_CSXSetupWidget() function.
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
* widget uses the CSX sensing method.
*
* Calling this function directly from the application program is not
* recommended. This function is used to implement only the user's specific
* use cases (for example faster execution).
*
* This function is called when no scanning is in progress. I.e.
* Cy_CapSense_IsBusy() returns a non-busy status and the widget must be
* preconfigured using the Cy_CapSense_CSXSetupWidget() function prior
* to calling this function.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXScan(cy_stc_capsense_context_t * context)
{
    Cy_CapSense_InitActivePtrSns(0u, context);
    
    /* Connect electrodes */
    Cy_CapSense_CSXConnectTxExt(context);
    Cy_CapSense_CSXConnectRxExt(context);

    /* Set Start of scan flag */
    Cy_CapSense_SetBusyFlags(context);
    /* Set scope flag */
    context->ptrActiveScanSns->scanScopeSns = CY_CAPSENSE_SCAN_SCOPE_ALL_SNS;

    Cy_CapSense_CSXStartSample(context);
}

/*******************************************************************************
* Function Name: Cy_CapSense_CSXScanExt()
****************************************************************************//**
*
* Starts the CSD conversion on the preconfigured sensor. 
* 
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_ScanExt() function should be used instead.
*
* This function performs scanning of a specific sensor in the widget 
* previously initialized using the Cy_CapSense_CSXSetupWidgetExt() function. 
* 
* This function is called when no scanning is in progress. 
* I.e. Cy_CapSense_IsBusy() returns a non-busy status and the widget must 
* be preconfigured using Cy_CapSense_CSXSetupWidgetExt() function prior 
* to calling this function. Calling this function directly from 
* the application program is not recommended. This function is used to 
* implement only the user's specific use cases (for example faster execution).
* 
* This function does not disconnect sensor GPIOs from CSD HW block at the 
* end of a scan, therefore making subsequent scan of the same sensor is faster. 
* If sensor must be disconnected after the scan, 
* the Cy_CapSense_CSXDisconnectTx() or Cy_CapSense_CSXDisconnectRx() functions
* can be used.
* 
* Calling Cy_CapSense_SetupWidget(), Cy_CapSense_CSXSetupWidget(), 
* Cy_CapSense_ScanAllWidgets(), or if Cy_CapSense_RunTuner() returns 
* CY_CAPSENSE_STATUS_RESTART_DONE status invalidated initialization 
* made by this function.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXScanExt(cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;

    /* Set MFS channel index to 0 */
    ptrActive->mfsChannelIndex = 0u;

    /* Initialize the Active Context pointer with the CH0 context */
    ptrActive->ptrSnsContext = &ptrActive->ptrWdConfig->ptrSnsContext[ptrActive->sensorIndex];
    
    /* Set busy flag and start conversion */
    Cy_CapSense_SetBusyFlags(context);
    /* Set scope flag */
    context->ptrActiveScanSns->scanScopeSns = CY_CAPSENSE_SCAN_SCOPE_SGL_SNS;

    Cy_CapSense_CSXStartSample(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXCalibrateWidget
****************************************************************************//**
*
* Executes the IDAC calibration for all the sensors in the widget specified in
* the input.
*
* \note This function is obsolete and kept for backward compatibility only. 
* The Cy_CapSense_CalibrateWidget() function should be used instead.
*
* Performs a successive approximation search algorithm to find appropriate
* IDAC values for all sensors in the specified widget that provide 
* the raw count to the level specified by the target parameter.
*
* Calibration returns CYRET_BAD_DATA if the achieved raw count is outside 
* of the range specified by the target and acceptable calibration deviation
* parameters.
*
* This function could be used when the CSX Enable IDAC auto-calibration
* parameter is enabled. 
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
cy_status Cy_CapSense_CSXCalibrateWidget(
                uint32_t widgetId, 
                uint32_t target, 
                cy_stc_capsense_context_t * context)
{
    uint32_t cpuFreqMHz;
    uint32_t watchdogCounter;

    uint32_t freqChIndex;
    uint32_t freqChNumber;

    cy_status calibrateStatus = CY_RET_SUCCESS;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;
    cy_stc_capsense_sensor_context_t * ptrActSnsContext;

    uint32_t rawTarget;
    uint32_t totalSns;
    uint32_t calibrationIndex;
    /* Currently used IDAC-bit */
    uint8_t curIdacMask = CY_CAPSENSE_CAL_MIDDLE_VALUE;
    /* Next used IDAC-bit */
    uint8_t nextIdacMask = (curIdacMask >> 1u);

    /* Approximate duration of Wait For Init loop */
    const uint32_t isBusyLoopDuration = 5uL;

    /* Wait For Init watchdog timeout in microseconds */
    const uint32_t isBusyWatchdogTimeUs = 200000uL;


    if((context->ptrCommonConfig->numWd <= widgetId) ||
        (CY_CAPSENSE_DISABLE == context->ptrCommonConfig->csxIdacAutocalEn)) 
    {
        calibrateStatus = CY_RET_BAD_PARAM;
    }

    if((uint8_t)CY_CAPSENSE_SENSE_METHOD_CSX_E != context->ptrWdConfig[widgetId].senseMethod)
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
        ptrWdCfg->ptrWdContext->idacGainIndex = context->ptrCommonConfig->csxIdacGainInitIndex;

        ptrActSnsContext = ptrWdCfg->ptrSnsContext;
        totalSns = ptrWdCfg->numSns;
    
        /* Calculate target raw count */
        rawTarget = ((uint32_t)context->ptrWdContext[widgetId].maxRawCount * target) / CY_CAPSENSE_PERCENTAGE_100;
        freqChNumber = (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn) ? 3u : 1u;

        for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
        {
            /* Clear raw count registers and IDAC registers of all the sensors/nodes */
            for (calibrationIndex = 0u; calibrationIndex < totalSns; calibrationIndex++)
            {
                ptrActSnsContext[calibrationIndex + (freqChIndex * context->ptrCommonConfig->numSns)].raw = 0u;
                ptrActSnsContext[calibrationIndex + (freqChIndex * context->ptrCommonConfig->numSns)].idacComp = curIdacMask;
            }
        }
        /* Perform binary search for accurate IDAC value for each sensor/node */
        do
        {
            /* Scan all the sensors/nodes in widget */
            (void)Cy_CapSense_SetupWidget(widgetId, context);
            (void)Cy_CapSense_Scan(context);

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
            
            /* Set pointer to the widget's first sensor data */
            ptrActSnsContext = ptrWdCfg->ptrSnsContext;

            for (freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
            {
                /* Check raw count and adjust IDAC, loop through all the sensors/nodes */
                for (calibrationIndex = 0u; calibrationIndex < totalSns; calibrationIndex++)
                {
                    /* Check whether the current raw count is above target.
                    * If yes, clear the MS bit of bit.
                    * If no, keep the MS bit and set next bit.
                    */
                    if (ptrActSnsContext[calibrationIndex + (freqChIndex * context->ptrCommonConfig->numSns)].raw > rawTarget)
                    {
                        ptrActSnsContext[calibrationIndex + (freqChIndex * context->ptrCommonConfig->numSns)].idacComp &= (uint8_t)(~curIdacMask);
                    }
                    ptrActSnsContext[calibrationIndex + (freqChIndex * context->ptrCommonConfig->numSns)].idacComp |= nextIdacMask;
                }
            }
            /* Shift both current IDAC and pre IDAC values to the right by 1 */
            curIdacMask = nextIdacMask;
            nextIdacMask = nextIdacMask >> 1u;
        }
        while (curIdacMask != 0u);
        calibrateStatus = CY_RET_SUCCESS;
    }

    if(CY_RET_SUCCESS == calibrateStatus)
    {
        /* Perform specified widget scan to check calibration result */
        /* Scan all the sensors/nodes in widget */
        (void)Cy_CapSense_SetupWidget(widgetId, context);
        (void)Cy_CapSense_Scan(context);

        cpuFreqMHz = context->ptrCommonConfig->cpuClkHz / CY_CAPSENSE_CONVERSION_MEGA;
        watchdogCounter = Cy_CapSense_WatchdogCyclesNum(isBusyWatchdogTimeUs, cpuFreqMHz, isBusyLoopDuration);
                    
        /* Wait for EOS */
        while (CY_CAPSENSE_BUSY  == (context->ptrCommonContext->status & CY_CAPSENSE_BUSY))
        {
            if(0uL == watchdogCounter)
            {
                calibrateStatus = CY_RET_TIMEOUT;
                break;
            }
            
            watchdogCounter--;
        }

        calibrateStatus = Cy_CapSense_CalibrateCheck(widgetId, target, (uint32_t)CY_CAPSENSE_SENSE_METHOD_CSX_E, context);
    }

    return(calibrateStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXSetUpIdacs
****************************************************************************//**
*
* Configures IDAC for the CSX sensing method.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXSetUpIdacs(cy_stc_capsense_context_t * context)
{
    uint32_t tmpRegVal;

    /* Set IDAC gain */
    tmpRegVal = context->ptrCommonConfig->idacGainTable[context->ptrActiveScanSns->ptrWdContext->idacGainIndex].gainReg;
    /* Set IDAC code */
    tmpRegVal |= (uint32_t)context->ptrActiveScanSns->ptrSnsContext->idacComp & CY_CAPSENSE_CSD_IDACA_VAL_MSK;
    tmpRegVal |= CY_CAPSENSE_DEFAULT_CSD_IDACA_CFG;

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IDACA, tmpRegVal);
}

/*******************************************************************************
* Function Name: Cy_CapSense_SsCSXStartSample
****************************************************************************//**
*
* Starts scanning for the CSX widget.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSXStartSample(cy_stc_capsense_context_t * context)
{
    /* Set up IDAC Value */
    Cy_CapSense_CSXSetUpIdacs(context);
        
    /* Clear previous interrupts */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR, CY_CAPSENSE_CSD_INTR_ALL_MSK);
    (void)Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR);

    /* Enable interrupt */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR_MASK, CY_CAPSENSE_CSD_INTR_MASK_SAMPLE_MSK);
    
    if(NULL != context->ptrCommonContext->ptrSSCallback)
    {
        context->ptrCommonContext->ptrSSCallback(context->ptrActiveScanSns);
    }

    Cy_CapSense_CSXStartSampleExt(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXConnectRx
****************************************************************************//**
*
* Connects port pin (an Rx electrode) to the CSD HW block using AMUX bus.
* 
* This function can be used to customize the default sensor connection 
* by connecting one or more pins to an existing sensor as an Rx electrode
* prior to initiating scan of the sensor.
* 
* The function ignores whether the sensor is a ganged sensor and 
* connects only a specified port pin to the CSD HW block. This function can 
* only use GPIOs that is already assigned to CapSense middleware.
* 
* The functions that perform a setup and scan of a sensor/widget do not 
* take into account changes in the design made by 
* the Cy_CapSense_CSXConnectRx() function. Hence all GPIOs connected 
* using this function must be disconnected using 
* the Cy_CapSense_CSXDisconnectRx() function prior to initializing 
* new widgets. Use this function in StartSample 
* callback (see the \ref group_capsense_callbacks section for details) 
* or with low-level functions that perform a single-sensor scanning.
* 
* Scanning should be completed before calling this function.
*
* \param rxPtr
* Specifies the pointer to the cy_stc_capsense_pin_config_t object belonging to
* a sensor which to be connected to the CSD HW block as an Rx electrode.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \funcusage
* 
* An example of using the function to perform port pin re-connection: 
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_CSXConnect
* 
*******************************************************************************/
void Cy_CapSense_CSXConnectRx(
                const cy_stc_capsense_pin_config_t * rxPtr, 
                cy_stc_capsense_context_t * context)
{
    Cy_CapSense_SsConfigPinRegisters(rxPtr->pcPtr, (uint32_t)rxPtr->pinNumber, CY_GPIO_DM_ANALOG, CY_CAPSENSE_HSIOM_SEL_AMUXA);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXConnectTx
****************************************************************************//**
*
* Connects port pin (a Tx electrode) to the CSD HW block.
* 
* This function can be used to customize the default sensor connection 
* by connecting one or more pins to an existing sensor as a Tx electrode
* prior to initiating scan of the sensor.
* 
* The function ignores whether the sensor is a ganged sensor and 
* connects only a specified port pin to the CSD HW block. This function can 
* only use GPIOs that is already assigned to CapSense middleware.
* 
* The functions that perform a setup and scan of a sensor/widget do not 
* take into account changes in the design made by 
* the Cy_CapSense_CSXConnectTx() function. Hence all GPIOs connected 
* using this function must be disconnected using 
* the Cy_CapSense_CSXDisconnectTx() function prior to initializing 
* new widgets. Use this function in StartSample 
* callback (see the \ref group_capsense_callbacks section for details) 
* or with low-level functions that perform a single-sensor scanning.
* 
* Scanning should be completed before calling this function.
*
* \param txPtr
* Specifies the pointer to the cy_stc_capsense_pin_config_t object belonging to
* a sensor which to be connected to the CSD HW block as a Tx electrode.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \funcusage
* 
* An example of using the function to perform port pin re-connection: 
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_CSXConnect
* 
*******************************************************************************/
void Cy_CapSense_CSXConnectTx(
                const cy_stc_capsense_pin_config_t * txPtr, 
                cy_stc_capsense_context_t * context)
{
    Cy_CapSense_SsConfigPinRegisters(txPtr->pcPtr, (uint32_t)txPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF, CY_CAPSENSE_HSIOM_SEL_CSD_SHIELD);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXDisconnectRx
****************************************************************************//**
* 
* Disconnects port pin (an Rx electrode) from the CSD HW block by disconnecting 
* it from the AMUX bus.
* 
* This function can be used to disconnect a sensor connected 
* using the Cy_CapSense_CSXConnectRx() function. In addition, this 
* function can be used to customize default sensor connection by 
* disconnecting one or more already connected sensors prior to 
* initiating scan of the sensor.
* 
* This function works identically to the Cy_CapSense_CSDConnectRx() function 
* except it disconnects the specified port pin used by the sensor.
*
* Scanning should be completed before calling this function.
*
* \param rxPtr
* Specifies the pointer to the cy_stc_capsense_pin_config_t object belonging
* to an Rx pin sensor to be disconnected from the CSD HW block.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \funcusage
* 
* An example of using the function to perform port pin re-connection: 
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_CSXConnect
* 
*******************************************************************************/
void Cy_CapSense_CSXDisconnectRx(
                const cy_stc_capsense_pin_config_t * rxPtr, 
                cy_stc_capsense_context_t * context)
{
    uint32_t interruptState;
    
    Cy_CapSense_SsConfigPinRegisters(rxPtr->pcPtr, (uint32_t)rxPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF, CY_CAPSENSE_HSIOM_SEL_GPIO);

    interruptState = Cy_SysLib_EnterCriticalSection();
    Cy_GPIO_Clr(rxPtr->pcPtr, (uint32_t)rxPtr->pinNumber);
    Cy_SysLib_ExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXDisconnectTx
****************************************************************************//**
* 
* Disconnects port pin (a Tx electrode) from the CSD HW block.
* 
* This function can be used to disconnect a sensor connected 
* using the Cy_CapSense_CSXConnectTx() function. In addition, this 
* function can be used to customize default sensor connection by 
* disconnecting one or more already connected sensors prior to 
* initiating scan of the sensor.
* 
* This function works identically to the Cy_CapSense_CSDConnectTx() function 
* except it disconnects the specified port pin used by the sensor.
*
* Scanning should be completed before calling this function.
*
* \param txPtr
* Specifies the pointer to the cy_stc_capsense_pin_config_t object belonging
* to a Tx pin sensor to be disconnected from the CSD HW block.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \funcusage
* 
* An example of using the function to perform port pin re-connection: 
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_CSXConnect
* 
*******************************************************************************/
void Cy_CapSense_CSXDisconnectTx(
                const cy_stc_capsense_pin_config_t * txPtr, 
                cy_stc_capsense_context_t * context)
{
    Cy_CapSense_SsConfigPinRegisters(txPtr->pcPtr, (uint32_t)txPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF, CY_CAPSENSE_HSIOM_SEL_GPIO);
    Cy_GPIO_Clr(txPtr->pcPtr, (uint32_t)txPtr->pinNumber);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXConnectTxExt
****************************************************************************//**
*
* Connects a current Tx electrode to the CSX scanning hardware.
*
* This function connects all current Tx electrode's pins to the CSD_SENSE signal.
* It is assumed that drive mode of the port pin is already set to STRONG
* in the HSIOM_PORT_SELx register.
*
* Calling this function directly from the application program is not
* recommended. This function is used to implement only the user's specific
* use cases (for faster execution time when there is only one port pin for an
* electrode for example).
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXConnectTxExt(cy_stc_capsense_context_t * context)
{
    uint32_t pinIndex;
    const cy_stc_capsense_pin_config_t * pinPointer = context->ptrActiveScanSns->ptrTxConfig->ptrPin;
    
    for (pinIndex = context->ptrActiveScanSns->ptrTxConfig->numPins; pinIndex-- > 0u;)
    {
        Cy_CapSense_CSXConnectTx(pinPointer, context);
        pinPointer++;
    }
    context->ptrActiveScanSns->connectedSnsState = CY_CAPSENSE_SNS_CONNECTED;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXConnectRxExt
****************************************************************************//**
*
* Connects a current Rx electrode to the CSX scanning hardware.
*
* This function connects all current Rx electrode's pins to the CSD_SENSE signal.
* It is assumed that drive mode of the port pin is already set to STRONG
* in the HSIOM_PORT_SELx register.
*
* Calling this function directly from the application program is not
* recommended. This function is used to implement only the user's specific
* use cases (for faster execution time when there is only one port pin for an
* electrode for example).
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXConnectRxExt(cy_stc_capsense_context_t * context)
{
    uint32_t pinIndex;
    const cy_stc_capsense_pin_config_t * pinPointer = context->ptrActiveScanSns->ptrRxConfig->ptrPin;
    
    for (pinIndex = context->ptrActiveScanSns->ptrRxConfig->numPins; pinIndex-- > 0u;)
    {
        Cy_CapSense_CSXConnectRx(pinPointer, context);
        pinPointer++;
    }
    context->ptrActiveScanSns->connectedSnsState = CY_CAPSENSE_SNS_CONNECTED;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXDisconnectTxExt
****************************************************************************//**
*
* Disconnects a current Tx electrode from the CSX scanning hardware.
*
* This function connects all current Tx electrode's pins to the CSD_SENSE signal.
* It is assumed that drive mode of the port pin is already set to STRONG
* in the HSIOM_PORT_SELx register.
*
* Calling this function directly from the application program is not
* recommended. This function is used to implement only the user's specific
* use cases (for faster execution time when there is only one port pin for an
* electrode for example).
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXDisconnectTxExt(cy_stc_capsense_context_t * context)
{
    uint32_t pinIndex;
    const cy_stc_capsense_pin_config_t * pinPointer = context->ptrActiveScanSns->ptrTxConfig->ptrPin;
    
    for (pinIndex = context->ptrActiveScanSns->ptrTxConfig->numPins; pinIndex-- > 0u;)
    {
        Cy_CapSense_CSXDisconnectTx(pinPointer, context);
        pinPointer++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXDisconnectRxExt
****************************************************************************//**
*
* Connects a current Tx electrode to the CSX scanning hardware.
*
* This function connects all current Tx electrode's pins to the CSD_SENSE signal.
* It is assumed that drive mode of the port pin is already set to STRONG
* in the HSIOM_PORT_SELx register.
*
* Calling this function directly from the application program is not
* recommended. This function is used to implement only the user's specific
* use cases (for faster execution time when there is only one port pin for an
* electrode for example).
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXDisconnectRxExt(cy_stc_capsense_context_t * context)
{
    uint32_t pinIndex;
    const cy_stc_capsense_pin_config_t * pinPointer = context->ptrActiveScanSns->ptrRxConfig->ptrPin;
    
    for (pinIndex = context->ptrActiveScanSns->ptrRxConfig->numPins; pinIndex-- > 0u;)
    {
        Cy_CapSense_CSXDisconnectRx(pinPointer, context);
        pinPointer++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXSetWidgetTxClkSrc
****************************************************************************//**
*
* Sets a source for the Tx clock for a widget.
*
* \param ptrWdConfig
* The pointer to the cy_stc_capsense_widget_context_t structure.
*
*******************************************************************************/
void Cy_CapSense_CSXSetWidgetTxClkSrc(const cy_stc_capsense_widget_config_t * ptrWdConfig)
{
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    if (0u != (ptrWdCxt->snsClkSource & CY_CAPSENSE_CLK_SOURCE_AUTO_MASK))
    {
        ptrWdCxt->snsClkSource = (uint8_t)Cy_CapSense_SsCalcLfsrSize((uint32_t)ptrWdCxt->snsClk, (uint32_t)ptrWdCxt->resolution) | CY_CAPSENSE_CLK_SOURCE_AUTO_MASK;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXScanISR
****************************************************************************//**
*
* This is an internal ISR function to handle the CSX sensing method operation.
*
* This handler covers the following functionality:
* - Read the result of the measurement and store it into the corresponding register of
* the data structure.
* - If the Noise Metric functionality is enabled, then check the number of bad
* conversions and repeat the scan of the current sensor if the number of bad
* conversions is greater than the Noise Metric Threshold.
* - Initiate the scan of the next sensor for multiple sensor scanning mode.
* - Update the Status register in the data structure.
* - Switch the HW IP block to the default state if scanning of all the sensors is
* completed.
*
* \param capsenseContext
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CSXScanISR(void * capsenseContext)
{
    uint32_t tmpRawCount;
    cy_stc_capsense_context_t * cxt = (cy_stc_capsense_context_t *)capsenseContext;
    cy_stc_active_scan_sns_t * ptrActive = cxt->ptrActiveScanSns;    
    uint32_t maxCount = (uint32_t) ptrActive->ptrWdContext->maxRawCount;

    Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR_MASK, CY_CAPSENSE_CSD_INTR_MASK_CLEAR_MSK);
        
    /* Clear all pending interrupts of the CSD HW block */
    Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR, CY_CAPSENSE_CSD_INTR_ALL_MSK);
    (void)Cy_CSD_ReadReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR);
    

    tmpRawCount  = (uint16_t)(Cy_CSD_ReadReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_RESULT_VAL1) &
                                             CY_CAPSENSE_CSD_RESULT_VAL1_VALUE_MSK);

    tmpRawCount += (uint16_t)(Cy_CSD_ReadReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_RESULT_VAL2) &
                                             CY_CAPSENSE_CSD_RESULT_VAL2_VALUE_MSK);

    /* This workaround needed to prevent overflow in the SW register map. Cypress ID #234358 */
    if(tmpRawCount > maxCount)
    {
        tmpRawCount = maxCount;
    }
    ptrActive->ptrSnsContext->raw = (uint16_t) (maxCount - tmpRawCount);

    /* Either complete scan or initiate new one */
    if((CY_CAPSENSE_ENABLE == cxt->ptrCommonConfig->mfsEn) &&
       (ptrActive->mfsChannelIndex < CY_CAPSENSE_MFS_CH2_INDEX))

    {
        Cy_CapSense_CSXInitNextChScan(cxt);
    }
    else if ((CY_CAPSENSE_SCAN_SCOPE_SGL_WD == ptrActive->scanScopeAll) &&
        (CY_CAPSENSE_SCAN_SCOPE_SGL_SNS == ptrActive->scanScopeSns))
    {
        Cy_CapSense_ClrBusyFlags(cxt);
    }
    else
    { 
        /* Disconnect Tx electrodes of previously scanned sensor as preparation for next sensor scan */
        Cy_CapSense_CSXDisconnectTxExt(cxt);
        /* Scan the next sensor */
        Cy_CapSense_CSXInitNextScan(cxt);
    }
    
    if(CY_CAPSENSE_NOT_BUSY == (cxt->ptrCommonContext->status & CY_CAPSENSE_BUSY))
    {
        Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CONFIG, cxt->ptrInternalContext->csxRegConfigInit);
        Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CSDCMP, CY_CAPSENSE_DEFAULT_CSD_CSDCMP_CFG);
        Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IDACA, CY_CAPSENSE_DEFAULT_CSD_IDACA_CFG);
        Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IO_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_IO_SEL_CFG);
    }
    
    /* Clear previous interrupts */
    Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR, CY_CAPSENSE_CSD_INTR_ALL_MSK);
    (void)Cy_CSD_ReadReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR);

    /* Enable interrupt */
    Cy_CSD_WriteReg(cxt->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR_MASK, CY_CAPSENSE_CSD_INTR_MASK_SAMPLE_MSK);

}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXInitNextScan
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
static void Cy_CapSense_CSXInitNextScan(cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;
    
    ptrActive->mfsChannelIndex = 0u;
    ptrActive->ptrSnsContext = &ptrActive->ptrWdConfig->ptrSnsContext[ptrActive->sensorIndex];
    
    /* Increment sensor index and sensor's context pointer to scope next sensor in widget */
    ptrActive->sensorIndex++;
    ptrActive->ptrSnsContext++;
    /* Initiate scan for next sensor in widget, if all the sensors in widget are not scanned */
    ptrActive->txIndex--;
    
        if (ptrActive->txIndex > 0u)
        {
            /* Increment pointer to the next Tx electrode and connect it for scan  */
            ptrActive->ptrTxConfig++;
            Cy_CapSense_CSXConnectTxExt(context);
            /* Initiate a next scan */
            Cy_CapSense_CSXSetUpIdacs(context);
            Cy_CapSense_CSXStartSample(context);
        }
        /* Check whether all Rx are looped through */
        else
        {
            ptrActive->rxIndex--;
            if (ptrActive->rxIndex > 0u)
            {
                /* Disconnect the current Rx electrode and connect a next Rx electrode */
                Cy_CapSense_CSXDisconnectRxExt(context);
                ptrActive->ptrRxConfig++;
                Cy_CapSense_CSXConnectRxExt(context);
                /* Re-initialize Tx index to be scanned per Rx and connect the first Tx electrode again */
                ptrActive->txIndex = ptrActive->ptrWdConfig->numRows;
                ptrActive->ptrTxConfig = &ptrActive->ptrWdConfig->ptrEltdConfig[ptrActive->ptrWdConfig->numCols];
                Cy_CapSense_CSXConnectTxExt(context);
                /* Initiate a next scan */
                Cy_CapSense_CSXSetUpIdacs(context);
                Cy_CapSense_CSXStartSample(context);
            }
            else
            {
                /* If all the sensors are scanned, disconnect the last Rx electrode */
                Cy_CapSense_CSXDisconnectRxExt(context);
                ptrActive->connectedSnsState = CY_CAPSENSE_SNS_DISCONNECTED;
                /* Call scan of next widget if requested. If not, complete scan */
                if (CY_CAPSENSE_SCAN_SCOPE_ALL_WD == ptrActive->scanScopeAll)
                {
                    Cy_CapSense_SsPostAllWidgetsScan(context);
                }
                else
                {
                    /* All pending scans completed, clear busy state as scanning is completed  */
                    Cy_CapSense_ClrBusyFlags(context);
            }
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_SsCSXStartSampleExt
****************************************************************************//**
*
* Starts the HW sequencer to perform the CSX conversion.
*
* This function covers the following functionality:
* 1. Configures the HW sequencer to perform the coarse initialization.
* 2. Waiting for completion of the coarse initialization.
* 3. Configures the HW sequencer to perform the normal conversion.
* 4. Starts the normal conversion
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CapSense_CSXStartSampleExt(cy_stc_capsense_context_t * context)
{
    uint32_t cpuFreqMHz;
    uint32_t watchdogCounter;

    /* Approximate duration of Wait For Init loop */
    const uint32_t intrInitLoopDuration = 5uL;

    /* Wait For Init watchdog timeout in microseconds */
    const uint32_t initWatchdogTimeUs = 200000uL;


    /* Configure the HW sequencer to perform the coarse initialization. */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CONFIG, context->ptrInternalContext->csxRegConfigInit);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_HSCMP, CY_CAPSENSE_PRECHARGE_CSD_HSCMP_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_P_SEL, CY_CAPSENSE_PRECHARGE_CSD_SW_HS_P_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_N_SEL, CY_CAPSENSE_PRECHARGE_CSD_SW_HS_N_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_DSI_SEL, CY_CAPSENSE_PRECHARGE_CSD_SW_DSI_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_SHIELD_SEL, CY_CAPSENSE_PRECHARGE_CSD_SW_SHIELD_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_MOD_SEL, CY_CAPSENSE_PRECHARGE_CSD_SW_FW_MOD_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_TANK_SEL, CY_CAPSENSE_PRECHARGE_CSD_SW_FW_TANK_SEL_CFG);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_RES, context->ptrInternalContext->csxRegSwResPrech);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START, CY_CAPSENSE_PRECHARGE_CSD_SEQ_START_CFG);

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


    /* Reset the sequencer to the IDLE state if HSCMP not triggered till watchdog period is out. */
    if(0u != (CY_CAPSENSE_CSD_SEQ_START_START_MSK & Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START)))
    {
        Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START, CY_CAPSENSE_DEFAULT_CSD_SEQ_START_CFG);
    }

    /* Configure the HW sequencer to perform the normal conversion. */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CONFIG, context->ptrInternalContext->csxRegConfigScan);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_CSDCMP, CY_CAPSENSE_PRESCAN_CSD_CSDCMP_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_HSCMP, CY_CAPSENSE_DEFAULT_CSD_HSCMP_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_P_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_HS_P_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_HS_N_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_HS_N_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_DSI_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_DSI_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_SHIELD_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_SHIELD_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_MOD_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_FW_MOD_SEL_CFG);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_FW_TANK_SEL, CY_CAPSENSE_DEFAULT_CSD_SW_FW_TANK_SEL_CFG);

    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SW_RES, context->ptrInternalContext->csxRegSwResScan);
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_IO_SEL, CY_CAPSENSE_SCAN_CSD_SW_IO_SEL_CFG);

    /* Clear all pending interrupts of the CSD HW block */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR, CY_CAPSENSE_CSD_INTR_ALL_MSK);
    (void)Cy_CSD_ReadReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_INTR);

    /* Start the normal conversion */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SEQ_START, CY_CAPSENSE_SCAN_CSD_SEQ_START_CFG);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXInitNextChScan
****************************************************************************//**
*
* This function initializes the next frequency of the sensor scan.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSXInitNextChScan(cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = context->ptrActiveScanSns;
    
        ptrActive->mfsChannelIndex++;
        ptrActive->ptrSnsContext += context->ptrCommonConfig->numSns;

        Cy_CapSense_CSXChangeClkFreq((uint32_t)ptrActive->mfsChannelIndex, context);
        Cy_CapSense_CSXConnectTxExt(context);
        Cy_CapSense_CSXSetUpIdacs(context);
        Cy_CapSense_CSXStartSample(context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXChangeClkFreq
****************************************************************************//**
*
* Calculates sensor frequency depending on the specified frequency channel.
*
* \param channelIndex
* Specifies frequency index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_CSXChangeClkFreq(uint32_t channelIndex, cy_stc_capsense_context_t * context)
{
    uint32_t tmpRegVal;
    uint32_t snsClkDivider;
        
    /* SnsClk setup */
    snsClkDivider = (uint32_t) context->ptrActiveScanSns->ptrWdContext->snsClk;
    
    /* Check divider value */
    if (0u == snsClkDivider)
    {
        snsClkDivider = 1u;
    }

    /* Change the divider based on the chId */
    switch (channelIndex)
    {
        case CY_CAPSENSE_MFS_CH1_INDEX:
        {
            snsClkDivider += context->ptrCommonConfig->csxMfsDividerOffsetF1;
            break;
        }
        case CY_CAPSENSE_MFS_CH2_INDEX:
        {
            snsClkDivider += context->ptrCommonConfig->csxMfsDividerOffsetF2;
            break;
        }
        default:
        {
            break;
        }
    }
    
    tmpRegVal = (uint32_t)context->ptrActiveScanSns->ptrWdContext->snsClkSource & (uint32_t)~((uint32_t)CY_CAPSENSE_CLK_SOURCE_AUTO_MASK);
    tmpRegVal = ((tmpRegVal << CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_SIZE_POS) | (snsClkDivider - 1u) |
            CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_CLEAR_MSK | CY_CAPSENSE_CSD_SENSE_PERIOD_LFSR_BITS_MSK);
    /* Update reg value with divider and configuration */
    Cy_CSD_WriteReg(context->ptrCommonConfig->ptrCsdBase, CY_CSD_REG_OFFSET_SENSE_PERIOD, tmpRegVal);

}

#endif /* CY_IP_MXCSDV2 */


/* [] END OF FILE */
