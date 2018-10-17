/*********************************************************************************************************************
 * @file     syscalls.c
 * @brief    Newlib stubs
 * @version  V1.6
 * @date     20 Apr 2017
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2015-2017, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with
 * Infineon Technologies AG dave@infineon.com).
 *********************************************************************************************************************
 *
 * *************************** Change history ********************************
 * V0.1                  : Initial version
 * V0.2                  : Label updates
 * V1.0                  : Made _sbrk device agnostic
 * V1.1                  : C++ support
 * V1.2                  : Restored compatibility with old project files
 * V1.3    Jan 2014, PKB : Encapsulating everything in this file for use only with GCC
 * V1.4 11 Dec 2015, JFT : Fix heap overflow
 * V1.5 09 Mar 2016, JFT : Add dso_handle to support destructors call at exit 
 * V1.6 20 Apr 2017, JFT : Foward declaration of __sbrk to fix link time optimization (-flto) compilation errors
 * @endcond
 */

/*
 * This file contains stubs for standard C library functionality that must
 * typically be provided for the underlying platform.
 *
 */
#if defined ( __GNUC__ )

#include <stdint.h>
#include <errno.h>
#include <sys/types.h>

/* Forward prototypes.  */
caddr_t _sbrk(int nbytes) __attribute__((externally_visible));
void _init(void) __attribute__((externally_visible));

/* c++ destructor dynamic shared object needed if -fuse-cxa-atexit is used*/
void *__dso_handle __attribute__ ((weak));

// defined in linker script
extern caddr_t Heap_Bank1_Start;
extern caddr_t Heap_Bank1_End;
caddr_t _sbrk(int nbytes)
{
  static caddr_t heap_ptr = NULL;
  caddr_t base;

  if (heap_ptr == NULL) {
    heap_ptr = (caddr_t)&Heap_Bank1_Start;
  }

  base = heap_ptr;

  /* heap word alignment */
  nbytes = (nbytes + 3) & ~0x3U;
  if ((caddr_t)&Heap_Bank1_End > (heap_ptr + nbytes))
  {
    heap_ptr += nbytes;
    return (base);
  }
  else
  {
    /* Heap overflow */
    errno = ENOMEM;
    return ((caddr_t)-1);
  }
}

/* Init */
void _init(void)
{}

#endif /* __GNUC__ */
