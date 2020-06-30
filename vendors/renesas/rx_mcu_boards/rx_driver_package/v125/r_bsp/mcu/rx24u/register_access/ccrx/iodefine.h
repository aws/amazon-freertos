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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/*********************************************************************************
*
* Device     : RX/RX200/RX24T,RX24U
*
* File Name  : iodefine.h
*
* Abstract   : Definition of I/O Register.
*
* History    : 0.5  (2015-06-15)  [Hardware Manual Revision : 0.50 RX24T]
*            : 1.0  (2015-10-26)  [Hardware Manual Revision : 1.00 RX24T]
*            : 1.0A (2016-02-26)  [Hardware Manual Revision : 1.00 RX24T]
*            : 1.0B (2016-08-03)  [Hardware Manual Revision : 1.00 RX24T, Hardware Manual Revision : 0.50 RX24U]
*            : 1.0C (2016-11-24)  [Hardware Manual Revision : 1.10 RX24T, Hardware Manual Revision : 0.50 RX24U]
*            : 1.0D (2016-12-06)  [Hardware Manual Revision : 1.10 RX24T, Hardware Manual Revision : 0.50 RX24U]
*            : 1.0E (2017-01-06)  [Hardware Manual Revision : 1.10 RX24T, Hardware Manual Revision : 0.50 RX24U]
*            : 1.0F (2017-01-25)  [Hardware Manual Revision : 1.10 RX24T, Hardware Manual Revision : 0.50 RX24U]
*            : 1.0G (2017-01-26)  [Hardware Manual Revision : 1.10 RX24T, Hardware Manual Revision : 0.50 RX24U]
*            : 1.0H (2017-01-26)  [Hardware Manual Revision : 1.10 RX24T, Hardware Manual Revision : 0.50 RX24U]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2016 (2015) Renesas Electronics Corporation.
*
*********************************************************************************/
/********************************************************************************/
/*                                                                              */
/*  DESCRIPTION : Definition of ICU Register                                    */
/*  CPU TYPE    : RX24T,RX24U                                                   */
/*                                                                              */
/*  Usage : IR,DTCER,IER,IPR of ICU Register                                    */
/*     The following IR, DTCE, IEN, IPR macro functions simplify usage.         */
/*     The bit access operation is "Bit_Name(interrupt source,name)".           */
/*     A part of the name can be omitted.                                       */
/*     for example :                                                            */
/*       IR(MTU0,TGIA0) = 0;     expands to :                                   */
/*         ICU.IR[114].BIT.IR = 0;                                              */
/*                                                                              */
/*       DTCE(ICU,IRQ0) = 1;     expands to :                                   */
/*         ICU.DTCER[64].BIT.DTCE = 1;                                          */
/*                                                                              */
/*       IEN(CMT0,CMI0) = 1;     expands to :                                   */
/*         ICU.IER[0x03].BIT.IEN4 = 1;                                          */
/*                                                                              */
/*       IPR(MTU1,TGIA1) = 2;    expands to :                                   */
/*       IPR(MTU1,TGI  ) = 2;    // TGIA1,TGIB1 share IPR level.                */
/*         ICU.IPR[121].BIT.IPR = 2;                                            */
/*                                                                              */
/*       IPR(SCI1,ERI1) = 3;     expands to :                                   */
/*       IPR(SCI1,    ) = 3;     // SCI1 uses single IPR for all sources.       */
/*         ICU.IPR[218].BIT.IPR = 3;                                            */
/*                                                                              */
/*  Usage : #pragma interrupt Function_Identifier(vect=**)                      */
/*     The number of vector is "(interrupt source, name)".                      */
/*     for example :                                                            */
/*       #pragma interrupt INT_IRQ0(vect=VECT(ICU,IRQ0))          expands to :  */
/*         #pragma interrupt INT_IRQ0(vect=64)                                  */
/*       #pragma interrupt INT_CMT0_CMI0(vect=VECT(CMT0,CMI0))    expands to :  */
/*         #pragma interrupt INT_CMT0_CMI0(vect=28)                             */
/*       #pragma interrupt INT_MTU0_TGIA0(vect=VECT(MTU0,TGIA0))  expands to :  */
/*         #pragma interrupt INT_MTU0_TGIA0(vect=114)                           */
/*                                                                              */
/*  Usage : MSTPCRA,MSTPCRB,MSTPCRC of SYSTEM Register                          */
/*     The bit access operation is "MSTP(name)".                                */
/*     The name that can be used is a macro name defined with "iodefine.h".     */
/*     for example :                                                            */
/*       MSTP(TMR2) = 0;    // TMR2,TMR3,TMR23                    expands to :  */
/*         SYSTEM.MSTPCRA.BIT.MSTPA4  = 0;                                      */
/*       MSTP(MTU4) = 0;    // MTU,MTU0,MTU1,MTU2,MTU3,MTU4,...   expands to :  */
/*         SYSTEM.MSTPCRA.BIT.MSTPA9  = 0;                                      */
/*       MSTP(CMT3) = 0;    // CMT2,CMT3                          expands to :  */
/*         SYSTEM.MSTPCRA.BIT.MSTPA14 = 0;                                      */
/*                                                                              */
/*                                                                              */
/********************************************************************************/
#ifndef __RX24TIODEFINE_HEADER__
#define __RX24TIODEFINE_HEADER__

#define	IEN_BSC_BUSERR		IEN0
#define	IEN_FCU_FRDYI		IEN7
#define	IEN_ICU_SWINT		IEN3
#define	IEN_CMT0_CMI0		IEN4
#define	IEN_CMT1_CMI1		IEN5
#define	IEN_CMT2_CMI2		IEN6
#define	IEN_CMT3_CMI3		IEN7
#define	IEN_CAC_FERRF		IEN0
#define	IEN_CAC_MENDF		IEN1
#define	IEN_CAC_OVFF		IEN2
#define IEN_GPT_ETGIN           IEN0
#define IEN_GPT_ETGIP           IEN1
#define	IEN_RSPI0_SPEI0		IEN4
#define	IEN_RSPI0_SPRI0		IEN5
#define	IEN_RSPI0_SPTI0		IEN6
#define	IEN_RSPI0_SPII0		IEN7
#define IEN_GPT0_GTCIA0         IEN0
#define IEN_GPT0_GTCIB0         IEN1
#define IEN_GPT0_GTCIC0         IEN2
#define IEN_GPT0_GTCID0         IEN3
#define IEN_GPT0_GDTE0          IEN4
#define IEN_GPT0_GTCIE0         IEN5
#define IEN_GPT0_GTCIF0         IEN6
#define IEN_GPT0_GTCIV0         IEN7
#define IEN_GPT0_GTCIU0         IEN0
#define	IEN_DOC_DOPCF		IEN1
#define	IEN_RSCAN_COMFRXINT	IEN3
#define	IEN_RSCAN_RXFINT	IEN4
#define	IEN_RSCAN_TXINT		IEN5
#define	IEN_RSCAN_CHERRINT	IEN6
#define	IEN_RSCAN_GLERRINT	IEN7
#define	IEN_ICU_IRQ0		IEN0
#define	IEN_ICU_IRQ1		IEN1
#define	IEN_ICU_IRQ2		IEN2
#define	IEN_ICU_IRQ3		IEN3
#define	IEN_ICU_IRQ4		IEN4
#define	IEN_ICU_IRQ5		IEN5
#define	IEN_ICU_IRQ6		IEN6
#define	IEN_ICU_IRQ7		IEN7
#define	IEN_LVD_LVD1		IEN0
#define	IEN_LVD_LVD2		IEN1
#define IEN_GPT1_GTCIA1         IEN2
#define IEN_GPT1_GTCIB1         IEN3
#define IEN_GPT1_GTCIC1         IEN4
#define IEN_GPT1_GTCID1         IEN5
#define	IEN_S12AD_S12ADI	IEN6
#define	IEN_S12AD_GBADI		IEN7
#define	IEN_S12AD_GCADI		IEN0
#define	IEN_S12AD1_S12ADI1	IEN1
#define	IEN_S12AD1_GBADI1	IEN2
#define	IEN_S12AD1_GCADI1	IEN3
#define	IEN_CMPC0_CMPC0		IEN4
#define	IEN_CMPC1_CMPC1		IEN5
#define	IEN_CMPC2_CMPC2		IEN6
#define	IEN_S12AD2_S12ADI2	IEN7
#define	IEN_S12AD2_GBADI2	IEN0
#define	IEN_S12AD2_GCADI2	IEN1
#define	IEN_MTU0_TGIA0		IEN2
#define	IEN_MTU0_TGIB0		IEN3
#define	IEN_MTU0_TGIC0		IEN4
#define	IEN_MTU0_TGID0		IEN5
#define	IEN_MTU0_TCIV0		IEN6
#define	IEN_MTU0_TGIE0		IEN7
#define	IEN_MTU0_TGIF0		IEN0
#define	IEN_MTU1_TGIA1		IEN1
#define	IEN_MTU1_TGIB1		IEN2
#define	IEN_MTU1_TCIV1		IEN3
#define	IEN_MTU1_TCIU1		IEN4
#define	IEN_MTU2_TGIA2		IEN5
#define	IEN_MTU2_TGIB2		IEN6
#define	IEN_MTU2_TCIV2		IEN7
#define	IEN_MTU2_TCIU2		IEN0
#define	IEN_MTU3_TGIA3		IEN1
#define	IEN_MTU3_TGIB3		IEN2
#define	IEN_MTU3_TGIC3		IEN3
#define	IEN_MTU3_TGID3		IEN4
#define	IEN_MTU3_TCIV3		IEN5
#define	IEN_MTU4_TGIA4		IEN6
#define	IEN_MTU4_TGIB4		IEN7
#define	IEN_MTU4_TGIC4		IEN0
#define	IEN_MTU4_TGID4		IEN1
#define	IEN_MTU4_TCIV4		IEN2
#define	IEN_MTU5_TGIU5		IEN3
#define	IEN_MTU5_TGIV5		IEN4
#define	IEN_MTU5_TGIW5		IEN5
#define	IEN_MTU6_TGIA6		IEN6
#define	IEN_MTU6_TGIB6		IEN7
#define	IEN_MTU6_TGIC6		IEN0
#define	IEN_MTU6_TGID6		IEN1
#define	IEN_MTU6_TCIV6		IEN2
#define	IEN_MTU7_TGIA7		IEN5
#define	IEN_MTU7_TGIB7		IEN6
#define	IEN_MTU7_TGIC7		IEN7
#define	IEN_MTU7_TGID7		IEN0
#define	IEN_MTU7_TCIV7		IEN1
#define	IEN_MTU9_TGIA9		IEN7
#define	IEN_MTU9_TGIB9		IEN0
#define	IEN_MTU9_TGIC9		IEN1
#define	IEN_MTU9_TGID9		IEN2
#define	IEN_MTU9_TCIV9		IEN3
#define	IEN_MTU9_TGIE9		IEN4
#define	IEN_MTU9_TGIF9		IEN5
#define	IEN_POE_OEI1		IEN0
#define	IEN_POE_OEI2		IEN1
#define	IEN_POE_OEI3		IEN2
#define	IEN_POE_OEI4		IEN3
#define	IEN_POE_OEI5		IEN4
#define	IEN_CMPC3_CMPC3		IEN5
#define	IEN_TMR0_CMIA0		IEN6
#define	IEN_TMR0_CMIB0		IEN7
#define	IEN_TMR0_OVI0		IEN0
#define	IEN_TMR1_CMIA1		IEN1
#define	IEN_TMR1_CMIB1		IEN2
#define	IEN_TMR1_OVI1		IEN3
#define	IEN_TMR2_CMIA2		IEN4
#define	IEN_TMR2_CMIB2		IEN5
#define	IEN_TMR2_OVI2		IEN6
#define	IEN_TMR3_CMIA3		IEN7
#define	IEN_TMR3_CMIB3		IEN0
#define	IEN_TMR3_OVI3		IEN1
#define	IEN_TMR4_CMIA4		IEN2
#define	IEN_TMR4_CMIB4		IEN3
#define	IEN_TMR4_OVI4		IEN4
#define	IEN_TMR5_CMIA5		IEN5
#define	IEN_TMR5_CMIB5		IEN6
#define	IEN_TMR5_OVI5		IEN7
#define	IEN_TMR6_CMIA6		IEN0
#define	IEN_TMR6_CMIB6		IEN1
#define	IEN_TMR6_OVI6		IEN2
#define	IEN_TMR7_CMIA7		IEN3
#define	IEN_TMR7_CMIB7		IEN4
#define	IEN_TMR7_OVI7		IEN5
#define IEN_GPT1_GDTE1          IEN2
#define IEN_GPT1_GTCIE1         IEN3
#define IEN_GPT1_GTCIF1         IEN4
#define IEN_GPT1_GTCIV1         IEN5
#define IEN_GPT1_GTCIU1         IEN6
#define IEN_GPT2_GTCIA2         IEN7
#define IEN_GPT2_GTCIB2         IEN0
#define IEN_GPT2_GTCIC2         IEN1
#define IEN_GPT2_GTCID2         IEN2
#define IEN_GPT2_GDTE2          IEN3
#define IEN_GPT2_GTCIE2         IEN4
#define IEN_GPT2_GTCIF2         IEN5
#define IEN_GPT2_GTCIV2         IEN6
#define IEN_GPT2_GTCIU2         IEN7
#define IEN_GPT3_GTCIA3         IEN0
#define IEN_GPT3_GTCIB3         IEN1
#define	IEN_SCI1_ERI1		IEN2
#define	IEN_SCI1_RXI1		IEN3
#define	IEN_SCI1_TXI1		IEN4
#define	IEN_SCI1_TEI1		IEN5
#define	IEN_SCI5_ERI5		IEN6
#define	IEN_SCI5_RXI5		IEN7
#define	IEN_SCI5_TXI5		IEN0
#define	IEN_SCI5_TEI5		IEN1
#define	IEN_SCI6_ERI6		IEN2
#define	IEN_SCI6_RXI6		IEN3
#define	IEN_SCI6_TXI6		IEN4
#define	IEN_SCI6_TEI6		IEN5
#define IEN_SCI8_ERI8           IEN6
#define IEN_SCI8_RXI8           IEN7
#define IEN_SCI8_TXI8           IEN0
#define IEN_SCI8_TEI8           IEN1
#define IEN_SCI9_ERI9           IEN2
#define IEN_SCI9_RXI9           IEN3
#define IEN_SCI9_TXI9           IEN4
#define IEN_SCI9_TEI9           IEN5
#define IEN_GPT3_GTCIC3         IEN6
#define IEN_GPT3_GTCID3         IEN7
#define IEN_GPT3_GDTE3          IEN0
#define IEN_GPT3_GTCIE3         IEN1
#define IEN_GPT3_GTCIF3         IEN2
#define IEN_GPT3_GTCIV3         IEN3
#define IEN_GPT3_GTCIU3         IEN4
#define	IEN_RIIC0_EEI0		IEN6
#define	IEN_RIIC0_RXI0		IEN7
#define	IEN_RIIC0_TXI0		IEN0
#define	IEN_RIIC0_TEI0		IEN1
#define IEN_SCI11_ERI11         IEN2
#define IEN_SCI11_RXI11         IEN3
#define IEN_SCI11_TXI11         IEN4
#define IEN_SCI11_TEI11         IEN5

#define	VECT_BSC_BUSERR		16
#define	VECT_FCU_FRDYI		23
#define	VECT_ICU_SWINT		27
#define	VECT_CMT0_CMI0		28
#define	VECT_CMT1_CMI1		29
#define	VECT_CMT2_CMI2		30
#define	VECT_CMT3_CMI3		31
#define	VECT_CAC_FERRF		32
#define	VECT_CAC_MENDF		33
#define	VECT_CAC_OVFF		34
#define VECT_GPT_ETGIN          40
#define VECT_GPT_ETGIP          41
#define	VECT_RSPI0_SPEI0	44
#define	VECT_RSPI0_SPRI0	45
#define	VECT_RSPI0_SPTI0	46
#define	VECT_RSPI0_SPII0	47
#define VECT_GPT0_GTCIA0        48
#define VECT_GPT0_GTCIB0        49
#define VECT_GPT0_GTCIC0        50
#define VECT_GPT0_GTCID0        51
#define VECT_GPT0_GDTE0         52
#define VECT_GPT0_GTCIE0        53
#define VECT_GPT0_GTCIF0        54
#define VECT_GPT0_GTCIV0        55
#define VECT_GPT0_GTCIU0        56
#define	VECT_DOC_DOPCF		57
#define	VECT_RSCAN_COMFRXINT	59
#define	VECT_RSCAN_RXFINT	60
#define	VECT_RSCAN_TXINT	61
#define	VECT_RSCAN_CHERRINT	62
#define	VECT_RSCAN_GLERRINT	63
#define	VECT_ICU_IRQ0		64
#define	VECT_ICU_IRQ1		65
#define	VECT_ICU_IRQ2		66
#define	VECT_ICU_IRQ3		67
#define	VECT_ICU_IRQ4		68
#define	VECT_ICU_IRQ5		69
#define	VECT_ICU_IRQ6		70
#define	VECT_ICU_IRQ7		71
#define	VECT_LVD_LVD1		88
#define	VECT_LVD_LVD2		89
#define VECT_GPT1_GTCIA1        98
#define VECT_GPT1_GTCIB1        99
#define VECT_GPT1_GTCIC1        100
#define VECT_GPT1_GTCID1        101
#define	VECT_S12AD_S12ADI	102
#define	VECT_S12AD_GBADI	103
#define	VECT_S12AD_GCADI	104
#define	VECT_S12AD1_S12ADI1	105
#define	VECT_S12AD1_GBADI1	106
#define	VECT_S12AD1_GCADI1	107
#define	VECT_CMPC0_CMPC0	108
#define	VECT_CMPC1_CMPC1	109
#define	VECT_CMPC2_CMPC2	110
#define	VECT_S12AD2_S12ADI2	111
#define	VECT_S12AD2_GBADI2	112
#define	VECT_S12AD2_GCADI2	113
#define	VECT_MTU0_TGIA0		114
#define	VECT_MTU0_TGIB0		115
#define	VECT_MTU0_TGIC0		116
#define	VECT_MTU0_TGID0		117
#define	VECT_MTU0_TCIV0		118
#define	VECT_MTU0_TGIE0		119
#define	VECT_MTU0_TGIF0		120
#define	VECT_MTU1_TGIA1		121
#define	VECT_MTU1_TGIB1		122
#define	VECT_MTU1_TCIV1		123
#define	VECT_MTU1_TCIU1		124
#define	VECT_MTU2_TGIA2		125
#define	VECT_MTU2_TGIB2		126
#define	VECT_MTU2_TCIV2		127
#define	VECT_MTU2_TCIU2		128
#define	VECT_MTU3_TGIA3		129
#define	VECT_MTU3_TGIB3		130
#define	VECT_MTU3_TGIC3		131
#define	VECT_MTU3_TGID3		132
#define	VECT_MTU3_TCIV3		133
#define	VECT_MTU4_TGIA4		134
#define	VECT_MTU4_TGIB4		135
#define	VECT_MTU4_TGIC4		136
#define	VECT_MTU4_TGID4		137
#define	VECT_MTU4_TCIV4		138
#define	VECT_MTU5_TGIU5		139
#define	VECT_MTU5_TGIV5		140
#define	VECT_MTU5_TGIW5		141
#define	VECT_MTU6_TGIA6		142
#define	VECT_MTU6_TGIB6		143
#define	VECT_MTU6_TGIC6		144
#define	VECT_MTU6_TGID6		145
#define	VECT_MTU6_TCIV6		146
#define	VECT_MTU7_TGIA7		149
#define	VECT_MTU7_TGIB7		150
#define	VECT_MTU7_TGIC7		151
#define	VECT_MTU7_TGID7		152
#define	VECT_MTU7_TCIV7		153
#define	VECT_MTU9_TGIA9		159
#define	VECT_MTU9_TGIB9		160
#define	VECT_MTU9_TGIC9		161
#define	VECT_MTU9_TGID9		162
#define	VECT_MTU9_TCIV9		163
#define	VECT_MTU9_TGIE9		164
#define	VECT_MTU9_TGIF9		165
#define	VECT_POE_OEI1		168
#define	VECT_POE_OEI2		169
#define	VECT_POE_OEI3		170
#define	VECT_POE_OEI4		171
#define	VECT_POE_OEI5		172
#define	VECT_CMPC3_CMPC3	173
#define	VECT_TMR0_CMIA0		174
#define	VECT_TMR0_CMIB0		175
#define	VECT_TMR0_OVI0		176
#define	VECT_TMR1_CMIA1		177
#define	VECT_TMR1_CMIB1		178
#define	VECT_TMR1_OVI1		179
#define	VECT_TMR2_CMIA2		180
#define	VECT_TMR2_CMIB2		181
#define	VECT_TMR2_OVI2		182
#define	VECT_TMR3_CMIA3		183
#define	VECT_TMR3_CMIB3		184
#define	VECT_TMR3_OVI3		185
#define	VECT_TMR4_CMIA4		186
#define	VECT_TMR4_CMIB4		187
#define	VECT_TMR4_OVI4		188
#define	VECT_TMR5_CMIA5		189
#define	VECT_TMR5_CMIB5		190
#define	VECT_TMR5_OVI5		191
#define	VECT_TMR6_CMIA6		192
#define	VECT_TMR6_CMIB6		193
#define	VECT_TMR6_OVI6		194
#define	VECT_TMR7_CMIA7		195
#define	VECT_TMR7_CMIB7		196
#define	VECT_TMR7_OVI7		197
#define VECT_GPT1_GDTE1         202
#define VECT_GPT1_GTCIE1        203
#define VECT_GPT1_GTCIF1        204
#define VECT_GPT1_GTCIV1        205
#define VECT_GPT1_GTCIU1        206
#define VECT_GPT2_GTCIA2        207
#define VECT_GPT2_GTCIB2        208
#define VECT_GPT2_GTCIC2        209
#define VECT_GPT2_GTCID2        210
#define VECT_GPT2_GDTE2         211
#define VECT_GPT2_GTCIE2        212
#define VECT_GPT2_GTCIF2        213
#define VECT_GPT2_GTCIV2        214
#define VECT_GPT2_GTCIU2        215
#define VECT_GPT3_GTCIA3        216
#define VECT_GPT3_GTCIB3        217
#define	VECT_SCI1_ERI1		218
#define	VECT_SCI1_RXI1		219
#define	VECT_SCI1_TXI1		220
#define	VECT_SCI1_TEI1		221
#define	VECT_SCI5_ERI5		222
#define	VECT_SCI5_RXI5		223
#define	VECT_SCI5_TXI5		224
#define	VECT_SCI5_TEI5		225
#define	VECT_SCI6_ERI6		226
#define	VECT_SCI6_RXI6		227
#define	VECT_SCI6_TXI6		228
#define	VECT_SCI6_TEI6		229
#define VECT_SCI8_ERI8          230
#define VECT_SCI8_RXI8          231
#define VECT_SCI8_TXI8          232
#define VECT_SCI8_TEI8          233
#define VECT_SCI9_ERI9          234
#define VECT_SCI9_RXI9          235
#define VECT_SCI9_TXI9          236
#define VECT_SCI9_TEI9          237
#define VECT_GPT3_GTCIC3        238
#define VECT_GPT3_GTCID3        239
#define VECT_GPT3_GDTE3         240
#define VECT_GPT3_GTCIE3        241
#define VECT_GPT3_GTCIF3        242
#define VECT_GPT3_GTCIV3        243
#define VECT_GPT3_GTCIU3        244
#define	VECT_RIIC0_EEI0		246
#define	VECT_RIIC0_RXI0		247
#define	VECT_RIIC0_TXI0		248
#define	VECT_RIIC0_TEI0		249
#define VECT_SCI11_ERI11        250
#define VECT_SCI11_RXI11        251
#define VECT_SCI11_TXI11        252
#define VECT_SCI11_TEI11        253

