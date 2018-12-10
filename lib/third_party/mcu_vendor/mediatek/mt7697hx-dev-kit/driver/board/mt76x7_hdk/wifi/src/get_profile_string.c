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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "os.h"
#include "connsys_profile.h"

#include "wifi_api.h"
#include "wifi_scan.h"
#include "get_profile_string.h"
#include "syslog.h"


#ifndef LOG_E
#define LOG_E( mod, format... )  printf(format)
#endif

static char *find_next_char(char *src, unsigned int src_len, char want, char replace)
{
    char            *ret = NULL;
    unsigned int    i;

    for (i = 0; i < src_len; i++) {
        if (want == (*(src + i))) {
            if (want != replace) {
                (*(src + i)) = replace;
            }

            return (src + i);
        }
    }

    return ret;
}


static int parse_ch_triplet(char *first_ch, char *ch_num, char *ch_prop, const char *str, unsigned int str_len)
{
    unsigned int    len = str_len;
    char            *ptr;
    char            *strr = (char *)str;

    ptr = find_next_char(strr, len, ',', ',');
    if (NULL == ptr) {
        return -1;
    }

    (*first_ch) = atoi(strr);
    len = len - ((ptr - strr) + 1);
    strr = ptr + 1;

    ptr = find_next_char(strr, len, ',', ',');
    if (NULL == ptr) {
        return -2;
    }

    (*ch_num) = atoi(strr);
    len = len - ((ptr - strr) + 1);
    strr = ptr + 1;

    (*ch_prop) = atoi(strr);

    return 0;
}


int wifi_conf_get_ch_table_from_str(ch_desc_t *ch_list, char ch_list_num, const char *ch_str, unsigned int ch_str_len)
{
    unsigned int    len = ch_str_len;
    char            *ptr;
    unsigned char   index;
    int             ch_num;

    if ((!ch_list) || (!ch_str)) {
        return -1;
    }

    index  = 0;
    ch_num = 0;

    do {
        ptr = find_next_char((char *)ch_str, len, '|', '|');
        if (NULL == ptr) {
            //return -1;
            break;
        }

        os_memset(&(ch_list[index]), 0x0, sizeof(*ch_list));

        parse_ch_triplet((char *) & (ch_list[index].first_channel),
                         (char *) & (ch_list[index].num_of_ch),
                         (char *) & (ch_list[index].channel_prop),
                         ch_str,
                         (ptr - ch_str));

        len    = len - ((ptr - ch_str) + 1);
        ch_str = ptr + 1;

        ch_num++;
        index++;
    } while (ch_num < ch_list_num);

    return ch_num;

}


void wifi_conf_get_mac_from_str(char *mac_dst, const char *mac_src)
{
    unsigned int len = os_strlen(mac_src);

    if (len == 17) {
        //Mac address acceptable format 01:02:03:04:05:06 length 17
        int     j;
        char    *p = (char *)mac_src;
        for (j = 0; j < ETH_LENGTH_OF_ADDRESS; j++) {
            AtoH(p, &mac_dst[j], 1);
            p += 3;
        }
    } else {
        LOG_E(wifi, "string format is wrong.. %s", mac_src);
    }
}


void wifi_conf_get_ip_from_str(unsigned char *ip_dst, const char *ip_src)
{
    int     i   = 0;
    int     len = os_strlen(ip_src);
    char    *dot_ptr;

    if (os_strlen(ip_src) < 7) {
        LOG_E(wifi, "string format is wrong.. %s", ip_src);
        return;
    }

    for (i = 0; i < 3; i++) {
        char tmp_str[3];

        dot_ptr = find_next_char((char *)ip_src, len, '.', '.');
        if (dot_ptr > (ip_src + os_strlen(ip_src))) {
            LOG_E(wifi, "string format is wrong.. %s", ip_src);
            return;
        }
        os_memset(tmp_str, 0x0, sizeof(tmp_str));
        os_memcpy(tmp_str, ip_src, (dot_ptr - ip_src));
        ip_dst[i] = atoi(tmp_str);

        len = len - ((dot_ptr - ip_src) + 1);
        ip_src = (dot_ptr + 1);
    }
    
    ip_dst[i] = atoi(ip_src);
}

#ifdef MTK_SINGLE_SKU_SUPPORT
void wifi_conf_get_pwr_from_str(singleSKU2G_t * single, const char *pwr_src)
{
    int     i   = 0;
    int     len = os_strlen(pwr_src);
    char    *dot_ptr;
    char    pwr_dst[266];
    
    /* 14ch * 19 power=266 */
    for (i = 0; i < 265; i++) {
        char tmp_str[2];

        dot_ptr = find_next_char((char *)pwr_src, len, ',', ',');
        if ((dot_ptr > (pwr_src + os_strlen(pwr_src))) || (dot_ptr == NULL)) {
            LOG_E(wifi, "Single SKU Pwr string format is wrong.. %s", pwr_src);
            return;
        }
        os_memset(tmp_str, 0x0, sizeof(tmp_str));
        os_memcpy(tmp_str, pwr_src, (dot_ptr - pwr_src));

        pwr_dst[i] = atoi(tmp_str);

        len = len - ((dot_ptr - pwr_src) + 1);
        pwr_src = (dot_ptr + 1);
    }
    
    pwr_dst[i] = atoi(pwr_src);

    //copy to 14 channel, each channel 19 power value.
    for(uint8_t i = 0; i<14; i++) {
        os_memcpy(single->singleSKU2G[i], pwr_dst+19*i, 19);
    }
}
#endif
