/* IAR file generator: v1.0 */

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
/********************************************************************************/
/*                                                                              */
/* Device     : RX/RX600/RX66T                                                  */
/* File Name  : iodefine.h                                                      */
/* Abstract   : Definition of I/O Register.                                     */
/* History    : V0.50 (2017-12-21)  [Hardware Manual Revision : 0.50]           */
/* History    : V0.50a(2018-03-31)  [Hardware Manual Revision : 0.50]           */
/* History    : V0.50b(2018-04-23)  [Hardware Manual Revision : 0.50]           */
/* History    : V0.50c(2018-06-27)  [Hardware Manual Revision : 0.50]           */
/* History    : V1.00 (2018-07-23)  [Hardware Manual Revision : 1.00]           */
/* Note       : This is a typical example.                                      */
/*                                                                              */
/*  Copyright(c) 2018 Renesas Electronics Corp.                                 */
/*                  And Renesas Solutions Corp. ,All Rights Reserved.           */
/*                                                                              */
/********************************************************************************/
/*                                                                              */
/*  DESCRIPTION : Definition of ICU Register                                    */
/*  CPU TYPE    : RX66T                                                         */
/*                                                                              */
/*  Usage : IR,DTCER,IER,IPR of ICU Register                                    */
/*     The following IR, DTCE, IEN, IPR macro functions simplify usage.         */
/*     The bit access operation is "Bit_Name(interrupt source,name)".           */
/*     A part of the name can be omitted.                                       */
/*     for example :                                                            */
/*       IR(BSC,BUSERR) = 0;     expands to :                                   */
/*         ICU.IR[16].BIT.IR = 0;                                               */
/*                                                                              */
/*       DTCE(ICU,IRQ0) = 1;     expands to :                                   */
/*         ICU.DTCER[64].BIT.DTCE = 1;                                          */
/*                                                                              */
/*       IEN(CMT0,CMI0) = 1;     expands to :                                   */
/*         ICU.IER[0x03].BIT.IEN4 = 1;                                          */
/*                                                                              */
/*       IPR(ICU,SWINT2) = 2;    expands to :                                   */
/*       IPR(ICU,SWI   ) = 2;    // SWINT2,SWINT share IPR level.               */
/*         ICU.IPR[3].BIT.IPR = 2;                                              */
/*                                                                              */
/*  Usage : #pragma interrupt Function_Identifier(vect=**)                      */
/*     The number of vector is "(interrupt source, name)".                      */
/*     for example :                                                            */
/*       #pragma interrupt INT_IRQ0(vect=VECT(ICU,IRQ0))          expands to :  */
/*         #pragma interrupt INT_IRQ0(vect=64)                                  */
/*       #pragma interrupt INT_CMT0_CMI0(vect=VECT(CMT0,CMI0))    expands to :  */
/*         #pragma interrupt INT_CMT0_CMI0(vect=28)                             */
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
#ifndef __RX66TIODEFINE_HEADER__
#define __RX66TIODEFINE_HEADER__

#ifdef __IAR_SYSTEMS_ICC__
#pragma language=save
#pragma language=extended
#ifndef _SYSTEM_BUILD
#pragma system_include
#endif
#endif

#ifdef __IAR_SYSTEMS_ICC__
#define __evenaccess
#else
#define __sfr
#endif



#define	IEN_BSC_BUSERR		IEN0
#define	IEN_RAM_RAMERR		IEN2
#define	IEN_FCU_FIFERR		IEN5
#define	IEN_FCU_FRDYI		IEN7
#define	IEN_ICU_SWINT2		IEN2
#define	IEN_ICU_SWINT		IEN3
#define	IEN_CMT0_CMI0		IEN4
#define	IEN_CMT1_CMI1		IEN5
#define	IEN_CMT2_CMI2		IEN6
#define	IEN_CMT3_CMI3		IEN7
#define	IEN_USB0_D0FIFO0	IEN2
#define	IEN_USB0_D1FIFO0	IEN3
#define	IEN_RSPI0_SPRI0		IEN6
#define	IEN_RSPI0_SPTI0		IEN7
#define	IEN_RIIC0_RXI0		IEN4
#define	IEN_RIIC0_TXI0		IEN5
#define	IEN_SCI1_RXI1		IEN4
#define	IEN_SCI1_TXI1		IEN5
#define	IEN_ICU_IRQ0		IEN0
#define	IEN_ICU_IRQ1		IEN1
#define	IEN_ICU_IRQ2		IEN2
#define	IEN_ICU_IRQ3		IEN3
#define	IEN_ICU_IRQ4		IEN4
#define	IEN_ICU_IRQ5		IEN5
#define	IEN_ICU_IRQ6		IEN6
#define	IEN_ICU_IRQ7		IEN7
#define	IEN_ICU_IRQ8		IEN0
#define	IEN_ICU_IRQ9		IEN1
#define	IEN_ICU_IRQ10		IEN2
#define	IEN_ICU_IRQ11		IEN3
#define	IEN_ICU_IRQ12		IEN4
#define	IEN_ICU_IRQ13		IEN5
#define	IEN_ICU_IRQ14		IEN6
#define	IEN_ICU_IRQ15		IEN7
#define	IEN_SCI5_RXI5		IEN4
#define	IEN_SCI5_TXI5		IEN5
#define	IEN_SCI6_RXI6		IEN6
#define	IEN_SCI6_TXI6		IEN7
#define	IEN_LVD1_LVD1		IEN0
#define	IEN_LVD2_LVD2		IEN1
#define	IEN_USB0_USBR0		IEN2
#define	IEN_IWDT_IWUNI		IEN7
#define	IEN_WDT_WUNI		IEN0
#define	IEN_SCI8_RXI8		IEN4
#define	IEN_SCI8_TXI8		IEN5
#define	IEN_SCI9_RXI9		IEN6
#define	IEN_SCI9_TXI9		IEN7
#define	IEN_ICU_GROUPBE0	IEN2
#define	IEN_ICU_GROUPBL0	IEN6
#define	IEN_ICU_GROUPBL1	IEN7
#define	IEN_ICU_GROUPAL0	IEN0
#define	IEN_SCI11_RXI11		IEN2
#define	IEN_SCI11_TXI11		IEN3
#define	IEN_SCI12_RXI12		IEN4
#define	IEN_SCI12_TXI12		IEN5
#define	IEN_DMAC_DMAC0I		IEN0
#define	IEN_DMAC_DMAC1I		IEN1
#define	IEN_DMAC_DMAC2I		IEN2
#define	IEN_DMAC_DMAC3I		IEN3
#define	IEN_DMAC_DMAC74I	IEN4
#define	IEN_OST_OSTDI		IEN5
#define	IEN_S12AD_S12ADI	IEN0
#define	IEN_S12AD_S12GBADI	IEN1
#define	IEN_S12AD_S12GCADI	IEN2
#define	IEN_S12AD1_S12ADI1	IEN4
#define	IEN_S12AD1_S12GBADI1	IEN5
#define	IEN_S12AD1_S12GCADI1	IEN6
#define	IEN_S12AD2_S12ADI2	IEN0
#define	IEN_S12AD2_S12GBADI2	IEN1
#define	IEN_S12AD2_S12GCADI2	IEN2
#define	IEN_TMR0_CMIA0		IEN2
#define	IEN_TMR0_CMIB0		IEN3
#define	IEN_TMR0_OVI0		IEN4
#define	IEN_TMR1_CMIA1		IEN5
#define	IEN_TMR1_CMIB1		IEN6
#define	IEN_TMR1_OVI1		IEN7
#define	IEN_TMR2_CMIA2		IEN0
#define	IEN_TMR2_CMIB2		IEN1
#define	IEN_TMR2_OVI2		IEN2
#define	IEN_TMR3_CMIA3		IEN3
#define	IEN_TMR3_CMIB3		IEN4
#define	IEN_TMR3_OVI3		IEN5
#define	IEN_TMR4_CMIA4		IEN6
#define	IEN_TMR4_CMIB4		IEN7
#define	IEN_TMR4_OVI4		IEN0
#define	IEN_TMR5_CMIA5		IEN1
#define	IEN_TMR5_CMIB5		IEN2
#define	IEN_TMR5_OVI5		IEN3
#define	IEN_TMR6_CMIA6		IEN4
#define	IEN_TMR6_CMIB6		IEN5
#define	IEN_TMR6_OVI6		IEN6
#define	IEN_TMR7_CMIA7		IEN7
#define	IEN_TMR7_CMIB7		IEN0
#define	IEN_TMR7_OVI7		IEN1
#define	IEN_CAN0_RXF0		IEN2
#define	IEN_CAN0_TXF0		IEN3
#define	IEN_CAN0_RXM0		IEN4
#define	IEN_CAN0_TXM0		IEN5
#define	IEN_USB0_USBI0		IEN6
#define	IEN_ELC_ELSR18I		IEN7
#define	IEN_ELC_ELSR19I		IEN0
#define	IEN_TSIP_RD			IEN1
#define	IEN_TSIP_WR			IEN2
#define	IEN_TSIP_ERR		IEN3
#define	IEN_CMPC0_CMPC0		IEN4
#define	IEN_CMPC1_CMPC1		IEN5
#define	IEN_CMPC2_CMPC2		IEN6
#define	IEN_CMPC3_CMPC3		IEN7
#define	IEN_CMPC4_CMPC4		IEN0
#define	IEN_CMPC5_CMPC5		IEN1
#define	IEN_PERIA_INTA208	IEN0
#define	IEN_PERIA_INTA209	IEN1
#define	IEN_PERIA_INTA210	IEN2
#define	IEN_PERIA_INTA211	IEN3
#define	IEN_PERIA_INTA212	IEN4
#define	IEN_PERIA_INTA213	IEN5
#define	IEN_PERIA_INTA214	IEN6
#define	IEN_PERIA_INTA215	IEN7
#define	IEN_PERIA_INTA216	IEN0
#define	IEN_PERIA_INTA217	IEN1
#define	IEN_PERIA_INTA218	IEN2
#define	IEN_PERIA_INTA219	IEN3
#define	IEN_PERIA_INTA220	IEN4
#define	IEN_PERIA_INTA221	IEN5
#define	IEN_PERIA_INTA222	IEN6
#define	IEN_PERIA_INTA223	IEN7
#define	IEN_PERIA_INTA224	IEN0
#define	IEN_PERIA_INTA225	IEN1
#define	IEN_PERIA_INTA226	IEN2
#define	IEN_PERIA_INTA227	IEN3
#define	IEN_PERIA_INTA228	IEN4
#define	IEN_PERIA_INTA229	IEN5
#define	IEN_PERIA_INTA230	IEN6
#define	IEN_PERIA_INTA231	IEN7
#define	IEN_PERIA_INTA232	IEN0
#define	IEN_PERIA_INTA233	IEN1
#define	IEN_PERIA_INTA234	IEN2
#define	IEN_PERIA_INTA235	IEN3
#define	IEN_PERIA_INTA236	IEN4
#define	IEN_PERIA_INTA237	IEN5
#define	IEN_PERIA_INTA238	IEN6
#define	IEN_PERIA_INTA239	IEN7
#define	IEN_PERIA_INTA240	IEN0
#define	IEN_PERIA_INTA241	IEN1
#define	IEN_PERIA_INTA242	IEN2
#define	IEN_PERIA_INTA243	IEN3
#define	IEN_PERIA_INTA244	IEN4
#define	IEN_PERIA_INTA245	IEN5
#define	IEN_PERIA_INTA246	IEN6
#define	IEN_PERIA_INTA247	IEN7
#define	IEN_PERIA_INTA248	IEN0
#define	IEN_PERIA_INTA249	IEN1
#define	IEN_PERIA_INTA250	IEN2
#define	IEN_PERIA_INTA251	IEN3
#define	IEN_PERIA_INTA252	IEN4
#define	IEN_PERIA_INTA253	IEN5
#define	IEN_PERIA_INTA254	IEN6
#define	IEN_PERIA_INTA255	IEN7
#define	VECT_BSC_BUSERR		16
#define	VECT_RAM_RAMERR		18
#define	VECT_FCU_FIFERR		21
#define	VECT_FCU_FRDYI		23
#define	VECT_ICU_SWINT2		26
#define	VECT_ICU_SWINT		27
#define	VECT_CMT0_CMI0		28
#define	VECT_CMT1_CMI1		29
#define	VECT_CMT2_CMI2		30
#define	VECT_CMT3_CMI3		31
#define	VECT_USB0_D0FIFO0	34
#define	VECT_USB0_D1FIFO0	35
#define	VECT_RSPI0_SPRI0	38
#define	VECT_RSPI0_SPTI0	39
#define	VECT_RIIC0_RXI0		52
#define	VECT_RIIC0_TXI0		53
#define	VECT_SCI1_RXI1		60
#define	VECT_SCI1_TXI1		61
#define	VECT_ICU_IRQ0		64
#define	VECT_ICU_IRQ1		65
#define	VECT_ICU_IRQ2		66
#define	VECT_ICU_IRQ3		67
#define	VECT_ICU_IRQ4		68
#define	VECT_ICU_IRQ5		69
#define	VECT_ICU_IRQ6		70
#define	VECT_ICU_IRQ7		71
#define	VECT_ICU_IRQ8		72
#define	VECT_ICU_IRQ9		73
#define	VECT_ICU_IRQ10		74
#define	VECT_ICU_IRQ11		75
#define	VECT_ICU_IRQ12		76
#define	VECT_ICU_IRQ13		77
#define	VECT_ICU_IRQ14		78
#define	VECT_ICU_IRQ15		79
#define	VECT_SCI5_RXI5		84
#define	VECT_SCI5_TXI5		85
#define	VECT_SCI6_RXI6		86
#define	VECT_SCI6_TXI6		87
#define	VECT_LVD1_LVD1		88
#define	VECT_LVD2_LVD2		89
#define	VECT_USB0_USBR0		90
#define	VECT_IWDT_IWUNI		95
#define	VECT_WDT_WUNI		96
#define	VECT_SCI8_RXI8		100
#define	VECT_SCI8_TXI8		101
#define	VECT_SCI9_RXI9		102
#define	VECT_SCI9_TXI9		103
#define	VECT_ICU_GROUPBE0	106
#define	VECT_ICU_GROUPBL0	110
#define	VECT_ICU_GROUPBL1	111
#define	VECT_ICU_GROUPAL0	112
#define	VECT_SCI11_RXI11	114
#define	VECT_SCI11_TXI11	115
#define	VECT_SCI12_RXI12	116
#define	VECT_SCI12_TXI12	117
#define	VECT_DMAC_DMAC0I	120
#define	VECT_DMAC_DMAC1I	121
#define	VECT_DMAC_DMAC2I	122
#define	VECT_DMAC_DMAC3I	123
#define	VECT_DMAC_DMAC74I	124
#define	VECT_OST_OSTDI		125
#define	VECT_S12AD_S12ADI	128
#define	VECT_S12AD_S12GBADI	129
#define	VECT_S12AD_S12GCADI	130
#define	VECT_S12AD1_S12ADI1	132
#define	VECT_S12AD1_S12GBADI1	133
#define	VECT_S12AD1_S12GCADI1	134
#define	VECT_S12AD2_S12ADI2	136
#define	VECT_S12AD2_S12GBADI2	137
#define	VECT_S12AD2_S12GCADI2	138
#define	VECT_TMR0_CMIA0		146
#define	VECT_TMR0_CMIB0		147
#define	VECT_TMR0_OVI0		148
#define	VECT_TMR1_CMIA1		149
#define	VECT_TMR1_CMIB1		150
#define	VECT_TMR1_OVI1		151
#define	VECT_TMR2_CMIA2		152
#define	VECT_TMR2_CMIB2		153
#define	VECT_TMR2_OVI2		154
#define	VECT_TMR3_CMIA3		155
#define	VECT_TMR3_CMIB3		156
#define	VECT_TMR3_OVI3		157
#define	VECT_TMR4_CMIA4		158
#define	VECT_TMR4_CMIB4		159
#define	VECT_TMR4_OVI4		160
#define	VECT_TMR5_CMIA5		161
#define	VECT_TMR5_CMIB5		162
#define	VECT_TMR5_OVI5		163
#define	VECT_TMR6_CMIA6		164
#define	VECT_TMR6_CMIB6		165
#define	VECT_TMR6_OVI6		166
#define	VECT_TMR7_CMIA7		167
#define	VECT_TMR7_CMIB7		168
#define	VECT_TMR7_OVI7		169
#define	VECT_CAN0_RXF0		170
#define	VECT_CAN0_TXF0		171
#define	VECT_CAN0_RXM0		172
#define	VECT_CAN0_TXM0		173
#define	VECT_USB0_USBI0		174
#define	VECT_ELC_ELSR18I	175
#define	VECT_ELC_ELSR19I	176
#define	VECT_TSIP_RD		177
#define	VECT_TSIP_WR		178
#define	VECT_TSIP_ERR		179
#define	VECT_CMPC0_CMPC0	180
#define	VECT_CMPC1_CMPC1	181
#define	VECT_CMPC2_CMPC2	182
#define	VECT_CMPC3_CMPC3	183
#define	VECT_CMPC4_CMPC4	184
#define	VECT_CMPC5_CMPC5	185
#define	VECT_PERIA_INTA208	208
#define	VECT_PERIA_INTA209	209
#define	VECT_PERIA_INTA210	210
#define	VECT_PERIA_INTA211	211
#define	VECT_PERIA_INTA212	212
#define	VECT_PERIA_INTA213	213
#define	VECT_PERIA_INTA214	214
#define	VECT_PERIA_INTA215	215
#define	VECT_PERIA_INTA216	216
#define	VECT_PERIA_INTA217	217
#define	VECT_PERIA_INTA218	218
#define	VECT_PERIA_INTA219	219
#define	VECT_PERIA_INTA220	220
#define	VECT_PERIA_INTA221	221
#define	VECT_PERIA_INTA222	222
#define	VECT_PERIA_INTA223	223
#define	VECT_PERIA_INTA224	224
#define	VECT_PERIA_INTA225	225
#define	VECT_PERIA_INTA226	226
#define	VECT_PERIA_INTA227	227
#define	VECT_PERIA_INTA228	228
#define	VECT_PERIA_INTA229	229
#define	VECT_PERIA_INTA230	230
#define	VECT_PERIA_INTA231	231
#define	VECT_PERIA_INTA232	232
#define	VECT_PERIA_INTA233	233
#define	VECT_PERIA_INTA234	234
#define	VECT_PERIA_INTA235	235
#define	VECT_PERIA_INTA236	236
#define	VECT_PERIA_INTA237	237
#define	VECT_PERIA_INTA238	238
#define	VECT_PERIA_INTA239	239
#define	VECT_PERIA_INTA240	240
#define	VECT_PERIA_INTA241	241
#define	VECT_PERIA_INTA242	242
#define	VECT_PERIA_INTA243	243
#define	VECT_PERIA_INTA244	244
#define	VECT_PERIA_INTA245	245
#define	VECT_PERIA_INTA246	246
#define	VECT_PERIA_INTA247	247
#define	VECT_PERIA_INTA248	248
#define	VECT_PERIA_INTA249	249
#define	VECT_PERIA_INTA250	250
#define	VECT_PERIA_INTA251	251
#define	VECT_PERIA_INTA252	252
#define	VECT_PERIA_INTA253	253
#define	VECT_PERIA_INTA254	254
#define	VECT_PERIA_INTA255	255
#define	MSTP_DMAC	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC0	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC1	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC2	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC3	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC4	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC5	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC6	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC7	SYSTEM.MSTPCRA.BIT.MSTPA28
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
#define	MSTP_GPTW	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW0	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW1	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW2	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW3	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW4	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW5	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW6	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW7	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW8	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_GPTW9	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_HRPWM	SYSTEM.MSTPCRA.BIT.MSTPA7
#define	MSTP_POEG	SYSTEM.MSTPCRA.BIT.MSTPA7
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
#define	MSTP_USB0	SYSTEM.MSTPCRB.BIT.MSTPB19
#define	MSTP_RSPI0	SYSTEM.MSTPCRB.BIT.MSTPB17
#define	MSTP_CMPC	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC0	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC1	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC2	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC3	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC4	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_CMPC5	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_ELC	SYSTEM.MSTPCRB.BIT.MSTPB9
#define	MSTP_DOC	SYSTEM.MSTPCRB.BIT.MSTPB6
#define	MSTP_SCI12	SYSTEM.MSTPCRB.BIT.MSTPB4
#define	MSTP_SMCI12	SYSTEM.MSTPCRB.BIT.MSTPB4
#define	MSTP_CAN0	SYSTEM.MSTPCRB.BIT.MSTPB0
#define	MSTP_SCI8	SYSTEM.MSTPCRC.BIT.MSTPC27
#define	MSTP_SMCI8	SYSTEM.MSTPCRC.BIT.MSTPC27
#define	MSTP_SCI9	SYSTEM.MSTPCRC.BIT.MSTPC26
#define	MSTP_SMCI9	SYSTEM.MSTPCRC.BIT.MSTPC26
#define	MSTP_SCI11	SYSTEM.MSTPCRC.BIT.MSTPC24
#define	MSTP_SMCI11	SYSTEM.MSTPCRC.BIT.MSTPC24
#define	MSTP_CAC	SYSTEM.MSTPCRC.BIT.MSTPC19
#define	MSTP_ECCRAM	SYSTEM.MSTPCRC.BIT.MSTPC6
#define	MSTP_RAM	SYSTEM.MSTPCRC.BIT.MSTPC0
#define	IS_CAN0_ERS0		IS0
#define	IS_SCI1_TEI1		IS2
#define	IS_SCI1_ERI1		IS3
#define	IS_SCI5_TEI5		IS10
#define	IS_SCI5_ERI5		IS11
#define	IS_SCI6_TEI6		IS12
#define	IS_SCI6_ERI6		IS13
#define	IS_SCI12_TEI12		IS16
#define	IS_SCI12_ERI12		IS17
#define	IS_SCI12_SCIX0		IS18
#define	IS_SCI12_SCIX1		IS19
#define	IS_SCI12_SCIX2		IS20
#define	IS_SCI12_SCIX3		IS21
#define	IS_CAC_FERRI		IS26
#define	IS_CAC_MENDI		IS27
#define	IS_CAC_OVFI			IS28
#define	IS_DOC_DOPCI		IS29
#define	IS_POEG_POEGGAI		IS0
#define	IS_POEG_POEGGBI		IS1
#define	IS_POEG_POEGGCI		IS2
#define	IS_POEG_POEGGDI		IS3
#define	IS_POE3_OEI5		IS8
#define	IS_POE3_OEI1		IS9
#define	IS_POE3_OEI2		IS10
#define	IS_POE3_OEI3		IS11
#define	IS_POE3_OEI4		IS12
#define	IS_RIIC0_TEI0		IS13
#define	IS_RIIC0_EEI0		IS14
#define	IS_S12AD2_S12CMPAI2	IS18
#define	IS_S12AD2_S12CMPBI2	IS19
#define	IS_S12AD_S12CMPAI	IS20
#define	IS_S12AD_S12CMPBI	IS21
#define	IS_S12AD1_S12CMPAI1	IS22
#define	IS_S12AD1_S12CMPBI1	IS23
#define	IS_SCI8_TEI8		IS24
#define	IS_SCI8_ERI8		IS25
#define	IS_SCI9_TEI9		IS26
#define	IS_SCI9_ERI9		IS27
#define	IS_SCI11_TEI11		IS12
#define	IS_SCI11_ERI11		IS13
#define	IS_RSPI0_SPII0		IS16
#define	IS_RSPI0_SPEI0		IS17
#define	EN_CAN0_ERS0		EN0
#define	EN_SCI1_TEI1		EN2
#define	EN_SCI1_ERI1		EN3
#define	EN_SCI5_TEI5		EN10
#define	EN_SCI5_ERI5		EN11
#define	EN_SCI6_TEI6		EN12
#define	EN_SCI6_ERI6		EN13
#define	EN_SCI12_TEI12		EN16
#define	EN_SCI12_ERI12		EN17
#define	EN_SCI12_SCIX0		EN18
#define	EN_SCI12_SCIX1		EN19
#define	EN_SCI12_SCIX2		EN20
#define	EN_SCI12_SCIX3		EN21
#define	EN_CAC_FERRI		EN26
#define	EN_CAC_MENDI		EN27
#define	EN_CAC_OVFI			EN28
#define	EN_DOC_DOPCI		EN29
#define	EN_POEG_POEGGAI		EN0
#define	EN_POEG_POEGGBI		EN1
#define	EN_POEG_POEGGCI		EN2
#define	EN_POEG_POEGGDI		EN3
#define	EN_POE3_OEI5		EN8
#define	EN_POE3_OEI1		EN9
#define	EN_POE3_OEI2		EN10
#define	EN_POE3_OEI3		EN11
#define	EN_POE3_OEI4		EN12
#define	EN_RIIC0_TEI0		EN13
#define	EN_RIIC0_EEI0		EN14
#define	EN_S12AD2_S12CMPAI2	EN18
#define	EN_S12AD2_S12CMPBI2	EN19
#define	EN_S12AD_S12CMPAI	EN20
#define	EN_S12AD_S12CMPBI	EN21
#define	EN_S12AD1_S12CMPAI1	EN22
#define	EN_S12AD1_S12CMPBI1	EN23
#define	EN_SCI8_TEI8		EN24
#define	EN_SCI8_ERI8		EN25
#define	EN_SCI9_TEI9		EN26
#define	EN_SCI9_ERI9		EN27
#define	EN_SCI11_TEI11		EN12
#define	EN_SCI11_ERI11		EN13
#define	EN_RSPI0_SPII0		EN16
#define	EN_RSPI0_SPEI0		EN17
#define	CLR_CAN0_ERS0		CLR0
#define	GEN_CAN0_ERS0			GENBE0
#define	GEN_SCI1_TEI1			GENBL0
#define	GEN_SCI1_ERI1			GENBL0
#define	GEN_SCI5_TEI5			GENBL0
#define	GEN_SCI5_ERI5			GENBL0
#define	GEN_SCI6_TEI6			GENBL0
#define	GEN_SCI6_ERI6			GENBL0
#define	GEN_SCI12_TEI12			GENBL0
#define	GEN_SCI12_ERI12			GENBL0
#define	GEN_SCI12_SCIX0			GENBL0
#define	GEN_SCI12_SCIX1			GENBL0
#define	GEN_SCI12_SCIX2			GENBL0
#define	GEN_SCI12_SCIX3			GENBL0
#define	GEN_CAC_FERRI			GENBL0
#define	GEN_CAC_MENDI			GENBL0
#define	GEN_CAC_OVFI			GENBL0
#define	GEN_DOC_DOPCI			GENBL0
#define	GEN_POEG_POEGGAI		GENBL1
#define	GEN_POEG_POEGGBI		GENBL1
#define	GEN_POEG_POEGGCI		GENBL1
#define	GEN_POEG_POEGGDI		GENBL1
#define	GEN_POE3_OEI5			GENBL1
#define	GEN_POE3_OEI1			GENBL1
#define	GEN_POE3_OEI2			GENBL1
#define	GEN_POE3_OEI3			GENBL1
#define	GEN_POE3_OEI4			GENBL1
#define	GEN_RIIC0_TEI0			GENBL1
#define	GEN_RIIC0_EEI0			GENBL1
#define	GEN_S12AD2_S12CMPAI2	GENBL1
#define	GEN_S12AD2_S12CMPBI2	GENBL1
#define	GEN_S12AD_S12CMPAI		GENBL1
#define	GEN_S12AD_S12CMPBI		GENBL1
#define	GEN_S12AD1_S12CMPAI1	GENBL1
#define	GEN_S12AD1_S12CMPBI1	GENBL1
#define	GEN_SCI8_TEI8			GENBL1
#define	GEN_SCI8_ERI8			GENBL1
#define	GEN_SCI9_TEI9			GENBL1
#define	GEN_SCI9_ERI9			GENBL1
#define	GEN_SCI11_TEI11			GENAL0
#define	GEN_SCI11_ERI11			GENAL0
#define	GEN_RSPI0_SPII0			GENAL0
#define	GEN_RSPI0_SPEI0			GENAL0
#define	GRP_CAN0_ERS0			GRPBE0
#define	GRP_SCI1_TEI1			GRPBL0
#define	GRP_SCI1_ERI1			GRPBL0
#define	GRP_SCI5_TEI5			GRPBL0
#define	GRP_SCI5_ERI5			GRPBL0
#define	GRP_SCI6_TEI6			GRPBL0
#define	GRP_SCI6_ERI6			GRPBL0
#define	GRP_SCI12_TEI12			GRPBL0
#define	GRP_SCI12_ERI12			GRPBL0
#define	GRP_SCI12_SCIX0			GRPBL0
#define	GRP_SCI12_SCIX1			GRPBL0
#define	GRP_SCI12_SCIX2			GRPBL0
#define	GRP_SCI12_SCIX3			GRPBL0
#define	GRP_CAC_FERRI			GRPBL0
#define	GRP_CAC_MENDI			GRPBL0
#define	GRP_CAC_OVFI			GRPBL0
#define	GRP_DOC_DOPCI			GRPBL0
#define	GRP_POEG_POEGGAI		GRPBL1
#define	GRP_POEG_POEGGBI		GRPBL1
#define	GRP_POEG_POEGGCI		GRPBL1
#define	GRP_POEG_POEGGDI		GRPBL1
#define	GRP_POE3_OEI5			GRPBL1
#define	GRP_POE3_OEI1			GRPBL1
#define	GRP_POE3_OEI2			GRPBL1
#define	GRP_POE3_OEI3			GRPBL1
#define	GRP_POE3_OEI4			GRPBL1
#define	GRP_RIIC0_TEI0			GRPBL1
#define	GRP_RIIC0_EEI0			GRPBL1
#define	GRP_S12AD2_S12CMPAI2	GRPBL1
#define	GRP_S12AD2_S12CMPBI2	GRPBL1
#define	GRP_S12AD_S12CMPAI		GRPBL1
#define	GRP_S12AD_S12CMPBI		GRPBL1
#define	GRP_S12AD1_S12CMPAI1	GRPBL1
#define	GRP_S12AD1_S12CMPBI1	GRPBL1
#define	GRP_SCI8_TEI8			GRPBL1
#define	GRP_SCI8_ERI8			GRPBL1
#define	GRP_SCI9_TEI9			GRPBL1
#define	GRP_SCI9_ERI9			GRPBL1
#define	GRP_SCI11_TEI11			GRPAL0
#define	GRP_SCI11_ERI11			GRPAL0
#define	GRP_RSPI0_SPII0			GRPAL0
#define	GRP_RSPI0_SPEI0			GRPAL0
#define	GCR_CAN0_ERS0			GCRBE0
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
#define	__IS( x )		ICU.GRP ## x.BIT.IS ## x
#define	 _IS( x )		__IS( x )
#define	  IS( x , y )	_IS( _ ## x ## _ ## y )
#define	__EN( x )		ICU.GEN ## x.BIT.EN ## x
#define	 _EN( x )		__EN( x )
#define	  EN( x , y )	_EN( _ ## x ## _ ## y )
#define	__CLR( x )		ICU.GCR ## x.BIT.CLR ## x
#define	 _CLR( x )		__CLR( x )
#define	  CLR( x , y )	_CLR( _ ## x ## _ ## y )
#define	BSC		(*(volatile struct st_bsc    __sfr __evenaccess *)0x81300)
#define	CAC		(*(volatile struct st_cac    __sfr __evenaccess *)0x8B000)
#define	CAN0	(*(volatile struct st_can    __sfr __evenaccess *)0x90200)
#define	CMPC0	(*(volatile struct st_cmpc   __sfr __evenaccess *)0xA0C80)
#define	CMPC1	(*(volatile struct st_cmpc   __sfr __evenaccess *)0xA0CA0)
#define	CMPC2	(*(volatile struct st_cmpc   __sfr __evenaccess *)0xA0CC0)
#define	CMPC3	(*(volatile struct st_cmpc   __sfr __evenaccess *)0xA0CE0)
#define	CMPC4	(*(volatile struct st_cmpc   __sfr __evenaccess *)0xA0D00)
#define	CMPC5	(*(volatile struct st_cmpc   __sfr __evenaccess *)0xA0D20)
#define	CMT		(*(volatile struct st_cmt    __sfr __evenaccess *)0x88000)
#define	CMT0	(*(volatile struct st_cmt0   __sfr __evenaccess *)0x88002)
#define	CMT1	(*(volatile struct st_cmt0   __sfr __evenaccess *)0x88008)
#define	CMT2	(*(volatile struct st_cmt0   __sfr __evenaccess *)0x88012)
#define	CMT3	(*(volatile struct st_cmt0   __sfr __evenaccess *)0x88018)
#define	CRC		(*(volatile struct st_crc    __sfr __evenaccess *)0x88280)
#define	DA		(*(volatile struct st_da     __sfr __evenaccess *)0x88040)
#define	DMAC	(*(volatile struct st_dmac   __sfr __evenaccess *)0x82200)
#define	DMAC0	(*(volatile struct st_dmac0  __sfr __evenaccess *)0x82000)
#define	DMAC1	(*(volatile struct st_dmac1  __sfr __evenaccess *)0x82040)
#define	DMAC2	(*(volatile struct st_dmac1  __sfr __evenaccess *)0x82080)
#define	DMAC3	(*(volatile struct st_dmac1  __sfr __evenaccess *)0x820C0)
#define	DMAC4	(*(volatile struct st_dmac1  __sfr __evenaccess *)0x82100)
#define	DMAC5	(*(volatile struct st_dmac1  __sfr __evenaccess *)0x82140)
#define	DMAC6	(*(volatile struct st_dmac1  __sfr __evenaccess *)0x82180)
#define	DMAC7	(*(volatile struct st_dmac1  __sfr __evenaccess *)0x821C0)
#define	DOC		(*(volatile struct st_doc    __sfr __evenaccess *)0x8B080)
#define	DTC		(*(volatile struct st_dtc    __sfr __evenaccess *)0x82400)
#define	ELC		(*(volatile struct st_elc    __sfr __evenaccess *)0x8B100)
#define	FLASH	(*(volatile struct st_flash  __sfr __evenaccess *)0x81000)
#define	GPTW0	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2000)
#define	GPTW1	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2100)
#define	GPTW2	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2200)
#define	GPTW3	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2300)
#define	GPTW4	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2400)
#define	GPTW5	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2500)
#define	GPTW6	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2600)
#define	GPTW7	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2700)
#define	GPTW8	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2800)
#define	GPTW9	(*(volatile struct st_gptw   __sfr __evenaccess *)0xC2900)
#define	HRPWM	(*(volatile struct st_hrpwm  __sfr __evenaccess *)0xC2A00)
#define	ICU		(*(volatile struct st_icu    __sfr __evenaccess *)0x87000)
#define	IWDT	(*(volatile struct st_iwdt   __sfr __evenaccess *)0x88030)
#define	MPC		(*(volatile struct st_mpc    __sfr __evenaccess *)0x8C100)
#define	MPU		(*(volatile struct st_mpu    __sfr __evenaccess *)0x86400)
#define	MTU		(*(volatile struct st_mtu    __sfr __evenaccess *)0xC120A)
#define	MTU0	(*(volatile struct st_mtu0   __sfr __evenaccess *)0xC1290)
#define	MTU1	(*(volatile struct st_mtu1   __sfr __evenaccess *)0xC1290)
#define	MTU2	(*(volatile struct st_mtu2   __sfr __evenaccess *)0xC1292)
#define	MTU3	(*(volatile struct st_mtu3   __sfr __evenaccess *)0xC1200)
#define	MTU4	(*(volatile struct st_mtu4   __sfr __evenaccess *)0xC1200)
#define	MTU5	(*(volatile struct st_mtu5   __sfr __evenaccess *)0xC1A94)
#define	MTU6	(*(volatile struct st_mtu6   __sfr __evenaccess *)0xC1A00)
#define	MTU7	(*(volatile struct st_mtu7   __sfr __evenaccess *)0xC1A00)
#define	MTU9	(*(volatile struct st_mtu9   __sfr __evenaccess *)0xC1296)
#define	OFSM	(*(volatile struct st_ofsm   __sfr __evenaccess *)0x120040)
#define	POE		(*(volatile struct st_poe    __sfr __evenaccess *)0x9E400)
#define	POEG	(*(volatile struct st_poeg   __sfr __evenaccess *)0x9E000)
#define	PORT0	(*(volatile struct st_port0  __sfr __evenaccess *)0x8C000)
#define	PORT1	(*(volatile struct st_port1  __sfr __evenaccess *)0x8C001)
#define	PORT2	(*(volatile struct st_port2  __sfr __evenaccess *)0x8C002)
#define	PORT3	(*(volatile struct st_port3  __sfr __evenaccess *)0x8C003)
#define	PORT4	(*(volatile struct st_port4  __sfr __evenaccess *)0x8C004)
#define	PORT5	(*(volatile struct st_port5  __sfr __evenaccess *)0x8C005)
#define	PORT6	(*(volatile struct st_port6  __sfr __evenaccess *)0x8C006)
#define	PORT7	(*(volatile struct st_port7  __sfr __evenaccess *)0x8C007)
#define	PORT8	(*(volatile struct st_port8  __sfr __evenaccess *)0x8C008)
#define	PORT9	(*(volatile struct st_port9  __sfr __evenaccess *)0x8C009)
#define	PORTA	(*(volatile struct st_porta  __sfr __evenaccess *)0x8C00A)
#define	PORTB	(*(volatile struct st_portb  __sfr __evenaccess *)0x8C00B)
#define	PORTC	(*(volatile struct st_portc  __sfr __evenaccess *)0x8C00C)
#define	PORTD	(*(volatile struct st_portd  __sfr __evenaccess *)0x8C00D)
#define	PORTE	(*(volatile struct st_porte  __sfr __evenaccess *)0x8C00E)
#define	PORTF	(*(volatile struct st_portf  __sfr __evenaccess *)0x8C00F)
#define	PORTG	(*(volatile struct st_portg  __sfr __evenaccess *)0x8C010)
#define	PORTH	(*(volatile struct st_porth  __sfr __evenaccess *)0x8C011)
#define	PORTK	(*(volatile struct st_portk  __sfr __evenaccess *)0x8C013)
#define	RAM		(*(volatile struct st_ram    __sfr __evenaccess *)0x81200)
#define	RIIC0	(*(volatile struct st_riic   __sfr __evenaccess *)0x88300)
#define	RSPI0	(*(volatile struct st_rspi   __sfr __evenaccess *)0xD0100)
#define	S12AD	(*(volatile struct st_s12ad  __sfr __evenaccess *)0x89000)
#define	S12AD1	(*(volatile struct st_s12ad1 __sfr __evenaccess *)0x89200)
#define	S12AD2	(*(volatile struct st_s12ad2 __sfr __evenaccess *)0x89400)
#define	SCI1	(*(volatile struct st_sci1   __sfr __evenaccess *)0x8A020)
#define	SCI5	(*(volatile struct st_sci1   __sfr __evenaccess *)0x8A0A0)
#define	SCI6	(*(volatile struct st_sci1   __sfr __evenaccess *)0x8A0C0)
#define	SCI8	(*(volatile struct st_sci1   __sfr __evenaccess *)0x8A100)
#define	SCI9	(*(volatile struct st_sci1   __sfr __evenaccess *)0x8A120)
#define	SCI11	(*(volatile struct st_sci11  __sfr __evenaccess *)0xD0000)
#define	SCI12	(*(volatile struct st_sci12  __sfr __evenaccess *)0x8B300)
#define	SMCI1	(*(volatile struct st_smci   __sfr __evenaccess *)0x8A020)
#define	SMCI5	(*(volatile struct st_smci   __sfr __evenaccess *)0x8A0A0)
#define	SMCI6	(*(volatile struct st_smci   __sfr __evenaccess *)0x8A0C0)
#define	SMCI8	(*(volatile struct st_smci   __sfr __evenaccess *)0x8A100)
#define	SMCI9	(*(volatile struct st_smci   __sfr __evenaccess *)0x8A120)
#define	SMCI11	(*(volatile struct st_smci   __sfr __evenaccess *)0xD0000)
#define	SMCI12	(*(volatile struct st_smci   __sfr __evenaccess *)0x8B300)
#define	SYSTEM	(*(volatile struct st_system __sfr __evenaccess *)0x80000)
#define	TEMPS	(*(volatile struct st_temps  __sfr __evenaccess *)0x7FB17C)
#define	TMR0	(*(volatile struct st_tmr0   __sfr __evenaccess *)0x88200)
#define	TMR1	(*(volatile struct st_tmr1   __sfr __evenaccess *)0x88201)
#define	TMR2	(*(volatile struct st_tmr0   __sfr __evenaccess *)0x88210)
#define	TMR3	(*(volatile struct st_tmr1   __sfr __evenaccess *)0x88211)
#define	TMR4	(*(volatile struct st_tmr4   __sfr __evenaccess *)0x88220)
#define	TMR5	(*(volatile struct st_tmr5   __sfr __evenaccess *)0x88221)
#define	TMR6	(*(volatile struct st_tmr4   __sfr __evenaccess *)0x88230)
#define	TMR7	(*(volatile struct st_tmr5   __sfr __evenaccess *)0x88231)
#define	TMR01	(*(volatile struct st_tmr01  __sfr __evenaccess *)0x88204)
#define	TMR23	(*(volatile struct st_tmr01  __sfr __evenaccess *)0x88214)
#define	TMR45	(*(volatile struct st_tmr01  __sfr __evenaccess *)0x88224)
#define	TMR67	(*(volatile struct st_tmr01  __sfr __evenaccess *)0x88234)
#define	USB0	(*(volatile struct st_usb    __sfr __evenaccess *)0xA0000)
#define	WDT		(*(volatile struct st_wdt    __sfr __evenaccess *)0x88020)

