/**
 * \file
 * \brief Kit Bridging HAL for cryptoauthlib. This is not intended to be a zero
 * copy driver. It should work with any interface that confirms to a few basic
 * requirements: a) will accept an arbitrary number of bytes and packetize it if
 * necessary for transmission, b) will block for the duration of the transmit.
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
#include "atca_hal.h"
#include "hal_kit_bridge.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

/** \brief Helper function that connects a physical layer context structure that will be used by the kit protocol bridge
 * \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_attach_phy(
    ATCAIfaceCfg*       cfg,    /**< [IN] Interface configuration structure */
    atca_hal_kit_phy_t* phy     /**< [IN] Structure with physical layer interface functions and context */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (cfg && phy)
    {
        cfg->cfg_data = phy;
        status = ATCA_SUCCESS;
    }
    return status;
}

#ifdef ATCA_HAL_KIT_DONT_CHECK_PHY
#define hal_kit_phy_send(phy, txdata, txlen)        phy->send(phy->hal_data, txdata, txlen)
#define hal_kit_phy_recv(phy, rxdata, rxlen)        phy->recv(phy->hal_data, rxdata, rxlen)
#define hal_kit_phy_packet_alloc(phy, length)       phy->packet_alloc(length)
#define hal_kit_phy_packet_free(phy, packet)        phy->packet_free(packet)
#else
/** \brief Helper function that sends data using the atca_hal_kit_phy_t structure
 */
static ATCA_STATUS hal_kit_phy_send(atca_hal_kit_phy_t* phy, uint8_t * txdata, uint16_t txlen)
{
    return (phy && phy->send) ? phy->send(phy->hal_data, txdata, txlen) : ATCA_BAD_PARAM;
}


/** \brief Helper function that receives data using the atca_hal_kit_phy_t structure
 */
static ATCA_STATUS hal_kit_phy_recv(atca_hal_kit_phy_t* phy, uint8_t* rxdata, uint16_t* rxlen)
{
    return (phy && phy->recv) ? phy->recv(phy->hal_data, rxdata, rxlen) : ATCA_BAD_PARAM;
}


/** \brief Helper function that allocates a packet using the atca_hal_kit_phy_t structure
 */
static uint8_t* hal_kit_phy_packet_alloc(atca_hal_kit_phy_t* phy, uint16_t length)
{
    return (phy && phy->packet_alloc) ? phy->packet_alloc(length) : NULL;
}


/** \brief Helper function that releases a packet using the atca_hal_kit_phy_t structure
 */
static void hal_kit_phy_packet_free(atca_hal_kit_phy_t* phy, uint8_t* packet)
{
    if (phy && phy->packet_free)
    {
        phy->packet_free(packet);
    }
}
#endif

/** \brief Request a list of busses from the kit host
 */
ATCA_STATUS hal_kit_discover_buses(int busses[], int max_buses)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 */
ATCA_STATUS hal_kit_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int* found)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief HAL implementation of Kit USB HID init
 *  \param[in] hal pointer to HAL specific data that is maintained by this HAL
 *  \param[in] cfg pointer to HAL specific configuration data that is used to initialize this HAL
 * \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_init(void* hal, ATCAIfaceCfg* cfg)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    /* Perform rationality checks on the configuration structure */
    if (cfg && cfg->cfg_data)
    {
        atca_hal_kit_phy_t* phy = cfg->cfg_data;
        /* Make sure all of the correct functions are attached */
        if (phy && phy->send && phy->recv && phy->packet_alloc && phy->packet_free)
        {
            status = ATCA_SUCCESS;
        }
    }

    return status;
}

/** \brief HAL implementation of Kit HID post init
 *  \param[in] iface  instance
 *  \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of kit protocol send over USB HID
 *  \param[in] iface          instance
 *  \param[in] word_address   determine device transaction type
 *  \param[in] txdata         pointer to bytes to send
 *  \param[in] txlength       number of bytes to send
 *  \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_send(ATCAIface iface, uint8_t word_address, uint8_t* txdata, int txlength)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (iface && iface->mIfaceCFG && txdata && txlength)
    {
        atca_hal_kit_phy_t* phy = iface->mIfaceCFG->cfg_data;

        uint16_t packet_size = txlength + HAL_KIT_HEADER_LEN + 1;
        uint8_t* packet = hal_kit_phy_packet_alloc(phy, packet_size);


        if (packet)
        {
            packet[0] = 1;
            packet[1] = HAL_KIT_COMMAND_SEND | (iface->mIfaceCFG->atcakit.dev_interface << 4);
            packet[2] = iface->mIfaceCFG->atcakit.dev_identity;
            packet[3] = word_address;
            memcpy(&packet[4], &txdata[1], txlength);

            status = hal_kit_phy_send(phy, packet, txlength + HAL_KIT_HEADER_LEN + 1);

            if (ATCA_SUCCESS == status)
            {
                status = hal_kit_phy_recv(phy, packet, &packet_size);
            }

            if (ATCA_SUCCESS == status)
            {
                status = packet[3];
            }

            hal_kit_phy_packet_free(phy, packet);
        }

    }
    return status;
}

/** \brief HAL implementation of send over USB HID
 * \param[in]    iface          instance
 * \param[in]    word_address   determine device transaction type
 * \param[in]    rxdata         pointer to space to receive the data
 * \param[in,out] rxsize        ptr to expected number of receive bytes to request
 * \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_receive(ATCAIface iface, uint8_t word_address, uint8_t* rxdata, uint16_t* rxsize)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (iface && iface->mIfaceCFG && rxdata && rxsize && *rxsize)
    {
        atca_hal_kit_phy_t* phy = iface->mIfaceCFG->cfg_data;
        uint8_t* packet = hal_kit_phy_packet_alloc(phy, 1200);
        uint16_t packet_size = 1200;

        if (packet)
        {
            packet[0] = 1;
            packet[1] = HAL_KIT_COMMAND_RECV | (iface->mIfaceCFG->atcakit.dev_interface << 4);
            packet[2] = iface->mIfaceCFG->atcakit.dev_identity;
            packet[3] = word_address;
            packet[4] = *rxsize & 0xFF;
            packet[5] = (*rxsize >> 8) & 0xFF;

            status = hal_kit_phy_send(phy, packet, HAL_KIT_HEADER_LEN + 3);

            if (ATCA_SUCCESS == status)
            {
                status = hal_kit_phy_recv(phy, packet, &packet_size);
            }

            if (ATCA_SUCCESS == status)
            {
                if (*rxsize < packet_size - HAL_KIT_HEADER_LEN - 1)
                {
                    status = ATCA_SMALL_BUFFER;
                }
                else
                {
                    *rxsize = packet_size - HAL_KIT_HEADER_LEN - 1;
                }

                memcpy(rxdata, &packet[4], *rxsize);
            }

            hal_kit_phy_packet_free(phy, packet);
        }
    }
    return status;
}

/** \brief Call the wake for kit protocol
 * \param[in] iface  ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_STATUS
 */