#define	MSTP_DTC	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_S12AD2	SYSTEM.MSTPCRA.BIT.MSTPA23
#define	MSTP_DA		SYSTEM.MSTPCRA.BIT.MSTPA19
#define	MSTP_S12AD	SYSTEM.MSTPCRA.BIT.MSTPA17
#define	MSTP_S12AD1	SYSTEM.MSTPCRA.BIT.MSTPA16
#define	MSTP_CMT0	SYSTEM.MSTPCRA.BIT.MSTPA15
#define	MSTP_CMT1	SYSTEM.MSTPCRA.BIT.MSTPA15
#define	MSTP_CMT2	SYSTEM.MSTPCRA.BIT.MSTPA14
#define	MSTP_CMT3	SYSTEM.MSTPCRA.BIT.MSTPA14
#define	MSTP_MTU	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU0	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU1	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU2	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU3	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU4	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU5	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU6	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU7	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU9	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_GPT	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPT0	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPT1	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPT2	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPT3	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPT01	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPT23	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_TMR0	SYSTEM.MSTPCRA.BIT.MSTPA5
#define	MSTP_TMR1	SYSTEM.MSTPCRA.BIT.MSTPA5
#define	MSTP_TMR01	SYSTEM.MSTPCRA.BIT.MSTPA5
#define	MSTP_TMR2	SYSTEM.MSTPCRA.BIT.MSTPA4
#define	MSTP_TMR3	SYSTEM.MSTPCRA.BIT.MSTPA4
#define	MSTP_TMR23	SYSTEM.MSTPCRA.BIT.MSTPA4
#define	MSTP_TMR4	SYSTEM.MSTPCRA.BIT.MSTPA3
#define	MSTP_TMR5	SYSTEM.MSTPCRA.BIT.MSTPA3
#define	MSTP_TMR45	SYSTEM.MSTPCRA.BIT.MSTPA3
#define	MSTP_TMR6	SYSTEM.MSTPCRA.BIT.MSTPA2
#define	MSTP_TMR7	SYSTEM.MSTPCRA.BIT.MSTPA2
#define	MSTP_TMR67	SYSTEM.MSTPCRA.BIT.MSTPA2
#define	MSTP_SCI1	SYSTEM.MSTPCRB.BIT.MSTPB30
#define	MSTP_SMCI1	SYSTEM.MSTPCRB.BIT.MSTPB30
#define	MSTP_SCI5	SYSTEM.MSTPCRB.BIT.MSTPB26
#define	MSTP_SMCI5	SYSTEM.MSTPCRB.BIT.MSTPB26
#define	MSTP_SCI6	SYSTEM.MSTPCRB.BIT.MSTPB25
#define	MSTP_SMCI6	SYSTEM.MSTPCRB.BIT.MSTPB25
#define	MSTP_CRC	SYSTEM.MSTPCRB.BIT.MSTPB23
#define	MSTP_RIIC0	SYSTEM.MSTPCRB.BIT.MSTPB21
#define	MSTP_RSPI0	SYSTEM.MSTPCRB.BIT.MSTPB17
#define	MSTP_CMPC0	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC1	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC2	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC3	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_DOC	SYSTEM.MSTPCRB.BIT.MSTPB6
#define	MSTP_RSCAN	SYSTEM.MSTPCRB.BIT.MSTPB0
#define	MSTP_RSCAN0	SYSTEM.MSTPCRB.BIT.MSTPB0
#define	MSTP_SCI8	SYSTEM.MSTPCRC.BIT.MSTPC27
#define	MSTP_SMCI8	SYSTEM.MSTPCRC.BIT.MSTPC27
#define	MSTP_SCI9	SYSTEM.MSTPCRC.BIT.MSTPC26
#define	MSTP_SMCI9	SYSTEM.MSTPCRC.BIT.MSTPC26
#define	MSTP_SCI11	SYSTEM.MSTPCRC.BIT.MSTPC24
#define	MSTP_SMCI11	SYSTEM.MSTPCRC.BIT.MSTPC24
#define	MSTP_CAC	SYSTEM.MSTPCRC.BIT.MSTPC19
#define	MSTP_RAM0	SYSTEM.MSTPCRC.BIT.MSTPC0

#define	__IR( x )		ICU.IR[ IR ## x ].BIT.IR
#define	 _IR( x )		__IR( x )
#define	  IR( x , y )	_IR( _ ## x ## _ ## y )
#define	__DTCE( x )		ICU.DTCER[ DTCE ## x ].BIT.DTCE
#define	 _DTCE( x )		__DTCE( x )
#define	  DTCE( x , y )	_DTCE( _ ## x ## _ ## y )
#define	__IEN( x )		ICU.IER[ IER ## x ].BIT.IEN ## x
#define	 _IEN( x )		__IEN( x )
#define	  IEN( x , y )	_IEN( _ ## x ## _ ## y )
#define	__IPR( x )		ICU.IPR[ IPR ## x ].BIT.IPR
#define	 _IPR( x )		__IPR( x )
#define	  IPR( x , y )	_IPR( _ ## x ## _ ## y )
#define	__VECT( x )		VECT ## x
#define	 _VECT( x )		__VECT( x )
#define	  VECT( x , y )	_VECT( _ ## x ## _ ## y )
#define	__MSTP( x )		MSTP ## x
#define	 _MSTP( x )		__MSTP( x )
#define	  MSTP( x )		_MSTP( _ ## x )

#define	BSC		(*(volatile struct st_bsc        __evenaccess *)0x81300)
#define	CAC		(*(volatile struct st_cac        __evenaccess *)0x8B000)
#define	RSCAN	(*(volatile struct st_rscan        __evenaccess *)0xA8322)
#define	RSCAN0	(*(volatile struct st_rscan0       __evenaccess *)0xA8300)
#define	CMPC0	(*(volatile struct st_cmpc       __evenaccess *)0xA0C80)
#define	CMPC1	(*(volatile struct st_cmpc       __evenaccess *)0xA0CA0)
#define	CMPC2	(*(volatile struct st_cmpc       __evenaccess *)0xA0CC0)
#define	CMPC3	(*(volatile struct st_cmpc       __evenaccess *)0xA0CE0)
#define	CMT		(*(volatile struct st_cmt        __evenaccess *)0x88000)
#define	CMT0	(*(volatile struct st_cmt0       __evenaccess *)0x88002)
#define	CMT1	(*(volatile struct st_cmt0       __evenaccess *)0x88008)
#define	CMT2	(*(volatile struct st_cmt0       __evenaccess *)0x88012)
#define	CMT3	(*(volatile struct st_cmt0       __evenaccess *)0x88018)
#define	CRC		(*(volatile struct st_crc        __evenaccess *)0x88280)
#define	DA		(*(volatile struct st_da         __evenaccess *)0x880C0)
#define	DOC		(*(volatile struct st_doc        __evenaccess *)0x8B080)
#define	DTC		(*(volatile struct st_dtc        __evenaccess *)0x82400)
#define	FLASH	(*(volatile struct st_flash      __evenaccess *)0x81000)
#define	FLASHCONST		(*(volatile struct st_flashconst __evenaccess *)0x7FC350)
#define	GPT		(*(volatile struct st_gpt        __evenaccess *)0xC2000)
#define	GPT0	(*(volatile struct st_gpt0       __evenaccess *)0xC2100)
#define	GPT1	(*(volatile struct st_gpt0       __evenaccess *)0xC2180)
#define	GPT2	(*(volatile struct st_gpt0       __evenaccess *)0xC2200)
#define	GPT3	(*(volatile struct st_gpt0       __evenaccess *)0xC2280)
#define	GPT01	(*(volatile struct st_gpt01      __evenaccess *)0xC2300)
#define	GPT23	(*(volatile struct st_gpt01      __evenaccess *)0xC2380)
#define	ICU		(*(volatile struct st_icu        __evenaccess *)0x87000)
#define	IWDT	(*(volatile struct st_iwdt       __evenaccess *)0x88030)
#define	MPC		(*(volatile struct st_mpc        __evenaccess *)0x8C11F)
#define	MPU		(*(volatile struct st_mpu        __evenaccess *)0x86400)
#define	MTU		(*(volatile struct st_mtu        __evenaccess *)0xC120A)
#define	MTU0	(*(volatile struct st_mtu0       __evenaccess *)0xC1290)
#define	MTU1	(*(volatile struct st_mtu1       __evenaccess *)0xC1290)
#define	MTU2	(*(volatile struct st_mtu2       __evenaccess *)0xC1292)
#define	MTU3	(*(volatile struct st_mtu3       __evenaccess *)0xC1200)
#define	MTU4	(*(volatile struct st_mtu4       __evenaccess *)0xC1200)
#define	MTU5	(*(volatile struct st_mtu5       __evenaccess *)0xC1A94)
#define	MTU6	(*(volatile struct st_mtu6       __evenaccess *)0xC1A00)
#define	MTU7	(*(volatile struct st_mtu7       __evenaccess *)0xC1A00)
#define	MTU9	(*(volatile struct st_mtu9       __evenaccess *)0xC1296)
#define	POE		(*(volatile struct st_poe        __evenaccess *)0x8C4C0)
#define	PORT0	(*(volatile struct st_port0      __evenaccess *)0x8C000)
#define	PORT1	(*(volatile struct st_port1      __evenaccess *)0x8C001)
#define	PORT2	(*(volatile struct st_port2      __evenaccess *)0x8C002)
#define	PORT3	(*(volatile struct st_port3      __evenaccess *)0x8C003)
#define	PORT4	(*(volatile struct st_port4      __evenaccess *)0x8C004)
#define	PORT5	(*(volatile struct st_port5      __evenaccess *)0x8C005)
#define	PORT6	(*(volatile struct st_port6      __evenaccess *)0x8C006)
#define	PORT7	(*(volatile struct st_port7      __evenaccess *)0x8C007)
#define	PORT8	(*(volatile struct st_port8      __evenaccess *)0x8C008)
#define	PORT9	(*(volatile struct st_port9      __evenaccess *)0x8C009)
#define	PORTA	(*(volatile struct st_porta      __evenaccess *)0x8C00A)
#define	PORTB	(*(volatile struct st_portb      __evenaccess *)0x8C00B)
#define	PORTC	(*(volatile struct st_portc      __evenaccess *)0x8C00C)
#define	PORTD	(*(volatile struct st_portd      __evenaccess *)0x8C00D)
#define	PORTE	(*(volatile struct st_porte      __evenaccess *)0x8C00E)
#define	PORTF	(*(volatile struct st_portf      __evenaccess *)0x8C00F)
#define	PORTG	(*(volatile struct st_portg      __evenaccess *)0x8C010)
#define	RIIC0	(*(volatile struct st_riic       __evenaccess *)0x88300)
#define	RSPI0	(*(volatile struct st_rspi       __evenaccess *)0x88380)
#define	S12AD	(*(volatile struct st_s12ad      __evenaccess *)0x89000)
#define	S12AD1	(*(volatile struct st_s12ad1     __evenaccess *)0x89200)
#define	S12AD2	(*(volatile struct st_s12ad2     __evenaccess *)0x89400)
#define	SCI1	(*(volatile struct st_sci        __evenaccess *)0x8A020)
#define	SCI5	(*(volatile struct st_sci        __evenaccess *)0x8A0A0)
#define	SCI6	(*(volatile struct st_sci        __evenaccess *)0x8A0C0)
#define	SCI8	(*(volatile struct st_sci        __evenaccess *)0x8A100)
#define	SCI9	(*(volatile struct st_sci        __evenaccess *)0x8A120)
#define	SCI11	(*(volatile struct st_sci        __evenaccess *)0xD0000)
#define	SMCI1	(*(volatile struct st_smci       __evenaccess *)0x8A020)
#define	SMCI5	(*(volatile struct st_smci       __evenaccess *)0x8A0A0)
#define	SMCI6	(*(volatile struct st_smci       __evenaccess *)0x8A0C0)
#define	SMCI8	(*(volatile struct st_smci       __evenaccess *)0x8A100)
#define	SMCI9	(*(volatile struct st_smci       __evenaccess *)0x8A120)
#define	SMCI11	(*(volatile struct st_smci       __evenaccess *)0xD0000)
#define	SYSTEM	(*(volatile struct st_system     __evenaccess *)0x80000)
#define	TMR0	(*(volatile struct st_tmr0       __evenaccess *)0x88200)
#define	TMR1	(*(volatile struct st_tmr1       __evenaccess *)0x88201)
#define	TMR2	(*(volatile struct st_tmr0       __evenaccess *)0x88210)
#define	TMR3	(*(volatile struct st_tmr1       __evenaccess *)0x88211)
#define	TMR4	(*(volatile struct st_tmr0       __evenaccess *)0x88220)
#define	TMR5	(*(volatile struct st_tmr1       __evenaccess *)0x88221)
#define	TMR6	(*(volatile struct st_tmr0       __evenaccess *)0x88230)
#define	TMR7	(*(volatile struct st_tmr1       __evenaccess *)0x88231)
#define	TMR01	(*(volatile struct st_tmr01      __evenaccess *)0x88204)
#define	TMR23	(*(volatile struct st_tmr01      __evenaccess *)0x88214)
#define	TMR45	(*(volatile struct st_tmr01      __evenaccess *)0x88224)
#define	TMR67	(*(volatile struct st_tmr01      __evenaccess *)0x88234)

typedef enum enum_ir {
IR_BSC_BUSERR=16,IR_FCU_FRDYI=23,
IR_ICU_SWINT=27,
IR_CMT0_CMI0,
IR_CMT1_CMI1,
IR_CMT2_CMI2,
IR_CMT3_CMI3,
IR_CAC_FERRF,IR_CAC_MENDF,IR_CAC_OVFF,
IR_GPT_ETGIN=40,IR_GPT_ETGIP,
IR_RSPI0_SPEI0=44,IR_RSPI0_SPRI0,IR_RSPI0_SPTI0,IR_RSPI0_SPII0,
IR_GPT0_GTCIA0,IR_GPT0_GTCIB0,IR_GPT0_GTCIC0,IR_GPT0_GTCID0,IR_GPT0_GDTE0,
IR_GPT0_GTCIE0,IR_GPT0_GTCIF0,IR_GPT0_GTCIV0,IR_GPT0_GTCIU0,
IR_DOC_DOPCF=57,
IR_RSCAN_COMFRXINT=59,IR_RSCAN_RXFINT,IR_RSCAN_TXINT,IR_RSCAN_CHERRINT,IR_RSCAN_GLERRINT,
IR_ICU_IRQ0=64,IR_ICU_IRQ1,IR_ICU_IRQ2,IR_ICU_IRQ3,IR_ICU_IRQ4,IR_ICU_IRQ5,IR_ICU_IRQ6,IR_ICU_IRQ7,
IR_LVD_LVD1=88,IR_LVD_LVD2,
IR_GPT1_GTCIA1=98,IR_GPT1_GTCIB1,IR_GPT1_GTCIC1,IR_GPT1_GTCID1,
IR_S12AD_S12ADI=102,IR_S12AD_GBADI,IR_S12AD_GCADI,
IR_S12AD1_S12ADI1,IR_S12AD1_GBADI1,IR_S12AD1_GCADI1,
IR_CMPC0_CMPC0,
IR_CMPC1_CMPC1,
IR_CMPC2_CMPC2,
IR_S12AD2_S12ADI2,IR_S12AD2_GBADI2,IR_S12AD2_GCADI2,
IR_MTU0_TGIA0,IR_MTU0_TGIB0,IR_MTU0_TGIC0,IR_MTU0_TGID0,IR_MTU0_TCIV0,IR_MTU0_TGIE0,IR_MTU0_TGIF0,
IR_MTU1_TGIA1,IR_MTU1_TGIB1,IR_MTU1_TCIV1,IR_MTU1_TCIU1,
IR_MTU2_TGIA2,IR_MTU2_TGIB2,IR_MTU2_TCIV2,IR_MTU2_TCIU2,
IR_MTU3_TGIA3,IR_MTU3_TGIB3,IR_MTU3_TGIC3,IR_MTU3_TGID3,IR_MTU3_TCIV3,
IR_MTU4_TGIA4,IR_MTU4_TGIB4,IR_MTU4_TGIC4,IR_MTU4_TGID4,IR_MTU4_TCIV4,
IR_MTU5_TGIU5,IR_MTU5_TGIV5,IR_MTU5_TGIW5,
IR_MTU6_TGIA6,IR_MTU6_TGIB6,IR_MTU6_TGIC6,IR_MTU6_TGID6,IR_MTU6_TCIV6,
IR_MTU7_TGIA7=149,IR_MTU7_TGIB7,IR_MTU7_TGIC7,IR_MTU7_TGID7,IR_MTU7_TCIV7,
IR_MTU9_TGIA9=159,IR_MTU9_TGIB9,IR_MTU9_TGIC9,IR_MTU9_TGID9,IR_MTU9_TCIV9,IR_MTU9_TGIE9,IR_MTU9_TGIF9,
IR_POE_OEI1=168,IR_POE_OEI2,IR_POE_OEI3,IR_POE_OEI4,IR_POE_OEI5,
IR_CMPC3_CMPC3,
IR_TMR0_CMIA0,IR_TMR0_CMIB0,IR_TMR0_OVI0,
IR_TMR1_CMIA1,IR_TMR1_CMIB1,IR_TMR1_OVI1,
IR_TMR2_CMIA2,IR_TMR2_CMIB2,IR_TMR2_OVI2,
IR_TMR3_CMIA3,IR_TMR3_CMIB3,IR_TMR3_OVI3,
IR_TMR4_CMIA4,IR_TMR4_CMIB4,IR_TMR4_OVI4,
IR_TMR5_CMIA5,IR_TMR5_CMIB5,IR_TMR5_OVI5,
IR_TMR6_CMIA6,IR_TMR6_CMIB6,IR_TMR6_OVI6,
IR_TMR7_CMIA7,IR_TMR7_CMIB7,IR_TMR7_OVI7,
IR_GPT1_GDTE1=202,IR_GPT1_GTCIE1,IR_GPT1_GTCIF1,IR_GPT1_GTCIV1,IR_GPT1_GTCIU1,
IR_GPT2_GTCIA2,IR_GPT2_GTCIB2,IR_GPT2_GTCIC2,IR_GPT2_GTCID2,IR_GPT2_GDTE2,
IR_GPT2_GTCIE2,IR_GPT2_GTCIF2,IR_GPT2_GTCIV2,IR_GPT2_GTCIU2,
IR_GPT3_GTCIA3,IR_GPT3_GTCIB3,
IR_SCI1_ERI1,IR_SCI1_RXI1,IR_SCI1_TXI1,IR_SCI1_TEI1,
IR_SCI5_ERI5,IR_SCI5_RXI5,IR_SCI5_TXI5,IR_SCI5_TEI5,
IR_SCI6_ERI6,IR_SCI6_RXI6,IR_SCI6_TXI6,IR_SCI6_TEI6,
IR_SCI8_ERI8,IR_SCI8_RXI8,IR_SCI8_TXI8,IR_SCI8_TEI8,
IR_SCI9_ERI9,IR_SCI9_RXI9,IR_SCI9_TXI9,IR_SCI9_TEI9,
IR_GPT3_GTCIC3,IR_GPT3_GTCID3,IR_GPT3_GDTE3,
IR_GPT3_GTCIE3,IR_GPT3_GTCIF3,IR_GPT3_GTCIV3,IR_GPT3_GTCIU3,
IR_RIIC0_EEI0=246,IR_RIIC0_RXI0,IR_RIIC0_TXI0,IR_RIIC0_TEI0,
IR_SCI11_ERI11,IR_SCI11_RXI11,IR_SCI11_TXI11,IR_SCI11_TEI11
} enum_ir_t;

