/**
 * \file
 *
 * \brief  Microchip CryptoAuthLib hardware interface object
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

/** \defgroup interface ATCAIface (atca_)
 *  \brief Abstract interface to all CryptoAuth device types.  This interface
 *  connects to the HAL implementation and abstracts the physical details of the
 *  device communication from all the upper layers of CryptoAuthLib
   @{ */


/** \brief Initializer for ATCAIface objects
 * \param[in] cfg       Logical configuration for the interface
 * \param[in] ca_iface  Interface structure to initialize.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS initATCAIface(ATCAIfaceCfg *cfg, ATCAIface ca_iface)
{
    ATCA_STATUS status;

    if (cfg == NULL || ca_iface == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    ca_iface->mIfaceCFG = cfg;

    status = ATCA_TRACE(atinit(ca_iface), "atinit");
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}

#ifndef ATCA_NO_HEAP
/** \brief Constructor for ATCAIface objects
 * \param[in] cfg  Logical configuration for the interface
 * \return New interface instance on success. NULL on failure.
 */
ATCAIface newATCAIface(ATCAIfaceCfg *cfg)
{
    ATCAIface ca_iface;
    ATCA_STATUS status;

    ca_iface = (ATCAIface)malloc(sizeof(struct atca_iface));
    status = initATCAIface(cfg, ca_iface);
    if (status != ATCA_SUCCESS)
    {
        free(ca_iface);
        ca_iface = NULL;
        return NULL;
    }

    return ca_iface;
}
#endif

/** \brief Performs the HAL initialization by calling intermediate HAL wrapper
 *         function. If using the basic API, the atcab_init() function should
 *         be called instead.
 * \param[in] ca_iface  Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atinit(ATCAIface ca_iface)
{
    ATCA_STATUS status = ATCA_COMM_FAIL;

    if (ca_iface)
    {
        status = ATCA_TRACE(hal_iface_init(ca_iface->mIfaceCFG, &ca_iface->hal, &ca_iface->phy), "Failed to configure HAL");

        /* Initialize the physical interface if one is required for the hal */
        if (ATCA_SUCCESS == status && ca_iface->phy)
        {
            if (ca_iface->phy->halinit && ca_iface->phy->halpostinit)
            {
                if (ATCA_SUCCESS == (status = ATCA_TRACE(ca_iface->phy->halinit(ca_iface, ca_iface->mIfaceCFG), "phyinit")))
                {
                    status = ATCA_TRACE(ca_iface->phy->halpostinit(ca_iface), "phypostinit");
                }
            }
            else
            {
                status = ATCA_TRACE(ATCA_ASSERT_FAILURE, "phy is invalid");
            }
        }

        /* Initialize the hal itself */
        if (ATCA_SUCCESS == status)
        {
            if (ca_iface->hal->halinit && ca_iface->hal->halpostinit)
            {
                if (ATCA_SUCCESS == (status = ATCA_TRACE(ca_iface->hal->halinit(ca_iface, ca_iface->mIfaceCFG), "halinit")))
                {
                    status = ATCA_TRACE(ca_iface->hal->halpostinit(ca_iface), "halpostinit");
                }
            }
            else
            {
                status = ATCA_TRACE(ATCA_ASSERT_FAILURE, "hal is invalid");
            }
        }
    }

    return status;
}

