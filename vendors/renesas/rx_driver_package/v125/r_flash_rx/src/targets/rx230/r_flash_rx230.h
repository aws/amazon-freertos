/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name    : r_flash_rx230.h
* Description  : This file has specific information about the ROM and DF on 
*                RX230 MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 13.01.2015 1.00    First Release
*         : 22.07.2016 2.00    Modified for BSPless flash. Fixed block range comments.
*                              Modified values for
*                              FLASH_CF_LOWEST_VALID_BLOCK and FLASH_CF_BLOCK_INVALID.
*         : 25.05.2017 2.10    Removed equates for MCU_CFG_PART_MEMORY_SIZE 3.
*         : 07.08.2017 2.20    Added WAIT_MAX_ERASE_DF alias.
******************************************************************************/

#ifndef _FLASH_API_RX230_H
#define _FLASH_API_RX230_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard typedefs used in this file */
#include <stdint.h>
/******************************************************************************
Macro definitions
******************************************************************************/
#define FLASH_NUM_BLOCKS_DF           (8)
#define FLASH_DF_MIN_PGM_SIZE         (1)
#define FLASH_CF_MIN_PGM_SIZE         (8)

#define FLASH_CF_BLOCK_SIZE           (2048)
#define FLASH_DF_BLOCK_SIZE           (1024)
#define FLASH_DF_FULL_SIZE            (FLASH_NUM_BLOCKS_DF*FLASH_DF_BLOCK_SIZE)
#define FLASH_DF_FULL_PGM_SIZE        (FLASH_DF_FULL_SIZE-FLASH_DF_MIN_PGM_SIZE)
#define FLASH_DF_LAST_VALID_ADDR      (FLASH_DF_BLOCK_INVALID-1)
#define FLASH_DF_HIGHEST_VALID_BLOCK  (FLASH_DF_BLOCK_INVALID-FLASH_DF_BLOCK_SIZE)


#define FLASH_NUM_BLOCKS_CF           (MCU_ROM_SIZE_BYTES / FLASH_CF_BLOCK_SIZE)
#define FLASH_CF_FULL_SIZE            (FLASH_NUM_BLOCKS_CF*FLASH_CF_BLOCK_SIZE)
#define FLASH_CF_LOWEST_VALID_BLOCK   (FLASH_CF_BLOCK_INVALID + 1)
#define FLASH_CF_LAST_VALID_ADDR      (FLASH_CF_LOWEST_VALID_BLOCK)

