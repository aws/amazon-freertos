/*--------------------------------------------------------------- 
 * Copyright (c) 1999,2000,2001,2002,2003                              
 * The Board of Trustees of the University of Illinois            
 * All Rights Reserved.                                           
 *--------------------------------------------------------------- 
 * Permission is hereby granted, free of charge, to any person    
 * obtaining a copy of this software (Iperf) and associated       
 * documentation files (the "Software"), to deal in the Software  
 * without restriction, including without limitation the          
 * rights to use, copy, modify, merge, publish, distribute,        
 * sublicense, and/or sell copies of the Software, and to permit     
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions: 
 *
 *     
 * Redistributions of source code must retain the above 
 * copyright notice, this list of conditions and 
 * the following disclaimers. 
 *
 *     
 * Redistributions in binary form must reproduce the above 
 * copyright notice, this list of conditions and the following 
 * disclaimers in the documentation and/or other materials 
 * provided with the distribution. 
 * 
 *     
 * Neither the names of the University of Illinois, NCSA, 
 * nor the names of its contributors may be used to endorse 
 * or promote products derived from this Software without
 * specific prior written permission. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * ________________________________________________________________
 * National Laboratory for Applied Network Research 
 * National Center for Supercomputing Applications 
 * University of Illinois at Urbana-Champaign 
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________ 
 * Author: Mark Gates
 *
 * snprintf.c
 *
 * This is from
 * W. Richard Stevens, 'UNIX Network Programming', Vol 1, 2nd Edition,
 *   Prentice Hall, 1998.
 *
 *
 * Throughout the book I use snprintf() because it's safer than sprintf().
 * But as of the time of this writing, not all systems provide this
 * function.  The function below should only be built on those systems
 * that do not provide a real snprintf().
 * The function below just acts like sprintf(); it is not safe, but it
 * tries to detect overflow.
 * ________________________________________________________________ */

#if defined(HAVE_CONFIG_H) && !defined(INCLUDED_CONFIG_H_)
	/* NOTE: config.h doesn't have guard includes! */
    #define INCLUDED_CONFIG_H_
    #include "config.h"
#endif /* defined(HAVE_CONFIG_H) && !defined(INCLUDED_CONFIG_H_) */

#ifndef HAVE_SNPRINTF

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "snprintf.h"

#ifdef __cplusplus
extern "C"
#endif /* __cplusplus */
int snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list ap;
    int ret;

    /* See http://www.ijs.si/software/snprintf/ for portable
     * implementation of snprintf.
     */

    va_start(ap, fmt);
    ret = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    if (size > 0)
        buf[size - 1] = '\0';
    return ret;
} // end snprintf

#endif /* HAVE_SNPRINTF */