typedef enum enum_dtce {
DTCE_ICU_SWINT=27,
DTCE_CMT0_CMI0,
DTCE_CMT1_CMI1,
DTCE_CMT2_CMI2,
DTCE_CMT3_CMI3,
DTCE_RSPI0_SPRI0=45,DTCE_RSPI0_SPTI0,
DTCE_GPT0_GTCIA0=48,DTCE_GPT0_GTCIB0,DTCE_GPT0_GTCIC0,DTCE_GPT0_GTCID0,
DTCE_GPT0_GTCIE0=53,DTCE_GPT0_GTCIF0,DTCE_GPT0_GTCIV0,DTCE_GPT0_GTCIU0,
DTCE_RSCAN_COMFRXINT=59,
DTCE_ICU_IRQ0=64,DTCE_ICU_IRQ1,DTCE_ICU_IRQ2,DTCE_ICU_IRQ3,DTCE_ICU_IRQ4,DTCE_ICU_IRQ5,DTCE_ICU_IRQ6,DTCE_ICU_IRQ7,
DTCE_GPT1_GTCIA1=98,DTCE_GPT1_GTCIB1,DTCE_GPT1_GTCIC1,DTCE_GPT1_GTCID1,
DTCE_S12AD_S12ADI=102,DTCE_S12AD_GBADI,DTCE_S12AD_GCADI,
DTCE_S12AD1_S12ADI1,DTCE_S12AD1_GBADI1,DTCE_S12AD1_GCADI1,
DTCE_CMPC0_CMPC0,
DTCE_CMPC1_CMPC1,
DTCE_CMPC2_CMPC2,
DTCE_S12AD2_S12ADI2,DTCE_S12AD2_GBADI2,DTCE_S12AD2_GCADI2,
DTCE_MTU0_TGIA0,DTCE_MTU0_TGIB0,DTCE_MTU0_TGIC0,DTCE_MTU0_TGID0,
DTCE_MTU1_TGIA1=121,DTCE_MTU1_TGIB1,
DTCE_MTU2_TGIA2=125,DTCE_MTU2_TGIB2,
DTCE_MTU3_TGIA3=129,DTCE_MTU3_TGIB3,DTCE_MTU3_TGIC3,DTCE_MTU3_TGID3,
DTCE_MTU4_TGIA4=134,DTCE_MTU4_TGIB4,DTCE_MTU4_TGIC4,DTCE_MTU4_TGID4,DTCE_MTU4_TCIV4,
DTCE_MTU5_TGIU5,DTCE_MTU5_TGIV5,DTCE_MTU5_TGIW5,
DTCE_MTU6_TGIA6,DTCE_MTU6_TGIB6,DTCE_MTU6_TGIC6,DTCE_MTU6_TGID6,
DTCE_MTU7_TGIA7=149,DTCE_MTU7_TGIB7,DTCE_MTU7_TGIC7,DTCE_MTU7_TGID7,DTCE_MTU7_TCIV7,
DTCE_MTU9_TGIA9=159,DTCE_MTU9_TGIB9,DTCE_MTU9_TGIC9,DTCE_MTU9_TGID9,
DTCE_CMPC3_CMPC3=173,
DTCE_TMR0_CMIA0,DTCE_TMR0_CMIB0,
DTCE_TMR1_CMIA1=177,DTCE_TMR1_CMIB1,
DTCE_TMR2_CMIA2=180,DTCE_TMR2_CMIB2,
DTCE_TMR3_CMIA3=183,DTCE_TMR3_CMIB3,
DTCE_TMR4_CMIA4=186,DTCE_TMR4_CMIB4,
DTCE_TMR5_CMIA5=189,DTCE_TMR5_CMIB5,
DTCE_TMR6_CMIA6=192,DTCE_TMR6_CMIB6,
DTCE_TMR7_CMIA7=195,DTCE_TMR7_CMIB7,
DTCE_GPT1_GTCIE1=203,DTCE_GPT1_GTCIF1,DTCE_GPT1_GTCIV1,DTCE_GPT1_GTCIU1,
DTCE_GPT2_GTCIA2,DTCE_GPT2_GTCIB2,DTCE_GPT2_GTCIC2,DTCE_GPT2_GTCID2,
DTCE_GPT2_GTCIE2=212,DTCE_GPT2_GTCIF2,DTCE_GPT2_GTCIV2,DTCE_GPT2_GTCIU2,
DTCE_GPT3_GTCIA3,DTCE_GPT3_GTCIB3,
DTCE_SCI1_RXI1=219,DTCE_SCI1_TXI1,
DTCE_SCI5_RXI5=223,DTCE_SCI5_TXI5,
DTCE_SCI6_RXI6=227,DTCE_SCI6_TXI6,
DTCE_SCI8_RXI8=231,DTCE_SCI8_TXI8,
DTCE_SCI9_RXI9=235,DTCE_SCI9_TXI9,
DTCE_GPT3_GTCIC3=238,DTCE_GPT3_GTCID3,
DTCE_GPT3_GTCIE3=241,DTCE_GPT3_GTCIF3,DTCE_GPT3_GTCIV3,DTCE_GPT3_GTCIU3,
DTCE_RIIC0_RXI0=247,DTCE_RIIC0_TXI0,
DTCE_SCI11_RXI11=251,DTCE_SCI11_TXI11
} enum_dtce_t;

typedef enum enum_ier {
IER_BSC_BUSERR=0x02,
IER_FCU_FRDYI=0x02,
IER_ICU_SWINT=0x03,
IER_CMT0_CMI0=0x03,
IER_CMT1_CMI1=0x03,
IER_CMT2_CMI2=0x03,
IER_CMT3_CMI3=0x03,
IER_CAC_FERRF=0x04,IER_CAC_MENDF=0x04,IER_CAC_OVFF=0x04,
IER_GPT_ETGIN=0x05,IER_GPT_ETGIP=0x05,
IER_RSPI0_SPEI0=0x05,IER_RSPI0_SPRI0=0x05,IER_RSPI0_SPTI0=0x05,IER_RSPI0_SPII0=0x05,
IER_GPT0_GTCIA0=0x06,IER_GPT0_GTCIB0=0x06,IER_GPT0_GTCIC0=0x06,IER_GPT0_GTCID0=0x06,
IER_GPT0_GDTE0=0x06,IER_GPT0_GTCIE0=0x06,IER_GPT0_GTCIF0=0x06,IER_GPT0_GTCIV0=0x06,IER_GPT0_GTCIU0=0x07,
IER_DOC_DOPCF=0x07,
IER_RSCAN_COMFRXINT=0x07,IER_RSCAN_RXFINT=0x07,IER_RSCAN_TXINT=0x07,IER_RSCAN_CHERRINT=0x07,IER_RSCAN_GLERRINT=0x07,
IER_ICU_IRQ0=0x08,IER_ICU_IRQ1=0x08,IER_ICU_IRQ2=0x08,IER_ICU_IRQ3=0x08,IER_ICU_IRQ4=0x08,IER_ICU_IRQ5=0x08,IER_ICU_IRQ6=0x08,IER_ICU_IRQ7=0x08,
IER_LVD_LVD1=0x0B,IER_LVD_LVD2=0x0B,
IER_GPT1_GTCIA1=0x0C,IER_GPT1_GTCIB1=0x0C,IER_GPT1_GTCIC1=0x0C,IER_GPT1_GTCID1=0x0C,
IER_S12AD_S12ADI=0x0C,IER_S12AD_GBADI=0x0C,IER_S12AD_GCADI=0x0D,
IER_S12AD1_S12ADI1=0x0D,IER_S12AD1_GBADI1=0x0D,IER_S12AD1_GCADI1=0x0D,
IER_CMPC0_CMPC0=0x0D,
IER_CMPC1_CMPC1=0x0D,
IER_CMPC2_CMPC2=0x0D,
IER_S12AD2_S12ADI2=0x0D,IER_S12AD2_GBADI2=0x0E,IER_S12AD2_GCADI2=0x0E,
IER_MTU0_TGIA0=0x0E,IER_MTU0_TGIB0=0x0E,IER_MTU0_TGIC0=0x0E,IER_MTU0_TGID0=0x0E,IER_MTU0_TCIV0=0x0E,IER_MTU0_TGIE0=0x0E,IER_MTU0_TGIF0=0x0F,
IER_MTU1_TGIA1=0x0F,IER_MTU1_TGIB1=0x0F,IER_MTU1_TCIV1=0x0F,IER_MTU1_TCIU1=0x0F,
IER_MTU2_TGIA2=0x0F,IER_MTU2_TGIB2=0x0F,IER_MTU2_TCIV2=0x0F,IER_MTU2_TCIU2=0x10,
IER_MTU3_TGIA3=0x10,IER_MTU3_TGIB3=0x10,IER_MTU3_TGIC3=0x10,IER_MTU3_TGID3=0x10,IER_MTU3_TCIV3=0x10,
IER_MTU4_TGIA4=0x10,IER_MTU4_TGIB4=0x10,IER_MTU4_TGIC4=0x11,IER_MTU4_TGID4=0x11,IER_MTU4_TCIV4=0x11,
IER_MTU5_TGIU5=0x11,IER_MTU5_TGIV5=0x11,IER_MTU5_TGIW5=0x11,
IER_MTU6_TGIA6=0x11,IER_MTU6_TGIB6=0x11,IER_MTU6_TGIC6=0x12,IER_MTU6_TGID6=0x12,IER_MTU6_TCIV6=0x12,
IER_MTU7_TGIA7=0x12,IER_MTU7_TGIB7=0x12,IER_MTU7_TGIC7=0x12,IER_MTU7_TGID7=0x13,IER_MTU7_TCIV7=0x13,
IER_MTU9_TGIA9=0x13,IER_MTU9_TGIB9=0x14,IER_MTU9_TGIC9=0x14,IER_MTU9_TGID9=0x14,IER_MTU9_TCIV9=0x14,IER_MTU9_TGIE9=0x14,IER_MTU9_TGIF9=0x14,
IER_POE_OEI1=0x15,IER_POE_OEI2=0x15,IER_POE_OEI3=0x15,IER_POE_OEI4=0x15,IER_POE_OEI5=0x15,
IER_CMPC3_CMPC3=0x15,
IER_TMR0_CMIA0=0x15,IER_TMR0_CMIB0=0x15,IER_TMR0_OVI0=0x16,
IER_TMR1_CMIA1=0x16,IER_TMR1_CMIB1=0x16,IER_TMR1_OVI1=0x16,
IER_TMR2_CMIA2=0x16,IER_TMR2_CMIB2=0x16,IER_TMR2_OVI2=0x16,
IER_TMR3_CMIA3=0x16,IER_TMR3_CMIB3=0x17,IER_TMR3_OVI3=0x17,
IER_TMR4_CMIA4=0x17,IER_TMR4_CMIB4=0x17,IER_TMR4_OVI4=0x17,
IER_TMR5_CMIA5=0x17,IER_TMR5_CMIB5=0x17,IER_TMR5_OVI5=0x17,
IER_TMR6_CMIA6=0x18,IER_TMR6_CMIB6=0x18,IER_TMR6_OVI6=0x18,
IER_TMR7_CMIA7=0x18,IER_TMR7_CMIB7=0x18,IER_TMR7_OVI7=0x18,
IER_GPT1_GDTE1=0x19,IER_GPT1_GTCIE1=0x19,IER_GPT1_GTCIF1=0x19,IER_GPT1_GTCIV1=0x19,IER_GPT1_GTCIU1=0x19,
IER_GPT2_GTCIA2=0x19,IER_GPT2_GTCIB2=0x1A,IER_GPT2_GTCIC2=0x1A,IER_GPT2_GTCID2=0x1A,IER_GPT2_GDTE2=0x1A,
IER_GPT2_GTCIE2=0x1A,IER_GPT2_GTCIF2=0x1A,IER_GPT2_GTCIV2=0x1A,IER_GPT2_GTCIU2=0x1A,
IER_GPT3_GTCIA3=0x1B,IER_GPT3_GTCIB3=0x1B,
IER_SCI1_ERI1=0x1B,IER_SCI1_RXI1=0x1B,IER_SCI1_TXI1=0x1B,IER_SCI1_TEI1=0x1B,
IER_SCI5_ERI5=0x1B,IER_SCI5_RXI5=0x1B,IER_SCI5_TXI5=0x1C,IER_SCI5_TEI5=0x1C,
IER_SCI6_ERI6=0x1C,IER_SCI6_RXI6=0x1C,IER_SCI6_TXI6=0x1C,IER_SCI6_TEI6=0x1C,
IER_SCI8_ERI8=0x1C,IER_SCI8_RXI8=0x1C,IER_SCI8_TXI8=0x1D,IER_SCI8_TEI8=0x1D,
IER_SCI9_ERI9=0x1D,IER_SCI9_RXI9=0x1D,IER_SCI9_TXI9=0x1D,IER_SCI9_TEI9=0x1D,
IER_GPT3_GTCIC3=0x1D,IER_GPT3_GTCID3=0x1D,IER_GPT3_GDTE3=0x1E,
IER_GPT3_GTCIE3=0x1E,IER_GPT3_GTCIF3=0x1E,IER_GPT3_GTCIV3=0x1E,IER_GPT3_GTCIU3=0x1E,
IER_RIIC0_EEI0=0x1E,IER_RIIC0_RXI0=0x1E,IER_RIIC0_TXI0=0x1F,IER_RIIC0_TEI0=0x1F,
IER_SCI11_ERI11=0x1F,IER_SCI11_RXI11=0x1F,IER_SCI11_TXI11=0x1F,IER_SCI11_TEI11=0x1F
} enum_ier_t;

typedef enum enum_ipr {
IPR_BSC_BUSERR=0,
IPR_FCU_FRDYI=2,
IPR_ICU_SWINT=3,
IPR_CMT0_CMI0=4,
IPR_CMT1_CMI1=5,
IPR_CMT2_CMI2=6,
IPR_CMT3_CMI3=7,
IPR_CAC_FERRF=32,IPR_CAC_MENDF=33,IPR_CAC_OVFF=34,
IPR_GPT_ETGIN=40,IPR_GPT_ETGIP=41,
IPR_RSPI0_SPEI0=44,IPR_RSPI0_SPRI0=44,IPR_RSPI0_SPTI0=44,IPR_RSPI0_SPII0=44,
IPR_GPT0_GTCIA0=48,IPR_GPT0_GTCIB0=49,IPR_GPT0_GTCIC0=50,IPR_GPT0_GTCID0=51,IPR_GPT0_GDTE0=52,
IPR_GPT0_GTCIE0=53,IPR_GPT0_GTCIF0=54,IPR_GPT0_GTCIV0=55,IPR_GPT0_GTCIU0=56,
IPR_DOC_DOPCF=57,
IPR_RSCAN_COMFRXINT=59,IPR_RSCAN_RXFINT=60,IPR_RSCAN_TXINT=61,IPR_RSCAN_CHERRINT=62,IPR_RSCAN_GLERRINT=63,
IPR_ICU_IRQ0=64,IPR_ICU_IRQ1=65,IPR_ICU_IRQ2=66,IPR_ICU_IRQ3=67,IPR_ICU_IRQ4=68,IPR_ICU_IRQ5=69,IPR_ICU_IRQ6=70,IPR_ICU_IRQ7=71,
IPR_LVD_LVD1=88,IPR_LVD_LVD2=89,
IPR_GPT1_GTCIA1=98,IPR_GPT1_GTCIB1=99,IPR_GPT1_GTCIC1=100,IPR_GPT1_GTCID1=101,
IPR_S12AD_S12ADI=102,IPR_S12AD_GBADI=103,IPR_S12AD_GCADI=104,
IPR_S12AD1_S12ADI1=105,IPR_S12AD1_GBADI1=106,IPR_S12AD1_GCADI1=107,
IPR_CMPC0_CMPC0=108,
IPR_CMPC1_CMPC1=109,
IPR_CMPC2_CMPC2=110,
IPR_S12AD2_S12ADI2=111,IPR_S12AD2_GBADI2=112,IPR_S12AD2_GCADI2=113,
IPR_MTU0_TGIA0=114,IPR_MTU0_TGIB0=114,IPR_MTU0_TGIC0=114,IPR_MTU0_TGID0=114,IPR_MTU0_TCIV0=118,IPR_MTU0_TGIE0=118,IPR_MTU0_TGIF0=118,
IPR_MTU1_TGIA1=121,IPR_MTU1_TGIB1=121,IPR_MTU1_TCIV1=123,IPR_MTU1_TCIU1=123,
IPR_MTU2_TGIA2=125,IPR_MTU2_TGIB2=125,IPR_MTU2_TCIV2=127,IPR_MTU2_TCIU2=127,
IPR_MTU3_TGIA3=129,IPR_MTU3_TGIB3=129,IPR_MTU3_TGIC3=129,IPR_MTU3_TGID3=129,IPR_MTU3_TCIV3=133,
IPR_MTU4_TGIA4=134,IPR_MTU4_TGIB4=134,IPR_MTU4_TGIC4=134,IPR_MTU4_TGID4=134,IPR_MTU4_TCIV4=138,
IPR_MTU5_TGIU5=139,IPR_MTU5_TGIV5=139,IPR_MTU5_TGIW5=139,
IPR_MTU6_TGIA6=142,IPR_MTU6_TGIB6=142,IPR_MTU6_TGIC6=142,IPR_MTU6_TGID6=142,IPR_MTU6_TCIV6=146,
IPR_MTU7_TGIA7=149,IPR_MTU7_TGIB7=149,IPR_MTU7_TGIC7=151,IPR_MTU7_TGID7=151,IPR_MTU7_TCIV7=153,
IPR_MTU9_TGIA9=159,IPR_MTU9_TGIB9=159,IPR_MTU9_TGIC9=159,IPR_MTU9_TGID9=159,IPR_MTU9_TCIV9=163,IPR_MTU9_TGIE9=163,IPR_MTU9_TGIF9=163,
IPR_POE_OEI1=168,IPR_POE_OEI2=168,IPR_POE_OEI3=168,IPR_POE_OEI4=168,IPR_POE_OEI5=168,
IPR_CMPC3_CMPC3=173,
IPR_TMR0_CMIA0=174,IPR_TMR0_CMIB0=174,IPR_TMR0_OVI0=174,
IPR_TMR1_CMIA1=177,IPR_TMR1_CMIB1=177,IPR_TMR1_OVI1=177,
IPR_TMR2_CMIA2=180,IPR_TMR2_CMIB2=180,IPR_TMR2_OVI2=180,
IPR_TMR3_CMIA3=183,IPR_TMR3_CMIB3=183,IPR_TMR3_OVI3=183,
IPR_TMR4_CMIA4=186,IPR_TMR4_CMIB4=186,IPR_TMR4_OVI4=186,
IPR_TMR5_CMIA5=189,IPR_TMR5_CMIB5=189,IPR_TMR5_OVI5=189,
IPR_TMR6_CMIA6=192,IPR_TMR6_CMIB6=192,IPR_TMR6_OVI6=192,
IPR_TMR7_CMIA7=195,IPR_TMR7_CMIB7=195,IPR_TMR7_OVI7=195,
IPR_GPT1_GDTE1=202,IPR_GPT1_GTCIE1=203,IPR_GPT1_GTCIF1=204,IPR_GPT1_GTCIV1=205,IPR_GPT1_GTCIU1=206,
IPR_GPT2_GTCIA2=207,IPR_GPT2_GTCIB2=208,IPR_GPT2_GTCIC2=209,IPR_GPT2_GTCID2=210,IPR_GPT2_GDTE2=211,
IPR_GPT2_GTCIE2=212,IPR_GPT2_GTCIF2=213,IPR_GPT2_GTCIV2=214,IPR_GPT2_GTCIU2=215,
IPR_GPT3_GTCIA3=216,IPR_GPT3_GTCIB3=217,
IPR_SCI1_ERI1=218,IPR_SCI1_RXI1=218,IPR_SCI1_TXI1=218,IPR_SCI1_TEI1=218,
IPR_SCI5_ERI5=222,IPR_SCI5_RXI5=222,IPR_SCI5_TXI5=222,IPR_SCI5_TEI5=222,
IPR_SCI6_ERI6=226,IPR_SCI6_RXI6=226,IPR_SCI6_TXI6=226,IPR_SCI6_TEI6=226,
IPR_SCI8_ERI8=230,IPR_SCI8_RXI8=230,IPR_SCI8_TXI8=230,IPR_SCI8_TEI8=230,
IPR_SCI9_ERI9=234,IPR_SCI9_RXI9=234,IPR_SCI9_TXI9=234,IPR_SCI9_TEI9=234,
IPR_GPT3_GTCIC3=238,IPR_GPT3_GTCID3=239,IPR_GPT3_GDTE3=240,
IPR_GPT3_GTCIE3=241,IPR_GPT3_GTCIF3=242,IPR_GPT3_GTCIV3=243,IPR_GPT3_GTCIU3=244,
IPR_RIIC0_EEI0=246,IPR_RIIC0_RXI0=247,IPR_RIIC0_TXI0=248,IPR_RIIC0_TEI0=249,
IPR_SCI11_ERI11=250,IPR_SCI11_RXI11=250,IPR_SCI11_TXI11=250,IPR_SCI11_TEI11=250,
IPR_BSC_=0,
IPR_FCU_=2,
IPR_CMT0_=4,
IPR_CMT1_=5,
IPR_CMT2_=6,
IPR_CMT3_=7,
IPR_RSPI0_=44,
IPR_DOC_=57,
IPR_CMPC0_=108,
IPR_CMPC1_=109,
IPR_CMPC2_=110,
IPR_MTU1_TGI=121,
IPR_MTU1_TCI=123,
IPR_MTU2_TGI=125,
IPR_MTU2_TCI=127,
IPR_MTU3_TGI=129,
IPR_MTU4_TGI=134,
IPR_MTU5_=139,
IPR_MTU5_TGI=139,
IPR_MTU6_TGI=142,
IPR_POE_=168,
IPR_POE_OEI=168,
IPR_CMPC3_=173,
IPR_TMR0_=174,
IPR_TMR1_=177,
IPR_TMR2_=180,
IPR_TMR3_=183,
IPR_TMR4_=186,
IPR_TMR5_=189,
IPR_TMR6_=192,
IPR_TMR7_=195,
IPR_SCI1_=218,
IPR_SCI5_=222,
IPR_SCI6_=226
} enum_ipr_t;

#pragma bit_order left
#pragma unpack

typedef struct st_bsc {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char STSCLR:1;
		} BIT;
	} BERCLR;
	char           wk0[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char TOEN:1;
			unsigned char IGAEN:1;
		} BIT;
	} BEREN;
	char           wk1[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char MST:3;
			unsigned char :2;
			unsigned char TO:1;
			unsigned char IA:1;
		} BIT;
	} BERSR1;
	char           wk2[1];
	union {
		unsigned short WORD;
		struct {
			unsigned short ADDR:13;
			unsigned short :3;
		} BIT;
	} BERSR2;
	char           wk3[4];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short BPFB:2;
			unsigned short BPHB:2;
			unsigned short BPGB:2;
			unsigned short BPIB:2;
			unsigned short BPRO:2;
			unsigned short BPRA:2;
		} BIT;
	} BUSPRI;
} st_bsc_t;

