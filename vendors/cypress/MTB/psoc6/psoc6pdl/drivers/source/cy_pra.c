/***************************************************************************//**
* \file cy_pra.c
* \version 1.0
*
* \brief The source code file for the PRA driver.
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

#include "cy_pra.h"
#include "cy_pra_cfg.h"
#include "cy_sysint.h"
#include "cy_ipc_drv.h"
#include "cy_gpio.h"
#include "cy_device.h"
#include "cy_syspm.h"

#if defined (CY_DEVICE_SECURE) || defined (CY_DOXYGEN)

#define CY_PRA_REG_POLICY_WRITE_ALL   (0x00000000UL)

/* Table to get register/function address based on its index */
cy_stc_pra_reg_policy_t regIndexToAddr[14U];


#if (CY_CPU_CORTEX_M0P) || defined (CY_DOXYGEN)
    static void Cy_PRA_Handler(void);
    static void Cy_PRA_ProcessCmd(cy_stc_pra_msg_t *message);
    static void Cy_PRA_PmHibernate(uint32_t funcProc);
    static void Cy_PRA_PmCm4DpFlagSet(void);
    static cy_en_pra_status_t Cy_PRA_ClkDSBeforeTransition(void);
    static cy_en_pra_status_t Cy_PRA_ClkDSAfterTransition(void);
#endif /* (CY_CPU_CORTEX_M0P) || defined (CY_DOXYGEN) */


#if (CY_CPU_CORTEX_M0P) || defined (CY_DOXYGEN)
/*******************************************************************************
* Function Name: Cy_PRA_Init
****************************************************************************//**
*
* Initializes the Protected Register Access driver.
*
*******************************************************************************/
void Cy_PRA_Init(void)
{
    for (uint32_t i = 0UL; i < (sizeof(regIndexToAddr)/sizeof(regIndexToAddr[0U])); i++)
    {
        regIndexToAddr[i].writeMask = CY_PRA_REG_POLICY_WRITE_ALL;
    }
    regIndexToAddr[CY_PRA_INDX_SRSS_PWR_LVD_CTL].addr          = &SRSS_PWR_LVD_CTL;
    regIndexToAddr[CY_PRA_INDX_SRSS_SRSS_INTR].addr            = &SRSS_SRSS_INTR;
    regIndexToAddr[CY_PRA_INDX_SRSS_SRSS_INTR_SET].addr        = &SRSS_SRSS_INTR_SET;
    regIndexToAddr[CY_PRA_INDX_SRSS_SRSS_INTR_MASK].addr       = &SRSS_SRSS_INTR_MASK;
    regIndexToAddr[CY_PRA_INDX_SRSS_SRSS_INTR_CFG].addr        = &SRSS_SRSS_INTR_CFG;
    regIndexToAddr[CY_PRA_INDX_SRSS_CLK_ROOT_SELECT_1].addr    = &SRSS_CLK_ROOT_SELECT[1U];
    regIndexToAddr[CY_PRA_INDX_SRSS_CLK_ROOT_SELECT_2].addr    = &SRSS_CLK_ROOT_SELECT[2U];
    regIndexToAddr[CY_PRA_INDX_SRSS_CLK_ROOT_SELECT_3].addr    = &SRSS_CLK_ROOT_SELECT[3U];
    regIndexToAddr[CY_PRA_INDX_SRSS_CLK_ROOT_SELECT_4].addr    = &SRSS_CLK_ROOT_SELECT[4U];
    regIndexToAddr[CY_PRA_INDX_SRSS_CLK_ROOT_SELECT_5].addr    = (CY_SRSS_NUM_HFROOT > 4U) ? &SRSS_CLK_ROOT_SELECT[5U] : 0U;
    regIndexToAddr[CY_PRA_INDX_SRSS_CLK_ROOT_SELECT_6].addr    = (CY_SRSS_NUM_HFROOT > 5U) ? &SRSS_CLK_ROOT_SELECT[6U] : 0U;
    regIndexToAddr[CY_PRA_INDX_FLASHC_FLASH_CMD].addr          = &FLASHC_FLASH_CMD;
    regIndexToAddr[CY_PRA_INDX_SRSS_PWR_HIBERNATE].addr        = &SRSS_PWR_HIBERNATE;
    regIndexToAddr[CY_PRA_INDX_SRSS_PWR_HIBERNATE].writeMask   = (uint32_t) ~ (SRSS_PWR_HIBERNATE_TOKEN_Msk |
                                                                               SRSS_PWR_HIBERNATE_POLARITY_HIBPIN_Msk |
                                                                               SRSS_PWR_HIBERNATE_MASK_HIBPIN_Msk |
                                                                               SRSS_PWR_HIBERNATE_MASK_HIBALARM_Msk |
                                                                               SRSS_PWR_HIBERNATE_MASK_HIBWDT_Msk);

    /* Configure the IPC interrupt handler. */
    Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(CY_IPC_INTR_PRA), CY_PRA_IPC_NONE_INTR, CY_PRA_IPC_CHAN_INTR);
    cy_stc_sysint_t intr = {
        .intrSrc = (IRQn_Type)CY_SYSINT_CM0P_MUX4,
        .cm0pSrc = (cy_en_intr_t)(int32_t) CY_IPC_INTR_NUM_TO_VECT((int32_t) CY_IPC_INTR_PRA),
        .intrPriority = 0UL
    };
    (void) Cy_SysInt_Init(&intr, &Cy_PRA_Handler);
    NVIC_EnableIRQ(intr.intrSrc);
}


/*******************************************************************************
* Function Name: Cy_PRA_Handler
****************************************************************************//**
*
* The IPC interrupt handler called once there is request from non-secure core.
*
*******************************************************************************/
static void Cy_PRA_Handler(void)
{
    cy_stc_pra_msg_t msgLocal;
    cy_stc_pra_msg_t* msgRemote;

    /* Process internal command's copy and update original value */
    msgRemote = (cy_stc_pra_msg_t *)Cy_IPC_Drv_ReadDataValue(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_PRA));

    msgLocal = *msgRemote;
    Cy_PRA_ProcessCmd(&msgLocal);
    *msgRemote = msgLocal;

    /* Clear interrupt logic. Required to detect next interrupt */
    Cy_IPC_Drv_ClearInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(CY_IPC_INTR_PRA), CY_PRA_IPC_NONE_INTR, CY_PRA_IPC_CHAN_INTR);

    (void) Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_PRA), CY_PRA_IPC_NONE_INTR);
}


