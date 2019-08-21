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

#ifndef __HAL_PDMA_INTERNAL_H__
#define __HAL_PDMA_INTERNAL_H__

/**
 * @ingroup HAL
 * @addtogroup Peripheral DMA
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


#include "hal_gdma.h"
#include "mt7687_cm4_hw_memmap.h"
#ifdef HAL_GDMA_MODULE_ENABLED
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"

#define PDMA_INIT  1
#define PDMA_DEINIT  0
#define PDMA_CHECK_AND_SET_INIT(pdma_port)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(pdma_init_status[pdma_port] == PDMA_INIT){ \
		log_hal_info("This peripheral dma channel %d has been initialized",pdma_port); \
	    assert(0); \
    } else { \
        pdma_init_status[pdma_port] = PDMA_INIT;  \
    } \
        restore_interrupt_mask(saved_mask); \
}while(0)

#define PDMA_SET_IDLE(pdma_port)   \
do{ \
	uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
	if(pdma_init_status[pdma_port] == PDMA_DEINIT){ \
		log_hal_info("This peripheral dma channel %d has been deinitialized",pdma_port); \
			assert(0); \
	} else { \
			pdma_init_status[pdma_port] = PDMA_DEINIT;  \
		} \
			restore_interrupt_mask(saved_mask); \
	}while(0)

#define VDMA_INIT  1
#define VDMA_DEINIT  0
#define VDMA_CHECK_AND_SET_INIT(vdma_port)  \
	do{ \
		uint32_t saved_mask; \
		saved_mask = save_and_set_interrupt_mask(); \
		if(vdma_init_status[vdma_port] == VDMA_INIT){ \
			log_hal_info("This vfifo dma channel %d has been initialized",vdma_port); \
			assert(0); \
		} else { \
			vdma_init_status[vdma_port] = VDMA_INIT;  \
		} \
			restore_interrupt_mask(saved_mask); \
	}while(0)

#define VDMA_SET_IDLE(vdma_port)   \
	do{ \
		uint32_t saved_mask; \
		saved_mask = save_and_set_interrupt_mask(); \
		if(vdma_init_status[vdma_port] == VDMA_DEINIT){ \
			log_hal_info("This vfifo dma channel %d has been deinitialized",vdma_port); \
				assert(0); \
		} else { \
				vdma_init_status[vdma_port] = VDMA_DEINIT;	\
			} \
				restore_interrupt_mask(saved_mask); \
		}while(0)

/*****************************************************************************
* Enum
*****************************************************************************/

/** @brief general dma status */
typedef enum {
    GDMA_ERROR  = -1,           /**<general  dma function EEROR */
    GDMA_OK  = 0                /**<general  dma function OK*/
} gdma_status_t;




/** @brief  dma direction define */
typedef enum {
    DMA_RX  = 0,                /**< dma direction type --dma rx dection*/
    DMA_TX  = 1                 /**< dma direction type --dma tx dection*/
} dma_direction_t;


/** @brief peripheral  dma master name */
typedef enum {

    PDMA_START_CHANNEL = 2,                      /**< peripheral dma start channel  */
    PDMA_I2C1_TX = PDMA_START_CHANNEL,           /**< peripheral dma channel 2 */
    PDMA_I2C1_RX,                                /**< peripheral dma channel 3 */
    PDMA_I2C2_TX,                                /**< peripheral dma channel 4 */
    PDMA_I2C2_RX,                                /**< peripheral dma channel 5 */
    PDMA_RES1,                                /**< peripheral dma channel 6 */
    PDMA_RES2,                                /**< peripheral dma channel 7 */
    PDMA_RES3,                                /**< peripheral dma channel 8 */
    PDMA_RES4,                                /**< peripheral dma channel 9 */
    PDMA_RES5,                                /**< peripheral dma channel 10 */
    PDMA_END_CHANNEL                          /**< peripheral dma end channel (invalid) */
} pdma_channel_t;


/** @brief peripheral  dma transfer size format */
typedef enum {
    PDMA_BYTE = 0,        /**< peripheral dma transfer size -byte format */
    PDMA_HALF_WORD,       /**<peripheral  dma transfer size -half word format */
    PDMA_WORD             /**<peripheral  dma transfer size -word  format */
} pdma_transfer_size_format_t;


/** @brief peripheral  dma transaction error */
typedef enum {
    PDMA_EVENT_TRANSACTION_ERROR = -1,         /**<peripheral  dma transaction error */
    PDMA_EVENT_TRANSACTION_SUCCESS = 0         /**<peripheral dma transaction success */
} pdma_event_t;


