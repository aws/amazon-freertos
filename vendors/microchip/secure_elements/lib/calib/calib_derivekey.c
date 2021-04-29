/**
 * \file
 * \brief CryptoAuthLib Basic API methods for DeriveKey command.
 *
 * The DeriveKey command combines the current value of a key with the nonce
 * stored in TempKey using SHA-256 and derives a new key.
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

/** \brief Executes the DeviveKey command for deriving a new key from a
 *          nonce (TempKey) and an existing key.
 *
 *  \param[in] device      Device context pointer
 *  \param[in] mode        Bit 2 must match the value in TempKey.SourceFlag
 *  \param[in] target_key  Key slot to be written
 *  \param[in] mac         Optional 32 byte MAC used to validate operation. NULL
 *                         if not required.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_derivekey(ATCADevice device, uint8_t mode, uint16_t target_key, const uint8_t* mac)
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

        // build a deriveKey command (pass through mode)
        packet.param1 = mode;
        packet.param2 = target_key;

        if (mac != NULL)
        {
            memcpy(packet.data, mac, MAC_SIZE);
        }

        if ((status = atDeriveKey(atcab_get_device_type_ext(device), &packet, mac != NULL)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "atDeriveKey - failed");
            break;
        }

        if ((status = atca_execute_command(&packet, device)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "calib_derivekey - execution failed");
            break;
        }

    }
    while (0);

    return status;
}
