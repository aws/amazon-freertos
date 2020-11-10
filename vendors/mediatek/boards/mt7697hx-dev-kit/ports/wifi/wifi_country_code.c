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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "wifi_country_code.h"

static const wifi_country_code_ext_t wifiCountryCode_[] =
{
    {   // Argentina
        "AR",
        CE,
        3,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 52,  4,  24, BOTH, FALSE},    // 5G, ch 52~64
            { 149, 4,  30, BOTH, FALSE},    // 5G, ch 149~161
        },
        REG_DOMAIN_FCC
    },

    {   // Australia
        "AU",
        CE,
        4,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  23, BOTH, FALSE},    // 5G, ch 36~48
            { 52,  4,  24, BOTH, FALSE},    // 5G, ch 52~64
            { 149, 5,  30, BOTH, FALSE},    // 5G, ch 149~165
        },
        REG_DOMAIN_FCC
    },

    {   // Austria
        "AT",
        CE,
        4,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  23, IDOR, TRUE},     // 5G, ch 36~48
            { 52,  4,  23, IDOR, TRUE},     // 5G, ch 52~64
            { 100, 11, 30, BOTH, TRUE},     // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Brazil
        "BR",
        CE,
        5,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  23, BOTH, FALSE},    // 5G, ch 36~48
            { 52,  4,  24, BOTH, FALSE},    // 5G, ch 52~64
            { 100, 11, 24, BOTH, FALSE},    // 5G, ch 100~140
            { 149, 5,  30, BOTH, FALSE},    // 5G, ch 100~140
        },
        REG_DOMAIN_FCC
    },

    {   // Belgium
        "BE",
        CE,
        3,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  18, IDOR, FALSE},    // 5G, ch 36~48
            { 52,  4,  18, IDOR, FALSE},    // 5G, ch 52~64
        },
        REG_DOMAIN_ETSI
    },

    {   // Bulgaria
        "BG",
        CE,
        4,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  23, IDOR, FALSE},    // 5G, ch 36~48
            { 52,  4,  23, IDOR, TRUE}, // 5G, ch 52~64
            { 100, 11, 30, ODOR, TRUE}, // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Canada
        "CA",
        CE,
        4,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  23, BOTH, FALSE},    // 5G, ch 36~48
            { 52,  4,  23, BOTH, FALSE},    // 5G, ch 52~64
            { 149, 5,  30, BOTH, FALSE},    // 5G, ch 149~165
        },
        REG_DOMAIN_FCC
    },

    {   // Chile
        "CL",
        CE,
        4,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  20, BOTH, FALSE},    // 5G, ch 36~48
            { 52,  4,  20, BOTH, FALSE},    // 5G, ch 52~64
            { 149, 5,  20, BOTH, FALSE},    // 5G, ch 149~165
        },
        REG_DOMAIN_FCC
    },

    {   // China
        "CN",
        CE,
        2,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 149, 4,  27, BOTH, FALSE},    // 5G, ch 149~161
        },
        REG_DOMAIN_ETSI
    },

    {   // Colombia
        "CO",
        CE,
        5,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  17, BOTH, FALSE},    // 5G, ch 36~48
            { 52,  4,  24, BOTH, FALSE},    // 5G, ch 52~64
            { 100, 11, 30, BOTH, FALSE},    // 5G, ch 100~140
            { 149, 5,  30, BOTH, FALSE},    // 5G, ch 149~165
        },
        REG_DOMAIN_FCC
    },

    {   // Costa Rica
        "CR",
        CE,
        4,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  17, BOTH, FALSE},    // 5G, ch 36~48
            { 52,  4,  24, BOTH, FALSE},    // 5G, ch 52~64
            { 149, 4,  30, BOTH, FALSE},    // 5G, ch 149~161
        },
        REG_DOMAIN_FCC
    },

    {   // Cyprus
        "CY",
        CE,
        4,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  23, IDOR, FALSE},    // 5G, ch 36~48
            { 52,  4,  24, IDOR, TRUE},     // 5G, ch 52~64
            { 100, 11, 30, BOTH, TRUE},     // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Czech_Republic
        "CZ",
        CE,
        3,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  23, IDOR, FALSE},    // 5G, ch 36~48
            { 52,  4,  23, IDOR, TRUE},     // 5G, ch 52~64
        },
        REG_DOMAIN_ETSI
    },

    {   // Denmark
        "DK",
        CE,
        4,
        {
            { 1,   13, 20, BOTH, FALSE},    // 2.4 G, ch 1~13
            { 36,  4,  23, IDOR, FALSE},    // 5G, ch 36~48
            { 52,  4,  23, IDOR, TRUE},     // 5G, ch 52~64
            { 100, 11, 30, BOTH, TRUE},     // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Dominican Republic
        "DO",
        CE,
        2,
        {
            { 1,   0,  20, BOTH, FALSE},    // 2.4 G, ch 0
            { 149, 4,  20, BOTH, FALSE},    // 5G, ch 149~161
        },
        REG_DOMAIN_FCC
    },

    {   // Finland
        "FI",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,  4,   23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,  4,   23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // France
        "FR",
        CE,
        3,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,  4,   23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,  4,   23, IDOR, TRUE},    // 5G, ch 52~64
        },
        REG_DOMAIN_ETSI
    },

    {   // Germany
        "DE",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,  4,   23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,  4,   23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Greece
        "GR",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,  4,   23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,  4,   23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, ODOR, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Hong Kong
        "HK",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, FALSE},   // 5G, ch 52~64
            { 149,  4,  30, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_FCC
    },

    {   // Hungary
        "HU",
        CE,
        3,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
        },
        REG_DOMAIN_ETSI
    },

    {   // Iceland
        "IS",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // India
        "IN",
        CE,
        2,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 149,  4,  24, IDOR, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_FCC
    },

    {   // Indonesia
        "ID",
        CE,
        2,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 149,  4,  27, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_ETSI
    },

    {   // Ireland
        "IE",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, ODOR, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Israel
        "IL",
        CE,
        3,
        {
            { 1,    3,  20, IDOR, FALSE},   // 2.4 G, ch 1~3
            { 4,    6,  20, BOTH, FALSE},   // 2.4 G, ch 4~9
            { 10,   4,  20, IDOR, FALSE},   // 2.4 G, ch 10~13
        },
        REG_DOMAIN_ETSI
    },

    {   // Italy
        "IT",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, ODOR, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Japan
        "JP",
        JAP,
        2,
        {
            { 1,   14,  20, BOTH, FALSE},   // 2.4 G, ch 1~14
            { 34,   4,  23, IDOR, FALSE},   // 5G, ch 34~46
        },
        REG_DOMAIN_TELEC
    },

    {   // Latvia
        "LV",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Liechtenstein
        "LI",
        CE,
        3,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Lithuania
        "LT",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Luxemburg
        "LU",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Malaysia
        "MY",
        CE,
        3,
        {
            { 36,   4,  23, BOTH, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, BOTH, FALSE},   // 5G, ch 52~64
            { 149,  5,  20, BOTH, FALSE},   // 5G, ch 149~165
        },
        REG_DOMAIN_FCC
    },

    {   // Malta
        "MT",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Mexico
        "MX",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, BOTH, FALSE},   // 5G, ch 36~48
            { 52,   4,  24, BOTH, FALSE},   // 5G, ch 52~64
            { 149,  5,  30, IDOR, FALSE},   // 5G, ch 149~165
        },
        REG_DOMAIN_FCC
    },

    {   // Netherlands
        "NL",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  24, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // New Zealand
        "NZ",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  24, BOTH, FALSE},   // 5G, ch 36~48
            { 52,   4,  24, BOTH, FALSE},   // 5G, ch 52~64
            { 149,  4,  30, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_FCC
    },

    {   // Norway
        "NO",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  24, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  24, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 149~161
        },
        REG_DOMAIN_ETSI
    },

    {   // Peru
        "PE",
        CE,
        2,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 149,  4,  27, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_FCC
    },

    {   // Portugal
        "PT",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Poland
        "PL",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Romania
        "RO",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Russia
        "RU",
        CE,
        2,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 149,  4,  20, IDOR, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_ETSI
    },

    {   // Saudi Arabia
        "SA",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, BOTH, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, BOTH, FALSE},   // 5G, ch 52~64
            { 149,  4,  23, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_ETSI
    },

    {   // Singapore
        "SG",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, BOTH, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, BOTH, FALSE},   // 5G, ch 52~64
            { 149,  4,  20, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_ETSI
    },

    {   // Slovakia
        "SK",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Slovenia
        "SI",
        CE,
        3,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
        },
        REG_DOMAIN_ETSI
    },

    {   // South Africa
        "ZA",
        CE,
        5,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, BOTH, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, FALSE},   // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
            { 149,  4,  30, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_ETSI
    },

    {   // South Korea
        "KR",
        CE,
        5,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  20, BOTH, FALSE},   // 5G, ch 36~48
            { 52,   4,  20, BOTH, FALSE},   // 5G, ch 52~64
            { 100,  8,  20, BOTH, FALSE},   // 5G, ch 100~128
            { 149,  4,  20, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_KCC
    },

    {   // Spain
        "ES",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  17, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Sweden
        "SE",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Switzerland
        "CH",
        CE,
        3,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~13
            { 36,   4,  23, IDOR, TRUE},    // 5G, ch 36~48
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
        },
        REG_DOMAIN_ETSI
    },

    {   // Taiwan
        "TW",
        CE,
        2,
        {
            { 1,   11,  30, BOTH, FALSE},   // 2.4 G, ch 1~11
            { 52,   4,  23, IDOR, FALSE},   // 5G, ch 52~64
        },
        REG_DOMAIN_FCC
    },

    {   // Turkey
        "TR",
        CE,
        3,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~11
            { 36,   4,  23, BOTH, FALSE},   // 5G, ch 36~48
            { 52,   4,  23, BOTH, FALSE},   // 5G, ch 52~64
        },
        REG_DOMAIN_ETSI
    },

    {   // UK
        "GB",
        CE,
        4,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~11
            { 36,   4,  23, IDOR, FALSE},   // 5G, ch 52~64
            { 52,   4,  23, IDOR, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
        },
        REG_DOMAIN_ETSI
    },

    {   // Ukraine
        "UA",
        CE,
        1,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~11
        },
        REG_DOMAIN_ETSI
    },

    {   // United_Arab_Emirates
        "AE",
        CE,
        1,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~11
        },
        REG_DOMAIN_ETSI
    },

    {   // United_States
        "US",
        CE,
        5,
        {
            { 1,   11,  30, BOTH, FALSE},   // 2.4 G, ch 1~11
            { 36,   4,  17, IDOR, FALSE},   // 5G, ch 52~64
            { 52,   4,  24, BOTH, TRUE},    // 5G, ch 52~64
            { 100, 11,  30, BOTH, TRUE},    // 5G, ch 100~140
            { 149,  5,  30, BOTH, FALSE},   // 5G, ch 149~165
        },
        REG_DOMAIN_FCC
    },

    {   // Venezuela
        "VE",
        CE,
        2,
        {
            { 1,   13,  20, BOTH, FALSE},   // 2.4 G, ch 1~11
            { 149,  4,  27, BOTH, FALSE},   // 5G, ch 149~161
        },
        REG_DOMAIN_FCC
    },

    {   // Default, world-safe
        "WS",
        CE,
        5,
        {
            { 1,   11,  20, BOTH, FALSE},   // 2.4 G, ch 1~11
            { 36,   4,  20, BOTH, FALSE},   // 5G, ch 52~64
            { 52,   4,  20, BOTH, FALSE},   // 5G, ch 52~64
            { 100, 11,  20, BOTH, FALSE},   // 5G, ch 100~140
            { 149,  5,  20, BOTH, FALSE},   // 5G, ch 149~165
        },
        REG_DOMAIN_FCC
    },
};

const wifi_country_code_ext_t* WIFI_HAL_FindCountryCode( const char* pcCountryCode )
{
    const wifi_country_code_ext_t* cc;
    uint8_t i;

    for (i = 0; i < sizeof(wifiCountryCode_)/sizeof(wifi_country_code_ext_t); i++)
    {
        cc = &wifiCountryCode_[i];
        if (!strncmp(pcCountryCode, (char*)cc->country_code, 4))
        {
            return cc;
        }
    }
    return NULL;
}


