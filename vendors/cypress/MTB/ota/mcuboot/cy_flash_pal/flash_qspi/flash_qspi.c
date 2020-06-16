/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 ***************************************************************************/
/**
* \file flash_qspi.c
* \version 1.0
*
* \brief
*  This is the source file of external flash driver adaptation layer between PSoC6
*  and standard MCUBoot code.
*
********************************************************************************
* \copyright
*
* (c) 2020, Cypress Semiconductor Corporation
* or a subsidiary of Cypress Semiconductor Corporation. All rights
* reserved.
*
* This software, including source code, documentation and related
* materials ("Software"), is owned by Cypress Semiconductor
* Corporation or one of its subsidiaries ("Cypress") and is protected by
* and subject to worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-
* exclusive, non-transferable license to copy, modify, and compile the
* Software source code solely for use in connection with Cypress?s
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. Cypress reserves the right to make
* changes to the Software without notice. Cypress does not assume any
* liability arising out of the application or use of the Software or any
* product or circuit described in the Software. Cypress does not
* authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*
******************************************************************************/
#include "cy_pdl.h"
#include <stdio.h>
#include "flash_qspi.h"

#define CY_SMIF_SYSCLK_HFCLK_DIVIDER     CY_SYSCLK_CLKHF_DIVIDE_BY_4

/* This is the board specific stuff that should align with your board.
 *
 * QSPI resources:
 * SS  - P11_2
 * D3  - P11_3
 * D2  - P11_4
 * D1  - P11_5
 * D0  - P11_6
 * SCK - P11_7
 * SMIF Block - SMIF0
 *
 */

static GPIO_PRT_Type *SS0Port = GPIO_PRT11;
static int SS0Pin = 2;
static en_hsiom_sel_t SS0MuxPort = P11_2_SMIF_SPI_SELECT0;

static GPIO_PRT_Type *SS1Port = GPIO_PRT11;
static int SS1Pin = 0;
// static en_hsiom_sel_t SS1MuxPort = P11_0_SMIF_SPI_SELECT2;
static en_hsiom_sel_t SS1MuxPort = P11_0_GPIO;

static GPIO_PRT_Type *D3Port = GPIO_PRT11;
static int D3Pin = 3;
static en_hsiom_sel_t D3MuxPort = P11_3_SMIF_SPI_DATA3;

static GPIO_PRT_Type *D2Port = GPIO_PRT11;
static int D2Pin = 4;
static en_hsiom_sel_t D2MuxPort = P11_4_SMIF_SPI_DATA2;

static GPIO_PRT_Type *D1Port = GPIO_PRT11;
static int D1Pin = 5;
static en_hsiom_sel_t D1MuxPort = P11_5_SMIF_SPI_DATA1;

static GPIO_PRT_Type *D0Port = GPIO_PRT11;
static int D0Pin = 6;
static en_hsiom_sel_t D0MuxPort = P11_6_SMIF_SPI_DATA0;

static GPIO_PRT_Type *SCKPort = GPIO_PRT11;
static int SCKPin = 7;
static en_hsiom_sel_t SCKMuxPort = P11_7_SMIF_SPI_CLK;

static SMIF_Type *QSPIPort  = SMIF0;

cy_stc_smif_mem_cmd_t sfdpcmd =
{
    .command = 0x5A,
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    .mode = 0xFFFFFFFFU,
    .dummyCycles = 8,
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
};

static cy_stc_smif_mem_cmd_t rdcmd0;
static cy_stc_smif_mem_cmd_t wrencmd0;
static cy_stc_smif_mem_cmd_t wrdiscmd0;
static cy_stc_smif_mem_cmd_t erasecmd0;
static cy_stc_smif_mem_cmd_t chiperasecmd0;
static cy_stc_smif_mem_cmd_t pgmcmd0;
static cy_stc_smif_mem_cmd_t readsts0;
static cy_stc_smif_mem_cmd_t readstsqecmd0;
static cy_stc_smif_mem_cmd_t writestseqcmd0;

