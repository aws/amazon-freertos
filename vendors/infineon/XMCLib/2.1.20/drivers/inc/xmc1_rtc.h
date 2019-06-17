/**
 * @file xmc1_rtc.h
 * @date 2015-05-20
 *
 * @cond
 *********************************************************************************************************************
 * XMClib v2.1.20 - XMC Peripheral Driver Library 
 *
 * Copyright (c) 2015-2018, Infineon Technologies AG
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
 * Change History
 * --------------
 *
 * 2015-02-20:
 *     - Initial version
 *      
 * 2015-05-20:
 *     - Documentation updates <br>
 *   
 * @endcond 
 *
 */

#ifndef XMC1_RTC_H
#define XMC1_RTC_H

/**
 * @addtogroup XMClib XMC Peripheral Library
 * @{
 */

/**
 * @addtogroup RTC
 * @{
 */
 
/*********************************************************************************************************************
 * ENUMS
 *********************************************************************************************************************/

/**
 * Debug mode status values
 */
typedef enum XMC_RTC_DEBUG_MODE
{
  XMC_RTC_RUN_IN_DEBUG_MODE  = 0U, /**< RTC is not stopped during halting mode debug */
  XMC_RTC_STOP_IN_DEBUG_MODE = 1U  /**< RTC is stopped during halting mode debug */  
} XMC_RTC_DEBUG_MODE_t;

/*********************************************************************************************************************
 * API PROTOTYPES
 *********************************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @param debug_mode Debug mode value containing in (::XMC_RTC_DEBUG_MODE_t) to be set                 
 * @return None
 *
 * \par<b>Description: </b><br>
 * Configures the RTC into running or stopping mode during halting mode debug <br>
 *
 * \par
 * The function sets the CTR.SUS bitfield to configure the RTC into running 
 * or stopping mode during halting mode debug.
 */   
void XMC_RTC_SetDebugMode(const XMC_RTC_DEBUG_MODE_t debug_mode);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

/**
 * @}
 */
 
#endif /* XMC1_RTC_H */
