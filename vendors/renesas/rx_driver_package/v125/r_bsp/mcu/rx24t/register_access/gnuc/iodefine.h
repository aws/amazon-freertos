                                                                          
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                          
/*********************************************************************************
*
* Device     : RX/RX200/RX24T
*
* File Name  : iodefine.h
*
* Abstract   : Definition of I/O Register.
*
* History    : 0.5  (2015-06-15)  [Hardware Manual Revision : 0.50]
*            : 1.0  (2015-10-26)  [Hardware Manual Revision : 1.00]
*            : 1.0A (2016-02-26)  [Hardware Manual Revision : 1.00]
*            : 1.0B (2016-08-03)  [Hardware Manual Revision : 1.00]
*            : 1.0C (2016-11-24)  [Hardware Manual Revision : 1.10]
*            : 1.0D (2016-12-06)  [Hardware Manual Revision : 1.10]
*            : 1.0E (2017-01-06)  [Hardware Manual Revision : 1.10]
*            : 1.0F (2017-01-25)  [Hardware Manual Revision : 1.10]
*            : 1.0G (2017-01-26)  [Hardware Manual Revision : 1.10]
*            : 1.0H (2017-01-26)  [Hardware Manual Revision : 1.10]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2016 (2015) Renesas Electronics Corporation.
*
*********************************************************************************/
/********************************************************************************/
/*                                                                              */
/*  DESCRIPTION : Definition of ICU Register                                    */
/*  CPU TYPE    : RX24T                                                         */
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

#define	BSC		(*(volatile struct st_bsc         *)0x81300)
#define	CAC		(*(volatile struct st_cac         *)0x8B000)
#define	RSCAN	(*(volatile struct st_rscan         *)0xA8322)
#define	RSCAN0	(*(volatile struct st_rscan0        *)0xA8300)
#define	CMPC0	(*(volatile struct st_cmpc        *)0xA0C80)
#define	CMPC1	(*(volatile struct st_cmpc        *)0xA0CA0)
#define	CMPC2	(*(volatile struct st_cmpc        *)0xA0CC0)
#define	CMPC3	(*(volatile struct st_cmpc        *)0xA0CE0)
#define	CMT		(*(volatile struct st_cmt         *)0x88000)
#define	CMT0	(*(volatile struct st_cmt0        *)0x88002)
#define	CMT1	(*(volatile struct st_cmt0        *)0x88008)
#define	CMT2	(*(volatile struct st_cmt0        *)0x88012)
#define	CMT3	(*(volatile struct st_cmt0        *)0x88018)
#define	CRC		(*(volatile struct st_crc         *)0x88280)
#define	DA		(*(volatile struct st_da          *)0x880C0)
#define	DOC		(*(volatile struct st_doc         *)0x8B080)
#define	DTC		(*(volatile struct st_dtc         *)0x82400)
#define	FLASH	(*(volatile struct st_flash       *)0x81000)
#define	FLASHCONST		(*(volatile struct st_flashconst  *)0x7FC350)
#define	GPT		(*(volatile struct st_gpt         *)0xC2000)
#define	GPT0	(*(volatile struct st_gpt0        *)0xC2100)
#define	GPT1	(*(volatile struct st_gpt0        *)0xC2180)
#define	GPT2	(*(volatile struct st_gpt0        *)0xC2200)
#define	GPT3	(*(volatile struct st_gpt0        *)0xC2280)
#define	GPT01	(*(volatile struct st_gpt01       *)0xC2300)
#define	GPT23	(*(volatile struct st_gpt01       *)0xC2380)
#define	ICU		(*(volatile struct st_icu         *)0x87000)
#define	IWDT	(*(volatile struct st_iwdt        *)0x88030)
#define	MPC		(*(volatile struct st_mpc         *)0x8C11F)
#define	MPU		(*(volatile struct st_mpu         *)0x86400)
#define	MTU		(*(volatile struct st_mtu         *)0xC120A)
#define	MTU0	(*(volatile struct st_mtu0        *)0xC1290)
#define	MTU1	(*(volatile struct st_mtu1        *)0xC1290)
#define	MTU2	(*(volatile struct st_mtu2        *)0xC1292)
#define	MTU3	(*(volatile struct st_mtu3        *)0xC1200)
#define	MTU4	(*(volatile struct st_mtu4        *)0xC1200)
#define	MTU5	(*(volatile struct st_mtu5        *)0xC1A94)
#define	MTU6	(*(volatile struct st_mtu6        *)0xC1A00)
#define	MTU7	(*(volatile struct st_mtu7        *)0xC1A00)
#define	MTU9	(*(volatile struct st_mtu9        *)0xC1296)
#define	POE		(*(volatile struct st_poe         *)0x8C4C0)
#define	PORT0	(*(volatile struct st_port0       *)0x8C000)
#define	PORT1	(*(volatile struct st_port1       *)0x8C001)
#define	PORT2	(*(volatile struct st_port2       *)0x8C002)
#define	PORT3	(*(volatile struct st_port3       *)0x8C003)
#define	PORT4	(*(volatile struct st_port4       *)0x8C004)
#define	PORT5	(*(volatile struct st_port5       *)0x8C005)
#define	PORT6	(*(volatile struct st_port6       *)0x8C006)
#define	PORT7	(*(volatile struct st_port7       *)0x8C007)
#define	PORT8	(*(volatile struct st_port8       *)0x8C008)
#define	PORT9	(*(volatile struct st_port9       *)0x8C009)
#define	PORTA	(*(volatile struct st_porta       *)0x8C00A)
#define	PORTB	(*(volatile struct st_portb       *)0x8C00B)
#define	PORTC	(*(volatile struct st_portc       *)0x8C00C)
#define	PORTD	(*(volatile struct st_portd       *)0x8C00D)
#define	PORTE	(*(volatile struct st_porte       *)0x8C00E)
#define	PORTF	(*(volatile struct st_portf       *)0x8C00F)
#define	PORTG	(*(volatile struct st_portg       *)0x8C010)
#define	RIIC0	(*(volatile struct st_riic        *)0x88300)
#define	RSPI0	(*(volatile struct st_rspi        *)0x88380)
#define	S12AD	(*(volatile struct st_s12ad       *)0x89000)
#define	S12AD1	(*(volatile struct st_s12ad1      *)0x89200)
#define	S12AD2	(*(volatile struct st_s12ad2      *)0x89400)
#define	SCI1	(*(volatile struct st_sci         *)0x8A020)
#define	SCI5	(*(volatile struct st_sci         *)0x8A0A0)
#define	SCI6	(*(volatile struct st_sci         *)0x8A0C0)
#define	SCI8	(*(volatile struct st_sci         *)0x8A100)
#define	SCI9	(*(volatile struct st_sci         *)0x8A120)
#define	SCI11	(*(volatile struct st_sci         *)0xD0000)
#define	SMCI1	(*(volatile struct st_smci        *)0x8A020)
#define	SMCI5	(*(volatile struct st_smci        *)0x8A0A0)
#define	SMCI6	(*(volatile struct st_smci        *)0x8A0C0)
#define	SMCI8	(*(volatile struct st_smci        *)0x8A100)
#define	SMCI9	(*(volatile struct st_smci        *)0x8A120)
#define	SMCI11	(*(volatile struct st_smci        *)0xD0000)
#define	SYSTEM	(*(volatile struct st_system      *)0x80000)
#define	TMR0	(*(volatile struct st_tmr0        *)0x88200)
#define	TMR1	(*(volatile struct st_tmr1        *)0x88201)
#define	TMR2	(*(volatile struct st_tmr0        *)0x88210)
#define	TMR3	(*(volatile struct st_tmr1        *)0x88211)
#define	TMR4	(*(volatile struct st_tmr0        *)0x88220)
#define	TMR5	(*(volatile struct st_tmr1        *)0x88221)
#define	TMR6	(*(volatile struct st_tmr0        *)0x88230)
#define	TMR7	(*(volatile struct st_tmr1        *)0x88231)
#define	TMR01	(*(volatile struct st_tmr01       *)0x88204)
#define	TMR23	(*(volatile struct st_tmr01       *)0x88214)
#define	TMR45	(*(volatile struct st_tmr01       *)0x88224)
#define	TMR67	(*(volatile struct st_tmr01       *)0x88234)

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


#pragma pack(4)


typedef struct st_bsc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char STSCLR : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char STSCLR : 1;
#endif
	} BIT;
	} BERCLR;
	char           wk0[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IGAEN : 1;
			unsigned char TOEN : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TOEN : 1;
			unsigned char IGAEN : 1;
#endif
	} BIT;
	} BEREN;
	char           wk1[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IA : 1;
			unsigned char TO : 1;
			unsigned char  : 2;
			unsigned char MST : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char MST : 3;
			unsigned char  : 2;
			unsigned char TO : 1;
			unsigned char IA : 1;
#endif
	} BIT;
	} BERSR1;
	char           wk2[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 3;
			unsigned short ADDR : 13;
#else
			unsigned short ADDR : 13;
			unsigned short  : 3;
#endif
	} BIT;
	} BERSR2;
	char           wk3[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BPRA : 2;
			unsigned short BPRO : 2;
			unsigned short BPIB : 2;
			unsigned short BPGB : 2;
			unsigned short BPHB : 2;
			unsigned short BPFB : 2;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short BPFB : 2;
			unsigned short BPHB : 2;
			unsigned short BPGB : 2;
			unsigned short BPIB : 2;
			unsigned short BPRO : 2;
			unsigned short BPRA : 2;
#endif
	} BIT;
	} BUSPRI;
} st_bsc_t;

typedef struct st_cac {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CFME : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char CFME : 1;
#endif
	} BIT;
	} CACR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CACREFE : 1;
			unsigned char FMCS : 3;
			unsigned char TCSS : 2;
			unsigned char EDGES : 2;
#else
			unsigned char EDGES : 2;
			unsigned char TCSS : 2;
			unsigned char FMCS : 3;
			unsigned char CACREFE : 1;
#endif
	} BIT;
	} CACR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RPS : 1;
			unsigned char RSCS : 3;
			unsigned char RCDS : 2;
			unsigned char DFS : 2;
#else
			unsigned char DFS : 2;
			unsigned char RCDS : 2;
			unsigned char RSCS : 3;
			unsigned char RPS : 1;
#endif
	} BIT;
	} CACR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FERRIE : 1;
			unsigned char MENDIE : 1;
			unsigned char OVFIE : 1;
			unsigned char  : 1;
			unsigned char FERRFCL : 1;
			unsigned char MENDFCL : 1;
			unsigned char OVFFCL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char OVFFCL : 1;
			unsigned char MENDFCL : 1;
			unsigned char FERRFCL : 1;
			unsigned char  : 1;
			unsigned char OVFIE : 1;
			unsigned char MENDIE : 1;
			unsigned char FERRIE : 1;
#endif
	} BIT;
	} CAICR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FERRF : 1;
			unsigned char MENDF : 1;
			unsigned char OVFF : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char OVFF : 1;
			unsigned char MENDF : 1;
			unsigned char FERRF : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TPRI : 1;
			unsigned short DCE : 1;
			unsigned short DRE : 1;
			unsigned short MME : 1;
			unsigned short DCS : 1;
			unsigned short  : 3;
			unsigned short TSP : 4;
			unsigned short TSSS : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short TSSS : 1;
			unsigned short TSP : 4;
			unsigned short  : 3;
			unsigned short DCS : 1;
			unsigned short MME : 1;
			unsigned short DRE : 1;
			unsigned short DCE : 1;
			unsigned short TPRI : 1;
#endif
	} BIT;
	} GCFGL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ITRCP : 16;
#else
			unsigned short ITRCP : 16;
#endif
	} BIT;
	} GCFGH;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GMDC : 2;
			unsigned short GSLPR : 1;
			unsigned short  : 5;
			unsigned short DEIE : 1;
			unsigned short MEIE : 1;
			unsigned short THLEIE : 1;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short THLEIE : 1;
			unsigned short MEIE : 1;
			unsigned short DEIE : 1;
			unsigned short  : 5;
			unsigned short GSLPR : 1;
			unsigned short GMDC : 2;
#endif
	} BIT;
	} GCTRL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TSRST : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short TSRST : 1;
#endif
	} BIT;
	} GCTRH;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GRSTSTS : 1;
			unsigned short GHLTSTS : 1;
			unsigned short GSLPSTS : 1;
			unsigned short GRAMINIT : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short GRAMINIT : 1;
			unsigned short GSLPSTS : 1;
			unsigned short GHLTSTS : 1;
			unsigned short GRSTSTS : 1;
#endif
	} BIT;
	} GSTS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DEF : 1;
			unsigned char MES : 1;
			unsigned char THLES : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char THLES : 1;
			unsigned char MES : 1;
			unsigned char DEF : 1;
#endif
	} BIT;
	} GERFLL;
	char           wk0[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TS : 16;
#else
			unsigned short TS : 16;
#endif
	} BIT;
	} GTSC;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RNC0 : 5;
			unsigned short  : 11;
#else
			unsigned short  : 11;
			unsigned short RNC0 : 5;
#endif
	} BIT;
	} GAFLCFG;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short NRXMB : 5;
			unsigned short  : 11;
#else
			unsigned short  : 11;
			unsigned short NRXMB : 5;
#endif
	} BIT;
	} RMNB;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMNS : 16;
#else
			unsigned short RMNS : 16;
#endif
	} BIT;
	} RMND0;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFE : 1;
			unsigned short RFIE : 1;
			unsigned short  : 6;
			unsigned short RFDC : 3;
			unsigned short  : 1;
			unsigned short RFIM : 1;
			unsigned short RFIGCV : 3;
#else
			unsigned short RFIGCV : 3;
			unsigned short RFIM : 1;
			unsigned short  : 1;
			unsigned short RFDC : 3;
			unsigned short  : 6;
			unsigned short RFIE : 1;
			unsigned short RFE : 1;
#endif
	} BIT;
	} RFCC0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFE : 1;
			unsigned short RFIE : 1;
			unsigned short  : 6;
			unsigned short RFDC : 3;
			unsigned short  : 1;
			unsigned short RFIM : 1;
			unsigned short RFIGCV : 3;
#else
			unsigned short RFIGCV : 3;
			unsigned short RFIM : 1;
			unsigned short  : 1;
			unsigned short RFDC : 3;
			unsigned short  : 6;
			unsigned short RFIE : 1;
			unsigned short RFE : 1;
#endif
	} BIT;
	} RFCC1;
	char           wk2[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFEMP : 1;
			unsigned short RFFLL : 1;
			unsigned short RFMLT : 1;
			unsigned short RFIF : 1;
			unsigned short  : 4;
			unsigned short RFMC : 6;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short RFMC : 6;
			unsigned short  : 4;
			unsigned short RFIF : 1;
			unsigned short RFMLT : 1;
			unsigned short RFFLL : 1;
			unsigned short RFEMP : 1;
#endif
	} BIT;
	} RFSTS0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFEMP : 1;
			unsigned short RFFLL : 1;
			unsigned short RFMLT : 1;
			unsigned short RFIF : 1;
			unsigned short  : 4;
			unsigned short RFMC : 6;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short RFMC : 6;
			unsigned short  : 4;
			unsigned short RFIF : 1;
			unsigned short RFMLT : 1;
			unsigned short RFFLL : 1;
			unsigned short RFEMP : 1;
#endif
	} BIT;
	} RFSTS1;
	char           wk3[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFPC : 8;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short RFPC : 8;
#endif
	} BIT;
	} RFPCTR0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFPC : 8;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short RFPC : 8;
#endif
	} BIT;
	} RFPCTR1;
	char           wk4[20];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RF0MLT : 1;
			unsigned char RF1MLT : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char RF1MLT : 1;
			unsigned char RF0MLT : 1;
#endif
	} BIT;
	} RFMSTS;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RF0IF : 1;
			unsigned char RF1IF : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char RF1IF : 1;
			unsigned char RF0IF : 1;
#endif
	} BIT;
	} RFISTS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CF0IF : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char CF0IF : 1;
#endif
	} BIT;
	} CFISTS;
	char           wk6[36];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TSIF0 : 1;
			unsigned short TAIF0 : 1;
			unsigned short CFTIF0 : 1;
			unsigned short THIF0 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short THIF0 : 1;
			unsigned short CFTIF0 : 1;
			unsigned short TAIF0 : 1;
			unsigned short TSIF0 : 1;
#endif
	} BIT;
	} GTINTSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RPAGE : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short RPAGE : 1;
#endif
	} BIT;
	} GRWCR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short RTMPS : 3;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short RTMPS : 3;
			unsigned short  : 8;
#endif
	} BIT;
	} GTSTCFG;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
			unsigned char RTME : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char RTME : 1;
			unsigned char  : 2;
#endif
	} BIT;
	} GTSTCTRL;
	char           wk7[5];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short LOCK : 16;
#else
			unsigned short LOCK : 16;
#endif
	} BIT;
	} GLOCKK;
	char           wk8[10];
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL0;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH0;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS0;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR0;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF00;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF20;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF30;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL1;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH1;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS1;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR1;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL2;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF01;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH2;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML2;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF21;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH2;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF31;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL2;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL2;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH2;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH2;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL3;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS2;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH3;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR2;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML3;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF02;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH3;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF12;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL3;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF22;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH3;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF32;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL4;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL3;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH4;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH3;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML4;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS3;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH4;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR3;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL4;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF03;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH4;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF13;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL5;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF23;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH5;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF33;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML5;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL4;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH5;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH4;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL5;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS4;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH5;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR4;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL6;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF04;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH6;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF14;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML6;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF24;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH6;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF34;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL6;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL5;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH6;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH5;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL7;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS5;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH7;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR5;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML7;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF05;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH7;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF15;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL7;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF25;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH7;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF35;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL8;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL6;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH8;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH6;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML8;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS6;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH8;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR6;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL8;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF06;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH8;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF16;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL9;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF26;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH9;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF36;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML9;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL7;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH9;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH7;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL9;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS7;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH9;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR7;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL10;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF07;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH10;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF17;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML10;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF27;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH10;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF37;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL10;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL8;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH10;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH8;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL11;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS8;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH11;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR8;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML11;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF08;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH11;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF18;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL11;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF28;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH11;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF38;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL12;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL9;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH12;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH9;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML12;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS9;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH12;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR9;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL12;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF09;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH12;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF19;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL13;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF29;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH13;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF39;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML13;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH13;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL13;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH13;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR10;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL14;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF010;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH14;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF110;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML14;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF210;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH14;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF310;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL14;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
		} RMIDL11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH14;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
		} RMIDH11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 16;
#else
			unsigned short GAFLID : 16;
#endif
	} BIT;
		} GAFLIDL15;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
		} RMTS11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLID : 13;
			unsigned short GAFLLB : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLIDE : 1;
#else
			unsigned short GAFLIDE : 1;
			unsigned short GAFLRTR : 1;
			unsigned short GAFLLB : 1;
			unsigned short GAFLID : 13;
#endif
	} BIT;
		} GAFLIDH15;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
		} RMPTR11;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 16;
#else
			unsigned short GAFLIDM : 16;
#endif
	} BIT;
		} GAFLML15;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
		} RMDF011;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLIDM : 13;
			unsigned short  : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short GAFLIDEM : 1;
#else
			unsigned short GAFLIDEM : 1;
			unsigned short GAFLRTRM : 1;
			unsigned short  : 1;
			unsigned short GAFLIDM : 13;
#endif
	} BIT;
		} GAFLMH15;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
		} RMDF111;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLFDP0 : 1;
			unsigned short GAFLFDP1 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 3;
			unsigned short GAFLRMDP : 7;
			unsigned short GAFLRMV : 1;
#else
			unsigned short GAFLRMV : 1;
			unsigned short GAFLRMDP : 7;
			unsigned short  : 3;
			unsigned short GAFLFDP4 : 1;
			unsigned short  : 2;
			unsigned short GAFLFDP1 : 1;
			unsigned short GAFLFDP0 : 1;
#endif
	} BIT;
		} GAFLPL15;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
		} RMDF211;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GAFLPTR : 12;
			unsigned short GAFLDLC : 4;
#else
			unsigned short GAFLDLC : 4;
			unsigned short GAFLPTR : 12;
#endif
	} BIT;
		} GAFLPH15;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
		} RMDF311;
	};
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
	} RMIDL12;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
	} RMIDH12;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
	} RMTS12;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
	} RMPTR12;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
	} RMDF012;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
	} RMDF112;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
	} RMDF212;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
	} RMDF312;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
	} RMIDL13;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
	} RMIDH13;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
	} RMTS13;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
	} RMPTR13;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
	} RMDF013;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
	} RMDF113;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
	} RMDF213;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
	} RMDF313;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
	} RMIDL14;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
	} RMIDH14;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
	} RMTS14;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
	} RMPTR14;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
	} RMDF014;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
	} RMDF114;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
	} RMDF214;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
	} RMDF314;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 16;
#else
			unsigned short RMID : 16;
#endif
	} BIT;
	} RMIDL15;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMID : 13;
			unsigned short  : 1;
			unsigned short RMRTR : 1;
			unsigned short RMIDE : 1;
#else
			unsigned short RMIDE : 1;
			unsigned short RMRTR : 1;
			unsigned short  : 1;
			unsigned short RMID : 13;
#endif
	} BIT;
	} RMIDH15;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMTS : 16;
#else
			unsigned short RMTS : 16;
#endif
	} BIT;
	} RMTS15;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMPTR : 12;
			unsigned short RMDLC : 4;
#else
			unsigned short RMDLC : 4;
			unsigned short RMPTR : 12;
#endif
	} BIT;
	} RMPTR15;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB0 : 8;
			unsigned short RMDB1 : 8;
#else
			unsigned short RMDB1 : 8;
			unsigned short RMDB0 : 8;
#endif
	} BIT;
	} RMDF015;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB2 : 8;
			unsigned short RMDB3 : 8;
#else
			unsigned short RMDB3 : 8;
			unsigned short RMDB2 : 8;
#endif
	} BIT;
	} RMDF115;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB4 : 8;
			unsigned short RMDB5 : 8;
#else
			unsigned short RMDB5 : 8;
			unsigned short RMDB4 : 8;
#endif
	} BIT;
	} RMDF215;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RMDB6 : 8;
			unsigned short RMDB7 : 8;
