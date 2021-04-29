/**
 * \file
 * \brief PKCS11 Library Token Handling
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
#include "pkcs11_token.h"
#include "pkcs11_slot.h"
#include "pkcs11_info.h"
#include "pkcs11_util.h"
#include "pkcs11_object.h"
#include "pkcs11_key.h"
#include "pkcs11_cert.h"
#include "pkcs11_session.h"


#include <stdio.h>

/**
 * \defgroup pkcs11 Token Management (pkcs11_)
   @{ */

#ifdef ATCA_ATECC508A_SUPPORT
#if !PKCS11_USE_STATIC_CONFIG
/** Standard Configuration Structure for ATECC508A devices */
static const uint8_t atecc508_config[] = {
    0x01, 0x23, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x00, 0x01, 0x00,
    0xC0, 0x00, 0x55, 0x00, 0x8F, 0x20, 0xC4, 0x44, 0x87, 0x20, 0x87, 0x20, 0x8F, 0x0F, 0xC4, 0x36,
    0x9F, 0x0F, 0x82, 0x20, 0x0F, 0x0F, 0xC4, 0x44, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x33, 0x00, 0x1C, 0x00, 0x13, 0x00, 0x13, 0x00, 0x7C, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x33, 0x00,
    0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00
};
#else
extern const uint8_t atecc508_config[];
#endif
#endif

#ifdef ATCA_ATECC608_SUPPORT
#if !PKCS11_USE_STATIC_CONFIG
/** Standard Configuration Structure for ATECC608 devices */
static const uint8_t atecc608_config[] = {
    0x01, 0x23, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x01, 0x01, 0x00,
    0xC0, 0x00, 0x00, 0x01, 0x8F, 0x20, 0xC4, 0x44, 0x87, 0x20, 0x87, 0x20, 0x8F, 0x0F, 0xC4, 0x36,
    0x9F, 0x0F, 0x82, 0x20, 0x0F, 0x0F, 0xC4, 0x44, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x33, 0x00, 0x1C, 0x00, 0x13, 0x00, 0x13, 0x00, 0x7C, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x33, 0x00,
    0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00,
};
#else
extern const uint8_t atecc608_config[];
#endif
#endif

/** Get the device model number from the info word */
static char * pkcs11_token_device(ATCADeviceType dev_type, uint8_t info[4])
{
    char * rv = "unknown";

    if (atcab_is_ca_device(dev_type))
    {
        switch (info[2])
        {
        case 0x00:
            if (0x02 == info[1])
            {
                rv = "ATSHA204A";
            }
            break;
        case 0x50:
            rv = "ATECC508A";
            break;
        case 0x60:
            if (0x02 < info[1])
            {
                rv = "ATECC608B";
            }
            else
            {
                rv = "ATECC608A";
            }
            break;
        default:
            break;
        }
    }
    else if (atcab_is_ta_device(dev_type))
    {
        rv = "TA100";
    }
    return rv;
}

/** Write the configuration into the device and generate new keys */

