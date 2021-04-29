/**
 * \file
 * \brief ATCA Hardware abstraction layer for 1WIRE or SWI over GPIO.
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

#include "hal_gpio_harmony.h"

static ATCA_STATUS gpio_receive_bytes(ATCAIfaceCfg *cfg, uint8_t *rxdata, uint16_t rxlength);
static ATCA_STATUS gpio_send_bytes(ATCAIfaceCfg *cfg, uint8_t *txdata, uint16_t txlength);
static ATCA_STATUS send_logic_bit(ATCAIfaceCfg *cfg, bool bit_value);

#ifdef ATCA_HAL_1WIRE
static ATCA_STATUS start_stop_cond_1wire(ATCAIfaceCfg *cfg);
static ATCA_STATUS read_data_ACK_1wire(ATCAIfaceCfg *cfg, bool* bit_value);
static ATCA_STATUS device_discovery_1wire(ATCAIfaceCfg *cfg);
static ATCA_STATUS generate_wake_condition(ATCAIfaceCfg *cfg);
static ATCA_STATUS check_wake_1wire(ATCAIfaceCfg *cfg);
static uint8_t get_slave_addr_1wire(uint8_t dev_addr, uint8_t oper);
static ATCA_STATUS read_logic_bit_1wire(ATCAIfaceCfg *cfg, bool *bit_value);
#endif

#ifdef ATCA_HAL_SWI
static ATCA_STATUS read_logic_bit_swi(ATCAIfaceCfg *cfg, bool *bit_value);
static ATCA_STATUS check_wake_swi(ATCAIfaceCfg *cfg);
#endif

/** \brief initialize an GPIO interface using given config
 * \param[in] cfg - interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_gpio_init(void *hal, ATCAIfaceCfg *cfg)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    atca_plib_gpio_api_t* plib;

    if (cfg)
    {
        plib = (atca_plib_gpio_api_t*)cfg->cfg_data;

        if (plib)
        {
            /* GPIO direction as output */
            plib->pin_setup(cfg->atcaswi.bus, ATCA_GPIO_OUTPUT_DIR);
            /* by default, driving high */
            plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
            status = ATCA_SUCCESS;
        }
    }
    return status;
}