static cy_stc_smif_mem_device_cfg_t dev_sfdp_0 =
{
    .numOfAddrBytes = 4,
    .readSfdpCmd = &sfdpcmd,
    .readCmd = &rdcmd0,
    .writeEnCmd = &wrencmd0,
    .writeDisCmd = &wrdiscmd0,
    .programCmd = &pgmcmd0,
    .eraseCmd = &erasecmd0,
    .chipEraseCmd = &chiperasecmd0,
    .readStsRegWipCmd = &readsts0,
    .readStsRegQeCmd = &readstsqecmd0,
    .writeStsRegQeCmd = &writestseqcmd0,
};

static cy_stc_smif_mem_config_t mem_sfdp_0 =
{
    /* The base address the memory slave is mapped to in the PSoC memory map.
    Valid when the memory-mapped mode is enabled. */
    .baseAddress = 0x18000000U,
    /* The size allocated in the PSoC memory map, for the memory slave device.
    The size is allocated from the base address. Valid when the memory mapped mode is enabled. */
/*    .memMappedSize = 0x4000000U, */
    .flags = CY_SMIF_FLAG_DETECT_SFDP,
    .slaveSelect = CY_SMIF_SLAVE_SELECT_0,
    .dataSelect = CY_SMIF_DATA_SEL0,
    .deviceCfg = &dev_sfdp_0
};

cy_stc_smif_mem_config_t *mems_sfdp[1] =
{
    &mem_sfdp_0
};

static cy_stc_smif_block_config_t smifBlockConfig_sfdp =
{
    .memCount = 1,
    .memConfig = mems_sfdp,
};

static cy_stc_smif_block_config_t *smif_blk_config;

static cy_stc_smif_context_t QSPI_context;

cy_stc_smif_config_t const QSPI_config =
{
    .mode = CY_SMIF_NORMAL,
    .deselectDelay = 1,
    .rxClockSel = CY_SMIF_SEL_INV_INTERNAL_CLK,
    .blockEvent = CY_SMIF_BUS_ERROR
};

cy_stc_sysint_t smifIntConfig =
{/* ATTENTION: make sure proper Interrupts configured for CM0p or M4 cores */
    .intrSrc = smif_interrupt_IRQn,
#if (CY_CPU_CORTEX_M0P)
    .cm0pSrc = smif_interrupt_IRQn,
#endif /* (CY_CPU_CORTEX_M0P) */
    .intrPriority = 1
};

/* SMIF pinouts configurations */
const cy_stc_gpio_pin_config_t QSPI_SS_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = P11_2_SMIF_SPI_SELECT0,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};
const cy_stc_gpio_pin_config_t QSPI_DATA3_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = P11_3_SMIF_SPI_DATA3,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};
const cy_stc_gpio_pin_config_t QSPI_DATA2_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = P11_4_SMIF_SPI_DATA2,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};
const cy_stc_gpio_pin_config_t QSPI_DATA1_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = P11_5_SMIF_SPI_DATA1,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};
const cy_stc_gpio_pin_config_t QSPI_DATA0_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = P11_6_SMIF_SPI_DATA0,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};
const cy_stc_gpio_pin_config_t QSPI_SCK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = P11_7_SMIF_SPI_CLK,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};

volatile uint32_t Isr_SMIF_cntr;

void Isr_SMIF(void)
{
Isr_SMIF_cntr++;
    Cy_SMIF_Interrupt(QSPIPort, &QSPI_context);
}