/** @brief peripheral dma status */
typedef enum {
    PDMA_ERROR               = -3,        /**<peripheral  dma function EEROR */
    PDMA_ERROR_CHANNEL       = -2,        /**< peripheral dma error channel */
    PDMA_INVALID_PARAMETER   = -1,        /**<peripheral  dma error invalid parameter */
    PDMA_OK   = 0                         /**<peripheral  dma function OK*/
} pdma_status_t;

/** @brief pdma  running status */
typedef enum {
    PDMA_IDLE = 0,                         /**< pdma idle */
    PDMA_BUSY = 1                         /**< pdma busy */
} pdma_running_status_t;


typedef enum {
    PDMA_TX,           /**<peripheral hw copy data from single buffer memory to register */
    PDMA_RX,           /**< peripheral hw copy data from register  to single buffer memeory */
    PDMA_TX_RINGBUFF,  /**<peripheral hw copy data from ring buffer memeory to register */
    PDMA_RX_RINGBUFF   /**<peripheral hw copy data from register to ring buffer memeory*/
} pdma_master_type_t;


/** @brief virtual fifo  dma master name */
typedef enum {
    VDMA_START_CHANNEL = 11,              /**< virtual fifo dma start channel */
	VDMA_I2STX = VDMA_START_CHANNEL,      /**< virtual fifo dma channel 11 */
	VDMA_I2SRX,                           /**< virtual fifo dma channel 12 */
    VDMA_UART0TX,                         /**< virtual fifo dma channel 13 */
    VDMA_UART0RX,                         /**<virtual fifo  dma channel 14 */
    VDMA_UART1TX,                         /**<virtual fifo  dma channel 15 */
    VDMA_UART1RX,                         /**< virtual fifo dma channel 16*/
    VDMA_BTIFTX,                         /**< virtual fifo dma channel 17 */
    VDMA_BTIFRX,                         /**<virtual fifo  dma channel 18 */
    VDMA_USBEP2O,                         /**<virtual fifo  dma channel 19 */
    VDMA_USBEP3I,                         /**< virtual fifo dma channel 20*/
    VDMA_USBEP3O,                         /**<virtual fifo  dma channel 21 */
    VDMA_USBEP4I,                         /**< virtual fifo dma channel 22*/
    VDMA_USBEP4O,                          /**< virtual fifo dma channel 23*/
    VDMA_ADCRX,                           /**< virtual fifo dma channel 24*/
    VDMA_END_CHANNEL                      /**< virtual fifo dma  end channel (invalid) */

} vdma_channel_t;

/** @brief virtual fifo dma status */
typedef enum {
    VDMA_ERROR               = -3,        /**<virtual fifo dma function EEROR */
    VDMA_ERROR_CHANNEL       = -2,        /**<virtual fifo dma error channel */
    VDMA_INVALID_PARAMETER   = -1,        /**< virtual fifo dma error invalid parameter */
    VDMA_OK   = 0                         /**< virtual fifo dma function OK*/
} vdma_status_t;



/** @brief virtual fifo dma  running status */
typedef enum {
    VDMA_IDLE = 0,                         /**< vdma idle */
    VDMA_BUSY = 1                          /**< vdma busy */
} vdma_running_status_t;

/** @brief virtual fifo  dma transaction error */
typedef enum {
    VDMA_EVENT_TRANSACTION_ERROR = -1,         /**<virtual fifo  dma transaction error */
    VDMA_EVENT_TRANSACTION_SUCCESS = 0         /**<virtual fifo  dma transaction success */
} vdma_event_t;

#define INVALID_STATUS          0xff

#define VALID_STATUS             0x0

#define HALF_TYPE_SIZE            2

#define WORD_TYPE_SIZE            4

#define VDMA_MAX_THRESHOLD       0xFFFF

#define VDMA_MAX_ALERT_LENGTH    0x3F

/* gdma channel number definition
*/
#define GDMA_NUMBER   2

/*pdma channel number definition
*/
#define PDMA_NUMBER   9

/*vdma channel number definition
*/
#define VDMA_NUMBER   14

/*all dma channel number definition,DMA1~DMA18, and DMA8 is reserved
*/
#define DMA_NUMBER   25




/*****************************************************************************
* Structure
*****************************************************************************/
/** @brief  peripheral dma transaction mode */
typedef struct {
    bool                           burst_mode;               /**< burst mode = 0 ==> single mode */
    pdma_transfer_size_format_t      size;                     /**<transfer size type , which is valiad when burst mode enabled*/
    uint32_t                         count;                    /**<count is beat number, which is valiad when burst mode enabled,max is 0x0FFF */
    pdma_master_type_t               master_type;              /**< master is TX/RX/ring buffer TX/ring buffer RX ,should select one*/
} pdma_config_t;



