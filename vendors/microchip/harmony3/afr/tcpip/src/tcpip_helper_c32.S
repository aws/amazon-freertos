/*******************************************************************************
  TCPIP Assembly Helpers for PIC32

  Summary:
    Library for Microchip TCP/IP Stack
    
  Description:
    - Optimized implementations for PIC32 helpers
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

#include <p32xxxx.h> 

.global   TCPIP_Helper_htonl
.global   TCPIP_Helper_htons
.global   TCPIP_Helper_htonll

.global   TCPIP_Helper_CalcIPChecksum


.set nomips16
.set noreorder 

.text

#; TCPIP_Helper_htonl : converts a long from host to network order
.ent TCPIP_Helper_htonl
TCPIP_Helper_htonl:
    wsbh    a0;
    jr      ra;
    rotr    v0, a0, 16;

.end TCPIP_Helper_htonl


#; TCPIP_Helper_htons : converts a short from host to network order
.ent TCPIP_Helper_htons
TCPIP_Helper_htons:
    wsbh    a0;
    jr      ra;
    or      v0, a0, $0;

.end TCPIP_Helper_htons

#; TCPIP_Helper_htonll : converts a long long from host to network order
.ent TCPIP_Helper_htonll
TCPIP_Helper_htonll:
    wsbh    a0;
    wsbh    a1;
    rotr    v0, a1, 16;
    jr      ra;
    rotr    v1, a0, 16;
.end TCPIP_Helper_htonll




#; TCPIP_Helper_CalcIPChecksum(uint8_t* ptr, uint16_t nBytes, uint16_t seed);
#; implements the IP checksum calculation
#; in: 
#;      ptr - pointer to buffer of bytes to calculate the checksum over
#;      nBytes - number of bytes in the buffer
#; out:
#;      the IP checksum
#;
#;  a0 - ptr
#;  a1 - byte counter
#;  a2 - seed, then word counter
#;  a3 - multiply factor - 1
#;  hi, lo - checksum
#;  v0 - swap flag; result
#;  t0 t7 - scratch
#;  s0 s7 - scratch

.ent TCPIP_Helper_CalcIPChecksum
TCPIP_Helper_CalcIPChecksum:

    addiu   t0, a1, -4;
    bgez    t0, _chksum_do;
    ori     a3, $0, 1;  # multiply factor; 
#; less than 4 byes to work with
    beq     a1, $0, _swap_done;
    or      v1, a2, $0;
    addiu   t0, a1, -1;
    bne     t0, $0, _less2;
    nop;
_less1:
    lbu     v1, 0(a0);
    b       _add_halves;
    addu    v1, v1, a2;
_less2:
    addiu   t0, t0, -1;
    bne     t0, $0, _less3;
    lbu     v1, 1(a0);
    lbu     t0, 0(a0);
    sll     v1, v1, 8;
    or      v1, v1, t0;
    b       _add_halves;
    addu    v1, v1, a2;
_less3:
    lbu     t1, 0(a0);
    lbu     t0, 2(a0);
    sll     v1, v1, 8;
    or      v1, v1, t1;
    addu    v1, v1, t0;
    addu    v1, v1, a2;
_add_halves:
    srl     t1, v1, 16; # hi half
    andi    t2, v1, 0xffff; # low half
    b       _swap_done;
    addu    v1, t1, t2;
#;    
_chksum_do:
    mthi    $0;
    mtlo    a2;    # clear the checksum
    andi    v0, a0, 0x1;    # swap flag
    andi    t1, a0, 0x3;    # offset mask
    beq     t1, $0, _done_unalign
    ori     t3, $0, 0x4;
#;  starting on unaligned boundary
    subu    t3, t3, t1;    # bytes to be subtracted
    or      t0, $0, $0;
    lwr     t0, 0(a0);      # unaligned data
    addiu   t1, t1, -1;
    beq     t1, $0, _u_do;
    ori     t2, $0, 8;      # shifts no: 8
    addiu   t1, t1, -1;
    beq     t1, $0, _u_do;
    addiu   t2, t2, 8;      # shifts no: 16
    addiu   t2, t2, 8;      # shifts no: 24
_u_do:
    sllv    t0, t0, t2;
    maddu   t0, a3;
    subu    a1, a1, t3;     # subtract bytes
    addu    a0, a0, t3;     # advance address
#;
_done_unalign:
#;  process 8 words chunk
    srl     v1, a1, 5;  # save 8 words # chunks in v1
    sll     a2, v1, 5;  # processed bytes
_loop32_8:
    beq     v1, $0, _loop32_8_done;
#; load 8 words chunk
    lw      t0, 0(a0);
    lw      t1, 4(a0);
    maddu   t0, a3;
    lw      t2, 8(a0);
    maddu   t1, a3;
    lw      t3, 12(a0);
    maddu   t2, a3;
    lw      t4, 16(a0);
    maddu   t3, a3;
    lw      t5, 20(a0);
    maddu   t4, a3;
    lw      t6, 24(a0);
    maddu   t5, a3;
    lw      t7, 28(a0);
    maddu   t6, a3;
    addiu   v1, v1, -1;
    maddu   t7, a3
    b       _loop32_8
    addu    a0, a0, 32
_loop32_8_done:
#;  process 4 words chunk
    subu    a1, a1, a2;     # remaining bytes to process
    srl     v1, a1, 4;      # save 4 words # chunks in v1
    sll     a2, v1, 4;      # processed bytes
_loop32_4:
    beq     v1, $0, _loop32_4_done;
#; load 4 words chunk
    lw      t0, 0(a0);
    lw      t1, 4(a0);
    maddu   t0, a3;
    lw      t2, 8(a0);
    maddu   t1, a3;
    lw      t3, 12(a0);
    maddu   t2, a3;
    addiu   v1, v1, -1;
    maddu   t3, a3;
    b       _loop32_4
    addu    a0, a0, 16;
_loop32_4_done:
#;  process 2 words chunk
    subu    a1, a1, a2;     # remaining bytes to process
    srl     v1, a1, 3;      # save 2 words #chunks in v1
    sll     a2, v1, 3;      # processed bytes
_loop32_2:
    beq     v1, $0, _loop32_2_done;
#; load 2 words chunk
    lw      t0, 0(a0);
    lw      t1, 4(a0);
    maddu   t0, a3;
    addiu   v1, v1, -1;
    maddu   t1, a3;
    b       _loop32_2;
    addu    a0, a0, 8;
_loop32_2_done:
#;  process 1 word chunk
    subu    a1, a1, a2;     # remaining bytes to process
    srl     v1, a1, 2;      # save 1 words #chunks in v1
    sll     a2, v1, 2;      # bytes processed
_loop32_1:
    beq     v1, $0, _loop32_1_done;
#; load 1 word chunk
    lw      t0, 0(a0);
    addu    a0, a0, 4;
    maddu   t0, a3;
_loop32_1_done:
    subu    a1, a1, a2;     # remaining bytes to process
    beq     a1, $0, _rem_done;
    andi    a2, a1, 0x1;
#;
    bne     a2, $0, _r13
    nop;
_r2:    # 2 bytes rem
    lwl     t0, 1(a0);
    b       _r_do;
    srl     t0, t0, 16
_r13:
    andi    a2, a1, 0x2;
    bne     a2, $0, _r3;
    nop;
_r1:    # 1 byte rem
    lwl     t0, 0(a0);
    b       _r_do;
    srl     t0, t0, 24
_r3:    # 3 bytes remaining
    lwl     t0, 2(a0);
    srl     t0, t0, 8
    #
_r_do:
    maddu   t0, a3;
_rem_done:  # compress hilo
    mfhi    t1;
    mflo    t0;
    addu    v1, t1, t0;
#; check overflow
    sltu    t3, v1, t0;
    beq     t3, $0, _r_done;
    nop;
    addiu   v1, v1, 1;
#;
_r_done: #;  add halves
    srl     t1, v1, 16; # hi half
    andi    t2, v1, 0xffff; # low half
    addu    v1, t1, t2;
#;  add halves
    srl     t1, v1, 16; # hi half
    andi    t2, v1, 0xffff; # low half
    addu    v1, t1, t2;

#;  check swap
    beq     v0, $0, _swap_done;
    nop;
    wsbh    v1, v1;

_swap_done:    
#;  done
    nor     v0, v1, $0;
    jr      ra;
    andi    v0, v0, 0xffff;
.end TCPIP_Helper_CalcIPChecksum





.set reorder

