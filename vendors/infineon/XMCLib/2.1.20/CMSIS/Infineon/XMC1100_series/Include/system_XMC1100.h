/*********************************************************************************************************************
 * @file     system_XMC1100.h
 * @brief    Device specific initialization for the XMC1100-Series according to CMSIS
 * @version  V1.2
 * @date     19 Jul 2013
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2012-2016, Infineon Technologies AG
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
 **************************** Change history *********************************
 * V1.1, 13 Dec 2012, PKB, Created this table, added extern and stdint
 * V1.2, 19 Jul 2013, PKB, Added header guard, BootROM header, C++ support
 *****************************************************************************
 * @endcond 
 */

#ifndef SYSTEM_XMC1100_H
#define SYSTEM_XMC1100_H

/*******************************************************************************
 * HEADER FILES
 *******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * GLOBAL VARIABLES
 *******************************************************************************/

extern uint32_t SystemCoreClock;

/*******************************************************************************
 * API PROTOTYPES
 *******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the system
 *
 */
void SystemInit(void);

/**
 * @brief Initialize CPU settings
 *
 */
void SystemCoreSetup(void);

/**
 * @brief Initialize clock
 *
 */
void SystemCoreClockSetup(void);

/**
 * @brief Update SystemCoreClock variable
 *
 */
void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif
