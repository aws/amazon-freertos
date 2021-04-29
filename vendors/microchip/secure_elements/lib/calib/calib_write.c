/**
 * \file
 * \brief CryptoAuthLib Basic API methods for Write command.
 *
 * The Write command writes either one 4-byte word or a 32-byte block to one of
 * the EEPROM zones on the device. Depending upon the value of the WriteConfig
 * byte for a slot, the data may be required to be encrypted by the system prior
 * to being sent to the device
 *
 * \note List of devices that support this command - ATSHA204A, ATECC108A,
 *       ATECC508A, and ATECC608A/B. There are differences in the modes that they
 *       support. Refer to device datasheets for full details.
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

/**
 * \brief Executes the Write command, which writes either one four byte word or
 *        a 32-byte block to one of the EEPROM zones on the device. Depending
 *        upon the value of the WriteConfig byte for this slot, the data may be
 *        required to be encrypted by the system prior to being sent to the
 *        device. This command cannot be used to write slots configured as ECC
 *        private keys.
 *
 * \param[in] device   Device context pointer
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
ATCA_STATUS calib_write(ATCADevice device, uint8_t zone, uint16_t address, const uint8_t *value, const uint8_t *mac)
{
    ATCAPacket packet;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    if ((device == NULL) || (value == NULL))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
    }

    do
    {
        // Build the write command
        packet.param1 = zone;
        packet.param2 = address;
        if (zone & ATCA_ZONE_READWRITE_32)
        {
            // 32-byte write
            memcpy(packet.data, value, 32);
            // Only 32-byte writes can have a MAC
            if (mac)
            {
                memcpy(&packet.data[32], mac, 32);
            }
        }
        else
        {
            // 4-byte write
            memcpy(packet.data, value, 4);
        }

        if ((status = atWrite(atcab_get_device_type_ext(device), &packet, mac && (zone & ATCA_ZONE_READWRITE_32))) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "atWrite - failed");
            break;
        }

        if ((status = atca_execute_command(&packet, device)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "calib_write - execution failed");
            break;
        }

    }
    while (0);

    return status;
}

/** \brief Executes the Write command, which writes either 4 or 32 bytes of
 *          data into a device zone.
 *
 *  \param[in] device  Device context pointer
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
ATCA_STATUS calib_write_zone(ATCADevice device, uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, const uint8_t *data, uint8_t len)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint16_t addr;

    // Check the input parameters
    if (data == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
    }

    if (len != 4 && len != 32)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Invalid length received");
    }

    do
    {
        // The get address function checks the remaining variables
        if ((status = calib_get_addr(zone, slot, block, offset, &addr)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "calib_get_addr - failed");
            break;
        }

        // If there are 32 bytes to write, then xor the bit into the mode
        if (len == ATCA_BLOCK_SIZE)
        {
            zone = zone | ATCA_ZONE_READWRITE_32;
        }

        status = calib_write(device, zone, addr, data, NULL);

    }
    while (0);

    return status;
}


/** \brief Executes the Write command, which performs an encrypted write of
 *          a 32 byte block into given slot.
 *
 * The function takes clear text bytes and encrypts them for writing over the
 * wire. Data zone must be locked and the slot configuration must be set to
 * encrypted write for the block to be successfully written.
 *
 *  \param[in] device      Device context pointer
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
ATCA_STATUS calib_write_enc(ATCADevice device, uint16_t key_id, uint8_t block, const uint8_t *data, const uint8_t* enc_key, const uint16_t enc_key_id)
{
    uint8_t num_in[NONCE_NUMIN_SIZE] = { 0 };

#else
ATCA_STATUS calib_write_enc(ATCADevice device, uint16_t key_id, uint8_t block, const uint8_t *data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
{
#endif
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t zone = ATCA_ZONE_DATA | ATCA_ZONE_READWRITE_32;
    atca_nonce_in_out_t nonce_params;
    atca_gen_dig_in_out_t gen_dig_param;
    atca_write_mac_in_out_t write_mac_param;
    atca_temp_key_t temp_key;
    uint8_t serial_num[32];
    uint8_t rand_out[RANDOM_NUM_SIZE] = { 0 };
    uint8_t cipher_text[ATCA_KEY_SIZE] = { 0 };
    uint8_t mac[WRITE_MAC_SIZE] = { 0 };
    uint8_t other_data[4] = { 0 };
    uint16_t addr;

    do
    {
        // Verify inputs parameters
        if (data == NULL || enc_key == NULL)
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
            break;
        }

        // Read the device SN
        if ((status = calib_read_zone(device, ATCA_ZONE_CONFIG, 0, 0, 0, serial_num, 32)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "calib_read_zone - failed");
            break;
        }
        // Make the SN continuous by moving SN[4:8] right after SN[0:3]
        memmove(&serial_num[4], &serial_num[8], 5);


        // Random Nonce inputs
        memset(&temp_key, 0, sizeof(temp_key));
        memset(&nonce_params, 0, sizeof(nonce_params));
        nonce_params.mode = NONCE_MODE_SEED_UPDATE;
        nonce_params.zero = 0;
        nonce_params.num_in = &num_in[0];
        nonce_params.rand_out = rand_out;
        nonce_params.temp_key = &temp_key;

        // Send the random Nonce command
        if ((status = calib_nonce_rand(device, num_in, rand_out)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "Nonce failed");
            break;
        }

        // Calculate Tempkey
        if ((status = atcah_nonce(&nonce_params)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "Calc TempKey failed");
            break;
        }

        // Supply OtherData so GenDig behavior is the same for keys with SlotConfig.NoMac set
        other_data[0] = ATCA_GENDIG;
        other_data[1] = GENDIG_ZONE_DATA;
        other_data[2] = (uint8_t)(enc_key_id);
        other_data[3] = (uint8_t)(enc_key_id >> 8);

        // Send the GenDig command
        if ((status = calib_gendig(device, GENDIG_ZONE_DATA, enc_key_id, other_data, sizeof(other_data))) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "GenDig failed");
            break;
        }

        // Calculate Tempkey
        // NoMac bit isn't being considered here on purpose to remove having to read SlotConfig.
        // OtherData is built to get the same result regardless of the NoMac bit.
        memset(&gen_dig_param, 0, sizeof(gen_dig_param));
        gen_dig_param.key_id = enc_key_id;
        gen_dig_param.is_key_nomac = false;
        gen_dig_param.sn = serial_num;
        gen_dig_param.stored_value = enc_key;
        gen_dig_param.zone = GENDIG_ZONE_DATA;
        gen_dig_param.other_data = other_data;
        gen_dig_param.temp_key = &temp_key;
        if ((status = atcah_gen_dig(&gen_dig_param)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "atcah_gen_dig() failed");
            break;
        }

        // The get address function checks the remaining variables
        if ((status = calib_get_addr(ATCA_ZONE_DATA, key_id, block, 0, &addr)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "Get address failed");
            break;
        }

        // Setting bit 6 to indicate input data is encrypted
        write_mac_param.zone = zone | ATCA_ZONE_ENCRYPTED;
        write_mac_param.key_id = addr;
        write_mac_param.sn = serial_num;
        write_mac_param.input_data = data;
        write_mac_param.encrypted_data = cipher_text;
        write_mac_param.auth_mac = mac;
        write_mac_param.temp_key = &temp_key;

        if ((status = atcah_write_auth_mac(&write_mac_param)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "Calculate Auth MAC failed");
            break;
        }

        status = calib_write(device, write_mac_param.zone, write_mac_param.key_id, write_mac_param.encrypted_data, write_mac_param.auth_mac);

    }
    while (0);

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
 *  \param[in]  device      Device context pointer
 *  \param[in] config_data  Data to the config zone data. This should be 88
 *                          bytes for SHA devices and 128 bytes for ECC
 *                          devices.
 *
 *  \returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_write_config_zone(ATCADevice device, const uint8_t* config_data)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    size_t config_size = 0;

    if (config_data == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
    }

    do
    {
        // Get config zone size for the device
        if (ATCA_SUCCESS != (status = calib_get_zone_size(device, ATCA_ZONE_CONFIG, 0, &config_size)))
        {
            ATCA_TRACE(status, "calib_get_zone_size - failed");
            break;
        }

        // Write config zone excluding UserExtra and Selector
        if (ATCA_SUCCESS != (status = calib_write_bytes_zone(device, ATCA_ZONE_CONFIG, 0, 16, &config_data[16], config_size - 16)))
        {
            ATCA_TRACE(status, "calib_write_bytes_zone - failed");
            break;
        }

        // Write the UserExtra and Selector. This may fail if either value is already non-zero.
        if (ATCA_SUCCESS != (status = calib_updateextra(device, UPDATE_MODE_USER_EXTRA, config_data[84])))
        {
            ATCA_TRACE(status, "calib_updateextra - failed");
            break;
        }

        if (ATCA_SUCCESS != (status = calib_updateextra(device, UPDATE_MODE_SELECTOR, config_data[85])))
        {
            ATCA_TRACE(status, "calib_updateextra - failed");
            break;
        }
    }
    while (0);

    return status;
}

/** \brief Uses the write command to write a public key to a slot in the
 *         proper format.
 *
 *  \param[in] device     Device context pointer
 *  \param[in] slot        Slot number to write. Only slots 8 to 15 are large
 *                         enough to store a public key.
 *  \param[in] public_key  Public key to write into the slot specified. X and Y
 *                         integers in big-endian format. 64 bytes for P256
 *                         curve.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_write_pubkey(ATCADevice device, uint16_t slot, const uint8_t *public_key)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t public_key_formatted[ATCA_BLOCK_SIZE * 3];
    int block;

    // Check the pointers
    if (public_key == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
    }

    // The 64 byte P256 public key gets written to a 72 byte slot in the following pattern
    // | Block 1                     | Block 2                                      | Block 3       |
    // | Pad: 4 Bytes | PubKey[0:27] | PubKey[28:31] | Pad: 4 Bytes | PubKey[32:55] | PubKey[56:63] |

    memset(public_key_formatted, 0, sizeof(public_key_formatted));
    memcpy(&public_key_formatted[4], &public_key[0], 32);   // Move X to padded position
    memcpy(&public_key_formatted[40], &public_key[32], 32); // Move Y to padded position

    // Using this instead of calib_write_zone_bytes, as that function doesn't work when
    // the data zone is unlocked
    for (block = 0; block < 3; block++)
    {
        if (ATCA_SUCCESS != (status = calib_write_zone(device, ATCA_ZONE_DATA, slot, block, 0, &public_key_formatted[ATCA_BLOCK_SIZE * block], ATCA_BLOCK_SIZE)))
        {
            ATCA_TRACE(status, "calib_write_zone - failed");
            break;
        }
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
 *  \param[in] device        Device context pointer
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
ATCA_STATUS calib_write_bytes_zone(ATCADevice device, uint8_t zone, uint16_t slot, size_t offset_bytes, const uint8_t *data, size_t length)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    size_t zone_size = 0;
    size_t data_idx = 0;
    size_t cur_block = 0;
    size_t cur_word = 0;

    if (zone != ATCA_ZONE_CONFIG && zone != ATCA_ZONE_OTP && zone != ATCA_ZONE_DATA)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received");
    }
    if (zone == ATCA_ZONE_DATA && slot > 15)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Invalid slot received");
    }
    if (length == 0)
    {
        return ATCA_SUCCESS;  // Always succeed writing 0 bytes
    }
    if (data == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
    }
    if (offset_bytes % ATCA_WORD_SIZE != 0 || length % ATCA_WORD_SIZE != 0)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Either Invalid length or offset received");
    }

    do
    {
        if (ATCA_SUCCESS != (status = calib_get_zone_size(device, zone, slot, &zone_size)))
        {
            ATCA_TRACE(status, "calib_get_zone_size - failed");
            break;
        }
        if (offset_bytes + length > zone_size)
        {
            return ATCA_TRACE(ATCA_BAD_PARAM, "Invalid parameter received");
        }

        cur_block = offset_bytes / ATCA_BLOCK_SIZE;
        cur_word = (offset_bytes % ATCA_BLOCK_SIZE) / ATCA_WORD_SIZE;

        while (data_idx < length)
        {
            // The last item makes sure we handle the selector, user extra, and lock bytes in the config properly
            if (cur_word == 0 && length - data_idx >= ATCA_BLOCK_SIZE && !(zone == ATCA_ZONE_CONFIG && cur_block == 2))
            {
                if (ATCA_SUCCESS != (status = calib_write_zone(device, zone, slot, (uint8_t)cur_block, 0, &data[data_idx], ATCA_BLOCK_SIZE)))
                {
                    ATCA_TRACE(status, "calib_write_zone - failed");
                    break;
                }
                data_idx += ATCA_BLOCK_SIZE;
                cur_block += 1;
            }
            else
            {
                // Skip trying to change UserExtra, Selector, LockValue, and LockConfig which require the UpdateExtra command to change
                if (!(zone == ATCA_ZONE_CONFIG && cur_block == 2 && cur_word == 5))
                {
                    if (ATCA_SUCCESS != (status = calib_write_zone(device, zone, slot, (uint8_t)cur_block, (uint8_t)cur_word, &data[data_idx], ATCA_WORD_SIZE)))
                    {
                        ATCA_TRACE(status, "calib_write_zone - failed");
                        break;
                    }
                }
                data_idx += ATCA_WORD_SIZE;
                cur_word += 1;
                if (cur_word == ATCA_BLOCK_SIZE / ATCA_WORD_SIZE)
                {
                    cur_block += 1;
                    cur_word = 0;
                }
            }
        }
    }
    while (false);

    return status;
}

/** \brief Initialize one of the monotonic counters in device with a specific
 *          value.
 *
 * The monotonic counters are stored in the configuration zone using a special
 * format. This encodes a binary count value into the 8 byte encoded value
 * required. Can only be set while the configuration zone is unlocked.
 *
 * \param[in]  device         Device context pointer
 * \param[in]  counter_id     Counter to be written.
 * \param[in]  counter_value  Counter value to set.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_write_config_counter(ATCADevice device, uint16_t counter_id, uint32_t counter_value)
{
    uint16_t lin_a, lin_b, bin_a, bin_b;
    uint8_t bytes[8];
    uint8_t idx = 0;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    if (counter_id > 1 || counter_value > COUNTER_MAX_VALUE)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Either invalid counter id or counter value received");
    }

    lin_a = 0xFFFF >> (counter_value % 32);
    lin_b = 0xFFFF >> ((counter_value >= 16) ? (counter_value - 16) % 32 : 0);
    bin_a = counter_value / 32;
    bin_b = (counter_value >= 16) ? ((counter_value - 16) / 32) : 0;

    bytes[idx++] = lin_a >> 8;
    bytes[idx++] = lin_a & 0xFF;
    bytes[idx++] = lin_b >> 8;
    bytes[idx++] = lin_b & 0xFF;

    bytes[idx++] = bin_a >> 8;
    bytes[idx++] = bin_a & 0xFF;
    bytes[idx++] = bin_b >> 8;
    bytes[idx++] = bin_b & 0xFF;

    status = calib_write_bytes_zone(device, ATCA_ZONE_CONFIG, 0, 52 + counter_id * 8, bytes, sizeof(bytes));

    return status;
}

#if defined(ATCA_ECC204_SUPPORT)
/** \brief Execute write command to write either 16 byte or 32 byte to one of the EEPROM zones
 *         on the ECC204 device.
 *
 *  \param[in] device   Device context pointer
 *  \param[in] zone     Zone/Param1 for the write command.
 *  \param[in] address  Address/Param2 for the write command.
 *  \param[in] value    Plain-text data to be written or cipher-text for
 *                      encrypted writes. 32 or 16 bytes depending on zone.
 *  \param[in] mac      MAC required for encrypted writes (32 bytes). Set to NULL
 *                      if not required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_ecc204_write(ATCADevice device, uint8_t zone, uint16_t address, const uint8_t *value,
                               const uint8_t *mac)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    ATCAPacket packet;

    if ((NULL == device) && (NULL == value))
    {
        status = ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    if (ATCA_SUCCESS == status)
    {
        packet.param1 = zone;
        packet.param2 = address;

        if (ATCA_ECC204_ZONE_CONFIG == zone)
        {
            memcpy(packet.data, value, 16);
        }
        else if (ATCA_ECC204_ZONE_DATA == zone)
        {
            memcpy(packet.data, value, ATCA_BLOCK_SIZE);
        }
        else
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received");
        }

        if (ATCA_SUCCESS == status)
        {
            if (mac && (ATCA_ECC204_ZONE_DATA == zone))
            {
                memcpy(&packet.data[ATCA_BLOCK_SIZE], mac, MAC_SIZE);
            }

            (void)atWrite(atcab_get_device_type_ext(device), &packet, mac && (ATCA_ECC204_ZONE_DATA == zone));
        }
    }

    if (ATCA_SUCCESS == status)
    {
        if (ATCA_SUCCESS != (status = atca_execute_command(&packet, device)))
        {
            ATCA_TRACE(status, "calib_ecc204_write - execution failed");
        }
    }

    return status;

}

/** \brief Execute write command to write data into configuration zone or data zone
 *         This function only support ECC204 device
 *
 *  \param[in]    device      Device context pointer
 *  \param[in]    zone        Device zone to write (config=1, data=0)
 *  \param[in]    slot        the slot number to be witten
 *  \param[in]    block       32-byte block to write
 *  \param[in]    offset      ignore for ECC204 device
 *  \param[in]    data        Data to be written into slot
 *  \param[in]    len         Number of bytes to be written. Must be either 16 or 32.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code
 */
