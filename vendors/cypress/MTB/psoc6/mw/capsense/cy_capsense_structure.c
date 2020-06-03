/***************************************************************************//**
* \file cy_capsense_structure.c
* \version 2.0
*
* \brief
* This file defines the data structure global variables and provides the
* implementation for the functions of the Data Structure module.
*
********************************************************************************
* \copyright
* Copyright 2018-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include <stddef.h>
#include <string.h>
#include "cy_syslib.h"
#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"
#include "cy_capsense_lib.h"
#include "cy_csd.h"

#if defined(CY_IP_MXCSDV2)


/*******************************************************************************
* Function Name: Cy_CapSense_IsAnyWidgetActive
****************************************************************************//**
*
* Reports whether any widget has detected touch.
*
* This function reports whether any widget has detected a touch by extracting
* information from the widget status registers. This function does 
* not process widget data but extracts previously processed results 
* from the \ref group_capsense_structures.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the touch detection status of all the widgets:
* - Zero - No touch is detected in any of the widgets or sensors.
* - Non-zero - At least one widget or sensor has detected a touch.
*
*******************************************************************************/
uint32_t Cy_CapSense_IsAnyWidgetActive(const cy_stc_capsense_context_t * context)
{
    uint32_t result = 0u;
    uint32_t wdIndex;

    for (wdIndex = context->ptrCommonConfig->numWd; wdIndex-- > 0u;)
    {
        result |= (uint32_t)context->ptrWdContext[wdIndex].status & CY_CAPSENSE_WD_ACTIVE_MASK;
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_IsWidgetActive
****************************************************************************//**
*
* Reports whether the specified widget detected touch on any of its sensors.
*
* This function reports whether the specified widget has detected a touch by
* extracting information from the widget status register.
* This function does not process widget data but extracts previously processed 
* results from the \ref group_capsense_structures.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the touch detection status of the specified widgets:
* - Zero - No touch is detected in the specified widget or a wrong widgetId
* is specified.
* - Non-zero if at least one sensor of the specified widget is active, i.e.
* a touch is detected.
*
*******************************************************************************/
uint32_t Cy_CapSense_IsWidgetActive(
                uint32_t widgetId, 
                const cy_stc_capsense_context_t * context)
{
    uint32_t result = 0uL;

    if (widgetId < context->ptrCommonConfig->numWd)
    {
        result = (uint32_t)context->ptrWdContext[widgetId].status & CY_CAPSENSE_WD_ACTIVE_MASK;
    }
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_IsSensorActive
****************************************************************************//**
*
* Reports whether the specified sensor in the widget detected touch.
*
* This function reports whether the specified sensor in the widget has detected a
* touch by extracting information from the widget status register.
* This function does not process widget or sensor data but extracts previously  
* processed results from the \ref group_capsense_structures.
*
* For proximity sensors, this function returns the proximity detection status. 
* To get the touch status of proximity sensors, use the
* Cy_CapSense_IsProximitySensorActive() function.
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
* \return
* Returns the touch detection status of the specified sensor/widget:
* - Zero if no touch is detected in the specified sensor/widget or a wrong
* widget ID/sensor ID is specified.
* - Non-zero if the specified sensor is active, i.e. touch is detected. If the
* specific sensor belongs to a proximity widget, the proximity detection
* status is returned.
*
*******************************************************************************/
uint32_t Cy_CapSense_IsSensorActive(
                uint32_t widgetId, 
                uint32_t sensorId, 
                const cy_stc_capsense_context_t * context)
{
    uint32_t result = 0uL;

    if (widgetId < context->ptrCommonConfig->numWd)
    {
        if (sensorId < context->ptrWdConfig[widgetId].numSns)
        {
            result = context->ptrWdConfig[widgetId].ptrSnsContext[sensorId].status;
        }
    }
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_IsProximitySensorActive
****************************************************************************//**
*
* Reports the status of the specified proximity widget/sensor.
*
* This function reports whether the specified proximity sensor has detected 
* a touch or proximity event by extracting information from the widget 
* status register. This function is used only with proximity widgets. 
* This function does not process widget data but extracts previously processed 
* results from the \ref group_capsense_structures.
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
* \return
* Returns the status of the specified sensor of the proximity widget. Zero
* indicates that no touch is detected in the specified sensor/widget or a
* wrong widgetId/proxId is specified.
* - Bits [31..2] are reserved.
* - Bit [1] indicates that a touch is detected.
* - Bit [0] indicates that a proximity is detected.
*
*******************************************************************************/
uint32_t Cy_CapSense_IsProximitySensorActive(
                uint32_t widgetId, 
                uint32_t sensorId, 
                const cy_stc_capsense_context_t * context)
{
    uint32_t result = 0uL;

    if (widgetId < context->ptrCommonConfig->numWd)
    {
        if ((uint8_t)CY_CAPSENSE_WD_PROXIMITY_E == context->ptrWdConfig[widgetId].wdType)
        {
            if (sensorId < context->ptrWdConfig[widgetId].numSns)
            {
                result |= context->ptrWdConfig[widgetId].ptrSnsContext[sensorId].status;
            }
        }
    }
    
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_GetTouchInfo
****************************************************************************//**
*
* Reports the details of touch position detected on the specified touchpad, 
* matrix buttons or slider widgets.
*
* This function does not process widget data but extracts previously processed 
* results from the \ref group_capsense_structures.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the pointer to widget cy_stc_capsense_touch_t structure that 
* contains number of positions and data about each position.
* 
*
*******************************************************************************/
cy_stc_capsense_touch_t * Cy_CapSense_GetTouchInfo(
                uint32_t widgetId, 
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_touch_t * ptrTouch = NULL;
    
    const cy_stc_capsense_widget_config_t * ptrWdCfg;

    if (widgetId < context->ptrCommonConfig->numWd)
    {
        ptrWdCfg = &context->ptrWdConfig[widgetId];
        switch (ptrWdCfg->wdType)
        {
            case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
            case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
            case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
            case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
                ptrTouch = &ptrWdCfg->ptrWdContext->wdTouch;
                break;
            default:
                break;
        }
    }
    
    return ptrTouch;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CheckConfigIntegrity
****************************************************************************//**
*
* Performs verification of CapSense data structure initialization.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return status
* Returns status of operation:
* - Zero        - Indicates successful initialization.
* - Non-zero    - One or more errors occurred in the initialization process.
*
*******************************************************************************/
cy_status Cy_CapSense_CheckConfigIntegrity(const cy_stc_capsense_context_t * context)
{
    cy_status result = CY_RET_SUCCESS;
    
    const cy_stc_capsense_common_config_t * ptrCommonCfg = context->ptrCommonConfig;
    const cy_stc_capsense_common_context_t * ptrCommonCxt = context->ptrCommonContext;
    const cy_stc_capsense_internal_context_t * ptrInternalCxt = context->ptrInternalContext;
    const cy_stc_capsense_widget_config_t * ptrWdCfg = context->ptrWdConfig;
    const cy_stc_capsense_widget_context_t * ptrWdCxt = context->ptrWdContext;
    const cy_stc_capsense_pin_config_t * ptrPinCfg = context->ptrPinConfig;
    const cy_stc_capsense_pin_config_t * ptrShieldPinCfg = context->ptrShieldPinConfig;
    const cy_stc_active_scan_sns_t * ptrActScanSns = context->ptrActiveScanSns;
    
    if (ptrCommonCfg == NULL)       {result |= CY_RET_BAD_DATA;}
    if (ptrCommonCxt == NULL)       {result |= CY_RET_BAD_DATA;}
    if (ptrInternalCxt == NULL)     {result |= CY_RET_BAD_DATA;}
    if (ptrWdCfg == NULL)           {result |= CY_RET_BAD_DATA;}
    if (ptrWdCxt == NULL)           {result |= CY_RET_BAD_DATA;}
    if (ptrPinCfg == NULL)          {result |= CY_RET_BAD_DATA;}
    if (ptrCommonCfg->csdShieldEn != 0u)
    {
        if((ptrCommonCfg->csdShieldNumPin > 0u) && (ptrShieldPinCfg == NULL))
        {
            result |= CY_RET_BAD_DATA;
        }
    }
    if (ptrActScanSns == NULL)      {result |= CY_RET_BAD_DATA;}
    
    
    return (result);
}

/**< Internal wrapper functions for the flash optimization */
/*******************************************************************************
* Function Name: Cy_CapSense_CSDCalibrateWidget_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSDCalibrateWidget function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSDCalibrateWidget().
*
* \param widgetId
* \param target
* \param context
* \return
*
*******************************************************************************/
cy_status Cy_CapSense_CSDCalibrateWidget_Call(
                uint32_t widgetId,
                uint32_t target,
                cy_stc_capsense_context_t * context)
{
    cy_status result = CY_RET_INVALID_STATE;
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSDCalibrateWidget)
    {
        result = ptrFptrCfg->fptrCSDCalibrateWidget(widgetId, target, context);
    }
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDSetupWidget_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSDSetupWidget function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSDSetupWidget().
*
* \param widgetId
* \param context
*
*******************************************************************************/
void Cy_CapSense_CSDSetupWidget_Call(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSDSetupWidget)
    {
        ptrFptrCfg->fptrCSDSetupWidget(widgetId, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDScan_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSDScan function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSDScan().
*
* \param context
*
*******************************************************************************/
void Cy_CapSense_CSDScan_Call(
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSDScan)
    {
        ptrFptrCfg->fptrCSDScan(context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXSetupWidget_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSXSetupWidget function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSXSetupWidget().
*
* \param widgetId
* \param context
*
*******************************************************************************/
void Cy_CapSense_CSXSetupWidget_Call(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSXSetupWidget)
    {
        ptrFptrCfg->fptrCSXSetupWidget(widgetId, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXScan_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSXScan function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSXScan().
*
* \param context
*
*******************************************************************************/
void Cy_CapSense_CSXScan_Call(
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSXScan)
    {
        ptrFptrCfg->fptrCSXScan(context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CalibrateAllCsdWidgets_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CalibrateAllCsdWidgets function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CalibrateAllCsdWidgets().
*
* \param context
* \return
*
*******************************************************************************/
cy_status Cy_CapSense_CalibrateAllCsdWidgets_Call(
                cy_stc_capsense_context_t * context)
{
    cy_status result = CY_RET_INVALID_STATE;
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCalibrateAllCsdWidgets)
    {
        result = ptrFptrCfg->fptrCalibrateAllCsdWidgets(context);
    }
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CalibrateAllCsxWidgets_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CalibrateAllCsxWidgets function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CalibrateAllCsxWidgets().
*
* \param context
* \return
*
*******************************************************************************/
cy_status Cy_CapSense_CalibrateAllCsxWidgets_Call(
                cy_stc_capsense_context_t * context)
{
    cy_status result = CY_RET_INVALID_STATE;
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCalibrateAllCsxWidgets)
    {
        result = ptrFptrCfg->fptrCalibrateAllCsxWidgets(context);
    }
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDDisableMode_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSDDisableMode function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSDDisableMode().
*
* \param context
*
*******************************************************************************/
void Cy_CapSense_CSDDisableMode_Call(
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSDDisableMode)
    {
        ptrFptrCfg->fptrCSDDisableMode(context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSDInitialize_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSDInitialize function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSDInitialize().
*
* \param context
*
*******************************************************************************/
void Cy_CapSense_CSDInitialize_Call(
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSDInitialize)
    {
        ptrFptrCfg->fptrCSDInitialize(context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessButton_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_DpProcessButton function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_DpProcessButton().
*
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpProcessButton_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessButton)
    {
        ptrFptrCfg->fptrDpProcessButton(ptrWdConfig, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxTouchpad_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_DpProcessCsxTouchpad function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_DpProcessCsxTouchpad().
*
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsxTouchpad_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessCsxTouchpad)
    {
        ptrFptrCfg->fptrDpProcessCsxTouchpad(ptrWdConfig, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessProximity_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_DpProcessProximity function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_DpProcessProximity().
*
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpProcessProximity_Call(
                cy_stc_capsense_widget_config_t const * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessProximity)
    {
        ptrFptrCfg->fptrDpProcessProximity(ptrWdConfig);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdTouchpad_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_DpProcessCsdTouchpad function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_DpProcessCsdTouchpad().
*
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdTouchpad_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessCsdTouchpad)
    {
        ptrFptrCfg->fptrDpProcessCsdTouchpad(ptrWdConfig, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessSlider_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_DpProcessSlider function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_DpProcessSlider().
*
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpProcessSlider_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessSlider)
    {
        ptrFptrCfg->fptrDpProcessSlider(ptrWdConfig, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdMatrix_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_DpProcessCsdMatrix function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_DpProcessCsdMatrix().
*
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdMatrix_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessCsdMatrix)
    {
        ptrFptrCfg->fptrDpProcessCsdMatrix(ptrWdConfig, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdWidgetStatus_Call
****************************************************************************//**
*
* This is a wrapper function for calling
* the Cy_CapSense_DpProcessCsdWidgetStatus function. All details,
* parameters and return,* For the operation details, refer to the Cy_CapSense_DpProcessCsdWidgetStatus().
*
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdWidgetStatus_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessCsdWidgetStatus)
    {
        ptrFptrCfg->fptrDpProcessCsdWidgetStatus(ptrWdConfig, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdWidgetRawCounts_Call
****************************************************************************//**
*
* This is a wrapper function for calling
* the Cy_CapSense_DpProcessCsdWidgetRawCounts function. All details,
* parameters and return,* For the operation details, refer to the Cy_CapSense_DpProcessCsdWidgetRawCounts().
*
* \param ptrWdConfig
* \param context
*
* \return
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsdWidgetRawCounts_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    cy_status result = CY_RET_INVALID_STATE;
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessCsdWidgetRawCounts)
    {
        result = ptrFptrCfg->fptrDpProcessCsdWidgetRawCounts(ptrWdConfig, context);
    }
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxWidgetStatus_Call
****************************************************************************//**
*
* This is a wrapper function for calling
* the Cy_CapSense_DpProcessCsxWidgetStatus function. All details,
* parameters and return,* For the operation details, refer to the Cy_CapSense_DpProcessCsxWidgetStatus().
*
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsxWidgetStatus_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpProcessCsxWidgetStatus)
    {
        ptrFptrCfg->fptrDpProcessCsxWidgetStatus(ptrWdConfig, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxWidgetRawCounts_Call
****************************************************************************//**
*
* This is a wrapper function for calling
* the Cy_CapSense_DpProcessCsxWidgetRawCounts function. All details,
* parameters and return,* For the operation details, refer to the Cy_CapSense_DpProcessCsxWidgetRawCounts().
*
* \param ptrWdConfig
* \param context
* \return
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsxWidgetRawCounts_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;
    uint32_t result = CY_RET_INVALID_STATE;

    if(NULL != ptrFptrCfg->fptrDpProcessCsxWidgetRawCounts)
    {
        result = ptrFptrCfg->fptrDpProcessCsxWidgetRawCounts(ptrWdConfig, context);
    }
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpAdvancedCentroidTouchpad_Call
****************************************************************************//**
*
* This is a wrapper function for calling
* the Cy_CapSense_DpAdvancedCentroidTouchpad function. All details, parameters
* and return,* For the operation details, refer to the Cy_CapSense_DpAdvancedCentroidTouchpad().
*
* \param newTouch
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpAdvancedCentroidTouchpad_Call(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpAdvancedCentroidTouchpad)
    {
        ptrFptrCfg->fptrDpAdvancedCentroidTouchpad(newTouch, ptrWdConfig);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitPositionFilters_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_InitPositionFilters function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_InitPositionFilters().
*
* \param filterConfig
* \param ptrInput
* \param ptrHistory
* \param context
*
*******************************************************************************/
void Cy_CapSense_InitPositionFilters_Call(
                uint32_t filterConfig,
                const cy_stc_capsense_position_t * ptrInput,
                cy_stc_capsense_position_t * ptrHistory,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrInitPositionFilters)
    {
        ptrFptrCfg->fptrInitPositionFilters(filterConfig, ptrInput, ptrHistory);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_RunPositionFilters_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_RunPositionFilters function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_RunPositionFilters().
*
* \param ptrWdConfig
* \param ptrInput
* \param ptrHistory
* \param context
*
*******************************************************************************/
void Cy_CapSense_RunPositionFilters_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_position_t * ptrInput,
                cy_stc_capsense_position_t * ptrHistory,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrRunPositionFilters)
    {
        ptrFptrCfg->fptrRunPositionFilters(ptrWdConfig, ptrInput, ptrHistory, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_ProcessPositionFilters_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_ProcessPositionFilters function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_ProcessPositionFilters().
*
* \param newTouch
* \param ptrWdConfig
* \param context
*
*******************************************************************************/
void Cy_CapSense_ProcessPositionFilters_Call(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrProcessPositionFilters)
    {
        ptrFptrCfg->fptrProcessPositionFilters(newTouch, ptrWdConfig, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeAllFilters_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_InitializeAllFilters function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_InitializeAllFilters().
*
* \param context
*
*******************************************************************************/
void Cy_CapSense_InitializeAllFilters_Call(
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrInitializeAllFilters)
    {
        ptrFptrCfg->fptrInitializeAllFilters(context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_FtRunEnabledFiltersInternal_Call
****************************************************************************//**
*
* This is a wrapper function for calling
* the Cy_CapSense_FtRunEnabledFiltersInternal function. All details,
* parameters and return,* For the operation details, refer to the Cy_CapSense_FtRunEnabledFiltersInternal().
*
* \param ptrWdConfig
* \param ptrSnsContext
* \param ptrSnsRawHistory
* \param ptrSnsRawHistoryLow
* \param context
*
*******************************************************************************/
void Cy_CapSense_FtRunEnabledFiltersInternal_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                uint16_t * ptrSnsRawHistory,
                uint8_t * ptrSnsRawHistoryLow,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrFtRunEnabledFiltersInternal)
    {
        ptrFptrCfg->fptrFtRunEnabledFiltersInternal(ptrWdConfig, ptrSnsContext, ptrSnsRawHistory, ptrSnsRawHistoryLow);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXDisableMode_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSXDisableMode function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSXDisableMode().
*
* \param context
*
*******************************************************************************/
void Cy_CapSense_CSXDisableMode_Call(
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSXDisableMode)
    {
        ptrFptrCfg->fptrCSXDisableMode(context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CSXInitialize_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_CSXInitialize function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_CSXInitialize().
*
* \param context
*
*******************************************************************************/
void Cy_CapSense_CSXInitialize_Call(
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrCSXInitialize)
    {
        ptrFptrCfg->fptrCSXInitialize(context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_AdaptiveFilterInitialize_Lib_Call
****************************************************************************//**
*
* This is a wrapper function for calling
* the Cy_CapSense_AdaptiveFilterInitialize_Lib function. All details,
* parameters and return,* For the operation details, refer to the Cy_CapSense_AdaptiveFilterInitialize_Lib().
*
* \param config
* \param positionContext
* \param context
*
*******************************************************************************/
void Cy_CapSense_AdaptiveFilterInitialize_Lib_Call(
                const cy_stc_capsense_adaptive_filter_config_t * config,
                cy_stc_capsense_position_t * positionContext,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrAdaptiveFilterInitializeLib)
    {
        ptrFptrCfg->fptrAdaptiveFilterInitializeLib(config, positionContext);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_AdaptiveFilterRun_Lib_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_AdaptiveFilterRun_Lib function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_AdaptiveFilterRun_Lib().
*
* \param config
* \param positionContext
* \param currentX
* \param currentY
* \param context
*
*******************************************************************************/
void Cy_CapSense_AdaptiveFilterRun_Lib_Call(
                const cy_stc_capsense_adaptive_filter_config_t * config,
                cy_stc_capsense_position_t * positionContext,
                uint32_t * currentX,
                uint32_t * currentY,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrAdaptiveFilterRunLib)
    {
        ptrFptrCfg->fptrAdaptiveFilterRunLib(config, positionContext, currentX, currentY);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_BallisticMultiplier_Lib_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_BallisticMultiplier_Lib function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_BallisticMultiplier_Lib().
*
* \param config
* \param touch
* \param delta
* \param timestamp
* \param ballisticContext
* \param context
*
*******************************************************************************/
void Cy_CapSense_BallisticMultiplier_Lib_Call(
                const cy_stc_capsense_ballistic_config_t * config,
                const cy_stc_capsense_touch_t * touch,
                cy_stc_capsense_ballistic_delta_t * delta,
                uint32_t timestamp,
                cy_stc_capsense_ballistic_context_t * ballisticContext,
                const cy_stc_capsense_context_t * context)

{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrBallisticMultiplierLib)
    {
        ptrFptrCfg->fptrBallisticMultiplierLib(config, touch, delta, timestamp, ballisticContext);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpUpdateThresholds_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_DpUpdateThresholds function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_DpUpdateThresholds().
*
* \param ptrWdContext
* \param ptrNoiseEnvelope
* \param startFlag
* \param context
*
*******************************************************************************/
void Cy_CapSense_DpUpdateThresholds_Call(
                cy_stc_capsense_widget_context_t * ptrWdContext,
                const cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                uint32_t startFlag,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrDpUpdateThresholds)
    {
        ptrFptrCfg->fptrDpUpdateThresholds(ptrWdContext, ptrNoiseEnvelope, startFlag);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeNoiseEnvelope_Lib_Call
****************************************************************************//**
*
* This is a wrapper function for calling
* the Cy_CapSense_InitializeNoiseEnvelope_Lib function. All details,
* parameters and return,* For the operation details, refer to the Cy_CapSense_InitializeNoiseEnvelope_Lib().
*
* \param rawCount
* \param sigPFC
* \param ptrNoiseEnvelope
* \param context
*
*******************************************************************************/
void Cy_CapSense_InitializeNoiseEnvelope_Lib_Call(
                uint16_t rawCount,
                uint16_t sigPFC,
                cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrInitializeNoiseEnvelopeLib)
    {
        ptrFptrCfg->fptrInitializeNoiseEnvelopeLib(rawCount, sigPFC, ptrNoiseEnvelope);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_RunNoiseEnvelope_Lib_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_RunNoiseEnvelope_Lib function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_RunNoiseEnvelope_Lib().
*
* \param rawCount
* \param sigPFC
* \param ptrNoiseEnvelope
* \param context
*
*******************************************************************************/
void Cy_CapSense_RunNoiseEnvelope_Lib_Call(
                uint16_t rawCount,
                uint16_t sigPFC,
                cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                const cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrRunNoiseEnvelopeLib)
    {
        ptrFptrCfg->fptrRunNoiseEnvelopeLib(rawCount, sigPFC, ptrNoiseEnvelope);
    }
}



/*******************************************************************************
* Function Name: Cy_CapSense_SsAutoTune_Call
****************************************************************************//**
*
* This is a wrapper of the Cy_CapSense_SsAutoTune function.
* It calls this function if the corresponding function pointer is initialized.
*
* For the operation details, refer to the Cy_CapSense_SsAutoTune().
*
* \param context
* \return
*
*******************************************************************************/
cy_status Cy_CapSense_SsAutoTune_Call(
                cy_stc_capsense_context_t * context)
{
    cy_status result = CY_RET_INVALID_STATE;
    cy_stc_capsense_fptr_config_t * ptrFptrCfg = (cy_stc_capsense_fptr_config_t *)context->ptrFptrConfig;

    if(NULL != ptrFptrCfg->fptrSsAutoTune)
    {
        result = ptrFptrCfg->fptrSsAutoTune(context);
    }
    return result;
}


#endif /* CY_IP_MXCSDV2 */


/* [] END OF FILE */
