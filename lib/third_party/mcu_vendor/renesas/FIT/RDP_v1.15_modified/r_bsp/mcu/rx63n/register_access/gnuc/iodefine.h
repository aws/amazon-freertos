                                                                          
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                          
/********************************************************************************
*
* Device     : RX/RX600/RX63N,RX631
*
* File Name  : iodefine.h
*
* Abstract   : Definition of I/O Register.
*
* History    : 0.02  (2010-11-01)  [Hardware Manual Revision : 0.01]
*            : 0.50  (2011-03-28)  [Hardware Manual Revision : 0.50]
*            : 1.0D  (2012-06-17)  [Hardware Manual Revision : 1.00]
*            : 1.5   (2012-11-24)  [Hardware Manual Revision : 1.50]
*            : 1.6   (2013-05-01)  [Hardware Manual Revision : 1.60]
*            : 1.6A  (2013-07-08)  [Hardware Manual Revision : 1.60]
*            : 1.8   (2015-04-22)  [Hardware Manual Revision : 1.80]
*            : 1.8A  (2015-12-10)  [Hardware Manual Revision : 1.80]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2015 (2010 - 2013) Renesas Electronics Corporation.
*
*********************************************************************************/
/********************************************************************************/
/*                                                                              */
/*  DESCRIPTION : Definition of ICU Register                                    */
/*  CPU TYPE    : RX63N                                                         */
/*                                                                              */
/*  Usage : IR,DTCER,IER,IPR of ICU Register                                    */
/*     The following IR, DTCE, IEN, IPR macro functions simplify usage.         */
/*     The bit access operation is "Bit_Name(interrupt source,name)".           */
/*     A part of the name can be omitted.                                       */
/*     for example :                                                            */
/*       IR(TPU0,TGI0A) = 0;     expands to :                                   */
/*         ICU.IR[126].BIT.IR = 0;                                              */
/*                                                                              */
/*       DTCE(ICU,IRQ0) = 1;     expands to :                                   */
/*         ICU.DTCER[64].BIT.DTCE = 1;                                          */
/*                                                                              */
/*       IEN(CMT0,CMI0) = 1;     expands to :                                   */
/*         ICU.IER[0x03].BIT.IEN4 = 1;                                          */
/*                                                                              */
/*       IPR(SCI12,SCIX0) = 2;   expands to :                                   */
/*       IPR(SCI12,SCI  ) = 2;    // SCIX0,SCIX1,SCIX2,SCIX3 share IPR level.   */
/*         ICU.IPR[122].BIT.IPR = 2;                                            */
/*                                                                              */
/*       IPR(SCI0,RXI0) = 3;     expands to :                                   */
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
/*         #pragma interrupt INT_MTU0_TGIA0(vect=142)                           */
/*       #pragma interrupt INT_TPU0_TGI0A(vect=VECT(TPU0,TGI0A))  expands to :  */
/*         #pragma interrupt INT_TPU0_TGI0A(vect=126)                           */
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
/*       MSTP(TPU4) = 0;    // TPU0,TPU1,TPU2,TPU3,TPU4,TPU5      expands to :  */
/*         SYSTEM.MSTPCRA.BIT.MSTPA13 = 0;                                      */
/*       MSTP(CMT3) = 0;    // CMT2,CMT3                          expands to :  */
/*         SYSTEM.MSTPCRA.BIT.MSTPA14 = 0;                                      */
/*                                                                              */
/*                                                                              */
/********************************************************************************/
#ifndef __RX63NIODEFINE_HEADER__
#define __RX63NIODEFINE_HEADER__

#pragma pack(4)

struct st_ad {
	unsigned short ADDRA;
	unsigned short ADDRB;
	unsigned short ADDRC;
	unsigned short ADDRD;
	unsigned short ADDRE;
	unsigned short ADDRF;
	unsigned short ADDRG;
	unsigned short ADDRH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CH : 3;
			unsigned char  : 2;
			unsigned char ADST : 1;
			unsigned char ADIE : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char ADIE : 1;
			unsigned char ADST : 1;
			unsigned char  : 2;
			unsigned char CH : 3;
#endif
	} BIT;
	} ADCSR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MODE : 2;
			unsigned char CKS : 2;
			unsigned char  : 1;
			unsigned char TRGS : 3;
#else
			unsigned char TRGS : 3;
			unsigned char  : 1;
			unsigned char CKS : 2;
			unsigned char MODE : 2;
#endif
	} BIT;
	} ADCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 4;
			unsigned char EXSEL : 2;
			unsigned char EXOEN : 1;
			unsigned char DPSEL : 1;
#else
			unsigned char DPSEL : 1;
			unsigned char EXOEN : 1;
			unsigned char EXSEL : 2;
			unsigned char : 4;
#endif
	} BIT;
	} ADCR2;
	unsigned char  ADSSTR;
	char           wk0[11];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DIAG : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char DIAG : 2;
#endif
	} BIT;
	} ADDIAGR;
};

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
			unsigned short : 3;
			unsigned short ADDR : 13;
#else
			unsigned short ADDR : 13;
			unsigned short : 3;
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
			unsigned short BPEB : 2;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short BPEB : 2;
			unsigned short BPFB : 2;
			unsigned short BPHB : 2;
			unsigned short BPGB : 2;
			unsigned short BPIB : 2;
			unsigned short BPRO : 2;
			unsigned short BPRA : 2;
#endif
	} BIT;
	} BUSPRI;
	char           wk4[7408];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WRMOD : 1;
			unsigned short  : 2;
			unsigned short EWENB : 1;
			unsigned short  : 4;
			unsigned short PRENB : 1;
			unsigned short PWENB : 1;
			unsigned short  : 5;
			unsigned short PRMOD : 1;
#else
			unsigned short PRMOD : 1;
			unsigned short  : 5;
			unsigned short PWENB : 1;
			unsigned short PRENB : 1;
			unsigned short  : 4;
			unsigned short EWENB : 1;
			unsigned short  : 2;
			unsigned short WRMOD : 1;
#endif
	} BIT;
	} CS0MOD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSPWWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSWWAIT : 5;
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
			unsigned long CSWWAIT : 5;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPWWAIT : 3;
#endif
	} BIT;
	} CS0WCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSROFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long AWAIT : 2;
			unsigned long  : 2;
			unsigned long RDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long RDON : 3;
			unsigned long  : 2;
			unsigned long AWAIT : 2;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long CSROFF : 3;
#endif
	} BIT;
	} CS0WCR2;
	char           wk5[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WRMOD : 1;
			unsigned short  : 2;
			unsigned short EWENB : 1;
			unsigned short  : 4;
			unsigned short PRENB : 1;
			unsigned short PWENB : 1;
			unsigned short  : 5;
			unsigned short PRMOD : 1;
#else
			unsigned short PRMOD : 1;
			unsigned short  : 5;
			unsigned short PWENB : 1;
			unsigned short PRENB : 1;
			unsigned short  : 4;
			unsigned short EWENB : 1;
			unsigned short  : 2;
			unsigned short WRMOD : 1;
#endif
	} BIT;
	} CS1MOD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSPWWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSWWAIT : 5;
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
			unsigned long CSWWAIT : 5;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPWWAIT : 3;
#endif
	} BIT;
	} CS1WCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSROFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long AWAIT : 2;
			unsigned long  : 2;
			unsigned long RDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long RDON : 3;
			unsigned long  : 2;
			unsigned long AWAIT : 2;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long CSROFF : 3;
#endif
	} BIT;
	} CS1WCR2;
	char           wk6[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WRMOD : 1;
			unsigned short  : 2;
			unsigned short EWENB : 1;
			unsigned short  : 4;
			unsigned short PRENB : 1;
			unsigned short PWENB : 1;
			unsigned short  : 5;
			unsigned short PRMOD : 1;
#else
			unsigned short PRMOD : 1;
			unsigned short  : 5;
			unsigned short PWENB : 1;
			unsigned short PRENB : 1;
			unsigned short  : 4;
			unsigned short EWENB : 1;
			unsigned short  : 2;
			unsigned short WRMOD : 1;
#endif
	} BIT;
	} CS2MOD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSPWWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSWWAIT : 5;
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
			unsigned long CSWWAIT : 5;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPWWAIT : 3;
#endif
	} BIT;
	} CS2WCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSROFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long AWAIT : 2;
			unsigned long  : 2;
			unsigned long RDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long RDON : 3;
			unsigned long  : 2;
			unsigned long AWAIT : 2;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long CSROFF : 3;
#endif
	} BIT;
	} CS2WCR2;
	char           wk7[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WRMOD : 1;
			unsigned short  : 2;
			unsigned short EWENB : 1;
			unsigned short  : 4;
			unsigned short PRENB : 1;
			unsigned short PWENB : 1;
			unsigned short  : 5;
			unsigned short PRMOD : 1;
#else
			unsigned short PRMOD : 1;
			unsigned short  : 5;
			unsigned short PWENB : 1;
			unsigned short PRENB : 1;
			unsigned short  : 4;
			unsigned short EWENB : 1;
			unsigned short  : 2;
			unsigned short WRMOD : 1;
#endif
	} BIT;
	} CS3MOD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSPWWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSWWAIT : 5;
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
			unsigned long CSWWAIT : 5;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPWWAIT : 3;
#endif
	} BIT;
	} CS3WCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSROFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long AWAIT : 2;
			unsigned long  : 2;
			unsigned long RDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long RDON : 3;
			unsigned long  : 2;
			unsigned long AWAIT : 2;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long CSROFF : 3;
#endif
	} BIT;
	} CS3WCR2;
	char           wk8[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WRMOD : 1;
			unsigned short  : 2;
			unsigned short EWENB : 1;
			unsigned short  : 4;
			unsigned short PRENB : 1;
			unsigned short PWENB : 1;
			unsigned short  : 5;
			unsigned short PRMOD : 1;
#else
			unsigned short PRMOD : 1;
			unsigned short  : 5;
			unsigned short PWENB : 1;
			unsigned short PRENB : 1;
			unsigned short  : 4;
			unsigned short EWENB : 1;
			unsigned short  : 2;
			unsigned short WRMOD : 1;
#endif
	} BIT;
	} CS4MOD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSPWWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSWWAIT : 5;
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
			unsigned long CSWWAIT : 5;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPWWAIT : 3;
#endif
	} BIT;
	} CS4WCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSROFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long AWAIT : 2;
			unsigned long  : 2;
			unsigned long RDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long RDON : 3;
			unsigned long  : 2;
			unsigned long AWAIT : 2;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long CSROFF : 3;
#endif
	} BIT;
	} CS4WCR2;
	char           wk9[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WRMOD : 1;
			unsigned short  : 2;
			unsigned short EWENB : 1;
			unsigned short  : 4;
			unsigned short PRENB : 1;
			unsigned short PWENB : 1;
			unsigned short  : 5;
			unsigned short PRMOD : 1;
#else
			unsigned short PRMOD : 1;
			unsigned short  : 5;
			unsigned short PWENB : 1;
			unsigned short PRENB : 1;
			unsigned short  : 4;
			unsigned short EWENB : 1;
			unsigned short  : 2;
			unsigned short WRMOD : 1;
#endif
	} BIT;
	} CS5MOD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSPWWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSWWAIT : 5;
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
			unsigned long CSWWAIT : 5;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPWWAIT : 3;
#endif
	} BIT;
	} CS5WCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSROFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long AWAIT : 2;
			unsigned long  : 2;
			unsigned long RDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long RDON : 3;
			unsigned long  : 2;
			unsigned long AWAIT : 2;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long CSROFF : 3;
#endif
	} BIT;
	} CS5WCR2;
	char           wk10[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WRMOD : 1;
			unsigned short  : 2;
			unsigned short EWENB : 1;
			unsigned short  : 4;
			unsigned short PRENB : 1;
			unsigned short PWENB : 1;
			unsigned short  : 5;
			unsigned short PRMOD : 1;
#else
			unsigned short PRMOD : 1;
			unsigned short  : 5;
			unsigned short PWENB : 1;
			unsigned short PRENB : 1;
			unsigned short  : 4;
			unsigned short EWENB : 1;
			unsigned short  : 2;
			unsigned short WRMOD : 1;
#endif
	} BIT;
	} CS6MOD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSPWWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSWWAIT : 5;
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
			unsigned long CSWWAIT : 5;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPWWAIT : 3;
#endif
	} BIT;
	} CS6WCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSROFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long AWAIT : 2;
			unsigned long  : 2;
			unsigned long RDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long RDON : 3;
			unsigned long  : 2;
			unsigned long AWAIT : 2;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long CSROFF : 3;
#endif
	} BIT;
	} CS6WCR2;
	char           wk11[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short WRMOD : 1;
			unsigned short  : 2;
			unsigned short EWENB : 1;
			unsigned short  : 4;
			unsigned short PRENB : 1;
			unsigned short PWENB : 1;
			unsigned short  : 5;
			unsigned short PRMOD : 1;
#else
			unsigned short PRMOD : 1;
			unsigned short  : 5;
			unsigned short PWENB : 1;
			unsigned short PRENB : 1;
			unsigned short  : 4;
			unsigned short EWENB : 1;
			unsigned short  : 2;
			unsigned short WRMOD : 1;
#endif
	} BIT;
	} CS7MOD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSPWWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSWWAIT : 5;
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long CSRWAIT : 5;
			unsigned long  : 3;
			unsigned long CSWWAIT : 5;
			unsigned long  : 5;
			unsigned long CSPRWAIT : 3;
			unsigned long  : 5;
			unsigned long CSPWWAIT : 3;
#endif
	} BIT;
	} CS7WCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CSROFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long AWAIT : 2;
			unsigned long  : 2;
			unsigned long RDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long CSON : 3;
			unsigned long  : 1;
			unsigned long WDON : 3;
			unsigned long  : 1;
			unsigned long WRON : 3;
			unsigned long  : 1;
			unsigned long RDON : 3;
			unsigned long  : 2;
			unsigned long AWAIT : 2;
			unsigned long  : 1;
			unsigned long WDOFF : 3;
			unsigned long  : 1;
			unsigned long CSWOFF : 3;
			unsigned long  : 1;
			unsigned long CSROFF : 3;
#endif
	} BIT;
	} CS7WCR2;
	char           wk12[1926];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EXENB : 1;
			unsigned short  : 3;
			unsigned short BSIZE : 2;
			unsigned short  : 2;
			unsigned short EMODE : 1;
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
			unsigned short EMODE : 1;
			unsigned short  : 2;
			unsigned short BSIZE : 2;
			unsigned short  : 3;
			unsigned short EXENB : 1;
#endif
	} BIT;
	} CS0CR;
	char           wk13[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RRCV : 4;
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
			unsigned short RRCV : 4;
#endif
	} BIT;
	} CS0REC;
	char           wk14[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EXENB : 1;
			unsigned short  : 3;
			unsigned short BSIZE : 2;
			unsigned short  : 2;
			unsigned short EMODE : 1;
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
			unsigned short EMODE : 1;
			unsigned short  : 2;
			unsigned short BSIZE : 2;
			unsigned short  : 3;
			unsigned short EXENB : 1;
#endif
	} BIT;
	} CS1CR;
	char           wk15[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RRCV : 4;
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
			unsigned short RRCV : 4;
#endif
	} BIT;
	} CS1REC;
	char           wk16[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EXENB : 1;
			unsigned short  : 3;
			unsigned short BSIZE : 2;
			unsigned short  : 2;
			unsigned short EMODE : 1;
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
			unsigned short EMODE : 1;
			unsigned short  : 2;
			unsigned short BSIZE : 2;
			unsigned short  : 3;
			unsigned short EXENB : 1;
#endif
	} BIT;
	} CS2CR;
	char           wk17[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RRCV : 4;
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
			unsigned short RRCV : 4;
#endif
	} BIT;
	} CS2REC;
	char           wk18[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EXENB : 1;
			unsigned short  : 3;
			unsigned short BSIZE : 2;
			unsigned short  : 2;
			unsigned short EMODE : 1;
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
			unsigned short EMODE : 1;
			unsigned short  : 2;
			unsigned short BSIZE : 2;
			unsigned short  : 3;
			unsigned short EXENB : 1;
#endif
	} BIT;
	} CS3CR;
	char           wk19[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RRCV : 4;
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
			unsigned short RRCV : 4;
#endif
	} BIT;
	} CS3REC;
	char           wk20[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EXENB : 1;
			unsigned short  : 3;
			unsigned short BSIZE : 2;
			unsigned short  : 2;
			unsigned short EMODE : 1;
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
			unsigned short EMODE : 1;
			unsigned short  : 2;
			unsigned short BSIZE : 2;
			unsigned short  : 3;
			unsigned short EXENB : 1;
#endif
	} BIT;
	} CS4CR;
	char           wk21[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RRCV : 4;
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
			unsigned short RRCV : 4;
#endif
	} BIT;
	} CS4REC;
	char           wk22[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EXENB : 1;
			unsigned short  : 3;
			unsigned short BSIZE : 2;
			unsigned short  : 2;
			unsigned short EMODE : 1;
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
			unsigned short EMODE : 1;
			unsigned short  : 2;
			unsigned short BSIZE : 2;
			unsigned short  : 3;
			unsigned short EXENB : 1;
#endif
	} BIT;
	} CS5CR;
	char           wk23[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RRCV : 4;
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
			unsigned short RRCV : 4;
#endif
	} BIT;
	} CS5REC;
	char           wk24[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EXENB : 1;
			unsigned short  : 3;
			unsigned short BSIZE : 2;
			unsigned short  : 2;
			unsigned short EMODE : 1;
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
			unsigned short EMODE : 1;
			unsigned short  : 2;
			unsigned short BSIZE : 2;
			unsigned short  : 3;
			unsigned short EXENB : 1;
#endif
	} BIT;
	} CS6CR;
	char           wk25[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RRCV : 4;
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
			unsigned short RRCV : 4;
#endif
	} BIT;
	} CS6REC;
	char           wk26[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EXENB : 1;
			unsigned short  : 3;
			unsigned short BSIZE : 2;
			unsigned short  : 2;
			unsigned short EMODE : 1;
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short MPXEN : 1;
			unsigned short  : 3;
			unsigned short EMODE : 1;
			unsigned short  : 2;
			unsigned short BSIZE : 2;
			unsigned short  : 3;
			unsigned short EXENB : 1;
#endif
	} BIT;
	} CS7CR;
	char           wk27[6];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RRCV : 4;
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short WRCV : 4;
			unsigned short  : 4;
			unsigned short RRCV : 4;
#endif
	} BIT;
	} CS7REC;
	char           wk28[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RCVEN0 : 1;
			unsigned short RCVEN1 : 1;
			unsigned short RCVEN2 : 1;
			unsigned short RCVEN3 : 1;
			unsigned short RCVEN4 : 1;
			unsigned short RCVEN5 : 1;
			unsigned short RCVEN6 : 1;
			unsigned short RCVEN7 : 1;
			unsigned short RCVENM0 : 1;
			unsigned short RCVENM1 : 1;
			unsigned short RCVENM2 : 1;
			unsigned short RCVENM3 : 1;
			unsigned short RCVENM4 : 1;
			unsigned short RCVENM5 : 1;
			unsigned short RCVENM6 : 1;
			unsigned short RCVENM7 : 1;
#else
			unsigned short RCVENM7 : 1;
			unsigned short RCVENM6 : 1;
			unsigned short RCVENM5 : 1;
			unsigned short RCVENM4 : 1;
			unsigned short RCVENM3 : 1;
			unsigned short RCVENM2 : 1;
			unsigned short RCVENM1 : 1;
			unsigned short RCVENM0 : 1;
			unsigned short RCVEN7 : 1;
			unsigned short RCVEN6 : 1;
			unsigned short RCVEN5 : 1;
			unsigned short RCVEN4 : 1;
			unsigned short RCVEN3 : 1;
			unsigned short RCVEN2 : 1;
			unsigned short RCVEN1 : 1;
			unsigned short RCVEN0 : 1;
#endif
	} BIT;
	} CSRECEN;
	char           wk29[894];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char EXENB : 1;
			unsigned char  : 3;
			unsigned char BSIZE : 2;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char BSIZE : 2;
			unsigned char  : 3;
			unsigned char EXENB : 1;
#endif
	} BIT;
	} SDCCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char EMODE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char EMODE : 1;
#endif
	} BIT;
	} SDCMOD;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char BE : 1;
#endif
	} BIT;
	} SDAMOD;
	char           wk30[13];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SFEN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char SFEN : 1;
#endif
	} BIT;
	} SDSELF;
	char           wk31[3];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFC : 12;
			unsigned short REFW : 4;
#else
			unsigned short REFW : 4;
			unsigned short RFC : 12;
#endif
	} BIT;
	} SDRFCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RFEN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char RFEN : 1;
#endif
	} BIT;
	} SDRFEN;
	char           wk32[9];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char INIRQ : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char INIRQ : 1;
#endif
	} BIT;
	} SDICR;
	char           wk33[3];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ARFI : 4;
			unsigned short ARFC : 4;
			unsigned short PRC : 3;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short PRC : 3;
			unsigned short ARFC : 4;
			unsigned short ARFI : 4;
#endif
	} BIT;
	} SDIR;
	char           wk34[26];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MXC : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char MXC : 2;
#endif
	} BIT;
	} SDADR;
	char           wk35[3];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CL : 3;
			unsigned long  : 5;
			unsigned long WR : 1;
			unsigned long RP : 3;
			unsigned long RCD : 2;
			unsigned long  : 2;
			unsigned long RAS : 3;
			unsigned long  : 13;
#else
			unsigned long  : 13;
			unsigned long RAS : 3;
			unsigned long  : 2;
			unsigned long RCD : 2;
			unsigned long RP : 3;
			unsigned long WR : 1;
			unsigned long  : 5;
			unsigned long CL : 3;
#endif
	} BIT;
	} SDTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MR : 15;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short MR : 15;
#endif
	} BIT;
	} SDMOD;
	char           wk36[6];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MRSST : 1;
			unsigned char  : 2;
			unsigned char INIST : 1;
			unsigned char SRFST : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char SRFST : 1;
			unsigned char INIST : 1;
			unsigned char  : 2;
			unsigned char MRSST : 1;
#endif
	} BIT;
	} SDSR;
};

struct st_can {
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
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long EID : 18;
			unsigned long SID : 11;
			unsigned long  : 1;
			unsigned long RTR : 1;
			unsigned long IDE : 1;
#else
			unsigned long IDE : 1;
			unsigned long RTR : 1;
			unsigned long  : 1;
			unsigned long SID : 11;
			unsigned long EID : 18;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long EID : 18;
			unsigned long SID : 11;
			unsigned long  : 3;
#else
			unsigned long  : 3;
			unsigned long SID : 11;
			unsigned long EID : 18;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long EID : 18;
			unsigned long SID : 11;
			unsigned long  : 1;
			unsigned long RTR : 1;
			unsigned long IDE : 1;
#else
			unsigned long IDE : 1;
			unsigned long RTR : 1;
			unsigned long  : 1;
			unsigned long SID : 11;
			unsigned long EID : 18;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long EID : 18;
			unsigned long SID : 11;
			unsigned long  : 1;
			unsigned long RTR : 1;
			unsigned long IDE : 1;
#else
			unsigned long IDE : 1;
			unsigned long RTR : 1;
			unsigned long  : 1;
			unsigned long SID : 11;
			unsigned long EID : 18;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MB0 : 1;
			unsigned long MB1 : 1;
			unsigned long MB2 : 1;
			unsigned long MB3 : 1;
			unsigned long MB4 : 1;
			unsigned long MB5 : 1;
			unsigned long MB6 : 1;
			unsigned long MB7 : 1;
			unsigned long MB8 : 1;
			unsigned long MB9 : 1;
			unsigned long MB10 : 1;
			unsigned long MB11 : 1;
			unsigned long MB12 : 1;
			unsigned long MB13 : 1;
			unsigned long MB14 : 1;
			unsigned long MB15 : 1;
			unsigned long MB16 : 1;
			unsigned long MB17 : 1;
			unsigned long MB18 : 1;
			unsigned long MB19 : 1;
			unsigned long MB20 : 1;
			unsigned long MB21 : 1;
			unsigned long MB22 : 1;
			unsigned long MB23 : 1;
			unsigned long MB24 : 1;
			unsigned long MB25 : 1;
			unsigned long MB26 : 1;
			unsigned long MB27 : 1;
			unsigned long MB28 : 1;
			unsigned long MB29 : 1;
			unsigned long MB30 : 1;
			unsigned long MB31 : 1;
#else
			unsigned long MB31 : 1;
			unsigned long MB30 : 1;
			unsigned long MB29 : 1;
			unsigned long MB28 : 1;
			unsigned long MB27 : 1;
			unsigned long MB26 : 1;
			unsigned long MB25 : 1;
			unsigned long MB24 : 1;
			unsigned long MB23 : 1;
			unsigned long MB22 : 1;
			unsigned long MB21 : 1;
			unsigned long MB20 : 1;
			unsigned long MB19 : 1;
			unsigned long MB18 : 1;
			unsigned long MB17 : 1;
			unsigned long MB16 : 1;
			unsigned long MB15 : 1;
			unsigned long MB14 : 1;
			unsigned long MB13 : 1;
			unsigned long MB12 : 1;
			unsigned long MB11 : 1;
			unsigned long MB10 : 1;
			unsigned long MB9 : 1;
			unsigned long MB8 : 1;
			unsigned long MB7 : 1;
			unsigned long MB6 : 1;
			unsigned long MB5 : 1;
			unsigned long MB4 : 1;
			unsigned long MB3 : 1;
			unsigned long MB2 : 1;
			unsigned long MB1 : 1;
			unsigned long MB0 : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MB0 : 1;
			unsigned long MB1 : 1;
			unsigned long MB2 : 1;
			unsigned long MB3 : 1;
			unsigned long MB4 : 1;
			unsigned long MB5 : 1;
			unsigned long MB6 : 1;
			unsigned long MB7 : 1;
			unsigned long MB8 : 1;
			unsigned long MB9 : 1;
			unsigned long MB10 : 1;
			unsigned long MB11 : 1;
			unsigned long MB12 : 1;
			unsigned long MB13 : 1;
			unsigned long MB14 : 1;
			unsigned long MB15 : 1;
			unsigned long MB16 : 1;
			unsigned long MB17 : 1;
			unsigned long MB18 : 1;
			unsigned long MB19 : 1;
			unsigned long MB20 : 1;
			unsigned long MB21 : 1;
			unsigned long MB22 : 1;
			unsigned long MB23 : 1;
			unsigned long MB24 : 1;
			unsigned long MB25 : 1;
			unsigned long MB26 : 1;
			unsigned long MB27 : 1;
			unsigned long MB28 : 1;
			unsigned long MB29 : 1;
			unsigned long MB30 : 1;
			unsigned long MB31 : 1;
#else
			unsigned long MB31 : 1;
			unsigned long MB30 : 1;
			unsigned long MB29 : 1;
			unsigned long MB28 : 1;
			unsigned long MB27 : 1;
			unsigned long MB26 : 1;
			unsigned long MB25 : 1;
			unsigned long MB24 : 1;
			unsigned long MB23 : 1;
			unsigned long MB22 : 1;
			unsigned long MB21 : 1;
			unsigned long MB20 : 1;
			unsigned long MB19 : 1;
			unsigned long MB18 : 1;
			unsigned long MB17 : 1;
			unsigned long MB16 : 1;
			unsigned long MB15 : 1;
			unsigned long MB14 : 1;
			unsigned long MB13 : 1;
			unsigned long MB12 : 1;
			unsigned long MB11 : 1;
			unsigned long MB10 : 1;
			unsigned long MB9 : 1;
			unsigned long MB8 : 1;
			unsigned long MB7 : 1;
			unsigned long MB6 : 1;
			unsigned long MB5 : 1;
			unsigned long MB4 : 1;
			unsigned long MB3 : 1;
			unsigned long MB2 : 1;
			unsigned long MB1 : 1;
			unsigned long MB0 : 1;
#endif
	} BIT;
	} MIER;
	char           wk0[1008];
	union {
		unsigned char BYTE;
		union {
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SENTDATA : 1;
			unsigned char TRMACTIVE : 1;
			unsigned char TRMABT : 1;
			unsigned char  : 1;
			unsigned char ONESHOT : 1;
			unsigned char  : 1;
			unsigned char RECREQ : 1;
			unsigned char TRMREQ : 1;
#else
			unsigned char TRMREQ : 1;
			unsigned char RECREQ : 1;
			unsigned char  : 1;
			unsigned char ONESHOT : 1;
			unsigned char  : 1;
			unsigned char TRMABT : 1;
			unsigned char TRMACTIVE : 1;
			unsigned char SENTDATA : 1;
#endif
	} TX;
			struct {
				
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NEWDATA : 1;
			unsigned char INVALDATA : 1;
			unsigned char MSGLOST : 1;
			unsigned char  : 1;
			unsigned char ONESHOT : 1;
			unsigned char  : 1;
			unsigned char RECREQ : 1;
			unsigned char TRMREQ : 1;
#else
			unsigned char TRMREQ : 1;
			unsigned char RECREQ : 1;
			unsigned char  : 1;
			unsigned char ONESHOT : 1;
			unsigned char  : 1;
			unsigned char MSGLOST : 1;
			unsigned char INVALDATA : 1;
			unsigned char NEWDATA : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MBM : 1;
			unsigned short IDFM : 2;
			unsigned short MLM : 1;
			unsigned short TPM : 1;
			unsigned short TSRC : 1;
			unsigned short TSPS : 2;
			unsigned short CANM : 2;
			unsigned short SLPM : 1;
			unsigned short BOM : 2;
			unsigned short RBOC : 1;
			unsigned short  : 2;
#else
			unsigned short  : 2;
			unsigned short RBOC : 1;
			unsigned short BOM : 2;
			unsigned short SLPM : 1;
			unsigned short CANM : 2;
			unsigned short TSPS : 2;
			unsigned short TSRC : 1;
			unsigned short TPM : 1;
			unsigned short MLM : 1;
			unsigned short IDFM : 2;
			unsigned short MBM : 1;
#endif
	} BIT;
	} CTLR;
	union {
		unsigned short WORD;
		struct {
			unsigned char H;
			unsigned char L;
		} BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short NDST : 1;
			unsigned short SDST : 1;
			unsigned short RFST : 1;
			unsigned short TFST : 1;
			unsigned short NMLST : 1;
			unsigned short FMLST : 1;
			unsigned short TABST : 1;
			unsigned short EST : 1;
			unsigned short RSTST : 1;
			unsigned short HLTST : 1;
			unsigned short SLPST : 1;
			unsigned short EPST : 1;
			unsigned short BOST : 1;
			unsigned short TRMST : 1;
			unsigned short RECST : 1;
			unsigned short  : 1;
#else
			unsigned short  : 1;
			unsigned short RECST : 1;
			unsigned short TRMST : 1;
			unsigned short BOST : 1;
			unsigned short EPST : 1;
			unsigned short SLPST : 1;
			unsigned short HLTST : 1;
			unsigned short RSTST : 1;
			unsigned short EST : 1;
			unsigned short TABST : 1;
			unsigned short FMLST : 1;
			unsigned short NMLST : 1;
			unsigned short TFST : 1;
			unsigned short RFST : 1;
			unsigned short SDST : 1;
			unsigned short NDST : 1;
#endif
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CCLKS : 1;
			unsigned long  : 7;
			unsigned long TSEG2 : 3;
			unsigned long  : 1;
			unsigned long SJW : 2;
			unsigned long  : 2;
			unsigned long BRP : 10;
			unsigned long  : 2;
			unsigned long TSEG1 : 4;
#else
			unsigned long TSEG1 : 4;
			unsigned long  : 2;
			unsigned long BRP : 10;
			unsigned long  : 2;
			unsigned long SJW : 2;
			unsigned long  : 1;
			unsigned long TSEG2 : 3;
			unsigned long  : 7;
			unsigned long CCLKS : 1;
#endif
	} BIT;
	} BCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RFE : 1;
			unsigned char RFUST : 3;
			unsigned char RFMLF : 1;
			unsigned char RFFST : 1;
			unsigned char RFWST : 1;
			unsigned char RFEST : 1;
