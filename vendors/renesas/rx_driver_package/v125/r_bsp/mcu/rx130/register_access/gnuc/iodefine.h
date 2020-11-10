                                                                          
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                          
/*********************************************************************************
*
* Device     : RX/RX100/RX130
*
* File Name  : iodefine.h
*
* Abstract   : Definition of I/O Register.
*
* History    : 0.5  (2015-06-30)  [Hardware Manual Revision : 0.50]
*            : 1.0  (2015-10-05)  [Hardware Manual Revision : 1.00]
*            : 2.0  (2017-04-17)  [Hardware Manual Revision : 2.00] 
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2015-2017 Renesas Electronics Corporation.
*
*********************************************************************************/
/********************************************************************************/
/*                                                                              */
/*  DESCRIPTION : Definition of ICU Register                                    */
/*  CPU TYPE    : RX130                                                         */
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
/*       IPR(SCI0,ERI0) = 3;     expands to :                                   */
/*       IPR(SCI0,    ) = 3;     // SCI0 uses single IPR for all sources.       */
/*         ICU.IPR[214].BIT.IPR = 3;                                            */
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
/*       MSTP(SCI0) = 0;    // SCI0,SMCI0                         expands to :  */
/*         SYSTEM.MSTPCRB.BIT.MSTPB31 = 0;                                      */
/*       MSTP(MTU4) = 0;    // MTU,MTU0,MTU1,MTU2,MTU3,MTU4,MTU5  expands to :  */
/*         SYSTEM.MSTPCRA.BIT.MSTPA9  = 0;                                      */
/*                                                                              */
/*                                                                              */
/********************************************************************************/
#ifndef __RX130IODEFINE_HEADER__
#define __RX130IODEFINE_HEADER__

#pragma pack(4)

struct st_bsc {
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
			unsigned short  : 2;
			unsigned short BPFB : 2;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short BPFB : 2;
			unsigned short  : 2;
			unsigned short BPGB : 2;
			unsigned short BPIB : 2;
			unsigned short BPRO : 2;
			unsigned short BPRA : 2;
#endif
	} BIT;
	} BUSPRI;
};

struct st_cac {
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
};

struct st_cmpb {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPB0INI : 1;
			unsigned char  : 3;
			unsigned char CPB1INI : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char CPB1INI : 1;
			unsigned char  : 3;
			unsigned char CPB0INI : 1;
#endif
	} BIT;
	} CPBCNT1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPB0WCP : 1;
			unsigned char  : 3;
			unsigned char CPB1WCP : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char CPB1WCP : 1;
			unsigned char  : 3;
			unsigned char CPB0WCP : 1;
#endif
	} BIT;
	} CPBCNT2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
			unsigned char CPB0OUT : 1;
			unsigned char  : 3;
			unsigned char CPB1OUT : 1;
#else
			unsigned char CPB1OUT : 1;
			unsigned char  : 3;
			unsigned char CPB0OUT : 1;
			unsigned char  : 3;
#endif
	} BIT;
	} CPBFLG;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPB0INTEN : 1;
			unsigned char CPB0INTEG : 1;
			unsigned char CPB0INTPL : 1;
			unsigned char  : 1;
			unsigned char CPB1INTEN : 1;
			unsigned char CPB1INTEG : 1;
			unsigned char CPB1INTPL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char CPB1INTPL : 1;
			unsigned char CPB1INTEG : 1;
			unsigned char CPB1INTEN : 1;
			unsigned char  : 1;
			unsigned char CPB0INTPL : 1;
			unsigned char CPB0INTEG : 1;
			unsigned char CPB0INTEN : 1;
#endif
	} BIT;
	} CPBINT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPB0FEN : 1;
			unsigned char  : 1;
			unsigned char CPB0F : 2;
			unsigned char CPB1FEN : 1;
			unsigned char  : 1;
			unsigned char CPB1F : 2;
#else
			unsigned char CPB1F : 2;
			unsigned char  : 1;
			unsigned char CPB1FEN : 1;
			unsigned char CPB0F : 2;
			unsigned char  : 1;
			unsigned char CPB0FEN : 1;
#endif
	} BIT;
	} CPBF;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPBSPDMD : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char CPBSPDMD : 1;
#endif
	} BIT;
	} CPBMD;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPB0VRF : 1;
			unsigned char  : 3;
			unsigned char CPB1VRF : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char CPB1VRF : 1;
			unsigned char  : 3;
			unsigned char CPB0VRF : 1;
#endif
	} BIT;
	} CPBREF;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPB0OE : 1;
			unsigned char CPB0OP : 1;
			unsigned char  : 2;
			unsigned char CPB1OE : 1;
			unsigned char CPB1OP : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char CPB1OP : 1;
			unsigned char CPB1OE : 1;
			unsigned char  : 2;
			unsigned char CPB0OP : 1;
			unsigned char CPB0OE : 1;
#endif
	} BIT;
	} CPBOCR;
};

struct st_cmt {
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
};

struct st_cmt0 {
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
};

struct st_crc {
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
};

struct st_ctsu {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUSTRT : 1;
			unsigned char CTSUCAP : 1;
			unsigned char CTSUSNZ : 1;
			unsigned char CTSUIOC : 1;
			unsigned char CTSUINIT : 1;
			unsigned char  : 2;
			unsigned char CTSUTXVSEL : 1;
#else
			unsigned char CTSUTXVSEL : 1;
			unsigned char  : 2;
			unsigned char CTSUINIT : 1;
			unsigned char CTSUIOC : 1;
			unsigned char CTSUSNZ : 1;
			unsigned char CTSUCAP : 1;
			unsigned char CTSUSTRT : 1;
#endif
	} BIT;
	} CTSUCR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUPON : 1;
			unsigned char CTSUCSW : 1;
			unsigned char CTSUATUNE0 : 1;
			unsigned char CTSUATUNE1 : 1;
			unsigned char CTSUCLK : 2;
			unsigned char CTSUMD : 2;
#else
			unsigned char CTSUMD : 2;
			unsigned char CTSUCLK : 2;
			unsigned char CTSUATUNE1 : 1;
			unsigned char CTSUATUNE0 : 1;
			unsigned char CTSUCSW : 1;
			unsigned char CTSUPON : 1;
#endif
	} BIT;
	} CTSUCR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUPRRATIO : 4;
			unsigned char CTSUPRMODE : 2;
			unsigned char CTSUSOFF : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char CTSUSOFF : 1;
			unsigned char CTSUPRMODE : 2;
			unsigned char CTSUPRRATIO : 4;
#endif
	} BIT;
	} CTSUSDPRS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUSST : 8;
#else
			unsigned char CTSUSST : 8;
#endif
	} BIT;
	} CTSUSST;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUMCH0 : 6;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char CTSUMCH0 : 6;
#endif
	} BIT;
	} CTSUMCH0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUMCH1 : 6;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char CTSUMCH1 : 6;
#endif
	} BIT;
	} CTSUMCH1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHAC00 : 1;
			unsigned char CTSUCHAC01 : 1;
			unsigned char CTSUCHAC02 : 1;
			unsigned char CTSUCHAC03 : 1;
			unsigned char CTSUCHAC04 : 1;
			unsigned char CTSUCHAC05 : 1;
			unsigned char CTSUCHAC06 : 1;
			unsigned char CTSUCHAC07 : 1;
#else
			unsigned char CTSUCHAC07 : 1;
			unsigned char CTSUCHAC06 : 1;
			unsigned char CTSUCHAC05 : 1;
			unsigned char CTSUCHAC04 : 1;
			unsigned char CTSUCHAC03 : 1;
			unsigned char CTSUCHAC02 : 1;
			unsigned char CTSUCHAC01 : 1;
			unsigned char CTSUCHAC00 : 1;
#endif
	} BIT;
	} CTSUCHAC0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHAC10 : 1;
			unsigned char CTSUCHAC11 : 1;
			unsigned char CTSUCHAC12 : 1;
			unsigned char CTSUCHAC13 : 1;
			unsigned char CTSUCHAC14 : 1;
			unsigned char CTSUCHAC15 : 1;
			unsigned char CTSUCHAC16 : 1;
			unsigned char CTSUCHAC17 : 1;
#else
			unsigned char CTSUCHAC17 : 1;
			unsigned char CTSUCHAC16 : 1;
			unsigned char CTSUCHAC15 : 1;
			unsigned char CTSUCHAC14 : 1;
			unsigned char CTSUCHAC13 : 1;
			unsigned char CTSUCHAC12 : 1;
			unsigned char CTSUCHAC11 : 1;
			unsigned char CTSUCHAC10 : 1;
#endif
	} BIT;
	} CTSUCHAC1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHAC20 : 1;
			unsigned char CTSUCHAC21 : 1;
			unsigned char CTSUCHAC22 : 1;
			unsigned char CTSUCHAC23 : 1;
			unsigned char CTSUCHAC24 : 1;
			unsigned char CTSUCHAC25 : 1;
			unsigned char CTSUCHAC26 : 1;
			unsigned char CTSUCHAC27 : 1;
#else
			unsigned char CTSUCHAC27 : 1;
			unsigned char CTSUCHAC26 : 1;
			unsigned char CTSUCHAC25 : 1;
			unsigned char CTSUCHAC24 : 1;
			unsigned char CTSUCHAC23 : 1;
			unsigned char CTSUCHAC22 : 1;
			unsigned char CTSUCHAC21 : 1;
			unsigned char CTSUCHAC20 : 1;