#else
			unsigned short RMDB7 : 8;
			unsigned short RMDB6 : 8;
#endif
	} BIT;
	} RMDF315;
	char           wk9[224];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC3;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC4;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC5;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC6;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC7;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC8;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC9;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC10;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC11;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC12;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC13;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC14;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC15;
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFID : 16;
#else
			unsigned short RFID : 16;
#endif
	} BIT;
		} RFIDL0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC16;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFID : 13;
			unsigned short  : 1;
			unsigned short RFRTR : 1;
			unsigned short RFIDE : 1;
#else
			unsigned short RFIDE : 1;
			unsigned short RFRTR : 1;
			unsigned short  : 1;
			unsigned short RFID : 13;
#endif
	} BIT;
		} RFIDH0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC17;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFTS : 16;
#else
			unsigned short RFTS : 16;
#endif
	} BIT;
		} RFTS0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC18;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFPTR : 12;
			unsigned short RFDLC : 4;
#else
			unsigned short RFDLC : 4;
			unsigned short RFPTR : 12;
#endif
	} BIT;
		} RFPTR0;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC19;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFDB0 : 8;
			unsigned short RFDB1 : 8;
#else
			unsigned short RFDB1 : 8;
			unsigned short RFDB0 : 8;
#endif
	} BIT;
		} RFDF00;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC20;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFDB2 : 8;
			unsigned short RFDB3 : 8;
#else
			unsigned short RFDB3 : 8;
			unsigned short RFDB2 : 8;
#endif
	} BIT;
		} RFDF10;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC21;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFDB4 : 8;
			unsigned short RFDB5 : 8;
#else
			unsigned short RFDB5 : 8;
			unsigned short RFDB4 : 8;
#endif
	} BIT;
		} RFDF20;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC22;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFDB6 : 8;
			unsigned short RFDB7 : 8;
#else
			unsigned short RFDB7 : 8;
			unsigned short RFDB6 : 8;
#endif
	} BIT;
		} RFDF30;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC23;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFID : 16;
#else
			unsigned short RFID : 16;
#endif
	} BIT;
		} RFIDL1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC24;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFID : 13;
			unsigned short  : 1;
			unsigned short RFRTR : 1;
			unsigned short RFIDE : 1;
#else
			unsigned short RFIDE : 1;
			unsigned short RFRTR : 1;
			unsigned short  : 1;
			unsigned short RFID : 13;
#endif
	} BIT;
		} RFIDH1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC25;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFTS : 16;
#else
			unsigned short RFTS : 16;
#endif
	} BIT;
		} RFTS1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC26;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFPTR : 12;
			unsigned short RFDLC : 4;
#else
			unsigned short RFDLC : 4;
			unsigned short RFPTR : 12;
#endif
	} BIT;
		} RFPTR1;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC27;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFDB0 : 8;
			unsigned short RFDB1 : 8;
#else
			unsigned short RFDB1 : 8;
			unsigned short RFDB0 : 8;
#endif
	} BIT;
		} RFDF01;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC28;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFDB2 : 8;
			unsigned short RFDB3 : 8;
#else
			unsigned short RFDB3 : 8;
			unsigned short RFDB2 : 8;
#endif
	} BIT;
		} RFDF11;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC29;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFDB4 : 8;
			unsigned short RFDB5 : 8;
#else
			unsigned short RFDB5 : 8;
			unsigned short RFDB4 : 8;
#endif
	} BIT;
		} RFDF21;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC30;
	};
	union {
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFDB6 : 8;
			unsigned short RFDB7 : 8;
#else
			unsigned short RFDB7 : 8;
			unsigned short RFDB6 : 8;
#endif
	} BIT;
		} RFDF31;
		union {
			unsigned short WORD;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
		} RPGACC31;
	};
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC32;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC33;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC34;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC35;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC36;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC37;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC38;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC39;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC40;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC41;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC42;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC43;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC44;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC45;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC46;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC47;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC48;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC49;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC50;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC51;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC52;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC53;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC54;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC55;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC56;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC57;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC58;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC59;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC60;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC61;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC62;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC63;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC64;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC65;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC66;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC67;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC68;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC69;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC70;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC71;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC72;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC73;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC74;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC75;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC76;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC77;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC78;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC79;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC80;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC81;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC82;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC83;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC84;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC85;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC86;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC87;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC88;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC89;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC90;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC91;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC92;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC93;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC94;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC95;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC96;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC97;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC98;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC99;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC100;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC101;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC102;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC103;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC104;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC105;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC106;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC107;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC108;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC109;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC110;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC111;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC112;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC113;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC114;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC115;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC116;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC117;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC118;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC119;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC120;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC121;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC122;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC123;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC124;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC125;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC126;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RDTA : 16;
#else
			unsigned short RDTA : 16;
#endif
	} BIT;
	} RPGACC127;
} st_rscan_t;

typedef struct st_rscan0 {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BRP : 10;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short BRP : 10;
#endif
	} BIT;
	} CFGL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TSEG1 : 4;
			unsigned short TSEG2 : 3;
			unsigned short  : 1;
			unsigned short SJW : 2;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short SJW : 2;
			unsigned short  : 1;
			unsigned short TSEG2 : 3;
			unsigned short TSEG1 : 4;
#endif
	} BIT;
	} CFGH;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CHMDC : 2;
			unsigned short CSLPR : 1;
			unsigned short RTBO : 1;
			unsigned short  : 4;
			unsigned short BEIE : 1;
			unsigned short EWIE : 1;
			unsigned short EPIE : 1;
			unsigned short BOEIE : 1;
			unsigned short BORIE : 1;
			unsigned short OLIE : 1;
			unsigned short BLIE : 1;
			unsigned short ALIE : 1;
#else
			unsigned short ALIE : 1;
			unsigned short BLIE : 1;
			unsigned short OLIE : 1;
			unsigned short BORIE : 1;
			unsigned short BOEIE : 1;
			unsigned short EPIE : 1;
			unsigned short EWIE : 1;
			unsigned short BEIE : 1;
			unsigned short  : 4;
			unsigned short RTBO : 1;
			unsigned short CSLPR : 1;
			unsigned short CHMDC : 2;
#endif
	} BIT;
	} CTRL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TAIE : 1;
			unsigned short  : 4;
			unsigned short BOM : 2;
			unsigned short ERRD : 1;
			unsigned short CTME : 1;
			unsigned short CTMS : 2;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short CTMS : 2;
			unsigned short CTME : 1;
			unsigned short ERRD : 1;
			unsigned short BOM : 2;
			unsigned short  : 4;
			unsigned short TAIE : 1;
#endif
	} BIT;
	} CTRH;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CRSTSTS : 1;
			unsigned short CHLTSTS : 1;
			unsigned short CSLPSTS : 1;
			unsigned short EPSTS : 1;
			unsigned short BOSTS : 1;
			unsigned short TRMSTS : 1;
			unsigned short RECSTS : 1;
			unsigned short COMSTS : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short COMSTS : 1;
			unsigned short RECSTS : 1;
			unsigned short TRMSTS : 1;
			unsigned short BOSTS : 1;
			unsigned short EPSTS : 1;
			unsigned short CSLPSTS : 1;
			unsigned short CHLTSTS : 1;
			unsigned short CRSTSTS : 1;
#endif
	} BIT;
	} STSL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short REC : 8;
			unsigned short TEC : 8;
#else
			unsigned short TEC : 8;
			unsigned short REC : 8;
#endif
	} BIT;
	} STSH;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BEF : 1;
			unsigned short EWF : 1;
			unsigned short EPF : 1;
			unsigned short BOEF : 1;
			unsigned short BORF : 1;
			unsigned short OVLF : 1;
			unsigned short BLF : 1;
			unsigned short ALF : 1;
			unsigned short SERR : 1;
			unsigned short FERR : 1;
			unsigned short AERR : 1;
			unsigned short CERR : 1;
			unsigned short B1ERR : 1;
			unsigned short B0ERR : 1;
			unsigned short ADERR : 1;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short ADERR : 1;
			unsigned short B0ERR : 1;
			unsigned short B1ERR : 1;
			unsigned short CERR : 1;
			unsigned short AERR : 1;
			unsigned short FERR : 1;
			unsigned short SERR : 1;
			unsigned short ALF : 1;
			unsigned short BLF : 1;
			unsigned short OVLF : 1;
			unsigned short BORF : 1;
			unsigned short BOEF : 1;
			unsigned short EPF : 1;
			unsigned short EWF : 1;
			unsigned short BEF : 1;
#endif
	} BIT;
	} ERFLL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CRCREG : 15;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short CRCREG : 15;
#endif
	} BIT;
	} ERFLH;
	char           wk0[64];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFE : 1;
			unsigned short CFRXIE : 1;
			unsigned short CFTXIE : 1;
			unsigned short  : 5;
			unsigned short CFDC : 3;
			unsigned short  : 1;
			unsigned short CFIM : 1;
			unsigned short CFIGCV : 3;
#else
			unsigned short CFIGCV : 3;
			unsigned short CFIM : 1;
			unsigned short  : 1;
			unsigned short CFDC : 3;
			unsigned short  : 5;
			unsigned short CFTXIE : 1;
			unsigned short CFRXIE : 1;
			unsigned short CFE : 1;
#endif
	} BIT;
	} CFCCL0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFM : 2;
			unsigned short CFITSS : 1;
			unsigned short CFITR : 1;
			unsigned short CFTML : 2;
			unsigned short  : 2;
			unsigned short CFITT : 8;
#else
			unsigned short CFITT : 8;
			unsigned short  : 2;
			unsigned short CFTML : 2;
			unsigned short CFITR : 1;
			unsigned short CFITSS : 1;
			unsigned short CFM : 2;
#endif
	} BIT;
	} CFCCH0;
	char           wk1[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFEMP : 1;
			unsigned short CFFLL : 1;
			unsigned short CFMLT : 1;
			unsigned short CFRXIF : 1;
			unsigned short CFTXIF : 1;
			unsigned short  : 3;
			unsigned short CFMC : 6;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short CFMC : 6;
			unsigned short  : 3;
			unsigned short CFTXIF : 1;
			unsigned short CFRXIF : 1;
			unsigned short CFMLT : 1;
			unsigned short CFFLL : 1;
			unsigned short CFEMP : 1;
#endif
	} BIT;
	} CFSTS0;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFPC : 8;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short CFPC : 8;
#endif
	} BIT;
	} CFPCTR0;
	char           wk3[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CF0MLT : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char CF0MLT : 1;
#endif
	} BIT;
	} CFMSTS;
	char           wk4[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMTR : 1;
			unsigned char TMTAR : 1;
			unsigned char TMOM : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TMOM : 1;
			unsigned char TMTAR : 1;
			unsigned char TMTR : 1;
#endif
	} BIT;
	} TMC0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMTR : 1;
			unsigned char TMTAR : 1;
			unsigned char TMOM : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TMOM : 1;
			unsigned char TMTAR : 1;
			unsigned char TMTR : 1;
#endif
	} BIT;
	} TMC1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMTR : 1;
			unsigned char TMTAR : 1;
			unsigned char TMOM : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TMOM : 1;
			unsigned char TMTAR : 1;
			unsigned char TMTR : 1;
#endif
	} BIT;
	} TMC2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMTR : 1;
			unsigned char TMTAR : 1;
			unsigned char TMOM : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TMOM : 1;
			unsigned char TMTAR : 1;
			unsigned char TMTR : 1;
#endif
	} BIT;
	} TMC3;
	char           wk5[4];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMTSTS : 1;
			unsigned char TMTRF : 2;
			unsigned char TMTRM : 1;
			unsigned char TMTARM : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char TMTARM : 1;
			unsigned char TMTRM : 1;
			unsigned char TMTRF : 2;
			unsigned char TMTSTS : 1;
#endif
	} BIT;
	} TMSTS0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMTSTS : 1;
			unsigned char TMTRF : 2;
			unsigned char TMTRM : 1;
			unsigned char TMTARM : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char TMTARM : 1;
			unsigned char TMTRM : 1;
			unsigned char TMTRF : 2;
			unsigned char TMTSTS : 1;
#endif
	} BIT;
	} TMSTS1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMTSTS : 1;
			unsigned char TMTRF : 2;
			unsigned char TMTRM : 1;
			unsigned char TMTARM : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char TMTARM : 1;
			unsigned char TMTRM : 1;
			unsigned char TMTRF : 2;
			unsigned char TMTSTS : 1;
#endif
	} BIT;
	} TMSTS2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMTSTS : 1;
			unsigned char TMTRF : 2;
			unsigned char TMTRM : 1;
			unsigned char TMTARM : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char TMTARM : 1;
			unsigned char TMTRM : 1;
			unsigned char TMTRF : 2;
			unsigned char TMTSTS : 1;
#endif
	} BIT;
	} TMSTS3;
	char           wk6[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMTRSTS0 : 1;
			unsigned short TMTRSTS1 : 1;
			unsigned short TMTRSTS2 : 1;
			unsigned short TMTRSTS3 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short TMTRSTS3 : 1;
			unsigned short TMTRSTS2 : 1;
			unsigned short TMTRSTS1 : 1;
			unsigned short TMTRSTS0 : 1;
#endif
	} BIT;
	} TMTRSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMTCSTS0 : 1;
			unsigned short TMTCSTS1 : 1;
			unsigned short TMTCSTS2 : 1;
			unsigned short TMTCSTS3 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short TMTCSTS3 : 1;
			unsigned short TMTCSTS2 : 1;
			unsigned short TMTCSTS1 : 1;
			unsigned short TMTCSTS0 : 1;
#endif
	} BIT;
	} TMTCSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMTASTS0 : 1;
			unsigned short TMTASTS1 : 1;
			unsigned short TMTASTS2 : 1;
			unsigned short TMTASTS3 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short TMTASTS3 : 1;
			unsigned short TMTASTS2 : 1;
			unsigned short TMTASTS1 : 1;
			unsigned short TMTASTS0 : 1;
#endif
	} BIT;
	} TMTASTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMIE0 : 1;
			unsigned short TMIE1 : 1;
			unsigned short TMIE2 : 1;
			unsigned short TMIE3 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short TMIE3 : 1;
			unsigned short TMIE2 : 1;
			unsigned short TMIE1 : 1;
			unsigned short TMIE0 : 1;
#endif
	} BIT;
	} TMIEC;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short THLE : 1;
			unsigned short  : 7;
			unsigned short THLIE : 1;
			unsigned short THLIM : 1;
			unsigned short THLDTE : 1;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short THLDTE : 1;
			unsigned short THLIM : 1;
			unsigned short THLIE : 1;
			unsigned short  : 7;
			unsigned short THLE : 1;
#endif
	} BIT;
	} THLCC0;
	char           wk7[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short THLEMP : 1;
			unsigned short THLFLL : 1;
			unsigned short THLELT : 1;
			unsigned short THLIF : 1;
			unsigned short  : 4;
			unsigned short THLMC : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short THLMC : 4;
			unsigned short  : 4;
			unsigned short THLIF : 1;
			unsigned short THLELT : 1;
			unsigned short THLFLL : 1;
			unsigned short THLEMP : 1;
#endif
	} BIT;
	} THLSTS0;
	char           wk8[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short THLPC : 8;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short THLPC : 8;
#endif
	} BIT;
	} THLPCTR0;
	char           wk9[602];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFID : 16;
#else
			unsigned short CFID : 16;
#endif
	} BIT;
	} CFIDL0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFID : 13;
			unsigned short THLEN : 1;
			unsigned short CFRTR : 1;
			unsigned short CFIDE : 1;
#else
			unsigned short CFIDE : 1;
			unsigned short CFRTR : 1;
			unsigned short THLEN : 1;
			unsigned short CFID : 13;
#endif
	} BIT;
	} CFIDH0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFTS : 16;
#else
			unsigned short CFTS : 16;
#endif
	} BIT;
	} CFTS0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFPTR : 12;
			unsigned short CFDLC : 4;
#else
			unsigned short CFDLC : 4;
			unsigned short CFPTR : 12;
#endif
	} BIT;
	} CFPTR0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFDB0 : 8;
			unsigned short CFDB1 : 8;
#else
			unsigned short CFDB1 : 8;
			unsigned short CFDB0 : 8;
#endif
	} BIT;
	} CFDF00;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFDB2 : 8;
			unsigned short CFDB3 : 8;
#else
			unsigned short CFDB3 : 8;
			unsigned short CFDB2 : 8;
#endif
	} BIT;
	} CFDF10;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFDB4 : 8;
			unsigned short CFDB5 : 8;
#else
			unsigned short CFDB5 : 8;
			unsigned short CFDB4 : 8;
#endif
	} BIT;
	} CFDF20;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CFDB6 : 8;
			unsigned short CFDB7 : 8;
#else
			unsigned short CFDB7 : 8;
			unsigned short CFDB6 : 8;
#endif
	} BIT;
	} CFDF30;
	char           wk10[16];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMID : 16;
#else
			unsigned short TMID : 16;
#endif
	} BIT;
	} TMIDL0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMID : 13;
			unsigned short THLEN : 1;
			unsigned short TMRTR : 1;
			unsigned short TMIDE : 1;
#else
			unsigned short TMIDE : 1;
			unsigned short TMRTR : 1;
			unsigned short THLEN : 1;
			unsigned short TMID : 13;
#endif
	} BIT;
	} TMIDH0;
	char           wk11[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMPTR : 8;
			unsigned short  : 4;
			unsigned short TMDLC : 4;
#else
			unsigned short TMDLC : 4;
			unsigned short  : 4;
			unsigned short TMPTR : 8;
#endif
	} BIT;
	} TMPTR0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB0 : 8;
			unsigned short TMDB1 : 8;
#else
			unsigned short TMDB1 : 8;
			unsigned short TMDB0 : 8;
#endif
	} BIT;
	} TMDF00;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB2 : 8;
			unsigned short TMDB3 : 8;
#else
			unsigned short TMDB3 : 8;
			unsigned short TMDB2 : 8;
#endif
	} BIT;
	} TMDF10;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB4 : 8;
			unsigned short TMDB5 : 8;
#else
			unsigned short TMDB5 : 8;
			unsigned short TMDB4 : 8;
#endif
	} BIT;
	} TMDF20;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB6 : 8;
			unsigned short TMDB7 : 8;
#else
			unsigned short TMDB7 : 8;
			unsigned short TMDB6 : 8;
#endif
	} BIT;
	} TMDF30;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMID : 16;
#else
			unsigned short TMID : 16;
#endif
	} BIT;
	} TMIDL1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMID : 13;
			unsigned short THLEN : 1;
			unsigned short TMRTR : 1;
			unsigned short TMIDE : 1;
#else
			unsigned short TMIDE : 1;
			unsigned short TMRTR : 1;
			unsigned short THLEN : 1;
			unsigned short TMID : 13;
#endif
	} BIT;
	} TMIDH1;
	char           wk12[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMPTR : 8;
			unsigned short  : 4;
			unsigned short TMDLC : 4;
#else
			unsigned short TMDLC : 4;
			unsigned short  : 4;
			unsigned short TMPTR : 8;
#endif
	} BIT;
	} TMPTR1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB0 : 8;
			unsigned short TMDB1 : 8;
#else
			unsigned short TMDB1 : 8;
			unsigned short TMDB0 : 8;
#endif
	} BIT;
	} TMDF01;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB2 : 8;
			unsigned short TMDB3 : 8;
#else
			unsigned short TMDB3 : 8;
			unsigned short TMDB2 : 8;
#endif
	} BIT;
	} TMDF11;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB4 : 8;
			unsigned short TMDB5 : 8;
#else
			unsigned short TMDB5 : 8;
			unsigned short TMDB4 : 8;
#endif
	} BIT;
	} TMDF21;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB6 : 8;
			unsigned short TMDB7 : 8;
#else
			unsigned short TMDB7 : 8;
			unsigned short TMDB6 : 8;
#endif
	} BIT;
	} TMDF31;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMID : 16;
#else
			unsigned short TMID : 16;
#endif
	} BIT;
	} TMIDL2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMID : 13;
			unsigned short THLEN : 1;
			unsigned short TMRTR : 1;
			unsigned short TMIDE : 1;
#else
			unsigned short TMIDE : 1;
			unsigned short TMRTR : 1;
			unsigned short THLEN : 1;
			unsigned short TMID : 13;
#endif
	} BIT;
	} TMIDH2;
	char           wk13[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMPTR : 8;
			unsigned short  : 4;
			unsigned short TMDLC : 4;
#else
			unsigned short TMDLC : 4;
			unsigned short  : 4;
			unsigned short TMPTR : 8;
#endif
	} BIT;
	} TMPTR2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB0 : 8;
			unsigned short TMDB1 : 8;
#else
			unsigned short TMDB1 : 8;
			unsigned short TMDB0 : 8;
#endif
	} BIT;
	} TMDF02;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB2 : 8;
			unsigned short TMDB3 : 8;
#else
			unsigned short TMDB3 : 8;
			unsigned short TMDB2 : 8;
#endif
	} BIT;
	} TMDF12;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB4 : 8;
			unsigned short TMDB5 : 8;
#else
			unsigned short TMDB5 : 8;
			unsigned short TMDB4 : 8;
#endif
	} BIT;
	} TMDF22;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB6 : 8;
			unsigned short TMDB7 : 8;
#else
			unsigned short TMDB7 : 8;
			unsigned short TMDB6 : 8;
#endif
	} BIT;
	} TMDF32;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMID : 16;
#else
			unsigned short TMID : 16;
#endif
	} BIT;
	} TMIDL3;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMID : 13;
			unsigned short THLEN : 1;
			unsigned short TMRTR : 1;
			unsigned short TMIDE : 1;
#else
			unsigned short TMIDE : 1;
			unsigned short TMRTR : 1;
			unsigned short THLEN : 1;
			unsigned short TMID : 13;
#endif
	} BIT;
	} TMIDH3;
	char           wk14[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMPTR : 8;
			unsigned short  : 4;
			unsigned short TMDLC : 4;
#else
			unsigned short TMDLC : 4;
			unsigned short  : 4;
			unsigned short TMPTR : 8;
#endif
	} BIT;
	} TMPTR3;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB0 : 8;
			unsigned short TMDB1 : 8;