#else
			unsigned char RFEST : 1;
			unsigned char RFWST : 1;
			unsigned char RFFST : 1;
			unsigned char RFMLF : 1;
			unsigned char RFUST : 3;
			unsigned char RFE : 1;
#endif
	} BIT;
	} RFCR;
	unsigned char  RFPCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TFE : 1;
			unsigned char TFUST : 3;
			unsigned char  : 2;
			unsigned char TFFST : 1;
			unsigned char TFEST : 1;
#else
			unsigned char TFEST : 1;
			unsigned char TFFST : 1;
			unsigned char  : 2;
			unsigned char TFUST : 3;
			unsigned char TFE : 1;
#endif
	} BIT;
	} TFCR;
	unsigned char  TFPCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BEIE : 1;
			unsigned char EWIE : 1;
			unsigned char EPIE : 1;
			unsigned char BOEIE : 1;
			unsigned char BORIE : 1;
			unsigned char ORIE : 1;
			unsigned char OLIE : 1;
			unsigned char BLIE : 1;
#else
			unsigned char BLIE : 1;
			unsigned char OLIE : 1;
			unsigned char ORIE : 1;
			unsigned char BORIE : 1;
			unsigned char BOEIE : 1;
			unsigned char EPIE : 1;
			unsigned char EWIE : 1;
			unsigned char BEIE : 1;
#endif
	} BIT;
	} EIER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BEIF : 1;
			unsigned char EWIF : 1;
			unsigned char EPIF : 1;
			unsigned char BOEIF : 1;
			unsigned char BORIF : 1;
			unsigned char ORIF : 1;
			unsigned char OLIF : 1;
			unsigned char BLIF : 1;
#else
			unsigned char BLIF : 1;
			unsigned char OLIF : 1;
			unsigned char ORIF : 1;
			unsigned char BORIF : 1;
			unsigned char BOEIF : 1;
			unsigned char EPIF : 1;
			unsigned char EWIF : 1;
			unsigned char BEIF : 1;
#endif
	} BIT;
	} EIFR;
	unsigned char  RECR;
	unsigned char  TECR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SEF : 1;
			unsigned char FEF : 1;
			unsigned char AEF : 1;
			unsigned char CEF : 1;
			unsigned char BE1F : 1;
			unsigned char BE0F : 1;
			unsigned char ADEF : 1;
			unsigned char EDPM : 1;
#else
			unsigned char EDPM : 1;
			unsigned char ADEF : 1;
			unsigned char BE0F : 1;
			unsigned char BE1F : 1;
			unsigned char CEF : 1;
			unsigned char AEF : 1;
			unsigned char FEF : 1;
			unsigned char SEF : 1;
#endif
	} BIT;
	} ECSR;
	unsigned char  CSSR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MBNST : 5;
			unsigned char  : 2;
			unsigned char SEST : 1;
#else
			unsigned char SEST : 1;
			unsigned char  : 2;
			unsigned char MBNST : 5;
#endif
	} BIT;
	} MSSR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char MBSM : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char MBSM : 2;
#endif
	} BIT;
	} MSMR;
	unsigned short TSR;
	unsigned short AFSR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TSTE : 1;
			unsigned char TSTM : 2;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char TSTM : 2;
			unsigned char TSTE : 1;
#endif
	} BIT;
	} TCR;
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

struct st_da {
	unsigned short DADR0;
	unsigned short DADR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 5;
			unsigned char DAE : 1;
			unsigned char DAOE0 : 1;
			unsigned char DAOE1 : 1;
#else
			unsigned char DAOE1 : 1;
			unsigned char DAOE0 : 1;
			unsigned char DAE : 1;
			unsigned char : 5;
#endif
	} BIT;
	} DACR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 7;
			unsigned char DPSEL : 1;
#else
			unsigned char DPSEL : 1;
			unsigned char : 7;
#endif
	} BIT;
	} DADPR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 7;
			unsigned char DAADST : 1;
#else
			unsigned char DAADST : 1;
			unsigned char : 7;
#endif
	} BIT;
	} DAADSCR;
};

struct st_dmac {
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
};

struct st_dmac0 {
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
};

struct st_dmac1 {
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
};

struct st_dtc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 4;
			unsigned char RRS : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char RRS : 1;
			unsigned char : 4;
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

struct st_edmac {
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long SWR : 1;
			unsigned long  : 3;
			unsigned long DL : 2;
			unsigned long DE : 1;
			unsigned long  : 25;
#else
			unsigned long  : 25;
			unsigned long DE : 1;
			unsigned long DL : 2;
			unsigned long  : 3;
			unsigned long SWR : 1;
#endif
	} BIT;
	} EDMR;
	char           wk0[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long TR : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long TR : 1;
#endif
	} BIT;
	} EDTRR;
	char           wk1[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long RR : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long RR : 1;
#endif
	} BIT;
	} EDRRR;
	char           wk2[4];
	void          *TDLAR;
	char           wk3[4];
	void          *RDLAR;
	char           wk4[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CERF : 1;
			unsigned long PRE : 1;
			unsigned long RTSF : 1;
			unsigned long RTLF : 1;
			unsigned long RRF : 1;
			unsigned long  : 2;
			unsigned long RMAF : 1;
			unsigned long TRO : 1;
			unsigned long CD : 1;
			unsigned long DLC : 1;
			unsigned long CND : 1;
			unsigned long  : 4;
			unsigned long RFOF : 1;
			unsigned long RDE : 1;
			unsigned long FR : 1;
			unsigned long TFUF : 1;
			unsigned long TDE : 1;
			unsigned long TC : 1;
			unsigned long ECI : 1;
			unsigned long ADE : 1;
			unsigned long RFCOF : 1;
			unsigned long RABT : 1;
			unsigned long TABT : 1;
			unsigned long  : 3;
			unsigned long TWB : 1;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long TWB : 1;
			unsigned long  : 3;
			unsigned long TABT : 1;
			unsigned long RABT : 1;
			unsigned long RFCOF : 1;
			unsigned long ADE : 1;
			unsigned long ECI : 1;
			unsigned long TC : 1;
			unsigned long TDE : 1;
			unsigned long TFUF : 1;
			unsigned long FR : 1;
			unsigned long RDE : 1;
			unsigned long RFOF : 1;
			unsigned long  : 4;
			unsigned long CND : 1;
			unsigned long DLC : 1;
			unsigned long CD : 1;
			unsigned long TRO : 1;
			unsigned long RMAF : 1;
			unsigned long  : 2;
			unsigned long RRF : 1;
			unsigned long RTLF : 1;
			unsigned long RTSF : 1;
			unsigned long PRE : 1;
			unsigned long CERF : 1;
#endif
	} BIT;
	} EESR;
	char           wk5[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CERFIP : 1;
			unsigned long PREIP : 1;
			unsigned long RTSFIP : 1;
			unsigned long RTLFIP : 1;
			unsigned long RRFIP : 1;
			unsigned long  : 2;
			unsigned long RMAFIP : 1;
			unsigned long TROIP : 1;
			unsigned long CDIP : 1;
			unsigned long DLCIP : 1;
			unsigned long CNDIP : 1;
			unsigned long  : 4;
			unsigned long RFOFIP : 1;
			unsigned long RDEIP : 1;
			unsigned long FRIP : 1;
			unsigned long TFUFIP : 1;
			unsigned long TDEIP : 1;
			unsigned long TCIP : 1;
			unsigned long ECIIP : 1;
			unsigned long ADEIP : 1;
			unsigned long RFCOFIP : 1;
			unsigned long RABTIP : 1;
			unsigned long TABTIP : 1;
			unsigned long  : 3;
			unsigned long TWBIP : 1;
			unsigned long  : 1;
#else
			unsigned long  : 1;
			unsigned long TWBIP : 1;
			unsigned long  : 3;
			unsigned long TABTIP : 1;
			unsigned long RABTIP : 1;
			unsigned long RFCOFIP : 1;
			unsigned long ADEIP : 1;
			unsigned long ECIIP : 1;
			unsigned long TCIP : 1;
			unsigned long TDEIP : 1;
			unsigned long TFUFIP : 1;
			unsigned long FRIP : 1;
			unsigned long RDEIP : 1;
			unsigned long RFOFIP : 1;
			unsigned long  : 4;
			unsigned long CNDIP : 1;
			unsigned long DLCIP : 1;
			unsigned long CDIP : 1;
			unsigned long TROIP : 1;
			unsigned long RMAFIP : 1;
			unsigned long  : 2;
			unsigned long RRFIP : 1;
			unsigned long RTLFIP : 1;
			unsigned long RTSFIP : 1;
			unsigned long PREIP : 1;
			unsigned long CERFIP : 1;
#endif
	} BIT;
	} EESIPR;
	char           wk6[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CERFCE : 1;
			unsigned long PRECE : 1;
			unsigned long RTSFCE : 1;
			unsigned long RTLFCE : 1;
			unsigned long RRFCE : 1;
			unsigned long  : 2;
			unsigned long RMAFCE : 1;
			unsigned long TROCE : 1;
			unsigned long CDCE : 1;
			unsigned long DLCCE : 1;
			unsigned long CNDCE : 1;
			unsigned long  : 20;
#else
			unsigned long  : 20;
			unsigned long CNDCE : 1;
			unsigned long DLCCE : 1;
			unsigned long CDCE : 1;
			unsigned long TROCE : 1;
			unsigned long RMAFCE : 1;
			unsigned long  : 2;
			unsigned long RRFCE : 1;
			unsigned long RTLFCE : 1;
			unsigned long RTSFCE : 1;
			unsigned long PRECE : 1;
			unsigned long CERFCE : 1;
#endif
	} BIT;
	} TRSCER;
	char           wk7[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MFC : 16;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long MFC : 16;
#endif
	} BIT;
	} RMFCR;
	char           wk8[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long TFT : 11;
			unsigned long  : 21;
#else
			unsigned long  : 21;
			unsigned long TFT : 11;
#endif
	} BIT;
	} TFTR;
	char           wk9[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long RFD : 5;
			unsigned long  : 3;
			unsigned long TFD : 5;
			unsigned long  : 19;
#else
			unsigned long  : 19;
			unsigned long TFD : 5;
			unsigned long  : 3;
			unsigned long RFD : 5;
#endif
	} BIT;
	} FDR;
	char           wk10[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long RNR : 1;
			unsigned long RNC : 1;
			unsigned long  : 30;
#else
			unsigned long  : 30;
			unsigned long RNC : 1;
			unsigned long RNR : 1;
#endif
	} BIT;
	} RMCR;
	char           wk11[8];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long UNDER : 16;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long UNDER : 16;
#endif
	} BIT;
	} TFUCR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long OVER : 16;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long OVER : 16;
#endif
	} BIT;
	} RFOCR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long ELB : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long ELB : 1;
#endif
	} BIT;
	} IOSR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long RFDO : 3;
			unsigned long  : 13;
			unsigned long RFFO : 3;
			unsigned long  : 13;
#else
			unsigned long  : 13;
			unsigned long RFFO : 3;
			unsigned long  : 13;
			unsigned long RFDO : 3;
#endif
	} BIT;
	} FCFTR;
	char           wk12[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long PADR : 6;
			unsigned long  : 10;
			unsigned long PADS : 2;
			unsigned long  : 14;
#else
			unsigned long  : 14;
			unsigned long PADS : 2;
			unsigned long  : 10;
			unsigned long PADR : 6;
#endif
	} BIT;
	} RPADIR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long TIS : 1;
			unsigned long  : 3;
			unsigned long TIM : 1;
			unsigned long  : 27;
#else
			unsigned long  : 27;
			unsigned long TIM : 1;
			unsigned long  : 3;
			unsigned long TIS : 1;
#endif
	} BIT;
	} TRIMD;
	char           wk13[72];
	void          *RBWAR;
	void          *RDFAR;
	char           wk14[4];
	void          *TBRAR;
	void          *TDFAR;
};

struct st_etherc {
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long PRM : 1;
			unsigned long DM : 1;
			unsigned long RTM : 1;
			unsigned long ILB : 1;
			unsigned long  : 1;
			unsigned long TE : 1;
			unsigned long RE : 1;
			unsigned long  : 2;
			unsigned long MPDE : 1;
			unsigned long  : 2;
			unsigned long PRCEF : 1;
			unsigned long  : 3;
			unsigned long TXF : 1;
			unsigned long RXF : 1;
			unsigned long PFR : 1;
			unsigned long ZPF : 1;
			unsigned long TPC : 1;
			unsigned long  : 11;
#else
			unsigned long  : 11;
			unsigned long TPC : 1;
			unsigned long ZPF : 1;
			unsigned long PFR : 1;
			unsigned long RXF : 1;
			unsigned long TXF : 1;
			unsigned long  : 3;
			unsigned long PRCEF : 1;
			unsigned long  : 2;
			unsigned long MPDE : 1;
			unsigned long  : 2;
			unsigned long RE : 1;
			unsigned long TE : 1;
			unsigned long  : 1;
			unsigned long ILB : 1;
			unsigned long RTM : 1;
			unsigned long DM : 1;
			unsigned long PRM : 1;
#endif
	} BIT;
	} ECMR;
	char           wk0[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long RFL : 12;
			unsigned long  : 20;
#else
			unsigned long  : 20;
			unsigned long RFL : 12;
#endif
	} BIT;
	} RFLR;
	char           wk1[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long ICD : 1;
			unsigned long MPD : 1;
			unsigned long LCHNG : 1;
			unsigned long  : 1;
			unsigned long PSRTO : 1;
			unsigned long BFR : 1;
			unsigned long  : 26;
#else
			unsigned long  : 26;
			unsigned long BFR : 1;
			unsigned long PSRTO : 1;
			unsigned long  : 1;
			unsigned long LCHNG : 1;
			unsigned long MPD : 1;
			unsigned long ICD : 1;
#endif
	} BIT;
	} ECSR;
	char           wk2[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long ICDIP : 1;
			unsigned long MPDIP : 1;
			unsigned long LCHNGIP : 1;
			unsigned long  : 1;
			unsigned long PSRTOIP : 1;
			unsigned long BFSIPR : 1;
			unsigned long  : 26;
#else
			unsigned long  : 26;
			unsigned long BFSIPR : 1;
			unsigned long PSRTOIP : 1;
			unsigned long  : 1;
			unsigned long LCHNGIP : 1;
			unsigned long MPDIP : 1;
			unsigned long ICDIP : 1;
#endif
	} BIT;
	} ECSIPR;
	char           wk3[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MDC : 1;
			unsigned long MMD : 1;
			unsigned long MDO : 1;
			unsigned long MDI : 1;
			unsigned long  : 28;
#else
			unsigned long  : 28;
			unsigned long MDI : 1;
			unsigned long MDO : 1;
			unsigned long MMD : 1;
			unsigned long MDC : 1;
#endif
	} BIT;
	} PIR;
	char           wk4[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long LMON : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long LMON : 1;
#endif
	} BIT;
	} PSR;
	char           wk5[20];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long RMD : 20;
			unsigned long  : 12;
#else
			unsigned long  : 12;
			unsigned long RMD : 20;
#endif
	} BIT;
	} RDMLR;
	char           wk6[12];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long IPG : 5;
			unsigned long  : 27;
#else
			unsigned long  : 27;
			unsigned long IPG : 5;
#endif
	} BIT;
	} IPGR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long AP : 16;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long AP : 16;
#endif
	} BIT;
	} APR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MP : 16;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long MP : 16;
#endif
	} BIT;
	} MPR;
	char           wk7[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long RPAUSE : 8;
			unsigned long  : 24;
#else
			unsigned long  : 24;
			unsigned long RPAUSE : 8;
#endif
	} BIT;
	} RFCF;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long TPAUSE : 16;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long TPAUSE : 16;
#endif
	} BIT;
	} TPAUSER;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long TXP : 8;
			unsigned long  : 24;
#else
			unsigned long  : 24;
			unsigned long TXP : 8;
#endif
	} BIT;
	} TPAUSECR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long BCF : 16;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long BCF : 16;
#endif
	} BIT;
	} BCFRR;
	char           wk8[80];
	unsigned long  MAHR;
	char           wk9[4];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MA : 16;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long MA : 16;
#endif
	} BIT;
	} MALR;
	char           wk10[4];
	unsigned long  TROCR;
	unsigned long  CDCR;
	unsigned long  LCCR;
	unsigned long  CNDCR;
	char           wk11[4];
	unsigned long  CEFCR;
	unsigned long  FRECR;
	unsigned long  TSFRCR;
	unsigned long  TLFRCR;
	unsigned long  RFCR;
	unsigned long  MAFCR;
};

struct st_exdmac {
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
	} EDMAST;
	char           wk0[479];
	unsigned long  CLSBR0;
	unsigned long  CLSBR1;
	unsigned long  CLSBR2;
	unsigned long  CLSBR3;
	unsigned long  CLSBR4;
	unsigned long  CLSBR5;
	unsigned long  CLSBR6;
	unsigned long  CLSBR7;
};

struct st_exdmac0 {
	void          *EDMSAR;
	void          *EDMDAR;
	unsigned long  EDMCRA;
	unsigned short EDMCRB;
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
	} EDMTMD;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DACKSEL : 1;
			unsigned char DACKW : 1;
			unsigned char DACKE : 1;
			unsigned char DACKS : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char DACKS : 1;
			unsigned char DACKE : 1;
			unsigned char DACKW : 1;
			unsigned char DACKSEL : 1;
#endif
	} BIT;
	} EDMOMD;
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
	} EDMINT;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long DARA : 5;
			unsigned long  : 1;
			unsigned long DM : 2;
			unsigned long SARA : 5;
			unsigned long  : 1;
			unsigned long SM : 2;
			unsigned long DIR : 1;
			unsigned long AMS : 1;
			unsigned long  : 14;
#else
			unsigned long  : 14;
			unsigned long AMS : 1;
			unsigned long DIR : 1;
			unsigned long SM : 2;
			unsigned long  : 1;
			unsigned long SARA : 5;
			unsigned long DM : 2;
			unsigned long  : 1;
			unsigned long DARA : 5;
#endif
	} BIT;
	} EDMAMD;
	unsigned long  EDMOFR;
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
	} EDMCNT;
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
	} EDMREQ;
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
	} EDMSTS;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DREQS : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char DREQS : 2;
#endif
	} BIT;
	} EDMRMD;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char EREQ : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char EREQ : 1;
#endif
	} BIT;
	} EDMERF;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PREQ : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char PREQ : 1;
#endif
	} BIT;
	} EDMPRF;
};

struct st_exdmac1 {
	void          *EDMSAR;
	void          *EDMDAR;
	unsigned long  EDMCRA;
	unsigned short EDMCRB;
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
	} EDMTMD;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DACKSEL : 1;
			unsigned char DACKW : 1;
			unsigned char DACKE : 1;
			unsigned char DACKS : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char DACKS : 1;
			unsigned char DACKE : 1;
			unsigned char DACKW : 1;
			unsigned char DACKSEL : 1;
#endif
	} BIT;
	} EDMOMD;
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
	} EDMINT;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long DARA : 5;
			unsigned long  : 1;
			unsigned long DM : 2;
			unsigned long SARA : 5;
			unsigned long  : 1;
			unsigned long SM : 2;
			unsigned long DIR : 1;
			unsigned long AMS : 1;
			unsigned long  : 14;
#else
			unsigned long  : 14;
			unsigned long AMS : 1;
			unsigned long DIR : 1;
			unsigned long SM : 2;
			unsigned long  : 1;
			unsigned long SARA : 5;
			unsigned long DM : 2;
			unsigned long  : 1;
			unsigned long DARA : 5;
#endif
	} BIT;
	} EDMAMD;
	char           wk1[4];
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
	} EDMCNT;
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
	} EDMREQ;
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
	} EDMSTS;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DREQS : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char DREQS : 2;
#endif
	} BIT;
	} EDMRMD;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char EREQ : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char EREQ : 1;
#endif
	} BIT;
	} EDMERF;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PREQ : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char PREQ : 1;
#endif
	} BIT;
	} EDMPRF;
};

struct st_flash {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FLWE : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char FLWE : 2;
#endif
	} BIT;
	} FWEPROR;
	char           wk0[7799147];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 4;
			unsigned char FRDMD : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char FRDMD : 1;
			unsigned char : 4;
#endif
	} BIT;
	} FMODR;
	char           wk1[13];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DFLWPE : 1;
			unsigned char DFLRPE : 1;
			unsigned char  : 1;
			unsigned char DFLAE : 1;
			unsigned char CMDLK : 1;
			unsigned char  : 2;
			unsigned char ROMAE : 1;
#else
			unsigned char ROMAE : 1;
			unsigned char  : 2;
			unsigned char CMDLK : 1;
			unsigned char DFLAE : 1;
			unsigned char  : 1;
			unsigned char DFLRPE : 1;
			unsigned char DFLWPE : 1;
#endif
	} BIT;
	} FASTAT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DFLWPEIE : 1;
			unsigned char DFLRPEIE : 1;
			unsigned char  : 1;
			unsigned char DFLAEIE : 1;
			unsigned char CMDLKIE : 1;
			unsigned char  : 2;
			unsigned char ROMAEIE : 1;
#else
			unsigned char ROMAEIE : 1;
			unsigned char  : 2;
			unsigned char CMDLKIE : 1;
			unsigned char DFLAEIE : 1;
			unsigned char  : 1;
			unsigned char DFLRPEIE : 1;
			unsigned char DFLWPEIE : 1;
#endif
	} BIT;
	} FAEINT;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FRDYIE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char FRDYIE : 1;
#endif
	} BIT;
	} FRDYIE;
	char           wk2[45];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DBRE00 : 1;
			unsigned short DBRE01 : 1;
			unsigned short DBRE02 : 1;
			unsigned short DBRE03 : 1;
			unsigned short DBRE04 : 1;
			unsigned short DBRE05 : 1;
			unsigned short DBRE06 : 1;
			unsigned short DBRE07 : 1;
			unsigned short KEY : 8;
#else
			unsigned short KEY : 8;
			unsigned short DBRE07 : 1;
			unsigned short DBRE06 : 1;
			unsigned short DBRE05 : 1;
			unsigned short DBRE04 : 1;
			unsigned short DBRE03 : 1;
			unsigned short DBRE02 : 1;
			unsigned short DBRE01 : 1;
			unsigned short DBRE00 : 1;
#endif
	} BIT;
	} DFLRE0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DBRE08 : 1;
			unsigned short DBRE09 : 1;
			unsigned short DBRE10 : 1;
			unsigned short DBRE11 : 1;
			unsigned short DBRE12 : 1;
			unsigned short DBRE13 : 1;
			unsigned short DBRE14 : 1;
			unsigned short DBRE15 : 1;
			unsigned short KEY : 8;
#else
			unsigned short KEY : 8;
			unsigned short DBRE15 : 1;
			unsigned short DBRE14 : 1;
			unsigned short DBRE13 : 1;
			unsigned short DBRE12 : 1;
			unsigned short DBRE11 : 1;
			unsigned short DBRE10 : 1;
			unsigned short DBRE09 : 1;
			unsigned short DBRE08 : 1;
#endif
	} BIT;
	} DFLRE1;
	char           wk3[12];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DBWE00 : 1;
			unsigned short DBWE01 : 1;
			unsigned short DBWE02 : 1;
			unsigned short DBWE03 : 1;
			unsigned short DBWE04 : 1;
			unsigned short DBWE05 : 1;
			unsigned short DBW006 : 1;
			unsigned short DBWE07 : 1;
			unsigned short KEY : 8;
#else
			unsigned short KEY : 8;
			unsigned short DBWE07 : 1;
			unsigned short DBW006 : 1;
			unsigned short DBWE05 : 1;
			unsigned short DBWE04 : 1;
			unsigned short DBWE03 : 1;
			unsigned short DBWE02 : 1;
			unsigned short DBWE01 : 1;
			unsigned short DBWE00 : 1;
#endif
	} BIT;
	} DFLWE0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DBWE08 : 1;
			unsigned short DBWE09 : 1;
			unsigned short DBWE10 : 1;
			unsigned short DBWE11 : 1;
			unsigned short DBWE12 : 1;
			unsigned short DBWE13 : 1;
			unsigned short DBWE14 : 1;
			unsigned short DBWE15 : 1;
			unsigned short KEY : 8;
#else
			unsigned short KEY : 8;
			unsigned short DBWE15 : 1;
			unsigned short DBWE14 : 1;
			unsigned short DBWE13 : 1;
			unsigned short DBWE12 : 1;
			unsigned short DBWE11 : 1;
			unsigned short DBWE10 : 1;
			unsigned short DBWE09 : 1;
			unsigned short DBWE08 : 1;
#endif
	} BIT;
	} DFLWE1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FCRME : 1;
			unsigned short  : 7;
			unsigned short KEY : 8;
#else
			unsigned short KEY : 8;
			unsigned short  : 7;
			unsigned short FCRME : 1;
#endif
	} BIT;
	} FCURAME;
	char           wk4[15194];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PRGSPD : 1;
			unsigned char ERSSPD : 1;
			unsigned char  : 1;
			unsigned char SUSRDY : 1;
			unsigned char PRGERR : 1;
			unsigned char ERSERR : 1;
			unsigned char ILGLERR : 1;
			unsigned char FRDY : 1;
#else
			unsigned char FRDY : 1;
			unsigned char ILGLERR : 1;
			unsigned char ERSERR : 1;
			unsigned char PRGERR : 1;
			unsigned char SUSRDY : 1;
			unsigned char  : 1;
			unsigned char ERSSPD : 1;
			unsigned char PRGSPD : 1;
#endif
	} BIT;
	} FSTATR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 4;
			unsigned char FLOCKST : 1;
			unsigned char  : 2;
			unsigned char FCUERR : 1;
#else
			unsigned char FCUERR : 1;
			unsigned char  : 2;
			unsigned char FLOCKST : 1;
			unsigned char : 4;
#endif
	} BIT;
	} FSTATR1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FENTRY0 : 1;
			unsigned short FENTRY1 : 1;
			unsigned short FENTRY2 : 1;
			unsigned short FENTRY3 : 1;
			unsigned short  : 3;
			unsigned short FENTRYD : 1;
			unsigned short FEKEY : 8;
#else
			unsigned short FEKEY : 8;
			unsigned short FENTRYD : 1;
			unsigned short  : 3;
			unsigned short FENTRY3 : 1;
			unsigned short FENTRY2 : 1;
			unsigned short FENTRY1 : 1;
			unsigned short FENTRY0 : 1;
#endif
	} BIT;
	} FENTRYR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FPROTCN : 1;
			unsigned short  : 7;
			unsigned short FPKEY : 8;
#else
			unsigned short FPKEY : 8;
			unsigned short  : 7;
			unsigned short FPROTCN : 1;
#endif
	} BIT;
	} FPROTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FRESET : 1;
			unsigned short  : 7;
			unsigned short FRKEY : 8;
#else
			unsigned short FRKEY : 8;
			unsigned short  : 7;
			unsigned short FRESET : 1;
#endif
	} BIT;
	} FRESETR;
	char           wk5[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PCMDR : 8;
			unsigned short CMDR : 8;
#else
			unsigned short CMDR : 8;
			unsigned short PCMDR : 8;
#endif
	} BIT;
	} FCMDR;
	char           wk6[12];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ESUSPMD : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short ESUSPMD : 1;
#endif
	} BIT;
	} FCPSR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BCADR : 11;
			unsigned short  : 4;
			unsigned short BCSIZE : 1;
#else
			unsigned short BCSIZE : 1;
			unsigned short  : 4;
			unsigned short BCADR : 11;
#endif
	} BIT;
	} DFLBCCNT;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PEERRST : 8;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short PEERRST : 8;
#endif
	} BIT;
	} FPESTAT;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BCST : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short BCST : 1;
#endif
	} BIT;
	} DFLBCSTAT;
	char           wk7[24];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PCKA : 8;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short PCKA : 8;
#endif
	} BIT;
	} PCKAR;
};

struct st_flashconst {
	unsigned char  UIDR0;
	unsigned char  UIDR1;
	unsigned char  UIDR2;
	unsigned char  UIDR3;
	unsigned char  UIDR4;
	unsigned char  UIDR5;
	unsigned char  UIDR6;
	unsigned char  UIDR7;
	unsigned char  UIDR8;
	unsigned char  UIDR9;
	unsigned char  UIDR10;
	unsigned char  UIDR11;
	unsigned char  UIDR12;
	unsigned char  UIDR13;
	unsigned char  UIDR14;
	unsigned char  UIDR15;
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
	} IR[254];
	char           wk0[2];
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
	} DTCER[252];
	char           wk1[4];
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
	} IPR[254];
	char           wk5[2];
	unsigned char  DMRSR0;
	char           wk6[3];
	unsigned char  DMRSR1;
	char           wk7[3];
	unsigned char  DMRSR2;
	char           wk8[3];
	unsigned char  DMRSR3;
	char           wk9[243];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 2;
			unsigned char IRQMD : 2;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char IRQMD : 2;
			unsigned char : 2;
#endif
	} BIT;
	} IRQCR[16];
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char FLTEN8 : 1;
			unsigned char FLTEN9 : 1;
			unsigned char FLTEN10 : 1;
			unsigned char FLTEN11 : 1;
			unsigned char FLTEN12 : 1;
			unsigned char FLTEN13 : 1;
			unsigned char FLTEN14 : 1;
			unsigned char FLTEN15 : 1;
#else
			unsigned char FLTEN15 : 1;
			unsigned char FLTEN14 : 1;
			unsigned char FLTEN13 : 1;
			unsigned char FLTEN12 : 1;
			unsigned char FLTEN11 : 1;
			unsigned char FLTEN10 : 1;
			unsigned char FLTEN9 : 1;
			unsigned char FLTEN8 : 1;
#endif
	} BIT;
	} IRQFLTE1;
	char           wk10[2];
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
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FCLKSEL8 : 2;
			unsigned short FCLKSEL9 : 2;
			unsigned short FCLKSEL10 : 2;
			unsigned short FCLKSEL11 : 2;
			unsigned short FCLKSEL12 : 2;
			unsigned short FCLKSEL13 : 2;
			unsigned short FCLKSEL14 : 2;
			unsigned short FCLKSEL15 : 2;
#else
			unsigned short FCLKSEL15 : 2;
			unsigned short FCLKSEL14 : 2;
			unsigned short FCLKSEL13 : 2;
			unsigned short FCLKSEL12 : 2;
			unsigned short FCLKSEL11 : 2;
			unsigned short FCLKSEL10 : 2;
			unsigned short FCLKSEL9 : 2;
			unsigned short FCLKSEL8 : 2;
#endif
	} BIT;
	} IRQFLTC1;
	char           wk11[104];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NMIST : 1;
			unsigned char OSTST : 1;
			unsigned char WDTST : 1;
			unsigned char IWDTST : 1;
			unsigned char LVD1ST : 1;
			unsigned char LVD2ST : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char LVD2ST : 1;
			unsigned char LVD1ST : 1;
			unsigned char IWDTST : 1;
			unsigned char WDTST : 1;
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
			unsigned char WDTEN : 1;
			unsigned char IWDTEN : 1;
			unsigned char LVD1EN : 1;
			unsigned char LVD2EN : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char LVD2EN : 1;
			unsigned char LVD1EN : 1;
			unsigned char IWDTEN : 1;
			unsigned char WDTEN : 1;
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
			unsigned char WDTCLR : 1;
			unsigned char IWDTCLR : 1;
			unsigned char LVD1CLR : 1;
			unsigned char LVD2CLR : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char LVD2CLR : 1;
			unsigned char LVD1CLR : 1;
			unsigned char IWDTCLR : 1;
			unsigned char WDTCLR : 1;
			unsigned char OSTCLR : 1;
			unsigned char NMICLR : 1;
#endif
	} BIT;
	} NMICLR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 3;
			unsigned char NMIMD : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char NMIMD : 1;
			unsigned char : 3;