ATCA_STATUS calib_ecc204_write_zone(ATCADevice device, uint8_t zone, uint16_t slot, uint8_t block,
                                    uint8_t offset, const uint8_t *data, uint8_t len)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint16_t addr;

    if ((NULL == device) && (NULL == data))
    {
        status = ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }
    else if (((ATCA_ECC204_ZONE_CONFIG == zone) && (16 != len)) ||
             ((ATCA_ECC204_ZONE_DATA == zone) && (ATCA_BLOCK_SIZE != len)))
    {
        status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid length received");
    }

    if (ATCA_SUCCESS == status)
    {
        if (ATCA_SUCCESS != (status = calib_ecc204_get_addr(zone, slot, block, 0, &addr)))
        {
            ATCA_TRACE(status, "calib_ecc204_get_addr - failed");
        }

        if (ATCA_SUCCESS == status)
        {
            status = calib_ecc204_write(device, zone, addr, data, NULL);
        }
    }

    return status;
}

/** \brief Use write command to write configuration data into ECC204 config zone
 *
 *  \param[in]  device       Device context pointer
 *  \param[in]  config_data  configuration data
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code
 */
ATCA_STATUS calib_ecc204_write_config_zone(ATCADevice device, uint8_t* config_data)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t slot = 1;

    if ((NULL == device) || (NULL == config_data))
    {
        status = ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    if (ATCA_SUCCESS == status)
    {
        while (slot <= 3)
        {
            if (ATCA_SUCCESS != (status = calib_ecc204_write_zone(device, ATCA_ECC204_ZONE_CONFIG, slot,
                                                                  0, 0, &config_data[16 * slot], 16)))
            {
                ATCA_TRACE(status, "calib_ecc204_write_zone - failed");
            }
            slot += 1; // Increment slot
        }
    }

    return status;
}