#else
			unsigned short TMDB1 : 8;
			unsigned short TMDB0 : 8;
#endif
	} BIT;
	} TMDF03;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB2 : 8;
			unsigned short TMDB3 : 8;
#else
			unsigned short TMDB3 : 8;
			unsigned short TMDB2 : 8;
#endif
	} BIT;
	} TMDF13;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB4 : 8;
			unsigned short TMDB5 : 8;
#else
			unsigned short TMDB5 : 8;
			unsigned short TMDB4 : 8;
#endif
	} BIT;
	} TMDF23;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TMDB6 : 8;
			unsigned short TMDB7 : 8;
#else
			unsigned short TMDB7 : 8;
			unsigned short TMDB6 : 8;
#endif
	} BIT;
	} TMDF33;
	char           wk15[64];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BT : 2;
			unsigned short  : 1;
			unsigned short BN : 2;
			unsigned short  : 3;
			unsigned short TID : 8;
#else
			unsigned short TID : 8;
			unsigned short  : 3;
			unsigned short BN : 2;
			unsigned short  : 1;
			unsigned short BT : 2;
#endif
	} BIT;
	} THLACC0;
} st_rscan0_t;

typedef struct st_cmpc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CINV : 1;
			unsigned char COE : 1;
			unsigned char  : 1;
			unsigned char CEG : 2;
			unsigned char CDFS : 2;
			unsigned char HCMPON : 1;
#else
			unsigned char HCMPON : 1;
			unsigned char CDFS : 2;
			unsigned char CEG : 2;
			unsigned char  : 1;
			unsigned char COE : 1;
			unsigned char CINV : 1;
#endif
	} BIT;
	} CMPCTL;
	char           wk0[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMPSEL : 4;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char CMPSEL : 4;
#endif
	} BIT;
	} CMPSEL0;
	char           wk1[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CVRS : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char CVRS : 2;
#endif
	} BIT;
	} CMPSEL1;
	char           wk2[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMPMON0 : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char CMPMON0 : 1;
#endif
	} BIT;
	} CMPMON;
	char           wk3[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPOE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char CPOE : 1;
#endif
	} BIT;
	} CMPIOC;
} st_cmpc_t;

typedef struct st_cmt {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short STR0 : 1;
			unsigned short STR1 : 1;
			unsigned short  : 14;
#else
			unsigned short  : 14;
			unsigned short STR1 : 1;
			unsigned short STR0 : 1;
#endif
	} BIT;
	} CMSTR0;
	char           wk0[14];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short STR2 : 1;
			unsigned short STR3 : 1;
			unsigned short  : 14;
#else
			unsigned short  : 14;
			unsigned short STR3 : 1;
			unsigned short STR2 : 1;
#endif
	} BIT;
	} CMSTR1;
} st_cmt_t;

typedef struct st_cmt0 {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CKS : 2;
			unsigned short  : 4;
			unsigned short CMIE : 1;
			unsigned short  : 9;
#else
			unsigned short  : 9;
			unsigned short CMIE : 1;
			unsigned short  : 4;
			unsigned short CKS : 2;
#endif
	} BIT;
	} CMCR;
	unsigned short CMCNT;
	unsigned short CMCOR;
} st_cmt0_t;

typedef struct st_crc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char GPS : 2;
			unsigned char LMS : 1;
			unsigned char  : 4;
			unsigned char DORCLR : 1;
#else
			unsigned char DORCLR : 1;
			unsigned char  : 4;
			unsigned char LMS : 1;
			unsigned char GPS : 2;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char DAOE0 : 1;
			unsigned char DAOE1 : 1;
#else
			unsigned char DAOE1 : 1;
			unsigned char DAOE0 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} DACR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char DPSEL : 1;
#else
			unsigned char DPSEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} DADPR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char DAADST : 1;
#else
			unsigned char DAADST : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} DAADSCR;
} st_da_t;

typedef struct st_doc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OMS : 2;
			unsigned char DCSEL : 1;
			unsigned char  : 1;
			unsigned char DOPCIE : 1;
			unsigned char DOPCF : 1;
			unsigned char DOPCFCL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char DOPCFCL : 1;
			unsigned char DOPCF : 1;
			unsigned char DOPCIE : 1;
			unsigned char  : 1;
			unsigned char DCSEL : 1;
			unsigned char OMS : 2;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char RRS : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char RRS : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} DTCCR;
	char           wk0[3];
	void          *DTCVBR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SHORT : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char SHORT : 1;
#endif
	} BIT;
	} DTCADMOD;
	char           wk1[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DTCST : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DTCST : 1;
#endif
	} BIT;
	} DTCST;
	char           wk2[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short VECN : 8;
			unsigned short  : 7;
			unsigned short ACT : 1;
#else
			unsigned short ACT : 1;
			unsigned short  : 7;
			unsigned short VECN : 8;
#endif
	} BIT;
	} DTCSTS;
} st_dtc_t;

typedef struct st_flash {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ROMCEN : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ROMCEN : 1;
#endif
	} BIT;
	} ROMCE;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ROMCIV : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ROMCIV : 1;
#endif
	} BIT;
	} ROMCIV;
	char           wk1[7843978];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DFLEN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DFLEN : 1;
#endif
	} BIT;
	} DFLCTL;
	char           wk2[111];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char FMS0 : 1;
			unsigned char  : 1;
			unsigned char RPDIS : 1;
			unsigned char FMS1 : 1;
			unsigned char  : 1;
			unsigned char LVPE : 1;
			unsigned char FMS2 : 1;
#else
			unsigned char FMS2 : 1;
			unsigned char LVPE : 1;
			unsigned char  : 1;
			unsigned char FMS1 : 1;
			unsigned char RPDIS : 1;
			unsigned char  : 1;
			unsigned char FMS0 : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} FPMCR;
	char           wk3[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char EXS : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char EXS : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMD : 4;
			unsigned char  : 2;
			unsigned char STOP : 1;
			unsigned char OPST : 1;
#else
			unsigned char OPST : 1;
			unsigned char STOP : 1;
			unsigned char  : 2;
			unsigned char CMD : 4;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FRESET : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char FRESET : 1;
#endif
	} BIT;
	} FRESETR;
	char           wk10[7];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char FRDY : 1;
			unsigned char EXRDY : 1;
#else
			unsigned char EXRDY : 1;
			unsigned char FRDY : 1;
			unsigned char  : 6;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PERR : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char PERR : 1;
#endif
	} BIT;
	} FPSR;
	char           wk17[59];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short SASMF : 1;
			unsigned short  : 7;
#else
			unsigned short  : 7;
			unsigned short SASMF : 1;
			unsigned short  : 8;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PCKA : 5;
			unsigned char  : 1;
			unsigned char SAS : 2;
#else
			unsigned char SAS : 2;
			unsigned char  : 1;
			unsigned char PCKA : 5;
#endif
	} BIT;
	} FISR;
	char           wk21[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMD : 3;
			unsigned char  : 4;
			unsigned char OPST : 1;
#else
			unsigned char OPST : 1;
			unsigned char  : 4;
			unsigned char CMD : 3;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ERERR : 1;
			unsigned char PRGERR : 1;
			unsigned char  : 1;
			unsigned char BCERR : 1;
			unsigned char ILGLERR : 1;
			unsigned char EILGLERR : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char EILGLERR : 1;
			unsigned char ILGLERR : 1;
			unsigned char BCERR : 1;
			unsigned char  : 1;
			unsigned char PRGERR : 1;
			unsigned char ERERR : 1;
#endif
	} BIT;
	} FSTATR0;
	char           wk25[15809];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FENTRY0 : 1;
			unsigned short  : 6;
			unsigned short FENTRYD : 1;
			unsigned short FEKEY : 8;
#else
			unsigned short FEKEY : 8;
			unsigned short FENTRYD : 1;
			unsigned short  : 6;
			unsigned short FENTRY0 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CST0 : 1;
			unsigned short CST1 : 1;
			unsigned short CST2 : 1;
			unsigned short CST3 : 1;
			unsigned short  : 4;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short  : 4;
			unsigned short CST3 : 1;
			unsigned short CST2 : 1;
			unsigned short CST1 : 1;
			unsigned short CST0 : 1;
#endif
	} BIT;
	} GTSTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short NFA0EN : 1;
			unsigned short NFB0EN : 1;
			unsigned short NFA1EN : 1;
			unsigned short NFB1EN : 1;
			unsigned short NFA2EN : 1;
			unsigned short NFB2EN : 1;
			unsigned short NFA3EN : 1;
			unsigned short NFB3EN : 1;
			unsigned short NFCS0 : 2;
			unsigned short NFCS1 : 2;
			unsigned short NFCS2 : 2;
			unsigned short NFCS3 : 2;
#else
			unsigned short NFCS3 : 2;
			unsigned short NFCS2 : 2;
			unsigned short NFCS1 : 2;
			unsigned short NFCS0 : 2;
			unsigned short NFB3EN : 1;
			unsigned short NFA3EN : 1;
			unsigned short NFB2EN : 1;
			unsigned short NFA2EN : 1;
			unsigned short NFB1EN : 1;
			unsigned short NFA1EN : 1;
			unsigned short NFB0EN : 1;
			unsigned short NFA0EN : 1;
#endif
	} BIT;
	} NFCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CSHW0 : 2;
			unsigned short CSHW1 : 2;
			unsigned short CSHW2 : 2;
			unsigned short CSHW3 : 2;
			unsigned short CPHW0 : 2;
			unsigned short CPHW1 : 2;
			unsigned short CPHW2 : 2;
			unsigned short CPHW3 : 2;
#else
			unsigned short CPHW3 : 2;
			unsigned short CPHW2 : 2;
			unsigned short CPHW1 : 2;
			unsigned short CPHW0 : 2;
			unsigned short CSHW3 : 2;
			unsigned short CSHW2 : 2;
			unsigned short CSHW1 : 2;
			unsigned short CSHW0 : 2;
#endif
	} BIT;
	} GTHSCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CCHW0 : 2;
			unsigned short CCHW1 : 2;
			unsigned short CCHW2 : 2;
			unsigned short CCHW3 : 2;
			unsigned short CCSW0 : 1;
			unsigned short CCSW1 : 1;
			unsigned short CCSW2 : 1;
			unsigned short CCSW3 : 1;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short CCSW3 : 1;
			unsigned short CCSW2 : 1;
			unsigned short CCSW1 : 1;
			unsigned short CCSW0 : 1;
			unsigned short CCHW3 : 2;
			unsigned short CCHW2 : 2;
			unsigned short CCHW1 : 2;
			unsigned short CCHW0 : 2;
#endif
	} BIT;
	} GTHCCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CSHSL0 : 4;
			unsigned short CSHSL1 : 4;
			unsigned short CSHSL2 : 4;
			unsigned short CSHSL3 : 4;
#else
			unsigned short CSHSL3 : 4;
			unsigned short CSHSL2 : 4;
			unsigned short CSHSL1 : 4;
			unsigned short CSHSL0 : 4;
#endif
	} BIT;
	} GTHSSR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CSHPL0 : 4;
			unsigned short CSHPL1 : 4;
			unsigned short CSHPL2 : 4;
			unsigned short CSHPL3 : 4;
#else
			unsigned short CSHPL3 : 4;
			unsigned short CSHPL2 : 4;
			unsigned short CSHPL1 : 4;
			unsigned short CSHPL0 : 4;
#endif
	} BIT;
	} GTHPSR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WP0 : 1;
			unsigned short WP1 : 1;
			unsigned short WP2 : 1;
			unsigned short WP3 : 1;
			unsigned short  : 4;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short  : 4;
			unsigned short WP3 : 1;
			unsigned short WP2 : 1;
			unsigned short WP1 : 1;
			unsigned short WP0 : 1;
#endif
	} BIT;
	} GTWP;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short SYNC0 : 2;
			unsigned short  : 2;
			unsigned short SYNC1 : 2;
			unsigned short  : 2;
			unsigned short SYNC2 : 2;
			unsigned short  : 2;
			unsigned short SYNC3 : 2;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short SYNC3 : 2;
			unsigned short  : 2;
			unsigned short SYNC2 : 2;
			unsigned short  : 2;
			unsigned short SYNC1 : 2;
			unsigned short  : 2;
			unsigned short SYNC0 : 2;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BD00 : 1;
			unsigned short BD01 : 1;
			unsigned short BD02 : 1;
			unsigned short BD03 : 1;
			unsigned short BD10 : 1;
			unsigned short BD11 : 1;
			unsigned short BD12 : 1;
			unsigned short BD13 : 1;
			unsigned short BD20 : 1;
			unsigned short BD21 : 1;
			unsigned short BD22 : 1;
			unsigned short BD23 : 1;
			unsigned short BD30 : 1;
			unsigned short BD31 : 1;
			unsigned short BD32 : 1;
			unsigned short BD33 : 1;
#else
			unsigned short BD33 : 1;
			unsigned short BD32 : 1;
			unsigned short BD31 : 1;
			unsigned short BD30 : 1;
			unsigned short BD23 : 1;
			unsigned short BD22 : 1;
			unsigned short BD21 : 1;
			unsigned short BD20 : 1;
			unsigned short BD13 : 1;
			unsigned short BD12 : 1;
			unsigned short BD11 : 1;
			unsigned short BD10 : 1;
			unsigned short BD03 : 1;
			unsigned short BD02 : 1;
			unsigned short BD01 : 1;
			unsigned short BD00 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short SWP0 : 1;
			unsigned short SWP1 : 1;
			unsigned short SWP2 : 1;
			unsigned short SWP3 : 1;
			unsigned short  : 4;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short  : 4;
			unsigned short SWP3 : 1;
			unsigned short SWP2 : 1;
			unsigned short SWP1 : 1;
			unsigned short SWP0 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CWP0 : 1;
			unsigned short CWP1 : 1;
			unsigned short CWP2 : 1;
			unsigned short CWP3 : 1;
			unsigned short  : 4;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short  : 4;
			unsigned short CWP3 : 1;
			unsigned short CWP2 : 1;
			unsigned short CWP1 : 1;
			unsigned short CWP0 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMNWP : 1;
			unsigned short  : 7;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short  : 7;
			unsigned short CMNWP : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short LWA01 : 1;
			unsigned short LWA23 : 1;
			unsigned short  : 6;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short  : 6;
			unsigned short LWA23 : 1;
			unsigned short LWA01 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long NFENECA : 1;
			unsigned long NFENECB : 1;
			unsigned long NFENECC : 1;
			unsigned long NFENECD : 1;
			unsigned long  : 4;
			unsigned long INVECA : 1;
			unsigned long INVECB : 1;
			unsigned long INVECC : 1;
			unsigned long INVECD : 1;
			unsigned long  : 4;
			unsigned long NFCSECA : 2;
			unsigned long NFCSECB : 2;
			unsigned long NFCSECC : 2;
			unsigned long NFCSECD : 2;
			unsigned long  : 8;
#else
			unsigned long  : 8;
			unsigned long NFCSECD : 2;
			unsigned long NFCSECC : 2;
			unsigned long NFCSECB : 2;
			unsigned long NFCSECA : 2;
			unsigned long  : 4;
			unsigned long INVECD : 1;
			unsigned long INVECC : 1;
			unsigned long INVECB : 1;
			unsigned long INVECA : 1;
			unsigned long  : 4;
			unsigned long NFENECD : 1;
			unsigned long NFENECC : 1;
			unsigned long NFENECB : 1;
			unsigned long NFENECA : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long ADSMS0 : 4;
			unsigned long  : 4;
			unsigned long ADSMEN0 : 1;
			unsigned long  : 7;
			unsigned long ADSMS1 : 4;
			unsigned long  : 4;
			unsigned long ADSMEN1 : 1;
			unsigned long  : 7;
#else
			unsigned long  : 7;
			unsigned long ADSMEN1 : 1;
			unsigned long  : 4;
			unsigned long ADSMS1 : 4;
			unsigned long  : 7;
			unsigned long ADSMEN0 : 1;
			unsigned long  : 4;
			unsigned long ADSMS0 : 4;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GTIOA : 6;
			unsigned short OADFLT : 1;
			unsigned short OAHLD : 1;
			unsigned short GTIOB : 6;
			unsigned short OBDFLT : 1;
			unsigned short OBHLD : 1;
#else
			unsigned short OBHLD : 1;
			unsigned short OBDFLT : 1;
			unsigned short GTIOB : 6;
			unsigned short OAHLD : 1;
			unsigned short OADFLT : 1;
			unsigned short GTIOA : 6;
#endif
	} BIT;
	} GTIOR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GTINTA : 1;
			unsigned short GTINTB : 1;
			unsigned short GTINTC : 1;
			unsigned short GTINTD : 1;
			unsigned short GTINTE : 1;
			unsigned short GTINTF : 1;
			unsigned short GTINTPR : 2;
			unsigned short  : 3;
			unsigned short EINT : 1;
			unsigned short ADTRAUEN : 1;
			unsigned short ADTRADEN : 1;
			unsigned short ADTRBUEN : 1;
			unsigned short ADTRBDEN : 1;
#else
			unsigned short ADTRBDEN : 1;
			unsigned short ADTRBUEN : 1;
			unsigned short ADTRADEN : 1;
			unsigned short ADTRAUEN : 1;
			unsigned short EINT : 1;
			unsigned short  : 3;
			unsigned short GTINTPR : 2;
			unsigned short GTINTF : 1;
			unsigned short GTINTE : 1;
			unsigned short GTINTD : 1;
			unsigned short GTINTC : 1;
			unsigned short GTINTB : 1;
			unsigned short GTINTA : 1;
#endif
	} BIT;
	} GTINTAD;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MD : 3;
			unsigned short  : 5;
			unsigned short TPCS : 4;
			unsigned short CCLR : 2;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short CCLR : 2;
			unsigned short TPCS : 4;
			unsigned short  : 5;
			unsigned short MD : 3;
#endif
	} BIT;
	} GTCR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CCRA : 2;
			unsigned short CCRB : 2;
			unsigned short PR : 2;
			unsigned short CCRSWT : 1;
			unsigned short  : 1;
			unsigned short ADTTA : 2;
			unsigned short ADTDA : 1;
			unsigned short  : 1;
			unsigned short ADTTB : 2;
			unsigned short ADTDB : 1;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short ADTDB : 1;
			unsigned short ADTTB : 2;
			unsigned short  : 1;
			unsigned short ADTDA : 1;
			unsigned short ADTTA : 2;
			unsigned short  : 1;
			unsigned short CCRSWT : 1;
			unsigned short PR : 2;
			unsigned short CCRB : 2;
			unsigned short CCRA : 2;
#endif
	} BIT;
	} GTBER;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short UD : 1;
			unsigned short UDF : 1;
			unsigned short  : 6;
			unsigned short OADTY : 2;
			unsigned short OADTYF : 1;
			unsigned short OADTYR : 1;
			unsigned short OBDTY : 2;
			unsigned short OBDTYF : 1;
			unsigned short OBDTYR : 1;
#else
			unsigned short OBDTYR : 1;
			unsigned short OBDTYF : 1;
			unsigned short OBDTY : 2;
			unsigned short OADTYR : 1;
			unsigned short OADTYF : 1;
			unsigned short OADTY : 2;
			unsigned short  : 6;
			unsigned short UDF : 1;
			unsigned short UD : 1;
#endif
	} BIT;
	} GTUDC;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ITLA : 1;
			unsigned short ITLB : 1;
			unsigned short ITLC : 1;
			unsigned short ITLD : 1;
			unsigned short ITLE : 1;
			unsigned short ITLF : 1;
			unsigned short IVTC : 2;
			unsigned short IVTT : 3;
			unsigned short  : 1;
			unsigned short ADTAL : 1;
			unsigned short  : 1;
			unsigned short ADTBL : 1;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short ADTBL : 1;
			unsigned short  : 1;
			unsigned short ADTAL : 1;
			unsigned short  : 1;
			unsigned short IVTT : 3;
			unsigned short IVTC : 2;
			unsigned short ITLF : 1;
			unsigned short ITLE : 1;
			unsigned short ITLD : 1;
			unsigned short ITLC : 1;
			unsigned short ITLB : 1;
			unsigned short ITLA : 1;
#endif
	} BIT;
	} GTITC;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short ITCNT : 3;
			unsigned short DTEF : 1;
			unsigned short  : 3;
			unsigned short TUCF : 1;
#else
			unsigned short TUCF : 1;
			unsigned short  : 3;
			unsigned short DTEF : 1;
			unsigned short ITCNT : 3;
			unsigned short  : 8;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short NEA : 1;
			unsigned short NEB : 1;
			unsigned short NVA : 1;
			unsigned short NVB : 1;
			unsigned short NFS : 4;
			unsigned short NFV : 1;
			unsigned short  : 3;
			unsigned short SWN : 1;
			unsigned short  : 1;
			unsigned short OAE : 1;
			unsigned short OBE : 1;
#else
			unsigned short OBE : 1;
			unsigned short OAE : 1;
			unsigned short  : 1;
			unsigned short SWN : 1;
			unsigned short  : 3;
			unsigned short NFV : 1;
			unsigned short NFS : 4;
			unsigned short NVB : 1;
			unsigned short NVA : 1;
			unsigned short NEB : 1;
			unsigned short NEA : 1;
#endif
	} BIT;
	} GTONCR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TDE : 1;
			unsigned short  : 3;
			unsigned short TDBUE : 1;
			unsigned short TDBDE : 1;
			unsigned short  : 2;
			unsigned short TDFER : 1;
			unsigned short  : 7;
#else
			unsigned short  : 7;
			unsigned short TDFER : 1;
			unsigned short  : 2;
			unsigned short TDBDE : 1;
			unsigned short TDBUE : 1;
			unsigned short  : 3;
			unsigned short TDE : 1;
#endif
	} BIT;
	} GTDTCR;
	unsigned short GTDVU;
	unsigned short GTDVD;
	unsigned short GTDBU;
	unsigned short GTDBD;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short SOS : 2;
			unsigned short  : 14;
#else
			unsigned short  : 14;
			unsigned short SOS : 2;
#endif
	} BIT;
	} GTSOS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short SOTR : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short SOTR : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IR : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char IR : 1;
#endif
	} BIT;
	} IR[256];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DTCE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DTCE : 1;