#endif
	} BIT;
	} NMICR;
	char           wk12[12];
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
	char           wk13[3];
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
	char           wk14[19819];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long IS0 : 1;
			unsigned long IS1 : 1;
			unsigned long IS2 : 1;
			unsigned long IS3 : 1;
			unsigned long IS4 : 1;
			unsigned long IS5 : 1;
			unsigned long IS6 : 1;
			unsigned long IS7 : 1;
			unsigned long IS8 : 1;
			unsigned long IS9 : 1;
			unsigned long IS10 : 1;
			unsigned long IS11 : 1;
			unsigned long IS12 : 1;
			unsigned long IS13 : 1;
			unsigned long IS14 : 1;
			unsigned long IS15 : 1;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long IS15 : 1;
			unsigned long IS14 : 1;
			unsigned long IS13 : 1;
			unsigned long IS12 : 1;
			unsigned long IS11 : 1;
			unsigned long IS10 : 1;
			unsigned long IS9 : 1;
			unsigned long IS8 : 1;
			unsigned long IS7 : 1;
			unsigned long IS6 : 1;
			unsigned long IS5 : 1;
			unsigned long IS4 : 1;
			unsigned long IS3 : 1;
			unsigned long IS2 : 1;
			unsigned long IS1 : 1;
			unsigned long IS0 : 1;
#endif
	} BIT;
	} GRP[13];
	char           wk15[12];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long EN0 : 1;
			unsigned long EN1 : 1;
			unsigned long EN2 : 1;
			unsigned long EN3 : 1;
			unsigned long EN4 : 1;
			unsigned long EN5 : 1;
			unsigned long EN6 : 1;
			unsigned long EN7 : 1;
			unsigned long EN8 : 1;
			unsigned long EN9 : 1;
			unsigned long EN10 : 1;
			unsigned long EN11 : 1;
			unsigned long EN12 : 1;
			unsigned long EN13 : 1;
			unsigned long EN14 : 1;
			unsigned long EN15 : 1;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long EN15 : 1;
			unsigned long EN14 : 1;
			unsigned long EN13 : 1;
			unsigned long EN12 : 1;
			unsigned long EN11 : 1;
			unsigned long EN10 : 1;
			unsigned long EN9 : 1;
			unsigned long EN8 : 1;
			unsigned long EN7 : 1;
			unsigned long EN6 : 1;
			unsigned long EN5 : 1;
			unsigned long EN4 : 1;
			unsigned long EN3 : 1;
			unsigned long EN2 : 1;
			unsigned long EN1 : 1;
			unsigned long EN0 : 1;
#endif
	} BIT;
	} GEN[13];
	char           wk16[12];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CLR0 : 1;
			unsigned long CLR1 : 1;
			unsigned long CLR2 : 1;
			unsigned long CLR3 : 1;
			unsigned long CLR4 : 1;
			unsigned long CLR5 : 1;
			unsigned long CLR6 : 1;
			unsigned long CLR7 : 1;
			unsigned long CLR8 : 1;
			unsigned long CLR9 : 1;
			unsigned long CLR10 : 1;
			unsigned long CLR11 : 1;
			unsigned long CLR12 : 1;
			unsigned long CLR13 : 1;
			unsigned long CLR14 : 1;
			unsigned long CLR15 : 1;
			unsigned long  : 16;
#else
			unsigned long  : 16;
			unsigned long CLR15 : 1;
			unsigned long CLR14 : 1;
			unsigned long CLR13 : 1;
			unsigned long CLR12 : 1;
			unsigned long CLR11 : 1;
			unsigned long CLR10 : 1;
			unsigned long CLR9 : 1;
			unsigned long CLR8 : 1;
			unsigned long CLR7 : 1;
			unsigned long CLR6 : 1;
			unsigned long CLR5 : 1;
			unsigned long CLR4 : 1;
			unsigned long CLR3 : 1;
			unsigned long CLR2 : 1;
			unsigned long CLR1 : 1;
			unsigned long CLR0 : 1;
#endif
	} BIT;
	} GCR[13];
	char           wk17[12];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long CN0 : 1;
			unsigned long CN1 : 1;
			unsigned long CN2 : 1;
			unsigned long CN3 : 1;
			unsigned long CN4 : 1;
			unsigned long CN5 : 1;
			unsigned long  : 26;
#else
			unsigned long  : 26;
			unsigned long CN5 : 1;
			unsigned long CN4 : 1;
			unsigned long CN3 : 1;
			unsigned long CN2 : 1;
			unsigned long CN1 : 1;
			unsigned long CN0 : 1;
#endif
	} BIT;
	} SEL;
};

struct st_ieb {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 3;
			unsigned char RE : 1;
			unsigned char  : 1;
			unsigned char DEE : 1;
			unsigned char IOL : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char IOL : 1;
			unsigned char DEE : 1;
			unsigned char  : 1;
			unsigned char RE : 1;
			unsigned char : 3;
#endif
	} BIT;
	} IECTR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CMD : 3;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char CMD : 3;
#endif
	} BIT;
	} IECMR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CTL : 4;
			unsigned char RN : 3;
			unsigned char SS : 1;
#else
			unsigned char SS : 1;
			unsigned char RN : 3;
			unsigned char CTL : 4;
#endif
	} BIT;
	} IEMCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char STE : 1;
			unsigned char  : 1;
			unsigned char IMD : 2;
			unsigned char IARL4 : 4;
#else
			unsigned char IARL4 : 4;
			unsigned char IMD : 2;
			unsigned char  : 1;
			unsigned char STE : 1;
#endif
	} BIT;
	} IEAR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IARU8 : 8;
#else
			unsigned char IARU8 : 8;
#endif
	} BIT;
	} IEAR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 4;
			unsigned char ISAL4 : 4;
#else
			unsigned char ISAL4 : 4;
			unsigned char : 4;
#endif
	} BIT;
	} IESA1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ISAU8 : 8;
#else
			unsigned char ISAU8 : 8;
#endif
	} BIT;
	} IESA2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IBFL : 8;
#else
			unsigned char IBFL : 8;
#endif
	} BIT;
	} IETBFL;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 4;
			unsigned char IMAL4 : 4;
#else
			unsigned char IMAL4 : 4;
			unsigned char : 4;
#endif
	} BIT;
	} IEMA1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IMAU8 : 8;
#else
			unsigned char IMAU8 : 8;
#endif
	} BIT;
	} IEMA2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RCTL : 4;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char RCTL : 4;
#endif
	} BIT;
	} IERCTL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RBFL : 8;
#else
			unsigned char RBFL : 8;
#endif
	} BIT;
	} IERBFL;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ILAL8 : 8;
#else
			unsigned char ILAL8 : 8;
#endif
	} BIT;
	} IELA1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ILAU4 : 4;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char ILAU4 : 4;
#endif
	} BIT;
	} IELA2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char GG : 1;
			unsigned char RSS : 1;
			unsigned char  : 1;
			unsigned char LCK : 1;
			unsigned char SRE : 1;
			unsigned char SRQ : 1;
			unsigned char MRQ : 1;
			unsigned char CMX : 1;
#else
			unsigned char CMX : 1;
			unsigned char MRQ : 1;
			unsigned char SRQ : 1;
			unsigned char SRE : 1;
			unsigned char LCK : 1;
			unsigned char  : 1;
			unsigned char RSS : 1;
			unsigned char GG : 1;
#endif
	} BIT;
	} IEFLG;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TXEACK : 1;
			unsigned char TXERO : 1;
			unsigned char TXETTME : 1;
			unsigned char TXEAL : 1;
			unsigned char  : 1;
			unsigned char TXF : 1;
			unsigned char TXS : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char TXS : 1;
			unsigned char TXF : 1;
			unsigned char  : 1;
			unsigned char TXEAL : 1;
			unsigned char TXETTME : 1;
			unsigned char TXERO : 1;
			unsigned char TXEACK : 1;
#endif
	} BIT;
	} IETSR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TXEACKE : 1;
			unsigned char TXEROE : 1;
			unsigned char TXETTMEE : 1;
			unsigned char TXEALE : 1;
			unsigned char  : 1;
			unsigned char TXFE : 1;
			unsigned char TXSE : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char TXSE : 1;
			unsigned char TXFE : 1;
			unsigned char  : 1;
			unsigned char TXEALE : 1;
			unsigned char TXETTMEE : 1;
			unsigned char TXEROE : 1;
			unsigned char TXEACKE : 1;
#endif
	} BIT;
	} IEIET;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RXEPE : 1;
			unsigned char RXEDLE : 1;
			unsigned char RXERTME : 1;
			unsigned char RXEOVE : 1;
			unsigned char RXEDE : 1;
			unsigned char RXF : 1;
			unsigned char RXS : 1;
			unsigned char RXBSY : 1;
#else
			unsigned char RXBSY : 1;
			unsigned char RXS : 1;
			unsigned char RXF : 1;
			unsigned char RXEDE : 1;
			unsigned char RXEOVE : 1;
			unsigned char RXERTME : 1;
			unsigned char RXEDLE : 1;
			unsigned char RXEPE : 1;
#endif
	} BIT;
	} IERSR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RXEPEE : 1;
			unsigned char RXEDLEE : 1;
			unsigned char RXERTMEE : 1;
			unsigned char RXEOVEE : 1;
			unsigned char RXEDEE : 1;
			unsigned char RXFE : 1;
			unsigned char RXSE : 1;
			unsigned char RXBSYE : 1;
#else
			unsigned char RXBSYE : 1;
			unsigned char RXSE : 1;
			unsigned char RXFE : 1;
			unsigned char RXEDEE : 1;
			unsigned char RXEOVEE : 1;
			unsigned char RXERTMEE : 1;
			unsigned char RXEDLEE : 1;
			unsigned char RXEPEE : 1;
#endif
	} BIT;
	} IEIER;
	char           wk3[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CKS : 3;
			unsigned char SRSTP : 1;
			unsigned char CKS3 : 1;
			unsigned char FCKS : 2;
			unsigned char FLT : 1;
#else
			unsigned char FLT : 1;
			unsigned char FCKS : 2;
			unsigned char CKS3 : 1;
			unsigned char SRSTP : 1;
			unsigned char CKS : 3;
#endif
	} BIT;
	} IECKSR;
	char           wk4[230];
	unsigned char  IETB[33];
	char           wk5[223];
	unsigned char  IERB[33];
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
			unsigned char : 7;
			unsigned char RSTIRQS : 1;
#else
			unsigned char RSTIRQS : 1;
			unsigned char : 7;
#endif
	} BIT;
	} IWDTRCR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 7;
			unsigned char SLCSTP : 1;
#else
			unsigned char SLCSTP : 1;
			unsigned char : 7;
#endif
	} BIT;
	} IWDTCSTPR;
};

struct st_mpc {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CS0E : 1;
			unsigned char CS1E : 1;
			unsigned char CS2E : 1;
			unsigned char CS3E : 1;
			unsigned char CS4E : 1;
			unsigned char CS5E : 1;
			unsigned char CS6E : 1;
			unsigned char CS7E : 1;
#else
			unsigned char CS7E : 1;
			unsigned char CS6E : 1;
			unsigned char CS5E : 1;
			unsigned char CS4E : 1;
			unsigned char CS3E : 1;
			unsigned char CS2E : 1;
			unsigned char CS1E : 1;
			unsigned char CS0E : 1;
#endif
	} BIT;
	} PFCSE;
	char           wk0[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CS0S : 1;
			unsigned char  : 1;
			unsigned char CS1S : 2;
			unsigned char CS2S : 2;
			unsigned char CS3S : 2;
#else
			unsigned char CS3S : 2;
			unsigned char CS2S : 2;
			unsigned char CS1S : 2;
			unsigned char  : 1;
			unsigned char CS0S : 1;
#endif
	} BIT;
	} PFCSS0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CS4S : 2;
			unsigned char CS5S : 2;
			unsigned char CS6S : 2;
			unsigned char CS7S : 2;
#else
			unsigned char CS7S : 2;
			unsigned char CS6S : 2;
			unsigned char CS5S : 2;
			unsigned char CS4S : 2;
#endif
	} BIT;
	} PFCSS1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char A8E : 1;
			unsigned char A9E : 1;
			unsigned char A10E : 1;
			unsigned char A11E : 1;
			unsigned char A12E : 1;
			unsigned char A13E : 1;
			unsigned char A14E : 1;
			unsigned char A15E : 1;
#else
			unsigned char A15E : 1;
			unsigned char A14E : 1;
			unsigned char A13E : 1;
			unsigned char A12E : 1;
			unsigned char A11E : 1;
			unsigned char A10E : 1;
			unsigned char A9E : 1;
			unsigned char A8E : 1;
#endif
	} BIT;
	} PFAOE0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char A16E : 1;
			unsigned char A17E : 1;
			unsigned char A18E : 1;
			unsigned char A19E : 1;
			unsigned char A20E : 1;
			unsigned char A21E : 1;
			unsigned char A22E : 1;
			unsigned char A23E : 1;
#else
			unsigned char A23E : 1;
			unsigned char A22E : 1;
			unsigned char A21E : 1;
			unsigned char A20E : 1;
			unsigned char A19E : 1;
			unsigned char A18E : 1;
			unsigned char A17E : 1;
			unsigned char A16E : 1;
#endif
	} BIT;
	} PFAOE1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADRLE : 1;
			unsigned char ADRHMS : 1;
			unsigned char  : 2;
			unsigned char DHE : 1;
			unsigned char DH32E : 1;
			unsigned char WR1BC1E : 1;
			unsigned char WR32BC32E : 1;
#else
			unsigned char WR32BC32E : 1;
			unsigned char WR1BC1E : 1;
			unsigned char DH32E : 1;
			unsigned char DHE : 1;
			unsigned char  : 2;
			unsigned char ADRHMS : 1;
			unsigned char ADRLE : 1;
#endif
	} BIT;
	} PFBCR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char WAITS : 2;
			unsigned char ALEOE : 1;
			unsigned char  : 1;
			unsigned char MDSDE : 1;
			unsigned char  : 1;
			unsigned char DQM1E : 1;
			unsigned char SDCLKE : 1;
#else
			unsigned char SDCLKE : 1;
			unsigned char DQM1E : 1;
			unsigned char  : 1;
			unsigned char MDSDE : 1;
			unsigned char  : 1;
			unsigned char ALEOE : 1;
			unsigned char WAITS : 2;
#endif
	} BIT;
	} PFBCR1;
	char           wk1[6];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 4;
			unsigned char PHYMODE : 1;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PHYMODE : 1;
			unsigned char : 4;
#endif
	} BIT;
	} PFENET;
	char           wk2[5];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 2;
			unsigned char PUPHZS : 1;
			unsigned char PDHZS : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char PDHZS : 1;
			unsigned char PUPHZS : 1;
			unsigned char : 2;
#endif
	} BIT;
	} PFUSB0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 2;
			unsigned char PUPHZS : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char PUPHZS : 1;
			unsigned char : 2;
#endif
	} BIT;
	} PFUSB1;
	char           wk3[9];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char PFSWE : 1;
			unsigned char B0WI : 1;
#else
			unsigned char B0WI : 1;
			unsigned char PFSWE : 1;
			unsigned char : 6;
#endif
	} BIT;
	} PWPR;
	char           wk4[32];
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
	} P00PFS;
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
	} P01PFS;
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
	} P02PFS;
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
	} P03PFS;
	char           wk5[1];
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
	} P05PFS;
	char           wk6[1];
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
	} P07PFS;
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
	} P20PFS;
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
	char           wk7[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char : 6;
#endif
	} BIT;
	} P40PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char : 6;
#endif
	} BIT;
	} P41PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char : 6;
#endif
	} BIT;
	} P42PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char : 6;
#endif
	} BIT;
	} P43PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char : 6;
#endif
	} BIT;
	} P44PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char : 6;
#endif
	} BIT;
	} P45PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char : 6;
#endif
	} BIT;
	} P46PFS;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char ISEL : 1;
			unsigned char ASEL : 1;
#else
			unsigned char ASEL : 1;
			unsigned char ISEL : 1;
			unsigned char : 6;
#endif
	} BIT;
	} P47PFS;
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
	} P50PFS;
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
	} P54PFS;
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
	} P55PFS;
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
	} P56PFS;
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
	} P57PFS;
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
	} P60PFS;
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
	} P61PFS;
	char           wk9[4];
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
	} P66PFS;
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
	} P67PFS;
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
	} P77PFS;
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
	char           wk10[2];
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
	} P86PFS;
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
	} P87PFS;
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
	} P90PFS;
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
	} P91PFS;
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
	} P92PFS;
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
	} P93PFS;
	char           wk11[4];
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
			unsigned char  : 1;
#else
			unsigned char  : 1;
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
			unsigned char  : 1;
#else
			unsigned char  : 1;
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
	} PC0PFS;
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
	} PC6PFS;
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
	} PE6PFS;
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
	} PE7PFS;
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
	char           wk12[2];
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
	} PF5PFS;
	char           wk13[21];
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
};

struct st_mpu {
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long : 4;
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
			unsigned long : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long : 4;
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
			unsigned long : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long : 4;
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
			unsigned long : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long : 4;
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
			unsigned long : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long : 4;
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
			unsigned long : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long : 4;
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
			unsigned long : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long : 4;
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
			unsigned long : 4;
			unsigned long RSPN : 28;
#else
			unsigned long RSPN : 28;
			unsigned long : 4;
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
			unsigned long : 1;
			unsigned long UBAC : 3;
			unsigned long  : 28;
#else
			unsigned long  : 28;
			unsigned long UBAC : 3;
			unsigned long : 1;
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
			unsigned long : 1;
			unsigned long UHACI : 3;
			unsigned long  : 12;
			unsigned long HITI : 8;
			unsigned long  : 8;
#else
			unsigned long  : 8;
			unsigned long HITI : 8;
			unsigned long  : 12;
			unsigned long UHACI : 3;
			unsigned long : 1;
#endif
	} BIT;
	} MHITI;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long : 1;
			unsigned long UHACD : 3;
			unsigned long  : 12;
			unsigned long HITD : 8;
			unsigned long  : 8;
#else
			unsigned long  : 8;
			unsigned long HITD : 8;
			unsigned long  : 12;
			unsigned long UHACD : 3;
			unsigned long : 1;
#endif
	} BIT;
	} MHITD;
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
			unsigned char : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char : 7;
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
			unsigned char : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char : 7;
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
			unsigned char : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char : 7;
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
			unsigned char : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char : 7;
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
			unsigned char : 7;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char : 7;
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
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short OIE1 : 1;
			unsigned short OCE1 : 1;
			unsigned short  : 5;
			unsigned short OSF1 : 1;
#else
			unsigned short OSF1 : 1;
			unsigned short  : 5;
			unsigned short OCE1 : 1;
			unsigned short OIE1 : 1;
			unsigned short : 8;
#endif
	} BIT;
	} OCSR1;
	char           wk0[4];
	union {
		unsigned short WORD;
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
			unsigned char : 4;
			unsigned char P3CZEA : 1;
			unsigned char P2CZEA : 1;
			unsigned char P1CZEA : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char P1CZEA : 1;
			unsigned char P2CZEA : 1;
			unsigned char P3CZEA : 1;
			unsigned char : 4;
#endif
	} BIT;
	} POECR2;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 9;
			unsigned short OSTSTE : 1;
			unsigned short  : 2;
			unsigned short OSTSTF : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short OSTSTF : 1;
			unsigned short  : 2;
			unsigned short OSTSTE : 1;
			unsigned short : 9;
#endif
	} BIT;
	} ICSR3;
};

struct st_pdc {
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long PCKE : 1;
			unsigned long VPS : 1;
			unsigned long HPS : 1;
			unsigned long PRST : 1;
			unsigned long DFIE : 1;
			unsigned long FEIE : 1;
			unsigned long OVIE : 1;
			unsigned long UDRIE : 1;
			unsigned long VERIE : 1;
			unsigned long HERIE : 1;
			unsigned long PCKOE : 1;
			unsigned long PCKDIV : 3;
			unsigned long EDS : 1;
			unsigned long  : 17;
#else
			unsigned long  : 17;
			unsigned long EDS : 1;
			unsigned long PCKDIV : 3;
			unsigned long PCKOE : 1;
			unsigned long HERIE : 1;
			unsigned long VERIE : 1;
			unsigned long UDRIE : 1;
			unsigned long OVIE : 1;
			unsigned long FEIE : 1;
			unsigned long DFIE : 1;
			unsigned long PRST : 1;
			unsigned long HPS : 1;
			unsigned long VPS : 1;
			unsigned long PCKE : 1;
#endif
	} BIT;
	} PCCR0;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long PCE : 1;
			unsigned long  : 31;
#else
			unsigned long  : 31;
			unsigned long PCE : 1;
#endif
	} BIT;
	} PCCR1;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long FBSY : 1;
			unsigned long FEMPF : 1;
			unsigned long FEF : 1;
			unsigned long OVRF : 1;
			unsigned long UDRF : 1;
			unsigned long VERF : 1;
			unsigned long HERF : 1;
			unsigned long  : 25;
#else
			unsigned long  : 25;
			unsigned long HERF : 1;
			unsigned long VERF : 1;
			unsigned long UDRF : 1;
			unsigned long OVRF : 1;
			unsigned long FEF : 1;
			unsigned long FEMPF : 1;
			unsigned long FBSY : 1;
#endif
	} BIT;
	} PCSR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long VSYNC : 1;
			unsigned long HSYNC : 1;
			unsigned long  : 30;
#else
			unsigned long  : 30;
			unsigned long HSYNC : 1;
			unsigned long VSYNC : 1;
#endif
	} BIT;
	} PCMONR;
	union {
		unsigned long LONG;
	} PCDR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long VST : 12;
			unsigned long  : 4;
			unsigned long VSZ : 12;
			unsigned long  : 4;
#else
			unsigned long  : 4;
			unsigned long VSZ : 12;
			unsigned long  : 4;
			unsigned long VST : 12;
#endif
	} BIT;
	} VCR;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long HST : 12;
			unsigned long  : 4;
			unsigned long HSZ : 12;
			unsigned long  : 4;
#else
			unsigned long  : 4;
			unsigned long HSZ : 12;
			unsigned long  : 4;
			unsigned long HST : 12;
#endif
	} BIT;
	} HCR;
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
			unsigned char : 6;
			unsigned char PSEL6 : 1;
			unsigned char PSEL7 : 1;
#else
			unsigned char PSEL7 : 1;
			unsigned char PSEL6 : 1;
			unsigned char : 6;
#endif
	} BIT;
	} PSRA;
};

struct st_port0 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
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
			unsigned char : 2;
			unsigned char B2 : 1;
			unsigned char  : 3;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char  : 3;
			unsigned char B2 : 1;
			unsigned char : 2;
#endif
	} BIT;
	} ODR1;
	char           wk4[62];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char B0 : 1;
			unsigned char B1 : 1;
			unsigned char B2 : 1;
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
};

struct st_port1 {
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
			unsigned char : 7;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char : 7;
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
			unsigned char  : 1;
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
			unsigned char  : 1;
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
	char           wk3[35];
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
	char           wk4[58];
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
	char           wk3[36];
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
	char           wk4[57];
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
			unsigned char  : 3;
			unsigned char B6 : 1;
			unsigned char B7 : 1;
#else
			unsigned char B7 : 1;
			unsigned char B6 : 1;
			unsigned char  : 3;
			unsigned char B2 : 1;
			unsigned char B1 : 1;
			unsigned char B0 : 1;
#endif
	} BIT;
	} DSCR;
};

struct st_port6 {
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
	char           wk3[37];
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
	char           wk4[56];
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

struct st_port7 {
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

struct st_port8 {
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

struct st_port9 {
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

struct st_portf {
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
	} ODR1;
	char           wk4[47];
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
};

struct st_portg {
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
	char           wk4[46];
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
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
			unsigned char : 3;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char : 3;
#endif
	} BIT;
	} PDR;
	char           wk0[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 3;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char : 3;
#endif
	} BIT;
	} PODR;
	char           wk1[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 3;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char : 3;
#endif
	} BIT;
	} PIDR;
	char           wk2[31];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 3;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char : 3;
#endif
	} BIT;
	} PMR;
	char           wk3[49];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 6;
			unsigned char B6 : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char B6 : 1;
			unsigned char : 6;
#endif
	} BIT;
	} ODR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 2;
			unsigned char B2 : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char B2 : 1;
			unsigned char : 2;
#endif
	} BIT;
	} ODR1;
	char           wk4[44];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 3;
			unsigned char B3 : 1;
			unsigned char  : 1;
			unsigned char B5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char B5 : 1;
			unsigned char  : 1;
			unsigned char B3 : 1;
			unsigned char : 3;
#endif
	} BIT;
	} PCR;
};

struct st_ppg0 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char G0CMS : 2;
			unsigned char G1CMS : 2;
			unsigned char G2CMS : 2;
			unsigned char G3CMS : 2;
#else
			unsigned char G3CMS : 2;
			unsigned char G2CMS : 2;
			unsigned char G1CMS : 2;
			unsigned char G0CMS : 2;
#endif
	} BIT;
	} PCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char G0NOV : 1;
			unsigned char G1NOV : 1;
			unsigned char G2NOV : 1;
			unsigned char G3NOV : 1;
			unsigned char G0INV : 1;
			unsigned char G1INV : 1;
			unsigned char G2INV : 1;
			unsigned char G3INV : 1;
#else
			unsigned char G3INV : 1;
			unsigned char G2INV : 1;
			unsigned char G1INV : 1;
			unsigned char G0INV : 1;
			unsigned char G3NOV : 1;
			unsigned char G2NOV : 1;
			unsigned char G1NOV : 1;
			unsigned char G0NOV : 1;
#endif
	} BIT;
	} PMR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDER8 : 1;
			unsigned char NDER9 : 1;
			unsigned char NDER10 : 1;
			unsigned char NDER11 : 1;
			unsigned char NDER12 : 1;
			unsigned char NDER13 : 1;
			unsigned char NDER14 : 1;
			unsigned char NDER15 : 1;
#else
			unsigned char NDER15 : 1;
			unsigned char NDER14 : 1;
			unsigned char NDER13 : 1;
			unsigned char NDER12 : 1;
			unsigned char NDER11 : 1;
			unsigned char NDER10 : 1;
			unsigned char NDER9 : 1;
			unsigned char NDER8 : 1;
#endif
	} BIT;
	} NDERH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDER0 : 1;
			unsigned char NDER1 : 1;
			unsigned char NDER2 : 1;
			unsigned char NDER3 : 1;
			unsigned char NDER4 : 1;
			unsigned char NDER5 : 1;
			unsigned char NDER6 : 1;
			unsigned char NDER7 : 1;
#else
			unsigned char NDER7 : 1;
			unsigned char NDER6 : 1;
			unsigned char NDER5 : 1;
			unsigned char NDER4 : 1;
			unsigned char NDER3 : 1;
			unsigned char NDER2 : 1;
			unsigned char NDER1 : 1;
			unsigned char NDER0 : 1;
#endif
	} BIT;
	} NDERL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POD8 : 1;
			unsigned char POD9 : 1;
			unsigned char POD10 : 1;
			unsigned char POD11 : 1;
			unsigned char POD12 : 1;
			unsigned char POD13 : 1;
			unsigned char POD14 : 1;
			unsigned char POD15 : 1;
#else
			unsigned char POD15 : 1;
			unsigned char POD14 : 1;
			unsigned char POD13 : 1;
			unsigned char POD12 : 1;
			unsigned char POD11 : 1;
			unsigned char POD10 : 1;
			unsigned char POD9 : 1;
			unsigned char POD8 : 1;
#endif
	} BIT;
	} PODRH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POD0 : 1;
			unsigned char POD1 : 1;
			unsigned char POD2 : 1;
			unsigned char POD3 : 1;
			unsigned char POD4 : 1;
			unsigned char POD5 : 1;
			unsigned char POD6 : 1;
			unsigned char POD7 : 1;
#else
			unsigned char POD7 : 1;
			unsigned char POD6 : 1;
			unsigned char POD5 : 1;
			unsigned char POD4 : 1;
			unsigned char POD3 : 1;
			unsigned char POD2 : 1;
			unsigned char POD1 : 1;
			unsigned char POD0 : 1;
#endif
	} BIT;
	} PODRL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDR8 : 1;
			unsigned char NDR9 : 1;
			unsigned char NDR10 : 1;
			unsigned char NDR11 : 1;
			unsigned char NDR12 : 1;
			unsigned char NDR13 : 1;
			unsigned char NDR14 : 1;
			unsigned char NDR15 : 1;
#else
			unsigned char NDR15 : 1;
			unsigned char NDR14 : 1;
			unsigned char NDR13 : 1;
			unsigned char NDR12 : 1;
			unsigned char NDR11 : 1;
			unsigned char NDR10 : 1;
			unsigned char NDR9 : 1;
			unsigned char NDR8 : 1;
#endif
	} BIT;
	} NDRH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDR0 : 1;
			unsigned char NDR1 : 1;
			unsigned char NDR2 : 1;
			unsigned char NDR3 : 1;
			unsigned char NDR4 : 1;
			unsigned char NDR5 : 1;
			unsigned char NDR6 : 1;
			unsigned char NDR7 : 1;
#else
			unsigned char NDR7 : 1;
			unsigned char NDR6 : 1;
			unsigned char NDR5 : 1;
			unsigned char NDR4 : 1;
			unsigned char NDR3 : 1;
			unsigned char NDR2 : 1;
			unsigned char NDR1 : 1;
			unsigned char NDR0 : 1;
#endif
	} BIT;
	} NDRL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDR8 : 1;
			unsigned char NDR9 : 1;
			unsigned char NDR10 : 1;
			unsigned char NDR11 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char NDR11 : 1;
			unsigned char NDR10 : 1;
			unsigned char NDR9 : 1;
			unsigned char NDR8 : 1;
#endif
	} BIT;
	} NDRH2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDR0 : 1;
			unsigned char NDR1 : 1;
			unsigned char NDR2 : 1;
			unsigned char NDR3 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char NDR3 : 1;
			unsigned char NDR2 : 1;
			unsigned char NDR1 : 1;
			unsigned char NDR0 : 1;
#endif
	} BIT;
	} NDRL2;
};

struct st_ppg1 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PTRSL : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char PTRSL : 1;
#endif
	} BIT;
	} PTRSLR;
	char           wk0[5];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char G0CMS : 2;
			unsigned char G1CMS : 2;
			unsigned char G2CMS : 2;
			unsigned char G3CMS : 2;
#else
			unsigned char G3CMS : 2;
			unsigned char G2CMS : 2;
			unsigned char G1CMS : 2;
			unsigned char G0CMS : 2;
#endif
	} BIT;
	} PCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char G0NOV : 1;
			unsigned char G1NOV : 1;
			unsigned char G2NOV : 1;
			unsigned char G3NOV : 1;
			unsigned char G0INV : 1;
			unsigned char G1INV : 1;
			unsigned char G2INV : 1;
			unsigned char G3INV : 1;
#else
			unsigned char G3INV : 1;
			unsigned char G2INV : 1;
			unsigned char G1INV : 1;
			unsigned char G0INV : 1;
			unsigned char G3NOV : 1;
			unsigned char G2NOV : 1;
			unsigned char G1NOV : 1;
			unsigned char G0NOV : 1;
#endif
	} BIT;
	} PMR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDER24 : 1;
			unsigned char NDER25 : 1;
			unsigned char NDER26 : 1;
			unsigned char NDER27 : 1;
			unsigned char NDER28 : 1;
			unsigned char NDER29 : 1;
			unsigned char NDER30 : 1;
			unsigned char NDER31 : 1;
#else
			unsigned char NDER31 : 1;
			unsigned char NDER30 : 1;
			unsigned char NDER29 : 1;
			unsigned char NDER28 : 1;
			unsigned char NDER27 : 1;
			unsigned char NDER26 : 1;
			unsigned char NDER25 : 1;
			unsigned char NDER24 : 1;
#endif
	} BIT;
	} NDERH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDER16 : 1;
			unsigned char NDER17 : 1;
			unsigned char NDER18 : 1;
			unsigned char NDER19 : 1;
			unsigned char NDER20 : 1;
			unsigned char NDER21 : 1;
			unsigned char NDER22 : 1;
			unsigned char NDER23 : 1;
