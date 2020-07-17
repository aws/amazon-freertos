/***************************************************************************//**
* \file cy_capsense_common.h
* \version 2.0
*
* \brief
* This file provides the common CapSense definitions.
*
********************************************************************************
* \copyright
* Copyright 2018-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_CAPSENSE_COMMON_H)
#define CY_CAPSENSE_COMMON_H

#include "cy_device_headers.h"
#include "cy_sysint.h"

#if defined(CY_IP_MXCSDV2)

#if defined(__cplusplus)
extern "C" {
#endif


/*******************************************************************************
* Macros
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_capsense_macros_general *//** \{ */
/******************************************************************************/
/** Middleware major version */
#define CY_CAPSENSE_MW_VERSION_MAJOR                   (2)
/** Middleware minor version */
#define CY_CAPSENSE_MW_VERSION_MINOR                   (0)
/** Middleware ID */
#define CY_CAPSENSE_ID                                 (CY_PDL_DRV_ID(0x07uL))


/* Scanning status */
/** The CapSense middleware is busy */
#define CY_CAPSENSE_BUSY                                (0x80u)
/** The CapSense middleware is not busy */
#define CY_CAPSENSE_NOT_BUSY                            (0x00u)

/** Feature enabled */
#define CY_CAPSENSE_ENABLE                              (1u)
/** Feature disabled */
#define CY_CAPSENSE_DISABLE                             (0u)

/** Sensor active status mask */
#define CY_CAPSENSE_SNS_TOUCH_STATUS_MASK               (0x01u)
/** Proximity active status mask */
#define CY_CAPSENSE_SNS_PROX_STATUS_MASK                (0x02u)
/** Widget active status mask */
#define CY_CAPSENSE_WD_ACTIVE_MASK                      (0x01u)
/** Widget disable status mask */
#define CY_CAPSENSE_WD_DISABLE_MASK                     (0x02u)
/** Widget working status mask */
#define CY_CAPSENSE_WD_WORKING_MASK                     (0x04u)
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros_settings *//** \{ */
/******************************************************************************/
/* Sensing methods */
/** CSD Sensing method */
#define CY_CAPSENSE_CSD_SENSING_METHOD                  (0u)
/** CSX Sensing method */
#define CY_CAPSENSE_CSX_SENSING_METHOD                  (1u)

/* SmartSense modes */
/** Manual tunning mode */
#define CY_CAPSENSE_CSD_SS_DIS                          (0x00uL)
/** Hardware auto-tune mask */
#define CY_CAPSENSE_CSD_SS_HW_EN                        (0x01uL)
/** Threshold auto-tune mask */
#define CY_CAPSENSE_CSD_SS_TH_EN                        (0x02uL)
/** Full auto-tune is enabled */
#define CY_CAPSENSE_CSD_SS_HWTH_EN                      (CY_CAPSENSE_CSD_SS_HW_EN | CY_CAPSENSE_CSD_SS_TH_EN)

/* MFS macros */
/** Number of multi-frequency scan channels */
#define CY_CAPSENSE_FREQ_CHANNELS_NUM                   (3u)
/** Multi-frequency channel 0 constant */
#define CY_CAPSENSE_MFS_CH0_INDEX                       (0u)
/** Multi-frequency channel 1 constant */
#define CY_CAPSENSE_MFS_CH1_INDEX                       (1u)
/** Multi-frequency channel 2 constant */
#define CY_CAPSENSE_MFS_CH2_INDEX                       (2u)

/* Inactive sensor connection options */
/** Inactive sensor connection to ground */
#define CY_CAPSENSE_SNS_CONNECTION_GROUND               (1u)
/** Inactive sensor connection to High-Z */
#define CY_CAPSENSE_SNS_CONNECTION_HIGHZ                (2u)
/** Inactive sensor connection to shield */
#define CY_CAPSENSE_SNS_CONNECTION_SHIELD               (3u)

/* Shield electrode delay macros */
/** No shield signal delay */
#define CY_CAPSENSE_SH_DELAY_0NS                        (0u)
/** Shield signal delay > 5 ns */
#define CY_CAPSENSE_SH_DELAY_5NS                        (1u)
/** Shield signal delay > 10 ns */
#define CY_CAPSENSE_SH_DELAY_10NS                       (2u)
/** Shield signal delay > 20 ns */
#define CY_CAPSENSE_SH_DELAY_20NS                       (3u)

