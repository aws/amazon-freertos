/**
 * \file
 * \brief low-level HAL - methods used to setup indirection to physical layer interface
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


#ifndef ATCA_HAL_H_
#define ATCA_HAL_H_

#include <stdlib.h>

#include "atca_config.h"

#include "atca_status.h"
#include "atca_iface.h"
#include "atca_start_config.h"
#include "atca_start_iface.h"


/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

typedef struct
{
    ATCA_STATUS (*send)(void* ctx, uint8_t* txdata, uint16_t txlen);        /**< Must be a blocking send */
    ATCA_STATUS (*recv)(void* ctx, uint8_t* rxdata, uint16_t* rxlen);       /**< Must be a blocking receive */
    void* (*packet_alloc)(size_t bytes);                                    /**< Allocate a phy packet */
    void (*packet_free)(void* packet);                                      /**< Free a phy packet */
    void* hal_data;                                                         /**< Physical layer context */
} atca_hal_kit_phy_t;

#ifdef __cplusplus
extern "C" {
#endif

ATCA_STATUS hal_iface_init(ATCAIfaceCfg *, ATCAHAL_t** hal, ATCAHAL_t** phy);
ATCA_STATUS hal_iface_release(ATCAIfaceType, void* hal_data);

ATCA_STATUS hal_check_wake(const uint8_t* response, int response_size);

#ifdef ATCA_HAL_I2C
ATCA_STATUS hal_i2c_init(ATCAIface iface, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_i2c_post_init(ATCAIface iface);
ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength);
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_i2c_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
#ifdef ATCA_LEGACY_HAL
ATCA_STATUS hal_i2c_wake(ATCAIface iface);
ATCA_STATUS hal_i2c_idle(ATCAIface iface);
ATCA_STATUS hal_i2c_sleep(ATCAIface iface);
#endif
ATCA_STATUS hal_i2c_release(void *hal_data);
#endif

#ifdef ATCA_HAL_SWI
ATCA_STATUS hal_swi_init(ATCAIface iface, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_swi_post_init(ATCAIface iface);
ATCA_STATUS hal_swi_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength);
ATCA_STATUS hal_swi_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_swi_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
#ifdef ATCA_LEGACY_HAL
ATCA_STATUS hal_swi_wake(ATCAIface iface);
ATCA_STATUS hal_swi_idle(ATCAIface iface);
ATCA_STATUS hal_swi_sleep(ATCAIface iface);
#endif
ATCA_STATUS hal_swi_release(void *hal_data);
#endif


#if defined(ATCA_HAL_1WIRE) || defined(ATCA_HAL_SWI)
ATCA_STATUS hal_gpio_init(void *hal, ATCAIfaceCfg* cfg);
ATCA_STATUS hal_gpio_post_init(ATCAIface iface);
ATCA_STATUS hal_gpio_send(ATCAIface iface, uint8_t word_address, uint8_t* txdata, int txlength);
ATCA_STATUS hal_gpio_receive(ATCAIface iface, uint8_t word_address, uint8_t* rxdata, uint16_t* rxlength);
ATCA_STATUS hal_gpio_idle(ATCAIface iface);
ATCA_STATUS hal_gpio_sleep(ATCAIface iface);
ATCA_STATUS hal_gpio_wake(ATCAIface iface);
ATCA_STATUS hal_gpio_release(void *hal_data);
ATCA_STATUS hal_gpio_device_discovery(ATCAIface iface);
#endif

#ifdef ATCA_HAL_UART
ATCA_STATUS hal_uart_init(ATCAIface iface, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_uart_post_init(ATCAIface iface);
ATCA_STATUS hal_uart_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength);
ATCA_STATUS hal_uart_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_uart_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
#ifdef ATCA_LEGACY_HAL
ATCA_STATUS hal_uart_wake(ATCAIface iface);
ATCA_STATUS hal_uart_idle(ATCAIface iface);
ATCA_STATUS hal_uart_sleep(ATCAIface iface);
#endif
ATCA_STATUS hal_uart_release(ATCAIface iface);
#endif

#ifdef ATCA_HAL_SPI
ATCA_STATUS hal_spi_init(ATCAIface iface, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_spi_post_init(ATCAIface iface);
ATCA_STATUS hal_spi_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength);
ATCA_STATUS hal_spi_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_spi_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
#ifdef ATCA_LEGACY_HAL
ATCA_STATUS hal_spi_wake(ATCAIface iface);
ATCA_STATUS hal_spi_idle(ATCAIface iface);
ATCA_STATUS hal_spi_sleep(ATCAIface iface);
#endif
ATCA_STATUS hal_spi_release(void *hal_data);
#endif

#ifdef ATCA_HAL_KIT_HID
ATCA_STATUS hal_kit_hid_init(ATCAIface iface, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_kit_hid_post_init(ATCAIface iface);
ATCA_STATUS hal_kit_hid_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength);
ATCA_STATUS hal_kit_hid_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_kit_hid_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
ATCA_STATUS hal_kit_hid_release(void *hal_data);
#endif

#ifdef ATCA_HAL_KIT_BRIDGE
ATCA_STATUS hal_kit_init(ATCAIface iface, ATCAIfaceCfg* cfg);
ATCA_STATUS hal_kit_post_init(ATCAIface iface);
ATCA_STATUS hal_kit_send(ATCAIface iface, uint8_t word_address, uint8_t* txdata, int txlength);
ATCA_STATUS hal_kit_receive(ATCAIface iface, uint8_t word_address, uint8_t* rxdata, uint16_t* rxlength);
ATCA_STATUS hal_kit_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
ATCA_STATUS hal_kit_release(void* hal_data);
#endif

#ifdef ATCA_HAL_CUSTOM
ATCA_STATUS hal_custom_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
#endif

/* Polling defaults if not overwritten by the configuration */
#ifndef ATCA_POLLING_INIT_TIME_MSEC
#define ATCA_POLLING_INIT_TIME_MSEC       1
#endif

#ifndef ATCA_POLLING_FREQUENCY_TIME_MSEC
#define ATCA_POLLING_FREQUENCY_TIME_MSEC  2
#endif

#ifndef ATCA_POLLING_MAX_TIME_MSEC
#define ATCA_POLLING_MAX_TIME_MSEC        2500
#endif

/*  */
typedef enum
{
    ATCA_HAL_CONTROL_WAKE = 0,
    ATCA_HAL_CONTROL_IDLE = 1,
    ATCA_HAL_CONTROL_SLEEP = 2,
    ATCA_HAL_CONTROL_RESET = 3,
    ATCA_HAL_CONTROL_SELECT = 4,
    ATCA_HAL_CONTROL_DESELECT = 5,
    ATCA_HAL_CHANGE_BAUD = 6
} ATCA_HAL_CONTROL;

/** \brief Timer API for legacy implementations */
#ifndef atca_delay_ms
void atca_delay_ms(uint32_t ms);
#endif

#ifndef atca_delay_us
void atca_delay_us(uint32_t us);
#endif

/** \brief Timer API implemented at the HAL level */
void hal_rtos_delay_ms(uint32_t ms);
void hal_delay_ms(uint32_t ms);
void hal_delay_us(uint32_t us);

/** \brief Optional hal interfaces */
ATCA_STATUS hal_create_mutex(void ** ppMutex, char* pName);
ATCA_STATUS hal_destroy_mutex(void * pMutex);
ATCA_STATUS hal_lock_mutex(void * pMutex);
ATCA_STATUS hal_unlock_mutex(void * pMutex);

#ifndef ATCA_NO_HEAP
#ifdef ATCA_TESTS_ENABLED
void hal_test_set_memory_f(void* (*malloc_func)(size_t), void (*free_func)(void*));
#endif

#if defined(ATCA_TESTS_ENABLED) || !defined(ATCA_PLATFORM_MALLOC)
void*   hal_malloc(size_t size);
void    hal_free(void* ptr);
#else
#define hal_malloc      ATCA_PLATFORM_MALLOC
#define hal_free        ATCA_PLATFORM_FREE
#endif
#endif

#ifdef memset_s
#define hal_memset_s    memset_s
#else
#define hal_memset_s    atcab_memset_s
#endif


ATCA_STATUS hal_iface_register_hal(ATCAIfaceType iface_type, ATCAHAL_t *hal, ATCAHAL_t **old_hal, ATCAHAL_t* phy, ATCAHAL_t** old_phy);
uint8_t hal_is_command_word(uint8_t word_address);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ATCA_HAL_H_ */
