/** @file mw300_pmu.h
*
*  @brief This file contains PMU driver module header
*
*  (C) Copyright 2012-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/****************************************************************************//*
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_PMU_H__
#define __MW300_PMU_H__

#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  PMU
 *  @{
 */

/** @defgroup PMU_Public_Types PMU_Public_Types
 *  @{
 */

/** 
 *  @brief PMU enable or disable definition 
 */

/** 
 *  @brief sleep mode definition 
 */
typedef enum
{
  PMU_PM1 = 0,                            /*!< Power Mode 1 */
  PMU_PM2,                                /*!< Power Mode 2 */
  PMU_PM3,                                /*!< Power Mode 3 */
  PMU_PM4,                                /*!< Power Mode 4 */
}PMU_SleepMode_Type;

/**
 *  @brief wakeup pin source list
 */
typedef enum
{
  PMU_GPIO22_INT,                        /*!< Wakeup Int PIN GPIO22 */
  PMU_GPIO23_INT,                        /*!< Wakeup Int PIN GPIO23 */
}PMU_WakeupPin_Type;

/**
 *  @brief wakeup pin trigger mode list
 */
typedef enum
{
  PMU_WAKEUP_LEVEL_LOW = 0,              /*!< Wakeup at low level */
  PMU_WAKEUP_LEVEL_HIGH,                 /*!< Wakeup at high level */
}PMU_WakeupTriggerMode_Type; 

/** 
 *  @brief last reset cause list 
 */
typedef enum
{
  PMU_BROWNOUT_VBAT   = 0,               /*!< Vbat brown-out reset */
  PMU_CM3_SYSRESETREQ,                   /*!< CM3 SYSRESETREQ reset */
  PMU_CM3_LOCKUP,                        /*!< CM3 lockup reset */
  PMU_WDTOUT,                            /*!< WDT-out reset */
  PMU_RESETSRC_ALL,                      /*!< All reset source */
}PMU_LastResetCause_Type;

/**
 *  @brief wakeup source list
 */
typedef enum
{
  PMU_WAKEUP_PIN0 = 0,                    /*!< Wakeup source pin0: GPIO22 */
  PMU_WAKEUP_PIN1,                        /*!< Wakeup source pin1: GPIO23 */
  PMU_WAKEUP_WLAN,                        /*!< Wakeup source WLAN */
  PMU_WAKEUP_RTC,                         /*!< Wakeup source RTC */
  PMU_WAKEUP_ULPCOMP,                     /*!< Wakeup source ULPCOMP */
}PMU_WakeupSource_Type; 

/** 
 *  @brief vddio domain list 
 */
typedef enum
{
  PMU_VDDIO_0 = 0,                         /*!< VDDIO domain 0 */
                                           /*!< GPIO_0/1/2/3/4/5/6/7/8/9/10/11/12/13/14/15 */
                                           
  PMU_VDDIO_1,                             /*!< VDDIO domain 1 */
                                           /*!< GPIO_16/17/18/19/20/21 */
                                           
  PMU_VDDIO_AON,                           /*!< VDDIO domain AON */
                                           /*!< RESETN/GPIO_22:WAKEUP_0/23:WAKEUP_1/24:OSC32K/25:XTAL32K_IN/26:XTAL32K_OUT */  
                                           
  PMU_VDDIO_2,                             /*!< VDDIO domain 2 */
                                           /*!< GPIO_28/29/30/31/32/33 */  
                                           
  PMU_VDDIO_3,                             /*!< VDDIO domain 3 */
                                           /*!< GPIO_27/34/35/36/37/38/39/40/41/42/43/44/45/46/47/48/49 */
                                           
}PMU_VDDIODOMAIN_Type;

/** 
 *  @brief vddio level list 
 */
typedef enum
{
  PMU_VDDIO_LEVEL_3P3V = 0,                 /*!< VDDIO 3.3V */
  PMU_VDDIO_LEVEL_1P8V = 1,                 /*!< VDDIO 1.8V */     
  PMU_VDDIO_LEVEL_2P5V = 2,                 /*!< VDDIO 2.5V */ 
}PMU_VDDIOLEVEL_Type;

/** 
 *  @brief extra interrupt list 
 */
