/***************************************************************************//**
* \file cy_capsense_structure.h
* \version 2.0
*
* \brief
* This file provides the top-level declarations of the CapSense data
* structure. Also, the file declares the functions for data access.
*
********************************************************************************
* \copyright
* Copyright 2018-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_CAPSENSE_STRUCTURE_H)
#define CY_CAPSENSE_STRUCTURE_H

#include "cy_syslib.h"
#include "cy_device_headers.h"
#include "cy_csd.h"
#include "cy_capsense_lib.h"
#include "cy_capsense_gesture_lib.h"
#include "cy_capsense_common.h"

#if defined(CY_IP_MXCSDV2)

#if defined(__cplusplus)
extern "C" {
#endif


/*******************************************************************************
* CapSense Enumerated Types
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_capsense_enums *//** \{ */
/******************************************************************************/

/** Defines MW Tuner module states */
typedef enum
{
    CY_CAPSENSE_TU_FSM_RUNNING          = 0x00u,                /**< Running state is a state when CapSense middleware is not 
                                                                   * blocked by the CapSense Tuner tool and application program continuously scans */
    CY_CAPSENSE_TU_FSM_SUSPENDED        = 0x01u,                /**< Scanning is suspended */
    CY_CAPSENSE_TU_FSM_ONE_SCAN         = 0x03u,                /**< Scanning is suspended after one scan cycle */
} cy_en_capsense_tuner_state_t;

/** Defines the Tuner command codes */
typedef enum
{
    CY_CAPSENSE_TU_CMD_NONE_E           = 0u,                   /**< No command */
    CY_CAPSENSE_TU_CMD_SUSPEND_E        = 1u,                   /**< Suspend command */
    CY_CAPSENSE_TU_CMD_RESUME_E         = 2u,                   /**< Resume command switches state from suspend to running */
    CY_CAPSENSE_TU_CMD_RESTART_E        = 3u,                   /**< Restart command requests to perform CapSense re-initialization */
    CY_CAPSENSE_TU_CMD_RUN_SNR_TEST_E   = 4u,                   /**< Reserved */
    CY_CAPSENSE_TU_CMD_PING_E           = 5u,                   /**< Ping command to check whether application program calls Cy_CapSense_RunTuner() */
    CY_CAPSENSE_TU_CMD_ONE_SCAN_E       = 6u,                   /**< Execute one scan cycle and then switch to suspend state */
    CY_CAPSENSE_TU_CMD_WRITE_E          = 7u,                   /**< Writes specified data with offset into cy_capsense_tuner */
} cy_en_capsense_tuner_cmd_t;

/** Defines widget types */
typedef enum
{
    CY_CAPSENSE_WD_BUTTON_E             = 0x01u,                /**< Button widget */
    CY_CAPSENSE_WD_LINEAR_SLIDER_E      = 0x02u,                /**< Linear Slider widget */
    CY_CAPSENSE_WD_RADIAL_SLIDER_E      = 0x03u,                /**< Radial Slider widget */
    CY_CAPSENSE_WD_MATRIX_BUTTON_E      = 0x04u,                /**< Matrix Buttons widget */
    CY_CAPSENSE_WD_TOUCHPAD_E           = 0x05u,                /**< Touchpad widget */
    CY_CAPSENSE_WD_PROXIMITY_E          = 0x06u,                /**< Proximity widget */
} cy_en_capsense_widget_type_t;

/** Defines sensing methods types */
typedef enum
{
    CY_CAPSENSE_UNDEFINED_E             = 0x00u,                /**< Undefined method used at initialization or releasing the CSD HW block */
    CY_CAPSENSE_SENSE_METHOD_CSD_E      = 0x01u,                /**< CSD sensing method */
    CY_CAPSENSE_SENSE_METHOD_CSX_E      = 0x02u,                /**< CSX sensing method */
} cy_en_capsense_sensing_method_t;

/** Defines types of electrode */
typedef enum
{
    CY_CAPSENSE_ELTD_TYPE_SELF_E        = 0x01u,                /**< Electrode used as a sensor in CSD sensing method */
    CY_CAPSENSE_ELTD_TYPE_MUT_TX_E      = 0x02u,                /**< Electrode used as a TX in CSX sensing method */
    CY_CAPSENSE_ELTD_TYPE_MUT_RX_E      = 0x03u,                /**< Electrode used as a RX in CSX sensing method */
} cy_en_capsense_eltd_t;


/** Defines connections of sensing capacitors */
typedef enum
{
    CY_CAPSENSE_CMODPAD_E               = 0x01u,                /**< External capacitor is connected to dedicated CMOD pad */
    CY_CAPSENSE_CTANKPAD_E              = 0x02u,                /**< External capacitor is connected to dedicated CSH pad */
    CY_CAPSENSE_CSHIELDPAD_E            = 0x03u,                /**< External capacitor is connected to dedicated SHIELD pad */
    CY_CAPSENSE_VREFEXTPAD_E            = 0x04u,                /**< External capacitor is connected to dedicated VREF pad */
} cy_en_capsense_cap_connection_t;


/** Defines CapSense middleware execution events
  * when the CapSense callback can be executed. */
typedef enum
{
    CY_CAPSENSE_START_SAMPLE_E          = 0x01u,                /**< Start Sample Callback. The callback will be executed before each sensor
                                                                   *scan triggering */
    CY_CAPSENSE_END_OF_SCAN_E           = 0x02u,                /**< End Of Scan Callback. The callback will be executed when sensor scan
                                                                   * is finished and there is no other sensors in the queue to be scanned. */
} cy_en_capsense_callback_event_t;


/** \} */


/*******************************************************************************
* CapSense Data Structures
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_capsense_structures *//** \{ */
/******************************************************************************/

/** Sensor context structure */
typedef struct
{
    uint16_t raw;                                               /**< Sensor raw count */
    uint16_t bsln;                                              /**< Sensor baseline */
    uint16_t diff;                                              /**< Sensor difference count */
    uint8_t status;                                             /**< Sensor status */
    uint8_t negBslnRstCnt;                                      /**< Negative baseline reset counter */
    uint8_t idacComp;                                           /**< Compensation IDAC of CSD or IDAC in CSX */
    uint8_t bslnExt;                                            /**< Sensor baseline fractional */
} cy_stc_capsense_sensor_context_t;

