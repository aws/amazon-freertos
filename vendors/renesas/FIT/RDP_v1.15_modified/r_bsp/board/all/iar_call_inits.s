#ifdef __ICCRX__

#ifndef __ROPI__
#error __ROPI__ not defined
#endif

;------------------------------------------------------------------------------
; This entry point is used for the User Boot Mode only.
;------------------------------------------------------------------------------
        section .text:CODE:NOROOT
        extern  __iar_program_start
        public  _PowerON_Reset_PC
        code
_PowerON_Reset_PC:
        bra.a   __iar_program_start

;------------------------------------------------------------------------------
; Initialize sections.
;------------------------------------------------------------------------------
        section .text:CODE:NOROOT
        public  __INITSCT
        code
__INITSCT:
        bsr.a   ___iar_data_init2
        rts

;------------------------------------------------------------------------------
; Initialize global class objects.
;------------------------------------------------------------------------------
#if __ROPI__
        extern  __PID_TOP
#endif
        extern  SHT$$PREINIT_ARRAY$$Base
        extern  SHT$$INIT_ARRAY$$Limit
        extern  ___call_ctors
        public  __CALL_INIT
        code
__CALL_INIT:
#if __ROPI__
        add     #SHT$$PREINIT_ARRAY$$Base - __PID_TOP, r13, r1
        add     #SHT$$INIT_ARRAY$$Limit - __PID_TOP, r13, r2
#else
        mov.l   #SHT$$PREINIT_ARRAY$$Base, r1
        mov.l   #SHT$$INIT_ARRAY$$Limit, r2
#endif
        bsr.a   ___call_ctors
        rts
        end

#endif