#endif
	} BIT;
	} DTCER[256];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IEN0 : 1;
			unsigned char IEN1 : 1;
			unsigned char IEN2 : 1;
			unsigned char IEN3 : 1;
			unsigned char IEN4 : 1;
			unsigned char IEN5 : 1;
			unsigned char IEN6 : 1;
			unsigned char IEN7 : 1;
#else
			unsigned char IEN7 : 1;
			unsigned char IEN6 : 1;
			unsigned char IEN5 : 1;
			unsigned char IEN4 : 1;
			unsigned char IEN3 : 1;
			unsigned char IEN2 : 1;
			unsigned char IEN1 : 1;
			unsigned char IEN0 : 1;
#endif
	} BIT;
	} IER[32];
	char           wk0[192];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SWINT : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char SWINT : 1;
#endif
	} BIT;
	} SWINTR;
	char           wk1[15];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FVCT : 8;
			unsigned short  : 7;
			unsigned short FIEN : 1;
#else
			unsigned short FIEN : 1;
			unsigned short  : 7;
			unsigned short FVCT : 8;
#endif
	} BIT;
	} FIR;
	char           wk2[14];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IPR : 4;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char IPR : 4;
#endif
	} BIT;
	} IPR[256];
	char           wk3[256];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
			unsigned char IRQMD : 2;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char IRQMD : 2;
			unsigned char  : 2;
#endif
	} BIT;
	} IRQCR[8];
	char           wk4[8];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FLTEN0 : 1;
			unsigned char FLTEN1 : 1;
			unsigned char FLTEN2 : 1;
			unsigned char FLTEN3 : 1;
			unsigned char FLTEN4 : 1;
			unsigned char FLTEN5 : 1;
			unsigned char FLTEN6 : 1;
			unsigned char FLTEN7 : 1;
#else
			unsigned char FLTEN7 : 1;
			unsigned char FLTEN6 : 1;
			unsigned char FLTEN5 : 1;
			unsigned char FLTEN4 : 1;
			unsigned char FLTEN3 : 1;
			unsigned char FLTEN2 : 1;
			unsigned char FLTEN1 : 1;
			unsigned char FLTEN0 : 1;
#endif
	} BIT;
	} IRQFLTE0;
	char           wk5[3];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FCLKSEL0 : 2;
			unsigned short FCLKSEL1 : 2;
			unsigned short FCLKSEL2 : 2;
			unsigned short FCLKSEL3 : 2;
			unsigned short FCLKSEL4 : 2;
			unsigned short FCLKSEL5 : 2;
			unsigned short FCLKSEL6 : 2;
			unsigned short FCLKSEL7 : 2;
#else
			unsigned short FCLKSEL7 : 2;
			unsigned short FCLKSEL6 : 2;
			unsigned short FCLKSEL5 : 2;
			unsigned short FCLKSEL4 : 2;
			unsigned short FCLKSEL3 : 2;
			unsigned short FCLKSEL2 : 2;
			unsigned short FCLKSEL1 : 2;
			unsigned short FCLKSEL0 : 2;
#endif
	} BIT;
	} IRQFLTC0;
	char           wk6[106];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NMIST : 1;
			unsigned char OSTST : 1;
			unsigned char  : 1;
			unsigned char IWDTST : 1;
			unsigned char LVD1ST : 1;
			unsigned char LVD2ST : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char LVD2ST : 1;
			unsigned char LVD1ST : 1;
			unsigned char IWDTST : 1;
			unsigned char  : 1;
			unsigned char OSTST : 1;
			unsigned char NMIST : 1;
#endif
	} BIT;
	} NMISR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NMIEN : 1;
			unsigned char OSTEN : 1;
			unsigned char  : 1;
			unsigned char IWDTEN : 1;
			unsigned char LVD1EN : 1;
			unsigned char LVD2EN : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char LVD2EN : 1;
			unsigned char LVD1EN : 1;
			unsigned char IWDTEN : 1;
			unsigned char  : 1;
			unsigned char OSTEN : 1;
			unsigned char NMIEN : 1;
#endif
	} BIT;
	} NMIER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NMICLR : 1;
			unsigned char OSTCLR : 1;
			unsigned char  : 1;
			unsigned char IWDTCLR : 1;
			unsigned char LVD1CLR : 1;
			unsigned char LVD2CLR : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char LVD2CLR : 1;
			unsigned char LVD1CLR : 1;
			unsigned char IWDTCLR : 1;
			unsigned char  : 1;
			unsigned char OSTCLR : 1;
			unsigned char NMICLR : 1;
#endif
	} BIT;
	} NMICLR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
			unsigned char NMIMD : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char NMIMD : 1;
			unsigned char  : 3;
#endif
	} BIT;
	} NMICR;
	char           wk7[12];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFLTEN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char NFLTEN : 1;
#endif
	} BIT;
	} NMIFLTE;
	char           wk8[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFCLKSEL : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char NFCLKSEL : 2;
#endif
	} BIT;
	} NMIFLTC;
} st_icu_t;

typedef struct st_iwdt {
	unsigned char  IWDTRR;
	char           wk0[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TOPS : 2;
			unsigned short  : 2;
			unsigned short CKS : 4;
			unsigned short RPES : 2;
			unsigned short  : 2;
			unsigned short RPSS : 2;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short RPSS : 2;
			unsigned short  : 2;
			unsigned short RPES : 2;
			unsigned short CKS : 4;
			unsigned short  : 2;
			unsigned short TOPS : 2;
#endif
	} BIT;
	} IWDTCR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CNTVAL : 14;
			unsigned short UNDFF : 1;
			unsigned short REFEF : 1;
#else
			unsigned short REFEF : 1;
			unsigned short UNDFF : 1;
			unsigned short CNTVAL : 14;
#endif
	} BIT;
	} IWDTSR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char RSTIRQS : 1;
#else
			unsigned char RSTIRQS : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} IWDTRCR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char SLCSTP : 1;
#else
			unsigned char SLCSTP : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} IWDTCSTPR;
} st_iwdt_t;

typedef struct st_mpc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char PFSWE : 1;
			unsigned char B0WI : 1;
#else
			unsigned char B0WI : 1;
			unsigned char PFSWE : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} PWPR;
	char           wk0[32];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P00PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P01PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P02PFS;
	char           wk1[5];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P10PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P11PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P12PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P13PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P14PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P15PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P16PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P17PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P20PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P21PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P22PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 2;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 2;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P23PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 2;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 2;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P24PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P25PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P26PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P27PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P30PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P31PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P32PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P33PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P34PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P35PFS;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P40PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P41PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P42PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P43PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P44PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P45PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P46PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P47PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P50PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P51PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} P52PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} P53PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} P54PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} P55PFS;
	char           wk3[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} P60PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} P61PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} P62PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} P63PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P64PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} P65PFS;
	char           wk4[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P70PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P71PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P72PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P73PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P74PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P75PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P76PFS;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P80PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P81PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P82PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P83PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P84PFS;
	char           wk6[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P90PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P91PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P92PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P93PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P94PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P95PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P96PFS;
	char           wk7[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PA0PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PA1PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PA2PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PA3PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PA4PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PA5PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PA6PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PA7PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PB0PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PB1PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PB2PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PB3PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PB4PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PB5PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PB6PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PB7PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PC0PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PC1PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PC2PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PC3PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PC4PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PC5PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PC6PFS;
	char           wk8[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PD0PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PD1PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PD2PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PD3PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PD4PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PD5PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PD6PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PD7PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PE0PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PE1PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 2;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char  : 2;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PE2PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PE3PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PE4PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PE5PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ISEL : 1;
			unsigned char  : 1;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PE6PFS;
	char           wk9[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PF0PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PF1PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PF2PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PF3PFS;
	char           wk10[4];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PG0PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PG1PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} PG2PFS;
} st_mpc_t;

typedef struct st_mpu {
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long  : 4;
#endif
	} BIT;
	} RSPAGE0;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long V : 1;
			unsigned long UAC : 3;
			unsigned long REPN : 28;
#else
			unsigned long REPN : 28;
			unsigned long UAC : 3;
			unsigned long V : 1;
#endif
	} BIT;
	} REPAGE0;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long  : 4;
#endif
	} BIT;
	} RSPAGE1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long V : 1;
			unsigned long UAC : 3;
			unsigned long REPN : 28;
#else
			unsigned long REPN : 28;
			unsigned long UAC : 3;
			unsigned long V : 1;
#endif
	} BIT;
	} REPAGE1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long  : 4;
#endif
	} BIT;
	} RSPAGE2;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long V : 1;
			unsigned long UAC : 3;
			unsigned long REPN : 28;
#else
			unsigned long REPN : 28;
			unsigned long UAC : 3;
			unsigned long V : 1;
#endif
	} BIT;
	} REPAGE2;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long  : 4;
#endif
	} BIT;
	} RSPAGE3;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long V : 1;
			unsigned long UAC : 3;
			unsigned long REPN : 28;
#else
			unsigned long REPN : 28;
			unsigned long UAC : 3;
			unsigned long V : 1;
#endif
	} BIT;
	} REPAGE3;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long  : 4;
#endif
	} BIT;
	} RSPAGE4;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long V : 1;
			unsigned long UAC : 3;
			unsigned long REPN : 28;
#else
			unsigned long REPN : 28;
			unsigned long UAC : 3;
			unsigned long V : 1;
#endif
	} BIT;
	} REPAGE4;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long  : 4;
#endif
	} BIT;
	} RSPAGE5;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long V : 1;
			unsigned long UAC : 3;
			unsigned long REPN : 28;
#else
			unsigned long REPN : 28;
			unsigned long UAC : 3;
			unsigned long V : 1;
#endif
	} BIT;
	} REPAGE5;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long  : 4;
#endif
	} BIT;
	} RSPAGE6;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long V : 1;
			unsigned long UAC : 3;
			unsigned long REPN : 28;
#else
			unsigned long REPN : 28;
			unsigned long UAC : 3;
			unsigned long V : 1;
#endif
	} BIT;
	} REPAGE6;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long  : 4;
#endif
	} BIT;
	} RSPAGE7;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long V : 1;
			unsigned long UAC : 3;
			unsigned long REPN : 28;
#else
			unsigned long REPN : 28;
			unsigned long UAC : 3;
			unsigned long V : 1;
#endif
	} BIT;
	} REPAGE7;
	char           wk0[192];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MPEN : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long MPEN : 1;
#endif
	} BIT;
	} MPEN;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long UBAC : 3;
			unsigned long  : 28;
#else
			unsigned long  : 28;
			unsigned long UBAC : 3;
			unsigned long  : 1;
#endif
	} BIT;
	} MPBAC;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CLR : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long CLR : 1;
#endif
	} BIT;
	} MPECLR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long IMPER : 1;
			unsigned long DMPER : 1;
			unsigned long DRW : 1;
			unsigned long  : 29;
#else
			unsigned long  : 29;
			unsigned long DRW : 1;
			unsigned long DMPER : 1;
			unsigned long IMPER : 1;
#endif
	} BIT;
	} MPESTS;
	char           wk1[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long DEA : 32;
#else
			unsigned long DEA : 32;
#endif
	} BIT;
	} MPDEA;
	char           wk2[8];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long SA : 32;
#else
			unsigned long SA : 32;
#endif
	} BIT;
	} MPSA;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short S : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short S : 1;
#endif
	} BIT;
	} MPOPS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short INV : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short INV : 1;
#endif
	} BIT;
	} MPOPI;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long UHACI : 3;
			unsigned long  : 12;
			unsigned long HITI : 8;
			unsigned long  : 8;
#else
			unsigned long  : 8;
			unsigned long HITI : 8;
			unsigned long  : 12;
			unsigned long UHACI : 3;
			unsigned long  : 1;
#endif
	} BIT;
	} MHITI;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long UHACD : 3;
			unsigned long  : 12;
			unsigned long HITD : 8;
			unsigned long  : 8;
#else
			unsigned long  : 8;
			unsigned long HITD : 8;
			unsigned long  : 12;
			unsigned long UHACD : 3;
			unsigned long  : 1;
#endif
	} BIT;
	} MHITD;
} st_mpu_t;

typedef struct st_mtu {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OE3B : 1;
			unsigned char OE4A : 1;
			unsigned char OE4B : 1;
			unsigned char OE3D : 1;
			unsigned char OE4C : 1;
			unsigned char OE4D : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char OE4D : 1;
			unsigned char OE4C : 1;
			unsigned char OE3D : 1;
			unsigned char OE4B : 1;
			unsigned char OE4A : 1;
			unsigned char OE3B : 1;
#endif
	} BIT;
	} TOERA;
	char           wk0[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char UF : 1;
			unsigned char VF : 1;
			unsigned char WF : 1;
			unsigned char FB : 1;
			unsigned char P : 1;
			unsigned char N : 1;
			unsigned char BDC : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char BDC : 1;
			unsigned char N : 1;
			unsigned char P : 1;
			unsigned char FB : 1;
			unsigned char WF : 1;
			unsigned char VF : 1;
			unsigned char UF : 1;
#endif
	} BIT;
	} TGCRA;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OLSP : 1;
			unsigned char OLSN : 1;
			unsigned char TOCS : 1;
			unsigned char TOCL : 1;
			unsigned char  : 2;
			unsigned char PSYE : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char PSYE : 1;
			unsigned char  : 2;
			unsigned char TOCL : 1;
			unsigned char TOCS : 1;
			unsigned char OLSN : 1;
			unsigned char OLSP : 1;
#endif
	} BIT;
	} TOCR1A;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OLS1P : 1;
			unsigned char OLS1N : 1;
			unsigned char OLS2P : 1;
			unsigned char OLS2N : 1;
			unsigned char OLS3P : 1;
			unsigned char OLS3N : 1;
			unsigned char BF : 2;
#else
			unsigned char BF : 2;
			unsigned char OLS3N : 1;
			unsigned char OLS3P : 1;
			unsigned char OLS2N : 1;
			unsigned char OLS2P : 1;
			unsigned char OLS1N : 1;
			unsigned char OLS1P : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char T4VCOR : 3;
			unsigned char T4VEN : 1;
			unsigned char T3ACOR : 3;
			unsigned char T3AEN : 1;
#else
			unsigned char T3AEN : 1;
			unsigned char T3ACOR : 3;
			unsigned char T4VEN : 1;
			unsigned char T4VCOR : 3;
#endif
	} BIT;
	} TITCR1A;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char T4VCNT : 3;
			unsigned char  : 1;
			unsigned char T3ACNT : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char T3ACNT : 3;
			unsigned char  : 1;
			unsigned char T4VCNT : 3;
#endif
	} BIT;
	} TITCNT1A;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BTE : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char BTE : 2;
#endif
	} BIT;
	} TBTERA;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TDER : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char TDER : 1;
#endif
	} BIT;
	} TDERA;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OLS1P : 1;
			unsigned char OLS1N : 1;
			unsigned char OLS2P : 1;
			unsigned char OLS2N : 1;
			unsigned char OLS3P : 1;
			unsigned char OLS3N : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char OLS3N : 1;
			unsigned char OLS3P : 1;
			unsigned char OLS2N : 1;
			unsigned char OLS2P : 1;
			unsigned char OLS1N : 1;
			unsigned char OLS1P : 1;
#endif
	} BIT;
	} TOLBRA;
	char           wk6[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TITM : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char TITM : 1;
#endif
	} BIT;
	} TITMRA;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TRG4COR : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TRG4COR : 3;
#endif
	} BIT;
	} TITCR2A;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TRG4CNT : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TRG4CNT : 3;
#endif
	} BIT;
	} TITCNT2A;
	char           wk7[35];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char WRE : 1;
			unsigned char  : 6;
			unsigned char CCE : 1;
#else
			unsigned char CCE : 1;
			unsigned char  : 6;
			unsigned char WRE : 1;
#endif
	} BIT;
	} TWCRA;
	char           wk8[15];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DRS : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DRS : 1;
#endif
	} BIT;
	} TMDR2A;
	char           wk9[15];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CST0 : 1;
			unsigned char CST1 : 1;
			unsigned char CST2 : 1;
			unsigned char  : 1;
			unsigned char CST9 : 1;
			unsigned char  : 1;
			unsigned char CST3 : 1;
			unsigned char CST4 : 1;
#else
			unsigned char CST4 : 1;
			unsigned char CST3 : 1;
			unsigned char  : 1;
			unsigned char CST9 : 1;
			unsigned char  : 1;
			unsigned char CST2 : 1;
			unsigned char CST1 : 1;
			unsigned char CST0 : 1;
#endif
	} BIT;
	} TSTRA;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SYNC0 : 1;
			unsigned char SYNC1 : 1;
			unsigned char SYNC2 : 1;
			unsigned char SYNC9 : 1;
			unsigned char  : 2;
			unsigned char SYNC3 : 1;
			unsigned char SYNC4 : 1;
#else
			unsigned char SYNC4 : 1;
			unsigned char SYNC3 : 1;
			unsigned char  : 2;
			unsigned char SYNC9 : 1;
			unsigned char SYNC2 : 1;
			unsigned char SYNC1 : 1;
			unsigned char SYNC0 : 1;
#endif
	} BIT;
	} TSYRA;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SCH7 : 1;
			unsigned char SCH6 : 1;
			unsigned char SCH9 : 1;
			unsigned char SCH4 : 1;
			unsigned char SCH3 : 1;
			unsigned char SCH2 : 1;
			unsigned char SCH1 : 1;
			unsigned char SCH0 : 1;
#else
			unsigned char SCH0 : 1;
			unsigned char SCH1 : 1;
			unsigned char SCH2 : 1;
			unsigned char SCH3 : 1;
			unsigned char SCH4 : 1;
			unsigned char SCH9 : 1;
			unsigned char SCH6 : 1;
			unsigned char SCH7 : 1;
#endif
	} BIT;
	} TCSYSTR;
	char           wk10[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RWE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char RWE : 1;
#endif
	} BIT;
	} TRWERA;
	char           wk11[1925];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OE6B : 1;
			unsigned char OE7A : 1;
			unsigned char OE7B : 1;
			unsigned char OE6D : 1;
			unsigned char OE7C : 1;
			unsigned char OE7D : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char OE7D : 1;
			unsigned char OE7C : 1;
			unsigned char OE6D : 1;
			unsigned char OE7B : 1;
			unsigned char OE7A : 1;
			unsigned char OE6B : 1;
#endif
	} BIT;
	} TOERB;
	char           wk12[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char UF : 1;
			unsigned char VF : 1;
			unsigned char WF : 1;
			unsigned char FB : 1;
			unsigned char P : 1;
			unsigned char N : 1;
			unsigned char BDC : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char BDC : 1;
			unsigned char N : 1;
			unsigned char P : 1;
			unsigned char FB : 1;
			unsigned char WF : 1;
			unsigned char VF : 1;
			unsigned char UF : 1;
#endif
	} BIT;
	} TGCRB;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OLSP : 1;
			unsigned char OLSN : 1;
			unsigned char TOCS : 1;
			unsigned char TOCL : 1;
			unsigned char  : 2;
			unsigned char PSYE : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char PSYE : 1;
			unsigned char  : 2;
			unsigned char TOCL : 1;
			unsigned char TOCS : 1;
			unsigned char OLSN : 1;
			unsigned char OLSP : 1;
#endif
	} BIT;
	} TOCR1B;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OLS1P : 1;
			unsigned char OLS1N : 1;
			unsigned char OLS2P : 1;
			unsigned char OLS2N : 1;
			unsigned char OLS3P : 1;
			unsigned char OLS3N : 1;
			unsigned char BF : 2;
#else
			unsigned char BF : 2;
			unsigned char OLS3N : 1;
			unsigned char OLS3P : 1;
			unsigned char OLS2N : 1;
			unsigned char OLS2P : 1;
			unsigned char OLS1N : 1;
			unsigned char OLS1P : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char T7VCOR : 3;
			unsigned char T7VEN : 1;
			unsigned char T6ACOR : 3;
			unsigned char T6AEN : 1;
#else
			unsigned char T6AEN : 1;
			unsigned char T6ACOR : 3;
			unsigned char T7VEN : 1;
			unsigned char T7VCOR : 3;
#endif
	} BIT;
	} TITCR1B;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char T7VCNT : 3;
			unsigned char  : 1;
			unsigned char T6ACNT : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char T6ACNT : 3;
			unsigned char  : 1;
			unsigned char T7VCNT : 3;
#endif
	} BIT;
	} TITCNT1B;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BTE : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char BTE : 2;
#endif
	} BIT;
	} TBTERB;
	char           wk16[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TDER : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char TDER : 1;
#endif
	} BIT;
	} TDERB;
	char           wk17[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OLS1P : 1;
			unsigned char OLS1N : 1;
			unsigned char OLS2P : 1;
			unsigned char OLS2N : 1;
			unsigned char OLS3P : 1;
			unsigned char OLS3N : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char OLS3N : 1;
			unsigned char OLS3P : 1;
			unsigned char OLS2N : 1;
			unsigned char OLS2P : 1;
			unsigned char OLS1N : 1;
			unsigned char OLS1P : 1;
#endif
	} BIT;
	} TOLBRB;
	char           wk18[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TITM : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char TITM : 1;
#endif
	} BIT;
	} TITMRB;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TRG7COR : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TRG7COR : 3;
#endif
	} BIT;
	} TITCR2B;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TRG7CNT : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TRG7CNT : 3;
#endif
	} BIT;
	} TITCNT2B;
	char           wk19[35];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char WRE : 1;
			unsigned char SCC : 1;
			unsigned char  : 5;
			unsigned char CCE : 1;
#else
			unsigned char CCE : 1;
			unsigned char  : 5;
			unsigned char SCC : 1;
			unsigned char WRE : 1;
#endif
	} BIT;
	} TWCRB;
	char           wk20[15];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DRS : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DRS : 1;
#endif
	} BIT;
	} TMDR2B;
	char           wk21[15];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char CST6 : 1;
			unsigned char CST7 : 1;
#else
			unsigned char CST7 : 1;
			unsigned char CST6 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} TSTRB;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char SYNC6 : 1;
			unsigned char SYNC7 : 1;
#else
			unsigned char SYNC7 : 1;
			unsigned char SYNC6 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} TSYRB;
	char           wk22[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RWE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char RWE : 1;
#endif
	} BIT;
	} TRWERB;
	char           wk23[683];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TADSTRS0 : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char TADSTRS0 : 5;