CK_RV pkcs11_token_init(CK_SLOT_ID slotID, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen, CK_UTF8CHAR_PTR pLabel)
{
#if PKCS11_TOKEN_INIT_SUPPORT
    CK_RV rv;
    uint8_t buf[32] = {0};
    uint8_t * pConfig = NULL;
    bool lock = false;
    pkcs11_lib_ctx_ptr pLibCtx;
    pkcs11_slot_ctx_ptr pSlotCtx;

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    /* Get the slot context */
    pSlotCtx = pkcs11_slot_get_context(pLibCtx, slotID);
    if (!pSlotCtx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    /* Lock the library */
    rv = pkcs11_lock_context(pLibCtx);

    if (CKR_OK == rv)
    {
        /* Check the config zone lock status */
        rv = atcab_is_locked(LOCK_ZONE_CONFIG, &lock);
    }

    if (atcab_is_ca_device(pSlotCtx->interface_config.devtype))
    {
        if (ATCA_SUCCESS == rv)
        {
            /* Get the device type */
            rv = atcab_info(buf);
        }

        switch (buf[2])
        {
    #ifdef ATCA_ATECC508A_SUPPORT
        case 0x50:
            pConfig = (uint8_t*)atecc508_config;
            break;
    #endif
    #ifdef ATCA_ATECC608_SUPPORT
        case 0x60:
            pConfig = (uint8_t*)atecc608_config;
            break;
    #endif
        default:
            rv = CKR_TOKEN_NOT_RECOGNIZED;
            break;
        }
    }

    /* Program the configuration zone */
    if (!lock && ATCA_SUCCESS == rv)
    {
        if (atcab_is_ca_device(pSlotCtx->interface_config.devtype))
        {
#ifdef ATCA_I2C_ECC_ADDRESS
#if (ATCA_I2C_ECC_ADDRESS == 0) || (ATCA_I2C_ECC_ADDRESS & 0x01) || (ATCA_I2C_ECC_ADDRESS > 0xFE)
#error "Sanity Check Failure: ATCA_I2C_ECC_ADDRESS is invalid for the device"
#endif

            buf[0] = ATCA_I2C_ECC_ADDRESS;
#endif

            if (ATCA_SUCCESS == rv)
            {
                rv = atcab_write_config_zone(pConfig);
            }
        }

        if (ATCA_SUCCESS == rv)
        {
            rv = atcab_lock_config_zone();
        }
    }

    if (ATCA_SUCCESS == rv)
    {
        /* Check data zone lock */
        rv = atcab_is_locked(LOCK_ZONE_DATA, &lock);
    }


    if (!lock && ATCA_SUCCESS == rv)
    {
        size_t buflen = sizeof(buf);

        if (atcab_is_ca_device(pSlotCtx->interface_config.devtype))
        {
            /* Generate New Keys */
            for (int i = 0; (i < 16) && (ATCA_SUCCESS == rv); i++)
            {
                if (ATCA_KEY_CONFIG_PRIVATE_MASK & ((atecc608_config_t*)pConfig)->KeyConfig[i])
                {
                    rv = atcab_genkey(i, NULL);
                }
            }
        }
        else
        {
#if ATCA_TA_SUPPORT
            const ta_element_attributes_t attr_ecc_private_deletable = { 1, 0x1700, 0, 0, 0, 0x41, 0 };
            rv = talib_create_element_with_handle(atcab_get_device(), 0x8102, &attr_ecc_private_deletable);
            if (!rv)
            {
                rv = talib_genkey_compat(atcab_get_device(), 0x8102, NULL);
            }
#endif
        }

        if (ulPinLen)
        {
            if (64 != ulPinLen)
            {
                    rv = pkcs11_util_convert_rv(atcab_read_serial_number(buf));

                if (CKR_OK == rv)
                {
                    rv = pkcs11_token_convert_pin_to_key(pPin, ulPinLen, buf, ATCA_SERIAL_NUM_SIZE,
                                                         buf, buflen);
                }
            }
            else
            {
                rv = pkcs11_token_convert_pin_to_key(pPin, ulPinLen, NULL, 0, buf, buflen);
            }

            if ((CKR_OK == rv) && (pSlotCtx->so_pin_handle != 0xFFFF))
            {
                if (atcab_is_ca_device(pSlotCtx->interface_config.devtype))
                {
                    /* Write the default pin */
                    if (CKR_OK == rv)
                    {
                        rv = atcab_write_zone(ATCA_ZONE_DATA, pSlotCtx->so_pin_handle, 0, 0, buf, buflen);
                    }
                }
            }
        }

        /* Lock the data zone */
        if (ATCA_SUCCESS == rv)
        {
            rv = atcab_lock_data_zone();
        }
    }

    /* If the I2C address changed it'll have to be put back to sleep before it'll
       change */
    (void)atcab_release();

    /* Release the lock on the library */
    (void)pkcs11_unlock_context(pLibCtx);

    /* Trigger a reinitialization of the slot */
    if (ATCA_SUCCESS == rv)
    {
        pSlotCtx->initialized = FALSE;
        rv = pkcs11_slot_init(0);
    }

    if (ATCA_SUCCESS != rv)
    {
        return CKR_FUNCTION_FAILED;
    }
    else
    {
        return CKR_OK;
    }
#else
    return CKR_FUNCTION_NOT_SUPPORTED;
#endif
}

CK_RV pkcs11_token_get_access_type(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        if (atcab_is_ca_device(atcab_get_device_type()))
        {
            atecc508a_config_t * pConfig = (atecc508a_config_t*)obj_ptr->config;

            if (ATCA_KEY_CONFIG_PRIVATE_MASK & pConfig->KeyConfig[obj_ptr->slot])
            {
                return pkcs11_attrib_true(pObject, pAttribute);
            }
            else
            {
                return pkcs11_attrib_false(pObject, pAttribute);
            }
        }
        else
        {
#if ATCA_TA_SUPPORT
            if (TA_PERM_READ(1) == (obj_ptr->handle_info.permission & TA_PERM_READ_MASK))
            {
                return pkcs11_attrib_false(pObject, pAttribute);
            }
            else
            {
                return pkcs11_attrib_true(pObject, pAttribute);
            }
#endif
        }

    }

    return CKR_ARGUMENTS_BAD;
}

