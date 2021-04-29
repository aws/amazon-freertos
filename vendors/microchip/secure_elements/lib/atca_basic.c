/**
 * \file
 * \brief CryptoAuthLib Basic API methods. These methods provide a simpler way
 *        to access the core crypto methods.
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

#include "atca_basic.h"
#include "atca_version.h"

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
#if defined(_MSC_VER)
#pragma message("Warning : Using a constant host nonce with atcab_read_enc, atcab_write_enc, etcc., can allow spoofing of a device by replaying previously recorded messages")
#else
#warning "Using a constant host nonce with atcab_read_enc, atcab_write_enc, etcc., can allow spoofing of a device by replaying previously recorded messages"
#endif
#endif

const char atca_version[] = ATCA_LIBRARY_VERSION_DATE;
ATCADevice _gDevice = NULL;
#ifdef ATCA_NO_HEAP
SHARED_LIB_EXPORT struct atca_iface g_atcab_iface;
SHARED_LIB_EXPORT struct atca_device g_atcab_device;
#endif

/** \brief basic API methods are all prefixed with atcab_  (CryptoAuthLib Basic)
 *  the fundamental premise of the basic API is it is based on a single interface
 *  instance and that instance is global, so all basic API commands assume that
 *  one global device is the one to operate on.
 */

/** \brief returns a version string for the CryptoAuthLib release.
 *  The format of the version string returned is "yyyymmdd"
 * \param[out] ver_str ptr to space to receive version string
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_version(char *ver_str)
{
    strcpy(ver_str, atca_version);
    return ATCA_SUCCESS;
}


/** \brief Creates and initializes a ATCADevice context
 *  \param[out] device Pointer to the device context pointer
 *  \param[in]  cfg    Logical interface configuration. Some predefined
 *                     configurations can be found in atca_cfgs.h
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_init_ext(ATCADevice* device, ATCAIfaceCfg *cfg)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;

    if (device)
    {
        // If a device has already been initialized, release it
        if (*device)
        {
            atcab_release_ext(device);
        }

#ifdef ATCA_NO_HEAP
        g_atcab_device.mIface = g_atcab_iface;
        status = initATCADevice(cfg, &g_atcab_device);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
        *device = &g_atcab_device;
#else
        *device = newATCADevice(cfg);
        if (*device == NULL)
        {
            return ATCA_GEN_FAIL;
        }
#endif

#ifdef ATCA_ATECC608_SUPPORT
        if (cfg->devtype == ATECC608)
        {
            if ((status = calib_read_bytes_zone(*device, ATCA_ZONE_CONFIG, 0, ATCA_CHIPMODE_OFFSET, &(*device)->clock_divider, 1)) != ATCA_SUCCESS)
            {
                return status;
            }
            (*device)->clock_divider &= ATCA_CHIPMODE_CLOCK_DIV_MASK;
        }
#endif
    }

    return ATCA_SUCCESS;
}

/** \brief Creates a global ATCADevice object used by Basic API.
 *  \param[in] cfg  Logical interface configuration. Some predefined
 *                  configurations can be found in atca_cfgs.h
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_init(ATCAIfaceCfg* cfg)
{
    return atcab_init_ext(&_gDevice, cfg);
}

/** \brief Initialize the global ATCADevice object to point to one of your
 *         choosing for use with all the atcab_ basic API.
 *
 * \deprecated This function is not recommended for use generally. Use of _ext
 * is recommended instead. You can use atcab_init_ext to obtain an initialized
 * instance and associated it with the global structure - but this shouldn't be
 * a required process except in extremely unusual circumstances.
 *
 *  \param[in] ca_device  ATCADevice instance to use as the global Basic API
 *                        crypto device instance
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_init_device(ATCADevice ca_device)
{
    if (ca_device == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // if there's already a device created, release it
    if (_gDevice)
    {
        atcab_release();
    }

    _gDevice = ca_device;

    return ATCA_SUCCESS;
}

/** \brief release (free) the an ATCADevice instance.
 *  \param[in]  device      Pointer to the device context pointer
 *  \return Returns ATCA_SUCCESS .
 */
ATCA_STATUS atcab_release_ext(ATCADevice* device)
{
#ifdef ATCA_NO_HEAP
    ATCA_STATUS status = releaseATCADevice(*device);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }
    *device = NULL;
#else
    deleteATCADevice(device);
#endif
    return ATCA_SUCCESS;
}

/** \brief release (free) the global ATCADevice instance.
 *  This must be called in order to release or free up the interface.
 *  \return Returns ATCA_SUCCESS .
 */
ATCA_STATUS atcab_release(void)
{
    return atcab_release_ext(&_gDevice);
}

/** \brief Get the global device object.
 *  \return instance of global ATCADevice
 */
ATCADevice atcab_get_device(void)
{
    return _gDevice;
}

/** \brief Get the selected device type of rthe device context
 *
 *  \param[in]  device      Device context pointer
 *  \return Device type if basic api is initialized or ATCA_DEV_UNKNOWN.
 */
ATCADeviceType atcab_get_device_type_ext(ATCADevice device)
{
    ATCADeviceType ret = ATCA_DEV_UNKNOWN;

    if (device && device->mIface.mIfaceCFG)
    {
        ret = device->mIface.mIfaceCFG->devtype;
    }
    return ret;
}

/** \brief Get the current device type configured for the global ATCADevice
 *  \return Device type if basic api is initialized or ATCA_DEV_UNKNOWN.
 */
ATCADeviceType atcab_get_device_type(void)
{
    return atcab_get_device_type_ext(_gDevice);
}

/** \brief Get the current device address based on the configured device
 * and interface
 * \return the device address if applicable else 0xFF
 */
uint8_t atcab_get_device_address(ATCADevice device)
{
    if (device && device->mIface.mIfaceCFG)
    {
        switch (device->mIface.mIfaceCFG->iface_type)
        {
        case ATCA_I2C_IFACE:
            return device->mIface.mIfaceCFG->atcai2c.address;
        default:
            break;
        }
    }
    return 0xFF;
}


/** \brief Check whether the device is cryptoauth device
 *  \return True if device is cryptoauth device or False.
 */
bool atcab_is_ca_device(ATCADeviceType dev_type)
{
    return (dev_type < TA100) ? true : false;
}

/** \brief Check whether the device is Trust Anchor device
 *  \return True if device is Trust Anchor device or False.
 */
bool atcab_is_ta_device(ATCADeviceType dev_type)
{
    return (dev_type == TA100) ? true : false;
}

#if (ATCA_CA_SUPPORT && ATCA_TA_SUPPORT) || defined(ATCA_USE_ATCAB_FUNCTIONS) || defined(ATCA_ECC204_SUPPORT)

/** \brief wakeup the CryptoAuth device
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_wakeup(void)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_wakeup(_gDevice);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = ATCA_SUCCESS;
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief idle the CryptoAuth device
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_idle(void)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_idle(_gDevice);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = ATCA_SUCCESS;
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief invoke sleep on the CryptoAuth device
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sleep(void)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sleep(_gDevice);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = ATCA_SUCCESS;
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }

    return status;
}

/** \brief Gets the size of the specified zone in bytes.
 *
 * \param[in]  zone  Zone to get size information from. Config(0), OTP(1), or
 *                   Data(2) which requires a slot.
 * \param[in]  slot  If zone is Data(2), the slot to query for size.
 * \param[out] size  Zone size is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_get_zone_size(uint8_t zone, uint16_t slot, size_t* size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_get_zone_size(_gDevice, zone, slot,  size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_get_zone_size(_gDevice, zone, slot, size);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* AES commands */
