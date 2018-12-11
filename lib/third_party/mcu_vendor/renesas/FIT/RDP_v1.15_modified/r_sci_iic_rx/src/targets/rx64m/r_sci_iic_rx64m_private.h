/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
 * No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
 * applicable laws, including copyright laws. 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO 
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the 
 * following link:
 * http://www.renesas.com/disclaimer 
 *
 * Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_sci_iic_rx64m_private.h
 * Description  : Functions for using SCI_IIC on RX devices. 
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 22.09.2014 1.00     First Release
 *         : 15.11.2014 1.01     Add comment
 *         : 27.02.2015 1.60     Replaced the vector number with the macro defined in iodefine.h.
 *         : 01.10.2016 2.00     Fixed a program according to the Renesas coding rules.
 *         : 31.08.2017 2.20     Deleted definitions of PORT_PFS_OFFSET, PORT_ODR0_OFFSET, PORT_ODR1_OFFSET
 *                                and PORT_DSCR_OFFSET.
 *                               Added macro definition SCI_IIC_PRV_DSCR_BASE_REG.
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef SCI_IIC_RX64M_PRIVATE_H
    #define SCI_IIC_RX64M_PRIVATE_H
/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
/* Access to peripherals and board defines. */
    #include "platform.h"
    #ifdef BSP_MCU_RX64M

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/*============================================================================*/
/*  Parameter check of Configuration Options                                  */
/*============================================================================*/
        #if (1 == SCI_IIC_CFG_CH8_INCLUDED)
            #error "ERROR - SCI_IIC_CFG_CH8_INCLUDED - This channel is not supported by the current MCU."
        #endif

        #if (1 == SCI_IIC_CFG_CH9_INCLUDED)
            #error "ERROR - SCI_IIC_CFG_CH9_INCLUDED - This channel is not supported by the current MCU."
        #endif

        #if (1 == SCI_IIC_CFG_CH10_INCLUDED)
            #error "ERROR - SCI_IIC_CFG_CH10_INCLUDED - This channel is not supported by the current MCU."
        #endif

        #if (1 == SCI_IIC_CFG_CH11_INCLUDED)
            #error "ERROR - SCI_IIC_CFG_CH11_INCLUDED - This channel is not supported by the current MCU."
        #endif