#else
			unsigned char NDER23 : 1;
			unsigned char NDER22 : 1;
			unsigned char NDER21 : 1;
			unsigned char NDER20 : 1;
			unsigned char NDER19 : 1;
			unsigned char NDER18 : 1;
			unsigned char NDER17 : 1;
			unsigned char NDER16 : 1;
#endif
	} BIT;
	} NDERL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POD24 : 1;
			unsigned char POD25 : 1;
			unsigned char POD26 : 1;
			unsigned char POD27 : 1;
			unsigned char POD28 : 1;
			unsigned char POD29 : 1;
			unsigned char POD30 : 1;
			unsigned char POD31 : 1;
#else
			unsigned char POD31 : 1;
			unsigned char POD30 : 1;
			unsigned char POD29 : 1;
			unsigned char POD28 : 1;
			unsigned char POD27 : 1;
			unsigned char POD26 : 1;
			unsigned char POD25 : 1;
			unsigned char POD24 : 1;
#endif
	} BIT;
	} PODRH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char POD16 : 1;
			unsigned char POD17 : 1;
			unsigned char POD18 : 1;
			unsigned char POD19 : 1;
			unsigned char POD20 : 1;
			unsigned char POD21 : 1;
			unsigned char POD22 : 1;
			unsigned char POD23 : 1;
#else
			unsigned char POD23 : 1;
			unsigned char POD22 : 1;
			unsigned char POD21 : 1;
			unsigned char POD20 : 1;
			unsigned char POD19 : 1;
			unsigned char POD18 : 1;
			unsigned char POD17 : 1;
			unsigned char POD16 : 1;
#endif
	} BIT;
	} PODRL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDR24 : 1;
			unsigned char NDR25 : 1;
			unsigned char NDR26 : 1;
			unsigned char NDR27 : 1;
			unsigned char NDR28 : 1;
			unsigned char NDR29 : 1;
			unsigned char NDR30 : 1;
			unsigned char NDR31 : 1;
#else
			unsigned char NDR31 : 1;
			unsigned char NDR30 : 1;
			unsigned char NDR29 : 1;
			unsigned char NDR28 : 1;
			unsigned char NDR27 : 1;
			unsigned char NDR26 : 1;
			unsigned char NDR25 : 1;
			unsigned char NDR24 : 1;
#endif
	} BIT;
	} NDRH;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDR16 : 1;
			unsigned char NDR17 : 1;
			unsigned char NDR18 : 1;
			unsigned char NDR19 : 1;
			unsigned char NDR20 : 1;
			unsigned char NDR21 : 1;
			unsigned char NDR22 : 1;
			unsigned char NDR23 : 1;
#else
			unsigned char NDR23 : 1;
			unsigned char NDR22 : 1;
			unsigned char NDR21 : 1;
			unsigned char NDR20 : 1;
			unsigned char NDR19 : 1;
			unsigned char NDR18 : 1;
			unsigned char NDR17 : 1;
			unsigned char NDR16 : 1;
#endif
	} BIT;
	} NDRL;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDR24 : 1;
			unsigned char NDR25 : 1;
			unsigned char NDR26 : 1;
			unsigned char NDR27 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char NDR27 : 1;
			unsigned char NDR26 : 1;
			unsigned char NDR25 : 1;
			unsigned char NDR24 : 1;
#endif
	} BIT;
	} NDRH2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char NDR16 : 1;
			unsigned char NDR17 : 1;
			unsigned char NDR18 : 1;
			unsigned char NDR19 : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char NDR19 : 1;
			unsigned char NDR18 : 1;
			unsigned char NDR17 : 1;
			unsigned char NDR16 : 1;
#endif
	} BIT;
	} NDRL2;
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
			unsigned char : 1;
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
			unsigned char : 1;
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
			unsigned char TMWE : 1;
			unsigned char SDDL : 3;
			unsigned char DLCS : 1;
#else
			unsigned char DLCS : 1;
			unsigned char SDDL : 3;
			unsigned char TMWE : 1;
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
			unsigned char FMPE : 1;
#else
			unsigned char FMPE : 1;
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
			unsigned char  BYTE;
	   } TMOCNTL;
	};
	union {
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
			unsigned char  BYTE;
	   } TMOCNTU;
	};
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
	unsigned char SPBR;
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
			unsigned char  : 4;
#else
			unsigned char  : 4;
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
			unsigned char : 7;
			unsigned char ENB : 1;
#else
			unsigned char ENB : 1;
			unsigned char : 7;
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
			unsigned char  : 1;
			unsigned char PES : 4;
#else
			unsigned char PES : 4;
			unsigned char  : 1;
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
			unsigned char  : 1;
#else
			unsigned char  : 1;
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
	char           wk16[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char RCKSEL : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char RCKSEL : 1;
#endif
	} BIT;
	} RCR4;
	char           wk17[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFC : 1;
			unsigned short  : 15;
#else
			unsigned short  : 15;
			unsigned short RFC : 1;
#endif
	} BIT;
	} RFRH;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RFC : 16;
#else
			unsigned short RFC : 16;
#endif
	} BIT;
	} RFRL;
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
	char           wk18[17];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TCCT : 2;
			unsigned char TCST : 1;
			unsigned char  : 1;
			unsigned char TCNF : 2;
			unsigned char  : 1;
			unsigned char TCEN : 1;
#else
			unsigned char TCEN : 1;
			unsigned char  : 1;
			unsigned char TCNF : 2;
			unsigned char  : 1;
			unsigned char TCST : 1;
			unsigned char TCCT : 2;
#endif
	} BIT;
	} RTCCR0;
	char           wk19[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TCCT : 2;
			unsigned char TCST : 1;
			unsigned char  : 1;
			unsigned char TCNF : 2;
			unsigned char  : 1;
			unsigned char TCEN : 1;
#else
			unsigned char TCEN : 1;
			unsigned char  : 1;
			unsigned char TCNF : 2;
			unsigned char  : 1;
			unsigned char TCST : 1;
			unsigned char TCCT : 2;
#endif
	} BIT;
	} RTCCR1;
	char           wk20[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TCCT : 2;
			unsigned char TCST : 1;
			unsigned char  : 1;
			unsigned char TCNF : 2;
			unsigned char  : 1;
			unsigned char TCEN : 1;
#else
			unsigned char TCEN : 1;
			unsigned char  : 1;
			unsigned char TCNF : 2;
			unsigned char  : 1;
			unsigned char TCST : 1;
			unsigned char TCCT : 2;
#endif
	} BIT;
	} RTCCR2;
	char           wk21[13];
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
	} RSECCP0;
	char           wk22[1];
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
	} RMINCP0;
	char           wk23[1];
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
	} RHRCP0;
	char           wk24[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DATE1 : 4;
			unsigned char DATE10 : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char DATE10 : 3;
			unsigned char DATE1 : 4;
#endif
	} BIT;
	} RDAYCP0;
	char           wk25[1];
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
	} RMONCP0;
	char           wk26[5];
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
	} RSECCP1;
	char           wk27[1];
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
	} RMINCP1;
	char           wk28[1];
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
	} RHRCP1;
	char           wk29[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DATE1 : 4;
			unsigned char DATE10 : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char DATE10 : 3;
			unsigned char DATE1 : 4;
#endif
	} BIT;
	} RDAYCP1;
	char           wk30[1];
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
	} RMONCP1;
	char           wk31[5];
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
	} RSECCP2;
	char           wk32[1];
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
	} RMINCP2;
	char           wk33[1];
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
	} RHRCP2;
	char           wk34[3];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DATE1 : 4;
			unsigned char DATE10 : 3;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char DATE10 : 3;
			unsigned char DATE1 : 4;
#endif
	} BIT;
	} RDAYCP2;
	char           wk35[1];
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
	} RMONCP2;
};

struct st_s12ad {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char EXTRG : 1;
			unsigned char TRGE : 1;
			unsigned char CKS : 2;
			unsigned char ADIE : 1;
			unsigned char  : 1;
			unsigned char ADCS : 1;
			unsigned char ADST : 1;
#else
			unsigned char ADST : 1;
			unsigned char ADCS : 1;
			unsigned char  : 1;
			unsigned char ADIE : 1;
			unsigned char CKS : 2;
			unsigned char TRGE : 1;
			unsigned char EXTRG : 1;
#endif
	} BIT;
	} ADCSR;
	char           wk0[3];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANS0 : 16;
#else
			unsigned short ANS0 : 16;
#endif
	} BIT;
	} ADANS0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ANS1 : 5;
			unsigned short  : 11;
#else
			unsigned short  : 11;
			unsigned short ANS1 : 5;
#endif
	} BIT;
	} ADANS1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ADS0 : 16;
#else
			unsigned short ADS0 : 16;
#endif
	} BIT;
	} ADADS0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ADS1 : 5;
			unsigned short  : 11;
#else
			unsigned short  : 11;
			unsigned short ADS1 : 5;
#endif
	} BIT;
	} ADADS1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADC : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char ADC : 2;
#endif
	} BIT;
	} ADADC;
	char           wk1[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 5;
			unsigned short ACE : 1;
			unsigned short  : 9;
			unsigned short ADRFMT : 1;
#else
			unsigned short ADRFMT : 1;
			unsigned short  : 9;
			unsigned short ACE : 1;
			unsigned short : 5;
#endif
	} BIT;
	} ADCER;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char ADSTRS : 4;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char ADSTRS : 4;
#endif
	} BIT;
	} ADSTRGR;
	char           wk2[1];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short TSSAD : 1;
			unsigned short OCSAD : 1;
			unsigned short  : 6;
			unsigned short TSS : 1;
			unsigned short OCS : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short OCS : 1;
			unsigned short TSS : 1;
			unsigned short  : 6;
			unsigned short OCSAD : 1;
			unsigned short TSSAD : 1;
#endif
	} BIT;
	} ADEXICR;
	char           wk3[6];
	unsigned short ADTSDR;
	unsigned short ADOCDR;
	char           wk4[2];
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
	unsigned short ADDR12;
	unsigned short ADDR13;
	unsigned short ADDR14;
	unsigned short ADDR15;
	unsigned short ADDR16;
	unsigned short ADDR17;
	unsigned short ADDR18;
	unsigned short ADDR19;
	unsigned short ADDR20;
	char           wk5[22];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short SST1 : 8;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short SST1 : 8;
#endif
	} BIT;
	} ADSSTR01;
	char           wk6[14];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short SST2 : 8;
#else
			unsigned short SST2 : 8;
			unsigned short : 8;
#endif
	} BIT;
	} ADSSTR23;
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
			unsigned char  : 3;
			unsigned char BCP2 : 1;
#else
			unsigned char BCP2 : 1;
			unsigned char  : 3;
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
			unsigned char  : 3;
			unsigned char ABCS : 1;
			unsigned char NFEN : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFEN : 1;
			unsigned char ABCS : 1;
			unsigned char  : 3;
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
			unsigned char  : 3;
			unsigned char BCP2 : 1;
#else
			unsigned char BCP2 : 1;
			unsigned char  : 3;
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
			unsigned char  : 3;
			unsigned char ABCS : 1;
			unsigned char NFEN : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char NFEN : 1;
			unsigned char ABCS : 1;
			unsigned char  : 3;
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
	char           wk0[18];
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
			unsigned char : 1;
			unsigned char SFSF : 1;
			unsigned char RXDSF : 1;
			unsigned char BRME : 1;
			unsigned char  : 4;
#else
			unsigned char  : 4;
			unsigned char BRME : 1;
			unsigned char RXDSF : 1;
			unsigned char SFSF : 1;
			unsigned char : 1;
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

struct st_smci0 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CKS : 2;
			unsigned char BCP : 2;
			unsigned char PM : 1;
			unsigned char PE : 1;
			unsigned char BCLK : 1;
			unsigned char GM : 1;
#else
			unsigned char GM : 1;
			unsigned char BCLK : 1;
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
			unsigned char  : 2;
#else
			unsigned char  : 2;
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
			unsigned char  : 3;
			unsigned char BCP2 : 1;
#else
			unsigned char BCP2 : 1;
			unsigned char  : 3;
			unsigned char SDIR : 1;
			unsigned char SINV : 1;
			unsigned char  : 1;
			unsigned char SMIF : 1;
#endif
	} BIT;
	} SCMR;
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
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 5;
			unsigned short UBTS : 1;
			unsigned short  : 10;
#else
			unsigned short  : 10;
			unsigned short UBTS : 1;
			unsigned short : 5;
#endif
	} BIT;
	} MDSR;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short ROME : 1;
			unsigned short EXBE : 1;
			unsigned short  : 6;
			unsigned short KEY : 8;
#else
			unsigned short KEY : 8;
			unsigned short  : 6;
			unsigned short EXBE : 1;
			unsigned short ROME : 1;
#endif
	} BIT;
	} SYSCR0;
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
			unsigned short : 14;
			unsigned short OPE : 1;
			unsigned short SSBY : 1;
#else
			unsigned short SSBY : 1;
			unsigned short OPE : 1;
			unsigned short : 14;
#endif
	} BIT;
	} SBYCR;
	char           wk2[2];
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long : 4;
			unsigned long MSTPA4 : 1;
			unsigned long MSTPA5 : 1;
			unsigned long  : 3;
			unsigned long MSTPA9 : 1;
			unsigned long MSTPA10 : 1;
			unsigned long MSTPA11 : 1;
			unsigned long MSTPA12 : 1;
			unsigned long MSTPA13 : 1;
			unsigned long MSTPA14 : 1;
			unsigned long MSTPA15 : 1;
			unsigned long  : 1;
			unsigned long MSTPA17 : 1;
			unsigned long  : 1;
			unsigned long MSTPA19 : 1;
			unsigned long  : 3;
			unsigned long MSTPA23 : 1;
			unsigned long MSTPA24 : 1;
			unsigned long  : 2;
			unsigned long MSTPA27 : 1;
			unsigned long MSTPA28 : 1;
			unsigned long MSTPA29 : 1;
			unsigned long  : 1;
			unsigned long ACSE : 1;
#else
			unsigned long ACSE : 1;
			unsigned long  : 1;
			unsigned long MSTPA29 : 1;
			unsigned long MSTPA28 : 1;
			unsigned long MSTPA27 : 1;
			unsigned long  : 2;
			unsigned long MSTPA24 : 1;
			unsigned long MSTPA23 : 1;
			unsigned long  : 3;
			unsigned long MSTPA19 : 1;
			unsigned long  : 1;
			unsigned long MSTPA17 : 1;
			unsigned long  : 1;
			unsigned long MSTPA15 : 1;
			unsigned long MSTPA14 : 1;
			unsigned long MSTPA13 : 1;
			unsigned long MSTPA12 : 1;
			unsigned long MSTPA11 : 1;
			unsigned long MSTPA10 : 1;
			unsigned long MSTPA9 : 1;
			unsigned long  : 3;
			unsigned long MSTPA5 : 1;
			unsigned long MSTPA4 : 1;
			unsigned long : 4;
#endif
	} BIT;
	} MSTPCRA;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MSTPB0 : 1;
			unsigned long MSTPB1 : 1;
			unsigned long MSTPB2 : 1;
			unsigned long  : 1;
			unsigned long MSTPB4 : 1;
			unsigned long  : 3;
			unsigned long MSTPB8 : 1;
			unsigned long  : 6;
			unsigned long MSTPB15 : 1;
			unsigned long MSTPB16 : 1;
			unsigned long MSTPB17 : 1;
			unsigned long MSTPB18 : 1;
			unsigned long MSTPB19 : 1;
			unsigned long MSTPB20 : 1;
			unsigned long MSTPB21 : 1;
			unsigned long MSTPB22 : 1;
			unsigned long MSTPB23 : 1;
			unsigned long MSTPB24 : 1;
			unsigned long MSTPB25 : 1;
			unsigned long MSTPB26 : 1;
			unsigned long MSTPB27 : 1;
			unsigned long MSTPB28 : 1;
			unsigned long MSTPB29 : 1;
			unsigned long MSTPB30 : 1;
			unsigned long MSTPB31 : 1;
#else
			unsigned long MSTPB31 : 1;
			unsigned long MSTPB30 : 1;
			unsigned long MSTPB29 : 1;
			unsigned long MSTPB28 : 1;
			unsigned long MSTPB27 : 1;
			unsigned long MSTPB26 : 1;
			unsigned long MSTPB25 : 1;
			unsigned long MSTPB24 : 1;
			unsigned long MSTPB23 : 1;
			unsigned long MSTPB22 : 1;
			unsigned long MSTPB21 : 1;
			unsigned long MSTPB20 : 1;
			unsigned long MSTPB19 : 1;
			unsigned long MSTPB18 : 1;
			unsigned long MSTPB17 : 1;
			unsigned long MSTPB16 : 1;
			unsigned long MSTPB15 : 1;
			unsigned long  : 6;
			unsigned long MSTPB8 : 1;
			unsigned long  : 3;
			unsigned long MSTPB4 : 1;
			unsigned long  : 1;
			unsigned long MSTPB2 : 1;
			unsigned long MSTPB1 : 1;
			unsigned long MSTPB0 : 1;
#endif
	} BIT;
	} MSTPCRB;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long MSTPC0 : 1;
			unsigned long MSTPC1 : 1;
			unsigned long  : 14;
			unsigned long MSTPC16 : 1;
			unsigned long MSTPC17 : 1;
			unsigned long MSTPC18 : 1;
			unsigned long MSTPC19 : 1;
			unsigned long  : 2;
			unsigned long MSTPC22 : 1;
			unsigned long  : 1;
			unsigned long MSTPC24 : 1;
			unsigned long MSTPC25 : 1;
			unsigned long MSTPC26 : 1;
			unsigned long MSTPC27 : 1;
			unsigned long  : 4;
#else
			unsigned long  : 4;
			unsigned long MSTPC27 : 1;
			unsigned long MSTPC26 : 1;
			unsigned long MSTPC25 : 1;
			unsigned long MSTPC24 : 1;
			unsigned long  : 1;
			unsigned long MSTPC22 : 1;
			unsigned long  : 2;
			unsigned long MSTPC19 : 1;
			unsigned long MSTPC18 : 1;
			unsigned long MSTPC17 : 1;
			unsigned long MSTPC16 : 1;
			unsigned long  : 14;
			unsigned long MSTPC1 : 1;
			unsigned long MSTPC0 : 1;
#endif
	} BIT;
	} MSTPCRC;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long  : 31;
			unsigned long MSTPD31 : 1;
#else
			unsigned long MSTPD31 : 1;
			unsigned long  : 31;
#endif
	} BIT;
	} MSTPCRD;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long : 8;
			unsigned long PCKB : 4;
			unsigned long PCKA : 4;
			unsigned long BCK : 4;
			unsigned long  : 2;
			unsigned long PSTOP0 : 1;
			unsigned long PSTOP1 : 1;
			unsigned long ICK : 4;
			unsigned long FCK : 4;
#else
			unsigned long FCK : 4;
			unsigned long ICK : 4;
			unsigned long PSTOP1 : 1;
			unsigned long PSTOP0 : 1;
			unsigned long  : 2;
			unsigned long BCK : 4;
			unsigned long PCKA : 4;
			unsigned long PCKB : 4;
			unsigned long : 8;
#endif
	} BIT;
	} SCKCR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short IEBCK : 4;
			unsigned short UCK : 4;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short UCK : 4;
			unsigned short IEBCK : 4;
#endif
	} BIT;
	} SCKCR2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short CKSEL : 3;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short CKSEL : 3;
			unsigned short : 8;
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
	char           wk4[5];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char BCLKDIV : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char BCLKDIV : 1;
#endif
	} BIT;
	} BCKCR;
	char           wk5[1];
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
	char           wk6[9];
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
	char           wk7[94];
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
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SSTS : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char SSTS : 5;
#endif
	} BIT;
	} SOSCWTCR;
	char           wk8[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PSTS : 5;
			unsigned char  : 3;
#else
			unsigned char  : 3;
			unsigned char PSTS : 5;
#endif
	} BIT;
	} PLLWTCR;
	char           wk9[25];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char IWDTRF : 1;
			unsigned char WDTRF : 1;
			unsigned char SWRF : 1;
			unsigned char  : 5;
#else
			unsigned char  : 5;
			unsigned char SWRF : 1;
			unsigned char WDTRF : 1;
			unsigned char IWDTRF : 1;
#endif
	} BIT;
	} RSTSR2;
	char           wk10[1];
	unsigned short SWRR;
	char           wk11[28];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD1IDTSEL : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
			unsigned char  : 6;
#else
			unsigned char  : 6;
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
	char           wk12[794];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PRC0 : 1;
			unsigned short PRC1 : 1;
			unsigned short  : 1;
			unsigned short PRC3 : 1;
			unsigned short  : 4;
			unsigned short PRKEY : 8;
#else
			unsigned short PRKEY : 8;
			unsigned short  : 4;
			unsigned short PRC3 : 1;
			unsigned short  : 1;
			unsigned short PRC1 : 1;
			unsigned short PRC0 : 1;
#endif
	} BIT;
	} PRCR;
	char           wk13[48768];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DEEPCUT : 2;
			unsigned char  : 4;
			unsigned char IOKEEP : 1;
			unsigned char DPSBY : 1;
#else
			unsigned char DPSBY : 1;
			unsigned char IOKEEP : 1;
			unsigned char  : 4;
			unsigned char DEEPCUT : 2;
#endif
	} BIT;
	} DPSBYCR;
	char           wk14[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DIRQ0E : 1;
			unsigned char DIRQ1E : 1;
			unsigned char DIRQ2E : 1;
			unsigned char DIRQ3E : 1;
			unsigned char DIRQ4E : 1;
			unsigned char DIRQ5E : 1;
			unsigned char DIRQ6E : 1;
			unsigned char DIRQ7E : 1;
#else
			unsigned char DIRQ7E : 1;
			unsigned char DIRQ6E : 1;
			unsigned char DIRQ5E : 1;
			unsigned char DIRQ4E : 1;
			unsigned char DIRQ3E : 1;
			unsigned char DIRQ2E : 1;
			unsigned char DIRQ1E : 1;
			unsigned char DIRQ0E : 1;
#endif
	} BIT;
	} DPSIER0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DIRQ8E : 1;
			unsigned char DIRQ9E : 1;
			unsigned char DIRQ10E : 1;
			unsigned char DIRQ11E : 1;
			unsigned char DIRQ12E : 1;
			unsigned char DIRQ13E : 1;
			unsigned char DIRQ14E : 1;
			unsigned char DIRQ15E : 1;
#else
			unsigned char DIRQ15E : 1;
			unsigned char DIRQ14E : 1;
			unsigned char DIRQ13E : 1;
			unsigned char DIRQ12E : 1;
			unsigned char DIRQ11E : 1;
			unsigned char DIRQ10E : 1;
			unsigned char DIRQ9E : 1;
			unsigned char DIRQ8E : 1;
#endif
	} BIT;
	} DPSIER1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DLVD1IE : 1;
			unsigned char DLVD2IE : 1;
			unsigned char DRTCIIE : 1;
			unsigned char DRTCAIE : 1;
			unsigned char DNMIE : 1;
			unsigned char DRIICDIE : 1;
			unsigned char DRIICCIE : 1;
			unsigned char DUSBIE : 1;
#else
			unsigned char DUSBIE : 1;
			unsigned char DRIICCIE : 1;
			unsigned char DRIICDIE : 1;
			unsigned char DNMIE : 1;
			unsigned char DRTCAIE : 1;
			unsigned char DRTCIIE : 1;
			unsigned char DLVD2IE : 1;
			unsigned char DLVD1IE : 1;
#endif
	} BIT;
	} DPSIER2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DCANIE : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DCANIE : 1;
#endif
	} BIT;
	} DPSIER3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DIRQ0F : 1;
			unsigned char DIRQ1F : 1;
			unsigned char DIRQ2F : 1;
			unsigned char DIRQ3F : 1;
			unsigned char DIRQ4F : 1;
			unsigned char DIRQ5F : 1;
			unsigned char DIRQ6F : 1;
			unsigned char DIRQ7F : 1;
#else
			unsigned char DIRQ7F : 1;
			unsigned char DIRQ6F : 1;
			unsigned char DIRQ5F : 1;
			unsigned char DIRQ4F : 1;
			unsigned char DIRQ3F : 1;
			unsigned char DIRQ2F : 1;
			unsigned char DIRQ1F : 1;
			unsigned char DIRQ0F : 1;
#endif
	} BIT;
	} DPSIFR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DIRQ8F : 1;
			unsigned char DIRQ9F : 1;
			unsigned char DIRQ10F : 1;
			unsigned char DIRQ11F : 1;
			unsigned char DIRQ12F : 1;
			unsigned char DIRQ13F : 1;
			unsigned char DIRQ14F : 1;
			unsigned char DIRQ15F : 1;
#else
			unsigned char DIRQ15F : 1;
			unsigned char DIRQ14F : 1;
			unsigned char DIRQ13F : 1;
			unsigned char DIRQ12F : 1;
			unsigned char DIRQ11F : 1;
			unsigned char DIRQ10F : 1;
			unsigned char DIRQ9F : 1;
			unsigned char DIRQ8F : 1;
#endif
	} BIT;
	} DPSIFR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DLVD1IF : 1;
			unsigned char DLVD2IF : 1;
			unsigned char DRTCIIF : 1;
			unsigned char DRTCAIF : 1;
			unsigned char DNMIF : 1;
			unsigned char DRIICDIF : 1;
			unsigned char DRIICCIF : 1;
			unsigned char DUSBIF : 1;
#else
			unsigned char DUSBIF : 1;
			unsigned char DRIICCIF : 1;
			unsigned char DRIICDIF : 1;
			unsigned char DNMIF : 1;
			unsigned char DRTCAIF : 1;
			unsigned char DRTCIIF : 1;
			unsigned char DLVD2IF : 1;
			unsigned char DLVD1IF : 1;
#endif
	} BIT;
	} DPSIFR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DCANIF : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DCANIF : 1;
#endif
	} BIT;
	} DPSIFR3;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DIRQ0EG : 1;
			unsigned char DIRQ1EG : 1;
			unsigned char DIRQ2EG : 1;
			unsigned char DIRQ3EG : 1;
			unsigned char DIRQ4EG : 1;
			unsigned char DIRQ5EG : 1;
			unsigned char DIRQ6EG : 1;
			unsigned char DIRQ7EG : 1;
#else
			unsigned char DIRQ7EG : 1;
			unsigned char DIRQ6EG : 1;
			unsigned char DIRQ5EG : 1;
			unsigned char DIRQ4EG : 1;
			unsigned char DIRQ3EG : 1;
			unsigned char DIRQ2EG : 1;
			unsigned char DIRQ1EG : 1;
			unsigned char DIRQ0EG : 1;
#endif
	} BIT;
	} DPSIEGR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DIRQ8EG : 1;
			unsigned char DIRQ9EG : 1;
			unsigned char DIRQ10EG : 1;
			unsigned char DIRQ11EG : 1;
			unsigned char DIRQ12EG : 1;
			unsigned char DIRQ13EG : 1;
			unsigned char DIRQ14EG : 1;
			unsigned char DIRQ15EG : 1;
#else
			unsigned char DIRQ15EG : 1;
			unsigned char DIRQ14EG : 1;
			unsigned char DIRQ13EG : 1;
			unsigned char DIRQ12EG : 1;
			unsigned char DIRQ11EG : 1;
			unsigned char DIRQ10EG : 1;
			unsigned char DIRQ9EG : 1;
			unsigned char DIRQ8EG : 1;
#endif
	} BIT;
	} DPSIEGR1;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DLVD1EG : 1;
			unsigned char DLVD2EG : 1;
			unsigned char  : 2;
			unsigned char DNMIEG : 1;
			unsigned char DRIICDEG : 1;
			unsigned char DRIICCEG : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char DRIICCEG : 1;
			unsigned char DRIICDEG : 1;
			unsigned char DNMIEG : 1;
			unsigned char  : 2;
			unsigned char DLVD2EG : 1;
			unsigned char DLVD1EG : 1;
#endif
	} BIT;
	} DPSIEGR2;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char DCANIEG : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char DCANIEG : 1;
#endif
	} BIT;
	} DPSIEGR3;
	char           wk15[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char PORF : 1;
			unsigned char LVD0RF : 1;
			unsigned char LVD1RF : 1;
			unsigned char LVD2RF : 1;
			unsigned char  : 3;
			unsigned char DPSRSTF : 1;
#else
			unsigned char DPSRSTF : 1;
			unsigned char  : 3;
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
			unsigned char MOFXIN : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char MOFXIN : 1;
#endif
	} BIT;
	} MOFCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char HOCOPCNT : 1;
			unsigned char  : 7;
#else
			unsigned char  : 7;
			unsigned char HOCOPCNT : 1;
#endif
	} BIT;
	} HOCOPCR;
	char           wk17[2];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 5;
			unsigned char LVD1E : 1;
			unsigned char LVD2E : 1;
			unsigned char  : 1;
#else
			unsigned char  : 1;
			unsigned char LVD2E : 1;
			unsigned char LVD1E : 1;
			unsigned char : 5;
#endif
	} BIT;
	} LVCMPCR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD1LVL : 4;
			unsigned char LVD2LVL : 4;
#else
			unsigned char LVD2LVL : 4;
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
			unsigned char LVD1DFDIS : 1;
			unsigned char LVD1CMPE : 1;
			unsigned char  : 1;
			unsigned char LVD1FSAMP : 2;
			unsigned char LVD1RI : 1;
			unsigned char LVD1RN : 1;
#else
			unsigned char LVD1RN : 1;
			unsigned char LVD1RI : 1;
			unsigned char LVD1FSAMP : 2;
			unsigned char  : 1;
			unsigned char LVD1CMPE : 1;
			unsigned char LVD1DFDIS : 1;
			unsigned char LVD1RIE : 1;
#endif
	} BIT;
	} LVD1CR0;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char LVD2RIE : 1;
			unsigned char LVD2DFDIS : 1;
			unsigned char LVD2CMPE : 1;
			unsigned char  : 1;
			unsigned char LVD2FSAMP : 2;
			unsigned char LVD2RI : 1;
			unsigned char LVD2RN : 1;
#else
			unsigned char LVD2RN : 1;
			unsigned char LVD2RI : 1;
			unsigned char LVD2FSAMP : 2;
			unsigned char  : 1;
			unsigned char LVD2CMPE : 1;
			unsigned char LVD2DFDIS : 1;
			unsigned char LVD2RIE : 1;
#endif
	} BIT;
	} LVD2CR0;
	char           wk19[4];
	unsigned char  DPSBKR[32];
	char           wk20[1472];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SCK : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char SCK : 2;
#endif
	} BIT;
	} SCK1;
	char           wk21[15];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SCK : 2;
			unsigned char  : 6;
#else
			unsigned char  : 6;
			unsigned char SCK : 2;
#endif
	} BIT;
	} SCK2;
};

struct st_temps {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 4;
			unsigned char TSOE : 1;
			unsigned char  : 2;
			unsigned char TSEN : 1;
#else
			unsigned char TSEN : 1;
			unsigned char  : 2;
			unsigned char TSOE : 1;
			unsigned char : 4;
#endif
	} BIT;
	} TSCR;
};

struct st_tempsconst {
	unsigned char  TSCDRL;
	unsigned char  TSCDRH;
};

struct st_tmr0 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 3;
			unsigned char CCLR : 2;
			unsigned char OVIE : 1;
			unsigned char CMIEA : 1;
			unsigned char CMIEB : 1;
#else
			unsigned char CMIEB : 1;
			unsigned char CMIEA : 1;
			unsigned char OVIE : 1;
			unsigned char CCLR : 2;
			unsigned char : 3;
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
};