/**
 * \brief HAL implementation of GPIO post init.
 *
 * \param[in] iface  ATCAIface instance
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_gpio_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/** \brief Discovery Response sequence is used by the master to perform a general
   bus call to determine if a device is present on the bus
 * \param[in]    iface          Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_gpio_device_discovery(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_plib_gpio_api_t* plib;

    if (cfg)
    {
        plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
        if (plib)
        {
            status = ATCA_UNIMPLEMENTED;
            if (cfg->devtype == ATECC204A)
            {
                #ifdef ATCA_HAL_1WIRE
                status = device_discovery_1wire(cfg);
                #endif
            }
        }
    }

    return status;
}

/** \brief HAL implementation of bit banging send over Harmony
 * \param[in] iface         instance
 * \param[in] word_address  device transaction type
 * \param[in] txdata        pointer to space to bytes to send
 * \param[in] txlength      number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_gpio_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCA_STATUS status = ATCA_BAD_PARAM;

    #ifdef ATCA_HAL_1WIRE
    uint8_t dev_write_addr;
    #endif

    if (!cfg)
    {
        return status;
    }

    if (cfg->devtype == ATECC204A)
    {
        #ifdef ATCA_HAL_1WIRE
        if (ATCA_SUCCESS == (status = start_stop_cond_1wire(cfg)))
        {
            dev_write_addr = get_slave_addr_1wire(cfg->atcaswi.slave_address, ATCA_GPIO_WRITE);
            if (ATCA_SUCCESS == (status = gpio_send_bytes(cfg, &dev_write_addr, sizeof(dev_write_addr))))
            {
                if (ATCA_SUCCESS == (status = gpio_send_bytes(cfg, &word_address, sizeof(word_address))))
                {
                    status = gpio_send_bytes(cfg, txdata, txlength);
                }
            }
            status = start_stop_cond_1wire(cfg);
        }
        #endif
    }
    else
    {
        #ifdef ATCA_HAL_SWI
        if (ATCA_SUCCESS == (status = gpio_send_bytes(cfg, &word_address, sizeof(word_address))))
        {
            status = gpio_send_bytes(cfg, txdata, txlength);
        }
        #endif
    }

    return status;
}

/** \brief HAL implementation of bit banging receive from HARMONY
 * \param[in]    iface          Device to interact with.
 * \param[in]    word_address   device transaction type
 * \param[out]   rxdata         Data received will be returned here.
 * \param[in,out] rxlength      As input, the size of the rxdata buffer.
 *                              As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_gpio_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCA_STATUS status;
    int retries;

    if ((NULL == cfg) || (NULL == rxlength) || (NULL == rxdata))
    {
        return ATCA_BAD_PARAM;
    }

    if (cfg->devtype == ATECC204A)
    {
        #ifdef ATCA_HAL_1WIRE
        word_address = get_slave_addr_1wire(cfg->atcaswi.slave_address, ATCA_GPIO_READ);
        if (ATCA_SUCCESS != (status = start_stop_cond_1wire(cfg)))
        {
            return status;
        }
        #endif
    }
    else
    {
        word_address = ATCA_SWI_TX_WORD_ADDR;
    }

    retries = cfg->rx_retries;
    do
    {
        if (ATCA_SUCCESS == (status = gpio_send_bytes(cfg, &word_address, sizeof(word_address))))
        {
            /* Reading response length byte */
            if (ATCA_SUCCESS == (status = gpio_receive_bytes(cfg, &rxdata[0], ATCA_1WIRE_RESPONSE_LENGTH_SIZE)))
            {
                #ifdef ATCA_HAL_1WIRE
                if (cfg->devtype == ATECC204A)
                {
                    (void)send_ACK_1wire(cfg);
                }
                #endif
                if (rxdata[0] >= ATCA_MIN_RESPONSE_LENGTH)
                {
                    if (*rxlength < rxdata[0])
                    {
                        status = ATCA_TRACE(ATCA_SMALL_BUFFER, "rxdata is small buffer");
                        break;
                    }
                    else
                    {
                        /* Reading remaining response */
                        if (ATCA_SUCCESS == (status = gpio_receive_bytes(cfg, &rxdata[1], rxdata[0] - ATCA_1WIRE_RESPONSE_LENGTH_SIZE)))
                        {
                            #ifdef ATCA_HAL_1WIRE
                            if (cfg->devtype == ATECC204A)
                            {
                                (void)send_NACK_1wire(cfg);
                            }
                            #endif
                            *rxlength = rxdata[0];
                        }
                    }
                }
                else
                {
                    status = ATCA_TRACE(ATCA_COMM_FAIL, "invalid response length");
                }
            }
        }
    }
    while ((retries-- > 0) && (status == ATCA_COMM_FAIL));

#ifdef ATCA_HAL_1WIRE
    status = start_stop_cond_1wire(cfg);
#endif

    return status;
}