typedef enum enum_ir {
IR_BSC_BUSERR=16,IR_RAM_RAMERR=18,
IR_FCU_FIFERR=21,IR_FCU_FRDYI=23,
IR_ICU_SWINT2=26,IR_ICU_SWINT,
IR_CMT0_CMI0,
IR_CMT1_CMI1,
IR_CMT2_CMI2,
IR_CMT3_CMI3,
IR_USB0_D0FIFO0=34,IR_USB0_D1FIFO0,
IR_RSPI0_SPRI0=38,IR_RSPI0_SPTI0,
IR_RIIC0_RXI0=52,IR_RIIC0_TXI0,
IR_SCI1_RXI1=60,IR_SCI1_TXI1,
IR_ICU_IRQ0=64,IR_ICU_IRQ1,IR_ICU_IRQ2,IR_ICU_IRQ3,IR_ICU_IRQ4,IR_ICU_IRQ5,IR_ICU_IRQ6,IR_ICU_IRQ7,IR_ICU_IRQ8,IR_ICU_IRQ9,IR_ICU_IRQ10,IR_ICU_IRQ11,IR_ICU_IRQ12,IR_ICU_IRQ13,IR_ICU_IRQ14,IR_ICU_IRQ15,
IR_SCI5_RXI5=84,IR_SCI5_TXI5,
IR_SCI6_RXI6,IR_SCI6_TXI6,
IR_LVD1_LVD1,
IR_LVD2_LVD2,
IR_USB0_USBR0,
IR_IWDT_IWUNI=95,
IR_WDT_WUNI,
IR_SCI8_RXI8=100,IR_SCI8_TXI8,
IR_SCI9_RXI9,IR_SCI9_TXI9,
IR_ICU_GROUPBE0=106,IR_ICU_GROUPBL0=110,IR_ICU_GROUPBL1,IR_ICU_GROUPAL0,
IR_SCI11_RXI11=114,IR_SCI11_TXI11,
IR_SCI12_RXI12,IR_SCI12_TXI12,
IR_DMAC_DMAC0I=120,IR_DMAC_DMAC1I,IR_DMAC_DMAC2I,IR_DMAC_DMAC3I,IR_DMAC_DMAC74I,
IR_OST_OSTDI,
IR_S12AD_S12ADI=128,IR_S12AD_S12GBADI,IR_S12AD_S12GCADI,
IR_S12AD1_S12ADI1=132,IR_S12AD1_S12GBADI1,IR_S12AD1_S12GCADI1,
IR_S12AD2_S12ADI2=136,IR_S12AD2_S12GBADI2,IR_S12AD2_S12GCADI2,
IR_TMR0_CMIA0=146,IR_TMR0_CMIB0,IR_TMR0_OVI0,
IR_TMR1_CMIA1,IR_TMR1_CMIB1,IR_TMR1_OVI1,
IR_TMR2_CMIA2,IR_TMR2_CMIB2,IR_TMR2_OVI2,
IR_TMR3_CMIA3,IR_TMR3_CMIB3,IR_TMR3_OVI3,
IR_TMR4_CMIA4,IR_TMR4_CMIB4,IR_TMR4_OVI4,
IR_TMR5_CMIA5,IR_TMR5_CMIB5,IR_TMR5_OVI5,
IR_TMR6_CMIA6,IR_TMR6_CMIB6,IR_TMR6_OVI6,
IR_TMR7_CMIA7,IR_TMR7_CMIB7,IR_TMR7_OVI7,
IR_CAN0_RXF0,IR_CAN0_TXF0,IR_CAN0_RXM0,IR_CAN0_TXM0,
IR_USB0_USBI0,
IR_ELC_ELSR18I,IR_ELC_ELSR19I,
IR_TSIP_RD,IR_TSIP_WR,IR_TSIP_ERR,
IR_CMPC0_CMPC0,
IR_CMPC1_CMPC1,
IR_CMPC2_CMPC2,
IR_CMPC3_CMPC3,
IR_CMPC4_CMPC4,
IR_CMPC5_CMPC5,
IR_PERIA_INTA208=208,IR_PERIA_INTA209,IR_PERIA_INTA210,IR_PERIA_INTA211,IR_PERIA_INTA212,IR_PERIA_INTA213,IR_PERIA_INTA214,IR_PERIA_INTA215,IR_PERIA_INTA216,IR_PERIA_INTA217,IR_PERIA_INTA218,IR_PERIA_INTA219,IR_PERIA_INTA220,IR_PERIA_INTA221,IR_PERIA_INTA222,IR_PERIA_INTA223,IR_PERIA_INTA224,IR_PERIA_INTA225,IR_PERIA_INTA226,IR_PERIA_INTA227,IR_PERIA_INTA228,IR_PERIA_INTA229,IR_PERIA_INTA230,IR_PERIA_INTA231,IR_PERIA_INTA232,IR_PERIA_INTA233,IR_PERIA_INTA234,IR_PERIA_INTA235,IR_PERIA_INTA236,IR_PERIA_INTA237,IR_PERIA_INTA238,IR_PERIA_INTA239,IR_PERIA_INTA240,IR_PERIA_INTA241,IR_PERIA_INTA242,IR_PERIA_INTA243,IR_PERIA_INTA244,IR_PERIA_INTA245,IR_PERIA_INTA246,IR_PERIA_INTA247,IR_PERIA_INTA248,IR_PERIA_INTA249,IR_PERIA_INTA250,IR_PERIA_INTA251,IR_PERIA_INTA252,IR_PERIA_INTA253,IR_PERIA_INTA254,IR_PERIA_INTA255
} enum_ir_t;

typedef enum enum_dtce {
DTCE_ICU_SWINT2=26,DTCE_ICU_SWINT,
DTCE_CMT0_CMI0,
DTCE_CMT1_CMI1,
DTCE_CMT2_CMI2,
DTCE_CMT3_CMI3,
DTCE_USB0_D0FIFO0=34,DTCE_USB0_D1FIFO0,
DTCE_RSPI0_SPRI0=38,DTCE_RSPI0_SPTI0,
DTCE_RIIC0_RXI0=52,DTCE_RIIC0_TXI0,
DTCE_SCI1_RXI1=60,DTCE_SCI1_TXI1,
DTCE_ICU_IRQ0=64,DTCE_ICU_IRQ1,DTCE_ICU_IRQ2,DTCE_ICU_IRQ3,DTCE_ICU_IRQ4,DTCE_ICU_IRQ5,DTCE_ICU_IRQ6,DTCE_ICU_IRQ7,DTCE_ICU_IRQ8,DTCE_ICU_IRQ9,DTCE_ICU_IRQ10,DTCE_ICU_IRQ11,DTCE_ICU_IRQ12,DTCE_ICU_IRQ13,DTCE_ICU_IRQ14,DTCE_ICU_IRQ15,
DTCE_SCI5_RXI5=84,DTCE_SCI5_TXI5,
DTCE_SCI6_RXI6,DTCE_SCI6_TXI6,
DTCE_SCI8_RXI8=100,DTCE_SCI8_TXI8,
DTCE_SCI9_RXI9,DTCE_SCI9_TXI9,
DTCE_SCI11_RXI11=114,DTCE_SCI11_TXI11,
DTCE_SCI12_RXI12,DTCE_SCI12_TXI12,
DTCE_DMAC_DMAC0I=120,DTCE_DMAC_DMAC1I,DTCE_DMAC_DMAC2I,DTCE_DMAC_DMAC3I,
DTCE_S12AD_S12ADI=128,DTCE_S12AD_S12GBADI,DTCE_S12AD_S12GCADI,
DTCE_S12AD1_S12ADI1=132,DTCE_S12AD1_S12GBADI1,DTCE_S12AD1_S12GCADI1,
DTCE_S12AD2_S12ADI2=136,DTCE_S12AD2_S12GBADI2,DTCE_S12AD2_S12GCADI2,
DTCE_TMR0_CMIA0=146,DTCE_TMR0_CMIB0,
DTCE_TMR1_CMIA1=149,DTCE_TMR1_CMIB1,
DTCE_TMR2_CMIA2=152,DTCE_TMR2_CMIB2,
DTCE_TMR3_CMIA3=155,DTCE_TMR3_CMIB3,
DTCE_TMR4_CMIA4=158,DTCE_TMR4_CMIB4,
DTCE_TMR5_CMIA5=161,DTCE_TMR5_CMIB5,
DTCE_TMR6_CMIA6=164,DTCE_TMR6_CMIB6,
DTCE_TMR7_CMIA7=167,DTCE_TMR7_CMIB7,
DTCE_ELC_ELSR18I=175,DTCE_ELC_ELSR19I,
DTCE_TSIP_RD,DTCE_TSIP_WR,
DTCE_CMPC0_CMPC0=180,
DTCE_CMPC1_CMPC1,
DTCE_CMPC2_CMPC2,
DTCE_CMPC3_CMPC3,
DTCE_CMPC4_CMPC4,
DTCE_CMPC5_CMPC5,
DTCE_PERIA_INTA208=208,DTCE_PERIA_INTA209,DTCE_PERIA_INTA210,DTCE_PERIA_INTA211,DTCE_PERIA_INTA212,DTCE_PERIA_INTA213,DTCE_PERIA_INTA214,DTCE_PERIA_INTA215,DTCE_PERIA_INTA216,DTCE_PERIA_INTA217,DTCE_PERIA_INTA218,DTCE_PERIA_INTA219,DTCE_PERIA_INTA220,DTCE_PERIA_INTA221,DTCE_PERIA_INTA222,DTCE_PERIA_INTA223,DTCE_PERIA_INTA224,DTCE_PERIA_INTA225,DTCE_PERIA_INTA226,DTCE_PERIA_INTA227,DTCE_PERIA_INTA228,DTCE_PERIA_INTA229,DTCE_PERIA_INTA230,DTCE_PERIA_INTA231,DTCE_PERIA_INTA232,DTCE_PERIA_INTA233,DTCE_PERIA_INTA234,DTCE_PERIA_INTA235,DTCE_PERIA_INTA236,DTCE_PERIA_INTA237,DTCE_PERIA_INTA238,DTCE_PERIA_INTA239,DTCE_PERIA_INTA240,DTCE_PERIA_INTA241,DTCE_PERIA_INTA242,DTCE_PERIA_INTA243,DTCE_PERIA_INTA244,DTCE_PERIA_INTA245,DTCE_PERIA_INTA246,DTCE_PERIA_INTA247,DTCE_PERIA_INTA248,DTCE_PERIA_INTA249,DTCE_PERIA_INTA250,DTCE_PERIA_INTA251,DTCE_PERIA_INTA252,DTCE_PERIA_INTA253,DTCE_PERIA_INTA254,DTCE_PERIA_INTA255
} enum_dtce_t;