struct st_tmr1 {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 3;
			unsigned char CCLR : 2;
			unsigned char OVIE : 1;
			unsigned char CMIEA : 1;
			unsigned char CMIEB : 1;
#else
			unsigned char CMIEB : 1;
			unsigned char CMIEA : 1;
			unsigned char OVIE : 1;
			unsigned char CCLR : 2;
			unsigned char : 3;
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

struct st_tpu0 {
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
	char           wk0[7];
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
			unsigned char ICSELB : 1;
			unsigned char ICSELD : 1;
#else
			unsigned char ICSELD : 1;
			unsigned char ICSELB : 1;
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
			unsigned char TCIEU : 1;
			unsigned char  : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 1;
			unsigned char TCIEU : 1;
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
			unsigned char TGFA : 1;
			unsigned char TGFB : 1;
			unsigned char TGFC : 1;
			unsigned char TGFD : 1;
			unsigned char TCFV : 1;
			unsigned char TCFU : 1;
			unsigned char  : 1;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 1;
			unsigned char TCFU : 1;
			unsigned char TCFV : 1;
			unsigned char TGFD : 1;
			unsigned char TGFC : 1;
			unsigned char TGFB : 1;
			unsigned char TGFA : 1;
#endif
	} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
	unsigned short TGRC;
	unsigned short TGRD;
};

struct st_tpu1 {
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
	char           wk1[22];
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
			unsigned char ICSELB : 1;
			unsigned char ICSELD : 1;
#else
			unsigned char ICSELD : 1;
			unsigned char ICSELB : 1;
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
	} TIOR;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char TCIEU : 1;
			unsigned char  : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 1;
			unsigned char TCIEU : 1;
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
			unsigned char TGFA : 1;
			unsigned char TGFB : 1;
			unsigned char TGFC : 1;
			unsigned char TGFD : 1;
			unsigned char TCFV : 1;
			unsigned char TCFU : 1;
			unsigned char  : 1;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 1;
			unsigned char TCFU : 1;
			unsigned char TCFV : 1;
			unsigned char TGFD : 1;
			unsigned char TGFC : 1;
			unsigned char TGFB : 1;
			unsigned char TGFA : 1;
#endif
	} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
};

struct st_tpu2 {
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
	char           wk0[37];
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
			unsigned char ICSELB : 1;
			unsigned char ICSELD : 1;
#else
			unsigned char ICSELD : 1;
			unsigned char ICSELB : 1;
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
	} TIOR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char TCIEU : 1;
			unsigned char  : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 1;
			unsigned char TCIEU : 1;
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
			unsigned char TGFA : 1;
			unsigned char TGFB : 1;
			unsigned char TGFC : 1;
			unsigned char TGFD : 1;
			unsigned char TCFV : 1;
			unsigned char TCFU : 1;
			unsigned char  : 1;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 1;
			unsigned char TCFU : 1;
			unsigned char TCFV : 1;
			unsigned char TGFD : 1;
			unsigned char TGFC : 1;
			unsigned char TGFB : 1;
			unsigned char TGFA : 1;
#endif
	} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
};

struct st_tpu3 {
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
	char           wk1[52];
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
			unsigned char ICSELB : 1;
			unsigned char ICSELD : 1;
#else
			unsigned char ICSELD : 1;
			unsigned char ICSELB : 1;
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
			unsigned char TCIEU : 1;
			unsigned char  : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 1;
			unsigned char TCIEU : 1;
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
			unsigned char TGFA : 1;
			unsigned char TGFB : 1;
			unsigned char TGFC : 1;
			unsigned char TGFD : 1;
			unsigned char TCFV : 1;
			unsigned char TCFU : 1;
			unsigned char  : 1;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 1;
			unsigned char TCFU : 1;
			unsigned char TCFV : 1;
			unsigned char TGFD : 1;
			unsigned char TGFC : 1;
			unsigned char TGFB : 1;
			unsigned char TGFA : 1;
#endif
	} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
	unsigned short TGRC;
	unsigned short TGRD;
};

struct st_tpu4 {
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
	char           wk0[67];
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
			unsigned char ICSELB : 1;
			unsigned char ICSELD : 1;
#else
			unsigned char ICSELD : 1;
			unsigned char ICSELB : 1;
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
	} TIOR;
	char           wk1[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char TCIEU : 1;
			unsigned char  : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 1;
			unsigned char TCIEU : 1;
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
			unsigned char TGFA : 1;
			unsigned char TGFB : 1;
			unsigned char TGFC : 1;
			unsigned char TGFD : 1;
			unsigned char TCFV : 1;
			unsigned char TCFU : 1;
			unsigned char  : 1;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 1;
			unsigned char TCFU : 1;
			unsigned char TCFV : 1;
			unsigned char TGFD : 1;
			unsigned char TGFC : 1;
			unsigned char TGFB : 1;
			unsigned char TGFA : 1;
#endif
	} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
};

struct st_tpu5 {
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
	char           wk1[82];
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
			unsigned char ICSELB : 1;
			unsigned char ICSELD : 1;
#else
			unsigned char ICSELD : 1;
			unsigned char ICSELB : 1;
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
	} TIOR;
	char           wk2[1];
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char TGIEA : 1;
			unsigned char TGIEB : 1;
			unsigned char TGIEC : 1;
			unsigned char TGIED : 1;
			unsigned char TCIEV : 1;
			unsigned char TCIEU : 1;
			unsigned char  : 1;
			unsigned char TTGE : 1;
#else
			unsigned char TTGE : 1;
			unsigned char  : 1;
			unsigned char TCIEU : 1;
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
			unsigned char TGFA : 1;
			unsigned char TGFB : 1;
			unsigned char TGFC : 1;
			unsigned char TGFD : 1;
			unsigned char TCFV : 1;
			unsigned char TCFU : 1;
			unsigned char  : 1;
			unsigned char TCFD : 1;
#else
			unsigned char TCFD : 1;
			unsigned char  : 1;
			unsigned char TCFU : 1;
			unsigned char TCFV : 1;
			unsigned char TGFD : 1;
			unsigned char TGFC : 1;
			unsigned char TGFB : 1;
			unsigned char TGFA : 1;
#endif
	} BIT;
	} TSR;
	unsigned short TCNT;
	unsigned short TGRA;
	unsigned short TGRB;
};

struct st_tpua {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CST0 : 1;
			unsigned char CST1 : 1;
			unsigned char CST2 : 1;
			unsigned char CST3 : 1;
			unsigned char CST4 : 1;
			unsigned char CST5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char CST5 : 1;
			unsigned char CST4 : 1;
			unsigned char CST3 : 1;
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
			unsigned char SYNC3 : 1;
			unsigned char SYNC4 : 1;
			unsigned char SYNC5 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char SYNC5 : 1;
			unsigned char SYNC4 : 1;
			unsigned char SYNC3 : 1;
			unsigned char SYNC2 : 1;
			unsigned char SYNC1 : 1;
			unsigned char SYNC0 : 1;
#endif
	} BIT;
	} TSYR;
};

struct st_tpub {
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char CST6 : 1;
			unsigned char CST7 : 1;
			unsigned char CST8 : 1;
			unsigned char CST9 : 1;
			unsigned char CST10 : 1;
			unsigned char CST11 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char CST11 : 1;
			unsigned char CST10 : 1;
			unsigned char CST9 : 1;
			unsigned char CST8 : 1;
			unsigned char CST7 : 1;
			unsigned char CST6 : 1;
#endif
	} BIT;
	} TSTR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char SYNC6 : 1;
			unsigned char SYNC7 : 1;
			unsigned char SYNC8 : 1;
			unsigned char SYNC9 : 1;
			unsigned char SYNC10 : 1;
			unsigned char SYNC11 : 1;
			unsigned char  : 2;
#else
			unsigned char  : 2;
			unsigned char SYNC11 : 1;
			unsigned char SYNC10 : 1;
			unsigned char SYNC9 : 1;
			unsigned char SYNC8 : 1;
			unsigned char SYNC7 : 1;
			unsigned char SYNC6 : 1;
#endif
	} BIT;
	} TSYR;
};

struct st_usb {
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long SRPC0 : 1;
			unsigned long  : 3;
			unsigned long FIXPHY0 : 1;
			unsigned long  : 3;
			unsigned long SRPC1 : 1;
			unsigned long  : 3;
			unsigned long FIXPHY1 : 1;
			unsigned long  : 3;
			unsigned long DP0 : 1;
			unsigned long DM0 : 1;
			unsigned long  : 2;
			unsigned long DOVCA0 : 1;
			unsigned long DOVCB0 : 1;
			unsigned long  : 1;
			unsigned long DVBSTS0 : 1;
			unsigned long DP1 : 1;
			unsigned long DM1 : 1;
			unsigned long  : 5;
			unsigned long DVBSTS1 : 1;
#else
			unsigned long DVBSTS1 : 1;
			unsigned long  : 5;
			unsigned long DM1 : 1;
			unsigned long DP1 : 1;
			unsigned long DVBSTS0 : 1;
			unsigned long  : 1;
			unsigned long DOVCB0 : 1;
			unsigned long DOVCA0 : 1;
			unsigned long  : 2;
			unsigned long DM0 : 1;
			unsigned long DP0 : 1;
			unsigned long  : 3;
			unsigned long FIXPHY1 : 1;
			unsigned long  : 3;
			unsigned long SRPC1 : 1;
			unsigned long  : 3;
			unsigned long FIXPHY0 : 1;
			unsigned long  : 3;
			unsigned long SRPC0 : 1;
#endif
	} BIT;
	} DPUSR0R;
	union {
		unsigned long LONG;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned long DPINTE0 : 1;
			unsigned long DMINTE0 : 1;
			unsigned long  : 2;
			unsigned long DOVRCRAE0 : 1;
			unsigned long DOVRCRBE0 : 1;
			unsigned long  : 1;
			unsigned long DVBSE0 : 1;
			unsigned long DPINTE1 : 1;
			unsigned long DMINTE1 : 1;
			unsigned long  : 5;
			unsigned long DVBSE1 : 1;
			unsigned long DPINT0 : 1;
			unsigned long DMINT0 : 1;
			unsigned long  : 2;
			unsigned long DOVRCRA0 : 1;
			unsigned long DOVRCRB0 : 1;
			unsigned long  : 1;
			unsigned long DVBINT0 : 1;
			unsigned long DPINT1 : 1;
			unsigned long DMINT1 : 1;
			unsigned long  : 5;
			unsigned long DVBINT1 : 1;
#else
			unsigned long DVBINT1 : 1;
			unsigned long  : 5;
			unsigned long DMINT1 : 1;
			unsigned long DPINT1 : 1;
			unsigned long DVBINT0 : 1;
			unsigned long  : 1;
			unsigned long DOVRCRB0 : 1;
			unsigned long DOVRCRA0 : 1;
			unsigned long  : 2;
			unsigned long DMINT0 : 1;
			unsigned long DPINT0 : 1;
			unsigned long DVBSE1 : 1;
			unsigned long  : 5;
			unsigned long DMINTE1 : 1;
			unsigned long DPINTE1 : 1;
			unsigned long DVBSE0 : 1;
			unsigned long  : 1;
			unsigned long DOVRCRBE0 : 1;
			unsigned long DOVRCRAE0 : 1;
			unsigned long  : 2;
			unsigned long DMINTE0 : 1;
			unsigned long DPINTE0 : 1;
#endif
	} BIT;
	} DPUSR1R;
};

struct st_usb0 {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short USBE : 1;
			unsigned short  : 3;
			unsigned short DPRPU : 1;
			unsigned short DRPD : 1;
			unsigned short DCFM : 1;
			unsigned short  : 3;
			unsigned short SCKE : 1;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short SCKE : 1;
			unsigned short  : 3;
			unsigned short DCFM : 1;
			unsigned short DRPD : 1;
			unsigned short DPRPU : 1;
			unsigned short  : 3;
			unsigned short USBE : 1;
#endif
	} BIT;
	} SYSCFG;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short LNST : 2;
			unsigned short IDMON : 1;
			unsigned short  : 3;
			unsigned short HTACT : 1;
			unsigned short  : 7;
			unsigned short OVCMON : 2;
#else
			unsigned short OVCMON : 2;
			unsigned short  : 7;
			unsigned short HTACT : 1;
			unsigned short  : 3;
			unsigned short IDMON : 1;
			unsigned short LNST : 2;
#endif
	} BIT;
	} SYSSTS0;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RHST : 3;
			unsigned short  : 1;
			unsigned short UACT : 1;
			unsigned short RESUME : 1;
			unsigned short USBRST : 1;
			unsigned short RWUPE : 1;
			unsigned short WKUP : 1;
			unsigned short VBUSEN : 1;
			unsigned short EXICEN : 1;
			unsigned short HNPBTOA : 1;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short HNPBTOA : 1;
			unsigned short EXICEN : 1;
			unsigned short VBUSEN : 1;
			unsigned short WKUP : 1;
			unsigned short RWUPE : 1;
			unsigned short USBRST : 1;
			unsigned short RESUME : 1;
			unsigned short UACT : 1;
			unsigned short  : 1;
			unsigned short RHST : 3;
#endif
	} BIT;
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
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CURPIPE : 4;
			unsigned short  : 1;
			unsigned short ISEL : 1;
			unsigned short  : 2;
			unsigned short BIGEND : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 3;
			unsigned short REW : 1;
			unsigned short RCNT : 1;
#else
			unsigned short RCNT : 1;
			unsigned short REW : 1;
			unsigned short  : 3;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short BIGEND : 1;
			unsigned short  : 2;
			unsigned short ISEL : 1;
			unsigned short  : 1;
			unsigned short CURPIPE : 4;
#endif
	} BIT;
	} CFIFOSEL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DTLN : 9;
			unsigned short  : 4;
			unsigned short FRDY : 1;
			unsigned short BCLR : 1;
			unsigned short BVAL : 1;
#else
			unsigned short BVAL : 1;
			unsigned short BCLR : 1;
			unsigned short FRDY : 1;
			unsigned short  : 4;
			unsigned short DTLN : 9;
#endif
	} BIT;
	} CFIFOCTR;
	char           wk6[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CURPIPE : 4;
			unsigned short  : 4;
			unsigned short BIGEND : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short DREQE : 1;
			unsigned short DCLRM : 1;
			unsigned short REW : 1;
			unsigned short RCNT : 1;
#else
			unsigned short RCNT : 1;
			unsigned short REW : 1;
			unsigned short DCLRM : 1;
			unsigned short DREQE : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short BIGEND : 1;
			unsigned short  : 4;
			unsigned short CURPIPE : 4;
#endif
	} BIT;
	} D0FIFOSEL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DTLN : 9;
			unsigned short  : 4;
			unsigned short FRDY : 1;
			unsigned short BCLR : 1;
			unsigned short BVAL : 1;
#else
			unsigned short BVAL : 1;
			unsigned short BCLR : 1;
			unsigned short FRDY : 1;
			unsigned short  : 4;
			unsigned short DTLN : 9;
#endif
	} BIT;
	} D0FIFOCTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CURPIPE : 4;
			unsigned short  : 4;
			unsigned short BIGEND : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short DREQE : 1;
			unsigned short DCLRM : 1;
			unsigned short REW : 1;
			unsigned short RCNT : 1;
#else
			unsigned short RCNT : 1;
			unsigned short REW : 1;
			unsigned short DCLRM : 1;
			unsigned short DREQE : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short BIGEND : 1;
			unsigned short  : 4;
			unsigned short CURPIPE : 4;
#endif
	} BIT;
	} D1FIFOSEL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DTLN : 9;
			unsigned short  : 4;
			unsigned short FRDY : 1;
			unsigned short BCLR : 1;
			unsigned short BVAL : 1;
#else
			unsigned short BVAL : 1;
			unsigned short BCLR : 1;
			unsigned short FRDY : 1;
			unsigned short  : 4;
			unsigned short DTLN : 9;
#endif
	} BIT;
	} D1FIFOCTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short BRDYE : 1;
			unsigned short NRDYE : 1;
			unsigned short BEMPE : 1;
			unsigned short CTRE : 1;
			unsigned short DVSE : 1;
			unsigned short SOFE : 1;
			unsigned short RSME : 1;
			unsigned short VBSE : 1;
#else
			unsigned short VBSE : 1;
			unsigned short RSME : 1;
			unsigned short SOFE : 1;
			unsigned short DVSE : 1;
			unsigned short CTRE : 1;
			unsigned short BEMPE : 1;
			unsigned short NRDYE : 1;
			unsigned short BRDYE : 1;
			unsigned short : 8;
#endif
	} BIT;
	} INTENB0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 4;
			unsigned short SACKE : 1;
			unsigned short SIGNE : 1;
			unsigned short EOFERRE : 1;
			unsigned short  : 4;
			unsigned short ATTCHE : 1;
			unsigned short DTCHE : 1;
			unsigned short  : 1;
			unsigned short BCHGE : 1;
			unsigned short OVRCRE : 1;
#else
			unsigned short OVRCRE : 1;
			unsigned short BCHGE : 1;
			unsigned short  : 1;
			unsigned short DTCHE : 1;
			unsigned short ATTCHE : 1;
			unsigned short  : 4;
			unsigned short EOFERRE : 1;
			unsigned short SIGNE : 1;
			unsigned short SACKE : 1;
			unsigned short : 4;
#endif
	} BIT;
	} INTENB1;
	char           wk7[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0BRDYE : 1;
			unsigned short PIPE1BRDYE : 1;
			unsigned short PIPE2BRDYE : 1;
			unsigned short PIPE3BRDYE : 1;
			unsigned short PIPE4BRDYE : 1;
			unsigned short PIPE5BRDYE : 1;
			unsigned short PIPE6BRDYE : 1;
			unsigned short PIPE7BRDYE : 1;
			unsigned short PIPE8BRDYE : 1;
			unsigned short PIPE9BRDYE : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9BRDYE : 1;
			unsigned short PIPE8BRDYE : 1;
			unsigned short PIPE7BRDYE : 1;
			unsigned short PIPE6BRDYE : 1;
			unsigned short PIPE5BRDYE : 1;
			unsigned short PIPE4BRDYE : 1;
			unsigned short PIPE3BRDYE : 1;
			unsigned short PIPE2BRDYE : 1;
			unsigned short PIPE1BRDYE : 1;
			unsigned short PIPE0BRDYE : 1;
#endif
	} BIT;
	} BRDYENB;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0NRDYE : 1;
			unsigned short PIPE1NRDYE : 1;
			unsigned short PIPE2NRDYE : 1;
			unsigned short PIPE3NRDYE : 1;
			unsigned short PIPE4NRDYE : 1;
			unsigned short PIPE5NRDYE : 1;
			unsigned short PIPE6NRDYE : 1;
			unsigned short PIPE7NRDYE : 1;
			unsigned short PIPE8NRDYE : 1;
			unsigned short PIPE9NRDYE : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9NRDYE : 1;
			unsigned short PIPE8NRDYE : 1;
			unsigned short PIPE7NRDYE : 1;
			unsigned short PIPE6NRDYE : 1;
			unsigned short PIPE5NRDYE : 1;
			unsigned short PIPE4NRDYE : 1;
			unsigned short PIPE3NRDYE : 1;
			unsigned short PIPE2NRDYE : 1;
			unsigned short PIPE1NRDYE : 1;
			unsigned short PIPE0NRDYE : 1;
#endif
	} BIT;
	} NRDYENB;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0BEMPE : 1;
			unsigned short PIPE1BEMPE : 1;
			unsigned short PIPE2BEMPE : 1;
			unsigned short PIPE3BEMPE : 1;
			unsigned short PIPE4BEMPE : 1;
			unsigned short PIPE5BEMPE : 1;
			unsigned short PIPE6BEMPE : 1;
			unsigned short PIPE7BEMPE : 1;
			unsigned short PIPE8BEMPE : 1;
			unsigned short PIPE9BEMPE : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9BEMPE : 1;
			unsigned short PIPE8BEMPE : 1;
			unsigned short PIPE7BEMPE : 1;
			unsigned short PIPE6BEMPE : 1;
			unsigned short PIPE5BEMPE : 1;
			unsigned short PIPE4BEMPE : 1;
			unsigned short PIPE3BEMPE : 1;
			unsigned short PIPE2BEMPE : 1;
			unsigned short PIPE1BEMPE : 1;
			unsigned short PIPE0BEMPE : 1;
#endif
	} BIT;
	} BEMPENB;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 4;
			unsigned short EDGESTS : 1;
			unsigned short  : 1;
			unsigned short BRDYM : 1;
			unsigned short  : 1;
			unsigned short TRNENSEL : 1;
			unsigned short  : 7;
#else
			unsigned short  : 7;
			unsigned short TRNENSEL : 1;
			unsigned short  : 1;
			unsigned short BRDYM : 1;
			unsigned short  : 1;
			unsigned short EDGESTS : 1;
			unsigned short : 4;
#endif
	} BIT;
	} SOFCFG;
	char           wk8[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CTSQ : 3;
			unsigned short VALID : 1;
			unsigned short DVSQ : 3;
			unsigned short VBSTS : 1;
			unsigned short BRDY : 1;
			unsigned short NRDY : 1;
			unsigned short BEMP : 1;
			unsigned short CTRT : 1;
			unsigned short DVST : 1;
			unsigned short SOFR : 1;
			unsigned short RESM : 1;
			unsigned short VBINT : 1;
#else
			unsigned short VBINT : 1;
			unsigned short RESM : 1;
			unsigned short SOFR : 1;
			unsigned short DVST : 1;
			unsigned short CTRT : 1;
			unsigned short BEMP : 1;
			unsigned short NRDY : 1;
			unsigned short BRDY : 1;
			unsigned short VBSTS : 1;
			unsigned short DVSQ : 3;
			unsigned short VALID : 1;
			unsigned short CTSQ : 3;
#endif
	} BIT;
	} INTSTS0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 4;
			unsigned short SACK : 1;
			unsigned short SIGN : 1;
			unsigned short EOFERR : 1;
			unsigned short  : 4;
			unsigned short ATTCH : 1;
			unsigned short DTCH : 1;
			unsigned short  : 1;
			unsigned short BCHG : 1;
			unsigned short OVRCR : 1;
#else
			unsigned short OVRCR : 1;
			unsigned short BCHG : 1;
			unsigned short  : 1;
			unsigned short DTCH : 1;
			unsigned short ATTCH : 1;
			unsigned short  : 4;
			unsigned short EOFERR : 1;
			unsigned short SIGN : 1;
			unsigned short SACK : 1;
			unsigned short : 4;
#endif
	} BIT;
	} INTSTS1;
	char           wk9[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0BRDY : 1;
			unsigned short PIPE1BRDY : 1;
			unsigned short PIPE2BRDY : 1;
			unsigned short PIPE3BRDY : 1;
			unsigned short PIPE4BRDY : 1;
			unsigned short PIPE5BRDY : 1;
			unsigned short PIPE6BRDY : 1;
			unsigned short PIPE7BRDY : 1;
			unsigned short PIPE8BRDY : 1;
			unsigned short PIPE9BRDY : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9BRDY : 1;
			unsigned short PIPE8BRDY : 1;
			unsigned short PIPE7BRDY : 1;
			unsigned short PIPE6BRDY : 1;
			unsigned short PIPE5BRDY : 1;
			unsigned short PIPE4BRDY : 1;
			unsigned short PIPE3BRDY : 1;
			unsigned short PIPE2BRDY : 1;
			unsigned short PIPE1BRDY : 1;
			unsigned short PIPE0BRDY : 1;
#endif
	} BIT;
	} BRDYSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0NRDY : 1;
			unsigned short PIPE1NRDY : 1;
			unsigned short PIPE2NRDY : 1;
			unsigned short PIPE3NRDY : 1;
			unsigned short PIPE4NRDY : 1;
			unsigned short PIPE5NRDY : 1;
			unsigned short PIPE6NRDY : 1;
			unsigned short PIPE7NRDY : 1;
			unsigned short PIPE8NRDY : 1;
			unsigned short PIPE9NRDY : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9NRDY : 1;
			unsigned short PIPE8NRDY : 1;
			unsigned short PIPE7NRDY : 1;
			unsigned short PIPE6NRDY : 1;
			unsigned short PIPE5NRDY : 1;
			unsigned short PIPE4NRDY : 1;
			unsigned short PIPE3NRDY : 1;
			unsigned short PIPE2NRDY : 1;
			unsigned short PIPE1NRDY : 1;
			unsigned short PIPE0NRDY : 1;
#endif
	} BIT;
	} NRDYSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0BEMP : 1;
			unsigned short PIPE1BEMP : 1;
			unsigned short PIPE2BEMP : 1;
			unsigned short PIPE3BEMP : 1;
			unsigned short PIPE4BEMP : 1;
			unsigned short PIPE5BEMP : 1;
			unsigned short PIPE6BEMP : 1;
			unsigned short PIPE7BEMP : 1;
			unsigned short PIPE8BEMP : 1;
			unsigned short PIPE9BEMP : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9BEMP : 1;
			unsigned short PIPE8BEMP : 1;
			unsigned short PIPE7BEMP : 1;
			unsigned short PIPE6BEMP : 1;
			unsigned short PIPE5BEMP : 1;
			unsigned short PIPE4BEMP : 1;
			unsigned short PIPE3BEMP : 1;
			unsigned short PIPE2BEMP : 1;
			unsigned short PIPE1BEMP : 1;
			unsigned short PIPE0BEMP : 1;
#endif
	} BIT;
	} BEMPSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FRNM : 11;
			unsigned short  : 3;
			unsigned short CRCE : 1;
			unsigned short OVRN : 1;
#else
			unsigned short OVRN : 1;
			unsigned short CRCE : 1;
			unsigned short  : 3;
			unsigned short FRNM : 11;
#endif
	} BIT;
	} FRMNUM;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 15;
			unsigned short DVCHG : 1;
#else
			unsigned short DVCHG : 1;
			unsigned short : 15;
#endif
	} BIT;
	} DVCHGR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short USBADDR : 7;
			unsigned short  : 1;
			unsigned short STSRECOV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short STSRECOV : 4;
			unsigned short  : 1;
			unsigned short USBADDR : 7;
#endif
	} BIT;
	} USBADDR;
	char           wk10[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BMREQUESTTYPE : 8;
			unsigned short BREQUEST : 8;
#else
			unsigned short BREQUEST : 8;
			unsigned short BMREQUESTTYPE : 8;
#endif
	} BIT;
	} USBREQ;
	unsigned short USBVAL;
	unsigned short USBINDX;
	unsigned short USBLENG;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 4;
			unsigned short DIR : 1;
			unsigned short  : 2;
			unsigned short SHTNAK : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short SHTNAK : 1;
			unsigned short  : 2;
			unsigned short DIR : 1;
			unsigned short : 4;
#endif
	} BIT;
	} DCPCFG;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MXPS : 7;
			unsigned short  : 5;
			unsigned short DEVSEL : 4;
#else
			unsigned short DEVSEL : 4;
			unsigned short  : 5;
			unsigned short MXPS : 7;
#endif
	} BIT;
	} DCPMAXP;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short CCPL : 1;
			unsigned short  : 2;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short  : 2;
			unsigned short SUREQCLR : 1;
			unsigned short  : 2;
			unsigned short SUREQ : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short SUREQ : 1;
			unsigned short  : 2;
			unsigned short SUREQCLR : 1;
			unsigned short  : 2;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 2;
			unsigned short CCPL : 1;
			unsigned short PID : 2;
#endif
	} BIT;
	} DCPCTR;
	char           wk11[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPESEL : 4;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short PIPESEL : 4;
#endif
	} BIT;
	} PIPESEL;
	char           wk12[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EPNUM : 4;
			unsigned short DIR : 1;
			unsigned short  : 2;
			unsigned short SHTNAK : 1;
			unsigned short  : 1;
			unsigned short DBLB : 1;
			unsigned short BFRE : 1;
			unsigned short  : 3;
			unsigned short TYPE : 2;
#else
			unsigned short TYPE : 2;
			unsigned short  : 3;
			unsigned short BFRE : 1;
			unsigned short DBLB : 1;
			unsigned short  : 1;
			unsigned short SHTNAK : 1;
			unsigned short  : 2;
			unsigned short DIR : 1;
			unsigned short EPNUM : 4;
#endif
	} BIT;
	} PIPECFG;
	char           wk13[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MXPS : 9;
			unsigned short  : 3;
			unsigned short DEVSEL : 4;
#else
			unsigned short DEVSEL : 4;
			unsigned short  : 3;
			unsigned short MXPS : 9;
#endif
	} BIT;
	} PIPEMAXP;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short IITV : 3;
			unsigned short  : 9;
			unsigned short IFIS : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short IFIS : 1;
			unsigned short  : 9;
			unsigned short IITV : 3;
#endif
	} BIT;
	} PIPEPERI;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE1CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE2CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE3CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE4CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE5CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short  : 5;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short  : 5;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE6CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short  : 5;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short  : 5;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE7CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short  : 5;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short  : 5;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE8CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short  : 5;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short  : 5;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE9CTR;
	char           wk14[14];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE1TRE;
	unsigned short PIPE1TRN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE2TRE;
	unsigned short PIPE2TRN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE3TRE;
	unsigned short PIPE3TRN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE4TRE;
	unsigned short PIPE4TRN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE5TRE;
	unsigned short PIPE5TRN;
	char           wk15[44];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD3;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD4;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD5;
};

struct st_usb1 {
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short USBE : 1;
			unsigned short  : 3;
			unsigned short DPRPU : 1;
			unsigned short DRPD : 1;
			unsigned short DCFM : 1;
			unsigned short  : 3;
			unsigned short SCKE : 1;
			unsigned short  : 5;
#else
			unsigned short  : 5;
			unsigned short SCKE : 1;
			unsigned short  : 3;
			unsigned short DCFM : 1;
			unsigned short DRPD : 1;
			unsigned short DPRPU : 1;
			unsigned short  : 3;
			unsigned short USBE : 1;
#endif
	} BIT;
	} SYSCFG;
	char           wk0[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short LNST : 2;
			unsigned short IDMON : 1;
			unsigned short  : 3;
			unsigned short HTACT : 1;
			unsigned short  : 7;
			unsigned short OVCMON : 2;
#else
			unsigned short OVCMON : 2;
			unsigned short  : 7;
			unsigned short HTACT : 1;
			unsigned short  : 3;
			unsigned short IDMON : 1;
			unsigned short LNST : 2;
#endif
	} BIT;
	} SYSSTS0;
	char           wk1[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short RHST : 3;
			unsigned short  : 1;
			unsigned short UACT : 1;
			unsigned short RESUME : 1;
			unsigned short USBRST : 1;
			unsigned short RWUPE : 1;
			unsigned short WKUP : 1;
			unsigned short VBUSEN : 1;
			unsigned short EXICEN : 1;
			unsigned short HNPBTOA : 1;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short HNPBTOA : 1;
			unsigned short EXICEN : 1;
			unsigned short VBUSEN : 1;
			unsigned short WKUP : 1;
			unsigned short RWUPE : 1;
			unsigned short USBRST : 1;
			unsigned short RESUME : 1;
			unsigned short UACT : 1;
			unsigned short  : 1;
			unsigned short RHST : 3;
#endif
	} BIT;
	} DVSTCTR0;
	char           wk2[10];
	unsigned short CFIFO;
	char           wk3[2];
	unsigned short D0FIFO;
	char           wk4[2];
	unsigned short D1FIFO;
	char           wk5[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CURPIPE : 4;
			unsigned short  : 1;
			unsigned short ISEL : 1;
			unsigned short  : 2;
			unsigned short BIGEND : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 3;
			unsigned short REW : 1;
			unsigned short RCNT : 1;
#else
			unsigned short RCNT : 1;
			unsigned short REW : 1;
			unsigned short  : 3;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short BIGEND : 1;
			unsigned short  : 2;
			unsigned short ISEL : 1;
			unsigned short  : 1;
			unsigned short CURPIPE : 4;
#endif
	} BIT;
	} CFIFOSEL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DTLN : 9;
			unsigned short  : 4;
			unsigned short FRDY : 1;
			unsigned short BCLR : 1;
			unsigned short BVAL : 1;
#else
			unsigned short BVAL : 1;
			unsigned short BCLR : 1;
			unsigned short FRDY : 1;
			unsigned short  : 4;
			unsigned short DTLN : 9;
#endif
	} BIT;
	} CFIFOCTR;
	char           wk6[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CURPIPE : 4;
			unsigned short  : 4;
			unsigned short BIGEND : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short DREQE : 1;
			unsigned short DCLRM : 1;
			unsigned short REW : 1;
			unsigned short RCNT : 1;
#else
			unsigned short RCNT : 1;
			unsigned short REW : 1;
			unsigned short DCLRM : 1;
			unsigned short DREQE : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short BIGEND : 1;
			unsigned short  : 4;
			unsigned short CURPIPE : 4;
#endif
	} BIT;
	} D0FIFOSEL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DTLN : 9;
			unsigned short  : 4;
			unsigned short FRDY : 1;
			unsigned short BCLR : 1;
			unsigned short BVAL : 1;
