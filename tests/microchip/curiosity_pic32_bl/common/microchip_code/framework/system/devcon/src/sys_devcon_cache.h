/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

#ifndef __PIC32_CACHE_H
#define __PIC32_CACHE_H

#ifndef _CP0_ERRCTL
#define _CP0_ERRCTL                         $26, 0
#endif

#ifndef _CP0_TAGLO
#define _CP0_TAGLO                          $28, 0
#endif

/*
 * Cache operations
 */

#define Index_Store_Tag_I                0x08        /* 2       0 */
#define Index_Store_Tag_D                0x09        /* 2       1 */

#define Hit_Writeback_Inv_D              0x15        /* 5       1 */
#define Hit_Invalidate_I                 0x10        /* 4       0 */
#define Hit_Writeback_Inv_S              0x17        /* 5       3 */
#define Hit_Invalidate_D                 0x11        /* 4       1 */
#define Index_Invalidate_I               0x00        /* 0       0 */
#define Index_Writeback_Inv_D            0x01        /* 0       1 */
#define Fetch_Lock_I                 	 0x1C        /* 7       0 */
#define Fetch_Lock_D                 	 0x1D        /* 7       1 */

#define tmp t0
#define cfg t1
#define icachesize t2
#define ilinesize t3
#define iways t4
#define dcachesize t5
#define dlinesize t6
#define dways t7
#define coherency v0
#define save_ra v1

#endif