#endif
	} BIT;
	} TADSTRGR0;
	char           wk24[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TADSTRS1 : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char TADSTRS1 : 5;
#endif
	} BIT;
	} TADSTRGR1;
} st_mtu_t;

typedef struct st_mtu0 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFDEN : 1;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char NFDEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCR0;
	char           wk0[8];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFDEN : 1;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char NFDEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCRC;
	char           wk1[102];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 3;
			unsigned char CKEG : 2;
			unsigned char CCLR : 3;
#else
			unsigned char CCLR : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC : 3;
#endif
	} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MD : 4;
			unsigned char BFA : 1;
			unsigned char BFB : 1;
			unsigned char BFE : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char BFE : 1;
			unsigned char BFB : 1;
			unsigned char BFA : 1;
			unsigned char MD : 4;
#endif
	} BIT;
	} TMDR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOA : 4;
			unsigned char IOB : 4;
#else
			unsigned char IOB : 4;
			unsigned char IOA : 4;
#endif
	} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 4;
			unsigned char IOD : 4;
#else
			unsigned char IOD : 4;
			unsigned char IOC : 4;
#endif
	} BIT;
	} TIORL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char  : 2;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 2;
			unsigned char TCIEV : 1;
			unsigned char TGIED : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEA : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEE : 1;
			unsigned char TGIEF : 1;
			unsigned char  : 5;
			unsigned char TTGE2 : 1;
#else
			unsigned char TTGE2 : 1;
			unsigned char  : 5;
			unsigned char TGIEF : 1;
			unsigned char TGIEE : 1;
#endif
	} BIT;
	} TIER2;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TTSA : 1;
			unsigned char TTSB : 1;
			unsigned char TTSE : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TTSE : 1;
			unsigned char TTSB : 1;
			unsigned char TTSA : 1;
#endif
	} BIT;
	} TBTM;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2;
} st_mtu0_t;

typedef struct st_mtu1 {
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char  : 2;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCR1;
	char           wk1[238];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 3;
			unsigned char CKEG : 2;
			unsigned char CCLR : 2;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char CCLR : 2;
			unsigned char CKEG : 2;
			unsigned char TPSC : 3;
#endif
	} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MD : 4;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char MD : 4;
#endif
	} BIT;
	} TMDR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOA : 4;
			unsigned char IOB : 4;
#else
			unsigned char IOB : 4;
			unsigned char IOA : 4;
#endif
	} BIT;
	} TIOR;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char  : 2;
			unsigned char TCIEV : 1;
			unsigned char TCIEU : 1;
			unsigned char  : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 1;
			unsigned char TCIEU : 1;
			unsigned char TCIEV : 1;
			unsigned char  : 2;
			unsigned char TGIEB : 1;
			unsigned char TGIEA : 1;
#endif
	} BIT;
	} TIER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
	char           wk3[4];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char I1AE : 1;
			unsigned char I1BE : 1;
			unsigned char I2AE : 1;
			unsigned char I2BE : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char I2BE : 1;
			unsigned char I2AE : 1;
			unsigned char I1BE : 1;
			unsigned char I1AE : 1;
#endif
	} BIT;
	} TICCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LWA : 1;
			unsigned char PHCKSEL : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char PHCKSEL : 1;
			unsigned char LWA : 1;
#endif
	} BIT;
	} TMDR3;
	char           wk4[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char PCB : 2;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PCB : 2;
			unsigned char TPSC2 : 3;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char  : 2;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCR2;
	char           wk0[365];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 3;
			unsigned char CKEG : 2;
			unsigned char CCLR : 2;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char CCLR : 2;
			unsigned char CKEG : 2;
			unsigned char TPSC : 3;
#endif
	} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MD : 4;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char MD : 4;
#endif
	} BIT;
	} TMDR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOA : 4;
			unsigned char IOB : 4;
#else
			unsigned char IOB : 4;
			unsigned char IOA : 4;
#endif
	} BIT;
	} TIOR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char  : 2;
			unsigned char TCIEV : 1;
			unsigned char TCIEU : 1;
			unsigned char  : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 1;
			unsigned char TCIEU : 1;
			unsigned char TCIEV : 1;
			unsigned char  : 2;
			unsigned char TGIEB : 1;
			unsigned char TGIEA : 1;
#endif
	} BIT;
	} TIER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char PCB : 2;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PCB : 2;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2;
} st_mtu2_t;

typedef struct st_mtu3 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 3;
			unsigned char CKEG : 2;
			unsigned char CCLR : 3;
#else
			unsigned char CCLR : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC : 3;
#endif
	} BIT;
	} TCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MD : 4;
			unsigned char BFA : 1;
			unsigned char BFB : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char BFB : 1;
			unsigned char BFA : 1;
			unsigned char MD : 4;
#endif
	} BIT;
	} TMDR1;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOA : 4;
			unsigned char IOB : 4;
#else
			unsigned char IOB : 4;
			unsigned char IOA : 4;
#endif
	} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 4;
			unsigned char IOD : 4;
#else
			unsigned char IOD : 4;
			unsigned char IOC : 4;
#endif
	} BIT;
	} TIORL;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char  : 2;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 2;
			unsigned char TCIEV : 1;
			unsigned char TGIED : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEA : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} TSR;
	char           wk7[11];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TTSA : 1;
			unsigned char TTSB : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TTSB : 1;
			unsigned char TTSA : 1;
#endif
	} BIT;
	} TBTM;
	char           wk8[19];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2;
	char           wk9[37];
	unsigned short TGRE;
	char           wk10[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFDEN : 1;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char NFDEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCR3;
} st_mtu3_t;

typedef struct st_mtu4 {
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 3;
			unsigned char CKEG : 2;
			unsigned char CCLR : 3;
#else
			unsigned char CCLR : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC : 3;
#endif
	} BIT;
	} TCR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MD : 4;
			unsigned char BFA : 1;
			unsigned char BFB : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char BFB : 1;
			unsigned char BFA : 1;
			unsigned char MD : 4;
#endif
	} BIT;
	} TMDR1;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOA : 4;
			unsigned char IOB : 4;
#else
			unsigned char IOB : 4;
			unsigned char IOA : 4;
#endif
	} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 4;
			unsigned char IOD : 4;
#else
			unsigned char IOD : 4;
			unsigned char IOC : 4;
#endif
	} BIT;
	} TIORL;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char  : 1;
			unsigned char TTGE2 : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char TTGE2 : 1;
			unsigned char  : 1;
			unsigned char TCIEV : 1;
			unsigned char TGIED : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEA : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} TSR;
	char           wk8[11];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TTSA : 1;
			unsigned char TTSB : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TTSB : 1;
			unsigned char TTSA : 1;
#endif
	} BIT;
	} TBTM;
	char           wk9[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ITB4VE : 1;
			unsigned short ITB3AE : 1;
			unsigned short ITA4VE : 1;
			unsigned short ITA3AE : 1;
			unsigned short DT4BE : 1;
			unsigned short UT4BE : 1;
			unsigned short DT4AE : 1;
			unsigned short UT4AE : 1;
			unsigned short  : 6;
			unsigned short BF : 2;
#else
			unsigned short BF : 2;
			unsigned short  : 6;
			unsigned short UT4AE : 1;
			unsigned short DT4AE : 1;
			unsigned short UT4BE : 1;
			unsigned short DT4BE : 1;
			unsigned short ITA3AE : 1;
			unsigned short ITA4VE : 1;
			unsigned short ITB3AE : 1;
			unsigned short ITB4VE : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2;
	char           wk12[38];
	unsigned short TGRE;
	unsigned short TGRF;
	char           wk13[28];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFDEN : 1;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char NFDEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCR4;
} st_mtu4_t;

typedef struct st_mtu5 {
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFUEN : 1;
			unsigned char NFVEN : 1;
			unsigned char NFWEN : 1;
			unsigned char  : 1;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char  : 1;
			unsigned char NFWEN : 1;
			unsigned char NFVEN : 1;
			unsigned char NFUEN : 1;
#endif
	} BIT;
	} NFCR5;
	char           wk1[490];
	unsigned short TCNTU;
	unsigned short TGRU;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TPSC : 2;
#endif
	} BIT;
	} TCRU;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char CKEG : 2;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2U;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char IOC : 5;
#endif
	} BIT;
	} TIORU;
	char           wk2[9];
	unsigned short TCNTV;
	unsigned short TGRV;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TPSC : 2;
#endif
	} BIT;
	} TCRV;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char CKEG : 2;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2V;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char IOC : 5;
#endif
	} BIT;
	} TIORV;
	char           wk3[9];
	unsigned short TCNTW;
	unsigned short TGRW;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TPSC : 2;
#endif
	} BIT;
	} TCRW;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char CKEG : 2;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2W;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char IOC : 5;
#endif
	} BIT;
	} TIORW;
	char           wk4[11];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIE5W : 1;
			unsigned char TGIE5V : 1;
			unsigned char TGIE5U : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TGIE5U : 1;
			unsigned char TGIE5V : 1;
			unsigned char TGIE5W : 1;
#endif
	} BIT;
	} TIER;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CSTW5 : 1;
			unsigned char CSTV5 : 1;
			unsigned char CSTU5 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char CSTU5 : 1;
			unsigned char CSTV5 : 1;
			unsigned char CSTW5 : 1;
#endif
	} BIT;
	} TSTR;
	char           wk6[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMPCLR5W : 1;
			unsigned char CMPCLR5V : 1;
			unsigned char CMPCLR5U : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char CMPCLR5U : 1;
			unsigned char CMPCLR5V : 1;
			unsigned char CMPCLR5W : 1;
#endif
	} BIT;
	} TCNTCMPCLR;
} st_mtu5_t;

typedef struct st_mtu6 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 3;
			unsigned char CKEG : 2;
			unsigned char CCLR : 3;
#else
			unsigned char CCLR : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC : 3;
#endif
	} BIT;
	} TCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MD : 4;
			unsigned char BFA : 1;
			unsigned char BFB : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char BFB : 1;
			unsigned char BFA : 1;
			unsigned char MD : 4;
#endif
	} BIT;
	} TMDR1;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOA : 4;
			unsigned char IOB : 4;
#else
			unsigned char IOB : 4;
			unsigned char IOA : 4;
#endif
	} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 4;
			unsigned char IOD : 4;
#else
			unsigned char IOD : 4;
			unsigned char IOC : 4;
#endif
	} BIT;
	} TIORL;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char  : 2;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 2;
			unsigned char TCIEV : 1;
			unsigned char TGIED : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEA : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} TSR;
	char           wk7[11];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TTSA : 1;
			unsigned char TTSB : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TTSB : 1;
			unsigned char TTSA : 1;
#endif
	} BIT;
	} TBTM;
	char           wk8[19];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2;
	char           wk9[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CE2B : 1;
			unsigned char CE2A : 1;
			unsigned char CE1B : 1;
			unsigned char CE1A : 1;
			unsigned char CE0D : 1;
			unsigned char CE0C : 1;
			unsigned char CE0B : 1;
			unsigned char CE0A : 1;
#else
			unsigned char CE0A : 1;
			unsigned char CE0B : 1;
			unsigned char CE0C : 1;
			unsigned char CE0D : 1;
			unsigned char CE1A : 1;
			unsigned char CE1B : 1;
			unsigned char CE2A : 1;
			unsigned char CE2B : 1;
#endif
	} BIT;
	} TSYCR;
	char           wk10[33];
	unsigned short TGRE;
	char           wk11[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFDEN : 1;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char NFDEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCR6;
} st_mtu6_t;

typedef struct st_mtu7 {
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 3;
			unsigned char CKEG : 2;
			unsigned char CCLR : 3;
#else
			unsigned char CCLR : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC : 3;
#endif
	} BIT;
	} TCR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MD : 4;
			unsigned char BFA : 1;
			unsigned char BFB : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char BFB : 1;
			unsigned char BFA : 1;
			unsigned char MD : 4;
#endif
	} BIT;
	} TMDR1;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOA : 4;
			unsigned char IOB : 4;
#else
			unsigned char IOB : 4;
			unsigned char IOA : 4;
#endif
	} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 4;
			unsigned char IOD : 4;
#else
			unsigned char IOD : 4;
			unsigned char IOC : 4;
#endif
	} BIT;
	} TIORL;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char  : 1;
			unsigned char TTGE2 : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char TTGE2 : 1;
			unsigned char  : 1;
			unsigned char TCIEV : 1;
			unsigned char TGIED : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEA : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} TSR;
	char           wk8[11];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TTSA : 1;
			unsigned char TTSB : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TTSB : 1;
			unsigned char TTSA : 1;
#endif
	} BIT;
	} TBTM;
	char           wk9[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ITB7VE : 1;
			unsigned short ITB6AE : 1;
			unsigned short ITA7VE : 1;
			unsigned short ITA6AE : 1;
			unsigned short DT7BE : 1;
			unsigned short UT7BE : 1;
			unsigned short DT7AE : 1;
			unsigned short UT7AE : 1;
			unsigned short  : 6;
			unsigned short BF : 2;
#else
			unsigned short BF : 2;
			unsigned short  : 6;
			unsigned short UT7AE : 1;
			unsigned short DT7AE : 1;
			unsigned short UT7BE : 1;
			unsigned short DT7BE : 1;
			unsigned short ITA6AE : 1;
			unsigned short ITA7VE : 1;
			unsigned short ITB6AE : 1;
			unsigned short ITB7VE : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TPSC2 : 3;
#endif
	} BIT;
	} TCR2;
	char           wk12[38];
	unsigned short TGRE;
	unsigned short TGRF;
	char           wk13[28];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFDEN : 1;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char NFDEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCR7;
} st_mtu7_t;

typedef struct st_mtu9 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFAEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFDEN : 1;
			unsigned char NFCS : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFCS : 2;
			unsigned char NFDEN : 1;
			unsigned char NFCEN : 1;
			unsigned char NFBEN : 1;
			unsigned char NFAEN : 1;
#endif
	} BIT;
	} NFCR9;
	char           wk0[745];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC : 3;
			unsigned char CKEG : 2;
			unsigned char CCLR : 3;
#else
			unsigned char CCLR : 3;
			unsigned char CKEG : 2;
			unsigned char TPSC : 3;
#endif
	} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MD : 4;
			unsigned char BFA : 1;
			unsigned char BFB : 1;
			unsigned char BFE : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char BFE : 1;
			unsigned char BFB : 1;
			unsigned char BFA : 1;
			unsigned char MD : 4;
#endif
	} BIT;
	} TMDR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOA : 4;
			unsigned char IOB : 4;
#else
			unsigned char IOB : 4;
			unsigned char IOA : 4;
#endif
	} BIT;
	} TIORH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IOC : 4;
			unsigned char IOD : 4;
#else
			unsigned char IOD : 4;
			unsigned char IOC : 4;
#endif
	} BIT;
	} TIORL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char  : 2;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 2;
			unsigned char TCIEV : 1;
			unsigned char TGIED : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEA : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEE : 1;
			unsigned char TGIEF : 1;
			unsigned char  : 5;
			unsigned char TTGE2 : 1;
#else
			unsigned char TTGE2 : 1;
			unsigned char  : 5;
			unsigned char TGIEF : 1;
			unsigned char TGIEE : 1;
#endif
	} BIT;
	} TIER2;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TTSA : 1;
			unsigned char TTSB : 1;
			unsigned char TTSE : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TTSE : 1;
			unsigned char TTSB : 1;
			unsigned char TTSA : 1;
#endif
	} BIT;
	} TBTM;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TPSC2 : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TPSC2 : 3;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short POE0M : 2;
			unsigned short  : 6;
			unsigned short PIE1 : 1;
			unsigned short  : 3;
			unsigned short POE0F : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short POE0F : 1;
			unsigned short  : 3;
			unsigned short PIE1 : 1;
			unsigned short  : 6;
			unsigned short POE0M : 2;
#endif
	} BIT;
	} ICSR1;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short OIE1 : 1;
			unsigned short OCE1 : 1;
			unsigned short  : 5;
			unsigned short OSF1 : 1;
#else
			unsigned short OSF1 : 1;
			unsigned short  : 5;
			unsigned short OCE1 : 1;
			unsigned short OIE1 : 1;
			unsigned short  : 8;
#endif
	} BIT;
	} OCSR1;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short POE4M : 2;
			unsigned short  : 6;
			unsigned short PIE2 : 1;
			unsigned short  : 3;
			unsigned short POE4F : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short POE4F : 1;
			unsigned short  : 3;
			unsigned short PIE2 : 1;
			unsigned short  : 6;
			unsigned short POE4M : 2;
#endif
	} BIT;
	} ICSR2;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short OIE2 : 1;
			unsigned short OCE2 : 1;
			unsigned short  : 5;
			unsigned short OSF2 : 1;
#else
			unsigned short OSF2 : 1;
			unsigned short  : 5;
			unsigned short OCE2 : 1;
			unsigned short OIE2 : 1;
			unsigned short  : 8;
#endif
	} BIT;
	} OCSR2;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short POE8M : 2;
			unsigned short  : 6;
			unsigned short PIE3 : 1;
			unsigned short POE8E : 1;
			unsigned short  : 2;
			unsigned short POE8F : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short POE8F : 1;
			unsigned short  : 2;
			unsigned short POE8E : 1;
			unsigned short PIE3 : 1;
			unsigned short  : 6;
			unsigned short POE8M : 2;
#endif
	} BIT;
	} ICSR3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MTUCH34HIZ : 1;
			unsigned char MTUCH67HIZ : 1;
			unsigned char MTUCH0HIZ : 1;
			unsigned char GPT02HIZ : 1;
			unsigned char GPT03HIZ : 1;
			unsigned char  : 1;
			unsigned char MTUCH9HIZ : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char MTUCH9HIZ : 1;
			unsigned char  : 1;
			unsigned char GPT03HIZ : 1;
			unsigned char GPT02HIZ : 1;
			unsigned char MTUCH0HIZ : 1;
			unsigned char MTUCH67HIZ : 1;
			unsigned char MTUCH34HIZ : 1;
#endif
	} BIT;
	} SPOER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MTU0AZE : 1;
			unsigned char MTU0BZE : 1;
			unsigned char MTU0CZE : 1;
			unsigned char MTU0DZE : 1;
			unsigned char MTU0A1ZE : 1;
			unsigned char MTU0B1ZE : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char MTU0B1ZE : 1;
			unsigned char MTU0A1ZE : 1;
			unsigned char MTU0DZE : 1;
			unsigned char MTU0CZE : 1;
			unsigned char MTU0BZE : 1;
			unsigned char MTU0AZE : 1;
#endif
	} BIT;
	} POECR1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MTU7BDZE : 1;
			unsigned short MTU7ACZE : 1;
			unsigned short MTU6BDZE : 1;
			unsigned short  : 5;
			unsigned short MTU4BDZE : 1;
			unsigned short MTU4ACZE : 1;
			unsigned short MTU3BDZE : 1;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short MTU3BDZE : 1;
			unsigned short MTU4ACZE : 1;
			unsigned short MTU4BDZE : 1;
			unsigned short  : 5;
			unsigned short MTU6BDZE : 1;
			unsigned short MTU7ACZE : 1;
			unsigned short MTU7BDZE : 1;
#endif
	} BIT;
	} POECR2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GPT0AZE : 1;
			unsigned short GPT0BZE : 1;
			unsigned short GPT1AZE : 1;
			unsigned short GPT1BZE : 1;
			unsigned short GPT2AZE : 1;
			unsigned short GPT2BZE : 1;
			unsigned short  : 2;
			unsigned short GPT0A1ZE : 1;
			unsigned short GPT0B1ZE : 1;
			unsigned short GPT1A1ZE : 1;
			unsigned short GPT1B1ZE : 1;
			unsigned short GPT2A1ZE : 1;
			unsigned short GPT2B1ZE : 1;
			unsigned short GPT3A1ZE : 1;
			unsigned short GPT3B1ZE : 1;
#else
			unsigned short GPT3B1ZE : 1;
			unsigned short GPT3A1ZE : 1;
			unsigned short GPT2B1ZE : 1;
			unsigned short GPT2A1ZE : 1;
			unsigned short GPT1B1ZE : 1;
			unsigned short GPT1A1ZE : 1;
			unsigned short GPT0B1ZE : 1;
			unsigned short GPT0A1ZE : 1;
			unsigned short  : 2;
			unsigned short GPT2BZE : 1;
			unsigned short GPT2AZE : 1;
			unsigned short GPT1BZE : 1;
			unsigned short GPT1AZE : 1;
			unsigned short GPT0BZE : 1;
			unsigned short GPT0AZE : 1;
#endif
	} BIT;
	} POECR3;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMADDMT34ZE : 1;
			unsigned short  : 1;
			unsigned short IC2ADDMT34ZE : 1;
			unsigned short IC3ADDMT34ZE : 1;
			unsigned short IC4ADDMT34ZE : 1;
			unsigned short IC5ADDMT34ZE : 1;
			unsigned short IC6ADDMT34ZE : 1;
			unsigned short  : 1;
			unsigned short CMADDMT67ZE : 1;
			unsigned short IC1ADDMT67ZE : 1;
			unsigned short  : 1;
			unsigned short IC3ADDMT67ZE : 1;
			unsigned short IC4ADDMT67ZE : 1;
			unsigned short IC5ADDMT67ZE : 1;
			unsigned short IC6ADDMT67ZE : 1;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short IC6ADDMT67ZE : 1;
			unsigned short IC5ADDMT67ZE : 1;
			unsigned short IC4ADDMT67ZE : 1;
			unsigned short IC3ADDMT67ZE : 1;
			unsigned short  : 1;
			unsigned short IC1ADDMT67ZE : 1;
			unsigned short CMADDMT67ZE : 1;
			unsigned short  : 1;
			unsigned short IC6ADDMT34ZE : 1;
			unsigned short IC5ADDMT34ZE : 1;
			unsigned short IC4ADDMT34ZE : 1;
			unsigned short IC3ADDMT34ZE : 1;
			unsigned short IC2ADDMT34ZE : 1;
			unsigned short  : 1;
			unsigned short CMADDMT34ZE : 1;
