/***************************************************************************//**
* \file cy_capsense_processing.c
* \version 2.0
*
* \brief
* This file provides the source code for the Data Processing module functions.
* The Data Processing module is responsible for the low-level raw count
* processing provided by the sensing module, maintaining baseline and
* difference values and performing high-level widget processing like
* updating button status or calculating slider position.
*
********************************************************************************
* \copyright
* Copyright 2018-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "cy_syslib.h"
#include <stddef.h>
#include <string.h>
#include "cy_capsense_common.h"
#include "cy_capsense_processing.h"
#include "cy_capsense_filter.h"
#include "cy_capsense_lib.h"
#include "cy_capsense_sensing.h"
#include "cy_capsense_centroid.h"

#if defined(CY_IP_MXCSDV2)


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeAllStatuses
****************************************************************************//**
*
* Performs initialization of all statuses and related modules including
* debounce counters and touch positions of all the widgets.
*
* The initialization includes the following tasks:
* * Reset the debounce counters of all the widgets.
* * Reset the number of touches.
* * Reset the position filter history for slider and touchpad widgets.
* * Clear all status of widgets and sensors.
* * Enable all the widgets.
*
* Calling this function is accompanied by 
* * Cy_CapSense_InitializeAllBaselines().
* * Cy_CapSense_InitializeAllFilters().
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InitializeAllStatuses(const cy_stc_capsense_context_t * context)
{
    uint32_t widgetId;

    for(widgetId = context->ptrCommonConfig->numWd; widgetId-- > 0u;)
    {
        Cy_CapSense_InitializeWidgetStatus(widgetId, context);
        Cy_CapSense_InitializeWidgetGestures(widgetId, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeWidgetStatus
****************************************************************************//**
*
* Performs initialization of all statuses, debounce counters, and touch positions
* of the specified widget.
*
* The initialization includes:
* * Resets the debounce counter of the widget.
* * Resets the number of touches.
* * Resets the position filter history for slider and touchpad widgets.
* * Clears widget and sensor statuses.
* * Enables the widget.
*
* The Button and Matrix Button widgets have individual debounce counters per
* sensor for the CSD widgets and per node for the CSX widgets.
*
* The Slider and Touchpad widgets have a single debounce counter per widget.
*
* The Proximity widget has two debounce counters per sensor. One is for the
* proximity event and the second is for the touch event.
*
* All debounce counters during initialization are set to the value of the
* onDebounce widget parameter.
*
* Calling this function is accompanied by 
* * Cy_CapSense_InitializeWidgetBaseline().
* * Cy_CapSense_InitializeWidgetFilter().
* 
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InitializeWidgetStatus(
                uint32_t widgetId, 
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t filterSize;
    const cy_stc_capsense_widget_config_t * ptrWdCfg = &context->ptrWdConfig[widgetId];
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdCfg->ptrWdContext;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdCfg->ptrSnsContext;
    uint32_t snsNumber = ptrWdCfg->numSns;
    cy_stc_capsense_position_t * ptrHistory;

    /* Clear widget statuses (Non active, Not disabled, Working) */
    ptrWdCxt->status &= (uint8_t)~(CY_CAPSENSE_WD_ACTIVE_MASK |
                                   CY_CAPSENSE_WD_DISABLE_MASK |
                                   CY_CAPSENSE_WD_WORKING_MASK);
    /* Clear sensor status */
    for (snsIndex = snsNumber; snsIndex-- >0u;)
    {
        ptrSnsCxt->status &= (uint8_t)~(CY_CAPSENSE_SNS_TOUCH_STATUS_MASK | CY_CAPSENSE_SNS_PROX_STATUS_MASK);
        ptrSnsCxt++;
    }

    /* Reset debounce counters */
    switch (ptrWdCfg->wdType)
    {
        case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
        case (uint8_t)CY_CAPSENSE_WD_BUTTON_E:
            /* Each button requires one debounce counter */
            (void)memset(ptrWdCfg->ptrDebounceArr, (int32_t)ptrWdCxt->onDebounce, (size_t)snsNumber);
            break;
        case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
        case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
        case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
            /* Each widget requires one debounce counter */
            if (ptrWdCfg->senseMethod == (uint8_t)CY_CAPSENSE_SENSE_METHOD_CSD_E)
            {
                *(ptrWdCfg->ptrDebounceArr) = ptrWdCxt->onDebounce;
            }
            else
            {
                /*
                * CSX Touchpad has debounce located in another place. Moreover,
                * debounce counter is initialized at ID assignment, so no need
                * to do it here.
                */
            }
            break;
        case (uint8_t)CY_CAPSENSE_WD_PROXIMITY_E:
            /* Proximity widgets have 2 debounce counters per sensor (for touch and prox detection) */
            (void)memset(ptrWdCfg->ptrDebounceArr, (int32_t)ptrWdCxt->onDebounce, (size_t)(snsNumber << 1u));
            break;
        default:
            break;
    }

    /* Reset touch numbers */
    switch (ptrWdCfg->wdType)
    {
        case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
        case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
        case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
        case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
            /* Clean number of touches */
            ptrWdCxt->wdTouch.numPosition = CY_CAPSENSE_POSITION_NONE;
            if (0u != (ptrWdCfg->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_MASK))
            {
                ptrWdCfg->ptrPosFilterHistory->numPosition = CY_CAPSENSE_POSITION_NONE;
            }
            break;
        default:
            break;
    }

    /* Reset ballistic displacement */
    if (0u != (ptrWdCfg->centroidConfig & CY_CAPSENSE_CENTROID_BALLISTIC_MASK))
    {
        ptrWdCxt->xDelta = 0;
        ptrWdCxt->yDelta = 0;
        ptrWdCfg->ptrBallisticContext->oldTouchNumber = 0u;
    }

    /* Reset touch history */
    if (0u != (ptrWdCfg->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_MASK))
    {
        switch (ptrWdCfg->wdType)
        {
            case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
                /* Clean position filter history */
                if (ptrWdCfg->senseMethod == (uint8_t)CY_CAPSENSE_SENSE_METHOD_CSX_E)
                {
                    /* Reset all history IDs to undefined state */
                    ptrHistory = ptrWdCfg->ptrPosFilterHistory->ptrPosition;
                    filterSize = (ptrWdCfg->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_SIZE_MASK) >> CY_CAPSENSE_POSITION_FILTERS_SIZE_OFFSET;
                    for (snsIndex = 0u; snsIndex < CY_CAPSENSE_MAX_CENTROIDS; snsIndex++)
                    {
                        ptrHistory->id = CY_CAPSENSE_CSX_TOUCHPAD_ID_UNDEFINED;
                        ptrHistory += filterSize;
                    }
                }
                break;
            default:
                break;
        }
        /* Init Adaptive IIR filter */
        if (0u != (ptrWdCfg->posFilterConfig & CY_CAPSENSE_POSITION_AIIR_MASK))
        {
            Cy_CapSense_AdaptiveFilterInitialize_Lib_Call(&ptrWdCfg->aiirConfig,
                                                          ptrWdCfg->ptrPosFilterHistory->ptrPosition,
                                                          context);
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeWidgetGestures
****************************************************************************//**
*
* Performs initialization of all gestures for the specified widget.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InitializeWidgetGestures(
                uint32_t widgetId, 
                const cy_stc_capsense_context_t * context)
{
    const cy_stc_capsense_widget_config_t * ptrWdCfg = &context->ptrWdConfig[widgetId];
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdCfg->ptrWdContext;

    if (((uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E == ptrWdCfg->wdType) ||
        ((uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType))
    {
        if (NULL != ptrWdCfg->ptrGestureConfig)
        {
            if (0u != (ptrWdCfg->ptrGestureConfig->gestureEnableMask & CY_CAPSENSE_GESTURE_ALL_GESTURES_MASK))
            {
                ptrWdCxt->gestureDetected = 0u;
                ptrWdCxt->gestureDirection = 0u;
                Cy_CapSense_Gesture_ResetState(ptrWdCfg->ptrGestureContext);
            }
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DecodeWidgetGestures
****************************************************************************//**
*
* Performs processing of all gestures for the specified widget.
*
* This function should be called by application program only after all sensors
* are scanned and all data processing is executed using 
* Cy_CapSense_ProcessAllWidgets() or Cy_CapSense_ProcessWidget() functions 
* for the widget. Calling this function multiple times without a new sensor
* scan and process causes unexpected behavior.
* 
* \note The function (Gesture detection functionality) requires a timestamp 
* for its operation. The timestamp should be initialized and maintained 
* in the application program prior to calling this function. See the 
* descriptions of the Cy_CapSense_SetGestureTimestamp() and 
* Cy_CapSense_IncrementGestureTimestamp() functions for details.
* 
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found 
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the detected Gesture mask and direction of detected gestures.
* The same information is stored in ptrWdContext->gestureDetected and 
* ptrWdContext->gestureDirection registers. Corresponding macros could be found 
* \ref group_capsense_macros_gesture.
* * bit[0..15] - detected gesture masks gesture
*   * bit[0] - one-finger single click gesture
*   * bit[1] - one-finger double click gesture
*   * bit[2] - one-finger click and drag gesture
*   * bit[3] - two-finger single click gesture
*   * bit[4] - one-finger scroll gesture
*   * bit[5] - two-finger scroll gesture
*   * bit[6] - one-finger edge swipe
*   * bit[7] - one-finger flick
*   * bit[8] - one-finger rotate
*   * bit[9] - two-finger zoom
*   * bit[13] - touchdown event
*   * bit[14] - liftoff event
* * bit[16..31] - gesture direction if detected
*    * bit[0..1] - direction of one-finger scroll gesture
*    * bit[2..3] - direction of two-finger scroll gesture
*    * bit[4..5] - direction of one-finger edge swipe gesture
*    * bit[6] - direction of one-finger rotate gesture
*    * bit[7] - direction of two-finger zoom gesture
*    * bit[8..10] - direction of one-finger flick gesture
*
* \funcusage
* 
* An example of gesture decoding:
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_Gesture
* 
* An example of gesture status parsing:
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_Gesture_Macro
* 
*******************************************************************************/
uint32_t Cy_CapSense_DecodeWidgetGestures(
                uint32_t widgetId, 
                const cy_stc_capsense_context_t * context)
{
    uint32_t gestureStatus = 0u;
    uint32_t posIndex;
    uint32_t positionNum;
    const cy_stc_capsense_widget_config_t * ptrWdCfg = &context->ptrWdConfig[widgetId];
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdCfg->ptrWdContext;
    cy_stc_capsense_gesture_position_t position[CY_CAPSENSE_MAX_CENTROIDS];

    if (((uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E == ptrWdCfg->wdType) ||
        ((uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType))
    {
        if (NULL != ptrWdCfg->ptrGestureConfig)
        {
            if (0u != (ptrWdCfg->ptrGestureConfig->gestureEnableMask & CY_CAPSENSE_GESTURE_ALL_GESTURES_MASK))
            {
                positionNum = ptrWdCxt->wdTouch.numPosition;
                if (positionNum > CY_CAPSENSE_MAX_CENTROIDS)
                {
                    positionNum = 0u;
                }
                for (posIndex = 0u; posIndex < positionNum; posIndex++)
                {
                    position[posIndex].x = ptrWdCxt->wdTouch.ptrPosition[posIndex].x;
                    position[posIndex].y = ptrWdCxt->wdTouch.ptrPosition[posIndex].y;
                }
                Cy_CapSense_Gesture_Decode(context->ptrCommonContext->timestamp, (uint32_t)ptrWdCxt->wdTouch.numPosition,
                        &position[0u], ptrWdCfg->ptrGestureConfig, ptrWdCfg->ptrGestureContext);
                ptrWdCxt->gestureDetected = ptrWdCfg->ptrGestureContext->detected;
                ptrWdCxt->gestureDirection = ptrWdCfg->ptrGestureContext->direction;
                gestureStatus = (uint32_t)ptrWdCxt->gestureDetected | ((uint32_t)ptrWdCxt->gestureDirection << CY_CAPSENSE_GESTURE_DIRECTION_OFFSET);
            }
        }
    }
    return gestureStatus;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxWidgetRawCounts
****************************************************************************//**
*
* Performs default processing of the raw counts of the specified CSX widget.
*
* The processing includes the following tasks:
* - Run Filters.
* - Update Baselines.
* - Update Differences.
* The same process is applied to all the sensors of the specified widget.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified widget processing operation:
* - Zero - if operation was successfully completed;
* - Non-zero - if baseline processing of any sensor of the specified widget
* failed. The result is concatenated with the index of failed sensor.
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsxWidgetRawCounts(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t result = CY_RET_SUCCESS;
    uint32_t snsIndex;
    uint32_t snsHistorySize;
    uint32_t freqChIndex;
    uint32_t freqChNumber;
    uint16_t * ptrHistoryCh;
    uint16_t * ptrHistorySns;
    uint8_t * ptrHistoryLowCh = NULL;
    uint8_t * ptrHistoryLowSns = NULL;
    cy_stc_capsense_sensor_context_t * ptrSnsCxtCh;
    cy_stc_capsense_sensor_context_t * ptrSnsCxtSns;

    snsHistorySize = (uint32_t)ptrWdConfig->rawFilterConfig & CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_MASK;
    freqChNumber = (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn) ? 3u : 1u;

    ptrSnsCxtCh = &ptrWdConfig->ptrSnsContext[0u];
    ptrHistoryCh = &ptrWdConfig->ptrRawFilterHistory[0u];
    if(CY_CAPSENSE_IIR_FILTER_PERFORMANCE == (ptrWdConfig->rawFilterConfig & CY_CAPSENSE_RC_FILTER_IIR_MODE_MASK))
    {
        ptrHistoryLowCh = &ptrWdConfig->ptrRawFilterHistoryLow[0u];
    }

    for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
    {
        ptrSnsCxtSns = ptrSnsCxtCh;
        ptrHistorySns = ptrHistoryCh;
        ptrHistoryLowSns = ptrHistoryLowCh;
        for (snsIndex = 0u; snsIndex < ptrWdConfig->numSns; snsIndex++)    
        {
            Cy_CapSense_FtRunEnabledFiltersInternal_Call(ptrWdConfig,
                                                         ptrSnsCxtSns,
                                                         ptrHistorySns,
                                                         ptrHistoryLowSns,
                                                         context);
            result |= Cy_CapSense_FtUpdateBaseline(ptrWdConfig->ptrWdContext, ptrSnsCxtSns, context);
            Cy_CapSense_DpUpdateDifferences(ptrWdConfig->ptrWdContext, ptrSnsCxtSns);
            ptrSnsCxtSns++;
            ptrHistorySns += snsHistorySize;
            if(NULL != ptrHistoryLowSns)
            {
                ptrHistoryLowSns++;
            }
        }

        ptrSnsCxtCh += context->ptrCommonConfig->numSns;
        ptrHistoryCh += context->ptrCommonConfig->numSns * snsHistorySize;
        if(NULL != ptrHistoryLowCh)
        {
            ptrHistoryLowCh += context->ptrCommonConfig->numSns;
        }
    }

    if(CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn)
    {
        ptrSnsCxtSns = ptrWdConfig->ptrSnsContext;
        for (snsIndex = ptrWdConfig->numSns; snsIndex-- > 0u;)
        {
            Cy_CapSense_RunMfsFiltering(ptrSnsCxtSns, context);
            ptrSnsCxtSns++;
        }
    }
    
    return result;
}

/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxWidgetStatus
****************************************************************************//**
*
* Updates the status of the CSX widget in the Data Structure.
*
* This function determines the type of widget and runs the appropriate function
* that implements the status update algorithm for this type of widget.
*
* When the widget-specific processing completes this function updates the 
* sensor and widget status registers in the data structure.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsxWidgetStatus(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    switch (ptrWdConfig->wdType)
    {
        case (uint8_t)CY_CAPSENSE_WD_BUTTON_E:
        case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
            Cy_CapSense_DpProcessButton_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
            Cy_CapSense_DpProcessCsxTouchpad_Call(ptrWdConfig, context);
            break;

    default:
        CY_ASSERT(0 != 0);
        break;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxSensorRawCountsExt
****************************************************************************//**
*
* Performs customized processing of the CSX sensor raw counts.
*
* If all bits are set at once, the default processing order will take place.
* For a custom order, this function can be called multiple times and execute
* one task at a time.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param ptrSnsContext    
* The pointer to the sensor context structure.
*
* \param ptrSnsRawHistory    
* The pointer to the filter history.
*
* \param ptrSnsRawHistoryLow    
* The pointer to the extended filter history.
*
* \param mode         
* The bit-mask with the data processing tasks to be executed.
* The mode parameters can take the following values:
* - CY_CAPSENSE_PROCESS_FILTER     (0x01) Run Firmware Filter
* - CY_CAPSENSE_PROCESS_BASELINE   (0x02) Update Baselines
* - CY_CAPSENSE_PROCESS_DIFFCOUNTS (0x04) Update Difference Counts
* - CY_CAPSENSE_PROCESS_ALL               Execute all tasks
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified sensor processing operation:
* - CY_RET_SUCCESS if operation was successfully completed;
* - CY_CAPSENSE_PROCESS_BASELINE_FAILED if baseline processing of any
* sensor of the specified widget failed. The result is concatenated with the index
* of failed sensor.
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsxSensorRawCountsExt(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                uint16_t * ptrSnsRawHistory,
                uint8_t * ptrSnsRawHistoryLow,
                uint32_t mode,
                const cy_stc_capsense_context_t * context)
{
    uint32_t  result = CY_RET_SUCCESS;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    if (0u != (mode & CY_CAPSENSE_PROCESS_FILTER))
    {
        Cy_CapSense_FtRunEnabledFiltersInternal_Call(ptrWdConfig,
                                                     ptrSnsContext,
                                                     ptrSnsRawHistory,
                                                     ptrSnsRawHistoryLow,
                                                     context);

    }

    if (0u != (mode & CY_CAPSENSE_PROCESS_BASELINE))
    {
        result = Cy_CapSense_FtUpdateBaseline(ptrWdCxt, ptrSnsContext, context);
    }
    if (0u != (mode & CY_CAPSENSE_PROCESS_DIFFCOUNTS))
    {
        Cy_CapSense_DpUpdateDifferences(ptrWdCxt, ptrSnsContext);
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdWidgetRawCounts
****************************************************************************//**
*
* Performs default processing of the raw counts of the specified CSD widget.
*
* The processing includes the following tasks:
* - Run Filters.
* - Update Baselines.
* - Update Differences.
* The same process is applied to all the sensors of the specified widget.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified widget processing operation:
* - Zero - if operation was successfully completed.
* - Non-zero - if baseline processing of any sensor of the specified widget
* failed. The result is concatenated with the index of failed sensor.
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsdWidgetRawCounts(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t result = CY_RET_SUCCESS;
    uint32_t snsIndex;
    uint32_t snsHistorySize;
    uint32_t freqChIndex;
    uint32_t freqChNumber;
    uint16_t * ptrHistoryCh;
    uint16_t * ptrHistorySns;
    uint8_t * ptrHistoryLowCh = NULL;
    uint8_t * ptrHistoryLowSns = NULL;
    cy_stc_capsense_sensor_context_t * ptrSnsCxtCh;
    cy_stc_capsense_sensor_context_t * ptrSnsCxtSns;
    cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNEHistory = ptrWdConfig->ptrNoiseEnvelope;

    snsHistorySize = (uint32_t)ptrWdConfig->rawFilterConfig & CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_MASK;
    freqChNumber = (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn) ? 3u : 1u;

    ptrSnsCxtCh = &ptrWdConfig->ptrSnsContext[0u];
    ptrHistoryCh = &ptrWdConfig->ptrRawFilterHistory[0u];
    if(CY_CAPSENSE_IIR_FILTER_PERFORMANCE == (ptrWdConfig->rawFilterConfig & CY_CAPSENSE_RC_FILTER_IIR_MODE_MASK))
    {
        ptrHistoryLowCh = &ptrWdConfig->ptrRawFilterHistoryLow[0u];
    }
        
    for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
    {
        ptrSnsCxtSns = ptrSnsCxtCh;
        ptrHistorySns = ptrHistoryCh;
        ptrHistoryLowSns = ptrHistoryLowCh;
        for (snsIndex = 0u; snsIndex < ptrWdConfig->numSns; snsIndex++)
        {
            Cy_CapSense_FtRunEnabledFiltersInternal_Call(ptrWdConfig,
                                                         ptrSnsCxtSns,
                                                         ptrHistorySns,
                                                         ptrHistoryLowSns,
                                                         context);
            /* Run auto-tuning activities */
            if (CY_CAPSENSE_CSD_SS_HWTH_EN == context->ptrCommonConfig->csdAutotuneEn)
            {
                Cy_CapSense_RunNoiseEnvelope_Lib_Call(ptrSnsCxtSns->raw,
                                                      ptrWdConfig->ptrWdContext->sigPFC,
                                                      ptrNEHistory,
                                                      context);
                Cy_CapSense_DpUpdateThresholds_Call(ptrWdConfig->ptrWdContext, ptrNEHistory, snsIndex, context);
                if ((uint8_t)CY_CAPSENSE_WD_PROXIMITY_E == ptrWdConfig->wdType)
                {
                    ptrWdConfig->ptrWdContext->proxTh = (uint16_t)(((uint32_t)ptrWdConfig->ptrWdContext->fingerTh * 
                        context->ptrCommonConfig->proxTouchCoeff) / CY_CAPSENSE_PERCENTAGE_100);
                }
                ptrNEHistory++;
            }
            
            result |= Cy_CapSense_FtUpdateBaseline(ptrWdConfig->ptrWdContext, ptrSnsCxtSns, context);
            Cy_CapSense_DpUpdateDifferences(ptrWdConfig->ptrWdContext, ptrSnsCxtSns);

            ptrSnsCxtSns++;
            ptrHistorySns += snsHistorySize;
            if(NULL != ptrHistoryLowSns)
            {
                ptrHistoryLowSns++;
            }

        }

        ptrSnsCxtCh += context->ptrCommonConfig->numSns;
        ptrHistoryCh += context->ptrCommonConfig->numSns * snsHistorySize;
        if(NULL != ptrHistoryLowCh)
        {
            ptrHistoryLowCh += context->ptrCommonConfig->numSns;
        }

    }

    if(CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn)
    {
        ptrSnsCxtSns = ptrWdConfig->ptrSnsContext;
        for (snsIndex = ptrWdConfig->numSns; snsIndex-- > 0u;)
        {
            Cy_CapSense_RunMfsFiltering(ptrSnsCxtSns, context);
            ptrSnsCxtSns++;
        }
    }

    return result;

}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdSensorRawCountsExt
****************************************************************************//**
*
* Performs customized processing of the CSX sensor raw counts.
*
* If all bits are set at once, the default processing order will take place.
* For a custom order, this function can be called multiple times and execute 
* one task at a time.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param ptrSnsContext    
* The pointer to the sensor context structure.
*
* \param ptrSnsRawHistory    
* The pointer to the filter history.
*
* \param ptrSnsRawHistoryLow    
* The pointer to the extended filter history.
*
* \param mode         
* The bit-mask with the data processing tasks to be executed.
* The mode parameters can take the following values:
* - CY_CAPSENSE_PROCESS_FILTER     (0x01) Run Firmware Filter
* - CY_CAPSENSE_PROCESS_BASELINE   (0x02) Update Baselines
* - CY_CAPSENSE_PROCESS_DIFFCOUNTS (0x04) Update Difference Counts
* - CY_CAPSENSE_PROCESS_CALC_NOISE (0x08) Calculate Noise (only if FW Tuning is enabled)
* - CY_CAPSENSE_PROCESS_THRESHOLDS (0x10) Update Thresholds (only if FW Tuning is enabled)
* - CY_CAPSENSE_PROCESS_ALL               Execute all tasks
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified sensor processing operation:
* - CY_RET_SUCCESS if operation was successfully completed.
* - CY_CAPSENSE_PROCESS_BASELINE_FAILED if baseline processing of any
* sensor of the specified widget failed. The result is concatenated with the index
* of the failed sensor.
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsdSensorRawCountsExt(
                    const cy_stc_capsense_widget_config_t * ptrWdConfig,
                    cy_stc_capsense_sensor_context_t * ptrSnsContext,
                    uint16_t * ptrSnsRawHistory,
                    uint8_t * ptrSnsRawHistoryLow,
                    uint32_t mode,
                    const cy_stc_capsense_context_t * context)
{
    uint32_t  result = CY_RET_SUCCESS;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    if (0u != (mode & CY_CAPSENSE_PROCESS_FILTER))
    {
        Cy_CapSense_FtRunEnabledFiltersInternal_Call(ptrWdConfig,
                                                     ptrSnsContext,
                                                     ptrSnsRawHistory,
                                                     ptrSnsRawHistoryLow,
                                                     context);
    }

    if (0u != (mode & CY_CAPSENSE_PROCESS_BASELINE))
    {
        result = Cy_CapSense_FtUpdateBaseline(ptrWdCxt, ptrSnsContext, context);
    }
    if (0u != (mode & CY_CAPSENSE_PROCESS_DIFFCOUNTS))
    {
        Cy_CapSense_DpUpdateDifferences(ptrWdCxt, ptrSnsContext);
    }

    return result;
}

/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdWidgetStatus
****************************************************************************//**
*
* Updates the status of the CSD widget in the Data Structure.
*
* This function determines the type of widget and runs the appropriate function
* that implements the status update algorithm for this type of widget.
*
* When the widget-specific processing completes this function updates the 
* sensor and widget status registers in the data structure.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdWidgetStatus(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    switch (ptrWdConfig->wdType)
    {
        case (uint8_t)CY_CAPSENSE_WD_BUTTON_E:
            Cy_CapSense_DpProcessButton_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
        case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
            Cy_CapSense_DpProcessSlider_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
            Cy_CapSense_DpProcessCsdMatrix_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
            Cy_CapSense_DpProcessCsdTouchpad_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_PROXIMITY_E:
            Cy_CapSense_DpProcessProximity_Call(ptrWdConfig, context);
            break;

        default:
            break;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpUpdateThresholds
****************************************************************************//**
*
* Updates noise and finger thresholds for a specified widget.
*
* This algorithm is a part of SmartSense feature.
*
* \param ptrWdContext       
* The pointer to the widget context structure.
*
* \param ptrNoiseEnvelope 
* The pointer to the noise-envelope history structure.
*
* \param startFlag        
* The flag indicates when a new widget is processed.
*
*******************************************************************************/
void Cy_CapSense_DpUpdateThresholds(
                cy_stc_capsense_widget_context_t * ptrWdContext,
                const cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                uint32_t startFlag)
{
    cy_stc_capsense_smartsense_update_thresholds_t thresholds;

    /* Calculate Thresholds */
    thresholds.fingerTh = ptrWdContext->fingerTh;
    Cy_CapSense_UpdateThresholds_Lib(ptrNoiseEnvelope, &thresholds, ptrWdContext->sigPFC, startFlag);

    /* Update CapSense context */
    ptrWdContext->fingerTh = thresholds.fingerTh;
    ptrWdContext->noiseTh = thresholds.noiseTh;
    ptrWdContext->nNoiseTh = thresholds.nNoiseTh;
    ptrWdContext->hysteresis = thresholds.hysteresis;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpUpdateDifferences
****************************************************************************//**
*
* Calculates new difference values.
*
* This function calculates the difference between the baseline and raw counts.
* If the difference is positive (raw > baseline), and higher than the widget
* noise threshold, it is saved into the data structure for further processing.
* Otherwise the difference is set to zero. The final difference value is saved
* with the subtracted noise threshold value.
*
* \param ptrWdContext       
* The pointer to the widget context structure.
*
* \param ptrSnsContext       
* The pointer to the sensor context structure.
*
*******************************************************************************/
void Cy_CapSense_DpUpdateDifferences(
                const cy_stc_capsense_widget_context_t * ptrWdContext,
                cy_stc_capsense_sensor_context_t * ptrSnsContext)
{
    ptrSnsContext->diff = 0u;
    if (ptrSnsContext->raw > (ptrSnsContext->bsln + ptrWdContext->noiseTh))
    {
        ptrSnsContext->diff = ptrSnsContext->raw - ptrSnsContext->bsln;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessButton
****************************************************************************//**
*
* Processes the status of the Button widget.
*
* This function processes the status of the CSD/CSX Button widget and
* CSX Matrix Button widget. It applies the hysteresis and debounce algorithm
* to each sensor difference value. This function is expected to be called
* after each new widget scan. If it is called multiple times for the same
* scan data, the debounce algorithm will not work properly.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessButton(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t activeCount = 0u;
    uint32_t startIndex = 0u;
    uint32_t touchTh;
    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    ptrWdCxt->status &= (uint8_t)~CY_CAPSENSE_WD_ACTIVE_MASK;

    /* Go through all widget's sensors */
    for (snsIndex = 0u; snsIndex < ptrWdConfig->numSns; snsIndex++)
    {
        /* Calculate touch threshold based on current sensor state */
        touchTh = (0u == ptrSnsCxt->status) ?
                  ((uint32_t)ptrWdCxt->fingerTh + ptrWdCxt->hysteresis) :
                  ((uint32_t)ptrWdCxt->fingerTh - ptrWdCxt->hysteresis);

        if (0u < (*ptrDebounceCnt))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        /* No touch */
        if (ptrSnsCxt->diff <= touchTh)
        {
            /* Reset debounce counter */
            *ptrDebounceCnt = ptrWdCxt->onDebounce;
            ptrSnsCxt->status = 0u;
        }

        /* New touch or touch still exists */
        if (0u == (*ptrDebounceCnt))
        {
            ptrSnsCxt->status = CY_CAPSENSE_SNS_TOUCH_STATUS_MASK;
            activeCount++;
            startIndex = snsIndex;
        }

        /* Update widget status */
        if (0u != ptrSnsCxt->status)
        {
            ptrWdCxt->status |= (uint8_t)CY_CAPSENSE_WD_ACTIVE_MASK;
        }

        ptrSnsCxt++;
        ptrDebounceCnt++;
    }
    /* Update position struct */
    if (((uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrWdConfig->wdType) &&
        ((uint8_t)CY_CAPSENSE_SENSE_METHOD_CSX_E == ptrWdConfig->senseMethod))
    {
        ptrWdCxt->wdTouch.numPosition = (uint8_t)activeCount;
        ptrWdCxt->wdTouch.ptrPosition->id = (uint16_t)startIndex;
        ptrWdCxt->wdTouch.ptrPosition->x = (uint16_t)(startIndex / ptrWdConfig->numRows);
        ptrWdCxt->wdTouch.ptrPosition->y = (uint16_t)(startIndex % ptrWdConfig->numRows);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessProximity
****************************************************************************//**
*
* Processes the status of the Proximity widget.
*
* This function processes the status of the CSD Proximity widget. It applies the
* hysteresis and debounce algorithm to each sensor difference value.
* The proximity and touch events are considered independently so debounce and
* hysteresis are also applied independently.
*
* This function is expected to be called after each new widget scan. If it is
* called multiple times for the same scan data the debounce algorithm
* will not work properly.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
*******************************************************************************/
void Cy_CapSense_DpProcessProximity(const cy_stc_capsense_widget_config_t * ptrWdConfig)
{
    uint32_t snsTh;
    uint32_t snsIndex;
    uint32_t snsStMask;

    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    /* Reset widget status */
    ptrWdCxt->status &= (uint8_t)~CY_CAPSENSE_WD_ACTIVE_MASK;

    /* Go through all sensor's status bits */
    for (snsIndex = 0u; snsIndex < ((uint32_t)ptrWdConfig->numSns << 1u); snsIndex++)
    {
        /*
        * Proximity - odd; Touch - even. Example:
        * Bit 0 -> touch event
        * Bit 1 -> proximity event
        */
        snsTh = ptrWdCxt->proxTh;
        snsStMask = CY_CAPSENSE_SNS_PROX_STATUS_MASK;
        if (0u == (snsIndex & 0x01u))
        {
            snsTh = ptrWdCxt->fingerTh;
            snsStMask = CY_CAPSENSE_SNS_TOUCH_STATUS_MASK;
        }
        /* Calculate threshold based on current sensor state */
        snsTh = (0u == (snsStMask & ptrSnsCxt->status)) ?
                  (snsTh + ptrWdCxt->hysteresis) :
                  (snsTh - ptrWdCxt->hysteresis);

        if (0u < (*ptrDebounceCnt))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        /* No touch */
        if (ptrSnsCxt->diff <= snsTh)
        {
            /* Reset debounce counter */
            *ptrDebounceCnt = ptrWdCxt->onDebounce;
            ptrSnsCxt->status &= (uint8_t)(~snsStMask);
        }

        /* New touch or touch still exists */
        if (0u == (*ptrDebounceCnt))
        {
            ptrSnsCxt->status |= (uint8_t)snsStMask;
        }

        /* Update widget status */
        if (0u != (ptrSnsCxt->status & (uint8_t)snsStMask))
        {
            ptrWdCxt->status |= (uint8_t)CY_CAPSENSE_WD_ACTIVE_MASK;
        }

        if (0u != (snsIndex & 0x01u))
        {
            ptrSnsCxt++;
        }
        ptrDebounceCnt++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessSlider
****************************************************************************//**
*
* Processes the status of the Linear/Radial Slider widget.
* The process involves running the Linear/Radial centroid algorithm.
*
* It applies the hysteresis and debounce algorithm to the widget ignoring
* the individual states of the sensors.
*
* This function is expected to be called after each new widget scan. If it is
* called multiple times for the same scan data, the debounce algorithm
* will not work properly.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessSlider(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t touchTh;
    uint32_t wdActive = 0u;
    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;
    uint32_t sensorNumber = ptrWdConfig->numSns;
    cy_stc_capsense_position_t newPosition[CY_CAPSENSE_MAX_CENTROIDS];
    cy_stc_capsense_touch_t newTouch = {&newPosition[0u], CY_CAPSENSE_POSITION_NONE};

    /* Calculate touch threshold based on current slider state */
    touchTh = (0u == (ptrWdCxt->status & CY_CAPSENSE_WD_ACTIVE_MASK)) ?
              ((uint32_t)ptrWdCxt->fingerTh + ptrWdCxt->hysteresis) :
              ((uint32_t)ptrWdCxt->fingerTh - ptrWdCxt->hysteresis);

    if (0u < (*ptrDebounceCnt))
    {
        /* Decrement debounce counter */
        (*ptrDebounceCnt)--;
    }

    /* Check new widget activity */
    for (snsIndex = sensorNumber; snsIndex-- > 0u;)
    {
        ptrSnsCxt->status = (touchTh < ptrSnsCxt->diff) ? CY_CAPSENSE_SNS_TOUCH_STATUS_MASK : 0u;
        wdActive |= ptrSnsCxt->status;
        ptrSnsCxt++;
    }

    /* No touch detected */
    if (0u == wdActive)
    {
        /* Reset debounce counter */
        (*ptrDebounceCnt) = ptrWdCxt->onDebounce;
        ptrWdCxt->status &= (uint8_t)(~CY_CAPSENSE_WD_ACTIVE_MASK);
    }

    if (0u == (*ptrDebounceCnt))
    {
        /* New touch detected or touch still exists from previous cycle */
        ptrWdCxt->status |= CY_CAPSENSE_WD_ACTIVE_MASK;
    }
    else
    {
        if (0u != wdActive)
        {
            /* Clear sensor state if activity was detected but debounce was not passed */
            ptrSnsCxt = ptrWdConfig->ptrSnsContext;

            for (snsIndex = sensorNumber; snsIndex-- > 0u;)
            {
                ptrSnsCxt->status = 0u;
                ptrSnsCxt++;
            }
        }
    }

    /* Centroid processing */
    if (CY_CAPSENSE_WD_ACTIVE_MASK == (ptrWdCxt->status & CY_CAPSENSE_WD_ACTIVE_MASK))
    {
        switch (ptrWdConfig->wdType)
        {
            case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
                Cy_CapSense_DpCentroidRadial(&newTouch, ptrWdConfig);
                break;

            case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
                if ((0u != (ptrWdConfig->centroidConfig & CY_CAPSENSE_DIPLEXING_MASK)))
                {
                    /* Run local maximum search for diplexed slider */
                    Cy_CapSense_DpCentroidDiplex(&newTouch, ptrWdConfig);
                }
                else
                {
                    Cy_CapSense_DpCentroidLinear(&newTouch, ptrWdConfig);
                }
                break;

            default:
                break;
        }
    }

    /* Position filtering */
    if (0u != (ptrWdConfig->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_MASK))
    {
        Cy_CapSense_ProcessPositionFilters_Call(&newTouch, ptrWdConfig, context);
    }
    /* Copy positions into public structure */
    ptrWdConfig->ptrWdContext->wdTouch.numPosition = newTouch.numPosition;
    for (snsIndex = 0u; snsIndex < newTouch.numPosition; snsIndex++)
    {
        ptrWdConfig->ptrWdContext->wdTouch.ptrPosition[snsIndex] = newTouch.ptrPosition[snsIndex];
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdMatrix
****************************************************************************//**
*
* Processes the status of the CSD Matrix Button widget.
*
* This function processes the status of the CSD Matrix Button widget.
* It applies the hysteresis and debounce algorithm to each sensor value.
*
* Then the function analyzes how many row and column sensors are active. 
* If only one per row and one per column, the function considers this as 
* a valid touch and updates the corresponding Data Structure registers 
* with the button id and active row and column sensors.
*
* If multiple sensors are active in row or column sensors, this function sets
* the corresponding registers to the CY_CAPSENSE_POSITION_MULTIPLE
* value that indicates that it is not possible to detect the touched button id.
*
* This function is expected to be called after each new widget scan. If it is
* called multiple times for the same scan data, the debounce algorithm
* will not work properly.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdMatrix(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t touchTh;
    uint32_t colNumber = ptrWdConfig->numCols;
    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;
    cy_stc_capsense_position_t * ptrSnsTouch = ptrWdCxt->wdTouch.ptrPosition;

    uint32_t numActiveRows = 0u;
    uint32_t numActiveCols = 0u;

    uint32_t activeRow = 0u;
    uint32_t activeCol = 0u;

    /* Reset status */
    ptrWdCxt->status &= (uint8_t)~CY_CAPSENSE_WD_ACTIVE_MASK;

    /* Go through all widget's sensors */
    for (snsIndex = 0u; snsIndex < ptrWdConfig->numSns; snsIndex++)
    {
        /* Calculate touch threshold based on current sensor state */
        touchTh = (0u == ptrSnsCxt->status) ?
                  ((uint32_t)ptrWdCxt->fingerTh + ptrWdCxt->hysteresis) :
                  ((uint32_t)ptrWdCxt->fingerTh - ptrWdCxt->hysteresis);

        if (0u < (*ptrDebounceCnt))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        /* No touch */
        if (ptrSnsCxt->diff <= touchTh)
        {
            /* Reset debounce counter */
            *ptrDebounceCnt = ptrWdCxt->onDebounce;
            ptrSnsCxt->status = 0u;
        }

        /* New touch or touch still exists */
        if (0u == (*ptrDebounceCnt))
        {
            ptrSnsCxt->status = CY_CAPSENSE_SNS_TOUCH_STATUS_MASK;
        }

        /* Update information about active row/col sensors */
        if (0u != ptrSnsCxt->status)
        {
            if (snsIndex < colNumber)
            {
                numActiveCols++;
                activeCol = snsIndex;
            }
            else
            {
                numActiveRows++;
                activeRow = snsIndex - colNumber;
            }
        }

        ptrSnsCxt++;
        ptrDebounceCnt++;
    }

    /*
    * Number of touches (numActiveCols * numActiveRows):
    * 0 -> No touch
    * 1 -> One touch
    * 2+ -> Multiple touches
    */

    ptrWdCxt->wdTouch.numPosition = (uint8_t)(numActiveCols * numActiveRows);
    if (ptrWdCxt->wdTouch.numPosition > CY_CAPSENSE_POSITION_ONE)
    {
        ptrWdCxt->wdTouch.numPosition = CY_CAPSENSE_POSITION_MULTIPLE;
    }

    if (CY_CAPSENSE_POSITION_ONE == ptrWdCxt->wdTouch.numPosition)
    {
        ptrSnsTouch->x = (uint16_t)activeCol;
        ptrSnsTouch->y = (uint16_t)activeRow;
        ptrSnsTouch->z = 0u;
        ptrSnsTouch->id = (uint16_t)((activeRow * colNumber) + activeCol);
    }

    /* Update widget status if any activity is detected (even non-valid) */
    if ((0u != numActiveRows) || (0u != numActiveCols))
    {
        ptrWdCxt->status |= (uint8_t)CY_CAPSENSE_WD_ACTIVE_MASK;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdTouchpad
****************************************************************************//**
*
* Processes status of the CSD Touchpad widget. This includes running
* Centroid algorithm and updating status in the Data Structure registers.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdTouchpad(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t touchTh;
    uint32_t wdActiveCol = 0uL;
    uint32_t wdActiveRow = 0uL;
    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;
    uint32_t sensorNumber = ptrWdConfig->numSns;
    uint32_t colNumber = ptrWdConfig->numCols;
    uint32_t rowNumber = ptrWdConfig->numRows;
    cy_stc_capsense_position_t newPosition[CY_CAPSENSE_MAX_CENTROIDS];
    cy_stc_capsense_touch_t newTouch = {&newPosition[0u], CY_CAPSENSE_POSITION_NONE};
    cy_stc_capsense_ballistic_delta_t delta;

    /* Calculate touch threshold based on current sensor state */
    touchTh = (0u == (ptrWdCxt->status & CY_CAPSENSE_WD_ACTIVE_MASK)) ?
              ((uint32_t)ptrWdCxt->fingerTh + ptrWdCxt->hysteresis) :
              ((uint32_t)ptrWdCxt->fingerTh - ptrWdCxt->hysteresis);

    if (0u < (*ptrDebounceCnt))
    {
        /* Decrement debounce counter */
        (*ptrDebounceCnt)--;
    }

    /* Widget is considered as active if at least one sensor is active on both axes */
    for (snsIndex = colNumber; snsIndex-- > 0uL;)
    {
        ptrSnsCxt->status = (touchTh < ptrSnsCxt->diff) ? CY_CAPSENSE_SNS_TOUCH_STATUS_MASK : 0u;
        wdActiveCol |= ptrSnsCxt->status;
        ptrSnsCxt++;
    }
    for (snsIndex = rowNumber; snsIndex-- > 0uL;)
    {
        ptrSnsCxt->status = (touchTh < ptrSnsCxt->diff) ? CY_CAPSENSE_SNS_TOUCH_STATUS_MASK : 0u;
        wdActiveRow |= ptrSnsCxt->status;
        ptrSnsCxt++;
    }

    /* No touch detected */
    if ((0uL == wdActiveCol) || (0uL == wdActiveRow))
    {
        /* Reset debounce counter */
        (*ptrDebounceCnt) = ptrWdCxt->onDebounce;
        ptrWdCxt->status &= (uint8_t)(~CY_CAPSENSE_WD_ACTIVE_MASK);
    }
    if (0u == (*ptrDebounceCnt))
    {
        /* New touch detected or touch still exists from previous cycle */
        ptrWdCxt->status |= CY_CAPSENSE_WD_ACTIVE_MASK;
    }
    else
    {
        if ((0uL != wdActiveCol) && (0uL != wdActiveRow))
        {
            /* Clear sensor state if activity was detected but debounce was not passed */
            ptrSnsCxt = ptrWdConfig->ptrSnsContext;
            for (snsIndex = sensorNumber; snsIndex-- > 0uL;)
            {
                ptrSnsCxt->status = 0u;
                ptrSnsCxt++;
            }
        }
    }

    /* If widget is still active after debounce run the centroid algorithm */
    if (CY_CAPSENSE_WD_ACTIVE_MASK == (ptrWdCxt->status & CY_CAPSENSE_WD_ACTIVE_MASK))
    {
        /* 3x3 CSD Touchpad processing */
        if (0u != (ptrWdConfig->centroidConfig & CY_CAPSENSE_CENTROID_3X3_MASK))
        {
            /* Centroid processing */
            Cy_CapSense_DpCentroidTouchpad(&newTouch, ptrWdConfig);
        }
        /* 5x5 Advanced CSD Touchpad processing */
        if (0u != (ptrWdConfig->centroidConfig & CY_CAPSENSE_CENTROID_5X5_MASK))
        {
            /* Centroid processing */
            Cy_CapSense_DpAdvancedCentroidTouchpad_Call(&newTouch, ptrWdConfig, context);
        }
    }

    /* Position filtering */
    if (0u != (ptrWdConfig->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_MASK))
    {
        Cy_CapSense_ProcessPositionFilters_Call(&newTouch, ptrWdConfig, context);
    }

    /* Copy positions into public structure */
    ptrWdCxt->wdTouch.numPosition = newTouch.numPosition;
    if (CY_CAPSENSE_POSITION_MULTIPLE != ptrWdCxt->wdTouch.numPosition)
    {
        for (snsIndex = 0u; snsIndex < newTouch.numPosition; snsIndex++)
        {
            ptrWdCxt->wdTouch.ptrPosition[snsIndex] = newTouch.ptrPosition[snsIndex];
        }
    }

    /* Ballistic Multiplier Filtering */
    if (0u != (ptrWdConfig->centroidConfig & CY_CAPSENSE_CENTROID_BALLISTIC_MASK))
    {
        Cy_CapSense_BallisticMultiplier_Lib_Call(&ptrWdConfig->ballisticConfig,
                                                 &ptrWdCxt->wdTouch,
                                                 &delta,
                                                 context->ptrCommonContext->timestamp,
                                                 ptrWdConfig->ptrBallisticContext,
                                                 context);
        ptrWdCxt->xDelta = delta.deltaX;
        ptrWdCxt->yDelta = delta.deltaY;
    }

}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxTouchpad
****************************************************************************//**
*
* Processes the status of the CSX Touchpad widget.
* The process involves running the 3x3 centroid algorithm with the 
* tracking of finger id.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsxTouchpad(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    (void)context;
    /* Check whether sensors are active and located all local maxima */
    Cy_CapSense_DpFindLocalMaxDd(ptrWdConfig);
    /* Calculate centroid position for all found local maxima */
    Cy_CapSense_DpCalcTouchPadCentroid(ptrWdConfig);
    /* Identify all touches and assign them correct ID based on historical data */
    Cy_CapSense_DpTouchTracking(ptrWdConfig);
    /* Filter the position data and write it into data structure */
    Cy_CapSense_DpFilterTouchRecord(ptrWdConfig, context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_RunMfsFiltering
****************************************************************************//**
*
* Selects the median difference signal when the multi-frequency scan is enabled.
*
* \param ptrSnsContext
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_RunMfsFiltering(
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                const cy_stc_capsense_context_t * context)
{
    ptrSnsContext->diff = (uint16_t)Cy_CapSense_FtMedian((uint32_t)ptrSnsContext->diff,
                  (uint32_t)ptrSnsContext[CY_CAPSENSE_MFS_CH1_INDEX * context->ptrCommonConfig->numSns].diff,
                  (uint32_t)ptrSnsContext[CY_CAPSENSE_MFS_CH2_INDEX * context->ptrCommonConfig->numSns].diff);
}

#endif /* CY_IP_MXCSDV2 */


/* [] END OF FILE */
