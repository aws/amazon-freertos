/**
 * \file
 * \brief low-level HAL - methods used to setup indirection to physical layer interface.
 * this level does the dirty work of abstracting the higher level ATCAIFace methods from the
 * low-level physical interfaces.  Its main goal is to keep low-level details from bleeding into
 * the logical interface implemetation.
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


/* when incorporating ATCA HAL into your application, you need to adjust the #defines in atca_hal.h to include
 * and exclude appropriate interfaces - this optimizes memory use when not using a specific iface implementation in your application */

#include "cryptoauthlib.h"
#include "atca_hal.h"

#ifndef ATCA_MAX_HAL_CACHE
#define ATCA_MAX_HAL_CACHE
#endif

#ifdef ATCA_HAL_I2C
static ATCAHAL_t hal_i2c = {
    hal_i2c_init,
    hal_i2c_post_init,
    hal_i2c_send,
    hal_i2c_receive,
    hal_i2c_control,
    hal_i2c_release
};
#endif

#ifdef ATCA_HAL_SWI_UART
static ATCAHAL_t hal_swi_uart = {
    hal_swi_init,
    hal_swi_post_init,
    hal_swi_send,
    hal_swi_receive,
    hal_swi_control,
    hal_swi_release
};
#endif

#ifdef ATCA_HAL_UART
static ATCAHAL_t hal_uart = {
    hal_uart_init,
    hal_uart_post_init,
    hal_uart_send,
    hal_uart_receive,
    hal_uart_control,
    hal_uart_release
};
#endif

#ifdef ATCA_HAL_SPI
static ATCAHAL_t hal_spi = {
    hal_spi_init,
    hal_spi_post_init,
    hal_spi_send,
    hal_spi_receive,
    hal_spi_control,
    hal_spi_release
};
#endif

#ifdef ATCA_HAL_GPIO
static ATCAHAL_t hal_gpio = {
    hal_gpio_init,
    hal_gpio_post_init,
    hal_gpio_send,
    hal_gpio_receive,
    hal_gpio_wake,
    hal_gpio_idle,
    hal_gpio_sleep,
    hal_gpio_release
};
#endif

#ifdef ATCA_HAL_KIT_HID
static ATCAHAL_t hal_hid = {
    hal_kit_hid_init,
    hal_kit_hid_post_init,
    hal_kit_hid_send,
    hal_kit_hid_receive,
    hal_kit_hid_control,
    hal_kit_hid_release
};
#endif

#if defined(ATCA_HAL_KIT_HID) || defined(ATCA_HAL_KIT_UART)
#include "kit_protocol.h"
static ATCAHAL_t hal_kit_v1 = {
    kit_init,
    kit_post_init,
    kit_send,
    kit_receive,
    kit_control,
    kit_release
};
#endif

#ifdef ATCA_HAL_KIT_BRIDGE
static ATCAHAL_t hal_kit_bridge = {
    hal_kit_init,
    hal_kit_post_init,
    hal_kit_send,
    hal_kit_receive,
    hal_kit_control,
    hal_kit_release
};
#endif

#ifdef ATCA_HAL_CUSTOM
static ATCAHAL_t hal_custom;
#endif

/** \brief Structure that holds the hal/phy maping for different interface types
 */
typedef struct
{
    uint8_t    iface_type;          /**<  */
    ATCAHAL_t* hal;                 /**<  */
    ATCAHAL_t* phy;                 /**< Physical interface for the specific HAL*/
} atca_hal_list_entry_t;


static atca_hal_list_entry_t atca_registered_hal_list[ATCA_MAX_HAL_CACHE] = {
#ifdef ATCA_HAL_I2C
    { ATCA_I2C_IFACE,      &hal_i2c,        NULL      },
#endif
#ifdef ATCA_HAL_SWI
    { ATCA_SWI_IFACE,      &hal_swi_uart,   &hal_uart },
#endif
#ifdef ATCA_HAL_KIT_UART
    { ATCA_UART_IFACE,     &hal_kit_v1,     &hal_uart },
#endif
#ifdef ATCA_HAL_SPI
    { ATCA_SPI_IFACE,      &hal_spi,        NULL      },
#endif
#ifdef ATCA_HAL_KIT_HID
    { ATCA_HID_IFACE,      &hal_kit_v1,     &hal_hid  },
#endif
#ifdef ATCA_HAL_KIT_BRIDGE
    { ATCA_KIT_IFACE,      &hal_kit_bridge, NULL      },
#endif
#if ATCA_HAL_SWI_GPIO
    { ATCA_SWI_GPIO_IFACE, &hal_gpio,       NULL      },
#endif
};

static const size_t atca_registered_hal_list_size = sizeof(atca_registered_hal_list) / sizeof(atca_hal_list_entry_t);