typedef enum enum_ier {
IER_BSC_BUSERR=0x02,
IER_RAM_RAMERR=0x02,
IER_FCU_FIFERR=0x02,IER_FCU_FRDYI=0x02,
IER_ICU_SWINT2=0x03,IER_ICU_SWINT=0x03,
IER_CMT0_CMI0=0x03,
IER_CMT1_CMI1=0x03,
IER_CMT2_CMI2=0x03,
IER_CMT3_CMI3=0x03,
IER_USB0_D0FIFO0=0x04,IER_USB0_D1FIFO0=0x04,
IER_RSPI0_SPRI0=0x04,IER_RSPI0_SPTI0=0x04,
IER_RIIC0_RXI0=0x06,IER_RIIC0_TXI0=0x06,
IER_SCI1_RXI1=0x07,IER_SCI1_TXI1=0x07,
IER_ICU_IRQ0=0x08,IER_ICU_IRQ1=0x08,IER_ICU_IRQ2=0x08,IER_ICU_IRQ3=0x08,IER_ICU_IRQ4=0x08,IER_ICU_IRQ5=0x08,IER_ICU_IRQ6=0x08,IER_ICU_IRQ7=0x08,IER_ICU_IRQ8=0x09,IER_ICU_IRQ9=0x09,IER_ICU_IRQ10=0x09,IER_ICU_IRQ11=0x09,IER_ICU_IRQ12=0x09,IER_ICU_IRQ13=0x09,IER_ICU_IRQ14=0x09,IER_ICU_IRQ15=0x09,
IER_SCI5_RXI5=0x0A,IER_SCI5_TXI5=0x0A,
IER_SCI6_RXI6=0x0A,IER_SCI6_TXI6=0x0A,
IER_LVD1_LVD1=0x0B,
IER_LVD2_LVD2=0x0B,
IER_USB0_USBR0=0x0B,
IER_IWDT_IWUNI=0x0B,
IER_WDT_WUNI=0x0C,
IER_SCI8_RXI8=0x0C,IER_SCI8_TXI8=0x0C,
IER_SCI9_RXI9=0x0C,IER_SCI9_TXI9=0x0C,
IER_ICU_GROUPBE0=0x0D,IER_ICU_GROUPBL0=0x0D,IER_ICU_GROUPBL1=0x0D,IER_ICU_GROUPAL0=0x0E,
IER_SCI11_RXI11=0x0E,IER_SCI11_TXI11=0x0E,
IER_SCI12_RXI12=0x0E,IER_SCI12_TXI12=0x0E,
IER_DMAC_DMAC0I=0x0F,IER_DMAC_DMAC1I=0x0F,IER_DMAC_DMAC2I=0x0F,IER_DMAC_DMAC3I=0x0F,IER_DMAC_DMAC74I=0x0F,
IER_OST_OSTDI=0x0F,
IER_S12AD_S12ADI=0x10,IER_S12AD_S12GBADI=0x10,IER_S12AD_S12GCADI=0x10,
IER_S12AD1_S12ADI1=0x10,IER_S12AD1_S12GBADI1=0x10,IER_S12AD1_S12GCADI1=0x10,
IER_S12AD2_S12ADI2=0x11,IER_S12AD2_S12GBADI2=0x11,IER_S12AD2_S12GCADI2=0x11,
IER_TMR0_CMIA0=0x12,IER_TMR0_CMIB0=0x12,IER_TMR0_OVI0=0x12,
IER_TMR1_CMIA1=0x12,IER_TMR1_CMIB1=0x12,IER_TMR1_OVI1=0x12,
IER_TMR2_CMIA2=0x13,IER_TMR2_CMIB2=0x13,IER_TMR2_OVI2=0x13,
IER_TMR3_CMIA3=0x13,IER_TMR3_CMIB3=0x13,IER_TMR3_OVI3=0x13,
IER_TMR4_CMIA4=0x13,IER_TMR4_CMIB4=0x13,IER_TMR4_OVI4=0x14,
IER_TMR5_CMIA5=0x14,IER_TMR5_CMIB5=0x14,IER_TMR5_OVI5=0x14,
IER_TMR6_CMIA6=0x14,IER_TMR6_CMIB6=0x14,IER_TMR6_OVI6=0x14,
IER_TMR7_CMIA7=0x14,IER_TMR7_CMIB7=0x15,IER_TMR7_OVI7=0x15,
IER_CAN0_RXF0=0x15,IER_CAN0_TXF0=0x15,IER_CAN0_RXM0=0x15,IER_CAN0_TXM0=0x15,
IER_USB0_USBI0=0x15,
IER_ELC_ELSR18I=0x15,IER_ELC_ELSR19I=0x16,
IER_TSIP_RD=0x16,IER_TSIP_WR=0x16,IER_TSIP_ERR=0x16,
IER_CMPC0_CMPC0=0x16,
IER_CMPC1_CMPC1=0x16,
IER_CMPC2_CMPC2=0x16,
IER_CMPC3_CMPC3=0x16,
IER_CMPC4_CMPC4=0x17,
IER_CMPC5_CMPC5=0x17,
IER_PERIA_INTA208=0x1A,IER_PERIA_INTA209=0x1A,IER_PERIA_INTA210=0x1A,IER_PERIA_INTA211=0x1A,IER_PERIA_INTA212=0x1A,IER_PERIA_INTA213=0x1A,IER_PERIA_INTA214=0x1A,IER_PERIA_INTA215=0x1A,IER_PERIA_INTA216=0x1B,IER_PERIA_INTA217=0x1B,IER_PERIA_INTA218=0x1B,IER_PERIA_INTA219=0x1B,IER_PERIA_INTA220=0x1B,IER_PERIA_INTA221=0x1B,IER_PERIA_INTA222=0x1B,IER_PERIA_INTA223=0x1B,IER_PERIA_INTA224=0x1C,IER_PERIA_INTA225=0x1C,IER_PERIA_INTA226=0x1C,IER_PERIA_INTA227=0x1C,IER_PERIA_INTA228=0x1C,IER_PERIA_INTA229=0x1C,IER_PERIA_INTA230=0x1C,IER_PERIA_INTA231=0x1C,IER_PERIA_INTA232=0x1D,IER_PERIA_INTA233=0x1D,IER_PERIA_INTA234=0x1D,IER_PERIA_INTA235=0x1D,IER_PERIA_INTA236=0x1D,IER_PERIA_INTA237=0x1D,IER_PERIA_INTA238=0x1D,IER_PERIA_INTA239=0x1D,IER_PERIA_INTA240=0x1E,IER_PERIA_INTA241=0x1E,IER_PERIA_INTA242=0x1E,IER_PERIA_INTA243=0x1E,IER_PERIA_INTA244=0x1E,IER_PERIA_INTA245=0x1E,IER_PERIA_INTA246=0x1E,IER_PERIA_INTA247=0x1E,IER_PERIA_INTA248=0x1F,IER_PERIA_INTA249=0x1F,IER_PERIA_INTA250=0x1F,IER_PERIA_INTA251=0x1F,IER_PERIA_INTA252=0x1F,IER_PERIA_INTA253=0x1F,IER_PERIA_INTA254=0x1F,IER_PERIA_INTA255=0x1F
} enum_ier_t;

typedef enum enum_ipr {
IPR_BSC_BUSERR=0,
IPR_RAM_RAMERR=0,
IPR_FCU_FIFERR=1,IPR_FCU_FRDYI=2,
IPR_ICU_SWINT2=3,IPR_ICU_SWINT=3,
IPR_CMT0_CMI0=4,
IPR_CMT1_CMI1=5,
IPR_CMT2_CMI2=6,
IPR_CMT3_CMI3=7,
IPR_USB0_D0FIFO0=34,IPR_USB0_D1FIFO0=35,
IPR_RSPI0_SPRI0=38,IPR_RSPI0_SPTI0=39,
IPR_RIIC0_RXI0=52,IPR_RIIC0_TXI0=53,
IPR_SCI1_RXI1=60,IPR_SCI1_TXI1=61,
IPR_ICU_IRQ0=64,IPR_ICU_IRQ1=65,IPR_ICU_IRQ2=66,IPR_ICU_IRQ3=67,IPR_ICU_IRQ4=68,IPR_ICU_IRQ5=69,IPR_ICU_IRQ6=70,IPR_ICU_IRQ7=71,IPR_ICU_IRQ8=72,IPR_ICU_IRQ9=73,IPR_ICU_IRQ10=74,IPR_ICU_IRQ11=75,IPR_ICU_IRQ12=76,IPR_ICU_IRQ13=77,IPR_ICU_IRQ14=78,IPR_ICU_IRQ15=79,
IPR_SCI5_RXI5=84,IPR_SCI5_TXI5=85,
IPR_SCI6_RXI6=86,IPR_SCI6_TXI6=87,
IPR_LVD1_LVD1=88,
IPR_LVD2_LVD2=89,
IPR_USB0_USBR0=90,
IPR_IWDT_IWUNI=95,
IPR_WDT_WUNI=96,
IPR_SCI8_RXI8=100,IPR_SCI8_TXI8=101,
IPR_SCI9_RXI9=102,IPR_SCI9_TXI9=103,
IPR_ICU_GROUPBE0=106,IPR_ICU_GROUPBL0=110,IPR_ICU_GROUPBL1=111,IPR_ICU_GROUPAL0=112,
IPR_SCI11_RXI11=114,IPR_SCI11_TXI11=115,
IPR_SCI12_RXI12=116,IPR_SCI12_TXI12=117,
IPR_DMAC_DMAC0I=120,IPR_DMAC_DMAC1I=121,IPR_DMAC_DMAC2I=122,IPR_DMAC_DMAC3I=123,IPR_DMAC_DMAC74I=124,
IPR_OST_OSTDI=125,
IPR_S12AD_S12ADI=128,IPR_S12AD_S12GBADI=129,IPR_S12AD_S12GCADI=130,
IPR_S12AD1_S12ADI1=132,IPR_S12AD1_S12GBADI1=133,IPR_S12AD1_S12GCADI1=134,
IPR_S12AD2_S12ADI2=136,IPR_S12AD2_S12GBADI2=137,IPR_S12AD2_S12GCADI2=138,
IPR_TMR0_CMIA0=146,IPR_TMR0_CMIB0=146,IPR_TMR0_OVI0=146,
IPR_TMR1_CMIA1=149,IPR_TMR1_CMIB1=149,IPR_TMR1_OVI1=149,
IPR_TMR2_CMIA2=152,IPR_TMR2_CMIB2=152,IPR_TMR2_OVI2=152,
IPR_TMR3_CMIA3=155,IPR_TMR3_CMIB3=155,IPR_TMR3_OVI3=155,
IPR_TMR4_CMIA4=158,IPR_TMR4_CMIB4=158,IPR_TMR4_OVI4=158,
IPR_TMR5_CMIA5=161,IPR_TMR5_CMIB5=161,IPR_TMR5_OVI5=161,
IPR_TMR6_CMIA6=164,IPR_TMR6_CMIB6=164,IPR_TMR6_OVI6=164,
IPR_TMR7_CMIA7=167,IPR_TMR7_CMIB7=167,IPR_TMR7_OVI7=167,
IPR_CAN0_RXF0=170,IPR_CAN0_TXF0=170,IPR_CAN0_RXM0=170,IPR_CAN0_TXM0=170,
IPR_USB0_USBI0=174,
IPR_ELC_ELSR18I=175,IPR_ELC_ELSR19I=176,
IPR_TSIP_RD=177,IPR_TSIP_WR=177,IPR_TSIP_ERR=179,
IPR_CMPC0_CMPC0=180,
IPR_CMPC1_CMPC1=181,
IPR_CMPC2_CMPC2=182,
IPR_CMPC3_CMPC3=183,
IPR_CMPC4_CMPC4=184,
IPR_CMPC5_CMPC5=185,
IPR_PERIA_INTA208=208,IPR_PERIA_INTA209=209,IPR_PERIA_INTA210=210,IPR_PERIA_INTA211=211,IPR_PERIA_INTA212=212,IPR_PERIA_INTA213=213,IPR_PERIA_INTA214=214,IPR_PERIA_INTA215=215,IPR_PERIA_INTA216=216,IPR_PERIA_INTA217=217,IPR_PERIA_INTA218=218,IPR_PERIA_INTA219=219,IPR_PERIA_INTA220=220,IPR_PERIA_INTA221=221,IPR_PERIA_INTA222=222,IPR_PERIA_INTA223=223,IPR_PERIA_INTA224=224,IPR_PERIA_INTA225=225,IPR_PERIA_INTA226=226,IPR_PERIA_INTA227=227,IPR_PERIA_INTA228=228,IPR_PERIA_INTA229=229,IPR_PERIA_INTA230=230,IPR_PERIA_INTA231=231,IPR_PERIA_INTA232=232,IPR_PERIA_INTA233=233,IPR_PERIA_INTA234=234,IPR_PERIA_INTA235=235,IPR_PERIA_INTA236=236,IPR_PERIA_INTA237=237,IPR_PERIA_INTA238=238,IPR_PERIA_INTA239=239,IPR_PERIA_INTA240=240,IPR_PERIA_INTA241=241,IPR_PERIA_INTA242=242,IPR_PERIA_INTA243=243,IPR_PERIA_INTA244=244,IPR_PERIA_INTA245=245,IPR_PERIA_INTA246=246,IPR_PERIA_INTA247=247,IPR_PERIA_INTA248=248,IPR_PERIA_INTA249=249,IPR_PERIA_INTA250=250,IPR_PERIA_INTA251=251,IPR_PERIA_INTA252=252,IPR_PERIA_INTA253=253,IPR_PERIA_INTA254=254,IPR_PERIA_INTA255=255,
IPR_ICU_SWI=3,
IPR_CMT0_=4,
IPR_CMT1_=5,
IPR_CMT2_=6,
IPR_CMT3_=7,
IPR_RSPI0_=38,
IPR_LVD1_=88,
IPR_LVD2_=89,
IPR_IWDT_=95,
IPR_WDT_=96,
IPR_OST_=125,
IPR_TMR0_=146,
IPR_TMR1_=149,
IPR_TMR2_=152,
IPR_TMR3_=155,
IPR_TMR4_=158,
IPR_TMR5_=161,
IPR_TMR6_=164,
IPR_TMR7_=167,
IPR_CAN0_=170,
IPR_CMPC0_=180,
IPR_CMPC1_=181,
IPR_CMPC2_=182,
IPR_CMPC3_=183,
IPR_CMPC4_=184,
IPR_CMPC5_=185
} enum_ipr_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma bitfields=reversed
#else
#pragma bit_order left
#endif

#ifndef __IAR_SYSTEMS_ICC__
#pragma unpack
#endif


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
			unsigned short :2;
			unsigned short BPEB:2;
			unsigned short BPFB:2;
			unsigned short BPHB:2;
			unsigned short BPGB:2;
			unsigned short BPIB:2;
			unsigned short BPRO:2;
			unsigned short BPRA:2;
		} BIT;
	} BUSPRI;
	char           wk4[7408];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRMOD:1;
			unsigned short :5;
			unsigned short PWENB:1;
			unsigned short PRENB:1;
			unsigned short :4;
			unsigned short EWENB:1;
			unsigned short :2;
			unsigned short WRMOD:1;
		} BIT;
	} CS0MOD;
	union {
		unsigned long LONG;
		struct {
			unsigned long :3;
			unsigned long CSRWAIT:5;
			unsigned long :3;
			unsigned long CSWWAIT:5;
			unsigned long :5;
			unsigned long CSPRWAIT:3;
			unsigned long :5;
			unsigned long CSPWWAIT:3;
		} BIT;
	} CS0WCR1;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long CSON:3;
			unsigned long :1;
			unsigned long WDON:3;
			unsigned long :1;
			unsigned long WRON:3;
			unsigned long :1;
			unsigned long RDON:3;
			unsigned long :2;
			unsigned long AWAIT:2;
			unsigned long :1;
			unsigned long WDOFF:3;
			unsigned long :1;
			unsigned long CSWOFF:3;
			unsigned long :1;
			unsigned long CSROFF:3;
		} BIT;
	} CS0WCR2;
	char           wk5[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRMOD:1;
			unsigned short :5;
			unsigned short PWENB:1;
			unsigned short PRENB:1;
			unsigned short :4;
			unsigned short EWENB:1;
			unsigned short :2;
			unsigned short WRMOD:1;
		} BIT;
	} CS1MOD;
	union {
		unsigned long LONG;
		struct {
			unsigned long :3;
			unsigned long CSRWAIT:5;
			unsigned long :3;
			unsigned long CSWWAIT:5;
			unsigned long :5;
			unsigned long CSPRWAIT:3;
			unsigned long :5;
			unsigned long CSPWWAIT:3;
		} BIT;
	} CS1WCR1;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long CSON:3;
			unsigned long :1;
			unsigned long WDON:3;
			unsigned long :1;
			unsigned long WRON:3;
			unsigned long :1;
			unsigned long RDON:3;
			unsigned long :2;
			unsigned long AWAIT:2;
			unsigned long :1;
			unsigned long WDOFF:3;
			unsigned long :1;
			unsigned long CSWOFF:3;
			unsigned long :1;
			unsigned long CSROFF:3;
		} BIT;
	} CS1WCR2;
	char           wk6[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRMOD:1;
			unsigned short :5;
			unsigned short PWENB:1;
			unsigned short PRENB:1;
			unsigned short :4;
			unsigned short EWENB:1;
			unsigned short :2;
			unsigned short WRMOD:1;
		} BIT;
	} CS2MOD;
	union {
		unsigned long LONG;
		struct {
			unsigned long :3;
			unsigned long CSRWAIT:5;
			unsigned long :3;
			unsigned long CSWWAIT:5;
			unsigned long :5;
			unsigned long CSPRWAIT:3;
			unsigned long :5;
			unsigned long CSPWWAIT:3;
		} BIT;
	} CS2WCR1;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long CSON:3;
			unsigned long :1;
			unsigned long WDON:3;
			unsigned long :1;
			unsigned long WRON:3;
			unsigned long :1;
			unsigned long RDON:3;
			unsigned long :2;
			unsigned long AWAIT:2;
			unsigned long :1;
			unsigned long WDOFF:3;
			unsigned long :1;
			unsigned long CSWOFF:3;
			unsigned long :1;
			unsigned long CSROFF:3;
		} BIT;
	} CS2WCR2;
	char           wk7[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRMOD:1;
			unsigned short :5;
			unsigned short PWENB:1;
			unsigned short PRENB:1;
			unsigned short :4;
			unsigned short EWENB:1;
			unsigned short :2;
			unsigned short WRMOD:1;
		} BIT;
	} CS3MOD;
	union {
		unsigned long LONG;
		struct {
			unsigned long :3;
			unsigned long CSRWAIT:5;
			unsigned long :3;
			unsigned long CSWWAIT:5;
			unsigned long :5;
			unsigned long CSPRWAIT:3;
			unsigned long :5;
			unsigned long CSPWWAIT:3;
		} BIT;
	} CS3WCR1;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long CSON:3;
			unsigned long :1;
			unsigned long WDON:3;
			unsigned long :1;
			unsigned long WRON:3;
			unsigned long :1;
			unsigned long RDON:3;
			unsigned long :2;
			unsigned long AWAIT:2;
			unsigned long :1;
			unsigned long WDOFF:3;
			unsigned long :1;
			unsigned long CSWOFF:3;
			unsigned long :1;
			unsigned long CSROFF:3;
		} BIT;
	} CS3WCR2;
	char           wk8[1990];
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short MPXEN:1;
			unsigned short :3;
			unsigned short EMODE:1;
			unsigned short :2;
			unsigned short BSIZE:2;
			unsigned short :3;
			unsigned short EXENB:1;
		} BIT;
	} CS0CR;
	char           wk9[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short WRCV:4;
			unsigned short :4;
			unsigned short RRCV:4;
		} BIT;
	} CS0REC;
	char           wk10[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short MPXEN:1;
			unsigned short :3;
			unsigned short EMODE:1;
			unsigned short :2;
			unsigned short BSIZE:2;
			unsigned short :3;
			unsigned short EXENB:1;
		} BIT;
	} CS1CR;
	char           wk11[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short WRCV:4;
			unsigned short :4;
			unsigned short RRCV:4;
		} BIT;
	} CS1REC;
	char           wk12[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short MPXEN:1;
			unsigned short :3;
			unsigned short EMODE:1;
			unsigned short :2;
			unsigned short BSIZE:2;
			unsigned short :3;
			unsigned short EXENB:1;
		} BIT;
	} CS2CR;
	char           wk13[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short WRCV:4;
			unsigned short :4;
			unsigned short RRCV:4;
		} BIT;
	} CS2REC;
	char           wk14[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short MPXEN:1;
			unsigned short :3;
			unsigned short EMODE:1;
			unsigned short :2;
			unsigned short BSIZE:2;
			unsigned short :3;
			unsigned short EXENB:1;
		} BIT;
	} CS3CR;
	char           wk15[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short WRCV:4;
			unsigned short :4;
			unsigned short RRCV:4;
		} BIT;
	} CS3REC;
	char           wk16[68];
	union {
		unsigned short WORD;
		struct {
			unsigned short RCVENM7:1;
			unsigned short RCVENM6:1;
			unsigned short RCVENM5:1;
			unsigned short RCVENM4:1;
			unsigned short RCVENM3:1;
			unsigned short RCVENM2:1;
			unsigned short RCVENM1:1;
			unsigned short RCVENM0:1;
			unsigned short RCVEN7:1;
			unsigned short RCVEN6:1;
			unsigned short RCVEN5:1;
			unsigned short RCVEN4:1;
			unsigned short RCVEN3:1;
			unsigned short RCVEN2:1;
			unsigned short RCVEN1:1;
			unsigned short RCVEN0:1;
		} BIT;
	} CSRECEN;
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