cy_en_smif_status_t qspi_init_hardware()
{
    cy_en_smif_status_t st;

    Cy_GPIO_Pin_Init(SS0Port, SS0Pin, &QSPI_SS_config);
    Cy_GPIO_SetHSIOM(SS0Port, SS0Pin, SS0MuxPort);

    Cy_GPIO_Pin_Init(SS1Port, SS1Pin, &QSPI_SS_config);
    Cy_GPIO_SetHSIOM(SS1Port, SS1Pin, SS1MuxPort);

    Cy_GPIO_Pin_Init(D3Port, D3Pin, &QSPI_DATA3_config);
    Cy_GPIO_SetHSIOM(D3Port, D3Pin, D3MuxPort);

    Cy_GPIO_Pin_Init(D2Port, D2Pin, &QSPI_DATA2_config);
    Cy_GPIO_SetHSIOM(D2Port, D2Pin, D2MuxPort);

    Cy_GPIO_Pin_Init(D1Port, D1Pin, &QSPI_DATA1_config);
    Cy_GPIO_SetHSIOM(D1Port, D1Pin, D1MuxPort);

    Cy_GPIO_Pin_Init(D0Port, D0Pin, &QSPI_DATA0_config);
    Cy_GPIO_SetHSIOM(D0Port, D0Pin, D0MuxPort);

    Cy_GPIO_Pin_Init(SCKPort, SCKPin, &QSPI_SCK_config);
    Cy_GPIO_SetHSIOM(SCKPort, SCKPin, SCKMuxPort);

    Cy_SysClk_ClkHfSetSource(CY_SYSCLK_CLKHF_IN_CLKPATH2, CY_SYSCLK_CLKHF_IN_CLKPATH0);
    Cy_SysClk_ClkHfSetDivider(CY_SYSCLK_CLKHF_IN_CLKPATH2, CY_SMIF_SYSCLK_HFCLK_DIVIDER);
    Cy_SysClk_ClkHfEnable(CY_SYSCLK_CLKHF_IN_CLKPATH2);

    /*
     * Setup the interrupt for the SMIF block.  For the CM0 there
     * is a two stage process to setup the interrupts.
     */
    Cy_SysInt_Init(&smifIntConfig, Isr_SMIF);

    st = Cy_SMIF_Init(QSPIPort, &QSPI_config, 1000, &QSPI_context);
    if (st != CY_SMIF_SUCCESS)
    {
        return st;
    }
    NVIC_EnableIRQ(smifIntConfig.intrSrc); /* Finally, Enable the SMIF interrupt */

    Cy_SMIF_Enable(QSPIPort, &QSPI_context);

    return CY_SMIF_SUCCESS;
}

cy_stc_smif_mem_config_t *qspi_get_memory_config(int index)
{
    return smif_blk_config->memConfig[index];
}

SMIF_Type *qspi_get_device()
{
    return QSPIPort;
}

cy_stc_smif_context_t *qspi_get_context()
{
    return &QSPI_context;
}

cy_en_smif_status_t qspi_init(cy_stc_smif_block_config_t *blk_config)
{
    cy_en_smif_status_t st;

    st = qspi_init_hardware();
    if (st == CY_SMIF_SUCCESS)
    {
        smif_blk_config = blk_config;
        st = Cy_SMIF_MemInit(QSPIPort, smif_blk_config, &QSPI_context);
    }
    return st;
}

cy_en_smif_status_t qspi_init_sfdp(uint32_t smif_id)
{
    cy_en_smif_status_t stat = CY_SMIF_SUCCESS;

    cy_stc_smif_mem_config_t **memCfg = smifBlockConfig_sfdp.memConfig;

    switch(smif_id)
    {
    case 1:
        (*memCfg)->slaveSelect = CY_SMIF_SLAVE_SELECT_0;
        break;
    case 2:
        (*memCfg)->slaveSelect = CY_SMIF_SLAVE_SELECT_1;
        break;
    case 3:
        (*memCfg)->slaveSelect = CY_SMIF_SLAVE_SELECT_2;
        break;
    case 4:
        (*memCfg)->slaveSelect = CY_SMIF_SLAVE_SELECT_3;
        break;
    default:
        stat = -1;
        break;
    }

    if (stat == CY_SMIF_SUCCESS)
    {
        stat = qspi_init(&smifBlockConfig_sfdp);
    }
    return stat;
}

uint32_t qspi_get_prog_size(void)
{
    cy_stc_smif_mem_config_t **memCfg = smifBlockConfig_sfdp.memConfig;
    return (*memCfg)->deviceCfg->programSize;
}

uint32_t qspi_get_erase_size(void)
{
    cy_stc_smif_mem_config_t **memCfg = smifBlockConfig_sfdp.memConfig;
    return (*memCfg)->deviceCfg->eraseSize;
}