/** \brief Sends the data to the device by calling intermediate HAL wrapper
 *         function.
 * \param[in] ca_iface       Device to interact with.
 * \param[in] word_address   device transaction type
 * \param[in] txdata         Data to be transmitted to the device.
 * \param[in] txlength       Number of bytes to be transmitted to the device.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atsend(ATCAIface ca_iface, uint8_t address, uint8_t *txdata, int txlength)
{
    if (!ca_iface)
    {
        return ATCA_BAD_PARAM;
    }

    if (ca_iface->hal && ca_iface->hal->halsend)
    {
#ifdef ATCA_HAL_I2C
        if (ATCA_I2C_IFACE == ca_iface->mIfaceCFG->iface_type && 0xFF == address)
        {
            address = ca_iface->mIfaceCFG->atcai2c.address;
        }
#endif

        return ca_iface->hal->halsend(ca_iface, address, txdata, txlength);
    }
    else
    {
        return ATCA_NOT_INITIALIZED;
    }
}

/** \brief Receives data from the device by calling intermediate HAL wrapper
 *        function.
 * \param[in]     ca_iface       Device to interact with.
 * \param[in]     word_address   device transaction type
 * \param[out]    rxdata         Data received will be returned here.
 * \param[in,out] rxlength       As input, the size of the rxdata buffer.
 *                               As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atreceive(ATCAIface ca_iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength)
{
    if (!ca_iface)
    {
        return ATCA_BAD_PARAM;
    }

    if (ca_iface->hal && ca_iface->hal->halreceive)
    {
        return ca_iface->hal->halreceive(ca_iface, word_address, rxdata, rxlength);
    }
    else
    {
        return ATCA_NOT_INITIALIZED;
    }
}


/** \brief Perform control operations with the underlying hal driver
 * \param[in]     ca_iface       Device to interact with.
 * \param[in]     option         Control parameter identifier
 * \param[in]     param          Optional pointer to parameter value
 * \param[in]     paramlen       Length of the parameter
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcontrol(ATCAIface ca_iface, uint8_t option, void* param, size_t paramlen)
{
    if (!ca_iface)
    {
        return ATCA_BAD_PARAM;
    }


    if (ca_iface->hal && ca_iface->hal->halcontrol)
    {
        return ca_iface->hal->halcontrol(ca_iface, option, param, paramlen);
    }
    else
    {
        return ATCA_NOT_INITIALIZED;
    }
}

/** \brief Wakes up the device by calling intermediate HAL wrapper function. The
 * atcab_wakeup() function should be used instead.
 * \deprecated This function does not have defined behavior when ATCA_HAL_LEGACY_API
 * is undefined.
 *
 * \param[in] ca_iface  Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atwake(ATCAIface ca_iface)
{
    if (!ca_iface)
    {
        return ATCA_BAD_PARAM;
    }


    if (ca_iface->hal && ca_iface->hal->halcontrol)
    {
        ATCA_STATUS status = ca_iface->hal->halcontrol(ca_iface, ATCA_HAL_CONTROL_WAKE, NULL, 0);

        if (ATCA_WAKE_FAILED == status)
        {
            // The device might be performing a POST. Wait for it to complete
            // and try again.
            atca_delay_ms(ATCA_POST_DELAY_MSEC);

            status = ca_iface->hal->halcontrol(ca_iface, ATCA_HAL_CONTROL_WAKE, NULL, 0);
        }
        return status;
    }
    else
    {
        return ATCA_NOT_INITIALIZED;
    }
}


/** \brief Puts the device into idle state by calling intermediate HAL wrapper
 *         function. The atcab_idle() function should be used instead.
 * \deprecated This function does not have defined behavior when ATCA_HAL_LEGACY_API
 * is undefined.
 * \param[in] ca_iface  Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atidle(ATCAIface ca_iface)
{
    if (!ca_iface)
    {
        return ATCA_BAD_PARAM;
    }

    if (ca_iface->hal && ca_iface->hal->halcontrol)
    {
        ATCA_STATUS status = ca_iface->hal->halcontrol(ca_iface, ATCA_HAL_CONTROL_IDLE, NULL, 0);
        atca_delay_ms(1);
        return status;
    }
    else
    {
        return ATCA_NOT_INITIALIZED;
    }
}

/** \brief Puts the device into sleep state by calling intermediate HAL wrapper
 *         function. The atcab_sleep() function should be used instead.
 * \deprecated This function does not have defined behavior when ATCA_HAL_LEGACY_API
 * is undefined.
 * \param[in] ca_iface  Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atsleep(ATCAIface ca_iface)
{
    if (!ca_iface)
    {
        return ATCA_BAD_PARAM;
    }

    if (ca_iface->hal && ca_iface->hal->halcontrol)
    {
        ATCA_STATUS status = ca_iface->hal->halcontrol(ca_iface, ATCA_HAL_CONTROL_SLEEP, NULL, 0);
        atca_delay_ms(1);
        return status;
    }
    else
    {
        return ATCA_NOT_INITIALIZED;
    }
}


/** \brief Returns the logical interface configuration for the device.
 * \param[in] ca_iface  Device interface.
 * \return Logical interface configuration.
 */
ATCAIfaceCfg * atgetifacecfg(ATCAIface ca_iface)
{
    return ca_iface ? ca_iface->mIfaceCFG : NULL;
}


/** \brief Returns the HAL data pointer for the device.
 * \param[in] ca_iface  Device interface.
 * \return HAL data pointer.
 */
void* atgetifacehaldat(ATCAIface ca_iface)
{
    return ca_iface ? ca_iface->hal_data : NULL;
}

/** \brief Check if the given interface is configured as a "kit protocol" one where
 * transactions are atomic
 * \return true if the interface is considered a kit
 */
bool atca_iface_is_kit(ATCAIface ca_iface)
{
    bool ret = false;

    if (ca_iface && ca_iface->mIfaceCFG)
    {
        if (ATCA_HID_IFACE == ca_iface->mIfaceCFG->iface_type || ATCA_KIT_IFACE == ca_iface->mIfaceCFG->iface_type)
        {
            ret = true;
        }
    }
    return ret;
}

/** \brief Retrive the number of retries for a configured interface */
int atca_iface_get_retries(ATCAIface ca_iface)
{
    if (ca_iface && ca_iface->mIfaceCFG)
    {
        return ca_iface->mIfaceCFG->rx_retries;
    }
    else
    {
        return 0;
    }
}

/** \brief Retrive the wake/retry delay for a configured interface/device */
uint16_t atca_iface_get_wake_delay(ATCAIface ca_iface)
{
    if (ca_iface && ca_iface->mIfaceCFG)
    {
        return ca_iface->mIfaceCFG->wake_delay;
    }
    else
    {
        return 1500;
    }
}


/** \brief Instruct the HAL driver to release any resources associated with
 *         this interface.
 *  \param[in] ca_iface  Device interface.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS releaseATCAIface(ATCAIface ca_iface)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ca_iface && ca_iface->mIfaceCFG)
    {
        if (ATCA_SUCCESS == (status = hal_iface_release(ca_iface->mIfaceCFG->iface_type, ca_iface->hal_data)))
        {
            ca_iface->hal_data = NULL;
        }
        if (ATCA_CUSTOM_IFACE == ca_iface->mIfaceCFG->iface_type)
        {
#ifndef ATCA_NO_HEAP
            hal_free(ca_iface->hal);
#endif
            ca_iface->hal = NULL;
        }
    }
    return status;
}

#ifndef ATCA_NO_HEAP
/** \brief Instruct the HAL driver to release any resources associated with
 *         this interface, then delete the object.
 * \param[in] ca_iface  Device interface.
 */
void deleteATCAIface(ATCAIface *ca_iface)
{
    if (ca_iface)
    {
        releaseATCAIface(*ca_iface);
        free(*ca_iface);
        *ca_iface = NULL;
    }
}
#endif

/** @} */