typedef struct st_can {
	struct {
		union {
			unsigned long LONG;
			struct {
				unsigned short H;
				unsigned short L;
			} WORD;
			struct {
				unsigned char HH;
				unsigned char HL;
				unsigned char LH;
				unsigned char LL;
			} BYTE;
			struct {
				unsigned long IDE:1;
				unsigned long RTR:1;
				unsigned long :1;
				unsigned long SID:11;
				unsigned long EID:18;
			} BIT;
		} ID;
		unsigned short DLC;
		unsigned char  DATA[8];
		unsigned short TS;
	} MB[32];
	union {
		unsigned long LONG;
		struct {
			unsigned short H;
			unsigned short L;
		} WORD;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned long :3;
			unsigned long SID:11;
			unsigned long EID:18;
		} BIT;
	} MKR[8];
	union {
		unsigned long LONG;
		struct {
			unsigned short H;
			unsigned short L;
		} WORD;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned long IDE:1;
			unsigned long RTR:1;
			unsigned long :1;
			unsigned long SID:11;
			unsigned long EID:18;
		} BIT;
	} FIDCR0;
	union {
		unsigned long LONG;
		struct {
			unsigned short H;
			unsigned short L;
		} WORD;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned long IDE:1;
			unsigned long RTR:1;
			unsigned long :1;
			unsigned long SID:11;
			unsigned long EID:18;
		} BIT;
	} FIDCR1;
	union {
		unsigned long LONG;
		struct {
			unsigned short H;
			unsigned short L;
		} WORD;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned char MB31:1;
			unsigned char MB30:1;
			unsigned char MB29:1;
			unsigned char MB28:1;
			unsigned char MB27:1;
			unsigned char MB26:1;
			unsigned char MB25:1;
			unsigned char MB24:1;
			unsigned char MB23:1;
			unsigned char MB22:1;
			unsigned char MB21:1;
			unsigned char MB20:1;
			unsigned char MB19:1;
			unsigned char MB18:1;
			unsigned char MB17:1;
			unsigned char MB16:1;
			unsigned char MB15:1;
			unsigned char MB14:1;
			unsigned char MB13:1;
			unsigned char MB12:1;
			unsigned char MB11:1;
			unsigned char MB10:1;
			unsigned char MB9:1;
			unsigned char MB8:1;
			unsigned char MB7:1;
			unsigned char MB6:1;
			unsigned char MB5:1;
			unsigned char MB4:1;
			unsigned char MB3:1;
			unsigned char MB2:1;
			unsigned char MB1:1;
			unsigned char MB0:1;
		} BIT;
	} MKIVLR;
	union {
		unsigned long LONG;
		struct {
			unsigned short H;
			unsigned short L;
		} WORD;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned char MB31:1;
			unsigned char MB30:1;
			unsigned char MB29:1;
			unsigned char MB28:1;
			unsigned char MB27:1;
			unsigned char MB26:1;
			unsigned char MB25:1;
			unsigned char MB24:1;
			unsigned char MB23:1;
			unsigned char MB22:1;
			unsigned char MB21:1;
			unsigned char MB20:1;
			unsigned char MB19:1;
			unsigned char MB18:1;
			unsigned char MB17:1;
			unsigned char MB16:1;
			unsigned char MB15:1;
			unsigned char MB14:1;
			unsigned char MB13:1;
			unsigned char MB12:1;
			unsigned char MB11:1;
			unsigned char MB10:1;
			unsigned char MB9:1;
			unsigned char MB8:1;
			unsigned char MB7:1;
			unsigned char MB6:1;
			unsigned char MB5:1;
			unsigned char MB4:1;
			unsigned char MB3:1;
			unsigned char MB2:1;
			unsigned char MB1:1;
			unsigned char MB0:1;
		} BIT;
	} MIER;
	char           wk0[1008];
	union {
		unsigned char BYTE;
		union {
			struct {
				unsigned char TRMREQ:1;
				unsigned char RECREQ:1;
				unsigned char :1;
				unsigned char ONESHOT:1;
				unsigned char :1;
				unsigned char TRMABT:1;
				unsigned char TRMACTIVE:1;
				unsigned char SENTDATA:1;
			} TX;
			struct {
				unsigned char TRMREQ:1;
				unsigned char RECREQ:1;
				unsigned char :1;
				unsigned char ONESHOT:1;
				unsigned char :1;
				unsigned char MSGLOST:1;
				unsigned char INVALDATA:1;
				unsigned char NEWDATA:1;
			} RX;
		} BIT;
	} MCTL[32];
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :2;
			unsigned char RBOC:1;
			unsigned char BOM:2;
			unsigned char SLPM:1;
			unsigned char CANM:2;
			unsigned char TSPS:2;
			unsigned char TSRC:1;
			unsigned char TPM:1;
			unsigned char MLM:1;
			unsigned char IDFM:2;
			unsigned char MBM:1;
		} BIT;
	} CTLR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :1;
			unsigned char RECST:1;
			unsigned char TRMST:1;
			unsigned char BOST:1;
			unsigned char EPST:1;
			unsigned char SLPST:1;
			unsigned char HLTST:1;
			unsigned char RSTST:1;
			unsigned char EST:1;
			unsigned char TABST:1;
			unsigned char FMLST:1;
			unsigned char NMLST:1;
			unsigned char TFST:1;
			unsigned char RFST:1;
			unsigned char SDST:1;
			unsigned char NDST:1;
		} BIT;
	} STR;
	union {
		unsigned long LONG;
		struct {
			unsigned short H;
			unsigned short L;
		} WORD;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned long TSEG1:4;
			unsigned long :2;
			unsigned long BRP:10;
			unsigned long :2;
			unsigned long SJW:2;
			unsigned long :1;
			unsigned long TSEG2:3;
			unsigned long :7;
			unsigned long CCLKS:1;
		} BIT;
	} BCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char RFEST:1;
			unsigned char RFWST:1;
			unsigned char RFFST:1;
			unsigned char RFMLF:1;
			unsigned char RFUST:3;
			unsigned char RFE:1;
		} BIT;
	} RFCR;
	unsigned char  RFPCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TFEST:1;
			unsigned char TFFST:1;
			unsigned char :2;
			unsigned char TFUST:3;
			unsigned char TFE:1;
		} BIT;
	} TFCR;
	unsigned char  TFPCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char BLIE:1;
			unsigned char OLIE:1;
			unsigned char ORIE:1;
			unsigned char BORIE:1;
			unsigned char BOEIE:1;
			unsigned char EPIE:1;
			unsigned char EWIE:1;
			unsigned char BEIE:1;
		} BIT;
	} EIER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char BLIF:1;
			unsigned char OLIF:1;
			unsigned char ORIF:1;
			unsigned char BORIF:1;
			unsigned char BOEIF:1;
			unsigned char EPIF:1;
			unsigned char EWIF:1;
			unsigned char BEIF:1;
		} BIT;
	} EIFR;
	unsigned char  RECR;
	unsigned char  TECR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char EDPM:1;
			unsigned char ADEF:1;
			unsigned char BE0F:1;
			unsigned char BE1F:1;
			unsigned char CEF:1;
			unsigned char AEF:1;
			unsigned char FEF:1;
			unsigned char SEF:1;
		} BIT;
	} ECSR;
	unsigned char  CSSR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char SEST:1;
			unsigned char :2;
			unsigned char MBNST:5;
		} BIT;
	} MSSR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char MBSM:2;
		} BIT;
	} MSMR;
	unsigned short TSR;
	unsigned short AFSR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char TSTM:2;
			unsigned char TSTE:1;
		} BIT;
	} TCR;
} st_can_t;

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
			unsigned char :4;
			unsigned char CVRS:4;
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
			unsigned char LMS:1;
			unsigned char :3;
			unsigned char GPS:3;
		} BIT;
	} CRCCR;
	char           wk0[3];
	union {	
		unsigned long LONG;
		unsigned char BYTE;	
	} CRCDIR;
	union {
		unsigned long LONG;	
		unsigned short WORD;	
		unsigned char BYTE;
	} CRCDOR;
} st_crc_t;

typedef struct st_da {
	unsigned short DADR0;
	unsigned short DADR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DAOE1:1;
			unsigned char DAOE0:1;
			unsigned char DAE:1;
			unsigned char :5;
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
	char           wk0[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char OUTREF1:1;
			unsigned char OUTREF0:1;
			unsigned char OUTDA1:1;
			unsigned char OUTDA0:1;
		} BIT;
	} DADSELR;
} st_da_t;

typedef struct st_dmac {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DMST:1;
		} BIT;
	} DMAST;
	char           wk0[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char DMIS7:1;
			unsigned char DMIS6:1;
			unsigned char DMIS5:1;
			unsigned char DMIS4:1;
			unsigned char :4;
		} BIT;
	} DMIST;
} st_dmac_t;

typedef struct st_dmac0 {
	void          *DMSAR;
	void          *DMDAR;
	unsigned long  DMCRA;
	unsigned short DMCRB;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short MD:2;
			unsigned short DTS:2;
			unsigned short :2;
			unsigned short SZ:2;
			unsigned short :6;
			unsigned short DCTG:2;
		} BIT;
	} DMTMD;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char DTIE:1;
			unsigned char ESIE:1;
			unsigned char RPTIE:1;
			unsigned char SARIE:1;
			unsigned char DARIE:1;
		} BIT;
	} DMINT;
	union {
		unsigned short WORD;
		struct {
			unsigned short SM:2;
			unsigned short :1;
			unsigned short SARA:5;
			unsigned short DM:2;
			unsigned short :1;
			unsigned short DARA:5;
		} BIT;
	} DMAMD;
	char           wk2[2];
	unsigned long  DMOFR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DTE:1;
		} BIT;
	} DMCNT;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char CLRS:1;
			unsigned char :3;
			unsigned char SWREQ:1;
		} BIT;
	} DMREQ;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ACT:1;
			unsigned char :2;
			unsigned char DTIF:1;
			unsigned char :3;
			unsigned char ESIF:1;
		} BIT;
	} DMSTS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DISEL:1;
		} BIT;
	} DMCSL;
} st_dmac0_t;

typedef struct st_dmac1 {
	void          *DMSAR;
	void          *DMDAR;
	unsigned long  DMCRA;
	unsigned short DMCRB;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short MD:2;
			unsigned short DTS:2;
			unsigned short :2;
			unsigned short SZ:2;
			unsigned short :6;
			unsigned short DCTG:2;
		} BIT;
	} DMTMD;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char DTIE:1;
			unsigned char ESIE:1;
			unsigned char RPTIE:1;
			unsigned char SARIE:1;
			unsigned char DARIE:1;
		} BIT;
	} DMINT;
	union {
		unsigned short WORD;
		struct {
			unsigned short SM:2;
			unsigned short :1;
			unsigned short SARA:5;
			unsigned short DM:2;
			unsigned short :1;
			unsigned short DARA:5;
		} BIT;
	} DMAMD;
	char           wk2[6];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DTE:1;
		} BIT;
	} DMCNT;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char CLRS:1;
			unsigned char :3;
			unsigned char SWREQ:1;
		} BIT;
	} DMREQ;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ACT:1;
			unsigned char :2;
			unsigned char DTIF:1;
			unsigned char :3;
			unsigned char ESIF:1;
		} BIT;
	} DMSTS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char DISEL:1;
		} BIT;
	} DMCSL;
} st_dmac1_t;

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

typedef struct st_elc {
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELCON:1;
			unsigned char :7;
		} BIT;
	} ELCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR0;
	char           wk0[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR4;
	char           wk1[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR7;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR10;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR11;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR12;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR13;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR15;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR16;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR18;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR19;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR20;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR21;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR22;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR23;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR24;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR25;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR26;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR27;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR28;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char MTU3MD:2;
			unsigned char :4;
			unsigned char MTU0MD:2;
		} BIT;
	} ELOPA;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char MTU4MD:2;
		} BIT;
	} ELOPB;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char CMT1MD:2;
			unsigned char :2;
		} BIT;
	} ELOPC;
	union {
		unsigned char BYTE;
		struct {
			unsigned char TMR3MD:2;
			unsigned char TMR2MD:2;
			unsigned char TMR1MD:2;
			unsigned char TMR0MD:2;
		} BIT;
	} ELOPD;
	union {
		unsigned char BYTE;
		struct {
			unsigned char PGR7:1;
			unsigned char PGR6:1;
			unsigned char PGR5:1;
			unsigned char PGR4:1;
			unsigned char PGR3:1;
			unsigned char PGR2:1;
			unsigned char PGR1:1;
			unsigned char PGR0:1;
		} BIT;
	} PGR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char PGR7:1;
			unsigned char PGR6:1;
			unsigned char PGR5:1;
			unsigned char PGR4:1;
			unsigned char PGR3:1;
			unsigned char PGR2:1;
			unsigned char PGR1:1;
			unsigned char PGR0:1;
		} BIT;
	} PGR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char PGCO:3;
			unsigned char :1;
			unsigned char PGCOVE:1;
			unsigned char PGCI:2;
		} BIT;
	} PGC1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char PGCO:3;
			unsigned char :1;
			unsigned char PGCOVE:1;
			unsigned char PGCI:2;
		} BIT;
	} PGC2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char PDBF7:1;
			unsigned char PDBF6:1;
			unsigned char PDBF5:1;
			unsigned char PDBF4:1;
			unsigned char PDBF3:1;
			unsigned char PDBF2:1;
			unsigned char PDBF1:1;
			unsigned char PDBF0:1;
		} BIT;
	} PDBF1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char PDBF7:1;
			unsigned char PDBF6:1;
			unsigned char PDBF5:1;
			unsigned char PDBF4:1;
			unsigned char PDBF3:1;
			unsigned char PDBF2:1;
			unsigned char PDBF1:1;
			unsigned char PDBF0:1;
		} BIT;
	} PDBF2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char PSM:2;
			unsigned char PSP:2;
			unsigned char PSB:3;
		} BIT;
	} PEL0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char PSM:2;
			unsigned char PSP:2;
			unsigned char PSB:3;
		} BIT;
	} PEL1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char PSM:2;
			unsigned char PSP:2;
			unsigned char PSB:3;
		} BIT;
	} PEL2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char PSM:2;
			unsigned char PSP:2;
			unsigned char PSB:3;
		} BIT;
	} PEL3;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char WI:1;
			unsigned char WE:1;
			unsigned char :5;
			unsigned char SEG:1;
		} BIT;
#endif
	} ELSEGR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR30;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR31;
	char           wk6[13];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR45;
	union {
		unsigned char BYTE;
		struct {
			unsigned char MTU9MD:2;
			unsigned char :2;
			unsigned char MTU7MD:2;
			unsigned char MTU6MD:2;
		} BIT;
	} ELOPE;
	char           wk7[5];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR46;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR47;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR48;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR49;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR50;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR51;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR52;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR53;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR54;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR55;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR56;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR57;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ELS:8;
		} BIT;
	} ELSR58;
} st_elc_t;

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
	char           wk1[58];
	unsigned long  NCRG0;
	union {
		unsigned long LONG;
		struct {
			unsigned long :11;
			unsigned long NCSZ:17;
			unsigned long NC3E:1;
			unsigned long NC2E:1;
			unsigned long NC1E:1;
			unsigned long :1;
		} BIT;
	} NCRC0;
	unsigned long  NCRG1;
	union {
		unsigned long LONG;
		struct {
			unsigned long :11;
			unsigned long NCSZ:17;
			unsigned long NC3E:1;
			unsigned long NC2E:1;
			unsigned long NC1E:1;
			unsigned long :1;
		} BIT;
	} NCRC1;
	char           wk2[45638];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char FLWE:2;
		} BIT;
	} FWEPROR;
	char           wk3[7794397];
	unsigned long  UIDR0;
	char           wk4[108];
	unsigned long  UIDR1;
	unsigned long  UIDR2;
	char           wk5[11812];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CFAE:1;
			unsigned char :2;
			unsigned char CMDLK:1;
			unsigned char DFAE:1;
			unsigned char :2;
			unsigned char ECRCT:1;
		} BIT;
	} FASTAT;
	char           wk6[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CFAEIE:1;
			unsigned char :2;
			unsigned char CMDLKIE:1;
			unsigned char DFAEIE:1;
			unsigned char :2;
			unsigned char ECRCTIE:1;
		} BIT;
	} FAEINT;
	char           wk7[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char FRDYIE:1;
		} BIT;
	} FRDYIE;
	char           wk8[23];
	union {
		unsigned long LONG;
		struct {
			unsigned long FSADDR:32;
		} BIT;
	} FSADDR;
	union {
		unsigned long LONG;
		struct {
			unsigned long FEADDR:32;
		} BIT;
	} FEADDR;
	char           wk9[72];
	union {
		unsigned long LONG;
		struct {
			unsigned long :16;
			unsigned long FRDY:1;
			unsigned long ILGLERR:1;
			unsigned long ERSERR:1;
			unsigned long PRGERR:1;
			unsigned long SUSRDY:1;
			unsigned long DBFULL:1;
			unsigned long ERSSPD:1;
			unsigned long PRGSPD:1;
			unsigned long FCUERR:1;
			unsigned long FLWEERR:1;
			unsigned long :6;
		} BIT;
	} FSTATR;
	union {
		unsigned short WORD;
		struct {
			unsigned short KEY:8;
			unsigned short FENTRYD:1;
			unsigned short :6;
			unsigned short FENTRYC:1;
		} BIT;
	} FENTRYR;
	char           wk10[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short KEY:8;
			unsigned short :7;
			unsigned short FPROTCN:1;
		} BIT;
	} FPROTR;
	char           wk11[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short KEY:8;
			unsigned short :7;
			unsigned short SUINIT:1;
		} BIT;
	} FSUINITR;
	char           wk12[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char FLOCKST:1;
		} BIT;
	} FLKSTAT;
	char           wk13[15];
	union {
		unsigned short WORD;
		struct {
			unsigned short CMDR:8;
			unsigned short PCMDR:8;
		} BIT;
	} FCMDR;
	char           wk14[30];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short PEERRST:8;
		} BIT;
	} FPESTAT;
	char           wk15[14];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char BCDIR:1;
		} BIT;
	} FBCCNT;
	char           wk16[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char BCST:1;
		} BIT;
	} FBCSTAT;
	char           wk17[3];
	union {
		unsigned long LONG;
		struct {
			unsigned long :13;
			unsigned long PSADR:19;
		} BIT;
	} FPSADDR;
	char           wk18[4];
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short ESUSPMD:1;
		} BIT;
	} FCPSR;
	char           wk19[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short KEY:8;
			unsigned short PCKA:8;
		} BIT;
	} FPCKAR;
} st_flash_t;