/** \brief Put the device in idle mode
 * \param[in] iface  interface to logical device to idle
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_gpio_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);

    if (cfg)
    {
        if (cfg->devtype == ATECC204A)
        {
            return ATCA_SUCCESS;
        }
        else
        {
            return hal_gpio_send(iface, ATCA_SWI_IDLE_WORD_ADDR, NULL, 0);
        }
    }

    return ATCA_BAD_PARAM;
}

/** \brief send sleep command
 * \param[in] iface  interface to logical device to sleep
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_gpio_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t word_address;

    if (cfg)
    {
        word_address = (cfg->devtype == ATECC204A) ? ATCA_1WIRE_SLEEP_WORD_ADDR : \
                       ATCA_SWI_SLEEP_WORD_ADDR;

        return hal_gpio_send(iface, word_address, NULL, 0);
    }

    return ATCA_BAD_PARAM;
}

/** \brief send wake token
 * \param[in] iface  interface to logical device to wakeup
 * \return ATCA_WAKE_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_gpio_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_plib_gpio_api_t* plib;
    ATCA_STATUS status = ATCA_WAKE_FAILED;

    if (NULL == cfg)
    {
        return ATCA_BAD_PARAM;
    }

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    if (cfg->devtype == ATECC204A)
    {
        #ifdef ATCA_HAL_1WIRE
        status = check_wake_1wire(cfg);
        #endif
    }
    else
    {
        #ifdef ATCA_HAL_SWI
        (void)generate_wake_condition(cfg);
        status = check_wake_swi(cfg);
        #endif
    }

    return status;
}

/** \brief releases resource if no more communication
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_gpio_release(void *hal_data)
{
    return ATCA_SUCCESS;
}

/** \brief Function to send the specified number of bytes through GPIO bit banging
 * \param[in] cfg           Driver interface configurations
 * \param[in] txdata        pointer pointing bytes to send
 * \param[in] txlength      number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS gpio_send_bytes(ATCAIfaceCfg *cfg, uint8_t *txdata, uint16_t txlength)
{
    uint8_t bit_mask;
    uint8_t count;

    #ifdef ATCA_HAL_1WIRE
    bool bit_value;
    #endif
    protocol_type proto_type;
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (txdata == NULL)
    {
        return status;
    }

    proto_type = (cfg->devtype == ATECC204A) ? ATCA_PROTOCOL_1WIRE : ATCA_PROTOCOL_SWI;
    /* Disabling interrupts */
    __disable_irq();
    for (count = 0; count < txlength; count++)
    {
        bit_mask = (cfg->devtype == ATECC204A) ? ATCA_1WIRE_BIT_MASK : ATCA_SWI_BIT_MASK;
        while (bit_mask > 0)
        {
            /* if the next bit transmitted is a logic '1'	*/
            if (bit_mask & txdata[count])
            {
                status = send_logic_bit(cfg, ATCA_GPIO_LOGIC_BIT1);
            }
            /* if the next bit transmitted is a logic '0'	*/
            else
            {
                status = send_logic_bit(cfg, ATCA_GPIO_LOGIC_BIT0);
            }

            bit_mask = (proto_type == ATCA_PROTOCOL_1WIRE) ? bit_mask >> 1 : bit_mask << 1;
        }
        #ifdef ATCA_HAL_1WIRE
        if ((status == ATCA_SUCCESS) && (proto_type == ATCA_PROTOCOL_1WIRE))
        {
            (void)read_data_ACK_1wire(cfg, &bit_value);
            /* check for ACK/NACK */
            if (bit_value) /* if a NAK is detected */
            {
                /* device failed to send ACK */
                status = ATCA_COMM_FAIL;
                break;
            }
            tRCV0_DLY; /* slave recovery time delay (same for logic'0' and logic '1') */
        }
        #endif
    }
    /* Enabling interrupts */
    __enable_irq();

    return status;
}