typedef struct st_cac {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CFME:1;
		} BIT;
	} CACR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char EDGES:2;
			unsigned char TCSS:2;
			unsigned char FMCS:3;
			unsigned char CACREFE:1;
		} BIT;
	} CACR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DFS:2;
			unsigned char RCDS:2;
			unsigned char RSCS:3;
			unsigned char RPS:1;
		} BIT;
	} CACR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char OVFFCL:1;
			unsigned char MENDFCL:1;
			unsigned char FERRFCL:1;
			unsigned char :1;
			unsigned char OVFIE:1;
			unsigned char MENDIE:1;
			unsigned char FERRIE:1;
		} BIT;
	} CAICR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char OVFF:1;
			unsigned char MENDF:1;
			unsigned char FERRF:1;
		} BIT;
	} CASTR;
	char           wk0[1];
	unsigned short CAULVR;
	unsigned short CALLVR;
	unsigned short CACNTBR;
} st_cac_t;

typedef struct st_rscan {
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short TSSS:1;
			unsigned short TSP:4;
			unsigned short :3;
			unsigned short DCS:1;
			unsigned short MME:1;
			unsigned short DRE:1;
			unsigned short DCE:1;
			unsigned short TPRI:1;
		} BIT;
	} GCFGL;
	union {
		unsigned short WORD;
		struct {
			unsigned short ITRCP:16;
		} BIT;
	} GCFGH;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short THLEIE:1;
			unsigned short MEIE:1;
			unsigned short DEIE:1;
			unsigned short :5;
			unsigned short GSLPR:1;
			unsigned short GMDC:2;
		} BIT;
	} GCTRL;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short TSRST:1;
		} BIT;
	} GCTRH;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short GRAMINIT:1;
			unsigned short GSLPSTS:1;
			unsigned short GHLTSTS:1;
			unsigned short GRSTSTS:1;
		} BIT;
	} GSTS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char THLES:1;
			unsigned char MES:1;
			unsigned char DEF:1;
		} BIT;
	} GERFLL;
	char           wk0[1];
	union {
		unsigned short WORD;
		struct {
			unsigned short TS:16;
		} BIT;
	} GTSC;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short RNC0:5;
		} BIT;
	} GAFLCFG;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short NRXMB:5;
		} BIT;
	} RMNB;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMNS:16;
		} BIT;
	} RMND0;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short RFIGCV:3;
			unsigned short RFIM:1;
			unsigned short :1;
			unsigned short RFDC:3;
			unsigned short :6;
			unsigned short RFIE:1;
			unsigned short RFE:1;
		} BIT;
	} RFCC0;
	union {
		unsigned short WORD;
		struct {
			unsigned short RFIGCV:3;
			unsigned short RFIM:1;
			unsigned short :1;
			unsigned short RFDC:3;
			unsigned short :6;
			unsigned short RFIE:1;
			unsigned short RFE:1;
		} BIT;
	} RFCC1;
	char           wk2[4];
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short RFMC:6;
			unsigned short :4;
			unsigned short RFIF:1;
			unsigned short RFMLT:1;
			unsigned short RFFLL:1;
			unsigned short RFEMP:1;
		} BIT;
	} RFSTS0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short RFMC:6;
			unsigned short :4;
			unsigned short RFIF:1;
			unsigned short RFMLT:1;
			unsigned short RFFLL:1;
			unsigned short RFEMP:1;
		} BIT;
	} RFSTS1;
	char           wk3[4];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short RFPC:8;
		} BIT;
	} RFPCTR0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short RFPC:8;
		} BIT;
	} RFPCTR1;
	char           wk4[20];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char RF1MLT:1;
			unsigned char RF0MLT:1;
		} BIT;
	} RFMSTS;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char RF1IF:1;
			unsigned char RF0IF:1;
		} BIT;
	} RFISTS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CF0IF:1;
		} BIT;
	} CFISTS;
	char           wk6[36];
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short THIF0:1;
			unsigned short CFTIF0:1;
			unsigned short TAIF0:1;
			unsigned short TSIF0:1;
		} BIT;
	} GTINTSTS;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short RPAGE:1;
		} BIT;
	} GRWCR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short RTMPS:3;
			unsigned short :8;
		} BIT;
	} GTSTCFG;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char RTME:1;
			unsigned char :2;
		} BIT;
	} GTSTCTRL;
	char           wk7[5];
	union {
		unsigned short WORD;
		struct {
			unsigned short LOCK:16;
		} BIT;
	} GLOCKK;
	char           wk8[10];
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL0;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH0;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS0;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR0;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF00;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF20;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF30;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL1;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH1;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS1;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR1;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL2;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF01;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH2;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML2;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF21;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH2;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF31;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL2;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL2;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH2;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH2;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL3;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS2;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH3;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR2;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML3;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF02;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH3;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF12;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL3;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF22;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH3;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF32;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL4;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL3;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH4;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH3;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML4;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS3;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH4;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR3;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL4;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF03;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH4;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF13;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL5;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF23;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH5;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF33;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML5;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL4;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH5;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH4;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL5;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS4;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH5;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR4;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL6;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF04;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH6;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF14;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML6;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF24;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH6;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF34;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL6;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL5;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH6;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH5;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL7;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS5;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH7;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR5;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML7;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF05;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH7;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF15;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL7;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF25;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH7;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF35;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL8;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL6;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH8;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH6;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML8;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS6;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH8;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR6;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL8;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF06;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH8;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF16;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL9;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF26;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH9;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF36;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML9;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL7;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH9;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH7;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL9;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS7;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH9;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR7;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL10;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF07;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH10;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF17;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML10;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF27;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH10;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF37;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL10;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL8;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH10;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH8;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL11;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS8;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH11;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR8;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML11;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF08;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH11;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF18;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL11;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF28;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH11;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF38;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL12;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL9;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH12;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH9;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML12;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS9;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH12;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR9;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL12;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF09;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH12;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF19;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL13;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF29;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH13;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF39;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML13;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH13;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL13;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH13;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL14;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF010;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH14;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF110;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML14;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF210;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH14;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF310;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL14;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMID:16;
			} BIT;
		} RMIDL11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH14;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMIDE:1;
				unsigned short RMRTR:1;
				unsigned short :1;
				unsigned short RMID:13;
			} BIT;
		} RMIDH11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLID:16;
			} BIT;
		} GAFLIDL15;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMTS:16;
			} BIT;
		} RMTS11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDE:1;
				unsigned short GAFLRTR:1;
				unsigned short GAFLLB:1;
				unsigned short GAFLID:13;
			} BIT;
		} GAFLIDH15;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDLC:4;
				unsigned short RMPTR:12;
			} BIT;
		} RMPTR11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDM:16;
			} BIT;
		} GAFLML15;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB1:8;
				unsigned short RMDB0:8;
			} BIT;
		} RMDF011;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLIDEM:1;
				unsigned short GAFLRTRM:1;
				unsigned short :1;
				unsigned short GAFLIDM:13;
			} BIT;
		} GAFLMH15;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB3:8;
				unsigned short RMDB2:8;
			} BIT;
		} RMDF111;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLRMV:1;
				unsigned short GAFLRMDP:7;
				unsigned short :3;
				unsigned short GAFLFDP4:1;
				unsigned short :2;
				unsigned short GAFLFDP1:1;
				unsigned short GAFLFDP0:1;
			} BIT;
		} GAFLPL15;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB5:8;
				unsigned short RMDB4:8;
			} BIT;
		} RMDF211;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short GAFLDLC:4;
				unsigned short GAFLPTR:12;
			} BIT;
		} GAFLPH15;
		union {
			unsigned short WORD;
			struct {
				unsigned short RMDB7:8;
				unsigned short RMDB6:8;
			} BIT;
		} RMDF311;
	};
	union {
		unsigned short WORD;
		struct {
			unsigned short RMID:16;
		} BIT;
	} RMIDL12;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMIDE:1;
			unsigned short RMRTR:1;
			unsigned short :1;
			unsigned short RMID:13;
		} BIT;
	} RMIDH12;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMTS:16;
		} BIT;
	} RMTS12;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDLC:4;
			unsigned short RMPTR:12;
		} BIT;
	} RMPTR12;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB1:8;
			unsigned short RMDB0:8;
		} BIT;
	} RMDF012;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB3:8;
			unsigned short RMDB2:8;
		} BIT;
	} RMDF112;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB5:8;
			unsigned short RMDB4:8;
		} BIT;
	} RMDF212;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB7:8;
			unsigned short RMDB6:8;
		} BIT;
	} RMDF312;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMID:16;
		} BIT;
	} RMIDL13;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMIDE:1;
			unsigned short RMRTR:1;
			unsigned short :1;
			unsigned short RMID:13;
		} BIT;
	} RMIDH13;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMTS:16;
		} BIT;
	} RMTS13;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDLC:4;
			unsigned short RMPTR:12;
		} BIT;
	} RMPTR13;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB1:8;
			unsigned short RMDB0:8;
		} BIT;
	} RMDF013;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB3:8;
			unsigned short RMDB2:8;
		} BIT;
	} RMDF113;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB5:8;
			unsigned short RMDB4:8;
		} BIT;
	} RMDF213;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB7:8;
			unsigned short RMDB6:8;
		} BIT;
	} RMDF313;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMID:16;
		} BIT;
	} RMIDL14;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMIDE:1;
			unsigned short RMRTR:1;
			unsigned short :1;
			unsigned short RMID:13;
		} BIT;
	} RMIDH14;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMTS:16;
		} BIT;
	} RMTS14;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDLC:4;
			unsigned short RMPTR:12;
		} BIT;
	} RMPTR14;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB1:8;
			unsigned short RMDB0:8;
		} BIT;
	} RMDF014;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB3:8;
			unsigned short RMDB2:8;
		} BIT;
	} RMDF114;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB5:8;
			unsigned short RMDB4:8;
		} BIT;
	} RMDF214;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB7:8;
			unsigned short RMDB6:8;
		} BIT;
	} RMDF314;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMID:16;
		} BIT;
	} RMIDL15;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMIDE:1;
			unsigned short RMRTR:1;
			unsigned short :1;
			unsigned short RMID:13;
		} BIT;
	} RMIDH15;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMTS:16;
		} BIT;
	} RMTS15;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDLC:4;
			unsigned short RMPTR:12;
		} BIT;
	} RMPTR15;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB1:8;
			unsigned short RMDB0:8;
		} BIT;
	} RMDF015;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB3:8;
			unsigned short RMDB2:8;
		} BIT;
	} RMDF115;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB5:8;
			unsigned short RMDB4:8;
		} BIT;
	} RMDF215;
	union {
		unsigned short WORD;
		struct {
			unsigned short RMDB7:8;
			unsigned short RMDB6:8;
		} BIT;
	} RMDF315;
	char           wk9[224];
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC0;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC1;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC2;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC3;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC4;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC5;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC6;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC7;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC8;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC9;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC10;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC11;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC12;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC13;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC14;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC15;
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFID:16;
			} BIT;
		} RFIDL0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC16;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFIDE:1;
				unsigned short RFRTR:1;
				unsigned short :1;
				unsigned short RFID:13;
			} BIT;
		} RFIDH0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC17;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFTS:16;
			} BIT;
		} RFTS0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC18;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDLC:4;
				unsigned short RFPTR:12;
			} BIT;
		} RFPTR0;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC19;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDB1:8;
				unsigned short RFDB0:8;
			} BIT;
		} RFDF00;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC20;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDB3:8;
				unsigned short RFDB2:8;
			} BIT;
		} RFDF10;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC21;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDB5:8;
				unsigned short RFDB4:8;
			} BIT;
		} RFDF20;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC22;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDB7:8;
				unsigned short RFDB6:8;
			} BIT;
		} RFDF30;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC23;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFID:16;
			} BIT;
		} RFIDL1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC24;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFIDE:1;
				unsigned short RFRTR:1;
				unsigned short :1;
				unsigned short RFID:13;
			} BIT;
		} RFIDH1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC25;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFTS:16;
			} BIT;
		} RFTS1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC26;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDLC:4;
				unsigned short RFPTR:12;
			} BIT;
		} RFPTR1;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC27;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDB1:8;
				unsigned short RFDB0:8;
			} BIT;
		} RFDF01;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC28;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDB3:8;
				unsigned short RFDB2:8;
			} BIT;
		} RFDF11;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC29;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDB5:8;
				unsigned short RFDB4:8;
			} BIT;
		} RFDF21;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC30;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned short RFDB7:8;
				unsigned short RFDB6:8;
			} BIT;
		} RFDF31;
		union {
			unsigned short WORD;
			struct {
				unsigned short RDTA:16;
			} BIT;
		} RPGACC31;
	};
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC32;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC33;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC34;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC35;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC36;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC37;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC38;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC39;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC40;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC41;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC42;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC43;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC44;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC45;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC46;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC47;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC48;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC49;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC50;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC51;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC52;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC53;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC54;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC55;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC56;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC57;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC58;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC59;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC60;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC61;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC62;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC63;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC64;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC65;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC66;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC67;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC68;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC69;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC70;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC71;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC72;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC73;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC74;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC75;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC76;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC77;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC78;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC79;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC80;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC81;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC82;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC83;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC84;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC85;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC86;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC87;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC88;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC89;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC90;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC91;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC92;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC93;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC94;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC95;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC96;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC97;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC98;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC99;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC100;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC101;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC102;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC103;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC104;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC105;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC106;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC107;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC108;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC109;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC110;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC111;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC112;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC113;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC114;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC115;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC116;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC117;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC118;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC119;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC120;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC121;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC122;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC123;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC124;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC125;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC126;
	union {
		unsigned short WORD;
		struct {
			unsigned short RDTA:16;
		} BIT;
	} RPGACC127;
} st_rscan_t;

typedef struct st_rscan0 {
	union {
		unsigned short WORD;
		struct {
			unsigned short :6;
			unsigned short BRP:10;
		} BIT;
	} CFGL;
	union {
		unsigned short WORD;
		struct {
			unsigned short :6;
			unsigned short SJW:2;
			unsigned short :1;
			unsigned short TSEG2:3;
			unsigned short TSEG1:4;
		} BIT;
	} CFGH;
	union {
		unsigned short WORD;
		struct {
			unsigned short ALIE:1;
			unsigned short BLIE:1;
			unsigned short OLIE:1;
			unsigned short BORIE:1;
			unsigned short BOEIE:1;
			unsigned short EPIE:1;
			unsigned short EWIE:1;
			unsigned short BEIE:1;
			unsigned short :4;
			unsigned short RTBO:1;
			unsigned short CSLPR:1;
			unsigned short CHMDC:2;
		} BIT;
	} CTRL;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short CTMS:2;
			unsigned short CTME:1;
			unsigned short ERRD:1;
			unsigned short BOM:2;
			unsigned short :4;
			unsigned short TAIE:1;
		} BIT;
	} CTRH;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short COMSTS:1;
			unsigned short RECSTS:1;
			unsigned short TRMSTS:1;
			unsigned short BOSTS:1;
			unsigned short EPSTS:1;
			unsigned short CSLPSTS:1;
			unsigned short CHLTSTS:1;
			unsigned short CRSTSTS:1;
		} BIT;
	} STSL;
	union {
		unsigned short WORD;
		struct {
			unsigned short TEC:8;
			unsigned short REC:8;
		} BIT;
	} STSH;
	union {
		unsigned short WORD;
		struct {
			unsigned short :1;
			unsigned short ADERR:1;
			unsigned short B0ERR:1;
			unsigned short B1ERR:1;
			unsigned short CERR:1;
			unsigned short AERR:1;
			unsigned short FERR:1;
			unsigned short SERR:1;
			unsigned short ALF:1;
			unsigned short BLF:1;
			unsigned short OVLF:1;
			unsigned short BORF:1;
			unsigned short BOEF:1;
			unsigned short EPF:1;
			unsigned short EWF:1;
			unsigned short BEF:1;
		} BIT;
	} ERFLL;
	union {
		unsigned short WORD;
		struct {
			unsigned short :1;
			unsigned short CRCREG:15;
		} BIT;
	} ERFLH;
	char           wk0[64];
	union {
		unsigned short WORD;
		struct {
			unsigned short CFIGCV:3;
			unsigned short CFIM:1;
			unsigned short :1;
			unsigned short CFDC:3;
			unsigned short :5;
			unsigned short CFTXIE:1;
			unsigned short CFRXIE:1;
			unsigned short CFE:1;
		} BIT;
	} CFCCL0;
	union {
		unsigned short WORD;
		struct {
			unsigned short CFITT:8;
			unsigned short :2;
			unsigned short CFTML:2;
			unsigned short CFITR:1;
			unsigned short CFITSS:1;
			unsigned short CFM:2;
		} BIT;
	} CFCCH0;
	char           wk1[4];
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short CFMC:6;
			unsigned short :3;
			unsigned short CFTXIF:1;
			unsigned short CFRXIF:1;
			unsigned short CFMLT:1;
			unsigned short CFFLL:1;
			unsigned short CFEMP:1;
		} BIT;
	} CFSTS0;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short CFPC:8;
		} BIT;
	} CFPCTR0;
	char           wk3[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CF0MLT:1;
		} BIT;
	} CFMSTS;
	char           wk4[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TMOM:1;
			unsigned char TMTAR:1;
			unsigned char TMTR:1;
		} BIT;
	} TMC0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TMOM:1;
			unsigned char TMTAR:1;
			unsigned char TMTR:1;
		} BIT;
	} TMC1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TMOM:1;
			unsigned char TMTAR:1;
			unsigned char TMTR:1;
		} BIT;
	} TMC2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TMOM:1;
			unsigned char TMTAR:1;
			unsigned char TMTR:1;
		} BIT;
	} TMC3;
	char           wk5[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char TMTARM:1;
			unsigned char TMTRM:1;
			unsigned char TMTRF:2;
			unsigned char TMTSTS:1;
		} BIT;
	} TMSTS0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char TMTARM:1;
			unsigned char TMTRM:1;
			unsigned char TMTRF:2;
			unsigned char TMTSTS:1;
		} BIT;
	} TMSTS1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char TMTARM:1;
			unsigned char TMTRM:1;
			unsigned char TMTRF:2;
			unsigned char TMTSTS:1;
		} BIT;
	} TMSTS2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char TMTARM:1;
			unsigned char TMTRM:1;
			unsigned char TMTRF:2;
			unsigned char TMTSTS:1;
		} BIT;
	} TMSTS3;
	char           wk6[4];
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short TMTRSTS3:1;
			unsigned short TMTRSTS2:1;
			unsigned short TMTRSTS1:1;
			unsigned short TMTRSTS0:1;
		} BIT;
	} TMTRSTS;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short TMTCSTS3:1;
			unsigned short TMTCSTS2:1;
			unsigned short TMTCSTS1:1;
			unsigned short TMTCSTS0:1;
		} BIT;
	} TMTCSTS;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short TMTASTS3:1;
			unsigned short TMTASTS2:1;
			unsigned short TMTASTS1:1;
			unsigned short TMTASTS0:1;
		} BIT;
	} TMTASTS;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short TMIE3:1;
			unsigned short TMIE2:1;
			unsigned short TMIE1:1;
			unsigned short TMIE0:1;
		} BIT;
	} TMIEC;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short THLDTE:1;
			unsigned short THLIM:1;
			unsigned short THLIE:1;
			unsigned short :7;
			unsigned short THLE:1;
		} BIT;
	} THLCC0;
	char           wk7[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short THLMC:4;
			unsigned short :4;
			unsigned short THLIF:1;
			unsigned short THLELT:1;
			unsigned short THLFLL:1;
			unsigned short THLEMP:1;
		} BIT;
	} THLSTS0;
	char           wk8[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short THLPC:8;
		} BIT;
	} THLPCTR0;
	char           wk9[602];
	union {
		unsigned short WORD;
		struct {
			unsigned short CFID:16;
		} BIT;
	} CFIDL0;
	union {
		unsigned short WORD;
		struct {
			unsigned short CFIDE:1;
			unsigned short CFRTR:1;
			unsigned short THLEN:1;
			unsigned short CFID:13;
		} BIT;
	} CFIDH0;
	union {
		unsigned short WORD;
		struct {
			unsigned short CFTS:16;
		} BIT;
	} CFTS0;
	union {
		unsigned short WORD;
		struct {
			unsigned short CFDLC:4;
			unsigned short CFPTR:12;
		} BIT;
	} CFPTR0;
	union {
		unsigned short WORD;
		struct {
			unsigned short CFDB1:8;
			unsigned short CFDB0:8;
		} BIT;
	} CFDF00;
	union {
		unsigned short WORD;
		struct {
			unsigned short CFDB3:8;
			unsigned short CFDB2:8;
		} BIT;
	} CFDF10;
	union {
		unsigned short WORD;
		struct {
			unsigned short CFDB5:8;
			unsigned short CFDB4:8;
		} BIT;
	} CFDF20;
	union {
		unsigned short WORD;
		struct {
			unsigned short CFDB7:8;
			unsigned short CFDB6:8;
		} BIT;
	} CFDF30;
	char           wk10[16];
	union {
		unsigned short WORD;
		struct {
			unsigned short TMID:16;
		} BIT;
	} TMIDL0;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMIDE:1;
			unsigned short TMRTR:1;
			unsigned short THLEN:1;
			unsigned short TMID:13;
		} BIT;
	} TMIDH0;
	char           wk11[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDLC:4;
			unsigned short :4;
			unsigned short TMPTR:8;
		} BIT;
	} TMPTR0;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB1:8;
			unsigned short TMDB0:8;
		} BIT;
	} TMDF00;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB3:8;
			unsigned short TMDB2:8;
		} BIT;
	} TMDF10;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB5:8;
			unsigned short TMDB4:8;
		} BIT;
	} TMDF20;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB7:8;
			unsigned short TMDB6:8;
		} BIT;
	} TMDF30;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMID:16;
		} BIT;
	} TMIDL1;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMIDE:1;
			unsigned short TMRTR:1;
			unsigned short THLEN:1;
			unsigned short TMID:13;
		} BIT;
	} TMIDH1;
	char           wk12[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDLC:4;
			unsigned short :4;
			unsigned short TMPTR:8;
		} BIT;
	} TMPTR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB1:8;
			unsigned short TMDB0:8;
		} BIT;
	} TMDF01;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB3:8;
			unsigned short TMDB2:8;
		} BIT;
	} TMDF11;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB5:8;
			unsigned short TMDB4:8;
		} BIT;
	} TMDF21;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB7:8;
			unsigned short TMDB6:8;
		} BIT;
	} TMDF31;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMID:16;
		} BIT;
	} TMIDL2;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMIDE:1;
			unsigned short TMRTR:1;
			unsigned short THLEN:1;
			unsigned short TMID:13;
		} BIT;
	} TMIDH2;
	char           wk13[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDLC:4;
			unsigned short :4;
			unsigned short TMPTR:8;
		} BIT;
	} TMPTR2;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB1:8;
			unsigned short TMDB0:8;
		} BIT;
	} TMDF02;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB3:8;
			unsigned short TMDB2:8;
		} BIT;
	} TMDF12;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB5:8;
			unsigned short TMDB4:8;
		} BIT;
	} TMDF22;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB7:8;
			unsigned short TMDB6:8;
		} BIT;
	} TMDF32;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMID:16;
		} BIT;
	} TMIDL3;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMIDE:1;
			unsigned short TMRTR:1;
			unsigned short THLEN:1;
			unsigned short TMID:13;
		} BIT;
	} TMIDH3;
	char           wk14[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDLC:4;
			unsigned short :4;
			unsigned short TMPTR:8;
		} BIT;
	} TMPTR3;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB1:8;
			unsigned short TMDB0:8;
		} BIT;
	} TMDF03;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB3:8;
			unsigned short TMDB2:8;
		} BIT;
	} TMDF13;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB5:8;
			unsigned short TMDB4:8;
		} BIT;
	} TMDF23;
	union {
		unsigned short WORD;
		struct {
			unsigned short TMDB7:8;
			unsigned short TMDB6:8;
		} BIT;
	} TMDF33;
	char           wk15[64];
	union {
		unsigned short WORD;
		struct {
			unsigned short TID:8;
			unsigned short :3;
			unsigned short BN:2;
			unsigned short :1;
			unsigned short BT:2;
		} BIT;
	} THLACC0;
} st_rscan0_t;

