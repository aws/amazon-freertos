/**
 * \file
 * \brief ATCA Hardware abstraction layer for SAMD21 I2C over ASF drivers.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the ASF I2C primitives to set up the interface.
 *
 * Prerequisite: add SERCOM I2C Master Polled support to application in Atmel Studio
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

#include <asf.h>
#include <string.h>
#include <stdio.h>

#include "hal_sam0_i2c_asf.h"
#include "cryptoauthlib.h"

#ifndef ATCA_HAL_LEGACY_API
#error "The use of this hal requires the ATCA_HAL_LEGACY_API option to be enabled.
#endif

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using I2C driver of ASF.
 *
   @{ */

/** \brief logical to physical bus mapping structure */

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in]  bus_num  logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg     pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] found   number of devices found on this bus
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief
    - this HAL implementation assumes you've included the ASF SERCOM I2C libraries in your project, otherwise,
    the HAL layer will not compile because the ASF I2C drivers are a dependency *
 */


/** \brief initialize an I2C interface using given config
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */


ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C post init
 * \param[in] iface  instance
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C send over ASF
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    struct i2c_master_module * i2c_master_instance;
    i2c_sam0_instance_t * plib;

    struct i2c_master_packet packet = {
        .address            = cfg->atcai2c.slave_address >> 1,
        .data_length        = txlength,
        .data               = txdata,
        .ten_bit_address    = false,
        .high_speed         = false,
        .hs_master_code     = 0x0,
    };

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_sam0_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (struct i2c_master_module *)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }


    // for this implementation of I2C with CryptoAuth chips, txdata is assumed to have ATCAPacket format
    // other device types that don't require i/o tokens on the front end of a command need a different hal_i2c_send and wire it up instead of this one
    // this covers devices such as ATSHA204A and ATECCx08A that require a word address value pre-pended to the packet
    // txdata[0] is using _reserved byte of the ATCAPacket
    if (0xFF != word_address)
    {
        txdata[0] = word_address;   // insert the Word Address Value, Command token
        txlength++;                 // account for word address value byte.
    }

    packet.data_length = txlength;

    if (i2c_master_write_packet_wait(i2c_master_instance, &packet) != STATUS_OK)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C receive function for ASF I2C
 * \param[in]    iface     Device to interact with.
 * \param[out]   rxdata    Data received will be returned here.
 * \param[in,out] rxlength  As input, the size of the rxdata buffer.
 *                         As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int retries = cfg->rx_retries;
    ATCA_STATUS status = !ATCA_SUCCESS;
    uint16_t read_length = 2;
    uint8_t min_resp_size = 4;
    uint16_t rxdata_max_size = *rxlength;
    struct i2c_master_module * i2c_master_instance;
    i2c_sam0_instance_t * plib;

    struct i2c_master_packet packet = {
        .address            = cfg->atcai2c.slave_address >> 1,
        .data_length        = 1,
        .data               = rxdata,
        .ten_bit_address    = false,
        .high_speed         = false,
        .hs_master_code     = 0x0,
    };

    if ((NULL == cfg) || (NULL == rxlength) || (NULL == rxdata))
    {
        return ATCA_TRACE(ATCA_INVALID_POINTER, "NULL pointer encountered");
    }

    plib = (i2c_sam0_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (struct i2c_master_module *)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }

    *rxlength = 0;
    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }
    do
    {
        /*Send Word address to device...*/
        retries = cfg->rx_retries;
        while (retries-- > 0 && status != ATCA_SUCCESS)
        {
            status = hal_i2c_send(iface, word_address, &word_address, 0);
        }
        if (ATCA_SUCCESS != status)
        {
            ATCA_TRACE(status, "hal_i2c_send - failed");
            break;
        }

#if ATCA_TA_SUPPORT
        /*Set read length.. Check for register reads or 1 byte reads*/
        if ((word_address == ATCA_MAIN_PROCESSOR_RD_CSR) || (word_address == ATCA_FAST_CRYPTO_RD_FSR)
            || (rxdata_max_size == 1))
        {
            read_length = 1;
        }
#endif

        packet.data_length = read_length;

        /* Read length bytes to know number of bytes to read */
        if (i2c_master_read_packet_wait(i2c_master_instance, &packet) != STATUS_OK)
        {
            status = ATCA_TRACE(ATCA_RX_FAIL, "hal_i2c_receive - failed");
            break;
        }

        if (1 == read_length)
        {
            ATCA_TRACE(status, "1 byte read completed");
            break;
        }

        /*Calculate bytes to read based on device response*/
        if (cfg->devtype == TA100)
        {
            read_length = ((uint16_t)rxdata[0] * 256) + rxdata[1];
            min_resp_size += 1;
        }
        else
        {
            read_length =  rxdata[0];
        }

        if (read_length > rxdata_max_size)
        {
            status = ATCA_TRACE(ATCA_SMALL_BUFFER, "rxdata is small buffer");
            break;
        }

        if (read_length < min_resp_size)
        {
            status = ATCA_TRACE(ATCA_RX_FAIL, "packet size is invalid");
            break;
        }

        /*Update receive length with first byte received and set to read rest of the data */
        packet.data_length = read_length - 2;
        packet.data = &rxdata[2];

        /* Read given length bytes from device */
        if (i2c_master_read_packet_wait(i2c_master_instance, &packet) != STATUS_OK)
        {
            status = ATCA_TRACE(ATCA_RX_FAIL, "hal_i2c_receive - failed");
            break;
        }
    }
    while (0);

    *rxlength = read_length;

    return status;
}


