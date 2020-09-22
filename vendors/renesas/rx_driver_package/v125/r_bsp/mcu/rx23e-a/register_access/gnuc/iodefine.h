                                                                          
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                          
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
/*******************************************************************************
*
* Device     : RX/RX200/RX23E-A
*
* File Name  : iodefine.h
*
* Abstract   : Definition of I/O Register.
*
* History    : V0.50  (2018-12-20)  [Hardware Manual Revision : 0.50]
*              V1.00  (2019-07-05)  [Hardware Manual Revision : 1.00]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
*  Copyright(c) 2019 Renesas Electronics Corporation.
*
*********************************************************************************/
/********************************************************************************/
/*                                                                              */
/*  DESCRIPTION : Definition of ICU Register                                    */
/*  CPU TYPE    : RX23E-A                                                       */
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
/*       MSTP(MTU4) = 0;    // MTU,MTU0,MTU1,MTU2,MTU3,MTU4,MTU5  expands to :  */
/*         SYSTEM.MSTPCRA.BIT.MSTPA9  = 0;                                      */
/*                                                                              */
/*                                                                              */
/********************************************************************************/
#ifndef __RX23EAIODEFINE_HEADER__
#define __RX23EAIODEFINE_HEADER__

#define	IEN_BSC_BUSERR		IEN0
#define	IEN_FCU_FRDYI		IEN7
#define	IEN_ICU_SWINT		IEN3
#define	IEN_CMT0_CMI0		IEN4
#define	IEN_CMT1_CMI1		IEN5
#define	IEN_CAC_FERRF		IEN0
#define	IEN_CAC_MENDF		IEN1
#define	IEN_CAC_OVFF		IEN2
#define	IEN_RSPI0_SPEI0		IEN4
#define	IEN_RSPI0_SPRI0		IEN5
#define	IEN_RSPI0_SPTI0		IEN6
#define	IEN_RSPI0_SPII0		IEN7
#define	IEN_RSCAN_COMFRXINT	IEN4
#define	IEN_RSCAN_RXFINT	IEN5
#define	IEN_RSCAN_TXINT		IEN6
#define	IEN_RSCAN_CHERRINT	IEN7
#define	IEN_RSCAN_GLERRINT	IEN0
#define	IEN_DOC_DOPCF		IEN1
#define	IEN_ICU_IRQ0		IEN0
#define	IEN_ICU_IRQ1		IEN1
#define	IEN_ICU_IRQ2		IEN2
#define	IEN_ICU_IRQ3		IEN3
#define	IEN_ICU_IRQ4		IEN4
#define	IEN_ICU_IRQ5		IEN5
#define	IEN_ICU_IRQ6		IEN6
#define	IEN_ICU_IRQ7		IEN7
#define	IEN_ELC_ELSR8I		IEN0
#define	IEN_LVD_LVD1		IEN0
#define	IEN_LVD_LVD2		IEN1
#define	IEN_S12AD_S12ADI0	IEN6
#define	IEN_S12AD_GBADI		IEN7
#define	IEN_ELC_ELSR18I		IEN2
#define	IEN_ELC_ELSR19I		IEN3
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
#define	IEN_POE_OEI1		IEN2
#define	IEN_POE_OEI2		IEN3
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
#define	IEN_DMAC_DMAC0I		IEN6
#define	IEN_DMAC_DMAC1I		IEN7
#define	IEN_DMAC_DMAC2I		IEN0
#define	IEN_DMAC_DMAC3I		IEN1
#define	IEN_DSAD0_ADI0		IEN6
#define	IEN_DSAD0_SCANEND0	IEN7
#define	IEN_DSAD1_ADI1		IEN1
#define	IEN_DSAD1_SCANEND1	IEN2
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
#define	IEN_SCI12_ERI12		IEN6
#define	IEN_SCI12_RXI12		IEN7
#define	IEN_SCI12_TXI12		IEN0
#define	IEN_SCI12_TEI12		IEN1
#define	IEN_SCI12_SCIX0		IEN2
#define	IEN_SCI12_SCIX1		IEN3
#define	IEN_SCI12_SCIX2		IEN4
#define	IEN_SCI12_SCIX3		IEN5
#define	IEN_RIIC0_EEI0		IEN6
#define	IEN_RIIC0_RXI0		IEN7
#define	IEN_RIIC0_TXI0		IEN0
#define	IEN_RIIC0_TEI0		IEN1

#define	VECT_BSC_BUSERR		16
#define	VECT_FCU_FRDYI		23
#define	VECT_ICU_SWINT		27
#define	VECT_CMT0_CMI0		28
#define	VECT_CMT1_CMI1		29
#define	VECT_CAC_FERRF		32
#define	VECT_CAC_MENDF		33
#define	VECT_CAC_OVFF		34
#define	VECT_RSPI0_SPEI0	44
#define	VECT_RSPI0_SPRI0	45
#define	VECT_RSPI0_SPTI0	46
#define	VECT_RSPI0_SPII0	47
#define	VECT_RSCAN_COMFRXINT	52
#define	VECT_RSCAN_RXFINT	53
#define	VECT_RSCAN_TXINT	54
#define	VECT_RSCAN_CHERRINT	55
#define	VECT_RSCAN_GLERRINT	56
#define	VECT_DOC_DOPCF		57
#define	VECT_ICU_IRQ0		64
#define	VECT_ICU_IRQ1		65
#define	VECT_ICU_IRQ2		66
#define	VECT_ICU_IRQ3		67
#define	VECT_ICU_IRQ4		68
#define	VECT_ICU_IRQ5		69
#define	VECT_ICU_IRQ6		70
#define	VECT_ICU_IRQ7		71
#define	VECT_ELC_ELSR8I		80
#define	VECT_LVD_LVD1		88
#define	VECT_LVD_LVD2		89
#define	VECT_S12AD_S12ADI0	102
#define	VECT_S12AD_GBADI	103
#define	VECT_ELC_ELSR18I	106
#define	VECT_ELC_ELSR19I	107
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
#define	VECT_POE_OEI1		170
#define	VECT_POE_OEI2		171
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
#define	VECT_DMAC_DMAC0I	198
#define	VECT_DMAC_DMAC1I	199
#define	VECT_DMAC_DMAC2I	200
#define	VECT_DMAC_DMAC3I	201
#define	VECT_DSAD0_ADI0		206
#define	VECT_DSAD0_SCANEND0	207
#define	VECT_DSAD1_ADI1		209
#define	VECT_DSAD1_SCANEND1	210
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
#define	VECT_SCI12_ERI12	238
#define	VECT_SCI12_RXI12	239
#define	VECT_SCI12_TXI12	240
#define	VECT_SCI12_TEI12	241
#define	VECT_SCI12_SCIX0	242
#define	VECT_SCI12_SCIX1	243
#define	VECT_SCI12_SCIX2	244
#define	VECT_SCI12_SCIX3	245
#define	VECT_RIIC0_EEI0		246
#define	VECT_RIIC0_RXI0		247
#define	VECT_RIIC0_TXI0		248
#define	VECT_RIIC0_TEI0		249

#define	MSTP_DMAC	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC0	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC1	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC2	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC3	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DTC	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DSAD1	SYSTEM.MSTPCRA.BIT.MSTPA26
#define	MSTP_DSAD0	SYSTEM.MSTPCRA.BIT.MSTPA25
#define	MSTP_S12AD	SYSTEM.MSTPCRA.BIT.MSTPA17
#define	MSTP_AFE	SYSTEM.MSTPCRA.BIT.MSTPA16
#define	MSTP_CMT	SYSTEM.MSTPCRA.BIT.MSTPA15
#define	MSTP_CMT0	SYSTEM.MSTPCRA.BIT.MSTPA15
#define	MSTP_CMT1	SYSTEM.MSTPCRA.BIT.MSTPA15
#define	MSTP_MTU	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU0	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU1	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU2	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU3	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU4	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_MTU5	SYSTEM.MSTPCRA.BIT.MSTPA9
#define	MSTP_TMR0	SYSTEM.MSTPCRA.BIT.MSTPA5
#define	MSTP_TMR1	SYSTEM.MSTPCRA.BIT.MSTPA5
#define	MSTP_TMR01	SYSTEM.MSTPCRA.BIT.MSTPA5
#define	MSTP_TMR2	SYSTEM.MSTPCRA.BIT.MSTPA4
#define	MSTP_TMR3	SYSTEM.MSTPCRA.BIT.MSTPA4
#define	MSTP_TMR23	SYSTEM.MSTPCRA.BIT.MSTPA4
#define	MSTP_SCI1	SYSTEM.MSTPCRB.BIT.MSTPB30
#define	MSTP_SMCI1	SYSTEM.MSTPCRB.BIT.MSTPB30
#define	MSTP_SCI5	SYSTEM.MSTPCRB.BIT.MSTPB26
#define	MSTP_SMCI5	SYSTEM.MSTPCRB.BIT.MSTPB26
#define	MSTP_SCI6	SYSTEM.MSTPCRB.BIT.MSTPB25
#define	MSTP_SMCI6	SYSTEM.MSTPCRB.BIT.MSTPB25
#define	MSTP_CRC	SYSTEM.MSTPCRB.BIT.MSTPB23
#define	MSTP_RIIC0	SYSTEM.MSTPCRB.BIT.MSTPB21
#define	MSTP_RSPI0	SYSTEM.MSTPCRB.BIT.MSTPB17
#define	MSTP_ELC	SYSTEM.MSTPCRB.BIT.MSTPB9
#define	MSTP_DOC	SYSTEM.MSTPCRB.BIT.MSTPB6
#define	MSTP_SCI12	SYSTEM.MSTPCRB.BIT.MSTPB4
#define	MSTP_SMCI12	SYSTEM.MSTPCRB.BIT.MSTPB4
#define	MSTP_RSCAN	SYSTEM.MSTPCRB.BIT.MSTPB0
#define	MSTP_RSCAN0	SYSTEM.MSTPCRB.BIT.MSTPB0
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

