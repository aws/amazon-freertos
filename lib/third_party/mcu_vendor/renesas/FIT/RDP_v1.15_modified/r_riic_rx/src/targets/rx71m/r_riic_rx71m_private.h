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
 * File Name    : r_riic_rx71m_private.h
 * Description  : Functions for using RIIC on RX devices. 
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 09.10.2014 1.60     First Release
 *         : 04.03.2016 1.90     Changed about the pin definisions.
 *         : 01.10.2016 2.00     Fixed a program according to the Renesas coding rules.
 *                               Corrected macro definitions of "RIIC_IR_RXI2" and "RIIC_IR_TXI2".
 *         : 02.06.2017 2.10     Deleted definition of PORT_PFS_OFFSET.
 *                               Deleted definitions of RIIC3.
 *         : 31.08.2017 2.20     Deleted definition of "RIIC_CFG_CH1_INCLUDED".
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef RIIC_RX71M_PRIVATE_H
    #define RIIC_RX71M_PRIVATE_H
/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/

/*----------------------------------------------------------------------------*/
/*   Define channel No.(max) + 1                                              */
/*----------------------------------------------------------------------------*/
    #define MAX_RIIC_CH_NUM           (3U)                /* Set number of RIIC channel */

/*----------------------------------------------------------------------------*/
/*   Define technical update[No.TN-RX*-A012A] process                         */
/*----------------------------------------------------------------------------*/
/* Unnecessary technical update [No.TN-RX*-A012A] process */

/*============================================================================*/
/*  Parameter check of Configuration Options                                  */
/*============================================================================*/
    #if (1U == RIIC_CFG_CH1_INCLUDED)
        #error "ERROR - RIIC_CFG_CH1_INCLUDED - RIIC1 is not supported in this device. "
    #endif

    #ifdef TN_RXA012A
        #error "ERROR - TN_RXA012A - Parameter error in configures file.  "
    #endif /* TN_RXA012A */

/*  RIIC0 Parameter check of Configuration Options */
    #if ((0U != RIIC_CFG_CH0_INCLUDED) && (1U != RIIC_CFG_CH0_INCLUDED))
        #error "ERROR - RIIC_CFG_CH0_INCLUDED - Parameter error in configures file.  "
    #endif

    #if ((0U >= RIIC_CFG_CH0_kBPS) || (1000U < RIIC_CFG_CH0_kBPS))
        #error "ERROR - RIIC_CFG_CH0_kBPS - Parameter error in configures file.  "
    #endif

    #if ((0U > RIIC_CFG_CH0_DIGITAL_FILTER) || (4U < RIIC_CFG_CH0_DIGITAL_FILTER))
        #error "ERROR - RIIC_CFG_CH0_DIGITAL_FILTER - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH0_MASTER_MODE) && (1U != RIIC_CFG_CH0_MASTER_MODE))
        #error "ERROR - RIIC_CFG_CH0_MASTER_MODE - Parameter error in configures file.  "
    #endif

    #if (0U == RIIC_CFG_CH0_SLV_ADDR0_FORMAT)
/* Do Nothing */
    #elif (1U == RIIC_CFG_CH0_SLV_ADDR0_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH0_SLV_ADDR0) || (0x007F < RIIC_CFG_CH0_SLV_ADDR0))
            #error "ERROR - RIIC_CFG_CH0_SLV_ADDR0 - Parameter error in configures file.  "
        #endif
    #elif (2U == RIIC_CFG_CH0_SLV_ADDR0_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH0_SLV_ADDR0) || (0x03FF < RIIC_CFG_CH0_SLV_ADDR0))
            #error "ERROR - RIIC_CFG_CH0_SLV_ADDR0 - Parameter error in configures file.  "
        #endif
    #else
        #error "ERROR - RIIC_CFG_CH0_SLV_ADDR0_FORMAT - Parameter error in configures file.  "
    #endif

    #if (0U == RIIC_CFG_CH0_SLV_ADDR1_FORMAT)