typedef struct st_gptw {
	union {
		unsigned long LONG;
		struct {
			unsigned long :16;
			unsigned long PRKEY:8;
			unsigned long :3;
			unsigned long CMNWP:1;
			unsigned long CLRWP:1;
			unsigned long STPWP:1;
			unsigned long STRWP:1;
			unsigned long WP:1;
		} BIT;
	} GTWP;
	union {
		unsigned long LONG;
		struct {
			unsigned long :22;
			unsigned long CSTRT9:1;
			unsigned long CSTRT8:1;
			unsigned long CSTRT7:1;
			unsigned long CSTRT6:1;
			unsigned long CSTRT5:1;
			unsigned long CSTRT4:1;
			unsigned long CSTRT3:1;
			unsigned long CSTRT2:1;
			unsigned long CSTRT1:1;
			unsigned long CSTRT0:1;
		} BIT;
	} GTSTR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :22;
			unsigned long CSTOP9:1;
			unsigned long CSTOP8:1;
			unsigned long CSTOP7:1;
			unsigned long CSTOP6:1;
			unsigned long CSTOP5:1;
			unsigned long CSTOP4:1;
			unsigned long CSTOP3:1;
			unsigned long CSTOP2:1;
			unsigned long CSTOP1:1;
			unsigned long CSTOP0:1;
		} BIT;
	} GTSTP;
	union {
		unsigned long LONG;
		struct {
			unsigned long :22;
			unsigned long CCLR9:1;
			unsigned long CCLR8:1;
			unsigned long CCLR7:1;
			unsigned long CCLR6:1;
			unsigned long CCLR5:1;
			unsigned long CCLR4:1;
			unsigned long CCLR3:1;
			unsigned long CCLR2:1;
			unsigned long CCLR1:1;
			unsigned long CCLR0:1;
		} BIT;
	} GTCLR;
	union {
		unsigned long LONG;
		struct {
			unsigned long CSTRT:1;
			unsigned long :7;
			unsigned long SSELCH:1;
			unsigned long SSELCG:1;
			unsigned long SSELCF:1;
			unsigned long SSELCE:1;
			unsigned long SSELCD:1;
			unsigned long SSELCC:1;
			unsigned long SSELCB:1;
			unsigned long SSELCA:1;
			unsigned long SSCBFAH:1;
			unsigned long SSCBFAL:1;
			unsigned long SSCBRAH:1;
			unsigned long SSCBRAL:1;
			unsigned long SSCAFBH:1;
			unsigned long SSCAFBL:1;
			unsigned long SSCARBH:1;
			unsigned long SSCARBL:1;
			unsigned long SSGTRGDF:1;
			unsigned long SSGTRGDR:1;
			unsigned long SSGTRGCF:1;
			unsigned long SSGTRGCR:1;
			unsigned long SSGTRGBF:1;
			unsigned long SSGTRGBR:1;
			unsigned long SSGTRGAF:1;
			unsigned long SSGTRGAR:1;
		} BIT;
	} GTSSR;
	union {
		unsigned long LONG;
		struct {
			unsigned long CSTOP:1;
			unsigned long :7;
			unsigned long PSELCH:1;
			unsigned long PSELCG:1;
			unsigned long PSELCF:1;
			unsigned long PSELCE:1;
			unsigned long PSELCD:1;
			unsigned long PSELCC:1;
			unsigned long PSELCB:1;
			unsigned long PSELCA:1;
			unsigned long PSCBFAH:1;
			unsigned long PSCBFAL:1;
			unsigned long PSCBRAH:1;
			unsigned long PSCBRAL:1;
			unsigned long PSCAFBH:1;
			unsigned long PSCAFBL:1;
			unsigned long PSCARBH:1;
			unsigned long PSCARBL:1;
			unsigned long PSGTRGDF:1;
			unsigned long PSGTRGDR:1;
			unsigned long PSGTRGCF:1;
			unsigned long PSGTRGCR:1;
			unsigned long PSGTRGBF:1;
			unsigned long PSGTRGBR:1;
			unsigned long PSGTRGAF:1;
			unsigned long PSGTRGAR:1;
		} BIT;
	} GTPSR;
	union {
		unsigned long LONG;
		struct {
			unsigned long CCLR:1;
			unsigned long :7;
			unsigned long CSELCH:1;
			unsigned long CSELCG:1;
			unsigned long CSELCF:1;
			unsigned long CSELCE:1;
			unsigned long CSELCD:1;
			unsigned long CSELCC:1;
			unsigned long CSELCB:1;
			unsigned long CSELCA:1;
			unsigned long CSCBFAH:1;
			unsigned long CSCBFAL:1;
			unsigned long CSCBRAH:1;
			unsigned long CSCBRAL:1;
			unsigned long CSCAFBH:1;
			unsigned long CSCAFBL:1;
			unsigned long CSCARBH:1;
			unsigned long CSCARBL:1;
			unsigned long CSGTRGDF:1;
			unsigned long CSGTRGDR:1;
			unsigned long CSGTRGCF:1;
			unsigned long CSGTRGCR:1;
			unsigned long CSGTRGBF:1;
			unsigned long CSGTRGBR:1;
			unsigned long CSGTRGAF:1;
			unsigned long CSGTRGAR:1;
		} BIT;
	} GTCSR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :8;
			unsigned long USELCH:1;
			unsigned long USELCG:1;
			unsigned long USELCF:1;
			unsigned long USELCE:1;
			unsigned long USELCD:1;
			unsigned long USELCC:1;
			unsigned long USELCB:1;
			unsigned long USELCA:1;
			unsigned long USCBFAH:1;
			unsigned long USCBFAL:1;
			unsigned long USCBRAH:1;
			unsigned long USCBRAL:1;
			unsigned long USCAFBH:1;
			unsigned long USCAFBL:1;
			unsigned long USCARBH:1;
			unsigned long USCARBL:1;
			unsigned long USGTRGDF:1;
			unsigned long USGTRGDR:1;
			unsigned long USGTRGCF:1;
			unsigned long USGTRGCR:1;
			unsigned long USGTRGBF:1;
			unsigned long USGTRGBR:1;
			unsigned long USGTRGAF:1;
			unsigned long USGTRGAR:1;
		} BIT;
	} GTUPSR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :8;
			unsigned long DSELCH:1;
			unsigned long DSELCG:1;
			unsigned long DSELCF:1;
			unsigned long DSELCE:1;
			unsigned long DSELCD:1;
			unsigned long DSELCC:1;
			unsigned long DSELCB:1;
			unsigned long DSELCA:1;
			unsigned long DSCBFAH:1;
			unsigned long DSCBFAL:1;
			unsigned long DSCBRAH:1;
			unsigned long DSCBRAL:1;
			unsigned long DSCAFBH:1;
			unsigned long DSCAFBL:1;
			unsigned long DSCARBH:1;
			unsigned long DSCARBL:1;
			unsigned long DSGTRGDF:1;
			unsigned long DSGTRGDR:1;
			unsigned long DSGTRGCF:1;
			unsigned long DSGTRGCR:1;
			unsigned long DSGTRGBF:1;
			unsigned long DSGTRGBR:1;
			unsigned long DSGTRGAF:1;
			unsigned long DSGTRGAR:1;
		} BIT;
	} GTDNSR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :8;
			unsigned long ASELCH:1;
			unsigned long ASELCG:1;
			unsigned long ASELCF:1;
			unsigned long ASELCE:1;
			unsigned long ASELCD:1;
			unsigned long ASELCC:1;
			unsigned long ASELCB:1;
			unsigned long ASELCA:1;
			unsigned long ASCBFAH:1;
			unsigned long ASCBFAL:1;
			unsigned long ASCBRAH:1;
			unsigned long ASCBRAL:1;
			unsigned long ASCAFBH:1;
			unsigned long ASCAFBL:1;
			unsigned long ASCARBH:1;
			unsigned long ASCARBL:1;
			unsigned long ASGTRGDF:1;
			unsigned long ASGTRGDR:1;
			unsigned long ASGTRGCF:1;
			unsigned long ASGTRGCR:1;
			unsigned long ASGTRGBF:1;
			unsigned long ASGTRGBR:1;
			unsigned long ASGTRGAF:1;
			unsigned long ASGTRGAR:1;
		} BIT;
	} GTICASR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :8;
			unsigned long BSELCH:1;
			unsigned long BSELCG:1;
			unsigned long BSELCF:1;
			unsigned long BSELCE:1;
			unsigned long BSELCD:1;
			unsigned long BSELCC:1;
			unsigned long BSELCB:1;
			unsigned long BSELCA:1;
			unsigned long BSCBFAH:1;
			unsigned long BSCBFAL:1;
			unsigned long BSCBRAH:1;
			unsigned long BSCBRAL:1;
			unsigned long BSCAFBH:1;
			unsigned long BSCAFBL:1;
			unsigned long BSCARBH:1;
			unsigned long BSCARBL:1;
			unsigned long BSGTRGDF:1;
			unsigned long BSGTRGDR:1;
			unsigned long BSGTRGCF:1;
			unsigned long BSGTRGCR:1;
			unsigned long BSGTRGBF:1;
			unsigned long BSGTRGBR:1;
			unsigned long BSGTRGAF:1;
			unsigned long BSGTRGAR:1;
		} BIT;
	} GTICBSR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :5;
			unsigned long TPCS:4;
			unsigned long :4;
			unsigned long MD:3;
			unsigned long :7;
			unsigned long ICDS:1;
			unsigned long :7;
			unsigned long CST:1;
		} BIT;
	} GTCR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :4;
			unsigned long OBDTYR:1;
			unsigned long OBDTYF:1;
			unsigned long OBDTY:2;
			unsigned long :4;
			unsigned long OADTYR:1;
			unsigned long OADTYF:1;
			unsigned long OADTY:2;
			unsigned long :14;
			unsigned long UDF:1;
			unsigned long UD:1;
		} BIT;
	} GTUDDTYC;
	union {
		unsigned long LONG;
		struct {
			unsigned long NFCSB:2;
			unsigned long NFBEN:1;
			unsigned long :2;
			unsigned long OBDF:2;
			unsigned long OBE:1;
			unsigned long OBHLD:1;
			unsigned long OBDFLT:1;
			unsigned long :1;
			unsigned long GTIOB:5;
			unsigned long NFCSA:2;
			unsigned long NFAEN:1;
			unsigned long :2;
			unsigned long OADF:2;
			unsigned long OAE:1;
			unsigned long OAHLD:1;
			unsigned long OADFLT:1;
			unsigned long :1;
			unsigned long GTIOA:5;
		} BIT;
	} GTIOR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long GRPABL:1;
			unsigned long GRPABH:1;
			unsigned long GRPDTE:1;
			unsigned long :2;
			unsigned long GRP:2;
			unsigned long :4;
			unsigned long ADTRBDEN:1;
			unsigned long ADTRBUEN:1;
			unsigned long ADTRADEN:1;
			unsigned long ADTRAUEN:1;
			unsigned long :8;
			unsigned long GTINTPR:2;
			unsigned long GTINTF:1;
			unsigned long GTINTE:1;
			unsigned long GTINTD:1;
			unsigned long GTINTC:1;
			unsigned long GTINTB:1;
			unsigned long GTINTA:1;
		} BIT;
	} GTINTAD;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long OABLF:1;
			unsigned long OABHF:1;
			unsigned long DTEF:1;
			unsigned long :3;
			unsigned long ODF:1;
			unsigned long :4;
			unsigned long ADTRBDF:1;
			unsigned long ADTRBUF:1;
			unsigned long ADTRADF:1;
			unsigned long ADTRAUF:1;
			unsigned long TUCF:1;
			unsigned long :4;
			unsigned long ITCNT:3;
			unsigned long :8;
		} BIT;
	} GTST;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long ADTDB:1;
			unsigned long ADTTB:2;
			unsigned long :1;
			unsigned long ADTDA:1;
			unsigned long ADTTA:2;
			unsigned long :1;
			unsigned long CCRSWT:1;
			unsigned long PR:2;
			unsigned long CCRB:2;
			unsigned long CCRA:2;
			unsigned long :5;
			unsigned long DBRTECB:1;
			unsigned long :1;
			unsigned long DBRTECA:1;
			unsigned long :4;
			unsigned long BD:4;
		} BIT;
	} GTBER;
	union {
		unsigned long LONG;
		struct {
			unsigned long :17;
			unsigned long ADTBL:1;
			unsigned long :1;
			unsigned long ADTAL:1;
			unsigned long :1;
			unsigned long IVTT:3;
			unsigned long IVTC:2;
			unsigned long ITLF:1;
			unsigned long ITLE:1;
			unsigned long ITLD:1;
			unsigned long ITLC:1;
			unsigned long ITLB:1;
			unsigned long ITLA:1;
		} BIT;
	} GTITC;
	unsigned long  GTCNT;
	unsigned long  GTCCRA;
	unsigned long  GTCCRB;
	unsigned long  GTCCRC;
	unsigned long  GTCCRE;
	unsigned long  GTCCRD;
	unsigned long  GTCCRF;
	unsigned long  GTPR;
	unsigned long  GTPBR;
	unsigned long  GTPDBR;
	unsigned long  GTADTRA;
	unsigned long  GTADTBRA;
	unsigned long  GTADTDBRA;
	unsigned long  GTADTRB;
	unsigned long  GTADTBRB;
	unsigned long  GTADTDBRB;
	union {
		unsigned long LONG;
		struct {
			unsigned long :23;
			unsigned long TDFER:1;
			unsigned long :2;
			unsigned long TDBDE:1;
			unsigned long TDBUE:1;
			unsigned long :3;
			unsigned long TDE:1;
		} BIT;
	} GTDTCR;
	unsigned long  GTDVU;
	unsigned long  GTDVD;
	unsigned long  GTDBU;
	unsigned long  GTDBD;
	union {
		unsigned long LONG;
		struct {
			unsigned long :30;
			unsigned long SOS:2;
		} BIT;
	} GTSOS;
	union {
		unsigned long LONG;
		struct {
			unsigned long :31;
			unsigned long SOTR:1;
		} BIT;
	} GTSOTR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :7;
			unsigned long ADSMEN1:1;
			unsigned long :6;
			unsigned long ADSMS1:2;
			unsigned long :7;
			unsigned long ADSMEN0:1;
			unsigned long :6;
			unsigned long ADSMS0:2;
		} BIT;
	} GTADSMR;
	union {
		unsigned long LONG;
		struct {
			unsigned long EITCNT2:4;
			unsigned long EITCNT2IV:4;
			unsigned long EIVTT2:4;
			unsigned long :2;
			unsigned long EIVTC2:2;
			unsigned long EITCNT1:4;
			unsigned long :4;
			unsigned long EIVTT1:4;
			unsigned long :2;
			unsigned long EIVTC1:2;
		} BIT;
	} GTEITC;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long EITLU:3;
			unsigned long :1;
			unsigned long EITLV:3;
			unsigned long :1;
			unsigned long EITLF:3;
			unsigned long :1;
			unsigned long EITLE:3;
			unsigned long :1;
			unsigned long EITLD:3;
			unsigned long :1;
			unsigned long EITLC:3;
			unsigned long :1;
			unsigned long EITLB:3;
			unsigned long :1;
			unsigned long EITLA:3;
		} BIT;
	} GTEITLI1;
	union {
		unsigned long LONG;
		struct {
			unsigned long :25;
			unsigned long EADTBL:3;
			unsigned long :1;
			unsigned long EADTAL:3;
		} BIT;
	} GTEITLI2;
	union {
		unsigned long LONG;
		struct {
			unsigned long :1;
			unsigned long EBTLDVD:3;
			unsigned long :1;
			unsigned long EBTLDVU:3;
			unsigned long :1;
			unsigned long EBTLADB:3;
			unsigned long :1;
			unsigned long EBTLADA:3;
			unsigned long :5;
			unsigned long EBTLPR:3;
			unsigned long :1;
			unsigned long EBTLCB:3;
			unsigned long :1;
			unsigned long EBTLCA:3;
		} BIT;
	} GTEITLB;
	char           wk0[24];
	union {
		unsigned long LONG;
		struct {
			unsigned long :22;
			unsigned long SECSEL9:1;
			unsigned long SECSEL8:1;
			unsigned long SECSEL7:1;
			unsigned long SECSEL6:1;
			unsigned long SECSEL5:1;
			unsigned long SECSEL4:1;
			unsigned long SECSEL3:1;
			unsigned long SECSEL2:1;
			unsigned long SECSEL1:1;
			unsigned long SECSEL0:1;
		} BIT;
	} GTSECSR;
	union {
		unsigned long LONG;
		struct {
			unsigned long :20;
			unsigned long SBDDD:1;
			unsigned long SBDAD:1;
			unsigned long SBDPD:1;
			unsigned long SBDCD:1;
			unsigned long :4;
			unsigned long SBDDE:1;
			unsigned long SBDAE:1;
			unsigned long SBDPE:1;
			unsigned long SBDCE:1;
		} BIT;
	} GTSECR;
} st_gptw_t;

typedef struct st_hrpwm {
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short HRRST:1;
			unsigned short DLLEN:1;
		} BIT;
	} HROCR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short HRDIS3:1;
			unsigned short HRDIS2:1;
			unsigned short HRDIS1:1;
			unsigned short HRDIS0:1;
			unsigned short :4;
			unsigned short HRSEL3:1;
			unsigned short HRSEL2:1;
			unsigned short HRSEL1:1;
			unsigned short HRSEL0:1;
		} BIT;
	} HROCR2;
	char           wk0[20];
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRREAR0A;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRREAR0B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRREAR1A;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRREAR1B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRREAR2A;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRREAR2B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRREAR3A;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRREAR3B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRFEAR0A;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRFEAR0B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRFEAR1A;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRFEAR1B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRFEAR2A;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRFEAR2B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRFEAR3A;
	union {
		unsigned short WORD;
		struct {
			unsigned short :11;
			unsigned short DLY:5;
		} BIT;
	} HRFEAR3B;
} st_hrpwm_t;

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
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char SWINT2:1;
		} BIT;
	} SWINT2R;
	char           wk1[14];
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
	unsigned char  DMRSR0;
	char           wk3[3];
	unsigned char  DMRSR1;
	char           wk4[3];
	unsigned char  DMRSR2;
	char           wk5[3];
	unsigned char  DMRSR3;
	char           wk6[3];
	unsigned char  DMRSR4;
	char           wk7[3];
	unsigned char  DMRSR5;
	char           wk8[3];
	unsigned char  DMRSR6;
	char           wk9[3];
	unsigned char  DMRSR7;
	char           wk10[227];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char IRQMD:2;
			unsigned char :2;
		} BIT;
	} IRQCR[16];
	char           wk11[16];
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
	union {
		unsigned char BYTE;
		struct {
			unsigned char FLTEN15:1;
			unsigned char FLTEN14:1;
			unsigned char FLTEN13:1;
			unsigned char FLTEN12:1;
			unsigned char FLTEN11:1;
			unsigned char FLTEN10:1;
			unsigned char FLTEN9:1;
			unsigned char FLTEN8:1;
		} BIT;
	} IRQFLTE1;
	char           wk12[6];
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
	union {
		unsigned short WORD;
		struct {
			unsigned short FCLKSEL15:2;
			unsigned short FCLKSEL14:2;
			unsigned short FCLKSEL13:2;
			unsigned short FCLKSEL12:2;
			unsigned short FCLKSEL11:2;
			unsigned short FCLKSEL10:2;
			unsigned short FCLKSEL9:2;
			unsigned short FCLKSEL8:2;
		} BIT;
	} IRQFLTC1;
	char           wk13[84];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char RAMST:1;
			unsigned char LVD2ST:1;
			unsigned char LVD1ST:1;
			unsigned char IWDTST:1;
			unsigned char WDTST:1;
			unsigned char OSTST:1;
			unsigned char NMIST:1;
		} BIT;
	} NMISR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char RAMEN:1;
			unsigned char LVD2EN:1;
			unsigned char LVD1EN:1;
			unsigned char IWDTEN:1;
			unsigned char WDTEN:1;
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
			unsigned char WDTCLR:1;
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
	char           wk14[12];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char NFLTEN:1;
		} BIT;
	} NMIFLTE;
	char           wk15[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char NFCLKSEL:2;
		} BIT;
	} NMIFLTC;
	char           wk16[107];
	union {
		unsigned long LONG;
		struct {
			unsigned long IS31:1;
			unsigned long IS30:1;
			unsigned long IS29:1;
			unsigned long IS28:1;
			unsigned long IS27:1;
			unsigned long IS26:1;
			unsigned long IS25:1;
			unsigned long IS24:1;
			unsigned long IS23:1;
			unsigned long IS22:1;
			unsigned long IS21:1;
			unsigned long IS20:1;
			unsigned long IS19:1;
			unsigned long IS18:1;
			unsigned long IS17:1;
			unsigned long IS16:1;
			unsigned long IS15:1;
			unsigned long IS14:1;
			unsigned long IS13:1;
			unsigned long IS12:1;
			unsigned long IS11:1;
			unsigned long IS10:1;
			unsigned long IS9:1;
			unsigned long IS8:1;
			unsigned long IS7:1;
			unsigned long IS6:1;
			unsigned long IS5:1;
			unsigned long IS4:1;
			unsigned long IS3:1;
			unsigned long IS2:1;
			unsigned long IS1:1;
			unsigned long IS0:1;
		} BIT;
	} GRPBE0;
	char           wk17[44];
	union {
		unsigned long LONG;
		struct {
			unsigned long IS31:1;
			unsigned long IS30:1;
			unsigned long IS29:1;
			unsigned long IS28:1;
			unsigned long IS27:1;
			unsigned long IS26:1;
			unsigned long IS25:1;
			unsigned long IS24:1;
			unsigned long IS23:1;
			unsigned long IS22:1;
			unsigned long IS21:1;
			unsigned long IS20:1;
			unsigned long IS19:1;
			unsigned long IS18:1;
			unsigned long IS17:1;
			unsigned long IS16:1;
			unsigned long IS15:1;
			unsigned long IS14:1;
			unsigned long IS13:1;
			unsigned long IS12:1;
			unsigned long IS11:1;
			unsigned long IS10:1;
			unsigned long IS9:1;
			unsigned long IS8:1;
			unsigned long IS7:1;
			unsigned long IS6:1;
			unsigned long IS5:1;
			unsigned long IS4:1;
			unsigned long IS3:1;
			unsigned long IS2:1;
			unsigned long IS1:1;
			unsigned long IS0:1;
		} BIT;
	} GRPBL0;
	union {
		unsigned long LONG;
		struct {
			unsigned long IS31:1;
			unsigned long IS30:1;
			unsigned long IS29:1;
			unsigned long IS28:1;
			unsigned long IS27:1;
			unsigned long IS26:1;
			unsigned long IS25:1;
			unsigned long IS24:1;
			unsigned long IS23:1;
			unsigned long IS22:1;
			unsigned long IS21:1;
			unsigned long IS20:1;
			unsigned long IS19:1;
			unsigned long IS18:1;
			unsigned long IS17:1;
			unsigned long IS16:1;
			unsigned long IS15:1;
			unsigned long IS14:1;
			unsigned long IS13:1;
			unsigned long IS12:1;
			unsigned long IS11:1;
			unsigned long IS10:1;
			unsigned long IS9:1;
			unsigned long IS8:1;
			unsigned long IS7:1;
			unsigned long IS6:1;
			unsigned long IS5:1;
			unsigned long IS4:1;
			unsigned long IS3:1;
			unsigned long IS2:1;
			unsigned long IS1:1;
			unsigned long IS0:1;
		} BIT;
	} GRPBL1;
	char           wk18[8];
	union {
		unsigned long LONG;
		struct {
			unsigned long EN31:1;
			unsigned long EN30:1;
			unsigned long EN29:1;
			unsigned long EN28:1;
			unsigned long EN27:1;
			unsigned long EN26:1;
			unsigned long EN25:1;
			unsigned long EN24:1;
			unsigned long EN23:1;
			unsigned long EN22:1;
			unsigned long EN21:1;
			unsigned long EN20:1;
			unsigned long EN19:1;
			unsigned long EN18:1;
			unsigned long EN17:1;
			unsigned long EN16:1;
			unsigned long EN15:1;
			unsigned long EN14:1;
			unsigned long EN13:1;
			unsigned long EN12:1;
			unsigned long EN11:1;
			unsigned long EN10:1;
			unsigned long EN9:1;
			unsigned long EN8:1;
			unsigned long EN7:1;
			unsigned long EN6:1;
			unsigned long EN5:1;
			unsigned long EN4:1;
			unsigned long EN3:1;
			unsigned long EN2:1;
			unsigned long EN1:1;
			unsigned long EN0:1;
		} BIT;
	} GENBE0;
	char           wk19[44];
	union {
		unsigned long LONG;
		struct {
			unsigned long EN31:1;
			unsigned long EN30:1;
			unsigned long EN29:1;
			unsigned long EN28:1;
			unsigned long EN27:1;
			unsigned long EN26:1;
			unsigned long EN25:1;
			unsigned long EN24:1;
			unsigned long EN23:1;
			unsigned long EN22:1;
			unsigned long EN21:1;
			unsigned long EN20:1;
			unsigned long EN19:1;
			unsigned long EN18:1;
			unsigned long EN17:1;
			unsigned long EN16:1;
			unsigned long EN15:1;
			unsigned long EN14:1;
			unsigned long EN13:1;
			unsigned long EN12:1;
			unsigned long EN11:1;
			unsigned long EN10:1;
			unsigned long EN9:1;
			unsigned long EN8:1;
			unsigned long EN7:1;
			unsigned long EN6:1;
			unsigned long EN5:1;
			unsigned long EN4:1;
			unsigned long EN3:1;
			unsigned long EN2:1;
			unsigned long EN1:1;
			unsigned long EN0:1;
		} BIT;
	} GENBL0;
	union {
		unsigned long LONG;
		struct {
			unsigned long EN31:1;
			unsigned long EN30:1;
			unsigned long EN29:1;
			unsigned long EN28:1;
			unsigned long EN27:1;
			unsigned long EN26:1;
			unsigned long EN25:1;
			unsigned long EN24:1;
			unsigned long EN23:1;
			unsigned long EN22:1;
			unsigned long EN21:1;
			unsigned long EN20:1;
			unsigned long EN19:1;
			unsigned long EN18:1;
			unsigned long EN17:1;
			unsigned long EN16:1;
			unsigned long EN15:1;
			unsigned long EN14:1;
			unsigned long EN13:1;
			unsigned long EN12:1;
			unsigned long EN11:1;
			unsigned long EN10:1;
			unsigned long EN9:1;
			unsigned long EN8:1;
			unsigned long EN7:1;
			unsigned long EN6:1;
			unsigned long EN5:1;
			unsigned long EN4:1;
			unsigned long EN3:1;
			unsigned long EN2:1;
			unsigned long EN1:1;
			unsigned long EN0:1;
		} BIT;
	} GENBL1;
	char           wk20[8];
	union {
		unsigned long LONG;
		struct {
			unsigned long CLR31:1;
			unsigned long CLR30:1;
			unsigned long CLR29:1;
			unsigned long CLR28:1;
			unsigned long CLR27:1;
			unsigned long CLR26:1;
			unsigned long CLR25:1;
			unsigned long CLR24:1;
			unsigned long CLR23:1;
			unsigned long CLR22:1;
			unsigned long CLR21:1;
			unsigned long CLR20:1;
			unsigned long CLR19:1;
			unsigned long CLR18:1;
			unsigned long CLR17:1;
			unsigned long CLR16:1;
			unsigned long CLR15:1;
			unsigned long CLR14:1;
			unsigned long CLR13:1;
			unsigned long CLR12:1;
			unsigned long CLR11:1;
			unsigned long CLR10:1;
			unsigned long CLR9:1;
			unsigned long CLR8:1;
			unsigned long CLR7:1;
			unsigned long CLR6:1;
			unsigned long CLR5:1;
			unsigned long CLR4:1;
			unsigned long CLR3:1;
			unsigned long CLR2:1;
			unsigned long CLR1:1;
			unsigned long CLR0:1;
		} BIT;
	} GCRBE0;
	char           wk21[428];
	union {
		unsigned long LONG;
		struct {
			unsigned long IS31:1;
			unsigned long IS30:1;
			unsigned long IS29:1;
			unsigned long IS28:1;
			unsigned long IS27:1;
			unsigned long IS26:1;
			unsigned long IS25:1;
			unsigned long IS24:1;
			unsigned long IS23:1;
			unsigned long IS22:1;
			unsigned long IS21:1;
			unsigned long IS20:1;
			unsigned long IS19:1;
			unsigned long IS18:1;
			unsigned long IS17:1;
			unsigned long IS16:1;
			unsigned long IS15:1;
			unsigned long IS14:1;
			unsigned long IS13:1;
			unsigned long IS12:1;
			unsigned long IS11:1;
			unsigned long IS10:1;
			unsigned long IS9:1;
			unsigned long IS8:1;
			unsigned long IS7:1;
			unsigned long IS6:1;
			unsigned long IS5:1;
			unsigned long IS4:1;
			unsigned long IS3:1;
			unsigned long IS2:1;
			unsigned long IS1:1;
			unsigned long IS0:1;
		} BIT;
	} GRPAL0;
	char           wk22[60];
	union {
		unsigned long LONG;
		struct {
			unsigned long EN31:1;
			unsigned long EN30:1;
			unsigned long EN29:1;
			unsigned long EN28:1;
			unsigned long EN27:1;
			unsigned long EN26:1;
			unsigned long EN25:1;
			unsigned long EN24:1;
			unsigned long EN23:1;
			unsigned long EN22:1;
			unsigned long EN21:1;
			unsigned long EN20:1;
			unsigned long EN19:1;
			unsigned long EN18:1;
			unsigned long EN17:1;
			unsigned long EN16:1;
			unsigned long EN15:1;
			unsigned long EN14:1;
			unsigned long EN13:1;
			unsigned long EN12:1;
			unsigned long EN11:1;
			unsigned long EN10:1;
			unsigned long EN9:1;
			unsigned long EN8:1;
			unsigned long EN7:1;
			unsigned long EN6:1;
			unsigned long EN5:1;
			unsigned long EN4:1;
			unsigned long EN3:1;
			unsigned long EN2:1;
			unsigned long EN1:1;
			unsigned long EN0:1;
		} BIT;
	} GENAL0;
	char           wk23[140];
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR0;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR1;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR2;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR3;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR4;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR5;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR6;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR7;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR8;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR9;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIARA;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIARB;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIARC;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIARD;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIARE;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIARF;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR10;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR11;
	union {
		unsigned char BYTE;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned char PIR7:1;
			unsigned char PIR6:1;
			unsigned char PIR5:1;
			unsigned char PIR4:1;
			unsigned char PIR3:1;
			unsigned char PIR2:1;
			unsigned char PIR1:1;
			unsigned char PIR0:1;
		} BIT;
