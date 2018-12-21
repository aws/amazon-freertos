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

/*****************************************************
  Usage:
   1. function: (a or b)
      a. ATTR_TEXT_IN_TCM int func(int par);
      b. ATTR_TEXT_IN_TCM int func(int par)
        {
        }
   2. RO data:
      a. ATTR_RODATA_IN_TCM const int b = 8;
   3. RW data:
      a. ATTR_RWDATA_IN_TCM int b = 8;
   4. ZI data:
      a. ATTR_ZIDATA_IN_TCM int b;

  Note: must put these attributes at HEAD of declaration.
*****************************************************/

#ifndef MEMORY_ATTRIBUTE_H_
#define MEMORY_ATTRIBUTE_H_

#ifndef __ICCARM__

#define ATTR_TEXT_IN_TCM              __attribute__ ((__section__(".ramTEXT")))
#define ATTR_RODATA_IN_TCM
#define ATTR_RWDATA_IN_TCM
#define ATTR_ZIDATA_IN_TCM            __attribute__ ((__section__(".tcmBSS")))


#define ATTR_TEXT_IN_RAM
#define ATTR_RWDATA_IN_NONCACHED_RAM
#define ATTR_ZIDATA_IN_NONCACHED_RAM

#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__aligned__(4)))
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__aligned__(4)))

#define ATTR_PACKED __attribute__ ((__packed__))

#define ATTR_4BYTE_ALIGN __attribute__ ((__aligned__(4)))
#else
#define ATTR_TEXT_IN_TCM              _Pragma("location=\".ramTEXT\"")
#define ATTR_RODATA_IN_TCM
#define ATTR_RWDATA_IN_TCM
#define ATTR_ZIDATA_IN_TCM            _Pragma("location=\".tcmBSS\"")
#define ATTR_TEXT_IN_RAM
#define ATTR_RWDATA_IN_NONCACHED_RAM
#define ATTR_ZIDATA_IN_NONCACHED_RAM
#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  _Pragma("location=\".noncached_rwdata\"") \
                                                  _Pragma("data_alignment=4")
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  _Pragma("location=\".noncached_zidata\"") \
                                                  _Pragma("data_alignment=4")
#define ATTR_PACKED __packed
#define ATTR_4BYTE_ALIGN _Pragma("data_alignment=4")
#endif
#endif