/* Do Nothing */
    #elif (1U == RIIC_CFG_CH0_SLV_ADDR1_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH0_SLV_ADDR1) || (0x007F < RIIC_CFG_CH0_SLV_ADDR1))
            #error "ERROR - RIIC_CFG_CH0_SLV_ADDR1 - Parameter error in configures file.  "
        #endif
    #elif (2U == RIIC_CFG_CH0_SLV_ADDR1_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH0_SLV_ADDR1) || (0x03FF < RIIC_CFG_CH0_SLV_ADDR1))
            #error "ERROR - RIIC_CFG_CH0_SLV_ADDR1 - Parameter error in configures file.  "
        #endif
    #else
        #error "ERROR - RIIC_CFG_CH0_SLV_ADDR1_FORMAT - Parameter error in configures file.  "
    #endif

    #if (0U == RIIC_CFG_CH0_SLV_ADDR2_FORMAT)
/* Do Nothing */
    #elif (1U == RIIC_CFG_CH0_SLV_ADDR2_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH0_SLV_ADDR2) || (0x007F < RIIC_CFG_CH0_SLV_ADDR2))
            #error "ERROR - RIIC_CFG_CH0_SLV_ADDR2 - Parameter error in configures file.  "
        #endif
    #elif (2U == RIIC_CFG_CH0_SLV_ADDR2_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH0_SLV_ADDR2) || (0x03FF < RIIC_CFG_CH0_SLV_ADDR2))
            #error "ERROR - RIIC_CFG_CH0_SLV_ADDR2 - Parameter error in configures file.  "
        #endif
    #else
        #error "ERROR - RIIC_CFG_CH0_SLV_ADDR2_FORMAT - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH0_SLV_GCA_ENABLE) && (1U != RIIC_CFG_CH0_SLV_GCA_ENABLE))
        #error "ERROR - RIIC_CFG_CH0_SLV_GCA_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((1U > RIIC_CFG_CH0_RXI_INT_PRIORITY) || (15U < RIIC_CFG_CH0_RXI_INT_PRIORITY))
        #error "ERROR - RIIC_CFG_CH0_RXI_INT_PRIORITY - Parameter error in configures file.  "
    #endif

    #if ((1U > RIIC_CFG_CH0_TXI_INT_PRIORITY) || (15U < RIIC_CFG_CH0_TXI_INT_PRIORITY))
        #error "ERROR - RIIC_CFG_CH0_TXI_INT_PRIORITY - Parameter error in configures file.  "
    #endif

    #if ((1U > RIIC_CFG_CH0_TEI_INT_PRIORITY) || (15U < RIIC_CFG_CH0_TEI_INT_PRIORITY) || \
     (RIIC_CFG_CH0_RXI_INT_PRIORITY > RIIC_CFG_CH0_TEI_INT_PRIORITY) || \
     (RIIC_CFG_CH0_TXI_INT_PRIORITY > RIIC_CFG_CH0_TEI_INT_PRIORITY))
        #error "ERROR - RIIC_CFG_CH0_TEI_INT_PRIORITY - Parameter error in configures file.  "
    #endif

    #if ((1U > RIIC_CFG_CH0_EEI_INT_PRIORITY) || (15U < RIIC_CFG_CH0_EEI_INT_PRIORITY) || \
     (RIIC_CFG_CH0_RXI_INT_PRIORITY > RIIC_CFG_CH0_EEI_INT_PRIORITY) || \
     (RIIC_CFG_CH0_TXI_INT_PRIORITY > RIIC_CFG_CH0_EEI_INT_PRIORITY))
        #error "ERROR - RIIC_CFG_CH0_EEI_INT_PRIORITY - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH0_TMO_ENABLE) && (1U != RIIC_CFG_CH0_TMO_ENABLE))
        #error "ERROR - RIIC_CFG_CH0_TMO_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH0_TMO_DET_TIME) && (1U != RIIC_CFG_CH0_TMO_DET_TIME))
        #error "ERROR - RIIC_CFG_CH0_TMO_DET_TIME - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH0_TMO_LCNT) && (1U != RIIC_CFG_CH0_TMO_LCNT))
        #error "ERROR - RIIC_CFG_CH0_TMO_LCNT - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH0_TMO_HCNT) && (1U != RIIC_CFG_CH0_TMO_HCNT))
        #error "ERROR - RIIC_CFG_CH0_TMO_HCNT - Parameter error in configures file.  "
    #endif

