/**
 * \file
 * \brief PKCS11 Library Slot Handling
 *
 * The nomenclature here can lead to some confusion - the pkcs11 slot is
 * not the same as a device slot. So for example each slot defined here is a
 * specific device (most systems would have only one). The "slots" as defined
 * by the device specification would be enumerated seperately as related to
 * specific supported mechanisms as cryptographic "objects".
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

#include "cryptoauthlib.h"
#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_init.h"
#include "pkcs11_slot.h"
#include "pkcs11_info.h"
#include "pkcs11_util.h"
#include "pkcs11_object.h"
#include "pkcs11_os.h"

#include <stdio.h>

/**
 * \defgroup pkcs11 Slot Management (pkcs11_)
   @{ */

#ifdef ATCA_NO_HEAP
static pkcs11_slot_ctx pkcs11_slot_cache[PKCS11_MAX_SLOTS_ALLOWED];
#endif

/**
 * \brief Retrieve the current slot context
 */
pkcs11_slot_ctx_ptr pkcs11_slot_get_context(pkcs11_lib_ctx_ptr lib_ctx, CK_SLOT_ID slotID)
{
    pkcs11_slot_ctx_ptr rv = NULL_PTR;

    if (lib_ctx)
    {
        if (lib_ctx->slots)
        {
            if (slotID < lib_ctx->slot_cnt)
            {
                rv = &((pkcs11_slot_ctx_ptr)lib_ctx->slots)[slotID];
            }
        }
    }
    return rv;
}

CK_VOID_PTR pkcs11_slot_initslots(CK_ULONG pulCount)
{
#ifdef ATCA_NO_HEAP
    int i;
    for (i = 0; i < PKCS11_MAX_SLOTS_ALLOWED; i++)
    {
        memset(&pkcs11_slot_cache[i], 0, sizeof(pkcs11_slot_ctx));
    }

    return &pkcs11_slot_cache;
#else
    pkcs11_slot_ctx_ptr slot_ctx_array = pkcs11_os_malloc(sizeof(pkcs11_slot_ctx) * pulCount);
    if (slot_ctx_array)
    {
        memset(slot_ctx_array, 0, sizeof(pkcs11_slot_ctx) * pulCount);
    }
    return slot_ctx_array;
#endif
}

CK_RV pkcs11_slot_config(CK_SLOT_ID slotID)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_slot_ctx_ptr slot_ctx;
    CK_RV rv;

    if (!lib_ctx)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    slot_ctx = pkcs11_slot_get_context(lib_ctx, slotID);

    if (!slot_ctx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    /* Set Defaults */
    slot_ctx->user_pin_handle = 0xFFFF;
    slot_ctx->so_pin_handle = 0xFFFF;

    /* Load the configuration */
    rv = pkcs11_config_load(slot_ctx);

    return rv;
}

#if 0
static ATCA_STATUS pkcs11_slot_check_device_type(ATCAIfaceCfg * ifacecfg)
{
    uint8_t info[4] = { 0 };
    ATCA_STATUS status = atcab_info(info);

    printf("pkcs11_slot_check_device_type: %d\n", ifacecfg->devtype);

    if (ATCA_SUCCESS == status)
    {
        ATCADeviceType devType = ATCA_DEV_UNKNOWN;

        if (0x50 == info[2])
        {
            devType = ATECC508A;
        }
        else if (0x60 == info[2])
        {
            devType = ATECC608;
        }

        if (ifacecfg->devtype != devType)
        {
            ifacecfg->devtype = devType;
            (void)atcab_release();
            atca_delay_ms(1);
            status = atcab_init(ifacecfg);
        }
    }

    printf("pkcs11_slot_check_device_type: %d\n", ifacecfg->devtype);

    return status;
}
#endif

CK_RV pkcs11_slot_init(CK_SLOT_ID slotID)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_slot_ctx_ptr slot_ctx;
    ATCA_STATUS status = CKR_OK;
    int retries;

    if (!lib_ctx)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    slot_ctx = pkcs11_slot_get_context(lib_ctx, slotID);

    if (!slot_ctx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    if (!slot_ctx->initialized)
    {
        ATCAIfaceCfg * ifacecfg = &slot_ctx->interface_config;

        retries = 2;
        do
        {
            /* If a PKCS11 was killed an left the device in the idle state then
               starting up again will require the device to go back to a known state
               that is accomplished here by retrying the initalization */
            status = atcab_init(ifacecfg);
        }
        while (retries-- && status);

    #ifdef ATCA_HAL_I2C
        if (ATCA_SUCCESS != status)
        {
            if (0xC0 != ifacecfg->atcai2c.address)
            {
                /* Try the default address */
                ifacecfg->atcai2c.address = 0xC0;
                atcab_release();
                atca_delay_ms(1);
                retries = 2;
                do
                {
                    /* Same as the above */
                    status = atcab_init(ifacecfg);
                }
                while (retries-- && status);
            }
        }
    #endif

    #if PKCS11_508_SUPPORT && PKCS11_608_SUPPORT
        /* If both are supported check the device to verify */
        if (ATCA_SUCCESS == status)
        {
            status = pkcs11_slot_check_device_type(ifacecfg);
        }
    #endif

        if (ATCA_SUCCESS == status)
        {
            if (atcab_is_ca_device(ifacecfg->devtype))
            {
                /* Only the classic cryptoauth devices require the configuration
                   to be loaded into memory */
                status = atcab_read_config_zone((uint8_t*)&slot_ctx->cfg_zone);
            }
            else
            {
#if ATCA_TA_SUPPORT
                /* Iterate through all objects and attach handle info */
                status = pkcs11_object_load_handle_info(lib_ctx);
#endif
            }
        }

        if (ATCA_SUCCESS == status)
        {
            slot_ctx->slot_id = slotID;
            slot_ctx->initialized = TRUE;
        }
    }

    return (ATCA_SUCCESS == status) ? CKR_OK : CKR_DEVICE_ERROR;
}

