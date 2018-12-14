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

#ifndef __HAL_GDMA_H__
#define __HAL_GDMA_H__
#include "hal_platform.h"

#ifdef HAL_GDMA_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GDMA
 * @{
 * This section introduces the GDMA APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, General Direct Memeory Access(GDMA) function groups, enums, structures and functions.
 *
 * @section HAL_GDMA_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b GDMA                | General Direct Memeory Access. GDMA is an operation in which data is copied (transported) from one resource to another in a computer system without the involvement of the CPU.
 *  For more information, please refer to <a href="https://en.wikipedia.org/wiki/Direct_memory_access"> GDMA in Wikipedia </a>.|
 * @section GDMA_Features_Chapter Supported features
 *
 * - \b Support \b polling \b and \b interrupt \b modes
 *   GDMA can copy data from source memory to destination memory, instead of from  memory to I/O-device or I/O-device to memory.
 *  - \b Polling \b mode:  In the polling mode, the execution status of the GDMA hardware returns valid during the transaction and invalid once the data transaction is finished. The status is retrieved only after the transaction is finished.
 *  - \b Interrupt \b mode: In this mode, the GDMA hardware generates an interrupt once the transaction is complete. \n
 *   \n
 * @section GDMA_Architecture_Chapter Software architecture of the GDMA
 *
 * The software architecture of GDMA driver is shown below.
 * -# GDMA polling mode architecture :\n
 * Call the function #hal_gdma_init() to initialize the GDMA source clock.\n
 * Call the function #hal_gdma_start_polling() to transfer data.\n
 * Call the function #hal_gdma_stop() to stop the GDMA data transfer.\n
 * Call the function #hal_gdma_get_running_status() to get the status, either busy or idle. The application can also de-initialize the GDMA by calling #hal_gdma_deinit().\n
 *  Polling mode architecture is similar to the polling mode architecture in HAL. See @ref HAL_Overview_3_Chapter for polling mode architecture.
 * -# GDMA interrupt mode architecture :\n
 * Call the function #hal_gdma_init() to initialize the GDMA source clock.\n
 * Call the function #hal_gdma_register_callback() to register user's callback, and then call the function #hal_gdma_start_interrupt() to transfer data.\n
 * Call the function #hal_gdma_stop() to stop GDMA data transfer.\n
 * Call the function #hal_gdma_get_running_status() to get the status, either busy or idle.\n
 * Call the function #hal_gdma_deinit() to de-initialize GDMA to its original state.\n
 * Interrupt mode architecture is similar to the interrupt mode architecture in HAL. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.


 * @section GDMA_Driver_Usage_Chapter How to use this driver
 *
 * - Use GDMA with polling mode. \n
 *  1. Call #hal_gdma_init() to initialize GDMA source clock.
 *  2. Call #hal_gdma_start_polling() to transfer data.
 *  3. Call #hal_gdma_get_running_status() to get the status, either busy or idle.
 *  4. Call #hal_gdma_stop() to stop GDMA.
 *  5. Call #hal_gdma_deinit() to de-initialize GDMA.
 *    @code
 *    Note 1. To use DMA:
 *           - Memory should be non-cacheable.
 *           - Memory address should be 4 bytes aligned.
 *    Note 2. To get non-cacheable memory:
 *           - Define a global data array and add a memory attribute.
 *                      ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN, if the array has no initial value.
 *                      ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN, if the array has an initial value.
 *             Example:  ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  source_address[data_length];
 *                      ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  destination_address[data_length];
 *            - Apply memory dynamically with an API void* PvPortMallocNC(size_t xWantedSize).
 *             Example:  char *source_address= PvPortMallocNC(data_length);
 *                       char *destination_address= PvPortMallocNC(data_length);
 *    Note 3. To apply UART/SPI/I2C's in DMA mode, please follow Note 2.
 *
 *       hal_gdma_channel_t channel=HAL_GDMA_CHANNEL0;
 *       static uint32_t dma_length = 64;
 *       ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  source_address[64];
 *       ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  destination_address[64];
 *       hal_gdma_running_status_t running_status;
 *
 *       if(HAL_GDMA_STATUS_OK != hal_gdma_init(channel)) { // Initialize the GDMA source clock.
 *             // Error handle.
 *       }
 *       if(HAL_GDMA_STATUS_OK != hal_gdma_start_polling(channel, destination_address, source_address, data_length)) {
 *             // Error handle.
 *       }
 *
 *       hal_gdma_get_running_status(channel,&running_status);
 *       //...
 *       hal_gdma_stop(channel); // Stop GDMA.
 *       hal_gdma_deinit(channel);   // De-initialize the GDMA.
 *     @endcode
 * - Use GDMA with interrupt mode. \n
 *  1. Call #hal_gdma_init() to initialize GDMA source clock.
 *  2. call #hal_gdma_register_callback() to register a callback function.
 *  3. Call #hal_gdma_start_interrupt() to transfer data.
 *  4. Call #hal_gdma_get_running_status() to get the status, either busy or idle.
 *  5. Call #hal_gdma_stop() to stop GDMA.
 *  6. Call #hal_gdma_deinit() to de-initialize GDMA.

 *  - Sample code:
 *    @code
 *    Note 1. To use DMA:
 *           - Memory should be non-cacheable.
 *           - Memory address should be 4 bytes aligned.
 *    Note 2. To get non-cacheable memory:
 *           - Define a global data array and add a memory attribute.
 *                      ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN, if the array has no initial value.
 *                      ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN, if the array has an initial value.
 *             Example:  ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  source_address[data_length];
 *                      ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  destination_address[data_length];
 *            - Apply memory dynamically with an API void* PvPortMallocNC(size_t xWantedSize).
 *             Example:  char *source_address= PvPortMallocNC(data_length);
 *                       char *destination_address= PvPortMallocNC(data_length);
 *    Note 3. To apply UART/SPI/I2C's in DMA mode, please follow Note 2.
 *
 *       hal_gdma_channel_t channel=HAL_GDMA_CHANNEL0;
 *       static uint32_t dma_length = 64;
 *       ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  source_address[64];
 *       ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  destination_address[64];
 *       uint32_t user_data;
 *       user_data = 1;//this value will be passed to the application once the callback function is called
 *       hal_gdma_running_status_t running_status;
 *
 *       if(HAL_GDMA_STATUS_OK != hal_gdma_init(channel)) { // Initialize the GDMA source clock.
 *             // Error handle.
 *       }
 *       if(HAL_GDMA_STATUS_OK != hal_gdma_register_callback(channel, callback,(void *) &user_data) {  // Register user's callback.
 *             // Error handle.
 *       }
 *       if(HAL_GDMA_STATUS_OK != hal_gdma_start_interrupt(channel,  destination_address, source_address, data_length)) { //Enable GDMA to start transferring data.
 *             // Error handle.
 *       }
 *       //...
 *       hal_gdma_get_running_status(channel, &running_status);
 *       //...
 *       hal_gdma_stop(channel); // Stop the GDMA.
 *       hal_gdma_deinit(channel);   //De-initialize the GDMA.
 *
 *    @endcode
 *    @code
 *       // Callback function. This function should be registered with hal_gdma_register_callback().
 *         static void gdma_callback(hal_gdma_event_t event, void  *user_data)
 *         {  //gdma_callback will be triggered when gdma transfers finished.
 *            //add your code here, such as:
 *
 *               hal_gdma_stop(channel);
 *               hal_gdma_deinit(channel);
 *               printf("\r\n ---gdma_example finished!!!---\r\n");
 *         }
 *
 *    @endcode
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_gdma_enum Enum
  * @{
  */


/** @brief GDMA status */
typedef enum {
    HAL_GDMA_STATUS_ERROR               = -3,         /**< GDMA function error occurred. */
    HAL_GDMA_STATUS_ERROR_CHANNEL       = -2,         /**< Invalid GDMA channel. */
    HAL_GDMA_STATUS_INVALID_PARAMETER   = -1,         /**< Invalid input parameter. */
    HAL_GDMA_STATUS_OK   = 0,                         /**< GDMA completed successfully. */
} hal_gdma_status_t;


/** @brief GDMA transaction error */
typedef enum {
    HAL_GDMA_EVENT_TRANSACTION_ERROR = -1,       /**< GDMA transaction error occurred. */
    HAL_GDMA_EVENT_TRANSACTION_SUCCESS = 0,      /**< GDMA transaction completed successfully. */
} hal_gdma_event_t;


/** @brief GDMA current running status. */
typedef enum {
    HAL_GDMA_IDLE = 0,                         /**< GDMA idle. */
    HAL_GDMA_BUSY = 1,                         /**< GDMA busy. */
} hal_gdma_running_status_t;

/**
  * @}
  */

/** @defgroup hal_gdma_typedef Typedef
   * @{
   */

/** @brief  This defines the callback function prototype.
 *          Register a callback function when in an interrupt mode, this function will be called in GDMA interrupt
 *          service routine after a transaction is complete. For more details about the callback, please refer to the #hal_gdma_register_callback() function.
 *          For more details about the callback architecture, please refer to @ref GDMA_Architecture_Chapter.
 *  @param [in] event is the transaction event for the current transaction, application can get the transaction result from this parameter.
 *              For more details about the event type, please refer to #hal_gdma_event_t
 *  @param [in] user_data is a parameter provided  by the application applying the #hal_gdma_register_callback() function.
 */

typedef void (*hal_gdma_callback_t)(hal_gdma_event_t event, void  *user_data);

/**
  * @}
  */

/**
 * @brief     This function initializes the GDMA based configuration.
 * @param[in] channel GDMA definition in enum #hal_gdma_channel_t
 * @return   To indicate whether this function call is successful or not.
 *           #HAL_GDMA_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa  hal_gdma_deinit()
 */
hal_gdma_status_t hal_gdma_init(hal_gdma_channel_t channel);


/**
 * @brief     This function resets GDMA registers and state.
 * @param[in] channel GDMA master name definition in enum #hal_gdma_channel_t.
 * @return   To indicate whether this function call is successful or not.
 *            #HAL_GDMA_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa  hal_gdma_init()
 */

hal_gdma_status_t hal_gdma_deinit(hal_gdma_channel_t channel);


/**
 * @brief      This function enables the GDMA to operate in a polling mode.
 * @param[in] channel GDMA master name definition in enum #hal_gdma_channel_t.
 * @param[in] destination_address is the address to copy the source data into.
 * @param[in] source_address is the address where application's source data is stored.
 * @param[in]  data_length is the data length used in the transaction.
 * @return   To indicate whether this function call is successful or not.
 *           #HAL_GDMA_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa  hal_gdma_start_interrupt()
 */

hal_gdma_status_t hal_gdma_start_polling(hal_gdma_channel_t channel, uint32_t destination_address, uint32_t source_address,  uint32_t data_length);


/**
 * @brief      This function enables the GDMA to operate in an interrupt mode, the application can register a callback when a GDMA interrupt occurs.
 * @param[in] channel GDMA master name definition in enum #hal_gdma_channel_t.
 * @param[in] destination_address is the address to copy the source data into.
 * @param[in] source_address is the address where application's source data is stored.
 * @param[in]  data_length is the data length used in the transaction.
 * @return   To indicate whether this function call is successful or not.
 *          #HAL_GDMA_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa  hal_gdma_start_polling()
 */

hal_gdma_status_t hal_gdma_start_interrupt(hal_gdma_channel_t channel, uint32_t destination_address, uint32_t source_address,  uint32_t data_length);


/**
 * @brief     This function registers a GDMA callback.
 * @param[in] channel GDMA master name definition in enum #hal_gdma_channel_t.
 * @param[in] callback is the callback function given by the application, that is called at the GDMA interrupt service routine.
 * @param[in] user_data is a user input provided by the application and is passed to the application once the callback function is called. See the last parameter of #hal_gdma_callback_t.
 * @par       Example
 * Sample code, please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa   hal_gdma_start_interrupt()
 */

hal_gdma_status_t hal_gdma_register_callback(hal_gdma_channel_t channel, hal_gdma_callback_t callback, void *user_data);


/**
 * @brief    This function gets the current state of the GDMA.
 * @param[in] channel GDMA master name definition in enum #hal_gdma_channel_t.
 * @param[out] running_status is the current running status.
 *             #HAL_GDMA_BUSY, the GDMA is busy.
 *             #HAL_GDMA_IDLE, the GDMA is idle and ready to transfer data.
 * @return   To indicate whether this function call is successful or not.
 *           #HAL_GDMA_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa   hal_gdma_start_interrupt()
 */

hal_gdma_status_t hal_gdma_get_running_status(hal_gdma_channel_t channel, hal_gdma_running_status_t *running_status);


/**

 * @brief     This function stops the GDMA operation.
 * @param[in] channel GDMA master name definition in enum #hal_gdma_channel_t.
 * @return   To indicate whether this function call is successful or not.
 *           #HAL_GDMA_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa   hal_gdma_start_polling(), #hal_gdma_start_interrupt()
 */

hal_gdma_status_t hal_gdma_stop(hal_gdma_channel_t channel);


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_GDMA_MODULE_ENABLED*/
#endif /* __HAL_GDMA_H__ */




