/**
 * \file
 * \brief ATCA Hardware abstraction layer for SWI bit banging.
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
 */

#include "cryptoauthlib.h"
#include "atca_config.h"

/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief These methods define the hardware abstraction layer for
 *        communicating with a CryptoAuth device using SWI bit banging.
   @{ */

/** \brief discover swi buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application.This function is currently not supported.
 * of the a-priori knowledge
 * \param[in] swi_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_swi_discover_buses(int swi_buses[], int max_buses)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number.This function is curently not supported.
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_swi_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/**
 * \brief Send a Wake Token.
 */
static ATCA_STATUS swi_send_wake_token(atca_plib_swi_bb_api_t* plib, uint8_t pin)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (plib)
    {
#ifdef __arm__
        __disable_irq();
#else
        __builtin_disable_interrupts();
#endif

        plib->write(pin, false);
        atca_delay_us(60);
        plib->write(pin, true);
        status = ATCA_SUCCESS;

#ifdef __arm__
        __enable_irq();
#else
        __builtin_enable_interrupts();
#endif
    }

    return status;
}

/**
 * \brief Send a number of bytes.This function should not be called directly ,instead should use hal_swi_send() which call this function.
 *
 * \param[in] count   number of bytes to send.
 * \param[in] buffer  pointer to buffer containing bytes to send
 */
static ATCA_STATUS swi_send_bytes(atca_plib_swi_bb_api_t* plib, uint8_t pin, uint8_t count,
                                  uint8_t *buffer)
{
    uint8_t i, bit_mask;

    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    //set port direction to output
    plib->set_pin_output_dir(pin);

#ifdef __arm__
    __disable_irq();
#else
    __builtin_disable_interrupts();
#endif
    for (i = 0; i < count; i++)
    {
        for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1)
        {
            if (bit_mask & buffer[i])   //!< Send Logic 1 (7F)
            {
                plib->write(pin, false);
                BIT_DELAY_1L;
                plib->write(pin, true);
                BIT_DELAY_7;
            }
            else     //!< Send Logic 0 (7D)
            {
                plib->write(pin, false);
                BIT_DELAY_1L;
                plib->write(pin, true);
                BIT_DELAY_1H;
                plib->write(pin, false);
                BIT_DELAY_1L;
                plib->write(pin, true);
                BIT_DELAY_5;
            }
        }
    }
#ifdef __arm__
    __enable_irq();
#else
    __builtin_enable_interrupts();
#endif

    return ATCA_SUCCESS;
}

/**
 * \brief Send one byte.
 *
 * \param[in] byte  byte to send
 */
static ATCA_STATUS swi_send_byte(atca_plib_swi_bb_api_t* plib, uint8_t pin, uint8_t byte)
{
    return swi_send_bytes(plib, pin, 1, &byte);
}

/**
 * \brief Receive one bit as byte using SWI bitbang
 * \param[in] bit_data byte equivalent of bit data on SWI
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS swi_receive_bit(atca_plib_swi_bb_api_t* plib, uint8_t pin, uint8_t *bit_data)
{
    int8_t timeout_count;
    bool current_state = true, prev_state = true;
    uint8_t bit_count = 0;
    ATCA_STATUS status = ATCA_SUCCESS;

    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    do
    {
        //wait for start bit
        timeout_count = 90 / 2;
        while (timeout_count-- >= 0)
        {
            current_state = plib->read(pin);
            if (current_state != prev_state)
            {
                prev_state = current_state;
                if (!current_state)
                {
                    break;
                }

            }
            atca_delay_us(2);
        }

        if (timeout_count <= 0)
        {
            status = ATCA_TRACE(ATCA_RX_NO_RESPONSE, "No response is received");
            break;
        }

        //Read bit
        timeout_count = 40 / 2;
        while (timeout_count-- >= 0)
        {
            current_state = plib->read(pin);
            if (current_state != prev_state)
            {
                prev_state = current_state;
                if (current_state)
                {
                    bit_count++;
                }

            }
            atca_delay_us(2);
        }

        if (bit_count == 2)
        {
            *bit_data = 0;
        }
        else if (bit_count == 1)
        {
            *bit_data = 1;
        }
        else
        {
            status = ATCA_TRACE(ATCA_RX_FAIL, "Rx Receive - failed");
        }
    }
    while (0);

    return status;
}

/**
 * \brief Receive a number of bytes.This function should not be called directly ,instead should use hal_swi_receive() which call this function.
 *
 * \param[in]  count   number of bytes to receive
 * \param[out] buffer  pointer to receive buffer
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS swi_receive_bytes(atca_plib_swi_bb_api_t* plib, uint8_t pin, uint8_t count,
                                     uint8_t *buffer)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t i;
    uint8_t bit_mask, bit_data;

    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    //set port direction to input to receive response
    plib->set_pin_input_dir(pin);

#ifdef __arm__
    __disable_irq();
#else
    __builtin_disable_interrupts();
#endif
    //! Receive bits and store in buffer.
    for (i = 0; i < count; i++)
    {
        *buffer = 0;
        for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1)
        {
            bit_data = 0;
            if (ATCA_SUCCESS != (status = swi_receive_bit(plib, pin, &bit_data)))
            {
                break;
            }


            if (bit_data == 1)
            {
                *buffer |=  bit_mask;
            }

        }

        if (status != ATCA_SUCCESS)
        {
            break;
        }


        if (i == 0)
        {
            if (*buffer < ATCA_RSP_SIZE_MIN)
            {
                status = ATCA_TRACE(ATCA_RX_FAIL, "packet size is invalid");
                break;
            }
            else if (*buffer > count)
            {
                status = ATCA_TRACE(ATCA_SMALL_BUFFER, "rxdata is small buffer");
                break;
            }
            else
            {
                count = *buffer;
                status = ATCA_SUCCESS;
            }
        }
        buffer++;
    }

#ifdef __arm__
    __enable_irq();
#else
    __builtin_enable_interrupts();
#endif
    RX_TX_DELAY;

    return status;
}

/**
 * \brief hal_swi_init manages requests to initialize a physical
 *        interface. It manages use counts so when an interface has
 *        released the physical layer, it will disable the interface for
 *        some other use. You can have multiple ATCAIFace instances using
 *        the same bus, and you can have multiple ATCAIFace instances on
 *        multiple swi buses, so hal_swi_init manages these things and
 *        ATCAIFace is abstracted from the physical details.
 */