/* Idac sensing configuration */
/** Idac sourcing */
#define CY_CAPSENSE_IDAC_SOURCING                       (0u)
/** Idac sinking */
#define CY_CAPSENSE_IDAC_SINKING                        (1u)

/* Shield tank capacitor pre-charge source options */
/** Shield tank capacitor pre-charge from Vref */
#define CY_CAPSENSE_CSH_PRECHARGE_VREF                  (0u)
/** Shield tank capacitor pre-charge from IO buffer */
#define CY_CAPSENSE_CSH_PRECHARGE_IO_BUF                (1u)

/* Sense clock selection options */
/** Auto-mode of clock source selection mask */
#define CY_CAPSENSE_CLK_SOURCE_AUTO_MASK                (0x80u)
/** Clock source direct */
#define CY_CAPSENSE_CLK_SOURCE_DIRECT                   (0x00u)
/** Clock source SSC6 */
#define CY_CAPSENSE_CLK_SOURCE_SSC6                     (0x01u)
/** Clock source SSC7 */
#define CY_CAPSENSE_CLK_SOURCE_SSC7                     (0x02u)
/** Clock source SSC9 */
#define CY_CAPSENSE_CLK_SOURCE_SSC9                     (0x03u)
/** Clock source SSC10 */
#define CY_CAPSENSE_CLK_SOURCE_SSC10                     (0x04u)
/** Clock source PRS8 */
#define CY_CAPSENSE_CLK_SOURCE_PRS8                     (0x05u)
/** Clock source PRS12 */
#define CY_CAPSENSE_CLK_SOURCE_PRS12                    (0x06u)

/* Initialization switch resistance options */
/** Low switch resistance at initialization */
#define CY_CAPSENSE_INIT_SW_RES_LOW                     (0x00000000uL)
/** Medium switch resistance at initialization */
#define CY_CAPSENSE_INIT_SW_RES_MEDIUM                  (0x00000001uL)
/** High switch resistance at initialization */
#define CY_CAPSENSE_INIT_SW_RES_HIGH                    (0x00000002uL)

/* Scanning switch resistance options */
/** Low switch resistance at scanning */
#define CY_CAPSENSE_SCAN_SW_RES_LOW                     (0x00000000uL)
/** Medium switch resistance at scanning */
#define CY_CAPSENSE_SCAN_SW_RES_MEDIUM                  (0x00000001uL)
/** High switch resistance at scanning */
#define CY_CAPSENSE_SCAN_SW_RES_HIGH                    (0x00000002uL)

/* CSD shield switch resistance options */
/** Low shield switch resistance */
#define CY_CAPSENSE_SHIELD_SW_RES_LOW                   (0x00000000uL)
/** Medium shield switch resistance */
#define CY_CAPSENSE_SHIELD_SW_RES_MEDIUM                (0x00000001uL)
/** High shield switch resistance */
#define CY_CAPSENSE_SHIELD_SW_RES_HIGH                  (0x00000002uL)
/** Low-EMI shield switch resistance */
#define CY_CAPSENSE_SHIELD_SW_RES_LOW_EMI               (0x00000003uL)

/* Vref source options */
/** Vref source is taken from SRSS */
#define CY_CAPSENSE_VREF_SRSS                           (0x00000000uL)
/** Vref source is taken from PASS */
#define CY_CAPSENSE_VREF_PASS                           (0x00000001uL)

/* Iref source options */
/** Iref source is taken from SRSS */
#define CY_CAPSENSE_IREF_SRSS                           (0x00000000uL)
/** Iref source is taken from PASS */
#define CY_CAPSENSE_IREF_PASS                           (0x00000001uL)