#define	AFE		(*(volatile struct st_afe         *)0xA1400)
#define	BSC		(*(volatile struct st_bsc         *)0x81300)
#define	CAC		(*(volatile struct st_cac         *)0x8B000)
#define	CMT		(*(volatile struct st_cmt         *)0x88000)
#define	CMT0	(*(volatile struct st_cmt0        *)0x88002)
#define	CMT1	(*(volatile struct st_cmt0        *)0x88008)
#define	CRC		(*(volatile struct st_crc         *)0x88280)
#define	DMAC	(*(volatile struct st_dmac        *)0x82200)
#define	DMAC0	(*(volatile struct st_dmac0       *)0x82000)
#define	DMAC1	(*(volatile struct st_dmac1       *)0x82040)
#define	DMAC2	(*(volatile struct st_dmac1       *)0x82080)
#define	DMAC3	(*(volatile struct st_dmac1       *)0x820C0)
#define	DOC		(*(volatile struct st_doc         *)0x8B080)
#define	DSAD0	(*(volatile struct st_dsad        *)0xA1000)
#define	DSAD1	(*(volatile struct st_dsad        *)0xA1200)
#define	DTC		(*(volatile struct st_dtc         *)0x82400)
#define	ELC		(*(volatile struct st_elc         *)0x8B100)
#define	FLASH	(*(volatile struct st_flash       *)0x7FC090)
#define	FLASHCONST		(*(volatile struct st_flashconst  *)0x7FC350)
#define	ICU		(*(volatile struct st_icu         *)0x87000)
#define	IWDT	(*(volatile struct st_iwdt        *)0x88030)
#define	LPT		(*(volatile struct st_lpt         *)0x800B0)
#define	MPC		(*(volatile struct st_mpc         *)0x8C11F)
#define	MPU		(*(volatile struct st_mpu         *)0x86400)
#define	MTU		(*(volatile struct st_mtu         *)0xD0A0A)
#define	MTU0	(*(volatile struct st_mtu0        *)0xD0A90)
#define	MTU1	(*(volatile struct st_mtu1        *)0xD0A90)
#define	MTU2	(*(volatile struct st_mtu2        *)0xD0A92)
#define	MTU3	(*(volatile struct st_mtu3        *)0xD0A00)
#define	MTU4	(*(volatile struct st_mtu4        *)0xD0A00)
#define	MTU5	(*(volatile struct st_mtu5        *)0xD0A94)
#define	OFSM	(*(volatile struct st_ofsm        *)0xFFFFFF80)
#define	POE		(*(volatile struct st_poe         *)0x88900)
#define	PORT1	(*(volatile struct st_port1       *)0x8C001)
#define	PORT2	(*(volatile struct st_port2       *)0x8C002)
#define	PORT3	(*(volatile struct st_port3       *)0x8C003)
#define	PORTB	(*(volatile struct st_portb       *)0x8C00B)
#define	PORTC	(*(volatile struct st_portc       *)0x8C00C)
#define	PORTH	(*(volatile struct st_porth       *)0x8C011)
#define	RIIC0	(*(volatile struct st_riic        *)0x88300)
#define	RSCAN	(*(volatile struct st_rscan       *)0xA8322)
#define	RSCAN0	(*(volatile struct st_rscan0      *)0xA8300)
#define	RSPI0	(*(volatile struct st_rspi        *)0x88380)
#define	S12AD	(*(volatile struct st_s12ad       *)0x89000)
#define	SCI1	(*(volatile struct st_sci1        *)0x8A020)
#define	SCI5	(*(volatile struct st_sci1        *)0x8A0A0)
#define	SCI6	(*(volatile struct st_sci1        *)0x8A0C0)
#define	SCI12	(*(volatile struct st_sci12       *)0x8B300)
#define	SMCI1	(*(volatile struct st_smci        *)0x8A020)
#define	SMCI5	(*(volatile struct st_smci        *)0x8A0A0)
#define	SMCI6	(*(volatile struct st_smci        *)0x8A0C0)
#define	SMCI12	(*(volatile struct st_smci        *)0x8B300)
#define	SYSTEM	(*(volatile struct st_system      *)0x80000)
#define	TMR0	(*(volatile struct st_tmr0        *)0x88200)
#define	TMR1	(*(volatile struct st_tmr1        *)0x88201)
#define	TMR2	(*(volatile struct st_tmr0        *)0x88210)
#define	TMR3	(*(volatile struct st_tmr1        *)0x88211)
#define	TMR01	(*(volatile struct st_tmr01       *)0x88204)
#define	TMR23	(*(volatile struct st_tmr01       *)0x88214)

typedef enum enum_ir {
IR_BSC_BUSERR=16,IR_FCU_FRDYI=23,
IR_ICU_SWINT=27,
IR_CMT0_CMI0,
IR_CMT1_CMI1,
IR_CAC_FERRF=32,IR_CAC_MENDF,IR_CAC_OVFF,
IR_RSPI0_SPEI0=44,IR_RSPI0_SPRI0,IR_RSPI0_SPTI0,IR_RSPI0_SPII0,
IR_RSCAN_COMFRXINT=52,IR_RSCAN_RXFINT,IR_RSCAN_TXINT,IR_RSCAN_CHERRINT,IR_RSCAN_GLERRINT,
IR_DOC_DOPCF,
IR_ICU_IRQ0=64,IR_ICU_IRQ1,IR_ICU_IRQ2,IR_ICU_IRQ3,IR_ICU_IRQ4,IR_ICU_IRQ5,IR_ICU_IRQ6,IR_ICU_IRQ7,
IR_ELC_ELSR8I=80,
IR_LVD_LVD1=88,IR_LVD_LVD2,
IR_S12AD_S12ADI0=102,IR_S12AD_GBADI,
IR_ELC_ELSR18I=106,IR_ELC_ELSR19I,
IR_MTU0_TGIA0=114,IR_MTU0_TGIB0,IR_MTU0_TGIC0,IR_MTU0_TGID0,IR_MTU0_TCIV0,IR_MTU0_TGIE0,IR_MTU0_TGIF0,
IR_MTU1_TGIA1,IR_MTU1_TGIB1,IR_MTU1_TCIV1,IR_MTU1_TCIU1,
IR_MTU2_TGIA2,IR_MTU2_TGIB2,IR_MTU2_TCIV2,IR_MTU2_TCIU2,
IR_MTU3_TGIA3,IR_MTU3_TGIB3,IR_MTU3_TGIC3,IR_MTU3_TGID3,IR_MTU3_TCIV3,
IR_MTU4_TGIA4,IR_MTU4_TGIB4,IR_MTU4_TGIC4,IR_MTU4_TGID4,IR_MTU4_TCIV4,
IR_MTU5_TGIU5,IR_MTU5_TGIV5,IR_MTU5_TGIW5,
IR_POE_OEI1=170,IR_POE_OEI2,
IR_TMR0_CMIA0=174,IR_TMR0_CMIB0,IR_TMR0_OVI0,
IR_TMR1_CMIA1,IR_TMR1_CMIB1,IR_TMR1_OVI1,
IR_TMR2_CMIA2,IR_TMR2_CMIB2,IR_TMR2_OVI2,
IR_TMR3_CMIA3,IR_TMR3_CMIB3,IR_TMR3_OVI3,
IR_DMAC_DMAC0I=198,IR_DMAC_DMAC1I,IR_DMAC_DMAC2I,IR_DMAC_DMAC3I,
IR_DSAD0_ADI0=206,IR_DSAD0_SCANEND0,
IR_DSAD1_ADI1=209,IR_DSAD1_SCANEND1,
IR_SCI1_ERI1=218,IR_SCI1_RXI1,IR_SCI1_TXI1,IR_SCI1_TEI1,
IR_SCI5_ERI5,IR_SCI5_RXI5,IR_SCI5_TXI5,IR_SCI5_TEI5,
IR_SCI6_ERI6,IR_SCI6_RXI6,IR_SCI6_TXI6,IR_SCI6_TEI6,
IR_SCI12_ERI12=238,IR_SCI12_RXI12,IR_SCI12_TXI12,IR_SCI12_TEI12,IR_SCI12_SCIX0,IR_SCI12_SCIX1,IR_SCI12_SCIX2,IR_SCI12_SCIX3,
IR_RIIC0_EEI0,IR_RIIC0_RXI0,IR_RIIC0_TXI0,IR_RIIC0_TEI0
} enum_ir_t;

typedef enum enum_dtce {
DTCE_ICU_SWINT=27,
DTCE_CMT0_CMI0,
DTCE_CMT1_CMI1,
DTCE_RSPI0_SPRI0=45,DTCE_RSPI0_SPTI0,
DTCE_RSCAN_COMFRXINT=52,
DTCE_ICU_IRQ0=64,DTCE_ICU_IRQ1,DTCE_ICU_IRQ2,DTCE_ICU_IRQ3,DTCE_ICU_IRQ4,DTCE_ICU_IRQ5,DTCE_ICU_IRQ6,DTCE_ICU_IRQ7,
DTCE_S12AD_S12ADI0=102,DTCE_S12AD_GBADI,
DTCE_ELC_ELSR18I=106,DTCE_ELC_ELSR19I,
DTCE_MTU0_TGIA0=114,DTCE_MTU0_TGIB0,DTCE_MTU0_TGIC0,DTCE_MTU0_TGID0,
DTCE_MTU1_TGIA1=121,DTCE_MTU1_TGIB1,
DTCE_MTU2_TGIA2=125,DTCE_MTU2_TGIB2,
DTCE_MTU3_TGIA3=129,DTCE_MTU3_TGIB3,DTCE_MTU3_TGIC3,DTCE_MTU3_TGID3,
DTCE_MTU4_TGIA4=134,DTCE_MTU4_TGIB4,DTCE_MTU4_TGIC4,DTCE_MTU4_TGID4,DTCE_MTU4_TCIV4,
DTCE_MTU5_TGIU5,DTCE_MTU5_TGIV5,DTCE_MTU5_TGIW5,
DTCE_TMR0_CMIA0=174,DTCE_TMR0_CMIB0,
DTCE_TMR1_CMIA1=177,DTCE_TMR1_CMIB1,
DTCE_TMR2_CMIA2=180,DTCE_TMR2_CMIB2,
DTCE_TMR3_CMIA3=183,DTCE_TMR3_CMIB3,
DTCE_DMAC_DMAC0I=198,DTCE_DMAC_DMAC1I,DTCE_DMAC_DMAC2I,DTCE_DMAC_DMAC3I,
DTCE_DSAD0_ADI0=206,DTCE_DSAD0_SCANEND0,
DTCE_DSAD1_ADI1=209,DTCE_DSAD1_SCANEND1,
DTCE_SCI1_RXI1=219,DTCE_SCI1_TXI1,
DTCE_SCI5_RXI5=223,DTCE_SCI5_TXI5,
DTCE_SCI6_RXI6=227,DTCE_SCI6_TXI6,
DTCE_SCI12_RXI12=239,DTCE_SCI12_TXI12,
DTCE_RIIC0_RXI0=247,DTCE_RIIC0_TXI0
} enum_dtce_t;