/** \brief Compute the AES-128 encrypt, decrypt, or GFM calculation.
 *  \param[in]  mode     The mode for the AES command.
 *  \param[in]  key_id   Key location. Can either be a slot number or
 *                       ATCA_TEMPKEY_KEYID for TempKey.
 *  \param[in]  aes_in   Input data to the AES command (16 bytes).
 *  \param[out] aes_out  Output data from the AES command is returned here (16
 *                       bytes).
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes(uint8_t mode, uint16_t key_id, const uint8_t* aes_in, uint8_t* aes_out)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATECC608_SUPPORT)
        status = calib_aes(_gDevice, mode, key_id, aes_in, aes_out);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Perform an AES-128 encrypt operation with a key in the device.
 *
 * \param[in]  device      Device context pointer
 * \param[in]  key_id      Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]  key_block   Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[in]  plaintext   Input plaintext to be encrypted (16 bytes).
 * \param[out] ciphertext  Output ciphertext is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_encrypt_ext(ATCADevice device, uint16_t key_id, uint8_t key_block, const uint8_t* plaintext, uint8_t* ciphertext)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type_ext(device);

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATECC608_SUPPORT)
        status = calib_aes_encrypt(device, key_id, key_block, plaintext, ciphertext);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_aes_encrypt(device, key_id, key_block, plaintext, ciphertext);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Perform an AES-128 encrypt operation with a key in the device.
 *
 * \param[in]  key_id      Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]  key_block   Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[in]  plaintext   Input plaintext to be encrypted (16 bytes).
 * \param[out] ciphertext  Output ciphertext is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_encrypt(uint16_t key_id, uint8_t key_block, const uint8_t* plaintext, uint8_t* ciphertext)
{
    return atcab_aes_encrypt_ext(_gDevice, key_id, key_block, plaintext, ciphertext);
}

/** \brief Perform an AES-128 decrypt operation with a key in the device.
 *
 * \param[in]  device      Device context pointer
 * \param[in]  key_id      Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]  key_block   Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[in]  ciphertext  Input ciphertext to be decrypted (16 bytes).
 * \param[out] plaintext   Output plaintext is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_decrypt_ext(ATCADevice device, uint16_t key_id, uint8_t key_block, const uint8_t* ciphertext, uint8_t* plaintext)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type_ext(device);

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATECC608_SUPPORT)
        status = calib_aes_decrypt(device, key_id, key_block, ciphertext, plaintext);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_aes_decrypt(device, key_id, key_block, ciphertext, plaintext);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Perform an AES-128 decrypt operation with a key in the device.
 *
 * \param[in]   key_id     Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]   key_block  Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[in]  ciphertext  Input ciphertext to be decrypted (16 bytes).
 * \param[out] plaintext   Output plaintext is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_decrypt(uint16_t key_id, uint8_t key_block, const uint8_t* ciphertext, uint8_t* plaintext)
{
    return atcab_aes_decrypt_ext(_gDevice, key_id, key_block, ciphertext, plaintext);
}

/** \brief Perform a Galois Field Multiply (GFM) operation.
 *
 * \param[in]   h       First input value (16 bytes).
 * \param[in]   input   Second input value (16 bytes).
 * \param[out]  output  GFM result is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gfm(const uint8_t* h, const uint8_t* input, uint8_t* output)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATECC608_SUPPORT)
        status = calib_aes_gfm(_gDevice, h, input, output);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Initialize context for AES GCM operation with an existing IV, which
 *         is common when starting a decrypt operation.
 *
 * \param[in] ctx           AES GCM context to be initialized.
 * \param[in] key_id        Key location. Can either be a slot number or
 *                          ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block     Index of the 16-byte block to use within the key
 *                          location for the actual key.
 * \param[in] iv            Initialization vector.
 * \param[in] iv_size       Size of IV in bytes. Standard is 12 bytes.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_init(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, const uint8_t* iv, size_t iv_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_aes_gcm_init(_gDevice, ctx, key_id, key_block, iv, iv_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Initialize context for AES GCM operation with a IV composed of a
 *         random and optional fixed(free) field, which is common when
 *         starting an encrypt operation.
 *
 * \param[in]  ctx              AES CTR context to be initialized.
 * \param[in]  key_id           Key location. Can either be a slot number or
 *                              ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]  key_block        Index of the 16-byte block to use within the
 *                              key location for the actual key.
 * \param[in]  rand_size        Size of the random field in bytes. Minimum and
 *                              recommended size is 12 bytes. Max is 32 bytes.
 * \param[in]  free_field       Fixed data to include in the IV after the
 *                              random field. Can be NULL if not used.
 * \param[in]  free_field_size  Size of the free field in bytes.
 * \param[out] iv               Initialization vector is returned here. Its
 *                              size will be rand_size and free_field_size
 *                              combined.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_init_rand(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, size_t rand_size,
                                    const uint8_t* free_field, size_t free_field_size, uint8_t* iv)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_aes_gcm_init_rand(_gDevice, ctx, key_id, key_block, rand_size, free_field, free_field_size, iv);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Process Additional Authenticated Data (AAD) using GCM mode and a
 *         key within the ATECC608 device.
 *
 * This can be called multiple times. atcab_aes_gcm_init() or
 * atcab_aes_gcm_init_rand() should be called before the first use of this
 * function. When there is AAD to include, this should be called before
 * atcab_aes_gcm_encrypt_update() or atcab_aes_gcm_decrypt_update().
 *
 * \param[in] ctx       AES GCM context
 * \param[in] aad       Additional authenticated data to be added
 * \param[in] aad_size  Size of aad in bytes
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_aad_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* aad, uint32_t aad_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_aes_gcm_aad_update(_gDevice, ctx, aad, aad_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Encrypt data using GCM mode and a key within the ATECC608 device.
 *         atcab_aes_gcm_init() or atcab_aes_gcm_init_rand() should be called
 *         before the first use of this function.
 *
 * \param[in]  ctx             AES GCM context structure.
 * \param[in]  plaintext       Plaintext to be encrypted (16 bytes).
 * \param[in]  plaintext_size  Size of plaintext in bytes.
 * \param[out] ciphertext      Encrypted data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_encrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* plaintext, uint32_t plaintext_size, uint8_t* ciphertext)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_aes_gcm_encrypt_update(_gDevice, ctx, plaintext, plaintext_size, ciphertext);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Complete a GCM encrypt operation returning the authentication tag.
 *
 * \param[in]  ctx       AES GCM context structure.
 * \param[out] tag       Authentication tag is returned here.
 * \param[in]  tag_size  Tag size in bytes (12 to 16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_encrypt_finish(atca_aes_gcm_ctx_t* ctx, uint8_t* tag, size_t tag_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_aes_gcm_encrypt_finish(_gDevice, ctx, tag, tag_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Decrypt data using GCM mode and a key within the ATECC608 device.
 *         atcab_aes_gcm_init() or atcab_aes_gcm_init_rand() should be called
 *         before the first use of this function.
 *
 * \param[in]  ctx              AES GCM context structure.
 * \param[in]  ciphertext       Ciphertext to be decrypted.
 * \param[in]  ciphertext_size  Size of ciphertext in bytes.
 * \param[out] plaintext        Decrypted data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_decrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* ciphertext, uint32_t ciphertext_size, uint8_t* plaintext)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_aes_gcm_decrypt_update(_gDevice, ctx, ciphertext, ciphertext_size, plaintext);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Complete a GCM decrypt operation verifying the authentication tag.
 *
 * \param[in]  ctx          AES GCM context structure.
 * \param[in]  tag          Expected authentication tag.
 * \param[in]  tag_size     Size of tag in bytes (12 to 16 bytes).
 * \param[out] is_verified  Returns whether or not the tag verified.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_decrypt_finish(atca_aes_gcm_ctx_t* ctx, const uint8_t* tag, size_t tag_size, bool* is_verified)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_aes_gcm_decrypt_finish(_gDevice, ctx, tag, tag_size, is_verified);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* CheckMAC command */

/** \brief Compares a MAC response with input values
 *	\param[in] mode        Controls which fields within the device are used in
 *                         the message
 *	\param[in] key_id      Key location in the CryptoAuth device to use for the
 *                         MAC
 *	\param[in] challenge   Challenge data (32 bytes)
 *	\param[in] response    MAC response data (32 bytes)
 *	\param[in] other_data  OtherData parameter (13 bytes)
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_checkmac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, const uint8_t* response, const uint8_t* other_data)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_checkmac(_gDevice, mode, key_id, challenge, response, other_data);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* Counter command */

/** \brief Compute the Counter functions
 *  \param[in]  mode the mode used for the counter
 *  \param[in]  counter_id The counter to be used
 *  \param[out] counter_value pointer to the counter value returned from device
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_counter(uint8_t mode, uint16_t counter_id, uint32_t* counter_value)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_counter(_gDevice, mode, counter_id, counter_value);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_counter(_gDevice, mode, counter_id, counter_value);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Increments one of the device's monotonic counters
 *  \param[in]  counter_id     Counter to be incremented
 *  \param[out] counter_value  New value of the counter is returned here. Can be
 *                             NULL if not needed.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_counter_increment(uint16_t counter_id, uint32_t* counter_value)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_counter_increment(_gDevice, counter_id, counter_value);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_counter_increment(_gDevice, counter_id, counter_value);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Read one of the device's monotonic counters
 *  \param[in]  counter_id     Counter to be read
 *  \param[out] counter_value  Counter value is returned here.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_counter_read(uint16_t counter_id, uint32_t* counter_value)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_counter_read(_gDevice, counter_id, counter_value);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_counter_read(_gDevice, counter_id, counter_value);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* DeriveKey command */

/** \brief Executes the DeviveKey command for deriving a new key from a
 *          nonce (TempKey) and an existing key.
 *
 *  \param[in] mode        Bit 2 must match the value in TempKey.SourceFlag
 *  \param[in] key_id      Key slot to be written
 *  \param[in] mac         Optional 32 byte MAC used to validate operation. NULL
 *                         if not required.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_derivekey(uint8_t mode, uint16_t key_id, const uint8_t* mac)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_derivekey(_gDevice, mode, key_id, mac);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* ECDH command */