CK_RV pkcs11_token_get_writable(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        atecc508a_config_t * pConfig = (atecc508a_config_t*)obj_ptr->config;

        if ((ATCA_KEY_CONFIG_PRIVATE_MASK & pConfig->KeyConfig[obj_ptr->slot]) ||
            (ATCA_SLOT_CONFIG_IS_SECRET_MASK & pConfig->SlotConfig[obj_ptr->slot]))
        {
            return pkcs11_attrib_false(pObject, pAttribute);
        }
        else
        {
            return pkcs11_attrib_true(pObject, pAttribute);
        }
    }

    return CKR_ARGUMENTS_BAD;
}

CK_RV pkcs11_token_get_storage(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr->slot == 0xFFFF)
    {
        return pkcs11_attrib_false(pObject, pAttribute);
    }
    else
    {
        return pkcs11_attrib_true(pObject, pAttribute);
    }
}

/**
 * \brief Obtains information about a particular token
 */
CK_RV pkcs11_token_get_info(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_slot_ctx_ptr slot_ctx;
    CK_UTF8CHAR buf[16];
    bool lock = FALSE;

    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!pInfo)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* Initialize the target structure */
    memset(pInfo, 0, sizeof(CK_TOKEN_INFO));

    /* Retrieve the slot context - i.e. the attached device (ECC508A, SHA256, etc) */
    slot_ctx = pkcs11_slot_get_context(lib_ctx, slotID);

    if (!slot_ctx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    /* Default Info Fields */
    pInfo->hardwareVersion.major = (CK_BYTE)CK_UNAVAILABLE_INFORMATION;
    pInfo->hardwareVersion.minor = (CK_BYTE)CK_UNAVAILABLE_INFORMATION;
    pInfo->firmwareVersion.major = (CK_BYTE)CK_UNAVAILABLE_INFORMATION;
    pInfo->firmwareVersion.minor = (CK_BYTE)CK_UNAVAILABLE_INFORMATION;
    pInfo->ulTotalPublicMemory = CK_UNAVAILABLE_INFORMATION;
    pInfo->ulFreePublicMemory = CK_UNAVAILABLE_INFORMATION;
    pInfo->ulTotalPrivateMemory = CK_UNAVAILABLE_INFORMATION;
    pInfo->ulFreePrivateMemory = CK_UNAVAILABLE_INFORMATION;
    pInfo->ulMaxPinLen = 128;
    pInfo->ulMinPinLen = 0;
    pInfo->flags = CKF_RNG;// | CKF_LOGIN_REQUIRED;

    pInfo->ulMaxSessionCount = 1;
    pInfo->ulMaxRwSessionCount = 1;

    pInfo->ulSessionCount = (slot_ctx->session) ? TRUE : FALSE;
    pInfo->ulRwSessionCount = (slot_ctx->session) ? TRUE : FALSE;

    PKCS11_DEBUG("Token Info: %d\r\n", slot_ctx->initialized);

    if (slot_ctx->initialized)
    {
        (void)pkcs11_lock_context(lib_ctx);

        /* Read the serial number */
        if (!atcab_read_serial_number(buf))
        {
#ifdef PKCS11_LABEL_IS_SERNUM
            size_t len = sizeof(pInfo->label);
            (void)atcab_bin2hex_(buf, 9, (char*)pInfo->label, &len, FALSE, FALSE, TRUE);
            memcpy(pInfo->serialNumber, &pInfo->label[2], sizeof(pInfo->serialNumber));
#else
            size_t len = sizeof(pInfo->serialNumber);
            (void)atcab_bin2hex_(&buf[1], 8, (char*)pInfo->serialNumber, &len, FALSE, FALSE, TRUE);
#endif
        }

#ifndef PKCS11_LABEL_IS_SERNUM
        memcpy(pInfo->label, slot_ctx->label, strlen(slot_ctx->label));
#endif

        /* Read the hardware revision data */
        if (!atcab_info(buf))
        {
            /* SHA204 = 00 02 00 09, ECC508 = 00 00 50 00, AES132 = 0A 07*/
            pInfo->hardwareVersion.major = 0;
            pInfo->hardwareVersion.minor = buf[3];
            snprintf((char*)pInfo->model, sizeof(pInfo->model), "%s", pkcs11_token_device(slot_ctx->interface_config.devtype, buf));
        }

        /* Check if the device locks are set */
        if (ATCA_SUCCESS == atcab_is_locked(LOCK_ZONE_DATA, &lock))
        {
            if (lock)
            {
                pInfo->flags |= CKF_TOKEN_INITIALIZED;
                PKCS11_DEBUG("Token Locked\r\n");
            }
        }

        /* Check if the device locks are set */
        if (slot_ctx->user_pin_handle != 0xFFFF)
        {
            if (ATCA_SUCCESS == atcab_is_slot_locked(slot_ctx->user_pin_handle, &lock))
            {
                if (lock)
                {
                    pInfo->flags |= CKF_USER_PIN_INITIALIZED;
                    PKCS11_DEBUG("Pin Slot Locked\r\n");
                }
            }
        }

        if (slot_ctx->so_pin_handle != 0xFFFF)
        {
            if (ATCA_SUCCESS == atcab_is_slot_locked(slot_ctx->so_pin_handle, &lock))
            {
                if (lock)
                {
                    pInfo->flags |= CKF_SO_PIN_LOCKED;
                    PKCS11_DEBUG("Pin Slot Locked\r\n");
                }
            }
        }

        (void)pkcs11_unlock_context(lib_ctx);
    }

    /* Use the same manufacturer ID we use throughout */
    snprintf((char*)pInfo->manufacturerID, sizeof(pInfo->manufacturerID), "%s", pkcs11_lib_manufacturer_id);

    /* Make sure strings are escaped properly */
    pkcs11_util_escape_string(pInfo->label, sizeof(pInfo->label));
    pkcs11_util_escape_string(pInfo->manufacturerID, sizeof(pInfo->manufacturerID));
    pkcs11_util_escape_string(pInfo->model, sizeof(pInfo->model));
    pkcs11_util_escape_string(pInfo->serialNumber, sizeof(pInfo->serialNumber));

    return CKR_OK;
}