#else
			unsigned short BVAL : 1;
			unsigned short BCLR : 1;
			unsigned short FRDY : 1;
			unsigned short  : 4;
			unsigned short DTLN : 9;
#endif
	} BIT;
	} D0FIFOCTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CURPIPE : 4;
			unsigned short  : 4;
			unsigned short BIGEND : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short DREQE : 1;
			unsigned short DCLRM : 1;
			unsigned short REW : 1;
			unsigned short RCNT : 1;
#else
			unsigned short RCNT : 1;
			unsigned short REW : 1;
			unsigned short DCLRM : 1;
			unsigned short DREQE : 1;
			unsigned short  : 1;
			unsigned short MBW : 1;
			unsigned short  : 1;
			unsigned short BIGEND : 1;
			unsigned short  : 4;
			unsigned short CURPIPE : 4;
#endif
	} BIT;
	} D1FIFOSEL;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short DTLN : 9;
			unsigned short  : 4;
			unsigned short FRDY : 1;
			unsigned short BCLR : 1;
			unsigned short BVAL : 1;
#else
			unsigned short BVAL : 1;
			unsigned short BCLR : 1;
			unsigned short FRDY : 1;
			unsigned short  : 4;
			unsigned short DTLN : 9;
#endif
	} BIT;
	} D1FIFOCTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short BRDYE : 1;
			unsigned short NRDYE : 1;
			unsigned short BEMPE : 1;
			unsigned short CTRE : 1;
			unsigned short DVSE : 1;
			unsigned short SOFE : 1;
			unsigned short RSME : 1;
			unsigned short VBSE : 1;
#else
			unsigned short VBSE : 1;
			unsigned short RSME : 1;
			unsigned short SOFE : 1;
			unsigned short DVSE : 1;
			unsigned short CTRE : 1;
			unsigned short BEMPE : 1;
			unsigned short NRDYE : 1;
			unsigned short BRDYE : 1;
			unsigned short : 8;
#endif
	} BIT;
	} INTENB0;
	char           wk7[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0BRDYE : 1;
			unsigned short PIPE1BRDYE : 1;
			unsigned short PIPE2BRDYE : 1;
			unsigned short PIPE3BRDYE : 1;
			unsigned short PIPE4BRDYE : 1;
			unsigned short PIPE5BRDYE : 1;
			unsigned short PIPE6BRDYE : 1;
			unsigned short PIPE7BRDYE : 1;
			unsigned short PIPE8BRDYE : 1;
			unsigned short PIPE9BRDYE : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9BRDYE : 1;
			unsigned short PIPE8BRDYE : 1;
			unsigned short PIPE7BRDYE : 1;
			unsigned short PIPE6BRDYE : 1;
			unsigned short PIPE5BRDYE : 1;
			unsigned short PIPE4BRDYE : 1;
			unsigned short PIPE3BRDYE : 1;
			unsigned short PIPE2BRDYE : 1;
			unsigned short PIPE1BRDYE : 1;
			unsigned short PIPE0BRDYE : 1;
#endif
	} BIT;
	} BRDYENB;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0NRDYE : 1;
			unsigned short PIPE1NRDYE : 1;
			unsigned short PIPE2NRDYE : 1;
			unsigned short PIPE3NRDYE : 1;
			unsigned short PIPE4NRDYE : 1;
			unsigned short PIPE5NRDYE : 1;
			unsigned short PIPE6NRDYE : 1;
			unsigned short PIPE7NRDYE : 1;
			unsigned short PIPE8NRDYE : 1;
			unsigned short PIPE9NRDYE : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9NRDYE : 1;
			unsigned short PIPE8NRDYE : 1;
			unsigned short PIPE7NRDYE : 1;
			unsigned short PIPE6NRDYE : 1;
			unsigned short PIPE5NRDYE : 1;
			unsigned short PIPE4NRDYE : 1;
			unsigned short PIPE3NRDYE : 1;
			unsigned short PIPE2NRDYE : 1;
			unsigned short PIPE1NRDYE : 1;
			unsigned short PIPE0NRDYE : 1;
#endif
	} BIT;
	} NRDYENB;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0BEMPE : 1;
			unsigned short PIPE1BEMPE : 1;
			unsigned short PIPE2BEMPE : 1;
			unsigned short PIPE3BEMPE : 1;
			unsigned short PIPE4BEMPE : 1;
			unsigned short PIPE5BEMPE : 1;
			unsigned short PIPE6BEMPE : 1;
			unsigned short PIPE7BEMPE : 1;
			unsigned short PIPE8BEMPE : 1;
			unsigned short PIPE9BEMPE : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9BEMPE : 1;
			unsigned short PIPE8BEMPE : 1;
			unsigned short PIPE7BEMPE : 1;
			unsigned short PIPE6BEMPE : 1;
			unsigned short PIPE5BEMPE : 1;
			unsigned short PIPE4BEMPE : 1;
			unsigned short PIPE3BEMPE : 1;
			unsigned short PIPE2BEMPE : 1;
			unsigned short PIPE1BEMPE : 1;
			unsigned short PIPE0BEMPE : 1;
#endif
	} BIT;
	} BEMPENB;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 4;
			unsigned short EDGESTS : 1;
			unsigned short  : 1;
			unsigned short BRDYM : 1;
			unsigned short  : 9;
#else
			unsigned short  : 9;
			unsigned short BRDYM : 1;
			unsigned short  : 1;
			unsigned short EDGESTS : 1;
			unsigned short : 4;
#endif
	} BIT;
	} SOFCFG;
	char           wk8[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short CTSQ : 3;
			unsigned short VALID : 1;
			unsigned short DVSQ : 3;
			unsigned short VBSTS : 1;
			unsigned short BRDY : 1;
			unsigned short NRDY : 1;
			unsigned short BEMP : 1;
			unsigned short CTRT : 1;
			unsigned short DVST : 1;
			unsigned short SOFR : 1;
			unsigned short RESM : 1;
			unsigned short VBINT : 1;
#else
			unsigned short VBINT : 1;
			unsigned short RESM : 1;
			unsigned short SOFR : 1;
			unsigned short DVST : 1;
			unsigned short CTRT : 1;
			unsigned short BEMP : 1;
			unsigned short NRDY : 1;
			unsigned short BRDY : 1;
			unsigned short VBSTS : 1;
			unsigned short DVSQ : 3;
			unsigned short VALID : 1;
			unsigned short CTSQ : 3;
#endif
	} BIT;
	} INTSTS0;
	char           wk9[4];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0BRDY : 1;
			unsigned short PIPE1BRDY : 1;
			unsigned short PIPE2BRDY : 1;
			unsigned short PIPE3BRDY : 1;
			unsigned short PIPE4BRDY : 1;
			unsigned short PIPE5BRDY : 1;
			unsigned short PIPE6BRDY : 1;
			unsigned short PIPE7BRDY : 1;
			unsigned short PIPE8BRDY : 1;
			unsigned short PIPE9BRDY : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9BRDY : 1;
			unsigned short PIPE8BRDY : 1;
			unsigned short PIPE7BRDY : 1;
			unsigned short PIPE6BRDY : 1;
			unsigned short PIPE5BRDY : 1;
			unsigned short PIPE4BRDY : 1;
			unsigned short PIPE3BRDY : 1;
			unsigned short PIPE2BRDY : 1;
			unsigned short PIPE1BRDY : 1;
			unsigned short PIPE0BRDY : 1;
#endif
	} BIT;
	} BRDYSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0NRDY : 1;
			unsigned short PIPE1NRDY : 1;
			unsigned short PIPE2NRDY : 1;
			unsigned short PIPE3NRDY : 1;
			unsigned short PIPE4NRDY : 1;
			unsigned short PIPE5NRDY : 1;
			unsigned short PIPE6NRDY : 1;
			unsigned short PIPE7NRDY : 1;
			unsigned short PIPE8NRDY : 1;
			unsigned short PIPE9NRDY : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9NRDY : 1;
			unsigned short PIPE8NRDY : 1;
			unsigned short PIPE7NRDY : 1;
			unsigned short PIPE6NRDY : 1;
			unsigned short PIPE5NRDY : 1;
			unsigned short PIPE4NRDY : 1;
			unsigned short PIPE3NRDY : 1;
			unsigned short PIPE2NRDY : 1;
			unsigned short PIPE1NRDY : 1;
			unsigned short PIPE0NRDY : 1;
#endif
	} BIT;
	} NRDYSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPE0BEMP : 1;
			unsigned short PIPE1BEMP : 1;
			unsigned short PIPE2BEMP : 1;
			unsigned short PIPE3BEMP : 1;
			unsigned short PIPE4BEMP : 1;
			unsigned short PIPE5BEMP : 1;
			unsigned short PIPE6BEMP : 1;
			unsigned short PIPE7BEMP : 1;
			unsigned short PIPE8BEMP : 1;
			unsigned short PIPE9BEMP : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short PIPE9BEMP : 1;
			unsigned short PIPE8BEMP : 1;
			unsigned short PIPE7BEMP : 1;
			unsigned short PIPE6BEMP : 1;
			unsigned short PIPE5BEMP : 1;
			unsigned short PIPE4BEMP : 1;
			unsigned short PIPE3BEMP : 1;
			unsigned short PIPE2BEMP : 1;
			unsigned short PIPE1BEMP : 1;
			unsigned short PIPE0BEMP : 1;
#endif
	} BIT;
	} BEMPSTS;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short FRNM : 11;
			unsigned short  : 3;
			unsigned short CRCE : 1;
			unsigned short OVRN : 1;
#else
			unsigned short OVRN : 1;
			unsigned short CRCE : 1;
			unsigned short  : 3;
			unsigned short FRNM : 11;
#endif
	} BIT;
	} FRMNUM;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 15;
			unsigned short DVCHG : 1;
#else
			unsigned short DVCHG : 1;
			unsigned short : 15;
#endif
	} BIT;
	} DVCHGR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short USBADDR : 7;
			unsigned short  : 1;
			unsigned short STSRECOV : 4;
			unsigned short  : 4;
#else
			unsigned short  : 4;
			unsigned short STSRECOV : 4;
			unsigned short  : 1;
			unsigned short USBADDR : 7;
#endif
	} BIT;
	} USBADDR;
	char           wk10[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short BMREQUESTTYPE : 8;
			unsigned short BREQUEST : 8;
#else
			unsigned short BREQUEST : 8;
			unsigned short BMREQUESTTYPE : 8;
#endif
	} BIT;
	} USBREQ;
	unsigned short USBVAL;
	unsigned short USBINDX;
	unsigned short USBLENG;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 4;
			unsigned short DIR : 1;
			unsigned short  : 2;
			unsigned short SHTNAK : 1;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short SHTNAK : 1;
			unsigned short  : 2;
			unsigned short DIR : 1;
			unsigned short : 4;
#endif
	} BIT;
	} DCPCFG;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MXPS : 7;
			unsigned short  : 5;
			unsigned short DEVSEL : 4;
#else
			unsigned short DEVSEL : 4;
			unsigned short  : 5;
			unsigned short MXPS : 7;
#endif
	} BIT;
	} DCPMAXP;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short CCPL : 1;
			unsigned short  : 2;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short  : 2;
			unsigned short SUREQCLR : 1;
			unsigned short  : 2;
			unsigned short SUREQ : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short SUREQ : 1;
			unsigned short  : 2;
			unsigned short SUREQCLR : 1;
			unsigned short  : 2;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 2;
			unsigned short CCPL : 1;
			unsigned short PID : 2;
#endif
	} BIT;
	} DCPCTR;
	char           wk11[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PIPESEL : 4;
			unsigned short  : 12;
#else
			unsigned short  : 12;
			unsigned short PIPESEL : 4;
#endif
	} BIT;
	} PIPESEL;
	char           wk12[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short EPNUM : 4;
			unsigned short DIR : 1;
			unsigned short  : 2;
			unsigned short SHTNAK : 1;
			unsigned short  : 1;
			unsigned short DBLB : 1;
			unsigned short BFRE : 1;
			unsigned short  : 3;
			unsigned short TYPE : 2;
#else
			unsigned short TYPE : 2;
			unsigned short  : 3;
			unsigned short BFRE : 1;
			unsigned short DBLB : 1;
			unsigned short  : 1;
			unsigned short SHTNAK : 1;
			unsigned short  : 2;
			unsigned short DIR : 1;
			unsigned short EPNUM : 4;
#endif
	} BIT;
	} PIPECFG;
	char           wk13[2];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short MXPS : 9;
			unsigned short  : 3;
			unsigned short DEVSEL : 4;
#else
			unsigned short DEVSEL : 4;
			unsigned short  : 3;
			unsigned short MXPS : 9;
#endif
	} BIT;
	} PIPEMAXP;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short IITV : 3;
			unsigned short  : 9;
			unsigned short IFIS : 1;
			unsigned short  : 3;
#else
			unsigned short  : 3;
			unsigned short IFIS : 1;
			unsigned short  : 9;
			unsigned short IITV : 3;
#endif
	} BIT;
	} PIPEPERI;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE1CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE2CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE3CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE4CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short ATREPM : 1;
			unsigned short  : 3;
			unsigned short INBUFM : 1;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short INBUFM : 1;
			unsigned short  : 3;
			unsigned short ATREPM : 1;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE5CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short  : 5;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short  : 5;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE6CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short  : 5;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short  : 5;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE7CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short  : 5;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short  : 5;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE8CTR;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short PID : 2;
			unsigned short  : 3;
			unsigned short PBUSY : 1;
			unsigned short SQMON : 1;
			unsigned short SQSET : 1;
			unsigned short SQCLR : 1;
			unsigned short ACLRM : 1;
			unsigned short  : 5;
			unsigned short BSTS : 1;
#else
			unsigned short BSTS : 1;
			unsigned short  : 5;
			unsigned short ACLRM : 1;
			unsigned short SQCLR : 1;
			unsigned short SQSET : 1;
			unsigned short SQMON : 1;
			unsigned short PBUSY : 1;
			unsigned short  : 3;
			unsigned short PID : 2;
#endif
	} BIT;
	} PIPE9CTR;
	char           wk14[14];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE1TRE;
	unsigned short PIPE1TRN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE2TRE;
	unsigned short PIPE2TRN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE3TRE;
	unsigned short PIPE3TRN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE4TRE;
	unsigned short PIPE4TRN;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 8;
			unsigned short TRCLR : 1;
			unsigned short TRENB : 1;
			unsigned short  : 6;
#else
			unsigned short  : 6;
			unsigned short TRENB : 1;
			unsigned short TRCLR : 1;
			unsigned short : 8;
#endif
	} BIT;
	} PIPE5TRE;
	unsigned short PIPE5TRN;
	char           wk15[44];
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD0;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD1;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD2;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD3;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD4;
	union {
		unsigned short WORD;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned short : 6;
			unsigned short USBSPD : 2;
			unsigned short  : 8;
#else
			unsigned short  : 8;
			unsigned short USBSPD : 2;
			unsigned short : 6;
#endif
	} BIT;
	} DEVADD5;
};

struct st_wdt {
	unsigned char  WDTRR;
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
	} WDTCR;
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
	} WDTSR;
	union {
		unsigned char BYTE;
		struct {
			
#ifdef __RX_LITTLE_ENDIAN__
			unsigned char : 7;
			unsigned char RSTIRQS : 1;
#else
			unsigned char RSTIRQS : 1;
			unsigned char : 7;
#endif
	} BIT;
	} WDTRCR;
};

enum enum_ir {
IR_BSC_BUSERR=16,IR_FCU_FIFERR=21,IR_FCU_FRDYI=23,
IR_ICU_SWINT=27,
IR_CMT0_CMI0,
IR_CMT1_CMI1,
IR_CMT2_CMI2,
IR_CMT3_CMI3,
IR_ETHER_EINT,
IR_USB0_D0FIFO0,IR_USB0_D1FIFO0,IR_USB0_USBI0,
IR_USB1_D0FIFO1,IR_USB1_D1FIFO1,IR_USB1_USBI1,
IR_RSPI0_SPRI0,IR_RSPI0_SPTI0,IR_RSPI0_SPII0,
IR_RSPI1_SPRI1,IR_RSPI1_SPTI1,IR_RSPI1_SPII1,
IR_RSPI2_SPRI2,IR_RSPI2_SPTI2,IR_RSPI2_SPII2,
IR_CAN0_RXF0,IR_CAN0_TXF0,IR_CAN0_RXM0,IR_CAN0_TXM0,
IR_CAN1_RXF1,IR_CAN1_TXF1,IR_CAN1_RXM1,IR_CAN1_TXM1,
IR_CAN2_RXF2,IR_CAN2_TXF2,IR_CAN2_RXM2,IR_CAN2_TXM2,
IR_RTC_CUP=62,
IR_ICU_IRQ0=64,IR_ICU_IRQ1,IR_ICU_IRQ2,IR_ICU_IRQ3,IR_ICU_IRQ4,IR_ICU_IRQ5,IR_ICU_IRQ6,IR_ICU_IRQ7,IR_ICU_IRQ8,IR_ICU_IRQ9,IR_ICU_IRQ10,IR_ICU_IRQ11,IR_ICU_IRQ12,IR_ICU_IRQ13,IR_ICU_IRQ14,IR_ICU_IRQ15,
IR_USB_USBR0=90,IR_USB_USBR1,
IR_RTC_ALM,IR_RTC_PRD,
IR_AD_ADI0=98,
IR_S12AD_S12ADI0=102,
IR_ICU_GROUP0=106,IR_ICU_GROUP1,IR_ICU_GROUP2,IR_ICU_GROUP3,IR_ICU_GROUP4,IR_ICU_GROUP5,IR_ICU_GROUP6,IR_ICU_GROUP12=114,
IR_SCI12_SCIX0=122,IR_SCI12_SCIX1,IR_SCI12_SCIX2,IR_SCI12_SCIX3,
IR_TPU0_TGI0A,IR_TPU0_TGI0B,IR_TPU0_TGI0C,IR_TPU0_TGI0D,
IR_TPU1_TGI1A,IR_TPU1_TGI1B,
IR_TPU2_TGI2A,IR_TPU2_TGI2B,
IR_TPU3_TGI3A,IR_TPU3_TGI3B,IR_TPU3_TGI3C,IR_TPU3_TGI3D,
IR_TPU4_TGI4A,IR_TPU4_TGI4B,
IR_TPU5_TGI5A,IR_TPU5_TGI5B,
IR_TPU6_TGI6A,IR_TPU6_TGI6B,IR_TPU6_TGI6C,IR_TPU6_TGI6D,
IR_MTU0_TGIA0=142,IR_MTU0_TGIB0,IR_MTU0_TGIC0,IR_MTU0_TGID0,IR_MTU0_TGIE0,IR_MTU0_TGIF0,
IR_TPU7_TGI7A,IR_TPU7_TGI7B,
IR_MTU1_TGIA1=148,IR_MTU1_TGIB1,
IR_TPU8_TGI8A,IR_TPU8_TGI8B,
IR_MTU2_TGIA2=150,IR_MTU2_TGIB2,
IR_TPU9_TGI9A,IR_TPU9_TGI9B,IR_TPU9_TGI9C,IR_TPU9_TGI9D,
IR_MTU3_TGIA3=152,IR_MTU3_TGIB3,IR_MTU3_TGIC3,IR_MTU3_TGID3,
IR_TPU10_TGI10A,IR_TPU10_TGI10B,
IR_MTU4_TGIA4=156,IR_MTU4_TGIB4,IR_MTU4_TGIC4,IR_MTU4_TGID4,IR_MTU4_TCIV4,
IR_TPU11_TGI11A=164,IR_TPU11_TGI11B,
IR_MTU5_TGIU5=161,IR_MTU5_TGIV5,IR_MTU5_TGIW5,
IR_POE_OEI1=166,IR_POE_OEI2,
IR_TMR0_CMIA0=170,IR_TMR0_CMIB0,IR_TMR0_OVI0,
IR_TMR1_CMIA1,IR_TMR1_CMIB1,IR_TMR1_OVI1,
IR_TMR2_CMIA2,IR_TMR2_CMIB2,IR_TMR2_OVI2,
IR_TMR3_CMIA3,IR_TMR3_CMIB3,IR_TMR3_OVI3,
IR_RIIC0_EEI0,IR_RIIC0_RXI0,IR_RIIC0_TXI0,IR_RIIC0_TEI0,
IR_RIIC1_EEI1,IR_RIIC1_RXI1,IR_RIIC1_TXI1,IR_RIIC1_TEI1,
IR_RIIC2_EEI2,IR_RIIC2_RXI2,IR_RIIC2_TXI2,IR_RIIC2_TEI2,
IR_RIIC3_EEI3,IR_RIIC3_RXI3,IR_RIIC3_TXI3,IR_RIIC3_TEI3,
IR_DMAC_DMAC0I,IR_DMAC_DMAC1I,IR_DMAC_DMAC2I,IR_DMAC_DMAC3I,
IR_EXDMAC_EXDMAC0I,IR_EXDMAC_EXDMAC1I,
IR_DEU_DEU0=206,IR_DEU_DEU1,
IR_PDC_PCDFI,IR_PDC_PCFEI,IR_PDC_PCERI,
IR_SCI0_RXI0=214,IR_SCI0_TXI0,IR_SCI0_TEI0,
IR_SCI1_RXI1,IR_SCI1_TXI1,IR_SCI1_TEI1,
IR_SCI2_RXI2,IR_SCI2_TXI2,IR_SCI2_TEI2,
IR_SCI3_RXI3,IR_SCI3_TXI3,IR_SCI3_TEI3,
IR_SCI4_RXI4,IR_SCI4_TXI4,IR_SCI4_TEI4,
IR_SCI5_RXI5,IR_SCI5_TXI5,IR_SCI5_TEI5,
IR_SCI6_RXI6,IR_SCI6_TXI6,IR_SCI6_TEI6,
IR_SCI7_RXI7,IR_SCI7_TXI7,IR_SCI7_TEI7,
IR_SCI8_RXI8,IR_SCI8_TXI8,IR_SCI8_TEI8,
IR_SCI9_RXI9,IR_SCI9_TXI9,IR_SCI9_TEI9,
IR_SCI10_RXI10,IR_SCI10_TXI10,IR_SCI10_TEI10,
IR_SCI11_RXI11,IR_SCI11_TXI11,IR_SCI11_TEI11,
IR_SCI12_RXI12,IR_SCI12_TXI12,IR_SCI12_TEI12,
IR_IEB_IEBINT
};

enum enum_dtce {
DTCE_ICU_SWINT=27,
DTCE_CMT0_CMI0,
DTCE_CMT1_CMI1,
DTCE_CMT2_CMI2,
DTCE_CMT3_CMI3,
DTCE_USB0_D0FIFO0=33,DTCE_USB0_D1FIFO0,
DTCE_USB1_D0FIFO1=36,DTCE_USB1_D1FIFO1,
DTCE_RSPI0_SPRI0=39,DTCE_RSPI0_SPTI0,
DTCE_RSPI1_SPRI1=42,DTCE_RSPI1_SPTI1,
DTCE_RSPI2_SPRI2=45,DTCE_RSPI2_SPTI2,
DTCE_ICU_IRQ0=64,DTCE_ICU_IRQ1,DTCE_ICU_IRQ2,DTCE_ICU_IRQ3,DTCE_ICU_IRQ4,DTCE_ICU_IRQ5,DTCE_ICU_IRQ6,DTCE_ICU_IRQ7,DTCE_ICU_IRQ8,DTCE_ICU_IRQ9,DTCE_ICU_IRQ10,DTCE_ICU_IRQ11,DTCE_ICU_IRQ12,DTCE_ICU_IRQ13,DTCE_ICU_IRQ14,DTCE_ICU_IRQ15,
DTCE_AD_ADI0=98,
DTCE_S12AD_S12ADI0=102,
DTCE_TPU0_TGI0A=126,DTCE_TPU0_TGI0B,DTCE_TPU0_TGI0C,DTCE_TPU0_TGI0D,
DTCE_TPU1_TGI1A,DTCE_TPU1_TGI1B,
DTCE_TPU2_TGI2A,DTCE_TPU2_TGI2B,
DTCE_TPU3_TGI3A,DTCE_TPU3_TGI3B,DTCE_TPU3_TGI3C,DTCE_TPU3_TGI3D,
DTCE_TPU4_TGI4A,DTCE_TPU4_TGI4B,
DTCE_TPU5_TGI5A,DTCE_TPU5_TGI5B,
DTCE_TPU6_TGI6A,DTCE_TPU6_TGI6B,DTCE_TPU6_TGI6C,DTCE_TPU6_TGI6D,
DTCE_MTU0_TGIA0=142,DTCE_MTU0_TGIB0,DTCE_MTU0_TGIC0,DTCE_MTU0_TGID0,
DTCE_TPU7_TGI7A=148,DTCE_TPU7_TGI7B,
DTCE_MTU1_TGIA1=148,DTCE_MTU1_TGIB1,
DTCE_TPU8_TGI8A,DTCE_TPU8_TGI8B,
DTCE_MTU2_TGIA2=150,DTCE_MTU2_TGIB2,
DTCE_TPU9_TGI9A,DTCE_TPU9_TGI9B,DTCE_TPU9_TGI9C,DTCE_TPU9_TGI9D,
DTCE_MTU3_TGIA3=152,DTCE_MTU3_TGIB3,DTCE_MTU3_TGIC3,DTCE_MTU3_TGID3,
DTCE_TPU10_TGI10A,DTCE_TPU10_TGI10B,
DTCE_MTU4_TGIA4=156,DTCE_MTU4_TGIB4,DTCE_MTU4_TGIC4,DTCE_MTU4_TGID4,DTCE_MTU4_TCIV4,
DTCE_TPU11_TGI11A=164,DTCE_TPU11_TGI11B,
DTCE_MTU5_TGIU5=161,DTCE_MTU5_TGIV5,DTCE_MTU5_TGIW5,
DTCE_TMR0_CMIA0=170,DTCE_TMR0_CMIB0,
DTCE_TMR1_CMIA1=173,DTCE_TMR1_CMIB1,
DTCE_TMR2_CMIA2=176,DTCE_TMR2_CMIB2,
DTCE_TMR3_CMIA3=179,DTCE_TMR3_CMIB3,
DTCE_RIIC0_RXI0=183,DTCE_RIIC0_TXI0,
DTCE_RIIC1_RXI1=187,DTCE_RIIC1_TXI1,
DTCE_RIIC2_RXI2=191,DTCE_RIIC2_TXI2,
DTCE_RIIC3_RXI3=195,DTCE_RIIC3_TXI3,
DTCE_DMAC_DMAC0I=198,DTCE_DMAC_DMAC1I,DTCE_DMAC_DMAC2I,DTCE_DMAC_DMAC3I,
DTCE_EXDMAC_EXDMAC0I,DTCE_EXDMAC_EXDMAC1I,
DTCE_DEU_DEU0=206, DTCE_DEU_DEU1,
DTCE_PDC_PCDFI,
DTCE_SCI0_RXI0=214,DTCE_SCI0_TXI0,
DTCE_SCI1_RXI1=217,DTCE_SCI1_TXI1,
DTCE_SCI2_RXI2=220,DTCE_SCI2_TXI2,
DTCE_SCI3_RXI3=223,DTCE_SCI3_TXI3,
DTCE_SCI4_RXI4=226,DTCE_SCI4_TXI4,
DTCE_SCI5_RXI5=229,DTCE_SCI5_TXI5,
DTCE_SCI6_RXI6=232,DTCE_SCI6_TXI6,
DTCE_SCI7_RXI7=235,DTCE_SCI7_TXI7,
DTCE_SCI8_RXI8=238,DTCE_SCI8_TXI8,
DTCE_SCI9_RXI9=241,DTCE_SCI9_TXI9,
DTCE_SCI10_RXI10=244,DTCE_SCI10_TXI10,
DTCE_SCI11_RXI11=247,DTCE_SCI11_TXI11,
DTCE_SCI12_RXI12=250,DTCE_SCI12_TXI12
};