/** CSX Touchpad touch tracking history */
typedef struct
{
    uint32_t velocity;                                          /**< The square of the "speed" (maximum distance change per refresh interval) threshold
                                                                   * distinguishing a fast finger movement from separate finger touches 
                                                                   * (in [pixels/refresh interval]).
                                                                   * Squared speeds exceeding this value indicate separate finger touches.
                                                                   * Squared speeds below this value indicate fast single finger movement. */   
    cy_stc_capsense_position_t oldPeak[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];
                                                                /**< Touch Positions */
    uint8_t oldPeakNumber;                                      /**< Number of detected peaks */
    uint8_t oldActiveIdsMask;                                   /**< Mask of used IDs */
} cy_stc_capsense_csx_touch_history_t;        

/** Internal CSX Touchpad buffer structure for CSX for Touchpads' processing */
typedef struct
{
    int32_t distanceMap[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS * CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];
                                                               /**< Buffer for distance map data */
    int32_t colMap[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];         /**< Buffer for column map data */
    int32_t rowMap[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];         /**< Buffer for row map data */
    int32_t minsMap[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];        /**< Buffer for minimums map data */
    cy_stc_capsense_position_t newPeak[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];
                                                                /**< Touch Positions */
    int8_t fingerPosIndexMap[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS + 3u];
                                                                /**< Buffer for index map data */
    int8_t linksMap[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];        /**< Buffer for linked map data */
    int8_t visitedMap[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];      /**< Buffer for visited map data */
    int8_t markIndicesMap[CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS];  /**< Buffer for mark map data */
    uint8_t newPeakNumber;                                      /**< Number of detected peaks */
    uint8_t newActiveIdsMask;                                   /**< Mask of used IDs */
} cy_stc_capsense_csx_touch_buffer_t;

/** Widget context structure */
typedef struct
{
    uint16_t fingerCap;                                         /**< Widget finger capacitance parameter used for the CSD 
                                                                   * widgets only when SmartSense is enabled */
    uint16_t sigPFC;                                            /**< The 75% of signal per user-defined finger capacitance */
    uint16_t resolution;                                        /**< Provides scan resolution for the CSD Widgets.
                                                                   * Provides number of sub-conversions for the CSX Widgets */
    uint16_t maxRawCount;                                       /**< Calculated maximum raw count of widget */
    uint16_t fingerTh;                                          /**< Widget Finger Threshold */
    uint16_t proxTh;                                            /**< Widget Proximity Threshold */
    uint16_t lowBslnRst;                                        /**< The widget low baseline reset count. Specifies the number 
                                                                   * of samples the sensor signal must be below the Negative 
                                                                   * Noise Threshold \ref nNoiseTh to trigger a baseline reset */
    uint16_t snsClk;                                            /**< Sense Clock Divider. For the Matrix Buttons and Touchpad widgets 
                                                                   * specifies the column sense clock divider */
    uint16_t rowSnsClk;                                         /**< Row Sense Clock Divider for the Matrix Buttons and Touchpad widgets */
    uint16_t gestureDetected;                                   /**< Mask of detected gestures */
    uint16_t gestureDirection;                                  /**< Mask of directions of detected gestures */
    int16_t xDelta;                                             /**< The filtered by Ballistic Multiplier X-displacement */
    int16_t yDelta;                                             /**< The filtered by Ballistic Multiplier Y-displacement */
    uint8_t noiseTh;                                            /**< Widget Noise Threshold */
    uint8_t nNoiseTh;                                           /**< Widget Negative Noise Threshold */
    uint8_t hysteresis;                                         /**< Widget Hysteresis for the signal crossing finger threshold */
    uint8_t onDebounce;                                         /**< Widget Debounce for the signal above the finger threshold 1 to 255.
                                                                   * * 1 - touch reported immediately as soon as detected
                                                                   * * 2 - touch reported on the second consecutive detection
                                                                   * * 3 - touch reported on the third consecutive detection */
    uint8_t snsClkSource;                                       /**< Widget clock source:
                                                                   * * bit[7] - Indicates auto mode of clock source selection
                                                                   * * bit[0:6] - Clock source: 
                                                                   *   * 0 - Direct (CY_CAPSENSE_CLK_SOURCE_DIRECT) 
                                                                   *   * 1 - SSC6 (CY_CAPSENSE_CLK_SOURCE_SSC6) 
                                                                   *   * 2 - SSC7 (CY_CAPSENSE_CLK_SOURCE_SSC7) 
                                                                   *   * 3 - SSC9 (CY_CAPSENSE_CLK_SOURCE_SSC9) 
                                                                   *   * 4 - SSC10 (CY_CAPSENSE_CLK_SOURCE_SSC10) 
                                                                   *   * 5 - PRS8 (CY_CAPSENSE_CLK_SOURCE_PRS8) 
                                                                   *   * 6 - PRS12 (CY_CAPSENSE_CLK_SOURCE_PRS12) */
    uint8_t idacMod[CY_CAPSENSE_FREQ_CHANNELS_NUM];             /**< Sets the current of the modulation IDAC for the CSD widgets. 
                                                                * For the CSD Touchpad and Matrix Button widgets sets the current of the 
                                                                * modulation IDAC for the column sensors. Not used for the CSX widgets. */
    uint8_t idacGainIndex;                                      /**< Index of IDAC gain in table \ref cy_stc_capsense_idac_gain_table_t */
    uint8_t rowIdacMod[CY_CAPSENSE_FREQ_CHANNELS_NUM];          /**< Sets the current of the modulation IDAC for the row sensors 
                                                                * for the CSD Touchpad and Matrix Button widgets. Not used for the CSX widgets. */
    uint8_t bslnCoeff;                                          /**< Baseline IIR filter coefficient. Lower value leads to higher filtering. */
    uint8_t status;                                             /**< Contains masks: 
                                                                   * * bit[0] - Widget Active (CY_CAPSENSE_WD_ACTIVE_MASK)
                                                                   * * bit[1] - Widget Disabled (CY_CAPSENSE_WD_DISABLE_MASK)
                                                                   * * bit[2] - Widget Working (CY_CAPSENSE_WD_WORKING_MASK) - reserved */
    cy_stc_capsense_touch_t wdTouch;                            /**< Widget touch structure used for Matrix Buttons, Sliders, and Touchpads */                  
} cy_stc_capsense_widget_context_t;                                

/** Pin configuration structure */
typedef struct
{
    GPIO_PRT_Type * pcPtr;                                      /**< Pointer to the base port register of the IO */
    uint8_t pinNumber;                                          /**< Position of the IO in the port */
} cy_stc_capsense_pin_config_t;

/** Electrode objects configuration structure */
typedef struct
{
    const cy_stc_capsense_pin_config_t * ptrPin;                /**< Pointer to pin configuration structure */
    uint8_t type;                                               /**< Electrode type \ref cy_en_capsense_eltd_t */
    uint8_t numPins;                                            /**< Total number of pins in this sensor */
} cy_stc_capsense_electrode_config_t;