/* Delay function Setting */
#define WAIT_DIV_LOOP_CYCLE           (4000) /* 4 cycle of 1 loop * 1000(us) */
#define WAIT_OVERHEAD_COUNT           (5)    /* Overhead is reduced from a loop count.
                                                overhead loop count = 20 cycle / 4 cycle of 1 loop */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum _flash_block_address
{
    FLASH_CF_BLOCK_END     = 0xFFFFFFFF,    /*   Top  of the CS */
    FLASH_CF_BLOCK_0       = 0xFFFFF800,    /*   2KB: 0xFFFFF800 - 0xFFFFFFFF */
    FLASH_CF_BLOCK_1       = 0xFFFFF000,    /*   2KB: 0xFFFFF000 - 0xFFFFF7FF */
    FLASH_CF_BLOCK_2       = 0xFFFFE800,    /*   2KB: 0xFFFFE800 - 0xFFFFEFFF */
    FLASH_CF_BLOCK_3       = 0xFFFFE000,    /*   2KB: 0xFFFFE000 - 0xFFFFE7FF */
    FLASH_CF_BLOCK_4       = 0xFFFFD800,    /*   2KB: 0xFFFFD800 - 0xFFFFDFFF */
    FLASH_CF_BLOCK_5       = 0xFFFFD000,    /*   2KB: 0xFFFFD000 - 0xFFFFD7FF */
    FLASH_CF_BLOCK_6       = 0xFFFFC800,    /*   2KB: 0xFFFFC800 - 0xFFFFCFFF */
    FLASH_CF_BLOCK_7       = 0xFFFFC000,    /*   2KB: 0xFFFFC000 - 0xFFFFC7FF */
    FLASH_CF_BLOCK_8       = 0xFFFFB800,    /*   2KB: 0xFFFFB800 - 0xFFFFBFFF */
    FLASH_CF_BLOCK_9       = 0xFFFFB000,    /*   2KB: 0xFFFFB000 - 0xFFFFB7FF */
    FLASH_CF_BLOCK_10      = 0xFFFFA800,    /*   2KB: 0xFFFFA800 - 0xFFFFAFFF */
    FLASH_CF_BLOCK_11      = 0xFFFFA000,    /*   2KB: 0xFFFFA000 - 0xFFFFA7FF */
    FLASH_CF_BLOCK_12      = 0xFFFF9800,    /*   2KB: 0xFFFF9800 - 0xFFFF9FFF */
    FLASH_CF_BLOCK_13      = 0xFFFF9000,    /*   2KB: 0xFFFF9000 - 0xFFFF97FF */
    FLASH_CF_BLOCK_14      = 0xFFFF8800,    /*   2KB: 0xFFFF8800 - 0xFFFF8FFF */
    FLASH_CF_BLOCK_15      = 0xFFFF8000,    /*   2KB: 0xFFFF8000 - 0xFFFF87FF */
    FLASH_CF_BLOCK_16      = 0xFFFF7800,    /*   2KB: 0xFFFF7800 - 0xFFFF7FFF */
    FLASH_CF_BLOCK_17      = 0xFFFF7000,    /*   2KB: 0xFFFF7000 - 0xFFFF77FF */
    FLASH_CF_BLOCK_18      = 0xFFFF6800,    /*   2KB: 0xFFFF6800 - 0xFFFF6FFF */
    FLASH_CF_BLOCK_19      = 0xFFFF6000,    /*   2KB: 0xFFFF6000 - 0xFFFF67FF */
    FLASH_CF_BLOCK_20      = 0xFFFF5800,    /*   2KB: 0xFFFF5800 - 0xFFFF5FFF */
    FLASH_CF_BLOCK_21      = 0xFFFF5000,    /*   2KB: 0xFFFF5000 - 0xFFFF57FF */
    FLASH_CF_BLOCK_22      = 0xFFFF4800,    /*   2KB: 0xFFFF4800 - 0xFFFF4FFF */
    FLASH_CF_BLOCK_23      = 0xFFFF4000,    /*   2KB: 0xFFFF4000 - 0xFFFF47FF */
    FLASH_CF_BLOCK_24      = 0xFFFF3800,    /*   2KB: 0xFFFF3800 - 0xFFFF3FFF */
    FLASH_CF_BLOCK_25      = 0xFFFF3000,    /*   2KB: 0xFFFF3000 - 0xFFFF37FF */
    FLASH_CF_BLOCK_26      = 0xFFFF2800,    /*   2KB: 0xFFFF2800 - 0xFFFF2FFF */
    FLASH_CF_BLOCK_27      = 0xFFFF2000,    /*   2KB: 0xFFFF2000 - 0xFFFF27FF */
    FLASH_CF_BLOCK_28      = 0xFFFF1800,    /*   2KB: 0xFFFF1800 - 0xFFFF1FFF */
    FLASH_CF_BLOCK_29      = 0xFFFF1000,    /*   2KB: 0xFFFF1000 - 0xFFFF17FF */
    FLASH_CF_BLOCK_30      = 0xFFFF0800,    /*   2KB: 0xFFFF0800 - 0xFFFF0FFF */
    FLASH_CF_BLOCK_31      = 0xFFFF0000,    /*   2KB: 0xFFFF0000 - 0xFFFF07FF */
    FLASH_CF_BLOCK_32      = 0xFFFEF800,    /*   2KB: 0xFFFEF800 - 0xFFFEFFFF */
    FLASH_CF_BLOCK_33      = 0xFFFEF000,    /*   2KB: 0xFFFEF000 - 0xFFFEF7FF */
    FLASH_CF_BLOCK_34      = 0xFFFEE800,    /*   2KB: 0xFFFEE800 - 0xFFFEEFFF */
    FLASH_CF_BLOCK_35      = 0xFFFEE000,    /*   2KB: 0xFFFEE000 - 0xFFFEE7FF */
    FLASH_CF_BLOCK_36      = 0xFFFED800,    /*   2KB: 0xFFFED800 - 0xFFFEDFFF */
    FLASH_CF_BLOCK_37      = 0xFFFED000,    /*   2KB: 0xFFFED000 - 0xFFFED7FF */
    FLASH_CF_BLOCK_38      = 0xFFFEC800,    /*   2KB: 0xFFFEC800 - 0xFFFECFFF */
    FLASH_CF_BLOCK_39      = 0xFFFEC000,    /*   2KB: 0xFFFEC000 - 0xFFFEC7FF */
    FLASH_CF_BLOCK_40      = 0xFFFEB800,    /*   2KB: 0xFFFEB800 - 0xFFFEBFFF */
    FLASH_CF_BLOCK_41      = 0xFFFEB000,    /*   2KB: 0xFFFEB000 - 0xFFFEB7FF */
    FLASH_CF_BLOCK_42      = 0xFFFEA800,    /*   2KB: 0xFFFEA800 - 0xFFFEAFFF */
    FLASH_CF_BLOCK_43      = 0xFFFEA000,    /*   2KB: 0xFFFEA000 - 0xFFFEA7FF */
    FLASH_CF_BLOCK_44      = 0xFFFE9800,    /*   2KB: 0xFFFE9800 - 0xFFFE9FFF */
    FLASH_CF_BLOCK_45      = 0xFFFE9000,    /*   2KB: 0xFFFE9000 - 0xFFFE97FF */
    FLASH_CF_BLOCK_46      = 0xFFFE8800,    /*   2KB: 0xFFFE8800 - 0xFFFE8FFF */
    FLASH_CF_BLOCK_47      = 0xFFFE8000,    /*   2KB: 0xFFFE8000 - 0xFFFE87FF */
    FLASH_CF_BLOCK_48      = 0xFFFE7800,    /*   2KB: 0xFFFE7800 - 0xFFFE7FFF */
    FLASH_CF_BLOCK_49      = 0xFFFE7000,    /*   2KB: 0xFFFE7000 - 0xFFFE77FF */
    FLASH_CF_BLOCK_50      = 0xFFFE6800,    /*   2KB: 0xFFFE6800 - 0xFFFE6FFF */
    FLASH_CF_BLOCK_51      = 0xFFFE6000,    /*   2KB: 0xFFFE6000 - 0xFFFE67FF */
    FLASH_CF_BLOCK_52      = 0xFFFE5800,    /*   2KB: 0xFFFE5800 - 0xFFFE5FFF */
    FLASH_CF_BLOCK_53      = 0xFFFE5000,    /*   2KB: 0xFFFE5000 - 0xFFFE57FF */
    FLASH_CF_BLOCK_54      = 0xFFFE4800,    /*   2KB: 0xFFFE4800 - 0xFFFE4FFF */
    FLASH_CF_BLOCK_55      = 0xFFFE4000,    /*   2KB: 0xFFFE4000 - 0xFFFE47FF */
    FLASH_CF_BLOCK_56      = 0xFFFE3800,    /*   2KB: 0xFFFE3800 - 0xFFFE3FFF */
    FLASH_CF_BLOCK_57      = 0xFFFE3000,    /*   2KB: 0xFFFE3000 - 0xFFFE37FF */
    FLASH_CF_BLOCK_58      = 0xFFFE2800,    /*   2KB: 0xFFFE2800 - 0xFFFE2FFF */
    FLASH_CF_BLOCK_59      = 0xFFFE2000,    /*   2KB: 0xFFFE2000 - 0xFFFE27FF */
    FLASH_CF_BLOCK_60      = 0xFFFE1800,    /*   2KB: 0xFFFE1800 - 0xFFFE1FFF */
    FLASH_CF_BLOCK_61      = 0xFFFE1000,    /*   2KB: 0xFFFE1000 - 0xFFFE17FF */
    FLASH_CF_BLOCK_62      = 0xFFFE0800,    /*   2KB: 0xFFFE0800 - 0xFFFE0FFF */
    FLASH_CF_BLOCK_63      = 0xFFFE0000,    /*   2KB: 0xFFFE0000 - 0xFFFE07FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x5   /*   '5' parts 128KB ROM */
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_63 - 1),
#else
    FLASH_CF_BLOCK_64      = 0xFFFDF800,    /*   2KB: 0xFFFDF800 - 0xFFFDFFFF */
    FLASH_CF_BLOCK_65      = 0xFFFDF000,    /*   2KB: 0xFFFDF000 - 0xFFFDF7FF */
    FLASH_CF_BLOCK_66      = 0xFFFDE800,    /*   2KB: 0xFFFDE800 - 0xFFFDEFFF */
    FLASH_CF_BLOCK_67      = 0xFFFDE000,    /*   2KB: 0xFFFDE000 - 0xFFFDE7FF */
    FLASH_CF_BLOCK_68      = 0xFFFDD800,    /*   2KB: 0xFFFDD800 - 0xFFFDDFFF */
    FLASH_CF_BLOCK_69      = 0xFFFDD000,    /*   2KB: 0xFFFDD000 - 0xFFFDD7FF */
    FLASH_CF_BLOCK_70      = 0xFFFDC800,    /*   2KB: 0xFFFDC800 - 0xFFFDCFFF */
    FLASH_CF_BLOCK_71      = 0xFFFDC000,    /*   2KB: 0xFFFDC000 - 0xFFFDC7FF */
    FLASH_CF_BLOCK_72      = 0xFFFDB800,    /*   2KB: 0xFFFDB800 - 0xFFFDBFFF */
    FLASH_CF_BLOCK_73      = 0xFFFDB000,    /*   2KB: 0xFFFDB000 - 0xFFFDB7FF */
    FLASH_CF_BLOCK_74      = 0xFFFDA800,    /*   2KB: 0xFFFDA800 - 0xFFFDAFFF */
    FLASH_CF_BLOCK_75      = 0xFFFDA000,    /*   2KB: 0xFFFDA000 - 0xFFFDA7FF */
    FLASH_CF_BLOCK_76      = 0xFFFD9800,    /*   2KB: 0xFFFD9800 - 0xFFFD9FFF */
    FLASH_CF_BLOCK_77      = 0xFFFD9000,    /*   2KB: 0xFFFD9000 - 0xFFFD97FF */
    FLASH_CF_BLOCK_78      = 0xFFFD8800,    /*   2KB: 0xFFFD8800 - 0xFFFD8FFF */
    FLASH_CF_BLOCK_79      = 0xFFFD8000,    /*   2KB: 0xFFFD8000 - 0xFFFD87FF */
    FLASH_CF_BLOCK_80      = 0xFFFD7800,    /*   2KB: 0xFFFD7800 - 0xFFFD7FFF */
    FLASH_CF_BLOCK_81      = 0xFFFD7000,    /*   2KB: 0xFFFD7000 - 0xFFFD77FF */
    FLASH_CF_BLOCK_82      = 0xFFFD6800,    /*   2KB: 0xFFFD6800 - 0xFFFD6FFF */
    FLASH_CF_BLOCK_83      = 0xFFFD6000,    /*   2KB: 0xFFFD6000 - 0xFFFD67FF */
    FLASH_CF_BLOCK_84      = 0xFFFD5800,    /*   2KB: 0xFFFD5800 - 0xFFFD5FFF */
    FLASH_CF_BLOCK_85      = 0xFFFD5000,    /*   2KB: 0xFFFD5000 - 0xFFFD57FF */
    FLASH_CF_BLOCK_86      = 0xFFFD4800,    /*   2KB: 0xFFFD4800 - 0xFFFD4FFF */
    FLASH_CF_BLOCK_87      = 0xFFFD4000,    /*   2KB: 0xFFFD4000 - 0xFFFD47FF */
    FLASH_CF_BLOCK_88      = 0xFFFD3800,    /*   2KB: 0xFFFD3800 - 0xFFFD3FFF */
    FLASH_CF_BLOCK_89      = 0xFFFD3000,    /*   2KB: 0xFFFD3000 - 0xFFFD37FF */
    FLASH_CF_BLOCK_90      = 0xFFFD2800,    /*   2KB: 0xFFFD2800 - 0xFFFD2FFF */
    FLASH_CF_BLOCK_91      = 0xFFFD2000,    /*   2KB: 0xFFFD2000 - 0xFFFD27FF */
    FLASH_CF_BLOCK_92      = 0xFFFD1800,    /*   2KB: 0xFFFD1800 - 0xFFFD1FFF */
    FLASH_CF_BLOCK_93      = 0xFFFD1000,    /*   2KB: 0xFFFD1000 - 0xFFFD17FF */
    FLASH_CF_BLOCK_94      = 0xFFFD0800,    /*   2KB: 0xFFFD0800 - 0xFFFD0FFF */
    FLASH_CF_BLOCK_95      = 0xFFFD0000,    /*   2KB: 0xFFFD0000 - 0xFFFD07FF */
    FLASH_CF_BLOCK_96      = 0xFFFCF800,    /*   2KB: 0xFFFCF800 - 0xFFFCFFFF */
    FLASH_CF_BLOCK_97      = 0xFFFCF000,    /*   2KB: 0xFFFCF000 - 0xFFFCF7FF */
    FLASH_CF_BLOCK_98      = 0xFFFCE800,    /*   2KB: 0xFFFCE800 - 0xFFFCEFFF */
    FLASH_CF_BLOCK_99      = 0xFFFCE000,    /*   2KB: 0xFFFCE000 - 0xFFFCE7FF */
    FLASH_CF_BLOCK_100     = 0xFFFCD800,    /*   2KB: 0xFFFCD800 - 0xFFFCDFFF */
    FLASH_CF_BLOCK_101     = 0xFFFCD000,    /*   2KB: 0xFFFCD000 - 0xFFFCD7FF */
    FLASH_CF_BLOCK_102     = 0xFFFCC800,    /*   2KB: 0xFFFCC800 - 0xFFFCCFFF */
    FLASH_CF_BLOCK_103     = 0xFFFCC000,    /*   2KB: 0xFFFCC000 - 0xFFFCC7FF */
    FLASH_CF_BLOCK_104     = 0xFFFCB800,    /*   2KB: 0xFFFCB800 - 0xFFFCBFFF */
    FLASH_CF_BLOCK_105     = 0xFFFCB000,    /*   2KB: 0xFFFCB000 - 0xFFFCB7FF */
    FLASH_CF_BLOCK_106     = 0xFFFCA800,    /*   2KB: 0xFFFCA800 - 0xFFFCAFFF */
    FLASH_CF_BLOCK_107     = 0xFFFCA000,    /*   2KB: 0xFFFCA000 - 0xFFFCA7FF */
    FLASH_CF_BLOCK_108     = 0xFFFC9800,    /*   2KB: 0xFFFC9800 - 0xFFFC9FFF */
    FLASH_CF_BLOCK_109     = 0xFFFC9000,    /*   2KB: 0xFFFC9000 - 0xFFFC97FF */
    FLASH_CF_BLOCK_110     = 0xFFFC8800,    /*   2KB: 0xFFFC8800 - 0xFFFC8FFF */
    FLASH_CF_BLOCK_111     = 0xFFFC8000,    /*   2KB: 0xFFFC8000 - 0xFFFC87FF */
    FLASH_CF_BLOCK_112     = 0xFFFC7800,    /*   2KB: 0xFFFC7800 - 0xFFFC7FFF */
    FLASH_CF_BLOCK_113     = 0xFFFC7000,    /*   2KB: 0xFFFC7000 - 0xFFFC77FF */
    FLASH_CF_BLOCK_114     = 0xFFFC6800,    /*   2KB: 0xFFFC6800 - 0xFFFC6FFF */
    FLASH_CF_BLOCK_115     = 0xFFFC6000,    /*   2KB: 0xFFFC6000 - 0xFFFC67FF */
    FLASH_CF_BLOCK_116     = 0xFFFC5800,    /*   2KB: 0xFFFC5800 - 0xFFFC5FFF */
    FLASH_CF_BLOCK_117     = 0xFFFC5000,    /*   2KB: 0xFFFC5000 - 0xFFFC57FF */
    FLASH_CF_BLOCK_118     = 0xFFFC4800,    /*   2KB: 0xFFFC4800 - 0xFFFC4FFF */
    FLASH_CF_BLOCK_119     = 0xFFFC4000,    /*   2KB: 0xFFFC4000 - 0xFFFC47FF */
    FLASH_CF_BLOCK_120     = 0xFFFC3800,    /*   2KB: 0xFFFC3800 - 0xFFFC3FFF */
    FLASH_CF_BLOCK_121     = 0xFFFC3000,    /*   2KB: 0xFFFC3000 - 0xFFFC37FF */
    FLASH_CF_BLOCK_122     = 0xFFFC2800,    /*   2KB: 0xFFFC2800 - 0xFFFC2FFF */
    FLASH_CF_BLOCK_123     = 0xFFFC2000,    /*   2KB: 0xFFFC2000 - 0xFFFC27FF */
    FLASH_CF_BLOCK_124     = 0xFFFC1800,    /*   2KB: 0xFFFC1800 - 0xFFFC1FFF */
    FLASH_CF_BLOCK_125     = 0xFFFC1000,    /*   2KB: 0xFFFC1000 - 0xFFFC17FF */
    FLASH_CF_BLOCK_126     = 0xFFFC0800,    /*   2KB: 0xFFFC0800 - 0xFFFC0FFF */
    FLASH_CF_BLOCK_127     = 0xFFFC0000,    /*   2KB: 0xFFFC0000 - 0xFFFC07FF */
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_127 - 1),
#endif


    FLASH_DF_BLOCK_0       = 0x00100000,    /*   1KB: 0x00100000 - 0x001003ff */
    FLASH_DF_BLOCK_1       = 0x00100400,    /*   1KB: 0x00100400 - 0x001007ff */
    FLASH_DF_BLOCK_2       = 0x00100800,    /*   1KB: 0x00100800 - 0x00100bff */
    FLASH_DF_BLOCK_3       = 0x00100C00,    /*   1KB: 0x00100C00 - 0x00100fff */
    FLASH_DF_BLOCK_4       = 0x00101000,    /*   1KB: 0x00101000 - 0x001013ff */
    FLASH_DF_BLOCK_5       = 0x00101400,    /*   1KB: 0x00101400 - 0x001017ff */
    FLASH_DF_BLOCK_6       = 0x00101800,    /*   1KB: 0x00101800 - 0x00101bff */
    FLASH_DF_BLOCK_7       = 0x00101C00,    /*   1KB: 0x00101C00 - 0x00101fff */
    FLASH_DF_BLOCK_INVALID = 0x00102000     /*   1KB: Can't write beyond 0x00101fff */
} flash_block_address_t;


