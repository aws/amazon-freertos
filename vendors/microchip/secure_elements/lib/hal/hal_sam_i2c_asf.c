/**
 * \file
 * \brief ATCA Hardware abstraction layer for SAM flexcom & twi I2C over ASF drivers.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the ASF I2C primitives to set up the interface.
 *
 * Prerequisite: add "TWI - Two-Wire Interface (Common API) (service)" module to application in Atmel Studio
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
#include "cryptoauthlib.h"
#include "hal_sam_i2c_asf.h"

#ifndef ATCA_HAL_LEGACY_API
#error "The use of this hal requires the ATCA_HAL_LEGACY_API option to be enabled.
#endif

//!< Uncomment when debugging
/*#define DEBUG_HAL*/

/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using I2C driver of ASF.
 *
   @{ */

/**
 * \brief
 * This HAL implementation assumes you've included the ASF TWI libraries in your project, otherwise,
 * the HAL layer will not compile because the ASF TWI drivers are a dependency
 */

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{

    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/**
 * \brief
 * hal_i2c_init manages requests to initialize a physical interface. It manages use counts so when an interface
 * has released the physical layer, it will disable the interface for some other use.
 * You can have multiple ATCAIFace instances using the same bus, and you can have multiple ATCAIFace instances on
 * multiple i2c buses, so hal_i2c_init manages these things and ATCAIFace is abstracted from the physical details.
 */

/**
 * \brief initialize an I2C interface using given config
 *
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of I2C post init
 *
 * \param[in] iface  instance
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of I2C send over ASF
 *
 * \param[in] iface         instance
 * \param[in] word_address  device word address
 * \param[in] txdata        pointer to space to bytes to send
 * \param[in] txlength      number of bytes to send
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    Twi * i2c_master_instance;
    i2c_sam_instance_t * plib;

    twi_packet_t packet = {
        .chip        = cfg->atcai2c.slave_address >> 1,
        .addr        = { 0 },
        .addr_length = 0,
        .buffer      = txdata,
        .length      = (uint32_t)txlength //(uint32_t)txdata[1]
    };

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_sam_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (Twi*)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }


    if (0xFF != word_address)
    {
        txdata[0] = word_address;   // insert the Word Address Value, Command token
        txlength++;                 // account for word address value byte.
    }

    packet.length = txlength;

    // for this implementation of I2C with CryptoAuth chips, txdata is assumed to have ATCAPacket format

    // other device types that don't require i/o tokens on the front end of a command need a different hal_i2c_send and wire it up instead of this one
    // this covers devices such as ATSHA204A and ATECCx08A that require a word address value pre-pended to the packet

    if (twi_master_write(i2c_master_instance, &packet) != TWI_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of I2C receive function for ASF I2C
 * \param[in]    iface          Device to interact with.
 * \param[in]    word_address   device word address
 * \param[out]   rxdata         Data received will be returned here.
 * \param[in,out] rxlength       As input, the size of the rxdata buffer.
 *                              As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength)
{

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int retries;
    uint32_t status = !ATCA_SUCCESS;
    uint16_t rxdata_max_size = *rxlength;
    uint8_t min_response_size = 4;
    uint16_t read_length = 2;
    Twi * i2c_master_instance;
    i2c_sam_instance_t * plib;

    twi_packet_t packet = {
        .chip        = cfg->atcai2c.slave_address >> 1,
        .addr        = { 0 },
        .addr_length = 0,
        .buffer      = rxdata,
        .length      = read_length
    };

    if ((NULL == cfg) || (NULL == rxlength) || (NULL == rxdata))
    {
        return ATCA_TRACE(ATCA_INVALID_POINTER, "NULL pointer encountered");
    }

    plib = (i2c_sam_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (Twi*)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }

    *rxlength = 0;
    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }
    //Read Length byte i.e. first byte from device

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

        packet.length = read_length;
        if (twi_master_read(i2c_master_instance, &packet) != TWI_SUCCESS)
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
            min_response_size += 1;
        }
        else
        {
            read_length =  rxdata[0];
        }

        if (read_length > rxdata_max_size)
        {
            status = ATCA_SMALL_BUFFER;
            ATCA_TRACE(status, "rxdata is small buffer");
        }

        if (read_length < min_response_size)
        {
            status = ATCA_TRACE(ATCA_RX_FAIL, "packet size is invalid");
            break;
        }

        /* Read given length bytes from device */
        packet.length = read_length - 2;
        packet.buffer = &rxdata[2];

        if (twi_master_read(i2c_master_instance, &packet) != TWI_SUCCESS)
        {
            status = ATCA_TRACE(ATCA_RX_FAIL, "hal_i2c_receive - failed");
            break;
        }

    }
    while (0);

    *rxlength = read_length;
    return status;
}