#endif
	} BIT;
	} POECR4;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMADDMT0ZE : 1;
			unsigned short IC1ADDMT0ZE : 1;
			unsigned short IC2ADDMT0ZE : 1;
			unsigned short  : 1;
			unsigned short IC4ADDMT0ZE : 1;
			unsigned short IC5ADDMT0ZE : 1;
			unsigned short IC6ADDMT0ZE : 1;
			unsigned short  : 9;
#else
			unsigned short  : 9;
			unsigned short IC6ADDMT0ZE : 1;
			unsigned short IC5ADDMT0ZE : 1;
			unsigned short IC4ADDMT0ZE : 1;
			unsigned short  : 1;
			unsigned short IC2ADDMT0ZE : 1;
			unsigned short IC1ADDMT0ZE : 1;
			unsigned short CMADDMT0ZE : 1;
#endif
	} BIT;
	} POECR5;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMADDGPT02ZE : 1;
			unsigned short IC1ADDGPT02ZE : 1;
			unsigned short IC2ADDGPT02ZE : 1;
			unsigned short IC3ADDGPT02ZE : 1;
			unsigned short  : 1;
			unsigned short IC5ADDGPT02ZE : 1;
			unsigned short IC6ADDGPT02ZE : 1;
			unsigned short  : 1;
			unsigned short CMADDGPT03ZE : 1;
			unsigned short IC1ADDGPT03ZE : 1;
			unsigned short IC2ADDGPT03ZE : 1;
			unsigned short IC3ADDGPT03ZE : 1;
			unsigned short IC4ADDGPT03ZE : 1;
			unsigned short  : 1;
			unsigned short IC6ADDGPT03ZE : 1;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short IC6ADDGPT03ZE : 1;
			unsigned short  : 1;
			unsigned short IC4ADDGPT03ZE : 1;
			unsigned short IC3ADDGPT03ZE : 1;
			unsigned short IC2ADDGPT03ZE : 1;
			unsigned short IC1ADDGPT03ZE : 1;
			unsigned short CMADDGPT03ZE : 1;
			unsigned short  : 1;
			unsigned short IC6ADDGPT02ZE : 1;
			unsigned short IC5ADDGPT02ZE : 1;
			unsigned short  : 1;
			unsigned short IC3ADDGPT02ZE : 1;
			unsigned short IC2ADDGPT02ZE : 1;
			unsigned short IC1ADDGPT02ZE : 1;
			unsigned short CMADDGPT02ZE : 1;
#endif
	} BIT;
	} POECR6;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short POE10M : 2;
			unsigned short  : 6;
			unsigned short PIE4 : 1;
			unsigned short POE10E : 1;
			unsigned short  : 2;
			unsigned short POE10F : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short POE10F : 1;
			unsigned short  : 2;
			unsigned short POE10E : 1;
			unsigned short PIE4 : 1;
			unsigned short  : 6;
			unsigned short POE10M : 2;
#endif
	} BIT;
	} ICSR4;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short POE11M : 2;
			unsigned short  : 6;
			unsigned short PIE5 : 1;
			unsigned short POE11E : 1;
			unsigned short  : 2;
			unsigned short POE11F : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short POE11F : 1;
			unsigned short  : 2;
			unsigned short POE11E : 1;
			unsigned short PIE5 : 1;
			unsigned short  : 6;
			unsigned short POE11M : 2;
#endif
	} BIT;
	} ICSR5;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short OLSG0A : 1;
			unsigned short OLSG0B : 1;
			unsigned short OLSG1A : 1;
			unsigned short OLSG1B : 1;
			unsigned short OLSG2A : 1;
			unsigned short OLSG2B : 1;
			unsigned short  : 1;
			unsigned short OLSEN : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short OLSEN : 1;
			unsigned short  : 1;
			unsigned short OLSG2B : 1;
			unsigned short OLSG2A : 1;
			unsigned short OLSG1B : 1;
			unsigned short OLSG1A : 1;
			unsigned short OLSG0B : 1;
			unsigned short OLSG0A : 1;
#endif
	} BIT;
	} ALR1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 9;
			unsigned short OSTSTE : 1;
			unsigned short  : 2;
			unsigned short OSTSTF : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short OSTSTF : 1;
			unsigned short  : 2;
			unsigned short OSTSTE : 1;
			unsigned short  : 9;
#endif
	} BIT;
	} ICSR6;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short OLSG4A : 1;
			unsigned short OLSG4B : 1;
			unsigned short OLSG5A : 1;
			unsigned short OLSG5B : 1;
			unsigned short OLSG6A : 1;
			unsigned short OLSG6B : 1;
			unsigned short  : 1;
			unsigned short OLSEN : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short OLSEN : 1;
			unsigned short  : 1;
			unsigned short OLSG6B : 1;
			unsigned short OLSG6A : 1;
			unsigned short OLSG5B : 1;
			unsigned short OLSG5A : 1;
			unsigned short OLSG4B : 1;
			unsigned short OLSG4A : 1;
#endif
	} BIT;
	} ALR2;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short POE12M : 2;
			unsigned short  : 6;
			unsigned short PIE7 : 1;
			unsigned short POE12E : 1;
			unsigned short  : 2;
			unsigned short POE12F : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short POE12F : 1;
			unsigned short  : 2;
			unsigned short POE12E : 1;
			unsigned short PIE7 : 1;
			unsigned short  : 6;
			unsigned short POE12M : 2;
#endif
	} BIT;
	} ICSR7;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MTU9AZE : 1;
			unsigned short MTU9BZE : 1;
			unsigned short MTU9CZE : 1;
			unsigned short MTU9DZE : 1;
			unsigned short MTU9A1ZE : 1;
			unsigned short MTU9B1ZE : 1;
			unsigned short MTU9C1ZE : 1;
			unsigned short MTU9D1ZE : 1;
			unsigned short MTU9A2ZE : 1;
			unsigned short  : 1;
			unsigned short MTU9C2ZE : 1;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short MTU9C2ZE : 1;
			unsigned short  : 1;
			unsigned short MTU9A2ZE : 1;
			unsigned short MTU9D1ZE : 1;
			unsigned short MTU9C1ZE : 1;
			unsigned short MTU9B1ZE : 1;
			unsigned short MTU9A1ZE : 1;
			unsigned short MTU9DZE : 1;
			unsigned short MTU9CZE : 1;
			unsigned short MTU9BZE : 1;
			unsigned short MTU9AZE : 1;
#endif
	} BIT;
	} POECR7;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMADDMT9ZE : 1;
			unsigned short IC1ADDMT9ZE : 1;
			unsigned short IC2ADDMT9ZE : 1;
			unsigned short IC3ADDMT9ZE : 1;
			unsigned short IC4ADDMT9ZE : 1;
			unsigned short IC5ADDMT9ZE : 1;
			unsigned short  : 10;
#else
			unsigned short  : 10;
			unsigned short IC5ADDMT9ZE : 1;
			unsigned short IC4ADDMT9ZE : 1;
			unsigned short IC3ADDMT9ZE : 1;
			unsigned short IC2ADDMT9ZE : 1;
			unsigned short IC1ADDMT9ZE : 1;
			unsigned short CMADDMT9ZE : 1;
#endif
	} BIT;
	} POECR8;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short C0FLAG : 1;
			unsigned short C1FLAG : 1;
			unsigned short C2FLAG : 1;
			unsigned short C3FLAG : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short C3FLAG : 1;
			unsigned short C2FLAG : 1;
			unsigned short C1FLAG : 1;
			unsigned short C0FLAG : 1;
#endif
	} BIT;
	} POECMPFR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short POEREQ0 : 1;
			unsigned short POEREQ1 : 1;
			unsigned short POEREQ2 : 1;
			unsigned short POEREQ3 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short POEREQ3 : 1;
			unsigned short POEREQ2 : 1;
			unsigned short POEREQ1 : 1;
			unsigned short POEREQ0 : 1;
#endif
	} BIT;
	} POECMPSEL;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short OIE3 : 1;
			unsigned short OCE3 : 1;
			unsigned short  : 5;
			unsigned short OSF3 : 1;
#else
			unsigned short OSF3 : 1;
			unsigned short  : 5;
			unsigned short OCE3 : 1;
			unsigned short OIE3 : 1;
			unsigned short  : 8;
#endif
	} BIT;
	} OCSR3;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short OLSG0A : 1;
			unsigned short OLSG0B : 1;
			unsigned short OLSG1A : 1;
			unsigned short OLSG1B : 1;
			unsigned short OLSG2A : 1;
			unsigned short OLSG2B : 1;
			unsigned short  : 1;
			unsigned short OLSEN : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short OLSEN : 1;
			unsigned short  : 1;
			unsigned short OLSG2B : 1;
			unsigned short OLSG2A : 1;
			unsigned short OLSG1B : 1;
			unsigned short OLSG1A : 1;
			unsigned short OLSG0B : 1;
			unsigned short OLSG0A : 1;
#endif
	} BIT;
	} ALR3;
	char           wk0[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MTU0AME : 1;
			unsigned char MTU0BME : 1;
			unsigned char MTU0CME : 1;
			unsigned char MTU0DME : 1;
			unsigned char MTU0A1ME : 1;
			unsigned char MTU0B1ME : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char MTU0B1ME : 1;
			unsigned char MTU0A1ME : 1;
			unsigned char MTU0DME : 1;
			unsigned char MTU0CME : 1;
			unsigned char MTU0BME : 1;
			unsigned char MTU0AME : 1;
#endif
	} BIT;
	} PMMCR0;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MTU7BME : 1;
			unsigned short MTU7AME : 1;
			unsigned short MTU6BME : 1;
			unsigned short MTU7DME : 1;
			unsigned short MTU7CME : 1;
			unsigned short MTU6DME : 1;
			unsigned short  : 2;
			unsigned short MTU4BME : 1;
			unsigned short MTU4AME : 1;
			unsigned short MTU3BME : 1;
			unsigned short MTU4DME : 1;
			unsigned short MTU4CME : 1;
			unsigned short MTU3DME : 1;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short MTU3DME : 1;
			unsigned short MTU4CME : 1;
			unsigned short MTU4DME : 1;
			unsigned short MTU3BME : 1;
			unsigned short MTU4AME : 1;
			unsigned short MTU4BME : 1;
			unsigned short  : 2;
			unsigned short MTU6DME : 1;
			unsigned short MTU7CME : 1;
			unsigned short MTU7DME : 1;
			unsigned short MTU6BME : 1;
			unsigned short MTU7AME : 1;
			unsigned short MTU7BME : 1;
#endif
	} BIT;
	} PMMCR1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short GPT0AME : 1;
			unsigned short GPT0BME : 1;
			unsigned short GPT1AME : 1;
			unsigned short GPT1BME : 1;
			unsigned short GPT2AME : 1;
			unsigned short GPT2BME : 1;
			unsigned short  : 2;
			unsigned short GPT0A1ME : 1;
			unsigned short GPT0B1ME : 1;
			unsigned short GPT1A1ME : 1;
			unsigned short GPT1B1ME : 1;
			unsigned short GPT2A1ME : 1;
			unsigned short GPT2B1ME : 1;
			unsigned short GPT3A1ME : 1;
			unsigned short GPT3B1ME : 1;
#else
			unsigned short GPT3B1ME : 1;
			unsigned short GPT3A1ME : 1;
			unsigned short GPT2B1ME : 1;
			unsigned short GPT2A1ME : 1;
			unsigned short GPT1B1ME : 1;
			unsigned short GPT1A1ME : 1;
			unsigned short GPT0B1ME : 1;
			unsigned short GPT0A1ME : 1;
			unsigned short  : 2;
			unsigned short GPT2BME : 1;
			unsigned short GPT2AME : 1;
			unsigned short GPT1BME : 1;
			unsigned short GPT1AME : 1;
			unsigned short GPT0BME : 1;
			unsigned short GPT0AME : 1;
#endif
	} BIT;
	} PMMCR2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MTU9AME : 1;
			unsigned short MTU9BME : 1;
			unsigned short MTU9CME : 1;
			unsigned short MTU9DME : 1;
			unsigned short MTU9A1ME : 1;
			unsigned short MTU9B1ME : 1;
			unsigned short MTU9C1ME : 1;
			unsigned short MTU9D1ME : 1;
			unsigned short MTU9A2ME : 1;
			unsigned short  : 1;
			unsigned short MTU9C2ME : 1;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short MTU9C2ME : 1;
			unsigned short  : 1;
			unsigned short MTU9A2ME : 1;
			unsigned short MTU9D1ME : 1;
			unsigned short MTU9C1ME : 1;
			unsigned short MTU9B1ME : 1;
			unsigned short MTU9A1ME : 1;
			unsigned short MTU9DME : 1;
			unsigned short MTU9CME : 1;
			unsigned short MTU9BME : 1;
			unsigned short MTU9AME : 1;
#endif
	} BIT;
	} PMMCR3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POEREQ0 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char POEREQ3 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ0 : 1;
#endif
	} BIT;
	} POECMPEX0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POEREQ0 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char POEREQ3 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ0 : 1;
#endif
	} BIT;
	} POECMPEX1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POEREQ0 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char POEREQ3 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ0 : 1;
#endif
	} BIT;
	} POECMPEX2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POEREQ0 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char POEREQ3 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ0 : 1;
#endif
	} BIT;
	} POECMPEX3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POEREQ0 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char POEREQ3 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ0 : 1;
#endif
	} BIT;
	} POECMPEX4;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POEREQ0 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char POEREQ3 : 1;
			unsigned char POEREQ2 : 1;
			unsigned char POEREQ1 : 1;
			unsigned char POEREQ0 : 1;
#endif
	} BIT;
	} POECMPEX5;
} st_poe_t;

typedef struct st_port0 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	char           wk4[63];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_port0_t;

typedef struct st_port1 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[32];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[61];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_port1_t;

typedef struct st_port2 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[33];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[60];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_port2_t;

typedef struct st_port3 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[34];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[59];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_port3_t;

typedef struct st_port4 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[127];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
} st_port4_t;

typedef struct st_port5 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[127];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
} st_port5_t;

typedef struct st_port6 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[127];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
} st_port6_t;

typedef struct st_port7 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[38];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[55];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_port7_t;

typedef struct st_port8 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[39];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[54];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_port8_t;

typedef struct st_port9 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[40];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[53];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} DSCR;
} st_port9_t;

typedef struct st_porta {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[41];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[52];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_porta_t;

typedef struct st_portb {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[42];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[51];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 2;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char  : 2;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_portb_t;

typedef struct st_portc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[43];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[50];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_portc_t;

typedef struct st_portd {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[44];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[49];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_portd_t;

typedef struct st_porte {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[45];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 3;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 3;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR1;
	char           wk4[48];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_porte_t;

typedef struct st_portf {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[46];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	char           wk4[48];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_portf_t;

typedef struct st_portg {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[47];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	char           wk4[47];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_portg_t;

typedef struct st_riic {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SDAI : 1;
			unsigned char SCLI : 1;
			unsigned char SDAO : 1;
			unsigned char SCLO : 1;
			unsigned char SOWP : 1;
			unsigned char CLO : 1;
			unsigned char IICRST : 1;
			unsigned char ICE : 1;
#else
			unsigned char ICE : 1;
			unsigned char IICRST : 1;
			unsigned char CLO : 1;
			unsigned char SOWP : 1;
			unsigned char SCLO : 1;
			unsigned char SDAO : 1;
			unsigned char SCLI : 1;
			unsigned char SDAI : 1;
#endif
	} BIT;
	} ICCR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char ST : 1;
			unsigned char RS : 1;
			unsigned char SP : 1;
			unsigned char  : 1;
			unsigned char TRS : 1;
			unsigned char MST : 1;
			unsigned char BBSY : 1;
#else
			unsigned char BBSY : 1;
			unsigned char MST : 1;
			unsigned char TRS : 1;
			unsigned char  : 1;
			unsigned char SP : 1;
			unsigned char RS : 1;
			unsigned char ST : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} ICCR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BC : 3;
			unsigned char BCWP : 1;
			unsigned char CKS : 3;
			unsigned char MTWP : 1;
#else
			unsigned char MTWP : 1;
			unsigned char CKS : 3;
			unsigned char BCWP : 1;
			unsigned char BC : 3;
#endif
	} BIT;
	} ICMR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMOS : 1;
			unsigned char TMOL : 1;
			unsigned char TMOH : 1;
			unsigned char  : 1;
			unsigned char SDDL : 3;
			unsigned char DLCS : 1;
#else
			unsigned char DLCS : 1;
			unsigned char SDDL : 3;
			unsigned char  : 1;
			unsigned char TMOH : 1;
			unsigned char TMOL : 1;
			unsigned char TMOS : 1;
#endif
	} BIT;
	} ICMR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NF : 2;
			unsigned char ACKBR : 1;
			unsigned char ACKBT : 1;
			unsigned char ACKWP : 1;
			unsigned char RDRFS : 1;
			unsigned char WAIT : 1;
			unsigned char SMBS : 1;
#else
			unsigned char SMBS : 1;
			unsigned char WAIT : 1;
			unsigned char RDRFS : 1;
			unsigned char ACKWP : 1;
			unsigned char ACKBT : 1;
			unsigned char ACKBR : 1;
			unsigned char NF : 2;
#endif
	} BIT;
	} ICMR3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMOE : 1;
			unsigned char MALE : 1;
			unsigned char NALE : 1;
			unsigned char SALE : 1;
			unsigned char NACKE : 1;
			unsigned char NFE : 1;
			unsigned char SCLE : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char SCLE : 1;
			unsigned char NFE : 1;
			unsigned char NACKE : 1;
			unsigned char SALE : 1;
			unsigned char NALE : 1;
			unsigned char MALE : 1;
			unsigned char TMOE : 1;
#endif
	} BIT;
	} ICFER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SAR0E : 1;
			unsigned char SAR1E : 1;
			unsigned char SAR2E : 1;
			unsigned char GCAE : 1;
			unsigned char  : 1;
			unsigned char DIDE : 1;
			unsigned char  : 1;
			unsigned char HOAE : 1;
#else
			unsigned char HOAE : 1;
			unsigned char  : 1;
			unsigned char DIDE : 1;
			unsigned char  : 1;
			unsigned char GCAE : 1;
			unsigned char SAR2E : 1;
			unsigned char SAR1E : 1;
			unsigned char SAR0E : 1;
#endif
	} BIT;
	} ICSER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMOIE : 1;
			unsigned char ALIE : 1;
			unsigned char STIE : 1;
			unsigned char SPIE : 1;
			unsigned char NAKIE : 1;
			unsigned char RIE : 1;
			unsigned char TEIE : 1;
			unsigned char TIE : 1;
#else
			unsigned char TIE : 1;
			unsigned char TEIE : 1;
			unsigned char RIE : 1;
			unsigned char NAKIE : 1;
			unsigned char SPIE : 1;
			unsigned char STIE : 1;
			unsigned char ALIE : 1;
			unsigned char TMOIE : 1;
#endif
	} BIT;
	} ICIER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char AAS0 : 1;
			unsigned char AAS1 : 1;
			unsigned char AAS2 : 1;
			unsigned char GCA : 1;
			unsigned char  : 1;
			unsigned char DID : 1;
			unsigned char  : 1;
			unsigned char HOA : 1;
#else
			unsigned char HOA : 1;
			unsigned char  : 1;
			unsigned char DID : 1;
			unsigned char  : 1;
			unsigned char GCA : 1;
			unsigned char AAS2 : 1;
			unsigned char AAS1 : 1;
			unsigned char AAS0 : 1;
#endif
	} BIT;
	} ICSR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMOF : 1;
			unsigned char AL : 1;
			unsigned char START : 1;
			unsigned char STOP : 1;
			unsigned char NACKF : 1;
			unsigned char RDRF : 1;
			unsigned char TEND : 1;
			unsigned char TDRE : 1;
#else
			unsigned char TDRE : 1;
			unsigned char TEND : 1;
			unsigned char RDRF : 1;
			unsigned char NACKF : 1;
			unsigned char STOP : 1;
			unsigned char START : 1;
			unsigned char AL : 1;
			unsigned char TMOF : 1;
#endif
	} BIT;
	} ICSR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SVA0 : 1;
			unsigned char SVA : 7;
#else
			unsigned char SVA : 7;
			unsigned char SVA0 : 1;
#endif
	} BIT;
	} SARL0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FS : 1;
			unsigned char SVA : 2;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SVA : 2;
			unsigned char FS : 1;
#endif
	} BIT;
	} SARU0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SVA0 : 1;
			unsigned char SVA : 7;
#else
			unsigned char SVA : 7;
			unsigned char SVA0 : 1;
#endif
	} BIT;
	} SARL1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FS : 1;
			unsigned char SVA : 2;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SVA : 2;
			unsigned char FS : 1;
#endif
	} BIT;
	} SARU1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SVA0 : 1;
			unsigned char SVA : 7;
#else
			unsigned char SVA : 7;
			unsigned char SVA0 : 1;
#endif
	} BIT;
	} SARL2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FS : 1;
			unsigned char SVA : 2;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SVA : 2;
			unsigned char FS : 1;
#endif
	} BIT;
	} SARU2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BRL : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char BRL : 5;
#endif
	} BIT;
	} ICBRL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BRH : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char BRH : 5;
#endif
	} BIT;
	} ICBRH;
	unsigned char  ICDRT;
	unsigned char  ICDRR;
} st_riic_t;

typedef struct st_rspi {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SPMS : 1;
			unsigned char TXMD : 1;
			unsigned char MODFEN : 1;
			unsigned char MSTR : 1;
			unsigned char SPEIE : 1;
			unsigned char SPTIE : 1;
			unsigned char SPE : 1;
			unsigned char SPRIE : 1;
#else
			unsigned char SPRIE : 1;
			unsigned char SPE : 1;
			unsigned char SPTIE : 1;
			unsigned char SPEIE : 1;
			unsigned char MSTR : 1;
			unsigned char MODFEN : 1;
			unsigned char TXMD : 1;
			unsigned char SPMS : 1;
#endif
	} BIT;
	} SPCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SSL0P : 1;
			unsigned char SSL1P : 1;
			unsigned char SSL2P : 1;
			unsigned char SSL3P : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char SSL3P : 1;
			unsigned char SSL2P : 1;
			unsigned char SSL1P : 1;
			unsigned char SSL0P : 1;