/*  RIIC2 Parameter check of Configuration Options */
    #if ((0U != RIIC_CFG_CH2_INCLUDED) && (1U != RIIC_CFG_CH2_INCLUDED))
        #error "ERROR - RIIC_CFG_CH2_INCLUDED - Parameter error in configures file.  "
    #endif

    #if ((0U >= RIIC_CFG_CH2_kBPS) || (400U < RIIC_CFG_CH2_kBPS))
        #error "ERROR - RIIC_CFG_CH2_kBPS - Parameter error in configures file.  "
    #endif

    #if ((0U > RIIC_CFG_CH2_DIGITAL_FILTER) || (4U < RIIC_CFG_CH2_DIGITAL_FILTER))
        #error "ERROR - RIIC_CFG_CH2_DIGITAL_FILTER - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH2_MASTER_MODE) && (1U != RIIC_CFG_CH2_MASTER_MODE))
        #error "ERROR - RIIC_CFG_CH2_MASTER_MODE - Parameter error in configures file.  "
    #endif

    #if (0U == RIIC_CFG_CH2_SLV_ADDR0_FORMAT)
/* Do Nothing */
    #elif (1U == RIIC_CFG_CH2_SLV_ADDR0_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH2_SLV_ADDR0) || (0x007F < RIIC_CFG_CH2_SLV_ADDR0))
            #error "ERROR - RIIC_CFG_CH2_SLV_ADDR0 - Parameter error in configures file.  "
        #endif
    #elif (2U == RIIC_CFG_CH2_SLV_ADDR0_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH2_SLV_ADDR0) || (0x03FF < RIIC_CFG_CH2_SLV_ADDR0))
            #error "ERROR - RIIC_CFG_CH2_SLV_ADDR0 - Parameter error in configures file.  "
        #endif
    #else
        #error "ERROR - RIIC_CFG_CH2_SLV_ADDR0_FORMAT - Parameter error in configures file.  "
    #endif

    #if (0U == RIIC_CFG_CH2_SLV_ADDR1_FORMAT)
/* Do Nothing */
    #elif (1U == RIIC_CFG_CH2_SLV_ADDR1_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH2_SLV_ADDR1) || (0x007F < RIIC_CFG_CH2_SLV_ADDR1))
            #error "ERROR - RIIC_CFG_CH2_SLV_ADDR1 - Parameter error in configures file.  "
        #endif
    #elif (2U == RIIC_CFG_CH2_SLV_ADDR1_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH2_SLV_ADDR1) || (0x03FF < RIIC_CFG_CH2_SLV_ADDR1))
            #error "ERROR - RIIC_CFG_CH2_SLV_ADDR1 - Parameter error in configures file.  "
        #endif
    #else
        #error "ERROR - RIIC_CFG_CH2_SLV_ADDR1_FORMAT - Parameter error in configures file.  "
    #endif

    #if (0U == RIIC_CFG_CH2_SLV_ADDR2_FORMAT)