typedef struct st_cmpc {
	union {
		unsigned char BYTE;
		struct {
			unsigned char HCMPON:1;
			unsigned char CDFS:2;
			unsigned char CEG:2;
			unsigned char :1;
			unsigned char COE:1;
			unsigned char CINV:1;
		} BIT;
	} CMPCTL;
	char           wk0[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char CMPSEL:4;
		} BIT;
	} CMPSEL0;
	char           wk1[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char CVRS:2;
		} BIT;
	} CMPSEL1;
	char           wk2[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CMPMON0:1;
		} BIT;
	} CMPMON;
	char           wk3[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CPOE:1;
		} BIT;
	} CMPIOC;
} st_cmpc_t;

typedef struct st_cmt {
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short STR1:1;
			unsigned short STR0:1;
		} BIT;
	} CMSTR0;
	char           wk0[14];
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short STR3:1;
			unsigned short STR2:1;
		} BIT;
	} CMSTR1;
} st_cmt_t;

typedef struct st_cmt0 {
	union {
		unsigned short WORD;
		struct {
			unsigned short :9;
			unsigned short CMIE:1;
			unsigned short :4;
			unsigned short CKS:2;
		} BIT;
	} CMCR;
	unsigned short CMCNT;
	unsigned short CMCOR;
} st_cmt0_t;

typedef struct st_crc {
	union {
		unsigned char BYTE;
		struct {
			unsigned char DORCLR:1;
			unsigned char :4;
			unsigned char LMS:1;
			unsigned char GPS:2;
		} BIT;
	} CRCCR;
	unsigned char  CRCDIR;
	unsigned short CRCDOR;
} st_crc_t;

typedef struct st_da {
	unsigned short DADR0;
	unsigned short DADR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DAOE1:1;
			unsigned char DAOE0:1;
			unsigned char :6;
		} BIT;
	} DACR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DPSEL:1;
			unsigned char :7;
		} BIT;
	} DADPR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DAADST:1;
			unsigned char :7;
		} BIT;
	} DAADSCR;
} st_da_t;

typedef struct st_doc {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char DOPCFCL:1;
			unsigned char DOPCF:1;
			unsigned char DOPCIE:1;
			unsigned char :1;
			unsigned char DCSEL:1;
			unsigned char OMS:2;
		} BIT;
	} DOCR;
	char           wk0[1];
	unsigned short DODIR;
	unsigned short DODSR;
} st_doc_t;

typedef struct st_dtc {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char RRS:1;
			unsigned char :4;
		} BIT;
	} DTCCR;
	char           wk0[3];
	void          *DTCVBR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char SHORT:1;
		} BIT;
	} DTCADMOD;
	char           wk1[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DTCST:1;
		} BIT;
	} DTCST;
	char           wk2[1];
	union {
		unsigned short WORD;
		struct {
			unsigned short ACT:1;
			unsigned short :7;
			unsigned short VECN:8;
		} BIT;
	} DTCSTS;
} st_dtc_t;

typedef struct st_flash {
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ROMCEN:1;
		} BIT;
	} ROMCE;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ROMCIV:1;
		} BIT;
	} ROMCIV;
	char           wk1[7843978];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DFLEN:1;
		} BIT;
	} DFLCTL;
	char           wk2[111];
	union {
		unsigned char BYTE;
		struct {
			unsigned char FMS2:1;
			unsigned char LVPE:1;
			unsigned char :1;
			unsigned char FMS1:1;
			unsigned char RPDIS:1;
			unsigned char :1;
			unsigned char FMS0:1;
			unsigned char :1;
		} BIT;
	} FPMCR;
	char           wk3[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char EXS:1;
		} BIT;
	} FASR;
	char           wk4[3];
	unsigned short FSARL;
	char           wk5[6];
	unsigned short FSARH;
	char           wk6[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char OPST:1;
			unsigned char STOP:1;
			unsigned char :2;
			unsigned char CMD:4;
		} BIT;
	} FCR;
	char           wk7[3];
	unsigned short FEARL;
	char           wk8[6];
	unsigned short FEARH;
	char           wk9[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char FRESET:1;
		} BIT;
	} FRESETR;
	char           wk10[7];
	union {
		unsigned char BYTE;
		struct {
			unsigned char EXRDY:1;
			unsigned char FRDY:1;
			unsigned char :6;
		} BIT;
	} FSTATR1;
	char           wk11[3];
	unsigned short FWB0;
	char           wk12[6];
	unsigned short FWB1;
	char           wk13[6];
	unsigned short FWB2;
	char           wk14[2];
	unsigned short FWB3;
	char           wk15[58];
	unsigned char  FPR;
	char           wk16[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char PERR:1;
		} BIT;
	} FPSR;
	char           wk17[59];
	union {
		unsigned short WORD;
		struct {
			unsigned short :7;
			unsigned short SASMF:1;
			unsigned short :8;
		} BIT;
	} FSCMR;
	char           wk18[6];
	unsigned short FAWSMR;
	char           wk19[6];
	unsigned short FAWEMR;
	char           wk20[6];
	union {
		unsigned char BYTE;
		struct {
			unsigned char SAS:2;
			unsigned char :1;
			unsigned char PCKA:5;
		} BIT;
	} FISR;
	char           wk21[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char OPST:1;
			unsigned char :4;
			unsigned char CMD:3;
		} BIT;
	} FEXCR;
	char           wk22[3];
	unsigned short FEAML;
	char           wk23[6];
	unsigned short FEAMH;
	char           wk24[6];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char EILGLERR:1;
			unsigned char ILGLERR:1;
			unsigned char BCERR:1;
			unsigned char :1;
			unsigned char PRGERR:1;
			unsigned char ERERR:1;
		} BIT;
	} FSTATR0;
	char           wk25[15809];
	union {
		unsigned short WORD;
		struct {
			unsigned short FEKEY:8;
			unsigned short FENTRYD:1;
			unsigned short :6;
			unsigned short FENTRY0:1;
		} BIT;
	} FENTRYR;
} st_flash_t;

typedef struct st_flashconst {
	unsigned long  UIDR0;
	unsigned long  UIDR1;
	unsigned long  UIDR2;
	unsigned long  UIDR3;
} st_flashconst_t;

typedef struct st_gpt {
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char :4;
			unsigned char CST3:1;
			unsigned char CST2:1;
			unsigned char CST1:1;
			unsigned char CST0:1;
		} BIT;
	} GTSTR;
	union {
		unsigned short WORD;
		struct {
			unsigned short NFCS3:2;
			unsigned short NFCS2:2;
			unsigned short NFCS1:2;
			unsigned short NFCS0:2;
			unsigned short NFB3EN:1;
			unsigned short NFA3EN:1;
			unsigned short NFB2EN:1;
			unsigned short NFA2EN:1;
			unsigned short NFB1EN:1;
			unsigned short NFA1EN:1;
			unsigned short NFB0EN:1;
			unsigned short NFA0EN:1;
		} BIT;
	} NFCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char CPHW3:2;
			unsigned char CPHW2:2;
			unsigned char CPHW1:2;
			unsigned char CPHW0:2;
			unsigned char CSHW3:2;
			unsigned char CSHW2:2;
			unsigned char CSHW1:2;
			unsigned char CSHW0:2;
		} BIT;
	} GTHSCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :4;
			unsigned char CCSW3:1;
			unsigned char CCSW2:1;
			unsigned char CCSW1:1;
			unsigned char CCSW0:1;
			unsigned char CCHW3:2;
			unsigned char CCHW2:2;
			unsigned char CCHW1:2;
			unsigned char CCHW0:2;
		} BIT;
	} GTHCCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char CSHSL3:4;
			unsigned char CSHSL2:4;
			unsigned char CSHSL1:4;
			unsigned char CSHSL0:4;
		} BIT;
	} GTHSSR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char CSHPL3:4;
			unsigned char CSHPL2:4;
			unsigned char CSHPL1:4;
			unsigned char CSHPL0:4;
		} BIT;
	} GTHPSR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char :4;
			unsigned char WP3:1;
			unsigned char WP2:1;
			unsigned char WP1:1;
			unsigned char WP0:1;
		} BIT;
	} GTWP;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :2;
			unsigned char SYNC3:2;
			unsigned char :2;
			unsigned char SYNC2:2;
			unsigned char :2;
			unsigned char SYNC1:2;
			unsigned char :2;
			unsigned char SYNC0:2;
		} BIT;
	} GTSYNC;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char GTETRGEN:1;
			unsigned char GTENFCS:2;
#ifdef IODEFINE_H_HISTORY
			unsigned char :8;
			unsigned char :3;
#endif
			unsigned char :5;
			unsigned char :6;
			unsigned char ETINEN:1;
			unsigned char ETIPEN:1;
		} BIT;
	} GTETINT;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char BD33:1;
			unsigned char BD32:1;
			unsigned char BD31:1;
			unsigned char BD30:1;
			unsigned char BD23:1;
			unsigned char BD22:1;
			unsigned char BD21:1;
			unsigned char BD20:1;
			unsigned char BD13:1;
			unsigned char BD12:1;
			unsigned char BD11:1;
			unsigned char BD10:1;
			unsigned char BD03:1;
			unsigned char BD02:1;
			unsigned char BD01:1;
			unsigned char BD00:1;
		} BIT;
	} GTBDR;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char :4;
			unsigned char SWP3:1;
			unsigned char SWP2:1;
			unsigned char SWP1:1;
			unsigned char SWP0:1;
		} BIT;
	} GTSWP;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char :4;
			unsigned char CWP3:1;
			unsigned char CWP2:1;
			unsigned char CWP1:1;
			unsigned char CWP0:1;
		} BIT;
	} GTCWP;
	char           wk3[2];
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char :7;
			unsigned char CMNWP:1;
		} BIT;
	} GTCMNWP;
	char           wk4[2];
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char :6;
			unsigned char LWA23:1;
			unsigned char LWA01:1;
		} BIT;
	} GTMDR;
	char           wk5[2];
	union {
		unsigned long LONG;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char NFCSECD:2;
			unsigned char NFCSECC:2;
			unsigned char NFCSECB:2;
			unsigned char NFCSECA:2;
			unsigned char :4;
			unsigned char INVECD:1;
			unsigned char INVECC:1;
			unsigned char INVECB:1;
			unsigned char INVECA:1;
			unsigned char :4;
			unsigned char NFENECD:1;
			unsigned char NFENECC:1;
			unsigned char NFENECB:1;
			unsigned char NFENECA:1;
		} BIT;
	} GTECNFCR;
	union {
		unsigned long LONG;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned char :7;
			unsigned char ADSMEN1:1;
			unsigned char :4;
			unsigned char ADSMS1:4;
			unsigned char :7;
			unsigned char ADSMEN0:1;
			unsigned char :4;
			unsigned char ADSMS0:4;
		} BIT;
	} GTADSMR;
} st_gpt_t;

typedef struct st_gpt0 {
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char OBHLD:1;
			unsigned char OBDFLT:1;
			unsigned char GTIOB:6;
			unsigned char OAHLD:1;
			unsigned char OADFLT:1;
			unsigned char GTIOA:6;
		} BIT;
	} GTIOR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char ADTRBDEN:1;
			unsigned char ADTRBUEN:1;
			unsigned char ADTRADEN:1;
			unsigned char ADTRAUEN:1;
			unsigned char EINT:1;
			unsigned char :3;
			unsigned char GTINTPR:2;
			unsigned char GTINTF:1;
			unsigned char GTINTE:1;
			unsigned char GTINTD:1;
			unsigned char GTINTC:1;
			unsigned char GTINTB:1;
			unsigned char GTINTA:1;
		} BIT;
	} GTINTAD;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :2;
			unsigned char CCLR:2;
			unsigned char TPCS:4;
			unsigned char :5;
			unsigned char MD:3;
		} BIT;
	} GTCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :1;
			unsigned char ADTDB:1;
			unsigned char ADTTB:2;
			unsigned char :1;
			unsigned char ADTDA:1;
			unsigned char ADTTA:2;
			unsigned char :1;
			unsigned char CCRSWT:1;
			unsigned char PR:2;
			unsigned char CCRB:2;
			unsigned char CCRA:2;
		} BIT;
	} GTBER;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char OBDTYR:1;
			unsigned char OBDTYF:1;
			unsigned char OBDTY:2;
			unsigned char OADTYR:1;
			unsigned char OADTYF:1;
			unsigned char OADTY:2;
			unsigned char :6;
			unsigned char UDF:1;
			unsigned char UD:1;
		} BIT;
	} GTUDC;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :1;
			unsigned char ADTBL:1;
			unsigned char :1;
			unsigned char ADTAL:1;
			unsigned char :1;
			unsigned char IVTT:3;
			unsigned char IVTC:2;
			unsigned char ITLF:1;
			unsigned char ITLE:1;
			unsigned char ITLD:1;
			unsigned char ITLC:1;
			unsigned char ITLB:1;
			unsigned char ITLA:1;
		} BIT;
	} GTITC;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char TUCF:1;
			unsigned char :3;
			unsigned char DTEF:1;
			unsigned char ITCNT:3;
			unsigned char :8;
		} BIT;
	} GTST;
	unsigned short GTCNT;
	unsigned short GTCCRA;
	unsigned short GTCCRB;
	unsigned short GTCCRC;
	unsigned short GTCCRD;
	unsigned short GTCCRE;
	unsigned short GTCCRF;
	unsigned short GTPR;
	unsigned short GTPBR;
	unsigned short GTPDBR;
	char           wk0[2];
	unsigned short GTADTRA;
	unsigned short GTADTBRA;
	unsigned short GTADTDBRA;
	char           wk1[2];
	unsigned short GTADTRB;
	unsigned short GTADTBRB;
	unsigned short GTADTDBRB;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short OBE:1;
			unsigned short OAE:1;
			unsigned short :1;
			unsigned short SWN:1;
			unsigned short :3;
			unsigned short NFV:1;
			unsigned short NFS:4;
			unsigned short NVB:1;
			unsigned short NVA:1;
			unsigned short NEB:1;
			unsigned short NEA:1;
		} BIT;
	} GTONCR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :7;
			unsigned short TDFER:1;
			unsigned short :2;
			unsigned short TDBDE:1;
			unsigned short TDBUE:1;
			unsigned short :3;
			unsigned short TDE:1;
		} BIT;
	} GTDTCR;
	unsigned short GTDVU;
	unsigned short GTDVD;
	unsigned short GTDBU;
	unsigned short GTDBD;
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short SOS:2;
		} BIT;
	} GTSOS;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short SOTR:1;
		} BIT;
	} GTSOTR;
} st_gpt0_t;

typedef struct st_gpt01 {
	unsigned long  GTCNTLW;
	unsigned long  GTCCRALW;
	unsigned long  GTCCRBLW;
	unsigned long  GTCCRCLW;
	unsigned long  GTCCRDLW;
	unsigned long  GTCCRELW;
	unsigned long  GTCCRFLW;
	unsigned long  GTPRLW;
	unsigned long  GTPBRLW;
	unsigned long  GTPDBRLW;
	unsigned long  GTADTRALW;
	unsigned long  GTADTBRALW;
	unsigned long  GTADTDBRALW;
	unsigned long  GTADTRBLW;
	unsigned long  GTADTBRBLW;
	unsigned long  GTADTDBRBLW;
	unsigned long  GTDVULW;
	unsigned long  GTDVDLW;
	unsigned long  GTDBULW;
	unsigned long  GTDBDLW;
} st_gpt01_t;

typedef struct st_icu {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char IR:1;
		} BIT;
	} IR[256];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DTCE:1;
		} BIT;
	} DTCER[256];
	union {
		unsigned char BYTE;
		struct {
			unsigned char IEN7:1;
			unsigned char IEN6:1;
			unsigned char IEN5:1;
			unsigned char IEN4:1;
			unsigned char IEN3:1;
			unsigned char IEN2:1;
			unsigned char IEN1:1;
			unsigned char IEN0:1;
		} BIT;
	} IER[32];
	char           wk0[192];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char SWINT:1;
		} BIT;
	} SWINTR;
	char           wk1[15];
	union {
		unsigned short WORD;
		struct {
			unsigned short FIEN:1;
			unsigned short :7;
			unsigned short FVCT:8;
		} BIT;
	} FIR;
	char           wk2[14];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char IPR:4;
		} BIT;
	} IPR[256];
	char           wk3[256];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char IRQMD:2;
			unsigned char :2;
		} BIT;
	} IRQCR[8];
	char           wk4[8];
	union {
		unsigned char BYTE;
		struct {
			unsigned char FLTEN7:1;
			unsigned char FLTEN6:1;
			unsigned char FLTEN5:1;
			unsigned char FLTEN4:1;
			unsigned char FLTEN3:1;
			unsigned char FLTEN2:1;
			unsigned char FLTEN1:1;
			unsigned char FLTEN0:1;
		} BIT;
	} IRQFLTE0;
	char           wk5[3];
	union {
		unsigned short WORD;
		struct {
			unsigned short FCLKSEL7:2;
			unsigned short FCLKSEL6:2;
			unsigned short FCLKSEL5:2;
			unsigned short FCLKSEL4:2;
			unsigned short FCLKSEL3:2;
			unsigned short FCLKSEL2:2;
			unsigned short FCLKSEL1:2;
			unsigned short FCLKSEL0:2;
		} BIT;
	} IRQFLTC0;
	char           wk6[106];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char LVD2ST:1;
			unsigned char LVD1ST:1;
			unsigned char IWDTST:1;
			unsigned char :1;
			unsigned char OSTST:1;
			unsigned char NMIST:1;
		} BIT;
	} NMISR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char LVD2EN:1;
			unsigned char LVD1EN:1;
			unsigned char IWDTEN:1;
			unsigned char :1;
			unsigned char OSTEN:1;
			unsigned char NMIEN:1;
		} BIT;
	} NMIER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char LVD2CLR:1;
			unsigned char LVD1CLR:1;
			unsigned char IWDTCLR:1;
			unsigned char :1;
			unsigned char OSTCLR:1;
			unsigned char NMICLR:1;
		} BIT;
	} NMICLR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char NMIMD:1;
			unsigned char :3;
		} BIT;
	} NMICR;
	char           wk7[12];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char NFLTEN:1;
		} BIT;
	} NMIFLTE;
	char           wk8[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char NFCLKSEL:2;
		} BIT;
	} NMIFLTC;
} st_icu_t;

typedef struct st_iwdt {
	unsigned char  IWDTRR;
	char           wk0[1];
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short RPSS:2;
			unsigned short :2;
			unsigned short RPES:2;
			unsigned short CKS:4;
			unsigned short :2;
			unsigned short TOPS:2;
		} BIT;
	} IWDTCR;
	union {
		unsigned short WORD;
		struct {
			unsigned short REFEF:1;
			unsigned short UNDFF:1;
			unsigned short CNTVAL:14;
		} BIT;
	} IWDTSR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char RSTIRQS:1;
			unsigned char :7;
		} BIT;
	} IWDTRCR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char SLCSTP:1;
			unsigned char :7;
		} BIT;
	} IWDTCSTPR;
} st_iwdt_t;

typedef struct st_mpc {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B0WI:1;
			unsigned char PFSWE:1;
			unsigned char :6;
		} BIT;
	} PWPR;
	char           wk0[32];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P00PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P01PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P02PFS;
	char           wk1[5];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P10PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P11PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P12PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P13PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P14PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P15PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P16PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P17PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P20PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P21PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P22PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :2;
			unsigned char PSEL:5;
		} BIT;
	} P23PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :2;
			unsigned char PSEL:5;
		} BIT;
	} P24PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P25PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P26PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P27PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P30PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P31PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P32PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P33PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P34PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P35PFS;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P40PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P41PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P42PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P43PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P44PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P45PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P46PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P47PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P50PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P51PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P52PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P53PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P54PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P55PFS;
	char           wk3[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P60PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P61PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P62PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P63PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P64PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} P65PFS;
	char           wk4[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P70PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P71PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P72PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P73PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P74PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P75PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P76PFS;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P80PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P81PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P82PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P83PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P84PFS;
	char           wk6[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P90PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P91PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P92PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P93PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P94PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} P95PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} P96PFS;
	char           wk7[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PA0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PA1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PA2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PA3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PA4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PA5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PA6PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PA7PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PB0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PB1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PB2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PB3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PB4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PB5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PB6PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PB7PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PC0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PC1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PC2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PC3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PC4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PC5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PC6PFS;
	char           wk8[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PD0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PD1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PD2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PD3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PD4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PD5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PD6PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PD7PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PE0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PE1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :2;
			unsigned char PSEL:5;
		} BIT;
	} PE2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PE3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PE4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PE5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char :1;
			unsigned char PSEL:5;
		} BIT;
	} PE6PFS;
	char           wk9[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PF0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PF1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PF2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PF3PFS;
	char           wk10[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PG0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PG1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PSEL:5;
		} BIT;
	} PG2PFS;
} st_mpc_t;

