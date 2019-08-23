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
#include "driver/i2c.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
#define PAL_I2C_MASTER_TX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define PAL_I2C_MASTER_RX_BUF_DISABLE   0                /*!< I2C master do not need buffer */

#define CONFIG_PAL_I2C_INIT_ENABLE 1

#define WRITE_BIT                       I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                        I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                    0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                   0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                         0x0              /*!< I2C ack value */
#define NACK_VAL                        0x1              /*!< I2C nack value */

/// @cond hidden
/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
typedef struct esp32_i2c_ctx {
	uint8_t  port;
	uint8_t	 scl_io;
	uint8_t	 sda_io;
	uint32_t bitrate;
}esp32_i2c_ctx_t;

/* Varibale to indicate the re-entrant count of the i2c bus acquire function*/
static volatile uint32_t g_entry_count = 0;

/* Pointer to the current pal i2c context*/
static pal_i2c_t * gp_pal_i2c_current_ctx;

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

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
	int master_port;
	esp32_i2c_ctx_t* master_ctx;
	i2c_config_t conf;
	
#ifdef CONFIG_PAL_I2C_INIT_ENABLE	
	printf("Initialize pal_i2c_init  ");

	if ((p_i2c_context == NULL) || (p_i2c_context->p_i2c_hw_config == NULL))
		return PAL_STATUS_FAILURE;
	
	master_ctx = (esp32_i2c_ctx_t*)p_i2c_context->p_i2c_hw_config;
	master_port = master_ctx->port;
	
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = master_ctx->sda_io;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = master_ctx->scl_io;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = master_ctx->bitrate;
    i2c_param_config(master_port, &conf);
    i2c_driver_install(master_port, conf.mode,
                       PAL_I2C_MASTER_TX_BUF_DISABLE,
                       PAL_I2C_MASTER_RX_BUF_DISABLE, 0);
	
    printf("OK\r\n");
#endif

    return PAL_STATUS_SUCCESS;
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
	esp32_i2c_ctx_t* master_ctx;

#ifdef CONFIG_PAL_I2C_INIT_ENABLE			
	if ((p_i2c_context == NULL) || (p_i2c_context->p_i2c_hw_config == NULL))
		return PAL_STATUS_FAILURE;
	
	master_ctx = (esp32_i2c_ctx_t*)p_i2c_context->p_i2c_hw_config;
	
	i2c_driver_delete(master_ctx->port);
#endif
	
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
	app_event_handler_t upper_layer_handler;
	int i2c_master_port;
	esp32_i2c_ctx_t* master_ctx;

	gp_pal_i2c_current_ctx = p_i2c_context;

	if ((p_i2c_context == NULL) || (p_i2c_context->p_i2c_hw_config == NULL))
		return status;

	master_ctx = (esp32_i2c_ctx_t*)p_i2c_context->p_i2c_hw_config;
	i2c_master_port = master_ctx->port;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (p_i2c_context->slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, p_data, length, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

	upper_layer_handler = (app_event_handler_t)gp_pal_i2c_current_ctx->upper_layer_event_handler;

	if (ret == ESP_OK) {
		upper_layer_handler(p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
		status = PAL_STATUS_SUCCESS;
	} else {
        upper_layer_handler(p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_ERROR);
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
    app_event_handler_t upper_layer_handler;
	int i2c_master_port;
	esp32_i2c_ctx_t* master_ctx;

	gp_pal_i2c_current_ctx = p_i2c_context;

	if (length == 0)
        return status;
	
	if ((p_i2c_context == NULL) || (p_i2c_context->p_i2c_hw_config == NULL))
		return status;
	


	master_ctx = (esp32_i2c_ctx_t*)p_i2c_context->p_i2c_hw_config;
	i2c_master_port = master_ctx->port;
	
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( p_i2c_context->slave_address << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (length > 1) {
        i2c_master_read(cmd, p_data, length - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, p_data + length - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
	
    upper_layer_handler = (app_event_handler_t)gp_pal_i2c_current_ctx->upper_layer_event_handler;
	
	if (ret == ESP_OK) {
		upper_layer_handler(p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
		status = PAL_STATUS_SUCCESS;
	} else {
        upper_layer_handler(p_i2c_context->upper_layer_ctx , PAL_I2C_EVENT_ERROR);
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
    return PAL_STATUS_SUCCESS;
}

/**
* @}
*/