static ATCA_STATUS hal_kit_wake(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (iface && iface->mIfaceCFG)
    {
        atca_hal_kit_phy_t* phy = iface->mIfaceCFG->cfg_data;
        uint8_t* packet = hal_kit_phy_packet_alloc(phy, 10);
        uint16_t packet_size = HAL_KIT_HEADER_LEN;

        if (packet)
        {
            packet[0] = 1;
            packet[1] = HAL_KIT_COMMAND_WAKE | (iface->mIfaceCFG->atcakit.dev_interface << 4);
            packet[2] = iface->mIfaceCFG->atcakit.dev_identity;

            status = hal_kit_phy_send(phy, packet, packet_size);

            if (ATCA_SUCCESS == status)
            {
                packet_size = HAL_KIT_HEADER_LEN + 1;
                status = hal_kit_phy_recv(phy, packet, &packet_size);
            }

            if (ATCA_SUCCESS == status)
            {
                status = packet[3];
            }

            hal_kit_phy_packet_free(phy, packet);
        }
    }

    return status;
}

/** \brief Call the idle for kit protocol
 * \param[in] iface  ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_STATUS
 */
static ATCA_STATUS hal_kit_idle(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (iface && iface->mIfaceCFG)
    {
        atca_hal_kit_phy_t* phy = iface->mIfaceCFG->cfg_data;
        uint8_t* packet = hal_kit_phy_packet_alloc(phy, 10);
        uint16_t packet_size = HAL_KIT_HEADER_LEN;

        if (packet)
        {
            packet[0] = 1;
            packet[1] = HAL_KIT_COMMAND_IDLE | (iface->mIfaceCFG->atcakit.dev_interface << 4);
            packet[2] = iface->mIfaceCFG->atcakit.dev_identity;

            status = hal_kit_phy_send(phy, packet, packet_size);

            if (ATCA_SUCCESS == status)
            {
                packet_size = HAL_KIT_HEADER_LEN + 1;
                status = hal_kit_phy_recv(phy, packet, &packet_size);
            }

            if (ATCA_SUCCESS == status)
            {
                status = packet[3];
            }

            hal_kit_phy_packet_free(phy, packet);
        }
    }

    return status;
}

/** \brief Call the sleep for kit protocol
 * \param[in] iface  ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_STATUS
 */
static ATCA_STATUS hal_kit_sleep(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (iface && iface->mIfaceCFG)
    {
        atca_hal_kit_phy_t* phy = iface->mIfaceCFG->cfg_data;
        uint8_t* packet = hal_kit_phy_packet_alloc(phy, 10);
        uint16_t packet_size = HAL_KIT_HEADER_LEN;

        if (packet)
        {
            packet[0] = 1;
            packet[1] = HAL_KIT_COMMAND_SLEEP | (iface->mIfaceCFG->atcakit.dev_interface << 4);
            packet[2] = iface->mIfaceCFG->atcakit.dev_identity;

            status = hal_kit_phy_send(phy, packet, packet_size);

            if (ATCA_SUCCESS == status)
            {
                packet_size = HAL_KIT_HEADER_LEN + 1;
                status = hal_kit_phy_recv(phy, packet, &packet_size);
            }

            if (ATCA_SUCCESS == status)
            {
                status = packet[3];
            }

            hal_kit_phy_packet_free(phy, packet);
        }
    }

    return status;
}

/** \brief Kit Protocol Control
 * \param[in] iface  ATCAIface instance that is the interface object to send the bytes over
 * \param[in] option  Control option to use
 * \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_control(ATCAIface iface, uint8_t option)
{
    switch (option)
    {
    case ATCA_HAL_CONTROL_WAKE:
        return hal_kit_wake(iface);
    case ATCA_HAL_CONTROL_IDLE:
        return hal_kit_idle(iface);
    case ATCA_HAL_CONTROL_SLEEP:
        return hal_kit_sleep(iface);
    default:
        return ATCA_BAD_PARAM;
    }
}


/** \brief Close the physical port for HID
 * \param[in] hal_data  The hardware abstraction data specific to this HAL
 * \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_release(void* hal_data)
{
    return ATCA_SUCCESS;
}

/** @} */