/* Position Filter Configuration */
/** Mask of all filters enabling */
#define CY_CAPSENSE_POSITION_FILTERS_MASK               (0x000000FFu)
/** Median position filter enable mask */
#define CY_CAPSENSE_POSITION_MED_MASK                   (0x00000001u)
/** IIR position filter enable mask */
#define CY_CAPSENSE_POSITION_IIR_MASK                   (0x00000002u)
/** Adaptive IIR position filter enable mask */
#define CY_CAPSENSE_POSITION_AIIR_MASK                  (0x00000004u)
/** Average position filter enable mask */
#define CY_CAPSENSE_POSITION_AVG_MASK                   (0x00000008u)
/** Jitter position filter enable mask */
#define CY_CAPSENSE_POSITION_JIT_MASK                   (0x00000010u)
/** Mask of position filters history size */
#define CY_CAPSENSE_POSITION_FILTERS_SIZE_MASK          (0x0000FF00u)
/** Offset of position filters history size */
#define CY_CAPSENSE_POSITION_FILTERS_SIZE_OFFSET        (8u)
/** Mask of IIR coefficient of position filter */
#define CY_CAPSENSE_POSITION_IIR_COEFF_MASK             (0x00FF0000u)
/** Offset of IIR coefficient of position filter */
#define CY_CAPSENSE_POSITION_IIR_COEFF_OFFSET           (16u)

/* Raw count filter macros */
/** Offset of raw count filter history size */
#define CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_OFFSET   (0u)
/** Offset of raw count median filter enable mask */
#define CY_CAPSENSE_RC_FILTER_MEDIAN_EN_OFFSET          (4u)
/** Offset of raw count median filter mode mask */
#define CY_CAPSENSE_RC_FILTER_MEDIAN_MODE_OFFSET        (5u)
/** Offset of raw count IIR filter enable mask */
#define CY_CAPSENSE_RC_FILTER_IIR_EN_OFFSET             (7u)
/** Offset of raw count IIR filter mode mask */
#define CY_CAPSENSE_RC_FILTER_IIR_MODE_OFFSET           (8u)
/** Offset of raw count average filter enable mask */
#define CY_CAPSENSE_RC_FILTER_AVERAGE_EN_OFFSET         (10u)
/** Offset of raw count average filter mode mask */
#define CY_CAPSENSE_RC_FILTER_AVERAGE_MODE_OFFSET       (11u)

/** Mask of raw count filter history size */
#define CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_MASK     ((uint16_t)((uint16_t)0x000Fu << CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_OFFSET))
/** Median raw count filter enable mask */
#define CY_CAPSENSE_RC_FILTER_MEDIAN_EN_MASK            ((uint16_t)((uint16_t)0x0001u << CY_CAPSENSE_RC_FILTER_MEDIAN_EN_OFFSET))
/** Median raw count filter mode mask */
#define CY_CAPSENSE_RC_FILTER_MEDIAN_MODE_MASK          ((uint16_t)((uint16_t)0x0003u << CY_CAPSENSE_RC_FILTER_MEDIAN_MODE_OFFSET))
/** IIR raw count filter enable mask */
#define CY_CAPSENSE_RC_FILTER_IIR_EN_MASK               ((uint16_t)((uint16_t)0x0001u << CY_CAPSENSE_RC_FILTER_IIR_EN_OFFSET))
/** IIR raw count filter mode mask */
#define CY_CAPSENSE_RC_FILTER_IIR_MODE_MASK             ((uint16_t)((uint16_t)0x0003u << CY_CAPSENSE_RC_FILTER_IIR_MODE_OFFSET))
/** Average raw count filter enable mask */
#define CY_CAPSENSE_RC_FILTER_AVERAGE_EN_MASK           ((uint16_t)((uint16_t)0x0001u << CY_CAPSENSE_RC_FILTER_AVERAGE_EN_OFFSET))
/** Average raw count filter mode mask */
#define CY_CAPSENSE_RC_FILTER_AVERAGE_MODE_MASK         ((uint16_t)((uint16_t)0x0003u << CY_CAPSENSE_RC_FILTER_AVERAGE_MODE_OFFSET))
/** All raw count filters enable mask */
#define CY_CAPSENSE_RC_FILTER_ALL_EN_MASK               (CY_CAPSENSE_RC_FILTER_MEDIAN_EN_MASK |\
                                                         CY_CAPSENSE_RC_FILTER_IIR_EN_MASK |\
                                                         CY_CAPSENSE_RC_FILTER_AVERAGE_EN_OFFSET)