#endif
	} BIT;
	} SSLP;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SPLP : 1;
			unsigned char SPLP2 : 1;
			unsigned char  : 2;
			unsigned char MOIFV : 1;
			unsigned char MOIFE : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char MOIFE : 1;
			unsigned char MOIFV : 1;
			unsigned char  : 2;
			unsigned char SPLP2 : 1;
			unsigned char SPLP : 1;
#endif
	} BIT;
	} SPPCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OVRF : 1;
			unsigned char IDLNF : 1;
			unsigned char MODF : 1;
			unsigned char PERF : 1;
			unsigned char UDRF : 1;
			unsigned char SPTEF : 1;
			unsigned char  : 1;
			unsigned char SPRF : 1;
#else
			unsigned char SPRF : 1;
			unsigned char  : 1;
			unsigned char SPTEF : 1;
			unsigned char UDRF : 1;
			unsigned char PERF : 1;
			unsigned char MODF : 1;
			unsigned char IDLNF : 1;
			unsigned char OVRF : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SPSLN : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SPSLN : 3;
#endif
	} BIT;
	} SPSCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SPCP : 3;
			unsigned char  : 1;
			unsigned char SPECM : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char SPECM : 3;
			unsigned char  : 1;
			unsigned char SPCP : 3;
#endif
	} BIT;
	} SPSSR;
	unsigned char  SPBR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SPFC : 2;
			unsigned char  : 2;
			unsigned char SPRDTD : 1;
			unsigned char SPLW : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char SPLW : 1;
			unsigned char SPRDTD : 1;
			unsigned char  : 2;
			unsigned char SPFC : 2;
#endif
	} BIT;
	} SPDCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SCKDL : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SCKDL : 3;
#endif
	} BIT;
	} SPCKD;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SLNDL : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SLNDL : 3;
#endif
	} BIT;
	} SSLND;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SPNDL : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SPNDL : 3;
#endif
	} BIT;
	} SPND;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SPPE : 1;
			unsigned char SPOE : 1;
			unsigned char SPIIE : 1;
			unsigned char PTE : 1;
			unsigned char SCKASE : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char SCKASE : 1;
			unsigned char PTE : 1;
			unsigned char SPIIE : 1;
			unsigned char SPOE : 1;
			unsigned char SPPE : 1;
#endif
	} BIT;
	} SPCR2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CPHA : 1;
			unsigned short CPOL : 1;
			unsigned short BRDV : 2;
			unsigned short SSLA : 3;
			unsigned short SSLKP : 1;
			unsigned short SPB : 4;
			unsigned short LSBF : 1;
			unsigned short SPNDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SCKDEN : 1;
#else
			unsigned short SCKDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SPNDEN : 1;
			unsigned short LSBF : 1;
			unsigned short SPB : 4;
			unsigned short SSLKP : 1;
			unsigned short SSLA : 3;
			unsigned short BRDV : 2;
			unsigned short CPOL : 1;
			unsigned short CPHA : 1;
#endif
	} BIT;
	} SPCMD0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CPHA : 1;
			unsigned short CPOL : 1;
			unsigned short BRDV : 2;
			unsigned short SSLA : 3;
			unsigned short SSLKP : 1;
			unsigned short SPB : 4;
			unsigned short LSBF : 1;
			unsigned short SPNDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SCKDEN : 1;
#else
			unsigned short SCKDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SPNDEN : 1;
			unsigned short LSBF : 1;
			unsigned short SPB : 4;
			unsigned short SSLKP : 1;
			unsigned short SSLA : 3;
			unsigned short BRDV : 2;
			unsigned short CPOL : 1;
			unsigned short CPHA : 1;
#endif
	} BIT;
	} SPCMD1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CPHA : 1;
			unsigned short CPOL : 1;
			unsigned short BRDV : 2;
			unsigned short SSLA : 3;
			unsigned short SSLKP : 1;
			unsigned short SPB : 4;
			unsigned short LSBF : 1;
			unsigned short SPNDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SCKDEN : 1;
#else
			unsigned short SCKDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SPNDEN : 1;
			unsigned short LSBF : 1;
			unsigned short SPB : 4;
			unsigned short SSLKP : 1;
			unsigned short SSLA : 3;
			unsigned short BRDV : 2;
			unsigned short CPOL : 1;
			unsigned short CPHA : 1;
#endif
	} BIT;
	} SPCMD2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CPHA : 1;
			unsigned short CPOL : 1;
			unsigned short BRDV : 2;
			unsigned short SSLA : 3;
			unsigned short SSLKP : 1;
			unsigned short SPB : 4;
			unsigned short LSBF : 1;
			unsigned short SPNDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SCKDEN : 1;
#else
			unsigned short SCKDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SPNDEN : 1;
			unsigned short LSBF : 1;
			unsigned short SPB : 4;
			unsigned short SSLKP : 1;
			unsigned short SSLA : 3;
			unsigned short BRDV : 2;
			unsigned short CPOL : 1;
			unsigned short CPHA : 1;
#endif
	} BIT;
	} SPCMD3;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CPHA : 1;
			unsigned short CPOL : 1;
			unsigned short BRDV : 2;
			unsigned short SSLA : 3;
			unsigned short SSLKP : 1;
			unsigned short SPB : 4;
			unsigned short LSBF : 1;
			unsigned short SPNDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SCKDEN : 1;
#else
			unsigned short SCKDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SPNDEN : 1;
			unsigned short LSBF : 1;
			unsigned short SPB : 4;
			unsigned short SSLKP : 1;
			unsigned short SSLA : 3;
			unsigned short BRDV : 2;
			unsigned short CPOL : 1;
			unsigned short CPHA : 1;
#endif
	} BIT;
	} SPCMD4;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CPHA : 1;
			unsigned short CPOL : 1;
			unsigned short BRDV : 2;
			unsigned short SSLA : 3;
			unsigned short SSLKP : 1;
			unsigned short SPB : 4;
			unsigned short LSBF : 1;
			unsigned short SPNDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SCKDEN : 1;
#else
			unsigned short SCKDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SPNDEN : 1;
			unsigned short LSBF : 1;
			unsigned short SPB : 4;
			unsigned short SSLKP : 1;
			unsigned short SSLA : 3;
			unsigned short BRDV : 2;
			unsigned short CPOL : 1;
			unsigned short CPHA : 1;
#endif
	} BIT;
	} SPCMD5;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CPHA : 1;
			unsigned short CPOL : 1;
			unsigned short BRDV : 2;
			unsigned short SSLA : 3;
			unsigned short SSLKP : 1;
			unsigned short SPB : 4;
			unsigned short LSBF : 1;
			unsigned short SPNDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SCKDEN : 1;
#else
			unsigned short SCKDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SPNDEN : 1;
			unsigned short LSBF : 1;
			unsigned short SPB : 4;
			unsigned short SSLKP : 1;
			unsigned short SSLA : 3;
			unsigned short BRDV : 2;
			unsigned short CPOL : 1;
			unsigned short CPHA : 1;
#endif
	} BIT;
	} SPCMD6;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CPHA : 1;
			unsigned short CPOL : 1;
			unsigned short BRDV : 2;
			unsigned short SSLA : 3;
			unsigned short SSLKP : 1;
			unsigned short SPB : 4;
			unsigned short LSBF : 1;
			unsigned short SPNDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SCKDEN : 1;
#else
			unsigned short SCKDEN : 1;
			unsigned short SLNDEN : 1;
			unsigned short SPNDEN : 1;
			unsigned short LSBF : 1;
			unsigned short SPB : 4;
			unsigned short SSLKP : 1;
			unsigned short SSLA : 3;
			unsigned short BRDV : 2;
			unsigned short CPOL : 1;
			unsigned short CPHA : 1;
#endif
	} BIT;
	} SPCMD7;
} st_rspi_t;

typedef struct st_s12ad {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DBLANS : 5;
			unsigned short  : 1;
			unsigned short GBADIE : 1;
			unsigned short DBLE : 1;
			unsigned short EXTRG : 1;
			unsigned short TRGE : 1;
			unsigned short  : 2;
			unsigned short ADIE : 1;
			unsigned short ADCS : 2;
			unsigned short ADST : 1;
#else
			unsigned short ADST : 1;
			unsigned short ADCS : 2;
			unsigned short ADIE : 1;
			unsigned short  : 2;
			unsigned short TRGE : 1;
			unsigned short EXTRG : 1;
			unsigned short DBLE : 1;
			unsigned short GBADIE : 1;
			unsigned short  : 1;
			unsigned short DBLANS : 5;
#endif
	} BIT;
	} ADCSR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSA000 : 1;
			unsigned short ANSA001 : 1;
			unsigned short ANSA002 : 1;
			unsigned short ANSA003 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short ANSA003 : 1;
			unsigned short ANSA002 : 1;
			unsigned short ANSA001 : 1;
			unsigned short ANSA000 : 1;
#endif
	} BIT;
	} ADANSA0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSA100 : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ANSA100 : 1;
#endif
	} BIT;
	} ADANSA1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ADS000 : 1;
			unsigned short ADS001 : 1;
			unsigned short ADS002 : 1;
			unsigned short ADS003 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short ADS003 : 1;
			unsigned short ADS002 : 1;
			unsigned short ADS001 : 1;
			unsigned short ADS000 : 1;
#endif
	} BIT;
	} ADADS0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ADS100 : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ADS100 : 1;
#endif
	} BIT;
	} ADADS1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADC : 3;
			unsigned char  : 4;
			unsigned char AVEE : 1;
#else
			unsigned char AVEE : 1;
			unsigned char  : 4;
			unsigned char ADC : 3;
#endif
	} BIT;
	} ADADC;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 5;
			unsigned short ACE : 1;
			unsigned short  : 2;
			unsigned short DIAGVAL : 2;
			unsigned short DIAGLD : 1;
			unsigned short DIAGM : 1;
			unsigned short  : 3;
			unsigned short ADRFMT : 1;
#else
			unsigned short ADRFMT : 1;
			unsigned short  : 3;
			unsigned short DIAGM : 1;
			unsigned short DIAGLD : 1;
			unsigned short DIAGVAL : 2;
			unsigned short  : 2;
			unsigned short ACE : 1;
			unsigned short  : 5;
#endif
	} BIT;
	} ADCER;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TRSB : 6;
			unsigned short  : 2;
			unsigned short TRSA : 6;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short TRSA : 6;
			unsigned short  : 2;
			unsigned short TRSB : 6;
#endif
	} BIT;
	} ADSTRGR;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSB000 : 1;
			unsigned short ANSB001 : 1;
			unsigned short ANSB002 : 1;
			unsigned short ANSB003 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short ANSB003 : 1;
			unsigned short ANSB002 : 1;
			unsigned short ANSB001 : 1;
			unsigned short ANSB000 : 1;
#endif
	} BIT;
	} ADANSB0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSB100 : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ANSB100 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADNDIS : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char ADNDIS : 5;
#endif
	} BIT;
	} ADDISCR;
	char           wk6[5];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PGS : 1;
			unsigned short GBRSCN : 1;
			unsigned short  : 12;
			unsigned short LGRRS : 1;
			unsigned short GBRP : 1;
#else
			unsigned short GBRP : 1;
			unsigned short LGRRS : 1;
			unsigned short  : 12;
			unsigned short GBRSCN : 1;
			unsigned short PGS : 1;
#endif
	} BIT;
	} ADGSPCR;
	char           wk7[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk8[76];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSC000 : 1;
			unsigned short ANSC001 : 1;
			unsigned short ANSC002 : 1;
			unsigned short ANSC003 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short ANSC003 : 1;
			unsigned short ANSC002 : 1;
			unsigned short ANSC001 : 1;
			unsigned short ANSC000 : 1;
#endif
	} BIT;
	} ADANSC0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSC100 : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ANSC100 : 1;
#endif
	} BIT;
	} ADANSC1;
	char           wk9[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TRSC : 6;
			unsigned char GCADIE : 1;
			unsigned char GRCE : 1;
#else
			unsigned char GRCE : 1;
			unsigned char GCADIE : 1;
			unsigned char TRSC : 6;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 1;
			unsigned short P000SEL1 : 1;
			unsigned short P000ENAMP : 1;
			unsigned short  : 13;
#else
			unsigned short  : 13;
			unsigned short P000ENAMP : 1;
			unsigned short P000SEL1 : 1;
			unsigned short  : 1;
#endif
	} BIT;
	} ADPGACR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short P000GAIN : 4;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short P000GAIN : 4;
#endif
	} BIT;
	} ADPGAGS0;
} st_s12ad_t;

typedef struct st_s12ad1 {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DBLANS : 5;
			unsigned short  : 1;
			unsigned short GBADIE : 1;
			unsigned short DBLE : 1;
			unsigned short EXTRG : 1;
			unsigned short TRGE : 1;
			unsigned short  : 2;
			unsigned short ADIE : 1;
			unsigned short ADCS : 2;
			unsigned short ADST : 1;
#else
			unsigned short ADST : 1;
			unsigned short ADCS : 2;
			unsigned short ADIE : 1;
			unsigned short  : 2;
			unsigned short TRGE : 1;
			unsigned short EXTRG : 1;
			unsigned short DBLE : 1;
			unsigned short GBADIE : 1;
			unsigned short  : 1;
			unsigned short DBLANS : 5;
#endif
	} BIT;
	} ADCSR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSA000 : 1;
			unsigned short ANSA001 : 1;
			unsigned short ANSA002 : 1;
			unsigned short ANSA003 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short ANSA003 : 1;
			unsigned short ANSA002 : 1;
			unsigned short ANSA001 : 1;
			unsigned short ANSA000 : 1;
#endif
	} BIT;
	} ADANSA0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSA100 : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ANSA100 : 1;
#endif
	} BIT;
	} ADANSA1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ADS000 : 1;
			unsigned short ADS001 : 1;
			unsigned short ADS002 : 1;
			unsigned short ADS003 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short ADS003 : 1;
			unsigned short ADS002 : 1;
			unsigned short ADS001 : 1;
			unsigned short ADS000 : 1;
#endif
	} BIT;
	} ADADS0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ADS100 : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ADS100 : 1;
#endif
	} BIT;
	} ADADS1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADC : 3;
			unsigned char  : 4;
			unsigned char AVEE : 1;
#else
			unsigned char AVEE : 1;
			unsigned char  : 4;
			unsigned char ADC : 3;
#endif
	} BIT;
	} ADADC;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 5;
			unsigned short ACE : 1;
			unsigned short  : 2;
			unsigned short DIAGVAL : 2;
			unsigned short DIAGLD : 1;
			unsigned short DIAGM : 1;
			unsigned short  : 3;
			unsigned short ADRFMT : 1;
#else
			unsigned short ADRFMT : 1;
			unsigned short  : 3;
			unsigned short DIAGM : 1;
			unsigned short DIAGLD : 1;
			unsigned short DIAGVAL : 2;
			unsigned short  : 2;
			unsigned short ACE : 1;
			unsigned short  : 5;
#endif
	} BIT;
	} ADCER;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TRSB : 6;
			unsigned short  : 2;
			unsigned short TRSA : 6;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short TRSA : 6;
			unsigned short  : 2;
			unsigned short TRSB : 6;
#endif
	} BIT;
	} ADSTRGR;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSB000 : 1;
			unsigned short ANSB001 : 1;
			unsigned short ANSB002 : 1;
			unsigned short ANSB003 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short ANSB003 : 1;
			unsigned short ANSB002 : 1;
			unsigned short ANSB001 : 1;
			unsigned short ANSB000 : 1;
#endif
	} BIT;
	} ADANSB0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSB100 : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ANSB100 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short SSTSH : 8;
			unsigned short SHANS : 3;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short SHANS : 3;
			unsigned short SSTSH : 8;
#endif
	} BIT;
	} ADSHCR;
	char           wk6[18];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADNDIS : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char ADNDIS : 5;
#endif
	} BIT;
	} ADDISCR;
	char           wk7[5];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PGS : 1;
			unsigned short GBRSCN : 1;
			unsigned short  : 12;
			unsigned short LGRRS : 1;
			unsigned short GBRP : 1;
#else
			unsigned short GBRP : 1;
			unsigned short LGRRS : 1;
			unsigned short  : 12;
			unsigned short GBRSCN : 1;
			unsigned short PGS : 1;
#endif
	} BIT;
	} ADGSPCR;
	char           wk8[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk9[76];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSC000 : 1;
			unsigned short ANSC001 : 1;
			unsigned short ANSC002 : 1;
			unsigned short ANSC003 : 1;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short ANSC003 : 1;
			unsigned short ANSC002 : 1;
			unsigned short ANSC001 : 1;
			unsigned short ANSC000 : 1;
#endif
	} BIT;
	} ADANSC0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSC100 : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ANSC100 : 1;
#endif
	} BIT;
	} ADANSC1;
	char           wk10[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TRSC : 6;
			unsigned char GCADIE : 1;
			unsigned char GRCE : 1;
#else
			unsigned char GRCE : 1;
			unsigned char GCADIE : 1;
			unsigned char TRSC : 6;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 1;
			unsigned short P100SEL1 : 1;
			unsigned short P100ENAMP : 1;
			unsigned short  : 2;
			unsigned short P101SEL1 : 1;
			unsigned short P101ENAMP : 1;
			unsigned short  : 2;
			unsigned short P102SEL1 : 1;
			unsigned short P102ENAMP : 1;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short P102ENAMP : 1;
			unsigned short P102SEL1 : 1;
			unsigned short  : 2;
			unsigned short P101ENAMP : 1;
			unsigned short P101SEL1 : 1;
			unsigned short  : 2;
			unsigned short P100ENAMP : 1;
			unsigned short P100SEL1 : 1;
			unsigned short  : 1;
#endif
	} BIT;
	} ADPGACR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short P100GAIN : 4;
			unsigned short P101GAIN : 4;
			unsigned short P102GAIN : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short P102GAIN : 4;
			unsigned short P101GAIN : 4;
			unsigned short P100GAIN : 4;
#endif
	} BIT;
	} ADPGAGS0;
} st_s12ad1_t;

typedef struct st_s12ad2 {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DBLANS : 5;
			unsigned short  : 1;
			unsigned short GBADIE : 1;
			unsigned short DBLE : 1;
			unsigned short EXTRG : 1;
			unsigned short TRGE : 1;
			unsigned short  : 2;
			unsigned short ADIE : 1;
			unsigned short ADCS : 2;
			unsigned short ADST : 1;
#else
			unsigned short ADST : 1;
			unsigned short ADCS : 2;
			unsigned short ADIE : 1;
			unsigned short  : 2;
			unsigned short TRGE : 1;
			unsigned short EXTRG : 1;
			unsigned short DBLE : 1;
			unsigned short GBADIE : 1;
			unsigned short  : 1;
			unsigned short DBLANS : 5;
#endif
	} BIT;
	} ADCSR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSA000 : 1;
			unsigned short ANSA001 : 1;
			unsigned short ANSA002 : 1;
			unsigned short ANSA003 : 1;
			unsigned short ANSA004 : 1;
			unsigned short ANSA005 : 1;
			unsigned short ANSA006 : 1;
			unsigned short ANSA007 : 1;
			unsigned short ANSA008 : 1;
			unsigned short ANSA009 : 1;
			unsigned short ANSA010 : 1;
			unsigned short ANSA011 : 1;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short ANSA011 : 1;
			unsigned short ANSA010 : 1;
			unsigned short ANSA009 : 1;
			unsigned short ANSA008 : 1;
			unsigned short ANSA007 : 1;
			unsigned short ANSA006 : 1;
			unsigned short ANSA005 : 1;
			unsigned short ANSA004 : 1;
			unsigned short ANSA003 : 1;
			unsigned short ANSA002 : 1;
			unsigned short ANSA001 : 1;
			unsigned short ANSA000 : 1;
#endif
	} BIT;
	} ADANSA0;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ADS000 : 1;
			unsigned short ADS001 : 1;
			unsigned short ADS002 : 1;
			unsigned short ADS003 : 1;
			unsigned short ADS004 : 1;
			unsigned short ADS005 : 1;
			unsigned short ADS006 : 1;
			unsigned short ADS007 : 1;
			unsigned short ADS008 : 1;
			unsigned short ADS009 : 1;
			unsigned short ADS010 : 1;
			unsigned short ADS011 : 1;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short ADS011 : 1;
			unsigned short ADS010 : 1;
			unsigned short ADS009 : 1;
			unsigned short ADS008 : 1;
			unsigned short ADS007 : 1;
			unsigned short ADS006 : 1;
			unsigned short ADS005 : 1;
			unsigned short ADS004 : 1;
			unsigned short ADS003 : 1;
			unsigned short ADS002 : 1;
			unsigned short ADS001 : 1;
			unsigned short ADS000 : 1;
#endif
	} BIT;
	} ADADS0;
	char           wk2[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADC : 3;
			unsigned char  : 4;
			unsigned char AVEE : 1;
#else
			unsigned char AVEE : 1;
			unsigned char  : 4;
			unsigned char ADC : 3;
#endif
	} BIT;
	} ADADC;
	char           wk3[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 5;
			unsigned short ACE : 1;
			unsigned short  : 2;
			unsigned short DIAGVAL : 2;
			unsigned short DIAGLD : 1;
			unsigned short DIAGM : 1;
			unsigned short  : 3;
			unsigned short ADRFMT : 1;
#else
			unsigned short ADRFMT : 1;
			unsigned short  : 3;
			unsigned short DIAGM : 1;
			unsigned short DIAGLD : 1;
			unsigned short DIAGVAL : 2;
			unsigned short  : 2;
			unsigned short ACE : 1;
			unsigned short  : 5;
#endif
	} BIT;
	} ADCER;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TRSB : 6;
			unsigned short  : 2;
			unsigned short TRSA : 6;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short TRSA : 6;
			unsigned short  : 2;
			unsigned short TRSB : 6;
#endif
	} BIT;
	} ADSTRGR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 1;
			unsigned short OCSAD : 1;
			unsigned short  : 7;
			unsigned short OCSA : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short OCSA : 1;
			unsigned short  : 7;
			unsigned short OCSAD : 1;
			unsigned short  : 1;