/**
 * \brief Generate the specified amount of random data
 */
CK_RV pkcs11_token_random(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pRandomData, CK_ULONG ulRandomLen)
{
    pkcs11_session_ctx_ptr pSession;
    pkcs11_lib_ctx_ptr lib_ctx;
    ATCA_STATUS status;
    uint8_t buf[32];
    CK_RV rv;

    rv = pkcs11_init_check(&lib_ctx, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pRandomData || !ulRandomLen)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    do
    {
        (void)pkcs11_lock_context(lib_ctx);

        status = atcab_random(buf);

        (void)pkcs11_unlock_context(lib_ctx);

        if (status)
        {
            return CKR_DEVICE_ERROR;
        }

        if (32 < ulRandomLen)
        {
            memcpy(pRandomData, buf, 32);
            pRandomData += 32;
            ulRandomLen -= 32;
        }
        else
        {
            memcpy(pRandomData, buf, ulRandomLen);
            ulRandomLen = 0;
        }
    }
    while (ulRandomLen);

    return CKR_OK;
}

CK_RV pkcs11_token_convert_pin_to_key(
    const CK_UTF8CHAR_PTR pPin,
    const CK_ULONG        ulPinLen,
    const CK_UTF8CHAR_PTR pSalt,
    const CK_ULONG        ulSaltLen,
    CK_BYTE_PTR           pKey,
    CK_ULONG              ulKeyLen
    )
{
    ATCA_STATUS status = ATCA_SUCCESS;

    if (!pPin || !ulPinLen || !pKey || 32 > ulKeyLen)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (64 != ulPinLen)
    {
        atcac_sha2_256_ctx ctx;
        status = atcac_sw_sha2_256_init(&ctx);

        if (ATCA_SUCCESS == status)
        {
            status = atcac_sw_sha2_256_update(&ctx, pPin, ulPinLen);
        }

        if (ATCA_SUCCESS == status)
        {
            status = atcac_sw_sha2_256_update(&ctx, pSalt, ulSaltLen);
        }

        if (ATCA_SUCCESS == status)
        {
            status = atcac_sw_sha2_256_finish(&ctx, pKey);
        }
    }
    else
    {
        size_t out_len = ulKeyLen;
        status = atcab_hex2bin((char*)pPin, ulPinLen, pKey, &out_len);
        ulKeyLen = (CK_ULONG)out_len;
    }

    return pkcs11_util_convert_rv(status);
}