typedef struct st_mpu {
	union {
		unsigned long LONG;
		struct {
			unsigned long RSPN:28;
			unsigned long :4;
		} BIT;
	} RSPAGE0;
	union {
		unsigned long LONG;
		struct {
			unsigned long REPN:28;
			unsigned long UAC:3;
			unsigned long V:1;
		} BIT;
	} REPAGE0;
	union {
		unsigned long LONG;
		struct {
			unsigned long RSPN:28;
			unsigned long :4;
		} BIT;
	} RSPAGE1;
	union {
		unsigned long LONG;
		struct {
			unsigned long REPN:28;
			unsigned long UAC:3;
			unsigned long V:1;
		} BIT;
	} REPAGE1;
	union {
		unsigned long LONG;
		struct {
			unsigned long RSPN:28;
			unsigned long :4;
		} BIT;
	} RSPAGE2;
	union {
		unsigned long LONG;
		struct {
			unsigned long REPN:28;
			unsigned long UAC:3;
			unsigned long V:1;
		} BIT;
	} REPAGE2;
	union {
		unsigned long LONG;
		struct {
			unsigned long RSPN:28;
			unsigned long :4;
		} BIT;
	} RSPAGE3;
	union {
		unsigned long LONG;
		struct {
			unsigned long REPN:28;
			unsigned long UAC:3;
			unsigned long V:1;
		} BIT;
	} REPAGE3;
	union {
		unsigned long LONG;
		struct {
			unsigned long RSPN:28;
			unsigned long :4;
		} BIT;
	} RSPAGE4;
	union {
		unsigned long LONG;
		struct {
			unsigned long REPN:28;
			unsigned long UAC:3;
			unsigned long V:1;
		} BIT;
	} REPAGE4;
	union {
		unsigned long LONG;
		struct {
			unsigned long RSPN:28;
			unsigned long :4;
		} BIT;
	} RSPAGE5;
	union {
		unsigned long LONG;
		struct {
			unsigned long REPN:28;
			unsigned long UAC:3;
			unsigned long V:1;
		} BIT;
	} REPAGE5;
	union {
		unsigned long LONG;
		struct {
			unsigned long RSPN:28;
			unsigned long :4;
		} BIT;
	} RSPAGE6;
	union {
		unsigned long LONG;
		struct {
			unsigned long REPN:28;
			unsigned long UAC:3;
			unsigned long V:1;
		} BIT;
	} REPAGE6;
	union {
		unsigned long LONG;
		struct {
			unsigned long RSPN:28;
			unsigned long :4;
		} BIT;
	} RSPAGE7;
	union {
		unsigned long LONG;
		struct {
			unsigned long REPN:28;
			unsigned long UAC:3;
			unsigned long V:1;
		} BIT;
	} REPAGE7;
	char           wk0[192];
	union {
		unsigned long LONG;
		struct {
			unsigned long :31;
			unsigned long MPEN:1;
		} BIT;
	} MPEN;
	union {
		unsigned long LONG;
		struct {
			unsigned long :28;
			unsigned long UBAC:3;
			unsigned long :1;
		} BIT;
	} MPBAC;
	union {
		unsigned long LONG;
		struct {
			unsigned long :31;
			unsigned long CLR:1;
		} BIT;
	} MPECLR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :29;
			unsigned long DRW:1;
			unsigned long DMPER:1;
			unsigned long IMPER:1;
		} BIT;
	} MPESTS;
	char           wk1[4];
	union {
		unsigned long LONG;
		struct {
			unsigned long DEA:32;
		} BIT;
	} MPDEA;
	char           wk2[8];
	union {
		unsigned long LONG;
		struct {
			unsigned long SA:32;
		} BIT;
	} MPSA;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short S:1;
		} BIT;
	} MPOPS;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short INV:1;
		} BIT;
	} MPOPI;
	union {
		unsigned long LONG;
		struct {
			unsigned long :8;
			unsigned long HITI:8;
			unsigned long :12;
			unsigned long UHACI:3;
			unsigned long :1;
		} BIT;
	} MHITI;
	union {
		unsigned long LONG;
		struct {
			unsigned long :8;
			unsigned long HITD:8;
			unsigned long :12;
			unsigned long UHACD:3;
			unsigned long :1;
		} BIT;
	} MHITD;
} st_mpu_t;

typedef struct st_mtu {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char OE4D:1;
			unsigned char OE4C:1;
			unsigned char OE3D:1;
			unsigned char OE4B:1;
			unsigned char OE4A:1;
			unsigned char OE3B:1;
		} BIT;
	} TOERA;
	char           wk0[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char BDC:1;
			unsigned char N:1;
			unsigned char P:1;
			unsigned char FB:1;
			unsigned char WF:1;
			unsigned char VF:1;
			unsigned char UF:1;
		} BIT;
	} TGCRA;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char PSYE:1;
			unsigned char :2;
			unsigned char TOCL:1;
			unsigned char TOCS:1;
			unsigned char OLSN:1;
			unsigned char OLSP:1;
		} BIT;
	} TOCR1A;
	union {
		unsigned char BYTE;
		struct {
			unsigned char BF:2;
			unsigned char OLS3N:1;
			unsigned char OLS3P:1;
			unsigned char OLS2N:1;
			unsigned char OLS2P:1;
			unsigned char OLS1N:1;
			unsigned char OLS1P:1;
		} BIT;
	} TOCR2A;
	char           wk1[4];
	unsigned short TCDRA;
	unsigned short TDDRA;
	char           wk2[8];
	unsigned short TCNTSA;
	unsigned short TCBRA;
	char           wk3[12];
	union {
		unsigned char BYTE;
		struct {
			unsigned char T3AEN:1;
			unsigned char T3ACOR:3;
			unsigned char T4VEN:1;
			unsigned char T4VCOR:3;
		} BIT;
	} TITCR1A;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char T3ACNT:3;
			unsigned char :1;
			unsigned char T4VCNT:3;
		} BIT;
	} TITCNT1A;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char BTE:2;
		} BIT;
	} TBTERA;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char TDER:1;
		} BIT;
	} TDERA;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char OLS3N:1;
			unsigned char OLS3P:1;
			unsigned char OLS2N:1;
			unsigned char OLS2P:1;
			unsigned char OLS1N:1;
			unsigned char OLS1P:1;
		} BIT;
	} TOLBRA;
	char           wk6[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char TITM:1;
		} BIT;
	} TITMRA;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TRG4COR:3;
		} BIT;
	} TITCR2A;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TRG4CNT:3;
		} BIT;
	} TITCNT2A;
	char           wk7[35];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CCE:1;
			unsigned char :6;
			unsigned char WRE:1;
		} BIT;
	} TWCRA;
	char           wk8[15];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DRS:1;
		} BIT;
	} TMDR2A;
	char           wk9[15];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CST4:1;
			unsigned char CST3:1;
			unsigned char :1;
			unsigned char CST9:1;
			unsigned char :1;
			unsigned char CST2:1;
			unsigned char CST1:1;
			unsigned char CST0:1;
		} BIT;
	} TSTRA;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SYNC4:1;
			unsigned char SYNC3:1;
			unsigned char :2;
			unsigned char SYNC9:1;
			unsigned char SYNC2:1;
			unsigned char SYNC1:1;
			unsigned char SYNC0:1;
		} BIT;
	} TSYRA;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SCH0:1;
			unsigned char SCH1:1;
			unsigned char SCH2:1;
			unsigned char SCH3:1;
			unsigned char SCH4:1;
			unsigned char SCH9:1;
			unsigned char SCH6:1;
			unsigned char SCH7:1;
		} BIT;
	} TCSYSTR;
	char           wk10[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char RWE:1;
		} BIT;
	} TRWERA;
	char           wk11[1925];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char OE7D:1;
			unsigned char OE7C:1;
			unsigned char OE6D:1;
			unsigned char OE7B:1;
			unsigned char OE7A:1;
			unsigned char OE6B:1;
		} BIT;
	} TOERB;
	char           wk12[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char BDC:1;
			unsigned char N:1;
			unsigned char P:1;
			unsigned char FB:1;
			unsigned char WF:1;
			unsigned char VF:1;
			unsigned char UF:1;
		} BIT;
	} TGCRB;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char PSYE:1;
			unsigned char :2;
			unsigned char TOCL:1;
			unsigned char TOCS:1;
			unsigned char OLSN:1;
			unsigned char OLSP:1;
		} BIT;
	} TOCR1B;
	union {
		unsigned char BYTE;
		struct {
			unsigned char BF:2;
			unsigned char OLS3N:1;
			unsigned char OLS3P:1;
			unsigned char OLS2N:1;
			unsigned char OLS2P:1;
			unsigned char OLS1N:1;
			unsigned char OLS1P:1;
		} BIT;
	} TOCR2B;
	char           wk13[4];
	unsigned short TCDRB;
	unsigned short TDDRB;
	char           wk14[8];
	unsigned short TCNTSB;
	unsigned short TCBRB;
	char           wk15[12];
	union {
		unsigned char BYTE;
		struct {
			unsigned char T6AEN:1;
			unsigned char T6ACOR:3;
			unsigned char T7VEN:1;
			unsigned char T7VCOR:3;
		} BIT;
	} TITCR1B;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char T6ACNT:3;
			unsigned char :1;
			unsigned char T7VCNT:3;
		} BIT;
	} TITCNT1B;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char BTE:2;
		} BIT;
	} TBTERB;
	char           wk16[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char TDER:1;
		} BIT;
	} TDERB;
	char           wk17[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char OLS3N:1;
			unsigned char OLS3P:1;
			unsigned char OLS2N:1;
			unsigned char OLS2P:1;
			unsigned char OLS1N:1;
			unsigned char OLS1P:1;
		} BIT;
	} TOLBRB;
	char           wk18[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char TITM:1;
		} BIT;
	} TITMRB;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TRG7COR:3;
		} BIT;
	} TITCR2B;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TRG7CNT:3;
		} BIT;
	} TITCNT2B;
	char           wk19[35];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CCE:1;
			unsigned char :5;
			unsigned char SCC:1;
			unsigned char WRE:1;
		} BIT;
	} TWCRB;
	char           wk20[15];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DRS:1;
		} BIT;
	} TMDR2B;
	char           wk21[15];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CST7:1;
			unsigned char CST6:1;
			unsigned char :6;
		} BIT;
	} TSTRB;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SYNC7:1;
			unsigned char SYNC6:1;
			unsigned char :6;
		} BIT;
	} TSYRB;
	char           wk22[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char RWE:1;
		} BIT;
	} TRWERB;
	char           wk23[683];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char TADSTRS0:5;
		} BIT;
	} TADSTRGR0;
	char           wk24[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char TADSTRS1:5;
		} BIT;
	} TADSTRGR1;
} st_mtu_t;

typedef struct st_mtu0 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char NFDEN:1;
			unsigned char NFCEN:1;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCR0;
	char           wk0[8];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char NFDEN:1;
			unsigned char NFCEN:1;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCRC;
	char           wk1[102];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CCLR:3;
			unsigned char CKEG:2;
			unsigned char TPSC:3;
		} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char BFE:1;
			unsigned char BFB:1;
			unsigned char BFA:1;
			unsigned char MD:4;
		} BIT;
	} TMDR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOB:4;
			unsigned char IOA:4;
		} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOD:4;
			unsigned char IOC:4;
		} BIT;
	} TIORL;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE:1;
			unsigned char :2;
			unsigned char TCIEV:1;
			unsigned char TGIED:1;
			unsigned char TGIEC:1;
			unsigned char TGIEB:1;
			unsigned char TGIEA:1;
		} BIT;
	} TIER;
	char           wk2[1];
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
	unsigned short TGRC;
	unsigned short TGRD;
	char           wk3[16];
	unsigned short TGRE;
	unsigned short TGRF;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE2:1;
			unsigned char :5;
			unsigned char TGIEF:1;
			unsigned char TGIEE:1;
		} BIT;
	} TIER2;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TTSE:1;
			unsigned char TTSB:1;
			unsigned char TTSA:1;
		} BIT;
	} TBTM;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2;
} st_mtu0_t;

typedef struct st_mtu1 {
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char :2;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCR1;
	char           wk1[238];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char CCLR:2;
			unsigned char CKEG:2;
			unsigned char TPSC:3;
		} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char MD:4;
		} BIT;
	} TMDR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOB:4;
			unsigned char IOA:4;
		} BIT;
	} TIOR;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE:1;
			unsigned char :1;
			unsigned char TCIEU:1;
			unsigned char TCIEV:1;
			unsigned char :2;
			unsigned char TGIEB:1;
			unsigned char TGIEA:1;
		} BIT;
	} TIER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TCFD:1;
			unsigned char :7;
		} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
	char           wk3[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char I2BE:1;
			unsigned char I2AE:1;
			unsigned char I1BE:1;
			unsigned char I1AE:1;
		} BIT;
	} TICCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char PHCKSEL:1;
			unsigned char LWA:1;
		} BIT;
	} TMDR3;
	char           wk4[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PCB:2;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2;
	char           wk5[11];
	unsigned long  TCNTLW;
	unsigned long  TGRALW;
	unsigned long  TGRBLW;
} st_mtu1_t;

typedef struct st_mtu2 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char :2;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCR2;
	char           wk0[365];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char CCLR:2;
			unsigned char CKEG:2;
			unsigned char TPSC:3;
		} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char MD:4;
		} BIT;
	} TMDR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOB:4;
			unsigned char IOA:4;
		} BIT;
	} TIOR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE:1;
			unsigned char :1;
			unsigned char TCIEU:1;
			unsigned char TCIEV:1;
			unsigned char :2;
			unsigned char TGIEB:1;
			unsigned char TGIEA:1;
		} BIT;
	} TIER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TCFD:1;
			unsigned char :7;
		} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char PCB:2;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2;
} st_mtu2_t;

typedef struct st_mtu3 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char CCLR:3;
			unsigned char CKEG:2;
			unsigned char TPSC:3;
		} BIT;
	} TCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char BFB:1;
			unsigned char BFA:1;
			unsigned char MD:4;
		} BIT;
	} TMDR1;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOB:4;
			unsigned char IOA:4;
		} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOD:4;
			unsigned char IOC:4;
		} BIT;
	} TIORL;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE:1;
			unsigned char :2;
			unsigned char TCIEV:1;
			unsigned char TGIED:1;
			unsigned char TGIEC:1;
			unsigned char TGIEB:1;
			unsigned char TGIEA:1;
		} BIT;
	} TIER;
	char           wk3[7];
	unsigned short TCNT;
	char           wk4[6];
	unsigned short TGRA;
	unsigned short TGRB;
	char           wk5[8];
	unsigned short TGRC;
	unsigned short TGRD;
	char           wk6[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TCFD:1;
			unsigned char :7;
		} BIT;
	} TSR;
	char           wk7[11];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char TTSB:1;
			unsigned char TTSA:1;
		} BIT;
	} TBTM;
	char           wk8[19];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2;
	char           wk9[37];
	unsigned short TGRE;
	char           wk10[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char NFDEN:1;
			unsigned char NFCEN:1;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCR3;
} st_mtu3_t;

typedef struct st_mtu4 {
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CCLR:3;
			unsigned char CKEG:2;
			unsigned char TPSC:3;
		} BIT;
	} TCR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char BFB:1;
			unsigned char BFA:1;
			unsigned char MD:4;
		} BIT;
	} TMDR1;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOB:4;
			unsigned char IOA:4;
		} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOD:4;
			unsigned char IOC:4;
		} BIT;
	} TIORL;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE:1;
			unsigned char TTGE2:1;
			unsigned char :1;
			unsigned char TCIEV:1;
			unsigned char TGIED:1;
			unsigned char TGIEC:1;
			unsigned char TGIEB:1;
			unsigned char TGIEA:1;
		} BIT;
	} TIER;
	char           wk4[8];
	unsigned short TCNT;
	char           wk5[8];
	unsigned short TGRA;
	unsigned short TGRB;
	char           wk6[8];
	unsigned short TGRC;
	unsigned short TGRD;
	char           wk7[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TCFD:1;
			unsigned char :7;
		} BIT;
	} TSR;
	char           wk8[11];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char TTSB:1;
			unsigned char TTSA:1;
		} BIT;
	} TBTM;
	char           wk9[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short BF:2;
			unsigned short :6;
			unsigned short UT4AE:1;
			unsigned short DT4AE:1;
			unsigned short UT4BE:1;
			unsigned short DT4BE:1;
			unsigned short ITA3AE:1;
			unsigned short ITA4VE:1;
			unsigned short ITB3AE:1;
			unsigned short ITB4VE:1;
		} BIT;
	} TADCR;
	char           wk10[2];
	unsigned short TADCORA;
	unsigned short TADCORB;
	unsigned short TADCOBRA;
	unsigned short TADCOBRB;
	char           wk11[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2;
	char           wk12[38];
	unsigned short TGRE;
	unsigned short TGRF;
	char           wk13[28];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char NFDEN:1;
			unsigned char NFCEN:1;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCR4;
} st_mtu4_t;

typedef struct st_mtu5 {
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char :1;
			unsigned char NFWEN:1;
			unsigned char NFVEN:1;
			unsigned char NFUEN:1;
		} BIT;
	} NFCR5;
	char           wk1[490];
	unsigned short TCNTU;
	unsigned short TGRU;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char TPSC:2;
		} BIT;
	} TCRU;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char CKEG:2;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2U;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char IOC:5;
		} BIT;
	} TIORU;
	char           wk2[9];
	unsigned short TCNTV;
	unsigned short TGRV;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char TPSC:2;
		} BIT;
	} TCRV;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char CKEG:2;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2V;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char IOC:5;
		} BIT;
	} TIORV;
	char           wk3[9];
	unsigned short TCNTW;
	unsigned short TGRW;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char TPSC:2;
		} BIT;
	} TCRW;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char CKEG:2;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2W;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char IOC:5;
		} BIT;
	} TIORW;
	char           wk4[11];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TGIE5U:1;
			unsigned char TGIE5V:1;
			unsigned char TGIE5W:1;
		} BIT;
	} TIER;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char CSTU5:1;
			unsigned char CSTV5:1;
			unsigned char CSTW5:1;
		} BIT;
	} TSTR;
	char           wk6[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char CMPCLR5U:1;
			unsigned char CMPCLR5V:1;
			unsigned char CMPCLR5W:1;
		} BIT;
	} TCNTCMPCLR;
} st_mtu5_t;

typedef struct st_mtu6 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char CCLR:3;
			unsigned char CKEG:2;
			unsigned char TPSC:3;
		} BIT;
	} TCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char BFB:1;
			unsigned char BFA:1;
			unsigned char MD:4;
		} BIT;
	} TMDR1;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOB:4;
			unsigned char IOA:4;
		} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOD:4;
			unsigned char IOC:4;
		} BIT;
	} TIORL;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE:1;
			unsigned char :2;
			unsigned char TCIEV:1;
			unsigned char TGIED:1;
			unsigned char TGIEC:1;
			unsigned char TGIEB:1;
			unsigned char TGIEA:1;
		} BIT;
	} TIER;
	char           wk3[7];
	unsigned short TCNT;
	char           wk4[6];
	unsigned short TGRA;
	unsigned short TGRB;
	char           wk5[8];
	unsigned short TGRC;
	unsigned short TGRD;
	char           wk6[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TCFD:1;
			unsigned char :7;
		} BIT;
	} TSR;
	char           wk7[11];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char TTSB:1;
			unsigned char TTSA:1;
		} BIT;
	} TBTM;
	char           wk8[19];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2;
	char           wk9[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CE0A:1;
			unsigned char CE0B:1;
			unsigned char CE0C:1;
			unsigned char CE0D:1;
			unsigned char CE1A:1;
			unsigned char CE1B:1;
			unsigned char CE2A:1;
			unsigned char CE2B:1;
		} BIT;
	} TSYCR;
	char           wk10[33];
	unsigned short TGRE;
	char           wk11[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char NFDEN:1;
			unsigned char NFCEN:1;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCR6;
} st_mtu6_t;

typedef struct st_mtu7 {
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CCLR:3;
			unsigned char CKEG:2;
			unsigned char TPSC:3;
		} BIT;
	} TCR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char BFB:1;
			unsigned char BFA:1;
			unsigned char MD:4;
		} BIT;
	} TMDR1;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOB:4;
			unsigned char IOA:4;
		} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOD:4;
			unsigned char IOC:4;
		} BIT;
	} TIORL;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE:1;
			unsigned char TTGE2:1;
			unsigned char :1;
			unsigned char TCIEV:1;
			unsigned char TGIED:1;
			unsigned char TGIEC:1;
			unsigned char TGIEB:1;
			unsigned char TGIEA:1;
		} BIT;
	} TIER;
	char           wk4[8];
	unsigned short TCNT;
	char           wk5[8];
	unsigned short TGRA;
	unsigned short TGRB;
	char           wk6[8];
	unsigned short TGRC;
	unsigned short TGRD;
	char           wk7[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TCFD:1;
			unsigned char :7;
		} BIT;
	} TSR;
	char           wk8[11];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char TTSB:1;
			unsigned char TTSA:1;
		} BIT;
	} TBTM;
	char           wk9[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short BF:2;
			unsigned short :6;
			unsigned short UT7AE:1;
			unsigned short DT7AE:1;
			unsigned short UT7BE:1;
			unsigned short DT7BE:1;
			unsigned short ITA6AE:1;
			unsigned short ITA7VE:1;
			unsigned short ITB6AE:1;
			unsigned short ITB7VE:1;
		} BIT;
	} TADCR;
	char           wk10[2];
	unsigned short TADCORA;
	unsigned short TADCORB;
	unsigned short TADCOBRA;
	unsigned short TADCOBRB;
	char           wk11[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2;
	char           wk12[38];
	unsigned short TGRE;
	unsigned short TGRF;
	char           wk13[28];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char NFDEN:1;
			unsigned char NFCEN:1;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCR7;
} st_mtu7_t;