#endif
	} BIT;
	} CTSUCHAC2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHAC30 : 1;
			unsigned char CTSUCHAC31 : 1;
			unsigned char CTSUCHAC32 : 1;
			unsigned char CTSUCHAC33 : 1;
			unsigned char CTSUCHAC34 : 1;
			unsigned char CTSUCHAC35 : 1;
			unsigned char CTSUCHAC36 : 1;
			unsigned char CTSUCHAC37 : 1;
#else
			unsigned char CTSUCHAC37 : 1;
			unsigned char CTSUCHAC36 : 1;
			unsigned char CTSUCHAC35 : 1;
			unsigned char CTSUCHAC34 : 1;
			unsigned char CTSUCHAC33 : 1;
			unsigned char CTSUCHAC32 : 1;
			unsigned char CTSUCHAC31 : 1;
			unsigned char CTSUCHAC30 : 1;
#endif
	} BIT;
	} CTSUCHAC3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHAC40 : 1;
			unsigned char CTSUCHAC41 : 1;
			unsigned char CTSUCHAC42 : 1;
			unsigned char CTSUCHAC43 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char CTSUCHAC43 : 1;
			unsigned char CTSUCHAC42 : 1;
			unsigned char CTSUCHAC41 : 1;
			unsigned char CTSUCHAC40 : 1;
#endif
	} BIT;
	} CTSUCHAC4;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHTRC00 : 1;
			unsigned char CTSUCHTRC01 : 1;
			unsigned char CTSUCHTRC02 : 1;
			unsigned char CTSUCHTRC03 : 1;
			unsigned char CTSUCHTRC04 : 1;
			unsigned char CTSUCHTRC05 : 1;
			unsigned char CTSUCHTRC06 : 1;
			unsigned char CTSUCHTRC07 : 1;
#else
			unsigned char CTSUCHTRC07 : 1;
			unsigned char CTSUCHTRC06 : 1;
			unsigned char CTSUCHTRC05 : 1;
			unsigned char CTSUCHTRC04 : 1;
			unsigned char CTSUCHTRC03 : 1;
			unsigned char CTSUCHTRC02 : 1;
			unsigned char CTSUCHTRC01 : 1;
			unsigned char CTSUCHTRC00 : 1;
#endif
	} BIT;
	} CTSUCHTRC0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHTRC10 : 1;
			unsigned char CTSUCHTRC11 : 1;
			unsigned char CTSUCHTRC12 : 1;
			unsigned char CTSUCHTRC13 : 1;
			unsigned char CTSUCHTRC14 : 1;
			unsigned char CTSUCHTRC15 : 1;
			unsigned char CTSUCHTRC16 : 1;
			unsigned char CTSUCHTRC17 : 1;
#else
			unsigned char CTSUCHTRC17 : 1;
			unsigned char CTSUCHTRC16 : 1;
			unsigned char CTSUCHTRC15 : 1;
			unsigned char CTSUCHTRC14 : 1;
			unsigned char CTSUCHTRC13 : 1;
			unsigned char CTSUCHTRC12 : 1;
			unsigned char CTSUCHTRC11 : 1;
			unsigned char CTSUCHTRC10 : 1;
#endif
	} BIT;
	} CTSUCHTRC1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHTRC20 : 1;
			unsigned char CTSUCHTRC21 : 1;
			unsigned char CTSUCHTRC22 : 1;
			unsigned char CTSUCHTRC23 : 1;
			unsigned char CTSUCHTRC24 : 1;
			unsigned char CTSUCHTRC25 : 1;
			unsigned char CTSUCHTRC26 : 1;
			unsigned char CTSUCHTRC27 : 1;
#else
			unsigned char CTSUCHTRC27 : 1;
			unsigned char CTSUCHTRC26 : 1;
			unsigned char CTSUCHTRC25 : 1;
			unsigned char CTSUCHTRC24 : 1;
			unsigned char CTSUCHTRC23 : 1;
			unsigned char CTSUCHTRC22 : 1;
			unsigned char CTSUCHTRC21 : 1;
			unsigned char CTSUCHTRC20 : 1;
#endif
	} BIT;
	} CTSUCHTRC2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHTRC30 : 1;
			unsigned char CTSUCHTRC31 : 1;
			unsigned char CTSUCHTRC32 : 1;
			unsigned char CTSUCHTRC33 : 1;
			unsigned char CTSUCHTRC34 : 1;
			unsigned char CTSUCHTRC35 : 1;
			unsigned char CTSUCHTRC36 : 1;
			unsigned char CTSUCHTRC37 : 1;
#else
			unsigned char CTSUCHTRC37 : 1;
			unsigned char CTSUCHTRC36 : 1;
			unsigned char CTSUCHTRC35 : 1;
			unsigned char CTSUCHTRC34 : 1;
			unsigned char CTSUCHTRC33 : 1;
			unsigned char CTSUCHTRC32 : 1;
			unsigned char CTSUCHTRC31 : 1;
			unsigned char CTSUCHTRC30 : 1;
#endif
	} BIT;
	} CTSUCHTRC3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUCHTRC40 : 1;
			unsigned char CTSUCHTRC41 : 1;
			unsigned char CTSUCHTRC42 : 1;
			unsigned char CTSUCHTRC43 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char CTSUCHTRC43 : 1;
			unsigned char CTSUCHTRC42 : 1;
			unsigned char CTSUCHTRC41 : 1;
			unsigned char CTSUCHTRC40 : 1;
#endif
	} BIT;
	} CTSUCHTRC4;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUSSMOD : 2;
			unsigned char  : 2;
			unsigned char CTSUSSCNT : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char CTSUSSCNT : 2;
			unsigned char  : 2;
			unsigned char CTSUSSMOD : 2;
#endif
	} BIT;
	} CTSUDCLKC;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTSUSTC : 3;
			unsigned char  : 1;
			unsigned char CTSUDTSR : 1;
			unsigned char CTSUSOVF : 1;
			unsigned char CTSUROVF : 1;
			unsigned char CTSUPS : 1;
#else
			unsigned char CTSUPS : 1;
			unsigned char CTSUROVF : 1;
			unsigned char CTSUSOVF : 1;
			unsigned char CTSUDTSR : 1;
			unsigned char  : 1;
			unsigned char CTSUSTC : 3;
#endif
	} BIT;
	} CTSUST;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short  : 8;
			unsigned short CTSUSSDIV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short CTSUSSDIV : 4;
			unsigned short  : 8;
#endif
	} BIT;
	} CTSUSSC;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CTSUSO : 10;
			unsigned short CTSUSNUM : 6;
#else
			unsigned short CTSUSNUM : 6;
			unsigned short CTSUSO : 10;
#endif
	} BIT;
	} CTSUSO0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CTSURICOA : 8;
			unsigned short CTSUSDPA : 5;
			unsigned short CTSUICOG : 2;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short CTSUICOG : 2;
			unsigned short CTSUSDPA : 5;
			unsigned short CTSURICOA : 8;
#endif
	} BIT;
	} CTSUSO1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CTSUSC : 16;
#else
			unsigned short CTSUSC : 16;
#endif
	} BIT;
	} CTSUSC;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CTSURC : 16;
#else
			unsigned short CTSURC : 16;
#endif
	} BIT;
	} CTSURC;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CTSUSPMD : 2;
			unsigned short CTSUTSOD : 1;
			unsigned short CTSUDRV : 1;
			unsigned short  : 3;
			unsigned short CTSUTSOC : 1;
			unsigned short  : 7;
			unsigned short CTSUICOMP : 1;
#else
			unsigned short CTSUICOMP : 1;
			unsigned short  : 7;
			unsigned short CTSUTSOC : 1;
			unsigned short  : 3;
			unsigned short CTSUDRV : 1;
			unsigned short CTSUTSOD : 1;
			unsigned short CTSUSPMD : 2;
#endif
	} BIT;
	} CTSUERRS;
	char           wk0[7730848];
	unsigned char  CTSUTRMR;
};

struct st_da {
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
};

struct st_doc {
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
};

struct st_dtc {
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
};

struct st_elc {
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ELS : 8;
#else
			unsigned char ELS : 8;
#endif
	} BIT;
	} ELSR[26];
	char           wk0[4];
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PGR0 : 1;
			unsigned char PGR1 : 1;
			unsigned char PGR2 : 1;
			unsigned char PGR3 : 1;
			unsigned char PGR4 : 1;
			unsigned char PGR5 : 1;
			unsigned char PGR6 : 1;
			unsigned char PGR7 : 1;
#else
			unsigned char PGR7 : 1;
			unsigned char PGR6 : 1;
			unsigned char PGR5 : 1;
			unsigned char PGR4 : 1;
			unsigned char PGR3 : 1;
			unsigned char PGR2 : 1;
			unsigned char PGR1 : 1;
			unsigned char PGR0 : 1;
#endif
	} BIT;
	} PGR1;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PGCI : 2;
			unsigned char PGCOVE : 1;
			unsigned char  : 1;
			unsigned char PGCO : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char PGCO : 3;
			unsigned char  : 1;
			unsigned char PGCOVE : 1;
			unsigned char PGCI : 2;
#endif
	} BIT;
	} PGC1;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PDBF0 : 1;
			unsigned char PDBF1 : 1;
			unsigned char PDBF2 : 1;
			unsigned char PDBF3 : 1;
			unsigned char PDBF4 : 1;
			unsigned char PDBF5 : 1;
			unsigned char PDBF6 : 1;
			unsigned char PDBF7 : 1;
#else
			unsigned char PDBF7 : 1;
			unsigned char PDBF6 : 1;
			unsigned char PDBF5 : 1;
			unsigned char PDBF4 : 1;
			unsigned char PDBF3 : 1;
			unsigned char PDBF2 : 1;
			unsigned char PDBF1 : 1;
			unsigned char PDBF0 : 1;