/* Do Nothing */
    #elif (1U == RIIC_CFG_CH2_SLV_ADDR2_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH2_SLV_ADDR2) || (0x007F < RIIC_CFG_CH2_SLV_ADDR2))
            #error "ERROR - RIIC_CFG_CH2_SLV_ADDR2 - Parameter error in configures file.  "
        #endif
    #elif (2U == RIIC_CFG_CH2_SLV_ADDR2_FORMAT)
        #if ((0x0000 > RIIC_CFG_CH2_SLV_ADDR2) || (0x03FF < RIIC_CFG_CH2_SLV_ADDR2))
            #error "ERROR - RIIC_CFG_CH2_SLV_ADDR2 - Parameter error in configures file.  "
        #endif
    #else
        #error "ERROR - RIIC_CFG_CH2_SLV_ADDR2_FORMAT - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH2_SLV_GCA_ENABLE) && (1U != RIIC_CFG_CH2_SLV_GCA_ENABLE))
        #error "ERROR - RIIC_CFG_CH2_SLV_GCA_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((1U > RIIC_CFG_CH2_RXI_INT_PRIORITY) || (15U < RIIC_CFG_CH2_RXI_INT_PRIORITY))
        #error "ERROR - RIIC_CFG_CH2_RXI_INT_PRIORITY - Parameter error in configures file.  "
    #endif

    #if ((1U > RIIC_CFG_CH2_TXI_INT_PRIORITY) || (15U < RIIC_CFG_CH2_TXI_INT_PRIORITY))
        #error "ERROR - RIIC_CFG_CH2_TXI_INT_PRIORITY - Parameter error in configures file.  "
    #endif

    #if ((1U > RIIC_CFG_CH2_TEI_INT_PRIORITY) || (15U < RIIC_CFG_CH2_TEI_INT_PRIORITY) || \
     (RIIC_CFG_CH2_RXI_INT_PRIORITY > RIIC_CFG_CH2_TEI_INT_PRIORITY) || \
     (RIIC_CFG_CH2_TXI_INT_PRIORITY > RIIC_CFG_CH2_TEI_INT_PRIORITY))
        #error "ERROR - RIIC_CFG_CH2_TEI_INT_PRIORITY - Parameter error in configures file.  "
    #endif

    #if ((1U > RIIC_CFG_CH2_EEI_INT_PRIORITY) || (15U < RIIC_CFG_CH2_EEI_INT_PRIORITY) || \
     (RIIC_CFG_CH2_RXI_INT_PRIORITY > RIIC_CFG_CH2_EEI_INT_PRIORITY) || \
     (RIIC_CFG_CH2_TXI_INT_PRIORITY > RIIC_CFG_CH2_EEI_INT_PRIORITY))
        #error "ERROR - RIIC_CFG_CH2_EEI_INT_PRIORITY - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH2_TMO_ENABLE) && (1U != RIIC_CFG_CH2_TMO_ENABLE))
        #error "ERROR - RIIC_CFG_CH2_TMO_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH2_TMO_DET_TIME) && (1U != RIIC_CFG_CH2_TMO_DET_TIME))
        #error "ERROR - RIIC_CFG_CH2_TMO_DET_TIME - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH2_TMO_LCNT) && (1U != RIIC_CFG_CH2_TMO_LCNT))
        #error "ERROR - RIIC_CFG_CH2_TMO_LCNT - Parameter error in configures file.  "
    #endif

    #if ((0U != RIIC_CFG_CH2_TMO_HCNT) && (1U != RIIC_CFG_CH2_TMO_HCNT))
        #error "ERROR - RIIC_CFG_CH2_TMO_HCNT - Parameter error in configures file.  "
    #endif

/*============================================================================*/
/*   PORT/ICU Channel setting.                                                */
/*============================================================================*/
/*----------------------------------------------------------------------------*/
/*   PORT/ICU register setting for RIIC Channel 0or2.                         */
/*----------------------------------------------------------------------------*/
/* EEI0/TEI0/EEI2/TEI2 Interrupt request register */
    #define RIIC_IR_EEI_TEI        (ICU.IR[IR_ICU_GROUPBL1].BIT.IR)

/*----------------------------------------------------------------------------*/
/*   PORT/ICU register setting for RIIC Channel 0.                            */
/*----------------------------------------------------------------------------*/
/* Sets interrupt source priority initialization. */
    #define RIIC_IPR_CH0_RXI_INIT  ((uint8_t)(0x00)) /* RXI0 interrupt source priority initialization */
    #define RIIC_IPR_CH0_TXI_INIT  ((uint8_t)(0x00)) /* TXI0 interrupt source priority initialization */

/* EEI0 interrupt source priority initialization*/
    #define RIIC_IPR_CH0_EEI_SET   ((uint8_t)(RIIC_CFG_CH0_EEI_INT_PRIORITY))
/* RXI0 interrupt source priority initialization*/
    #define RIIC_IPR_CH0_RXI_SET   ((uint8_t)(RIIC_CFG_CH0_RXI_INT_PRIORITY))
/* TXI0 interrupt source priority initialization*/
    #define RIIC_IPR_CH0_TXI_SET   ((uint8_t)(RIIC_CFG_CH0_TXI_INT_PRIORITY))
/* TEI0 interrupt source priority initialization*/
    #define RIIC_IPR_CH0_TEI_SET   ((uint8_t)(RIIC_CFG_CH0_TEI_INT_PRIORITY))

