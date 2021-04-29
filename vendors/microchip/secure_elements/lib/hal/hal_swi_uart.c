/**
 * \file
 * \brief ATCA Hardware abstraction layer for SWI over UART drivers.
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

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using SWI Interface.
 *
   @{ */

/** \brief initialize an SWI interface using given config
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_swi_init(ATCAIface iface, ATCAIfaceCfg *cfg)
{

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of SWI post init
 * \param[in] iface  instance
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_swi_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/**
 * \brief Receive one bit as byte using uart for SWI
 * \param[in] bit_data byte equivalent of bit data on SWI
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS hal_swi_uart_receive_bit(ATCAIface iface, uint8_t *bit_data)
{
    uint16_t rxlen = 1;

    return iface->phy->halreceive(iface, 0, bit_data, &rxlen);
}

/**
 * \brief Send one bit as byte using uart for SWI
 * \param[in] bit_data byte equivalent of bit data on SWI
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS hal_swi_uart_send_bit(ATCAIface iface, uint8_t data)
{
    ATCA_STATUS status;
    uint8_t read_data;

    if (ATCA_SUCCESS == (status = iface->phy->halsend(iface, 0xFF, &data, 1)))
    {
        //Nothing to process... Reading to ensure write is complete
        status = hal_swi_uart_receive_bit(iface, &read_data);
        if (ATCA_SUCCESS == status)
        {
            if (read_data != data)
            {
                status = ATCA_TRACE(ATCA_TX_FAIL, "Tx send failed");
            }
        }
    }

    return status;
}

/** \brief HAL implementation of SWI send command over UART
 * \param[in] iface         instance
 * \param[in] word_address  device transaction type
 * \param[in] txdata        pointer to space to bytes to send
 * \param[in] txlength      number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_swi_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    (void)word_address;
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t i, bit_mask, bit_data;

    (void)iface->phy->halcontrol(iface, ATCA_HAL_FLUSH_BUFFER, NULL, 0);

    for (i = 0; i < txlength; i++)
    {
        for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1)
        {
            // Send one byte that represent one bit, 0x7F for one or 0x7D for zero
            // The LSB (least significant bit) is sent first.
            bit_data = (bit_mask & *txdata) ? 0x7F : 0x7D;
            status = hal_swi_uart_send_bit(iface, bit_data);
            if (status != ATCA_SUCCESS)
            {
                return ATCA_COMM_FAIL;
            }
        }
        txdata++;
    }

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of SWI receive function over UART
 * \param[in]    iface         Device to interact with.
 * \param[in]    word_address  device transaction type
 * \param[out]   rxdata        Data received will be returned here.
 * \param[in,out] rxlength     As input, the size of the rxdata buffer.
 *                             As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_swi_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCA_STATUS status = ATCA_COMM_FAIL;
    uint8_t bit_mask, bit_data = 0;

    if ((cfg == NULL) || (rxlength == NULL) || (rxdata == NULL) || (*rxlength < 1))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    (void)word_address;

    for (int i = 0; i < *rxlength; i++, rxdata++)
    {
        for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1)
        {
            bit_data = 0;
            status = hal_swi_uart_receive_bit(iface, &bit_data);
            if (status != ATCA_SUCCESS)
            {
                break;
            }
            if ((bit_data ^ 0x7F) < 2)
            {
                *rxdata |= bit_mask;
            }
        }
    }

    return status;
}

/**
 * \brief Send Wake flag via SWI.
 *
 * \param[in] iface  interface of the logical device to wake up
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_swi_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCA_STATUS status = ATCA_COMM_FAIL;
    uint32_t temp;
    uint16_t rxlength = 4;

    if (cfg == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    // Clear the rx ring buffer, before transmitting any data, because Tx and Rx are short
    // Tx data will be appeared in Rx ring buffer
    (void)iface->phy->halcontrol(iface, ATCA_HAL_FLUSH_BUFFER, NULL, 0);

    do
    {
        temp = 115200UL;
        (void)iface->phy->halcontrol(iface, ATCA_HAL_CHANGE_BAUD, (uint8_t*)&temp, rxlength);

        status = hal_swi_uart_send_bit(iface, CALIB_SWI_FLAG_WAKE);

        //Reset to Normal baud rate
        temp = 230400UL;
        (void)iface->phy->halcontrol(iface, ATCA_HAL_CHANGE_BAUD, (uint8_t*)&temp, rxlength);

        // Wait tWHI + tWLO
        atca_delay_us(cfg->wake_delay);

        temp = CALIB_SWI_FLAG_TX;
        hal_swi_send(iface, 0xFF, (uint8_t*)&temp, 1);

        // Read Wake response
        temp = 0;
        if (ATCA_SUCCESS == (status = hal_swi_receive(iface, 0x00, (uint8_t*)&temp, &rxlength)))
        {
            status = hal_check_wake((uint8_t*)&temp, sizeof(temp));
        }
    }
    while (0);

    return status;
}

/**
 * \brief Send Sleep flag via SWI.
 *
 * \param[in] iface  interface of the logical device to sleep
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_swi_sleep(ATCAIface iface)
{
    uint8_t temp = CALIB_SWI_FLAG_SLEEP;

    return hal_swi_send(iface, 0xFF, &temp, sizeof(temp));
}

/**
 * \brief Send Idle flag via SWI.
 *
 * \param[in] iface  interface of the logical device to idle
 *
 * \return ATCA_SUCCES
 */
ATCA_STATUS hal_swi_idle(ATCAIface iface)
{
    uint8_t temp = CALIB_SWI_FLAG_IDLE;

    return hal_swi_send(iface, 0xFF, &temp, sizeof(temp));
}


/** \brief Perform control operations for the kit protocol
 * \param[in]     iface          Interface to interact with.
 * \param[in]     option         Control parameter identifier
 * \param[in]     param          Optional pointer to parameter value
 * \param[in]     paramlen       Length of the parameter
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_swi_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    if (iface && iface->mIfaceCFG)
    {
        switch (option)
        {
        case ATCA_HAL_CONTROL_WAKE:
            return hal_swi_wake(iface);
        case ATCA_HAL_CONTROL_IDLE:
            return hal_swi_idle(iface);
        case ATCA_HAL_CONTROL_SLEEP:
            return hal_swi_sleep(iface);
        case ATCA_HAL_CHANGE_BAUD:
            return iface->phy->halcontrol(iface, ATCA_HAL_CHANGE_BAUD, param, paramlen);
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

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_swi_release(void *hal_data)
{

    return ATCA_SUCCESS;
}

/** @} */
