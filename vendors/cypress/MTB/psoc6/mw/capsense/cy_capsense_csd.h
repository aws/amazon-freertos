/***************************************************************************//**
* \file cy_capsense_csd.h
* \version 2.0
*
* \brief
* This file provides the function prototypes specific to the CSD sensing
* method implementation.
*
********************************************************************************
* \copyright
* Copyright 2018-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_CAPSENSE_CSD_H)
#define CY_CAPSENSE_CSD_H

#include "cy_syslib.h"
#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"

#if defined(CY_IP_MXCSDV2)

#if defined(__cplusplus)
extern "C" {
#endif


/****************************************************************************
* Register and mode mask definition
****************************************************************************/
#define CY_CAPSENSE_DEFAULT_CSD_SW_DSI_SEL                      (0x00000000uL)
#define CY_CAPSENSE_DEFAULT_CSD_INTR_SET                        (0x00000000uL)
#define CY_CAPSENSE_DEFAULT_SW_HS_N_SEL                         (0x00000000uL)
#define CY_CAPSENSE_DEFAULT_CSD_ADC_CTL                         (0x00000000uL)

/* SW_FW_MOD_SEL switches states */
#define CY_CAPSENSE_CSD_SW_FW_MOD_SEL_SCAN                      (0x00000000uL)
#define CY_CAPSENSE_CSD_SW_FW_MOD_SEL_INIT                      (0x00000000uL)

#define CY_CAPSENSE_EXT_CAP_DISCHARGE_TIME                      (1u)

/***************************************
* Function Prototypes
**************************************/

/******************************************************************************/
/** \addtogroup group_capsense_low_level *//** \{ */
/******************************************************************************/

cy_status Cy_CapSense_CSDCalibrateWidget(uint32_t widgetId, uint32_t target, cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDSetupWidget(uint32_t widgetId, cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDSetupWidgetExt(uint32_t widgetId, uint32_t sensorId, cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDScan(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDScanExt(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDConnectSns(const cy_stc_capsense_pin_config_t * snsAddrPtr, cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDDisconnectSns(const cy_stc_capsense_pin_config_t * snsAddrPtr, const cy_stc_capsense_context_t * context);

/** \} */

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/

void Cy_CapSense_CSDDisableMode(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDInitialize(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDStartSample(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDDisableShieldElectrodes(const cy_stc_capsense_context_t * context);

uint32_t Cy_CapSense_CSDGetNumberOfConversions(uint32_t snsClkDivider, uint32_t resolution, uint32_t snsClkSrc);
void Cy_CapSense_CSDSetUpIdacs(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDSnsStateCheck(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDCalculateScanDuration(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDConnectSnsExt(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDDisconnectSnsExt(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDConfigClock(cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDClearSensors(const cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDSetWidgetSenseClkSrc(const cy_stc_capsense_widget_config_t * ptrWdConfig);
uint32_t Cy_CapSense_CSDCalcPrsSize(uint32_t snsClkDivider, uint32_t resolution);
void Cy_CapSense_CSDScanISR(void * capsenseContext);

/** \} \endcond */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCSDV2 */

#endif /* CY_CAPSENSE_CSD_H */


/* [] END OF FILE */