#include <stdio.h>
CK_RV pkcs11_token_set_pin(CK_SESSION_HANDLE hSession, CK_UTF8CHAR_PTR pOldPin,
                           CK_ULONG ulOldLen, CK_UTF8CHAR_PTR pNewPin, CK_ULONG ulNewLen)
{
    pkcs11_session_ctx_ptr pSession;
    pkcs11_lib_ctx_ptr pLibCtx;
    uint16_t pin_slot;
    uint8_t buf[32];
    CK_RV rv;

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pNewPin || !ulNewLen)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    if (64 != ulNewLen)
    {
        if (CKR_OK == (rv = pkcs11_lock_context(pLibCtx)))
        {
            rv = pkcs11_util_convert_rv(atcab_read_serial_number(buf));
            (void)pkcs11_unlock_context(pLibCtx);
        }

        if (CKR_OK == rv)
        {
            rv = pkcs11_token_convert_pin_to_key(pNewPin, ulNewLen, buf, ATCA_SERIAL_NUM_SIZE,
                                                 buf, (CK_LONG)sizeof(buf));
        }
    }
    else
    {
        rv = pkcs11_token_convert_pin_to_key(pNewPin, ulNewLen, NULL, 0, buf, (CK_LONG)sizeof(buf));
    }

    pin_slot = (ATCA_CHIP_OPT_IO_PROT_KEY_MASK & pSession->slot->cfg_zone.ChipOptions) >> ATCA_CHIP_OPT_IO_PROT_KEY_SHIFT;

    if (CKR_OK == rv)
    {
        rv = atcab_write_zone(ATCA_ZONE_DATA, pin_slot, 0, 0, buf, sizeof(buf));
    }

    /* Lock the pin once it has been written */
#if PKCS11_LOCK_PIN_SLOT
    if (CKR_OK == rv)
    {
        rv = atcab_lock_data_slot(pin_slot);
    }
#endif

    return rv;
}

/** @} */
