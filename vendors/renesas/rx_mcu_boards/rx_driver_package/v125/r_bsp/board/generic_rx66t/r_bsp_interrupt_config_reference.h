/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp_interrupt_config.h
* Description  : This module maps Interrupt A interrupts. More information on how this is done is given below.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 27.07.2018 1.00      First Release
***********************************************************************************************************************/
#ifndef R_BSP_INTERRUPT_CONFIG_REF_HEADER_FILE
#define R_BSP_INTERRUPT_CONFIG_REF_HEADER_FILE

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* Interrupt A Sources.
 * -Valid vector numbers are 208-255.
 * -There are more A sources than A vector slots. By default none of the GPTW interrupts are mapped.
 */
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIA0                              208
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIB0                              209
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIC0                              210
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGID0                              211
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TCIV0                              212
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIE0                              213
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIF0                              214
#define BSP_MAPPED_INT_CFG_A_VECT_MTU1_TGIA1                              215
#define BSP_MAPPED_INT_CFG_A_VECT_MTU1_TGIB1                              216
#define BSP_MAPPED_INT_CFG_A_VECT_MTU1_TCIV1                              217
#define BSP_MAPPED_INT_CFG_A_VECT_MTU1_TCIU1                              218
#define BSP_MAPPED_INT_CFG_A_VECT_MTU2_TGIA2                              219
#define BSP_MAPPED_INT_CFG_A_VECT_MTU2_TGIB2                              220
#define BSP_MAPPED_INT_CFG_A_VECT_MTU2_TCIV2                              221
#define BSP_MAPPED_INT_CFG_A_VECT_MTU2_TCIU2                              222
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIA3                              223
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIB3                              224
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIC3                              225
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGID3                              226
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TCIV3                              227
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIA4                              228
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIB4                              229
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIC4                              230
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGID4                              231
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TCIV4                              232
#define BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIU5                              233
#define BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIV5                              234
#define BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIW5                              235
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIA6                              236
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIB6                              237
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIC6                              238
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGID6                              239
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TCIV6                              240
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIA7                              241
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIB7                              242
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIC7                              243
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGID7                              244
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TCIV7                              245
#define BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIA9                              246
#define BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIB9                              247
#define BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIC9                              248
#define BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGID9                              249
#define BSP_MAPPED_INT_CFG_A_VECT_MTU9_TCIV9                              250
#define BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIE9                              251
#define BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIF9                              252
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIA0
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIB0
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIC0
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCID0
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GDTE0 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIE0
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIF0
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIV0
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIU0
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIA1
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIB1
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIC1
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCID1
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GDTE1 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIE1
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIF1
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIV1
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIU1
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIA2
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIB2
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIC2
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCID2
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GDTE2 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIE2
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIF2
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIV2
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIU2
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIA3
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIB3
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIC3
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCID3
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GDTE3 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIE3
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIF3
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIV3
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIU3
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIA4
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIB4
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIC4
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCID4
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GDTE4 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIE4
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIF4
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIV4
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIU4
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIA5
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIB5
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIC5
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCID5
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GDTE5 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIE5
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIF5
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIV5
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIU5
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIA6
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIB6
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIC6
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCID6
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GDTE6 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIE6
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIF6
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIV6
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIU6
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIA7
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIB7
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIC7
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCID7
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GDTE7 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIE7
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIF7
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIV7
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIU7
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIA8
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIB8
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIC8
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCID8
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GDTE8 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIE8
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIF8
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIV8
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIU8
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIA9
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIB9
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIC9
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCID9
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GDTE9 
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIE9
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIF9
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIV9
#define BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIU9

#endif /* R_BSP_INTERRUPT_CONFIG_REF_HEADER_FILE */