#endif
	} BIT;
	} ADEXICR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSB000 : 1;
			unsigned short ANSB001 : 1;
			unsigned short ANSB002 : 1;
			unsigned short ANSB003 : 1;
			unsigned short ANSB004 : 1;
			unsigned short ANSB005 : 1;
			unsigned short ANSB006 : 1;
			unsigned short ANSB007 : 1;
			unsigned short ANSB008 : 1;
			unsigned short ANSB009 : 1;
			unsigned short ANSB010 : 1;
			unsigned short ANSB011 : 1;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short ANSB011 : 1;
			unsigned short ANSB010 : 1;
			unsigned short ANSB009 : 1;
			unsigned short ANSB008 : 1;
			unsigned short ANSB007 : 1;
			unsigned short ANSB006 : 1;
			unsigned short ANSB005 : 1;
			unsigned short ANSB004 : 1;
			unsigned short ANSB003 : 1;
			unsigned short ANSB002 : 1;
			unsigned short ANSB001 : 1;
			unsigned short ANSB000 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADNDIS : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char ADNDIS : 5;
#endif
	} BIT;
	} ADDISCR;
	char           wk7[5];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PGS : 1;
			unsigned short GBRSCN : 1;
			unsigned short  : 12;
			unsigned short LGRRS : 1;
			unsigned short GBRP : 1;
#else
			unsigned short GBRP : 1;
			unsigned short LGRRS : 1;
			unsigned short  : 12;
			unsigned short GBRSCN : 1;
			unsigned short PGS : 1;
#endif
	} BIT;
	} ADGSPCR;
	char           wk8[2];
	unsigned short ADDBLDRA;
	unsigned short ADDBLDRB;
	char           wk9[76];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSC000 : 1;
			unsigned short ANSC001 : 1;
			unsigned short ANSC002 : 1;
			unsigned short ANSC003 : 1;
			unsigned short ANSC004 : 1;
			unsigned short ANSC005 : 1;
			unsigned short ANSC006 : 1;
			unsigned short ANSC007 : 1;
			unsigned short ANSC008 : 1;
			unsigned short ANSC009 : 1;
			unsigned short ANSC010 : 1;
			unsigned short ANSC011 : 1;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short ANSC011 : 1;
			unsigned short ANSC010 : 1;
			unsigned short ANSC009 : 1;
			unsigned short ANSC008 : 1;
			unsigned short ANSC007 : 1;
			unsigned short ANSC006 : 1;
			unsigned short ANSC005 : 1;
			unsigned short ANSC004 : 1;
			unsigned short ANSC003 : 1;
			unsigned short ANSC002 : 1;
			unsigned short ANSC001 : 1;
			unsigned short ANSC000 : 1;
#endif
	} BIT;
	} ADANSC0;
	char           wk10[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TRSC : 6;
			unsigned char GCADIE : 1;
			unsigned char GRCE : 1;
#else
			unsigned char GRCE : 1;
			unsigned char GCADIE : 1;
			unsigned char TRSC : 6;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CKS : 2;
			unsigned char MP : 1;
			unsigned char STOP : 1;
			unsigned char PM : 1;
			unsigned char PE : 1;
			unsigned char CHR : 1;
			unsigned char CM : 1;
#else
			unsigned char CM : 1;
			unsigned char CHR : 1;
			unsigned char PE : 1;
			unsigned char PM : 1;
			unsigned char STOP : 1;
			unsigned char MP : 1;
			unsigned char CKS : 2;
#endif
	} BIT;
	} SMR;
	unsigned char  BRR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CKE : 2;
			unsigned char TEIE : 1;
			unsigned char MPIE : 1;
			unsigned char RE : 1;
			unsigned char TE : 1;
			unsigned char RIE : 1;
			unsigned char TIE : 1;
#else
			unsigned char TIE : 1;
			unsigned char RIE : 1;
			unsigned char TE : 1;
			unsigned char RE : 1;
			unsigned char MPIE : 1;
			unsigned char TEIE : 1;
			unsigned char CKE : 2;
#endif
	} BIT;
	} SCR;
	unsigned char  TDR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MPBT : 1;
			unsigned char MPB : 1;
			unsigned char TEND : 1;
			unsigned char PER : 1;
			unsigned char FER : 1;
			unsigned char ORER : 1;
			unsigned char RDRF : 1;
			unsigned char TDRE : 1;
#else
			unsigned char TDRE : 1;
			unsigned char RDRF : 1;
			unsigned char ORER : 1;
			unsigned char FER : 1;
			unsigned char PER : 1;
			unsigned char TEND : 1;
			unsigned char MPB : 1;
			unsigned char MPBT : 1;
#endif
	} BIT;
	} SSR;
	unsigned char  RDR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SMIF : 1;
			unsigned char  : 1;
			unsigned char SINV : 1;
			unsigned char SDIR : 1;
			unsigned char CHR1 : 1;
			unsigned char  : 2;
			unsigned char BCP2 : 1;
#else
			unsigned char BCP2 : 1;
			unsigned char  : 2;
			unsigned char CHR1 : 1;
			unsigned char SDIR : 1;
			unsigned char SINV : 1;
			unsigned char  : 1;
			unsigned char SMIF : 1;
#endif
	} BIT;
	} SCMR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ACS0 : 1;
			unsigned char  : 1;
			unsigned char BRME : 1;
			unsigned char  : 1;
			unsigned char ABCS : 1;
			unsigned char NFEN : 1;
			unsigned char BGDM : 1;
			unsigned char RXDESEL : 1;
#else
			unsigned char RXDESEL : 1;
			unsigned char BGDM : 1;
			unsigned char NFEN : 1;
			unsigned char ABCS : 1;
			unsigned char  : 1;
			unsigned char BRME : 1;
			unsigned char  : 1;
			unsigned char ACS0 : 1;
#endif
	} BIT;
	} SEMR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NFCS : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char NFCS : 3;
#endif
	} BIT;
	} SNFR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IICM : 1;
			unsigned char  : 2;
			unsigned char IICDL : 5;
#else
			unsigned char IICDL : 5;
			unsigned char  : 2;
			unsigned char IICM : 1;
#endif
	} BIT;
	} SIMR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IICINTM : 1;
			unsigned char IICCSC : 1;
			unsigned char  : 3;
			unsigned char IICACKT : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char IICACKT : 1;
			unsigned char  : 3;
			unsigned char IICCSC : 1;
			unsigned char IICINTM : 1;
#endif
	} BIT;
	} SIMR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IICSTAREQ : 1;
			unsigned char IICRSTAREQ : 1;
			unsigned char IICSTPREQ : 1;
			unsigned char IICSTIF : 1;
			unsigned char IICSDAS : 2;
			unsigned char IICSCLS : 2;
#else
			unsigned char IICSCLS : 2;
			unsigned char IICSDAS : 2;
			unsigned char IICSTIF : 1;
			unsigned char IICSTPREQ : 1;
			unsigned char IICRSTAREQ : 1;
			unsigned char IICSTAREQ : 1;
#endif
	} BIT;
	} SIMR3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IICACKR : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char IICACKR : 1;
#endif
	} BIT;
	} SISR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SSE : 1;
			unsigned char CTSE : 1;
			unsigned char MSS : 1;
			unsigned char  : 1;
			unsigned char MFF : 1;
			unsigned char  : 1;
			unsigned char CKPOL : 1;
			unsigned char CKPH : 1;
#else
			unsigned char CKPH : 1;
			unsigned char CKPOL : 1;
			unsigned char  : 1;
			unsigned char MFF : 1;
			unsigned char  : 1;
			unsigned char MSS : 1;
			unsigned char CTSE : 1;
			unsigned char SSE : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CKS : 2;
			unsigned char BCP : 2;
			unsigned char PM : 1;
			unsigned char PE : 1;
			unsigned char BLK : 1;
			unsigned char GM : 1;
#else
			unsigned char GM : 1;
			unsigned char BLK : 1;
			unsigned char PE : 1;
			unsigned char PM : 1;
			unsigned char BCP : 2;
			unsigned char CKS : 2;
#endif
	} BIT;
	} SMR;
	unsigned char  BRR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CKE : 2;
			unsigned char TEIE : 1;
			unsigned char MPIE : 1;
			unsigned char RE : 1;
			unsigned char TE : 1;
			unsigned char RIE : 1;
			unsigned char TIE : 1;
#else
			unsigned char TIE : 1;
			unsigned char RIE : 1;
			unsigned char TE : 1;
			unsigned char RE : 1;
			unsigned char MPIE : 1;
			unsigned char TEIE : 1;
			unsigned char CKE : 2;
#endif
	} BIT;
	} SCR;
	unsigned char  TDR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MPBT : 1;
			unsigned char MPB : 1;
			unsigned char TEND : 1;
			unsigned char PER : 1;
			unsigned char ERS : 1;
			unsigned char ORER : 1;
			unsigned char RDRF : 1;
			unsigned char TDRE : 1;
#else
			unsigned char TDRE : 1;
			unsigned char RDRF : 1;
			unsigned char ORER : 1;
			unsigned char ERS : 1;
			unsigned char PER : 1;
			unsigned char TEND : 1;
			unsigned char MPB : 1;
			unsigned char MPBT : 1;
#endif
	} BIT;
	} SSR;
	unsigned char  RDR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SMIF : 1;
			unsigned char  : 1;
			unsigned char SINV : 1;
			unsigned char SDIR : 1;
			unsigned char CHR1 : 1;
			unsigned char  : 2;
			unsigned char BCP2 : 1;
#else
			unsigned char BCP2 : 1;
			unsigned char  : 2;
			unsigned char CHR1 : 1;
			unsigned char SDIR : 1;
			unsigned char SINV : 1;
			unsigned char  : 1;
			unsigned char SMIF : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MD : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short MD : 1;
#endif
	} BIT;
	} MDMONR;
	char           wk0[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RAME : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short RAME : 1;
#endif
	} BIT;
	} SYSCR1;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 15;
			unsigned short SSBY : 1;
#else
			unsigned short SSBY : 1;
			unsigned short  : 15;
#endif
	} BIT;
	} SBYCR;
	char           wk2[2];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 2;
			unsigned long MSTPA2 : 1;
			unsigned long MSTPA3 : 1;
			unsigned long MSTPA4 : 1;
			unsigned long MSTPA5 : 1;
			unsigned long  : 1;
			unsigned long MSTPA7 : 1;
			unsigned long  : 1;
			unsigned long MSTPA9 : 1;
			unsigned long  : 4;
			unsigned long MSTPA14 : 1;
			unsigned long MSTPA15 : 1;
			unsigned long MSTPA16 : 1;
			unsigned long MSTPA17 : 1;
			unsigned long  : 1;
			unsigned long MSTPA19 : 1;
			unsigned long  : 3;
			unsigned long MSTPA23 : 1;
			unsigned long  : 4;
			unsigned long MSTPA28 : 1;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long MSTPA28 : 1;
			unsigned long  : 4;
			unsigned long MSTPA23 : 1;
			unsigned long  : 3;
			unsigned long MSTPA19 : 1;
			unsigned long  : 1;
			unsigned long MSTPA17 : 1;
			unsigned long MSTPA16 : 1;
			unsigned long MSTPA15 : 1;
			unsigned long MSTPA14 : 1;
			unsigned long  : 4;
			unsigned long MSTPA9 : 1;
			unsigned long  : 1;
			unsigned long MSTPA7 : 1;
			unsigned long  : 1;
			unsigned long MSTPA5 : 1;
			unsigned long MSTPA4 : 1;
			unsigned long MSTPA3 : 1;
			unsigned long MSTPA2 : 1;
			unsigned long  : 2;
#endif
	} BIT;
	} MSTPCRA;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MSTPB0 : 1;
			unsigned long  : 5;
			unsigned long MSTPB6 : 1;
			unsigned long  : 3;
			unsigned long MSTPB10 : 1;
			unsigned long  : 6;
			unsigned long MSTPB17 : 1;
			unsigned long  : 3;
			unsigned long MSTPB21 : 1;
			unsigned long  : 1;
			unsigned long MSTPB23 : 1;
			unsigned long  : 1;
			unsigned long MSTPB25 : 1;
			unsigned long MSTPB26 : 1;
			unsigned long  : 3;
			unsigned long MSTPB30 : 1;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long MSTPB30 : 1;
			unsigned long  : 3;
			unsigned long MSTPB26 : 1;
			unsigned long MSTPB25 : 1;
			unsigned long  : 1;
			unsigned long MSTPB23 : 1;
			unsigned long  : 1;
			unsigned long MSTPB21 : 1;
			unsigned long  : 3;
			unsigned long MSTPB17 : 1;
			unsigned long  : 6;
			unsigned long MSTPB10 : 1;
			unsigned long  : 3;
			unsigned long MSTPB6 : 1;
			unsigned long  : 5;
			unsigned long MSTPB0 : 1;
#endif
	} BIT;
	} MSTPCRB;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MSTPC0 : 1;
			unsigned long  : 18;
			unsigned long MSTPC19 : 1;
			unsigned long  : 4;
			unsigned long MSTPC24 : 1;
			unsigned long  : 1;
			unsigned long MSTPC26 : 1;
			unsigned long MSTPC27 : 1;
			unsigned long  : 3;
			unsigned long DSLPE : 1;
#else
			unsigned long DSLPE : 1;
			unsigned long  : 3;
			unsigned long MSTPC27 : 1;
			unsigned long MSTPC26 : 1;
			unsigned long  : 1;
			unsigned long MSTPC24 : 1;
			unsigned long  : 4;
			unsigned long MSTPC19 : 1;
			unsigned long  : 18;
			unsigned long MSTPC0 : 1;
#endif
	} BIT;
	} MSTPCRC;
	char           wk3[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long PCKD : 4;
			unsigned long  : 4;
			unsigned long PCKB : 4;
			unsigned long PCKA : 4;
			unsigned long  : 8;
			unsigned long ICK : 4;
			unsigned long FCK : 4;
#else
			unsigned long FCK : 4;
			unsigned long ICK : 4;
			unsigned long  : 8;
			unsigned long PCKA : 4;
			unsigned long PCKB : 4;
			unsigned long  : 4;
			unsigned long PCKD : 4;
#endif
	} BIT;
	} SCKCR;
	char           wk4[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short CKSEL : 3;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short CKSEL : 3;
			unsigned short  : 8;
#endif
	} BIT;
	} SCKCR3;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PLIDIV : 2;
			unsigned short PLLSRCSEL : 1;
			unsigned short  : 5;
			unsigned short STC : 6;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short STC : 6;
			unsigned short  : 5;
			unsigned short PLLSRCSEL : 1;
			unsigned short PLIDIV : 2;
#endif
	} BIT;
	} PLLCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PLLEN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char PLLEN : 1;
#endif
	} BIT;
	} PLLCR2;
	char           wk5[6];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MEMWAIT : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char MEMWAIT : 2;
#endif
	} BIT;
	} MEMWAIT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MOSTP : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char MOSTP : 1;
#endif
	} BIT;
	} MOSCCR;
	char           wk6[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LCSTP : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char LCSTP : 1;
#endif
	} BIT;
	} LOCOCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ILCSTP : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char ILCSTP : 1;
#endif
	} BIT;
	} ILOCOCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HCSTP : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char HCSTP : 1;
#endif
	} BIT;
	} HOCOCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HCFRQ : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char HCFRQ : 2;
#endif
	} BIT;
	} HOCOCR2;
	char           wk7[4];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MOOVF : 1;
			unsigned char  : 1;
			unsigned char PLOVF : 1;
			unsigned char HCOVF : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char HCOVF : 1;
			unsigned char PLOVF : 1;
			unsigned char  : 1;
			unsigned char MOOVF : 1;
#endif
	} BIT;
	} OSCOVFSR;
	char           wk8[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OSTDIE : 1;
			unsigned char  : 6;
			unsigned char OSTDE : 1;
#else
			unsigned char OSTDE : 1;
			unsigned char  : 6;
			unsigned char OSTDIE : 1;
#endif
	} BIT;
	} OSTDCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OSTDF : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char OSTDF : 1;
#endif
	} BIT;
	} OSTDSR;
	char           wk9[94];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OPCM : 3;
			unsigned char  : 1;
			unsigned char OPCMTSF : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char OPCMTSF : 1;
			unsigned char  : 1;
			unsigned char OPCM : 3;
#endif
	} BIT;
	} OPCCR;
	char           wk10[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MSTS : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char MSTS : 5;
#endif
	} BIT;
	} MOSCWTCR;
	char           wk11[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HSTS : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char HSTS : 3;
#endif
	} BIT;
	} HOCOWTCR;
	char           wk12[26];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IWDTRF : 1;
			unsigned char  : 1;
			unsigned char SWRF : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SWRF : 1;
			unsigned char  : 1;
			unsigned char IWDTRF : 1;
#endif
	} BIT;
	} RSTSR2;
	char           wk13[1];
	unsigned short SWRR;
	char           wk14[28];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD1IDTSEL : 2;
			unsigned char LVD1IRQSEL : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char LVD1IRQSEL : 1;
			unsigned char LVD1IDTSEL : 2;
#endif
	} BIT;
	} LVD1CR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD1DET : 1;
			unsigned char LVD1MON : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char LVD1MON : 1;
			unsigned char LVD1DET : 1;
#endif
	} BIT;
	} LVD1SR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD2IDTSEL : 2;
			unsigned char LVD2IRQSEL : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char LVD2IRQSEL : 1;
			unsigned char LVD2IDTSEL : 2;
#endif
	} BIT;
	} LVD2CR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD2DET : 1;
			unsigned char LVD2MON : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char LVD2MON : 1;
			unsigned char LVD2DET : 1;
#endif
	} BIT;
	} LVD2SR;
	char           wk15[794];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PRC0 : 1;
			unsigned short PRC1 : 1;
			unsigned short PRC2 : 1;
			unsigned short PRC3 : 1;
			unsigned short  : 4;
			unsigned short PRKEY : 8;
#else
			unsigned short PRKEY : 8;
			unsigned short  : 4;
			unsigned short PRC3 : 1;
			unsigned short PRC2 : 1;
			unsigned short PRC1 : 1;
			unsigned short PRC0 : 1;
#endif
	} BIT;
	} PRCR;
	char           wk16[48784];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PORF : 1;
			unsigned char LVD0RF : 1;
			unsigned char LVD1RF : 1;
			unsigned char LVD2RF : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char LVD2RF : 1;
			unsigned char LVD1RF : 1;
			unsigned char LVD0RF : 1;
			unsigned char PORF : 1;
#endif
	} BIT;
	} RSTSR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CWSF : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char CWSF : 1;
#endif
	} BIT;
	} RSTSR1;
	char           wk17[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 5;
			unsigned char MODRV21 : 1;
			unsigned char MOSEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char MOSEL : 1;
			unsigned char MODRV21 : 1;
			unsigned char  : 5;
#endif
	} BIT;
	} MOFCR;
	char           wk18[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 5;
			unsigned char LVD1E : 1;
			unsigned char LVD2E : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char LVD2E : 1;
			unsigned char LVD1E : 1;
			unsigned char  : 5;
#endif
	} BIT;
	} LVCMPCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD1LVL : 4;
			unsigned char LVD2LVL : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char LVD2LVL : 2;
			unsigned char LVD1LVL : 4;
#endif
	} BIT;
	} LVDLVLR;
	char           wk19[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD1RIE : 1;
			unsigned char  : 1;
			unsigned char LVD1CMPE : 1;
			unsigned char  : 3;
			unsigned char LVD1RI : 1;
			unsigned char LVD1RN : 1;
#else
			unsigned char LVD1RN : 1;
			unsigned char LVD1RI : 1;
			unsigned char  : 3;
			unsigned char LVD1CMPE : 1;
			unsigned char  : 1;
			unsigned char LVD1RIE : 1;
#endif
	} BIT;
	} LVD1CR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD2RIE : 1;
			unsigned char  : 1;
			unsigned char LVD2CMPE : 1;
			unsigned char  : 3;
			unsigned char LVD2RI : 1;
			unsigned char LVD2RN : 1;
#else
			unsigned char LVD2RN : 1;
			unsigned char LVD2RI : 1;
			unsigned char  : 3;
			unsigned char LVD2CMPE : 1;
			unsigned char  : 1;
			unsigned char LVD2RIE : 1;
#endif
	} BIT;
	} LVD2CR0;
} st_system_t;

typedef struct st_tmr0 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
			unsigned char CCLR : 2;
			unsigned char OVIE : 1;
			unsigned char CMIEA : 1;
			unsigned char CMIEB : 1;
#else
			unsigned char CMIEB : 1;
			unsigned char CMIEA : 1;
			unsigned char OVIE : 1;
			unsigned char CCLR : 2;
			unsigned char  : 3;
#endif
	} BIT;
	} TCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OSA : 2;
			unsigned char OSB : 2;
			unsigned char ADTE : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char ADTE : 1;
			unsigned char OSB : 2;
			unsigned char OSA : 2;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CKS : 3;
			unsigned char CSS : 2;
			unsigned char  : 2;
			unsigned char TMRIS : 1;
#else
			unsigned char TMRIS : 1;
			unsigned char  : 2;
			unsigned char CSS : 2;
			unsigned char CKS : 3;
#endif
	} BIT;
	} TCCR;
} st_tmr0_t;

typedef struct st_tmr1 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
			unsigned char CCLR : 2;
			unsigned char OVIE : 1;
			unsigned char CMIEA : 1;
			unsigned char CMIEB : 1;
#else
			unsigned char CMIEB : 1;
			unsigned char CMIEA : 1;
			unsigned char OVIE : 1;
			unsigned char CCLR : 2;
			unsigned char  : 3;
#endif
	} BIT;
	} TCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char OSA : 2;
			unsigned char OSB : 2;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char OSB : 2;
			unsigned char OSA : 2;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CKS : 3;
			unsigned char CSS : 2;
			unsigned char  : 2;
			unsigned char TMRIS : 1;
#else
			unsigned char TMRIS : 1;
			unsigned char  : 2;
			unsigned char CSS : 2;
			unsigned char CKS : 3;
#endif
	} BIT;
	} TCCR;
} st_tmr1_t;

typedef struct st_tmr01 {
	unsigned short TCORA;
	unsigned short TCORB;
	unsigned short TCNT;
	unsigned short TCCR;
} st_tmr01_t;


#pragma pack()

#endif

