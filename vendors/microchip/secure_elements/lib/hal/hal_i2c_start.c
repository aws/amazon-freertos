/**
 * \file
 * \brief ATCA Hardware abstraction layer for SAMD21 I2C over START drivers.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the START I2C primitives to set up the interface.
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
 #include <string.h>
#include <stdio.h>
#include <atmel_start.h>
#include <hal_gpio.h>
#include <hal_delay.h>
#include "hal_i2c_start.h"
#include "atca_start_config.h"
#include "atca_start_iface.h"
#include "cryptoauthlib.h"

#ifndef ATCA_HAL_LEGACY_API
#error "The use of this hal requires the ATCA_HAL_LEGACY_API option to be enabled.
#endif

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */


/* Notes:
    - this HAL implementation assumes you've included the Atmel START SERCOM I2C libraries in your project, otherwise,
    the HAL layer will not compile because the START I2C drivers are a dependency *
 */

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{

    /* if every SERCOM was a likely candidate bus, then would need to initialize the entire array to all SERCOM n numbers.
     * As an optimization and making discovery safer, make assumptions about bus-num / SERCOM map based on D21 Xplained Pro board
     * If you were using a raw D21 on your own board, you would supply your own bus numbers based on your particular hardware configuration.
     */

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
    - this HAL implementation assumes you've included the START Twi libraries in your project, otherwise,
    the HAL layer will not compile because the START TWI drivers are a dependency *
 */

/** \brief hal_i2c_init manages requests to initialize a physical interface.  it manages use counts so when an interface
 * has released the physical layer, it will disable the interface for some other use.
 * You can have multiple ATCAIFace instances using the same bus, and you can have multiple ATCAIFace instances on
 * multiple i2c buses, so hal_i2c_init manages these things and ATCAIFace is abstracted from the physical details.
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

/** \brief HAL implementation of I2C send over START
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    struct i2c_m_sync_desc * i2c_descriptor_instance;
    i2c_start_instance_t * plib;

    struct _i2c_m_msg packet = {
        .addr   = cfg->atcai2c.slave_address >> 1,
        .buffer = txdata,
        .flags  = I2C_M_SEVEN | I2C_M_STOP,
    };

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_start_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_descriptor_instance = (struct i2c_m_sync_desc *)plib->i2c_descriptor;

    if (!i2c_descriptor_instance)
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

    packet.len = txlength;

    if (i2c_m_sync_transfer(i2c_descriptor_instance, &packet) != I2C_OK)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C receive function for START I2C
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
    struct i2c_m_sync_desc * i2c_descriptor_instance;
    i2c_start_instance_t * plib;

    struct _i2c_m_msg packet = {
        .addr   = cfg->atcai2c.slave_address >> 1,
        .len    = 1,
        .buffer = rxdata,
        .flags  = I2C_M_SEVEN | I2C_M_RD | I2C_M_STOP,
    };

    if ((NULL == cfg) || (NULL == rxlength) || (NULL == rxdata))
    {
        return ATCA_TRACE(ATCA_INVALID_POINTER, "NULL pointer encountered");
    }

    plib = (i2c_start_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_descriptor_instance = (struct i2c_m_sync_desc *)plib->i2c_descriptor;

    if (!i2c_descriptor_instance)
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
        /*Update the length to be received from device */
        packet.len = read_length;

        /* Read length bytes to know number of bytes to read */
        if (i2c_m_sync_transfer(i2c_descriptor_instance, &packet) != I2C_OK)
        {
            status = ATCA_RX_FAIL;
            ATCA_TRACE(status, "hal_i2c_receive - failed");
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
        packet.len = read_length - 2;
        packet.buffer = &rxdata[2];

        /* Read given length bytes from device */
        if (i2c_m_sync_transfer(i2c_descriptor_instance, &packet) != I2C_OK)
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
    int retries = cfg->rx_retries;
    uint32_t bdrt = cfg->atcai2c.baud;
    int status = !I2C_OK;
    uint8_t data[4];
    struct i2c_m_sync_desc * i2c_descriptor_instance;
    i2c_start_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    retries = cfg->rx_retries;
    bdrt = cfg->atcai2c.baud;
    plib = (i2c_start_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_descriptor_instance = (struct i2c_m_sync_desc *)plib->i2c_descriptor;

    if (!i2c_descriptor_instance)
    {
        return ATCA_BAD_PARAM;
    }


    if (bdrt != 100000)    // if not already at 100KHz, change it
    {
        plib->change_baudrate(iface, 100000);
    }

    // send the wake by writing to an address of 0x00
    struct _i2c_m_msg packet = {
        .addr   = 0x00,
        .len    = 0,
        .buffer = NULL,
        .flags  = I2C_M_SEVEN | I2C_M_STOP,
    };

    // Send the 00 address as the wake pulse; part will NACK, so don't check for status
    status = i2c_m_sync_transfer(i2c_descriptor_instance, &packet);

    // wait tWHI + tWLO which is configured based on device type and configuration structure
    delay_us(cfg->wake_delay);

    // receive the wake up response
    packet.addr = cfg->atcai2c.slave_address >> 1;
    packet.len = 4;
    packet.buffer = data;
    packet.flags  = I2C_M_SEVEN | I2C_M_RD | I2C_M_STOP;

    while (retries-- > 0 && status != I2C_OK)
    {
        status = i2c_m_sync_transfer(i2c_descriptor_instance, &packet);
    }

    if (status == I2C_OK)
    {
        // if necessary, revert baud rate to what came in.
        if (bdrt != 100000)
        {
            plib->change_baudrate(iface, 100000);
        }
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
    uint8_t data[4];
    struct i2c_m_sync_desc * i2c_descriptor_instance;
    i2c_start_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_start_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_descriptor_instance = (struct i2c_m_sync_desc *)plib->i2c_descriptor;

    if (!i2c_descriptor_instance)
    {
        return ATCA_BAD_PARAM;
    }
    struct _i2c_m_msg packet = {
        .addr   = cfg->atcai2c.slave_address >> 1,
        .len    = 1,
        .buffer = &data[0],
        .flags  = I2C_M_SEVEN | I2C_M_STOP,
    };

    data[0] = 0x02;  // idle word address value
    if (i2c_m_sync_transfer(i2c_descriptor_instance, &packet) != I2C_OK)
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
    uint8_t data[4];
    struct i2c_m_sync_desc * i2c_descriptor_instance;
    i2c_start_instance_t * plib;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (i2c_start_instance_t*)cfg->cfg_data;

    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    i2c_descriptor_instance = (struct i2c_m_sync_desc *)plib->i2c_descriptor;

    if (!i2c_descriptor_instance)
    {
        return ATCA_BAD_PARAM;
    }

    struct _i2c_m_msg packet = {
        .addr   = cfg->atcai2c.slave_address >> 1,
        .len    = 1,
        .buffer = &data[0],
        .flags  = I2C_M_SEVEN | I2C_M_STOP,
    };

    data[0] = 0x01;  // sleep word address value
    if (i2c_m_sync_transfer(i2c_descriptor_instance, &packet) != I2C_OK)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{
    return ATCA_SUCCESS;
}

/** @} */