#endif
	} PIAR12;
	char           wk24[189];
	union {
		unsigned char BYTE;
	} SLIAR208;
	union {
		unsigned char BYTE;
	} SLIAR209;
	union {
		unsigned char BYTE;
	} SLIAR210;
	union {
		unsigned char BYTE;
	} SLIAR211;
	union {
		unsigned char BYTE;
	} SLIAR212;
	union {
		unsigned char BYTE;
	} SLIAR213;
	union {
		unsigned char BYTE;
	} SLIAR214;
	union {
		unsigned char BYTE;
	} SLIAR215;
	union {
		unsigned char BYTE;
	} SLIAR216;
	union {
		unsigned char BYTE;
	} SLIAR217;
	union {
		unsigned char BYTE;
	} SLIAR218;
	union {
		unsigned char BYTE;
	} SLIAR219;
	union {
		unsigned char BYTE;
	} SLIAR220;
	union {
		unsigned char BYTE;
	} SLIAR221;
	union {
		unsigned char BYTE;
	} SLIAR222;
	union {
		unsigned char BYTE;
	} SLIAR223;
	union {
		unsigned char BYTE;
	} SLIAR224;
	union {
		unsigned char BYTE;
	} SLIAR225;
	union {
		unsigned char BYTE;
	} SLIAR226;
	union {
		unsigned char BYTE;
	} SLIAR227;
	union {
		unsigned char BYTE;
	} SLIAR228;
	union {
		unsigned char BYTE;
	} SLIAR229;
	union {
		unsigned char BYTE;
	} SLIAR230;
	union {
		unsigned char BYTE;
	} SLIAR231;
	union {
		unsigned char BYTE;
	} SLIAR232;
	union {
		unsigned char BYTE;
	} SLIAR233;
	union {
		unsigned char BYTE;
	} SLIAR234;
	union {
		unsigned char BYTE;
	} SLIAR235;
	union {
		unsigned char BYTE;
	} SLIAR236;
	union {
		unsigned char BYTE;
	} SLIAR237;
	union {
		unsigned char BYTE;
	} SLIAR238;
	union {
		unsigned char BYTE;
	} SLIAR239;
	union {
		unsigned char BYTE;
	} SLIAR240;
	union {
		unsigned char BYTE;
	} SLIAR241;
	union {
		unsigned char BYTE;
	} SLIAR242;
	union {
		unsigned char BYTE;
	} SLIAR243;
	union {
		unsigned char BYTE;
	} SLIAR244;
	union {
		unsigned char BYTE;
	} SLIAR245;
	union {
		unsigned char BYTE;
	} SLIAR246;
	union {
		unsigned char BYTE;
	} SLIAR247;
	union {
		unsigned char BYTE;
	} SLIAR248;
	union {
		unsigned char BYTE;
	} SLIAR249;
	union {
		unsigned char BYTE;
	} SLIAR250;
	union {
		unsigned char BYTE;
	} SLIAR251;
	union {
		unsigned char BYTE;
	} SLIAR252;
	union {
		unsigned char BYTE;
	} SLIAR253;
	union {
		unsigned char BYTE;
	} SLIAR254;
	union {
		unsigned char BYTE;
	} SLIAR255;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char WPRC:1;
		} BIT;
	} SLIPRCR;
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
			unsigned char :4;
			unsigned char CS3E:1;
			unsigned char CS2E:1;
			unsigned char CS1E:1;
			unsigned char CS0E:1;
		} BIT;
	} PFCSE;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char CS3S:1;
			unsigned char CS2S:2;
			unsigned char CS1S:2;
			unsigned char :1;
			unsigned char CS0S:1;
		} BIT;
	} PFCSS0;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char A15E:1;
			unsigned char A14E:1;
			unsigned char A13E:1;
			unsigned char A12E:1;
			unsigned char A11E:1;
			unsigned char A10E:1;
			unsigned char A9E:1;
			unsigned char A8E:1;
		} BIT;
	} PFAOE0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char A20E:1;
			unsigned char A19E:1;
			unsigned char A18E:1;
			unsigned char A17E:1;
			unsigned char A16E:1;
		} BIT;
	} PFAOE1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char WR1BC1E:1;
			unsigned char :1;
			unsigned char DHE:1;
			unsigned char BCLKO:1;
			unsigned char :1;
			unsigned char ADRHMS:1;
			unsigned char ADRLE:1;
		} BIT;
	} PFBCR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char ALEOE:1;
			unsigned char WAITS:2;
		} BIT;
	} PFBCR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char A0S:2;
			unsigned char :1;
			unsigned char DHS:1;
			unsigned char :2;
		} BIT;
	} PFBCR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char RDS:1;
		} BIT;
	} PFBCR3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ADRHMS2:1;
			unsigned char :1;
			unsigned char ADRLMS:1;
			unsigned char CS3S2:1;
			unsigned char :3;
		} BIT;
	} PFBCR4;
	char           wk2[20];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B0WI:1;
			unsigned char PFSWE:1;
			unsigned char :6;
		} BIT;
	} PWPR;
	char           wk3[32];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P00PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P01PFS;
	char           wk4[6];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P10PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P11PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P12PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P13PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P14PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P15PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P16PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P17PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P20PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P21PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P22PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P23PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P24PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P25PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P26PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P27PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P30PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P31PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P32PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P33PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P34PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P35PFS;
	char           wk5[2];
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
	char           wk6[2];
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
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P64PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char ISEL:1;
			unsigned char :6;
		} BIT;
	} P65PFS;
	char           wk7[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P70PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P71PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P72PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P73PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P74PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P75PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P76PFS;
	char           wk8[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P80PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P81PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P82PFS;
	char           wk9[5];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P90PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P91PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P92PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P93PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P94PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} P95PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} P96PFS;
	char           wk10[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PA0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PA1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PA2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PA3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PA4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PA5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PA6PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PA7PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PB0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PB1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PB2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PB3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PB4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PB5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PB6PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PB7PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PC0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PC1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PC2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PC3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PC4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PC5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PC6PFS;
	char           wk11[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PD0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PD1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PD2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PD3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PD4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PD5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PD6PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PD7PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PE0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PE1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char PSEL:6;
		} BIT;
	} PE2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PE3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PE4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PE5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PE6PFS;
	char           wk12[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PF0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PF1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PF2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PF3PFS;
	char           wk13[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PG0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PG1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PG2PFS;
	char           wk14[5];
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} PH0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} PH1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} PH2PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} PH3PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} PH4PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} PH5PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} PH6PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char ASEL:1;
			unsigned char :7;
		} BIT;
	} PH7PFS;
	char           wk15[8];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PK0PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PK1PFS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char ISEL:1;
			unsigned char PSEL:6;
		} BIT;
	} PK2PFS;
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
			unsigned char :5;
			unsigned char SCC:1;
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
			unsigned char TADSMEN0:1;
			unsigned char :2;
			unsigned char TADSTRS0:5;
		} BIT;
	} TADSTRGR0;
	char           wk24[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char TADSMEN1:1;
			unsigned char :2;
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
			unsigned char :1;
			unsigned char TCFU:1;
			unsigned char TCFV:1;
			unsigned char :2;
			unsigned char TGFB:1;
			unsigned char TGFA:1;
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
			unsigned char :1;
			unsigned char TCFU:1;
			unsigned char TCFV:1;
			unsigned char :2;
			unsigned char TGFB:1;
			unsigned char TGFA:1;
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
			unsigned char :2;
			unsigned char TCFV:1;
			unsigned char TGFD:1;
			unsigned char TGFC:1;
			unsigned char TGFB:1;
			unsigned char TGFA:1;
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
			unsigned char :2;
			unsigned char TCFV:1;
			unsigned char TGFD:1;
			unsigned char TGFC:1;
			unsigned char TGFB:1;
			unsigned char TGFA:1;
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

typedef struct st_ofsm {
	union {
		unsigned long LONG;
		struct {
			unsigned long RDPR:1;
			unsigned long WRPR:1;
			unsigned long SEPR:1;
			unsigned long :1;
			unsigned long SPE:1;
			unsigned long :2;
			unsigned long IDE:1;
			unsigned long :24;
		} BIT;
	} SPCC;
	char           wk0[4];
	union {
		unsigned long LONG;
		struct {
			unsigned long :5;
			unsigned long TMEF:3;
			unsigned long :24;
		} BIT;
	} TMEF;
	char           wk1[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned long ID4:8;
			unsigned long ID3:8;
			unsigned long ID2:8;
			unsigned long ID1:8;
			unsigned long ID8:8;
			unsigned long ID7:8;
			unsigned long ID6:8;
			unsigned long ID5:8;
			unsigned long ID12:8;
			unsigned long ID11:8;
			unsigned long ID10:8;
			unsigned long ID9:8;
			unsigned long ID16:8;
			unsigned long ID15:8;
			unsigned long ID14:8;
			unsigned long ID13:8;
		} BIT;
	} OSIS;
	unsigned long  TMINF;
	union {
		unsigned long LONG;
		struct {
			unsigned long :29;
			unsigned long MDE:3;
		} BIT;
	} MDE;
	union {
		unsigned long LONG;
		struct {
			unsigned long :3;
			unsigned long WDTRSTIRQS:1;
			unsigned long WDTRPSS:2;
			unsigned long WDTRPES:2;
			unsigned long WDTCKS:4;
			unsigned long WDTTOPS:2;
			unsigned long WDTSTRT:1;
			unsigned long :2;
			unsigned long IWDTSLCSTP:1;
			unsigned long :1;
			unsigned long IWDTRSTIRQS:1;
			unsigned long IWDTRPSS:2;
			unsigned long IWDTRPES:2;
			unsigned long IWDTCKS:4;
			unsigned long IWDTTOPS:2;
			unsigned long IWDTSTRT:1;
			unsigned long :1;
		} BIT;
	} OFS0;
	union {
		unsigned long LONG;
		struct {
			unsigned long :23;
			unsigned long HOCOEN:1;
			unsigned long :5;
			unsigned long LVDAS:1;
			unsigned long VDSEL:2;
		} BIT;
	} OFS1;
	char           wk2[12];
	union {
		unsigned long LONG;
		struct {
			unsigned long CODE:32;
		} BIT;
	} ROMCODE;
} st_ofsm_t;

typedef struct st_poe {
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE0F:1;
			unsigned short :3;
			unsigned short PIE1:1;
			unsigned short POE0M2:4;
			unsigned short POE0M:4;
		} BIT;
	} ICSR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short OSF1:1;
			unsigned short :5;
			unsigned short OCE1:1;
			unsigned short OIE1:1;
			unsigned short :8;
		} BIT;
	} OCSR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE4F:1;
			unsigned short :3;
			unsigned short PIE2:1;
			unsigned short POE4M2:4;
			unsigned short POE4M:4;
		} BIT;
	} ICSR2;
	union {
		unsigned short WORD;
		struct {
			unsigned short OSF2:1;
			unsigned short :5;
			unsigned short OCE2:1;
			unsigned short OIE2:1;
			unsigned short :8;
		} BIT;
	} OCSR2;
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE8F:1;
			unsigned short :2;
			unsigned short POE8E:1;
			unsigned short PIE3:1;
			unsigned short POE8M2:4;
			unsigned short POE8M:4;
		} BIT;
	} ICSR3;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
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
			unsigned short :6;
			unsigned short GPT9ABZE:1;
			unsigned short GPT8ABZE:1;
			unsigned short GPT7ABZE:1;
			unsigned short GPT6ABZE:1;
			unsigned short GPT5ABZE:1;
			unsigned short GPT4ABZE:1;
			unsigned short GPT3ABZE:1;
			unsigned short GPT2ABZE:1;
			unsigned short GPT1ABZE:1;
			unsigned short GPT0ABZE:1;
		} BIT;
	} POECR3;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDMT34ZE:1;
			unsigned short IC9ADDMT34ZE:1;
			unsigned short IC8ADDMT34ZE:1;
			unsigned short :1;
			unsigned short IC6ADDMT34ZE:1;
			unsigned short IC5ADDMT34ZE:1;
			unsigned short IC4ADDMT34ZE:1;
			unsigned short IC3ADDMT34ZE:1;
			unsigned short IC2ADDMT34ZE:1;
			unsigned short IC1ADDMT34ZE:1;
			unsigned short CMADDMT34ZE:1;
		} BIT;
	} POECR4;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDMT0ZE:1;
			unsigned short IC9ADDMT0ZE:1;
			unsigned short IC8ADDMT0ZE:1;
			unsigned short :1;
			unsigned short IC6ADDMT0ZE:1;
			unsigned short IC5ADDMT0ZE:1;
			unsigned short IC4ADDMT0ZE:1;
			unsigned short IC3ADDMT0ZE:1;
			unsigned short IC2ADDMT0ZE:1;
			unsigned short IC1ADDMT0ZE:1;
			unsigned short CMADDMT0ZE:1;
		} BIT;
	} POECR5;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDGPT01ZE:1;
			unsigned short IC9ADDGPT01ZE:1;
			unsigned short IC8ADDGPT01ZE:1;
			unsigned short :1;
			unsigned short IC6ADDGPT01ZE:1;
			unsigned short IC5ADDGPT01ZE:1;
			unsigned short IC4ADDGPT01ZE:1;
			unsigned short IC3ADDGPT01ZE:1;
			unsigned short IC2ADDGPT01ZE:1;
			unsigned short IC1ADDGPT01ZE:1;
			unsigned short CMADDGPT01ZE:1;
		} BIT;
	} POECR6;
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE10F:1;
			unsigned short :2;
			unsigned short POE10E:1;
			unsigned short PIE4:1;
			unsigned short POE10M2:4;
			unsigned short POE10M:4;
		} BIT;
	} ICSR4;
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE11F:1;
			unsigned short :2;
			unsigned short POE11E:1;
			unsigned short PIE5:1;
			unsigned short POE11M2:4;
			unsigned short POE11M:4;
		} BIT;
	} ICSR5;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short OLSEN:1;
			unsigned short :1;
			unsigned short OLSG2B:1;
			unsigned short OLSG2A:1;
			unsigned short OLSG1B:1;
			unsigned short OLSG1A:1;
			unsigned short OLSG0B:1;
			unsigned short OLSG0A:1;
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
			unsigned short :8;
			unsigned short OLSEN:1;
			unsigned short :1;
			unsigned short OLSG6B:1;
			unsigned short OLSG6A:1;
			unsigned short OLSG5B:1;
			unsigned short OLSG5A:1;
			unsigned short OLSG4B:1;
			unsigned short OLSG4A:1;
		} BIT;
	} ALR2;
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE12F:1;
			unsigned short :2;
			unsigned short POE12E:1;
			unsigned short PIE7:1;
			unsigned short POE12M2:4;
			unsigned short POE12M:4;
		} BIT;
	} ICSR7;
	union {
		unsigned short WORD;
		struct {
			unsigned short :12;
			unsigned short MTU9DZE:1;
			unsigned short MTU9CZE:1;
			unsigned short MTU9BZE:1;
			unsigned short MTU9AZE:1;
		} BIT;
	} POECR7;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDMT9ZE:1;
			unsigned short IC9ADDMT9ZE:1;
			unsigned short IC8ADDMT9ZE:1;
			unsigned short :1;
			unsigned short IC6ADDMT9ZE:1;
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
			unsigned short :10;
			unsigned short C5FLAG:1;
			unsigned short C4FLAG:1;
			unsigned short C3FLAG:1;
			unsigned short C2FLAG:1;
			unsigned short C1FLAG:1;
			unsigned short C0FLAG:1;
		} BIT;
	} POECMPFR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :10;
			unsigned short POEREQ5:1;
			unsigned short POEREQ4:1;
			unsigned short POEREQ3:1;
			unsigned short POEREQ2:1;
			unsigned short POEREQ1:1;
			unsigned short POEREQ0:1;
		} BIT;
	} POECMPSEL;
	union {
		unsigned short WORD;
		struct {
			unsigned short OSF3:1;
			unsigned short :5;
			unsigned short OCE3:1;
			unsigned short OIE3:1;
			unsigned short :8;
		} BIT;
	} OCSR3;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short OLSEN:1;
			unsigned short :1;
			unsigned short OLSG2B:1;
			unsigned short OLSG2A:1;
			unsigned short OLSG1B:1;
			unsigned short OLSG1A:1;
			unsigned short OLSG0B:1;
			unsigned short OLSG0A:1;
		} BIT;
	} ALR3;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short GPT79HIZ:1;
			unsigned short GPT46HIZ:1;
			unsigned short GPT02HIZ:1;
			unsigned short :1;
			unsigned short MTUCH9HIZ:1;
			unsigned short :1;
			unsigned short GPT23HIZ:1;
			unsigned short GPT01HIZ:1;
			unsigned short MTUCH0HIZ:1;
			unsigned short MTUCH67HIZ:1;
			unsigned short MTUCH34HIZ:1;
		} BIT;
	} SPOER;
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short MTU9DME:1;
			unsigned short MTU9CME:1;
			unsigned short MTU9BME:1;
			unsigned short MTU9AME:1;
			unsigned short :4;
			unsigned short MTU0DME:1;
			unsigned short MTU0CME:1;
			unsigned short MTU0BME:1;
			unsigned short MTU0AME:1;
		} BIT;
	} PMMCR0;
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
			unsigned short GPT7BME:1;
			unsigned short GPT7AME:1;
			unsigned short GPT6BME:1;
			unsigned short GPT6AME:1;
			unsigned short GPT5BME:1;
			unsigned short GPT5AME:1;
			unsigned short GPT4BME:1;
			unsigned short GPT4AME:1;
			unsigned short GPT3BME:1;
			unsigned short GPT3AME:1;
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
			unsigned short :12;
			unsigned short GPT9BME:1;
			unsigned short GPT9AME:1;
			unsigned short GPT8BME:1;
			unsigned short GPT8AME:1;
		} BIT;
	} PMMCR3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX4;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX5;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE9F:1;
			unsigned short :2;
			unsigned short POE9E:1;
			unsigned short PIE8:1;
			unsigned short POE9M2:4;
			unsigned short POE9M:4;
		} BIT;
	} ICSR8;
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE13F:1;
			unsigned short :2;
			unsigned short POE13E:1;
			unsigned short PIE9:1;
			unsigned short POE13M2:4;
			unsigned short POE13M:4;
		} BIT;
	} ICSR9;
	union {
		unsigned short WORD;
		struct {
			unsigned short :3;
			unsigned short POE14F:1;
			unsigned short :2;
			unsigned short POE14E:1;
			unsigned short PIE10:1;
			unsigned short POE14M2:4;
			unsigned short POE14M:4;
		} BIT;
	} ICSR10;
	union {
		unsigned short WORD;
		struct {
			unsigned short OSF4:1;
			unsigned short :5;
			unsigned short OCE4:1;
			unsigned short OIE4:1;
			unsigned short :8;
		} BIT;
	} OCSR4;
	union {
		unsigned short WORD;
		struct {
			unsigned short OSF5:1;
			unsigned short :5;
			unsigned short OCE5:1;
			unsigned short OIE5:1;
			unsigned short :8;
		} BIT;
	} OCSR5;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short OLSEN:1;
			unsigned short :1;
			unsigned short OLSG2B:1;
			unsigned short OLSG2A:1;
			unsigned short OLSG1B:1;
			unsigned short OLSG1A:1;
			unsigned short OLSG0B:1;
			unsigned short OLSG0A:1;
		} BIT;
	} ALR4;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short OLSEN:1;
			unsigned short :1;
			unsigned short OLSG2B:1;
			unsigned short OLSG2A:1;
			unsigned short OLSG1B:1;
			unsigned short OLSG1A:1;
			unsigned short OLSG0B:1;
			unsigned short OLSG0A:1;
		} BIT;
	} ALR5;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDMT67ZE:1;
			unsigned short IC9ADDMT67ZE:1;
			unsigned short IC8ADDMT67ZE:1;
			unsigned short :1;
			unsigned short IC6ADDMT67ZE:1;
			unsigned short IC5ADDMT67ZE:1;
			unsigned short IC4ADDMT67ZE:1;
			unsigned short IC3ADDMT67ZE:1;
			unsigned short IC2ADDMT67ZE:1;
			unsigned short IC1ADDMT67ZE:1;
			unsigned short CMADDMT67ZE:1;
		} BIT;
	} POECR4B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDGPT23ZE:1;
			unsigned short IC9ADDGPT23ZE:1;
			unsigned short IC8ADDGPT23ZE:1;
			unsigned short :1;
			unsigned short IC6ADDGPT23ZE:1;
			unsigned short IC5ADDGPT23ZE:1;
			unsigned short IC4ADDGPT23ZE:1;
			unsigned short IC3ADDGPT23ZE:1;
			unsigned short IC2ADDGPT23ZE:1;
			unsigned short IC1ADDGPT23ZE:1;
			unsigned short CMADDGPT23ZE:1;
		} BIT;
	} POECR6B;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDGPT02ZE:1;
			unsigned short IC9ADDGPT02ZE:1;
			unsigned short IC8ADDGPT02ZE:1;
			unsigned short :1;
			unsigned short IC6ADDGPT02ZE:1;
			unsigned short IC5ADDGPT02ZE:1;
			unsigned short IC4ADDGPT02ZE:1;
			unsigned short IC3ADDGPT02ZE:1;
			unsigned short IC2ADDGPT02ZE:1;
			unsigned short IC1ADDGPT02ZE:1;
			unsigned short CMADDGPT02ZE:1;
		} BIT;
	} POECR9;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDGPT46ZE:1;
			unsigned short IC9ADDGPT46ZE:1;
			unsigned short IC8ADDGPT46ZE:1;
			unsigned short :1;
			unsigned short IC6ADDGPT46ZE:1;
			unsigned short IC5ADDGPT46ZE:1;
			unsigned short IC4ADDGPT46ZE:1;
			unsigned short IC3ADDGPT46ZE:1;
			unsigned short IC2ADDGPT46ZE:1;
			unsigned short IC1ADDGPT46ZE:1;
			unsigned short CMADDGPT46ZE:1;
		} BIT;
	} POECR10;
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short IC10ADDGPT79ZE:1;
			unsigned short IC9ADDGPT79ZE:1;
			unsigned short IC8ADDGPT79ZE:1;
			unsigned short :1;
			unsigned short IC6ADDGPT79ZE:1;
			unsigned short IC5ADDGPT79ZE:1;
			unsigned short IC4ADDGPT79ZE:1;
			unsigned short IC3ADDGPT79ZE:1;
			unsigned short IC2ADDGPT79ZE:1;
			unsigned short IC1ADDGPT79ZE:1;
			unsigned short CMADDGPT79ZE:1;
		} BIT;
	} POECR11;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX6;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX7;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char POEREQ5:1;
			unsigned char POEREQ4:1;
			unsigned char POEREQ3:1;
			unsigned char POEREQ2:1;
			unsigned char POEREQ1:1;
			unsigned char POEREQ0:1;
		} BIT;
	} POECMPEX8;
	char           wk2[5];
	union {
		unsigned char BYTE;
		struct {
			unsigned char M0BSEL:4;
			unsigned char M0ASEL:4;
		} BIT;
	} M0SELR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M0DSEL:4;
			unsigned char M0CSEL:4;
		} BIT;
	} M0SELR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M3DSEL:4;
			unsigned char M3BSEL:4;
		} BIT;
	} M3SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M4CSEL:4;
			unsigned char M4ASEL:4;
		} BIT;
	} M4SELR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M4DSEL:4;
			unsigned char M4BSEL:4;
		} BIT;
	} M4SELR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M6DSEL:4;
			unsigned char M6BSEL:4;
		} BIT;
	} M6SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M7CSEL:4;
			unsigned char M7ASEL:4;
		} BIT;
	} M7SELR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M7DSEL:4;
			unsigned char M7BSEL:4;
		} BIT;
	} M7SELR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M9BSEL:4;
			unsigned char M9ASEL:4;
		} BIT;
	} M9SELR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char M9DSEL:4;
			unsigned char M9CSEL:4;
		} BIT;
	} M9SELR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G0BSEL:4;
			unsigned char G0ASEL:4;
		} BIT;
	} G0SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G1BSEL:4;
			unsigned char G1ASEL:4;
		} BIT;
	} G1SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G2BSEL:4;
			unsigned char G2ASEL:4;
		} BIT;
	} G2SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G3BSEL:4;
			unsigned char G3ASEL:4;
		} BIT;
	} G3SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G4BSEL:4;
			unsigned char G4ASEL:4;
		} BIT;
	} G4SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G5BSEL:4;
			unsigned char G5ASEL:4;
		} BIT;
	} G5SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G6BSEL:4;
			unsigned char G6ASEL:4;
		} BIT;
	} G6SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G7BSEL:4;
			unsigned char G7ASEL:4;
		} BIT;
	} G7SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G8BSEL:4;
			unsigned char G8ASEL:4;
		} BIT;
	} G8SELR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char G9BSEL:4;
			unsigned char G9ASEL:4;
		} BIT;
	} G9SELR;
} st_poe_t;

typedef struct st_poeg {
	union {
		unsigned long LONG;
		struct {
			unsigned long NFCS:2;
			unsigned long NFEN:1;
			unsigned long INV:1;
			unsigned long :11;
			unsigned long ST:1;
			unsigned long :2;
			unsigned long CDRE5:1;
			unsigned long CDRE4:1;
			unsigned long CDRE3:1;
			unsigned long CDRE2:1;
			unsigned long CDRE1:1;
			unsigned long CDRE0:1;
			unsigned long :1;
			unsigned long OSTPE:1;
			unsigned long IOCE:1;
			unsigned long PIDE:1;
			unsigned long SSF:1;
			unsigned long OSTPF:1;
			unsigned long IOCF:1;
			unsigned long PIDF:1;
		} BIT;
	} POEGGA;
	char           wk0[60];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRKEY:8;
			unsigned short :7;
			unsigned short WP:1;
		} BIT;
	} GTONCWPA;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :7;
			unsigned short NFV:1;
			unsigned short NFS:4;
			unsigned short :3;
			unsigned short NE:1;
		} BIT;
	} GTONCCRA;
	char           wk2[186];
	union {
		unsigned long LONG;
		struct {
			unsigned long NFCS:2;
			unsigned long NFEN:1;
			unsigned long INV:1;
			unsigned long :11;
			unsigned long ST:1;
			unsigned long :2;
			unsigned long CDRE5:1;
			unsigned long CDRE4:1;
			unsigned long CDRE3:1;
			unsigned long CDRE2:1;
			unsigned long CDRE1:1;
			unsigned long CDRE0:1;
			unsigned long :1;
			unsigned long OSTPE:1;
			unsigned long IOCE:1;
			unsigned long PIDE:1;
			unsigned long SSF:1;
			unsigned long OSTPF:1;
			unsigned long IOCF:1;
			unsigned long PIDF:1;
		} BIT;
	} POEGGB;
	char           wk3[60];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRKEY:8;
			unsigned short :7;
			unsigned short WP:1;
		} BIT;
	} GTONCWPB;
	char           wk4[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :7;
			unsigned short NFV:1;
			unsigned short NFS:4;
			unsigned short :3;
			unsigned short NE:1;
		} BIT;
	} GTONCCRB;
	char           wk5[186];
	union {
		unsigned long LONG;
		struct {
			unsigned long NFCS:2;
			unsigned long NFEN:1;
			unsigned long INV:1;
			unsigned long :11;
			unsigned long ST:1;
			unsigned long :2;
			unsigned long CDRE5:1;
			unsigned long CDRE4:1;
			unsigned long CDRE3:1;
			unsigned long CDRE2:1;
			unsigned long CDRE1:1;
			unsigned long CDRE0:1;
			unsigned long :1;
			unsigned long OSTPE:1;
			unsigned long IOCE:1;
			unsigned long PIDE:1;
			unsigned long SSF:1;
			unsigned long OSTPF:1;
			unsigned long IOCF:1;
			unsigned long PIDF:1;
		} BIT;
	} POEGGC;
	char           wk6[60];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRKEY:8;
			unsigned short :7;
			unsigned short WP:1;
		} BIT;
	} GTONCWPC;
	char           wk7[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :7;
			unsigned short NFV:1;
			unsigned short NFS:4;
			unsigned short :3;
			unsigned short NE:1;
		} BIT;
	} GTONCCRC;
	char           wk8[186];
	union {
		unsigned long LONG;
		struct {
			unsigned long NFCS:2;
			unsigned long NFEN:1;
			unsigned long INV:1;
			unsigned long :11;
			unsigned long ST:1;
			unsigned long :2;
			unsigned long CDRE5:1;
			unsigned long CDRE4:1;
			unsigned long CDRE3:1;
			unsigned long CDRE2:1;
			unsigned long CDRE1:1;
			unsigned long CDRE0:1;
			unsigned long :1;
			unsigned long OSTPE:1;
			unsigned long IOCE:1;
			unsigned long PIDE:1;
			unsigned long SSF:1;
			unsigned long OSTPF:1;
			unsigned long IOCF:1;
			unsigned long PIDF:1;
		} BIT;
	} POEGGD;
	char           wk9[60];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRKEY:8;
			unsigned short :7;
			unsigned short WP:1;
		} BIT;
	} GTONCWPD;
	char           wk10[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :7;
			unsigned short NFV:1;
			unsigned short NFS:4;
			unsigned short :3;
			unsigned short NE:1;
		} BIT;
	} GTONCCRD;
} st_poeg_t;