/** Raw count IIR filter mode standard */
#define CY_CAPSENSE_IIR_FILTER_STANDARD                 ((uint16_t)((uint16_t)1u << CY_CAPSENSE_RC_FILTER_IIR_MODE_OFFSET))
/** Raw count IIR filter mode performance */
#define CY_CAPSENSE_IIR_FILTER_PERFORMANCE              ((uint16_t)((uint16_t)2u << CY_CAPSENSE_RC_FILTER_IIR_MODE_OFFSET))
/** Raw count average filter mode */
#define CY_CAPSENSE_AVERAGE_FILTER_LEN_4                ((uint16_t)((uint16_t)2u << CY_CAPSENSE_RC_FILTER_AVERAGE_MODE_OFFSET))
/** \} */

/* Centroid configuration */
/** Supported touch count mask */
#define CY_CAPSENSE_CENTROID_NUMBER_MASK                (0x0003u)
/** Diplex enable mask */
#define CY_CAPSENSE_DIPLEXING_MASK                      (0x0004u)
/** Ballistic multiplier enable mask */
#define CY_CAPSENSE_CENTROID_BALLISTIC_MASK             (0x0008u)
/** 3x3 centroid enable mask */
#define CY_CAPSENSE_CENTROID_3X3_MASK                   (0x0020u)
/** 5x5 centroid enable mask */
#define CY_CAPSENSE_CENTROID_5X5_MASK                   (0x0040u)
/** Edge correction enable mask */
#define CY_CAPSENSE_EDGE_CORRECTION_MASK                (0x0080u)
/** 
* Centroid calculation method mask
* * Direct provides correct calculation but worse linearity on edges 
*   and requires edge correction [MaxPosition / (numSns)]
* * Approximate provides better linearity on edges that leads to
*   worse accuracy in general [MaxPosition / (numSns - 1u)]
*/
#define CY_CAPSENSE_CALC_METHOD_MASK                    (0x0100u)

