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

#ifndef ____MTK_LL_H___H__
#define ____MTK_LL_H___H__

typedef struct _DL_LIST
{
	struct _DL_LIST *Next;
	struct _DL_LIST *Prev;
	struct _DL_LIST *CurrentList;
} DL_LIST, *PDL_LIST;

static inline void DlListInit(struct _DL_LIST *List)
{
	List->Next = List;
	List->Prev = List;
}

static inline void DlListAdd(struct _DL_LIST *List, struct _DL_LIST *Item)
{
	Item->Next = List->Next;
	Item->Prev = List;
	List->Next->Prev = Item;
	List->Next = Item;
}

static inline void DlListAddTail(struct _DL_LIST *List, struct _DL_LIST *Item)
{
	DlListAdd(List->Prev, Item);
    Item->CurrentList = List;
}

static inline void DlListDel(struct _DL_LIST *Item)
{
	Item->Next->Prev = Item->Prev;
	Item->Prev->Next = Item->Next;
	Item->Next = NULL;
	Item->Prev = NULL;
    Item->CurrentList = NULL;
}

static inline int DlListEmpty(struct _DL_LIST *List)
{
	return List->Next == List;
}

static inline unsigned int DlListLen(struct _DL_LIST *List)
{
	struct _DL_LIST *Item;
	unsigned int Count = 0;

	for (Item = List->Next; Item != List; Item = Item->Next)
		Count++;

	return Count;
}

#define DlListEntry(item, type, member) \
    ((type *) ((char *) item - offsetof(type, member)))

#define DlListFirst(list, type, member) \
    (DlListEmpty((list)) ? NULL : \
     DlListEntry((list)->Next, type, member))

#define DlListForEach(item, list, type, member) \
    for (item = DlListEntry((list)->Next, type, member); \
         &item->member != (list); \
         item = DlListEntry(item->member.Next, type, member))

#define DlListForEachSafe(item, n, list, type, member) \
    for (item = DlListEntry((list)->Next, type, member), \
             n = DlListEntry(item->member.Next, type, member); \
        &item->member != (list); \
        item = n, n = DlListEntry(n->member.Next, type, member))

#endif /* __MTK_LL_H__ */