#endif
	} BIT;
	} PDBF1;
	char           wk3[1];
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
	char           wk4[2];
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
};

struct st_flash {
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
	char           wk0[31];
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
	unsigned short FAWSMR;
	unsigned short FAWEMR;
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
	unsigned short FEAML;
	unsigned char  FEAMH;
	char           wk1[5];
	unsigned char  FPR;
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
	unsigned short FRBL;
	unsigned short FRBH;
	char           wk2[16058];
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
	unsigned short FSARL;
	unsigned char  FSARH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMD : 4;
			unsigned char DRC : 1;
			unsigned char  : 1;
			unsigned char STOP : 1;
			unsigned char OPST : 1;
#else
			unsigned char OPST : 1;
			unsigned char STOP : 1;
			unsigned char  : 1;
			unsigned char DRC : 1;
			unsigned char CMD : 4;
#endif
	} BIT;
	} FCR;
	unsigned short FEARL;
	unsigned char  FEARH;
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char DRRDY : 1;
			unsigned char  : 4;
			unsigned char FRDY : 1;
			unsigned char EXRDY : 1;
#else
			unsigned char EXRDY : 1;
			unsigned char FRDY : 1;
			unsigned char  : 4;
			unsigned char DRRDY : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} FSTATR1;
	unsigned short FWBL;
	unsigned short FWBH;
	char           wk3[34];
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
};

struct st_icu {
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
	} IR[250];
	char           wk0[6];
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
	} DTCER[249];
	char           wk1[7];
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
	char           wk2[192];
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
	char           wk3[15];
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
	char           wk4[14];
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
	} IPR[250];
	char           wk5[262];
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
	char           wk6[8];
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
	char           wk7[3];
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
	char           wk8[106];
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
	char           wk9[12];
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
	char           wk10[3];
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
};

struct st_iwdt {
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
};

struct st_lpt {
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
};

struct st_mpc {
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
	char           wk0[35];
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
	} P03PFS;
	char           wk1[1];
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
	} P05PFS;
	char           wk2[1];
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
	} P07PFS;
	char           wk3[2];
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
	} P12PFS;
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
	} P13PFS;
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
	} P20PFS;
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
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSEL : 5;
#endif
	} BIT;
	} P23PFS;
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
	} P32PFS;
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
	} P33PFS;
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
	} P34PFS;
	char           wk4[3];
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
	} P51PFS;
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
	} P52PFS;
	char           wk6[1];
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
	} P54PFS;
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
	} P55PFS;
	char           wk7[34];
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
	} PA3PFS;
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
	} PA4PFS;
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
			unsigned char  : 3;
#else
			unsigned char  : 3;
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
			unsigned char  : 3;
#else
			unsigned char  : 3;
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
	} PD0PFS;
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
	} PD1PFS;
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
	} PD2PFS;
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
	} PD3PFS;
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
	} PD4PFS;
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
	} PD5PFS;
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
	} PD6PFS;
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
	} PD7PFS;
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
	} PE0PFS;
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
	} PE1PFS;
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
	} PE2PFS;
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
	} PE3PFS;
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
	} PE4PFS;
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
	} PE5PFS;
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
	} PE6PFS;
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
	} PE7PFS;
	char           wk8[16];
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
	char           wk9[5];
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
	} PJ1PFS;
	char           wk10[1];
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
	} PJ3PFS;
	char           wk11[2];
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
	} PJ6PFS;
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
	} PJ7PFS;
};

struct st_mtu {
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
};

struct st_mtu0 {
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
};

struct st_mtu1 {
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
};

struct st_mtu2 {
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
};

struct st_mtu3 {
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
};

struct st_mtu4 {
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
};

struct st_mtu5 {
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
};

struct st_poe {
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
};

struct st_port {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSEL0 : 1;
			unsigned char PSEL1 : 1;
			unsigned char  : 1;
			unsigned char PSEL3 : 1;
			unsigned char  : 1;
			unsigned char PSEL5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char PSEL5 : 1;
			unsigned char  : 1;
			unsigned char PSEL3 : 1;
			unsigned char  : 1;
			unsigned char PSEL1 : 1;
			unsigned char PSEL0 : 1;
#endif
	} BIT;
	} PSRB;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 6;
			unsigned char PSEL6 : 1;
			unsigned char PSEL7 : 1;
#else
			unsigned char PSEL7 : 1;
			unsigned char PSEL6 : 1;
			unsigned char  : 6;
#endif
	} BIT;
	} PSRA;
};

struct st_port0 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
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
			unsigned char  : 3;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
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
			unsigned char  : 3;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
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
			unsigned char  : 3;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B5 : 1;
			unsigned char  : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char  : 1;
			unsigned char B5 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char  : 3;
#endif
	} BIT;
	} PMR;
	char           wk3[95];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
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
			unsigned char  : 3;
#endif
	} BIT;
	} PCR;
};

struct st_port1 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
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
			unsigned char  : 2;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
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
			unsigned char  : 2;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
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
			unsigned char  : 2;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
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
			unsigned char  : 2;
#endif
	} BIT;
	} PMR;
	char           wk3[32];
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
			unsigned char  : 2;
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
			unsigned char  : 2;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 2;
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
			unsigned char  : 2;
#endif
	} BIT;
	} DSCR;
};

struct st_port2 {
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
};

struct st_port3 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
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
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
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
			unsigned char  : 3;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char  : 3;
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
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
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
	} DSCR;
};

struct st_port4 {
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
	char           wk3[95];
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
};

struct st_port5 {
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
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[95];
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
	char           wk4[31];
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
};

struct st_porta {
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
};

struct st_portb {
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
};

struct st_portc {
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
};

struct st_portd {
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
};

struct st_porte {
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
	char           wk3[45];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char  : 1;
			unsigned char B2 : 1;
			unsigned char B3 : 1;
			unsigned char B4 : 1;
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 1;
			unsigned char B4 : 1;
			unsigned char B3 : 1;
			unsigned char B2 : 1;
			unsigned char  : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} ODR0;
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
};

struct st_porth {
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
	char           wk3[95];
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
	char           wk4[31];
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
};

struct st_portj {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char  : 2;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 2;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char  : 2;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 2;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char  : 2;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 2;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char  : 2;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 2;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} PMR;
	char           wk3[49];
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
	} ODR0;
	char           wk4[45];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char  : 2;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 2;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} PCR;
	char           wk5[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B1 : 1;
			unsigned char  : 1;
#endif
	} BIT;
	} DSCR;
};

struct st_remc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ENFLG : 1;
			unsigned char INV : 1;
			unsigned char FIL : 1;
			unsigned char INFLG : 1;
			unsigned char EC : 1;
			unsigned char  : 1;
			unsigned char FILSEL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char FILSEL : 1;
			unsigned char  : 1;
			unsigned char EC : 1;
			unsigned char INFLG : 1;
			unsigned char FIL : 1;
			unsigned char INV : 1;
			unsigned char ENFLG : 1;
#endif
	} BIT;
	} REMCON0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TYP : 2;
			unsigned char EN : 1;
			unsigned char CSRC : 4;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char CSRC : 4;
			unsigned char EN : 1;
			unsigned char TYP : 2;
#endif
	} BIT;
	} REMCON1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPFLG : 1;
			unsigned char REFLG : 1;
			unsigned char DRFLG : 1;
			unsigned char BFULFLG : 1;
			unsigned char HDFLG : 1;
			unsigned char D0FLG : 1;
			unsigned char D1FLG : 1;
			unsigned char SDFLG : 1;
#else
			unsigned char SDFLG : 1;
			unsigned char D1FLG : 1;
			unsigned char D0FLG : 1;
			unsigned char HDFLG : 1;
			unsigned char BFULFLG : 1;
			unsigned char DRFLG : 1;
			unsigned char REFLG : 1;
			unsigned char CPFLG : 1;
#endif
	} BIT;
	} REMSTS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPINT : 1;
			unsigned char REINT : 1;
			unsigned char DRINT : 1;
			unsigned char BFULINT : 1;
			unsigned char HDINT : 1;
			unsigned char DINT : 1;
			unsigned char  : 1;
			unsigned char SDINT : 1;
#else
			unsigned char SDINT : 1;
			unsigned char  : 1;
			unsigned char DINT : 1;
			unsigned char HDINT : 1;
			unsigned char BFULINT : 1;
			unsigned char DRINT : 1;
			unsigned char REINT : 1;
			unsigned char CPINT : 1;
#endif
	} BIT;
	} REMINT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPN : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char CPN : 3;
#endif
	} BIT;
	} REMCPC;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CPD : 8;
#else
			unsigned char CPD : 8;
#endif
	} BIT;
	} REMCPD;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short HDPMIN : 11;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short HDPMIN : 11;
#endif
	} BIT;
	} HDPMIN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short HDPMAX : 11;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short HDPMAX : 11;
#endif
	} BIT;
	} HDPMAX;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char D0PMIN : 8;
#else
			unsigned char D0PMIN : 8;
#endif
	} BIT;
	} D0PMIN;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char D0PMAX : 8;
#else
			unsigned char D0PMAX : 8;
#endif
	} BIT;
	} D0PMAX;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char D1PMIN : 8;
#else
			unsigned char D1PMIN : 8;
#endif
	} BIT;
	} D1PMIN;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char D1PMAX : 8;
#else
			unsigned char D1PMAX : 8;
#endif
	} BIT;
	} D1PMAX;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short SDPMIN : 11;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short SDPMIN : 11;