/*----------------------------------------------------------------------------*/
/*   Define register setting.                                                 */
/*----------------------------------------------------------------------------*/
/* Sets to use multi-function pin controller. */
        #define SCI_IIC_MPC_SSDA_INIT       ((uint8_t)(0x00))   /* Pin function select to Hi-Z */
        #define SCI_IIC_MPC_SSCL_INIT       ((uint8_t)(0x00))   /* Pin function select to Hi-Z */

        #if SCI_IIC_CFG_CH0_INCLUDED
            #if ((SCI_IIC_CFG_CH0_SSDA_GP == PORT3_GR) && (SCI_IIC_CFG_CH0_SSDA_PIN == 2))
                #define SCI_IIC_MPC_SSDA0_ENABLE ((uint8_t)(0x0B))   /* Pin function select to SCI0 SSDA pin:b01011 */
            #else
                #define SCI_IIC_MPC_SSDA0_ENABLE ((uint8_t)(0x0A))   /* Pin function select to SCI0 SSDA pin:b01010 */
            #endif
            #if ((SCI_IIC_CFG_CH0_SSCL_GP == PORT3_GR) && (SCI_IIC_CFG_CH0_SSCL_PIN == 3))
                #define SCI_IIC_MPC_SSCL0_ENABLE ((uint8_t)(0x0B))   /* Pin function select to SCI0 SSCL pin:b01011 */
            #else
                #define SCI_IIC_MPC_SSCL0_ENABLE ((uint8_t)(0x0A))   /* Pin function select to SCI0 SSCL pin:b01010 */
            #endif
        #endif /* #ifdef SCI_IIC_CFG_CH0_INCLUDED */

        #if SCI_IIC_CFG_CH1_INCLUDED
            #define SCI_IIC_MPC_SSDA1_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI1 SSDA pin:b01010 */
            #define SCI_IIC_MPC_SSCL1_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI1 SSCL pin:b01010 */
        #endif /* #ifdef SCI_IIC_CFG_CH1_INCLUDED */

        #if SCI_IIC_CFG_CH2_INCLUDED
            #define SCI_IIC_MPC_SSDA2_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI2 SSDA pin:b01010 */
            #define SCI_IIC_MPC_SSCL2_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI2 SSCL pin:b01010 */
        #endif /* #ifdef SCI_IIC_CFG_CH2_INCLUDED */

        #if SCI_IIC_CFG_CH3_INCLUDED
            #if ((SCI_IIC_CFG_CH3_SSDA_GP == PORT1_GR) && (SCI_IIC_CFG_CH3_SSDA_PIN == 7))
                #define SCI_IIC_MPC_SSDA3_ENABLE ((uint8_t)(0x0B))   /* Pin function select to SCI3 SSDA pin:b01011 */
            #else
                #define SCI_IIC_MPC_SSDA3_ENABLE ((uint8_t)(0x0A))   /* Pin function select to SCI3 SSDA pin:b01010 */
            #endif
            #if ((SCI_IIC_CFG_CH3_SSCL_GP == PORT1_GR) && (SCI_IIC_CFG_CH3_SSCL_PIN == 6))
                #define SCI_IIC_MPC_SSCL3_ENABLE ((uint8_t)(0x0B))   /* Pin function select to SCI3 SSCL pin:b01011 */
            #else
                #define SCI_IIC_MPC_SSCL3_ENABLE ((uint8_t)(0x0A))   /* Pin function select to SCI3 SSCL pin:b01010 */
            #endif
        #endif /* #ifdef SCI_IIC_CFG_CH3_INCLUDED */

        #if SCI_IIC_CFG_CH4_INCLUDED
            #define SCI_IIC_MPC_SSDA4_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI4 SSDA pin:b01010 */
            #define SCI_IIC_MPC_SSCL4_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI4 SSCL pin:b01010 */
        #endif /* #ifdef SCI_IIC_CFG_CH4_INCLUDED */

        #if SCI_IIC_CFG_CH5_INCLUDED
            #define SCI_IIC_MPC_SSDA5_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI5 SSDA pin:b01010 */
            #define SCI_IIC_MPC_SSCL5_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI5 SSCL pin:b01010 */
        #endif /* #ifdef SCI_IIC_CFG_CH5_INCLUDED */

        #if SCI_IIC_CFG_CH6_INCLUDED
            #if ((SCI_IIC_CFG_CH6_SSDA_GP == PORTB_GR) && (SCI_IIC_CFG_CH6_SSDA_PIN == 1))
                #define SCI_IIC_MPC_SSDA6_ENABLE ((uint8_t)(0x0B))   /* Pin function select to SCI6 SSDA pin:b01011 */
            #else
                #define SCI_IIC_MPC_SSDA6_ENABLE ((uint8_t)(0x0A))   /* Pin function select to SCI6 SSDA pin:b01010 */
            #endif
            #if ((SCI_IIC_CFG_CH6_SSCL_GP == PORTB_GR) && (SCI_IIC_CFG_CH6_SSCL_PIN == 0))
                #define SCI_IIC_MPC_SSCL6_ENABLE ((uint8_t)(0x0B))   /* Pin function select to SCI6 SSCL pin:b01011 */
            #else
                #define SCI_IIC_MPC_SSCL6_ENABLE ((uint8_t)(0x0A))   /* Pin function select to SCI6 SSCL pin:b01010 */
            #endif
        #endif /* #ifdef SCI_IIC_CFG_CH6_INCLUDED */

        #if SCI_IIC_CFG_CH7_INCLUDED
            #define SCI_IIC_MPC_SSDA7_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI7 SSDA pin:b01010 */
            #define SCI_IIC_MPC_SSCL7_ENABLE    ((uint8_t)(0x0A))   /* Pin function select to SCI7 SSCL pin:b01010 */
        #endif /* #ifdef SCI_IIC_CFG_CH7_INCLUDED */

        #if SCI_IIC_CFG_CH12_INCLUDED
            #define SCI_IIC_MPC_SSDA12_ENABLE   ((uint8_t)(0x0C))   /* Pin function select to SCI12 SSDA pin:b01100 */
            #define SCI_IIC_MPC_SSCL12_ENABLE   ((uint8_t)(0x0C))   /* Pin function select to SCI12 SSCL pin:b01100 */
        #endif /* #ifdef SCI_IIC_CFG_CH12_INCLUDED */