typedef struct st_mtu9 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char NFCS:2;
			unsigned char NFDEN:1;
			unsigned char NFCEN:1;
			unsigned char NFBEN:1;
			unsigned char NFAEN:1;
		} BIT;
	} NFCR9;
	char           wk0[745];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CCLR:3;
			unsigned char CKEG:2;
			unsigned char TPSC:3;
		} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char BFE:1;
			unsigned char BFB:1;
			unsigned char BFA:1;
			unsigned char MD:4;
		} BIT;
	} TMDR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOB:4;
			unsigned char IOA:4;
		} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IOD:4;
			unsigned char IOC:4;
		} BIT;
	} TIORL;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE:1;
			unsigned char :2;
			unsigned char TCIEV:1;
			unsigned char TGIED:1;
			unsigned char TGIEC:1;
			unsigned char TGIEB:1;
			unsigned char TGIEA:1;
		} BIT;
	} TIER;
	char           wk1[1];
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
	unsigned short TGRC;
	unsigned short TGRD;
	char           wk2[16];
	unsigned short TGRE;
	unsigned short TGRF;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TTGE2:1;
			unsigned char :5;
			unsigned char TGIEF:1;
			unsigned char TGIEE:1;
		} BIT;
	} TIER2;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TTSE:1;
			unsigned char TTSB:1;
			unsigned char TTSA:1;
		} BIT;
	} TBTM;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TPSC2:3;
		} BIT;
	} TCR2;
} st_mtu9_t;

typedef struct st_poe {
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :3;
			unsigned char POE0F:1;
			unsigned char :3;
			unsigned char PIE1:1;
			unsigned char :6;
			unsigned char POE0M:2;
		} BIT;
	} ICSR1;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char OSF1:1;
			unsigned char :5;
			unsigned char OCE1:1;
			unsigned char OIE1:1;
			unsigned char :8;
		} BIT;
	} OCSR1;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :3;
			unsigned char POE4F:1;
			unsigned char :3;
			unsigned char PIE2:1;
			unsigned char :6;
			unsigned char POE4M:2;
		} BIT;
	} ICSR2;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char OSF2:1;
			unsigned char :5;
			unsigned char OCE2:1;
			unsigned char OIE2:1;
			unsigned char :8;
		} BIT;
	} OCSR2;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :3;
			unsigned char POE8F:1;
			unsigned char :2;
			unsigned char POE8E:1;
			unsigned char PIE3:1;
			unsigned char :6;
			unsigned char POE8M:2;
		} BIT;
	} ICSR3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char MTUCH9HIZ:1;
			unsigned char :1;
			unsigned char GPT03HIZ:1;
			unsigned char GPT02HIZ:1;
			unsigned char MTUCH0HIZ:1;
			unsigned char MTUCH67HIZ:1;
			unsigned char MTUCH34HIZ:1;
		} BIT;
	} SPOER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char MTU0B1ZE:1;
			unsigned char MTU0A1ZE:1;
			unsigned char MTU0DZE:1;
			unsigned char MTU0CZE:1;
			unsigned char MTU0BZE:1;
			unsigned char MTU0AZE:1;
		} BIT;
	} POECR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short MTU3BDZE:1;
			unsigned short MTU4ACZE:1;
			unsigned short MTU4BDZE:1;
			unsigned short :5;
			unsigned short MTU6BDZE:1;
			unsigned short MTU7ACZE:1;
			unsigned short MTU7BDZE:1;
		} BIT;
	} POECR2;
	union {
		unsigned short WORD;
		struct {
			unsigned short GPT3B1ZE:1;
			unsigned short GPT3A1ZE:1;
			unsigned short GPT2B1ZE:1;
			unsigned short GPT2A1ZE:1;
			unsigned short GPT1B1ZE:1;
			unsigned short GPT1A1ZE:1;
			unsigned short GPT0B1ZE:1;
			unsigned short GPT0A1ZE:1;
			unsigned short :2;
			unsigned short GPT2BZE:1;
			unsigned short GPT2AZE:1;
			unsigned short GPT1BZE:1;
			unsigned short GPT1AZE:1;
			unsigned short GPT0BZE:1;
			unsigned short GPT0AZE:1;
		} BIT;
	} POECR3;
	union {
		unsigned short WORD;
		struct {
			unsigned short :1;
			unsigned short IC6ADDMT67ZE:1;
			unsigned short IC5ADDMT67ZE:1;
			unsigned short IC4ADDMT67ZE:1;
			unsigned short IC3ADDMT67ZE:1;
			unsigned short :1;
			unsigned short IC1ADDMT67ZE:1;
			unsigned short CMADDMT67ZE:1;
			unsigned short :1;
			unsigned short IC6ADDMT34ZE:1;
			unsigned short IC5ADDMT34ZE:1;
			unsigned short IC4ADDMT34ZE:1;
			unsigned short IC3ADDMT34ZE:1;
			unsigned short IC2ADDMT34ZE:1;
			unsigned short :1;
			unsigned short CMADDMT34ZE:1;
		} BIT;
	} POECR4;
	union {
		unsigned short WORD;
		struct {
			unsigned short :9;
			unsigned short IC6ADDMT0ZE:1;
			unsigned short IC5ADDMT0ZE:1;
			unsigned short IC4ADDMT0ZE:1;
			unsigned short :1;
			unsigned short IC2ADDMT0ZE:1;
			unsigned short IC1ADDMT0ZE:1;
			unsigned short CMADDMT0ZE:1;
		} BIT;
	} POECR5;
	union {
		unsigned short WORD;
		struct {
			unsigned short :1;
			unsigned short IC6ADDGPT03ZE:1;
			unsigned short :1;
			unsigned short IC4ADDGPT03ZE:1;
			unsigned short IC3ADDGPT03ZE:1;
			unsigned short IC2ADDGPT03ZE:1;
			unsigned short IC1ADDGPT03ZE:1;
			unsigned short CMADDGPT03ZE:1;
			unsigned short :1;
			unsigned short IC6ADDGPT02ZE:1;
			unsigned short IC5ADDGPT02ZE:1;
			unsigned short :1;
			unsigned short IC3ADDGPT02ZE:1;
			unsigned short IC2ADDGPT02ZE:1;
			unsigned short IC1ADDGPT02ZE:1;
			unsigned short CMADDGPT02ZE:1;
		} BIT;
	} POECR6;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :3;
			unsigned char POE10F:1;
			unsigned char :2;
			unsigned char POE10E:1;
			unsigned char PIE4:1;
			unsigned char :6;
			unsigned char POE10M:2;
		} BIT;
	} ICSR4;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :3;
			unsigned char POE11F:1;
			unsigned char :2;
			unsigned char POE11E:1;
			unsigned char PIE5:1;
			unsigned char :6;
			unsigned char POE11M:2;
		} BIT;
	} ICSR5;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char OLSEN:1;
			unsigned char :1;
			unsigned char OLSG2B:1;
			unsigned char OLSG2A:1;
			unsigned char OLSG1B:1;
			unsigned char OLSG1A:1;
			unsigned char OLSG0B:1;
			unsigned char OLSG0A:1;
		} BIT;
	} ALR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short OSTSTF:1;
			unsigned short :2;
			unsigned short OSTSTE:1;
			unsigned short :9;
		} BIT;
	} ICSR6;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char OLSEN:1;
			unsigned char :1;
			unsigned char OLSG6B:1;
			unsigned char OLSG6A:1;
			unsigned char OLSG5B:1;
			unsigned char OLSG5A:1;
			unsigned char OLSG4B:1;
			unsigned char OLSG4A:1;
		} BIT;
	} ALR2;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :3;
			unsigned char POE12F:1;
			unsigned char :2;
			unsigned char POE12E:1;
			unsigned char PIE7:1;
			unsigned char :6;
			unsigned char POE12M:2;
		} BIT;
	} ICSR7;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short MTU9C2ZE:1;
			unsigned short :1;
			unsigned short MTU9A2ZE:1;
			unsigned short MTU9D1ZE:1;
			unsigned short MTU9C1ZE:1;
			unsigned short MTU9B1ZE:1;
			unsigned short MTU9A1ZE:1;
			unsigned short MTU9DZE:1;
			unsigned short MTU9CZE:1;
			unsigned short MTU9BZE:1;
			unsigned short MTU9AZE:1;
		} BIT;
	} POECR7;
	union {
		unsigned short WORD;
		struct {
			unsigned short :10;
			unsigned short IC5ADDMT9ZE:1;
			unsigned short IC4ADDMT9ZE:1;
			unsigned short IC3ADDMT9ZE:1;
			unsigned short IC2ADDMT9ZE:1;
			unsigned short IC1ADDMT9ZE:1;
			unsigned short CMADDMT9ZE:1;
		} BIT;
	} POECR8;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short C3FLAG:1;
			unsigned short C2FLAG:1;
			unsigned short C1FLAG:1;
			unsigned short C0FLAG:1;
		} BIT;
	} POECMPFR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short POEREQ3:1;
			unsigned short POEREQ2:1;
			unsigned short POEREQ1:1;
			unsigned short POEREQ0:1;
		} BIT;
	} POECMPSEL;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char OSF3:1;
			unsigned char :5;
			unsigned char OCE3:1;
			unsigned char OIE3:1;
			unsigned char :8;
		} BIT;
	} OCSR3;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :8;
			unsigned char OLSEN:1;
			unsigned char :1;
			unsigned char OLSG2B:1;
			unsigned char OLSG2A:1;
			unsigned char OLSG1B:1;
			unsigned char OLSG1A:1;
			unsigned char OLSG0B:1;
			unsigned char OLSG0A:1;
		} BIT;
	} ALR3;
	char           wk0[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char MTU0B1ME:1;
			unsigned char MTU0A1ME:1;
			unsigned char MTU0DME:1;
			unsigned char MTU0CME:1;
			unsigned char MTU0BME:1;
			unsigned char MTU0AME:1;
		} BIT;
	} PMMCR0;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short MTU3DME:1;
			unsigned short MTU4CME:1;
			unsigned short MTU4DME:1;
			unsigned short MTU3BME:1;
			unsigned short MTU4AME:1;
			unsigned short MTU4BME:1;
			unsigned short :2;
			unsigned short MTU6DME:1;
			unsigned short MTU7CME:1;
			unsigned short MTU7DME:1;
			unsigned short MTU6BME:1;
			unsigned short MTU7AME:1;
			unsigned short MTU7BME:1;
		} BIT;
	} PMMCR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short GPT3B1ME:1;
			unsigned short GPT3A1ME:1;
			unsigned short GPT2B1ME:1;
			unsigned short GPT2A1ME:1;
			unsigned short GPT1B1ME:1;
			unsigned short GPT1A1ME:1;
			unsigned short GPT0B1ME:1;
			unsigned short GPT0A1ME:1;
			unsigned short :2;
			unsigned short GPT2BME:1;
			unsigned short GPT2AME:1;
			unsigned short GPT1BME:1;
			unsigned short GPT1AME:1;
			unsigned short GPT0BME:1;
			unsigned short GPT0AME:1;
		} BIT;
	} PMMCR2;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short MTU9C2ME:1;
			unsigned short :1;
			unsigned short MTU9A2ME:1;
			unsigned short MTU9D1ME:1;
			unsigned short MTU9C1ME:1;
			unsigned short MTU9B1ME:1;
			unsigned short MTU9A1ME:1;
			unsigned short MTU9DME:1;
			unsigned short MTU9CME:1;
			unsigned short MTU9BME:1;
			unsigned short MTU9AME:1;
		} BIT;
	} PMMCR3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX4;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX5;
} st_poe_t;

typedef struct st_port0 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	char           wk4[63];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_port0_t;

typedef struct st_port1 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[32];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[61];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_port1_t;

typedef struct st_port2 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[33];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[60];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_port2_t;

typedef struct st_port3 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[34];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[59];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_port3_t;

typedef struct st_port4 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[127];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
} st_port4_t;

typedef struct st_port5 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[127];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
} st_port5_t;

typedef struct st_port6 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[127];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
} st_port6_t;

typedef struct st_port7 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[38];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[55];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_port7_t;

typedef struct st_port8 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[39];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[54];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_port8_t;

typedef struct st_port9 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[40];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[53];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :6;
		} BIT;
	} DSCR;
} st_port9_t;

typedef struct st_porta {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[41];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[52];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_porta_t;

typedef struct st_portb {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[42];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[51];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char :2;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_portb_t;

typedef struct st_portc {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[43];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[50];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_portc_t;

typedef struct st_portd {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[44];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[49];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_portd_t;

typedef struct st_porte {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[45];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :3;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[48];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char :1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_porte_t;

typedef struct st_portf {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[46];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	char           wk4[48];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_portf_t;

typedef struct st_portg {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[47];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	char           wk4[47];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
} st_portg_t;

typedef struct st_riic {
	union {
		unsigned char BYTE;
		struct {
			unsigned char ICE:1;
			unsigned char IICRST:1;
			unsigned char CLO:1;
			unsigned char SOWP:1;
			unsigned char SCLO:1;
			unsigned char SDAO:1;
			unsigned char SCLI:1;
			unsigned char SDAI:1;
		} BIT;
	} ICCR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char BBSY:1;
			unsigned char MST:1;
			unsigned char TRS:1;
			unsigned char :1;
			unsigned char SP:1;
			unsigned char RS:1;
			unsigned char ST:1;
			unsigned char :1;
		} BIT;
	} ICCR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char MTWP:1;
			unsigned char CKS:3;
			unsigned char BCWP:1;
			unsigned char BC:3;
		} BIT;
	} ICMR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DLCS:1;
			unsigned char SDDL:3;
			unsigned char :1;
			unsigned char TMOH:1;
			unsigned char TMOL:1;
			unsigned char TMOS:1;
		} BIT;
	} ICMR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SMBS:1;
			unsigned char WAIT:1;
			unsigned char RDRFS:1;
			unsigned char ACKWP:1;
			unsigned char ACKBT:1;
			unsigned char ACKBR:1;
			unsigned char NF:2;
		} BIT;
	} ICMR3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char SCLE:1;
			unsigned char NFE:1;
			unsigned char NACKE:1;
			unsigned char SALE:1;
			unsigned char NALE:1;
			unsigned char MALE:1;
			unsigned char TMOE:1;
		} BIT;
	} ICFER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char HOAE:1;
			unsigned char :1;
			unsigned char DIDE:1;
			unsigned char :1;
			unsigned char GCAE:1;
			unsigned char SAR2E:1;
			unsigned char SAR1E:1;
			unsigned char SAR0E:1;
		} BIT;
	} ICSER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TIE:1;
			unsigned char TEIE:1;
			unsigned char RIE:1;
			unsigned char NAKIE:1;
			unsigned char SPIE:1;
			unsigned char STIE:1;
			unsigned char ALIE:1;
			unsigned char TMOIE:1;
		} BIT;
	} ICIER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char HOA:1;
			unsigned char :1;
			unsigned char DID:1;
			unsigned char :1;
			unsigned char GCA:1;
			unsigned char AAS2:1;
			unsigned char AAS1:1;
			unsigned char AAS0:1;
		} BIT;
	} ICSR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TDRE:1;
			unsigned char TEND:1;
			unsigned char RDRF:1;
			unsigned char NACKF:1;
			unsigned char STOP:1;
			unsigned char START:1;
			unsigned char AL:1;
			unsigned char TMOF:1;
		} BIT;
	} ICSR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SVA:7;
			unsigned char SVA0:1;
		} BIT;
	} SARL0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SVA:2;
			unsigned char FS:1;
		} BIT;
	} SARU0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SVA:7;
			unsigned char SVA0:1;
		} BIT;
	} SARL1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SVA:2;
			unsigned char FS:1;
		} BIT;
	} SARU1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SVA:7;
			unsigned char SVA0:1;
		} BIT;
	} SARL2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SVA:2;
			unsigned char FS:1;
		} BIT;
	} SARU2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char BRL:5;
		} BIT;
	} ICBRL;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char BRH:5;
		} BIT;
	} ICBRH;
	unsigned char  ICDRT;
	unsigned char  ICDRR;
} st_riic_t;

typedef struct st_rspi {
	union {
		unsigned char BYTE;
		struct {
			unsigned char SPRIE:1;
			unsigned char SPE:1;
			unsigned char SPTIE:1;
			unsigned char SPEIE:1;
			unsigned char MSTR:1;
			unsigned char MODFEN:1;
			unsigned char TXMD:1;
			unsigned char SPMS:1;
		} BIT;
	} SPCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char SSL3P:1;
			unsigned char SSL2P:1;
			unsigned char SSL1P:1;
			unsigned char SSL0P:1;
		} BIT;
	} SSLP;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char MOIFE:1;
			unsigned char MOIFV:1;
			unsigned char :2;
			unsigned char SPLP2:1;
			unsigned char SPLP:1;
		} BIT;
	} SPPCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SPRF:1;
			unsigned char :1;
			unsigned char SPTEF:1;
			unsigned char UDRF:1;
			unsigned char PERF:1;
			unsigned char MODF:1;
			unsigned char IDLNF:1;
			unsigned char OVRF:1;
		} BIT;
	} SPSR;
	union {
		unsigned long LONG;
		struct {
			unsigned short H;
		} WORD;
	} SPDR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SPSLN:3;
		} BIT;
	} SPSCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char SPECM:3;
			unsigned char :1;
			unsigned char SPCP:3;
		} BIT;
	} SPSSR;
	unsigned char  SPBR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char SPLW:1;
			unsigned char SPRDTD:1;
			unsigned char :2;
			unsigned char SPFC:2;
		} BIT;
	} SPDCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SCKDL:3;
		} BIT;
	} SPCKD;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SLNDL:3;
		} BIT;
	} SSLND;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SPNDL:3;
		} BIT;
	} SPND;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char SCKASE:1;
			unsigned char PTE:1;
			unsigned char SPIIE:1;
			unsigned char SPOE:1;
			unsigned char SPPE:1;
		} BIT;
	} SPCR2;
	union {
		unsigned short WORD;
		struct {
			unsigned short SCKDEN:1;
			unsigned short SLNDEN:1;
			unsigned short SPNDEN:1;
			unsigned short LSBF:1;
			unsigned short SPB:4;
			unsigned short SSLKP:1;
			unsigned short SSLA:3;
			unsigned short BRDV:2;
			unsigned short CPOL:1;
			unsigned short CPHA:1;
		} BIT;
	} SPCMD0;
	union {
		unsigned short WORD;
		struct {
			unsigned short SCKDEN:1;
			unsigned short SLNDEN:1;
			unsigned short SPNDEN:1;
			unsigned short LSBF:1;
			unsigned short SPB:4;
			unsigned short SSLKP:1;
			unsigned short SSLA:3;
			unsigned short BRDV:2;
			unsigned short CPOL:1;
			unsigned short CPHA:1;
		} BIT;
	} SPCMD1;
	union {
		unsigned short WORD;
		struct {
			unsigned short SCKDEN:1;
			unsigned short SLNDEN:1;
			unsigned short SPNDEN:1;
			unsigned short LSBF:1;
			unsigned short SPB:4;
			unsigned short SSLKP:1;
			unsigned short SSLA:3;
			unsigned short BRDV:2;
			unsigned short CPOL:1;
			unsigned short CPHA:1;
		} BIT;
	} SPCMD2;
	union {
		unsigned short WORD;
		struct {
			unsigned short SCKDEN:1;
			unsigned short SLNDEN:1;
			unsigned short SPNDEN:1;
			unsigned short LSBF:1;
			unsigned short SPB:4;
			unsigned short SSLKP:1;
			unsigned short SSLA:3;
			unsigned short BRDV:2;
			unsigned short CPOL:1;
			unsigned short CPHA:1;
		} BIT;
	} SPCMD3;
	union {
		unsigned short WORD;
		struct {
			unsigned short SCKDEN:1;
			unsigned short SLNDEN:1;
			unsigned short SPNDEN:1;
			unsigned short LSBF:1;
			unsigned short SPB:4;
			unsigned short SSLKP:1;
			unsigned short SSLA:3;
			unsigned short BRDV:2;
			unsigned short CPOL:1;
			unsigned short CPHA:1;
		} BIT;
	} SPCMD4;
	union {
		unsigned short WORD;
		struct {
			unsigned short SCKDEN:1;
			unsigned short SLNDEN:1;
			unsigned short SPNDEN:1;
			unsigned short LSBF:1;
			unsigned short SPB:4;
			unsigned short SSLKP:1;
			unsigned short SSLA:3;
			unsigned short BRDV:2;
			unsigned short CPOL:1;
			unsigned short CPHA:1;
		} BIT;
	} SPCMD5;
	union {
		unsigned short WORD;
		struct {
			unsigned short SCKDEN:1;
			unsigned short SLNDEN:1;
			unsigned short SPNDEN:1;
			unsigned short LSBF:1;
			unsigned short SPB:4;
			unsigned short SSLKP:1;
			unsigned short SSLA:3;
			unsigned short BRDV:2;
			unsigned short CPOL:1;
			unsigned short CPHA:1;
		} BIT;
	} SPCMD6;
	union {
		unsigned short WORD;
		struct {
			unsigned short SCKDEN:1;
			unsigned short SLNDEN:1;
			unsigned short SPNDEN:1;
			unsigned short LSBF:1;
			unsigned short SPB:4;
			unsigned short SSLKP:1;
			unsigned short SSLA:3;
			unsigned short BRDV:2;
			unsigned short CPOL:1;
			unsigned short CPHA:1;
		} BIT;
	} SPCMD7;
} st_rspi_t;

