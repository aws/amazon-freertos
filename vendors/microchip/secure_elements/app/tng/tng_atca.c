/**
 * \file
 * \brief TNG Helper Functions
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include <string.h>
#include "cryptoauthlib.h"
#include "tng_atca.h"
#include "tnglora_cert_def_2_device.h"
#include "tnglora_cert_def_4_device.h"
#include "tngtls_cert_def_2_device.h"
#include "tngtls_cert_def_3_device.h"
#include "tflxtls_cert_def_4_device.h"
#include "atcacert/atcacert_def.h"


typedef struct
{
    const char*           otpcode;
    const atcacert_def_t* cert_def;
} tng_cert_map_element;

static tng_cert_map_element g_tng_cert_def_map[] =
{
#ifdef ATCA_TNG_LEGACY_SUPPORT
    { "wdNxAjae", &g_tngtls_cert_def_2_device          },
    { "Rsuy5YJh", &g_tngtls_cert_def_2_device          },
    { "BxZvm6q2", &g_tnglora_cert_def_2_device         },
#endif
#ifdef ATCA_TFLEX_SUPPORT
    { "MKMwyhP1", &g_tflxtls_cert_def_4_device         },
#endif
#ifdef ATCA_TNGTLS_SUPPORT
    { "KQp2ZkD8", &g_tngtls_cert_def_3_device          },
    { "x6tjuZMy", &g_tngtls_cert_def_3_device          },
#endif
#ifdef ATCA_TNGLORA_SUPPORT
    { "jsMu7iYO", &g_tnglora_cert_def_4_device         },
    { "09qJNxI3", &g_tnglora_cert_def_4_device         },
#endif
    { "",         NULL                                 }
};

static const size_t g_tng_cert_def_cnt = sizeof(g_tng_cert_def_map) / sizeof(tng_cert_map_element) - 1;

const atcacert_def_t* tng_map_get_device_cert_def(int index)
{
    if (index < (int)g_tng_cert_def_cnt)
    {
        return g_tng_cert_def_map[index].cert_def;
    }
    else
    {
        return NULL;
    }
}

ATCA_STATUS tng_get_device_cert_def(const atcacert_def_t **cert_def)
{
    ATCA_STATUS status;
    char otpcode[32];
    uint8_t i;

    if (cert_def == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    status = atcab_read_zone(ATCA_ZONE_OTP, 0, 0, 0, (uint8_t*)otpcode, 32);
    if (ATCA_SUCCESS == status)
    {
        for (i = 0; i < g_tng_cert_def_cnt; i++)
        {
            if (0 == strncmp(g_tng_cert_def_map[i].otpcode, otpcode, 8))
            {
                *cert_def = g_tng_cert_def_map[i].cert_def;
                break;
            }
        }

        if (i == g_tng_cert_def_cnt)
        {
            status = ATCACERT_E_WRONG_CERT_DEF;
        }
    }

    return status;
}

ATCA_STATUS tng_get_device_pubkey(uint8_t *public_key)
{
    ATCA_STATUS status;
    const atcacert_def_t* cert_def = NULL;

    status = tng_get_device_cert_def(&cert_def);

    if (ATCA_SUCCESS == status)
    {
        status = atcab_get_pubkey(cert_def->private_key_slot, public_key);
    }

    return status;
}