/** Configuration structure of advanced touchpad */
typedef struct
{
    uint16_t penultimateTh;                                     /**< Defines a threshold for determining arrival at edges. This 
                                                                   * value may have to be increased for small diamonds, so that the edge handling is 
                                                                   * initiated sooner. If this number is too high, there is jumping at the edge with 
                                                                   * a smaller finger. If this number is too low, there is jumping at the edge with a 
                                                                   * larger finger. */
    uint16_t virtualSnsTh;                                      /**< Defines a virtual sensor signal. This value should be set 
                                                                   * to the value of any sensor when a medium-sized finger is placed directly over 
                                                                   * it. If this value is too high, a position is reported nearer the edge than ideal 
                                                                   * position. If this value is too low, a position is reported nearer the middle of 
                                                                   * touchpad. */
    uint8_t crossCouplingTh;                                    /**< Defines cross coupling threshold. It is subtracted from 
                                                                   * sensor signals at centroid position calculation to improve the accuracy.
                                                                   * The threshold should be equal to a sensor signal when your finger is near the 
                                                                   * sensor, but not touching the sensor. This can be determined by slowly dragging 
                                                                   * your finger across the panel and finding the inflection point of the difference 
                                                                   * counts at the base of the curve. The difference value at this point should be 
                                                                   * the Cross-coupling threshold. */
    uint8_t reserved0;                                          /**< Reserved field */                                              
    uint8_t reserved1;                                          /**< Reserved field */
    uint8_t reserved2;                                          /**< Reserved field */
} cy_stc_capsense_advanced_touchpad_config_t;

/** Widget configuration structure */
typedef struct
{
    cy_stc_capsense_widget_context_t * ptrWdContext;            /**< Pointer to context structure of this widget */
    cy_stc_capsense_sensor_context_t * ptrSnsContext;           /**< Pointer to the first object of sensor context structure that belongs to this widget */
    const cy_stc_capsense_electrode_config_t * ptrEltdConfig;   /**< Pointer to the first object of electrode configuration structure that belongs to this widget */

    cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope;   
                                                                /**< Pointer to the noise envelope filter used by SmartSense */
    uint16_t * ptrRawFilterHistory;                             /**< Pointer to the raw count filter history of the widget */
    uint8_t  * ptrRawFilterHistoryLow;                          /**< Pointer to the raw count filter history extended of the widget */                                                            
    uint32_t   iirCoeff;                                        /**< Raw count IIR filter coefficient. Smaller value leads to higher filtering */

    uint8_t * ptrDebounceArr;                                   /**< Pointer to the debounce array of the widget */

    const uint8_t * ptrDiplexTable;                             /**< Pointer to the diplex table used for Linear slider when Diplex option is enabled */
    uint32_t centroidConfig;                                    /**< Configuration of centroids */
    uint16_t xResolution;                                       /**< Keeps maximum position value. For Touchpads X-axis maximum position */
    uint16_t yResolution;                                       /**< For Touchpads Y-Axis maximum position */
    uint16_t numSns;                                            /**< The total number of sensors:
                                                                   * For CSD widgets: WD_NUM_ROWS + WD_NUM_COLS. 
                                                                   * For CSX widgets: WD_NUM_ROWS * WD_NUM_COLS. */
    uint8_t numCols;                                            /**< For CSD Button and Proximity Widgets, the number of sensors.
                                                                   * For CSD Slider Widget, the number of segments.
                                                                   * For CSD Touchpad and Matrix Button, the number of the column sensors.
                                                                   * For CSX Button, Touchpad, and Matrix Button, the number of the Rx electrodes. */
    uint8_t numRows;                                            /**< For CSD Touchpad and Matrix Buttons, the number of the row sensors.
                                                                   * For the CSX Button, the number of the Tx electrodes (constant 1u).
                                                                   * For CSX Touchpad and Matrix Button, the number of the Tx electrodes. */
    cy_stc_capsense_touch_t * ptrPosFilterHistory;              /**< Pointer to the position filter history */
    cy_stc_capsense_csx_touch_history_t * ptrCsxTouchHistory;   /**< Pointer to the CSX touchpad history */
    cy_stc_capsense_csx_touch_buffer_t * ptrCsxTouchBuffer;     /**< Pointer to the single CSX buffer needed for CSX touchpad processing */
    uint16_t * ptrCsdTouchBuffer;                               /**< Pointer to the CSD buffer needed for advanced CSD touchpad processing */

    cy_stc_capsense_gesture_config_t * ptrGestureConfig;        /**< Pointer to Gesture configuration structure */
    cy_stc_capsense_gesture_context_t * ptrGestureContext;      /**< Pointer to Gesture context structure */

    cy_stc_capsense_ballistic_config_t ballisticConfig;         /**< The configuration data for position ballistic filter. */
    cy_stc_capsense_ballistic_context_t * ptrBallisticContext;  /**< Pointer to Ballistic filter context structure */

    cy_stc_capsense_adaptive_filter_config_t aiirConfig;        /**< The configuration of position adaptive filter. */
    cy_stc_capsense_advanced_touchpad_config_t advConfig;       /**< The configuration of CSD advanced touchpad */

    uint32_t posFilterConfig;                                   /**< Position filters configuration */
    uint16_t rawFilterConfig;                                   /**< Raw count filters configuration */

    uint8_t senseMethod;                                        /**< Specifies the widget sensing method */
    uint8_t wdType;                                             /**< Specifies the widget type */

} cy_stc_capsense_widget_config_t;

/** Declares the IDAC gain table */
typedef struct
{
    uint32_t gainReg;                                           /**< Register value of IDAC gain */
    uint32_t gainValue;                                         /**< Absolute gain value in pA */
} cy_stc_capsense_idac_gain_table_t;

