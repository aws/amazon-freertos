/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include "hal_platform.h"

#ifdef HAL_UART_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup UART
 * @{
 * This section introduces the Universal Asynchronous Receiver/Transmitter(UART) APIs including terms and acronyms, supported features, software architecture, details on how to use this driver, UART function groups, enums, structures and functions.
 *
 * @section HAL_UART_Terms_Chapter Terms and acronyms
 *
 * |Terms                 |Details                                           |
 * |----------------------|--------------------------------------------------|
 * |\b CTS                | This UART pin is used to indicate if the device can transmit data. It is used only when the UART hardware flow control is enabled. |
 * |\b GPIO               | General Purpose Input Output is a generic pin on an integrated circuit defined as an input or output pin, and controlled by the user at runtime. |
 * |\b MCU                | Micro Controller Unit is a small computer on a single integrated circuit containing a processor core, memory and programmable input/output peripherals. |
 * |\b NVIC               | The Nested Vectored Interrupt Controller (NVIC) is the interrupt controller of ARM Cortex-M. |
 * |\b RTS                | This UART pin is used to indicate if the device can receive data. It is used only when the UART hardware flow control is enabled. |
 * |\b RXD                | This UART pin is used for receiving data. |
 * |\b TX/RX              | Abbreviation of Transmit/Receive. |
 * |\b TXD                | This UART pin is used for sending data. |
 * |\b UART               | Universal Asynchronous Receiver/Transmitter is usually an individual (or part of an) integrated circuit (IC) used for serial communications over a micro-controller unit(MCU) or peripheral device serial port. For an introduction to UART, please refer to <a href="https://en.wikipedia.org/wiki/Uart"> UART in Wikipedia </a> .|
 * |\b VFIFO \b DMA       | Virtual First In First Out Direct Memory Access is a special DMA used to transfer data between memory and UART without MCU's handle. |
 *
 * @section HAL_UART_Features_Chapter Supported features
 *
 * - \b Support \b poll \b and \b DMA \b modes. \n
 *   Based on user configuration, UART operates in different modes:
 *  - \b Poll \b mode: In this mode, whether the data has been received or sent is determined by polling the UART status register.
 *                    To receive data, it reads the data from the data register. To send the data, it writes to the data register.
 *                    When a task calls the UART driver API in the poll mode, it will block the other tasks from running, until the API in the poll mode returns.
 *  - \b DMA \b mode: In this mode, a separate VFIFO DMA hardware is enabled to transmit or receive the data between the VFIFO DMA buffer and the UART hardware.
 *                    The occupancy rate of the CPU in this mode is very low, as the CPU is not involved in the operation of transmitting or receiving data byte by byte.
 *   \n \n
 * - \b Support \b two \b types \b of \b flow \b control \b modes. \n
 *   UART flow control mechanism is used to synchronize the data transfer between a receiver and a transmitter to prevent data loss during the transfer. \n
 *  - \b Software \b flow \b control: Receive operation: UART automatically sends XOFF character to the peer if there is no space in the receiving FIFO.
 *                    And automatically sends XON character if the receiving FIFO becomes available.
 *                    Transmit operation: UART raises an interrupt and updates the status of the related register when receiving XOFF or XON character.
 *                    The values of XON and XOFF can be configured if needed.
 *  - \b Hardware \b flow \b control: The UART automatically activates the request to send(RTS) pin when the space in the receiving FIFO becomes available and automatically deactivates the RTS pin when the space in the receiving FIFO is unavailable.
 *                    Transmit operation: The UART automatically sends the data when it detects the clear to send(CTS) input pin is active.
 *                    In both receive or transmit operations, UART raises an interrupt if request to send(RTS) or clear to send(CTS) are active.
 *   \n \n
 * - \b Support \b custom \b configuration. \n
 *   In addition to the default settings of the UART driver APIs, the user has access to auxiliary UART APIs for UART driver configuration to further
 *   customize the settings such as baud rate, flow control, escape character and more.
 *
 * @section HAL_UART_Architecture_Chapter Software architecture of UART
 *  - \b Poll \b mode: \n
 *                    In this mode, a user directly calls hal_uart_get_char(), hal_uart_put_char(),
 *                    hal_uart_receive_polling() or hal_uart_send_polling() to transmit or receive data.
 *                    UART driver polls the status of UART transmit or receive operation in a loop.
 *                    Once it's ready, UART driver fetches the data by reading from the data register or
 *                    sends the data by writing to the data register.
 *                    The hal_uart_get_char(), hal_uart_put_char(), hal_uart_receive_polling() and hal_uart_send_polling() are blocking calls. The calling to these APIs is blocked until the data has been received or sent.
 *                    For architecture diagram in this mode, please refer to @ref HAL_Overview_3_Chapter.
 *  - \b DMA \b mode: \n
 *                    Unlike poll mode, VFIFO DMA hardware and transmit (TX) or receive (RX) buffers are required to operate in the DMA mode.
 *                    The VFIFO DMA hardware is a special DMA used with the UART data transmit and receive operations.
 *                    The VFIFO DMA hardware assists MCU to transmit or receive data
 *                    and prevents copying the data from buffer byte by byte only by MCU itself.
 *                    TX/RX buffers are used to cache the data during transmit and receive operations.
 *                    The UART hardware is not used when the user reads or writes to and from the buffer.
 *                    It is more useful for the receiving channel, as it's uncertain when the data is received
 *                    and the user has no option to continuously request UART hardware status. \n
 *                    In this mode, user should call hal_uart_set_dma() function to initialize the DMA hardware
 *                    before transmitting or receiving the data.
 *                    The TX/RX buffers are registered in DMA hardware with their corresponding addresses and sizes. \n
 *                    For transmit operation, call hal_uart_send_dma() function to transmit data.
 *                    UART driver will query free space in the TX buffer, if the free space is greater than or equal to the requested data size,
 *                    the UART driver will copy user data to the TX buffer and return with requested data length.
 *                    If the free space is smaller than the requested data size,
 *                    the UART driver will only copy the user data that fits into the TX buffer
 *                    and activate the VFIFO DMA TX threshold interrupt.
 *                    The UART driver will then return the data size that was actually transmitted.
 *                    The user should check the return value,
 *                    then decide whether to wait for sending out remaining data when the TX buffer becomes available again.
 *                    The UART driver triggers a VFIFO DMA interrupt
 *                    if the data is transmitted partially and the remaining data length is less than the threshold of the transmit buffer.
 *                    Then the VFIFO DMA interrupt handler is called.
 *                    The DMA interrupt handler will call user's callback to notify that
 *                    the transmit buffer is available to transmit data either using callback or user's task.
 *                    During this period, other tasks can also be performed. \n
 *                    For receive operation, hal_uart_receive_dma() function is used
 *                    and the UART driver provides the same functionality as during the transmit operation.
 *                    It also checks the receive buffer and fetches data from the receive buffer until it's empty.
 *                    If the data length fetched is less than requested length from the user,
 *                    the UART driver activates the VFIFO DMA RX threshold interrupt.
 *                    When there is enough data cached in the receive buffer, the VFIFO DMA RX threshold interrupt is triggered.
 *                    Then the VFIFO DMA RX handler calls the user's callback to notify that the receive buffer is available.
 *                    User can receive data from the receive buffer again by calling UART HAL receive interface.
 *                    During this period, other tasks can also be performed.
 *                    For architecture diagram in this mode, please refer to @ref HAL_Overview_3_Chapter.
 *
 * @section HAL_UART_Driver_Usage_Chapter How to use this driver
 *
 * - Configure GPIO pins used by the UART hardware. \n
 *
 *  - User should configure GPIO pins used by the UART. Those pins include TXD and RXD.
 *     If the hardware flow control is enabled, RTS and CTS are needed also.
 *     Concerning issues about how to configure UART pins, please refer to @ref GPIO of driver document.
 *  - sample code:
 *    @code
 *       void uart_pinmux_init(void)
 *       {
 *           //In this part, we use mt2523_hdk as an example. if user use other board, please query the related gpio port and pinmux function in hal_pinmux_define.h.
 *
 *           //configure RXD pin
 *           hal_gpio_init(HAL_GPIO_0);
 *           //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *           //function_index = HAL_GPIO_0_U2RXD, need not to configure pinmux if EPT tool is used.
 *           hal_pinmux_set_function(HAL_GPIO_0, function_index);
 *
 *           // configure TXD pin
 *           hal_gpio_init(HAL_GPIO_1);
 *           //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *           //function_index = HAL_GPIO_1_U2TXD, need not to configure pinmux if EPT tool is used.
 *           hal_pinmux_set_function(HAL_GPIO_1, function_index);
 *
 *           // configure RTS pin
 *           hal_gpio_init(HAL_GPIO_3);
 *           //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *           //function_index = HAL_GPIO_3_U0RTS, need not to configure pinmux if EPT tool is used.
 *           hal_pinmux_set_function(HAL_GPIO_3, function_index);
 *
 *           // configure CTS pin
 *           hal_gpio_init(HAL_GPIO_2);
 *           //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *           //function_index = HAL_GPIO_2_U0CTS, need not to configure pinmux if EPT tool is used.
 *           hal_pinmux_set_function(HAL_GPIO_2, function_index);
 *       }
 *    @endcode
 *
 * - Use UART with poll mode. \n
 *  - Step1: call hal_uart_init() to configure the UART in poll mode.
 *  - Step2: call hal_uart_get_char() to get a character from the UART.
 *  - Step3: call hal_uart_put_char() to put a character to the UART.
 *  - Step4: call hal_uart_receive_polling() to get data from the UART with a user requested data length.
 *  - Step5: call hal_uart_send_polling() to put data to the UART with a user requested data length.
 *  - Step6: call hal_uart_deinit() to de-initialize the UART port.
 *  - sample code:
 *    @code
 *       void uart_poll_application(void)
 *       {
 *           hal_uart_config_t uart_config;
 *           char data;
 *           char buffer[10];
 *           uart_config.baudrate = HAL_UART_BAUDRATE_921600;
 *           uart_config.parity = HAL_UART_PARITY_NONE;
 *           uart_config.stop_bit = HAL_UART_STOP_BIT_1;
 *           uart_config.word_length = HAL_UART_WORD_LENGTH_8;
 *
 *           hal_uart_init(HAL_UART_0, &uart_config);
 *           data = hal_uart_get_char(HAL_UART_0);
 *           hal_uart_put_char(HAL_UART_0, data);
 *           hal_uart_receive_polling(HAL_UART_0, buffer, 10);
 *           hal_uart_send_polling(HAL_UART_0, buffer, 10);
 *           hal_uart_deinit(HAL_UART_0);
 *       }
 *    @endcode
 * - Use UART with DMA mode. \n
 *  - Step1: call hal_uart_init() to configure the UART in DMA mode.
 *  - Step2: call hal_uart_set_dma() to configure the VFIFO DMA hardware. Note: the address should be non_cacheable, for more information, please refer to GDMA part. 
 *  - Step3: call hal_uart_register_callback() to register a callback function.
 *  - Step4: call hal_uart_receive_dma() to receive data from the UART.
 *  - Step5: call hal_uart_send_dma() to transmit data to the UART.
 *  - Step6: call hal_uart_deinit() to de-initialize the UART port.
 *  - sample code:
 *    @code
 *       static volatile uint32_t receive_notice = 0;
 *       static volatile uint32_t send_notice = 0;
 *       static char rx_vfifo_buffer[512] __attribute__ ((section(".noncached_zidata")));
 *       static char tx_vfifo_buffer[512] __attribute__ ((section(".noncached_zidata")));
 *
 *       static void uart_dma_application(void)
 *       {
 *          hal_uart_config_t uart_config;
 *          char buffer[64];
 *          char *pbuf;
 *          uint32_t left, snd_cnt, rcv_cnt;
 *          hal_uart_dma_config_t dma_config;
 *          char uart_prompt[] = "UART DMA mode begin\n";
 *
 *          uart_config.baudrate = HAL_UART_BAUDRATE_921600;
 *          uart_config.parity = HAL_UART_PARITY_NONE;
 *          uart_config.stop_bit = HAL_UART_STOP_BIT_1;
 *          uart_config.word_length = HAL_UART_WORD_LENGTH_8;
 *          hal_uart_init(HAL_UART_0, &uart_config);
 *
 *          dma_config.receive_vfifo_alert_size = 50;
 *          dma_config.receive_vfifo_buffer = rx_vfifo_buffer;
 *          dma_config.receive_vfifo_buffer_size = 512;
 *          dma_config.receive_vfifo_threshold_size = 128;
 *          dma_config.send_vfifo_buffer = tx_vfifo_buffer;
 *          dma_config.send_vfifo_buffer_size = 512;
 *          dma_config.send_vfifo_threshold_size = 51;
 *          hal_uart_set_dma(HAL_UART_0, &dma_config);
 *          hal_uart_register_callback(HAL_UART_0, user_uart_callback, NULL);
 *
 *          snd_cnt = hal_uart_send_dma(HAL_UART_0, uart_prompt, sizeof(uart_prompt));
 *
 *          left = 64;
 *          pbuf = buffer;
 *
 *          while(1){
 *              //Note: In this sample code, we just use while(1) to wait the interrupt,
 *              //but user can do anything before the interrupt is triggered.
 *
 *              rcv_cnt = hal_uart_receive_dma(HAL_UART_0, pbuf, left);
 *              left -= rcv_cnt;
 *              pbuf += rcv_cnt;
 *              if(left == 0)
 *                  break;
 *
 *              while(!receive_notice);
 *              receive_notice = 0;
 *          }
 *
 *          left = 64;
 *          pbuf = buffer;
 *          while(1){
 *              //Note: In this sample code, we just use while(1) to wait the interrupt,
 *              //but user can do anything before the interrupt is triggered.
 *
 *              snd_cnt = hal_uart_send_dma(HAL_UART_0, pbuf, left);
 *              left -= snd_cnt;
 *              pbuf += snd_cnt;
 *              if(left == 0)
 *                  break;
 *
 *              while(!send_notice);
 *              send_notice = 0;
 *          }
 *
 *          hal_uart_deinit(HAL_UART_0);
 *       }
 *    @endcode
 *    @code
 *       //Callback function. This function should be registered with #hal_uart_register_callback().
 *       static void user_uart_callback(hal_uart_callback_event_t status, void *user_data)
 *       {
 *          if(status == HAL_UART_EVENT_READY_TO_WRITE)
 *              send_notice = 1;
 *          else if(status == HAL_UART_EVENT_READY_TO_READ)
 *              receive_notice = 1;
 *       }
 *    @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_uart_enum Enum
  * @{
  */


/** @brief This enum defines baud rate of the UART frame */
typedef enum {
    HAL_UART_BAUDRATE_110 = 0,           /**< Defines UART baudrate as 110 bps */
    HAL_UART_BAUDRATE_300 = 1,           /**< Defines UART baudrate as 300 bps */
    HAL_UART_BAUDRATE_1200 = 2,          /**< Defines UART baudrate as 1200 bps */
    HAL_UART_BAUDRATE_2400 = 3,          /**< Defines UART baudrate as 2400 bps */
    HAL_UART_BAUDRATE_4800 = 4,          /**< Defines UART baudrate as 4800 bps */
    HAL_UART_BAUDRATE_9600 = 5,          /**< Defines UART baudrate as 9600 bps */
    HAL_UART_BAUDRATE_19200 = 6,         /**< Defines UART baudrate as 19200 bps */
    HAL_UART_BAUDRATE_38400 = 7,         /**< Defines UART baudrate as 38400 bps */
    HAL_UART_BAUDRATE_57600 = 8,         /**< Defines UART baudrate as 57600 bps */
    HAL_UART_BAUDRATE_115200 = 9,        /**< Defines UART baudrate as 115200 bps */
    HAL_UART_BAUDRATE_230400 = 10,       /**< Defines UART baudrate as 230400 bps */
    HAL_UART_BAUDRATE_460800 = 11,       /**< Defines UART baudrate as 460800 bps */
    HAL_UART_BAUDRATE_921600 = 12,       /**< Defines UART baudrate as 921600 bps */
#ifdef HAL_UART_FEATURE_5M_BAUDRATE
    HAL_UART_BAUDRATE_5000000 = 13,       /**< Defines UART baudrate as 5000000 bps */
#endif
    HAL_UART_BAUDRATE_MAX                /**< Defines maximum enum value of UART baudrate */
} hal_uart_baudrate_t;


/** @brief This enum defines word length of UART frame */
typedef enum {
    HAL_UART_WORD_LENGTH_5 = 0,           /**< Defines UART word length as 5 bit per frame */
    HAL_UART_WORD_LENGTH_6 = 1,           /**< Defines UART word length as 6 bit per frame */
    HAL_UART_WORD_LENGTH_7 = 2,           /**< Defines UART word length as 7 bit per frame */
    HAL_UART_WORD_LENGTH_8 = 3            /**< Defines UART word length as 8 bit per frame */
} hal_uart_word_length_t;


/** @brief This enum defines stop bit of UART frame */
typedef enum {
    HAL_UART_STOP_BIT_1 = 0,              /**< Defines UART stop bit as 1 bit per frame */
    HAL_UART_STOP_BIT_2 = 1,              /**< Defines UART stop bit as 2 bit per frame */
} hal_uart_stop_bit_t;


/** @brief This enum defines parity of UART frame */
typedef enum {
    HAL_UART_PARITY_NONE = 0,            /**< Defines UART parity as none */
    HAL_UART_PARITY_ODD = 1,             /**< Defines UART parity as odd */
    HAL_UART_PARITY_EVEN = 2             /**< Defines UART parity as even */
} hal_uart_parity_t;


/** @brief This enum defines return status of UART HAL public API. User should check return value after calling those APIs. */
typedef enum {
    HAL_UART_STATUS_ERROR_PARAMETER = -4,      /**< Indicates user's parameter is invalid */
    HAL_UART_STATUS_ERROR_BUSY = -3,           /**< Indicates UART port is currently used by other user */
    HAL_UART_STATUS_ERROR_UNINITIALIZED = -2,  /**< Indicates UART port has not been initialized */
    HAL_UART_STATUS_ERROR = -1,                /**< Indicates UART driver detected a common error */
    HAL_UART_STATUS_OK = 0                     /**< Indicates UART function executed successfully */
} hal_uart_status_t;


/** @brief This enum defines UART event when an interrupt occurs */
typedef enum {
    HAL_UART_EVENT_TRANSACTION_ERROR = -1,          /**< Indicates if there is a transaction error when receiving data */
    HAL_UART_EVENT_READY_TO_READ = 1,               /**< Indicates if there is enough data available in the RX buffer for the user to read from it */
    HAL_UART_EVENT_READY_TO_WRITE = 2               /**< Indicates if there is enough free space available in the TX buffer for the user to write into it */
} hal_uart_callback_event_t;


/**
  * @}
  */


/** @defgroup hal_uart_struct Struct
  * @{
  */


/** @brief This struct defines UART configure parameters */
typedef struct {
    hal_uart_baudrate_t baudrate;              /**< This field represents the baudrate of UART frame */
    hal_uart_word_length_t word_length;        /**< This field represents the word length of UART frame */
    hal_uart_stop_bit_t stop_bit;              /**< This field represents the stop bit of UART frame */
    hal_uart_parity_t parity;                  /**< This field represents the parity of UART frame */
} hal_uart_config_t;


/** @brief This struct defines config parameters and TX/RX buffers for the VFIFO DMA associated with a specific UART channel */
typedef struct {
    uint8_t *send_vfifo_buffer;                /**< This field represents the transmitting VFIFO buffer allocated by user. It will only be used by the UART driver for sending data, and must be non-cacheable and align to 4bytes */
    uint32_t send_vfifo_buffer_size;           /**< This field represents the size of the transmitting VFIFO buffer */
    uint32_t send_vfifo_threshold_size;        /**< This field represents the threshold of the transmitting VFIFO buffer. VFIFO DMA will trigger an interrupt when the available bytes in VFIFO buffer are lower than the threshold */
    uint8_t *receive_vfifo_buffer;             /**< This field represents the receiving VFIFO buffer allocated by the user. It will only be used by the UART driver for receiving data, and must be non-cacheable and align to 4bytes */
    uint32_t receive_vfifo_buffer_size;        /**< This field represents size of the receiving VFIFO buffer */
    uint32_t receive_vfifo_threshold_size;     /**< This field represents the threshold of the receiving VFIFO buffer. VFIFO DMA will trigger receive interrupt when avail bytes in VFIFO buffer is larger than threshold */
    uint32_t receive_vfifo_alert_size;         /**< This field represents the threshold size of free space left in the VFIFO buffer that actives the UART's flow control system */
} hal_uart_dma_config_t;


/**
  * @}
  */


/** @defgroup hal_uart_typedef Typedef
  * @{
  */


/** @brief This typedef defines user's callback function prototype.
 *             This callback function will be called in UART interrupt handler when UART interrupt is raised.
 *             User should call hal_uart_register_callback() to register callbacks to UART driver explicitly.
 *             Note, that the callback function is not appropriate for time consuming operations. \n
 *             parameter "event" : for more information, please refer to description of #hal_uart_callback_event_t.
 *             parameter "user_data" : a user defined data used in the callback function.
 */
typedef void (*hal_uart_callback_t)(hal_uart_callback_event_t event, void *user_data);


/**
  * @}
  */


/**
 * @brief     This function initializes the UART hardware with basic functionality.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] uart_config   specifies configure parameters for UART hardware initialization.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_BUSY if the UART hardware has been initialized before. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 */
hal_uart_status_t hal_uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config);


/**
 * @brief     This function deinitializes the UART hardware to its default status.
 * @param[in] uart_port   initializes the specified UART port number.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if UART has not been initialized. \n
 * @note
 *            user must call this function when they don't use this UART port.
 */
hal_uart_status_t hal_uart_deinit(hal_uart_port_t uart_port);


/**
 * @brief     This function places one character at a time to the UART port in a poll mode.
 *               In this function, driver first poll the status of UART transmit in a loop until it's ready.
 *               Then driver places the character in the UART transmit register and returns from this function.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] byte   specifies the data to send.
 */
void hal_uart_put_char(hal_uart_port_t uart_port, char byte);


/**
 * @brief     This function gets one character from UART port in a poll mode.
 *               In this function, driver first polls the status of a UART receive operation in a loop until it's ready.
 *               Then driver get the character from UART receive register and return it.
 * @param[in] uart_port   initializes the specified UART port number.
 * @return
 *                 the characters received from the UART port.
 */
char hal_uart_get_char(hal_uart_port_t uart_port);



/**
 * @brief     This function sends user data byte by byte in a poll mode.
 *               In this function, driver first poll status of UART transmit in a loop until it's ready.
 *               Then driver put the character into UART transmit register.
 *               Driver continues this operation until all user data is sent out,
 *               then it returns the transmitted bytes.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] data   specifies pointer to the user's data buffer.
 * @param[in] size   specifies size of the user's data buffer.
 * @return
 *                 bytes of transmitted data.
 */
uint32_t hal_uart_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size);


