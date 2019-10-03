/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file implements the platform abstraction layer(pal) APIs for I2C.
*
* \ingroup  grPAL
* @{
*/

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "optiga/pal/pal_i2c.h"
#include "stm32l4xx_hal.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/// @cond hidden
/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
/* Varibale to indicate the re-entrant count of the i2c bus acquire function*/
static volatile uint32_t g_entry_count = 0;

/* Pointer to the current pal i2c context*/
static pal_i2c_t * gp_pal_i2c_current_ctx;

/**< OPTIGA™ Trust X I2C module queue. */
QueueHandle_t trustx_i2cresult_queue;

typedef struct stm_i2c_params
{
	I2C_TypeDef* i2c_inst;
	uint32_t     speed;
	uint32_t     timing;
}stm_i2c_params_t;

typedef struct stm_i2c_ctx
{
	I2C_HandleTypeDef 	i2c;
	stm_i2c_params_t    i2c_params;
}stm_i2c_ctx_t;

typedef struct i2c_result {
	/// Pointer to store upper layer callback context (For example: Ifx i2c context)
	pal_i2c_t * i2c_ctx;
	/// I2C Transmission result (e.g. PAL_I2C_EVENT_SUCCESS)
	uint16_t i2c_result;
}i2c_result_t;

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/
// I2C acquire bus function
//lint --e{715} suppress the unused p_i2c_context variable lint error , since this is kept for future enhancements
static pal_status_t pal_i2c_acquire(const void* p_i2c_context)
{
    if(g_entry_count == 0)
    {
        g_entry_count++;
        if(g_entry_count == 1)
        {
            return PAL_STATUS_SUCCESS;
        }
    }
    return PAL_STATUS_FAILURE;
}

// I2C release bus function
//lint --e{715} suppress the unused p_i2c_context variable lint, since this is kept for future enhancements
static void pal_i2c_release(const void* p_i2c_context)
{
    g_entry_count = 0;
}
/// @endcond

/**
 * Pal I2C event handler function to invoke the registered upper layer callback<br>
 *
 *<b>API Details:</b>
 *  - This function implements the platform specific i2c event handling mechanism<br>
 *  - It calls the registered upper layer function after completion of the I2C read/write operations<br>
 *  - The respective event status are explained below.
 *   - #PAL_I2C_EVENT_ERROR when I2C fails due to low level failures(NACK/I2C protocol errors)
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed
 *
 * \param[in] p_pal_i2c_ctx   Pointer to the pal i2c context #pal_i2c_t
 * \param[in] event           Status of the event reported after read/write completion or due to I2C errors
 *
 */
void invoke_upper_layer_callback (const pal_i2c_t* p_pal_i2c_ctx, host_lib_status_t event)
{
    app_event_handler_t upper_layer_handler;
    //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
    upper_layer_handler = (app_event_handler_t)p_pal_i2c_ctx->upper_layer_event_handler;

    upper_layer_handler(p_pal_i2c_ctx->upper_layer_ctx , event);

    //Release I2C Bus
    pal_i2c_release(p_pal_i2c_ctx->upper_layer_ctx);
}

//void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	i2c_result_t i2_result;
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//
//	i2_result.i2c_ctx = gp_pal_i2c_current_ctx;
//	i2_result.i2c_result = PAL_I2C_EVENT_SUCCESS;
//
//    /*
//     * You cann't call callback from the timer callback, this might lead to a corruption
//     * Use queues instead to activate corresponding handler
//     * */
//	xQueueSendFromISR( trustx_i2cresult_queue, ( void * ) &i2_result, &xHigherPriorityTaskWoken );
//}
//
//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	i2c_result_t i2_result;
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//
//	i2_result.i2c_ctx = gp_pal_i2c_current_ctx;
//	i2_result.i2c_result = PAL_I2C_EVENT_SUCCESS;
//
//    /*
//     * You cann't call callback from the timer callback, this might lead to a corruption
//     * Use queues instead to activate corresponding handler
//     * */
//	xQueueSendFromISR( trustx_i2cresult_queue, ( void * ) &i2_result, &xHigherPriorityTaskWoken );
//}
//
///**
//  * @brief  I2C error callbacks
//  * @param  I2cHandle: I2C handle
//  * @note   This example shows a simple way to report transfer error, and you can
//  *         add your own implementation.
//  * @retval None
//  */
//void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
//{
//	i2c_result_t i2_result;
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//
//	i2_result.i2c_ctx = gp_pal_i2c_current_ctx;
//	i2_result.i2c_result = PAL_I2C_EVENT_ERROR;
//
//    /*
//     * You cann't call callback from the ISR, this might lead to a corruption
//     * Use queues instead to activate corresponding handler
//     * */
//	xQueueSendFromISR( trustx_i2cresult_queue, ( void * ) &i2_result, &xHigherPriorityTaskWoken );
//}

void i2c_result_handler( void * pvParameters )
{
	i2c_result_t i2_result;

	do {
		if( xQueueReceive( trustx_i2cresult_queue, &( i2_result ), ( TickType_t ) portMAX_DELAY ) )
		{
			invoke_upper_layer_callback(i2_result.i2c_ctx, i2_result.i2c_result);
		}
	} while(1);

	vTaskDelete( NULL );
}

/// @endcond
/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

/**
 * Initializes the i2c master with the given context.
 * <br>
 *
 *<b>API Details:</b>
 * - The platform specific initialization of I2C master has to be implemented as part of this API, if required.<br>
 * - If the target platform does not demand explicit initialization of i2c master
 *   (Example: If the platform driver takes care of init after the reset), it would not be required to implement.<br>
 * - The implementation must take care the following scenarios depending upon the target platform selected.
 *   - The implementation must handle the acquiring and releasing of the I2C bus before initializing the I2C master to
 *     avoid interrupting the ongoing slave I2C transactions using the same I2C master.
 *   - If the I2C bus is in busy state, the API must not initialize and return #PAL_STATUS_I2C_BUSY status.
 *   - Repeated initialization must be taken care with respect to the platform requirements. (Example: Multiple users/applications  
 *     sharing the same I2C master resource)
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 *
 * \param[in] p_i2c_context   Pal i2c context to be initialized
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C master init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C init fails.
 */
pal_status_t pal_i2c_init(const pal_i2c_t* p_i2c_context)
{
	uint16_t status = PAL_STATUS_FAILURE;
	stm_i2c_ctx_t * p_i2c_ctx = NULL;
	I2C_HandleTypeDef* p_i2c = NULL;
	GPIO_InitTypeDef  gpio_init_structure;

	do {

		if (p_i2c_context == NULL)
			break;

		p_i2c_ctx =  (stm_i2c_ctx_t *)p_i2c_context->p_i2c_hw_config;
		p_i2c = &p_i2c_ctx->i2c;

		/* I2C configuration */
		p_i2c->Instance              = p_i2c_ctx->i2c_params.i2c_inst;
		p_i2c->Init.Timing           = p_i2c_ctx->i2c_params.timing;
		p_i2c->Init.OwnAddress1      = 0;
		p_i2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
		p_i2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
		p_i2c->Init.OwnAddress2      = 0;
		p_i2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
		p_i2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

		/*** Configure the GPIOs ***/
		/* Enable GPIO clock */
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/* Configure I2C Tx, Rx as alternate function */
		gpio_init_structure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
		gpio_init_structure.Mode = GPIO_MODE_AF_OD;
		gpio_init_structure.Pull = GPIO_NOPULL;
		gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		gpio_init_structure.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &gpio_init_structure);

		/*** Configure the I2C peripheral ***/
		/* Enable I2C clock */
		__HAL_RCC_I2C1_CLK_ENABLE();

		/* Force the I2C peripheral clock reset */
		__HAL_RCC_I2C1_FORCE_RESET();

		/* Release the I2C peripheral clock reset */
		__HAL_RCC_I2C1_RELEASE_RESET();

		/* Enable and set I2Cx Interrupt to a lower priority */
		HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0x0F, 0);
		HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

		/* Enable and set I2Cx Interrupt to a lower priority */
		HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0x0F, 0);
		HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

		if(HAL_I2C_Init(p_i2c) != HAL_OK)
		{
			break;
		}

		HAL_I2CEx_ConfigAnalogFilter(p_i2c, I2C_ANALOGFILTER_ENABLE);

		/* Create the handler for the callbacks. */
		xTaskCreate( i2c_result_handler,       /* Function that implements the task. */
					"TrstI2CXHndlr",          /* Text name for the task. */
					configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
					NULL,    /* Parameter passed into the task. */
					tskIDLE_PRIORITY,/* Priority at which the task is created. */
					NULL );      /* Used to pass out the created task's handle. */

		/* Create a queue for results. Not more than 2 interrupts one by one are expected*/
		trustx_i2cresult_queue = xQueueCreate( 2, sizeof( i2c_result_t ) );

		status = PAL_STATUS_SUCCESS;
	}while(0);

    return status;
}

