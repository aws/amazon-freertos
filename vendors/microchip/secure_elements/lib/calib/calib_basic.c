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

#include "cryptoauthlib.h"

/** \brief basic API methods are all prefixed with atcab_  (CryptoAuthLib Basic)
 *  the fundamental premise of the basic API is it is based on a single interface
 *  instance and that instance is global, so all basic API commands assume that
 *  one global device is the one to operate on.
 */

ATCA_STATUS calib_wakeup_i2c(ATCADevice device)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    ATCAIface iface = atGetIFace(device);

    if (iface)
    {
        uint8_t retries = atca_iface_get_retries(iface);
        uint8_t address = atcab_get_device_address(device);
        uint32_t temp;
        uint16_t rxlen;

        do
        {
            if (100000UL < iface->mIfaceCFG->atcai2c.baud)
            {
                temp = 100000UL;
                status = atcontrol(iface, ATCA_HAL_CHANGE_BAUD, &temp, sizeof(temp));
                if (ATCA_UNIMPLEMENTED == status)
                {
                    status = atcontrol(iface, ATCA_HAL_CONTROL_WAKE, NULL, 0);
                    break;
                }
            }
            else
            {
                status = ATCA_SUCCESS;
            }

    #ifdef ATCA_ECC204_SUPPORT
            if (ECC204 == atcab_get_device_type_ext(device))
            {
                (void)atsend(iface, address, NULL, 0);
            }
            else
            {

                (void)atsend(iface, 0x00, NULL, 0);
            }
    #else
            (void)atsend(iface, 0x00, NULL, 0);
    #endif
            atca_delay_us(atca_iface_get_wake_delay(iface));

            rxlen = sizeof(temp);
            if (ATCA_SUCCESS == status)
            {
                status = atreceive(iface, address, (uint8_t*)&temp, &rxlen);
            }

            if ((ATCA_SUCCESS == status) && (100000UL < iface->mIfaceCFG->atcai2c.baud))
            {
                temp = iface->mIfaceCFG->atcai2c.baud;
                status = atcontrol(iface, ATCA_HAL_CHANGE_BAUD, &temp, sizeof(temp));
            }

            if (ATCA_SUCCESS == status)
            {
                status = hal_check_wake((uint8_t*)&temp, rxlen);
            }
        }
        while (0 < retries-- && ATCA_SUCCESS != status);
    }
    return status;
}