typedef enum enum_ier {
IER_BSC_BUSERR=0x02,
IER_FCU_FRDYI=0x02,
IER_ICU_SWINT=0x03,
IER_CMT0_CMI0=0x03,
IER_CMT1_CMI1=0x03,
IER_CAC_FERRF=0x04,IER_CAC_MENDF=0x04,IER_CAC_OVFF=0x04,
IER_RSPI0_SPEI0=0x05,IER_RSPI0_SPRI0=0x05,IER_RSPI0_SPTI0=0x05,IER_RSPI0_SPII0=0x05,
IER_RSCAN_COMFRXINT=0x06,IER_RSCAN_RXFINT=0x06,IER_RSCAN_TXINT=0x06,IER_RSCAN_CHERRINT=0x06,IER_RSCAN_GLERRINT=0x07,
IER_DOC_DOPCF=0x07,
IER_ICU_IRQ0=0x08,IER_ICU_IRQ1=0x08,IER_ICU_IRQ2=0x08,IER_ICU_IRQ3=0x08,IER_ICU_IRQ4=0x08,IER_ICU_IRQ5=0x08,IER_ICU_IRQ6=0x08,IER_ICU_IRQ7=0x08,
IER_ELC_ELSR8I=0x0A,
IER_LVD_LVD1=0x0B,IER_LVD_LVD2=0x0B,
IER_S12AD_S12ADI0=0x0C,IER_S12AD_GBADI=0x0C,
IER_ELC_ELSR18I=0x0D,IER_ELC_ELSR19I=0x0D,
IER_MTU0_TGIA0=0x0E,IER_MTU0_TGIB0=0x0E,IER_MTU0_TGIC0=0x0E,IER_MTU0_TGID0=0x0E,IER_MTU0_TCIV0=0x0E,IER_MTU0_TGIE0=0x0E,IER_MTU0_TGIF0=0x0F,
IER_MTU1_TGIA1=0x0F,IER_MTU1_TGIB1=0x0F,IER_MTU1_TCIV1=0x0F,IER_MTU1_TCIU1=0x0F,
IER_MTU2_TGIA2=0x0F,IER_MTU2_TGIB2=0x0F,IER_MTU2_TCIV2=0x0F,IER_MTU2_TCIU2=0x10,
IER_MTU3_TGIA3=0x10,IER_MTU3_TGIB3=0x10,IER_MTU3_TGIC3=0x10,IER_MTU3_TGID3=0x10,IER_MTU3_TCIV3=0x10,
IER_MTU4_TGIA4=0x10,IER_MTU4_TGIB4=0x10,IER_MTU4_TGIC4=0x11,IER_MTU4_TGID4=0x11,IER_MTU4_TCIV4=0x11,
IER_MTU5_TGIU5=0x11,IER_MTU5_TGIV5=0x11,IER_MTU5_TGIW5=0x11,
IER_POE_OEI1=0x15,IER_POE_OEI2=0x15,
IER_TMR0_CMIA0=0x15,IER_TMR0_CMIB0=0x15,IER_TMR0_OVI0=0x16,
IER_TMR1_CMIA1=0x16,IER_TMR1_CMIB1=0x16,IER_TMR1_OVI1=0x16,
IER_TMR2_CMIA2=0x16,IER_TMR2_CMIB2=0x16,IER_TMR2_OVI2=0x16,
IER_TMR3_CMIA3=0x16,IER_TMR3_CMIB3=0x17,IER_TMR3_OVI3=0x17,
IER_DMAC_DMAC0I=0x18,IER_DMAC_DMAC1I=0x18,IER_DMAC_DMAC2I=0x19,IER_DMAC_DMAC3I=0x19,
IER_DSAD0_ADI0=0x19,IER_DSAD0_SCANEND0=0x19,
IER_DSAD1_ADI1=0x1A,IER_DSAD1_SCANEND1=0x1A,
IER_SCI1_ERI1=0x1B,IER_SCI1_RXI1=0x1B,IER_SCI1_TXI1=0x1B,IER_SCI1_TEI1=0x1B,
IER_SCI5_ERI5=0x1B,IER_SCI5_RXI5=0x1B,IER_SCI5_TXI5=0x1C,IER_SCI5_TEI5=0x1C,
IER_SCI6_ERI6=0x1C,IER_SCI6_RXI6=0x1C,IER_SCI6_TXI6=0x1C,IER_SCI6_TEI6=0x1C,
IER_SCI12_ERI12=0x1D,IER_SCI12_RXI12=0x1D,IER_SCI12_TXI12=0x1E,IER_SCI12_TEI12=0x1E,IER_SCI12_SCIX0=0x1E,IER_SCI12_SCIX1=0x1E,IER_SCI12_SCIX2=0x1E,IER_SCI12_SCIX3=0x1E,
IER_RIIC0_EEI0=0x1E,IER_RIIC0_RXI0=0x1E,IER_RIIC0_TXI0=0x1F,IER_RIIC0_TEI0=0x1F
} enum_ier_t;

typedef enum enum_ipr {
IPR_BSC_BUSERR=0,
IPR_FCU_FRDYI=2,
IPR_ICU_SWINT=3,
IPR_CMT0_CMI0=4,
IPR_CMT1_CMI1=5,
IPR_CAC_FERRF=32,IPR_CAC_MENDF=33,IPR_CAC_OVFF=34,
IPR_RSPI0_SPEI0=44,IPR_RSPI0_SPRI0=44,IPR_RSPI0_SPTI0=44,IPR_RSPI0_SPII0=44,
IPR_RSCAN_COMFRXINT=52,IPR_RSCAN_RXFINT=53,IPR_RSCAN_TXINT=54,IPR_RSCAN_CHERRINT=55,IPR_RSCAN_GLERRINT=56,
IPR_DOC_DOPCF=57,
IPR_ICU_IRQ0=64,IPR_ICU_IRQ1=65,IPR_ICU_IRQ2=66,IPR_ICU_IRQ3=67,IPR_ICU_IRQ4=68,IPR_ICU_IRQ5=69,IPR_ICU_IRQ6=70,IPR_ICU_IRQ7=71,
IPR_ELC_ELSR8I=80,
IPR_LVD_LVD1=88,IPR_LVD_LVD2=89,
IPR_S12AD_S12ADI0=102,IPR_S12AD_GBADI=103,
IPR_ELC_ELSR18I=106,IPR_ELC_ELSR19I=107,
IPR_MTU0_TGIA0=114,IPR_MTU0_TGIB0=114,IPR_MTU0_TGIC0=114,IPR_MTU0_TGID0=114,IPR_MTU0_TCIV0=118,IPR_MTU0_TGIE0=118,IPR_MTU0_TGIF0=118,
IPR_MTU1_TGIA1=121,IPR_MTU1_TGIB1=121,IPR_MTU1_TCIV1=123,IPR_MTU1_TCIU1=123,
IPR_MTU2_TGIA2=125,IPR_MTU2_TGIB2=125,IPR_MTU2_TCIV2=127,IPR_MTU2_TCIU2=127,
IPR_MTU3_TGIA3=129,IPR_MTU3_TGIB3=129,IPR_MTU3_TGIC3=129,IPR_MTU3_TGID3=129,IPR_MTU3_TCIV3=133,
IPR_MTU4_TGIA4=134,IPR_MTU4_TGIB4=134,IPR_MTU4_TGIC4=134,IPR_MTU4_TGID4=134,IPR_MTU4_TCIV4=138,
IPR_MTU5_TGIU5=139,IPR_MTU5_TGIV5=139,IPR_MTU5_TGIW5=139,
IPR_POE_OEI1=170,IPR_POE_OEI2=171,
IPR_TMR0_CMIA0=174,IPR_TMR0_CMIB0=174,IPR_TMR0_OVI0=174,
IPR_TMR1_CMIA1=177,IPR_TMR1_CMIB1=177,IPR_TMR1_OVI1=177,
IPR_TMR2_CMIA2=180,IPR_TMR2_CMIB2=180,IPR_TMR2_OVI2=180,
IPR_TMR3_CMIA3=183,IPR_TMR3_CMIB3=183,IPR_TMR3_OVI3=183,
IPR_DMAC_DMAC0I=198,IPR_DMAC_DMAC1I=199,IPR_DMAC_DMAC2I=200,IPR_DMAC_DMAC3I=201,
IPR_DSAD0_ADI0=206,IPR_DSAD0_SCANEND0=207,
IPR_DSAD1_ADI1=209,IPR_DSAD1_SCANEND1=210,
IPR_SCI1_ERI1=218,IPR_SCI1_RXI1=218,IPR_SCI1_TXI1=218,IPR_SCI1_TEI1=218,
IPR_SCI5_ERI5=222,IPR_SCI5_RXI5=222,IPR_SCI5_TXI5=222,IPR_SCI5_TEI5=222,
IPR_SCI6_ERI6=226,IPR_SCI6_RXI6=226,IPR_SCI6_TXI6=226,IPR_SCI6_TEI6=226,
IPR_SCI12_ERI12=238,IPR_SCI12_RXI12=238,IPR_SCI12_TXI12=238,IPR_SCI12_TEI12=238,IPR_SCI12_SCIX0=242,IPR_SCI12_SCIX1=243,IPR_SCI12_SCIX2=244,IPR_SCI12_SCIX3=245,
IPR_RIIC0_EEI0=246,IPR_RIIC0_RXI0=247,IPR_RIIC0_TXI0=248,IPR_RIIC0_TEI0=249,
IPR_BSC_=0,
IPR_FCU_=2,
IPR_CMT0_=4,
IPR_CMT1_=5,
IPR_RSPI0_=44,
IPR_DOC_=57,
IPR_MTU1_TGI=121,
IPR_MTU1_TCI=123,
IPR_MTU2_TGI=125,
IPR_MTU2_TCI=127,
IPR_MTU3_TGI=129,
IPR_MTU4_TGI=134,
IPR_MTU5_=139,
IPR_MTU5_TGI=139,
IPR_TMR0_=174,
IPR_TMR1_=177,
IPR_TMR2_=180,
IPR_TMR3_=183,
IPR_SCI1_=218,
IPR_SCI5_=222,
IPR_SCI6_=226
} enum_ipr_t;