enum enum_ier {
IER_BSC_BUSERR=0x02,
IER_FCU_FIFERR=0x02,IER_FCU_FRDYI=0x02,
IER_ICU_SWINT=0x03,
IER_CMT0_CMI0=0x03,
IER_CMT1_CMI1=0x03,
IER_CMT2_CMI2=0x03,
IER_CMT3_CMI3=0x03,
IER_ETHER_EINT=0x04,
IER_USB0_D0FIFO0=0x04,IER_USB0_D1FIFO0=0x04,IER_USB0_USBI0=0x04,
IER_USB1_D0FIFO1=0x04,IER_USB1_D1FIFO1=0x04,IER_USB1_USBI1=0x04,
IER_RSPI0_SPRI0=0x04,IER_RSPI0_SPTI0=0x05,IER_RSPI0_SPII0=0x05,
IER_RSPI1_SPRI1=0x05,IER_RSPI1_SPTI1=0x05,IER_RSPI1_SPII1=0x05,
IER_RSPI2_SPRI2=0x05,IER_RSPI2_SPTI2=0x05,IER_RSPI2_SPII2=0x05,
IER_CAN0_RXF0=0x06,IER_CAN0_TXF0=0x06,IER_CAN0_RXM0=0x06,IER_CAN0_TXM0=0x06,
IER_CAN1_RXF1=0x06,IER_CAN1_TXF1=0x06,IER_CAN1_RXM1=0x06,IER_CAN1_TXM1=0x06,
IER_CAN2_RXF2=0x07,IER_CAN2_TXF2=0x07,IER_CAN2_RXM2=0x07,IER_CAN2_TXM2=0x07,
IER_RTC_CUP=0x07,
IER_ICU_IRQ0=0x08,IER_ICU_IRQ1=0x08,IER_ICU_IRQ2=0x08,IER_ICU_IRQ3=0x08,IER_ICU_IRQ4=0x08,IER_ICU_IRQ5=0x08,IER_ICU_IRQ6=0x08,IER_ICU_IRQ7=0x08,IER_ICU_IRQ8=0x09,IER_ICU_IRQ9=0x09,IER_ICU_IRQ10=0x09,IER_ICU_IRQ11=0x09,IER_ICU_IRQ12=0x09,IER_ICU_IRQ13=0x09,IER_ICU_IRQ14=0x09,IER_ICU_IRQ15=0x09,
IER_USB_USBR0=0x0B,IER_USB_USBR1=0x0B,
IER_RTC_ALM=0x0B,IER_RTC_PRD=0x0B,
IER_AD_ADI0=0x0C,
IER_S12AD_S12ADI0=0x0C,
IER_ICU_GROUP0=0x0D,IER_ICU_GROUP1=0x0D,IER_ICU_GROUP2=0x0D,IER_ICU_GROUP3=0x0D,IER_ICU_GROUP4=0x0D,IER_ICU_GROUP5=0x0D,IER_ICU_GROUP6=0x0E,IER_ICU_GROUP12=0x0E,
IER_SCI12_SCIX0=0x0F,IER_SCI12_SCIX1=0x0F,IER_SCI12_SCIX2=0x0F,IER_SCI12_SCIX3=0x0F,
IER_TPU0_TGI0A=0x0F,IER_TPU0_TGI0B=0x0F,IER_TPU0_TGI0C=0x10,IER_TPU0_TGI0D=0x10,
IER_TPU1_TGI1A=0x10,IER_TPU1_TGI1B=0x10,
IER_TPU2_TGI2A=0x10,IER_TPU2_TGI2B=0x10,
IER_TPU3_TGI3A=0x10,IER_TPU3_TGI3B=0x10,IER_TPU3_TGI3C=0x11,IER_TPU3_TGI3D=0x11,
IER_TPU4_TGI4A=0x11,IER_TPU4_TGI4B=0x11,
IER_TPU5_TGI5A=0x11,IER_TPU5_TGI5B=0x11,
IER_TPU6_TGI6A=0x11,IER_TPU6_TGI6B=0x11,IER_TPU6_TGI6C=0x12,IER_TPU6_TGI6D=0x12,
IER_MTU0_TGIA0=0x11,IER_MTU0_TGIB0=0x11,IER_MTU0_TGIC0=0x12,IER_MTU0_TGID0=0x12,IER_MTU0_TGIE0=0x12,IER_MTU0_TGIF0=0x12,
IER_TPU7_TGI7A=0x12,IER_TPU7_TGI7B=0x12,
IER_MTU1_TGIA1=0x12,IER_MTU1_TGIB1=0x12,
IER_TPU8_TGI8A=0x12,IER_TPU8_TGI8B=0x12,
IER_MTU2_TGIA2=0x12,IER_MTU2_TGIB2=0x12,
IER_TPU9_TGI9A=0x13,IER_TPU9_TGI9B=0x13,IER_TPU9_TGI9C=0x13,IER_TPU9_TGI9D=0x13,
IER_MTU3_TGIA3=0x13,IER_MTU3_TGIB3=0x13,IER_MTU3_TGIC3=0x13,IER_MTU3_TGID3=0x13,
IER_TPU10_TGI10A=0x13,IER_TPU10_TGI10B=0x13,
IER_MTU4_TGIA4=0x13,IER_MTU4_TGIB4=0x13,IER_MTU4_TGIC4=0x13,IER_MTU4_TGID4=0x13,IER_MTU4_TCIV4=0x14,
IER_TPU11_TGI11A=0x14,IER_TPU11_TGI11B=0x14,
IER_MTU5_TGIU5=0x14,IER_MTU5_TGIV5=0x14,IER_MTU5_TGIW5=0x14,
IER_POE_OEI1=0x14,IER_POE_OEI2=0x14,
IER_TMR0_CMIA0=0x15,IER_TMR0_CMIB0=0x15,IER_TMR0_OVI0=0x15,
IER_TMR1_CMIA1=0x15,IER_TMR1_CMIB1=0x15,IER_TMR1_OVI1=0x15,
IER_TMR2_CMIA2=0x16,IER_TMR2_CMIB2=0x16,IER_TMR2_OVI2=0x16,
IER_TMR3_CMIA3=0x16,IER_TMR3_CMIB3=0x16,IER_TMR3_OVI3=0x16,
IER_RIIC0_EEI0=0x16,IER_RIIC0_RXI0=0x16,IER_RIIC0_TXI0=0x17,IER_RIIC0_TEI0=0x17,
IER_RIIC1_EEI1=0x17,IER_RIIC1_RXI1=0x17,IER_RIIC1_TXI1=0x17,IER_RIIC1_TEI1=0x17,
IER_RIIC2_EEI2=0x17,IER_RIIC2_RXI2=0x17,IER_RIIC2_TXI2=0x18,IER_RIIC2_TEI2=0x18,
IER_RIIC3_EEI3=0x18,IER_RIIC3_RXI3=0x18,IER_RIIC3_TXI3=0x18,IER_RIIC3_TEI3=0x18,
IER_DMAC_DMAC0I=0x18,IER_DMAC_DMAC1I=0x18,IER_DMAC_DMAC2I=0x19,IER_DMAC_DMAC3I=0x19,
IER_EXDMAC_EXDMAC0I=0x19,IER_EXDMAC_EXDMAC1I=0x19,
IER_DEU_DEU0=0x19,IER_DEU_DEU1=0x19,
IER_PDC_PCDFI=0x1A,IER_PDC_PCFEI=0x1A,IER_PDC_PCERI=0x1A,
IER_SCI0_RXI0=0x1A,IER_SCI0_TXI0=0x1A,IER_SCI0_TEI0=0x1B,
IER_SCI1_RXI1=0x1B,IER_SCI1_TXI1=0x1B,IER_SCI1_TEI1=0x1B,
IER_SCI2_RXI2=0x1B,IER_SCI2_TXI2=0x1B,IER_SCI2_TEI2=0x1B,
IER_SCI3_RXI3=0x1B,IER_SCI3_TXI3=0x1C,IER_SCI3_TEI3=0x1C,
IER_SCI4_RXI4=0x1C,IER_SCI4_TXI4=0x1C,IER_SCI4_TEI4=0x1C,
IER_SCI5_RXI5=0x1C,IER_SCI5_TXI5=0x1C,IER_SCI5_TEI5=0x1C,
IER_SCI6_RXI6=0x1D,IER_SCI6_TXI6=0x1D,IER_SCI6_TEI6=0x1D,
IER_SCI7_RXI7=0x1D,IER_SCI7_TXI7=0x1D,IER_SCI7_TEI7=0x1D,
IER_SCI8_RXI8=0x1D,IER_SCI8_TXI8=0x1D,IER_SCI8_TEI8=0x1E,
IER_SCI9_RXI9=0x1E,IER_SCI9_TXI9=0x1E,IER_SCI9_TEI9=0x1E,
IER_SCI10_RXI10=0x1E,IER_SCI10_TXI10=0x1E,IER_SCI10_TEI10=0x1E,
IER_SCI11_RXI11=0x1E,IER_SCI11_TXI11=0x1F,IER_SCI11_TEI11=0x1F,
IER_SCI12_RXI12=0x1F,IER_SCI12_TXI12=0x1F,IER_SCI12_TEI12=0x1F,
IER_IEB_IEBINT=0x1F
};

enum enum_ipr {
IPR_BSC_BUSERR=0,
IPR_FCU_FIFERR=1,IPR_FCU_FRDYI=2,
IPR_ICU_SWINT=3,
IPR_CMT0_CMI0=4,
IPR_CMT1_CMI1=5,
IPR_CMT2_CMI2=6,
IPR_CMT3_CMI3=7,
IPR_ETHER_EINT=32,
IPR_USB0_D0FIFO0=33,IPR_USB0_D1FIFO0=34,IPR_USB0_USBI0=35,
IPR_USB1_D0FIFO1=36,IPR_USB1_D1FIFO1=37,IPR_USB1_USBI1=38,
IPR_RSPI0_SPRI0=39,IPR_RSPI0_SPTI0=39,IPR_RSPI0_SPII0=39,
IPR_RSPI1_SPRI1=42,IPR_RSPI1_SPTI1=42,IPR_RSPI1_SPII1=42,
IPR_RSPI2_SPRI2=45,IPR_RSPI2_SPTI2=45,IPR_RSPI2_SPII2=45,
IPR_CAN0_RXF0=48,IPR_CAN0_TXF0=48,IPR_CAN0_RXM0=48,IPR_CAN0_TXM0=48,
IPR_CAN1_RXF1=52,IPR_CAN1_TXF1=52,IPR_CAN1_RXM1=52,IPR_CAN1_TXM1=52,
IPR_CAN2_RXF2=56,IPR_CAN2_TXF2=56,IPR_CAN2_RXM2=56,IPR_CAN2_TXM2=56,
IPR_RTC_CUP=62,
IPR_ICU_IRQ0=64,IPR_ICU_IRQ1=65,IPR_ICU_IRQ2=66,IPR_ICU_IRQ3=67,IPR_ICU_IRQ4=68,IPR_ICU_IRQ5=69,IPR_ICU_IRQ6=70,IPR_ICU_IRQ7=71,IPR_ICU_IRQ8=72,IPR_ICU_IRQ9=73,IPR_ICU_IRQ10=74,IPR_ICU_IRQ11=75,IPR_ICU_IRQ12=76,IPR_ICU_IRQ13=77,IPR_ICU_IRQ14=78,IPR_ICU_IRQ15=79,
IPR_USB_USBR0=90,IPR_USB_USBR1=91,
IPR_RTC_ALM=92,IPR_RTC_PRD=93,
IPR_AD_ADI0=98,
IPR_S12AD_S12ADI0=102,
IPR_ICU_GROUP0=106,IPR_ICU_GROUP1=107,IPR_ICU_GROUP2=108,IPR_ICU_GROUP3=109,IPR_ICU_GROUP4=110,IPR_ICU_GROUP5=111,IPR_ICU_GROUP6=112,IPR_ICU_GROUP12=114,
IPR_SCI12_SCIX0=122,IPR_SCI12_SCIX1=122,IPR_SCI12_SCIX2=122,IPR_SCI12_SCIX3=122,
IPR_TPU0_TGI0A=126,IPR_TPU0_TGI0B=126,IPR_TPU0_TGI0C=126,IPR_TPU0_TGI0D=126,
IPR_TPU1_TGI1A=130,IPR_TPU1_TGI1B=130,
IPR_TPU2_TGI2A=132,IPR_TPU2_TGI2B=132,
IPR_TPU3_TGI3A=134,IPR_TPU3_TGI3B=134,IPR_TPU3_TGI3C=134,IPR_TPU3_TGI3D=134,
IPR_TPU4_TGI4A=138,IPR_TPU4_TGI4B=138,
IPR_TPU5_TGI5A=140,IPR_TPU5_TGI5B=140,
IPR_TPU6_TGI6A=142,IPR_TPU6_TGI6B=142,IPR_TPU6_TGI6C=142,IPR_TPU6_TGI6D=142,
IPR_MTU0_TGIA0=142,IPR_MTU0_TGIB0=142,IPR_MTU0_TGIC0=142,IPR_MTU0_TGID0=142,IPR_MTU0_TGIE0=146,IPR_MTU0_TGIF0=146,
IPR_TPU7_TGI7A=148,IPR_TPU7_TGI7B=148,
IPR_MTU1_TGIA1=148,IPR_MTU1_TGIB1=148,
IPR_TPU8_TGI8A=150,IPR_TPU8_TGI8B=150,
IPR_MTU2_TGIA2=150,IPR_MTU2_TGIB2=150,
IPR_TPU9_TGI9A=152,IPR_TPU9_TGI9B=152,IPR_TPU9_TGI9C=152,IPR_TPU9_TGI9D=152,
IPR_MTU3_TGIA3=152,IPR_MTU3_TGIB3=152,IPR_MTU3_TGIC3=152,IPR_MTU3_TGID3=152,
IPR_TPU10_TGI10A=156,IPR_TPU10_TGI10B=156,
IPR_MTU4_TGIA4=156,IPR_MTU4_TGIB4=156,IPR_MTU4_TGIC4=156,IPR_MTU4_TGID4=156,IPR_MTU4_TCIV4=160,
IPR_TPU11_TGI11A=164,IPR_TPU11_TGI11B=164,
IPR_MTU5_TGIU5=161,IPR_MTU5_TGIV5=161,IPR_MTU5_TGIW5=161,
IPR_POE_OEI1=166,IPR_POE_OEI2=166,
IPR_TMR0_CMIA0=170,IPR_TMR0_CMIB0=170,IPR_TMR0_OVI0=170,
IPR_TMR1_CMIA1=173,IPR_TMR1_CMIB1=173,IPR_TMR1_OVI1=173,
IPR_TMR2_CMIA2=176,IPR_TMR2_CMIB2=176,IPR_TMR2_OVI2=176,
IPR_TMR3_CMIA3=179,IPR_TMR3_CMIB3=179,IPR_TMR3_OVI3=179,
IPR_RIIC0_EEI0=182,IPR_RIIC0_RXI0=183,IPR_RIIC0_TXI0=184,IPR_RIIC0_TEI0=185,
IPR_RIIC1_EEI1=186,IPR_RIIC1_RXI1=187,IPR_RIIC1_TXI1=188,IPR_RIIC1_TEI1=189,
IPR_RIIC2_EEI2=190,IPR_RIIC2_RXI2=191,IPR_RIIC2_TXI2=192,IPR_RIIC2_TEI2=193,
IPR_RIIC3_EEI3=194,IPR_RIIC3_RXI3=195,IPR_RIIC3_TXI3=196,IPR_RIIC3_TEI3=197,
IPR_DMAC_DMAC0I=198,IPR_DMAC_DMAC1I=199,IPR_DMAC_DMAC2I=200,IPR_DMAC_DMAC3I=201,
IPR_EXDMAC_EXDMAC0I=202,IPR_EXDMAC_EXDMAC1I=203,
IPR_DEU_DEU0=206,IPR_DEU_DEU1=207,
IPR_PDC_PCDFI=208,IPR_PDC_PCFEI=209,IPR_PDC_PCERI=210,
IPR_SCI0_RXI0=214,IPR_SCI0_TXI0=214,IPR_SCI0_TEI0=214,
IPR_SCI1_RXI1=217,IPR_SCI1_TXI1=217,IPR_SCI1_TEI1=217,
IPR_SCI2_RXI2=220,IPR_SCI2_TXI2=220,IPR_SCI2_TEI2=220,
IPR_SCI3_RXI3=223,IPR_SCI3_TXI3=223,IPR_SCI3_TEI3=223,
IPR_SCI4_RXI4=226,IPR_SCI4_TXI4=226,IPR_SCI4_TEI4=226,
IPR_SCI5_RXI5=229,IPR_SCI5_TXI5=229,IPR_SCI5_TEI5=229,
IPR_SCI6_RXI6=232,IPR_SCI6_TXI6=232,IPR_SCI6_TEI6=232,
IPR_SCI7_RXI7=235,IPR_SCI7_TXI7=235,IPR_SCI7_TEI7=235,
IPR_SCI8_RXI8=238,IPR_SCI8_TXI8=238,IPR_SCI8_TEI8=238,
IPR_SCI9_RXI9=241,IPR_SCI9_TXI9=241,IPR_SCI9_TEI9=241,
IPR_SCI10_RXI10=244,IPR_SCI10_TXI10=244,IPR_SCI10_TEI10=244,
IPR_SCI11_RXI11=247,IPR_SCI11_TXI11=247,IPR_SCI11_TEI11=247,
IPR_SCI12_RXI12=250,IPR_SCI12_TXI12=250,IPR_SCI12_TEI12=250,
IPR_IEB_IEBINT=253,
IPR_BSC_=0,
IPR_CMT0_=4,
IPR_CMT1_=5,
IPR_CMT2_=6,
IPR_CMT3_=7,
IPR_ETHER_=32,
IPR_RSPI0_=39,
IPR_RSPI1_=42,
IPR_RSPI2_=45,
IPR_CAN0_=48,
IPR_CAN1_=52,
IPR_CAN2_=56,
IPR_AD_=98,
IPR_S12AD_=102,
IPR_SCI12_SCIX_=122,
IPR_TPU0_=126,
IPR_TPU0_TGI=126,
IPR_TPU1_=130,
IPR_TPU1_TGI=130,
IPR_TPU2_=132,
IPR_TPU2_TGI=132,
IPR_TPU3_=134,
IPR_TPU3_TGI=134,
IPR_TPU4_=138,
IPR_TPU4_TGI=138,
IPR_TPU5_=140,
IPR_TPU5_TGI=140,
IPR_MTU5_=161,
IPR_MTU5_TGI=161,
IPR_TPU11_=164,
IPR_TPU11_TGI=164,
IPR_POE_=166,
IPR_TMR0_=170,
IPR_TMR1_=173,
IPR_TMR2_=176,
IPR_TMR3_=179,
IPR_SCI0_=214,
IPR_SCI1_=217,
IPR_SCI2_=220,
IPR_SCI3_=223,
IPR_SCI4_=226,
IPR_SCI5_=229,
IPR_SCI6_=232,
IPR_SCI7_=235,
IPR_SCI8_=238,
IPR_SCI9_=241,
IPR_SCI10_=244,
IPR_SCI11_=247,
IPR_SCI12_=250,
IPR_IEB_=253
};

enum enum_grp {
GRP_CAN0_ERS0=0,GRP_CAN1_ERS1=0,GRP_CAN2_ERS2=0,
GRP_MTU0_TCIV0=1,GRP_MTU1_TCIV1=1,GRP_MTU1_TCIU1=1,
GRP_MTU2_TCIV2=2,GRP_MTU2_TCIU2=2,GRP_MTU3_TCIV3=2,
GRP_TPU0_TCI0V=3,GRP_TPU1_TCI1V=3,GRP_TPU1_TCI1U=3,GRP_TPU5_TCI5V=3,GRP_TPU5_TCI5U=3,
GRP_TPU2_TCI2V=4,GRP_TPU2_TCI2U=4,GRP_TPU3_TCI3V=4,GRP_TPU4_TCI4V=4,GRP_TPU4_TCI4U=4,
GRP_TPU6_TCI6V=5,GRP_TPU7_TCI7V=5,GRP_TPU7_TCI7U=5,GRP_TPU11_TCI11V=5,GRP_TPU11_TCI11U=5,
GRP_TPU8_TCI8V=6,GRP_TPU8_TCI8U=6,GRP_TPU9_TCI9V=6,GRP_TPU10_TCI10V=6,GRP_TPU10_TCI10U=6,
GRP_SCI0_ERI0=12,GRP_SCI1_ERI1=12,GRP_SCI2_ERI2=12,GRP_SCI3_ERI3=12,GRP_SCI4_ERI4=12,GRP_SCI5_ERI5=12,GRP_SCI6_ERI6=12,
GRP_SCI7_ERI7=12,GRP_SCI8_ERI8=12,GRP_SCI9_ERI9=12,GRP_SCI10_ERI10=12,GRP_SCI11_ERI11=12,GRP_SCI12_ERI12=12,
GRP_RSPI0_SPEI0=12,GRP_RSPI1_SPEI1=12,GRP_RSPI2_SPEI2=12
};

enum enum_gen {
GEN_CAN0_ERS0=0,GEN_CAN1_ERS1=0,GEN_CAN2_ERS2=0,
GEN_MTU0_TCIV0=1,GEN_MTU1_TCIV1=1,GEN_MTU1_TCIU1=1,
GEN_MTU2_TCIV2=2,GEN_MTU2_TCIU2=2,GEN_MTU3_TCIV3=2,
GEN_TPU0_TCI0V=3,GEN_TPU1_TCI1V=3,GEN_TPU1_TCI1U=3,GEN_TPU5_TCI5V=3,GEN_TPU5_TCI5U=3,
GEN_TPU2_TCI2V=4,GEN_TPU2_TCI2U=4,GEN_TPU3_TCI3V=4,GEN_TPU4_TCI4V=4,GEN_TPU4_TCI4U=4,
GEN_TPU6_TCI6V=5,GEN_TPU7_TCI7V=5,GEN_TPU7_TCI7U=5,GEN_TPU11_TCI11V=5,GEN_TPU11_TCI11U=5,
GEN_TPU8_TCI8V=6,GEN_TPU8_TCI8U=6,GEN_TPU9_TCI9V=6,GEN_TPU10_TCI10V=6,GEN_TPU10_TCI10U=6,
GEN_SCI0_ERI0=12,GEN_SCI1_ERI1=12,GEN_SCI2_ERI2=12,GEN_SCI3_ERI3=12,GEN_SCI4_ERI4=12,GEN_SCI5_ERI5=12,GEN_SCI6_ERI6=12,
GEN_SCI7_ERI7=12,GEN_SCI8_ERI8=12,GEN_SCI9_ERI9=12,GEN_SCI10_ERI10=12,GEN_SCI11_ERI11=12,GEN_SCI12_ERI12=12,
GEN_RSPI0_SPEI0=12,GEN_RSPI1_SPEI1=12,GEN_RSPI2_SPEI2=12
};

enum enum_gcr {
GCR_CAN0_ERS0=0,GCR_CAN1_ERS1=0,GCR_CAN2_ERS2=0,
GCR_MTU0_TCIV0=1,GCR_MTU1_TCIV1=1,GCR_MTU1_TCIU1=1,
GCR_MTU2_TCIV2=2,GCR_MTU2_TCIU2=2,GCR_MTU3_TCIV3=2,
GCR_TPU0_TCI0V=3,GCR_TPU1_TCI1V=3,GCR_TPU1_TCI1U=3,GCR_TPU5_TCI5V=3,GCR_TPU5_TCI5U=3,
GCR_TPU2_TCI2V=4,GCR_TPU2_TCI2U=4,GCR_TPU3_TCI3V=4,GCR_TPU4_TCI4V=4,GCR_TPU4_TCI4U=4,
GCR_TPU6_TCI6V=5,GCR_TPU7_TCI7V=5,GCR_TPU7_TCI7U=5,GCR_TPU11_TCI11V=5,GCR_TPU11_TCI11U=5,
GCR_TPU8_TCI8V=6,GCR_TPU8_TCI8U=6,GCR_TPU9_TCI9V=6,GCR_TPU10_TCI10V=6,GCR_TPU10_TCI10U=6
};

#define	IEN_BSC_BUSERR		IEN0
#define	IEN_FCU_FIFERR		IEN5
#define	IEN_FCU_FRDYI		IEN7
#define	IEN_ICU_SWINT		IEN3
#define	IEN_CMT0_CMI0		IEN4
#define	IEN_CMT1_CMI1		IEN5
#define	IEN_CMT2_CMI2		IEN6
#define	IEN_CMT3_CMI3		IEN7
#define	IEN_ETHER_EINT		IEN0
#define	IEN_USB0_D0FIFO0	IEN1
#define	IEN_USB0_D1FIFO0	IEN2
#define	IEN_USB0_USBI0		IEN3
#define	IEN_USB1_D0FIFO1	IEN4
#define	IEN_USB1_D1FIFO1	IEN5
#define	IEN_USB1_USBI1		IEN6
#define	IEN_RSPI0_SPRI0		IEN7
#define	IEN_RSPI0_SPTI0		IEN0
#define	IEN_RSPI0_SPII0		IEN1
#define	IEN_RSPI1_SPRI1		IEN2
#define	IEN_RSPI1_SPTI1		IEN3
#define	IEN_RSPI1_SPII1		IEN4
#define	IEN_RSPI2_SPRI2		IEN5
#define	IEN_RSPI2_SPTI2		IEN6
#define	IEN_RSPI2_SPII2		IEN7
#define	IEN_CAN0_RXF0		IEN0
#define	IEN_CAN0_TXF0		IEN1
#define	IEN_CAN0_RXM0		IEN2
#define	IEN_CAN0_TXM0		IEN3
#define	IEN_CAN1_RXF1		IEN4
#define	IEN_CAN1_TXF1		IEN5
#define	IEN_CAN1_RXM1		IEN6
#define	IEN_CAN1_TXM1		IEN7
#define	IEN_CAN2_RXF2		IEN0
#define	IEN_CAN2_TXF2		IEN1
#define	IEN_CAN2_RXM2		IEN2
#define	IEN_CAN2_TXM2		IEN3
#define	IEN_RTC_CUP			IEN6
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
#define	IEN_USB_USBR0		IEN2
#define	IEN_USB_USBR1		IEN3
#define	IEN_RTC_ALM			IEN4
#define	IEN_RTC_PRD			IEN5
#define	IEN_AD_ADI0			IEN2
#define	IEN_S12AD_S12ADI0	IEN6
#define	IEN_ICU_GROUP0		IEN2
#define	IEN_ICU_GROUP1		IEN3
#define	IEN_ICU_GROUP2		IEN4
#define	IEN_ICU_GROUP3		IEN5
#define	IEN_ICU_GROUP4		IEN6
#define	IEN_ICU_GROUP5		IEN7
#define	IEN_ICU_GROUP6		IEN0
#define	IEN_ICU_GROUP12		IEN2
#define	IEN_SCI12_SCIX0		IEN2
#define	IEN_SCI12_SCIX1		IEN3
#define	IEN_SCI12_SCIX2		IEN4
#define	IEN_SCI12_SCIX3		IEN5
#define	IEN_TPU0_TGI0A		IEN6
#define	IEN_TPU0_TGI0B		IEN7
#define	IEN_TPU0_TGI0C		IEN0
#define	IEN_TPU0_TGI0D		IEN1
#define	IEN_TPU1_TGI1A		IEN2
#define	IEN_TPU1_TGI1B		IEN3
#define	IEN_TPU2_TGI2A		IEN4
#define	IEN_TPU2_TGI2B		IEN5
#define	IEN_TPU3_TGI3A		IEN6
#define	IEN_TPU3_TGI3B		IEN7
#define	IEN_TPU3_TGI3C		IEN0
#define	IEN_TPU3_TGI3D		IEN1
#define	IEN_TPU4_TGI4A		IEN2
#define	IEN_TPU4_TGI4B		IEN3
#define	IEN_TPU5_TGI5A		IEN4
#define	IEN_TPU5_TGI5B		IEN5
#define	IEN_TPU6_TGI6A		IEN6
#define	IEN_TPU6_TGI6B		IEN7
#define	IEN_TPU6_TGI6C		IEN0
#define	IEN_TPU6_TGI6D		IEN1
#define	IEN_MTU0_TGIA0		IEN6
#define	IEN_MTU0_TGIB0		IEN7
#define	IEN_MTU0_TGIC0		IEN0
#define	IEN_MTU0_TGID0		IEN1
#define	IEN_MTU0_TGIE0		IEN2
#define	IEN_MTU0_TGIF0		IEN3
#define	IEN_TPU7_TGI7A		IEN4
#define	IEN_TPU7_TGI7B		IEN5
#define	IEN_MTU1_TGIA1		IEN4
#define	IEN_MTU1_TGIB1		IEN5
#define	IEN_TPU8_TGI8A		IEN6
#define	IEN_TPU8_TGI8B		IEN7
#define	IEN_MTU2_TGIA2		IEN6
#define	IEN_MTU2_TGIB2		IEN7
#define	IEN_TPU9_TGI9A		IEN0
#define	IEN_TPU9_TGI9B		IEN1
#define	IEN_TPU9_TGI9C		IEN2
#define	IEN_TPU9_TGI9D		IEN3
#define	IEN_MTU3_TGIA3		IEN0
#define	IEN_MTU3_TGIB3		IEN1
#define	IEN_MTU3_TGIC3		IEN2
#define	IEN_MTU3_TGID3		IEN3
#define	IEN_TPU10_TGI10A	IEN4
#define	IEN_TPU10_TGI10B	IEN5
#define	IEN_MTU4_TGIA4		IEN4
#define	IEN_MTU4_TGIB4		IEN5
#define	IEN_MTU4_TGIC4		IEN6
#define	IEN_MTU4_TGID4		IEN7
#define	IEN_MTU4_TCIV4		IEN0
#define	IEN_TPU11_TGI11A	IEN4
#define	IEN_TPU11_TGI11B	IEN5
#define	IEN_MTU5_TGIU5		IEN1
#define	IEN_MTU5_TGIV5		IEN2
#define	IEN_MTU5_TGIW5		IEN3
#define	IEN_POE_OEI1		IEN6
#define	IEN_POE_OEI2		IEN7
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
#define	IEN_RIIC0_EEI0		IEN6
#define	IEN_RIIC0_RXI0		IEN7
#define	IEN_RIIC0_TXI0		IEN0
#define	IEN_RIIC0_TEI0		IEN1
#define	IEN_RIIC1_EEI1		IEN2
#define	IEN_RIIC1_RXI1		IEN3
#define	IEN_RIIC1_TXI1		IEN4
#define	IEN_RIIC1_TEI1		IEN5
#define	IEN_RIIC2_EEI2		IEN6
#define	IEN_RIIC2_RXI2		IEN7
#define	IEN_RIIC2_TXI2		IEN0
#define	IEN_RIIC2_TEI2		IEN1
#define	IEN_RIIC3_EEI3		IEN2
#define	IEN_RIIC3_RXI3		IEN3
#define	IEN_RIIC3_TXI3		IEN4
#define	IEN_RIIC3_TEI3		IEN5
#define	IEN_DMAC_DMAC0I		IEN6
#define	IEN_DMAC_DMAC1I		IEN7
#define	IEN_DMAC_DMAC2I		IEN0
#define	IEN_DMAC_DMAC3I		IEN1
#define	IEN_EXDMAC_EXDMAC0I	IEN2
#define	IEN_EXDMAC_EXDMAC1I	IEN3
#define	IEN_DEU_DEU0		IEN6
#define	IEN_DEU_DEU1		IEN7
#define	IEN_PDC_PCDFI		IEN0
#define	IEN_PDC_PCFEI		IEN1
#define	IEN_PDC_PCERI		IEN2
#define	IEN_SCI0_RXI0		IEN6
#define	IEN_SCI0_TXI0		IEN7
#define	IEN_SCI0_TEI0		IEN0
#define	IEN_SCI1_RXI1		IEN1
#define	IEN_SCI1_TXI1		IEN2
#define	IEN_SCI1_TEI1		IEN3
#define	IEN_SCI2_RXI2		IEN4
#define	IEN_SCI2_TXI2		IEN5
#define	IEN_SCI2_TEI2		IEN6
#define	IEN_SCI3_RXI3		IEN7
#define	IEN_SCI3_TXI3		IEN0
#define	IEN_SCI3_TEI3		IEN1
#define	IEN_SCI4_RXI4		IEN2
#define	IEN_SCI4_TXI4		IEN3
#define	IEN_SCI4_TEI4		IEN4
#define	IEN_SCI5_RXI5		IEN5
#define	IEN_SCI5_TXI5		IEN6
#define	IEN_SCI5_TEI5		IEN7
#define	IEN_SCI6_RXI6		IEN0
#define	IEN_SCI6_TXI6		IEN1
#define	IEN_SCI6_TEI6		IEN2
#define	IEN_SCI7_RXI7		IEN3
#define	IEN_SCI7_TXI7		IEN4
#define	IEN_SCI7_TEI7		IEN5
#define	IEN_SCI8_RXI8		IEN6
#define	IEN_SCI8_TXI8		IEN7
#define	IEN_SCI8_TEI8		IEN0
#define	IEN_SCI9_RXI9		IEN1
#define	IEN_SCI9_TXI9		IEN2
#define	IEN_SCI9_TEI9		IEN3
#define	IEN_SCI10_RXI10		IEN4
#define	IEN_SCI10_TXI10		IEN5
#define	IEN_SCI10_TEI10		IEN6
#define	IEN_SCI11_RXI11		IEN7
#define	IEN_SCI11_TXI11		IEN0
#define	IEN_SCI11_TEI11		IEN1
#define	IEN_SCI12_RXI12		IEN2
#define	IEN_SCI12_TXI12		IEN3
#define	IEN_SCI12_TEI12		IEN4
#define	IEN_IEB_IEBINT		IEN5