/** \brief Function to send logic bit 1 or 0 over GPIO using 1WIRE
 * \param[in] cfg           Driver interface configurations
 * \param[in] bit_value     Logical bit value to be send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS send_logic_bit(ATCAIfaceCfg *cfg, bool bit_value)
{
    atca_plib_gpio_api_t* plib;
    const uint8_t* timings;
    const uint8_t logic1_swi_timings[] = { 4, 26 };
    const uint8_t logic1_1wire_timings[] = { 1, 41 };
    const uint8_t logic0_swi_timings[] = { 4, 34, 4, 4 };
    const uint8_t logic0_1wire_timings[] = { 11, 31 };

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    if (bit_value == ATCA_GPIO_LOGIC_BIT1)
    {
        timings = (cfg->devtype == ATECC204A) ? logic1_1wire_timings : logic1_swi_timings;
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_CLEAR);
        atca_delay_us(*timings++);
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
        atca_delay_us(*timings++);
    }
    else
    {
        timings = (cfg->devtype == ATECC204A) ? logic0_1wire_timings : logic0_swi_timings;
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_CLEAR);
        atca_delay_us(*timings++);
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
        atca_delay_us(*timings++);
        #ifdef ATCA_HAL_SWI
        if (cfg->devtype != ATECC204A)
        {
            plib->write(cfg->atcaswi.bus, ATCA_GPIO_CLEAR);
            atca_delay_us(*timings++);
            plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
            atca_delay_us(*timings++);
        }
        #endif
    }

    return ATCA_SUCCESS;
}

/** \brief Function to receive the response bytes through GPIO bit banging
 * \param[in] cfg          Driver interface configurations
 * \param[in] rxdata        pointer pointing where received bytes to be stored
 * \param[in] rxlength      number of bytes to receive
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS gpio_receive_bytes(ATCAIfaceCfg *cfg, uint8_t *rxdata, uint16_t rxlength)
{
    uint8_t bit_mask;
    uint8_t count;
    atca_plib_gpio_api_t* plib;
    protocol_type proto_type;
    bool bit_value = 1;

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if ((plib == NULL) || (rxdata == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    proto_type = (cfg->devtype == ATECC204A) ? ATCA_PROTOCOL_1WIRE : ATCA_PROTOCOL_SWI;

    __disable_irq();
    for (count = 0; count < rxlength; count++)
    {
        bit_mask = (cfg->devtype == ATECC204A) ? ATCA_1WIRE_BIT_MASK : ATCA_SWI_BIT_MASK;
        while (bit_mask >= 1)
        {
            #ifdef ATCA_HAL_1WIRE
            if (proto_type == ATCA_PROTOCOL_1WIRE)
            {
                plib->write(cfg->atcaswi.bus, ATCA_GPIO_CLEAR);
                /* device is set for high-speed communication */
                tHIGH_SPEED_DLY;
                plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
                tSWIN_DLY; /* delay to put master read inside the master sampling window */
                (void)read_logic_bit_1wire(cfg, &bit_value);
            }
            #endif

            #ifdef ATCA_HAL_SWI
            if (proto_type == ATCA_PROTOCOL_SWI)
            {
                (void)read_logic_bit_swi(cfg, &bit_value);
            }
            #endif
            if (bit_value) /* if a logic '1' is detected; received "one" bit */
            {
                rxdata[count] |= bit_mask;
            }
            if (proto_type == ATCA_PROTOCOL_1WIRE)
            {
                tBIT_DLY;//bit frame duration (tBIT) before reading the next bit
                bit_mask >>= 1;
            }
            else
            {
                bit_mask <<= 1;
            }
        }
        #ifdef ATCA_HAL_1WIRE
        /* send ACK except for last byte of read --> GO TO send_ACK() */
        if ((count < (rxlength - 1)) && (proto_type == ATCA_PROTOCOL_1WIRE))
        {
            (void)send_ACK_1wire(cfg);
        }
        #endif
    }
    __enable_irq();

    return ATCA_SUCCESS;
}

#ifdef ATCA_HAL_1WIRE
/** \brief Function to generate start or stop condition
 * \param[in] cfg          Driver interface configurations
 * \return  ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS start_stop_cond_1wire(ATCAIfaceCfg *cfg)
{
    atca_plib_gpio_api_t* plib;

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if (plib)
    {
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
        tHTSS_DLY;
        return ATCA_SUCCESS;
    }

    return ATCA_BAD_PARAM;
}

/** \brief Function to read the data ACK for the transmitted byte
 * \param[in] cfg          Driver interface configurations
 * \param[in] bit_value     Contains logical bit value(ACK or NACK) to be received
 * \return  ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS read_data_ACK_1wire(ATCAIfaceCfg *cfg, bool *bit_value)
{
    atca_plib_gpio_api_t* plib;
    ATCA_STATUS status = ATCA_BAD_PARAM;

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;

    if (plib)
    {
        /* this checks for an ACK or NACK from device */
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_CLEAR);
        tRD_DLY;
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
        tLOW1_DLY;

        status = read_logic_bit_1wire(cfg, bit_value);
    }

    return status;
}