/** \brief Internal function to get a value from the hal cache
 * \param[in] iface_type - the type of physical interface to register
 * \param[out] hal pointer to the existing ATCAHAL_t structure
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS hal_iface_get_registered(ATCAIfaceType iface_type, ATCAHAL_t** hal, ATCAHAL_t **phy)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (hal && phy)
    {
        int i;
        for (i = 0; i < atca_registered_hal_list_size; i++)
        {
            if (iface_type == atca_registered_hal_list[i].iface_type)
            {
                break;
            }
        }

        if (i < atca_registered_hal_list_size)
        {
            *hal = atca_registered_hal_list[i].hal;
            *phy = atca_registered_hal_list[i].phy;
            status = ATCA_SUCCESS;
        }
        else
        {
            status = ATCA_GEN_FAIL;
        }
    }

    return status;
}

/** \brief Internal function to set a value in the hal cache
 * \param[in] iface_type - the type of physical interface to register
 * \param[in] hal pointer to the existing ATCAHAL_t structure
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS hal_iface_set_registered(ATCAIfaceType iface_type, ATCAHAL_t* hal, ATCAHAL_t* phy)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (hal)
    {
        int i;
        int empty = atca_registered_hal_list_size;
        for (i = 0; i < atca_registered_hal_list_size; i++)
        {
            if (iface_type == atca_registered_hal_list[i].iface_type)
            {
                break;
            }
            else if (empty == atca_registered_hal_list_size)
            {
                if (!atca_registered_hal_list[i].hal && !atca_registered_hal_list[i].phy)
                {
                    empty = i;
                }
            }
        }

        if (i < atca_registered_hal_list_size)
        {
            atca_registered_hal_list[i].hal = hal;
            atca_registered_hal_list[i].phy = phy;
            status = ATCA_SUCCESS;
        }
        else if (empty < atca_registered_hal_list_size)
        {
            atca_registered_hal_list[empty].hal = hal;
            atca_registered_hal_list[empty].hal = phy;
            status = ATCA_SUCCESS;
        }
        else
        {
            status = ATCA_ALLOC_FAILURE;
        }

    }

    return status;
}

/** \brief Register/Replace a HAL with a
 * \param[in] iface_type - the type of physical interface to register
 * \param[in] hal pointer to the new ATCAHAL_t structure to register
 * \param[out] old pointer to the existing ATCAHAL_t structure
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_iface_register_hal(ATCAIfaceType iface_type, ATCAHAL_t *hal, ATCAHAL_t **old_hal, ATCAHAL_t* phy, ATCAHAL_t** old_phy)
{
    ATCA_STATUS status;

    status = (old_hal && old_phy) ? hal_iface_get_registered(iface_type, old_hal, old_phy) : ATCA_SUCCESS;

    if (ATCA_SUCCESS == status)
    {
        status = hal_iface_set_registered(iface_type, hal, phy);
    }

    return ATCA_SUCCESS;
}

/** \brief Standard HAL API for ATCA to initialize a physical interface
 * \param[in] cfg pointer to ATCAIfaceCfg object
 * \param[in] hal pointer to ATCAHAL_t intermediate data structure
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_iface_init(ATCAIfaceCfg *cfg, ATCAHAL_t **hal, ATCAHAL_t **phy)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (cfg && hal)
    {
        status = hal_iface_get_registered(cfg->iface_type, hal, phy);

#ifdef ATCA_HAL_CUSTOM
        if (ATCA_CUSTOM_IFACE == cfg->iface_type)
        {
            *hal = hal_malloc(sizeof(ATCAHAL_t));
            if (*hal)
            {
                (*hal)->halinit = cfg->atcacustom.halinit;
                (*hal)->halpostinit = cfg->atcacustom.halpostinit;
                (*hal)->halreceive = cfg->atcacustom.halreceive;
                (*hal)->halsend = cfg->atcacustom.halsend;
                (*hal)->halcontrol = hal_custom_control;
                (*hal)->halrelease = cfg->atcacustom.halrelease;
                status = ATCA_SUCCESS;
            }
            else
            {
                status = ATCA_ALLOC_FAILURE;
            }
        }
#endif
    }

    return status;
}

/** \brief releases a physical interface, HAL knows how to interpret hal_data
 * \param[in] iface_type - the type of physical interface to release
 * \param[in] hal_data - pointer to opaque hal data maintained by HAL implementation for this interface type
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_iface_release(ATCAIfaceType iface_type, void *hal_data)
{
    ATCA_STATUS status;
    ATCAHAL_t * hal;
    ATCAHAL_t* phy;

    status = hal_iface_get_registered(iface_type, &hal, &phy);

    if (ATCA_SUCCESS == status)
    {
        status = hal->halrelease ? hal->halrelease(hal_data) : ATCA_BAD_PARAM;
    }

    return status;
}

/** \brief Utility function for hal_wake to check the reply.
 * \param[in] response       Wake response to be checked.
 * \param[in] response_size  Size of the response to check.
 * \return ATCA_SUCCESS for expected wake, ATCA_STATUS_SELFTEST_ERROR if the
 *         power on self test failed, ATCA_WAKE_FAILED for other failures.
 */