typedef struct st_s12ad {
	union {
		unsigned short WORD;
		struct {
			unsigned short ADST:1;
			unsigned short ADCS:2;
			unsigned short ADIE:1;
			unsigned short :2;
			unsigned short TRGE:1;
			unsigned short EXTRG:1;
			unsigned short DBLE:1;
			unsigned short GBADIE:1;
			unsigned short :1;
			unsigned short DBLANS:5;
		} BIT;
	} ADCSR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short ANSA003:1;
			unsigned short ANSA002:1;
			unsigned short ANSA001:1;
			unsigned short ANSA000:1;
		} BIT;
	} ADANSA0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ANSA100:1;
		} BIT;
	} ADANSA1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short ADS003:1;
			unsigned short ADS002:1;
			unsigned short ADS001:1;
			unsigned short ADS000:1;
		} BIT;
	} ADADS0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ADS100:1;
		} BIT;
	} ADADS1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char AVEE:1;
			unsigned char :4;
			unsigned char ADC:3;
		} BIT;
	} ADADC;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			unsigned short ADRFMT:1;
			unsigned short :3;
			unsigned short DIAGM:1;
			unsigned short DIAGLD:1;
			unsigned short DIAGVAL:2;
			unsigned short :2;
			unsigned short ACE:1;
			unsigned short :5;
		} BIT;
	} ADCER;
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short TRSA:6;
			unsigned short :2;
			unsigned short TRSB:6;
		} BIT;
	} ADSTRGR;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short ANSB003:1;
			unsigned short ANSB002:1;
			unsigned short ANSB001:1;
			unsigned short ANSB000:1;
		} BIT;
	} ADANSB0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ANSB100:1;
		} BIT;
	} ADANSB1;
	unsigned short ADDBLDR;
	char           wk3[4];
	unsigned short ADRD;
	unsigned short ADDR0;
	unsigned short ADDR1;
	unsigned short ADDR2;
	unsigned short ADDR3;
	char           wk4[24];
	unsigned short ADDR16;
	char           wk5[56];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char ADNDIS:5;
		} BIT;
	} ADDISCR;
	char           wk6[5];
	union {
		unsigned short WORD;
		struct {
			unsigned short GBRP:1;
			unsigned short LGRRS:1;
			unsigned short :12;
			unsigned short GBRSCN:1;
			unsigned short PGS:1;
		} BIT;
	} ADGSPCR;
	char           wk7[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk8[76];
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short ANSC003:1;
			unsigned short ANSC002:1;
			unsigned short ANSC001:1;
			unsigned short ANSC000:1;
		} BIT;
	} ADANSC0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ANSC100:1;
		} BIT;
	} ADANSC1;
	char           wk9[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char GRCE:1;
			unsigned char GCADIE:1;
			unsigned char TRSC:6;
		} BIT;
	} ADGCTRGR;
	char           wk10[3];
	unsigned char  ADSSTRL;
	char           wk11[2];
	unsigned char  ADSSTR0;
	unsigned char  ADSSTR1;
	unsigned char  ADSSTR2;
	unsigned char  ADSSTR3;
	char           wk12[188];
	union {
		unsigned short WORD;
		struct {
			unsigned short :13;
			unsigned short P000ENAMP:1;
			unsigned short P000SEL1:1;
			unsigned short :1;
		} BIT;
	} ADPGACR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short P000GAIN:4;
		} BIT;
	} ADPGAGS0;
} st_s12ad_t;

typedef struct st_s12ad1 {
	union {
		unsigned short WORD;
		struct {
			unsigned short ADST:1;
			unsigned short ADCS:2;
			unsigned short ADIE:1;
			unsigned short :2;
			unsigned short TRGE:1;
			unsigned short EXTRG:1;
			unsigned short DBLE:1;
			unsigned short GBADIE:1;
			unsigned short :1;
			unsigned short DBLANS:5;
		} BIT;
	} ADCSR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short ANSA003:1;
			unsigned short ANSA002:1;
			unsigned short ANSA001:1;
			unsigned short ANSA000:1;
		} BIT;
	} ADANSA0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ANSA100:1;
		} BIT;
	} ADANSA1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short ADS003:1;
			unsigned short ADS002:1;
			unsigned short ADS001:1;
			unsigned short ADS000:1;
		} BIT;
	} ADADS0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ADS100:1;
		} BIT;
	} ADADS1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char AVEE:1;
			unsigned char :4;
			unsigned char ADC:3;
		} BIT;
	} ADADC;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			unsigned short ADRFMT:1;
			unsigned short :3;
			unsigned short DIAGM:1;
			unsigned short DIAGLD:1;
			unsigned short DIAGVAL:2;
			unsigned short :2;
			unsigned short ACE:1;
			unsigned short :5;
		} BIT;
	} ADCER;
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short TRSA:6;
			unsigned short :2;
			unsigned short TRSB:6;
		} BIT;
	} ADSTRGR;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short ANSB003:1;
			unsigned short ANSB002:1;
			unsigned short ANSB001:1;
			unsigned short ANSB000:1;
		} BIT;
	} ADANSB0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ANSB100:1;
		} BIT;
	} ADANSB1;
	unsigned short ADDBLDR;
	char           wk3[4];
	unsigned short ADRD;
	unsigned short ADDR0;
	unsigned short ADDR1;
	unsigned short ADDR2;
	unsigned short ADDR3;
	char           wk4[24];
	unsigned short ADDR16;
	char           wk5[36];
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short SHANS:3;
			unsigned short SSTSH:8;
		} BIT;
	} ADSHCR;
	char           wk6[18];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char ADNDIS:5;
		} BIT;
	} ADDISCR;
	char           wk7[5];
	union {
		unsigned short WORD;
		struct {
			unsigned short GBRP:1;
			unsigned short LGRRS:1;
			unsigned short :12;
			unsigned short GBRSCN:1;
			unsigned short PGS:1;
		} BIT;
	} ADGSPCR;
	char           wk8[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk9[76];
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short ANSC003:1;
			unsigned short ANSC002:1;
			unsigned short ANSC001:1;
			unsigned short ANSC000:1;
		} BIT;
	} ADANSC0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ANSC100:1;
		} BIT;
	} ADANSC1;
	char           wk10[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char GRCE:1;
			unsigned char GCADIE:1;
			unsigned char TRSC:6;
		} BIT;
	} ADGCTRGR;
	char           wk11[3];
	unsigned char  ADSSTRL;
	char           wk12[2];
	unsigned char  ADSSTR0;
	unsigned char  ADSSTR1;
	unsigned char  ADSSTR2;
	unsigned char  ADSSTR3;
	char           wk13[188];
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short P102ENAMP:1;
			unsigned short P102SEL1:1;
			unsigned short :2;
			unsigned short P101ENAMP:1;
			unsigned short P101SEL1:1;
			unsigned short :2;
			unsigned short P100ENAMP:1;
			unsigned short P100SEL1:1;
			unsigned short :1;
		} BIT;
	} ADPGACR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short P102GAIN:4;
			unsigned short P101GAIN:4;
			unsigned short P100GAIN:4;
		} BIT;
	} ADPGAGS0;
} st_s12ad1_t;

typedef struct st_s12ad2 {
	union {
		unsigned short WORD;
		struct {
			unsigned short ADST:1;
			unsigned short ADCS:2;
			unsigned short ADIE:1;
			unsigned short :2;
			unsigned short TRGE:1;
			unsigned short EXTRG:1;
			unsigned short DBLE:1;
			unsigned short GBADIE:1;
			unsigned short :1;
			unsigned short DBLANS:5;
		} BIT;
	} ADCSR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short ANSA011:1;
			unsigned short ANSA010:1;
			unsigned short ANSA009:1;
			unsigned short ANSA008:1;
			unsigned short ANSA007:1;
			unsigned short ANSA006:1;
			unsigned short ANSA005:1;
			unsigned short ANSA004:1;
			unsigned short ANSA003:1;
			unsigned short ANSA002:1;
			unsigned short ANSA001:1;
			unsigned short ANSA000:1;
		} BIT;
	} ADANSA0;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short ADS011:1;
			unsigned short ADS010:1;
			unsigned short ADS009:1;
			unsigned short ADS008:1;
			unsigned short ADS007:1;
			unsigned short ADS006:1;
			unsigned short ADS005:1;
			unsigned short ADS004:1;
			unsigned short ADS003:1;
			unsigned short ADS002:1;
			unsigned short ADS001:1;
			unsigned short ADS000:1;
		} BIT;
	} ADADS0;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char AVEE:1;
			unsigned char :4;
			unsigned char ADC:3;
		} BIT;
	} ADADC;
	char           wk3[1];
	union {
		unsigned short WORD;
		struct {
			unsigned short ADRFMT:1;
			unsigned short :3;
			unsigned short DIAGM:1;
			unsigned short DIAGLD:1;
			unsigned short DIAGVAL:2;
			unsigned short :2;
			unsigned short ACE:1;
			unsigned short :5;
		} BIT;
	} ADCER;
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short TRSA:6;
			unsigned short :2;
			unsigned short TRSB:6;
		} BIT;
	} ADSTRGR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :6;
			unsigned short OCSA:1;
			unsigned short :7;
			unsigned short OCSAD:1;
			unsigned short :1;
		} BIT;
	} ADEXICR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short ANSB011:1;
			unsigned short ANSB010:1;
			unsigned short ANSB009:1;
			unsigned short ANSB008:1;
			unsigned short ANSB007:1;
			unsigned short ANSB006:1;
			unsigned short ANSB005:1;
			unsigned short ANSB004:1;
			unsigned short ANSB003:1;
			unsigned short ANSB002:1;
			unsigned short ANSB001:1;
			unsigned short ANSB000:1;
		} BIT;
	} ADANSB0;
	char           wk4[2];
	unsigned short ADDBLDR;
	char           wk5[2];
	unsigned short ADOCDR;
	unsigned short ADRD;
	unsigned short ADDR0;
	unsigned short ADDR1;
	unsigned short ADDR2;
	unsigned short ADDR3;
	unsigned short ADDR4;
	unsigned short ADDR5;
	unsigned short ADDR6;
	unsigned short ADDR7;
	unsigned short ADDR8;
	unsigned short ADDR9;
	unsigned short ADDR10;
	unsigned short ADDR11;
	char           wk6[66];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char ADNDIS:5;
		} BIT;
	} ADDISCR;
	char           wk7[5];
	union {
		unsigned short WORD;
		struct {
			unsigned short GBRP:1;
			unsigned short LGRRS:1;
			unsigned short :12;
			unsigned short GBRSCN:1;
			unsigned short PGS:1;
		} BIT;
	} ADGSPCR;
	char           wk8[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk9[76];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short ANSC011:1;
			unsigned short ANSC010:1;
			unsigned short ANSC009:1;
			unsigned short ANSC008:1;
			unsigned short ANSC007:1;
			unsigned short ANSC006:1;
			unsigned short ANSC005:1;
			unsigned short ANSC004:1;
			unsigned short ANSC003:1;
			unsigned short ANSC002:1;
			unsigned short ANSC001:1;
			unsigned short ANSC000:1;
		} BIT;
	} ADANSC0;
	char           wk10[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char GRCE:1;
			unsigned char GCADIE:1;
			unsigned char TRSC:6;
		} BIT;
	} ADGCTRGR;
	char           wk11[5];
	unsigned char  ADSSTRO;
	unsigned char  ADSSTR0;
	unsigned char  ADSSTR1;
	unsigned char  ADSSTR2;
	unsigned char  ADSSTR3;
	unsigned char  ADSSTR4;
	unsigned char  ADSSTR5;
	unsigned char  ADSSTR6;
	unsigned char  ADSSTR7;
	unsigned char  ADSSTR8;
	unsigned char  ADSSTR9;
	unsigned char  ADSSTR10;
	unsigned char  ADSSTR11;
} st_s12ad2_t;

typedef struct st_sci {
	union {
		unsigned char BYTE;
		struct {
			unsigned char CM:1;
			unsigned char CHR:1;
			unsigned char PE:1;
			unsigned char PM:1;
			unsigned char STOP:1;
			unsigned char MP:1;
			unsigned char CKS:2;
		} BIT;
	} SMR;
	unsigned char  BRR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TIE:1;
			unsigned char RIE:1;
			unsigned char TE:1;
			unsigned char RE:1;
			unsigned char MPIE:1;
			unsigned char TEIE:1;
			unsigned char CKE:2;
		} BIT;
	} SCR;
	unsigned char  TDR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TDRE:1;
			unsigned char RDRF:1;
			unsigned char ORER:1;
			unsigned char FER:1;
			unsigned char PER:1;
			unsigned char TEND:1;
			unsigned char MPB:1;
			unsigned char MPBT:1;
		} BIT;
	} SSR;
	unsigned char  RDR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char BCP2:1;
			unsigned char :2;
			unsigned char CHR1:1;
			unsigned char SDIR:1;
			unsigned char SINV:1;
			unsigned char :1;
			unsigned char SMIF:1;
		} BIT;
	} SCMR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char RXDESEL:1;
			unsigned char BGDM:1;
			unsigned char NFEN:1;
			unsigned char ABCS:1;
			unsigned char :1;
			unsigned char BRME:1;
			unsigned char :1;
			unsigned char ACS0:1;
		} BIT;
	} SEMR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char NFCS:3;
		} BIT;
	} SNFR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IICDL:5;
			unsigned char :2;
			unsigned char IICM:1;
		} BIT;
	} SIMR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char IICACKT:1;
			unsigned char :3;
			unsigned char IICCSC:1;
			unsigned char IICINTM:1;
		} BIT;
	} SIMR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char IICSCLS:2;
			unsigned char IICSDAS:2;
			unsigned char IICSTIF:1;
			unsigned char IICSTPREQ:1;
			unsigned char IICRSTAREQ:1;
			unsigned char IICSTAREQ:1;
		} BIT;
	} SIMR3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char IICACKR:1;
		} BIT;
	} SISR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char CKPH:1;
			unsigned char CKPOL:1;
			unsigned char :1;
			unsigned char MFF:1;
			unsigned char :1;
			unsigned char MSS:1;
			unsigned char CTSE:1;
			unsigned char SSE:1;
		} BIT;
	} SPMR;
	union {
		unsigned short WORD;
		struct {
			unsigned char TDRH;
			unsigned char TDRL;
		} BYTE;
	} TDRHL;
	union {
		unsigned short WORD;
		struct {
			unsigned char RDRH;
			unsigned char RDRL;
		} BYTE;
	} RDRHL;
	unsigned char  MDDR;
} st_sci_t;

typedef struct st_smci {
	union {
		unsigned char BYTE;
		struct {
			unsigned char GM:1;
			unsigned char BLK:1;
			unsigned char PE:1;
			unsigned char PM:1;
			unsigned char BCP:2;
			unsigned char CKS:2;
		} BIT;
	} SMR;
	unsigned char  BRR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TIE:1;
			unsigned char RIE:1;
			unsigned char TE:1;
			unsigned char RE:1;
			unsigned char MPIE:1;
			unsigned char TEIE:1;
			unsigned char CKE:2;
		} BIT;
	} SCR;
	unsigned char  TDR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TDRE:1;
			unsigned char RDRF:1;
			unsigned char ORER:1;
			unsigned char ERS:1;
			unsigned char PER:1;
			unsigned char TEND:1;
			unsigned char MPB:1;
			unsigned char MPBT:1;
		} BIT;
	} SSR;
	unsigned char  RDR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char BCP2:1;
			unsigned char :2;
			unsigned char CHR1:1;
			unsigned char SDIR:1;
			unsigned char SINV:1;
			unsigned char :1;
			unsigned char SMIF:1;
		} BIT;
	} SCMR;
	char           wk0[7];
	union {
		unsigned short WORD;
		struct {
			unsigned char TDRH;
			unsigned char TDRL;
		} BYTE;
	} TDRHL;
	union {
		unsigned short WORD;
		struct {
			unsigned char RDRH;
			unsigned char RDRL;
		} BYTE;
	} RDRHL;
	unsigned char  MDDR;
} st_smci_t;

typedef struct st_system {
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short MD:1;
		} BIT;
	} MDMONR;
	char           wk0[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short RAME:1;
		} BIT;
	} SYSCR1;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short SSBY:1;
			unsigned short :15;
		} BIT;
	} SBYCR;
	char           wk2[2];
	union {
		unsigned long LONG;
		struct {
			unsigned long :3;
			unsigned long MSTPA28:1;
			unsigned long :4;
			unsigned long MSTPA23:1;
			unsigned long :3;
			unsigned long MSTPA19:1;
			unsigned long :1;
			unsigned long MSTPA17:1;
			unsigned long MSTPA16:1;
			unsigned long MSTPA15:1;
			unsigned long MSTPA14:1;
			unsigned long :4;
			unsigned long MSTPA9:1;
			unsigned long :1;
			unsigned long MSTPA7:1;
			unsigned long :1;
			unsigned long MSTPA5:1;
			unsigned long MSTPA4:1;
			unsigned long MSTPA3:1;
			unsigned long MSTPA2:1;
			unsigned long :2;
		} BIT;
	} MSTPCRA;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long MSTPB30:1;
			unsigned long :3;
			unsigned long MSTPB26:1;
			unsigned long MSTPB25:1;
			unsigned long :1;
			unsigned long MSTPB23:1;
			unsigned long :1;
			unsigned long MSTPB21:1;
			unsigned long :3;
			unsigned long MSTPB17:1;
			unsigned long :6;
			unsigned long MSTPB10:1;
			unsigned long :3;
			unsigned long MSTPB6:1;
			unsigned long :5;
			unsigned long MSTPB0:1;
		} BIT;
	} MSTPCRB;
	union {
		unsigned long LONG;
		struct {
			unsigned long DSLPE:1;
			unsigned long :3;
			unsigned long MSTPC27:1;
			unsigned long MSTPC26:1;
			unsigned long :1;
			unsigned long MSTPC24:1;
			unsigned long :4;
			unsigned long MSTPC19:1;
			unsigned long :18;
			unsigned long MSTPC0:1;
		} BIT;
	} MSTPCRC;
	char           wk3[4];
	union {
		unsigned long LONG;
		struct {
			unsigned long FCK:4;
			unsigned long ICK:4;
			unsigned long :8;
			unsigned long PCKA:4;
			unsigned long PCKB:4;
			unsigned long :4;
			unsigned long PCKD:4;
		} BIT;
	} SCKCR;
	char           wk4[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short CKSEL:3;
			unsigned short :8;
		} BIT;
	} SCKCR3;
	union {
		unsigned short WORD;
		struct {
			unsigned short :2;
			unsigned short STC:6;
			unsigned short :5;
			unsigned short PLLSRCSEL:1;
			unsigned short PLIDIV:2;
		} BIT;
	} PLLCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char PLLEN:1;
		} BIT;
	} PLLCR2;
	char           wk5[6];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char MEMWAIT:2;
		} BIT;
	} MEMWAIT;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char MOSTP:1;
		} BIT;
	} MOSCCR;
	char           wk6[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char LCSTP:1;
		} BIT;
	} LOCOCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char ILCSTP:1;
		} BIT;
	} ILOCOCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char HCSTP:1;
		} BIT;
	} HOCOCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char HCFRQ:2;
		} BIT;
	} HOCOCR2;
	char           wk7[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char HCOVF:1;
			unsigned char PLOVF:1;
			unsigned char :1;
			unsigned char MOOVF:1;
		} BIT;
	} OSCOVFSR;
	char           wk8[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char OSTDE:1;
			unsigned char :6;
			unsigned char OSTDIE:1;
		} BIT;
	} OSTDCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char OSTDF:1;
		} BIT;
	} OSTDSR;
	char           wk9[94];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char OPCMTSF:1;
			unsigned char :1;
			unsigned char OPCM:3;
		} BIT;
	} OPCCR;
	char           wk10[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char MSTS:5;
		} BIT;
	} MOSCWTCR;
	char           wk11[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char HSTS:3;
		} BIT;
	} HOCOWTCR;
	char           wk12[26];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SWRF:1;
			unsigned char :1;
			unsigned char IWDTRF:1;
		} BIT;
	} RSTSR2;
	char           wk13[1];
	unsigned short SWRR;
	char           wk14[28];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char LVD1IRQSEL:1;
			unsigned char LVD1IDTSEL:2;
		} BIT;
	} LVD1CR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char LVD1MON:1;
			unsigned char LVD1DET:1;
		} BIT;
	} LVD1SR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char LVD2IRQSEL:1;
			unsigned char LVD2IDTSEL:2;
		} BIT;
	} LVD2CR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char LVD2MON:1;
			unsigned char LVD2DET:1;
		} BIT;
	} LVD2SR;
	char           wk15[794];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRKEY:8;
			unsigned short :4;
			unsigned short PRC3:1;
			unsigned short PRC2:1;
			unsigned short PRC1:1;
			unsigned short PRC0:1;
		} BIT;
	} PRCR;
	char           wk16[48784];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char LVD2RF:1;
			unsigned char LVD1RF:1;
			unsigned char LVD0RF:1;
			unsigned char PORF:1;
		} BIT;
	} RSTSR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CWSF:1;
		} BIT;
	} RSTSR1;
	char           wk17[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char MOSEL:1;
			unsigned char MODRV21:1;
			unsigned char :5;
		} BIT;
	} MOFCR;
	char           wk18[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char LVD2E:1;
			unsigned char LVD1E:1;
			unsigned char :5;
		} BIT;
	} LVCMPCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char LVD2LVL:2;
			unsigned char LVD1LVL:4;
		} BIT;
	} LVDLVLR;
	char           wk19[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char LVD1RN:1;
			unsigned char LVD1RI:1;
			unsigned char :3;
			unsigned char LVD1CMPE:1;
			unsigned char :1;
			unsigned char LVD1RIE:1;
		} BIT;
	} LVD1CR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char LVD2RN:1;
			unsigned char LVD2RI:1;
			unsigned char :3;
			unsigned char LVD2CMPE:1;
			unsigned char :1;
			unsigned char LVD2RIE:1;
		} BIT;
	} LVD2CR0;
} st_system_t;

typedef struct st_tmr0 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char CMIEB:1;
			unsigned char CMIEA:1;
			unsigned char OVIE:1;
			unsigned char CCLR:2;
			unsigned char :3;
		} BIT;
	} TCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char ADTE:1;
			unsigned char OSB:2;
			unsigned char OSA:2;
		} BIT;
	} TCSR;
	char           wk1[1];
	unsigned char  TCORA;
	char           wk2[1];
	unsigned char  TCORB;
	char           wk3[1];
	unsigned char  TCNT;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TMRIS:1;
			unsigned char :2;
			unsigned char CSS:2;
			unsigned char CKS:3;
		} BIT;
	} TCCR;
} st_tmr0_t;

typedef struct st_tmr1 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char CMIEB:1;
			unsigned char CMIEA:1;
			unsigned char OVIE:1;
			unsigned char CCLR:2;
			unsigned char :3;
		} BIT;
	} TCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char OSB:2;
			unsigned char OSA:2;
		} BIT;
	} TCSR;
	char           wk1[1];
	unsigned char  TCORA;
	char           wk2[1];
	unsigned char  TCORB;
	char           wk3[1];
	unsigned char  TCNT;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TMRIS:1;
			unsigned char :2;
			unsigned char CSS:2;
			unsigned char CKS:3;
		} BIT;
	} TCCR;
} st_tmr1_t;

typedef struct st_tmr01 {
	unsigned short TCORA;
	unsigned short TCORB;
	unsigned short TCNT;
	unsigned short TCCR;
} st_tmr01_t;

#pragma bit_order
#pragma packoption

#endif