#endif
	} BIT;
	} SDPMIN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short SDPMAX : 11;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short SDPMAX : 11;
#endif
	} BIT;
	} SDPMAX;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PE : 11;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short PE : 11;
#endif
	} BIT;
	} REMPE;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LPCE : 1;
			unsigned char DNFSL : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char DNFSL : 1;
			unsigned char LPCE : 1;
#endif
	} BIT;
	} REMSTC;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RBIT : 7;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char RBIT : 7;
#endif
	} BIT;
	} REMRBIT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAT0 : 8;
#else
			unsigned char DAT0 : 8;
#endif
	} BIT;
	} REMDAT0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAT1 : 8;
#else
			unsigned char DAT1 : 8;
#endif
	} BIT;
	} REMDAT1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAT2 : 8;
#else
			unsigned char DAT2 : 8;
#endif
	} BIT;
	} REMDAT2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAT3 : 8;
#else
			unsigned char DAT3 : 8;
#endif
	} BIT;
	} REMDAT3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAT4 : 8;
#else
			unsigned char DAT4 : 8;
#endif
	} BIT;
	} REMDAT4;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAT5 : 8;
#else
			unsigned char DAT5 : 8;
#endif
	} BIT;
	} REMDAT5;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAT6 : 8;
#else
			unsigned char DAT6 : 8;
#endif
	} BIT;
	} REMDAT6;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAT7 : 8;
#else
			unsigned char DAT7 : 8;
#endif
	} BIT;
	} REMDAT7;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TIM : 11;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short TIM : 11;
#endif
	} BIT;
	} REMTIM;
};

struct st_remcom {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HOSE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char HOSE : 1;
#endif
	} BIT;
	} HOSCR;
};

struct st_riic {
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
};

struct st_rspi {
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
			unsigned char  : 1;
			unsigned char SPTEF : 1;
			unsigned char  : 1;
			unsigned char SPRF : 1;
#else
			unsigned char SPRF : 1;
			unsigned char  : 1;
			unsigned char SPTEF : 1;
			unsigned char  : 1;
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
};

struct st_rtc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char F64HZ : 1;
			unsigned char F32HZ : 1;
			unsigned char F16HZ : 1;
			unsigned char F8HZ : 1;
			unsigned char F4HZ : 1;
			unsigned char F2HZ : 1;
			unsigned char F1HZ : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char F1HZ : 1;
			unsigned char F2HZ : 1;
			unsigned char F4HZ : 1;
			unsigned char F8HZ : 1;
			unsigned char F16HZ : 1;
			unsigned char F32HZ : 1;
			unsigned char F64HZ : 1;
#endif
	} BIT;
	} R64CNT;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SEC1 : 4;
			unsigned char SEC10 : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char SEC10 : 3;
			unsigned char SEC1 : 4;
#endif
	} BIT;
	} RSECCNT;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MIN1 : 4;
			unsigned char MIN10 : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char MIN10 : 3;
			unsigned char MIN1 : 4;
#endif
	} BIT;
	} RMINCNT;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HR1 : 4;
			unsigned char HR10 : 2;
			unsigned char PM : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char PM : 1;
			unsigned char HR10 : 2;
			unsigned char HR1 : 4;
#endif
	} BIT;
	} RHRCNT;
	char           wk3[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAYW : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char DAYW : 3;
#endif
	} BIT;
	} RWKCNT;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DATE1 : 4;
			unsigned char DATE10 : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char DATE10 : 2;
			unsigned char DATE1 : 4;
#endif
	} BIT;
	} RDAYCNT;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MON1 : 4;
			unsigned char MON10 : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char MON10 : 1;
			unsigned char MON1 : 4;
#endif
	} BIT;
	} RMONCNT;
	char           wk6[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short YR1 : 4;
			unsigned short YR10 : 4;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short YR10 : 4;
			unsigned short YR1 : 4;
#endif
	} BIT;
	} RYRCNT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SEC1 : 4;
			unsigned char SEC10 : 3;
			unsigned char ENB : 1;
#else
			unsigned char ENB : 1;
			unsigned char SEC10 : 3;
			unsigned char SEC1 : 4;
#endif
	} BIT;
	} RSECAR;
	char           wk7[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MIN1 : 4;
			unsigned char MIN10 : 3;
			unsigned char ENB : 1;
#else
			unsigned char ENB : 1;
			unsigned char MIN10 : 3;
			unsigned char MIN1 : 4;
#endif
	} BIT;
	} RMINAR;
	char           wk8[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HR1 : 4;
			unsigned char HR10 : 2;
			unsigned char PM : 1;
			unsigned char ENB : 1;
#else
			unsigned char ENB : 1;
			unsigned char PM : 1;
			unsigned char HR10 : 2;
			unsigned char HR1 : 4;
#endif
	} BIT;
	} RHRAR;
	char           wk9[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DAYW : 3;
			unsigned char  : 4;
			unsigned char ENB : 1;
#else
			unsigned char ENB : 1;
			unsigned char  : 4;
			unsigned char DAYW : 3;
#endif
	} BIT;
	} RWKAR;
	char           wk10[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DATE1 : 4;
			unsigned char DATE10 : 2;
			unsigned char  : 1;
			unsigned char ENB : 1;
#else
			unsigned char ENB : 1;
			unsigned char  : 1;
			unsigned char DATE10 : 2;
			unsigned char DATE1 : 4;
#endif
	} BIT;
	} RDAYAR;
	char           wk11[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MON1 : 4;
			unsigned char MON10 : 1;
			unsigned char  : 2;
			unsigned char ENB : 1;
#else
			unsigned char ENB : 1;
			unsigned char  : 2;
			unsigned char MON10 : 1;
			unsigned char MON1 : 4;
#endif
	} BIT;
	} RMONAR;
	char           wk12[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short YR1 : 4;
			unsigned short YR10 : 4;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short YR10 : 4;
			unsigned short YR1 : 4;
#endif
	} BIT;
	} RYRAR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 7;
			unsigned char ENB : 1;
#else
			unsigned char ENB : 1;
			unsigned char  : 7;
#endif
	} BIT;
	} RYRAREN;
	char           wk13[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char AIE : 1;
			unsigned char CIE : 1;
			unsigned char PIE : 1;
			unsigned char RTCOS : 1;
			unsigned char PES : 4;
#else
			unsigned char PES : 4;
			unsigned char RTCOS : 1;
			unsigned char PIE : 1;
			unsigned char CIE : 1;
			unsigned char AIE : 1;
#endif
	} BIT;
	} RCR1;
	char           wk14[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char START : 1;
			unsigned char RESET : 1;
			unsigned char ADJ30 : 1;
			unsigned char RTCOE : 1;
			unsigned char AADJE : 1;
			unsigned char AADJP : 1;
			unsigned char HR24 : 1;
			unsigned char CNTMD : 1;
#else
			unsigned char CNTMD : 1;
			unsigned char HR24 : 1;
			unsigned char AADJP : 1;
			unsigned char AADJE : 1;
			unsigned char RTCOE : 1;
			unsigned char ADJ30 : 1;
			unsigned char RESET : 1;
			unsigned char START : 1;
#endif
	} BIT;
	} RCR2;
	char           wk15[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RTCEN : 1;
			unsigned char RTCDV : 3;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char RTCDV : 3;
			unsigned char RTCEN : 1;
#endif
	} BIT;
	} RCR3;
	char           wk16[7];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADJ : 6;
			unsigned char PMADJ : 2;
#else
			unsigned char PMADJ : 2;
			unsigned char ADJ : 6;
#endif
	} BIT;
	} RADJ;
};

struct st_rtcb {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCNT : 8;
#else
			unsigned char BCNT : 8;
#endif
	} BIT;
	} BCNT0;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCNT : 8;
#else
			unsigned char BCNT : 8;
#endif
	} BIT;
	} BCNT1;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCNT : 8;
#else
			unsigned char BCNT : 8;
#endif
	} BIT;
	} BCNT2;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCNT : 8;
#else
			unsigned char BCNT : 8;
#endif
	} BIT;
	} BCNT3;
	char           wk3[7];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCNTAR : 8;
#else
			unsigned char BCNTAR : 8;
#endif
	} BIT;
	} BCNT0AR;
	char           wk4[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCNTAR : 8;
#else
			unsigned char BCNTAR : 8;
#endif
	} BIT;
	} BCNT1AR;
	char           wk5[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCNTAR : 8;
#else
			unsigned char BCNTAR : 8;
#endif
	} BIT;
	} BCNT2AR;
	char           wk6[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCNTAR : 8;
#else
			unsigned char BCNTAR : 8;
#endif
	} BIT;
	} BCNT3AR;
	char           wk7[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ENB : 8;
#else
			unsigned char ENB : 8;
#endif
	} BIT;
	} BCNT0AER;
	char           wk8[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ENB : 8;
#else
			unsigned char ENB : 8;
#endif
	} BIT;
	} BCNT1AER;
	char           wk9[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ENB : 8;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short ENB : 8;
#endif
	} BIT;
	} BCNT2AER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ENB : 8;
#else
			unsigned char ENB : 8;
#endif
	} BIT;
	} BCNT3AER;
};