/** \brief Executes write command, performs an encrypted write of a 32 byte block into given slot.
 *
 *  \param[in]  device          Device context pointer
 *  \param[in]  slot            key slot to be written
 *  \param[in]  data            32 bytes of clear text data
 *  \param[in]  transport_key   Transport key
 *  \param[in]  key_id          Transport key id
 *  \param[in]  num_in          20 byte host nonce to inject into Nonce calculation
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code
 */
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS calib_ecc204_write_enc(ATCADevice device, uint8_t slot, uint8_t* data, uint8_t* transport_key,
                                   uint8_t transport_key_id)
{
    uint8_t num_in[NONCE_NUMIN_SIZE] = { 0 };

#else
ATCA_STATUS calib_ecc204_write_enc(ATCADevice device, uint8_t slot, uint8_t* data, uint8_t* transport_key,
                                   uint8_t transport_key_id, uint8_t num_in[NONCE_NUMIN_SIZE])
{
#endif
    ATCA_STATUS status = ATCA_SUCCESS;
    atca_nonce_in_out_t nonce_params;
    atca_write_mac_in_out_t write_mac_param;
    atca_temp_key_t temp_key;
    atca_session_key_in_out_t session_key_params;
    uint8_t rand_out[RANDOM_NUM_SIZE] = { 0 };
    uint8_t serial_number[ATCA_SERIAL_NUM_SIZE] = { 0 };
    uint8_t session_key[ATCA_KEY_SIZE] = { 0 };
    uint8_t cipher_text[ATCA_KEY_SIZE] = { 0 };
    uint8_t mac[WRITE_MAC_SIZE] = { 0 };
    uint16_t addr;

    if ((NULL == data) || (NULL == transport_key))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encounteed");
    }

    do
    {
        // Read device serial number
        if (ATCA_SUCCESS != (status = calib_ecc204_read_serial_number(device, serial_number)))
        {
            ATCA_TRACE(status, "Read serial number failed");
            break;
        }

        // Generate session key on device
        if (ATCA_SUCCESS != (status = calib_nonce_gen_session_key(device, transport_key_id, num_in, rand_out)))
        {
            ATCA_TRACE(status, "Session key generation failed");
            break;
        }

        // Random Nonce inputs
        memset(&temp_key, 0, sizeof(temp_key));
        memset(&nonce_params, 0, sizeof(nonce_params));
        nonce_params.mode = NONCE_MODE_SEED_UPDATE;
        nonce_params.zero = transport_key_id;
        nonce_params.num_in = &num_in[0];
        nonce_params.rand_out = rand_out;
        nonce_params.temp_key = &temp_key;

        // Calculate Nonce
        if ((status = atcah_nonce(&nonce_params)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "Calculate nonce failed");
            break;
        }

        // Session key inputs
        memset(&session_key_params, 0, sizeof(session_key_params));
        session_key_params.transport_key = transport_key;
        session_key_params.transport_key_id = transport_key_id;
        session_key_params.sn = serial_number;
        session_key_params.nonce = temp_key.value;
        session_key_params.session_key = session_key;

        // calculate session key on host
        if (ATCA_SUCCESS != (status = atcah_gen_session_key(&session_key_params)))
        {
            ATCA_TRACE(status, "Host session key generation failed");
            break;
        }

        if (ATCA_SUCCESS != (status = calib_ecc204_get_addr(ATCA_ECC204_ZONE_DATA, slot, 0, 0, &addr)))
        {
            ATCA_TRACE(status, "Calculate slot address failed");
            break;
        }

        // copy session key into temp variable
        memcpy(temp_key.value, session_key, ATCA_KEY_SIZE);

        // Write mac inputs
        write_mac_param.zone = ATCA_ECC204_ZONE_DATA;
        write_mac_param.key_id = addr;
        write_mac_param.sn = serial_number;
        write_mac_param.input_data = data;
        write_mac_param.encrypted_data = cipher_text;
        write_mac_param.auth_mac = mac;
        write_mac_param.temp_key = &temp_key;

        // calculate MAC on host
        if (ATCA_SUCCESS != (status = atcah_ecc204_write_auth_mac(&write_mac_param)))
        {
            ATCA_TRACE(status, "Data encryption failed");
            break;
        }

        status = calib_ecc204_write(device, write_mac_param.zone, write_mac_param.key_id, write_mac_param.encrypted_data, write_mac_param.auth_mac);
    }
    while (0);

    return status;
}