/**
 * @brief     This function sends user data in a VFIFO DMA mode.
 *               In this function, driver first polls the free space in the VFIFO TX buffer.
 *               Then driver puts user data into the VFIFO TX buffer, that fits into the free space.
 *               If driver can't put all user data into VFIFO TX buffer, it will turn the VFIFO DMA TX interrupt on so that user is notified
 *               when VFIFO TX buffer becomes available again.
 *               At last, the driver returns the bytes that are placed in the VFIFO TX buffer.
 *               User should check the outcome to make sure that data is completely placed in the VFIFO TX buffer.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] data   specifies pointer to the user's data buffer.
 * @param[in] size   specifies size of the user's data buffer.
 * @return
 *                 bytes of transmitted data.
 * @note
 *            User must call hal_uart_init() before calling this function.
 */
uint32_t hal_uart_send_dma(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size);


/**
 * @brief     This function receives data byte by byte in a poll mode.
 *               In this function, driver first polls the status of a UART receive operation in a loop until it's ready.
 *               Then the driver gets the character from the UART transmit register and continues this operation until all user data is received.
 *               The final output is to get the character from the UART receive register and return it.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] buffer   specifies pointer to the user's data buffer.
 * @param[in] size   specifies size of the user's data buffer.
 * @return
 *                 the characters received from the UART port.
 */