/** \brief wakeup the CryptoAuth device
 *  \param[in] device     Device context pointer
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_wakeup(ATCADevice device)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    ATCAIface iface = atGetIFace(device);

    if (iface && iface->mIfaceCFG)
    {
#ifdef ATCA_HAL_LEGACY_API
        status = atwake(iface);
#else
        if (atca_iface_is_kit(iface) || (ATCA_SWI_IFACE == iface->mIfaceCFG->iface_type))
        {
            status = atwake(iface);
        }
        else if (ATCA_I2C_IFACE == iface->mIfaceCFG->iface_type)
        {
            status = calib_wakeup_i2c(device);
        }
        else
        {
            status = ATCA_SUCCESS;
        }
    }

#endif
        return status;
    }

/** \brief idle the CryptoAuth device
 *  \param[in] device     Device context pointer
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
    ATCA_STATUS calib_idle(ATCADevice device)
    {
        ATCA_STATUS status = ATCA_BAD_PARAM;

#ifdef ATCA_HAL_LEGACY_API
        status = atidle(device->mIface);
#else
        if (atca_iface_is_kit(&device->mIface) || (ATCA_SWI_IFACE == device->mIface.mIfaceCFG->iface_type))
        {
            status = atidle(&device->mIface);
        }
        else
        {
            if (ECC204 != atcab_get_device_type_ext(device))
            {
                uint8_t command = 0x02;
                status = atsend(&device->mIface, atcab_get_device_address(device), &command, 1);
            }
            else
            {
                status = ATCA_SUCCESS;
            }
        }
#endif
        return status;
    }

/** \brief invoke sleep on the CryptoAuth device
 *  \param[in] device     Device context pointer
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
    ATCA_STATUS calib_sleep(ATCADevice device)
    {
        ATCA_STATUS status = ATCA_BAD_PARAM;

#ifdef ATCA_HAL_LEGACY_API
        status = atsleep(device->mIface);
#else
        if (atca_iface_is_kit(&device->mIface) || (ATCA_SWI_IFACE == device->mIface.mIfaceCFG->iface_type))
        {
            status = atsleep(&device->mIface);
        }
        else
        {
            uint8_t command = 0x01;
            status = atsend(&device->mIface, atcab_get_device_address(device), &command, 1);
        }
#endif
        return status;
    }

/** \brief common cleanup code which idles the device after any operation
 *  \param[in] device     Device context pointer
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
    ATCA_STATUS _calib_exit(ATCADevice device)
    {
        return calib_idle(device);
    }


/** \brief Compute the address given the zone, slot, block, and offset
 *  \param[in] zone   Zone to get address from. Config(0), OTP(1), or
 *                    Data(2) which requires a slot.
 *  \param[in] slot   Slot Id number for data zone and zero for other zones.
 *  \param[in] block  Block number within the data or configuration or OTP zone .
 *  \param[in] offset Offset Number within the block of data or configuration or OTP zone.
 *  \param[out] addr  Pointer to the address of data or configuration or OTP zone.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
    ATCA_STATUS calib_get_addr(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint16_t* addr)
    {
        ATCA_STATUS status = ATCA_SUCCESS;
        uint8_t mem_zone = zone & 0x03;

        if (addr == NULL)
        {
            return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
        }
        if ((mem_zone != ATCA_ZONE_CONFIG) && (mem_zone != ATCA_ZONE_DATA) && (mem_zone != ATCA_ZONE_OTP))
        {
            return ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received");
        }
        do
        {
            // Initialize the addr to 00
            *addr = 0;
            // Mask the offset
            offset = offset & (uint8_t)0x07;
            if ((mem_zone == ATCA_ZONE_CONFIG) || (mem_zone == ATCA_ZONE_OTP))
            {
                *addr = block << 3;
                *addr |= offset;
            }
            else // ATCA_ZONE_DATA
            {
                *addr = slot << 3;
                *addr  |= offset;
                *addr |= block << 8;
            }
        }
        while (0);

        return status;
    }

/** \brief Compute the address given the zone, slot, block, and offset for ECC204 device
 *  \param[in] zone   Zone to get address from. Config(1) or
 *                    Data(0) which requires a slot.
 *  \param[in] slot   Slot Id number for data zone and zero for other zones.
 *  \param[in] block  Block number within the data zone .
 *  \param[in] offset Aalways zero.
 *  \param[out] addr  Pointer to the address of data or configuration zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
    ATCA_STATUS calib_ecc204_get_addr(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint16_t* addr)
    {
        ATCA_STATUS status = ATCA_SUCCESS;

        (void)offset;

        if (addr == NULL)
        {
            return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
        }

        // Initialize the addr to 00
        *addr = 0;
        *addr = slot << 3;
        *addr |= block << 8;

        return status;
    }

/** \brief Gets the size of the specified zone in bytes.
 *
 * \param[in]  device  Device context pointer
 * \param[in]  zone    Zone to get size information from. Config(0), OTP(1), or
 *                     Data(2) which requires a slot.
 * \param[in]  slot    If zone is Data(2), the slot to query for size.
 * \param[out] size    Zone size is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
    ATCA_STATUS calib_get_zone_size(ATCADevice device, uint8_t zone, uint16_t slot, size_t* size)
    {
        ATCA_STATUS status = ATCA_SUCCESS;

        if ((device == NULL) || (size == NULL))
        {
            return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
        }

        if (device->mIface.mIfaceCFG->devtype == ATSHA204A)
        {
            switch (zone)
            {
            case ATCA_ZONE_CONFIG: *size = 88; break;
            case ATCA_ZONE_OTP:    *size = 64; break;
            case ATCA_ZONE_DATA:   *size = 32; break;
            default: status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received"); break;
            }
        }
        else if (device->mIface.mIfaceCFG->devtype == ATSHA206A)
        {
            switch (zone)
            {
            case ATCA_ZONE_CONFIG: *size = 88; break;
            case ATCA_ZONE_OTP:    *size = 0; break;
            case ATCA_ZONE_DATA:   *size = 32; break;
            default: status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received"); break;
            }
        }
#ifdef ATCA_ECC204_SUPPORT
        else if (ECC204 == device->mIface.mIfaceCFG->devtype)
        {
            switch (zone)
            {
            case ATCA_ECC204_ZONE_CONFIG: *size = 16; break;
            case ATCA_ECC204_ZONE_DATA:
                if ((0 == slot) || (3 == slot))
                {
                    *size = 32;
                }
                else if (1 == slot)
                {
                    *size = 320;
                }
                else if (2 == slot)
                {
                    *size = 64;
                }
                else
                {
                    status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid slot received");
                }
                break;
            default: status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received"); break;
            }
        }
#endif
        else
        {
            switch (zone)
            {
            case ATCA_ZONE_CONFIG: *size = 128; break;
            case ATCA_ZONE_OTP:    *size = 64; break;
            case ATCA_ZONE_DATA:
                if (slot < 8)
                {
                    *size = 36;
                }
                else if (slot == 8)
                {
                    *size = 416;
                }
                else if (slot < 16)
                {
                    *size = 72;
                }
                else
                {
                    status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid slot received");
                }
                break;
            default: status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received"); break;
            }
        }

        return status;
    }
