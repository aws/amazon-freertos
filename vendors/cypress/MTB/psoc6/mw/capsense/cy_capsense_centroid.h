/***************************************************************************//**
* \file cy_capsense_centroid.h
* \version 2.0
*
* \brief
* This file provides the function prototypes for the centroid calculation
* methods.
*
********************************************************************************
* \copyright
* Copyright 2018-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_CAPSENSE_CENTROID_H)
#define CY_CAPSENSE_CENTROID_H

#include "cy_capsense_lib.h"
#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"

#if defined(CY_IP_MXCSDV2)

#if defined(__cplusplus)
extern "C" {
#endif


/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/*******************************************************************************
* Function Prototypes - internal functions
*******************************************************************************/

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/

void Cy_CapSense_DpCentroidLinear(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig);
void Cy_CapSense_DpCentroidRadial(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig);
void Cy_CapSense_DpCentroidDiplex(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig);
void Cy_CapSense_DpCentroidTouchpad(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig);
void Cy_CapSense_DpAdvancedCentroidTouchpad(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig);
void Cy_CapSense_DpFindLocalMaxDd(
                const cy_stc_capsense_widget_config_t * ptrWdConfig);
void Cy_CapSense_DpCalcTouchPadCentroid(
                const cy_stc_capsense_widget_config_t * ptrWdConfig);
void Cy_CapSense_DpTouchTracking(
                const cy_stc_capsense_widget_config_t * ptrWdConfig);
void Cy_CapSense_DpFilterTouchRecord(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InitPositionFilters(
                uint32_t filterConfig,
                const cy_stc_capsense_position_t * ptrInput, 
                cy_stc_capsense_position_t * ptrHistory);
void Cy_CapSense_RunPositionFilters(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_position_t * ptrInput, 
                cy_stc_capsense_position_t * ptrHistory,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_RunPositionFiltersRadial(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_position_t * ptrInput, 
                cy_stc_capsense_position_t * ptrHistory,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_ProcessPositionFilters(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);

/** \} \endcond */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCSDV2 */

#endif /* CY_CAPSENSE_CENTROID_H */


/* [] END OF FILE */