uint32_t hal_uart_receive_polling(hal_uart_port_t uart_port, uint8_t *buffer, uint32_t size);


/**
 * @brief     This function receives user data in a VFIFO DMA mode.
 *               In this function, driver first polls the available data in the VFIFO RX buffer.
 *               Then driver receives user data from the VFIFO RX buffer and places to the available free space.
 *               If the driver can't get all user data from VFIFO RX buffer, it will turn the VFIFO DMA RX interrupt on so that user is notified
 *               when VFIFO RX buffer becomes available again.
 *               At last, the driver returns the bytes that are received from the VFIFO RX buffer.
 *               User should check the outcome to make sure that data is completely received from the VFIFO RX buffer.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] buffer   specifies pointer to the user's data buffer.
 * @param[in] size   specifies size of the user's data buffer.
 * @return
 *                 the characters received from the UART port.
 */
uint32_t hal_uart_receive_dma(hal_uart_port_t uart_port, uint8_t *buffer, uint32_t size);


/**
 * @brief     This function queries available space in the VFIFO TX buffer.
 * @param[in] uart_port   initializes the specified UART port number.
 * @return
 *                Available space of the VFIFO TX buffer.
 * @note
 *            This function is used only in a DMA mode.
 */
uint32_t hal_uart_get_available_send_space(hal_uart_port_t uart_port);