#pragma pack(4)


typedef struct st_afe {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TEMPSEN : 1;
			unsigned short VREFEN : 1;
			unsigned short VBIASEN : 1;
			unsigned short IEXCEN : 1;
			unsigned short DSAD0EN : 1;
			unsigned short DSAD1EN : 1;
			unsigned short  : 9;
			unsigned short DSADLVM : 1;
#else
			unsigned short DSADLVM : 1;
			unsigned short  : 9;
			unsigned short DSAD1EN : 1;
			unsigned short DSAD0EN : 1;
			unsigned short IEXCEN : 1;
			unsigned short VBIASEN : 1;
			unsigned short VREFEN : 1;
			unsigned short TEMPSEN : 1;
#endif
	} BIT;
	} OPCR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DET0LVL : 1;
			unsigned short  : 3;
			unsigned short DET1LVL : 2;
			unsigned short  : 2;
			unsigned short VR0PDISA : 1;
			unsigned short VR0PDISC : 1;
			unsigned short VR0NDISA : 1;
			unsigned short VR0NDISC : 1;
			unsigned short VR1PDISA : 1;
			unsigned short VR1PDISC : 1;
			unsigned short VR1NDISA : 1;
			unsigned short VR1NDISC : 1;
#else
			unsigned short VR1NDISC : 1;
			unsigned short VR1NDISA : 1;
			unsigned short VR1PDISC : 1;
			unsigned short VR1PDISA : 1;
			unsigned short VR0NDISC : 1;
			unsigned short VR0NDISA : 1;
			unsigned short VR0PDISC : 1;
			unsigned short VR0PDISA : 1;
			unsigned short  : 2;
			unsigned short DET1LVL : 2;
			unsigned short  : 3;
			unsigned short DET0LVL : 1;
#endif
	} BIT;
	} VDETCR;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short LVDET0 : 1;
			unsigned short LVDET1 : 1;
			unsigned short  : 2;
			unsigned short IEXC0DET : 1;
			unsigned short IEXC1DET : 1;
			unsigned short IEXC2DET : 1;
			unsigned short IEXC3DET : 1;
			unsigned short DS0PDET : 1;
			unsigned short DS0NDET : 1;
			unsigned short DS0RDET : 1;
			unsigned short  : 1;
			unsigned short DS1PDET : 1;
			unsigned short DS1NDET : 1;
			unsigned short DS1RDET : 1;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short DS1RDET : 1;
			unsigned short DS1NDET : 1;
			unsigned short DS1PDET : 1;
			unsigned short  : 1;
			unsigned short DS0RDET : 1;
			unsigned short DS0NDET : 1;
			unsigned short DS0PDET : 1;
			unsigned short IEXC3DET : 1;
			unsigned short IEXC2DET : 1;
			unsigned short IEXC1DET : 1;
			unsigned short IEXC0DET : 1;
			unsigned short  : 2;
			unsigned short LVDET1 : 1;
			unsigned short LVDET0 : 1;
#endif
	} BIT;
	} VDETER;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short VBIASEN0 : 1;
			unsigned short VBIASEN1 : 1;
			unsigned short VBIASEN2 : 1;
			unsigned short VBIASEN3 : 1;
			unsigned short VBIASEN4 : 1;
			unsigned short VBIASEN5 : 1;
			unsigned short VBIASEN6 : 1;
			unsigned short VBIASEN7 : 1;
			unsigned short VBIASEN8 : 1;
			unsigned short VBIASEN9 : 1;
			unsigned short VBIASEN10 : 1;
			unsigned short VBIASEN11 : 1;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short VBIASEN11 : 1;
			unsigned short VBIASEN10 : 1;
			unsigned short VBIASEN9 : 1;
			unsigned short VBIASEN8 : 1;
			unsigned short VBIASEN7 : 1;
			unsigned short VBIASEN6 : 1;
			unsigned short VBIASEN5 : 1;
			unsigned short VBIASEN4 : 1;
			unsigned short VBIASEN3 : 1;
			unsigned short VBIASEN2 : 1;
			unsigned short VBIASEN1 : 1;
			unsigned short VBIASEN0 : 1;
#endif
	} BIT;
	} VBOSR;
	char           wk3[2];
	unsigned long  TC0R;
	unsigned long  TC1R;
	unsigned long  TC2R;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LSWEN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char LSWEN : 1;
#endif
	} BIT;
	} LSWC;
	char           wk4[3];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 3;
			unsigned short TSEN : 1;
#else
			unsigned short TSEN : 1;
			unsigned short  : 3;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS00ISR;
	char           wk5[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 3;
			unsigned short TSEN : 1;
#else
			unsigned short TSEN : 1;
			unsigned short  : 3;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS01ISR;
	char           wk6[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 3;
			unsigned short TSEN : 1;
#else
			unsigned short TSEN : 1;
			unsigned short  : 3;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS02ISR;
	char           wk7[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 3;
			unsigned short TSEN : 1;
#else
			unsigned short TSEN : 1;
			unsigned short  : 3;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS03ISR;
	char           wk8[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 3;
			unsigned short TSEN : 1;
#else
			unsigned short TSEN : 1;
			unsigned short  : 3;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS04ISR;
	char           wk9[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 3;
			unsigned short TSEN : 1;
#else
			unsigned short TSEN : 1;
			unsigned short  : 3;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS05ISR;
	char           wk10[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS10ISR;
	char           wk11[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS11ISR;
	char           wk12[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS12ISR;
	char           wk13[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS13ISR;
	char           wk14[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS14ISR;
	char           wk15[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PSEL : 4;
			unsigned short NSEL : 4;
			unsigned short RSEL : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short RSEL : 4;
			unsigned short NSEL : 4;
			unsigned short PSEL : 4;
#endif
	} BIT;
	} DS15ISR;
	char           wk16[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CUR : 3;
			unsigned char  : 1;
			unsigned char MODE : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char MODE : 1;
			unsigned char  : 1;
			unsigned char CUR : 3;
#endif
	} BIT;
	} EXCCR;
	char           wk17[3];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short IEXC0SEL : 4;
			unsigned short IEXC1SEL : 4;
			unsigned short IEXC2SEL : 4;
			unsigned short IEXC3SEL : 4;
#else
			unsigned short IEXC3SEL : 4;
			unsigned short IEXC2SEL : 4;
			unsigned short IEXC1SEL : 4;
			unsigned short IEXC0SEL : 4;
#endif
	} BIT;
	} EXCOSR;
} st_afe_t;

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

typedef struct st_dmac {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DMST : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DMST : 1;
#endif
	} BIT;
	} DMAST;
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DCTG : 2;
			unsigned short  : 6;
			unsigned short SZ : 2;
			unsigned short  : 2;
			unsigned short DTS : 2;
			unsigned short MD : 2;
#else
			unsigned short MD : 2;
			unsigned short DTS : 2;
			unsigned short  : 2;
			unsigned short SZ : 2;
			unsigned short  : 6;
			unsigned short DCTG : 2;
#endif
	} BIT;
	} DMTMD;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DARIE : 1;
			unsigned char SARIE : 1;
			unsigned char RPTIE : 1;
			unsigned char ESIE : 1;
			unsigned char DTIE : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char DTIE : 1;
			unsigned char ESIE : 1;
			unsigned char RPTIE : 1;
			unsigned char SARIE : 1;
			unsigned char DARIE : 1;
#endif
	} BIT;
	} DMINT;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DARA : 5;
			unsigned short  : 1;
			unsigned short DM : 2;
			unsigned short SARA : 5;
			unsigned short  : 1;
			unsigned short SM : 2;
#else
			unsigned short SM : 2;
			unsigned short  : 1;
			unsigned short SARA : 5;
			unsigned short DM : 2;
			unsigned short  : 1;
			unsigned short DARA : 5;
#endif
	} BIT;
	} DMAMD;
	char           wk2[2];
	unsigned long  DMOFR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DTE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DTE : 1;
#endif
	} BIT;
	} DMCNT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SWREQ : 1;
			unsigned char  : 3;
			unsigned char CLRS : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char CLRS : 1;
			unsigned char  : 3;
			unsigned char SWREQ : 1;
#endif
	} BIT;
	} DMREQ;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ESIF : 1;
			unsigned char  : 3;
			unsigned char DTIF : 1;
			unsigned char  : 2;
			unsigned char ACT : 1;
#else
			unsigned char ACT : 1;
			unsigned char  : 2;
			unsigned char DTIF : 1;
			unsigned char  : 3;
			unsigned char ESIF : 1;
#endif
	} BIT;
	} DMSTS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DISEL : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DISEL : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DCTG : 2;
			unsigned short  : 6;
			unsigned short SZ : 2;
			unsigned short  : 2;
			unsigned short DTS : 2;
			unsigned short MD : 2;
#else
			unsigned short MD : 2;
			unsigned short DTS : 2;
			unsigned short  : 2;
			unsigned short SZ : 2;
			unsigned short  : 6;
			unsigned short DCTG : 2;
#endif
	} BIT;
	} DMTMD;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DARIE : 1;
			unsigned char SARIE : 1;
			unsigned char RPTIE : 1;
			unsigned char ESIE : 1;
			unsigned char DTIE : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char DTIE : 1;
			unsigned char ESIE : 1;
			unsigned char RPTIE : 1;
			unsigned char SARIE : 1;
			unsigned char DARIE : 1;
#endif
	} BIT;
	} DMINT;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DARA : 5;
			unsigned short  : 1;
			unsigned short DM : 2;
			unsigned short SARA : 5;
			unsigned short  : 1;
			unsigned short SM : 2;
#else
			unsigned short SM : 2;
			unsigned short  : 1;
			unsigned short SARA : 5;
			unsigned short DM : 2;
			unsigned short  : 1;
			unsigned short DARA : 5;