struct st_s12ad {
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
			unsigned short ANSA006 : 1;
			unsigned short ANSA007 : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
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
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSA100 : 1;
			unsigned short ANSA101 : 1;
			unsigned short ANSA102 : 1;
			unsigned short ANSA103 : 1;
			unsigned short ANSA104 : 1;
			unsigned short ANSA105 : 1;
			unsigned short ANSA106 : 1;
			unsigned short ANSA107 : 1;
			unsigned short ANSA108 : 1;
			unsigned short ANSA109 : 1;
			unsigned short ANSA110 : 1;
			unsigned short ANSA111 : 1;
			unsigned short ANSA112 : 1;
			unsigned short ANSA113 : 1;
			unsigned short ANSA114 : 1;
			unsigned short ANSA115 : 1;
#else
			unsigned short ANSA115 : 1;
			unsigned short ANSA114 : 1;
			unsigned short ANSA113 : 1;
			unsigned short ANSA112 : 1;
			unsigned short ANSA111 : 1;
			unsigned short ANSA110 : 1;
			unsigned short ANSA109 : 1;
			unsigned short ANSA108 : 1;
			unsigned short ANSA107 : 1;
			unsigned short ANSA106 : 1;
			unsigned short ANSA105 : 1;
			unsigned short ANSA104 : 1;
			unsigned short ANSA103 : 1;
			unsigned short ANSA102 : 1;
			unsigned short ANSA101 : 1;
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
			unsigned short ADS004 : 1;
			unsigned short ADS005 : 1;
			unsigned short ADS006 : 1;
			unsigned short ADS007 : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
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
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ADS100 : 1;
			unsigned short ADS101 : 1;
			unsigned short ADS102 : 1;
			unsigned short ADS103 : 1;
			unsigned short ADS104 : 1;
			unsigned short ADS105 : 1;
			unsigned short ADS106 : 1;
			unsigned short ADS107 : 1;
			unsigned short ADS108 : 1;
			unsigned short ADS109 : 1;
			unsigned short ADS110 : 1;
			unsigned short ADS111 : 1;
			unsigned short ADS112 : 1;
			unsigned short ADS113 : 1;
			unsigned short ADS114 : 1;
			unsigned short ADS115 : 1;
#else
			unsigned short ADS115 : 1;
			unsigned short ADS114 : 1;
			unsigned short ADS113 : 1;
			unsigned short ADS112 : 1;
			unsigned short ADS111 : 1;
			unsigned short ADS110 : 1;
			unsigned short ADS109 : 1;
			unsigned short ADS108 : 1;
			unsigned short ADS107 : 1;
			unsigned short ADS106 : 1;
			unsigned short ADS105 : 1;
			unsigned short ADS104 : 1;
			unsigned short ADS103 : 1;
			unsigned short ADS102 : 1;
			unsigned short ADS101 : 1;
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
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TSSAD : 1;
			unsigned short OCSAD : 1;
			unsigned short  : 6;
			unsigned short TSSA : 1;
			unsigned short OCSA : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short OCSA : 1;
			unsigned short TSSA : 1;
			unsigned short  : 6;
			unsigned short OCSAD : 1;
			unsigned short TSSAD : 1;
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
			unsigned short  : 8;
#else
			unsigned short  : 8;
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
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANSB100 : 1;
			unsigned short ANSB101 : 1;
			unsigned short ANSB102 : 1;
			unsigned short ANSB103 : 1;
			unsigned short ANSB104 : 1;
			unsigned short ANSB105 : 1;
			unsigned short ANSB106 : 1;
			unsigned short ANSB107 : 1;
			unsigned short ANSB108 : 1;
			unsigned short ANSB109 : 1;
			unsigned short ANSB110 : 1;
			unsigned short ANSB111 : 1;
			unsigned short ANSB112 : 1;
			unsigned short ANSB113 : 1;
			unsigned short ANSB114 : 1;
			unsigned short ANSB115 : 1;
#else
			unsigned short ANSB115 : 1;
			unsigned short ANSB114 : 1;
			unsigned short ANSB113 : 1;
			unsigned short ANSB112 : 1;
			unsigned short ANSB111 : 1;
			unsigned short ANSB110 : 1;
			unsigned short ANSB109 : 1;
			unsigned short ANSB108 : 1;
			unsigned short ANSB107 : 1;
			unsigned short ANSB106 : 1;
			unsigned short ANSB105 : 1;
			unsigned short ANSB104 : 1;
			unsigned short ANSB103 : 1;
			unsigned short ANSB102 : 1;
			unsigned short ANSB101 : 1;
			unsigned short ANSB100 : 1;
#endif
	} BIT;
	} ADANSB1;
	unsigned short ADDBLDR;
	unsigned short ADTSDR;
	unsigned short ADOCDR;
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
	unsigned short ADDR6;
	unsigned short ADDR7;
	char           wk2[16];
	unsigned short ADDR16;
	unsigned short ADDR17;
	unsigned short ADDR18;
	unsigned short ADDR19;
	unsigned short ADDR20;
	unsigned short ADDR21;
	unsigned short ADDR22;
	unsigned short ADDR23;
	unsigned short ADDR24;
	unsigned short ADDR25;
	unsigned short ADDR26;
	unsigned short ADDR27;
	unsigned short ADDR28;
	unsigned short ADDR29;
	unsigned short ADDR30;
	unsigned short ADDR31;
	char           wk3[26];
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
	char           wk4[2];
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
	char           wk5[2];
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
	char           wk6[8];
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
	char           wk7[1];
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
	char           wk8[3];
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMPTSA : 1;
			unsigned char CMPOCA : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char CMPOCA : 1;
			unsigned char CMPTSA : 1;
#endif
	} BIT;
	} ADCMPANSER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMPLTSA : 1;
			unsigned char CMPLOCA : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char CMPLOCA : 1;
			unsigned char CMPLTSA : 1;
#endif
	} BIT;
	} ADCMPLER;
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
			unsigned short CMPCHA006 : 1;
			unsigned short CMPCHA007 : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short CMPCHA007 : 1;
			unsigned short CMPCHA006 : 1;
			unsigned short CMPCHA005 : 1;
			unsigned short CMPCHA004 : 1;
			unsigned short CMPCHA003 : 1;
			unsigned short CMPCHA002 : 1;
			unsigned short CMPCHA001 : 1;
			unsigned short CMPCHA000 : 1;
#endif
	} BIT;
	} ADCMPANSR0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMPCHA100 : 1;
			unsigned short CMPCHA101 : 1;
			unsigned short CMPCHA102 : 1;
			unsigned short CMPCHA103 : 1;
			unsigned short CMPCHA104 : 1;
			unsigned short CMPCHA105 : 1;
			unsigned short CMPCHA106 : 1;
			unsigned short CMPCHA107 : 1;
			unsigned short CMPCHA108 : 1;
			unsigned short CMPCHA109 : 1;
			unsigned short CMPCHA110 : 1;
			unsigned short CMPCHA111 : 1;
			unsigned short CMPCHA112 : 1;
			unsigned short CMPCHA113 : 1;
			unsigned short CMPCHA114 : 1;
			unsigned short CMPCHA115 : 1;
#else
			unsigned short CMPCHA115 : 1;
			unsigned short CMPCHA114 : 1;
			unsigned short CMPCHA113 : 1;
			unsigned short CMPCHA112 : 1;
			unsigned short CMPCHA111 : 1;
			unsigned short CMPCHA110 : 1;
			unsigned short CMPCHA109 : 1;
			unsigned short CMPCHA108 : 1;
			unsigned short CMPCHA107 : 1;
			unsigned short CMPCHA106 : 1;
			unsigned short CMPCHA105 : 1;
			unsigned short CMPCHA104 : 1;
			unsigned short CMPCHA103 : 1;
			unsigned short CMPCHA102 : 1;
			unsigned short CMPCHA101 : 1;
			unsigned short CMPCHA100 : 1;
#endif
	} BIT;
	} ADCMPANSR1;
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
			unsigned short CMPLCHA006 : 1;
			unsigned short CMPLCHA007 : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short CMPLCHA007 : 1;
			unsigned short CMPLCHA006 : 1;
			unsigned short CMPLCHA005 : 1;
			unsigned short CMPLCHA004 : 1;
			unsigned short CMPLCHA003 : 1;
			unsigned short CMPLCHA002 : 1;
			unsigned short CMPLCHA001 : 1;
			unsigned short CMPLCHA000 : 1;
#endif
	} BIT;
	} ADCMPLR0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMPLCHA100 : 1;
			unsigned short CMPLCHA101 : 1;
			unsigned short CMPLCHA102 : 1;
			unsigned short CMPLCHA103 : 1;
			unsigned short CMPLCHA104 : 1;
			unsigned short CMPLCHA105 : 1;
			unsigned short CMPLCHA106 : 1;
			unsigned short CMPLCHA107 : 1;
			unsigned short CMPLCHA108 : 1;
			unsigned short CMPLCHA109 : 1;
			unsigned short CMPLCHA110 : 1;
			unsigned short CMPLCHA111 : 1;
			unsigned short CMPLCHA112 : 1;
			unsigned short CMPLCHA113 : 1;
			unsigned short CMPLCHA114 : 1;
			unsigned short CMPLCHA115 : 1;
#else
			unsigned short CMPLCHA115 : 1;
			unsigned short CMPLCHA114 : 1;
			unsigned short CMPLCHA113 : 1;
			unsigned short CMPLCHA112 : 1;
			unsigned short CMPLCHA111 : 1;
			unsigned short CMPLCHA110 : 1;
			unsigned short CMPLCHA109 : 1;
			unsigned short CMPLCHA108 : 1;
			unsigned short CMPLCHA107 : 1;
			unsigned short CMPLCHA106 : 1;
			unsigned short CMPLCHA105 : 1;
			unsigned short CMPLCHA104 : 1;
			unsigned short CMPLCHA103 : 1;
			unsigned short CMPLCHA102 : 1;
			unsigned short CMPLCHA101 : 1;
			unsigned short CMPLCHA100 : 1;
