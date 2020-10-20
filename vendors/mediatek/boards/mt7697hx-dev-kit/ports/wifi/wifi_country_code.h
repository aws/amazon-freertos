/*
 * Amazon FreeRTOS
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * This file is based on wifi_country_code.txt in the MT7697 SDK, with necessary
 * modifications to make it work for AWS WIFI.
 */

#include "wifi_api.h"

// Geography values in the channel table
#define ODOR    0
#define IDOR    1
#define BOTH    2

// DFS type value in the country code
#define CE      0
#define FCC     1
#define JAP     2

// Regularoty domain values in the extended country code. They are mapped to the
// NDVM data item SingleSKU_index in this order
#define REG_DOMAIN_FCC      0
#define REG_DOMAIN_ETSI     1
#define REG_DOMAIN_TELEC    2
#define REG_DOMAIN_KCC      3

// Max number of channel tables per country
#define MAX_CH_TABLE   5

#ifndef TRUE
#define TRUE    true
#endif

#ifndef FALSE
#define FALSE   false
#endif

typedef struct {
    // These fields are identical to wifi_country_code_t defined in wifi_api.h
    uint8_t country_code[4];
    uint8_t dfs_type;           // 0: CE, 1: FCC, 2: JAP, 3:JAP_W53, JAP_W56
    uint8_t num_of_channel_table;
    wifi_channel_table_t channel_table[MAX_CH_TABLE];

    // Extended field for regulatory domain
    uint8_t reg_domain;
} wifi_country_code_ext_t;

const wifi_country_code_ext_t* WIFI_HAL_FindCountryCode( const char* pcCountryCode );

