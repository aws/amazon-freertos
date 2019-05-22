/*
 * Automatically generated register structure
 * 
 * Copyright 2013 Marvell International Ltd.  All rights reserved.
 * 
 * Date:     Fri Oct 25 01:34:02 AM PDT 2013
 * User:     chengwen
 * Hostname: sds-159s
 * OS:       Linux 2.6.18-238.12.1.el5
 * Midas:    V1.02 Thu Sep 13 12:12:00 PDT 2012
 * Usergen:  V1.02
 * Path:     /proj/klmcu0/wa/chengwen/klmcu0/top/sim/run
 * Cmdline:  midas -param internal=true -usergen /proj/cadsim/MIDAS/1.02/examples/usergen/cdef2/cdef2.tcl /proj/klmcu0/wa/chengwen/klmcu0/hard_ip/GAU/gau_dig/doc/bg.csv
 */

#ifndef _BG_REG_H
#define _BG_REG_H

struct bg_reg {
    /* 0x0: Control Register */
    union {
        struct {
            uint32_t PD            :  1;  /* [0]     r/w */
            uint32_t SEL_CLK_CHOP  :  2;  /* [2:1]   r/w */
            uint32_t CHOP_EN       :  1;  /* [3]     r/w */
            uint32_t RES_TRIM      :  4;  /* [7:4]   r/w */
            uint32_t RESERVED_31_8 : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL;

    /* 0x4: Status Register */
    union {
        struct {
            uint32_t RDY           :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

};

typedef volatile struct bg_reg bg_reg_t;

#ifdef BG_IMPL
BEGIN_REG_SECTION(bg_registers)
bg_reg_t BGREG;
END_REG_SECTION(bg_registers)
#else
extern bg_reg_t BGREG;
#endif

#endif /* _BG_REG_H */