#endif
	} BIT;
	} DMAMD;
	char           wk2[6];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DTE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DTE : 1;
#endif
	} BIT;
	} DMCNT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SWREQ : 1;
			unsigned char  : 3;
			unsigned char CLRS : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char CLRS : 1;
			unsigned char  : 3;
			unsigned char SWREQ : 1;
#endif
	} BIT;
	} DMREQ;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ESIF : 1;
			unsigned char  : 3;
			unsigned char DTIF : 1;
			unsigned char  : 2;
			unsigned char ACT : 1;
#else
			unsigned char ACT : 1;
			unsigned char  : 2;
			unsigned char DTIF : 1;
			unsigned char  : 3;
			unsigned char ESIF : 1;
#endif
	} BIT;
	} DMSTS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DISEL : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DISEL : 1;
#endif
	} BIT;
	} DMCSL;
} st_dmac1_t;

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

typedef struct st_dsad {
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CLKDIV : 4;
			unsigned long  : 3;
			unsigned long LPMD : 1;
			unsigned long  : 24;
#else
			unsigned long  : 24;
			unsigned long LPMD : 1;
			unsigned long  : 3;
			unsigned long CLKDIV : 4;
#endif
	} BIT;
	} CCR;
	char           wk0[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long SCMD : 1;
			unsigned long  : 1;
			unsigned long SYNCST : 1;
			unsigned long  : 1;
			unsigned long TRGMD : 1;
			unsigned long  : 3;
			unsigned long CH0EN : 1;
			unsigned long CH1EN : 1;
			unsigned long CH2EN : 1;
			unsigned long CH3EN : 1;
			unsigned long CH4EN : 1;
			unsigned long CH5EN : 1;
			unsigned long  : 18;
#else
			unsigned long  : 18;
			unsigned long CH5EN : 1;
			unsigned long CH4EN : 1;
			unsigned long CH3EN : 1;
			unsigned long CH2EN : 1;
			unsigned long CH1EN : 1;
			unsigned long CH0EN : 1;
			unsigned long  : 3;
			unsigned long TRGMD : 1;
			unsigned long  : 1;
			unsigned long SYNCST : 1;
			unsigned long  : 1;
			unsigned long SCMD : 1;
#endif
	} BIT;
	} MR;
	char           wk1[20];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long CVMD : 1;
			unsigned long SDF : 1;
			unsigned long  : 1;
			unsigned long OSR : 3;
			unsigned long  : 5;
			unsigned long DISAP : 1;
			unsigned long DISAN : 1;
			unsigned long  : 2;
			unsigned long AVMD : 2;
			unsigned long AVDN : 2;
			unsigned long  : 8;
			unsigned long DISC : 2;
			unsigned long  : 2;
#else
			unsigned long  : 2;
			unsigned long DISC : 2;
			unsigned long  : 8;
			unsigned long AVDN : 2;
			unsigned long AVMD : 2;
			unsigned long  : 2;
			unsigned long DISAN : 1;
			unsigned long DISAP : 1;
			unsigned long  : 5;
			unsigned long OSR : 3;
			unsigned long  : 1;
			unsigned long SDF : 1;
			unsigned long CVMD : 1;
			unsigned long  : 1;
#endif
	} BIT;
	} MR0;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long CVMD : 1;
			unsigned long SDF : 1;
			unsigned long  : 1;
			unsigned long OSR : 3;
			unsigned long  : 5;
			unsigned long DISAP : 1;
			unsigned long DISAN : 1;
			unsigned long  : 2;
			unsigned long AVMD : 2;
			unsigned long AVDN : 2;
			unsigned long  : 8;
			unsigned long DISC : 2;
			unsigned long  : 2;
#else
			unsigned long  : 2;
			unsigned long DISC : 2;
			unsigned long  : 8;
			unsigned long AVDN : 2;
			unsigned long AVMD : 2;
			unsigned long  : 2;
			unsigned long DISAN : 1;
			unsigned long DISAP : 1;
			unsigned long  : 5;
			unsigned long OSR : 3;
			unsigned long  : 1;
			unsigned long SDF : 1;
			unsigned long CVMD : 1;
			unsigned long  : 1;
#endif
	} BIT;
	} MR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long CVMD : 1;
			unsigned long SDF : 1;
			unsigned long  : 1;
			unsigned long OSR : 3;
			unsigned long  : 5;
			unsigned long DISAP : 1;
			unsigned long DISAN : 1;
			unsigned long  : 2;
			unsigned long AVMD : 2;
			unsigned long AVDN : 2;
			unsigned long  : 8;
			unsigned long DISC : 2;
			unsigned long  : 2;
#else
			unsigned long  : 2;
			unsigned long DISC : 2;
			unsigned long  : 8;
			unsigned long AVDN : 2;
			unsigned long AVMD : 2;
			unsigned long  : 2;
			unsigned long DISAN : 1;
			unsigned long DISAP : 1;
			unsigned long  : 5;
			unsigned long OSR : 3;
			unsigned long  : 1;
			unsigned long SDF : 1;
			unsigned long CVMD : 1;
			unsigned long  : 1;
#endif
	} BIT;
	} MR2;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long CVMD : 1;
			unsigned long SDF : 1;
			unsigned long  : 1;
			unsigned long OSR : 3;
			unsigned long  : 5;
			unsigned long DISAP : 1;
			unsigned long DISAN : 1;
			unsigned long  : 2;
			unsigned long AVMD : 2;
			unsigned long AVDN : 2;
			unsigned long  : 8;
			unsigned long DISC : 2;
			unsigned long  : 2;
#else
			unsigned long  : 2;
			unsigned long DISC : 2;
			unsigned long  : 8;
			unsigned long AVDN : 2;
			unsigned long AVMD : 2;
			unsigned long  : 2;
			unsigned long DISAN : 1;
			unsigned long DISAP : 1;
			unsigned long  : 5;
			unsigned long OSR : 3;
			unsigned long  : 1;
			unsigned long SDF : 1;
			unsigned long CVMD : 1;
			unsigned long  : 1;
#endif
	} BIT;
	} MR3;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long CVMD : 1;
			unsigned long SDF : 1;
			unsigned long  : 1;
			unsigned long OSR : 3;
			unsigned long  : 5;
			unsigned long DISAP : 1;
			unsigned long DISAN : 1;
			unsigned long  : 2;
			unsigned long AVMD : 2;
			unsigned long AVDN : 2;
			unsigned long  : 8;
			unsigned long DISC : 2;
			unsigned long  : 2;
#else
			unsigned long  : 2;
			unsigned long DISC : 2;
			unsigned long  : 8;
			unsigned long AVDN : 2;
			unsigned long AVMD : 2;
			unsigned long  : 2;
			unsigned long DISAN : 1;
			unsigned long DISAP : 1;
			unsigned long  : 5;
			unsigned long OSR : 3;
			unsigned long  : 1;
			unsigned long SDF : 1;
			unsigned long CVMD : 1;
			unsigned long  : 1;
#endif
	} BIT;
	} MR4;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long CVMD : 1;
			unsigned long SDF : 1;
			unsigned long  : 1;
			unsigned long OSR : 3;
			unsigned long  : 5;
			unsigned long DISAP : 1;
			unsigned long DISAN : 1;
			unsigned long  : 2;
			unsigned long AVMD : 2;
			unsigned long AVDN : 2;
			unsigned long  : 8;
			unsigned long DISC : 2;
			unsigned long  : 2;
#else
			unsigned long  : 2;
			unsigned long DISC : 2;
			unsigned long  : 8;
			unsigned long AVDN : 2;
			unsigned long AVMD : 2;
			unsigned long  : 2;
			unsigned long DISAN : 1;
			unsigned long DISAP : 1;
			unsigned long  : 5;
			unsigned long OSR : 3;
			unsigned long  : 1;
			unsigned long SDF : 1;
			unsigned long CVMD : 1;
			unsigned long  : 1;
#endif
	} BIT;
	} MR5;
	char           wk2[8];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CNY : 5;
			unsigned long CNX : 3;
			unsigned long  : 3;
			unsigned long CNMD : 1;
			unsigned long  : 4;
			unsigned long GAIN : 5;
			unsigned long  : 11;
#else
			unsigned long  : 11;
			unsigned long GAIN : 5;
			unsigned long  : 4;
			unsigned long CNMD : 1;
			unsigned long  : 3;
			unsigned long CNX : 3;
			unsigned long CNY : 5;
#endif
	} BIT;
	} CR0;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CNY : 5;
			unsigned long CNX : 3;
			unsigned long  : 3;
			unsigned long CNMD : 1;
			unsigned long  : 4;
			unsigned long GAIN : 5;
			unsigned long  : 11;
#else
			unsigned long  : 11;
			unsigned long GAIN : 5;
			unsigned long  : 4;
			unsigned long CNMD : 1;
			unsigned long  : 3;
			unsigned long CNX : 3;
			unsigned long CNY : 5;
#endif
	} BIT;
	} CR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CNY : 5;
			unsigned long CNX : 3;
			unsigned long  : 3;
			unsigned long CNMD : 1;
			unsigned long  : 4;
			unsigned long GAIN : 5;
			unsigned long  : 11;
#else
			unsigned long  : 11;
			unsigned long GAIN : 5;
			unsigned long  : 4;
			unsigned long CNMD : 1;
			unsigned long  : 3;
			unsigned long CNX : 3;
			unsigned long CNY : 5;
#endif
	} BIT;
	} CR2;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CNY : 5;
			unsigned long CNX : 3;
			unsigned long  : 3;
			unsigned long CNMD : 1;
			unsigned long  : 4;
			unsigned long GAIN : 5;
			unsigned long  : 11;
#else
			unsigned long  : 11;
			unsigned long GAIN : 5;
			unsigned long  : 4;
			unsigned long CNMD : 1;
			unsigned long  : 3;
			unsigned long CNX : 3;
			unsigned long CNY : 5;
#endif
	} BIT;
	} CR3;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CNY : 5;
			unsigned long CNX : 3;
			unsigned long  : 3;
			unsigned long CNMD : 1;
			unsigned long  : 4;
			unsigned long GAIN : 5;
			unsigned long  : 11;
#else
			unsigned long  : 11;
			unsigned long GAIN : 5;
			unsigned long  : 4;
			unsigned long CNMD : 1;
			unsigned long  : 3;
			unsigned long CNX : 3;
			unsigned long CNY : 5;