/**
 * De-initializes the I2C master with the specified context.
 * <br>
 *
 *<b>API Details:</b>
 * - The platform specific de-initialization of I2C master has to be implemented as part of this API, if required.<br>
 * - If the target platform does not demand explicit de-initialization of i2c master
 *   (Example: If the platform driver takes care of init after the reset), it would not be required to implement.<br>
 * - The implementation must take care the following scenarios depending upon the target platform selected.
 *   - The implementation must handle the acquiring and releasing of the I2C bus before de-initializing the I2C master to
 *     avoid interrupting the ongoing slave I2C transactions using the same I2C master.
 *   - If the I2C bus is in busy state, the API must not de-initialize and return #PAL_STATUS_I2C_BUSY status.
 *	 - This API must ensure that multiple users/applications sharing the same I2C master resource is not impacted.
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 *
 * \param[in] p_i2c_context   I2C context to be de-initialized
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C master de-init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C de-init fails.
 */
pal_status_t pal_i2c_deinit(const pal_i2c_t* p_i2c_context)
{
    return PAL_STATUS_SUCCESS;
}

/**
 * Writes the data to I2C slave.
 * <br>
 * <br>
 * \image html pal_i2c_write.png "pal_i2c_write()" width=20cm
 *
 *
 *<b>API Details:</b>
 * - The API attempts to write if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released only after the completion of transmission or after completion of error handling.<br>
 * - The API invokes the upper layer handler with the respective event status as explained below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed asynchronously
 *<br>
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 * - The upper_layer_event_handler must be initialized in the p_i2c_context before invoking the API.<br>
 *
 *<b>Notes:</b><br> 
 *  - Otherwise the below implementation has to be updated to handle different bitrates based on the input context.<br>
 *  - The caller of this API must take care of the guard time based on the slave's requirement.<br>
 *
 * \param[in] p_i2c_context  Pointer to the pal I2C context #pal_i2c_t
 * \param[in] p_data         Pointer to the data to be written
 * \param[in] length         Length of the data to be written
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C write is invoked successfully
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C write fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy. 
 */
 