/* Sets to use multi-function pin controller. */
    #define RIIC_MPC_SDA0_INIT     ((uint8_t)(0x00))                 /* Pin function select to Hi-Z */
    #define RIIC_MPC_SCL0_INIT     ((uint8_t)(0x00))                 /* Pin function select to Hi-Z */
    #define RIIC_MPC_SDA0_ENABLE   ((uint8_t)(0x0F))                 /* Pin function select to RIIC0 SDA pin */
    #define RIIC_MPC_SCL0_ENABLE   ((uint8_t)(0x0F))                 /* Pin function select to RIIC0 SCL pin */

/* Define interrupt request registers */
    #define RIIC_IR_RXI0         (ICU.IR[IR_RIIC0_RXI0].BIT.IR)      /* RXI0 Interrupt request register */
    #define RIIC_IR_TXI0         (ICU.IR[IR_RIIC0_TXI0].BIT.IR)      /* TXI0 Interrupt request register */

/* Define interrupt request enable registers */
    #define RIIC_IER_RXI0        (ICU.IER[IER_RIIC0_RXI0].BIT.IEN4)  /* RXI0 Interrupt request enable register */
    #define RIIC_IER_TXI0        (ICU.IER[IER_RIIC0_TXI0].BIT.IEN5)  /* TXI0 Interrupt request enable register */

/* Define GroupBL1 interrupt request enable registers */
    #define RIIC_IER_EEI0_GPBL1  (ICU.GENBL1.BIT.EN14)               /* EEI0 group Interrupt request enable register */
    #define RIIC_IER_TEI0_GPBL1  (ICU.GENBL1.BIT.EN13)               /* TEI0 group Interrupt request enable register */

/* Define interrupt source priority registers */
    #define RIIC_IPR_RXI0  (ICU.IPR[IPR_RIIC0_RXI0].BYTE)            /* RXI0 Interrupt source priority register */
    #define RIIC_IPR_TXI0  (ICU.IPR[IPR_RIIC0_TXI0].BYTE)            /* TXI0 Interrupt source priority register */

/*----------------------------------------------------------------------------*/
/*   PORT/ICU register setting for RIIC Channel 2.                            */
/*----------------------------------------------------------------------------*/
/* Sets interrupt source priority initialization. */
    #define RIIC_IPR_CH2_RXI_INIT  ((uint8_t)(0x00)) /* RXI2 interrupt source priority initialization */
    #define RIIC_IPR_CH2_TXI_INIT  ((uint8_t)(0x00)) /* TXI2 interrupt source priority initialization */

/* EEI2 interrupt source priority initialization*/
    #define RIIC_IPR_CH2_EEI_SET   ((uint8_t)(RIIC_CFG_CH2_EEI_INT_PRIORITY))
/* RXI2 interrupt source priority initialization*/
    #define RIIC_IPR_CH2_RXI_SET   ((uint8_t)(RIIC_CFG_CH2_RXI_INT_PRIORITY))
/* TXI2 interrupt source priority initialization*/
    #define RIIC_IPR_CH2_TXI_SET   ((uint8_t)(RIIC_CFG_CH2_TXI_INT_PRIORITY))
/* TEI2 interrupt source priority initialization*/
    #define RIIC_IPR_CH2_TEI_SET   ((uint8_t)(RIIC_CFG_CH2_TEI_INT_PRIORITY))

/* Sets to use multi-function pin controller. */
    #define RIIC_MPC_SDA2_INIT     ((uint8_t)(0x00))                 /* Pin function select to Hi-Z */
    #define RIIC_MPC_SCL2_INIT     ((uint8_t)(0x00))                 /* Pin function select to Hi-Z */
    #define RIIC_MPC_SDA2_ENABLE   ((uint8_t)(0x0F))                 /* Pin function select to RIIC2 SDA pin */
    #define RIIC_MPC_SCL2_ENABLE   ((uint8_t)(0x0F))                 /* Pin function select to RIIC2 SCL pin */

/* Define interrupt request registers */
    #define RIIC_IR_RXI2         (ICU.IR[IR_RIIC2_RXI2].BIT.IR)      /* RXI2 Interrupt request register */
    #define RIIC_IR_TXI2         (ICU.IR[IR_RIIC2_TXI2].BIT.IR)      /* TXI2 Interrupt request register */