#endif
	} BIT;
	} CR4;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CNY : 5;
			unsigned long CNX : 3;
			unsigned long  : 3;
			unsigned long CNMD : 1;
			unsigned long  : 4;
			unsigned long GAIN : 5;
			unsigned long  : 11;
#else
			unsigned long  : 11;
			unsigned long GAIN : 5;
			unsigned long  : 4;
			unsigned long CNMD : 1;
			unsigned long  : 3;
			unsigned long CNX : 3;
			unsigned long CNY : 5;
#endif
	} BIT;
	} CR5;
	char           wk3[8];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long START : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long START : 1;
#endif
	} BIT;
	} ADST;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long STOP : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long STOP : 1;
#endif
	} BIT;
	} ADSTP;
	char           wk4[8];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long DATA : 24;
			unsigned long OVF : 1;
			unsigned long ERR : 1;
			unsigned long  : 3;
			unsigned long CCH : 3;
#else
			unsigned long CCH : 3;
			unsigned long  : 3;
			unsigned long ERR : 1;
			unsigned long OVF : 1;
			unsigned long DATA : 24;
#endif
	} BIT;
	} DR;
	char           wk5[28];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long DATA : 24;
			unsigned long OVF : 1;
			unsigned long ERR : 1;
			unsigned long  : 3;
			unsigned long CCH : 3;
#else
			unsigned long CCH : 3;
			unsigned long  : 3;
			unsigned long ERR : 1;
			unsigned long OVF : 1;
			unsigned long DATA : 24;
#endif
	} BIT;
	} AVDR;
	char           wk6[28];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long ACT : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long ACT : 1;
#endif
	} BIT;
	} SR;
	char           wk7[12];
	unsigned long  OSR0;
	unsigned long  OSR1;
	unsigned long  OSR2;
	unsigned long  OSR3;
	unsigned long  OSR4;
	unsigned long  OSR5;
	char           wk8[72];
	unsigned long  GCR0;
	unsigned long  GCR1;
	unsigned long  GCR2;
	unsigned long  GCR3;
	unsigned long  GCR4;
	unsigned long  GCR5;
	char           wk9[8];
	unsigned long  OFCR0;
	unsigned long  OFCR1;
	unsigned long  OFCR2;
	unsigned long  OFCR3;
	unsigned long  OFCR4;
	unsigned long  OFCR5;
} st_dsad_t;

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

typedef struct st_elc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ELCON : 1;
#else
			unsigned char ELCON : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} ELCR;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR4;
	char           wk1[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR7;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR8;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR10;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR12;
	char           wk4[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR15;
	char           wk5[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR18;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR19;
	char           wk6[4];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR24;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR25;
	char           wk7[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR28;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR29;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
			unsigned char MTU1MD : 2;
			unsigned char MTU2MD : 2;
			unsigned char MTU3MD : 2;
#else
			unsigned char MTU3MD : 2;
			unsigned char MTU2MD : 2;
			unsigned char MTU1MD : 2;
			unsigned char  : 2;
#endif
	} BIT;
	} ELOPA;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MTU4MD : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char MTU4MD : 2;
#endif
	} BIT;
	} ELOPB;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
			unsigned char CMT1MD : 2;
			unsigned char LPTMD : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char LPTMD : 2;
			unsigned char CMT1MD : 2;
			unsigned char  : 2;
#endif
	} BIT;
	} ELOPC;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TMR0MD : 2;
			unsigned char  : 2;
			unsigned char TMR2MD : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char TMR2MD : 2;
			unsigned char  : 2;
			unsigned char TMR0MD : 2;
#endif
	} BIT;
	} ELOPD;
	char           wk8[6];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSB : 3;
			unsigned char PSP : 2;
			unsigned char PSM : 2;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char PSM : 2;
			unsigned char PSP : 2;
			unsigned char PSB : 3;
#endif
	} BIT;
	} PEL0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSB : 3;
			unsigned char PSP : 2;
			unsigned char PSM : 2;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char PSM : 2;
			unsigned char PSP : 2;
			unsigned char PSB : 3;
#endif
	} BIT;
	} PEL1;
	char           wk9[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SEG : 1;
			unsigned char  : 5;
			unsigned char WE : 1;
			unsigned char WI : 1;
#else
			unsigned char WI : 1;
			unsigned char WE : 1;
			unsigned char  : 5;
			unsigned char SEG : 1;
#endif
	} BIT;
	} ELSEGR;
	char           wk10[22];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR46;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR47;
} st_elc_t;

typedef struct st_flash {
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
	char           wk0[111];
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
	char           wk1[3];
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
	char           wk2[3];
	unsigned short FSARL;
	char           wk3[6];
	unsigned short FSARH;
	char           wk4[2];
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
	char           wk5[3];
	unsigned short FEARL;
	char           wk6[6];
	unsigned short FEARH;
	char           wk7[2];
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
	char           wk8[7];
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
	char           wk9[3];
	unsigned short FWB0;
	char           wk10[6];
	unsigned short FWB1;
	char           wk11[6];
	unsigned short FWB2;
	char           wk12[2];
	unsigned short FWB3;
	char           wk13[58];
	unsigned char  FPR;
	char           wk14[3];
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
	char           wk15[59];
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
	char           wk16[6];
	unsigned short FAWSMR;
	char           wk17[6];
	unsigned short FAWEMR;
	char           wk18[6];
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
	char           wk19[3];
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
	char           wk20[3];
	unsigned short FEAML;
	char           wk21[6];
	unsigned short FEAMH;
	char           wk22[6];
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
	char           wk23[15809];
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
	unsigned char  DMRSR0;
	char           wk3[3];
	unsigned char  DMRSR1;
	char           wk4[3];
	unsigned char  DMRSR2;
	char           wk5[3];
	unsigned char  DMRSR3;
	char           wk6[243];
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
	char           wk7[8];
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
	char           wk8[3];
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
	char           wk9[106];
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
	char           wk10[12];
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
	char           wk11[3];
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

typedef struct st_lpt {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LPCNTPSSEL : 3;
			unsigned char  : 1;
			unsigned char LPCNTCKSEL : 1;
			unsigned char  : 1;
			unsigned char LPCMRE0 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char LPCMRE0 : 1;
			unsigned char  : 1;
			unsigned char LPCNTCKSEL : 1;
			unsigned char  : 1;
			unsigned char LPCNTPSSEL : 3;
#endif
	} BIT;
	} LPTCR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LPCNTSTP : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char LPCNTSTP : 1;
#endif
	} BIT;
	} LPTCR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LPCNTEN : 1;
			unsigned char LPCNTRST : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char LPCNTRST : 1;
			unsigned char LPCNTEN : 1;
#endif
	} BIT;
	} LPTCR3;
	char           wk0[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short LPCNTPRD : 16;
#else
			unsigned short LPCNTPRD : 16;
#endif
	} BIT;
	} LPTPRD;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short LPCMR0 : 16;
#else
			unsigned short LPCMR0 : 16;
#endif
	} BIT;
	} LPCMR0;
	char           wk2[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 15;
			unsigned short LPWKUPEN : 1;
#else
			unsigned short LPWKUPEN : 1;
			unsigned short  : 15;
#endif
	} BIT;
	} LPWUCR;
} st_lpt_t;

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
	char           wk0[44];
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
	} P14PFS;
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
	} P15PFS;
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
	} P16PFS;
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
	} P17PFS;
	char           wk1[6];
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
	} P26PFS;
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
	char           wk2[62];
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
	char           wk3[10];
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
	} PC7PFS;
	char           wk4[32];
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
	} PH0PFS;
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
	} PH1PFS;
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
	} PH2PFS;
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
	} PH3PFS;
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
	} TOER;
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
	} TGCR;
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
	} TOCR1;
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
	} TOCR2;
	char           wk1[4];
	unsigned short TCDR;
	unsigned short TDDR;
	char           wk2[8];
	unsigned short TCNTS;
	unsigned short TCBR;
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
	} TITCR;
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
	} TITCNT;
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
	} TBTER;
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
	} TDER;
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
	} TOLBR;
	char           wk6[41];
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
	} TWCR;
	char           wk7[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CST0 : 1;
			unsigned char CST1 : 1;
			unsigned char CST2 : 1;
			unsigned char  : 3;
			unsigned char CST3 : 1;
			unsigned char CST4 : 1;
#else
			unsigned char CST4 : 1;
			unsigned char CST3 : 1;
			unsigned char  : 3;
			unsigned char CST2 : 1;
			unsigned char CST1 : 1;
			unsigned char CST0 : 1;
#endif
	} BIT;
	} TSTR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SYNC0 : 1;
			unsigned char SYNC1 : 1;
			unsigned char SYNC2 : 1;
			unsigned char  : 3;
			unsigned char SYNC3 : 1;
			unsigned char SYNC4 : 1;
#else
			unsigned char SYNC4 : 1;
			unsigned char SYNC3 : 1;
			unsigned char  : 3;
			unsigned char SYNC2 : 1;
			unsigned char SYNC1 : 1;
			unsigned char SYNC0 : 1;
#endif
	} BIT;
	} TSYR;
	char           wk8[2];
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
	} TRWER;
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
	} NFCR;
	char           wk0[111];
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
	} TMDR;
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
	unsigned short TGRC;
	unsigned short TGRD;
	char           wk1[16];
	unsigned short TGRE;
	unsigned short TGRF;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEE : 1;
			unsigned char TGIEF : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char TGIEF : 1;
			unsigned char TGIEE : 1;
#endif
	} BIT;
	} TIER2;
	char           wk2[1];
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
} st_mtu0_t;

typedef struct st_mtu1 {
	char           wk0[1];
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
	} NFCR;
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
	} TMDR;
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
} st_mtu1_t;

typedef struct st_mtu2 {
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
	} NFCR;
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
	} TMDR;
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
	} TMDR;
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
	char           wk8[90];
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
	} NFCR;
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
	} TMDR;
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
	char           wk11[72];
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
	} NFCR;
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
	} NFCR;
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
	char           wk2[1];
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
	char           wk3[9];
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
	char           wk4[1];
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
	char           wk5[9];
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
	char           wk6[1];
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
	char           wk7[11];
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
	char           wk8[1];
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
	char           wk9[1];
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