pal_status_t pal_i2c_write(pal_i2c_t* p_i2c_context,uint8_t* p_data , uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;
    I2C_HandleTypeDef * p_i2c = NULL;

    //Acquire the I2C bus before read/write
    if((PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context)) && (p_i2c_context != NULL))
    {
        gp_pal_i2c_current_ctx = p_i2c_context;
        p_i2c = &((stm_i2c_ctx_t *)(p_i2c_context->p_i2c_hw_config))->i2c;

        //Invoke the low level i2c master driver API to write to the bus
        if (HAL_I2C_Master_Transmit(p_i2c, (p_i2c_context->slave_address << 1), p_data, length, 50) != HAL_OK)
        {
            //If I2C Master fails to invoke the write operation, invoke upper layer event handler with error.

            //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
            ((app_event_handler_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_ERROR);
        }
        else
        {
            //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
            ((app_event_handler_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
            status = PAL_STATUS_SUCCESS;
        }

        //Release I2C Bus
        pal_i2c_release((void *)p_i2c_context);
    }
    else
    {
        status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
        ((app_event_handler_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_BUSY);
    }
    return status;
}

/**
 * Reads the data from I2C slave.
 * <br>
 * <br>
 * \image html pal_i2c_read.png "pal_i2c_read()" width=20cm
 *
 *<b>API Details:</b>
 * - The API attempts to read if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released only after the completion of reception or after completion of error handling.<br>
 * - The API invokes the upper layer handler with the respective event status as explained below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed asynchronously
 *<br>
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 * - The upper_layer_event_handler must be initialized in the p_i2c_context before invoking the API.<br>
 *
 *<b>Notes:</b><br> 
 *  - Otherwise the below implementation has to be updated to handle different bitrates based on the input context.<br>
 *  - The caller of this API must take care of the guard time based on the slave's requirement.<br>
 *
 * \param[in]  p_i2c_context  pointer to the PAL i2c context #pal_i2c_t
 * \param[in]  p_data         Pointer to the data buffer to store the read data
 * \param[in]  length         Length of the data to be read
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C read is invoked successfully
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C read fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy.
 */
pal_status_t pal_i2c_read(pal_i2c_t* p_i2c_context , uint8_t* p_data , uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;
    I2C_HandleTypeDef * p_i2c = NULL;

    //Acquire the I2C bus before read/write
    if ((PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context)) && (p_i2c_context != NULL))
    {
        gp_pal_i2c_current_ctx = p_i2c_context;
        p_i2c = &((stm_i2c_ctx_t *)(p_i2c_context->p_i2c_hw_config))->i2c;

        //Invoke the low level i2c master driver API to read from the bus
        if (HAL_I2C_Master_Receive(p_i2c, (p_i2c_context->slave_address << 1), p_data, length, 50) != HAL_OK)
        {
            //If I2C Master fails to invoke the read operation, invoke upper layer event handler with error.

            //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
            ((app_event_handler_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_ERROR);
        }
        else
        {
            //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
            ((app_event_handler_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
            status = PAL_STATUS_SUCCESS;
        }

        //Release I2C Bus
        pal_i2c_release((void *)p_i2c_context);
    }
    else
    {
        status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
        ((app_event_handler_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_BUSY);
    }
    return status;
}

   
/**
 * Sets the bitrate/speed(KHz) of I2C master.
 * <br>
 *
 *<b>API Details:</b>
 * - Sets the bitrate of I2C master if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released after the setting the bitrate.<br>
 * - This API must take care of setting the bitrate to I2C master's maximum supported value. 
 * - Eg. In XMC4500, the maximum supported bitrate is 400 KHz. If the supplied bitrate is greater than 400KHz, the API will 
 *   set the I2C master's bitrate to 400KHz.
 * - Use the #PAL_I2C_MASTER_MAX_BITRATE macro to specify the maximum supported bitrate value for the target platform.
 * - If upper_layer_event_handler is initialized, the upper layer handler is invoked with the respective event 
 *   status listed below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails to set the bit rate 
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successful
 *<br>
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t  p_i2c_context must not be NULL.<br>
 *
 * \param[in] p_i2c_context  Pointer to the pal i2c context
 * \param[in] bitrate        Bitrate to be used by i2c master in KHz
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the setting of bitrate is successfully completed
 * \retval  #PAL_STATUS_FAILURE  Returns when the setting of bitrate fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy.
 */
pal_status_t pal_i2c_set_bitrate(const pal_i2c_t* p_i2c_context , uint16_t bitrate)
{
	  pal_status_t return_status = PAL_STATUS_SUCCESS;
//    pal_status_t return_status = PAL_STATUS_FAILURE;
//    host_lib_status_t event = PAL_I2C_EVENT_ERROR;
//
//    //Acquire the I2C bus before setting the bitrate
//    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
//    {
//        // If the user provided bitrate is greater than the I2C master hardware maximum supported value,
//        // set the I2C master to its maximum supported value.
//        if (bitrate > PAL_I2C_MASTER_MAX_BITRATE)
//        {
//            bitrate = PAL_I2C_MASTER_MAX_BITRATE;
//        }
//        if (XMC_I2C_CH_STATUS_OK != XMC_I2C_CH_SetBaudrate(((I2C_MASTER_t *)p_i2c_context->p_i2c_hw_config)->channel, bitrate*1000))
//        {
//            return_status = PAL_STATUS_FAILURE;
//        }
//        else
//        {
//            return_status = PAL_STATUS_SUCCESS;
//            event = PAL_I2C_EVENT_SUCCESS;
//        }
//    }
//    else
//    {
//        return_status = PAL_STATUS_I2C_BUSY;
//        event = PAL_I2C_EVENT_BUSY;
//    }
//    if (p_i2c_context->upper_layer_event_handler)
//    {
//        //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
//        ((app_event_handler_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->upper_layer_ctx , event);
//    }
//    //Release I2C Bus
//    pal_i2c_release((void *)p_i2c_context);
    return return_status;
}

/**
* @}
*/