/**
 * @brief     This function queries available data in the VFIFO RX buffer.
 * @param[in] uart_port   initializes the specified UART port number.
 * @return
 *                Available data of the VFIFO RX buffer.
 * @note
 *            This function is used only in DMA mode.
 */
uint32_t hal_uart_get_available_receive_bytes(hal_uart_port_t uart_port);


/**
 * @brief     This function registers user's callback in the UART driver.
 *                This callback is used when the UART operates in a VFIFO DMA mode,
 *                then notifies the user if the UART Transmit is ready again.
 *                Also this callback is called in the UART VFIFO DMA interrupt handler.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] user_callback   specifies user's callback
 * @param[in] user_data   specifies user's data for this callback
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_BUSY if the UART port has been registered by others before. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if the UART has not been initialized. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid.
 */
hal_uart_status_t hal_uart_register_callback(hal_uart_port_t uart_port,
        hal_uart_callback_t user_callback,
        void *user_data);


/**
 * @brief     This function sets and enables hardware flow control of the UART.
 *                It enables the UART hardware to notify about a peer flow control event by automatically activating or deactivating an RTS output.
 *                Also the UART hardware can detect flow control indicator from peer based on the CTS input pin.
 *                For both RTS and CTS events, the UART hardware triggers interrupts.
 * @param[in] uart_port   initializes the specified UART port number.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if the UART has not been initialized. \n
 */