/**
 * \brief Initialize an SWI interface using given config.
 *
 * \param[in] hal  opaque pointer to HAL data
 * \param[in] cfg  interface configuration
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_swi_init(void *hal, ATCAIfaceCfg *cfg)
{
    atca_plib_swi_bb_api_t* plib = NULL;

    if (cfg == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    plib = (atca_plib_swi_bb_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    // Set crypto pin direction as output
    plib->set_pin_output_dir(cfg->atcaswi.bus);

    // Set the crypto pin
    plib->write(cfg->atcaswi.bus, true);

    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of SWI post init.
 *
 * \param[in] iface  ATCAIface instance
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_swi_post_init(ATCAIface iface)
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
ATCA_STATUS hal_swi_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCA_STATUS status = ATCA_COMM_FAIL;
    atca_plib_swi_bb_api_t* plib = NULL;

    if ((cfg == NULL) || (txdata == NULL) || (txlength < 1))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    plib = (atca_plib_swi_bb_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    (void)word_address;

    //! Send Command Flag
    if ((status = swi_send_byte(plib, cfg->atcaswi.bus, SWI_FLAG_CMD)) == ATCA_SUCCESS)
    {
        //! Skip the Word Address data as SWI doesn't use it
        txdata++;

        //! Send the remaining bytes
        status = swi_send_bytes(plib, cfg->atcaswi.bus, txlength, txdata);
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
ATCA_STATUS hal_swi_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCA_STATUS status = ATCA_COMM_FAIL;
    int retries;
    uint16_t rxdata_max_size;
    atca_plib_swi_bb_api_t* plib = NULL;

    if ((cfg == NULL) || (rxdata == NULL) || (rxlength == NULL) || (*rxlength < 1))
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    plib = (atca_plib_swi_bb_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    (void)word_address;
    retries = cfg->rx_retries;
    rxdata_max_size = *rxlength;
    *rxlength = 0;
    do
    {
        while (retries-- > 0 && status != ATCA_SUCCESS)
        {
            if ((status = swi_send_byte(plib, cfg->atcaswi.bus, SWI_FLAG_TX)) == ATCA_SUCCESS)
            {
                status = swi_receive_bytes(plib, cfg->atcaswi.bus, rxdata_max_size, rxdata);
            }
        }

        if (status != ATCA_SUCCESS)
        {
            break;
        }

        *rxlength = rxdata[0];

    }
    while (0);


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
    ATCA_STATUS status = ATCA_WAKE_FAILED;
    uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };
    uint16_t rxlength  = sizeof(data);
    atca_plib_swi_bb_api_t* plib = NULL;

    if (cfg == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    plib = (atca_plib_swi_bb_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    do
    {
        //! Generate Wake Token
        if (ATCA_SUCCESS != (status = swi_send_wake_token(plib, cfg->atcaswi.bus)))
        {
            break;
        }

        //! Wait tWHI + tWLO
        atca_delay_us(cfg->wake_delay);

        // Read Wake response
        if (ATCA_SUCCESS != (status = hal_swi_receive(iface, 0x00, data, &rxlength)))
        {
            break;
        }

        status = hal_check_wake(data, rxlength);

    }
    while (0);

    return status;
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
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_plib_swi_bb_api_t* plib = NULL;

    if (cfg == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    plib = (atca_plib_swi_bb_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    return swi_send_byte(plib, cfg->atcaswi.bus, SWI_FLAG_IDLE);
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
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_plib_swi_bb_api_t* plib = NULL;

    if (cfg == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    plib = (atca_plib_swi_bb_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    return swi_send_byte(plib, cfg->atcaswi.bus, SWI_FLAG_SLEEP);
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
ATCA_STATUS hal_swi_release(void *hal_data)
{
    return ATCA_SUCCESS;
}

/** @} */