/**
 * \brief wake up CryptoAuth device using I2C bus
 *
 * \param[in] iface  interface to logical device to wakeup
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int retries;
    uint32_t bdrt = cfg->atcai2c.baud;
    int status = !TWI_SUCCESS;
    uint8_t data[4];
    Twi * i2c_master_instance;
    i2c_sam_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    retries = cfg->rx_retries;
    bdrt = cfg->atcai2c.baud;
    plib = (i2c_sam_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (Twi*)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }

    if (bdrt != 100000)    // if not already at 100KHz, change it
    {
        plib->change_baudrate(iface, 100000);
    }


    // Send 0x00 as wake pulse
    twi_write_byte(i2c_master_instance, 0x00);

    // rounded up to the nearest ms
    atca_delay_ms(((uint32_t)cfg->wake_delay + (1000 - 1)) / 1000);   // wait tWHI + tWLO which is configured based on device type and configuration structure

    // if necessary, revert baud rate to what came in.
    if (bdrt != 100000)
    {
        plib->change_baudrate(iface, bdrt);
    }

    twi_packet_t packet = {
        .chip        = cfg->atcai2c.slave_address >> 1,
        .addr        = { 0 },
        .addr_length = 0,
        .buffer      = data,
        .length      = 4
    };

    while (retries-- > 0 && status != TWI_SUCCESS)
    {
        status = twi_master_read(i2c_master_instance, &packet);
    }
    if (status != TWI_SUCCESS)
    {
        if (retries <= 0)
        {
            return ATCA_TOO_MANY_COMM_RETRIES;
        }

        return ATCA_COMM_FAIL;
    }

    return hal_check_wake(data, 4);
}

/**
 * \brief idle CryptoAuth device using I2C bus
 *
 * \param[in] iface  interface to logical device to idle
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data = 0x02; // idle word address value

    Twi * i2c_master_instance;
    i2c_sam_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_sam_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (Twi*)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }
    twi_packet_t packet = {
        .chip        = cfg->atcai2c.slave_address >> 1,
        .addr        = { 0 },
        .addr_length = 0,
        .buffer      = &data,
        .length      = 1
    };

    if (twi_master_write(i2c_master_instance, &packet) != TWI_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/**
 * \brief sleep CryptoAuth device using I2C bus
 *
 * \param[in] iface  interface to logical device to sleep
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data = 0x01; // sleep word address value
    Twi * i2c_master_instance;
    i2c_sam_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_sam_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_master_instance = (Twi*)plib->i2c_instance;

    if (!i2c_master_instance)
    {
        return ATCA_BAD_PARAM;
    }
    twi_packet_t packet = {
        .chip        = cfg->atcai2c.slave_address >> 1,
        .addr        = { 0 },
        .addr_length = 0,
        .buffer      = &data,
        .length      = 1
    };

    if (twi_master_write(i2c_master_instance, &packet) != TWI_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/**
 * \brief manages reference count on given bus and releases resource if no more refernces exist
 *
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_release(void *hal_data)
{
    return ATCA_SUCCESS;
}

/** @} */