static CK_ULONG pkcs11_slot_get_active_count(pkcs11_lib_ctx_ptr lib_ctx)
{
    CK_ULONG active_cnt = 0;
    CK_ULONG i;

    for (i = 0; i < lib_ctx->slot_cnt; i++)
    {
        if (((pkcs11_slot_ctx_ptr*)lib_ctx->slots)[i])
        {
            active_cnt++;
        }
    }
    return active_cnt;
}

static void pkcs11_slot_fill_list(pkcs11_lib_ctx_ptr lib_ctx, CK_BBOOL tokenPresent, CK_SLOT_ID_PTR pSlotList)
{
    CK_ULONG i;
    CK_ULONG j = 0;

    for (i = 0; i < lib_ctx->slot_cnt; i++)
    {
        if (tokenPresent)
        {
            if (((pkcs11_slot_ctx_ptr*)lib_ctx->slots)[i])
            {
                pSlotList[j++] = i;
            }
        }
        else
        {
            pSlotList[j++] = i;
        }
    }
}

CK_RV pkcs11_slot_get_list(CK_BBOOL tokenPresent, CK_SLOT_ID_PTR pSlotList, CK_ULONG_PTR pulCount)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    CK_ULONG slot_cnt = 0;

    if (!lib_ctx)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    /* Ref PKCS11 Sec 5.5 - C_GetSlotList */
    if (!pulCount)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (tokenPresent)
    {
        /* Get the list of installed devices */
        slot_cnt = pkcs11_slot_get_active_count(lib_ctx);
    }
    else
    {
        slot_cnt = lib_ctx->slot_cnt;
    }

    /* Ref PKCS11 Sec 5.5 - C_GetSlotList Requirement #2 */
    if (pSlotList)
    {
        if (slot_cnt > *pulCount)
        {
            *pulCount = slot_cnt;
            return CKR_BUFFER_TOO_SMALL;
        }
        else
        {
            pkcs11_slot_fill_list(lib_ctx, tokenPresent, pSlotList);
        }
    }

    *pulCount = slot_cnt;

    return CKR_OK;
}

/**
 * \brief Obtains information about a particular slot
 */
CK_RV pkcs11_slot_get_info(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_slot_ctx_ptr slot_ctx;
    ATCAIfaceCfg *if_cfg_ptr;
    CK_UTF8CHAR buf[8];

    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!pInfo)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* Initialize the input structure */
    memset(pInfo, 0, sizeof(CK_SLOT_INFO));

    /* Retreive the slot context - i.e. the attached device (ECC508A, SHA256, etc) */
    slot_ctx = pkcs11_slot_get_context(lib_ctx, slotID);

    if (!slot_ctx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    /* Default version information */
    pInfo->hardwareVersion.major = (CK_BYTE)CK_UNAVAILABLE_INFORMATION;
    pInfo->hardwareVersion.minor = (CK_BYTE)CK_UNAVAILABLE_INFORMATION;
    pInfo->firmwareVersion.major = (CK_BYTE)CK_UNAVAILABLE_INFORMATION;
    pInfo->firmwareVersion.minor = (CK_BYTE)CK_UNAVAILABLE_INFORMATION;

    /* Get the reference to the slot configuration structure that was used */
    if_cfg_ptr = &slot_ctx->interface_config;

    /* Set up the flags - Always a hardware slot, only removable devices can
       be listed as not present */
    pInfo->flags = CKF_HW_SLOT | CKF_TOKEN_PRESENT;

    /* So there a number of rules about what a slot can and can not be - this
       means there needs to be a fixed configuration somewhere or a scan
       operation when we intialize */

    /* if the interface is USB then it's removable otherwise assume it's not - This might require a
        configuration API for cases where the device is used for consumable authentication */
    if (ATCA_UART_IFACE == if_cfg_ptr->iface_type || ATCA_HID_IFACE == if_cfg_ptr->iface_type)
    {
        pInfo->flags |= CKF_REMOVABLE_DEVICE;
        if (!slot_ctx->initialized)
        {
            pInfo->flags &= ~CKF_TOKEN_PRESENT;
        }
    }

    /* Basic description of the expected interface based on the configuration */
    snprintf((char*)pInfo->slotDescription, sizeof(pInfo->slotDescription), "%d_%d_%d", (int)slotID,
             (int)if_cfg_ptr->devtype, (int)if_cfg_ptr->iface_type);

    if (slot_ctx->initialized)
    {
        (void)pkcs11_lock_context(lib_ctx);

        if (!atcab_info(buf))
        {
            /* SHA204 = 00 02 00 09, ECC508 = 00 00 50 00, AES132 = 0A 07*/
            pInfo->hardwareVersion.major = 0;
            pInfo->hardwareVersion.minor = buf[3];
        }

        (void)pkcs11_unlock_context(lib_ctx);
    }

    /* Use the same manufacturer ID we use throughout */
    snprintf((char*)pInfo->manufacturerID, sizeof(pInfo->manufacturerID), "%s", pkcs11_lib_manufacturer_id);

    /* Make sure strings are escaped properly */
    pkcs11_util_escape_string(pInfo->manufacturerID, sizeof(pInfo->manufacturerID));
    pkcs11_util_escape_string(pInfo->slotDescription, sizeof(pInfo->slotDescription));

    return CKR_OK;
}

/** @} */