/** @brief  virtual  dma transaction mode */
typedef struct {
    uint32_t          base_address;                /**< the  virtual fifo  start address */
    uint32_t          size;                        /**< the  virtual fifo size */
} vdma_config_t;



/** @brief peripheral  dma callback typedef */
typedef void (*pdma_callback_t)(pdma_event_t event, void  *user_data);

/** @brief virtual dma callback typedef */
typedef void (*vdma_callback_t)(vdma_event_t event, void  *user_data);


/*****************************************************************************
* Function Interface
*****************************************************************************/


/**
 * @brief       init peripheral dma base config
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @return if OK ,returns PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @par       pdma_config_t *pdma_config
 * @code      pdma_init(PDMA_I2C1_TX,&pdma_config);
 * @endcode
 */
pdma_status_t pdma_init(pdma_channel_t channel);


/**
 * @brief       reset peripheral dma register and reset state
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @return if OK ,returns PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @code  	  pdma_deinit(PDMA_I2C1_TX);
 * @endcode
 */
pdma_status_t pdma_deinit(pdma_channel_t channel);


/**
 * @brief        peripheral dma set ring buffer function if need
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] wppt,the transter count required to perform before the jump point,which should be less than count value in COUNT register
 * @param[in] wpto,the wrap to address, address will wrap to wpto address when jump to wppt address
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @code  	  pdma_configure(PDMA_I2C1_TX,wwpt,wpto);
 * @endcode
 */
pdma_status_t pdma_set_ring_buffer(pdma_channel_t channel, uint32_t wppt, uint32_t wpto);

pdma_status_t pdma_set_limiter(pdma_channel_t channel, uint8_t limiter);
/**
 * @brief 	  peripheral dma all configuration
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] config sructure pointer,dma_config
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par         PDMA_I2C1_TX,pdma_config_t *pdma_config
 * @par         pdma_config->size        = DMA_WORD;
 * @par         pdma_config->burst_mode= TRUE;
 * @par          pdma_config->count  = 4;
 * @code         pdma_configure(PDMA_I2C1_TX,&pdma_config);
 * @endcode
 */
pdma_status_t pdma_configure(pdma_channel_t channel, pdma_config_t *pdma_config);


/**
 * @brief 	  dma work at polling mode ,
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] address
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par		  PDMA_I2C1_TX
 * @code  	  pdma_start_polling(PDMA_I2C1_TX,address);
 * @endcode
 */
pdma_status_t pdma_start_polling(pdma_channel_t channel, uint32_t address);

/**
 * @brief 	  dma work without interrupt mode ,
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] address
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par		  PDMA_I2C1_TX
 * @code  	  pdma_start_without_polling(PDMA_I2C1_TX,address);
 * @endcode
 */

pdma_status_t pdma_start_without_polling(pdma_channel_t channel, uint32_t address);
  
/**
 * @brief 	  dma work at interrupt mode ,user can register callback when dma interrupt happened
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] address
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par		  PDMA_I2C1_TX
 * @code  	  pdma_start_interrupt(PDMA_I2C1_TX,address,data_length);
 * @endcode
 */
pdma_status_t pdma_start_interrupt(pdma_channel_t channel, uint32_t address);


/**
 * @brief 	  stop dma work
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par		  PDMA_I2C1_TX
 * @code  	  pdma_stop(PDMA_I2C1_TX);
 * @endcode
 */
pdma_status_t pdma_stop(pdma_channel_t channel);


/**
 * @brief 	  peripheral dma register callback,
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] function_callback,function pointer
 * @param[in] parameter,user's parameter
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par		  PDMA_I2C1_TX
 * @par		  void function_callback(void *parameter)
 * @code  	  pdma_register_callback(PDMA_I2C1_TX,callback,&user_data);
 * @endcode
 */
pdma_status_t pdma_register_callback(pdma_channel_t channel, pdma_callback_t callback, void *user_data);


/**
 * @brief 	  get dma  current state
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[out] running_status ,get current pdma status-->DMA_BUSY or DMA_IDLE
 * @return if  get status  successfully ,return PDMA_OK
 * @note
 * @warning
 * @par		  PDMA_I2C1_TX
 * @code  	  pdma_get_running_status(PDMA_I2C1_TX,&running_status);
 * @endcode
 */
pdma_status_t pdma_get_running_status(pdma_channel_t channel, pdma_running_status_t *running_status);


/**
 * @brief       init virtual fifo dma init
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @return if OK ,returns VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @par       pdma_config_t *pdma_config
 * @code  	  pdma_init(VDMA_UART1TX);
 * @endcode
 */
vdma_status_t vdma_init(vdma_channel_t channel);