typedef struct st_port0 {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PMR;
	char           wk3[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR0;
	char           wk4[63];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
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
			unsigned char :3;
			unsigned char B3:1;
			unsigned char :3;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char :3;
			unsigned char B3:1;
			unsigned char :3;
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
	char           wk3[35];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :6;
		} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :6;
		} BIT;
	} ODR1;
	char           wk4[58];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char :3;
			unsigned char B3:1;
			unsigned char :3;
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
	char           wk2[31];
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
	} PMR;
	char           wk3[36];
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
			unsigned char :5;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[57];
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
	char           wk2[31];
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
	} PMR;
	char           wk3[37];
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
			unsigned char :5;
			unsigned char B2:1;
			unsigned char :1;
			unsigned char B0:1;
		} BIT;
	} ODR1;
	char           wk4[56];
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
	char           wk6[71];
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
			unsigned char :1;
		} BIT;
	} DSCR2;
} st_port7_t;

typedef struct st_port8 {
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
	char           wk3[39];
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
	char           wk4[55];
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
	char           wk6[71];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char B1:1;
			unsigned char :1;
		} BIT;
	} DSCR2;
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
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
	char           wk6[71];
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
	} DSCR2;
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
			unsigned char B5:1;
			unsigned char B4:1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
	char           wk6[71];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char B5:1;
			unsigned char :5;
		} BIT;
	} DSCR2;
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
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char B0:1;
		} BIT;
	} DSCR;
	char           wk6[71];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char B3:1;
			unsigned char :3;
		} BIT;
	} DSCR2;
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
			unsigned char :1;
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
			unsigned char :1;
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
			unsigned char :1;
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

typedef struct st_porth {
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char :1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char :1;
		} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char :1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char :1;
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
	char           wk3[48];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char B6:1;
			unsigned char :1;
			unsigned char B4:1;
			unsigned char :1;
			unsigned char B2:1;
			unsigned char :2;
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
			unsigned char :2;
		} BIT;
	} ODR1;
	char           wk4[45];
	union {
		unsigned char BYTE;
		struct {
			unsigned char B7:1;
			unsigned char B6:1;
			unsigned char B5:1;
			unsigned char :1;
			unsigned char B3:1;
			unsigned char B2:1;
			unsigned char B1:1;
			unsigned char :1;
		} BIT;
	} PCR;
} st_porth_t;

typedef struct st_portk {
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
	char           wk3[50];
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
	char           wk4[44];
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
} st_portk_t;

typedef struct st_ram {
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char RAMMODE:2;
		} BIT;
	} RAMMODE;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char RAMERR:1;
		} BIT;
	} RAMSTS;
	char           wk0[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char KW:7;
			unsigned char RAMPRCR:1;
		} BIT;
	} RAMPRCR;
	char           wk1[3];
	union {
		unsigned long LONG;
		struct {
			unsigned long :13;
			unsigned long READ:16;
			unsigned long :3;
		} BIT;
	} RAMECAD;
	char           wk2[180];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char RAMMOD:2;
		} BIT;
	} ECCRAMMODE;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char ECC2ERR:1;
		} BIT;
	} ECCRAM2STS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char ECC1STSEN:1;
		} BIT;
	} ECCRAM1STSEN;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char ECC1ERR:1;
		} BIT;
	} ECCRAM1STS;
	union {
		unsigned char BYTE;
		struct {
			unsigned char KW:7;
			unsigned char PRCR:1;
		} BIT;
	} ECCRAMPRCR;
	char           wk3[3];
	union {
		unsigned long LONG;
		struct {
			unsigned long :17;
			unsigned long ECC2EAD:12;
			unsigned long :3;
		} BIT;
	} ECCRAM2ECAD;
	union {
		unsigned long LONG;
		struct {
			unsigned long :17;
			unsigned long ECC1EAD:12;
			unsigned long :3;
		} BIT;
	} ECCRAM1ECAD;
	union {
		unsigned char BYTE;
		struct {
			unsigned char KW2:7;
			unsigned char PRCR2:1;
		} BIT;
	} ECCRAMPRCR2;
	char           wk4[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char TSTBYP:1;
		} BIT;
	} ECCRAMETST;
} st_ram_t;

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
		struct {
			unsigned char HH;
		} BYTE;
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
			unsigned char :1;
			unsigned char SPBYT:1;
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
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char BYSW:1;
		} BIT;
	} SPDCR2;
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
			unsigned short :8;
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
			unsigned short :8;
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
	char           wk4[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
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
	char           wk5[2];
	union {
		unsigned short WORD;
	} ADDBLDR;
	char           wk6[4];
	union {
		unsigned short WORD;
		union {
			struct {
				unsigned short DIAGST:2;
				unsigned short :2;
				unsigned short AD:12;
			} RIGHT;
			struct {
				unsigned short AD:12;
				unsigned short :2;
				unsigned short DIAGST:2;
			} LEFT;
		} BIT;
	} ADRD;
	unsigned short ADDR0;
	unsigned short ADDR1;
	unsigned short ADDR2;
	unsigned short ADDR3;
	unsigned short ADDR4;
	unsigned short ADDR5;
	unsigned short ADDR6;
	unsigned short ADDR7;
	char           wk7[54];
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short SHANS:3;
			unsigned short SSTSH:8;
		} BIT;
	} ADSHCR;
	char           wk8[18];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char ADNDIS:5;
		} BIT;
	} ADDISCR;
	char           wk9[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char SHMD:1;
		} BIT;
	} ADSHMSR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char ELCC:3;
		} BIT;
	} ADELCCR;
	char           wk10[2];
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
	char           wk11[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk12[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char MONCMPB:1;
			unsigned char MONCMPA:1;
			unsigned char :3;
			unsigned char MONCOMB:1;
		} BIT;
	} ADWINMON;
	char           wk13[3];
	union {
		unsigned short WORD;
		struct {
			unsigned short CMPAIE:1;
			unsigned short WCMPE:1;
			unsigned short CMPBIE:1;
			unsigned short :1;
			unsigned short CMPAE:1;
			unsigned short :1;
			unsigned short CMPBE:1;
			unsigned short :7;
			unsigned short CMPAB:2;
		} BIT;
	} ADCMPCR;
	char           wk14[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short CMPCHA007:1;
			unsigned short CMPCHA006:1;
			unsigned short CMPCHA005:1;
			unsigned short CMPCHA004:1;
			unsigned short CMPCHA003:1;
			unsigned short CMPCHA002:1;
			unsigned short CMPCHA001:1;
			unsigned short CMPCHA000:1;
		} BIT;
	} ADCMPANSR0;
	char           wk15[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short CMPLCHA007:1;
			unsigned short CMPLCHA006:1;
			unsigned short CMPLCHA005:1;
			unsigned short CMPLCHA004:1;
			unsigned short CMPLCHA003:1;
			unsigned short CMPLCHA002:1;
			unsigned short CMPLCHA001:1;
			unsigned short CMPLCHA000:1;
		} BIT;
	} ADCMPLR0;
	char           wk16[2];
	unsigned short ADCMPDR0;
	unsigned short ADCMPDR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short CMPSTCHA007:1;
			unsigned short CMPSTCHA006:1;
			unsigned short CMPSTCHA005:1;
			unsigned short CMPSTCHA004:1;
			unsigned short CMPSTCHA003:1;
			unsigned short CMPSTCHA002:1;
			unsigned short CMPSTCHA001:1;
			unsigned short CMPSTCHA000:1;
		} BIT;
	} ADCMPSR0;
	char           wk17[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CMPLB:1;
			unsigned char :1;
			unsigned char CMPCHB:6;
		} BIT;
	} ADCMPBNSR;
	char           wk18[1];
	unsigned short ADWINLLB;
	unsigned short ADWINULB;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CMPSTB:1;
		} BIT;
	} ADCMPBSR;
	char           wk19[39];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
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
	char           wk20[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char GRCE:1;
			unsigned char GCADIE:1;
			unsigned char TRSC:6;
		} BIT;
	} ADGCTRGR;
	char           wk21[6];
	unsigned char  ADSSTR0;
	unsigned char  ADSSTR1;
	unsigned char  ADSSTR2;
	unsigned char  ADSSTR3;
	unsigned char  ADSSTR4;
	unsigned char  ADSSTR5;
	unsigned char  ADSSTR6;
	unsigned char  ADSSTR7;
	char           wk22[184];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short P002CR:4;
			unsigned short P001CR:4;
			unsigned short P000CR:4;
		} BIT;
	} ADPGACR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short P002GAIN:4;
			unsigned short P001GAIN:4;
			unsigned short P000GAIN:4;
		} BIT;
	} ADPGAGS0;
	char           wk23[12];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short P002DEN:1;
			unsigned short :1;
			unsigned short P002DG:2;
			unsigned short P001DEN:1;
			unsigned short :1;
			unsigned short P001DG:2;
			unsigned short P000DEN:1;
			unsigned short :1;
			unsigned short P000DG:2;
		} BIT;
	} ADPGADCR0;
	char           wk24[14];
	unsigned char  ADSCS0;
	unsigned char  ADSCS1;
	unsigned char  ADSCS2;
	unsigned char  ADSCS3;
	unsigned char  ADSCS4;
	unsigned char  ADSCS5;
	unsigned char  ADSCS6;
	unsigned char  ADSCS7;
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
			unsigned short :8;
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
			unsigned short :8;
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
	char           wk4[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
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
	char           wk5[2];
	unsigned short ADDBLDR;
	char           wk6[4];
	union {
		unsigned short WORD;
		union {
			struct {
				unsigned short DIAGST:2;
				unsigned short :2;
				unsigned short AD:12;
			} RIGHT;
			struct {
				unsigned short AD:12;
				unsigned short :2;
				unsigned short DIAGST:2;
			} LEFT;
		} BIT;
	} ADRD;
	unsigned short ADDR0;
	unsigned short ADDR1;
	unsigned short ADDR2;
	unsigned short ADDR3;
	unsigned short ADDR4;
	unsigned short ADDR5;
	unsigned short ADDR6;
	unsigned short ADDR7;
	char           wk7[54];
	union {
		unsigned short WORD;
		struct {
			unsigned short :5;
			unsigned short SHANS:3;
			unsigned short SSTSH:8;
		} BIT;
	} ADSHCR;
	char           wk8[18];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char ADNDIS:5;
		} BIT;
	} ADDISCR;
	char           wk9[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char SHMD:1;
		} BIT;
	} ADSHMSR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char ELCC:3;
		} BIT;
	} ADELCCR;
	char           wk10[2];
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
	char           wk11[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk12[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char MONCMPB:1;
			unsigned char MONCMPA:1;
			unsigned char :3;
			unsigned char MONCOMB:1;
		} BIT;
	} ADWINMON;
	char           wk13[3];
	union {
		unsigned short WORD;
		struct {
			unsigned short CMPAIE:1;
			unsigned short WCMPE:1;
			unsigned short CMPBIE:1;
			unsigned short :1;
			unsigned short CMPAE:1;
			unsigned short :1;
			unsigned short CMPBE:1;
			unsigned short :7;
			unsigned short CMPAB:2;
		} BIT;
	} ADCMPCR;
	char           wk14[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short CMPCHA007:1;
			unsigned short CMPCHA006:1;
			unsigned short CMPCHA005:1;
			unsigned short CMPCHA004:1;
			unsigned short CMPCHA003:1;
			unsigned short CMPCHA002:1;
			unsigned short CMPCHA001:1;
			unsigned short CMPCHA000:1;
		} BIT;
	} ADCMPANSR0;
	char           wk15[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short CMPLCHA007:1;
			unsigned short CMPLCHA006:1;
			unsigned short CMPLCHA005:1;
			unsigned short CMPLCHA004:1;
			unsigned short CMPLCHA003:1;
			unsigned short CMPLCHA002:1;
			unsigned short CMPLCHA001:1;
			unsigned short CMPLCHA000:1;
		} BIT;
	} ADCMPLR0;
	char           wk16[2];
	unsigned short ADCMPDR0;
	unsigned short ADCMPDR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short CMPSTCHA007:1;
			unsigned short CMPSTCHA006:1;
			unsigned short CMPSTCHA005:1;
			unsigned short CMPSTCHA004:1;
			unsigned short CMPSTCHA003:1;
			unsigned short CMPSTCHA002:1;
			unsigned short CMPSTCHA001:1;
			unsigned short CMPSTCHA000:1;
		} BIT;
	} ADCMPSR0;
	char           wk17[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CMPLB:1;
			unsigned char :1;
			unsigned char CMPCHB:6;
		} BIT;
	} ADCMPBNSR;
	char           wk18[1];
	unsigned short ADWINLLB;
	unsigned short ADWINULB;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CMPSTB:1;
		} BIT;
	} ADCMPBSR;
	char           wk19[39];
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
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
	char           wk20[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char GRCE:1;
			unsigned char GCADIE:1;
			unsigned char TRSC:6;
		} BIT;
	} ADGCTRGR;
	char           wk21[6];
	unsigned char  ADSSTR0;
	unsigned char  ADSSTR1;
	unsigned char  ADSSTR2;
	unsigned char  ADSSTR3;
	unsigned char  ADSSTR4;
	unsigned char  ADSSTR5;
	unsigned char  ADSSTR6;
	unsigned char  ADSSTR7;
	char           wk22[184];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short P102CR:4;
			unsigned short P101CR:4;
			unsigned short P100CR:4;
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
	char           wk23[12];
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short P102DEN:1;
			unsigned short :1;
			unsigned short P102DG:2;
			unsigned short P101DEN:1;
			unsigned short :1;
			unsigned short P101DG:2;
			unsigned short P100DEN:1;
			unsigned short :1;
			unsigned short P100DG:2;
		} BIT;
	} ADPGADCR0;
	char           wk24[14];
	unsigned char  ADSCS0;
	unsigned char  ADSCS1;
	unsigned char  ADSCS2;
	unsigned char  ADSCS3;
	unsigned char  ADSCS4;
	unsigned char  ADSCS5;
	unsigned char  ADSCS6;
	unsigned char  ADSCS7;
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
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short ANSA101:1;
			unsigned short ANSA100:1;
		} BIT;
	} ADANSA1;
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
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short ADS101:1;
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
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short OCSB:1;
			unsigned short TSSB:1;
			unsigned short OCSA:1;
			unsigned short TSSA:1;
			unsigned short :6;
			unsigned short OCSAD:1;
			unsigned short TSSAD:1;
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
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short ANSB101:1;
			unsigned short ANSB100:1;
		} BIT;
	} ADANSB1;
	unsigned short ADDBLDR;
	unsigned short ADTSDR;
	unsigned short ADOCDR;
	union {
		unsigned short WORD;
		union {
			struct {
				unsigned short DIAGST:2;
				unsigned short :2;
				unsigned short AD:12;
			} RIGHT;
			struct {
				unsigned short AD:12;
				unsigned short :2;
				unsigned short DIAGST:2;
			} LEFT;
		} BIT;
	} ADRD;
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
	char           wk2[8];
	unsigned short ADDR16;
	unsigned short ADDR17;
	char           wk3[54];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char ADNDIS:5;
		} BIT;
	} ADDISCR;
	char           wk4[2];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char ELCC:3;
		} BIT;
	} ADELCCR;
	char           wk5[2];
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
	char           wk6[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk7[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char MONCMPB:1;
			unsigned char MONCMPA:1;
			unsigned char :3;
			unsigned char MONCOMB:1;
		} BIT;
	} ADWINMON;
	char           wk8[3];
	union {
		unsigned short WORD;
		struct {
			unsigned short CMPAIE:1;
			unsigned short WCMPE:1;
			unsigned short CMPBIE:1;
			unsigned short :1;
			unsigned short CMPAE:1;
			unsigned short :1;
			unsigned short CMPBE:1;
			unsigned short :7;
			unsigned short CMPAB:2;
		} BIT;
	} ADCMPCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char CMPSOC:1;
			unsigned char CMPSTS:1;
		} BIT;
	} ADCMPANSER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char CMPLOC:1;
			unsigned char CMPLTS:1;
		} BIT;
	} ADCMPLER;
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short CMPCHA011:1;
			unsigned short CMPCHA010:1;
			unsigned short CMPCHA009:1;
			unsigned short CMPCHA008:1;
			unsigned short CMPCHA007:1;
			unsigned short CMPCHA006:1;
			unsigned short CMPCHA005:1;
			unsigned short CMPCHA004:1;
			unsigned short CMPCHA003:1;
			unsigned short CMPCHA002:1;
			unsigned short CMPCHA001:1;
			unsigned short CMPCHA000:1;
		} BIT;
	} ADCMPANSR0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short CMPCHA101:1;
			unsigned short CMPCHA100:1;
		} BIT;
	} ADCMPANSR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short CMPLCHA011:1;
			unsigned short CMPLCHA010:1;
			unsigned short CMPLCHA009:1;
			unsigned short CMPLCHA008:1;
			unsigned short CMPLCHA007:1;
			unsigned short CMPLCHA006:1;
			unsigned short CMPLCHA005:1;
			unsigned short CMPLCHA004:1;
			unsigned short CMPLCHA003:1;
			unsigned short CMPLCHA002:1;
			unsigned short CMPLCHA001:1;
			unsigned short CMPLCHA000:1;
		} BIT;
	} ADCMPLR0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short CMPLCHA101:1;
			unsigned short CMPLCHA100:1;
		} BIT;
	} ADCMPLR1;
	unsigned short ADCMPDR0;
	unsigned short ADCMPDR1;
	union {
		unsigned short WORD;
		struct {
			unsigned short :4;
			unsigned short CMPSTCHA011:1;
			unsigned short CMPSTCHA010:1;
			unsigned short CMPSTCHA009:1;
			unsigned short CMPSTCHA008:1;
			unsigned short CMPSTCHA007:1;
			unsigned short CMPSTCHA006:1;
			unsigned short CMPSTCHA005:1;
			unsigned short CMPSTCHA004:1;
			unsigned short CMPSTCHA003:1;
			unsigned short CMPSTCHA002:1;
			unsigned short CMPSTCHA001:1;
			unsigned short CMPSTCHA000:1;
		} BIT;
	} ADCMPSR0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short CMPSTCHA101:1;
			unsigned short CMPSTCHA100:1;
		} BIT;
	} ADCMPSR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char CMPFOC:1;
			unsigned char CMPFTS:1;
		} BIT;
	} ADCMPSER;
	char           wk9[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char CMPLB:1;
			unsigned char :1;
			unsigned char CMPCHB:6;
		} BIT;
	} ADCMPBNSR;
	char           wk10[1];
	unsigned short ADWINLLB;
	unsigned short ADWINULB;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char CMPSTB:1;
		} BIT;
	} ADCMPBSR;
	char           wk11[39];
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
	union {
		unsigned short WORD;
		struct {
			unsigned short :14;
			unsigned short ANSC101:1;
			unsigned short ANSC100:1;
		} BIT;
	} ADANSC1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :6;
			unsigned char OCSC:1;
			unsigned char TSSC:1;
		} BIT;
	} ADGCEXCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char GRCE:1;
			unsigned char GCADIE:1;
			unsigned char TRSC:6;
		} BIT;
	} ADGCTRGR;
	char           wk12[3];
	unsigned char  ADSSTRL;
	unsigned char  ADSSTRT;
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
	char           wk13[212];
	unsigned char  ADSCS0;
	unsigned char  ADSCS1;
	unsigned char  ADSCS2;
	unsigned char  ADSCS3;
	unsigned char  ADSCS4;
	unsigned char  ADSCS5;
	unsigned char  ADSCS6;
	unsigned char  ADSCS7;
	unsigned char  ADSCS8;
	unsigned char  ADSCS9;
	unsigned char  ADSCS10;
	unsigned char  ADSCS11;
	char           wk14[4];
	unsigned char  ADSCS12;
	unsigned char  ADSCS13;
	char           wk15[16];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char VDE:1;
		} BIT;
	} ADVMONCR;
	char           wk16[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char VDO:1;
		} BIT;
	} ADVMONO;
} st_s12ad2_t;

typedef struct st_sci1 {
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
			unsigned char ABCSE:1;
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
	union {
		unsigned char BYTE;
		struct {
			unsigned char DCME:1;
			unsigned char IDSEL:1;
			unsigned char :1;
			unsigned char DFER:1;
			unsigned char DPER:1;
			unsigned char :2;
			unsigned char DCMF:1;
		} BIT;
	} DCCR;
	char           wk0[6];
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned short :7;
			unsigned short CMPD:9;
		} BIT;
	} CDR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SPB2IO:1;
			unsigned char SPB2DT:1;
			unsigned char RXDMON:1;
		} BIT;
	} SPTR;
} st_sci1_t;

typedef struct st_sci11 {
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
		union {
			unsigned char BYTE;
			struct {
				unsigned char TDFE:1;
				unsigned char RDF:1;
				unsigned char ORER:1;
				unsigned char FER:1;
				unsigned char PER:1;
				unsigned char TEND:1;
				unsigned char :1;
				unsigned char DR:1;
			} BIT;
		} SSRFIFO;
	};
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
			unsigned char ABCSE:1;
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
				unsigned char H;
				unsigned char L;
			} BYTE;
			struct {
				unsigned short :6;
				unsigned short MPBT:1;
				unsigned short TDAT:9;
			} BIT;
		} FTDR;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				unsigned char RDRH;
				unsigned char RDRL;
			} BYTE;
		} RDRHL;
		union {
			unsigned short WORD;
			struct {
				unsigned char H;
				unsigned char L;
			} BYTE;
			struct {
				unsigned short :1;
				unsigned short RDF:1;
				unsigned short ORER:1;
				unsigned short FER:1;
				unsigned short PER:1;
				unsigned short DR:1;
				unsigned short MPB:1;
				unsigned short RDAT:9;
			} BIT;
		} FRDR;
	};
	unsigned char  MDDR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DCME:1;
			unsigned char IDSEL:1;
			unsigned char :1;
			unsigned char DFER:1;
			unsigned char DPER:1;
			unsigned char :2;
			unsigned char DCMF:1;
		} BIT;
	} DCCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char RSTRG:4;
			unsigned char RTRG:4;
			unsigned char TTRG:4;
			unsigned char DRES:1;
			unsigned char TFRST:1;
			unsigned char RFRST:1;
			unsigned char FM:1;
		} BIT;
	} FCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :3;
			unsigned char T:5;
			unsigned char :3;
			unsigned char R:5;
		} BIT;
	} FDR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned char :3;
			unsigned char PNUM:5;
			unsigned char :1;
			unsigned char FNUM:5;
			unsigned char :1;
			unsigned char ORER:1;
		} BIT;
	} LSR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			unsigned short :7;
			unsigned short CMPD:9;
		} BIT;
	} CDR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SPB2IO:1;
			unsigned char SPB2DT:1;
			unsigned char RXDMON:1;
		} BIT;
	} SPTR;
} st_sci11_t;