/******************************************************************************/
/** \addtogroup group_capsense_macros_touch *//** \{ */
/******************************************************************************/
/** No touch detected */
#define CY_CAPSENSE_POSITION_NONE                       (0x00u)
/** One finger touch detected */
#define CY_CAPSENSE_POSITION_ONE                        (0x01u)
/** Two finger touches detected */
#define CY_CAPSENSE_POSITION_TWO                        (0x02u)
/** Three finger touches detected */
#define CY_CAPSENSE_POSITION_THREE                      (0x03u)
/** Multiple touches detected that exceed number of supported touches */
#define CY_CAPSENSE_POSITION_MULTIPLE                   (0xFFu)
/** Maximum possible centroid number */
#define CY_CAPSENSE_MAX_CENTROIDS                       (3u)
/** Maximum number of peaks in CSX Touchpad */
#define CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS              (5u)
/** Maximum age of a touch */
#define CY_CAPSENSE_CSX_TOUCHPAD_MAX_AGE                (0xFFu)
/** Undefined ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_UNDEFINED           (0x80u)
/** Non-assigned ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_ABSENT              (0x40u)
/** Touch below threshold ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_ON_FAIL             (0x20u)
/** Minimum ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_MIN                 (0u)
/** Maximum ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_MAX                 (7u)
/** ID mask */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_MASK                (0x00FFu)
/** Touch power mask */
#define CY_CAPSENSE_CSX_TOUCHPAD_Z_MASK                 (0x00FFu)
/** Age mask */
#define CY_CAPSENSE_CSX_TOUCHPAD_AGE_MASK               (0xFF00u)
/** Debounce mask */
#define CY_CAPSENSE_CSX_TOUCHPAD_DEBOUNCE_MASK          (0xFF00u)
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros_process *//** \{ */
/******************************************************************************/
/** Applies all enabled filters in the default order to the raw counts */
#define CY_CAPSENSE_PROCESS_FILTER                      (0x01u)
/** Updates baselines using current raw count values for the widget/sensor */
#define CY_CAPSENSE_PROCESS_BASELINE                    (0x02u)
/** Calculates differences for the widget/sensor */
#define CY_CAPSENSE_PROCESS_DIFFCOUNTS                  (0x04u)
/** Runs the noise envelope filter to measure noise magnitude for the widget/sensor */
#define CY_CAPSENSE_PROCESS_CALC_NOISE                  (0x08u)
/** Updates widget thresholds based on raw counts noise magnitude */
#define CY_CAPSENSE_PROCESS_THRESHOLDS                  (0x10u)
/** Runs the widget-specific processing algorithms and updates it status */
#define CY_CAPSENSE_PROCESS_STATUS                      (0x20u)
/** Definition that combines all possible processing tasks */
#define CY_CAPSENSE_PROCESS_ALL                         (CY_CAPSENSE_PROCESS_FILTER | \
                                                         CY_CAPSENSE_PROCESS_BASELINE | \
                                                         CY_CAPSENSE_PROCESS_DIFFCOUNTS | \
                                                         CY_CAPSENSE_PROCESS_CALC_NOISE | \
                                                         CY_CAPSENSE_PROCESS_THRESHOLDS | \
                                                         CY_CAPSENSE_PROCESS_STATUS)
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros_pin *//** \{ */
/******************************************************************************/
/** Connection of pin to ground */
#define CY_CAPSENSE_GROUND                              (0u)
/** Connection of pin to High-Z */
#define CY_CAPSENSE_HIGHZ                               (1u)
/** Configuring of pin as a shield */
#define CY_CAPSENSE_SHIELD                              (2u)
/** Configuring of pin as a CSD sensor */
#define CY_CAPSENSE_SENSOR                              (3u)
/** Configuring of pin as a Tx */
#define CY_CAPSENSE_TX_PIN                              (4u)
/** Configuring of pin as a Rx */
#define CY_CAPSENSE_RX_PIN                              (5u)
/** Pin is not connected to scanning bus */
#define CY_CAPSENSE_SNS_DISCONNECTED                    (0u)
/** Pin is connected to scanning bus */
#define CY_CAPSENSE_SNS_CONNECTED                       (1u)
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros_miscellaneous *//** \{ */
/******************************************************************************/
/** Defines the communication command complete bit mask */
#define CY_CAPSENSE_TU_CMD_COMPLETE_BIT                 (0x8000u)
/** Defines the status if restart was not performed in Cy_CapSense_RunTuner() */
#define CY_CAPSENSE_STATUS_RESTART_NONE                 (0x00u)
/** Defines the status if restart was done in Cy_CapSense_RunTuner() */
#define CY_CAPSENSE_STATUS_RESTART_DONE                 (0x01u)
/** Number of supported idac gains */
#define CY_CAPSENSE_IDAC_GAIN_NUMBER                    (6u)
/** 100% value */
#define CY_CAPSENSE_PERCENTAGE_100                      (100u)
/* Scope of scanning macros */
/** Sensor scanning scope is undefined */
#define CY_CAPSENSE_SCAN_SCOPE_UND                      (0u)
/** Sensor scanning scope is a single sensor */
#define CY_CAPSENSE_SCAN_SCOPE_SGL_SNS                  (1u)
/** Sensor scanning scope is all sensors in a queue */
#define CY_CAPSENSE_SCAN_SCOPE_ALL_SNS                  (2u)
/** Widget scanning scope is a single widget */
#define CY_CAPSENSE_SCAN_SCOPE_SGL_WD                   (0u)
/** Widget scanning scope is all widgets */
#define CY_CAPSENSE_SCAN_SCOPE_ALL_WD                   (1u)

/** \} */

#define CY_CAPSENSE_CONVERSION_MEGA                     (1000000u)

/* 
* These defines are obsolete and kept for backward compatibility only.
* They will be removed in the future versions.
*/
#define CY_CAPSENSE_MDW_VERSION_MAJOR                   (CY_CAPSENSE_MW_VERSION_MAJOR)
#define CY_CAPSENSE_MDW_VERSION_MINOR                   (CY_CAPSENSE_MW_VERSION_MINOR)
#define CY_CAPSENSE_MDW_ID                              (CY_CAPSENSE_ID)
#define CY_CAPSENSE_SW_STS_BUSY                         (CY_CAPSENSE_BUSY)


#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCSDV2 */

#endif /* CY_CAPSENSE_COMMON_H */


/* [] END OF FILE */