/** Common configuration structure */
typedef struct
{
    uint32_t cpuClkHz;                                          /**< CPU clock in Hz */
    uint32_t periClkHz;                                         /**< Peripheral clock in Hz */
    uint16_t vdda;                                              /**< VDDA in mV */
    uint16_t numPin;                                            /**< Total number of IOs. */
    uint16_t numSns;                                            /**< The total number of sensors. It is equal to the number of objects with raw count. 
                                                                   * * For CSD widgets: WD_NUM_ROWS + WD_NUM_COLS 
                                                                   * * For CSX widgets: WD_NUM_ROWS * WD_NUM_COLS */
    uint8_t numWd;                                              /**< Total number of widgets */                                                               

    uint8_t csdEn;                                              /**< CSD sensing method enabled, at least one CSD widget is configured */
    uint8_t csxEn;                                              /**< CSX sensing method enabled, at least one CSX widget is configured */
    uint8_t mfsEn;                                              /**< Multi-frequency Scan (MFS) enabled */
    uint8_t positionFilterEn;                                   /**< Position filtering enabled */

    uint8_t periDividerType;                                    /**< Peripheral clock type (8- or 16-bit type) */
    uint8_t periDividerIndex;                                   /**< Peripheral divider index */
    uint8_t analogWakeupDelay;                                  /**< Time needed to establish correct operation of the CSD HW block after power up or System Deep Sleep. */

    uint8_t ssIrefSource;                                       /**< Iref source */
    uint8_t ssVrefSource;                                       /**< Vref source */

    uint16_t proxTouchCoeff;                                    /**< Proximity touch coefficient in percentage used in SmartSense */
    uint8_t swSensorAutoResetEn;                                /**< Sensor auto reset enabled */
    
    uint8_t portCmodPadNum;                                     /**< Number of port of dedicated Cmod pad */
    uint8_t pinCmodPad;                                         /**< Position of the dedicated Cmod pad in the port */
    uint8_t portCshPadNum;                                      /**< Number of port of dedicated Csh pad */
    uint8_t pinCshPad;                                          /**< Position of the dedicated Csh pad in the port */
    uint8_t portShieldPadNum;                                   /**< Number of port of dedicated Shield pad */
    uint8_t pinShieldPad;                                       /**< Position of the dedicated Shield pad in the port */
    uint8_t portVrefExtPadNum;                                  /**< Number of port of dedicated VrefExt pad */
    uint8_t pinVrefExtPad;                                      /**< Position of the dedicated VrefExt pad in the port */
    uint8_t portCmodNum;                                        /**< Number of port of Cmod pin */

    cy_stc_capsense_idac_gain_table_t idacGainTable[CY_CAPSENSE_IDAC_GAIN_NUMBER];
                                                                /**< Table with the supported IDAC gains and corresponding register values */

    CSD_Type * ptrCsdBase;                                      /**< Pointer to the CSD HW block register */
    cy_stc_csd_context_t * ptrCsdContext;                       /**< Pointer to the CSD driver context */
    GPIO_PRT_Type * portCmod;                                   /**< Pointer to the base port register of the Cmod pin */
    GPIO_PRT_Type * portCsh;                                    /**< Pointer to the base port register of the Csh pin */
    GPIO_PRT_Type * portCintA;                                  /**< Pointer to the base port register of the CintA pin */
    GPIO_PRT_Type * portCintB;                                  /**< Pointer to the base port register of the CintB pin */

    uint8_t pinCmod;                                            /**< Position of the Cmod pin in the port */
    uint8_t portCshNum;                                         /**< Number of port of Csh pin */
    uint8_t pinCsh;                                             /**< Position of the Csh pin in the port */
    uint8_t pinCintA;                                           /**< Position of the CintA pin in the port */
    uint8_t pinCintB;                                           /**< Position of the CintB pin in the port */

    uint8_t csdShieldEn;                                        /**< Shield enabled */
    uint8_t csdInactiveSnsConnection;                           /**< Inactive sensor connection state:
                                                                   * * CY_CAPSENSE_SNS_CONNECTION_HIGHZ
                                                                   * * CY_CAPSENSE_SNS_CONNECTION_SHIELD
                                                                   * * CY_CAPSENSE_SNS_CONNECTION_GROUND */
    uint8_t csdShieldDelay;                                     /**< Shield signal delay */
    uint16_t csdVref;                                           /**< Vref for CSD method */
    uint16_t csdRConst;                                         /**< Sensor resistance in series used by SmartSense */                                                                   
    uint8_t csdCTankShieldEn;                                   /**< Csh enabled */
    uint8_t csdShieldNumPin;                                    /**< Number of shield IOs */
    uint8_t csdShieldSwRes;                                     /**< Shield switch resistance */
    uint8_t csdInitSwRes;                                       /**< Switch resistance at coarse initialization */
    uint8_t csdChargeTransfer;                                  /**< IDAC sensing configuration */
    uint8_t csdRawTarget;                                       /**< Raw count target in percentage for CSD calibration */
    uint8_t csdAutotuneEn;                                      /**< SmartSense enabled */
    uint8_t csdIdacAutocalEn;                                   /**< CSD IDAC calibration enabled */
    uint8_t csdIdacGainInitIndex;                               /**< IDAC gain index per \ref idacGainTable */
    uint8_t csdIdacAutoGainEn;                                  /**< IDAC gain auto-calibration enabled */
    uint8_t csdCalibrationError;                                /**< Acceptable calibration error */
    uint8_t csdIdacMin;                                         /**< Min acceptable IDAC value in CSD calibration */
    uint8_t csdIdacCompEn;                                      /**< Compensation IDAC enabled */
    uint8_t csdFineInitTime;                                    /**< Number of dummy SnsClk periods at fine initialization */
    uint8_t csdIdacRowColAlignEn;                               /**< Row-Column alignment enabled. It adjusts modulator IDAC for rows 
                                                                   * and for columns to achieve the similar sensitivity */
    uint8_t  csdMfsDividerOffsetF1;                             /**< Frequency divider offset for channel 1. This value is added to 
                                                                   * base (channel 0) SnsClk divider to form channel 1 frequency */
    uint8_t  csdMfsDividerOffsetF2;                             /**< Frequency divider offset for channel 2. This value is added to 
                                                                   * base (channel 0) SnsClk divider to form channel 2 frequency */
    uint8_t csxRawTarget;                                       /**< Raw count target in percentage for CSX calibration */
    uint8_t csxIdacGainInitIndex;                               /**< IDAC gain for CSX method */
    uint8_t csxRefGain;                                         /**< Refgen gain for CSX method */
    uint8_t csxIdacAutocalEn;                                   /**< CSX IDAC calibration enabled */
    uint8_t csxCalibrationError;                                /**< Acceptable calibration error */
    uint8_t csxFineInitTime;                                    /**< Number of dummy TX periods at fine initialization */
    uint8_t csxInitSwRes;                                       /**< Switch resistance at fine initialization */
    uint8_t csxScanSwRes;                                       /**< Switch resistance at scanning */
    uint8_t csxInitShieldSwRes;                                 /**< Switch resistance at fine initialization */
    uint8_t csxScanShieldSwRes;                                 /**< Switch resistance at scanning */
    uint8_t csxMfsDividerOffsetF1;                              /**< Frequency divider offset for channel 1. This value is added to 
                                                                   * base (channel 0) Tx divider to form channel 1 frequency */
    uint8_t csxMfsDividerOffsetF2;                              /**< Frequency divider offset for channel 2. This value is added to 
                                                                   * base (channel 0) Tx divider to form channel 2 frequency */
} cy_stc_capsense_common_config_t;