/** \brief Function to discover the available devices in the bus
 * \param[in] cfg          Driver interface configurations
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS device_discovery_1wire(ATCAIfaceCfg *cfg)
{
    atca_plib_gpio_api_t* plib;
    bool bit_value;
    ATCA_STATUS status = ATCA_BAD_PARAM;

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if (plib)
    {
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
        tRRT_DLY;
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_CLEAR);
        tDRR_DLY;
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
        tDACK_DLY;

        (void)read_logic_bit_1wire(cfg, &bit_value);
        status = (bit_value == ((bool)ATCA_GPIO_ACK)) ? ATCA_SUCCESS : ATCA_NO_DEVICES;
    }

    return status;
}

/** \brief Function to generate wake condition
 * \param[in] cfg          Driver interface configurations
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS generate_wake_condition(ATCAIfaceCfg *cfg)
{
    atca_plib_gpio_api_t* plib;

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if (plib)
    {
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_CLEAR);
        atca_delay_us(60);
        plib->write(cfg->atcaswi.bus, ATCA_GPIO_SET);
        atca_delay_us(cfg->wake_delay);
        return ATCA_SUCCESS;
    }
    return ATCA_BAD_PARAM;
}

/** \brief Function to check wake condition for 1WIRE
 * \param[in] cfg          Driver interface configurations
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS check_wake_1wire(ATCAIfaceCfg *cfg)
{
    ATCA_STATUS status;
    uint8_t dev_addr;

    status = device_discovery_1wire(cfg);
    if (status == ATCA_NO_DEVICES)
    {
        (void)generate_wake_condition(cfg);
        status = start_stop_cond_1wire(cfg);
        dev_addr = get_slave_addr_1wire(cfg->atcaswi.slave_address, ATCA_GPIO_WRITE);
        status = gpio_send_bytes(cfg, &dev_addr, sizeof(dev_addr));
        (void)start_stop_cond_1wire(cfg);
    }

    return status;
}

/** \brief Function to read the data ACK for the transmitted byte
 * \param[in] dev_addr      7 bit device address
 * \param[in] oper          indicates read or write operation
 * \return  8 bit device address for write or read operation
 */
static uint8_t get_slave_addr_1wire(uint8_t dev_addr, uint8_t oper)
{
    return (dev_addr << 1) | oper;
}

/** \brief Function to read the logic bit 1 or 0
 * \param[in] cfg          Driver interface configurations
 * \param[in] bit_value    Contains hogical bit value to be received
 * \return  ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS read_logic_bit_1wire(ATCAIfaceCfg *cfg, bool *bit_value)
{
    atca_plib_gpio_api_t* plib;

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if (plib)
    {
        /* Changing the PIN direction as Input */
        plib->pin_setup(cfg->atcaswi.bus, ATCA_GPIO_INPUT_DIR);
        *bit_value = plib->read(cfg->atcaswi.bus);
        /* Changing the PIN direction as Output */
        plib->pin_setup(cfg->atcaswi.bus, ATCA_GPIO_OUTPUT_DIR);
        return ATCA_SUCCESS;
    }

    return ATCA_BAD_PARAM;
}
#endif /* End of ATCA_HAL_1WIRE */

#ifdef ATCA_HAL_SWI
/** \brief Function to read the logic bit 1 or 0
 * \param[in] cfg          Driver interface configurations
 * \param[in] bit_value     Contains logical bit value to be received
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS read_logic_bit_swi(ATCAIfaceCfg *cfg, bool *bit_value)
{
    atca_plib_gpio_api_t* plib;
    int8_t timeout_count;
    bool current_state = true, prev_state = true;
    uint8_t bit_count = 0;
    ATCA_STATUS status = ATCA_BAD_PARAM;

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return status;
    }

    do
    {
        //wait for start bit
        timeout_count = 90 / 2;
        while (timeout_count-- >= 0)
        {
            current_state = plib->read(cfg->atcaswi.bus);
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
            current_state = plib->read(cfg->atcaswi.bus);
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
            *bit_value = 0;
        }
        else if (bit_count == 1)
        {
            *bit_value = 1;
        }
        else
        {
            status = ATCA_TRACE(ATCA_RX_FAIL, "Rx Receive - failed");
        }
    }
    while (0);

    return status;
}

/** \brief Function to check wake condition for SWI
 * \param[in] cfg          Driver interface configurations
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS check_wake_swi(ATCAIfaceCfg *cfg)
{
    atca_plib_gpio_api_t* plib;
    uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };
    uint16_t rxlength  = sizeof(data);
    const uint8_t expected_response[4] = { 0x04, 0x11, 0x33, 0x43 };
    const uint8_t selftest_fail_resp[4] = { 0x04, 0x07, 0xC4, 0x40 };

    plib = (atca_plib_gpio_api_t*)cfg->cfg_data;
    if (plib == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    if (gpio_receive_bytes(cfg, data, rxlength) == ATCA_SUCCESS)
    {
        if (memcmp(data, expected_response, 4) == 0)
        {
            return ATCA_SUCCESS;
        }
        if (memcmp(data, selftest_fail_resp, 4) == 0)
        {
            return ATCA_STATUS_SELFTEST_ERROR;
        }
    }
    return ATCA_WAKE_FAILED;
}
#endif /* ATCA_HAL_SWI */