/* Interrupt register setting */
        #define SCI_IIC_IR_CLR              ((uint8_t)(0x00))   /* Clears interrupt request register. */
        #define SCI_IIC_IR_SET              ((uint8_t)(0x01))   /* Sets interrupt request register. */

/* Common register setting */
        #define SCI_IIC_SMR_INIT            ((uint8_t)(0x00))   /* Initializes SMR. */

        #define SCI_IIC_SCR_INIT            ((uint8_t)(0x00))   /* Initializes SCR. */
        #define SCI_IIC_SCR_ENABLE          ((uint8_t)(0xB4))   /* Enable bit:TEIE,TIE,TE,RE bit */
        #define SCI_IIC_SCR_TE_RE           ((uint8_t)(0x30))   /* Enable bit:TE,RE bit */

        #define SCI_IIC_SCMR_INIT           ((uint8_t)(0xF2))   /* Initializes SCMR. */

        #define SCI_IIC_BRR_INIT            ((uint8_t)(0xFF))   /* Initializes BRR. */

        #define SCI_IIC_SEMR_INIT           ((uint8_t)(0x00))   /* Initializes SEMR. */

        #define SCI_IIC_SNFR_INIT           ((uint8_t)(0x00))   /* Initializes SNFR. */

        #define SCI_IIC_SIMR1_INIT          ((uint8_t)(0x00))   /* Initializes SIMR1. */

        #define SCI_IIC_SIMR2_INIT          ((uint8_t)(0x00))   /* Initializes SIMR2. */

        #define SCI_IIC_SIMR3_INIT          ((uint8_t)(0x00))   /* Initializes SIMR3. */

        #define SCI_IIC_SISR_INIT           ((uint8_t)(0x00))   /* Initializes SISR. */

        #define SCI_IIC_SPMR_INIT           ((uint8_t)(0x00))   /* Initializes SPMR. */

/* Base register of DSCR used to calculate all DSCR register addresses. This is constant for all supported MCUs. */
    #define SCI_IIC_PRV_DSCR_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-63))

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/* define sci register type */
typedef volatile __evenaccess struct st_sci0 * sci_regs_t;

/*----------------------------------------------------------------------------*/
/*   Define internal sci_iic information structure type.                      */
/*----------------------------------------------------------------------------*/
/* Specifies register addresses and bits associated with the SCI, module-stop, and ICU. */
typedef struct st_sci_iic_ch_rom /* SCI ROM info for channel control block */
{
    mcu_lock_t                       hd_lock; /* Hardware Lock number */
    sci_regs_t                       regs; /* base ptr to ch registers */
    volatile __evenaccess uint32_t * pmstp; /* ptr to mstp register */
    uint32_t                         stop_mask; /* mstp mask to disable ch */
    volatile __evenaccess uint8_t  * pipr; /* ptr to IPR register */
    volatile __evenaccess uint8_t  * pir_txi; /* ptr to TXI IR register */
    volatile __evenaccess uint32_t * pir_tei; /* ptr to TEI IR register */
    uint32_t                         tei_ir_mask; /* bit mask to enable/disable txi INT in ICU */
    volatile __evenaccess uint8_t  * picu_txi;
    uint8_t                          txi_en_mask; /* bit mask to enable/disable txi INT in ICU */
    volatile __evenaccess uint32_t * picu_tei;
    uint32_t                         tei_en_mask; /* bit mask to enable/disable tei INT in ICU */
    bsp_int_src_t                    grp_tei_def;
    void                           (*grp_tei_func) (void);
    uint8_t                          ipr_set_val;
    uint8_t                          sscl_port_gr;
    uint8_t                          sscl_port_pin;
    uint8_t                          sscl_en_val;
    uint8_t                          ssda_port_gr;
    uint8_t                          ssda_port_pin;
    uint8_t                          ssda_en_val;
    uint8_t                          ssda_delay;
    uint32_t                         bitrate;
    uint8_t                          df_sel;
    uint8_t                          df_clk;
} sci_iic_ch_rom_t;