typedef enum
{
  PMU_INT34_GPIO_0,                         /*!< INT_34 from GPIO_0 */
  PMU_INT34_GPIO_1,                         /*!< INT_34 from GPIO_1 */
 
  PMU_INT35_GPIO_2,                         /*!< INT_35 from GPIO_2 */
  PMU_INT35_GPIO_3,                         /*!< INT_35 from GPIO_3 */
  
  PMU_INT36_GPIO_4,                         /*!< INT_36 from GPIO_4 */
  PMU_INT36_GPIO_5,                         /*!< INT_36 from GPIO_5 */
  
  PMU_INT37_GPIO_6,                         /*!< INT_37 from GPIO_6 */
  PMU_INT37_GPIO_7,                         /*!< INT_37 from GPIO_7 */
  
  PMU_INT38_GPIO_8,                         /*!< INT_38 from GPIO_8 */
  PMU_INT38_GPIO_9,                         /*!< INT_38 from GPIO_9 */
  
  PMU_INT39_GPIO_10,                        /*!< INT_39 from GPIO_10 */
  PMU_INT39_GPIO_11,                        /*!< INT_39 from GPIO_11 */ 
  
  PMU_INT40_GPIO_12,                        /*!< INT_40 from GPIO_12 */
  PMU_INT40_GPIO_13,                        /*!< INT_40 from GPIO_13 */
  
  PMU_INT41_GPIO_14,                        /*!< INT_41 from GPIO_14 */
  PMU_INT41_GPIO_15,                        /*!< INT_41 from GPIO_15 */
  
  PMU_INT42_GPIO_16,                        /*!< INT_42 from GPIO_16 */
  PMU_INT42_GPIO_17,                        /*!< INT_42 from GPIO_17 */
  
  PMU_INT43_GPIO_18,                        /*!< INT_43 from GPIO_18 */
  PMU_INT43_GPIO_19,                        /*!< INT_43 from GPIO_19 */
  
  PMU_INT44_GPIO_20,                        /*!< INT_44 from GPIO_20 */
  PMU_INT44_GPIO_21,                        /*!< INT_44 from GPIO_21 */

  PMU_INT45_GPIO_22,                        /*!< INT_45 from GPIO_22 */
  PMU_INT45_GPIO_23,                        /*!< INT_45 from GPIO_23 */
  
  PMU_INT46_GPIO_24,                        /*!< INT_46 from GPIO_24 */
  PMU_INT46_GPIO_25,                        /*!< INT_46 from GPIO_25 */
  
  PMU_INT47_GPIO_26,                        /*!< INT_47 from GPIO_26 */
  PMU_INT47_GPIO_27,                        /*!< INT_47 from GPIO_27 */
  
  PMU_INT48_GPIO_28,                        /*!< INT_48 from GPIO_28 */
  PMU_INT48_GPIO_29,                        /*!< INT_48 from GPIO_29 */
  
  PMU_INT49_GPIO_30,                        /*!< INT_49 from GPIO_30 */     
  PMU_INT49_GPIO_31,                        /*!< INT_49 from GPIO_31 */ 
  
  PMU_INT50_GPIO_32,                        /*!< INT_50 from GPIO_32 */
  PMU_INT50_GPIO_33,                        /*!< INT_50 from GPIO_33 */   
  
  PMU_INT51_GPIO_34,                        /*!< INT_51 from GPIO_34 */
  PMU_INT51_GPIO_35,                        /*!< INT_51 from GPIO_35 */ 
  
  PMU_INT52_GPIO_36,                        /*!< INT_52 from GPIO_36 */
  PMU_INT52_GPIO_37,                        /*!< INT_52 from GPIO_37 */
  
  PMU_INT53_GPIO_38,                        /*!< INT_53 from GPIO_38 */
  PMU_INT53_GPIO_39,                        /*!< INT_53 from GPIO_39 */
  
  PMU_INT54_GPIO_40,                        /*!< INT_54 from GPIO_40 */
  PMU_INT54_GPIO_41,                        /*!< INT_54 from GPIO_41 */
  
  PMU_INT55_GPIO_42,                        /*!< INT_55 from GPIO_42 */
  PMU_INT55_GPIO_43,                        /*!< INT_55 from GPIO_43 */  
  
  PMU_INT56_GPIO_44,                        /*!< INT_56 from GPIO_44 */
  PMU_INT56_GPIO_45,                        /*!< INT_56 from GPIO_45 */
  
  PMU_INT57_GPIO_46,                        /*!< INT_57 from GPIO_46 */
  PMU_INT57_GPIO_47,                        /*!< INT_57 from GPIO_47 */ 
  
  PMU_INT58_GPIO_48,                        /*!< INT_58 from GPIO_48 */
  PMU_INT58_GPIO_49,                        /*!< INT_58 from GPIO_49 */
}PMU_EXTRAINT_Type;