/** Declares active sensor details */
typedef struct
{
    void (* ptrISRCallback)(void * context);                    /**< Pointer to the interrupt handler of the active sensor */

    const cy_stc_capsense_widget_config_t * ptrWdConfig;        /**< Pointer to the widget configuration structure of the active sensor */
    cy_stc_capsense_widget_context_t * ptrWdContext;            /**< Pointer to the widget context structure of the active sensor */

    const cy_stc_capsense_electrode_config_t * ptrEltdConfig;   /**< Pointer to the electrode configuration structure of the active sensor */
    const cy_stc_capsense_electrode_config_t * ptrRxConfig;     /**< Pointer to the Rx electrode configuration structure of the active sensor */
    const cy_stc_capsense_electrode_config_t * ptrTxConfig;     /**< Pointer to the Tx electrode configuration structure of the active sensor */

    cy_stc_capsense_sensor_context_t * ptrSnsContext;           /**< Pointer to the sensor context structure */

    volatile uint16_t sensorIndex;                              /**< Current sensor ID */    
    volatile uint8_t widgetIndex;                               /**< Current widget ID */
    volatile uint8_t rxIndex;                                   /**< Current Rx ID */
    volatile uint8_t txIndex;                                   /**< Current Tx ID */
    volatile uint8_t connectedSnsState;                         /**< Shows if the current sensor is connected to analog bus */
    volatile uint8_t scanScopeAll;                              /**< Request of scanning all widgets */
    volatile uint8_t scanScopeSns;                              /**< Request of scanning a single sensor */
    volatile uint8_t mfsChannelIndex;                           /**< MFS channel index */
    volatile uint8_t currentSenseMethod;                        /**< Current sensing method */
} cy_stc_active_scan_sns_t;


/** Declares internal Context Data Structure */
typedef struct
{
    uint32_t csdInactiveSnsDm;                                  /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegConfig;                                      /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwHsPSelScan;                                /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwHsPSelInit;                                /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwBypSel;                                    /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwResScan;                                   /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwResInit;                                   /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegAmuxbufInit;                                 /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwAmuxbufSel;                                /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwShieldSelScan;                             /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwShieldSelInit;                             /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegHscmpInit;                                   /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegHscmpScan;                                   /**< Internal pre-calculated data for faster operation */
    uint32_t csdIdacAConfig;                                    /**< Internal pre-calculated data for faster operation */
    uint32_t csdIdacBConfig;                                    /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwFwTankSelScan;                             /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwFwTankSelInit;                             /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegSwCmpPSel;                                   /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegIoSel;                                       /**< Internal pre-calculated data for faster operation */
    uint32_t csdRegRefgen;                                      /**< Internal pre-calculated data for faster operation */
    uint32_t csxRegConfigInit;                                  /**< Internal pre-calculated data for faster operation */
    uint32_t csxRegConfigScan;                                  /**< Internal pre-calculated data for faster operation */
    uint32_t csxRegSwResInit;                                   /**< Internal pre-calculated data for faster operation */
    uint32_t csxRegSwResPrech;                                  /**< Internal pre-calculated data for faster operation */
    uint32_t csxRegSwResScan;                                   /**< Internal pre-calculated data for faster operation */
    uint32_t csxRegRefgen;                                      /**< Internal pre-calculated data for faster operation */
    uint32_t regSwRefGenSel;                                    /**< Internal pre-calculated data for faster operation */

    uint8_t csdCmodConnection;                                  /**< Internal pre-calculated data for faster operation */
    uint8_t csdCshConnection;                                   /**< Internal pre-calculated data for faster operation */
    en_hsiom_sel_t csdInactiveSnsHsiom;                         /**< Internal pre-calculated data for faster operation */
    uint8_t csdVrefGain;                                        /**< Internal pre-calculated data for faster operation */
    uint16_t csdVrefVoltageMv;                                  /**< Internal pre-calculated data for faster operation */
}cy_stc_capsense_internal_context_t;

/**
* Provides the typedef for the callback function that is intended to be called when
* the \ref cy_en_capsense_callback_event_t events occurs.
*/
typedef void (*cy_capsense_callback_t)(cy_stc_active_scan_sns_t * ptrActiveScan);

/**
* Provides the typedef for the callback function that is called by the 
* Cy_CapSense_RunTuner() function to establish communication with 
* the CapSense Tuner tool to monitor CapSense operation. 
* Refer to \ref group_capsense_callbacks section.
*/
typedef void (*cy_capsense_tuner_send_callback_t)(void * context);

/**
* Provides the typedef for the callback function that is called by the 
* Cy_CapSense_RunTuner() function to establish communication with 
* the CapSense Tuner tool to support life-time tunning. 
* Refer to \ref group_capsense_callbacks section.
*/
typedef void (*cy_capsense_tuner_receive_callback_t)(uint8_t ** commandPacket, uint8_t ** tunerPacket, void * context);

/** \} */



/******************************************************************************/
/** \addtogroup group_capsense_structures *//** \{ */
/******************************************************************************/

/** Declares top-level Context Data Structure */
typedef struct
{
    uint16_t configId;                                          /**< 16-bit CRC calculated by the CapSense Configurator tool for the CapSense configuration. 
                                                                   * Used by the CapSense Tuner tool to identify if the FW corresponds to the specific user configuration. */
    uint16_t tunerCmd;                                          /**< Tuner Command Register \ref cy_en_capsense_tuner_cmd_t. 
                                                                   * Used for the communication between the CapSense Tuner tool and the middleware */
    uint16_t scanCounter;                                       /**< This counter increments after each scan. */
    uint8_t tunerSt;                                            /**< State of CapSense middleware tuner module. \ref cy_en_capsense_tuner_state_t */
    uint8_t initDone;                                           /**< Keep information whether initialization was done or not */        
    cy_capsense_callback_t ptrSSCallback;                       /**< Pointer to a user's Start Sample callback function. Refer to \ref group_capsense_callbacks section */
    cy_capsense_callback_t ptrEOSCallback;                      /**< Pointer to a user's End Of Scan callback function. Refer to \ref group_capsense_callbacks section */

    cy_capsense_tuner_send_callback_t ptrTunerSendCallback;     /**< Pointer to a user's tuner callback function. Refer to \ref group_capsense_callbacks section */
    cy_capsense_tuner_receive_callback_t ptrTunerReceiveCallback;  /**< Pointer to a user's tuner callback function. Refer to \ref group_capsense_callbacks section */

    volatile uint32_t status;                                   /**< Middleware status information, scan in progress or not */
    uint32_t timestampInterval;                                 /**< Timestamp interval used at increasing the timestamp by Cy_CapSense_IncrementGestureTimestamp() */   
    uint32_t timestamp;                                         /**< Current timestamp should be kept updated and operational, which is vital for the
                                                                   * operation of Gesture and Ballistic multiplier features */                                                            
    uint8_t modCsdClk;                                          /**< The modulator clock divider for the CSD widgets */
    uint8_t modCsxClk;                                          /**< The modulator clock divider for the CSX widgets */
    uint8_t tunerCnt;                                           /**< Command counter of CapSense middleware tuner module */
} cy_stc_capsense_common_context_t;