/*******************************************************************************
* Function Name: Cy_PRA_ProcessCmd
****************************************************************************//**
*
* Process and executes the command received from non-secure core.
*
* \param message \ref cy_stc_pra_msg_t
*
*******************************************************************************/
static void Cy_PRA_ProcessCmd(cy_stc_pra_msg_t *message)
{
    static uint32_t structInit = 0UL;
    static cy_stc_pra_system_config_t structCpy = {0UL};

    if ((CY_PRA_MSG_TYPE_REG32_GET     == message->praCommand) ||
        (CY_PRA_MSG_TYPE_REG32_CLR_SET == message->praCommand) ||
        (CY_PRA_MSG_TYPE_REG32_SET     == message->praCommand))
    {
        /* Check if access is within array range */
        if ((message->praIndex) > (sizeof(regIndexToAddr)/sizeof(regIndexToAddr[0U])))
        {
            message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
        }

        /* Some registers do not exist for some families */
        if (regIndexToAddr[message->praIndex].addr == (const volatile uint32_t *) 0U)
        {
            message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
        }
    }

    switch (message->praCommand)
    {
        case CY_PRA_MSG_TYPE_REG32_CLR_SET:
            if (0U == (message->praData2 & regIndexToAddr[message->praIndex].writeMask))
            {
                uint32_t tmp;

                tmp =  CY_GET_REG32(regIndexToAddr[message->praIndex].addr);
                tmp &= (message->praData1 | regIndexToAddr[message->praIndex].writeMask);
                tmp |= message->praData2;
                CY_SET_REG32(regIndexToAddr[message->praIndex].addr, tmp);
                message->praStatus = CY_PRA_STATUS_SUCCESS;
            }
            else
            {
                message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
            }
            break;

        case CY_PRA_MSG_TYPE_REG32_SET:
            if (0U == (message->praData1 & regIndexToAddr[message->praIndex].writeMask))
            {
                CY_SET_REG32(regIndexToAddr[message->praIndex].addr, message->praData1);
                message->praStatus = CY_PRA_STATUS_SUCCESS;
            }
            else
            {
                message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
            }
            break;

        case CY_PRA_MSG_TYPE_REG32_GET:
            message->praData1 = CY_GET_REG32(regIndexToAddr[message->praIndex].addr);
            message->praStatus = CY_PRA_STATUS_SUCCESS;
            break;

        case CY_PRA_MSG_TYPE_CM0_WAKEUP:
            message->praStatus = CY_PRA_STATUS_SUCCESS;
            break;

        case CY_PRA_MSG_TYPE_SYS_CFG_FUNC:
            structCpy = *((cy_stc_pra_system_config_t *)(message->praData1));
            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
            if((0UL == structInit) && (CY_PRA_STATUS_SUCCESS == message->praStatus))
            {
                structInit = 1UL;
            }
            break;

        case CY_PRA_MSG_TYPE_SECURE_ONLY:
            switch (message->praIndex)
            {
                case CY_PRA_PM_FUNC_HIBERNATE:
                    Cy_PRA_PmHibernate(message->praData1);
                    message->praStatus = CY_PRA_STATUS_SUCCESS;
                    break;

                case CY_PRA_PM_FUNC_CM4_DP_FLAG_SET:
                    Cy_PRA_PmCm4DpFlagSet();
                    message->praStatus = CY_PRA_STATUS_SUCCESS;
                    break;

                case CY_PRA_CLK_FUNC_DS_BEFORE_TRANSITION:
                    message->praStatus = Cy_PRA_ClkDSBeforeTransition();
                    break;

                case CY_PRA_CLK_FUNC_DS_AFTER_TRANSITION:
                    message->praStatus = Cy_PRA_ClkDSAfterTransition();
                    break;

                default:
                    message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
                    break;
            }
            break;

        case CY_PRA_MSG_TYPE_FUNC_POLICY:
            if(0UL != structInit)
            {
                switch (message->praIndex)
                {
                    case CY_PRA_PM_FUNC_LDO_SET_VOLTAGE:
                    {
                        bool powerEnableTmp, ldoEnableTmp, ulpEnableTmp;
                        powerEnableTmp = structCpy.powerEnable; /* old value backup */
                        ldoEnableTmp = structCpy.ldoEnable; /* old value backup */
                        ulpEnableTmp = structCpy.ulpEnable; /* old value backup */
                        structCpy.powerEnable = true;
                        structCpy.ldoEnable = true;
                        structCpy.ldoVoltage = (cy_en_syspm_ldo_voltage_t)message->praData1;
                        if (structCpy.ldoVoltage == CY_SYSPM_LDO_VOLTAGE_0_9V)
                        {
                            structCpy.ulpEnable = true;
                        }
                        else
                        {
                            structCpy.ulpEnable = false;
                        }
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.powerEnable = powerEnableTmp;
                            structCpy.ldoEnable = ldoEnableTmp;
                            structCpy.ulpEnable = ulpEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_PM_FUNC_BUCK_ENABLE:
                    {
                        bool powerEnableTmp, ldoEnableTmp, ulpEnableTmp;
                        powerEnableTmp = structCpy.powerEnable; /* old value backup */
                        ldoEnableTmp = structCpy.ldoEnable; /* old value backup */
                        ulpEnableTmp = structCpy.ulpEnable; /* old value backup */
                        structCpy.powerEnable = true;
                        structCpy.ldoEnable = false;
                        structCpy.buckVoltage = (cy_en_syspm_buck_voltage1_t)message->praData1;
                        if (structCpy.buckVoltage == CY_SYSPM_BUCK_OUT1_VOLTAGE_0_9V)
                        {
                            structCpy.ulpEnable = true;
                        }
                        else
                        {
                            structCpy.ulpEnable = false;
                        }
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.powerEnable = powerEnableTmp;
                            structCpy.ldoEnable = ldoEnableTmp;
                            structCpy.ulpEnable = ulpEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_PM_FUNC_SET_MIN_CURRENT:
                    {
                        bool powerEnableTmp, pwrCurrentModeMinTmp;
                        powerEnableTmp = structCpy.powerEnable;
                        pwrCurrentModeMinTmp = structCpy.pwrCurrentModeMin;
                        structCpy.powerEnable = true;
                        structCpy.pwrCurrentModeMin = true;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.powerEnable = powerEnableTmp;
                            structCpy.pwrCurrentModeMin = pwrCurrentModeMinTmp;
                        }
                    }
                    break;

                    case CY_PRA_PM_FUNC_SET_NORMAL_CURRENT:
                    {
                        bool powerEnableTmp, pwrCurrentModeMinTmp;
                        powerEnableTmp = structCpy.powerEnable;
                        pwrCurrentModeMinTmp = structCpy.pwrCurrentModeMin;
                        structCpy.powerEnable = true;
                        structCpy.pwrCurrentModeMin = false;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.powerEnable = powerEnableTmp;
                            structCpy.pwrCurrentModeMin = pwrCurrentModeMinTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_ECO_DISABLE:
                    {
                        bool ecoEnableTmp;
                        ecoEnableTmp = structCpy.ecoEnable;
                        structCpy.ecoEnable = false;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.ecoEnable = ecoEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_FLL_DISABLE:
                    {
                        bool fllEnableTmp;
                        uint32_t fllOutFreqHzTmp;
                        fllOutFreqHzTmp = structCpy.fllOutFreqHz;
                        fllEnableTmp = structCpy.fllEnable;
                        structCpy.fllEnable = false;
                        structCpy.fllOutFreqHz = 0UL;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.fllEnable = fllEnableTmp;
                            structCpy.fllOutFreqHz = fllOutFreqHzTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PLL_DISABLE:
                    {
                        bool pllEnable;
                        ((message->praData1) == 1UL) ? (pllEnable = structCpy.pll0Enable) : (pllEnable = structCpy.pll1Enable);

                        ((message->praData1) == 1UL) ? (structCpy.pll0Enable = false) : (structCpy.pll1Enable = false);

                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);

                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            ((message->praData1) == 1UL) ? (structCpy.pll0Enable = pllEnable) : (structCpy.pll1Enable = pllEnable);
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_ILO_ENABLE:
                    {
                        bool iloEnableTmp;
                        iloEnableTmp = structCpy.iloEnable;
                        structCpy.iloEnable = true;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.iloEnable = iloEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_ILO_DISABLE:
                    {
                        bool iloEnableTmp;
                        iloEnableTmp = structCpy.iloEnable;
                        structCpy.iloEnable = false;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.iloEnable = iloEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_ILO_HIBERNATE_ON:
                    {
                        bool iloHibernateOnTmp;
                        iloHibernateOnTmp = structCpy.iloHibernateON;
                        structCpy.iloHibernateON = (0UL != message->praData1);
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.iloHibernateON = iloHibernateOnTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PILO_ENABLE:
                    {
                        bool piloEnableTmp;
                        piloEnableTmp = structCpy.piloEnable;
                        structCpy.piloEnable = true;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.piloEnable = piloEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PILO_DISABLE:
                    {
                        bool piloEnableTmp;
                        piloEnableTmp = structCpy.piloEnable;
                        structCpy.piloEnable = false;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.piloEnable = piloEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_WCO_ENABLE:
                    {
                        bool wcoEnableTmp;
                        wcoEnableTmp = structCpy.wcoEnable;
                        structCpy.wcoEnable = true;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.wcoEnable = wcoEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_WCO_DISABLE:
                    {
                        bool wcoEnableTmp;
                        wcoEnableTmp = structCpy.wcoEnable;
                        structCpy.wcoEnable = false;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.wcoEnable = wcoEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_WCO_BYPASS:
                    {
                        bool bypassEnableTmp;
                        bypassEnableTmp = structCpy.bypassEnable;
                        structCpy.bypassEnable = ((cy_en_wco_bypass_modes_t) message->praData1 == CY_SYSCLK_WCO_BYPASSED) ? true : false;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        if (structCpy.wcoEnable)
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                structCpy.bypassEnable = bypassEnableTmp;
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_HF_ENABLE:
                    {
                        bool clkHFEnable;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        switch (message->praData1)
                        {
                            case 0UL:
                            clkHFEnable = structCpy.clkHF0Enable;
                            structCpy.clkHF0Enable = true;
                            break;

                            case 1UL:
                            clkHFEnable = structCpy.clkHF1Enable;
                            structCpy.clkHF1Enable = true;
                            break;

                            case 2UL:
                            clkHFEnable = structCpy.clkHF2Enable;
                            structCpy.clkHF2Enable = true;
                            break;

                            case 3UL:
                            clkHFEnable = structCpy.clkHF3Enable;
                            structCpy.clkHF3Enable = true;
                            break;

                            case 4UL:
                            clkHFEnable = structCpy.clkHF4Enable;
                            structCpy.clkHF4Enable = true;
                            break;

                            case 5UL:
                            clkHFEnable = structCpy.clkHF5Enable;
                            structCpy.clkHF5Enable = true;
                            break;

                            default:
                            clkHFEnable = false;
                            message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
                            break;

                        }
                        if (message->praStatus == CY_PRA_STATUS_SUCCESS)
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                switch (message->praData1)
                                {
                                    case 0UL:
                                    structCpy.clkHF0Enable = clkHFEnable;
                                    break;

                                    case 1UL:
                                    structCpy.clkHF1Enable = clkHFEnable;
                                    break;

                                    case 2UL:
                                    structCpy.clkHF2Enable = clkHFEnable;
                                    break;

                                    case 3UL:
                                    structCpy.clkHF3Enable = clkHFEnable;
                                    break;

                                    case 4UL:
                                    structCpy.clkHF4Enable = clkHFEnable;
                                    break;

                                    case 5UL:
                                    structCpy.clkHF5Enable = clkHFEnable;
                                    break;

                                    default:
                                    break;
                                }
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_HF_DISABLE:
                    {
                        bool clkHFEnable;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        switch (message->praData1)
                        {
                            case 0UL:
                            clkHFEnable = structCpy.clkHF0Enable;
                            structCpy.clkHF0Enable = false;
                            break;

                            case 1UL:
                            clkHFEnable = structCpy.clkHF1Enable;
                            structCpy.clkHF1Enable = false;
                            break;

                            case 2UL:
                            clkHFEnable = structCpy.clkHF2Enable;
                            structCpy.clkHF2Enable = false;
                            break;

                            case 3UL:
                            clkHFEnable = structCpy.clkHF3Enable;
                            structCpy.clkHF3Enable = false;
                            break;

                            case 4UL:
                            clkHFEnable = structCpy.clkHF4Enable;
                            structCpy.clkHF4Enable = false;
                            break;

                            case 5UL:
                            clkHFEnable = structCpy.clkHF5Enable;
                            structCpy.clkHF5Enable = false;
                            break;

                            default:
                            clkHFEnable = false;
                            message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
                            break;

                        }
                        if (message->praStatus == CY_PRA_STATUS_SUCCESS)
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                switch (message->praData1)
                                {
                                    case 0UL:
                                    structCpy.clkHF0Enable = clkHFEnable;
                                    break;

                                    case 1UL:
                                    structCpy.clkHF1Enable = clkHFEnable;
                                    break;

                                    case 2UL:
                                    structCpy.clkHF2Enable = clkHFEnable;
                                    break;

                                    case 3UL:
                                    structCpy.clkHF3Enable = clkHFEnable;
                                    break;

                                    case 4UL:
                                    structCpy.clkHF4Enable = clkHFEnable;
                                    break;

                                    case 5UL:
                                    structCpy.clkHF5Enable = clkHFEnable;
                                    break;

                                    default:
                                    break;
                                }
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_HF_SET_SOURCE:
                    {
                        cy_en_clkhf_in_sources_t hfSource;
                        uint32_t hfOutFreqMHz;
                        bool hfEnabled;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        switch (((cy_stc_pra_clkhfsetsource_t *) message->praData1)->clkHf)
                        {
                            case 0UL:
                            hfSource = structCpy.hf0Source;
                            structCpy.hf0Source = ((cy_stc_pra_clkhfsetsource_t *) message->praData1)->source;
                            hfOutFreqMHz = structCpy.hf0OutFreqMHz;
                            hfEnabled = structCpy.clkHF0Enable;
                            structCpy.hf0OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf0Source)/(1UL << structCpy.hf0Divider))/1000000UL;
                            break;

                            case 1UL:
                            hfSource = structCpy.hf1Source;
                            structCpy.hf1Source = ((cy_stc_pra_clkhfsetsource_t *) message->praData1)->source;
                            hfOutFreqMHz = structCpy.hf1OutFreqMHz;
                            hfEnabled = structCpy.clkHF1Enable;
                            structCpy.hf1OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf1Source)/(1UL << structCpy.hf1Divider))/1000000UL;
                            break;

                            case 2UL:
                            hfSource = structCpy.hf2Source;
                            structCpy.hf2Source = ((cy_stc_pra_clkhfsetsource_t *) message->praData1)->source;
                            hfOutFreqMHz = structCpy.hf2OutFreqMHz;
                            hfEnabled = structCpy.clkHF2Enable;
                            structCpy.hf2OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf2Source)/(1UL << structCpy.hf2Divider))/1000000UL;
                            break;

                            case 3UL:
                            hfSource = structCpy.hf3Source;
                            structCpy.hf3Source = ((cy_stc_pra_clkhfsetsource_t *) message->praData1)->source;
                            hfOutFreqMHz = structCpy.hf3OutFreqMHz;
                            hfEnabled = structCpy.clkHF3Enable;
                            structCpy.hf3OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf3Source)/(1UL << structCpy.hf3Divider))/1000000UL;
                            break;

                            case 4UL:
                            hfSource = structCpy.hf4Source;
                            structCpy.hf4Source = ((cy_stc_pra_clkhfsetsource_t *) message->praData1)->source;
                            hfOutFreqMHz = structCpy.hf4OutFreqMHz;
                            hfEnabled = structCpy.clkHF4Enable;
                            structCpy.hf4OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf4Source)/(1UL << structCpy.hf4Divider))/1000000UL;
                            break;

                            case 5UL:
                            hfSource = structCpy.hf5Source;
                            structCpy.hf5Source = ((cy_stc_pra_clkhfsetsource_t *) message->praData1)->source;
                            hfOutFreqMHz = structCpy.hf5OutFreqMHz;
                            hfEnabled = structCpy.clkHF5Enable;
                            structCpy.hf5OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf5Source)/(1UL << structCpy.hf5Divider))/1000000UL;
                            break;

                            default:
                            hfEnabled = false;
                            message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
                            break;
                        }
                        if ((message->praStatus == CY_PRA_STATUS_SUCCESS) && (hfEnabled))
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                switch (((cy_stc_pra_clkhfsetsource_t *) message->praData1)->clkHf)
                                {
                                    case 0UL:
                                    structCpy.hf0Source = hfSource;
                                    structCpy.hf0OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 1UL:
                                    structCpy.hf1Source = hfSource;
                                    structCpy.hf1OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 2UL:
                                    structCpy.hf2Source = hfSource;
                                    structCpy.hf2OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 3UL:
                                    structCpy.hf3Source = hfSource;
                                    structCpy.hf3OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 4UL:
                                    structCpy.hf4Source = hfSource;
                                    structCpy.hf4OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 5UL:
                                    structCpy.hf5Source = hfSource;
                                    structCpy.hf5OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    default:
                                    break;
                                }
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_HF_SET_DIVIDER:
                    {
                        cy_en_clkhf_dividers_t hfDivider;
                        uint32_t hfOutFreqMHz;
                        bool hfEnabled;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        switch (((cy_stc_pra_clkhfsetdivider_t *) message->praData1)->clkHf)
                        {
                            case 0UL:
                            hfDivider = structCpy.hf0Divider;
                            structCpy.hf0Divider = ((cy_stc_pra_clkhfsetdivider_t *) message->praData1)->divider;
                            hfOutFreqMHz = structCpy.hf0OutFreqMHz;
                            hfEnabled = structCpy.clkHF0Enable;
                            structCpy.hf0OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf0Source)/(1UL << structCpy.hf0Divider))/1000000UL;
                            break;

                            case 1UL:
                            hfDivider = structCpy.hf1Divider;
                            structCpy.hf1Divider = ((cy_stc_pra_clkhfsetdivider_t *) message->praData1)->divider;
                            hfOutFreqMHz = structCpy.hf1OutFreqMHz;
                            hfEnabled = structCpy.clkHF1Enable;
                            structCpy.hf1OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf1Source)/(1UL << structCpy.hf1Divider))/1000000UL;
                            break;

                            case 2UL:
                            hfDivider = structCpy.hf2Divider;
                            structCpy.hf2Divider = ((cy_stc_pra_clkhfsetdivider_t *) message->praData1)->divider;
                            hfOutFreqMHz = structCpy.hf2OutFreqMHz;
                            hfEnabled = structCpy.clkHF2Enable;
                            structCpy.hf2OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf2Source)/(1UL << structCpy.hf2Divider))/1000000UL;
                            break;

                            case 3UL:
                            hfDivider = structCpy.hf3Divider;
                            structCpy.hf3Divider = ((cy_stc_pra_clkhfsetdivider_t *) message->praData1)->divider;
                            hfOutFreqMHz = structCpy.hf3OutFreqMHz;
                            hfEnabled = structCpy.clkHF3Enable;
                            structCpy.hf3OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf3Source)/(1UL << structCpy.hf3Divider))/1000000UL;
                            break;

                            case 4UL:
                            hfDivider = structCpy.hf4Divider;
                            structCpy.hf4Divider = ((cy_stc_pra_clkhfsetdivider_t *) message->praData1)->divider;
                            hfOutFreqMHz = structCpy.hf4OutFreqMHz;
                            hfEnabled = structCpy.clkHF4Enable;
                            structCpy.hf4OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf4Source)/(1UL << structCpy.hf4Divider))/1000000UL;
                            break;

                            case 5UL:
                            hfDivider = structCpy.hf5Divider;
                            structCpy.hf5Divider = ((cy_stc_pra_clkhfsetdivider_t *) message->praData1)->divider;
                            hfOutFreqMHz = structCpy.hf5OutFreqMHz;
                            hfEnabled = structCpy.clkHF5Enable;
                            structCpy.hf5OutFreqMHz = (Cy_SysClk_ClkPathGetFrequency((uint32_t) structCpy.hf5Source)/(1UL << structCpy.hf5Divider))/1000000UL;
                            break;

                            default:
                            hfEnabled = false;
                            message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
                            break;
                        }
                        if ((message->praStatus == CY_PRA_STATUS_SUCCESS) && (hfEnabled))
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                switch (((cy_stc_pra_clkhfsetdivider_t *) message->praData1)->clkHf)
                                {
                                    case 0UL:
                                    structCpy.hf0Divider = hfDivider;
                                    structCpy.hf0OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 1UL:
                                    structCpy.hf1Divider = hfDivider;
                                    structCpy.hf1OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 2UL:
                                    structCpy.hf2Divider = hfDivider;
                                    structCpy.hf2OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 3UL:
                                    structCpy.hf3Divider = hfDivider;
                                    structCpy.hf3OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 4UL:
                                    structCpy.hf4Divider = hfDivider;
                                    structCpy.hf4OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    case 5UL:
                                    structCpy.hf5Divider = hfDivider;
                                    structCpy.hf5OutFreqMHz = hfOutFreqMHz;
                                    break;

                                    default:
                                    break;
                                }
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_FAST_SET_DIVIDER:
                    {
                        uint8_t clkFastDivTmp;
                        bool clkFastEnableTmp;
                        clkFastEnableTmp = structCpy.clkFastEnable;
                        clkFastDivTmp = structCpy.clkFastDiv;
                        structCpy.clkFastEnable = true;
                        structCpy.clkFastDiv =  (uint8_t)(message->praData1);
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkFastDiv = clkFastDivTmp;
                            structCpy.clkFastEnable = clkFastEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PERI_SET_DIVIDER:
                    {
                        uint8_t clkPeriDivTmp;
                        bool clkPeriEnableTmp;
                        clkPeriDivTmp = structCpy.clkPeriDiv;
                        clkPeriEnableTmp = structCpy.clkPeriEnable;
                        structCpy.clkPeriEnable = true;
                        structCpy.clkPeriDiv =  (uint8_t)(message->praData1);
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkPeriDiv = clkPeriDivTmp;
                            structCpy.clkPeriEnable = clkPeriEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_LF_SET_SOURCE:
                    {
                        cy_en_clklf_in_sources_t clkLfSourceTmp;
                        bool clkLFEnableTmp;
                        clkLFEnableTmp = structCpy.clkLFEnable;
                        clkLfSourceTmp = structCpy.clkLfSource;
                        structCpy.clkLFEnable = true;
                        structCpy.clkLfSource = (cy_en_clklf_in_sources_t)message->praData1;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkLfSource = clkLfSourceTmp;
                            structCpy.clkLFEnable = clkLFEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_TIMER_SET_SOURCE:
                    {
                        cy_en_clktimer_in_sources_t clkTimerSourceTmp;
                        clkTimerSourceTmp = structCpy.clkTimerSource;
                        structCpy.clkTimerSource = (cy_en_clktimer_in_sources_t)message->praData1;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        if (structCpy.clkTimerEnable)
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                structCpy.clkTimerSource = clkTimerSourceTmp;
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_TIMER_SET_DIVIDER:
                    {
                        uint8_t clkTimerDividerTmp;
                        clkTimerDividerTmp = structCpy.clkTimerDivider;
                        structCpy.clkTimerDivider = (uint8_t)(message->praData1);
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        if (structCpy.clkTimerEnable)
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                structCpy.clkTimerDivider = clkTimerDividerTmp;
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_TIMER_ENABLE:
                    {
                        bool clkTimerEnableTmp;
                        clkTimerEnableTmp = structCpy.clkTimerEnable;
                        structCpy.clkTimerEnable = true;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkTimerEnable = clkTimerEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_TIMER_DISABLE:
                    {
                        bool clkTimerEnableTmp;
                        clkTimerEnableTmp = structCpy.clkTimerEnable;
                        structCpy.clkTimerEnable = false;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkTimerEnable = clkTimerEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PUMP_SET_SOURCE:
                    {
                        cy_en_clkpump_in_sources_t pumpSourceTmp;
                        pumpSourceTmp = structCpy.pumpSource;
                        structCpy.pumpSource = (cy_en_clkpump_in_sources_t)message->praData1;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        if (structCpy.clkPumpEnable)
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                structCpy.pumpSource = pumpSourceTmp;
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PUMP_SET_DIVIDER:
                    {
                        cy_en_clkpump_divide_t pumpDividerTmp;
                        pumpDividerTmp = structCpy.pumpDivider;
                        structCpy.pumpDivider = (cy_en_clkpump_divide_t)message->praData1;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        if (structCpy.clkPumpEnable)
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                structCpy.pumpDivider = pumpDividerTmp;
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PUMP_ENABLE:
                    {
                        bool clkPumpEnableTmp;
                        clkPumpEnableTmp = structCpy.clkPumpEnable;
                        structCpy.clkPumpEnable = true;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkPumpEnable = clkPumpEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PUMP_DISABLE:
                    {
                        bool clkPumpEnableTmp;
                        clkPumpEnableTmp = structCpy.clkPumpEnable;
                        structCpy.clkPumpEnable = false;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkPumpEnable = clkPumpEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_BAK_SET_SOURCE:
                    {
                        cy_en_clkbak_in_sources_t clkBakSourceTmp;
                        bool clkBakEnableTmp;
                        clkBakEnableTmp = structCpy.clkBakEnable;
                        clkBakSourceTmp = structCpy.clkBakSource;
                        structCpy.clkBakEnable = true;
                        structCpy.clkBakSource = (cy_en_clkbak_in_sources_t)message->praData1;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkBakSource = clkBakSourceTmp;
                            structCpy.clkBakEnable = clkBakEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_ECO_CONFIGURE:
                    {
                        /* If ECO is enabled then return error */
                        if (structCpy.ecoEnable)
                        {
                            message->praStatus = CY_PRA_STATUS_ERROR_PROCESSING_ECO_ENABLED;
                        }
                        else
                        {
                            structCpy.ecoFreqHz = ((cy_stc_pra_clk_eco_configure_t *) message->praData1)->praClkEcofreq;
                            structCpy.ecoLoad = ((cy_stc_pra_clk_eco_configure_t *) message->praData1)->praCsum;
                            structCpy.ecoEsr = ((cy_stc_pra_clk_eco_configure_t *) message->praData1)->praEsr;
                            structCpy.ecoDriveLevel = ((cy_stc_pra_clk_eco_configure_t *) message->praData1)->praDriveLevel;
                            message->praStatus = CY_PRA_STATUS_SUCCESS;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_ECO_ENABLE:
                    {
                        bool ecoEnableTmp;
                        ecoEnableTmp = structCpy.ecoEnable;
                        structCpy.ecoEnable = true;
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.ecoEnable = ecoEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PATH_SET_SOURCE:
                    {
                        bool pathEnable = false;
                        cy_en_clkpath_in_sources_t pathSrc = CY_SYSCLK_CLKPATH_IN_IMO;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                        switch (((cy_stc_pra_clkpathsetsource_t *) message->praData1)->clk_path)
                        {
                            case 0UL:
                            pathEnable = structCpy.path0Enable;
                            pathSrc = structCpy.path0Src;
                            structCpy.path0Enable = true;
                            structCpy.path0Src = ((cy_stc_pra_clkpathsetsource_t *) message->praData1)->source;
                            break;

                            case 1UL:
                            pathEnable = structCpy.path1Enable;
                            pathSrc = structCpy.path1Src;
                            structCpy.path1Enable = true;
                            structCpy.path1Src = ((cy_stc_pra_clkpathsetsource_t *) message->praData1)->source;
                            break;

                            case 2UL:
                            pathEnable = structCpy.path2Enable;
                            pathSrc = structCpy.path2Src;
                            structCpy.path2Enable = true;
                            structCpy.path2Src = ((cy_stc_pra_clkpathsetsource_t *) message->praData1)->source;
                            break;

                            case 3UL:
                            pathEnable = structCpy.path3Enable;
                            pathSrc = structCpy.path3Src;
                            structCpy.path3Enable = true;
                            structCpy.path3Src = ((cy_stc_pra_clkpathsetsource_t *) message->praData1)->source;
                            break;

                            case 4UL:
                            pathEnable = structCpy.path4Enable;
                            pathSrc = structCpy.path4Src;
                            structCpy.path4Enable = true;
                            structCpy.path4Src = ((cy_stc_pra_clkpathsetsource_t *) message->praData1)->source;
                            break;

                            case 5UL:
                            pathEnable = structCpy.path5Enable;
                            pathSrc = structCpy.path5Src;
                            structCpy.path5Enable = true;
                            structCpy.path5Src = ((cy_stc_pra_clkpathsetsource_t *) message->praData1)->source;
                            break;

                            default:
                            message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
                            break;
                        }
                        if (message->praStatus == CY_PRA_STATUS_SUCCESS)
                        {
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                /* restore path */
                                switch (((cy_stc_pra_clkpathsetsource_t *) message->praData1)->clk_path)
                                {
                                    case 0UL:
                                    structCpy.path0Enable = pathEnable;
                                    structCpy.path0Src = pathSrc;
                                    break;

                                    case 1UL:
                                    structCpy.path1Enable = pathEnable;
                                    structCpy.path1Src = pathSrc;
                                    break;

                                    case 2UL:
                                    structCpy.path2Enable = pathEnable;
                                    structCpy.path2Src = pathSrc;
                                    break;

                                    case 3UL:
                                    structCpy.path3Enable = pathEnable;
                                    structCpy.path3Src = pathSrc;
                                    break;

                                    case 4UL:
                                    structCpy.path4Enable = pathEnable;
                                    structCpy.path4Src = pathSrc;
                                    break;

                                    case 5UL:
                                    structCpy.path5Enable = pathEnable;
                                    structCpy.path5Src = pathSrc;
                                    break;

                                    default:
                                    message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
                                    break;
                                }
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_FLL_MANCONFIG:
                    {
                        structCpy.fllMult = ((cy_stc_fll_manual_config_t *) message->praData1)->fllMult;
                        structCpy.fllRefDiv = ((cy_stc_fll_manual_config_t *) message->praData1)->refDiv;
                        structCpy.fllCcoRange = ((cy_stc_fll_manual_config_t *) message->praData1)->ccoRange;
                        structCpy.enableOutputDiv = ((cy_stc_fll_manual_config_t *) message->praData1)->enableOutputDiv;
                        structCpy.lockTolerance = ((cy_stc_fll_manual_config_t *) message->praData1)->lockTolerance;
                        structCpy.igain = ((cy_stc_fll_manual_config_t *) message->praData1)->igain;
                        structCpy.pgain = ((cy_stc_fll_manual_config_t *) message->praData1)->pgain;
                        structCpy.settlingCount = ((cy_stc_fll_manual_config_t *) message->praData1)->settlingCount;
                        structCpy.outputMode = ((cy_stc_fll_manual_config_t *) message->praData1)->outputMode;
                        structCpy.ccoFreq = ((cy_stc_fll_manual_config_t *) message->praData1)->cco_Freq;
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                    }
                    break;

                    case CY_PRA_CLK_FUNC_FLL_ENABLE:
                    {
                        if (structCpy.fllEnable == true) /* FLL is already enabled */
                        {
                            message->praStatus = CY_PRA_STATUS_ERROR_PROCESSING_FLL0_ENABLED;
                        }
                        else
                        {
                            structCpy.fllEnable = true;
                            /* calculate fll output frequency */
                            structCpy.fllOutFreqHz = Cy_PRA_CalculateFLLOutFreq(&structCpy);
                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                structCpy.fllOutFreqHz = 0UL;
                                structCpy.fllEnable = false;
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PLL_MANCONFIG:
                    {
                        if((((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->clkPath == 0UL) ||
                           (((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->clkPath > CY_SRSS_NUM_PLL))
                        {
                            message->praStatus = CY_PRA_STATUS_INVALID_PARAM_PLL_NUM;
                        }
                        else
                        {
                            if(((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->clkPath == 1UL)
                            {
                                structCpy.pll0FeedbackDiv = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->feedbackDiv;
                                structCpy.pll0ReferenceDiv = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->referenceDiv;
                                structCpy.pll0OutputDiv = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->outputDiv;
                                structCpy.pll0LfMode = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->lfMode;
                                structCpy.pll0OutputMode = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->outputMode;
                            }
                            else
                            {
                                structCpy.pll1FeedbackDiv = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->feedbackDiv;
                                structCpy.pll1ReferenceDiv = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->referenceDiv;
                                structCpy.pll1OutputDiv = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->outputDiv;
                                structCpy.pll1LfMode = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->lfMode;
                                structCpy.pll1OutputMode = ((cy_stc_pra_clk_pll_manconfigure_t *) message->praData1)->praConfig->outputMode;
                            }
                            message->praStatus = CY_PRA_STATUS_SUCCESS;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_PLL_ENABLE:
                    {
                        if ((((message->praData1) == 1UL) && (structCpy.pll0Enable == true))
                            || (((message->praData1) == 2UL) && (structCpy.pll1Enable == true))
                            || (message->praData1 > CY_SRSS_NUM_PLL) || (message->praData1 == 0UL))
                        {
                            message->praStatus = CY_PRA_STATUS_ERROR_PROCESSING_PLL_ENABLED;
                        }
                        else
                        {
                            if ((message->praData1) == 1UL)
                            {
                                structCpy.pll0OutFreqHz = Cy_PRA_CalculatePLLOutFreq(1U, &structCpy);
                                structCpy.pll0Enable = true;
                            }
                            else
                            {
                                structCpy.pll1OutFreqHz = Cy_PRA_CalculatePLLOutFreq(2U, &structCpy);
                                structCpy.pll1Enable = true;
                            }

                            message->praStatus = Cy_PRA_SystemConfig(&structCpy);

                            if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                            {
                                if ((message->praData1) == 1UL)
                                {
                                    structCpy.pll0OutFreqHz = 0UL;
                                    structCpy.pll0Enable = false;
                                }
                                else
                                {
                                    structCpy.pll1OutFreqHz = 0UL;
                                    structCpy.pll1Enable = false;
                                }
                            }
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_SLOW_SET_DIVIDER:
                    {
                        uint8_t clkSlowDivTmp;
                        bool clkSlowEnableTmp;
                        clkSlowEnableTmp = structCpy.clkSlowEnable;
                        clkSlowDivTmp = structCpy.clkSlowDiv;
                        structCpy.clkSlowEnable = true;
                        structCpy.clkSlowDiv = (uint8_t)(message->praData1);
                        message->praStatus = Cy_PRA_SystemConfig(&structCpy);
                        if (message->praStatus != CY_PRA_STATUS_SUCCESS)
                        {
                            structCpy.clkSlowDiv = clkSlowDivTmp;
                            structCpy.clkSlowEnable = clkSlowEnableTmp;
                        }
                    }
                    break;

                    case CY_PRA_CLK_FUNC_EXT_CLK_SET_FREQUENCY:
                    {
                        structCpy.extClkFreqHz = (uint32_t)(message->praData1);
                        message->praStatus = CY_PRA_STATUS_SUCCESS;
                    }
                    break;

                    default:
                        message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
                        break;
                }
            }
            else
            {
                message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
            }
            break;

        default:
            message->praStatus = CY_PRA_STATUS_ACCESS_DENIED;
           break;
    }

}
#endif /* (CY_CPU_CORTEX_M0P) */


#if (CY_CPU_CORTEX_M4) || defined (CY_DOXYGEN)

/*******************************************************************************
* Function Name: Cy_PRA_SendCmd
****************************************************************************//**
*
* Process and executes the command received from non-secure core.
*
* \param cmd Command to be executed on secure side.
* \param regIndex Index of the function or register depending on command parameter.
* \param clearMask Data send to secure core.
* \param setMask Additional data send to secure core.
*
* \return Status of the command execution. For register read command the read
* value is returned.
*
*******************************************************************************/
cy_en_pra_status_t Cy_PRA_SendCmd(uint16_t cmd, uint16_t regIndex, uint32_t clearMask, uint32_t setMask)
{
    cy_en_pra_status_t status;
    CY_ALIGN(4UL) cy_stc_pra_msg_t ipcMsg;
    IPC_STRUCT_Type *ipcPraBase = Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_PRA);
    uint32_t interruptState;

    ipcMsg.praCommand = cmd;
    ipcMsg.praStatus  = CY_PRA_STATUS_REQUEST_SENT;
    ipcMsg.praIndex   = regIndex;
    ipcMsg.praData1   = clearMask;
    ipcMsg.praData2   = setMask;

    interruptState = Cy_SysLib_EnterCriticalSection();

    while (CY_IPC_DRV_SUCCESS != Cy_IPC_Drv_SendMsgWord(ipcPraBase, CY_PRA_IPC_NOTIFY_INTR, (uint32_t)&ipcMsg))
    {
        /* Try to acquire the PRA IPC structure and pass the arguments */
    }

    /* Checks whether the IPC structure is not locked */
    while (Cy_IPC_Drv_IsLockAcquired(ipcPraBase))
    {
        /* Polls whether the IPC is released */
    }

    Cy_SysLib_ExitCriticalSection(interruptState);

    /* Cortex-M0+ has updated ipcMsg variable */

    status = (cy_en_pra_status_t) ipcMsg.praStatus;

    if (CY_PRA_STATUS_ACCESS_DENIED == status)
    {
        CY_HALT();
    }

    if (CY_PRA_MSG_TYPE_SYS_CFG_FUNC == ipcMsg.praCommand)
    {
        SystemCoreClockUpdate();
    }

    if (CY_PRA_MSG_TYPE_REG32_GET == ipcMsg.praCommand)
    {
        status = (cy_en_pra_status_t)ipcMsg.praData1;
    }

    return status;
}
#endif /* (CY_CPU_CORTEX_M4) */


#if (CY_CPU_CORTEX_M0P) || defined (CY_DOXYGEN)

/* The mask to unlock the Hibernate power mode */
#define HIBERNATE_UNLOCK_VAL                 ((uint32_t) 0x3Au << SRSS_PWR_HIBERNATE_UNLOCK_Pos)

/* The mask to set the Hibernate power mode */
#define SET_HIBERNATE_MODE                   ((HIBERNATE_UNLOCK_VAL |\
                                               SRSS_PWR_HIBERNATE_FREEZE_Msk |\
                                               SRSS_PWR_HIBERNATE_HIBERNATE_Msk))

/* The mask to retain the Hibernate power mode status */
#define HIBERNATE_RETAIN_STATUS_MASK         ((SRSS_PWR_HIBERNATE_TOKEN_Msk |\
                                               SRSS_PWR_HIBERNATE_MASK_HIBALARM_Msk |\
                                               SRSS_PWR_HIBERNATE_MASK_HIBWDT_Msk |\
                                               SRSS_PWR_HIBERNATE_POLARITY_HIBPIN_Msk |\
                                               SRSS_PWR_HIBERNATE_MASK_HIBPIN_Msk))

/** The mask for the Hibernate wakeup sources */
#define HIBERNATE_WAKEUP_MASK               ((SRSS_PWR_HIBERNATE_MASK_HIBALARM_Msk |\
                                              SRSS_PWR_HIBERNATE_MASK_HIBWDT_Msk |\
                                              SRSS_PWR_HIBERNATE_POLARITY_HIBPIN_Msk |\
                                              SRSS_PWR_HIBERNATE_MASK_HIBPIN_Msk))

/** The define to update the token to indicate the transition into Hibernate */
#define HIBERNATE_TOKEN                    ((uint32_t) 0x1BU << SRSS_PWR_HIBERNATE_TOKEN_Pos)


/*******************************************************************************
* Function Name: Cy_PRA_PmHibernate
****************************************************************************//**
*
* Update SRSS_PWR_HIBERNATE register for Cy_SysPm_SystemEnterHibernate and
* Cy_SysPm_IoUnfreeze functions.
*
*******************************************************************************/
static void Cy_PRA_PmHibernate(uint32_t funcProc)
{

    if(0UL == funcProc)
    {
        /* Preserve the token that will be retained through a wakeup sequence.
         * This could be used by Cy_SysLib_GetResetReason() to differentiate
         * Wakeup from a general reset event.
         * Preserve the wakeup source(s) configuration.
         */
        SRSS_PWR_HIBERNATE = (SRSS_PWR_HIBERNATE & HIBERNATE_WAKEUP_MASK) | HIBERNATE_TOKEN;

        /* Disable overriding by the peripherals the next pin-freeze command */
        SRSS_PWR_HIBERNATE |= SET_HIBERNATE_MODE;

        /* The second write causes freezing of I/O cells to save the I/O-cell state */
        SRSS_PWR_HIBERNATE |= SET_HIBERNATE_MODE;

        /* Third write cause system to enter Hibernate */
        SRSS_PWR_HIBERNATE |= SET_HIBERNATE_MODE;
    }
    else
    {
        /* Preserve the last reset reason and wakeup polarity. Then, unfreeze I/O:
         * write PWR_HIBERNATE.FREEZE=0, .UNLOCK=0x3A, .HIBERANTE=0
         */
        SRSS_PWR_HIBERNATE = (SRSS_PWR_HIBERNATE & HIBERNATE_RETAIN_STATUS_MASK) | HIBERNATE_UNLOCK_VAL;

        /* Lock the Hibernate mode:
        * write PWR_HIBERNATE.HIBERNATE=0, UNLOCK=0x00, HIBERANTE=0
        */
        SRSS_PWR_HIBERNATE &= HIBERNATE_RETAIN_STATUS_MASK;
    }
}


/*******************************************************************************
* Function Name: Cy_PRA_PmCm4DpFlagSet
****************************************************************************//**
*
* Set Deep Sleep Flag for the CM4 core
*
*******************************************************************************/
static void Cy_PRA_PmCm4DpFlagSet(void)
{
    uint32_t ddftStructData;

    /* Acquire the IPC to prevent changing of the shared resources at the same time */
    while (0U == _FLD2VAL(IPC_STRUCT_ACQUIRE_SUCCESS, REG_IPC_STRUCT_ACQUIRE(CY_IPC_STRUCT_PTR(CY_IPC_CHAN_DDFT))))
    {
        /* Wait until the IPC structure is released by another CPU */
    }

    ddftStructData = REG_IPC_STRUCT_DATA(CY_IPC_STRUCT_PTR(CY_IPC_CHAN_DDFT));

    /* Update CM4 core deep sleep mask */
    ddftStructData |= (0x01UL << 28u);

    /* Update pointer to the latest saved structure */
    REG_IPC_STRUCT_DATA(CY_IPC_STRUCT_PTR(CY_IPC_CHAN_DDFT)) = ddftStructData;

    /* Release the IPC */
    REG_IPC_STRUCT_RELEASE(CY_IPC_STRUCT_PTR(CY_IPC_CHAN_DDFT)) = 0U;

    /* Read the release value to make sure it is set */
    (void) REG_IPC_STRUCT_RELEASE(CY_IPC_STRUCT_PTR(CY_IPC_CHAN_DDFT));
}

/* Timeout count for use in function Cy_PRA_ClkDeepSleepCallback() is sufficiently large for ~1 second */
#define CY_PRA_TIMEOUT (1000000UL)
/* These variables act as locks to prevent collisions between clock measurement and entry into
   Deep Sleep mode. See Cy_SysClk_DeepSleep(). */
static uint16_t changedSourcePaths = 0U;
static uint16_t pllAutoModes = 0U;

/*******************************************************************************
* Function Name: Cy_PRA_ClkDSBeforeTransition
****************************************************************************//**
*
* SysClock before deep sleep transition.
*
*******************************************************************************/
static cy_en_pra_status_t Cy_PRA_ClkDSBeforeTransition(void)
{
    uint32_t fllPll; /* 0 = FLL, all other values = a PLL */

    /* Initialize the storage of changed paths */
    changedSourcePaths = 0U;
    pllAutoModes = 0U;

    /* For FLL and each PLL */
    for (fllPll = 0UL; fllPll <= CY_SRSS_NUM_PLL; fllPll++)
    {
        /* If FLL or PLL is enabled */
        if ((0UL == fllPll) ? Cy_SysClk_FllIsEnabled() : Cy_SysClk_PllIsEnabled(fllPll))
        {
            /* And the FLL/PLL has ECO as a source */
            if (Cy_SysClk_ClkPathGetSource(fllPll) == CY_SYSCLK_CLKPATH_IN_ECO)
            {
                /* Bypass the FLL/PLL */
                if (0UL == fllPll)
                {
                    CY_REG32_CLR_SET(SRSS_CLK_FLL_CONFIG3, SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, CY_SYSCLK_FLLPLL_OUTPUT_INPUT);
                }
                else
                {
                    if (((uint32_t)CY_SYSCLK_FLLPLL_OUTPUT_AUTO  == _FLD2VAL(SRSS_CLK_PLL_CONFIG_BYPASS_SEL, SRSS_CLK_PLL_CONFIG[fllPll - 1UL])) ||
                        ((uint32_t)CY_SYSCLK_FLLPLL_OUTPUT_AUTO1 == _FLD2VAL(SRSS_CLK_PLL_CONFIG_BYPASS_SEL, SRSS_CLK_PLL_CONFIG[fllPll - 1UL])))
                    {
                        pllAutoModes |= (uint16_t)(1UL << fllPll);
                    }

                    CY_REG32_CLR_SET(SRSS_CLK_PLL_CONFIG[fllPll - 1UL], SRSS_CLK_PLL_CONFIG_BYPASS_SEL, CY_SYSCLK_FLLPLL_OUTPUT_INPUT);
                }

                /* Change this path source to IMO */
                (void)Cy_SysClk_ClkPathSetSource(fllPll, CY_SYSCLK_CLKPATH_IN_IMO);

                /* Store a record that this path source was changed from ECO */
                changedSourcePaths |= (uint16_t)(1UL << fllPll);
            }
            else if (0UL == fllPll)
            {
                CY_REG32_CLR_SET(SRSS_CLK_FLL_CONFIG3, SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, CY_SYSCLK_FLLPLL_OUTPUT_INPUT);
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    return CY_PRA_STATUS_SUCCESS;
}

/*******************************************************************************
* Function Name: Cy_PRA_ClkDSAfterTransition
****************************************************************************//**
*
* SysClock after deep sleep transition.
*
*******************************************************************************/
static cy_en_pra_status_t Cy_PRA_ClkDSAfterTransition(void)
{
    /* Bitmapped paths with enabled FLL/PLL sourced by ECO */
    uint32_t timeout = CY_PRA_TIMEOUT;
    cy_en_pra_status_t retVal = CY_PRA_STATUS_ERROR_SYSPM_TIMEOUT;

    /* After return from Deep Sleep, for each FLL/PLL, if needed, restore the source to ECO.
     * And block until the FLL/PLL has regained its frequency lock.
     */
    if (0U != changedSourcePaths)
    {
        /* If any FLL/PLL was sourced by the ECO, timeout wait for the ECO to become fully stabilized again */
        while ((CY_SYSCLK_ECOSTAT_STABLE != Cy_SysClk_EcoGetStatus()) && (0UL != timeout))
        {
            timeout--;
        }

        if (0UL != timeout)
        {
            uint32_t fllPll; /* 0 = FLL, all other values = PLL */

            for (fllPll = 0UL; fllPll <= CY_SRSS_NUM_PLL; fllPll++)
            {
                /* If there is a correspondent record about a changed clock source */
                if (0U != (changedSourcePaths & (uint16_t)(1UL << fllPll)))
                {
                    /* Change this path source back to ECO */
                    (void)Cy_SysClk_ClkPathSetSource(fllPll, CY_SYSCLK_CLKPATH_IN_ECO);

                    /* Timeout wait for FLL/PLL to regain lock.
                     * Split FLL and PLL lock polling loops into two separate threads to minimize one polling loop duration.
                     */
                    if (0UL == fllPll)
                    {
                        while ((!Cy_SysClk_FllLocked()) && (0UL != timeout))
                        {
                            timeout--;
                        }
                    }
                    else
                    {
                        while ((!Cy_SysClk_PllLocked(fllPll)) && (0UL != timeout))
                        {
                            timeout--;
                        }
                    }

                    if (0UL != timeout)
                    {
                        /* Undo bypass the FLL/PLL */
                        if (0UL == fllPll)
                        {
                            CY_REG32_CLR_SET(SRSS_CLK_FLL_CONFIG3, SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, CY_SYSCLK_FLLPLL_OUTPUT_OUTPUT);
                        }
                        else
                        {
                            if (0U != (pllAutoModes & (uint16_t)(1UL << fllPll)))
                            {
                                CY_REG32_CLR_SET(SRSS_CLK_PLL_CONFIG[fllPll - 1UL], SRSS_CLK_PLL_CONFIG_BYPASS_SEL, CY_SYSCLK_FLLPLL_OUTPUT_AUTO);
                            }
                            else
                            {
                                CY_REG32_CLR_SET(SRSS_CLK_PLL_CONFIG[fllPll - 1UL], SRSS_CLK_PLL_CONFIG_BYPASS_SEL, CY_SYSCLK_FLLPLL_OUTPUT_OUTPUT);
                            }
                        }

                        retVal = CY_PRA_STATUS_SUCCESS;
                    }
                }
            }
        }
    }
    else if (Cy_SysClk_FllIsEnabled())
    {
        /* Timeout wait for FLL to regain lock */
        while ((!Cy_SysClk_FllLocked()) && (0UL != timeout))
        {
            timeout--;
        }

        if (0UL != timeout)
        {
            /* Undo bypass the FLL */
            CY_REG32_CLR_SET(SRSS_CLK_FLL_CONFIG3, SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, CY_SYSCLK_FLLPLL_OUTPUT_OUTPUT);
            retVal = CY_PRA_STATUS_SUCCESS;
        }
    }
    else
    {
        retVal = CY_PRA_STATUS_SUCCESS;
    }

    return (retVal);
}


#endif /* (CY_CPU_CORTEX_M0P) */

#endif /* (CY_DEVICE_SECURE) */


/* [] END OF FILE */