/*  According to HW Manual the Max Programming Time for 8 bytes (ROM)
    is 1367 us.  This is with a FCLK of 1MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 1367 us number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ROM_WRITE \
        ((int32_t)(1367 * (MCU_CFG_ICLK_HZ/1000000)))

/*  According to HW Manual the Max Programming Time for 1 bytes
    (Data Flash) is 1197 us.  This is with a FCLK of 1MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 1197 us number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_DF_WRITE \
        ((int32_t)(1197 * (MCU_CFG_ICLK_HZ/1000000)))

/*  According to HW Manual the Max Blank Check time for 1 bytes
    (Data Flash) is 85 usec.  This is with a FCLK of 1MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 85 usec number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_BLANK_CHECK \
        ((int32_t)(85 * (MCU_CFG_ICLK_HZ/1000000)))


/*  According to HW Manual the Max Erasure Time for a 512KB block is
    around 19221ms.  This is with a FCLK of 1MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 19221ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE_CF_1K \
        ((int32_t)(19221000 *(MCU_CFG_ICLK_HZ/1000000)))

/*  According to HW Manual the Max Erasure Time for a 8KB Data Flash block is
    around 2557 ms.  This is with a FCLK of 1MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 2557 ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE_DF_1K \
        ((int32_t)(2557000 * (MCU_CFG_ICLK_HZ/1000000)))

#define WAIT_MAX_ERASE_CF   WAIT_MAX_ERASE_CF_1K
#define WAIT_MAX_ERASE_DF   WAIT_MAX_ERASE_DF_1K

#endif /* _FLASH_API_RX230_H */
