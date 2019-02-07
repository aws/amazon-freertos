/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

 #ifndef _MET_TRACE_H
#define _MET_TRACE_H

#include "met_types.h"
extern uint8_t evtype;

#define MAX_TASK_NAME_LEN (6) /*configMAX_TASK_NAME_LEN=6 */

#define traceTASK_CREATE(pxNewTCB) \
    if (pxNewTCB != NULL) \
    { \
        met_add_task_object(pxNewTCB->uxTCBNumber, pxNewTCB->uxPriority, pxNewTCB->pcTaskName); \
        met_add_event1(EV_TASK_CREATE, pxNewTCB->uxTCBNumber); \
    }

#if ( ( INCLUDE_vTaskDelete == 1 ) && ( INCLUDE_vTaskSuspend == 1 ) )
#define traceTASK_SWITCHED_OUT()																\
{																								\
	List_t *pxStateList;																		\
																								\
	pxStateList = ( List_t * ) listLIST_ITEM_CONTAINER( &( pxCurrentTCB->xGenericListItem ) );			\
																								\
	if( ( pxStateList == pxDelayedTaskList ) || ( pxStateList == pxOverflowDelayedTaskList ) )	\
	{																							\
		evtype = EV_TASK_SWTCH_2;															    \
	}																							\
	else if( pxStateList == &xSuspendedTaskList )												\
	{																							\
		if( listLIST_ITEM_CONTAINER( &( pxCurrentTCB->xEventListItem ) ) == NULL )						\
		{																						\
			evtype = EV_TASK_SWTCH_3;															\
		}																						\
		else																					\
		{																						\
			evtype = EV_TASK_SWTCH_2;															\
		}																						\
	}																							\
	else if( pxStateList == &xTasksWaitingTermination )											\
	{																							\
		evtype = EV_TASK_SWTCH_4;																\
	}																							\
	else 																						\
	{																							\
		evtype = EV_TASK_SWTCH_1;																\
	} \
}
#endif

#if ( ( INCLUDE_vTaskDelete == 1 ) && ( INCLUDE_vTaskSuspend == 0 ) )

#define traceTASK_SWITCHED_OUT()																\
{																								\
	List_t *pxStateList;																		\
																								\
	pxStateList = ( List_t * ) listLIST_ITEM_CONTAINER( &( pxCurrentTCB->xGenericListItem ) );			\
																								\
	if( ( pxStateList == pxDelayedTaskList ) || ( pxStateList == pxOverflowDelayedTaskList ) )	\
	{																							\
		evtype = EV_TASK_SWTCH_2;																		\
	}																							\
	else if( pxStateList == &xTasksWaitingTermination )											\
	{																							\
		evtype = EV_TASK_SWTCH_4;																		\
	}																							\
	else 																						\
	{																							\
		evtype = EV_TASK_SWTCH_1;																		\
	} \
}
#endif

#if ( ( INCLUDE_vTaskDelete == 0 ) && ( INCLUDE_vTaskSuspend == 1 ) )

#define traceTASK_SWITCHED_OUT()																\
{																								\
	List_t *pxStateList;																		\
																								\
	pxStateList = ( List_t * ) listLIST_ITEM_CONTAINER( &( pxCurrentTCB->xGenericListItem ) );			\
																								\
	if( ( pxStateList == pxDelayedTaskList ) || ( pxStateList == pxOverflowDelayedTaskList ) )	\
	{																							\
		evtype = EV_TASK_SWTCH_2;																		\
	}																							\
	else if( pxStateList == &xSuspendedTaskList )												\
	{																							\
		if( listLIST_ITEM_CONTAINER( &( pxCurrentTCB->xEventListItem ) ) == NULL )						\
		{																						\
			evtype = EV_TASK_SWTCH_3;																\
		}																						\
		else																					\
		{																						\
			evtype = EV_TASK_SWTCH_2;																	\
		}																						\
	}																							\
	else 																						\
	{																							\
		evtype = EV_TASK_SWTCH_1;																		\
	} \
}
#endif

#if ( ( INCLUDE_vTaskDelete == 0 ) && ( INCLUDE_vTaskSuspend == 0 ) )

#define traceTASK_SWITCHED_OUT()																\
{																								\
	List_t *pxStateList;																		\
	pxStateList = ( List_t * ) listLIST_ITEM_CONTAINER( &( pxCurrentTCB->xGenericListItem ) );			\
																								\
	if( ( pxStateList == pxDelayedTaskList ) || ( pxStateList == pxOverflowDelayedTaskList ) )	\
	{																							\
		evtype = EV_TASK_SWTCH_2;																		\
	}																							\
	else 																						\
	{																							\
		evtype = EV_TASK_SWTCH_1;																		\
	} \
}
#endif

#define traceTASK_SWITCHED_IN()																	\
	met_add_event1(evtype, pxCurrentTCB->uxTCBNumber);


#endif //_MET_TRACE_H