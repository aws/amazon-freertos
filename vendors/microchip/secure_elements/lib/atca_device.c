/**
 * \file
 * \brief  Microchip CryptoAuth device object
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

#include <cryptoauthlib.h>

/** \defgroup device ATCADevice (atca_)
 * \brief ATCADevice object - composite of command and interface objects
   @{ */


#ifndef ATCA_NO_HEAP
/** \brief constructor for a Microchip CryptoAuth device
 * \param[in] cfg  Interface configuration object
 * \return Reference to a new ATCADevice on success. NULL on failure.
 */
ATCADevice newATCADevice(ATCAIfaceCfg *cfg)
{
    ATCADevice ca_dev = NULL;
    ATCA_STATUS status;

    if (cfg == NULL)
    {
        return NULL;
    }

    ca_dev = (ATCADevice)malloc(sizeof(*ca_dev));
    if (ca_dev == NULL)
    {
        return NULL;
    }

    memset(ca_dev, 0, sizeof(struct atca_device));

    status = initATCADevice(cfg, ca_dev);
    if (status != ATCA_SUCCESS)
    {
        free(ca_dev);
        ca_dev = NULL;
        return NULL;
    }

    return ca_dev;
}

/** \brief destructor for a device NULLs reference after object is freed
 * \param[in] ca_dev  pointer to a reference to a device
 */
void deleteATCADevice(ATCADevice *ca_dev)
{
    if (ca_dev == NULL)
    {
        return;
    }

    releaseATCADevice(*ca_dev);

    free(*ca_dev);
    *ca_dev = NULL;
}
#endif

/** \brief Initializer for an Microchip CryptoAuth device
 * \param[in]    cfg     pointer to an interface configuration object
 * \param[in,out] ca_dev  As input, pre-allocated structure to be initialized.
 *                       mCommands and mIface members should point to existing
 *                       structures to be initialized.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS initATCADevice(ATCAIfaceCfg *cfg, ATCADevice ca_dev)
{
    ATCA_STATUS status;

    if (cfg == NULL || ca_dev == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    status = initATCAIface(cfg, &ca_dev->mIface);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}

/** \brief returns a reference to the ATCAIface interface object for the device
 * \param[in] dev  reference to a device
 * \return reference to the ATCAIface object for the device
 */
ATCAIface atGetIFace(ATCADevice dev)
{
    return &dev->mIface;
}

/** \brief Release any resources associated with the device.
 *  \param[in] ca_dev  Device to release
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS releaseATCADevice(ATCADevice ca_dev)
{
    if (ca_dev == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    return releaseATCAIface(&ca_dev->mIface);
}

/** @} */