hal_uart_status_t hal_uart_set_hardware_flowcontrol(hal_uart_port_t uart_port);


/**
 * @brief     This function sets and enables software flow control of the UART.
 *                It enables the UART hardware to notify about a peer flow control event by sending an automatic XON or XOFF output.
 *                Also the UART hardware can issue flow control events if it receives XON or XOFF characters.
 *                For both transmit and receive operations, the UART hardware triggers interrupts.
 *                For transmit operation, escape characters are inserted before XON, XOFF and itself.
 *                For receive operation, if the UART hardware receives an escape character, it then reverses the consecutive byte and places it in the receive FIFO.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] xon   specifies user defined XON character that indicates if the data transmission is turned on.
 * @param[in] xoff   specifies user defined XOFF character that indicates if the data transmission is turned off.
 * @param[in] escape_character   specifies a user defined escape character.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if the UART has not been initialized. \n
 * @par       Example
 * @code
 *       uint8_t xon = 0x01;
 *       uint8_t xoff = 0x02;
 *       uint8_t escape_character = 0x77;
 *
 *       hal_uart_set_software_flowcontrol(HAL_UART_0, xon, xoff, escape_character);
 * @endcode
 */
hal_uart_status_t hal_uart_set_software_flowcontrol(hal_uart_port_t uart_port,
        uint8_t xon,
        uint8_t xoff,
        uint8_t escape_character);