/** \brief wake up CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to wakeup
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int retries;
    uint32_t bdrt;
    int status = !STATUS_OK;
    uint8_t data[4];
    struct i2c_master_module * i2c_master_instance;
    i2c_sam0_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    retries = cfg->rx_retries;
    bdrt = cfg->atcai2c.baud;
    plib = (i2c_sam0_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (struct i2c_master_module *)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }


    if (bdrt != 100000)    // if not already at 100KHz, change it
    {
        plib->change_baudrate(iface, 100000);
    }

    // Send the wake by writing to an address of 0x00
    struct i2c_master_packet packet = {
        .address            = 0x00,
        .data_length        = 0,
        .data               = &data[0],
        .ten_bit_address    = false,
        .high_speed         = false,
        .hs_master_code     = 0x0,
    };

    // Send the 00 address as the wake pulse
    // part will NACK, so don't check for status
    i2c_master_write_packet_wait(i2c_master_instance, &packet);

    // wait tWHI + tWLO which is configured based on device type and configuration structure
    atca_delay_us(cfg->wake_delay);

    packet.address = cfg->atcai2c.slave_address >> 1;
    packet.data_length = 4;
    packet.data = data;

    // if necessary, revert baud rate to what came in.
    if (bdrt != 100000)
    {
        plib->change_baudrate(iface, bdrt);
    }

    while (retries-- > 0 && status != STATUS_OK)
    {
        status = i2c_master_read_packet_wait(i2c_master_instance, &packet);
    }

    if (status != STATUS_OK)
    {
        if (retries <= 0)
        {
            return ATCA_TOO_MANY_COMM_RETRIES;
        }

        return ATCA_COMM_FAIL;
    }

    return hal_check_wake(data, 4);
}

/** \brief idle CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to idle
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data = 0x02;  // idle word address value
    struct i2c_master_module * i2c_master_instance;
    i2c_sam0_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_sam0_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (struct i2c_master_module *)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }

    struct i2c_master_packet packet = {
        .address            = cfg->atcai2c.slave_address >> 1,
        .data_length        = 1,
        .data               = &data,
        .ten_bit_address    = false,
        .high_speed         = false,
        .hs_master_code     = 0x0,
    };

    if (i2c_master_write_packet_wait(i2c_master_instance, &packet) != STATUS_OK)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;

}

/** \brief sleep CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to sleep
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data = 0x01;  // sleep word address value
    struct i2c_master_module * i2c_master_instance;
    i2c_sam0_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_sam0_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (struct i2c_master_module *)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }
    struct i2c_master_packet packet = {
        .address            = cfg->atcai2c.slave_address >> 1,
        .data_length        = 1,
        .data               = &data,
        .ten_bit_address    = false,
        .high_speed         = false,
        .hs_master_code     = 0x0,
    };

    if (i2c_master_write_packet_wait(i2c_master_instance, &packet) != STATUS_OK)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{

    return ATCA_SUCCESS;
}

/** @} */