typedef struct st_ofsm {
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MDE : 3;
			unsigned long  : 29;
#else
			unsigned long  : 29;
			unsigned long MDE : 3;
#endif
	} BIT;
	} MDE;
	char           wk0[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long VDSEL : 2;
			unsigned long LVDAS : 1;
			unsigned long FASTSTUP : 1;
			unsigned long  : 4;
			unsigned long HOCOEN : 1;
			unsigned long  : 23;
#else
			unsigned long  : 23;
			unsigned long HOCOEN : 1;
			unsigned long  : 4;
			unsigned long FASTSTUP : 1;
			unsigned long LVDAS : 1;
			unsigned long VDSEL : 2;
#endif
	} BIT;
	} OFS1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 1;
			unsigned long IWDTSTRT : 1;
			unsigned long IWDTTOPS : 2;
			unsigned long IWDTCKS : 4;
			unsigned long IWDTRPES : 2;
			unsigned long IWDTRPSS : 2;
			unsigned long IWDTRSTIRQS : 1;
			unsigned long  : 1;
			unsigned long IWDTSLCSTP : 1;
			unsigned long  : 17;
#else
			unsigned long  : 17;
			unsigned long IWDTSLCSTP : 1;
			unsigned long  : 1;
			unsigned long IWDTRSTIRQS : 1;
			unsigned long IWDTRPSS : 2;
			unsigned long IWDTRPES : 2;
			unsigned long IWDTCKS : 4;
			unsigned long IWDTTOPS : 2;
			unsigned long IWDTSTRT : 1;
			unsigned long  : 1;
#endif
	} BIT;
	} OFS0;
} st_ofsm_t;

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
			unsigned short POE1M : 2;
			unsigned short POE2M : 2;
			unsigned short POE3M : 2;
			unsigned short PIE1 : 1;
			unsigned short  : 3;
			unsigned short POE0F : 1;
			unsigned short POE1F : 1;
			unsigned short POE2F : 1;
			unsigned short POE3F : 1;
#else
			unsigned short POE3F : 1;
			unsigned short POE2F : 1;
			unsigned short POE1F : 1;
			unsigned short POE0F : 1;
			unsigned short  : 3;
			unsigned short PIE1 : 1;
			unsigned short POE3M : 2;
			unsigned short POE2M : 2;
			unsigned short POE1M : 2;
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
	char           wk0[4];
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
			unsigned short PIE2 : 1;
			unsigned short POE8E : 1;
			unsigned short  : 2;
			unsigned short POE8F : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short POE8F : 1;
			unsigned short  : 2;
			unsigned short POE8E : 1;
			unsigned short PIE2 : 1;
			unsigned short  : 6;
			unsigned short POE8M : 2;
#endif
	} BIT;
	} ICSR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CH34HIZ : 1;
			unsigned char CH0HIZ : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char CH0HIZ : 1;
			unsigned char CH34HIZ : 1;
#endif
	} BIT;
	} SPOER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PE0ZE : 1;
			unsigned char PE1ZE : 1;
			unsigned char PE2ZE : 1;
			unsigned char PE3ZE : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char PE3ZE : 1;
			unsigned char PE2ZE : 1;
			unsigned char PE1ZE : 1;
			unsigned char PE0ZE : 1;
#endif
	} BIT;
	} POECR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char P3CZEA : 1;
			unsigned char P2CZEA : 1;
			unsigned char P1CZEA : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char P1CZEA : 1;
			unsigned char P2CZEA : 1;
			unsigned char P3CZEA : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} POECR2;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 1;
			unsigned short  : 8;
			unsigned short OSTSTE : 1;
			unsigned short  : 2;
			unsigned short OSTSTF : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short OSTSTF : 1;
			unsigned short  : 2;
			unsigned short OSTSTE : 1;
			unsigned short  : 8;
			unsigned short  : 1;
#endif
	} BIT;
	} ICSR3;
} st_poe_t;

typedef struct st_port1 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
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
	} ODR1;
	char           wk4[61];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} DSCR;
} st_port1_t;

typedef struct st_port2 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} PMR;
	char           wk3[34];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} ODR1;
	char           wk4[60];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 6;
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
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 6;
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
			unsigned char  : 4;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 4;
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
			unsigned char  : 4;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 4;
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
			unsigned char  : 3;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char  : 3;
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
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
			unsigned char  : 5;
#else
			unsigned char  : 5;
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
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
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
			unsigned char  : 4;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 4;
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
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
} st_port3_t;

typedef struct st_portb {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
			unsigned char B3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
	char           wk4[52];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char B1 : 1;
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
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
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
	} ODR1;
	char           wk4[50];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 4;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 4;
#endif
	} BIT;
	} DSCR;
} st_portc_t;

typedef struct st_porth {
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
	char           wk3[48];
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
	char           wk4[46];
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
} st_porth_t;

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
			unsigned short ADHSC : 1;
			unsigned short  : 1;
			unsigned short ADIE : 1;
			unsigned short ADCS : 2;
			unsigned short ADST : 1;
#else
			unsigned short ADST : 1;
			unsigned short ADCS : 2;
			unsigned short ADIE : 1;
			unsigned short  : 1;
			unsigned short ADHSC : 1;
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
			unsigned short  : 10;
#else
			unsigned short  : 10;
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
			unsigned short  : 10;
#else
			unsigned short  : 10;
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
	char           wk4[2];
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
			unsigned short  : 10;
#else
			unsigned short  : 10;
			unsigned short ANSB005 : 1;
			unsigned short ANSB004 : 1;
			unsigned short ANSB003 : 1;
			unsigned short ANSB002 : 1;
			unsigned short ANSB001 : 1;
			unsigned short ANSB000 : 1;
#endif
	} BIT;
	} ADANSB0;
	char           wk5[2];
	unsigned short ADDBLDR;
	char           wk6[4];
	union {
		unsigned short WORD;
		union {
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short AD : 12;
			unsigned short  : 2;
			unsigned short DIAGST : 2;
#else
			unsigned short DIAGST : 2;
			unsigned short  : 2;
			unsigned short AD : 12;
#endif
	} RIGHT;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DIAGST : 2;
			unsigned short  : 2;
			unsigned short AD : 12;
#else
			unsigned short AD : 12;
			unsigned short  : 2;
			unsigned short DIAGST : 2;
#endif
	} LEFT;
		} BIT;
	} ADRD;
	unsigned short ADDR0;
	unsigned short ADDR1;
	unsigned short ADDR2;
	unsigned short ADDR3;
	unsigned short ADDR4;
	unsigned short ADDR5;
	char           wk7[78];
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
	char           wk8[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELCC : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char ELCC : 2;
#endif
	} BIT;
	} ADELCCR;
	char           wk9[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PGS : 1;
			unsigned short GBRSCN : 1;
			unsigned short  : 13;
			unsigned short GBRP : 1;
#else
			unsigned short GBRP : 1;
			unsigned short  : 13;
			unsigned short GBRSCN : 1;
			unsigned short PGS : 1;
#endif
	} BIT;
	} ADGSPCR;
	char           wk10[8];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HVSEL : 2;
			unsigned char  : 2;
			unsigned char LVSEL : 1;
			unsigned char  : 2;
			unsigned char ADSLP : 1;
#else
			unsigned char ADSLP : 1;
			unsigned char  : 2;
			unsigned char LVSEL : 1;
			unsigned char  : 2;
			unsigned char HVSEL : 2;
#endif
	} BIT;
	} ADHVREFCNT;
	char           wk11[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MONCOMB : 1;
			unsigned char  : 3;
			unsigned char MONCMPA : 1;
			unsigned char MONCMPB : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char MONCMPB : 1;
			unsigned char MONCMPA : 1;
			unsigned char  : 3;
			unsigned char MONCOMB : 1;
#endif
	} BIT;
	} ADWINMON;
	char           wk12[3];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMPAB : 2;
			unsigned short  : 7;
			unsigned short CMPBE : 1;
			unsigned short  : 1;
			unsigned short CMPAE : 1;
			unsigned short  : 2;
			unsigned short WCMPE : 1;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short WCMPE : 1;
			unsigned short  : 2;
			unsigned short CMPAE : 1;
			unsigned short  : 1;
			unsigned short CMPBE : 1;
			unsigned short  : 7;
			unsigned short CMPAB : 2;
#endif
	} BIT;
	} ADCMPCR;
	char           wk13[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMPCHA000 : 1;
			unsigned short CMPCHA001 : 1;
			unsigned short CMPCHA002 : 1;
			unsigned short CMPCHA003 : 1;
			unsigned short CMPCHA004 : 1;
			unsigned short CMPCHA005 : 1;
			unsigned short  : 10;
#else
			unsigned short  : 10;
			unsigned short CMPCHA005 : 1;
			unsigned short CMPCHA004 : 1;
			unsigned short CMPCHA003 : 1;
			unsigned short CMPCHA002 : 1;
			unsigned short CMPCHA001 : 1;
			unsigned short CMPCHA000 : 1;
#endif
	} BIT;
	} ADCMPANSR0;
	char           wk14[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMPLCHA000 : 1;
			unsigned short CMPLCHA001 : 1;
			unsigned short CMPLCHA002 : 1;
			unsigned short CMPLCHA003 : 1;
			unsigned short CMPLCHA004 : 1;
			unsigned short CMPLCHA005 : 1;
			unsigned short  : 10;
#else
			unsigned short  : 10;
			unsigned short CMPLCHA005 : 1;
			unsigned short CMPLCHA004 : 1;
			unsigned short CMPLCHA003 : 1;
			unsigned short CMPLCHA002 : 1;
			unsigned short CMPLCHA001 : 1;
			unsigned short CMPLCHA000 : 1;
#endif
	} BIT;
	} ADCMPLR0;
	char           wk15[2];
	unsigned short ADCMPDR0;
	unsigned short ADCMPDR1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMPSTCHA000 : 1;
			unsigned short CMPSTCHA001 : 1;
			unsigned short CMPSTCHA002 : 1;
			unsigned short CMPSTCHA003 : 1;
			unsigned short CMPSTCHA004 : 1;
			unsigned short CMPSTCHA005 : 1;
			unsigned short  : 10;