/* Define interrupt request enable registers */
    #define RIIC_IER_RXI2        (ICU.IER[IER_RIIC2_RXI2].BIT.IEN6)  /* RXI2 Interrupt request enable register */
    #define RIIC_IER_TXI2        (ICU.IER[IER_RIIC2_TXI2].BIT.IEN7)  /* TXI2 Interrupt request enable register */

/* Define GroupBL1 interrupt request enable registers */
    #define RIIC_IER_EEI2_GPBL1  (ICU.GENBL1.BIT.EN16)               /* EEI2 group Interrupt request enable register */
    #define RIIC_IER_TEI2_GPBL1  (ICU.GENBL1.BIT.EN15)               /* TEI2 group Interrupt request enable register */

/* Define interrupt source priority registers */
    #define RIIC_IPR_RXI2  (ICU.IPR[IPR_RIIC2_RXI2].BYTE)            /* RXI2 Interrupt source priority register */
    #define RIIC_IPR_TXI2  (ICU.IPR[IPR_RIIC2_TXI2].BYTE)            /* TXI2 Interrupt source priority register */

/*============================================================================*/
/*   RIIC register setting.                                                   */
/*============================================================================*/
/*----------------------------------------------------------------------------*/
/*   RIIC register setting for RIIC Channel 0 .                               */
/*----------------------------------------------------------------------------*/
    #if (0U == RIIC_CFG_CH0_TMO_ENABLE)
        #define RIIC_CH0_TMOE           ((uint8_t)(0x00))       /* Mask for ICFER.TMOE bit. */
    #elif (1U == RIIC_CFG_CH0_TMO_ENABLE)
        #define RIIC_CH0_TMOE           ((uint8_t)(0x01))       /* Mask for ICFER.TMOE bit. */
    #endif

    #if (0U == RIIC_CFG_CH0_MASTER_MODE)
        #define RIIC_CH0_MALE           ((uint8_t)(0x00))       /* Mask for RIIC0.ICFER.MALE bit. */
    #elif (1U == RIIC_CFG_CH0_MASTER_MODE)
        #define RIIC_CH0_MALE           ((uint8_t)(0x02))       /* Mask for RIIC0.ICFER.MALE bit. */
    #endif

    #if (0U == RIIC_CFG_CH0_DIGITAL_FILTER)
        #define RIIC_CH0_NFE            ((uint8_t)(0x00))       /* Mask for RIIC0.ICFER.NFE bit. */
    #elif (1U <= RIIC_CFG_CH0_DIGITAL_FILTER) && (4U >= RIIC_CFG_CH0_DIGITAL_FILTER)
        #define RIIC_CH0_NFE            ((uint8_t)(0x20))       /* Mask for RIIC0.ICFER.NFE bit. */
    #endif

    #if (400U < RIIC_CFG_CH0_kBPS)
        #define RIIC_CH0_FMPE           ((uint8_t)(0x80))       /* Mask for RIIC0.ICFER.FMPE bit. */
    #else
        #define RIIC_CH0_FMPE           ((uint8_t)(0x00))       /* Mask for RIIC0.ICFER.FMPE bit. */
    #endif

/* Sets ICFER */
    #define RIIC_CH0_ICFER_INIT ((uint8_t)(RIIC_CH0_TMOE | RIIC_CH0_MALE | RIIC_NALE | RIIC_SALE | \
                                       RIIC_NACKE | RIIC_CH0_NFE | RIIC_SCLE | RIIC_CH0_FMPE))

    #if (0U == RIIC_CFG_CH0_TMO_DET_TIME)
        #define RIIC_CH0_TMOS           ((uint8_t)(0x00))       /* long mode for the timeout detection time */
    #elif (1U == RIIC_CFG_CH0_TMO_DET_TIME)
        #define RIIC_CH0_TMOS           ((uint8_t)(0x01))       /* short mode for the timeout detection time */
    #endif

    #if (0U == RIIC_CFG_CH0_TMO_LCNT)
        #define RIIC_CH0_TMOL           ((uint8_t)(0x00))       /* Count is disable while the SCL is held LOW */
    #elif (1U == RIIC_CFG_CH0_TMO_LCNT)
        #define RIIC_CH0_TMOL           ((uint8_t)(0x02))       /* Count is enable while the SCL is held LOW */
    #endif

    #if (0U == RIIC_CFG_CH0_TMO_HCNT)
        #define RIIC_CH0_TMOH           ((uint8_t)(0x00))       /* Count is disable while the SCL is held HIGH */
    #elif (1U == RIIC_CFG_CH0_TMO_HCNT)
        #define RIIC_CH0_TMOH           ((uint8_t)(0x04))       /* Count is enable while the SCL is held HIGH */
    #endif

