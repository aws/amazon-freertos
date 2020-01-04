/**
* \copyright
* Copyright (c) 2018, Infineon Technologies AG
* All rights reserved.
*
* This software is provided with terms and conditions as specified in OPTIGA? Trust X Evaluation Kit License Agreement.
* \endcopyright
*
* \author   Infineon AG
*
* \file
*
* \brief   This file implements the prototype declarations of pal os event
*
* \ingroup  grPAL
* @{
*/


#ifndef _PAL_OS_EVENT_H_
#define _PAL_OS_EVENT_H_

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
 
#include "optiga/common/Datatypes.h"
#include "optiga/pal/pal.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * ENUMS
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * PAL extern definitions
 *********************************************************************************************************************/
 
/**
 * @brief typedef for Callback function when timer elapses.
 */
typedef void (*register_callback)(void*);

#ifdef PAL_OS_HAS_EVENT_INIT
/**
 * @brief Platform specific event init function.
 */
pal_status_t pal_os_event_init(void);
#endif

/**
 * @brief Callback registration function to trigger once when timer expires.
 */
void pal_os_event_register_callback_oneshot(register_callback callback, void* callback_args, uint32_t time_us);



#endif //_PAL_OS_EVENT_H_

/**
* @}
*/