#define	VECT_BSC_BUSERR		16
#define	VECT_FCU_FIFERR		21
#define	VECT_FCU_FRDYI		23
#define	VECT_ICU_SWINT		27
#define	VECT_CMT0_CMI0		28
#define	VECT_CMT1_CMI1		29
#define	VECT_CMT2_CMI2		30
#define	VECT_CMT3_CMI3		31
#define	VECT_ETHER_EINT		32
#define	VECT_USB0_D0FIFO0	33
#define	VECT_USB0_D1FIFO0	34
#define	VECT_USB0_USBI0		35
#define	VECT_USB1_D0FIFO1	36
#define	VECT_USB1_D1FIFO1	37
#define	VECT_USB1_USBI1		38
#define	VECT_RSPI0_SPRI0	39
#define	VECT_RSPI0_SPTI0	40
#define	VECT_RSPI0_SPII0	41
#define	VECT_RSPI1_SPRI1	42
#define	VECT_RSPI1_SPTI1	43
#define	VECT_RSPI1_SPII1	44
#define	VECT_RSPI2_SPRI2	45
#define	VECT_RSPI2_SPTI2	46
#define	VECT_RSPI2_SPII2	47
#define	VECT_CAN0_RXF0		48
#define	VECT_CAN0_TXF0		49
#define	VECT_CAN0_RXM0		50
#define	VECT_CAN0_TXM0		51
#define	VECT_CAN1_RXF1		52
#define	VECT_CAN1_TXF1		53
#define	VECT_CAN1_RXM1		54
#define	VECT_CAN1_TXM1		55
#define	VECT_CAN2_RXF2		56
#define	VECT_CAN2_TXF2		57
#define	VECT_CAN2_RXM2		58
#define	VECT_CAN2_TXM2		59
#define	VECT_RTC_CUP		62
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
#define	VECT_USB_USBR0		90
#define	VECT_USB_USBR1		91
#define	VECT_RTC_ALM		92
#define	VECT_RTC_PRD		93
#define	VECT_AD_ADI0		98
#define	VECT_S12AD_S12ADI0	102
#define	VECT_ICU_GROUP0		106
#define	VECT_ICU_GROUP1		107
#define	VECT_ICU_GROUP2		108
#define	VECT_ICU_GROUP3		109
#define	VECT_ICU_GROUP4		110
#define	VECT_ICU_GROUP5		111
#define	VECT_ICU_GROUP6		112
#define	VECT_ICU_GROUP12	114
#define	VECT_SCI12_SCIX0	122
#define	VECT_SCI12_SCIX1	123
#define	VECT_SCI12_SCIX2	124
#define	VECT_SCI12_SCIX3	125
#define	VECT_TPU0_TGI0A		126
#define	VECT_TPU0_TGI0B		127
#define	VECT_TPU0_TGI0C		128
#define	VECT_TPU0_TGI0D		129
#define	VECT_TPU1_TGI1A		130
#define	VECT_TPU1_TGI1B		131
#define	VECT_TPU2_TGI2A		132
#define	VECT_TPU2_TGI2B		133
#define	VECT_TPU3_TGI3A		134
#define	VECT_TPU3_TGI3B		135
#define	VECT_TPU3_TGI3C		136
#define	VECT_TPU3_TGI3D		137
#define	VECT_TPU4_TGI4A		138
#define	VECT_TPU4_TGI4B		139
#define	VECT_TPU5_TGI5A		140
#define	VECT_TPU5_TGI5B		141
#define	VECT_TPU6_TGI6A		142
#define	VECT_TPU6_TGI6B		143
#define	VECT_TPU6_TGI6C		144
#define	VECT_TPU6_TGI6D		145
#define	VECT_MTU0_TGIA0		142
#define	VECT_MTU0_TGIB0		143
#define	VECT_MTU0_TGIC0		144
#define	VECT_MTU0_TGID0		145
#define	VECT_MTU0_TGIE0		146
#define	VECT_MTU0_TGIF0		147
#define	VECT_TPU7_TGI7A		148
#define	VECT_TPU7_TGI7B		149
#define	VECT_MTU1_TGIA1		148
#define	VECT_MTU1_TGIB1		149
#define	VECT_TPU8_TGI8A		150
#define	VECT_TPU8_TGI8B		151
#define	VECT_MTU2_TGIA2		150
#define	VECT_MTU2_TGIB2		151
#define	VECT_TPU9_TGI9A		152
#define	VECT_TPU9_TGI9B		153
#define	VECT_TPU9_TGI9C		154
#define	VECT_TPU9_TGI9D		155
#define	VECT_MTU3_TGIA3		152
#define	VECT_MTU3_TGIB3		153
#define	VECT_MTU3_TGIC3		154
#define	VECT_MTU3_TGID3		155
#define	VECT_TPU10_TGI10A	156
#define	VECT_TPU10_TGI10B	157
#define	VECT_MTU4_TGIA4		156
#define	VECT_MTU4_TGIB4		157
#define	VECT_MTU4_TGIC4		158
#define	VECT_MTU4_TGID4		159
#define	VECT_MTU4_TCIV4		160
#define	VECT_TPU11_TGI11A	164
#define	VECT_TPU11_TGI11B	165
#define	VECT_MTU5_TGIU5		161
#define	VECT_MTU5_TGIV5		162
#define	VECT_MTU5_TGIW5		163
#define	VECT_POE_OEI1		166
#define	VECT_POE_OEI2		167
#define	VECT_TMR0_CMIA0		170
#define	VECT_TMR0_CMIB0		171
#define	VECT_TMR0_OVI0		172
#define	VECT_TMR1_CMIA1		173
#define	VECT_TMR1_CMIB1		174
#define	VECT_TMR1_OVI1		175
#define	VECT_TMR2_CMIA2		176
#define	VECT_TMR2_CMIB2		177
#define	VECT_TMR2_OVI2		178
#define	VECT_TMR3_CMIA3		179
#define	VECT_TMR3_CMIB3		180
#define	VECT_TMR3_OVI3		181
#define	VECT_RIIC0_EEI0		182
#define	VECT_RIIC0_RXI0		183
#define	VECT_RIIC0_TXI0		184
#define	VECT_RIIC0_TEI0		185
#define	VECT_RIIC1_EEI1		186
#define	VECT_RIIC1_RXI1		187
#define	VECT_RIIC1_TXI1		188
#define	VECT_RIIC1_TEI1		189
#define	VECT_RIIC2_EEI2		190
#define	VECT_RIIC2_RXI2		191
#define	VECT_RIIC2_TXI2		192
#define	VECT_RIIC2_TEI2		193
#define	VECT_RIIC3_EEI3		194
#define	VECT_RIIC3_RXI3		195
#define	VECT_RIIC3_TXI3		196
#define	VECT_RIIC3_TEI3		197
#define	VECT_DMAC_DMAC0I	198
#define	VECT_DMAC_DMAC1I	199
#define	VECT_DMAC_DMAC2I	200
#define	VECT_DMAC_DMAC3I	201
#define	VECT_EXDMAC_EXDMAC0I	202
#define	VECT_EXDMAC_EXDMAC1I	203
#define	VECT_DEU_DEU0		206
#define	VECT_DEU_DEU1		207
#define	VECT_PDC_PCDFI		208
#define	VECT_PDC_PCFEI		209
#define	VECT_PDC_PCERI		210
#define	VECT_SCI0_RXI0		214
#define	VECT_SCI0_TXI0		215
#define	VECT_SCI0_TEI0		216
#define	VECT_SCI1_RXI1		217
#define	VECT_SCI1_TXI1		218
#define	VECT_SCI1_TEI1		219
#define	VECT_SCI2_RXI2		220
#define	VECT_SCI2_TXI2		221
#define	VECT_SCI2_TEI2		222
#define	VECT_SCI3_RXI3		223
#define	VECT_SCI3_TXI3		224
#define	VECT_SCI3_TEI3		225
#define	VECT_SCI4_RXI4		226
#define	VECT_SCI4_TXI4		227
#define	VECT_SCI4_TEI4		228
#define	VECT_SCI5_RXI5		229
#define	VECT_SCI5_TXI5		230
#define	VECT_SCI5_TEI5		231
#define	VECT_SCI6_RXI6		232
#define	VECT_SCI6_TXI6		233
#define	VECT_SCI6_TEI6		234
#define	VECT_SCI7_RXI7		235
#define	VECT_SCI7_TXI7		236
#define	VECT_SCI7_TEI7		237
#define	VECT_SCI8_RXI8		238
#define	VECT_SCI8_TXI8		239
#define	VECT_SCI8_TEI8		240
#define	VECT_SCI9_RXI9		241
#define	VECT_SCI9_TXI9		242
#define	VECT_SCI9_TEI9		243
#define	VECT_SCI10_RXI10	244
#define	VECT_SCI10_TXI10	245
#define	VECT_SCI10_TEI10	246
#define	VECT_SCI11_RXI11	247
#define	VECT_SCI11_TXI11	248
#define	VECT_SCI11_TEI11	249
#define	VECT_SCI12_RXI12	250
#define	VECT_SCI12_TXI12	251
#define	VECT_SCI12_TEI12	252
#define	VECT_IEB_IEBINT		253

#define	MSTP_EXDMAC	SYSTEM.MSTPCRA.BIT.MSTPA29
#define	MSTP_EXDMAC0	SYSTEM.MSTPCRA.BIT.MSTPA29
#define	MSTP_EXDMAC1	SYSTEM.MSTPCRA.BIT.MSTPA29
#define	MSTP_DMAC	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC0	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC1	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC2	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DMAC3	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_DTC	SYSTEM.MSTPCRA.BIT.MSTPA28
#define	MSTP_AD		SYSTEM.MSTPCRA.BIT.MSTPA23
#define	MSTP_DA		SYSTEM.MSTPCRA.BIT.MSTPA19
#define	MSTP_S12AD	SYSTEM.MSTPCRA.BIT.MSTPA17
#define	MSTP_CMT0	SYSTEM.MSTPCRA.BIT.MSTPA15
#define	MSTP_CMT1	SYSTEM.MSTPCRA.BIT.MSTPA15
#define	MSTP_CMT2	SYSTEM.MSTPCRA.BIT.MSTPA14
#define	MSTP_CMT3	SYSTEM.MSTPCRA.BIT.MSTPA14
#define	MSTP_TPU0	SYSTEM.MSTPCRA.BIT.MSTPA13
#define	MSTP_TPU1	SYSTEM.MSTPCRA.BIT.MSTPA13
#define	MSTP_TPU2	SYSTEM.MSTPCRA.BIT.MSTPA13
#define	MSTP_TPU3	SYSTEM.MSTPCRA.BIT.MSTPA13
#define	MSTP_TPU4	SYSTEM.MSTPCRA.BIT.MSTPA13
#define	MSTP_TPU5	SYSTEM.MSTPCRA.BIT.MSTPA13
#define	MSTP_TPU6	SYSTEM.MSTPCRA.BIT.MSTPA12
#define	MSTP_TPU7	SYSTEM.MSTPCRA.BIT.MSTPA12
#define	MSTP_TPU8	SYSTEM.MSTPCRA.BIT.MSTPA12
#define	MSTP_TPU9	SYSTEM.MSTPCRA.BIT.MSTPA12
#define	MSTP_TPU10	SYSTEM.MSTPCRA.BIT.MSTPA12
#define	MSTP_TPU11	SYSTEM.MSTPCRA.BIT.MSTPA12
#define	MSTP_PPG0	SYSTEM.MSTPCRA.BIT.MSTPA11
#define	MSTP_PPG1	SYSTEM.MSTPCRA.BIT.MSTPA10
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
#define	MSTP_SCI2	SYSTEM.MSTPCRB.BIT.MSTPB29
#define	MSTP_SMCI2	SYSTEM.MSTPCRB.BIT.MSTPB29
#define	MSTP_SCI3	SYSTEM.MSTPCRB.BIT.MSTPB28
#define	MSTP_SMCI3	SYSTEM.MSTPCRB.BIT.MSTPB28
#define	MSTP_SCI4	SYSTEM.MSTPCRB.BIT.MSTPB27
#define	MSTP_SMCI4	SYSTEM.MSTPCRB.BIT.MSTPB27
#define	MSTP_SCI5	SYSTEM.MSTPCRB.BIT.MSTPB26
#define	MSTP_SMCI5	SYSTEM.MSTPCRB.BIT.MSTPB26
#define	MSTP_SCI6	SYSTEM.MSTPCRB.BIT.MSTPB25
#define	MSTP_SMCI6	SYSTEM.MSTPCRB.BIT.MSTPB25
#define	MSTP_SCI7	SYSTEM.MSTPCRB.BIT.MSTPB24
#define	MSTP_SMCI7	SYSTEM.MSTPCRB.BIT.MSTPB24
#define	MSTP_CRC	SYSTEM.MSTPCRB.BIT.MSTPB23
#define	MSTP_PDC	SYSTEM.MSTPCRB.BIT.MSTPB22
#define	MSTP_RIIC0	SYSTEM.MSTPCRB.BIT.MSTPB21
#define	MSTP_RIIC1	SYSTEM.MSTPCRB.BIT.MSTPB20
#define	MSTP_USB0	SYSTEM.MSTPCRB.BIT.MSTPB19
#define	MSTP_USB1	SYSTEM.MSTPCRB.BIT.MSTPB18
#define	MSTP_RSPI0	SYSTEM.MSTPCRB.BIT.MSTPB17
#define	MSTP_RSPI1	SYSTEM.MSTPCRB.BIT.MSTPB16
#define	MSTP_EDMAC	SYSTEM.MSTPCRB.BIT.MSTPB15
#define	MSTP_TEMPS	SYSTEM.MSTPCRB.BIT.MSTPB8
#define	MSTP_SCI12	SYSTEM.MSTPCRB.BIT.MSTPB4
#define	MSTP_SMCI12	SYSTEM.MSTPCRB.BIT.MSTPB4
#define	MSTP_CAN2	SYSTEM.MSTPCRB.BIT.MSTPB2
#define	MSTP_CAN1	SYSTEM.MSTPCRB.BIT.MSTPB1
#define	MSTP_CAN0	SYSTEM.MSTPCRB.BIT.MSTPB0
#define	MSTP_SCI8	SYSTEM.MSTPCRC.BIT.MSTPC27
#define	MSTP_SMCI8	SYSTEM.MSTPCRC.BIT.MSTPC27
#define	MSTP_SCI9	SYSTEM.MSTPCRC.BIT.MSTPC26
#define	MSTP_SMCI9	SYSTEM.MSTPCRC.BIT.MSTPC26
#define	MSTP_SCI10	SYSTEM.MSTPCRC.BIT.MSTPC25
#define	MSTP_SMCI10	SYSTEM.MSTPCRC.BIT.MSTPC25
#define	MSTP_SCI11	SYSTEM.MSTPCRC.BIT.MSTPC24
#define	MSTP_SMCI11	SYSTEM.MSTPCRC.BIT.MSTPC24
#define	MSTP_RSPI2	SYSTEM.MSTPCRC.BIT.MSTPC22
#define	MSTP_MCK	SYSTEM.MSTPCRC.BIT.MSTPC19
#define	MSTP_IEB	SYSTEM.MSTPCRC.BIT.MSTPC18
#define	MSTP_RIIC2	SYSTEM.MSTPCRC.BIT.MSTPC17
#define	MSTP_RIIC3	SYSTEM.MSTPCRC.BIT.MSTPC16
#define	MSTP_RAM1	SYSTEM.MSTPCRC.BIT.MSTPC1
#define	MSTP_RAM0	SYSTEM.MSTPCRC.BIT.MSTPC0
#define	MSTP_DEU	SYSTEM.MSTPCRD.BIT.MSTPD31

#define	IS_CAN0_ERS0		IS0
#define	IS_CAN1_ERS1		IS1
#define	IS_CAN2_ERS2		IS2
#define	IS_MTU0_TCIV0		IS0
#define	IS_MTU1_TCIV1		IS1
#define	IS_MTU1_TCIU1		IS2
#define	IS_MTU2_TCIV2		IS0
#define	IS_MTU2_TCIU2		IS1
#define	IS_MTU3_TCIV3		IS2
#define	IS_TPU0_TCI0V		IS0
#define	IS_TPU1_TCI1V		IS1
#define	IS_TPU1_TCI1U		IS2
#define	IS_TPU5_TCI5V		IS3
#define	IS_TPU5_TCI5U		IS4
#define	IS_TPU2_TCI2V		IS0
#define	IS_TPU2_TCI2U		IS1
#define	IS_TPU3_TCI3V		IS2
#define	IS_TPU4_TCI4V		IS3
#define	IS_TPU4_TCI4U		IS4
#define	IS_TPU6_TCI6V		IS0
#define	IS_TPU7_TCI7V		IS1
#define	IS_TPU7_TCI7U		IS2
#define	IS_TPU11_TCI11V		IS3
#define	IS_TPU11_TCI11U		IS4
#define	IS_TPU8_TCI8V		IS0
#define	IS_TPU8_TCI8U		IS1
#define	IS_TPU9_TCI9V		IS2
#define	IS_TPU10_TCI10V		IS3
#define	IS_TPU10_TCI10U		IS4
#define	IS_SCI0_ERI0		IS0
#define	IS_SCI1_ERI1		IS1
#define	IS_SCI2_ERI2		IS2
#define	IS_SCI3_ERI3		IS3
#define	IS_SCI4_ERI4		IS4
#define	IS_SCI5_ERI5		IS5
#define	IS_SCI6_ERI6		IS6
#define	IS_SCI7_ERI7		IS7
#define	IS_SCI8_ERI8		IS8
#define	IS_SCI9_ERI9		IS9
#define	IS_SCI10_ERI10		IS10
#define	IS_SCI11_ERI11		IS11
#define	IS_SCI12_ERI12		IS12
#define	IS_RSPI0_SPEI0		IS13
#define	IS_RSPI1_SPEI1		IS14
#define	IS_RSPI2_SPEI2		IS15


#define	EN_CAN0_ERS0		EN0
#define	EN_CAN1_ERS1		EN1
#define	EN_CAN2_ERS2		EN2
#define	EN_MTU0_TCIV0		EN0
#define	EN_MTU1_TCIV1		EN1
#define	EN_MTU1_TCIU1		EN2
#define	EN_MTU2_TCIV2		EN0
#define	EN_MTU2_TCIU2		EN1
#define	EN_MTU3_TCIV3		EN2
#define	EN_TPU0_TCI0V		EN0
#define	EN_TPU1_TCI1V		EN1
#define	EN_TPU1_TCI1U		EN2
#define	EN_TPU5_TCI5V		EN3
#define	EN_TPU5_TCI5U		EN4
#define	EN_TPU2_TCI2V		EN0
#define	EN_TPU2_TCI2U		EN1
#define	EN_TPU3_TCI3V		EN2
#define	EN_TPU4_TCI4V		EN3
#define	EN_TPU4_TCI4U		EN4
#define	EN_TPU6_TCI6V		EN0
#define	EN_TPU7_TCI7V		EN1
#define	EN_TPU7_TCI7U		EN2
#define	EN_TPU11_TCI11V		EN3
#define	EN_TPU11_TCI11U		EN4
#define	EN_TPU8_TCI8V		EN0
#define	EN_TPU8_TCI8U		EN1
#define	EN_TPU9_TCI9V		EN2
#define	EN_TPU10_TCI10V		EN3
#define	EN_TPU10_TCI10U		EN4
#define	EN_SCI0_ERI0		EN0
#define	EN_SCI1_ERI1		EN1
#define	EN_SCI2_ERI2		EN2
#define	EN_SCI3_ERI3		EN3
#define	EN_SCI4_ERI4		EN4
#define	EN_SCI5_ERI5		EN5
#define	EN_SCI6_ERI6		EN6
#define	EN_SCI7_ERI7		EN7
#define	EN_SCI8_ERI8		EN8
#define	EN_SCI9_ERI9		EN9
#define	EN_SCI10_ERI10		EN10
#define	EN_SCI11_ERI11		EN11
#define	EN_SCI12_ERI12		EN12
#define	EN_RSPI0_SPEI0		EN13
#define	EN_RSPI1_SPEI1		EN14
#define	EN_RSPI2_SPEI2		EN15

#define	CLR_CAN0_ERS0		CLR0
#define	CLR_CAN1_ERS1		CLR1
#define	CLR_CAN2_ERS2		CLR2
#define	CLR_MTU0_TCIV0		CLR0
#define	CLR_MTU1_TCIV1		CLR1
#define	CLR_MTU1_TCIU1		CLR2
#define	CLR_MTU2_TCIV2		CLR0
#define	CLR_MTU2_TCIU2		CLR1
#define	CLR_MTU3_TCIV3		CLR2
#define	CLR_TPU0_TCI0V		CLR0
#define	CLR_TPU1_TCI1V		CLR1
#define	CLR_TPU1_TCI1U		CLR2
#define	CLR_TPU5_TCI5V		CLR3
#define	CLR_TPU5_TCI5U		CLR4
#define	CLR_TPU2_TCI2V		CLR0
#define	CLR_TPU2_TCI2U		CLR1
#define	CLR_TPU3_TCI3V		CLR2
#define	CLR_TPU4_TCI4V		CLR3
#define	CLR_TPU4_TCI4U		CLR4
#define	CLR_TPU6_TCI6V		CLR0
#define	CLR_TPU7_TCI7V		CLR1
#define	CLR_TPU7_TCI7U		CLR2
#define	CLR_TPU11_TCI11V	CLR3
#define	CLR_TPU11_TCI11U	CLR4
#define	CLR_TPU8_TCI8V		CLR0
#define	CLR_TPU8_TCI8U		CLR1
#define	CLR_TPU9_TCI9V		CLR2
#define	CLR_TPU10_TCI10V	CLR3
#define	CLR_TPU10_TCI10U	CLR4

#define	CN_TPU6_TGI6A		CN0
#define	CN_TPU6_TGI6B		CN0
#define	CN_TPU6_TGI6C		CN0
#define	CN_TPU6_TGI6D		CN0
#define	CN_MTU0_TGIA0		CN0
#define	CN_MTU0_TGIB0		CN0
#define	CN_MTU0_TGIC0		CN0
#define	CN_MTU0_TGID0		CN0
#define	CN_MTU0_TGIE0		CN0
#define	CN_MTU0_TGIF0		CN0
#define	CN_TPU7_TGI7A		CN1
#define	CN_TPU7_TGI7B		CN1
#define	CN_MTU1_TGIA1		CN1
#define	CN_MTU1_TGIB1		CN1
#define	CN_TPU8_TGI8A		CN2
#define	CN_TPU8_TGI8B		CN2
#define	CN_MTU2_TGIA2		CN2
#define	CN_MTU2_TGIB2		CN2
#define	CN_TPU9_TGI9A		CN3
#define	CN_TPU9_TGI9B		CN3
#define	CN_TPU9_TGI9C		CN3
#define	CN_TPU9_TGI9D		CN3
#define	CN_MTU3_TGIA3		CN3
#define	CN_MTU3_TGIB3		CN3
#define	CN_MTU3_TGIC3		CN3
#define	CN_MTU3_TGID3		CN3
#define	CN_TPU10_TGI10A		CN4
#define	CN_TPU10_TGI10B		CN4
#define	CN_MTU4_TGIA4		CN4
#define	CN_MTU4_TGIB4		CN4
#define	CN_MTU4_TGIC4		CN4
#define	CN_MTU4_TGID4		CN4
#define	CN_MTU4_TCIV4		CN4
#define	CN_TPU11_TGI11A		CN5
#define	CN_TPU11_TGI11B		CN5
#define	CN_MTU5_TGIU5		CN5
#define	CN_MTU5_TGIV5		CN5
#define	CN_MTU5_TGIW5		CN5
#define	CN_TPU6_			CN0
#define	CN_MTU0_			CN0
#define	CN_TPU7_			CN1
#define	CN_MTU1_			CN1
#define	CN_TPU8_			CN2
#define	CN_MTU2_			CN2
#define	CN_TPU9_			CN3
#define	CN_MTU3_			CN3
#define	CN_TPU10_			CN4
#define	CN_MTU4_			CN4
#define	CN_TPU11_			CN5
#define	CN_MTU5_			CN5

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

#define	__IS( x )		ICU.GRP[ GRP ## x ].BIT.IS ## x
#define	 _IS( x )		__IS( x )
#define	  IS( x , y )	_IS( _ ## x ## _ ## y )
#define	__EN( x )		ICU.GEN[ GEN ## x ].BIT.EN ## x
#define	 _EN( x )		__EN( x )
#define	  EN( x , y )	_EN( _ ## x ## _ ## y )
#define	__CLR( x )		ICU.GCR[ GCR ## x ].BIT.CLR ## x
#define	 _CLR( x )		__CLR( x )
#define	  CLR( x , y )	_CLR( _ ## x ## _ ## y )
#define	__CN( x )		ICU.SEL.BIT.CN ## x
#define	 _CN( x )		__CN( x )
#define	  CN( x , y )	_CN( _ ## x ## _ ## y )

#define	AD		(*(volatile struct st_ad       *)0x89800)
#define	BSC		(*(volatile struct st_bsc      *)0x81300)
#define	CAN0	(*(volatile struct st_can      *)0x90200)
#define	CAN1	(*(volatile struct st_can      *)0x91200)
#define	CAN2	(*(volatile struct st_can      *)0x92200)
#define	CMT		(*(volatile struct st_cmt      *)0x88000)
#define	CMT0	(*(volatile struct st_cmt0     *)0x88002)
#define	CMT1	(*(volatile struct st_cmt0     *)0x88008)
#define	CMT2	(*(volatile struct st_cmt0     *)0x88012)
#define	CMT3	(*(volatile struct st_cmt0     *)0x88018)
#define	CRC		(*(volatile struct st_crc      *)0x88280)
#define	DA		(*(volatile struct st_da       *)0x880C0)
#define	DMAC	(*(volatile struct st_dmac     *)0x82200)
#define	DMAC0	(*(volatile struct st_dmac0    *)0x82000)
#define	DMAC1	(*(volatile struct st_dmac1    *)0x82040)
#define	DMAC2	(*(volatile struct st_dmac1    *)0x82080)
#define	DMAC3	(*(volatile struct st_dmac1    *)0x820C0)
#define	DTC		(*(volatile struct st_dtc      *)0x82400)
#define	EDMAC	(*(volatile struct st_edmac    *)0xC0000)
#define	ETHERC	(*(volatile struct st_etherc   *)0xC0100)
#define	EXDMAC	(*(volatile struct st_exdmac   *)0x82A00)
#define	EXDMAC0	(*(volatile struct st_exdmac0  *)0x82800)
#define	EXDMAC1	(*(volatile struct st_exdmac1  *)0x82840)
#define	FLASH	(*(volatile struct st_flash    *)0x8C296)
#define	ICU		(*(volatile struct st_icu      *)0x87000)
#define	IEB		(*(volatile struct st_ieb      *)0x8A800)
#define	IWDT	(*(volatile struct st_iwdt     *)0x88030)
#define	MPC		(*(volatile struct st_mpc      *)0x8C100)
#define	MPU		(*(volatile struct st_mpu      *)0x86400)
#define	MTU		(*(volatile struct st_mtu      *)0x8860A)
#define	MTU0	(*(volatile struct st_mtu0     *)0x88690)
#define	MTU1	(*(volatile struct st_mtu1     *)0x88690)
#define	MTU2	(*(volatile struct st_mtu2     *)0x88692)
#define	MTU3	(*(volatile struct st_mtu3     *)0x88600)
#define	MTU4	(*(volatile struct st_mtu4     *)0x88600)
#define	MTU5	(*(volatile struct st_mtu5     *)0x88694)
#define	PDC		(*(volatile struct st_pdc      *)0xA0500)
#define	POE		(*(volatile struct st_poe      *)0x88900)
#define	PORT	(*(volatile struct st_port     *)0x8C120)
#define	PORT0	(*(volatile struct st_port0    *)0x8C000)
#define	PORT1	(*(volatile struct st_port1    *)0x8C001)
#define	PORT2	(*(volatile struct st_port2    *)0x8C002)
#define	PORT3	(*(volatile struct st_port3    *)0x8C003)
#define	PORT4	(*(volatile struct st_port4    *)0x8C004)
#define	PORT5	(*(volatile struct st_port5    *)0x8C005)
#define	PORT6	(*(volatile struct st_port6    *)0x8C006)
#define	PORT7	(*(volatile struct st_port7    *)0x8C007)
#define	PORT8	(*(volatile struct st_port8    *)0x8C008)
#define	PORT9	(*(volatile struct st_port9    *)0x8C009)
#define	PORTA	(*(volatile struct st_porta    *)0x8C00A)
#define	PORTB	(*(volatile struct st_portb    *)0x8C00B)
#define	PORTC	(*(volatile struct st_portc    *)0x8C00C)
#define	PORTD	(*(volatile struct st_portd    *)0x8C00D)
#define	PORTE	(*(volatile struct st_porte    *)0x8C00E)
#define	PORTF	(*(volatile struct st_portf    *)0x8C00F)
#define	PORTG	(*(volatile struct st_portg    *)0x8C010)
#define	PORTJ	(*(volatile struct st_portj    *)0x8C012)
#define	PPG0	(*(volatile struct st_ppg0     *)0x881E6)
#define	PPG1	(*(volatile struct st_ppg1     *)0x881F0)
#define	RIIC0	(*(volatile struct st_riic     *)0x88300)
#define	RIIC1	(*(volatile struct st_riic     *)0x88320)
#define	RIIC2	(*(volatile struct st_riic     *)0x88340)
#define	RIIC3	(*(volatile struct st_riic     *)0x88360)
#define	RSPI0	(*(volatile struct st_rspi     *)0x88380)
#define	RSPI1	(*(volatile struct st_rspi     *)0x883A0)
#define	RSPI2	(*(volatile struct st_rspi     *)0x883C0)
#define	RTC		(*(volatile struct st_rtc      *)0x8C400)
#define	S12AD	(*(volatile struct st_s12ad    *)0x89000)
#define	SCI0	(*(volatile struct st_sci0     *)0x8A000)
#define	SCI1	(*(volatile struct st_sci0     *)0x8A020)
#define	SCI2	(*(volatile struct st_sci0     *)0x8A040)
#define	SCI3	(*(volatile struct st_sci0     *)0x8A060)
#define	SCI4	(*(volatile struct st_sci0     *)0x8A080)
#define	SCI5	(*(volatile struct st_sci0     *)0x8A0A0)
#define	SCI6	(*(volatile struct st_sci0     *)0x8A0C0)
#define	SCI7	(*(volatile struct st_sci0     *)0x8A0E0)
#define	SCI8	(*(volatile struct st_sci0     *)0x8A100)
#define	SCI9	(*(volatile struct st_sci0     *)0x8A120)
#define	SCI10	(*(volatile struct st_sci0     *)0x8A140)
#define	SCI11	(*(volatile struct st_sci0     *)0x8A160)
#define	SCI12	(*(volatile struct st_sci12    *)0x8B300)
#define	SMCI0	(*(volatile struct st_smci0    *)0x8A000)
#define	SMCI1	(*(volatile struct st_smci0    *)0x8A020)
#define	SMCI2	(*(volatile struct st_smci0    *)0x8A040)
#define	SMCI3	(*(volatile struct st_smci0    *)0x8A060)
#define	SMCI4	(*(volatile struct st_smci0    *)0x8A080)
#define	SMCI5	(*(volatile struct st_smci0    *)0x8A0A0)
#define	SMCI6	(*(volatile struct st_smci0    *)0x8A0C0)
#define	SMCI7	(*(volatile struct st_smci0    *)0x8A0E0)
#define	SMCI8	(*(volatile struct st_smci0    *)0x8A100)
#define	SMCI9	(*(volatile struct st_smci0    *)0x8A120)
#define	SMCI10	(*(volatile struct st_smci0    *)0x8A140)
#define	SMCI11	(*(volatile struct st_smci0    *)0x8A160)
#define	SMCI12	(*(volatile struct st_smci0    *)0x8B300)
#define	SYSTEM	(*(volatile struct st_system   *)0x80000)
#define	TEMPS	(*(volatile struct st_temps    *)0x8C500)
#define	TMR0	(*(volatile struct st_tmr0     *)0x88200)
#define	TMR1	(*(volatile struct st_tmr1     *)0x88201)
#define	TMR2	(*(volatile struct st_tmr0     *)0x88210)
#define	TMR3	(*(volatile struct st_tmr1     *)0x88211)
#define	TMR01	(*(volatile struct st_tmr01    *)0x88204)
#define	TMR23	(*(volatile struct st_tmr01    *)0x88214)
#define	TPU0	(*(volatile struct st_tpu0     *)0x88108)
#define	TPU1	(*(volatile struct st_tpu1     *)0x88108)
#define	TPU2	(*(volatile struct st_tpu2     *)0x8810A)
#define	TPU3	(*(volatile struct st_tpu3     *)0x8810A)
#define	TPU4	(*(volatile struct st_tpu4     *)0x8810C)
#define	TPU5	(*(volatile struct st_tpu5     *)0x8810C)
#define	TPU6	(*(volatile struct st_tpu0     *)0x88178)
#define	TPU7	(*(volatile struct st_tpu1     *)0x88178)
#define	TPU8	(*(volatile struct st_tpu2     *)0x8817A)
#define	TPU9	(*(volatile struct st_tpu3     *)0x8817A)
#define	TPU10	(*(volatile struct st_tpu4     *)0x8817C)
#define	TPU11	(*(volatile struct st_tpu5     *)0x8817C)
#define	TPUA	(*(volatile struct st_tpua     *)0x88100)
#define	TPUB	(*(volatile struct st_tpub     *)0x88170)
#define	USB		(*(volatile struct st_usb      *)0xA0400)
#define	USB0	(*(volatile struct st_usb0     *)0xA0000)
#define	USB1	(*(volatile struct st_usb1     *)0xA0200)
#define	WDT		(*(volatile struct st_wdt      *)0x88020)
#define	FLASHCONST	(*(volatile struct st_flashconst      *)0xFEFFFAC0)
#define	TEMPSCONST	(*(volatile struct st_tempsconst      *)0xFEFFFAD2)

#pragma pack()
#endif