/* CHANNEL CONTROL BLOCK */
typedef struct st_sci_iic_ch_ctrl /* SCI channel control (for handle) */
{
    const sci_iic_ch_rom_t * prom; /* pointer to rom info */
    sci_iic_api_event_t      api_event; /* Event flag pointer */
    sci_iic_api_mode_t       api_mode; /* Internal mode of control protocol */
    sci_iic_api_status_t     api_n_status; /* Internal Status of Now */
    sci_iic_api_status_t     api_b_status; /* Internal Status of Before */
    uint8_t                  mode; /* Check I2C condition */
    sci_iic_info_t         * psci_iic_info_ch; /* Channel 1 SCI driver information */
} sci_iic_ch_ctrl_t;

typedef sci_iic_ch_ctrl_t * sci_iic_hdl_t;

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/
extern const sci_iic_hdl_t g_sci_iic_handles[SCI_IIC_NUM_CH_MAX];

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
extern void r_sci_iic_power_on (uint8_t channel);
extern void r_sci_iic_power_off (uint8_t channel);
extern void r_sci_iic_io_open (uint8_t port_gr, uint8_t pin_num);
extern void r_sci_iic_init_io_register (sci_iic_info_t * p_sci_iic_info);
extern void r_sci_iic_clear_io_register (sci_iic_info_t * p_sci_iic_info);
extern void r_sci_iic_int_disable (sci_iic_info_t * p_sci_iic_info);
extern void r_sci_iic_int_enable (sci_iic_info_t * p_sci_iic_info);
extern void r_sci_iic_mpc_setting (uint8_t port_gr, uint8_t pin_num, uint8_t set_value);

/*----------------------------------------------------------------------------*/
/*   Interrupt Function                                                       */
/*----------------------------------------------------------------------------*/
        #if SCI_IIC_CFG_CH0_INCLUDED
/* ---- SCI IIC ch0 TEI ---- */
void sci_iic_int_sci_iic0_tei_isr(void);
        #endif

        #if SCI_IIC_CFG_CH1_INCLUDED
/* ---- SCI IIC ch1 TEI ---- */
void sci_iic_int_sci_iic1_tei_isr(void);
        #endif

        #if SCI_IIC_CFG_CH2_INCLUDED
/* ---- SCI IIC ch2 TEI ---- */
void sci_iic_int_sci_iic2_tei_isr(void);
        #endif

        #if SCI_IIC_CFG_CH3_INCLUDED
/* ---- SCI IIC ch3 TEI ---- */
void sci_iic_int_sci_iic3_tei_isr(void);
        #endif

        #if SCI_IIC_CFG_CH4_INCLUDED
/* ---- SCI IIC ch4 TEI ---- */
void sci_iic_int_sci_iic4_tei_isr(void);
        #endif

        #if SCI_IIC_CFG_CH5_INCLUDED
/* ---- SCI IIC ch5 TEI ---- */
void sci_iic_int_sci_iic5_tei_isr(void);
        #endif

        #if SCI_IIC_CFG_CH6_INCLUDED
/* ---- SCI IIC ch6 TEI ---- */
void sci_iic_int_sci_iic6_tei_isr(void);
        #endif

        #if SCI_IIC_CFG_CH7_INCLUDED
/* ---- SCI IIC ch7 TEI ---- */
void sci_iic_int_sci_iic7_tei_isr(void);
        #endif

        #if SCI_IIC_CFG_CH12_INCLUDED
/* ---- SCI IIC ch12 TEI ---- */
void sci_iic_int_sci_iic12_tei_isr(void);
        #endif

    #endif /* BSP_MCU_RX64M */
#endif /* SCI_IIC_RX64M_PRIVATE_H */

