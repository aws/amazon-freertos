/*
 * Copyright (c) 2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SL_NET_H__
#define __SL_NET_H__

#include <stdint.h>

#ifdef    __cplusplus
extern "C" {
#endif

/*!
    \defgroup SlNet SlNet group

    \short Interface for general SlNet services.

*/
/*!
    \addtogroup SlNet
    @{
*/

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

/*!

    \brief Prototype for SysConfig generated initialization function that
    initializes various parts of the NS stack based on the user's SysConfig
    script. This function should be called after the lower network stacks
    initialize. Non-SysConfig users can provide their own implementation of
    this function in their application's source code.

    \return                     Zero on success, or negative error code on failure
*/
int32_t ti_net_SlNet_initConfig();

/*!

 Close the Doxygen group.
 @}

*/

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __SL_NET_H__ */