#endif
	} BIT;
	} ADCMPLR1;
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
			unsigned short CMPSTCHA006 : 1;
			unsigned short CMPSTCHA007 : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short CMPSTCHA007 : 1;
			unsigned short CMPSTCHA006 : 1;
			unsigned short CMPSTCHA005 : 1;
			unsigned short CMPSTCHA004 : 1;
			unsigned short CMPSTCHA003 : 1;
			unsigned short CMPSTCHA002 : 1;
			unsigned short CMPSTCHA001 : 1;
			unsigned short CMPSTCHA000 : 1;
#endif
	} BIT;
	} ADCMPSR0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CMPSTCHA100 : 1;
			unsigned short CMPSTCHA101 : 1;
			unsigned short CMPSTCHA102 : 1;
			unsigned short CMPSTCHA103 : 1;
			unsigned short CMPSTCHA104 : 1;
			unsigned short CMPSTCHA105 : 1;
			unsigned short CMPSTCHA106 : 1;
			unsigned short CMPSTCHA107 : 1;
			unsigned short CMPSTCHA108 : 1;
			unsigned short CMPSTCHA109 : 1;
			unsigned short CMPSTCHA110 : 1;
			unsigned short CMPSTCHA111 : 1;
			unsigned short CMPSTCHA112 : 1;
			unsigned short CMPSTCHA113 : 1;
			unsigned short CMPSTCHA114 : 1;
			unsigned short CMPSTCHA115 : 1;
#else
			unsigned short CMPSTCHA115 : 1;
			unsigned short CMPSTCHA114 : 1;
			unsigned short CMPSTCHA113 : 1;
			unsigned short CMPSTCHA112 : 1;
			unsigned short CMPSTCHA111 : 1;
			unsigned short CMPSTCHA110 : 1;
			unsigned short CMPSTCHA109 : 1;
			unsigned short CMPSTCHA108 : 1;
			unsigned short CMPSTCHA107 : 1;
			unsigned short CMPSTCHA106 : 1;
			unsigned short CMPSTCHA105 : 1;
			unsigned short CMPSTCHA104 : 1;
			unsigned short CMPSTCHA103 : 1;
			unsigned short CMPSTCHA102 : 1;
			unsigned short CMPSTCHA101 : 1;
			unsigned short CMPSTCHA100 : 1;
#endif
	} BIT;
	} ADCMPSR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMPSTTSA : 1;
			unsigned char CMPSTOCA : 1;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char CMPSTOCA : 1;
			unsigned char CMPSTTSA : 1;
#endif
	} BIT;
	} ADCMPSER;
	char           wk9[1];
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
	char           wk10[1];
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
	char           wk11[3];
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
	char           wk12[1];
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
	char           wk13[10];
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
};

struct st_sci0 {
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
};

struct st_sci12 {
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
};

struct st_smci {
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
};

struct st_smci1 {
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
};

struct st_system {
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
			unsigned long  : 1;
			unsigned long MSTPA17 : 1;
			unsigned long  : 1;
			unsigned long MSTPA19 : 1;
			unsigned long  : 8;
			unsigned long MSTPA28 : 1;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long MSTPA28 : 1;
			unsigned long  : 8;
			unsigned long MSTPA19 : 1;
			unsigned long  : 1;
			unsigned long MSTPA17 : 1;
			unsigned long  : 1;
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
			unsigned long  : 4;
			unsigned long MSTPB4 : 1;
			unsigned long  : 1;
			unsigned long MSTPB6 : 1;
			unsigned long  : 2;
			unsigned long MSTPB9 : 1;
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
			unsigned long MSTPB31 : 1;
#else
			unsigned long MSTPB31 : 1;
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
			unsigned long MSTPB9 : 1;
			unsigned long  : 2;
			unsigned long MSTPB6 : 1;
			unsigned long  : 1;
			unsigned long MSTPB4 : 1;
			unsigned long  : 4;
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
			unsigned long  : 6;
			unsigned long MSTPC26 : 1;
			unsigned long MSTPC27 : 1;
			unsigned long MSTPC28 : 1;
			unsigned long MSTPC29 : 1;
			unsigned long  : 1;
			unsigned long DSLPE : 1;
#else
			unsigned long DSLPE : 1;
			unsigned long  : 1;
			unsigned long MSTPC29 : 1;
			unsigned long MSTPC28 : 1;
			unsigned long MSTPC27 : 1;
			unsigned long MSTPC26 : 1;
			unsigned long  : 6;
			unsigned long MSTPC19 : 1;
			unsigned long  : 18;
			unsigned long MSTPC0 : 1;
#endif
	} BIT;
	} MSTPCRC;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 10;
			unsigned long MSTPD10 : 1;
			unsigned long  : 21;
#else
			unsigned long  : 21;
			unsigned long MSTPD10 : 1;
			unsigned long  : 10;
#endif
	} BIT;
	} MSTPCRD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long PCKD : 4;
			unsigned long  : 4;
			unsigned long PCKB : 4;
			unsigned long  : 12;
			unsigned long ICK : 4;
			unsigned long FCK : 4;
#else
			unsigned long FCK : 4;
			unsigned long ICK : 4;
			unsigned long  : 12;
			unsigned long PCKB : 4;
			unsigned long  : 4;
			unsigned long PCKD : 4;
#endif
	} BIT;
	} SCKCR;
	char           wk3[2];
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
	char           wk4[7];
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SOSTP : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char SOSTP : 1;
#endif
	} BIT;
	} SOSCCR;
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
	char           wk5[5];
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HOFXIN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char HOFXIN : 1;
#endif
	} BIT;
	} HOFCR;
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
	char           wk6[30];
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
	char           wk7[3];
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
	char           wk8[3];
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
	char           wk9[55];
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RSTCKSEL : 3;
			unsigned char  : 4;
			unsigned char RSTCKEN : 1;
#else
			unsigned char RSTCKEN : 1;
			unsigned char  : 4;
			unsigned char RSTCKSEL : 3;
#endif
	} BIT;
	} RSTCKCR;
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
	char           wk10[7];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SOPCM : 1;
			unsigned char  : 3;
			unsigned char SOPCMTSF : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char SOPCMTSF : 1;
			unsigned char  : 3;
			unsigned char SOPCM : 1;
#endif
	} BIT;
	} SOPCCR;
	char           wk11[21];
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
	char           wk12[1];
	unsigned short SWRR;
	char           wk13[28];
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
	char           wk14[794];
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
	char           wk15[48784];
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
	char           wk16[1];
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
	char           wk17[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char  : 3;
			unsigned char EXVCCINP2 : 1;
			unsigned char  : 1;
			unsigned char LVD1E : 1;
			unsigned char LVD2E : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char LVD2E : 1;
			unsigned char LVD1E : 1;
			unsigned char  : 1;
			unsigned char EXVCCINP2 : 1;
			unsigned char  : 3;
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
	char           wk18[1];
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
};

struct st_temps {
	unsigned char  TSCDRL;
	unsigned char  TSCDRH;
};

struct st_tmr0 {
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
};

struct st_tmr1 {
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
};

struct st_tmr01 {
	unsigned short TCORA;
	unsigned short TCORB;
	unsigned short TCNT;
	unsigned short TCCR;
};

enum enum_ir {
IR_BSC_BUSERR=16,IR_FCU_FRDYI=23,
IR_ICU_SWINT=27,
IR_CMT0_CMI0,
IR_CMT1_CMI1,
IR_CAC_FERRF=32,IR_CAC_MENDF,IR_CAC_OVFF,
IR_RSPI0_SPEI0=44,IR_RSPI0_SPRI0,IR_RSPI0_SPTI0,IR_RSPI0_SPII0,
IR_DOC_DOPCF=57,
IR_CMPB_CMPB0,IR_CMPB_CMPB1,
IR_CTSU_CTSUWR,IR_CTSU_CTSURD,IR_CTSU_CTSUFN,
IR_RTC_CUP,
IR_ICU_IRQ0,IR_ICU_IRQ1,IR_ICU_IRQ2,IR_ICU_IRQ3,IR_ICU_IRQ4,IR_ICU_IRQ5,IR_ICU_IRQ6,IR_ICU_IRQ7,
IR_ELC_ELSR8I=80,
IR_LVD_LVD1=88,IR_LVD_LVD2,
IR_RTC_ALM=92,IR_RTC_PRD,
IR_REMC0_REMCI0,
IR_REMC1_REMCI1,
IR_S12AD_S12ADI0=102,IR_S12AD_GBADI,
IR_ELC_ELSR18I=106,
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
IR_SCI0_ERI0=214,IR_SCI0_RXI0,IR_SCI0_TXI0,IR_SCI0_TEI0,
IR_SCI1_ERI1,IR_SCI1_RXI1,IR_SCI1_TXI1,IR_SCI1_TEI1,
IR_SCI5_ERI5,IR_SCI5_RXI5,IR_SCI5_TXI5,IR_SCI5_TEI5,
IR_SCI6_ERI6,IR_SCI6_RXI6,IR_SCI6_TXI6,IR_SCI6_TEI6,
IR_SCI8_ERI8,IR_SCI8_RXI8,IR_SCI8_TXI8,IR_SCI8_TEI8,
IR_SCI9_ERI9,IR_SCI9_RXI9,IR_SCI9_TXI9,IR_SCI9_TEI9,
IR_SCI12_ERI12,IR_SCI12_RXI12,IR_SCI12_TXI12,IR_SCI12_TEI12,IR_SCI12_SCIX0,IR_SCI12_SCIX1,IR_SCI12_SCIX2,IR_SCI12_SCIX3,
IR_RIIC0_EEI0,IR_RIIC0_RXI0,IR_RIIC0_TXI0,IR_RIIC0_TEI0
};