/**
 *  @brief ulpcomp mode list
 */
typedef enum
{
  PMU_ULPCOMP_MODE_SINGLE = 0x0,            /*!< Ulpcomp single-ended mode */
  PMU_ULPCOMP_MODE_DIFFERENTIAL,            /*!< Ulpcomp differential mode */
}PMU_UlpcompMode_Type;

/**
 *  @brief ulpcomp reference voltage list
 */
typedef enum
{
  PMU_ULPCOMP_REFVOLT_0 = 0x0,              /*!< Ulpcomp reference voltage level 0 */
  PMU_ULPCOMP_REFVOLT_1,                    /*!< Ulpcomp reference voltage level 1 */
  PMU_ULPCOMP_REFVOLT_2,                    /*!< Ulpcomp reference voltage level 2 */
  PMU_ULPCOMP_REFVOLT_3,                    /*!< Ulpcomp reference voltage level 3 */
  PMU_ULPCOMP_REFVOLT_4,                    /*!< Ulpcomp reference voltage level 4 */
  PMU_ULPCOMP_REFVOLT_5,                    /*!< Ulpcomp reference voltage level 5 */
  PMU_ULPCOMP_REFVOLT_6,                    /*!< Ulpcomp reference voltage level 6 */
  PMU_ULPCOMP_REFVOLT_7,                    /*!< Ulpcomp reference voltage level 7 */
}PMU_UlpcompRef_Type;

/**
 *  @brief ulpcomp hysteresis list
 */
typedef enum
{
  PMU_ULPCOMP_HYST_0 = 0x0,                 /*!< Ulpcomp hysteresis level 0 */
  PMU_ULPCOMP_HYST_1,                       /*!< Ulpcomp hysteresis level 1 */
  PMU_ULPCOMP_HYST_2,                       /*!< Ulpcomp hysteresis level 2 */
  PMU_ULPCOMP_HYST_3,                       /*!< Ulpcomp hysteresis level 3 */
}PMU_UlpcompHyst_Type;

/**
 *  @brief vbat brownout detect trigger voltage list
 */
typedef enum
{
  PMU_VBAT_BRNDET_TRIGVOLT_0 = 0x0,         /*!< Vbat brndet trigger voltage level 0 */
  PMU_VBAT_BRNDET_TRIGVOLT_1,               /*!< Vbat brndet trigger voltage level 1 */
  PMU_VBAT_BRNDET_TRIGVOLT_2,               /*!< Vbat brndet trigger voltage level 2 */
  PMU_VBAT_BRNDET_TRIGVOLT_3,               /*!< Vbat brndet trigger voltage level 3 */
  PMU_VBAT_BRNDET_TRIGVOLT_4,               /*!< Vbat brndet trigger voltage level 4 */
  PMU_VBAT_BRNDET_TRIGVOLT_5,               /*!< Vbat brndet trigger voltage level 5 */
  PMU_VBAT_BRNDET_TRIGVOLT_6,               /*!< Vbat brndet trigger voltage level 6 */
  PMU_VBAT_BRNDET_TRIGVOLT_7,               /*!< Vbat brndet trigger voltage level 7 */
}PMU_VbatBrndetTrig_Type;

/**
 *  @brief vbat brownout detect hysteresis list
 */
typedef enum
{
  PMU_VBAT_BRNDET_HYST_0 = 0x0,             /*!< Vbat brndet hysteresis level 0 */
  PMU_VBAT_BRNDET_HYST_1,                   /*!< Vbat brndet hysteresis level 1 */
  PMU_VBAT_BRNDET_HYST_2,                   /*!< Vbat brndet hysteresis level 2 */
  PMU_VBAT_BRNDET_HYST_3,                   /*!< Vbat brndet hysteresis level 3 */
}PMU_VbatBrndetHyst_Type;

/**
 *  @brief vbat brownout detect filter list
 */
typedef enum
{
  PMU_VBAT_BRNDET_FILT_0 = 0x0,             /*!< Vbat brndet filter level 0 */
  PMU_VBAT_BRNDET_FILT_1,                   /*!< Vbat brndet filter level 1 */
  PMU_VBAT_BRNDET_FILT_2,                   /*!< Vbat brndet filter level 2 */
  PMU_VBAT_BRNDET_FILT_3,                   /*!< Vbat brndet filter level 3 */
}PMU_VbatBrndetFilt_Type;

/**
 *  @brief vbat brownout detect configure type
 */
