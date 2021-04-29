/**
 * \file
 * \brief ATCA Hardware abstraction layer for SAMD21 I2C over Harmony PLIB.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the Harmony I2C primitives to set up the interface.
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

#include "cryptoauthlib.h"


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
 * of the a-prior knowledge
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    i2c_buses[0] = 0;
    return ATCA_SUCCESS;
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

static ATCA_STATUS hal_i2c_wait(atca_plib_i2c_api_t* plib, uint32_t rate, uint16_t length)
{
    ATCA_STATUS status = ATCA_SUCCESS;

    /* I2C Address, start & stop, R/W and ACK (11 bits) + packet size */
    /* Maximum packet size is 1024 bytes (TA device) and 200 bytes (CA device) */
    length += 2;
    /* so assume rate can be sub 1kHz */
    uint32_t timeout = (uint32_t)length * 9 * 1000;

    rate /= 1000;
    timeout /= rate;
    timeout += 1;   /* Make sure the timeout value is non zero */

    while ((true == plib->is_busy()) && (timeout--))
    {
        atca_delay_us(1);
    }

    if (0 == timeout)
    {
        status = ATCA_COMM_FAIL;
    }

    return status;
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

ATCA_STATUS hal_i2c_init(ATCAIface iface, ATCAIfaceCfg *cfg)
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
 * \param[in] iface         instance
 * \param[in] word_address  device transaction type
 * \param[in] txdata        pointer to space to bytes to send
 * \param[in] txlength      number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg* cfg = atgetifacecfg(iface);
    atca_plib_i2c_api_t * plib;
    ATCA_STATUS status = ATCA_COMM_FAIL;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (atca_plib_i2c_api_t*)cfg->cfg_data;
    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    /* Wait for the I2C bus to be ready */
    /* Since the wait time is unknown, waiting for 30 bytes duration */
    status = hal_i2c_wait(plib, cfg->atcai2c.baud, 30);

    if (ATCA_SUCCESS == status)
    {
        status = ATCA_COMM_FAIL;
        if (plib->write(address >> 1, txdata, txlength) == true)
        {
            /* Wait for the I2C transfer to complete */
            status = hal_i2c_wait(plib, cfg->atcai2c.baud, txlength);

            if (ATCA_SUCCESS == status)
            {
                /* Transfer complete. Check if the transfer was successful */
                if (plib->error_get() != PLIB_I2C_ERROR_NONE)
                {
                    status = ATCA_TRACE(ATCA_COMM_FAIL, "plib->write failed");
                }
            }
        }
    }

    return status;
}

/** \brief HAL implementation of I2C receive function for START I2C
 * \param[in]    iface         Device to interact with.
 * \param[in]    word_address  device transaction type
 * \param[out]   rxdata        Data received will be returned here.
 * \param[in,out] rxlength     As input, the size of the rxdata buffer.
 *                             As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t address, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCA_STATUS status = ATCA_COMM_FAIL;
    ATCAIfaceCfg* cfg = atgetifacecfg(iface);
    atca_plib_i2c_api_t * plib;

    if ((NULL == cfg) || (NULL == rxlength) || (NULL == rxdata))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    if (NULL == (plib = (atca_plib_i2c_api_t*)cfg->cfg_data))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    /* Read given length bytes from device */
    status = ATCA_COMM_FAIL;
    if (plib->read(address >> 1, rxdata, *rxlength) == true)
    {
        /* Wait for the I2C transfer to complete */
        if (ATCA_SUCCESS == (status = hal_i2c_wait(plib, cfg->atcai2c.baud, *rxlength)))
        {
            /* Transfer complete. Check if the transfer was successful */
            if (plib->error_get() != PLIB_I2C_ERROR_NONE)
            {
                status = ATCA_COMM_FAIL;
            }
        }
    }
    if (ATCA_SUCCESS != status)
    {
        status = ATCA_TRACE(status, "plib->read - failed");
    }

    return status;
}

/** \brief method to change the bus speec of I2C
 * \param[in] iface  interface on which to change bus speed
 * \param[in] speed  baud rate (typically 100000 or 400000)
 */
ATCA_STATUS change_i2c_speed(ATCAIface iface, uint32_t speed)
{
    ATCAIfaceCfg* cfg = atgetifacecfg(iface);
    atca_plib_i2c_api_t * plib;
    ATCA_STATUS status = ATCA_COMM_FAIL;

    if (!cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (atca_plib_i2c_api_t*)cfg->cfg_data;
    if (!plib)
    {
        return ATCA_BAD_PARAM;
    }

    PLIB_I2C_TRANSFER_SETUP setup;
    setup.clkSpeed = speed;

    /* Make sure I2C is not busy before changing the I2C clock speed */
    /* Since wait time is unknown, wait for 30 bytes */
    status = hal_i2c_wait(plib, cfg->atcai2c.baud, 30);

    if (ATCA_SUCCESS == status)
    {
        (void)plib->transfer_setup(&setup, 0);
    }

    return status;
}

/** \brief Perform control operations for the kit protocol
 * \param[in]     iface          Interface to interact with.
 * \param[in]     option         Control parameter identifier
 * \param[in]     param          Optional pointer to parameter value
 * \param[in]     paramlen       Length of the parameter
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    if (iface && iface->mIfaceCFG)
    {
        if (ATCA_HAL_CHANGE_BAUD == option)
        {
            return change_i2c_speed(iface, *(uint32_t*)param);
        }
        else
        {
            return ATCA_UNIMPLEMENTED;
        }
    }
    return ATCA_BAD_PARAM;
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