/** \brief Base function for generating premaster secret key using ECDH.
 *  \param[in]  mode        Mode to be used for ECDH computation
 *  \param[in]  key_id      Slot of key for ECDH computation
 *  \param[in]  public_key  Public key input to ECDH calculation. X and Y
 *                          integers in big-endian format. 64 bytes for P256
 *                          key.
 *  \param[out] pms         Computed ECDH pre-master secret is returned here (32
 *                          bytes) if returned directly. Otherwise NULL.
 *  \param[out] out_nonce   Nonce used to encrypt pre-master secret. NULL if
 *                          output encryption not used.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_ecdh_base(uint8_t mode, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, uint8_t* out_nonce)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_ecdh_base(_gDevice, mode, key_id, public_key, pms, out_nonce);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is returned in the clear.
 *
 *  \param[in] key_id     Slot of private key for ECDH computation
 *  \param[in] public_key Public key input to ECDH calculation. X and Y
 *                        integers in big-endian format. 64 bytes for P256
 *                        key.
 *  \param[out] pms       Computed ECDH premaster secret is returned here.
 *                        32 bytes.
 *
 *  \return ATCA_SUCCESS on success
 */
ATCA_STATUS atcab_ecdh(uint16_t key_id, const uint8_t* public_key, uint8_t* pms)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_ecdh(_gDevice, key_id, public_key, pms);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_ecdh_compat(_gDevice, key_id, public_key, pms);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is read from the next slot.
 *
 * This function only works for even numbered slots with the proper
 * configuration.
 *
 *  \param[in]  key_id       Slot of key for ECDH computation
 *  \param[in]  public_key   Public key input to ECDH calculation. X and Y
 *                           integers in big-endian format. 64 bytes for P256
 *                           key.
 *  \param[out] pms          Computed ECDH premaster secret is returned here
 *                           (32 bytes).
 *  \param[in]  read_key     Read key for the premaster secret slot (key_id|1).
 *  \param[in]  read_key_id  Read key slot for read_key.
 *  \param[in]  num_in       20 byte host nonce to inject into Nonce calculation
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_ecdh_enc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id)
#else
ATCA_STATUS atcab_ecdh_enc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
#ifdef ATCA_USE_CONSTANT_HOST_NONCE
        status = calib_ecdh_enc(_gDevice, key_id, public_key, pms, read_key, read_key_id);
#else
        status = calib_ecdh_enc(_gDevice, key_id, public_key, pms, read_key, read_key_id, num_in);