typedef struct
{
  PMU_VbatBrndetTrig_Type brnTrigVolt;      /*!< Configure vbat brndet trigger voltage level 
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_0 (0): level 0
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_1 (1): level 1
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_2 (2): level 2
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_3 (3): level 3
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_4 (4): level 4
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_5 (5): level 5
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_6 (6): level 6
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_7 (7): level 7 */
  PMU_VbatBrndetHyst_Type brnHyst;          /*!< Configure vbat brndet hysteresis level
                                                                                            PMU_VBAT_BRNDET_HYST_0 (0): level 0
                                                                                            PMU_VBAT_BRNDET_HYST_1 (1): level 1
                                                                                            PMU_VBAT_BRNDET_HYST_2 (2): level 2
                                                                                            PMU_VBAT_BRNDET_HYST_3 (3): level 3 */
  PMU_VbatBrndetFilt_Type brnFilter;        /*!< Configure vbat brndet filter level
                                                                                            PMU_VBAT_BRNDET_FILT_0 (0): level 0
                                                                                            PMU_VBAT_BRNDET_FILT_1 (1): level 1
                                                                                            PMU_VBAT_BRNDET_FILT_2 (2): level 2
                                                                                            PMU_VBAT_BRNDET_FILT_3 (3): level 3 */
  
}PMU_VbatBrndetConfig_Type;


/** @defgroup PMU_Public_Constants
 *  @{
 */

/**
 *  @brief Total pmu power mode number 
 */
#define PMU_POWER_MODE_NUMBER                   4

/**
 *  @brief Total wakeup source number 
 */
#define PMU_WAKEUP_SOURCE_NUMBER                5

/**
 *  @brief all the last reset cause
 */
#define PMU_LAST_RST_CAUSE_ALL                   0x39

/**
 *  @brief Total VDDIO domain number 
 */
#define PMU_VDDIO_DOMAIN_NUMBER                 5

/**
 *  @brief Total VDDIO level number 
 */
#define PMU_VDDIO_LEVEL_NUMBER                  3

/**
 *  @brief Total ulpcomp reference voltage or brndet trigger voltage level number 
 */
#define PMU_VOLTAGE_LEVEL_NUMBER                8

/**
 *  @brief Total ulpcomp hysteresis or brndet hysteresis level number 
 */
#define PMU_HYSTERESIS_LEVEL_NUMBER             4

/**
 *  @brief Total brndet filter level number 
 */
#define PMU_BRNDET_FILTER_LEVEL_NUMBER          4

/**        
 *  @brief pmu power mode check
 */
#define IS_PMU_POWER_MODE(POWER_MODE)     ((POWER_MODE) < PMU_POWER_MODE_NUMBER)

/**        
 *  @brief pmu wakeup pin check
 */
#define IS_PMU_WAKEUP_PIN_TYPE(PIN_TYPE)  (((PIN_TYPE) == PMU_GPIO22_INT) || \
                                           ((PIN_TYPE) == PMU_GPIO23_INT))

/**        
 *  @brief pmu wakeup trigger mode check
 */
#define IS_PMU_WAKEUP_TRIGGER_MODE(TRIGGER_MODE)  (((TRIGGER_MODE) == PMU_WAKEUP_LEVEL_HIGH) || \
                                                   ((TRIGGER_MODE) == PMU_WAKEUP_LEVEL_LOW))

/**        
 *  @brief pmu wakeup source check
 */
#define IS_PMU_WAKEUP_SOURCE_TYPE(SRC_TYPE)   ((SRC_TYPE) < PMU_WAKEUP_SOURCE_NUMBER)

/**        
 *  @brief pmu VDDIO domain check
 */
#define IS_PMU_VDDIO_DOMAIN_TYPE(DOMAIN_TYPE) ((DOMAIN_TYPE) < PMU_VDDIO_DOMAIN_NUMBER)

/**        
 *  @brief pmu VDDIO level check
 */
#define IS_PMU_VDDIO_LEVEL_TYPE(LEVEL_TYPE) ((LEVEL_TYPE) < PMU_VDDIO_LEVEL_NUMBER)

/**        
 *  @brief pmu ulpcomp mode check
 */
#define IS_PMU_ULPCOMP_MODE(ULPCOMP_MODE)   (((ULPCOMP_MODE) == PMU_ULPCOMP_MODE_SINGLE) || \
                                             ((ULPCOMP_MODE) == PMU_ULPCOMP_MODE_DIFFERENTIAL))

