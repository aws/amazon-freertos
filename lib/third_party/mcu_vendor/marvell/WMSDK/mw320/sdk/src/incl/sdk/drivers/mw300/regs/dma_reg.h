/** @file dma_reg.h
*
*  @brief This file contains automatically generated register structure
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


#ifndef _DMA_REG_H
#define _DMA_REG_H

struct dma_reg {
    /* 0x000: DMA channel BLOCK TRANSFER INTERRUPT MASK registers */
    union {
        struct {
            uint32_t MASK_BLOCKINT0               :  1;  /* [0]     r/w */
            uint32_t MASK_BLOCKINT1               :  1;  /* [1]     r/w */
            uint32_t MASK_BLOCKINT2               :  1;  /* [2]     r/w */
            uint32_t MASK_BLOCKINT3               :  1;  /* [3]     r/w */
            uint32_t MASK_BLOCKINT4               :  1;  /* [4]     r/w */
            uint32_t MASK_BLOCKINT5               :  1;  /* [5]     r/w */
            uint32_t MASK_BLOCKINT6               :  1;  /* [6]     r/w */
            uint32_t MASK_BLOCKINT7               :  1;  /* [7]     r/w */
            uint32_t MASK_BLOCKINT8               :  1;  /* [8]     r/w */
            uint32_t MASK_BLOCKINT9               :  1;  /* [9]     r/w */
            uint32_t MASK_BLOCKINT10              :  1;  /* [10]    r/w */
            uint32_t MASK_BLOCKINT11              :  1;  /* [11]    r/w */
            uint32_t MASK_BLOCKINT12              :  1;  /* [12]    r/w */
            uint32_t MASK_BLOCKINT13              :  1;  /* [13]    r/w */
            uint32_t MASK_BLOCKINT14              :  1;  /* [14]    r/w */
            uint32_t MASK_BLOCKINT15              :  1;  /* [15]    r/w */
            uint32_t MASK_BLOCKINT16              :  1;  /* [16]    r/w */
            uint32_t MASK_BLOCKINT17              :  1;  /* [17]    r/w */
            uint32_t MASK_BLOCKINT18              :  1;  /* [18]    r/w */
            uint32_t MASK_BLOCKINT19              :  1;  /* [19]    r/w */
            uint32_t MASK_BLOCKINT20              :  1;  /* [20]    r/w */
            uint32_t MASK_BLOCKINT21              :  1;  /* [21]    r/w */
            uint32_t MASK_BLOCKINT22              :  1;  /* [22]    r/w */
            uint32_t MASK_BLOCKINT23              :  1;  /* [23]    r/w */
            uint32_t MASK_BLOCKINT24              :  1;  /* [24]    r/w */
            uint32_t MASK_BLOCKINT25              :  1;  /* [25]    r/w */
            uint32_t MASK_BLOCKINT26              :  1;  /* [26]    r/w */
            uint32_t MASK_BLOCKINT27              :  1;  /* [27]    r/w */
            uint32_t MASK_BLOCKINT28              :  1;  /* [28]    r/w */
            uint32_t MASK_BLOCKINT29              :  1;  /* [29]    r/w */
            uint32_t MASK_BLOCKINT30              :  1;  /* [30]    r/w */
            uint32_t MASK_BLOCKINT31              :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } MASK_BLOCKINT;

    /* 0x004: DMA channel BLOCK TRANSFER INTERRUPT Registers */
    union {
        struct {
            uint32_t STATUS_BLOCKINT0             :  1;  /* [0]     r/w1clr */
            uint32_t STATUS_BLOCKINT1             :  1;  /* [1]     r/w1clr */
            uint32_t STATUS_BLOCKINT2             :  1;  /* [2]     r/w1clr */
            uint32_t STATUS_BLOCKINT3             :  1;  /* [3]     r/w1clr */
            uint32_t STATUS_BLOCKINT4             :  1;  /* [4]     r/w1clr */
            uint32_t STATUS_BLOCKINT5             :  1;  /* [5]     r/w1clr */
            uint32_t STATUS_BLOCKINT6             :  1;  /* [6]     r/w1clr */
            uint32_t STATUS_BLOCKINT7             :  1;  /* [7]     r/w1clr */
            uint32_t STATUS_BLOCKINT8             :  1;  /* [8]     r/w1clr */
            uint32_t STATUS_BLOCKINT9             :  1;  /* [9]     r/w1clr */
            uint32_t STATUS_BLOCKINT10            :  1;  /* [10]    r/w1clr */
            uint32_t STATUS_BLOCKINT11            :  1;  /* [11]    r/w1clr */
            uint32_t STATUS_BLOCKINT12            :  1;  /* [12]    r/w1clr */
            uint32_t STATUS_BLOCKINT13            :  1;  /* [13]    r/w1clr */
            uint32_t STATUS_BLOCKINT14            :  1;  /* [14]    r/w1clr */
            uint32_t STATUS_BLOCKINT15            :  1;  /* [15]    r/w1clr */
            uint32_t STATUS_BLOCKINT16            :  1;  /* [16]    r/w1clr */
            uint32_t STATUS_BLOCKINT17            :  1;  /* [17]    r/w1clr */
            uint32_t STATUS_BLOCKINT18            :  1;  /* [18]    r/w1clr */
            uint32_t STATUS_BLOCKINT19            :  1;  /* [19]    r/w1clr */
            uint32_t STATUS_BLOCKINT20            :  1;  /* [20]    r/w1clr */
            uint32_t STATUS_BLOCKINT21            :  1;  /* [21]    r/w1clr */
            uint32_t STATUS_BLOCKINT22            :  1;  /* [22]    r/w1clr */
            uint32_t STATUS_BLOCKINT23            :  1;  /* [23]    r/w1clr */
            uint32_t STATUS_BLOCKINT24            :  1;  /* [24]    r/w1clr */
            uint32_t STATUS_BLOCKINT25            :  1;  /* [25]    r/w1clr */
            uint32_t STATUS_BLOCKINT26            :  1;  /* [26]    r/w1clr */
            uint32_t STATUS_BLOCKINT27            :  1;  /* [27]    r/w1clr */
            uint32_t STATUS_BLOCKINT28            :  1;  /* [28]    r/w1clr */
            uint32_t STATUS_BLOCKINT29            :  1;  /* [29]    r/w1clr */
            uint32_t STATUS_BLOCKINT30            :  1;  /* [30]    r/w1clr */
            uint32_t STATUS_BLOCKINT31            :  1;  /* [31]    r/w1clr */
        } BF;
        uint32_t WORDVAL;
    } STATUS_BLOCKINT;

    /* 0x008: DMA Channel transfer completion interrupt mask Registers */
    union {
        struct {
            uint32_t MASK_TFRINT0                 :  1;  /* [0]     r/w */
            uint32_t MASK_TFRINT1                 :  1;  /* [1]     r/w */
            uint32_t MASK_TFRINT2                 :  1;  /* [2]     r/w */
            uint32_t MASK_TFRINT3                 :  1;  /* [3]     r/w */
            uint32_t MASK_TFRINT4                 :  1;  /* [4]     r/w */
            uint32_t MASK_TFRINT5                 :  1;  /* [5]     r/w */
            uint32_t MASK_TFRINT6                 :  1;  /* [6]     r/w */
            uint32_t MASK_TFRINT7                 :  1;  /* [7]     r/w */
            uint32_t MASK_TFRINT8                 :  1;  /* [8]     r/w */
            uint32_t MASK_TFRINT9                 :  1;  /* [9]     r/w */
            uint32_t MASK_TFRINT10                :  1;  /* [10]    r/w */
            uint32_t MASK_TFRINT11                :  1;  /* [11]    r/w */
            uint32_t MASK_TFRINT12                :  1;  /* [12]    r/w */
            uint32_t MASK_TFRINT13                :  1;  /* [13]    r/w */
            uint32_t MASK_TFRINT14                :  1;  /* [14]    r/w */
            uint32_t MASK_TFRINT15                :  1;  /* [15]    r/w */
            uint32_t MASK_TFRINT16                :  1;  /* [16]    r/w */
            uint32_t MASK_TFRINT17                :  1;  /* [17]    r/w */
            uint32_t MASK_TFRINT18                :  1;  /* [18]    r/w */
            uint32_t MASK_TFRINT19                :  1;  /* [19]    r/w */
            uint32_t MASK_TFRINT20                :  1;  /* [20]    r/w */
            uint32_t MASK_TFRINT21                :  1;  /* [21]    r/w */
            uint32_t MASK_TFRINT22                :  1;  /* [22]    r/w */
            uint32_t MASK_TFRINT23                :  1;  /* [23]    r/w */
            uint32_t MASK_TFRINT24                :  1;  /* [24]    r/w */
            uint32_t MASK_TFRINT25                :  1;  /* [25]    r/w */
            uint32_t MASK_TFRINT26                :  1;  /* [26]    r/w */
            uint32_t MASK_TFRINT27                :  1;  /* [27]    r/w */
            uint32_t MASK_TFRINT28                :  1;  /* [28]    r/w */
            uint32_t MASK_TFRINT29                :  1;  /* [29]    r/w */
            uint32_t MASK_TFRINT30                :  1;  /* [30]    r/w */
            uint32_t MASK_TFRINT31                :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } MASK_TFRINT;

    /* 0x00c: DMA Channel transfer completion interrupt Registers */
    union {
        struct {
            uint32_t STATUS_TFRINT0               :  1;  /* [0]     r/w1clr */
            uint32_t STATUS_TFRINT1               :  1;  /* [1]     r/w1clr */
            uint32_t STATUS_TFRINT2               :  1;  /* [2]     r/w1clr */
            uint32_t STATUS_TFRINT3               :  1;  /* [3]     r/w1clr */
            uint32_t STATUS_TFRINT4               :  1;  /* [4]     r/w1clr */
            uint32_t STATUS_TFRINT5               :  1;  /* [5]     r/w1clr */
            uint32_t STATUS_TFRINT6               :  1;  /* [6]     r/w1clr */
            uint32_t STATUS_TFRINT7               :  1;  /* [7]     r/w1clr */
            uint32_t STATUS_TFRINT8               :  1;  /* [8]     r/w1clr */
            uint32_t STATUS_TFRINT9               :  1;  /* [9]     r/w1clr */
            uint32_t STATUS_TFRINT10              :  1;  /* [10]    r/w1clr */
            uint32_t STATUS_TFRINT11              :  1;  /* [11]    r/w1clr */
            uint32_t STATUS_TFRINT12              :  1;  /* [12]    r/w1clr */
            uint32_t STATUS_TFRINT13              :  1;  /* [13]    r/w1clr */
            uint32_t STATUS_TFRINT14              :  1;  /* [14]    r/w1clr */
            uint32_t STATUS_TFRINT15              :  1;  /* [15]    r/w1clr */
            uint32_t STATUS_TFRINT16              :  1;  /* [16]    r/w1clr */
            uint32_t STATUS_TFRINT17              :  1;  /* [17]    r/w1clr */
            uint32_t STATUS_TFRINT18              :  1;  /* [18]    r/w1clr */
            uint32_t STATUS_TFRINT19              :  1;  /* [19]    r/w1clr */
            uint32_t STATUS_TFRINT20              :  1;  /* [20]    r/w1clr */
            uint32_t STATUS_TFRINT21              :  1;  /* [21]    r/w1clr */
            uint32_t STATUS_TFRINT22              :  1;  /* [22]    r/w1clr */
            uint32_t STATUS_TFRINT23              :  1;  /* [23]    r/w1clr */
            uint32_t STATUS_TFRINT24              :  1;  /* [24]    r/w1clr */
            uint32_t STATUS_TFRINT25              :  1;  /* [25]    r/w1clr */
            uint32_t STATUS_TFRINT26              :  1;  /* [26]    r/w1clr */
            uint32_t STATUS_TFRINT27              :  1;  /* [27]    r/w1clr */
            uint32_t STATUS_TFRINT28              :  1;  /* [28]    r/w1clr */
            uint32_t STATUS_TFRINT29              :  1;  /* [29]    r/w1clr */
            uint32_t STATUS_TFRINT30              :  1;  /* [30]    r/w1clr */
            uint32_t STATUS_TFRINT31              :  1;  /* [31]    r/w1clr */
        } BF;
        uint32_t WORDVAL;
    } STATUS_TFRINT;

    /* 0x010: DMA Channel bus error interrupt mask Registers */
    union {
        struct {
            uint32_t MASK_BUSERRINT0              :  1;  /* [0]     r/w */
            uint32_t MASK_BUSERRINT1              :  1;  /* [1]     r/w */
            uint32_t MASK_BUSERRINT2              :  1;  /* [2]     r/w */
            uint32_t MASK_BUSERRINT3              :  1;  /* [3]     r/w */
            uint32_t MASK_BUSERRINT4              :  1;  /* [4]     r/w */
            uint32_t MASK_BUSERRINT5              :  1;  /* [5]     r/w */
            uint32_t MASK_BUSERRINT6              :  1;  /* [6]     r/w */
            uint32_t MASK_BUSERRINT7              :  1;  /* [7]     r/w */
            uint32_t MASK_BUSERRINT8              :  1;  /* [8]     r/w */
            uint32_t MASK_BUSERRINT9              :  1;  /* [9]     r/w */
            uint32_t MASK_BUSERRINT10             :  1;  /* [10]    r/w */
            uint32_t MASK_BUSERRINT11             :  1;  /* [11]    r/w */
            uint32_t MASK_BUSERRINT12             :  1;  /* [12]    r/w */
            uint32_t MASK_BUSERRINT13             :  1;  /* [13]    r/w */
            uint32_t MASK_BUSERRINT14             :  1;  /* [14]    r/w */
            uint32_t MASK_BUSERRINT15             :  1;  /* [15]    r/w */
            uint32_t MASK_BUSERRINT16             :  1;  /* [16]    r/w */
            uint32_t MASK_BUSERRINT17             :  1;  /* [17]    r/w */
            uint32_t MASK_BUSERRINT18             :  1;  /* [18]    r/w */
            uint32_t MASK_BUSERRINT19             :  1;  /* [19]    r/w */
            uint32_t MASK_BUSERRINT20             :  1;  /* [20]    r/w */
            uint32_t MASK_BUSERRINT21             :  1;  /* [21]    r/w */
            uint32_t MASK_BUSERRINT22             :  1;  /* [22]    r/w */
            uint32_t MASK_BUSERRINT23             :  1;  /* [23]    r/w */
            uint32_t MASK_BUSERRINT24             :  1;  /* [24]    r/w */
            uint32_t MASK_BUSERRINT25             :  1;  /* [25]    r/w */
            uint32_t MASK_BUSERRINT26             :  1;  /* [26]    r/w */
            uint32_t MASK_BUSERRINT27             :  1;  /* [27]    r/w */
            uint32_t MASK_BUSERRINT28             :  1;  /* [28]    r/w */
            uint32_t MASK_BUSERRINT29             :  1;  /* [29]    r/w */
            uint32_t MASK_BUSERRINT30             :  1;  /* [30]    r/w */
            uint32_t MASK_BUSERRINT31             :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } MASK_BUSERRINT;

    /* 0x014: DMA Channel bus error interrupt mask Registers */
    union {
        struct {
            uint32_t STATUS_BUSERRINT0            :  1;  /* [0]     r/w1clr */
            uint32_t STATUS_BUSERRINT1            :  1;  /* [1]     r/w1clr */
            uint32_t STATUS_BUSERRINT2            :  1;  /* [2]     r/w1clr */
            uint32_t STATUS_BUSERRINT3            :  1;  /* [3]     r/w1clr */
            uint32_t STATUS_BUSERRINT4            :  1;  /* [4]     r/w1clr */
            uint32_t STATUS_BUSERRINT5            :  1;  /* [5]     r/w1clr */
            uint32_t STATUS_BUSERRINT6            :  1;  /* [6]     r/w1clr */
            uint32_t STATUS_BUSERRINT7            :  1;  /* [7]     r/w1clr */
            uint32_t STATUS_BUSERRINT8            :  1;  /* [8]     r/w1clr */
            uint32_t STATUS_BUSERRINT9            :  1;  /* [9]     r/w1clr */
            uint32_t STATUS_BUSERRINT10           :  1;  /* [10]    r/w1clr */
            uint32_t STATUS_BUSERRINT11           :  1;  /* [11]    r/w1clr */
            uint32_t STATUS_BUSERRINT12           :  1;  /* [12]    r/w1clr */
            uint32_t STATUS_BUSERRINT13           :  1;  /* [13]    r/w1clr */
            uint32_t STATUS_BUSERRINT14           :  1;  /* [14]    r/w1clr */
            uint32_t STATUS_BUSERRINT15           :  1;  /* [15]    r/w1clr */
            uint32_t STATUS_BUSERRINT16           :  1;  /* [16]    r/w1clr */
            uint32_t STATUS_BUSERRINT17           :  1;  /* [17]    r/w1clr */
            uint32_t STATUS_BUSERRINT18           :  1;  /* [18]    r/w1clr */
            uint32_t STATUS_BUSERRINT19           :  1;  /* [19]    r/w1clr */
            uint32_t STATUS_BUSERRINT20           :  1;  /* [20]    r/w1clr */
            uint32_t STATUS_BUSERRINT21           :  1;  /* [21]    r/w1clr */
            uint32_t STATUS_BUSERRINT22           :  1;  /* [22]    r/w1clr */
            uint32_t STATUS_BUSERRINT23           :  1;  /* [23]    r/w1clr */
            uint32_t STATUS_BUSERRINT24           :  1;  /* [24]    r/w1clr */
            uint32_t STATUS_BUSERRINT25           :  1;  /* [25]    r/w1clr */
            uint32_t STATUS_BUSERRINT26           :  1;  /* [26]    r/w1clr */
            uint32_t STATUS_BUSERRINT27           :  1;  /* [27]    r/w1clr */
            uint32_t STATUS_BUSERRINT28           :  1;  /* [28]    r/w1clr */
            uint32_t STATUS_BUSERRINT29           :  1;  /* [29]    r/w1clr */
            uint32_t STATUS_BUSERRINT30           :  1;  /* [30]    r/w1clr */
            uint32_t STATUS_BUSERRINT31           :  1;  /* [31]    r/w1clr */
        } BF;
        uint32_t WORDVAL;
    } STATUS_BUSERRINT;

    /* 0x018: DMA Channel source/target address algnment error interrupt mask */
    /* Registers */
    union {
        struct {
            uint32_t MASK_ADDRERRINT0             :  1;  /* [0]     r/w */
            uint32_t MASK_ADDRERRINT1             :  1;  /* [1]     r/w */
            uint32_t MASK_ADDRERRINT2             :  1;  /* [2]     r/w */
            uint32_t MASK_ADDRERRINT3             :  1;  /* [3]     r/w */
            uint32_t MASK_ADDRERRINT4             :  1;  /* [4]     r/w */
            uint32_t MASK_ADDRERRINT5             :  1;  /* [5]     r/w */
            uint32_t MASK_ADDRERRINT6             :  1;  /* [6]     r/w */
            uint32_t MASK_ADDRERRINT7             :  1;  /* [7]     r/w */
            uint32_t MASK_ADDRERRINT8             :  1;  /* [8]     r/w */
            uint32_t MASK_ADDRERRINT9             :  1;  /* [9]     r/w */
            uint32_t MASK_ADDRERRINT10            :  1;  /* [10]    r/w */
            uint32_t MASK_ADDRERRINT11            :  1;  /* [11]    r/w */
            uint32_t MASK_ADDRERRINT12            :  1;  /* [12]    r/w */
            uint32_t MASK_ADDRERRINT13            :  1;  /* [13]    r/w */
            uint32_t MASK_ADDRERRINT14            :  1;  /* [14]    r/w */
            uint32_t MASK_ADDRERRINT15            :  1;  /* [15]    r/w */
            uint32_t MASK_ADDRERRINT16            :  1;  /* [16]    r/w */
            uint32_t MASK_ADDRERRINT17            :  1;  /* [17]    r/w */
            uint32_t MASK_ADDRERRINT18            :  1;  /* [18]    r/w */
            uint32_t MASK_ADDRERRINT19            :  1;  /* [19]    r/w */
            uint32_t MASK_ADDRERRINT20            :  1;  /* [20]    r/w */
            uint32_t MASK_ADDRERRINT21            :  1;  /* [21]    r/w */
            uint32_t MASK_ADDRERRINT22            :  1;  /* [22]    r/w */
            uint32_t MASK_ADDRERRINT23            :  1;  /* [23]    r/w */
            uint32_t MASK_ADDRERRINT24            :  1;  /* [24]    r/w */
            uint32_t MASK_ADDRERRINT25            :  1;  /* [25]    r/w */
            uint32_t MASK_ADDRERRINT26            :  1;  /* [26]    r/w */
            uint32_t MASK_ADDRERRINT27            :  1;  /* [27]    r/w */
            uint32_t MASK_ADDRERRINT28            :  1;  /* [28]    r/w */
            uint32_t MASK_ADDRERRINT29            :  1;  /* [29]    r/w */
            uint32_t MASK_ADDRERRINT30            :  1;  /* [30]    r/w */
            uint32_t MASK_ADDRERRINT31            :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } MASK_ADDRERRINT;

    /* 0x01c: DMA Channel source/target address algnment error interrupt */
    /* Registers */
    union {
        struct {
            uint32_t STATUS_ADDRERRINT0           :  1;  /* [0]     r/w1clr */
            uint32_t STATUS_ADDRERRINT1           :  1;  /* [1]     r/w1clr */
            uint32_t STATUS_ADDRERRINT2           :  1;  /* [2]     r/w1clr */
            uint32_t STATUS_ADDRERRINT3           :  1;  /* [3]     r/w1clr */
            uint32_t STATUS_ADDRERRINT4           :  1;  /* [4]     r/w1clr */
            uint32_t STATUS_ADDRERRINT5           :  1;  /* [5]     r/w1clr */
            uint32_t STATUS_ADDRERRINT6           :  1;  /* [6]     r/w1clr */
            uint32_t STATUS_ADDRERRINT7           :  1;  /* [7]     r/w1clr */
            uint32_t STATUS_ADDRERRINT8           :  1;  /* [8]     r/w1clr */
            uint32_t STATUS_ADDRERRINT9           :  1;  /* [9]     r/w1clr */
            uint32_t STATUS_ADDRERRINT10          :  1;  /* [10]    r/w1clr */
            uint32_t STATUS_ADDRERRINT11          :  1;  /* [11]    r/w1clr */
            uint32_t STATUS_ADDRERRINT12          :  1;  /* [12]    r/w1clr */
            uint32_t STATUS_ADDRERRINT13          :  1;  /* [13]    r/w1clr */
            uint32_t STATUS_ADDRERRINT14          :  1;  /* [14]    r/w1clr */
            uint32_t STATUS_ADDRERRINT15          :  1;  /* [15]    r/w1clr */
            uint32_t STATUS_ADDRERRINT16          :  1;  /* [16]    r/w1clr */
            uint32_t STATUS_ADDRERRINT17          :  1;  /* [17]    r/w1clr */
            uint32_t STATUS_ADDRERRINT18          :  1;  /* [18]    r/w1clr */
            uint32_t STATUS_ADDRERRINT19          :  1;  /* [19]    r/w1clr */
            uint32_t STATUS_ADDRERRINT20          :  1;  /* [20]    r/w1clr */
            uint32_t STATUS_ADDRERRINT21          :  1;  /* [21]    r/w1clr */
            uint32_t STATUS_ADDRERRINT22          :  1;  /* [22]    r/w1clr */
            uint32_t STATUS_ADDRERRINT23          :  1;  /* [23]    r/w1clr */
            uint32_t STATUS_ADDRERRINT24          :  1;  /* [24]    r/w1clr */
            uint32_t STATUS_ADDRERRINT25          :  1;  /* [25]    r/w1clr */
            uint32_t STATUS_ADDRERRINT26          :  1;  /* [26]    r/w1clr */
            uint32_t STATUS_ADDRERRINT27          :  1;  /* [27]    r/w1clr */
            uint32_t STATUS_ADDRERRINT28          :  1;  /* [28]    r/w1clr */
            uint32_t STATUS_ADDRERRINT29          :  1;  /* [29]    r/w1clr */
            uint32_t STATUS_ADDRERRINT30          :  1;  /* [30]    r/w1clr */
            uint32_t STATUS_ADDRERRINT31          :  1;  /* [31]    r/w1clr */
        } BF;
        uint32_t WORDVAL;
    } STATUS_ADDRERRINT;

    /* 0x020: DMA CHANNEL INTERRUPT REGISTER */
    union {
        struct {
            uint32_t STATUS_CHLINT0               :  1;  /* [0]     r/o */
            uint32_t STATUS_CHLINT1               :  1;  /* [1]     r/o */
            uint32_t STATUS_CHLINT2               :  1;  /* [2]     r/o */
            uint32_t STATUS_CHLINT3               :  1;  /* [3]     r/o */
            uint32_t STATUS_CHLINT4               :  1;  /* [4]     r/o */
            uint32_t STATUS_CHLINT5               :  1;  /* [5]     r/o */
            uint32_t STATUS_CHLINT6               :  1;  /* [6]     r/o */
            uint32_t STATUS_CHLINT7               :  1;  /* [7]     r/o */
            uint32_t STATUS_CHLINT8               :  1;  /* [8]     r/o */
            uint32_t STATUS_CHLINT9               :  1;  /* [9]     r/o */
            uint32_t STATUS_CHLINT10              :  1;  /* [10]    r/o */
            uint32_t STATUS_CHLINT11              :  1;  /* [11]    r/o */
            uint32_t STATUS_CHLINT12              :  1;  /* [12]    r/o */
            uint32_t STATUS_CHLINT13              :  1;  /* [13]    r/o */
            uint32_t STATUS_CHLINT14              :  1;  /* [14]    r/o */
            uint32_t STATUS_CHLINT15              :  1;  /* [15]    r/o */
            uint32_t STATUS_CHLINT16              :  1;  /* [16]    r/o */
            uint32_t STATUS_CHLINT17              :  1;  /* [17]    r/o */
            uint32_t STATUS_CHLINT18              :  1;  /* [18]    r/o */
            uint32_t STATUS_CHLINT19              :  1;  /* [19]    r/o */
            uint32_t STATUS_CHLINT20              :  1;  /* [20]    r/o */
            uint32_t STATUS_CHLINT21              :  1;  /* [21]    r/o */
            uint32_t STATUS_CHLINT22              :  1;  /* [22]    r/o */
            uint32_t STATUS_CHLINT23              :  1;  /* [23]    r/o */
            uint32_t STATUS_CHLINT24              :  1;  /* [24]    r/o */
            uint32_t STATUS_CHLINT25              :  1;  /* [25]    r/o */
            uint32_t STATUS_CHLINT26              :  1;  /* [26]    r/o */
            uint32_t STATUS_CHLINT27              :  1;  /* [27]    r/o */
            uint32_t STATUS_CHLINT28              :  1;  /* [28]    r/o */
            uint32_t STATUS_CHLINT29              :  1;  /* [29]    r/o */
            uint32_t STATUS_CHLINT30              :  1;  /* [30]    r/o */
            uint32_t STATUS_CHLINT31              :  1;  /* [31]    r/o */
        } BF;
        uint32_t WORDVAL;
    } STATUS_CHLINT;

    uint8_t zReserved0x024[92];  /* pad 0x024 - 0x07f */

    /* 0x080: THE PROTECTION CONTROL SIGNALS REGISTERS */
    union {
        struct {
            uint32_t HPROT                        :  4;  /* [3:0]   r/w */
            uint32_t RESERVED_31_4                : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } HPROT;

    uint8_t zReserved0x084[124];  /* pad 0x084 - 0x0ff */

    struct {
        /* 0x100: DMA SOURCE ADDRESS REGISTERS */
        union {
            struct {
                uint32_t SRCADDR0                     :  2;  /* [1:0]   r/w */
                uint32_t SRCADDR                      : 30;  /* [31:2]  r/w */
            } BF;
            uint32_t WORDVAL;
        } SADR;

        /* 0x104: DMA TARGET ADDRESS REGISTERS */
        union {
            struct {
                uint32_t TRGADDR0                     :  2;  /* [1:0]   r/w */
                uint32_t TRGADDR                      : 30;  /* [31:2]  r/w */
            } BF;
            uint32_t WORDVAL;
        } TADR;

        /* 0x108: DMA CONTROL REGISTERS A */
        union {
            struct {
                uint32_t LEN                          : 13;  /* [12:0]  r/w */
                uint32_t WIDTH                        :  2;  /* [14:13] r/w */
                uint32_t TRAN_SIZE                    :  2;  /* [16:15] r/w */
                uint32_t TRAN_TYPE                    :  2;  /* [18:17] r/w */
                uint32_t RESERVED_28_19               : 10;  /* [28:19] rsvd */
                uint32_t INCTRGADDR                   :  1;  /* [29]    r/w */
                uint32_t INCSRCADDR                   :  1;  /* [30]    r/w */
                uint32_t RESERVED_31                  :  1;  /* [31]    rsvd */
            } BF;
            uint32_t WORDVAL;
        } CTRLA;

        /* 0x10c: DMA CONTROL REGISTERS B */
        union {
            struct {
                uint32_t PERNUM                       :  6;  /* [5:0]   r/w */
                uint32_t RESERVED_31_6                : 26;  /* [31:6]  rsvd */
            } BF;
            uint32_t WORDVAL;
        } CTRLB;

        /* 0x110: DMA CHANNEL ENABLE REGISTERS */
        union {
            struct {
                uint32_t RESERVED_30_0                : 31;  /* [30:0]  rsvd */
                uint32_t CHL_EN                       :  1;  /* [31]    r/w */
            } BF;
            uint32_t WORDVAL;
        } CHL_EN;

        /* 0x114: DMA CHANNEL STOP REGISTERS */
        union {
            struct {
                uint32_t RESERVED_30_0                : 31;  /* [30:0]  rsvd */
                uint32_t CHL_STOP                     :  1;  /* [31]    r/w */
            } BF;
            uint32_t WORDVAL;
        } CHL_STOP;
        uint8_t zReserved[24];
    } CHANNEL[32];

    //uint8_t zReserved0x400[1024];  /* pad 0x400 - 0x7ff */
    uint8_t zReserved0x718[256];  /* pad 0x718 - 0x7ff */

    /* 0x800: DMA ACK DELAY CYCLE for single transfer in M2P transfer type */
    /* Registers */
    union {
        struct {
            uint32_t ACK_DELAY_NUM                : 10;  /* [9:0]   r/w */
            uint32_t RESERVED_31_10               : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ACK_DELAY;

    uint8_t zReserved0x804[252];  /* pad 0x804 - 0x8ff */

    /* 0x900: DMA ERROR INFORMATION REGISTER0 */
    union {
        struct {
            uint32_t ERR_ADDR                     : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ERR_INFO0;

    /* 0x904: DMA ERROR INFORMATION REGISTER1 */
    union {
        struct {
            uint32_t RESERVED_26_0                : 27;  /* [26:0]  rsvd */
            uint32_t ERR_CHLNUM                   :  5;  /* [31:27] r/o */
        } BF;
        uint32_t WORDVAL;
    } ERR_INFO1;

    /* 0x908: DMA DIAGNOSE INFORMATION REGISTER0 */
    union {
        struct {
            uint32_t DIAGNOSE_ADDR                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DIAGNOSE_INFO0;

    /* 0x90c: DMA DIAGNOSE INFORMATION REGISTER1 */
    union {
        struct {
            uint32_t DIAGNOSE_REQ_CHL_DATA_CHLNUM :  5;  /* [4:0]   r/o */
            uint32_t RESERVED_17_5                : 13;  /* [17:5]  rsvd */
            uint32_t DIAGNOSE_REST_LEN            : 13;  /* [30:18] r/o */
            uint32_t DIAGNOSE_REQ_CHL_DATA        :  1;  /* [31]    r/o */
        } BF;
        uint32_t WORDVAL;
    } DIAGNOSE_INFO1;

    /* 0x910: DMA DIAGNOSE INFORMATION REGISTER2 */
    union {
        struct {
            uint32_t RESERVED_27_0                : 28;  /* [27:0]  rsvd */
            uint32_t DIAGNOSE_CHL_STATE           :  4;  /* [31:28] r/o */
        } BF;
        uint32_t WORDVAL;
    } DIAGNOSE_INFO2;

    /* 0x914: DMA DIAGNOSE INFORMATION REGISTER3 */
    union {
        struct {
            uint32_t DIAGNOSE_SRC_ADDR0           :  2;  /* [1:0]   r/o */
            uint32_t DIAGNOSE_SRC_ADDR            : 30;  /* [31:2]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DIAGNOSE_INFO3;

    /* 0x918: DMA DIAGNOSE INFORMATION REGISTER4 */
    union {
        struct {
            uint32_t DIAGNOSE_TRG_ADDR0           :  2;  /* [1:0]   r/o */
            uint32_t DIAGNOSE_TRG_ADDR            : 30;  /* [31:2]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DIAGNOSE_INFO4;

    /* 0x91c: DMA DIAGNOSE INFORMATION REGISTER5 */
    union {
        struct {
            uint32_t RESERVED_9_0                 : 10;  /* [9:0]   rsvd */
            uint32_t DIAGNOSE_CTRL_WIDTH          :  2;  /* [11:10] r/o */
            uint32_t DIAGNOSE_CTRL_TRANTYPE       :  2;  /* [13:12] r/o */
            uint32_t DIAGNOSE_CTRL_TRANSIZE       :  2;  /* [15:14] r/o */
            uint32_t DIAGNOSE_CTRL_LEN            : 13;  /* [28:16] r/o */
            uint32_t DIAGNOSE_INCRTRGADDR         :  1;  /* [29]    r/o */
            uint32_t DIAGNOSE_INCRSRCADDR         :  1;  /* [30]    r/o */
            uint32_t DIAGNOSE_CHL_EN              :  1;  /* [31]    r/o */
        } BF;
        uint32_t WORDVAL;
    } DIAGNOSE_INFO5;

    /* 0x920: DMA DIAGNOSE INFORMATION REGISTER6 */
    union {
        struct {
            uint32_t RESERVED_16_0                : 17;  /* [16:0]  rsvd */
            uint32_t DIAGNOSE_SPLIT_HALFWORD      :  1;  /* [17]    r/o */
            uint32_t DIAGNOSE_SPLIT_WORD          :  1;  /* [18]    r/o */
            uint32_t DIAGNOSE_SLICECNT            :  7;  /* [25:19] r/o */
            uint32_t DIAGNOSE_AHB_BURST           :  2;  /* [27:26] r/o */
            uint32_t DIAGNOSE_AHB_SIZE            :  2;  /* [29:28] r/o */
            uint32_t DIAGNOSE_MAS_WRITE           :  1;  /* [30]    r/o */
            uint32_t DIAGNOSE_MAS_READ            :  1;  /* [31]    r/o */
        } BF;
        uint32_t WORDVAL;
    } DIAGNOSE_INFO6;

    /* 0x924: DMA DIAGNOSE INFORMATION REGISTER7 */
    union {
        struct {
            uint32_t RESERVED_21_0                : 22;  /* [21:0]  rsvd */
            uint32_t DIAGNOSE_AHB_HRESP           :  1;  /* [22]    r/o */
            uint32_t DIAGNOSE_AHB_HREADY          :  1;  /* [23]    r/o */
            uint32_t DIAGNOSE_AHB_SIZE            :  3;  /* [26:24] r/o */
            uint32_t DIAGNOSE_AHB_TRANS           :  2;  /* [28:27] r/o */
            uint32_t DIAGNOSE_AHB_BURST           :  3;  /* [31:29] r/o */
        } BF;
        uint32_t WORDVAL;
    } DIAGNOSE_INFO7;

};

typedef volatile struct dma_reg dma_reg_t;

#ifdef DMA_IMPL
BEGIN_REG_SECTION(dma_registers)
dma_reg_t DMAREG;
END_REG_SECTION(dma_registers)
#else
extern dma_reg_t DMAREG;
#endif

#endif /* _DMA_REG_H */
