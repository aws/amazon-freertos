/***************************************************************************//**
* \file cy_pra_cfg.h
* \version 1.0
*
* \brief The header file of the PRA driver.
*
********************************************************************************
* \copyright
* Copyright 2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#if !defined(CY_PRA_CFG_H)
#define CY_PRA_CFG_H

#include <stdint.h>
#include <stdbool.h>
#include "cy_pra.h"
#include "cy_sysclk.h"
#include "cy_systick.h"
#include "cy_ble_clk.h"
#include "cy_device_headers.h"

#if defined (CY_DEVICE_SECURE) || defined (CY_DOXYGEN)

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
*                            Type Definitions
*******************************************************************************/

/**
* \addtogroup group_pra_data_structures_cfg
* \{
*/

/** System configuration structure */
typedef struct
{
    bool powerEnable;                                   /**< Power is enabled or disabled */
    bool ldoEnable;                                     /**< Core Regulator */
    bool pmicEnable;                                    /**< Power using external PMIC output */
    bool vBackupVDDDEnable;                             /**< vBackup source using VDD or Direct supply */
    bool ulpEnable;                                     /**< System Active Power mode is ULP */
    bool ecoEnable;                                     /**< ECO Enable */
    bool extClkEnable;                                  /**< EXTCLK Enable */
    bool iloEnable;                                     /**< ILO Enable */
    bool wcoEnable;                                     /**< WCO Enable */
    bool fllEnable;                                     /**< FLL Enable */
    bool pll0Enable;                                    /**< PLL0 Enable */
    bool pll1Enable;                                    /**< PLL1 Enable */
    bool path0Enable;                                   /**< CLKPATH0 Enable */
    bool path1Enable;                                   /**< CLKPATH1 Enable */
    bool path2Enable;                                   /**< CLKPATH2 Enable */
    bool path3Enable;                                   /**< CLKPATH3 Enable */
    bool path4Enable;                                   /**< CLKPATH4 Enable */
    bool path5Enable;                                   /**< CLKPATH5 Enable */
    bool clkFastEnable;                                 /**< CLKFAST Enable */
    bool clkPeriEnable;                                 /**< CLKPERI Enable */
    bool clkSlowEnable;                                 /**< CLKSLOW Enable */
    bool clkHF0Enable;                                  /**< CLKHF0 Enable */
    bool clkHF1Enable;                                  /**< CLKHF1 Enable */
    bool clkHF2Enable;                                  /**< CLKHF2 Enable */
    bool clkHF3Enable;                                  /**< CLKHF3 Enable */
    bool clkHF4Enable;                                  /**< CLKHF4 Enable */
    bool clkHF5Enable;                                  /**< CLKHF5 Enable */
    bool clkPumpEnable;                                 /**< CLKPUMP Enable */
    bool clkLFEnable;                                   /**< CLKLF Enable */
    bool clkBakEnable;                                  /**< CLKBAK Enable */
    bool clkTimerEnable;                                /**< CLKTIMER Enable */
    bool clkAltSysTickEnable;                           /**< CLKALTSYSTICK Enable */
    bool piloEnable;                                    /**< PILO Enable */
    bool clkAltHfEnable;                                /**< BLE ECO Clock Enable */

    /* Power */
    cy_en_syspm_ldo_voltage_t ldoVoltage;               /**< LDO Voltage (LP or ULP) */
    cy_en_syspm_buck_voltage1_t buckVoltage;            /**< Buck Voltage */
    bool pwrCurrentModeMin;                             /**< Minimum core regulator current mode */

    /* System clocks */
    /* IMO is always enabled */
    /* ECO Configuration */
    uint32_t ecoFreqHz;                                 /**< ECO Frequency in Hz */
    uint32_t ecoLoad;                                   /**< Parallel Load Capacitance (pF) */
    uint32_t ecoEsr;                                    /**< Equivalent series resistance (ohm) */
    uint32_t ecoDriveLevel;                             /**< Drive Level (uW) */
    GPIO_PRT_Type *ecoInPort;                           /**< ECO input port */
    GPIO_PRT_Type *ecoOutPort;                          /**< ECO output port */
    uint32_t ecoInPinNum;                               /**< ECO input pin number */
    uint32_t ecoOutPinNum;                              /**< ECO output pin number */

    /* EXTCLK Configuration */
    uint32_t extClkFreqHz;                              /**< External clock frequency in Hz */
    GPIO_PRT_Type *extClkPort;                          /**< External connection port */
    uint32_t extClkPinNum;                              /**< External connection pin */
    en_hsiom_sel_t extClkHsiom;                         /**< IO mux value */

    /* ILO Configuration */
    bool iloHibernateON;                                /**< Run in Hibernate Mode */

    /* WCO Configuration */
    bool bypassEnable;                                  /**< Clock port bypass to External sine wave or to normal crystal */
    GPIO_PRT_Type *wcoInPort;                           /**< WCO Input port */
    GPIO_PRT_Type *wcoOutPort;                          /**< WCO Output port */
    uint32_t wcoInPinNum;                               /**< WCO Input pin */
    uint32_t wcoOutPinNum;                              /**< WCO Output pin */

    /* FLL Configuration */
    uint32_t fllOutFreqHz;                              /**< FLL Output Frequency in Hz */
    uint32_t                    fllMult;                /**< CLK_FLL_CONFIG  register, FLL_MULT bits */
    uint16_t                    fllRefDiv;              /**< CLK_FLL_CONFIG2 register, FLL_REF_DIV bits */
    cy_en_fll_cco_ranges_t      fllCcoRange;            /**< CLK_FLL_CONFIG4 register, CCO_RANGE bits */
    bool                        enableOutputDiv;        /**< CLK_FLL_CONFIG  register, FLL_OUTPUT_DIV bit */
    uint16_t                    lockTolerance;          /**< CLK_FLL_CONFIG2 register, LOCK_TOL bits */
    uint8_t                     igain;                  /**< CLK_FLL_CONFIG3 register, FLL_LF_IGAIN bits */
    uint8_t                     pgain;                  /**< CLK_FLL_CONFIG3 register, FLL_LF_PGAIN bits */
    uint16_t                    settlingCount;          /**< CLK_FLL_CONFIG3 register, SETTLING_COUNT bits */
    cy_en_fll_pll_output_mode_t outputMode;             /**< CLK_FLL_CONFIG3 register, BYPASS_SEL bits */
    uint16_t                    ccoFreq;                /**< CLK_FLL_CONFIG4 register, CCO_FREQ bits */

    /* Number of PLL available for the device is defined in CY_SRSS_NUM_PLL
     * Max 2 instances of PLLs are defined */

    /* PLL0 Configuration */
    uint32_t pll0OutFreqHz;                             /**< PLL0 output frequency in Hz */
    uint8_t                     pll0FeedbackDiv;        /**< PLL0 CLK_PLL_CONFIG register, FEEDBACK_DIV (P) bits */
    uint8_t                     pll0ReferenceDiv;       /**< PLL0 CLK_PLL_CONFIG register, REFERENCE_DIV (Q) bits */
    uint8_t                     pll0OutputDiv;          /**< PLL0 CLK_PLL_CONFIG register, OUTPUT_DIV bits */
    bool                        pll0LfMode;             /**< PLL0 CLK_PLL_CONFIG register, PLL_LF_MODE bit */
    cy_en_fll_pll_output_mode_t pll0OutputMode;         /**< PLL0 CLK_PLL_CONFIG register, BYPASS_SEL bits */

    /* PLL1 Configuration */
    uint32_t pll1OutFreqHz;                             /**< PLL1 output frequency in Hz */
    uint8_t                     pll1FeedbackDiv;        /**< PLL1 CLK_PLL_CONFIG register, FEEDBACK_DIV (P) bits */
    uint8_t                     pll1ReferenceDiv;       /**< PLL1 CLK_PLL_CONFIG register, REFERENCE_DIV (Q) bits */
    uint8_t                     pll1OutputDiv;          /**< PLL1 CLK_PLL_CONFIG register, OUTPUT_DIV bits */
    bool                        pll1LfMode;             /**< PLL1 CLK_PLL_CONFIG register, PLL_LF_MODE bit */
    cy_en_fll_pll_output_mode_t pll1OutputMode;         /**< PLL1 CLK_PLL_CONFIG register, BYPASS_SEL bits */

    /* Number of clock Path available for device is defined in CY_SRSS_NUM_CLKPATH.
     * Max 6 clock path are defined */

    /* Clock Paths Configuration */
    cy_en_clkpath_in_sources_t path0Src;                /**< Input multiplexer0 clock source */
    cy_en_clkpath_in_sources_t path1Src;                /**< Input multiplexer1 clock source */
    cy_en_clkpath_in_sources_t path2Src;                /**< Input multiplexer2 clock source */
    cy_en_clkpath_in_sources_t path3Src;                /**< Input multiplexer3 clock source */
    cy_en_clkpath_in_sources_t path4Src;                /**< Input multiplexer4 clock source */
    cy_en_clkpath_in_sources_t path5Src;                /**< Input multiplexer5 clock source */

    /* Clock Dividers */
    uint8_t clkFastDiv;                                 /**< Fast clock divider. User has to pass actual divider-1 */
    uint8_t clkPeriDiv;                                 /**< Peri clock divider. User has to pass actual divider-1 */
    uint8_t clkSlowDiv;                                 /**< Slow clock divider. User has to pass actual divider-1 */

    /* Number of HF clocks are defined in device specific header CY_SRSS_NUM_HFROOT
     * Max 6 HFs are defined */
     /* HF Configurations */
    cy_en_clkhf_in_sources_t hf0Source;                 /**< HF0 Source Clock Path */
    cy_en_clkhf_dividers_t hf0Divider;                  /**< HF0 Divider */
    uint32_t hf0OutFreqMHz;                             /**< HF0 Output Frequency in MHz */
    cy_en_clkhf_in_sources_t hf1Source;                 /**< HF1 Source Clock Path */
    cy_en_clkhf_dividers_t hf1Divider;                  /**< HF1 Divider */
    uint32_t hf1OutFreqMHz;                             /**< HF1 Output Frequency in MHz */
    cy_en_clkhf_in_sources_t hf2Source;                 /**< HF2 Source Clock Path */
    cy_en_clkhf_dividers_t hf2Divider;                  /**< HF2 Divider */
    uint32_t hf2OutFreqMHz;                             /**< HF2 Output Frequency in MHz */
    cy_en_clkhf_in_sources_t hf3Source;                 /**< HF3 Source Clock Path */
    cy_en_clkhf_dividers_t hf3Divider;                  /**< HF3 Divider */
    uint32_t hf3OutFreqMHz;                             /**< HF3 Output Frequency in MHz */
    cy_en_clkhf_in_sources_t hf4Source;                 /**< HF4 Source Clock Path */
    cy_en_clkhf_dividers_t hf4Divider;                  /**< HF4 Divider */
    uint32_t hf4OutFreqMHz;                             /**< HF4 Output Frequency in MHz */
    cy_en_clkhf_in_sources_t hf5Source;                 /**< HF5 Source Clock Path */
    cy_en_clkhf_dividers_t hf5Divider;                  /**< HF5 Divider */
    uint32_t hf5OutFreqMHz;                             /**< HF5 Output Frequency in MHz */
    cy_en_clkpump_in_sources_t pumpSource;              /**< PUMP Source Clock Path */
    cy_en_clkpump_divide_t pumpDivider;                 /**< PUMP Divider */

    /* Misc clock configurations */
    cy_en_clklf_in_sources_t clkLfSource;               /**< Clock LF Source */
    cy_en_clkbak_in_sources_t clkBakSource;             /**< Clock Backup domain Source */
    cy_en_clktimer_in_sources_t clkTimerSource;         /**< Clock Timer Source */
    uint8_t clkTimerDivider;                            /**< Clock Timer Divider */
    cy_en_systick_clock_source_t clkSrcAltSysTick;      /**< SysTick Source */

    /* BLE ECO */
    uint32_t altHFcLoad;                                /**< Load Cap (pF) */
    uint32_t altHFxtalStartUpTime;                      /**< Startup Time (us) */
    uint32_t altHFfreq;                                 /**< Output Frequency */
    uint32_t altHFsysClkDiv;                            /**< Clock Divider */
    uint32_t altHFvoltageReg;                           /**< BLE Voltage Regulator */
} cy_stc_pra_system_config_t;
/** \} group_pra_data_structures_cfg */


/*******************************************************************************
*        Function Prototypes
*******************************************************************************/

/**
* \addtogroup group_pra_functions
* \{
*/

cy_en_pra_status_t Cy_PRA_SystemConfig(const cy_stc_pra_system_config_t *devConfig);

/** \} group_pra_functions */

#if (CY_CPU_CORTEX_M0P) || defined (CY_DOXYGEN)
uint32_t Cy_PRA_CalculateFLLOutFreq(const cy_stc_pra_system_config_t *devConfig);
uint32_t Cy_PRA_CalculatePLLOutFreq(uint8_t pll, const cy_stc_pra_system_config_t *devConfig);
#endif /* (CY_CPU_CORTEX_M0P) || defined (CY_DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* (CY_DEVICE_SECURE) */

#endif /* #if !defined(CY_PRA_CFG_H) */

/* [] END OF FILE */