/**
 * @brief 	  reset peripheral dma register and reset state
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @return if OK ,returns VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code  	  pdma_deinit(VDMA_UART1TX);
 * @endcode
 */
vdma_status_t vdma_deinit(vdma_channel_t channel);

vdma_status_t vdma_start(vdma_channel_t channel);
vdma_status_t vdma_stop(vdma_channel_t channel);

/**
 * @brief 	  peripheral dma all configuration
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @param[in] config sructure pointer,vdma_config
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX,vdma_config_t  *vdma_config
 * @par           vdma_config->base_address  = vfifo_start_address;
 * @par		  pdma_config->size= 1024;
 * @code  	  vdma_configure(VDMA_UART1TX,vdma_config);
 * @endcode
 */
vdma_status_t vdma_configure(vdma_channel_t channel, vdma_config_t *vdma_config);


/**
 * @brief 	  enable vfifo dma interrupt
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_enable_interrupt(VDMA_UART1TX,address,data_length);
 * @endcode
 */
vdma_status_t vdma_enable_interrupt(vdma_channel_t channel);


/**
 * @brief 	  disable vfifo dma interrupt  ,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_disable_interrupt(VDMA_UART1TX);
 * @endcode
 */
vdma_status_t vdma_disable_interrupt(vdma_channel_t channel);

/**
 * @brief 	  set vfifo dma  threshold ,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[in] threshold
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_set_threshold(VDMA_UART1TX,1024);
 * @endcode
 */
vdma_status_t vdma_set_threshold(vdma_channel_t channel, uint32_t threshold);



/**
 * @brief 	  set vfifo dma  timeout ,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[in] timeout, which count tick is bus clock.
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_set_timeout(VDMA_UART1TX,200);
 * @endcode
 */

vdma_status_t vdma_set_timeout(vdma_channel_t channel, uint32_t timeout);


/**
 * @brief 	  set vfifo dma  alert length,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[in] alert_length,range 0~31
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_set_alert_length(VDMA_UART1TX,16);
 * @endcode
 */
vdma_status_t vdma_set_alert_length(vdma_channel_t channel, uint32_t alert_length);


/**
 * @brief 	  push data to virtual fifo,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[in] data
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_push_data(VDMA_UART1TX,data);
 * @endcode
 */
vdma_status_t vdma_push_data(vdma_channel_t channel, uint8_t data);

/**
 * @brief 	  pop data from virtual fifo,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *data,get single  byte from virtual fifo and put in data
 * @return if pop data correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_pop_data(VDMA_UART1TX,&data);
 * @endcode
 */
vdma_status_t vdma_pop_data(vdma_channel_t channel, uint8_t *data);

/**
 * @brief 	 get the number of data stored in virtual fifo
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *receive_bytes,get available received bytes from virtual fifo and put in receive_bytes,this is for RX VFIFO
 * @return if get data correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_get_available_room(VDMA_UART1TX,&receive_bytes);
 * @endcode
 */
vdma_status_t vdma_get_available_receive_bytes(vdma_channel_t channel, uint32_t *receive_bytes);

/**
 * @brief 	 get the left room of   virtual fifo
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *available_space ,get available space in virtual fifo and put in  available_space,this is for TX VFIFO
 * @return if get data correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @code  	  vdma_get_available_send_space(VDMA_UART1TX,&available_space);
 * @endcode
 */
vdma_status_t vdma_get_available_send_space(vdma_channel_t channel, uint32_t *available_space);



/**
 * @brief 	  virtual dma register callback,
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] function_callback,function pointer
 * @param[in] parameter,user's parameter
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @par		  void function_callback(void *parameter)
 * @code  	  vdma_register_callback(VDMA_UART1TX,function_callback,parameter);
 * @endcode
 */
vdma_status_t vdma_register_callback(vdma_channel_t channel, vdma_callback_t callback, void *user_data);


/**
 * @brief 	  virtual dma register timeout callback,
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] function_callback,function pointer
 * @param[in] parameter,user's parameter
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par		  VDMA_UART1TX
 * @par		  void function_timeout_callback(void *parameter)
 * @code  	  vdma_register_timeout_callback(VDMA_UART1TX,function_timeout_callback,parameter);
 * @endcode
 */
vdma_status_t vdma_register_timeout_callback(vdma_channel_t channel, vdma_callback_t timeout_callback, void *user_data);

void dma_interrupt_hander(hal_nvic_irq_t irq_number);
void dma_enable_clock(uint8_t channel);
void dma_disable_clock(uint8_t channel);


#ifdef __cplusplus
}
#endif

/**@ }*/

#endif /*HAL_GDMA_MODULE_ENABLED*/


#endif /* __HAL_PDMA_INTERNAL_H__ */

/**example
 Need to do
**/