enum enum_dtce {
DTCE_ICU_SWINT=27,
DTCE_CMT0_CMI0,
DTCE_CMT1_CMI1,
DTCE_RSPI0_SPRI0=45,DTCE_RSPI0_SPTI0,
DTCE_CMPB_CMPB0=58,DTCE_CMPB_CMPB1,
DTCE_CTSU_CTSUWR,DTCE_CTSU_CTSURD,
DTCE_ICU_IRQ0=64,DTCE_ICU_IRQ1,DTCE_ICU_IRQ2,DTCE_ICU_IRQ3,DTCE_ICU_IRQ4,DTCE_ICU_IRQ5,DTCE_ICU_IRQ6,DTCE_ICU_IRQ7,
DTCE_S12AD_S12ADI0=102,DTCE_S12AD_GBADI,
DTCE_ELC_ELSR18I=106,
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
DTCE_SCI0_RXI0=215,DTCE_SCI0_TXI0,
DTCE_SCI1_RXI1=219,DTCE_SCI1_TXI1,
DTCE_SCI5_RXI5=223,DTCE_SCI5_TXI5,
DTCE_SCI6_RXI6=227,DTCE_SCI6_TXI6,
DTCE_SCI8_RXI8=231,DTCE_SCI8_TXI8,
DTCE_SCI9_RXI9=235,DTCE_SCI9_TXI9,
DTCE_SCI12_RXI12=239,DTCE_SCI12_TXI12,
DTCE_RIIC0_RXI0=247,DTCE_RIIC0_TXI0
};

enum enum_ier {
IER_BSC_BUSERR=0x02,
IER_FCU_FRDYI=0x02,
IER_ICU_SWINT=0x03,
IER_CMT0_CMI0=0x03,
IER_CMT1_CMI1=0x03,
IER_CAC_FERRF=0x04,IER_CAC_MENDF=0x04,IER_CAC_OVFF=0x04,
IER_RSPI0_SPEI0=0x05,IER_RSPI0_SPRI0=0x05,IER_RSPI0_SPTI0=0x05,IER_RSPI0_SPII0=0x05,
IER_DOC_DOPCF=0x07,
IER_CMPB_CMPB0=0x07,IER_CMPB_CMPB1=0x07,
IER_CTSU_CTSUWR=0x07,IER_CTSU_CTSURD=0x07,IER_CTSU_CTSUFN=0x07,
IER_RTC_CUP=0x07,
IER_ICU_IRQ0=0x08,IER_ICU_IRQ1=0x08,IER_ICU_IRQ2=0x08,IER_ICU_IRQ3=0x08,IER_ICU_IRQ4=0x08,IER_ICU_IRQ5=0x08,IER_ICU_IRQ6=0x08,IER_ICU_IRQ7=0x08,
IER_ELC_ELSR8I=0x0A,
IER_LVD_LVD1=0x0B,IER_LVD_LVD2=0x0B,
IER_RTC_ALM=0x0B,IER_RTC_PRD=0x0B,
IER_REMC0_REMCI0=0x0B,
IER_REMC1_REMCI1=0x0B,
IER_S12AD_S12ADI0=0x0C,IER_S12AD_GBADI=0x0C,
IER_ELC_ELSR18I=0x0D,
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
IER_SCI0_ERI0=0x1A,IER_SCI0_RXI0=0x1A,IER_SCI0_TXI0=0x1B,IER_SCI0_TEI0=0x1B,
IER_SCI1_ERI1=0x1B,IER_SCI1_RXI1=0x1B,IER_SCI1_TXI1=0x1B,IER_SCI1_TEI1=0x1B,
IER_SCI5_ERI5=0x1B,IER_SCI5_RXI5=0x1B,IER_SCI5_TXI5=0x1C,IER_SCI5_TEI5=0x1C,
IER_SCI6_ERI6=0x1C,IER_SCI6_RXI6=0x1C,IER_SCI6_TXI6=0x1C,IER_SCI6_TEI6=0x1C,
IER_SCI8_ERI8=0x1C,IER_SCI8_RXI8=0x1C,IER_SCI8_TXI8=0x1D,IER_SCI8_TEI8=0x1D,
IER_SCI9_ERI9=0x1D,IER_SCI9_RXI9=0x1D,IER_SCI9_TXI9=0x1D,IER_SCI9_TEI9=0x1D,
IER_SCI12_ERI12=0x1D,IER_SCI12_RXI12=0x1D,IER_SCI12_TXI12=0x1E,IER_SCI12_TEI12=0x1E,IER_SCI12_SCIX0=0x1E,IER_SCI12_SCIX1=0x1E,IER_SCI12_SCIX2=0x1E,IER_SCI12_SCIX3=0x1E,
IER_RIIC0_EEI0=0x1E,IER_RIIC0_RXI0=0x1E,IER_RIIC0_TXI0=0x1F,IER_RIIC0_TEI0=0x1F
};

enum enum_ipr {
IPR_BSC_BUSERR=0,
IPR_FCU_FRDYI=2,
IPR_ICU_SWINT=3,
IPR_CMT0_CMI0=4,
IPR_CMT1_CMI1=5,
IPR_CAC_FERRF=32,IPR_CAC_MENDF=33,IPR_CAC_OVFF=34,
IPR_RSPI0_SPEI0=44,IPR_RSPI0_SPRI0=44,IPR_RSPI0_SPTI0=44,IPR_RSPI0_SPII0=44,
IPR_DOC_DOPCF=57,
IPR_CMPB_CMPB0=58,IPR_CMPB_CMPB1=59,
IPR_CTSU_CTSUWR=60,IPR_CTSU_CTSURD=60,IPR_CTSU_CTSUFN=60,
IPR_RTC_CUP=63,
IPR_ICU_IRQ0=64,IPR_ICU_IRQ1=65,IPR_ICU_IRQ2=66,IPR_ICU_IRQ3=67,IPR_ICU_IRQ4=68,IPR_ICU_IRQ5=69,IPR_ICU_IRQ6=70,IPR_ICU_IRQ7=71,
IPR_ELC_ELSR8I=80,
IPR_LVD_LVD1=88,IPR_LVD_LVD2=89,
IPR_RTC_ALM=92,IPR_RTC_PRD=93,
IPR_REMC0_REMCI0=94,
IPR_REMC1_REMCI1=95,
IPR_S12AD_S12ADI0=102,IPR_S12AD_GBADI=103,
IPR_ELC_ELSR18I=106,
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
IPR_SCI0_ERI0=214,IPR_SCI0_RXI0=214,IPR_SCI0_TXI0=214,IPR_SCI0_TEI0=214,
IPR_SCI1_ERI1=218,IPR_SCI1_RXI1=218,IPR_SCI1_TXI1=218,IPR_SCI1_TEI1=218,
IPR_SCI5_ERI5=222,IPR_SCI5_RXI5=222,IPR_SCI5_TXI5=222,IPR_SCI5_TEI5=222,
IPR_SCI6_ERI6=226,IPR_SCI6_RXI6=226,IPR_SCI6_TXI6=226,IPR_SCI6_TEI6=226,
IPR_SCI8_ERI8=230,IPR_SCI8_RXI8=230,IPR_SCI8_TXI8=230,IPR_SCI8_TEI8=230,
IPR_SCI9_ERI9=234,IPR_SCI9_RXI9=234,IPR_SCI9_TXI9=234,IPR_SCI9_TEI9=234,
IPR_SCI12_ERI12=238,IPR_SCI12_RXI12=238,IPR_SCI12_TXI12=238,IPR_SCI12_TEI12=238,IPR_SCI12_SCIX0=242,IPR_SCI12_SCIX1=243,IPR_SCI12_SCIX2=244,IPR_SCI12_SCIX3=245,
IPR_RIIC0_EEI0=246,IPR_RIIC0_RXI0=247,IPR_RIIC0_TXI0=248,IPR_RIIC0_TEI0=249,
IPR_BSC_=0,
IPR_FCU_=2,
IPR_CMT0_=4,
IPR_CMT1_=5,
IPR_RSPI0_=44,
IPR_DOC_=57,
IPR_CTSU_=60,
IPR_REMC0_=94,
IPR_REMC1_=95,
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
IPR_SCI0_=214,
IPR_SCI1_=218,
IPR_SCI5_=222,
IPR_SCI6_=226,
IPR_SCI8_=230,
IPR_SCI9_=234
};

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
#define	IEN_DOC_DOPCF		IEN1
#define	IEN_CMPB_CMPB0		IEN2
#define	IEN_CMPB_CMPB1		IEN3
#define	IEN_CTSU_CTSUWR		IEN4
#define	IEN_CTSU_CTSURD		IEN5
#define	IEN_CTSU_CTSUFN		IEN6
#define	IEN_RTC_CUP			IEN7
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
#define	IEN_RTC_ALM			IEN4
#define	IEN_RTC_PRD			IEN5
#define	IEN_REMC0_REMCI0	IEN6
#define	IEN_REMC1_REMCI1	IEN7
#define	IEN_S12AD_S12ADI0	IEN6
#define	IEN_S12AD_GBADI		IEN7
#define	IEN_ELC_ELSR18I		IEN2
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
#define	IEN_SCI0_ERI0		IEN6
#define	IEN_SCI0_RXI0		IEN7
#define	IEN_SCI0_TXI0		IEN0
#define	IEN_SCI0_TEI0		IEN1
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
#define	IEN_SCI8_ERI8		IEN6
#define	IEN_SCI8_RXI8		IEN7
#define	IEN_SCI8_TXI8		IEN0
#define	IEN_SCI8_TEI8		IEN1
#define	IEN_SCI9_ERI9		IEN2
#define	IEN_SCI9_RXI9		IEN3
#define	IEN_SCI9_TXI9		IEN4
#define	IEN_SCI9_TEI9		IEN5
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
#define	VECT_DOC_DOPCF		57
#define	VECT_CMPB_CMPB0		58
#define	VECT_CMPB_CMPB1		59
#define	VECT_CTSU_CTSUWR	60
#define	VECT_CTSU_CTSURD	61
#define	VECT_CTSU_CTSUFN	62
#define	VECT_RTC_CUP		63
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
#define	VECT_RTC_ALM		92
#define	VECT_RTC_PRD		93
#define	VECT_REMC0_REMCI0	94
#define	VECT_REMC1_REMCI1	95
#define	VECT_S12AD_S12ADI0	102
#define	VECT_S12AD_GBADI	103
#define	VECT_ELC_ELSR18I	106
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
#define	VECT_SCI0_ERI0		214
#define	VECT_SCI0_RXI0		215
#define	VECT_SCI0_TXI0		216
#define	VECT_SCI0_TEI0		217
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
#define	VECT_SCI8_ERI8		230
#define	VECT_SCI8_RXI8		231
#define	VECT_SCI8_TXI8		232
#define	VECT_SCI8_TEI8		233
#define	VECT_SCI9_ERI9		234
#define	VECT_SCI9_RXI9		235
#define	VECT_SCI9_TXI9		236
#define	VECT_SCI9_TEI9		237
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