typedef struct st_sci12 {
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
	char           wk0[13];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char ESME:1;
		} BIT;
	} ESMER;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :4;
			unsigned char BRME:1;
			unsigned char RXDSF:1;
			unsigned char SFSF:1;
			unsigned char :1;
		} BIT;
	} CR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char PIBS:3;
			unsigned char PIBE:1;
			unsigned char CF1DS:2;
			unsigned char CF0RE:1;
			unsigned char BFE:1;
		} BIT;
	} CR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char RTS:2;
			unsigned char BCCS:2;
			unsigned char :1;
			unsigned char DFCS:3;
		} BIT;
	} CR2;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char SDST:1;
		} BIT;
	} CR3;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char SHARPS:1;
			unsigned char :2;
			unsigned char RXDXPS:1;
			unsigned char TXDXPS:1;
		} BIT;
	} PCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char AEDIE:1;
			unsigned char BCDIE:1;
			unsigned char PIBDIE:1;
			unsigned char CF1MIE:1;
			unsigned char CF0MIE:1;
			unsigned char BFDIE:1;
		} BIT;
	} ICR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char AEDF:1;
			unsigned char BCDF:1;
			unsigned char PIBDF:1;
			unsigned char CF1MF:1;
			unsigned char CF0MF:1;
			unsigned char BFDF:1;
		} BIT;
	} STR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :2;
			unsigned char AEDCL:1;
			unsigned char BCDCL:1;
			unsigned char PIBDCL:1;
			unsigned char CF1MCL:1;
			unsigned char CF0MCL:1;
			unsigned char BFDCL:1;
		} BIT;
	} STCR;
	unsigned char  CF0DR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char CF0CE7:1;
			unsigned char CF0CE6:1;
			unsigned char CF0CE5:1;
			unsigned char CF0CE4:1;
			unsigned char CF0CE3:1;
			unsigned char CF0CE2:1;
			unsigned char CF0CE1:1;
			unsigned char CF0CE0:1;
		} BIT;
	} CF0CR;
	unsigned char  CF0RR;
	unsigned char  PCF1DR;
	unsigned char  SCF1DR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char CF1CE7:1;
			unsigned char CF1CE6:1;
			unsigned char CF1CE5:1;
			unsigned char CF1CE4:1;
			unsigned char CF1CE3:1;
			unsigned char CF1CE2:1;
			unsigned char CF1CE1:1;
			unsigned char CF1CE0:1;
		} BIT;
	} CF1CR;
	unsigned char  CF1RR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char TCST:1;
		} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char TCSS:3;
			unsigned char TWRC:1;
			unsigned char :1;
			unsigned char TOMS:2;
		} BIT;
	} TMR;
	unsigned char  TPRE;
	unsigned char  TCNT;
} st_sci12_t;

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
	char           wk0[1];
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
	char           wk1[1];
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
	char           wk2[1];
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
} st_smci_t;

typedef struct st_system {
	union {
		unsigned short WORD;
		struct {
			unsigned short :15;
			unsigned short MD:1;
		} BIT;
	} MDMONR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :10;
			unsigned short UBTS:1;
			unsigned short :5;
		} BIT;
	} MDSR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short KEY:8;
			unsigned short :6;
			unsigned short EXBE:1;
			unsigned short ROME:1;
		} BIT;
	} SYSCR0;
	union {
		unsigned short WORD;
		struct {
			unsigned short :9;
			unsigned short ECCRAME:1;
			unsigned short :5;
			unsigned short RAME:1;
		} BIT;
	} SYSCR1;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short SSBY:1;
			unsigned short OPE:1;
			unsigned short :14;
		} BIT;
	} SBYCR;
	char           wk2[2];
	union {
		unsigned long LONG;
		struct {
			unsigned long ACSE:1;
			unsigned long :1;
			unsigned long MSTPA29:1;
			unsigned long MSTPA28:1;
			unsigned long MSTPA27:1;
			unsigned long :2;
			unsigned long MSTPA24:1;
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
			unsigned long :1;
			unsigned long MSTPB19:1;
			unsigned long :1;
			unsigned long MSTPB17:1;
			unsigned long :6;
			unsigned long MSTPB10:1;
			unsigned long MSTPB9:1;
			unsigned long :2;
			unsigned long MSTPB6:1;
			unsigned long :1;
			unsigned long MSTPB4:1;
			unsigned long :3;
			unsigned long MSTPB0:1;
		} BIT;
	} MSTPCRB;
	union {
		unsigned long LONG;
		struct {
			unsigned long :4;
			unsigned long MSTPC27:1;
			unsigned long MSTPC26:1;
			unsigned long :1;
			unsigned long MSTPC24:1;
			unsigned long :4;
			unsigned long MSTPC19:1;
			unsigned long :12;
			unsigned long MSTPC6:1;
			unsigned long :5;
			unsigned long MSTPC0:1;
		} BIT;
	} MSTPCRC;
	union {
		unsigned long LONG;
		struct {
			unsigned long :4;
			unsigned long MSTPD27:1;
			unsigned long :19;
			unsigned long MSTPD7:1;
			unsigned long MSTPD6:1;
			unsigned long MSTPD5:1;
			unsigned long MSTPD4:1;
			unsigned long MSTPD3:1;
			unsigned long MSTPD2:1;
			unsigned long MSTPD1:1;
			unsigned long MSTPD0:1;
		} BIT;
	} MSTPCRD;
	union {
		unsigned long LONG;
		struct {
			unsigned long FCK:4;
			unsigned long ICK:4;
			unsigned long PSTOP1:1;
			unsigned long :3;
			unsigned long BCK:4;
			unsigned long PCKA:4;
			unsigned long PCKB:4;
			unsigned long PCKC:4;
			unsigned long PCKD:4;
		} BIT;
	} SCKCR;
	union {
		unsigned short WORD;
		struct {
			unsigned short :8;
			unsigned short UCK:4;
			unsigned short :4;
		} BIT;
	} SCKCR2;
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
			unsigned short :3;
			unsigned short PLLSRCSEL:1;
			unsigned short :2;
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
	char           wk3[5];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char BCLKDIV:1;
		} BIT;
	} BCKCR;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char MOSTP:1;
		} BIT;
	} MOSCCR;
	char           wk5[1];
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
	char           wk6[4];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char ILCOVF:1;
			unsigned char HCOVF:1;
			unsigned char PLOVF:1;
			unsigned char :1;
			unsigned char MOOVF:1;
		} BIT;
	} OSCOVFSR;
	char           wk7[3];
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
	char           wk8[95];
	union {
		unsigned char BYTE;
		struct {
			unsigned char RSTCKEN:1;
			unsigned char :4;
			unsigned char RSTCKSEL:3;
		} BIT;
	} RSTCKCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char MSTS:8;
		} BIT;
	} MOSCWTCR;
	char           wk9[29];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :5;
			unsigned char SWRF:1;
			unsigned char WDTRF:1;
			unsigned char IWDTRF:1;
		} BIT;
	} RSTSR2;
	char           wk10[1];
	unsigned short SWRR;
	char           wk11[28];
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
	char           wk12[794];
	union {
		unsigned short WORD;
		struct {
			unsigned short PRKEY:8;
			unsigned short :4;
			unsigned short PRC3:1;
			unsigned short :1;
			unsigned short PRC1:1;
			unsigned short PRC0:1;
		} BIT;
	} PRCR;
	char           wk13[3100];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char MEMWAIT:1;
		} BIT;
	} MEMWAIT;
	char           wk14[45667];
	union {
		unsigned char BYTE;
		struct {
			unsigned char DPSBY:1;
			unsigned char IOKEEP:1;
			unsigned char :6;
		} BIT;
	} DPSBYCR;
	char           wk15[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char DIRQ7E:1;
			unsigned char DIRQ6E:1;
			unsigned char DIRQ5E:1;
			unsigned char DIRQ4E:1;
			unsigned char DIRQ3E:1;
			unsigned char DIRQ2E:1;
			unsigned char DIRQ1E:1;
			unsigned char DIRQ0E:1;
		} BIT;
	} DPSIER0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DIRQ15E:1;
			unsigned char DIRQ14E:1;
			unsigned char DIRQ13E:1;
			unsigned char DIRQ12E:1;
			unsigned char DIRQ11E:1;
			unsigned char DIRQ10E:1;
			unsigned char DIRQ9E:1;
			unsigned char DIRQ8E:1;
		} BIT;
	} DPSIER1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char DNMIE:1;
			unsigned char :2;
			unsigned char DLVD2IE:1;
			unsigned char DLVD1IE:1;
		} BIT;
	} DPSIER2;
	char           wk16[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char DIRQ7F:1;
			unsigned char DIRQ6F:1;
			unsigned char DIRQ5F:1;
			unsigned char DIRQ4F:1;
			unsigned char DIRQ3F:1;
			unsigned char DIRQ2F:1;
			unsigned char DIRQ1F:1;
			unsigned char DIRQ0F:1;
		} BIT;
	} DPSIFR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DIRQ15F:1;
			unsigned char DIRQ14F:1;
			unsigned char DIRQ13F:1;
			unsigned char DIRQ12F:1;
			unsigned char DIRQ11F:1;
			unsigned char DIRQ10F:1;
			unsigned char DIRQ9F:1;
			unsigned char DIRQ8F:1;
		} BIT;
	} DPSIFR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char DNMIF:1;
			unsigned char :2;
			unsigned char DLVD2IF:1;
			unsigned char DLVD1IF:1;
		} BIT;
	} DPSIFR2;
	char           wk17[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char DIRQ7EG:1;
			unsigned char DIRQ6EG:1;
			unsigned char DIRQ5EG:1;
			unsigned char DIRQ4EG:1;
			unsigned char DIRQ3EG:1;
			unsigned char DIRQ2EG:1;
			unsigned char DIRQ1EG:1;
			unsigned char DIRQ0EG:1;
		} BIT;
	} DPSIEGR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char DIRQ15EG:1;
			unsigned char DIRQ14EG:1;
			unsigned char DIRQ13EG:1;
			unsigned char DIRQ12EG:1;
			unsigned char DIRQ11EG:1;
			unsigned char DIRQ10EG:1;
			unsigned char DIRQ9EG:1;
			unsigned char DIRQ8EG:1;
		} BIT;
	} DPSIEGR1;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :3;
			unsigned char DNMIEG:1;
			unsigned char :2;
			unsigned char DLVD2EG:1;
			unsigned char DLVD1EG:1;
		} BIT;
	} DPSIEGR2;
	char           wk18[3];
	union {
		unsigned char BYTE;
		struct {
			unsigned char DPSRSTF:1;
			unsigned char :3;
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
	char           wk19[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :1;
			unsigned char MOSEL:1;
			unsigned char MODRV2:2;
			unsigned char :4;
		} BIT;
	} MOFCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char HOCOPCNT:1;
		} BIT;
	} HOCOPCR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char RICVLS:1;
			unsigned char PGAVLS:1;
			unsigned char :3;
			unsigned char USBVON:1;
			unsigned char :2;
		} BIT;
	} VOLSR;
	char           wk20[1];
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
			unsigned char LVD2LVL:4;
			unsigned char LVD1LVL:4;
		} BIT;
	} LVDLVLR;
	char           wk21[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char LVD1RN:1;
			unsigned char LVD1RI:1;
			unsigned char LVD1FSAMP:2;
			unsigned char :1;
			unsigned char LVD1CMPE:1;
			unsigned char LVD1DFDIS:1;
			unsigned char LVD1RIE:1;
		} BIT;
	} LVD1CR0;
	union {
		unsigned char BYTE;
		struct {
			unsigned char LVD2RN:1;
			unsigned char LVD2RI:1;
			unsigned char LVD2FSAMP:2;
			unsigned char :1;
			unsigned char LVD2CMPE:1;
			unsigned char LVD2DFDIS:1;
			unsigned char LVD2RIE:1;
		} BIT;
	} LVD2CR0;
	char           wk22[4];
	unsigned char  DPSBKR[32];
} st_system_t;

typedef struct st_temps {
	unsigned long  TSCDR;
} st_temps_t;

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
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char TCS:1;
		} BIT;
	} TCSTR;
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
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			unsigned char :7;
			unsigned char TCS:1;
		} BIT;
	} TCSTR;
} st_tmr1_t;

typedef struct st_tmr4 {
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
} st_tmr4_t;

typedef struct st_tmr5 {
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
} st_tmr5_t;

typedef struct st_tmr01 {
	unsigned short TCORA;
	unsigned short TCORB;
	unsigned short TCNT;
	unsigned short TCCR;
} st_tmr01_t;

typedef struct st_usb {
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :5;
			unsigned short SCKE:1;
			unsigned short :3;
			unsigned short DCFM:1;
			unsigned short DRPD:1;
			unsigned short DPRPU:1;
			unsigned short :3;
			unsigned short USBE:1;
		} BIT;
#endif
	} SYSCFG;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short OVCMON:2;
			unsigned short :7;
			unsigned short HTACT:1;
			unsigned short :3;
			unsigned short IDMON:1;
			unsigned short LNST:2;
		} BIT;
	} SYSSTS0;
	char           wk1[2];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :4;
			unsigned short HNPBTOA:1;
			unsigned short EXICEN:1;
			unsigned short VBUSEN:1;
			unsigned short WKUP:1;
			unsigned short RWUPE:1;
			unsigned short USBRST:1;
			unsigned short RESUME:1;
			unsigned short UACT:1;
			unsigned short :1;
			unsigned short RHST:3;
		} BIT;
#endif
	} DVSTCTR0;
	char           wk2[10];
	union {
		unsigned short WORD;
		struct {
			unsigned char L;
			unsigned char H;
		} BYTE;
	} CFIFO;
	char           wk3[2];
	union {
		unsigned short WORD;
		struct {
			unsigned char L;
			unsigned char H;
		} BYTE;
	} D0FIFO;
	char           wk4[2];
	union {
		unsigned short WORD;
		struct {
			unsigned char L;
			unsigned char H;
		} BYTE;
	} D1FIFO;
	char           wk5[2];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short RCNT:1;
			unsigned short REW:1;
			unsigned short :3;
			unsigned short MBW:1;
			unsigned short :1;
			unsigned short BIGEND:1;
			unsigned short :2;
			unsigned short ISEL:1;
			unsigned short :1;
			unsigned short CURPIPE:4;
		} BIT;
#endif
	} CFIFOSEL;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BVAL:1;
			unsigned short BCLR:1;
			unsigned short FRDY:1;
			unsigned short :4;
			unsigned short DTLN:9;
		} BIT;
#endif
	} CFIFOCTR;
	char           wk6[4];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short RCNT:1;
			unsigned short REW:1;
			unsigned short DCLRM:1;
			unsigned short DREQE:1;
			unsigned short :1;
			unsigned short MBW:1;
			unsigned short :1;
			unsigned short BIGEND:1;
			unsigned short :4;
			unsigned short CURPIPE:4;
		} BIT;
#endif
	} D0FIFOSEL;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BVAL:1;
			unsigned short BCLR:1;
			unsigned short FRDY:1;
			unsigned short :4;
			unsigned short DTLN:9;
		} BIT;
#endif
	} D0FIFOCTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short RCNT:1;
			unsigned short REW:1;
			unsigned short DCLRM:1;
			unsigned short DREQE:1;
			unsigned short :1;
			unsigned short MBW:1;
			unsigned short :1;
			unsigned short BIGEND:1;
			unsigned short :4;
			unsigned short CURPIPE:4;
		} BIT;
#endif
	} D1FIFOSEL;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BVAL:1;
			unsigned short BCLR:1;
			unsigned short FRDY:1;
			unsigned short :4;
			unsigned short DTLN:9;
		} BIT;
#endif
	} D1FIFOCTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short VBSE:1;
			unsigned short RSME:1;
			unsigned short SOFE:1;
			unsigned short DVSE:1;
			unsigned short CTRE:1;
			unsigned short BEMPE:1;
			unsigned short NRDYE:1;
			unsigned short BRDYE:1;
			unsigned short :8;
		} BIT;
#endif
	} INTENB0;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short OVRCRE:1;
			unsigned short BCHGE:1;
			unsigned short :1;
			unsigned short DTCHE:1;
			unsigned short ATTCHE:1;
			unsigned short :4;
			unsigned short EOFERRE:1;
			unsigned short SIGNE:1;
			unsigned short SACKE:1;
			unsigned short :4;
		} BIT;
#endif
	} INTENB1;
	char           wk7[2];
	union {
		unsigned short WORD;
		struct {
			unsigned short :6;
			unsigned short PIPE9BRDYE:1;
			unsigned short PIPE8BRDYE:1;
			unsigned short PIPE7BRDYE:1;
			unsigned short PIPE6BRDYE:1;
			unsigned short PIPE5BRDYE:1;
			unsigned short PIPE4BRDYE:1;
			unsigned short PIPE3BRDYE:1;
			unsigned short PIPE2BRDYE:1;
			unsigned short PIPE1BRDYE:1;
			unsigned short PIPE0BRDYE:1;
		} BIT;
	} BRDYENB;
	union {
		unsigned short WORD;
		struct {
			unsigned short :6;
			unsigned short PIPE9NRDYE:1;
			unsigned short PIPE8NRDYE:1;
			unsigned short PIPE7NRDYE:1;
			unsigned short PIPE6NRDYE:1;
			unsigned short PIPE5NRDYE:1;
			unsigned short PIPE4NRDYE:1;
			unsigned short PIPE3NRDYE:1;
			unsigned short PIPE2NRDYE:1;
			unsigned short PIPE1NRDYE:1;
			unsigned short PIPE0NRDYE:1;
		} BIT;
	} NRDYENB;
	union {
		unsigned short WORD;
		struct {
			unsigned short :6;
			unsigned short PIPE9BEMPE:1;
			unsigned short PIPE8BEMPE:1;
			unsigned short PIPE7BEMPE:1;
			unsigned short PIPE6BEMPE:1;
			unsigned short PIPE5BEMPE:1;
			unsigned short PIPE4BEMPE:1;
			unsigned short PIPE3BEMPE:1;
			unsigned short PIPE2BEMPE:1;
			unsigned short PIPE1BEMPE:1;
			unsigned short PIPE0BEMPE:1;
		} BIT;
	} BEMPENB;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :7;
			unsigned short TRNENSEL:1;
			unsigned short :1;
			unsigned short BRDYM:1;
			unsigned short :1;
			unsigned short EDGESTS:1;
			unsigned short :4;
		} BIT;
#endif
	} SOFCFG;
	char           wk8[2];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short VBINT:1;
			unsigned short RESM:1;
			unsigned short SOFR:1;
			unsigned short DVST:1;
			unsigned short CTRT:1;
			unsigned short BEMP:1;
			unsigned short NRDY:1;
			unsigned short BRDY:1;
			unsigned short VBSTS:1;
			unsigned short DVSQ:3;
			unsigned short VALID:1;
			unsigned short CTSQ:3;
		} BIT;
#endif
	} INTSTS0;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short OVRCR:1;
			unsigned short BCHG:1;
			unsigned short :1;
			unsigned short DTCH:1;
			unsigned short ATTCH:1;
			unsigned short :4;
			unsigned short EOFERR:1;
			unsigned short SIGN:1;
			unsigned short SACK:1;
			unsigned short :4;
		} BIT;
#endif
	} INTSTS1;
	char           wk9[2];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :6;
			unsigned short PIPE9BRDY:1;
			unsigned short PIPE8BRDY:1;
			unsigned short PIPE7BRDY:1;
			unsigned short PIPE6BRDY:1;
			unsigned short PIPE5BRDY:1;
			unsigned short PIPE4BRDY:1;
			unsigned short PIPE3BRDY:1;
			unsigned short PIPE2BRDY:1;
			unsigned short PIPE1BRDY:1;
			unsigned short PIPE0BRDY:1;
		} BIT;
#endif
	} BRDYSTS;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :6;
			unsigned short PIPE9NRDY:1;
			unsigned short PIPE8NRDY:1;
			unsigned short PIPE7NRDY:1;
			unsigned short PIPE6NRDY:1;
			unsigned short PIPE5NRDY:1;
			unsigned short PIPE4NRDY:1;
			unsigned short PIPE3NRDY:1;
			unsigned short PIPE2NRDY:1;
			unsigned short PIPE1NRDY:1;
			unsigned short PIPE0NRDY:1;
		} BIT;
#endif
	} NRDYSTS;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :6;
			unsigned short PIPE9BEMP:1;
			unsigned short PIPE8BEMP:1;
			unsigned short PIPE7BEMP:1;
			unsigned short PIPE6BEMP:1;
			unsigned short PIPE5BEMP:1;
			unsigned short PIPE4BEMP:1;
			unsigned short PIPE3BEMP:1;
			unsigned short PIPE2BEMP:1;
			unsigned short PIPE1BEMP:1;
			unsigned short PIPE0BEMP:1;
		} BIT;
#endif
	} BEMPSTS;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short OVRN:1;
			unsigned short CRCE:1;
			unsigned short :3;
			unsigned short FRNM:11;
		} BIT;
#endif
	} FRMNUM;
	char           wk10[6];
	union {
		unsigned short WORD;
		struct {
			unsigned short BREQUEST:8;
			unsigned short BMREQUESTTYPE:8;
		} BIT;
	} USBREQ;
	unsigned short USBVAL;
	unsigned short USBINDX;
	unsigned short USBLENG;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :8;
			unsigned short SHTNAK:1;
			unsigned short :2;
			unsigned short DIR:1;
			unsigned short :4;
		} BIT;
#endif
	} DCPCFG;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short DEVSEL:4;
			unsigned short :5;
			unsigned short MXPS:7;
		} BIT;
#endif
	} DCPMAXP;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short SUREQ:1;
			unsigned short :2;
			unsigned short SUREQCLR:1;
			unsigned short :2;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :2;
			unsigned short CCPL:1;
			unsigned short PID:2;
		} BIT;
#endif
	} DCPCTR;
	char           wk11[2];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :12;
			unsigned short PIPESEL:4;
		} BIT;
#endif
	} PIPESEL;
	char           wk12[2];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short TYPE:2;
			unsigned short :3;
			unsigned short BFRE:1;
			unsigned short DBLB:1;
			unsigned short :1;
			unsigned short SHTNAK:1;
			unsigned short :2;
			unsigned short DIR:1;
			unsigned short EPNUM:4;
		} BIT;
#endif
	} PIPECFG;
	char           wk13[2];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short DEVSEL:4;
			unsigned short :3;
			unsigned short MXPS:9;
		} BIT;
#endif
	} PIPEMAXP;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :3;
			unsigned short IFIS:1;
			unsigned short :9;
			unsigned short IITV:3;
		} BIT;
#endif
	} PIPEPERI;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short INBUFM:1;
			unsigned short :3;
			unsigned short ATREPM:1;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE1CTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short INBUFM:1;
			unsigned short :3;
			unsigned short ATREPM:1;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE2CTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short INBUFM:1;
			unsigned short :3;
			unsigned short ATREPM:1;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE3CTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short INBUFM:1;
			unsigned short :3;
			unsigned short ATREPM:1;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE4CTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short INBUFM:1;
			unsigned short :3;
			unsigned short ATREPM:1;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE5CTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short :5;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE6CTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short :5;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE7CTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short :5;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE8CTR;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short BSTS:1;
			unsigned short :5;
			unsigned short ACLRM:1;
			unsigned short SQCLR:1;
			unsigned short SQSET:1;
			unsigned short SQMON:1;
			unsigned short PBUSY:1;
			unsigned short :3;
			unsigned short PID:2;
		} BIT;
#endif
	} PIPE9CTR;
	char           wk14[14];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :6;
			unsigned short TRENB:1;
			unsigned short TRCLR:1;
			unsigned short :8;
		} BIT;
#endif
	} PIPE1TRE;
	unsigned short PIPE1TRN;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :6;
			unsigned short TRENB:1;
			unsigned short TRCLR:1;
			unsigned short :8;
		} BIT;
#endif
	} PIPE2TRE;
	unsigned short PIPE2TRN;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :6;
			unsigned short TRENB:1;
			unsigned short TRCLR:1;
			unsigned short :8;
		} BIT;
#endif
	} PIPE3TRE;
	unsigned short PIPE3TRN;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :6;
			unsigned short TRENB:1;
			unsigned short TRCLR:1;
			unsigned short :8;
		} BIT;
#endif
	} PIPE4TRE;
	unsigned short PIPE4TRN;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :6;
			unsigned short TRENB:1;
			unsigned short TRCLR:1;
			unsigned short :8;
		} BIT;
#endif
	} PIPE5TRE;
	unsigned short PIPE5TRN;
	char           wk15[44];
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :8;
			unsigned short USBSPD:2;
			unsigned short :6;
		} BIT;
#endif
	} DEVADD0;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :8;
			unsigned short USBSPD:2;
			unsigned short :6;
		} BIT;
#endif
	} DEVADD1;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :8;
			unsigned short USBSPD:2;
			unsigned short :6;
		} BIT;
#endif
	} DEVADD2;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :8;
			unsigned short USBSPD:2;
			unsigned short :6;
		} BIT;
#endif
	} DEVADD3;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :8;
			unsigned short USBSPD:2;
			unsigned short :6;
		} BIT;
#endif
	} DEVADD4;
	union {
		unsigned short WORD;
#ifdef IODEFINE_H_HISTORY
		struct {
			unsigned short :8;
			unsigned short USBSPD:2;
			unsigned short :6;
		} BIT;
#endif
	} DEVADD5;
	char           wk16[20];
	union {
		unsigned long LONG;
		struct {
			unsigned long :28;
			unsigned long SLEWF01:1;
			unsigned long SLEWF00:1;
			unsigned long SLEWR01:1;
			unsigned long SLEWR00:1;
		} BIT;
	} PHYSLEW;
} st_usb_t;

typedef struct st_wdt {
	unsigned char  WDTRR;
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
	} WDTCR;
	union {
		unsigned short WORD;
		struct {
			unsigned short REFEF:1;
			unsigned short UNDFF:1;
			unsigned short CNTVAL:14;
		} BIT;
	} WDTSR;
	union {
		unsigned char BYTE;
		struct {
			unsigned char RSTIRQS:1;
			unsigned char :7;
		} BIT;
	} WDTRCR;
} st_wdt_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma bitfields=default
#pragma language=restore
#else
#pragma bit_order
#endif

#ifndef __IAR_SYSTEMS_ICC__
#pragma packoption
#endif


#endif