ATCA_STATUS hal_check_wake(const uint8_t* response, int response_size)
{
    const uint8_t expected_response[4] = { 0x04, 0x11, 0x33, 0x43 };
    const uint8_t selftest_fail_resp[4] = { 0x04, 0x07, 0xC4, 0x40 };

    if (response_size != 4)
    {
        return ATCA_WAKE_FAILED;
    }
    if (memcmp(response, expected_response, 4) == 0)
    {
        return ATCA_SUCCESS;
    }
    if (memcmp(response, selftest_fail_resp, 4) == 0)
    {
        return ATCA_STATUS_SELFTEST_ERROR;
    }
    return ATCA_WAKE_FAILED;
}

/** \brief Utility function for hal_wake to check the reply.
 * \param[in] word_address      Command to check
 * \return true if the word_address is considered a command
 */
uint8_t hal_is_command_word(uint8_t word_address)
{
    return 0xFF == word_address || 0x03 == word_address || 0x10 == word_address;
}


#if !defined(ATCA_NO_HEAP) && defined(ATCA_TESTS_ENABLED) && defined(ATCA_PLATFORM_MALLOC)

void* (*g_hal_malloc_f)(size_t) = ATCA_PLATFORM_MALLOC;
void (*g_hal_free_f)(void*) = ATCA_PLATFORM_FREE;

void* hal_malloc(size_t size)
{
    return g_hal_malloc_f(size);
}

void hal_free(void* ptr)
{
    g_hal_free_f(ptr);
}

void hal_test_set_memory_f(void* (*malloc_func)(size_t), void (*free_func)(void*))
{
    g_hal_malloc_f = malloc_func;
    g_hal_free_f = free_func;
}

#endif

#if defined(ATCA_HAL_LEGACY_API) && defined(ATCA_HAL_I2C)
ATCA_STATUS hal_i2c_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    switch (option)
    {
    case ATCA_HAL_CONTROL_WAKE:
        return hal_i2c_wake(iface);
    case ATCA_HAL_CONTROL_IDLE:
        return hal_i2c_idle(iface);
    case ATCA_HAL_CONTROL_SLEEP:
        return hal_i2c_sleep(iface);
    case ATCA_HAL_CONTROL_SELECT:
    /* fallthrough */
    case ATCA_HAL_CONTROL_DESELECT:
        return ATCA_SUCCESS;
    default:
        return ATCA_BAD_PARAM;
    }
}
#endif

#if defined(ATCA_HAL_LEGACY_API) && defined(ATCA_HAL_SWI)
ATCA_STATUS hal_swi_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    switch (option)
    {
    case ATCA_HAL_CONTROL_WAKE:
        return hal_swi_wake(iface);
    case ATCA_HAL_CONTROL_IDLE:
        return hal_swi_idle(iface);
    case ATCA_HAL_CONTROL_SLEEP:
        return hal_swi_sleep(iface);
    case ATCA_HAL_CONTROL_SELECT:
    /* fallthrough */
    case ATCA_HAL_CONTROL_DESELECT:
        return ATCA_SUCCESS;
    default:
        return ATCA_BAD_PARAM;
    }
}
#endif

#if defined(ATCA_HAL_LEGACY_API) && defined(ATCA_HAL_UART)
ATCA_STATUS hal_uart_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    switch (option)
    {
    case ATCA_HAL_CONTROL_WAKE:
        return hal_uart_wake(iface);
    case ATCA_HAL_CONTROL_IDLE:
        return hal_uart_idle(iface);
    case ATCA_HAL_CONTROL_SLEEP:
        return hal_uart_sleep(iface);
    case ATCA_HAL_CONTROL_SELECT:
    /* fallthrough */
    case ATCA_HAL_CONTROL_DESELECT:
        return ATCA_SUCCESS;
    default:
        return ATCA_BAD_PARAM;
    }
}
#endif

#if defined(ATCA_HAL_LEGACY_API) && defined(ATCA_HAL_SPI)
ATCA_STATUS hal_spi_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    if (iface)
    {
        switch (option)
        {
        case ATCA_HAL_CONTROL_WAKE:
            return hal_spi_wake(iface);
        case ATCA_HAL_CONTROL_IDLE:
            return hal_spi_idle(iface);
        case ATCA_HAL_CONTROL_SLEEP:
            return hal_spi_sleep(iface);
        case ATCA_HAL_CONTROL_SELECT:
        /* fallthrough */
        case ATCA_HAL_CONTROL_DESELECT:
            return ATCA_SUCCESS;
        default:
            break;
        }
    }
    return ATCA_BAD_PARAM;
}
#endif

#if defined(ATCA_HAL_CUSTOM)
ATCA_STATUS hal_custom_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    if (iface && iface->mIfaceCFG)
    {
        switch (option)
        {
        case ATCA_HAL_CONTROL_WAKE:
            return iface->mIfaceCFG->atcacustom.halwake(iface);
        case ATCA_HAL_CONTROL_IDLE:
            return iface->mIfaceCFG->atcacustom.halidle(iface);
        case ATCA_HAL_CONTROL_SLEEP:
            return iface->mIfaceCFG->atcacustom.halsleep(iface);
        case ATCA_HAL_CONTROL_SELECT:
        /* fallthrough */
        case ATCA_HAL_CONTROL_DESELECT:
            return ATCA_SUCCESS;
        default:
            break;
        }
    }
    return ATCA_BAD_PARAM;
}
#endif