/** \brief Use Write command to write bytes
 *
 * This function will issue the write command as many times as is required to
 * read the requested data.
 *
 *  \param[in]   device        Device context pointer
 *  \param[in]   zone          zone to write data to. Option are
 *                             ATCA_ECC204_ZONE_CONFIG(1), ATCA_ECC204_ZONE_DATA(0)
 *  \param[in]   slot          slot number to write to.
 *  \param[in]   block         offset bytes ignored
 *  \param[in]   data          data to be written
 *  \param[in]   length        number of bytes to e written
 *
 *  \return ATCA_SUCCESS on success, otheriwse an error code
 */
ATCA_STATUS calib_ecc204_write_bytes_zone(ATCADevice device, uint8_t zone, uint16_t slot, size_t block,
                                          const uint8_t *data, size_t length)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t block_size = (zone == ATCA_ECC204_ZONE_CONFIG) ? ATCA_ECC204_CONFIG_SLOT_SIZE : ATCA_BLOCK_SIZE;
    uint8_t no_of_blocks;
    uint8_t data_idx = 0;

    if ((NULL == device) || (NULL == data))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Encountered NULL pointer");
    }
    else if ((ATCA_ECC204_ZONE_DATA == zone) && (((length > 64) && (2 == slot)) ||
                                                 ((length > 320) && (3 == slot)) || (1 == slot) || (0 == slot)))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Invalid parameter received");
    }
    else if (0 == length)
    {
        return ATCA_SUCCESS;
    }

    no_of_blocks = length / block_size;
    while (no_of_blocks--)
    {
        if (ATCA_SUCCESS != (status = calib_ecc204_write_zone(device, zone, slot, (uint8_t)block, 0,
                                                              &data[block_size * data_idx],
                                                              block_size)))
        {
            ATCA_TRACE(status, "calib_ecc204_write_zone failed");
            break;
        }

        block += 1;      // Read next block
        data_idx += 1;   // increment data index
    }
}

#endif