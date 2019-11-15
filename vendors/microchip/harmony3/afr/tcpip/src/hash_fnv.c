/*******************************************************************************
  FNV Hash implementation file

  Summary:
    FNV Hash routines
    
  Description:
    This source file contains the functions that implement the FNV hash
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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

THIRD PARTY SOFTWARE:  Notwithstanding anything to the contrary, any third party software
accompanying this software is subject to the terms and conditions of the third party’s
license agreement or terms. To the extent required by third party licenses covering such
software, the terms of the third party license will apply in lieu of the terms provided
in this notice. To the extent the terms of such third party licenses prohibit any of the
restrictions described herein, such restrictions will not apply to such third party software. 
 
MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.


FNV Code terms of use:  

 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *      chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
*****************************************************************************/









#include "tcpip/src/hash_fnv.h"


uint32_t fnv_32_hash(const void *key, size_t keyLen)
{
    size_t      ix;
    uint32_t    hval = FNV_32_INIT;
    
    const uint8_t* p = (const uint8_t*)key;

    for(ix = 0; ix < keyLen; ix++)
    {
#if defined(NO_FNV_GCC_OPTIMIZATION)
        hval *= FNV_32_PRIME;
#else
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#endif
        hval ^= (uint32_t)*p++;
    }
    return hval;
}


uint32_t fnv_32a_hash(const void *key, size_t keyLen)
{
    size_t      ix;
    uint32_t    hval = FNV_32_INIT;

    const uint8_t* p = (const uint8_t*)key;

    for(ix = 0; ix < keyLen; ix++)
    {
        hval ^= (uint32_t)*p++;

#if defined(NO_FNV_GCC_OPTIMIZATION)
        hval *= FNV_32_PRIME;
#else
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#endif
    }

    return hval;
}