/**        
 *  @brief pmu ulpcomp reference voltage check
 */
#define IS_PMU_ULPCOMP_REFVOLT_TYPE(VOLT_TYPE)  ((VOLT_TYPE) < PMU_VOLTAGE_LEVEL_NUMBER)

/**        
 *  @brief pmu ulpcomp hysteresis check
 */
#define IS_PMU_ULPCOMP_HYST_TYPE(HYST_TYPE)   ((HYST_TYPE) < PMU_HYSTERESIS_LEVEL_NUMBER)

/**        
 *  @brief pmu vbat brndet trigger voltage check
 */
#define IS_PMU_VBAT_BRNDET_TRIG_TYPE(VOLT_TYPE)   ((VOLT_TYPE) < PMU_VOLTAGE_LEVEL_NUMBER)

/**        
 *  @brief pmu vbat brndet hysteresis check
 */
#define IS_PMU_VBAT_BRNDET_HYST_TYPE(HYST_TYPE)    ((HYST_TYPE) < PMU_HYSTERESIS_LEVEL_NUMBER)

/**        
 *  @brief pmu vbat brndet filter check
 */
#define IS_PMU_VBAT_BRNDET_FILT_TYPE(FILT_TYPE)    ((FILT_TYPE) < PMU_BRNDET_FILTER_LEVEL_NUMBER)

/** @defgroup PMU_Command
 *  @{
 */

/*@} end of group PMU_Command */

/** @defgroup PMU_Interrupt_Flag
 *  @{
 */

/*@} end of group PMU_Interrupt_Flag */

/*@} end of group PMUPublic_Constants */

/** @defgroup PMU_Public_Macro
 *  @{
 */

/*@} end of group PMU_Public_Macro */

/** @defgroup PMU_Public_FunctionDeclaration
 *  @brief PMU functions statement
 *  @{
 */
void PMU_SetSleepMode(PMU_SleepMode_Type pmuMode);
uint32_t PMU_GetSleepMode(void);
void PMU_ConfigWakeupPin(PMU_WakeupPin_Type wakeupPin, PMU_WakeupTriggerMode_Type trigmode);
uint32_t PMU_GetLastResetCause(void);
void PMU_ClrLastResetCause(PMU_LastResetCause_Type resetCause);
void PMU_ClearWakeupSrcInt(PMU_WakeupSource_Type wakeupSrc);
FlagStatus PMU_GetLastWakeupStatus(PMU_WakeupSource_Type wakeupSrc);
void PMU_WakeupSrcIntMask(PMU_WakeupSource_Type wakeupSrc, IntMask_Type intMask);
void PMU_PowerOnVDDIO(PMU_VDDIODOMAIN_Type domain);
void PMU_PowerOffVDDIO(PMU_VDDIODOMAIN_Type domain);
void PMU_PowerDownVDDIO(PMU_VDDIODOMAIN_Type domain);
void PMU_PowerNormalVDDIO(PMU_VDDIODOMAIN_Type domain);
void PMU_ConfigVDDIOLevel(PMU_VDDIODOMAIN_Type domain, PMU_VDDIOLEVEL_Type level);
void PMU_ConfigExtraInterrupt(PMU_EXTRAINT_Type gpioPin);
void PMU_UlpcompModeSelect(PMU_UlpcompMode_Type ulpcompMode);
void PMU_UlpcompRefVoltageSel(PMU_UlpcompRef_Type refVolType);
void PMU_UlpcompHysteresisSel(PMU_UlpcompHyst_Type hystType);
void PMU_UlpcompCmd(FunctionalState state);
FlagStatus PMU_GetUlpcompStatus(void);
uint32_t PMU_GetUlpcompOutValue(void);
void PMU_ConfigVbatBrndet(PMU_VbatBrndetConfig_Type* brndetConfig);
void PMU_VbatBrndetCmd(FunctionalState state);
void PMU_VbatBrndetRstCmd(FunctionalState state);
FlagStatus PMU_GetVbatBrndetStatus(void);
uint32_t PMU_GetVbatBrndetOutValue(void);
void PMU_VbatBrndetIntCmd(FunctionalState state);
void PMU_PowerOnWLAN(void);
void PMU_PowerDownWLAN(void);
void PMU_MCIWakeupWLANEnable(void);
void PMU_MCIWakeupWLANDisable(void);

/*@} end of group PMU_Public_FunctionDeclaration */

/*@} end of group PMU */

/*@} end of group MW300_Periph_Driver */
#endif
