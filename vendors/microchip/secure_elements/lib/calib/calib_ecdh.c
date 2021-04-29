/**
 * \file
 * \brief CryptoAuthLib Basic API methods for ECDH command.
 *
 * The ECDH command implements the Elliptic Curve Diffie-Hellman algorithm to
 * combine an internal private key with an external public key to calculate a
 * shared secret.
 *
 * \note List of devices that support this command - ATECC508A, ATECC608A/B.
 *       There are differences in  the modes that they support. Refer to device
 *       datasheets for full details.
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
#include "host/atca_host.h"

/** \brief Base function for generating premaster secret key using ECDH.
 *  \param[in]  device      Device context pointer
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
ATCA_STATUS calib_ecdh_base(ATCADevice device, uint8_t mode, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, uint8_t* out_nonce)
{
    ATCAPacket packet;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    do
    {
        if (device == NULL)
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
            break;
        }

        // Build Command
        packet.param1 = mode;
        packet.param2 = key_id;
        memcpy(packet.data, public_key, ATCA_PUB_KEY_SIZE);

        if ((status = atECDH(atcab_get_device_type_ext(device), &packet)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "atECDH - failed");
            break;
        }

        if ((status = atca_execute_command(&packet, device)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "calib_ecdh_base - execution failed");
            break;
        }

        if (pms != NULL && packet.data[ATCA_COUNT_IDX] >= (3 + ATCA_KEY_SIZE))
        {
            memcpy(pms, &packet.data[ATCA_RSP_DATA_IDX], ATCA_KEY_SIZE);
        }

        if (out_nonce != NULL && packet.data[ATCA_COUNT_IDX] >= (3 + ATCA_KEY_SIZE * 2))
        {
            memcpy(out_nonce, &packet.data[ATCA_RSP_DATA_IDX + ATCA_KEY_SIZE], ATCA_KEY_SIZE);
        }

    }
    while (0);

    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is returned in the clear.
 *
 *  \param[in] device     Device context pointer
 *  \param[in] key_id     Slot of key for ECDH computation
 *  \param[in] public_key Public key input to ECDH calculation. X and Y
 *                        integers in big-endian format. 64 bytes for P256
 *                        key.
 *  \param[out] pms       Computed ECDH premaster secret is returned here.
 *                        32 bytes.
 *
 *  \return ATCA_SUCCESS on success
 */
ATCA_STATUS calib_ecdh(ATCADevice device, uint16_t key_id, const uint8_t* public_key, uint8_t* pms)
{
    ATCA_STATUS status;

    status = calib_ecdh_base(device, ECDH_PREFIX_MODE, key_id, public_key, pms, NULL);

    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is read from the next slot.
 *
 * This function only works for even numbered slots with the proper
 * configuration.
 *
 *  \param[in]  device       Device context pointer
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
ATCA_STATUS calib_ecdh_enc(ATCADevice device, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id)
#else
ATCA_STATUS calib_ecdh_enc(ATCADevice device, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_SUCCESS;

    do
    {
        // Check the inputs
        if (public_key == NULL || pms == NULL || read_key == NULL)
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
            break;
        }

        // Send the ECDH command with the public key provided
        if ((status = calib_ecdh(device, key_id, public_key, NULL)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "ECDH Failed"); break;
        }
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
        if ((status = calib_read_enc(device, key_id | 0x0001, 0, pms, read_key, read_key_id)) != ATCA_SUCCESS)
#else
        if ((status = atcab_read_enc(key_id | 0x0001, 0, pms, read_key, read_key_id, num_in)) != ATCA_SUCCESS)
#endif
        {
            ATCA_TRACE(status, "Encrypted read failed"); break;
        }
    }
    while (0);

    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is returned encrypted using the IO protection key.
 *
 *  \param[in]  device       Device context pointer
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
ATCA_STATUS calib_ecdh_ioenc(ATCADevice device, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key)
{
    uint8_t mode = ECDH_MODE_SOURCE_EEPROM_SLOT | ECDH_MODE_OUTPUT_ENC | ECDH_MODE_COPY_OUTPUT_BUFFER;
    uint8_t out_nonce[ATCA_KEY_SIZE];
    atca_io_decrypt_in_out_t io_dec_params;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Perform ECDH operation requesting output buffer encryption
    if (ATCA_SUCCESS != (status = calib_ecdh_base(device, mode, key_id, public_key, pms, out_nonce)))
    {
        return ATCA_TRACE(status, "calib_ecdh_base - failed");
    }

    // Decrypt PMS
    memset(&io_dec_params, 0, sizeof(io_dec_params));
    io_dec_params.io_key = io_key;
    io_dec_params.out_nonce = out_nonce;
    io_dec_params.data = pms;
    io_dec_params.data_size = 32;
    if (ATCA_SUCCESS != (status = atcah_io_decrypt(&io_dec_params)))
    {
        return ATCA_TRACE(status, "atcah_io_decrypt - failed");
    }

    return status;
}

/** \brief ECDH command with a private key in TempKey and the premaster secret
 *         is returned in the clear.
 *
 *  \param[in]  device      Device context pointer
 *  \param[in]  public_key  Public key input to ECDH calculation. X and Y
 *                          integers in big-endian format. 64 bytes for P256
 *                          key.
 *  \param[out] pms         Computed ECDH premaster secret is returned here
 *                          (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_ecdh_tempkey(ATCADevice device, const uint8_t* public_key, uint8_t* pms)
{
    // Perform ECDH operation with TempKey
    uint8_t mode = ECDH_MODE_SOURCE_TEMPKEY | ECDH_MODE_COPY_OUTPUT_BUFFER;

    return calib_ecdh_base(device, mode, 0x0000, public_key, pms, NULL);
}

/** \brief ECDH command with a private key in TempKey and the premaster secret
 *         is returned encrypted using the IO protection key.
 *
 *  \param[in]  device      Device context pointer
 *  \param[in]  public_key  Public key input to ECDH calculation. X and Y
 *                          integers in big-endian format. 64 bytes for P256
 *                          key.
 *  \param[out] pms         Computed ECDH premaster secret is returned here
 *                          (32 bytes).
 *  \param[in]  io_key      IO protection key.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_ecdh_tempkey_ioenc(ATCADevice device, const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key)
{
    uint8_t mode = ECDH_MODE_SOURCE_TEMPKEY | ECDH_MODE_OUTPUT_ENC | ECDH_MODE_COPY_OUTPUT_BUFFER;
    uint8_t out_nonce[ATCA_KEY_SIZE];
    atca_io_decrypt_in_out_t io_dec_params;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Perform ECDH operation requesting output buffer encryption
    if (ATCA_SUCCESS != (status = calib_ecdh_base(device, mode, 0x0000, public_key, pms, out_nonce)))
    {
        return ATCA_TRACE(status, "calib_ecdh_base - failed");
    }

    // Decrypt PMS
    memset(&io_dec_params, 0, sizeof(io_dec_params));
    io_dec_params.io_key = io_key;
    io_dec_params.out_nonce = out_nonce;
    io_dec_params.data = pms;
    io_dec_params.data_size = 32;
    if (ATCA_SUCCESS != (status = atcah_io_decrypt(&io_dec_params)))
    {
        return ATCA_TRACE(status, "atcah_io_decrypt - failed");
    }

    return status;
}