/**
 * @brief     This function disables flow control of the UART.
 *                It disables the UART hardware to notify about a peer flow control event by automatically activating or deactivating an RTS output.
 *                The UART hardware is disabled to detect flow control indicator from peer based on the CTS input pin.
 *                This function will also disable the software flow control.
 * @param[in] uart_port   initializes the specified UART port number.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if the UART has not been initialized. \n
 */
hal_uart_status_t hal_uart_disable_flowcontrol(hal_uart_port_t uart_port);


/**
 * @brief     This function sets a baud rate for the UART frame.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] baudrate   specifies baud rate for the UART frame.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if the UART has not been initialized. \n
 * @par       Example
 * @code
 *       hal_uart_set_baudrate(HAL_UART_0, HAL_UART_BAUDRATE_115200);
 * @endcode
 */
hal_uart_status_t hal_uart_set_baudrate(hal_uart_port_t uart_port, hal_uart_baudrate_t baudrate);


/**
 * @brief     This function sets the UART's frame parameter.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] config   specifies a user defined frame parameter.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if the UART has not been initialized. \n
 * @par       Example
 * @code
 *       hal_uart_config_t uart_config;
 *
 *       uart_config.baudrate = HAL_UART_BAUDRATE_921600;
 *       uart_config.parity = HAL_UART_PARITY_NONE;
 *       uart_config.stop_bit = HAL_UART_STOP_BIT_1;
 *       uart_config.word_length = HAL_UART_WORD_LENGTH_8;
 *
 *       hal_uart_set_format(HAL_UART_0, &uart_config);
 * @endcode
 */