#define	MSTP_DTC	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DA		SYSTEM.MSTPCRA.BIT.MSTPA19
#define	MSTP_S12AD	SYSTEM.MSTPCRA.BIT.MSTPA17
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
#define	MSTP_SCI0	SYSTEM.MSTPCRB.BIT.MSTPB31
#define	MSTP_SMCI0	SYSTEM.MSTPCRB.BIT.MSTPB31
#define	MSTP_SCI1	SYSTEM.MSTPCRB.BIT.MSTPB30
#define	MSTP_SMCI1	SYSTEM.MSTPCRB.BIT.MSTPB30
#define	MSTP_SCI5	SYSTEM.MSTPCRB.BIT.MSTPB26
#define	MSTP_SMCI5	SYSTEM.MSTPCRB.BIT.MSTPB26
#define	MSTP_SCI6	SYSTEM.MSTPCRB.BIT.MSTPB25
#define	MSTP_SMCI6	SYSTEM.MSTPCRB.BIT.MSTPB25
#define	MSTP_CRC	SYSTEM.MSTPCRB.BIT.MSTPB23
#define	MSTP_RIIC0	SYSTEM.MSTPCRB.BIT.MSTPB21
#define	MSTP_RSPI0	SYSTEM.MSTPCRB.BIT.MSTPB17
#define	MSTP_CMPB	SYSTEM.MSTPCRB.BIT.MSTPB10
#define	MSTP_ELC	SYSTEM.MSTPCRB.BIT.MSTPB9
#define	MSTP_DOC	SYSTEM.MSTPCRB.BIT.MSTPB6
#define	MSTP_SCI12	SYSTEM.MSTPCRB.BIT.MSTPB4
#define	MSTP_SMCI12	SYSTEM.MSTPCRB.BIT.MSTPB4
#define	MSTP_REMC0	SYSTEM.MSTPCRC.BIT.MSTPC29
#define	MSTP_REMC1	SYSTEM.MSTPCRC.BIT.MSTPC28
#define	MSTP_SCI8	SYSTEM.MSTPCRC.BIT.MSTPC27
#define	MSTP_SMCI8	SYSTEM.MSTPCRC.BIT.MSTPC27
#define	MSTP_SCI9	SYSTEM.MSTPCRC.BIT.MSTPC26
#define	MSTP_SMCI9	SYSTEM.MSTPCRC.BIT.MSTPC26
#define	MSTP_CAC	SYSTEM.MSTPCRC.BIT.MSTPC19
#define	MSTP_RAM0	SYSTEM.MSTPCRC.BIT.MSTPC0
#define	MSTP_CTSU	SYSTEM.MSTPCRD.BIT.MSTPD10

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

#define	BSC		(*(volatile struct st_bsc     *)0x81300)
#define	CAC		(*(volatile struct st_cac     *)0x8B000)
#define	CMPB	(*(volatile struct st_cmpb    *)0x8C580)
#define	CMT		(*(volatile struct st_cmt     *)0x88000)
#define	CMT0	(*(volatile struct st_cmt0    *)0x88002)
#define	CMT1	(*(volatile struct st_cmt0    *)0x88008)
#define	CRC		(*(volatile struct st_crc     *)0x88280)
#define	CTSU	(*(volatile struct st_ctsu    *)0xA0900)
#define	DA		(*(volatile struct st_da      *)0x880C0)
#define	DOC		(*(volatile struct st_doc     *)0x8B080)
#define	DTC		(*(volatile struct st_dtc     *)0x82400)
#define	ELC		(*(volatile struct st_elc     *)0x8B100)
#define	FLASH	(*(volatile struct st_flash   *)0x7FC090)
#define	ICU		(*(volatile struct st_icu     *)0x87000)
#define	IWDT	(*(volatile struct st_iwdt    *)0x88030)
#define	LPT		(*(volatile struct st_lpt     *)0x800B0)
#define	MPC		(*(volatile struct st_mpc     *)0x8C11F)
#define	MTU		(*(volatile struct st_mtu     *)0x8860A)
#define	MTU0	(*(volatile struct st_mtu0    *)0x88690)
#define	MTU1	(*(volatile struct st_mtu1    *)0x88690)
#define	MTU2	(*(volatile struct st_mtu2    *)0x88692)
#define	MTU3	(*(volatile struct st_mtu3    *)0x88600)
#define	MTU4	(*(volatile struct st_mtu4    *)0x88600)
#define	MTU5	(*(volatile struct st_mtu5    *)0x88694)
#define	POE		(*(volatile struct st_poe     *)0x88900)
#define	PORT	(*(volatile struct st_port    *)0x8C120)
#define	PORT0	(*(volatile struct st_port0   *)0x8C000)
#define	PORT1	(*(volatile struct st_port1   *)0x8C001)
#define	PORT2	(*(volatile struct st_port2   *)0x8C002)
#define	PORT3	(*(volatile struct st_port3   *)0x8C003)
#define	PORT4	(*(volatile struct st_port4   *)0x8C004)
#define	PORT5	(*(volatile struct st_port5   *)0x8C005)
#define	PORTA	(*(volatile struct st_porta   *)0x8C00A)
#define	PORTB	(*(volatile struct st_portb   *)0x8C00B)
#define	PORTC	(*(volatile struct st_portc   *)0x8C00C)
#define	PORTD	(*(volatile struct st_portd   *)0x8C00D)
#define	PORTE	(*(volatile struct st_porte   *)0x8C00E)
#define	PORTH	(*(volatile struct st_porth   *)0x8C011)
#define	PORTJ	(*(volatile struct st_portj   *)0x8C012)
#define	REMC0	(*(volatile struct st_remc    *)0xA0B00)
#define	REMC1	(*(volatile struct st_remc    *)0xA0B80)
#define	REMCOM	(*(volatile struct st_remcom  *)0xA0C00)
#define	RIIC0	(*(volatile struct st_riic    *)0x88300)
#define	RSPI0	(*(volatile struct st_rspi    *)0x88380)
#define	RTC		(*(volatile struct st_rtc     *)0x8C400)
#define	RTCB	(*(volatile struct st_rtcb    *)0x8C402)
#define	S12AD	(*(volatile struct st_s12ad   *)0x89000)
#define	SCI0	(*(volatile struct st_sci0    *)0x8A000)
#define	SCI1	(*(volatile struct st_sci0    *)0x8A020)
#define	SCI5	(*(volatile struct st_sci0    *)0x8A0A0)
#define	SCI6	(*(volatile struct st_sci0    *)0x8A0C0)
#define	SCI8	(*(volatile struct st_sci0    *)0x8A100)
#define	SCI9	(*(volatile struct st_sci0    *)0x8A120)
#define	SCI12	(*(volatile struct st_sci12   *)0x8B300)
#define	SMCI0	(*(volatile struct st_smci   *)0x8A000)
#define	SMCI1	(*(volatile struct st_smci1   *)0x8A020)
#define	SMCI5	(*(volatile struct st_smci   *)0x8A0A0)
#define	SMCI6	(*(volatile struct st_smci1   *)0x8A0C0)
#define	SMCI8	(*(volatile struct st_smci   *)0x8A100)
#define	SMCI9	(*(volatile struct st_smci1   *)0x8A120)
#define	SMCI12	(*(volatile struct st_smci1   *)0x8B300)
#define	SYSTEM	(*(volatile struct st_system  *)0x80000)
#define	TEMPS	(*(volatile struct st_temps   *)0x7FC0AC)
#define	TMR0	(*(volatile struct st_tmr0    *)0x88200)
#define	TMR1	(*(volatile struct st_tmr1    *)0x88201)
#define	TMR2	(*(volatile struct st_tmr0    *)0x88210)
#define	TMR3	(*(volatile struct st_tmr1    *)0x88211)
#define	TMR01	(*(volatile struct st_tmr01   *)0x88204)
#define	TMR23	(*(volatile struct st_tmr01   *)0x88214)

#pragma pack()
#endif
