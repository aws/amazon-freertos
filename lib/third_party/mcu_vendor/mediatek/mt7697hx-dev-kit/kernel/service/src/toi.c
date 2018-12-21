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

#include "toi.h"

#define is_0_to_1(_c)   (_c == '0' || _c == '1')
#define is_0(_c)        (_c == '0')
#define is_x(_c)        (_c == 'x' || _c == 'X')
#define is_b(_c)        (_c == 'b' || _c == 'B')
#define is_0_to_7(_c)   (_c >= '0' && _c <= '7')
#define is_0_to_9(_c)   (_c >= '0' && _c <= '9')
#define is_1_to_9(_c)   (_c >= '1' && _c <= '9')
#define is_a_to_f(_c)   (_c >= 'a' && _c <= 'f')
#define is_A_to_F(_c)   (_c >= 'A' && _c <= 'F')

#define dec_to_dec(_c)  (_c - '0')
#define hex_to_dec(_c)  (_c - 'a' + 10)
#define heX_to_dec(_c)  (_c - 'A' + 10)

/**
 * Detect integer type.
 *
 * @param b     input string buffer.
 * @param type  the detected type.
 *
 * @return      Pointer address. The address is not useful if type is error.
 */
static char *_toi_detect(char *b, uint8_t *type)
{
    *type = TOI_ERR;

    if (is_1_to_9( *b )) {
        *type = TOI_DEC;
    } else if (is_0( *b )) {
        b++;
        if (is_x( *b )) {
            b++;
            *type = TOI_HEX;
        } else if (is_b( *b )) {
            b++;
            *type = TOI_BIN;
        } else if (is_0_to_7( *b )) {
            *type = TOI_OCT;
        } else if (*b == 0) {
            *type = TOI_DEC; // a single '0'.
        }
    }

    return b;
}

uint32_t toi(char *b, uint8_t *type)
{
    uint32_t    v       = 0;

    b = _toi_detect(b, type);

    if (*type == TOI_ERR)
        return v;

    if (*type == TOI_BIN) {
        while (*b != 0) {
            if (!is_0_to_1( *b )) {
                *type = TOI_ERR; break;
            }
            v  = v << 1;
            v += *b - '0';
            b++;
        }
    }

    if (*type == TOI_OCT) {
        while (*b != 0) {
            if (!is_0_to_7( *b )) {
                *type = TOI_ERR; break;
            }
            v  = v << 3;
            v += *b - '0';
            b++;
        }
    }

    if (*type == TOI_DEC) {
        while (*b != 0) {
            if (!is_0_to_9( *b )) {
                *type = TOI_ERR; break;
            }
            v  = v * 10;
            v += *b - '0';
            b++;
        }
    }

    if (*type == TOI_HEX) {
        while (*b != 0) {
            v  = v << 4;
            if (is_0_to_9( *b )) {
                v += *b - '0';
            } else if (is_a_to_f( *b )) {
                v += *b - 'a' + 10;
            } else if (is_A_to_F( *b )) {
                v += *b - 'A' + 10;
            } else {
                *type = TOI_ERR; break;
            }
            b++;
        }
    }

    return (*type == TOI_ERR) ? 0 : (uint32_t)v;
}


uint8_t tov(char *b, uint32_t *value)
{
    uint8_t     type;

    *value = toi(b, &type);

    return type;
}

