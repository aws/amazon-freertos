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

#ifndef __EXCEPTION_HANDLER_H__
#define __EXCEPTION_HANDLER_H__

#include <stdbool.h>

#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533)
#include "mt2523.h"
#include "hal_flash_mtd.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#include "mt7687.h"
#include "flash_sfc.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

void exception_get_assert_expr(const char **expr, const char **file, int *line);

#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

#define DISABLE_MEMDUMP_MAGIC 0xdeadbeef

typedef struct
{
  char *region_name;
  unsigned int *start_address;
  unsigned int *end_address;
  unsigned int is_dumped;
} memory_region_type;

typedef void (*f_exception_callback_t)(void);

typedef struct
{
  f_exception_callback_t init_cb;
  f_exception_callback_t dump_cb;
} exception_config_type;

bool exception_register_callbacks(exception_config_type *cb);
void exception_dump_config(int flag);
void exception_reboot(void);


typedef struct
{
  bool          is_valid;
  const char    *expr;
  const char    *file;
  int           line;
} assert_expr_t;

#define platform_assert(e, f, l) do               \
{                                                 \
    extern assert_expr_t assert_expr;             \
    __asm("cpsid i");                             \
    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;         \
    assert_expr.is_valid = true;                  \
    assert_expr.expr = (const char *)(e);         \
    assert_expr.file = (const char *)(f);         \
    assert_expr.line = l;                         \
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;  \
    for (;;);                                     \
} while (0)

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

bool do_write_to_ext_flash(void);
hal_flash_status_t hal_flash_erase_wrap(uint32_t start_address, hal_flash_block_t block);
hal_flash_status_t hal_flash_write_wrap(uint32_t address, const uint8_t *data, uint32_t length);
bool is_crash_dump_save_to_flash(void);

#endif

#endif /* __EXCEPTION_HANDLER_H__ */

