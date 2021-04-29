/**
 * \file
 * \brief ATCA Hardware abstraction layer for SWI uart over Harmony PLIB.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the Harmony UART (ring buffer mode) primitives to set up the interface.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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
 *
 */

#include "atca_config.h"
#include "cryptoauthlib.h"

PLIB_SWI_SERIAL_SETUP serial_setup = {
    .parity    = PLIB_SWI_PARITY_NONE,
    .dataWidth = PLIB_SWI_DATA_WIDTH,
    .stopBits  = PLIB_SWI_STOP_BIT
};


/** \brief The function flush the unread data from rx ring buffer
 */
static ATCA_STATUS hal_uart_flush_buffer(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_plib_swi_uart_api_t* plib;
    uint8_t dummy_read;

    if (cfg && cfg->cfg_data)
    {
        plib = (atca_plib_swi_uart_api_t*)cfg->cfg_data;

        // Until rx ring buffer gets cleared, read out all data from rx buffer
        while (plib->readcount_get() >= 1)
        {
            plib->read(&dummy_read, 1);
        }
    }
    return ATCA_SUCCESS;
}

/**
 * \brief Set baudrate default is 230KHz.
 *
 * \param[in] baudrate  contain new baudrate
 *
 * \return true on success, otherwise false.
 */
static ATCA_STATUS hal_uart_set_baudrate(ATCAIface iface, uint32_t baudrate)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_plib_swi_uart_api_t* plib;
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (cfg && cfg->cfg_data)
    {
        plib = (atca_plib_swi_uart_api_t*)cfg->cfg_data;

        serial_setup.baudRate = baudrate;

        status = plib->serial_setup(&serial_setup, 0) ? ATCA_SUCCESS : ATCA_GEN_FAIL;
    }

    return status;
}

/**
 * \brief Initialize an uart interface using given config.
 *
 * \param[in] hal  opaque pointer to HAL data
 * \param[in] cfg  interface configuration
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_uart_init(ATCAIface iface, ATCAIfaceCfg *cfg)
{
    atca_plib_swi_uart_api_t* plib;

    if (cfg == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    plib = (atca_plib_swi_uart_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of SWI post init.
 *
 * \param[in] iface  ATCAIface instance
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_uart_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/**
 * \brief Send byte(s) via SWI.
 *
 * \param[in] iface         interface of the logical device to send data to
 * \param[in] word_address  device transaction type
 * \param[in] txdata        pointer to bytes to send
 * \param[in] txlength      number of bytes to send
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_uart_send(ATCAIface iface, uint8_t word_address, uint8_t* txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_plib_swi_uart_api_t* plib;
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (cfg && cfg->cfg_data && txdata && txlength)
    {
        plib = (atca_plib_swi_uart_api_t*)cfg->cfg_data;

        (void)plib->error_get();
        if (txlength == plib->write(txdata, txlength))
        {
            status = ATCA_SUCCESS;
        }
        else
        {
            status = ATCA_RX_FAIL;
        }
    }

    return status;
}


/**
 * \brief Receive byte(s) via SWI.
 * \param[in]    iface         Device to interact with.
 * \param[in]    word_address  device transaction type
 * \param[out]   rxdata        Data received will be returned here.
 * \param[in,out] rxlength     As input, the size of the rxdata buffer.
 *                             As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_uart_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t* rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_plib_swi_uart_api_t* plib;
    ATCA_STATUS status = ATCA_BAD_PARAM;
    int16_t timeout = 300;

    if (cfg && cfg->cfg_data && rxdata && rxlength)
    {
        plib = (atca_plib_swi_uart_api_t*)cfg->cfg_data;

        while ((plib->readcount_get() < *rxlength) && (timeout > 0))
        {
            atca_delay_us(25);
            timeout -= 25;
        }

        if (*rxlength == plib->read(rxdata, *rxlength))
        {
            status = ATCA_SUCCESS;
        }
        else
        {
            status = ATCA_RX_FAIL;
        }
    }

    return status;
}

ATCA_STATUS hal_uart_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    if (iface && iface->mIfaceCFG)
    {
        switch (option)
        {
        case ATCA_HAL_CHANGE_BAUD:
            return hal_uart_set_baudrate(iface, *(uint32_t*)param);
        case ATCA_HAL_FLUSH_BUFFER:
            return hal_uart_flush_buffer(iface);
        case ATCA_HAL_CONTROL_SELECT:
        /* fallthrough */
        case ATCA_HAL_CONTROL_DESELECT:
            return ATCA_SUCCESS;
        default:
            return ATCA_UNIMPLEMENTED;
        }
    }
    return ATCA_BAD_PARAM;
}

/**
 * \brief Manages reference count on given bus and releases resource if
 *        no more reference(s) exist.
 *
 * \param[in] hal_data  opaque pointer to hal data structure - known only
 *                      to the HAL implementation
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_uart_release(void *hal_data)
{
    return ATCA_SUCCESS;
}