/** Declares top-level CapSense context data structure */
typedef struct
{
    const cy_stc_capsense_common_config_t * ptrCommonConfig;    /**< Pointer to the common configuration structure */
    cy_stc_capsense_common_context_t * ptrCommonContext;        /**< Pointer to the common context structure */
    cy_stc_capsense_internal_context_t * ptrInternalContext;    /**< Pointer to the internal context structure */
    const cy_stc_capsense_widget_config_t * ptrWdConfig;        /**< Pointer to the widget configuration structure */
    cy_stc_capsense_widget_context_t * ptrWdContext;            /**< Pointer to the widget context structure */
    const cy_stc_capsense_pin_config_t * ptrPinConfig;          /**< Pointer to the pin configuration structure */
    const cy_stc_capsense_pin_config_t * ptrShieldPinConfig;    /**< Pointer to the shield pin configuration structure */
    cy_stc_active_scan_sns_t * ptrActiveScanSns;                /**< Pointer to the current active sensor structure */
    const void * ptrFptrConfig;                                 /**< Pointer to the function pointers structure */
} cy_stc_capsense_context_t;

/** The type of a pointer to the Cy_CapSense_CSDSetupWidget() function */
typedef void (*cy_func_capsense_csd_setup_widget_ptr_t)
                        (uint32_t widgetId,
                         cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSDScan() function */
typedef void (*cy_func_capsense_csd_scan_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_DpProcessCsdWidgetRawCounts() function */
typedef uint32_t (*cy_func_capsense_dp_process_csd_widget_raw_counts_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         const cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_DpProcessCsdWidgetStatus() function */
typedef void (*cy_func_capsense_dp_process_csd_widget_status_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSDDisableMode() function */
typedef void (*cy_func_capsense_csd_disable_mode_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSDInitialize() function */
typedef void (*cy_func_capsense_csd_initialize_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSDScanISR() function */
typedef void (*cy_func_capsense_csd_scan_isr_ptr_t)
                        (void * capsenseContext);
/** The type of a pointer to the Cy_CapSense_CSXSetupWidget() function */
typedef void (*cy_func_capsense_csx_setup_widget_ptr_t)
                        (uint32_t widgetId, cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSXScan() function */
typedef void (*cy_func_capsense_csx_scan_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_DpProcessCsxWidgetRawCounts() function */
typedef uint32_t (*cy_capsense_dp_process_csx_widget_raw_counts_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         const cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_DpProcessCsxWidgetStatus() function */
typedef void (*cy_func_capsense_dp_process_csx_widget_status_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSXInitialize() function */
typedef void (*cy_func_capsense_csx_initialize_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSXDisableMode() function */
typedef void (*cy_func_capsense_csx_disablemode_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSXScanISR() function */
typedef void (*cy_func_capsense_csx_scan_isr_ptr_t)
                        (void * capsenseContext);
/** The type of a pointer to the Cy_CapSense_AdaptiveFilterInitialize_Lib() function */
typedef void (*cy_func_capsense_adaptive_filter_initialize_lib_ptr_t)
                        (const cy_stc_capsense_adaptive_filter_config_t * config,
                         cy_stc_capsense_position_t * context);
/** The type of a pointer to the Cy_CapSense_AdaptiveFilterRun_Lib() function */
typedef void (*cy_func_capsense_adaptive_filter_run_lib_ptr_t)
                        (const cy_stc_capsense_adaptive_filter_config_t * config,
                         cy_stc_capsense_position_t * context,
                         uint32_t * currentX,
                         uint32_t * currentY);
/** The type of a pointer to the Cy_CapSense_BallisticMultiplier_Lib() function */
typedef void (*cy_func_capsense_ballistic_multiplier_lib_ptr_t)
                        (const cy_stc_capsense_ballistic_config_t * config,
                         const cy_stc_capsense_touch_t * touch,
                         cy_stc_capsense_ballistic_delta_t * delta,
                         uint32_t timestamp,
                         cy_stc_capsense_ballistic_context_t * context);
/** The type of a pointer to the Cy_CapSense_InitializeAllFilters() function */
typedef void (*cy_func_capsense_initialize_all_filters_ptr_t)
                        (const cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_FtRunEnabledFiltersInternal() function */
typedef void (*cy_func_capsense_ft_run_enabled_filters_internal_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         cy_stc_capsense_sensor_context_t * ptrSnsContext,
                         uint16_t * ptrSnsRawHistory,
                         uint8_t * ptrSnsRawHistoryLow);
/** The type of a pointer to the Cy_CapSense_ProcessPositionFilters() function */
typedef void (*cy_func_capsense_process_position_filters_ptr_t)
                        (cy_stc_capsense_touch_t * newTouch,
                         const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         const cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_RunPositionFilters() function */
typedef void (*cy_func_capsense_run_position_filters_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         cy_stc_capsense_position_t * ptrInput,
                         cy_stc_capsense_position_t * ptrHistory,
                         const cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_InitPositionFilters() function */
typedef void (*cy_func_capsense_init_position_filters_ptr_t)
                        (uint32_t filterConfig,
                         const cy_stc_capsense_position_t * ptrInput,
                         cy_stc_capsense_position_t * ptrHistory);
/** The type of a pointer to the Cy_CapSense_DpProcessButton() function */
typedef void (*cy_func_capsense_dp_process_button_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_DpProcessSlider() function */
typedef void (*cy_func_capsense_dp_process_slider_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         const cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_DpProcessCsdMatrix() function */
typedef void (*cy_func_capsense_dp_process_csd_matrix_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_DpProcessCsdTouchpad() function */
typedef void (*cy_func_capsense_dp_process_csd_touchpad_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         const cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_DpAdvancedCentroidTouchpad() function */
typedef void (*cy_func_capsense_dp_advanced_centroid_touchpad_ptr_t)
                        (cy_stc_capsense_touch_t * newTouch,
                         const cy_stc_capsense_widget_config_t * ptrWdConfig);
/** The type of a pointer to the Cy_CapSense_DpProcessProximity() function */
typedef void (*cy_func_capsense_dp_process_proximity_ptr_t)
                        (cy_stc_capsense_widget_config_t const * ptrWdConfig);
/** The type of a pointer to the Cy_CapSense_DpProcessCsxTouchpad() function */
typedef void (*cy_func_capsense_dp_process_csx_touchpad_ptr_t)
                        (const cy_stc_capsense_widget_config_t * ptrWdConfig,
                         const cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CalibrateAllCsdWidgets() function */
typedef cy_status (*cy_func_capsense_calibrate_all_csd_widgets_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CSDCalibrateWidget() function */
typedef cy_status (*cy_func_capsense_csd_calibrate_widget_ptr_t)
                        (uint32_t widgetId,
                         uint32_t target,
                         cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_CalibrateAllCsxWidgets() function */
typedef cy_status (*cy_func_capsense_calibrate_all_csx_widgets_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_SsAutoTune() function */
typedef cy_status (*cy_func_capsense_ss_auto_tune_ptr_t)
                        (cy_stc_capsense_context_t * context);
/** The type of a pointer to the Cy_CapSense_RunNoiseEnvelope_Lib() function */
typedef void (*cy_func_capsense_run_noise_envelope_lib_ptr_t)
                        (uint16_t rawCount,
                         uint16_t sigPFC,
                         cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope);
/** The type of a pointer to the Cy_CapSense_DpUpdateThresholds() function */
typedef void (*cy_func_capsense_dp_update_thresholds_ptr_t)
                        (cy_stc_capsense_widget_context_t * ptrWdContext,
                         const cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                         uint32_t startFlag);
/** The type of a pointer to the Cy_CapSense_InitializeNoiseEnvelope_Lib() function */
typedef void (*cy_func_capsense_initialize_noise_envelope_lib_ptr_t)
                        (uint16_t rawCount,
                         uint16_t sigPFC,
                         cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope);

/** Function pointers configuration structure */
typedef struct
{
    cy_func_capsense_csd_setup_widget_ptr_t fptrCSDSetupWidget; /**< The Cy_CapSense_CSDSetupWidget() function pointer*/
    cy_func_capsense_csd_scan_ptr_t fptrCSDScan;                /**< The Cy_CapSense_CSDScan() function pointer */
    cy_func_capsense_dp_process_csd_widget_raw_counts_ptr_t fptrDpProcessCsdWidgetRawCounts;
                                                                /**< The Cy_CapSense_DpProcessCsdWidgetRawCounts() function pointer*/
    cy_func_capsense_dp_process_csd_widget_status_ptr_t fptrDpProcessCsdWidgetStatus;
                                                                /**< The Cy_CapSense_DpProcessCsdWidgetStatus() function pointer*/
    cy_func_capsense_csd_disable_mode_ptr_t fptrCSDDisableMode; /**< The Cy_CapSense_CSDDisableMode() function pointer*/
    cy_func_capsense_csd_initialize_ptr_t fptrCSDInitialize;    /**< The Cy_CapSense_CSDInitialize() function pointer*/
    cy_func_capsense_csd_scan_isr_ptr_t fptrCSDScanISR;         /**< The Cy_CapSense_CSDScanISR() function pointer*/
    cy_func_capsense_csx_setup_widget_ptr_t fptrCSXSetupWidget; /**< The Cy_CapSense_CSXSetupWidget() function pointer*/
    cy_func_capsense_csx_scan_ptr_t fptrCSXScan;                /**< The Cy_CapSense_CSXScan() function pointer*/
    cy_capsense_dp_process_csx_widget_raw_counts_ptr_t fptrDpProcessCsxWidgetRawCounts;
                                                                /**< The Cy_CapSense_DpProcessCsxWidgetRawCounts() function pointer*/
    cy_func_capsense_dp_process_csx_widget_status_ptr_t fptrDpProcessCsxWidgetStatus;
                                                                /**< The Cy_CapSense_DpProcessCsxWidgetStatus() function pointer*/
    cy_func_capsense_csx_initialize_ptr_t fptrCSXInitialize;    /**< The Cy_CapSense_CSXInitialize() function pointer*/
    cy_func_capsense_csx_disablemode_ptr_t fptrCSXDisableMode;  /**< The Cy_CapSense_CSXDisableMode() function pointer*/
    cy_func_capsense_csx_scan_isr_ptr_t fptrCSXScanISR;         /**< The Cy_CapSense_CSXScanISR() function pointer*/
    cy_func_capsense_adaptive_filter_initialize_lib_ptr_t fptrAdaptiveFilterInitializeLib;
                                                                /**< The Cy_CapSense_AdaptiveFilterInitialize_Lib() function pointer*/
    cy_func_capsense_adaptive_filter_run_lib_ptr_t fptrAdaptiveFilterRunLib;     /**< The Cy_CapSense_AdaptiveFilterRun_Lib() function pointer*/
    cy_func_capsense_ballistic_multiplier_lib_ptr_t fptrBallisticMultiplierLib;
                                                                /**< The Cy_CapSense_BallisticMultiplier_Lib() function pointer*/
    cy_func_capsense_initialize_all_filters_ptr_t fptrInitializeAllFilters;
                                                                /**< The Cy_CapSense_InitializeAllFilters() function pointer*/
    cy_func_capsense_ft_run_enabled_filters_internal_ptr_t fptrFtRunEnabledFiltersInternal;
                                                                /**< The Cy_CapSense_FtRunEnabledFiltersInternal() function pointer*/
    cy_func_capsense_process_position_filters_ptr_t fptrProcessPositionFilters;
                                                                /**< The Cy_CapSense_ProcessPositionFilters() function pointer*/
    cy_func_capsense_run_position_filters_ptr_t fptrRunPositionFilters;
                                                                /**< The Cy_CapSense_RunPositionFilters() function pointer*/
    cy_func_capsense_init_position_filters_ptr_t fptrInitPositionFilters;
                                                                /**< The Cy_CapSense_InitPositionFilters() function pointer*/
    cy_func_capsense_dp_process_button_ptr_t fptrDpProcessButton;
                                                                /**< The Cy_CapSense_DpProcessButton() function pointer*/
    cy_func_capsense_dp_process_slider_ptr_t fptrDpProcessSlider;
                                                                /**< The Cy_CapSense_DpProcessSlider() function pointer*/
    cy_func_capsense_dp_process_csd_matrix_ptr_t fptrDpProcessCsdMatrix;
                                                                /**< The Cy_CapSense_DpProcessCsdMatrix() function pointer*/
    cy_func_capsense_dp_process_csd_touchpad_ptr_t fptrDpProcessCsdTouchpad;
                                                                /**< The Cy_CapSense_DpProcessCsdTouchpad() function pointer*/
    cy_func_capsense_dp_advanced_centroid_touchpad_ptr_t fptrDpAdvancedCentroidTouchpad;
                                                                /**< The Cy_CapSense_DpAdvancedCentroidTouchpad() function pointer*/
    cy_func_capsense_dp_process_proximity_ptr_t fptrDpProcessProximity;
                                                                /**< The Cy_CapSense_DpProcessProximity() function pointer*/
    cy_func_capsense_dp_process_csx_touchpad_ptr_t fptrDpProcessCsxTouchpad;
                                                                /**< The Cy_CapSense_DpProcessCsxTouchpad() function pointer*/
    cy_func_capsense_calibrate_all_csd_widgets_ptr_t fptrCalibrateAllCsdWidgets;
                                                                /**< The Cy_CapSense_CalibrateAllCsdWidgets() function pointer*/
    cy_func_capsense_csd_calibrate_widget_ptr_t fptrCSDCalibrateWidget;
                                                                /**< The Cy_CapSense_CSDCalibrateWidget() function pointer*/
    cy_func_capsense_calibrate_all_csx_widgets_ptr_t fptrCalibrateAllCsxWidgets;
                                                                /**< The Cy_CapSense_CalibrateAllCsxWidgets() function pointer*/
    cy_func_capsense_ss_auto_tune_ptr_t fptrSsAutoTune;         /**< The Cy_CapSense_SsAutoTune() function pointer*/
    cy_func_capsense_run_noise_envelope_lib_ptr_t fptrRunNoiseEnvelopeLib;
                                                                /**< The Cy_CapSense_RunNoiseEnvelope_Lib() function pointer*/
    cy_func_capsense_dp_update_thresholds_ptr_t fptrDpUpdateThresholds;
                                                                /**< The Cy_CapSense_DpUpdateThresholds() function pointer*/
    cy_func_capsense_initialize_noise_envelope_lib_ptr_t fptrInitializeNoiseEnvelopeLib;
                                                                /**< The Cy_CapSense_InitializeNoiseEnvelope_Lib() function pointer*/
} cy_stc_capsense_fptr_config_t;

/** \} */


/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_capsense_high_level *//** \{ */
/******************************************************************************/

uint32_t Cy_CapSense_IsAnyWidgetActive(const cy_stc_capsense_context_t * context);
uint32_t Cy_CapSense_IsWidgetActive(
                uint32_t widgetId, 
                const cy_stc_capsense_context_t * context);
uint32_t Cy_CapSense_IsSensorActive(
                uint32_t widgetId, 
                uint32_t sensorId, 
                const cy_stc_capsense_context_t * context);
uint32_t Cy_CapSense_IsProximitySensorActive(
                uint32_t widgetId, 
                uint32_t sensorId, 
                const cy_stc_capsense_context_t * context);
cy_stc_capsense_touch_t * Cy_CapSense_GetTouchInfo(
                uint32_t widgetId, 
                const cy_stc_capsense_context_t * context);


/** \} */


/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/

cy_status Cy_CapSense_CheckConfigIntegrity(
                const cy_stc_capsense_context_t * context);

/**< Internal wrapper functions for the flash optimization definitions */
cy_status Cy_CapSense_CSDCalibrateWidget_Call(
                uint32_t widgetId,
                uint32_t target,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDSetupWidget_Call(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDScan_Call(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_CSXSetupWidget_Call(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_CSXScan_Call(
                cy_stc_capsense_context_t * context);
cy_status Cy_CapSense_CalibrateAllCsdWidgets_Call(
                cy_stc_capsense_context_t * context);
cy_status Cy_CapSense_CalibrateAllCsxWidgets_Call(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDDisableMode_Call(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_CSDInitialize_Call(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessButton_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessCsxTouchpad_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessProximity_Call(
                cy_stc_capsense_widget_config_t const * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessCsdTouchpad_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessSlider_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessCsdMatrix_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessCsdWidgetStatus_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context);
uint32_t Cy_CapSense_DpProcessCsdWidgetRawCounts_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessCsxWidgetStatus_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context);
uint32_t Cy_CapSense_DpProcessCsxWidgetRawCounts_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_DpAdvancedCentroidTouchpad_Call(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InitPositionFilters_Call(
                uint32_t filterConfig,
                const cy_stc_capsense_position_t * ptrInput,
                cy_stc_capsense_position_t * ptrHistory,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_RunPositionFilters_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_position_t * ptrInput,
                cy_stc_capsense_position_t * ptrHistory,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_ProcessPositionFilters_Call(
                cy_stc_capsense_touch_t * newTouch,
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InitializeAllFilters_Call(
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_FtRunEnabledFiltersInternal_Call(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                uint16_t * ptrSnsRawHistory,
                uint8_t * ptrSnsRawHistoryLow,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_CSXDisableMode_Call(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_CSXInitialize_Call(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_AdaptiveFilterInitialize_Lib_Call(
                const cy_stc_capsense_adaptive_filter_config_t * config,
                cy_stc_capsense_position_t * positionContext,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_AdaptiveFilterRun_Lib_Call(
                const cy_stc_capsense_adaptive_filter_config_t * config,
                cy_stc_capsense_position_t * positionContext,
                uint32_t * currentX,
                uint32_t * currentY,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_BallisticMultiplier_Lib_Call(
                const cy_stc_capsense_ballistic_config_t * config,
                const cy_stc_capsense_touch_t * touch,
                cy_stc_capsense_ballistic_delta_t * delta,
                uint32_t timestamp,
                cy_stc_capsense_ballistic_context_t * ballisticContext,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_DpUpdateThresholds_Call(
                cy_stc_capsense_widget_context_t * ptrWdContext,
                const cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                uint32_t startFlag,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InitializeNoiseEnvelope_Lib_Call(
                uint16_t rawCount,
                uint16_t sigPFC,
                cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_RunNoiseEnvelope_Lib_Call(
                uint16_t rawCount,
                uint16_t sigPFC,
                cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                const cy_stc_capsense_context_t * context);
cy_status Cy_CapSense_SsAutoTune_Call(
                cy_stc_capsense_context_t * context);


/** \} \endcond */


#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCSDV2 */

#endif /* CY_CAPSENSE_STRUCTURE_H */


/* [] END OF FILE */