#endif
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is returned encrypted using the IO protection key.
 *
 *  \param[in]  key_id       Slot of key for ECDH computation
 *  \param[in]  public_key   Public key input to ECDH calculation. X and Y
 *                           integers in big-endian format. 64 bytes for P256
 *                           key.
 *  \param[out] pms          Computed ECDH premaster secret is returned here
 *                           (32 bytes).
 *  \param[in]  io_key       IO protection key.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_ecdh_ioenc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608_SUPPORT
        status = calib_ecdh_ioenc(_gDevice, key_id, public_key, pms, io_key);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief ECDH command with a private key in TempKey and the premaster secret
 *         is returned in the clear.
 *
 *  \param[in]  public_key  Public key input to ECDH calculation. X and Y
 *                          integers in big-endian format. 64 bytes for P256
 *                          key.
 *  \param[out] pms         Computed ECDH premaster secret is returned here
 *                          (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_ecdh_tempkey(const uint8_t* public_key, uint8_t* pms)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608_SUPPORT
        status = calib_ecdh_tempkey(_gDevice, public_key, pms);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief ECDH command with a private key in TempKey and the premaster secret
 *         is returned encrypted using the IO protection key.
 *
 *  \param[in]  public_key  Public key input to ECDH calculation. X and Y
 *                          integers in big-endian format. 64 bytes for P256
 *                          key.
 *  \param[out] pms         Computed ECDH premaster secret is returned here
 *                          (32 bytes).
 *  \param[in]  io_key      IO protection key.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_ecdh_tempkey_ioenc(const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608_SUPPORT
        status = calib_ecdh_tempkey_ioenc(_gDevice, public_key, pms, io_key);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* GenDig command */

/** \brief Issues a GenDig command, which performs a SHA256 hash on the source data indicated by zone with the
 *  contents of TempKey.  See the CryptoAuth datasheet for your chip to see what the values of zone
 *  correspond to.
 *  \param[in] zone             Designates the source of the data to hash with TempKey.
 *  \param[in] key_id           Indicates the key, OTP block, or message order for shared nonce mode.
 *  \param[in] other_data       Four bytes of data for SHA calculation when using a NoMac key, 32 bytes for
 *                              "Shared Nonce" mode, otherwise ignored (can be NULL).
 *  \param[in] other_data_size  Size of other_data in bytes.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_gendig(uint8_t zone, uint16_t key_id, const uint8_t* other_data, uint8_t other_data_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_gendig(_gDevice, zone, key_id, other_data, other_data_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* GenKey command */

/** \brief Issues GenKey command, which can generate a private key, compute a
 *          public key, nd/or compute a digest of a public key.
 *
 * \param[in]  mode        Mode determines what operations the GenKey
 *                         command performs.
 * \param[in]  key_id      Slot to perform the GenKey command on.
 * \param[in]  other_data  OtherData for PubKey digest calculation. Can be set
 *                         to NULL otherwise.
 * \param[out] public_key  If the mode indicates a public key will be
 *                         calculated, it will be returned here. Format will
 *                         be the X and Y integers in big-endian format.
 *                         64 bytes for P256 curve. Set to NULL if public key
 *                         isn't required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_genkey_base(uint8_t mode, uint16_t key_id, const uint8_t* other_data, uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_genkey_base(_gDevice, mode, key_id, other_data, public_key);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Issues GenKey command, which generates a new random private key in
 *          slot/handle and returns the public key.
 *
 * \param[in]  key_id      Slot number where an ECC private key is configured.
 *                         Can also be ATCA_TEMPKEY_KEYID to generate a private
 *                         key in TempKey.
 * \param[out] public_key  Public key will be returned here. Format will be
 *                         the X and Y integers in big-endian format.
 *                         64 bytes for P256 curve. Set to NULL if public key
 *                         isn't required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_genkey(uint16_t key_id, uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_genkey(_gDevice, key_id, public_key);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_genkey_compat(_gDevice, key_id, public_key);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Uses GenKey command to calculate the public key from an existing
 *          private key in a slot.
 *
 *  \param[in]  key_id      Slot number of the private key.
 *  \param[out] public_key  Public key will be returned here. Format will be
 *                          the X and Y integers in big-endian format.
 *                          64 bytes for P256 curve. Set to NULL if public key
 *                          isn't required.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_get_pubkey_ext(ATCADevice device, uint16_t key_id, uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type_ext(device);

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_get_pubkey(device, key_id, public_key);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_get_pubkey_compat(device, key_id, public_key);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Uses GenKey command to calculate the public key from an existing
 *          private key in a slot.
 *
 *  \param[in]  key_id      Slot number of the private key.
 *  \param[out] public_key  Public key will be returned here. Format will be
 *                          the X and Y integers in big-endian format.
 *                          64 bytes for P256 curve. Set to NULL if public key
 *                          isn't required.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_get_pubkey(uint16_t key_id, uint8_t* public_key)
{
    return atcab_get_pubkey_ext(_gDevice, key_id, public_key);
}

// HMAC command functions

/** \brief Issues a HMAC command, which computes an HMAC/SHA-256 digest of a
 *          key stored in the device, a challenge, and other information on the
 *          device.
 *
 * \param[in]  mode    Controls which fields within the device are used in the
 *                     message.
 * \param[in]  key_id  Which key is to be used to generate the response.
 *                     Bits 0:3 only are used to select a slot but all 16 bits
 *                     are used in the HMAC message.
 * \param[out] digest  HMAC digest is returned in this buffer (32 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hmac(uint8_t mode, uint16_t key_id, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATSHA204A_SUPPORT) || defined(ATCA_ATECC508A_SUPPORT)
        status = calib_hmac(_gDevice, mode, key_id, digest);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

// Info command functions

/** \brief Issues an Info command, which return internal device information and
 *          can control GPIO and the persistent latch.
 *
 * \param[in]  mode      Selects which mode to be used for info command.
 * \param[in]  param2    Selects the particular fields for the mode.
 * \param[out] out_data  Response from info command (4 bytes). Can be set to
 *                       NULL if not required.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_info_base(uint8_t mode, uint16_t param2, uint8_t* out_data)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_info_base(_gDevice, mode, param2, out_data);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Use the Info command to get the device revision (DevRev).
 *  \param[out] revision  Device revision is returned here (4 bytes).
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_info(uint8_t* revision)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_info(_gDevice, revision);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_info_compat(_gDevice, revision);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Use the Info command to set the persistent latch state for an
 *          ATECC608 device.
 *
 *  \param[out] state  Persistent latch state. Set (true) or clear (false).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_info_set_latch(bool state)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_info_set_latch(_gDevice, state);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Use the Info command to get the persistent latch current state for
 *          an ATECC608 device.
 *
 *  \param[out] state  The state is returned here. Set (true) or Cler (false).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_info_get_latch(bool* state)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_info_get_latch(_gDevice, state);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

// KDF command functions

/** \brief Executes the KDF command, which derives a new key in PRF, AES, or
 *          HKDF modes.
 *
 * Generally this function combines a source key with an input string and
 * creates a result key/digest/array.
 *
 * \param[in]  mode       Mode determines KDF algorithm (PRF,AES,HKDF), source
 *                        key location, and target key locations.
 * \param[in]  key_id     Source and target key slots if locations are in the
 *                        EEPROM. Source key slot is the LSB and target key
 *                        slot is the MSB.
 * \param[in]  details    Further information about the computation, depending
 *                        on the algorithm (4 bytes).
 * \param[in]  message    Input value from system (up to 128 bytes). Actual size
 *                        of message is 16 bytes for AES algorithm or is encoded
 *                        in the MSB of the details parameter for other
 *                        algorithms.
 * \param[out] out_data   Output of the KDF function is returned here. If the
 *                        result remains in the device, this can be NULL.
 * \param[out] out_nonce  If the output is encrypted, a 32 byte random nonce
 *                        generated by the device is returned here. If output
 *                        encryption is not used, this can be NULL.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_kdf(uint8_t mode, uint16_t key_id, const uint32_t details, const uint8_t* message, uint8_t* out_data, uint8_t* out_nonce)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_kdf(_gDevice, mode, key_id, details, message, out_data, out_nonce);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* Lock commands */

/** \brief The Lock command prevents future modifications of the Configuration
 *         and/or Data and OTP zones. If the device is so configured, then
 *         this command can be used to lock individual data slots. This
 *         command fails if the designated area is already locked.
 *
 * \param[in]  mode           Zone, and/or slot, and summary check (bit 7).
 * \param[in]  summary_crc    CRC of the config or data zones. Ignored for
 *                            slot locks or when mode bit 7 is set.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock(uint8_t mode, uint16_t summary_crc)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_lock(_gDevice, mode, summary_crc);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Unconditionally (no CRC required) lock the config zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_config_zone(void)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_lock_config_zone(_gDevice);
#endif
        }
        else
        {
            status = calib_lock_config_zone(_gDevice);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_lock_config(_gDevice);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Lock the config zone with summary CRC.
 *
 *  The CRC is calculated over the entire config zone contents. 48 bytes for TA100,
 *  88 bytes for ATSHA devices, 128 bytes for ATECC devices. Lock will fail if the provided
 *  CRC doesn't match the internally calculated one.
 *
 *  \param[in] summary_crc  Expected CRC over the config zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_config_zone_crc(uint16_t summary_crc)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_lock_config_zone_crc(_gDevice, summary_crc);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_lock_config_with_crc(_gDevice, summary_crc);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Unconditionally (no CRC required) lock the data zone (slots and OTP).
 *         for CryptoAuth devices and lock the setup for Trust Anchor device.
 *
 *	ConfigZone must be locked and DataZone must be unlocked for the zone to be successfully locked.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_data_zone(void)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_lock_data_zone(_gDevice);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_lock_setup(_gDevice);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Lock the data zone (slots and OTP) with summary CRC.
 *
 *  The CRC is calculated over the concatenated contents of all the slots and
 *  OTP at the end. Private keys (KeyConfig.Private=1) are skipped. Lock will
 *  fail if the provided CRC doesn't match the internally calculated one.
 *
 *  \param[in] summary_crc  Expected CRC over the data zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_data_zone_crc(uint16_t summary_crc)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_lock_data_zone_crc(_gDevice, summary_crc);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Lock an individual slot in the data zone on an ATECC device. Not
 *         available for ATSHA devices. Slot must be configured to be slot
 *         lockable (KeyConfig.Lockable=1) (for cryptoauth devices) or Lock
 *         an individual handle in shared data element on an Trust Anchor device
 *         (for Trust Anchor devices).
 *
 *  \param[in] slot  Slot to be locked in data zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_data_slot(uint16_t slot)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_lock_data_slot(_gDevice, slot);
#endif
        }
        else
        {
            status = calib_lock_data_slot(_gDevice, slot);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_lock_handle(_gDevice, slot);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

// MAC command functions

/** \brief Executes MAC command, which computes a SHA-256 digest of a key
 *          stored in the device, a challenge, and other information on the
 *          device.
 *
 *	\param[in]  mode       Controls which fields within the device are used in
 *                         the message
 *	\param[in]  key_id     Key in the CryptoAuth device to use for the MAC
 *	\param[in]  challenge  Challenge message (32 bytes). May be NULL if mode
 *                         indicates a challenge isn't required.
 *	\param[out] digest     MAC response is returned here (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_mac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_mac(_gDevice, mode, key_id, challenge, digest);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

// Nonce command functions

/** \brief Executes Nonce command, which loads a random or fixed nonce/data
 *          into the device for use by subsequent commands.
 *
 * \param[in]  mode         Controls the mechanism of the internal RNG or fixed
 *                          write.
 * \param[in]  zero         Param2, normally 0, but can be used to indicate a
 *                          nonce calculation mode (bit 15).
 * \param[in]  num_in       Input value to either be included in the nonce
 *                          calculation in random modes (20 bytes) or to be
 *                          written directly (32 bytes or 64 bytes(ATECC608))
 *                          in pass-through mode.
 * \param[out] rand_out     If using a random mode, the internally generated
 *                          32-byte random number that was used in the nonce
 *                          calculation is returned here. Can be NULL if not
 *                          needed.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_nonce_base(uint8_t mode, uint16_t zero, const uint8_t* num_in, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_nonce_base(_gDevice, mode, zero, num_in, rand_out);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Execute a Nonce command in pass-through mode to initialize TempKey
 *         to a specified value.
 *
 *  \param[in] num_in  Data to be loaded into TempKey (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_nonce(const uint8_t* num_in)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_nonce(_gDevice, num_in);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Execute a Nonce command in pass-through mode to load one of the
 *          device's internal buffers with a fixed value.
 *
 * For the ATECC608, available targets are TempKey (32 or 64 bytes), Message
 * Digest Buffer (32 or 64 bytes), or the Alternate Key Buffer (32 bytes). For
 * all other devices, only TempKey (32 bytes) is available.
 *
 *  \param[in] target       Target device buffer to load. Can be
 *                          NONCE_MODE_TARGET_TEMPKEY,
 *                          NONCE_MODE_TARGET_MSGDIGBUF, or
 *                          NONCE_MODE_TARGET_ALTKEYBUF.
 *  \param[in] num_in       Data to load into the buffer.
 *  \param[in] num_in_size  Size of num_in in bytes. Can be 32 or 64 bytes
 *                          depending on device and target.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_nonce_load(uint8_t target, const uint8_t* num_in, uint16_t num_in_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_nonce_load(_gDevice, target, num_in, num_in_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Execute a Nonce command to generate a random nonce combining a host
 *          nonce (num_in) and a device random number.
 *
 *  \param[in]  num_in    Host nonce to be combined with the device random
 *                        number (20 bytes).
 *  \param[out] rand_out  Internally generated 32-byte random number that was
 *                        used in the nonce/challenge calculation is returned
 *                        here. Can be NULL if not needed.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_nonce_rand(const uint8_t* num_in, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_nonce_rand(_gDevice, num_in, rand_out);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Execute a Nonce command in pass-through mode to initialize TempKey
 *          to a specified value.
 *
 *  \param[in] num_in  Data to be loaded into TempKey (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_challenge(const uint8_t* num_in)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_challenge(_gDevice, num_in);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Execute a Nonce command to generate a random challenge combining
 *         a host nonce (num_in) and a device random number.
 *
 *  \param[in]  num_in    Host nonce to be combined with the device random
 *                        number (20 bytes).
 *  \param[out] rand_out  Internally generated 32-byte random number that was
 *                        used in the nonce/challenge calculation is returned
 *                        here. Can be NULL if not needed.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_challenge_seed_update(const uint8_t* num_in, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_challenge_seed_update(_gDevice, num_in, rand_out);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

// PrivWrite command functions

/** \brief Executes PrivWrite command, to write externally generated ECC
 *          private keys into the device.
 *
 *  \param[in] key_id        Slot to write the external private key into.
 *  \param[in] priv_key      External private key (36 bytes) to be written.
 *                           The first 4 bytes should be zero for P256 curve.
 *  \param[in] write_key_id  Write key slot. Ignored if write_key is NULL.
 *  \param[in] write_key     Write key (32 bytes). If NULL, perform an
 *                           unencrypted PrivWrite, which is only available when
 *                           the data zone is unlocked.
 *  \param[in]  num_in       20 byte host nonce to inject into Nonce calculation
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_priv_write(uint16_t key_id, const uint8_t priv_key[36], uint16_t write_key_id, const uint8_t write_key[32])
#else
ATCA_STATUS atcab_priv_write(uint16_t key_id, const uint8_t priv_key[36], uint16_t write_key_id, const uint8_t write_key[32], const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
#ifdef ATCA_USE_CONSTANT_HOST_NONCE
        status = calib_priv_write(_gDevice, key_id, priv_key, write_key_id, write_key);
#else
        status = calib_priv_write(_gDevice, key_id, priv_key, write_key_id, write_key, num_in);
#endif /* ATCA_USE_CONSTANT_HOST_NONCE */
#endif /* ATCA_ECC_SUPPORT */
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

// Random command functions

/** \brief Executes Random command, which generates a 32 byte random number
 *          from the device.
 *
 * \param[in]  device    Device context pointer
 * \param[out] rand_out  32 bytes of random data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_random_ext(ATCADevice device, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type_ext(device);

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_random(device, rand_out);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_random_compat(device, rand_out);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}


/** \brief Executes Random command, which generates a 32 byte random number
 *          from the device.
 *
 * \param[out] rand_out  32 bytes of random data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_random(uint8_t* rand_out)
{
    return atcab_random_ext(_gDevice, rand_out);
}

// Read command functions

/** \brief Executes Read command, which reads either 4 or 32 bytes of data from
 *          a given slot, configuration zone, or the OTP zone.
 *
 *   When reading a slot or OTP, data zone must be locked and the slot
 *   configuration must not be secret for a slot to be successfully read.
 *
 *  \param[in]  zone    Zone to be read from device. Options are
 *                      ATCA_ZONE_CONFIG, ATCA_ZONE_OTP, or ATCA_ZONE_DATA.
 *  \param[in]  slot    Slot number for data zone and ignored for other zones.
 *  \param[in]  block   32 byte block index within the zone.
 *  \param[in]  offset  4 byte work index within the block. Ignored for 32 byte
 *                      reads.
 *  \param[out] data    Read data is returned here.
 *  \param[in]  len     Length of the data to be read. Must be either 4 or 32.
 *
 *  returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_read_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint8_t* data, uint8_t len)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_read_zone(_gDevice, zone, slot, block, offset, data, len);
#endif
        }
        else
        {
            status = calib_read_zone(_gDevice, zone, slot, block, offset, data, len);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes Read command, which reads the configuration zone to see if
 *          the specified zone is locked.
 *
 *  \param[in]  zone       The zone to query for locked (use LOCK_ZONE_CONFIG or
 *                         LOCK_ZONE_DATA).
 *  \param[out] is_locked  Lock state returned here. True if locked.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_is_locked(uint8_t zone, bool* is_locked)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_is_locked(_gDevice, zone, is_locked);
#endif
        }
        else
        {
            status = calib_is_locked(_gDevice, zone, is_locked);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        if (LOCK_ZONE_CONFIG == zone)
        {
            status = talib_is_config_locked(_gDevice, is_locked);
        }
        else if (LOCK_ZONE_DATA == zone)
        {
            status = talib_is_setup_locked(_gDevice, is_locked);
        }
        else
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "");
        }
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief This function check whether configuration zone is locked or not
 *
 *  \param[out] is_locked  Lock state returned here. True if locked.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_is_config_locked(bool* is_locked)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_is_locked(_gDevice, ATCA_ECC204_ZONE_CONFIG, is_locked);
#endif
        }
        else
        {
            status = calib_is_locked(_gDevice, LOCK_ZONE_CONFIG, is_locked);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_is_config_locked(_gDevice, is_locked);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief This function check whether data/setup zone is locked or not
 *
 *  \param[out] is_locked  Lock state returned here. True if locked.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_is_data_locked(bool* is_locked)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
#if defined(ATCA_ECC204_SUPPORT)
        if (ECC204 == dev_type)
        {
            status = calib_ecc204_is_locked(_gDevice, ATCA_ECC204_ZONE_DATA, is_locked);
        }
        else
#endif
        {
            status = calib_is_locked(_gDevice, LOCK_ZONE_DATA, is_locked);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_is_setup_locked(_gDevice, is_locked);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief This function check whether slot/handle is locked or not
 *
 *  \param[in]  slot       Slot to query for locked
 *  \param[out] is_locked  Lock state returned here. True if locked.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_is_slot_locked(uint16_t slot, bool* is_locked)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_is_slot_locked(_gDevice, slot, is_locked);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_is_handle_locked(_gDevice, slot, is_locked);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Used to read an arbitrary number of bytes from any zone configured
 *          for clear reads.
 *
 * This function will issue the Read command as many times as is required to
 * read the requested data.
 *
 *  \param[in]  zone    Zone to read data from. Option are ATCA_ZONE_CONFIG(0),
 *                      ATCA_ZONE_OTP(1), or ATCA_ZONE_DATA(2).
 *  \param[in]  slot    Slot number to read from if zone is ATCA_ZONE_DATA(2).
 *                      Ignored for all other zones.
 *  \param[in]  offset  Byte offset within the zone to read from.
 *  \param[out] data    Read data is returned here.
 *  \param[in]  length  Number of bytes to read starting from the offset.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_read_bytes_zone(uint8_t zone, uint16_t slot, size_t offset, uint8_t* data, size_t length)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_read_bytes_zone(_gDevice, zone, slot, offset, data, length);
#endif
        }
        else
        {
            status = calib_read_bytes_zone(_gDevice, zone, slot, offset, data, length);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_read_bytes_zone(_gDevice, zone, slot, offset, data, length);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief This function returns serial number of the device.
 *
 *  \param[out] serial_number  9 byte serial number is returned here.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_read_serial_number(uint8_t* serial_number)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_read_serial_number(_gDevice, serial_number);
#endif
        }
        else
        {
            status = calib_read_serial_number(_gDevice, serial_number);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_info_serial_number_compat(_gDevice, serial_number);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes Read command to read an ECC P256 public key from a slot
 *          configured for clear reads.
 *
 * This function assumes the public key is stored using the ECC public key
 * format specified in the datasheet.
 *
 *  \param[in]  slot        Slot number to read from. Only slots 8 to 15 are
 *                          large enough for a public key.
 *  \param[out] public_key  Public key is returned here (64 bytes). Format will
 *                          be the 32 byte X and Y big-endian integers
 *                          concatenated.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_read_pubkey(uint16_t slot, uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_read_pubkey(_gDevice, slot, public_key);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_read_pubkey_compat(_gDevice, slot, public_key);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes Read command to read a 64 byte ECDSA P256 signature from a
 *          slot configured for clear reads.
 *
 *  \param[in]  slot  Slot number to read from. Only slots 8 to 15 are large
 *                    enough for a signature.
 *  \param[out] sig   Signature will be returned here (64 bytes). Format will be
 *                    the 32 byte R and S big-endian integers concatenated.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_read_sig(uint16_t slot, uint8_t* sig)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_read_sig(_gDevice, slot, sig);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_read_sig_compat(_gDevice, slot, sig);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes Read command to read the complete device configuration
 *          zone.
 *
 *  \param[out] config_data  Configuration zone data is returned here. 88 bytes
 *                           for ATSHA devices, 128 bytes for ATECC devices and
 *                           48 bytes for Trust Anchor devices.
 *
 *  \returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_read_config_zone(uint8_t* config_data)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_read_config_zone(_gDevice, config_data);
#endif
        }
        else
        {
            status = calib_read_config_zone(_gDevice, config_data);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_read_config_zone(_gDevice, config_data);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Compares a specified configuration zone with the configuration zone
 *          currently on the device.
 *
 * This only compares the static portions of the configuration zone and skips
 * those that are unique per device (first 16 bytes) and areas that can change
 * after the configuration zone has been locked (e.g. LastKeyUse).
 *
 * \param[in]  config_data  Full configuration data to compare the device
 *                          against.
 * \param[out] same_config  Result is returned here. True if the static portions
 *                          on the configuration zones are the same.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_cmp_config_zone(uint8_t* config_data, bool* same_config)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_cmp_config_zone(_gDevice, config_data, same_config);
#endif
        }
        else
        {
            status = calib_cmp_config_zone(_gDevice, config_data, same_config);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_cmp_config_zone(_gDevice, config_data, same_config);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes Read command on a slot configured for encrypted reads and
 *          decrypts the data to return it as plaintext.
 *
 * Data zone must be locked for this command to succeed. Can only read 32 byte
 * blocks.
 *
 *  \param[in]  key_id      The slot ID to read from.
 *  \param[in]  block       Index of the 32 byte block within the slot to read.
 *  \param[out] data        Decrypted (plaintext) data from the read is returned
 *                          here (32 bytes).
 *  \param[in]  enc_key     32 byte ReadKey for the slot being read.
 *  \param[in]  enc_key_id  KeyID of the ReadKey being used.
 *  \param[in]  num_in      20 byte host nonce to inject into Nonce calculation
 *
 *  returns ATCA_SUCCESS on success, otherwise an error code.
 */
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_read_enc(uint16_t key_id, uint8_t block, uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id)
#else
ATCA_STATUS atcab_read_enc(uint16_t key_id, uint8_t block, uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
        status = calib_read_enc(_gDevice, key_id, block, data, enc_key, enc_key_id);
#else
        status = calib_read_enc(_gDevice, key_id, block, data, enc_key, enc_key_id, num_in);
#endif
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

// SecureBoot command functions

/** \brief Executes Secure Boot command, which provides support for secure
 *          boot of an external MCU or MPU.
 *
 * \param[in]  mode       Mode determines what operations the SecureBoot
 *                        command performs.
 * \param[in]  param2     Not used, must be 0.
 * \param[in]  digest     Digest of the code to be verified (32 bytes).
 * \param[in]  signature  Signature of the code to be verified (64 bytes). Can
 *                        be NULL when using the FullStore mode.
 * \param[out] mac        Validating MAC will be returned here (32 bytes). Can
 *                        be NULL if not required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_secureboot(uint8_t mode, uint16_t param2, const uint8_t* digest, const uint8_t* signature, uint8_t* mac)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608_SUPPORT
        status = calib_secureboot(_gDevice, mode, param2, digest, signature, mac);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes Secure Boot command with encrypted digest and validated
 *          MAC response using the IO protection key.
 *
 * \param[in]  mode         Mode determines what operations the SecureBoot
 *                          command performs.
 * \param[in]  digest       Digest of the code to be verified (32 bytes).
 *                          This is the plaintext digest (not encrypted).
 * \param[in]  signature    Signature of the code to be verified (64 bytes). Can
 *                          be NULL when using the FullStore mode.
 * \param[in]  num_in       Host nonce (20 bytes).
 * \param[in]  io_key       IO protection key (32 bytes).
 * \param[out] is_verified  Verify result is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_secureboot_mac(uint8_t mode, const uint8_t* digest, const uint8_t* signature, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608_SUPPORT
        status = calib_secureboot_mac(_gDevice, mode, digest, signature, num_in, io_key, is_verified);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* SelfTest Command */

/** \brief Executes the SelfTest command, which performs a test of one or more
 *          of the cryptographic engines within the ATECC608 chip.
 *
 *  \param[in]  mode    Functions to test. Can be a bit field combining any
 *                      of the following: SELFTEST_MODE_RNG,
 *                      SELFTEST_MODE_ECDSA_VERIFY, SELFTEST_MODE_ECDSA_SIGN,
 *                      SELFTEST_MODE_ECDH, SELFTEST_MODE_AES,
 *                      SELFTEST_MODE_SHA, SELFTEST_MODE_ALL.
 *  \param[in]  param2  Currently unused, should be 0.
 *  \param[out] result  Results are returned here as a bit field.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_selftest(uint8_t mode, uint16_t param2, uint8_t* result)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608_SUPPORT
        status = calib_selftest(_gDevice, mode, param2, result);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* SHA Command  */

/** \brief Executes SHA command, which computes a SHA-256 or HMAC/SHA-256
 *          digest for general purpose use by the host system.
 *
 * Only the Start(0) and Compute(1) modes are available for ATSHA devices.
 *
 * \param[in]    mode           SHA command mode Start(0), Update/Compute(1),
 *                              End(2), Public(3), HMACstart(4), HMACend(5),
 *                              Read_Context(6), or Write_Context(7). Also
 *                              message digest target location for the
 *                              ATECC608.
 * \param[in]    length         Number of bytes in the message parameter or
 *                              KeySlot for the HMAC key if Mode is
 *                              HMACstart(4) or Public(3).
 * \param[in]    data_in        Message bytes to be hashed or Write_Context if
 *                              restoring a context on the ATECC608. Can be
 *                              NULL if not required by the mode.
 * \param[out]   data_out       Data returned by the command (digest or
 *                              context).
 * \param[in,out] data_out_size  As input, the size of the data_out buffer. As
 *                              output, the number of bytes returned in
 *                              data_out.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_base(uint8_t mode, uint16_t length, const uint8_t* data_in, uint8_t* data_out, uint16_t* data_out_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_base(_gDevice, mode, length, data_in, data_out, data_out_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sha_base_compat(_gDevice, mode, length, data_in, data_out, data_out_size);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes SHA command to initialize SHA-256 calculation engine
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_start(void)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_start(_gDevice);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sha_start(_gDevice);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes SHA command to add 64 bytes of message data to the current
 *          context.
 *
 * \param[in] message  64 bytes of message data to add to add to operation.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_update(const uint8_t* message)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_update(_gDevice, message);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sha_update_compat(_gDevice, message);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes SHA command to complete SHA-256 or HMAC/SHA-256 operation.
 *
 *  \param[out] digest   Digest from SHA-256 or HMAC/SHA-256 will be returned
 *                       here (32 bytes).
 *  \param[in]  length   Length of any remaining data to include in hash. Max 64
 *                       bytes.
 *  \param[in]  message  Remaining data to include in hash. NULL if length is 0.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_end(uint8_t* digest, uint16_t length, const uint8_t* message)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_end(_gDevice, digest, length, message);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sha_end_compat(_gDevice, digest, length, message);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes SHA command to read the SHA-256 context back. Only for
 *          ATECC608 with SHA-256 contexts. HMAC not supported.
 *
 *  \param[out]   context       Context data is returned here.
 *  \param[in,out] context_size  As input, the size of the context buffer in
 *                              bytes. As output, the size of the returned
 *                              context data.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_read_context(uint8_t* context, uint16_t* context_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_read_context(_gDevice, context, context_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sha_read_context(_gDevice, context, context_size);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes SHA command to write (restore) a SHA-256 context into the
 *          the device. Only supported for ATECC608 with SHA-256 contexts.
 *
 *  \param[in] context       Context data to be restored.
 *  \param[in] context_size  Size of the context data in bytes.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_write_context(const uint8_t* context, uint16_t context_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_write_context(_gDevice, context, context_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sha_write_context(_gDevice, context, context_size);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Use the SHA command to compute a SHA-256 digest.
 *
 * \param[in]  length   Size of message parameter in bytes.
 * \param[in]  message  Message data to be hashed.
 * \param[out] digest   Digest is returned here (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha(uint16_t length, const uint8_t* message, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha(_gDevice, length, message, digest);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sha(_gDevice, length, message, digest);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Use the SHA command to compute a SHA-256 digest.
 *
 * \param[in]  data       Message data to be hashed.
 * \param[in]  data_size  Size of data in bytes.
 * \param[out] digest     Digest is returned here (32 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hw_sha2_256(const uint8_t* data, size_t data_size, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_hw_sha2_256(_gDevice, data, data_size, digest);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sha(_gDevice, (uint16_t)data_size, data, digest);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Initialize a SHA context for performing a hardware SHA-256 operation
 *          on a device. Note that only one SHA operation can be run at a time.
 *
 * \param[in] ctx  SHA256 context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hw_sha2_256_init(atca_sha256_ctx_t* ctx)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_hw_sha2_256_init(_gDevice, ctx);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Add message data to a SHA context for performing a hardware SHA-256
 *          operation on a device.
 *
 * \param[in] ctx        SHA256 context
 * \param[in] data       Message data to be added to hash.
 * \param[in] data_size  Size of data in bytes.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hw_sha2_256_update(atca_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_hw_sha2_256_update(_gDevice, ctx, data, data_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Finish SHA-256 digest for a SHA context for performing a hardware
 *          SHA-256 operation on a device.
 *
 * \param[in]  ctx     SHA256 context
 * \param[out] digest  SHA256 digest is returned here (32 bytes)
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hw_sha2_256_finish(atca_sha256_ctx_t* ctx, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_hw_sha2_256_finish(_gDevice, ctx, digest);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes SHA command to start an HMAC/SHA-256 operation
 *
 * \param[in] ctx       HMAC/SHA-256 context
 * \param[in] key_slot  Slot key id to use for the HMAC calculation
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_hmac_init(atca_hmac_sha256_ctx_t* ctx, uint16_t key_slot)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_hmac_init(_gDevice, ctx, key_slot);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes SHA command to add an arbitrary amount of message data to
 *          a HMAC/SHA-256 operation.
 *
 * \param[in] ctx        HMAC/SHA-256 context
 * \param[in] data       Message data to add
 * \param[in] data_size  Size of message data in bytes
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_hmac_update(atca_hmac_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_hmac_update(_gDevice, ctx, data, data_size);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes SHA command to complete a HMAC/SHA-256 operation.
 *
 * \param[in]  ctx     HMAC/SHA-256 context
 * \param[out] digest  HMAC/SHA-256 result is returned here (32 bytes).
 * \param[in]  target  Where to save the digest internal to the device.
 *                     For ATECC608, can be SHA_MODE_TARGET_TEMPKEY,
 *                     SHA_MODE_TARGET_MSGDIGBUF, or SHA_MODE_TARGET_OUT_ONLY.
 *                     For all other devices, SHA_MODE_TARGET_TEMPKEY is the
 *                     only option.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_hmac_finish(atca_hmac_sha256_ctx_t* ctx, uint8_t* digest, uint8_t target)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_hmac_finish(_gDevice, ctx, digest, target);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Use the SHA command to compute an HMAC/SHA-256 operation.
 *
 * \param[in]  data       Message data to be hashed.
 * \param[in]  data_size  Size of data in bytes.
 * \param[in]  key_slot   Slot key id to use for the HMAC calculation
 * \param[out] digest     Digest is returned here (32 bytes).
 * \param[in]  target     Where to save the digest internal to the device.
 *                        For ATECC608, can be SHA_MODE_TARGET_TEMPKEY,
 *                        SHA_MODE_TARGET_MSGDIGBUF, or
 *                        SHA_MODE_TARGET_OUT_ONLY. For all other devices,
 *                        SHA_MODE_TARGET_TEMPKEY is the only option.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_hmac(const uint8_t* data, size_t data_size, uint16_t key_slot, uint8_t* digest, uint8_t target)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_sha_hmac(_gDevice, data, data_size, key_slot, digest, target);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_hmac_compat(_gDevice, data, data_size, key_slot, digest, target);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}


/* Sign command */

/** \brief Executes the Sign command, which generates a signature using the
 *          ECDSA algorithm.
 *
 * \param[in]  mode       Mode determines what the source of the message to be
 *                        signed.
 * \param[in]  key_id     Private key slot used to sign the message.
 * \param[out] signature  Signature is returned here. Format is R and S
 *                        integers in big-endian format. 64 bytes for P256
 *                        curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sign_base(uint8_t mode, uint16_t key_id, uint8_t* signature)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_sign_base(_gDevice, mode, key_id, signature);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes Sign command, to sign a 32-byte external message using the
 *                   private key in the specified slot. The message to be signed
 *                   will be loaded into the Message Digest Buffer to the
 *                   ATECC608 device or TempKey for other devices.
 *
 *  \param[in]  device     Device context pointer
 *  \param[in]  key_id     Slot of the private key to be used to sign the
 *                         message.
 *  \param[in]  msg        32-byte message to be signed. Typically the SHA256
 *                         hash of the full message.
 *  \param[out] signature  Signature will be returned here. Format is R and S
 *                         integers in big-endian format. 64 bytes for P256
 *                         curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sign_ext(ATCADevice device, uint16_t key_id, const uint8_t* msg, uint8_t* signature)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type_ext(device);

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_sign(_gDevice, key_id, msg, signature);
#endif
        }
        else
        {
            status = calib_sign(_gDevice, key_id, msg, signature);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_sign_compat(_gDevice, key_id, msg, signature);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes Sign command, to sign a 32-byte external message using the
 *                   private key in the specified slot. The message to be signed
 *                   will be loaded into the Message Digest Buffer to the
 *                   ATECC608 device or TempKey for other devices.
 *
 *  \param[in]  key_id     Slot of the private key to be used to sign the
 *                         message.
 *  \param[in]  msg        32-byte message to be signed. Typically the SHA256
 *                         hash of the full message.
 *  \param[out] signature  Signature will be returned here. Format is R and S
 *                         integers in big-endian format. 64 bytes for P256
 *                         curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sign(uint16_t key_id, const uint8_t* msg, uint8_t* signature)
{
    return atcab_sign_ext(_gDevice, key_id, msg, signature);
}

/** \brief Executes Sign command to sign an internally generated message.
 *
 *  \param[in]  key_id         Slot of the private key to be used to sign the
 *                             message.
 *  \param[in]  is_invalidate  Set to true if the signature will be used with
 *                             the Verify(Invalidate) command. false for all
 *                             other cases.
 *  \param[in]  is_full_sn     Set to true if the message should incorporate
 *                             the device's full serial number.
 *  \param[out] signature      Signature is returned here. Format is R and S
 *                             integers in big-endian format. 64 bytes for
 *                             P256 curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sign_internal(uint16_t key_id, bool is_invalidate, bool is_full_sn, uint8_t* signature)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_sign_internal(_gDevice, key_id, is_invalidate, is_full_sn, signature);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* UpdateExtra command */

/** \brief Executes UpdateExtra command to update the values of the two
 *          extra bytes within the Configuration zone (bytes 84 and 85).
 *
 * Can also be used to decrement the limited use counter associated with the
 * key in slot NewValue.
 *
 * \param[in] mode       Mode determines what operations the UpdateExtra
 *                      command performs.
 * \param[in] new_value  Value to be written.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_updateextra(uint8_t mode, uint16_t new_value)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_updateextra(_gDevice, mode, new_value);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* Verify command */

/** \brief Executes the Verify command, which takes an ECDSA [R,S] signature
 *          and verifies that it is correctly generated from a given message and
 *          public key. In all cases, the signature is an input to the command.
 *
 * For the Stored, External, and ValidateExternal Modes, the contents of
 * TempKey (or Message Digest Buffer in some cases for the ATECC608) should
 * contain the 32 byte message.
 *
 * \param[in] mode        Verify command mode and options
 * \param[in] key_id      Stored mode, the slot containing the public key to
 *                        be used for the verification.
 *                        ValidateExternal mode, the slot containing the
 *                        public key to be validated.
 *                        External mode, KeyID contains the curve type to be
 *                        used to Verify the signature.
 *                        Validate or Invalidate mode, the slot containing
 *                        the public key to be (in)validated.
 * \param[in] signature   Signature to be verified. R and S integers in
 *                        big-endian format. 64 bytes for P256 curve.
 * \param[in] public_key  If mode is External, the public key to be used for
 *                        verification. X and Y integers in big-endian format.
 *                        64 bytes for P256 curve. NULL for all other modes.
 * \param[in] other_data  If mode is Validate, the bytes used to generate the
 *                        message for the validation (19 bytes). NULL for all
 *                        other modes.
 * \param[out] mac        If mode indicates a validating MAC, then the MAC will
 *                        will be returned here. Can be NULL otherwise.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_verify(uint8_t mode, uint16_t key_id, const uint8_t* signature, const uint8_t* public_key, const uint8_t* other_data, uint8_t* mac)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify(_gDevice, mode, key_id, signature, public_key, other_data, mac);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Verify command, which verifies a signature (ECDSA
 *          verify operation) with all components (message, signature, and
 *          public key) supplied. The message to be signed will be loaded into
 *          the Message Digest Buffer to the ATECC608 device or TempKey for
 *          other devices.
 *
 * \param[in]  device       Device context pointer
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  public_key   The public key to be used for verification. X and
 *                          Y integers in big-endian format. 64 bytes for
 *                          P256 curve.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_extern_ext(ATCADevice device, const uint8_t* message, const uint8_t* signature, const uint8_t* public_key, bool* is_verified)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type_ext(device);

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_extern(device, message, signature, public_key, is_verified);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_verify_extern_compat(device, message, signature, public_key, is_verified);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Verify command, which verifies a signature (ECDSA
 *          verify operation) with all components (message, signature, and
 *          public key) supplied. The message to be signed will be loaded into
 *          the Message Digest Buffer to the ATECC608 device or TempKey for
 *          other devices.
 *
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  public_key   The public key to be used for verification. X and
 *                          Y integers in big-endian format. 64 bytes for
 *                          P256 curve.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_extern(const uint8_t* message, const uint8_t* signature, const uint8_t* public_key, bool* is_verified)
{
    return atcab_verify_extern_ext(_gDevice, message, signature, public_key, is_verified);
}

/** \brief Executes the Verify command with verification MAC, which verifies a
 *          signature (ECDSA verify operation) with all components (message,
 *          signature, and public key) supplied. This function is only available
 *          on the ATECC608.
 *
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  public_key   The public key to be used for verification. X and
 *                          Y integers in big-endian format. 64 bytes for
 *                          P256 curve.
 * \param[in]  num_in       System nonce (32 byte) used for the verification
 *                          MAC.
 * \param[in]  io_key       IO protection key for verifying the validation MAC.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_extern_mac(const uint8_t* message, const uint8_t* signature, const uint8_t* public_key, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_extern_mac(_gDevice, message, signature, public_key, num_in, io_key, is_verified);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Verify command, which verifies a signature (ECDSA
 *          verify operation) with a public key stored in the device. The
 *          message to be signed will be loaded into the Message Digest Buffer
 *          to the ATECC608 device or TempKey for other devices.
 *
 * \param[in]  device       Device context pointer
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  key_id       Slot containing the public key to be used in the
 *                         verification.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_stored_ext(ATCADevice device, const uint8_t* message, const uint8_t* signature, uint16_t key_id, bool* is_verified)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type_ext(device);

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_stored(device, message, signature, key_id, is_verified);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_verify_stored_compat(device, message, signature, key_id, is_verified);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Verify command, which verifies a signature (ECDSA
 *          verify operation) with a public key stored in the device. The
 *          message to be signed will be loaded into the Message Digest Buffer
 *          to the ATECC608 device or TempKey for other devices.
 *
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  key_id       Slot containing the public key to be used in the
 *                         verification.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_stored(const uint8_t* message, const uint8_t* signature, uint16_t key_id, bool* is_verified)
{
    return atcab_verify_stored_ext(_gDevice, message, signature, key_id, is_verified);
}

/** \brief Executes the Verify command with verification MAC, which verifies a
 *          signature (ECDSA verify operation) with a public key stored in the
 *          device. This function is only available on the ATECC608.
 *
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  key_id       Slot containing the public key to be used in the
 *                          verification.
 * \param[in]  num_in       System nonce (32 byte) used for the verification
 *                          MAC.
 * \param[in]  io_key       IO protection key for verifying the validation MAC.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_stored_mac(const uint8_t* message, const uint8_t* signature, uint16_t key_id, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_stored_mac(_gDevice, message, signature, key_id, num_in, io_key, is_verified);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Verify command in Validate mode to validate a public
 *          key stored in a slot.
 *
 * This command can only be run after GenKey has been used to create a PubKey
 * digest of the public key to be validated in TempKey (mode=0x10).
 *
 * \param[in]  key_id       Slot containing the public key to be validated.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  other_data   19 bytes of data used to build the verification
 *                          message.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          validation public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_validate(uint16_t key_id, const uint8_t* signature, const uint8_t* other_data, bool* is_verified)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_validate(_gDevice, key_id, signature, other_data, is_verified);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Verify command in Invalidate mode which invalidates a
 *          previously validated public key stored in a slot.
 *
 * This command can only be run after GenKey has been used to create a PubKey
 * digest of the public key to be invalidated in TempKey (mode=0x10).
 *
 * \param[in]  key_id       Slot containing the public key to be invalidated.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  other_data   19 bytes of data used to build the verification
 *                          message.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          validation public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_invalidate(uint16_t key_id, const uint8_t* signature, const uint8_t* other_data, bool* is_verified)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_invalidate(_gDevice, key_id, signature, other_data, is_verified);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/* Write command functions */

/**
 * \brief Executes the Write command, which writes either one four byte word or
 *        a 32-byte block to one of the EEPROM zones on the device. Depending
 *        upon the value of the WriteConfig byte for this slot, the data may be
 *        required to be encrypted by the system prior to being sent to the
 *        device. This command cannot be used to write slots configured as ECC
 *        private keys.
 *
 * \param[in] zone     Zone/Param1 for the write command.
 * \param[in] address  Address/Param2 for the write command.
 * \param[in] value    Plain-text data to be written or cipher-text for
 *                     encrypted writes. 32 or 4 bytes depending on bit 7 in the
 *                     zone.
 * \param[in] mac      MAC required for encrypted writes (32 bytes). Set to NULL
 *                     if not required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_write(uint8_t zone, uint16_t address, const uint8_t* value, const uint8_t* mac)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_write(_gDevice, zone, address, value, mac);
#endif
        }
        else
        {
            status = calib_write(_gDevice, zone, address, value, mac);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Write command, which writes either 4 or 32 bytes of
 *          data into a device zone.
 *
 *  \param[in] zone    Device zone to write to (0=config, 1=OTP, 2=data).
 *  \param[in] slot    If writing to the data zone, it is the slot to write to,
 *                     otherwise it should be 0.
 *  \param[in] block   32-byte block to write to.
 *  \param[in] offset  4-byte word within the specified block to write to. If
 *                     performing a 32-byte write, this should be 0.
 *  \param[in] data    Data to be written.
 *  \param[in] len     Number of bytes to be written. Must be either 4 or 32.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_write_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, const uint8_t* data, uint8_t len)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_write_zone(_gDevice, zone, slot, block, offset, data, len);
#endif
        }
        else
        {
            status = calib_write_zone(_gDevice, zone, slot, block, offset, data, len);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_write_zone(_gDevice, zone, slot, block, offset, data, len);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Write command, which writes data into the
 *          configuration, otp, or data zones with a given byte offset and
 *          length. Offset and length must be multiples of a word (4 bytes).
 *
 * Config zone must be unlocked for writes to that zone. If data zone is
 * unlocked, only 32-byte writes are allowed to slots and OTP and the offset
 * and length must be multiples of 32 or the write will fail.
 *
 *  \param[in] zone          Zone to write data to: ATCA_ZONE_CONFIG(0),
 *                           ATCA_ZONE_OTP(1), or ATCA_ZONE_DATA(2).
 *  \param[in] slot          If zone is ATCA_ZONE_DATA(2), the slot number to
 *                           write to. Ignored for all other zones.
 *  \param[in] offset_bytes  Byte offset within the zone to write to. Must be
 *                           a multiple of a word (4 bytes).
 *  \param[in] data          Data to be written.
 *  \param[in] length        Number of bytes to be written. Must be a multiple
 *                           of a word (4 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_write_bytes_zone(uint8_t zone, uint16_t slot, size_t offset_bytes, const uint8_t* data, size_t length)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_write_bytes_zone(_gDevice, zone, slot, offset_bytes, data, length);
#endif
        }
        else
        {
            status = calib_write_bytes_zone(_gDevice, zone, slot, offset_bytes, data, length);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_write_bytes_zone(_gDevice, zone, slot, offset_bytes, data, length);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Uses the write command to write a public key to a slot in the
 *         proper format.
 *
 *  \param[in] slot        Slot number to write. Only slots 8 to 15 are large
 *                         enough to store a public key.
 *  \param[in] public_key  Public key to write into the slot specified. X and Y
 *                         integers in big-endian format. 64 bytes for P256
 *                         curve.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_write_pubkey(uint16_t slot, const uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_write_pubkey(_gDevice, slot, public_key);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_write_pubkey_compat(_gDevice, slot, public_key);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Write command, which writes the configuration zone.
 *
 *  First 16 bytes are skipped as they are not writable. LockValue and
 *  LockConfig are also skipped and can only be changed via the Lock
 *  command.
 *
 *  This command may fail if UserExtra and/or Selector bytes have
 *  already been set to non-zero values.
 *
 *  \param[in] config_data  Data to the config zone data. This should be 88
 *                          bytes for SHA devices and 128 bytes for ECC
 *                          devices.
 *
 *  \returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_write_config_zone(const uint8_t* config_data)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        if (ECC204 == dev_type)
        {
#if defined(ATCA_ECC204_SUPPORT)
            status = calib_ecc204_write_config_zone(_gDevice, config_data);
#endif
        }
        else
        {
            status = calib_write_config_zone(_gDevice, config_data);
        }
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
#if ATCA_TA_SUPPORT
        status = talib_write_config_zone(_gDevice, config_data);
#endif
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Executes the Write command, which performs an encrypted write of
 *          a 32 byte block into given slot.
 *
 * The function takes clear text bytes and encrypts them for writing over the
 * wire. Data zone must be locked and the slot configuration must be set to
 * encrypted write for the block to be successfully written.
 *
 *  \param[in] key_id      Slot ID to write to.
 *  \param[in] block       Index of the 32 byte block to write in the slot.
 *  \param[in] data        32 bytes of clear text data to be written to the slot
 *  \param[in] enc_key     WriteKey to encrypt with for writing
 *  \param[in] enc_key_id  The KeyID of the WriteKey
 *  \param[in]  num_in       20 byte host nonce to inject into Nonce calculation
 *
 *  returns ATCA_SUCCESS on success, otherwise an error code.
 */
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_write_enc(uint16_t key_id, uint8_t block, const uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id)
#else
ATCA_STATUS atcab_write_enc(uint16_t key_id, uint8_t block, const uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
        status = calib_write_enc(_gDevice, key_id, block, data, enc_key, enc_key_id);
#else
        status = calib_write_enc(_gDevice, key_id, block, data, enc_key, enc_key_id, num_in);
#endif
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

/** \brief Initialize one of the monotonic counters in device with a specific
 *          value.
 *
 * The monotonic counters are stored in the configuration zone using a special
 * format. This encodes a binary count value into the 8 byte encoded value
 * required. Can only be set while the configuration zone is unlocked.
 *
 * \param[in]  counter_id     Counter to be written.
 * \param[in]  counter_value  Counter value to set.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_write_config_counter(uint16_t counter_id, uint32_t counter_value)
{
    ATCA_STATUS status = ATCA_UNIMPLEMENTED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if ATCA_CA_SUPPORT
        status = calib_write_config_counter(_gDevice, counter_id, counter_value);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    else
    {
        status = ATCA_NOT_INITIALIZED;
    }
    return status;
}

#endif