#else
			unsigned short  : 10;
			unsigned short CMPSTCHA005 : 1;
			unsigned short CMPSTCHA004 : 1;
			unsigned short CMPSTCHA003 : 1;
			unsigned short CMPSTCHA002 : 1;
			unsigned short CMPSTCHA001 : 1;
			unsigned short CMPSTCHA000 : 1;
#endif
	} BIT;
	} ADCMPSR0;
	char           wk16[4];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMPCHB : 6;
			unsigned char  : 1;
			unsigned char CMPLB : 1;
#else
			unsigned char CMPLB : 1;
			unsigned char  : 1;
			unsigned char CMPCHB : 6;
#endif
	} BIT;
	} ADCMPBNSR;
	char           wk17[1];
	unsigned short ADWINLLB;
	unsigned short ADWINULB;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMPSTB : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char CMPSTB : 1;
#endif
	} BIT;
	} ADCMPBSR;
	char           wk18[3];
	unsigned short ADBUF0;
	unsigned short ADBUF1;
	unsigned short ADBUF2;
	unsigned short ADBUF3;
	unsigned short ADBUF4;
	unsigned short ADBUF5;
	unsigned short ADBUF6;
	unsigned short ADBUF7;
	unsigned short ADBUF8;
	unsigned short ADBUF9;
	unsigned short ADBUF10;
	unsigned short ADBUF11;
	unsigned short ADBUF12;
	unsigned short ADBUF13;
	unsigned short ADBUF14;
	unsigned short ADBUF15;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BUFEN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char BUFEN : 1;
#endif
	} BIT;
	} ADBUFEN;
	char           wk19[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BUFPTR : 4;
			unsigned char PTROVF : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PTROVF : 1;
			unsigned char BUFPTR : 4;
#endif
	} BIT;
	} ADBUFPTR;
	char           wk20[13];
	unsigned char  ADSSTR0;
	unsigned char  ADSSTR1;
	unsigned char  ADSSTR2;
	unsigned char  ADSSTR3;
	unsigned char  ADSSTR4;
	unsigned char  ADSSTR5;
} st_s12ad_t;

typedef struct st_sci1 {
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
} st_sci1_t;

typedef struct st_sci12 {
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
	char           wk0[13];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ESME : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char ESME : 1;
#endif
	} BIT;
	} ESMER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char SFSF : 1;
			unsigned char RXDSF : 1;
			unsigned char BRME : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char BRME : 1;
			unsigned char RXDSF : 1;
			unsigned char SFSF : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} CR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BFE : 1;
			unsigned char CF0RE : 1;
			unsigned char CF1DS : 2;
			unsigned char PIBE : 1;
			unsigned char PIBS : 3;
#else
			unsigned char PIBS : 3;
			unsigned char PIBE : 1;
			unsigned char CF1DS : 2;
			unsigned char CF0RE : 1;
			unsigned char BFE : 1;
#endif
	} BIT;
	} CR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DFCS : 3;
			unsigned char  : 1;
			unsigned char BCCS : 2;
			unsigned char RTS : 2;
#else
			unsigned char RTS : 2;
			unsigned char BCCS : 2;
			unsigned char  : 1;
			unsigned char DFCS : 3;
#endif
	} BIT;
	} CR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SDST : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char SDST : 1;
#endif
	} BIT;
	} CR3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TXDXPS : 1;
			unsigned char RXDXPS : 1;
			unsigned char  : 2;
			unsigned char SHARPS : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char SHARPS : 1;
			unsigned char  : 2;
			unsigned char RXDXPS : 1;
			unsigned char TXDXPS : 1;
#endif
	} BIT;
	} PCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BFDIE : 1;
			unsigned char CF0MIE : 1;
			unsigned char CF1MIE : 1;
			unsigned char PIBDIE : 1;
			unsigned char BCDIE : 1;
			unsigned char AEDIE : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char AEDIE : 1;
			unsigned char BCDIE : 1;
			unsigned char PIBDIE : 1;
			unsigned char CF1MIE : 1;
			unsigned char CF0MIE : 1;
			unsigned char BFDIE : 1;
#endif
	} BIT;
	} ICR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BFDF : 1;
			unsigned char CF0MF : 1;
			unsigned char CF1MF : 1;
			unsigned char PIBDF : 1;
			unsigned char BCDF : 1;
			unsigned char AEDF : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char AEDF : 1;
			unsigned char BCDF : 1;
			unsigned char PIBDF : 1;
			unsigned char CF1MF : 1;
			unsigned char CF0MF : 1;
			unsigned char BFDF : 1;
#endif
	} BIT;
	} STR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BFDCL : 1;
			unsigned char CF0MCL : 1;
			unsigned char CF1MCL : 1;
			unsigned char PIBDCL : 1;
			unsigned char BCDCL : 1;
			unsigned char AEDCL : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char AEDCL : 1;
			unsigned char BCDCL : 1;
			unsigned char PIBDCL : 1;
			unsigned char CF1MCL : 1;
			unsigned char CF0MCL : 1;
			unsigned char BFDCL : 1;
#endif
	} BIT;
	} STCR;
	unsigned char  CF0DR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CF0CE0 : 1;
			unsigned char CF0CE1 : 1;
			unsigned char CF0CE2 : 1;
			unsigned char CF0CE3 : 1;
			unsigned char CF0CE4 : 1;
			unsigned char CF0CE5 : 1;
			unsigned char CF0CE6 : 1;
			unsigned char CF0CE7 : 1;
#else
			unsigned char CF0CE7 : 1;
			unsigned char CF0CE6 : 1;
			unsigned char CF0CE5 : 1;
			unsigned char CF0CE4 : 1;
			unsigned char CF0CE3 : 1;
			unsigned char CF0CE2 : 1;
			unsigned char CF0CE1 : 1;
			unsigned char CF0CE0 : 1;
#endif
	} BIT;
	} CF0CR;
	unsigned char  CF0RR;
	unsigned char  PCF1DR;
	unsigned char  SCF1DR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CF1CE0 : 1;
			unsigned char CF1CE1 : 1;
			unsigned char CF1CE2 : 1;
			unsigned char CF1CE3 : 1;
			unsigned char CF1CE4 : 1;
			unsigned char CF1CE5 : 1;
			unsigned char CF1CE6 : 1;
			unsigned char CF1CE7 : 1;
#else
			unsigned char CF1CE7 : 1;
			unsigned char CF1CE6 : 1;
			unsigned char CF1CE5 : 1;
			unsigned char CF1CE4 : 1;
			unsigned char CF1CE3 : 1;
			unsigned char CF1CE2 : 1;
			unsigned char CF1CE1 : 1;
			unsigned char CF1CE0 : 1;
#endif
	} BIT;
	} CF1CR;
	unsigned char  CF1RR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TCST : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char TCST : 1;
#endif
	} BIT;
	} TCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TOMS : 2;
			unsigned char  : 1;
			unsigned char TWRC : 1;
			unsigned char TCSS : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char TCSS : 3;
			unsigned char TWRC : 1;
			unsigned char  : 1;
			unsigned char TOMS : 2;
#endif
	} BIT;
	} TMR;
	unsigned char  TPRE;
	unsigned char  TCNT;
} st_sci12_t;

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
			unsigned long  : 4;
			unsigned long MSTPA4 : 1;
			unsigned long MSTPA5 : 1;
			unsigned long  : 3;
			unsigned long MSTPA9 : 1;
			unsigned long  : 5;
			unsigned long MSTPA15 : 1;
			unsigned long MSTPA16 : 1;
			unsigned long MSTPA17 : 1;
			unsigned long  : 7;
			unsigned long MSTPA25 : 1;
			unsigned long MSTPA26 : 1;
			unsigned long  : 1;
			unsigned long MSTPA28 : 1;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long MSTPA28 : 1;
			unsigned long  : 1;
			unsigned long MSTPA26 : 1;
			unsigned long MSTPA25 : 1;
			unsigned long  : 7;
			unsigned long MSTPA17 : 1;
			unsigned long MSTPA16 : 1;
			unsigned long MSTPA15 : 1;
			unsigned long  : 5;
			unsigned long MSTPA9 : 1;
			unsigned long  : 3;
			unsigned long MSTPA5 : 1;
			unsigned long MSTPA4 : 1;
			unsigned long  : 4;
#endif
	} BIT;
	} MSTPCRA;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MSTPB0 : 1;
			unsigned long  : 3;
			unsigned long MSTPB4 : 1;
			unsigned long  : 1;
			unsigned long MSTPB6 : 1;
			unsigned long  : 2;
			unsigned long MSTPB9 : 1;
			unsigned long  : 7;
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
			unsigned long  : 7;
			unsigned long MSTPB9 : 1;
			unsigned long  : 2;
			unsigned long MSTPB6 : 1;
			unsigned long  : 1;
			unsigned long MSTPB4 : 1;
			unsigned long  : 3;
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
			unsigned long  : 11;
			unsigned long DSLPE : 1;
#else
			unsigned long DSLPE : 1;
			unsigned long  : 11;
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
			unsigned short  : 6;
			unsigned short STC : 6;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short STC : 6;
			unsigned short  : 6;
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
	char           wk5[7];
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
	char           wk7[5];
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
	char           wk8[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short CKOSEL : 4;
			unsigned short CKODIV : 3;
			unsigned short CKOSTP : 1;
#else
			unsigned short CKOSTP : 1;
			unsigned short CKODIV : 3;
			unsigned short CKOSEL : 4;
			unsigned short  : 8;
#endif
	} BIT;
	} CKOCR;
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
	char           wk9[30];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LOCOTRD : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char LOCOTRD : 5;
#endif
	} BIT;
	} LOCOTRR;
	char           wk10[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ILOCOTRD : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char ILOCOTRD : 5;
#endif
	} BIT;
	} ILOCOTRR;
	char           wk11[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HOCOTRD : 6;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char HOCOTRD : 6;
#endif
	} BIT;
	} HOCOTRR0;
	char           wk12[55];
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
	char           wk13[1];
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
	char           wk14[29];
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
	char           wk15[1];
	unsigned short SWRR;
	char           wk16[28];
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
	char           wk17[794];
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
	char           wk18[48784];
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
	char           wk19[1];
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
	char           wk20[3];
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
	char           wk21[1];
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
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TCS : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char TCS : 1;
#endif
	} BIT;
	} TCSTR;
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