hal_uart_status_t hal_uart_set_format(hal_uart_port_t uart_port, const hal_uart_config_t *config);


/**
 * @brief     This function sets the VFIFO DMA hardware relative to the UART.
 *                With the help of the VFIFO DMA hardware, MCU doesn't need to copy data between the user buffer and the UART FIFO byte by byte.
 *                The MCU will be open to other tasks while the VFIFO DMA is copying data.
 *                The user is required to set threshold, address and size of the VFIFO TX/RX buffer for its proper operation.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] dma_config   specifies a user defined VFIFO DMA configuration parameter.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if UART has not been initialized. \n
 * @note
 *            This function must be called after hal_uart_init().
 */
hal_uart_status_t hal_uart_set_dma(hal_uart_port_t uart_port, const hal_uart_dma_config_t *dma_config);


#ifdef HAL_UART_FEATURE_VFIFO_DMA_TIMEOUT
/**
 * @brief     This function sets timeout value for the UART VFIFO DMA.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] timeout   specifies time out value for UART VFIFO DMA. The unit is us.
 * @return
 *                #HAL_UART_STATUS_OK if OK. \n
 *                #HAL_UART_STATUS_ERROR_PARAMETER if parameter is invalid. \n
 *                #HAL_UART_STATUS_ERROR_UNINITIALIZED if the UART has not been initialized. \n
 * @par       Example
 * @code
 *       hal_uart_set_dma_timeout(HAL_UART_0, 100); // set time out of VFIFO DMA to 100us.
 * @endcode
 */
hal_uart_status_t hal_uart_set_dma_timeout(hal_uart_port_t uart_port, uint32_t timeout);
#endif


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /* HAL_UART_MODULE_ENABLED */
#endif /* __HAL_UART_H__ */