/* Sets ICMR2 */
    #define RIIC_CH0_ICMR2_INIT     ((uint8_t)(RIIC_CH0_TMOS | RIIC_CH0_TMOL | RIIC_CH0_TMOH))

/* Sets ICMR3 */
    #if (0U == RIIC_CFG_CH0_DIGITAL_FILTER) || (1U == RIIC_CFG_CH0_DIGITAL_FILTER)
        #define RIIC_CH0_ICMR3_INIT ((uint8_t)(0x00)) /* Sets RIIC0.ICMR3(Noise Filter Stage: single-stage filter.) */
    #elif (2U == RIIC_CFG_CH0_DIGITAL_FILTER)
        #define RIIC_CH0_ICMR3_INIT ((uint8_t)(0x01)) /* Sets RIIC0.ICMR3(Noise Filter Stage: 2-stage filter.) */
    #elif (3U == RIIC_CFG_CH0_DIGITAL_FILTER)
        #define RIIC_CH0_ICMR3_INIT ((uint8_t)(0x02)) /* Sets RIIC0.ICMR3(Noise Filter Stage: 3-stage filter.) */
    #elif (4U == RIIC_CFG_CH0_DIGITAL_FILTER)
        #define RIIC_CH0_ICMR3_INIT ((uint8_t)(0x03)) /* Sets RIIC0.ICMR3(Noise Filter Stage: 4-stage filter.) */
    #endif

/*----------------------------------------------------------------------------*/
/*   RIIC register setting for RIIC Channel 2 .                               */
/*----------------------------------------------------------------------------*/
    #if (0U == RIIC_CFG_CH2_TMO_ENABLE)
        #define RIIC_CH2_TMOE           ((uint8_t)(0x00))       /* Mask for ICFER.TMOE bit. */
    #elif (1U == RIIC_CFG_CH2_TMO_ENABLE)
        #define RIIC_CH2_TMOE           ((uint8_t)(0x01))       /* Mask for ICFER.TMOE bit. */
    #endif

    #if (0U == RIIC_CFG_CH2_MASTER_MODE)
        #define RIIC_CH2_MALE           ((uint8_t)(0x00))       /* Mask for RIIC2.ICFER.MALE bit. */
    #elif (1U == RIIC_CFG_CH2_MASTER_MODE)
        #define RIIC_CH2_MALE           ((uint8_t)(0x02))       /* Mask for RIIC2.ICFER.MALE bit. */
    #endif

    #if (0U == RIIC_CFG_CH2_DIGITAL_FILTER)
        #define RIIC_CH2_NFE            ((uint8_t)(0x00))       /* Mask for RIIC2.ICFER.NFE bit. */
    #elif (1U <= RIIC_CFG_CH2_DIGITAL_FILTER) && (4U >= RIIC_CFG_CH2_DIGITAL_FILTER)
        #define RIIC_CH2_NFE            ((uint8_t)(0x20))       /* Mask for RIIC2.ICFER.NFE bit. */
    #endif

    #define RIIC_CH2_FMPE           ((uint8_t)(0x00))       /* Mask for RIIC2.ICFER.FMPE bit. */

/* Sets ICFER */
    #define RIIC_CH2_ICFER_INIT ((uint8_t)(RIIC_CH2_TMOE | RIIC_CH2_MALE | RIIC_NALE | RIIC_SALE | \
                                       RIIC_NACKE | RIIC_CH2_NFE | RIIC_SCLE | RIIC_CH2_FMPE))

    #if (0U == RIIC_CFG_CH2_TMO_DET_TIME)
        #define RIIC_CH2_TMOS           ((uint8_t)(0x00))       /* long mode for the timeout detection time */
    #elif (1U == RIIC_CFG_CH2_TMO_DET_TIME)
        #define RIIC_CH2_TMOS           ((uint8_t)(0x01))       /* short mode for the timeout detection time */
    #endif

    #if (0U == RIIC_CFG_CH2_TMO_LCNT)
        #define RIIC_CH2_TMOL           ((uint8_t)(0x00))       /* Count is disable while the SCL is held LOW */
    #elif (1U == RIIC_CFG_CH2_TMO_LCNT)
        #define RIIC_CH2_TMOL           ((uint8_t)(0x02))       /* Count is enable while the SCL is held LOW */
    #endif

    #if (0U == RIIC_CFG_CH2_TMO_HCNT)
        #define RIIC_CH2_TMOH           ((uint8_t)(0x00))       /* Count is disable while the SCL is held HIGH */
    #elif (1U == RIIC_CFG_CH2_TMO_HCNT)
        #define RIIC_CH2_TMOH           ((uint8_t)(0x04))       /* Count is enable while the SCL is held HIGH */
    #endif

/* Sets ICMR2 */
    #define RIIC_CH2_ICMR2_INIT     ((uint8_t)(RIIC_CH2_TMOS | RIIC_CH2_TMOL | RIIC_CH2_TMOH))

/* Sets ICMR3 */
    #if (0U == RIIC_CFG_CH2_DIGITAL_FILTER) || (1U == RIIC_CFG_CH2_DIGITAL_FILTER)
        #define RIIC_CH2_ICMR3_INIT ((uint8_t)(0x00)) /* Sets RIIC2.ICMR3(Noise Filter Stage: single-stage filter.) */
    #elif (2U == RIIC_CFG_CH2_DIGITAL_FILTER)
        #define RIIC_CH2_ICMR3_INIT ((uint8_t)(0x01)) /* Sets RIIC2.ICMR3(Noise Filter Stage: 2-stage filter.) */
    #elif (3U == RIIC_CFG_CH2_DIGITAL_FILTER)
        #define RIIC_CH2_ICMR3_INIT ((uint8_t)(0x02)) /* Sets RIIC2.ICMR3(Noise Filter Stage: 3-stage filter.) */
    #elif (4U == RIIC_CFG_CH2_DIGITAL_FILTER)
        #define RIIC_CH2_ICMR3_INIT ((uint8_t)(0x03)) /* Sets RIIC2.ICMR3(Noise Filter Stage: 4-stage filter.) */
    #endif

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/
extern const uint16_t g_riic_bps[MAX_RIIC_CH_NUM];
extern const uint16_t g_riic_slv_ad0_val[MAX_RIIC_CH_NUM];
extern const uint16_t g_riic_slv_ad1_val[MAX_RIIC_CH_NUM];
extern const uint16_t g_riic_slv_ad2_val[MAX_RIIC_CH_NUM];

extern const uint8_t g_riic_slv_ad0_format[MAX_RIIC_CH_NUM];
extern const uint8_t g_riic_slv_ad1_format[MAX_RIIC_CH_NUM];
extern const uint8_t g_riic_slv_ad2_format[MAX_RIIC_CH_NUM];
extern const uint8_t g_riic_gca_enable[MAX_RIIC_CH_NUM];

extern const uint8_t g_riic_icmr2_init[MAX_RIIC_CH_NUM];
extern const uint8_t g_riic_icmr3_init[MAX_RIIC_CH_NUM];
extern const uint8_t g_riic_icfer_init[MAX_RIIC_CH_NUM];

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*   interrupt function                                                       */
/*----------------------------------------------------------------------------*/
    #if (1U == RIIC_CFG_CH0_INCLUDED)
/* ---- RIIC0 EEI ---- */
void riic0_eei_isr(void*);

/* ---- RIIC0 TEI ---- */
void riic0_tei_isr(void*);
    #endif

    #if (1U == RIIC_CFG_CH2_INCLUDED)
/* ---- RIIC2 EEI ---- */
void riic2_eei_isr(void*);

/* ---- RIIC2 TEI ---- */
void riic2_tei_isr(void*);
    #endif

#endif /* RIIC_RX71M_PRIVATE_H */

