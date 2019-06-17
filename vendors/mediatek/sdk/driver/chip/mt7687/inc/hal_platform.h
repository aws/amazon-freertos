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

#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


#include "hal_define.h"
#include "hal_feature_config.h"
#include "mt7687.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
* module sub features define
*****************************************************************************/
#ifdef HAL_ADC_MODULE_ENABLED
#define HAL_ADC_FEATURE_NO_NEED_PINMUX             /*please not to modify*/
#endif

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#define HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE  /* Enable I2C master send to receive feature. */
#endif

#ifdef HAL_PWM_MODULE_ENABLED
#define HAL_PWM_FEATURE_SINGLE_SOURCE_CLOCK        /*please not to modify*/
#endif

#ifdef HAL_EINT_MODULE_ENABLED
#define HAL_EINT_FEATURE_MASK                /* Supports EINT mask interrupt. */
#define HAL_EINT_FEATURE_SW_TRIGGER_EINT     /* Supports software triggered EINT interrupt. */
#endif

#ifdef HAL_RTC_MODULE_ENABLED
#define HAL_RTC_FEATURE_SLEEP        /*please not to modify*/
#endif

#ifdef HAL_UART_MODULE_ENABLED
#define HAL_UART_FEATURE_VFIFO_DMA_TIMEOUT        /*please not to modify*/
#define HAL_UART_FEATURE_5M_BAUDRATE
#endif

#ifdef HAL_I2S_MODULE_ENABLED
#define HAL_I2S_EXTENDED                        /*please not to modify*/
#define HAL_I2S_SUPPORT_VFIFO                   /*please not to modify*/
#endif

#ifdef HAL_GDMA_MODULE_ENABLED
#define HAL_GDMA_WRAP_FLASH_ADDRESS_TO_VIRTUAL_ADDRESS       /*please not to modify*/
#endif


#ifdef HAL_UART_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup UART
 * @{
 * @addtogroup hal_uart_enum Enum
 * @{
 */
/*****************************************************************************
* uart
*****************************************************************************/
/** @brief UART port index
 * We have total 2 UART ports. And UART0 and UART1 support hardware flow control.
 * | UART port | hardware flow control |
 * |-----------|-----------------------|
 * |  UART0    |           V           |
 * |  UART1    |           V           |
 */
typedef enum {
    HAL_UART_0 = 0,                            /**< uart port0 */
    HAL_UART_1 = 1,                            /**< uart port1 */
    HAL_UART_MAX
} hal_uart_port_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_I2C_MASTER_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_MASTER
 * @{
 * @section HAL_I2C_Transaction_Pattern_Chapter Transaction Length And transaction packet of APIs
 *  The Transaction packet is the transaction packet sent by the I2C master using SCL and SDA.
 *  Different APIs have different transaction packets, as shown below.
 * - \b Transaction \b length \b supported \b by \b the \b APIs \n
 *  The total transaction length is determined by 4 parameters: \n
 *   send_packet_length(Ns), which indicates the counts of send packet.\n
 *   send_bytes_in_one_packet(Ms).\n
 *   receive_packet_length(Nr).\n
 *   receive_bytes_in_one_packet(Mr).\n
 *  And also, these 4 parameters will affect the transaction packet. The ralationship between transaction packet and these 4 parameters will be introduced in the next section.
 *  - Total transaction length = Ns * Ms + Nr * Mr.
 *   - Ns is the packet length to be sent by the I2C master.
 *   - Ms is the total number of bytes in a sent packet.
 *   - Nr is the packet length to be received by the I2C master.
 *   - Mr is the total number of bytes in a received packet.
 *  - NA means the related parameter should be ignored.
 *  - 1~8 specifies the parameter range is from 1 to 8. 1~15 specifies the parameter range is from 1 to 15. 1~255 specifies the parameter range from 1 to 255.
 *  - 1 means the parameter value can only be 1.
 *  - Note: Only those functions with the suffix "_ex" have these 4 parameters. Other functions only have the "size" parameter and the driver splits the "size" into these 4 parameters.
 *    #hal_i2c_master_send_polling() for example, the "size" will be divided like this: send_packet_length = 1, send_bytes_in_one_packet = size.
 *          As a result, the total size should be: send_packet_length * send_bytes_in_one_packet = 1 * size = size. The range of "size" should be from 1 to 8.
 * |API                                         |send_packet_length(Ns) | send_bytes_in_one_packet(Ms) | receive_packet_length(Nr) | receive_bytes_in_one_packet(Mr) |
 * |--------------------------------------------|-----------------------|------------------------------|---------------------------|---------------------------------|
 * |hal_i2c_master_send_polling                 |          1            |            1~8               |            NA             |                NA               |
 * |hal_i2c_master_receive_polling              |          NA           |            NA                |            1              |                1~8              |
 * |hal_i2c_master_send_to_receive_polling      |          1            |            1~8               |            1              |                1~8              |
 * |hal_i2c_master_send_dma                     |          1            |            1~2^15            |            NA             |                NA               |
 * |hal_i2c_master_receive_dma                  |          NA           |            NA                |            1              |                1~2^15           |
 * |hal_i2c_master_send_to_receive_dma          |          1            |            1~2^15            |            1              |                1~2^15           |
 *
 * - \b Waveform \b pattern \b supported \b by \b the \b APIs \n
 *  The 4 parameters(send_packet_length(Ns), send_bytes_in_one_packet(Ms), receive_packet_length(Nr), receive_bytes_in_one_packet(Mr) will also affect the transaction packet.
 *  The relationship between transaction packet and these 4 parameters is shown below.
 *  - Ns is the send_packet_length.
 *  - Ms is the send_bytes_in_one_packet.
 *  - Nr is the receive_packet_length.
 *  - Mr is the receive_bytes_in_one_packet.
 * |API                                          |transaction packet format                                 |
 * |---------------------------------------------|----------------------------------------------------------|
 * | hal_i2c_master_send_polling                 |  @image html hal_i2c_send_poling_waveform.png            |
 * | hal_i2c_master_receive_polling              |  @image html hal_i2c_receive_poling_waveform.png         |
 * | hal_i2c_master_send_to_receive_polling      |  @image html hal_i2c_send_to_receive_poling_waveform.png |
 * | hal_i2c_master_send_dma                     |  @image html hal_i2c_send_dma_waveform.png               |
 * | hal_i2c_master_receive_dma                  |  @image html hal_i2c_receive_dma_waveform.png            |
 * | hal_i2c_master_send_to_receive_dma          |  @image html hal_i2c_send_to_receive_dma_waveform.png    |
 *
 *
 *
 *
 */


/**
 * @defgroup hal_i2c_master_define Define
 * @{
  */

/** @brief  The maximum polling mode transaction size.
  */
#define HAL_I2C_MAXIMUM_POLLING_TRANSACTION_SIZE  8

/** @brief  The maximum DMA mode transaction size.
  */
#define HAL_I2C_MAXIMUM_DMA_TRANSACTION_SIZE   65535


/**
  * @}
  */

/** @addtogroup hal_i2c_master_enum Enum
  * @{
  */

/*****************************************************************************
* i2c master
*****************************************************************************/
/** @brief This enum define the i2c port.
 *  We have total 2 I2C masters. Both of them support polling mode and DMA mode.
 *
 *
*/
typedef enum {
    HAL_I2C_MASTER_0 = 0,           /**< i2c master 0. */
    HAL_I2C_MASTER_1 = 1,           /**< i2c master 1. */
    HAL_I2C_MASTER_MAX              /**< max i2c master number, \<invalid\> */
} hal_i2c_port_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_GPIO_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup GPIO
* @{
*
* @addtogroup hal_gpio_enum Enum
* @{
*/

/*****************************************************************************
* gpio
*****************************************************************************/
/** @brief This enum define the gpio port.
 *
*/

typedef enum {
    HAL_GPIO_0 = 0,                       /**< GPIO pin0 */
    HAL_GPIO_1 = 1,                       /**< GPIO pin1 */
    HAL_GPIO_2 = 2,                       /**< GPIO pin2 */
    HAL_GPIO_3 = 3,                       /**< GPIO pin3 */
    HAL_GPIO_4 = 4,                       /**< GPIO pin4 */
    HAL_GPIO_5 = 5,                       /**< GPIO pin5 */
    HAL_GPIO_6 = 6,                       /**< GPIO pin6 */
    HAL_GPIO_7 = 7,                       /**< GPIO pin7 */

    HAL_GPIO_24 = 24,                     /**< GPIO pin24 */
    HAL_GPIO_25 = 25,                     /**< GPIO pin25 */
    HAL_GPIO_26 = 26,                     /**< GPIO pin26 */
    HAL_GPIO_27 = 27,                     /**< GPIO pin27 */
    HAL_GPIO_28 = 28,                     /**< GPIO pin28 */
    HAL_GPIO_29 = 29,                     /**< GPIO pin29 */
    HAL_GPIO_30 = 30,                     /**< GPIO pin30 */
    HAL_GPIO_31 = 31,                     /**< GPIO pin31 */
    HAL_GPIO_32 = 32,                     /**< GPIO pin32 */
    HAL_GPIO_33 = 33,                     /**< GPIO pin33 */
    HAL_GPIO_34 = 34,                     /**< GPIO pin34 */
    HAL_GPIO_35 = 35,                     /**< GPIO pin35 */
    HAL_GPIO_36 = 36,                     /**< GPIO pin36 */
    HAL_GPIO_37 = 37,                     /**< GPIO pin37 */
    HAL_GPIO_38 = 38,                     /**< GPIO pin38 */
    HAL_GPIO_39 = 39,                     /**< GPIO pin39 */

    HAL_GPIO_57 = 57,                     /**< GPIO pin57 */
    HAL_GPIO_58 = 58,                     /**< GPIO pin58 */
    HAL_GPIO_59 = 59,                     /**< GPIO pin59 */
    HAL_GPIO_60 = 60,                     /**< GPIO pin60 */
    HAL_GPIO_MAX                          /**< GPIO pin max number\<invalid\> */
} hal_gpio_pin_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_ADC_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup ADC
* @{
*
* @addtogroup hal_adc_enum Enum
* @{
*/

/*****************************************************************************
* adc
*****************************************************************************/
/** @brief adc channel */
typedef enum {
    HAL_ADC_CHANNEL_0 = 0,                   /**< ADC channel 0 */
    HAL_ADC_CHANNEL_1 = 1,                   /**< ADC channel 1 */
    HAL_ADC_CHANNEL_2 = 2,                   /**< ADC channel 2 */
    HAL_ADC_CHANNEL_3 = 3,                   /**< ADC channel 3 */
    HAL_ADC_CHANNEL_MAX                      /**< ADC max channel \<invalid\> */
} hal_adc_channel_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif



#ifdef HAL_I2S_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup I2S
* @{
*
* @addtogroup hal_i2s_enum Enum
* @{
*/

/*****************************************************************************
* i2s
*****************************************************************************/
/** @brief This enum defines initial type of I2S.
 */

typedef enum {
    HAL_I2S_TYPE_EXTERNAL_MODE          = 0,        /**< i2s external mode */
    HAL_I2S_TYPE_EXTERNAL_TDM_MODE      = 1,        /**< i2s external tdm mode\<invalid\> */
    HAL_I2S_TYPE_INTERNAL_MODE          = 2,	    /**< i2s internal mode\<invalid\> */
    HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE = 3         /**< i2s internal loopback mode (For i2s 16bit only)*/
} hal_i2s_initial_type_t;

/** @brief I2S event */
typedef enum {
    HAL_I2S_EVENT_DATA_REQUEST        =  0, /**<  This value means request user to fill data. */
    HAL_I2S_EVENT_DATA_NOTIFICATION   =  1, /**<  This value means notify user the RX data is ready. */
    HAL_I2S_EVENT_END                 =  2  /**<  No more data from user.*/
} hal_i2s_event_t;

/** @brief I2S sample rate definition */
typedef enum {
    HAL_I2S_SAMPLE_RATE_8K        = 0,  /**<  8000Hz  */
    HAL_I2S_SAMPLE_RATE_12K       = 1,  /**<  12000Hz */
    HAL_I2S_SAMPLE_RATE_16K       = 2,  /**<  16000Hz */
    HAL_I2S_SAMPLE_RATE_24K       = 3,  /**<  24000Hz */
    HAL_I2S_SAMPLE_RATE_32K       = 4,  /**<  32000Hz */
    HAL_I2S_SAMPLE_RATE_48K       = 5,  /**<  48000Hz */
} hal_i2s_sample_rate_t;

/** @brief I2S sample widths.  */
typedef enum {
    HAL_I2S_SAMPLE_WIDTH_8BIT  = 0,   /**< I2S sample width is 8bit. (Invalid)*/
    HAL_I2S_SAMPLE_WIDTH_16BIT = 1,   /**< I2S sample width is 16bit. */
    HAL_I2S_SAMPLE_WIDTH_24BIT = 2,   /**< I2S sample width is 24bit. (Invalid)*/
    HAL_I2S_SAMPLE_WIDTH_32BIT = 3    /**< I2S sample width is 32bit. */
} hal_i2s_sample_width_t;

/** @brief Number of bits per frame sync(FS). This parameter determines the bits of a complete sample of both left and right channels.*/
typedef enum {
    HAL_I2S_FRAME_SYNC_WIDTH_32  = 0,   /**< 32 bits per frame. */
    HAL_I2S_FRAME_SYNC_WIDTH_64  = 1,   /**< 64 bits per frame. (Only for I2S 32bit mode) */
    HAL_I2S_FRAME_SYNC_WIDTH_128  = 2   /**< 128 bits per frame.  (Invalid)*/
} hal_i2s_frame_sync_width_t;

/** @brief Enable or disable right channel of the I2S TX to send the same data as on the left channel of the I2S TX. */
typedef enum {
    HAL_I2S_TX_MONO_DUPLICATE_DISABLE = 0,  /**< Keep data to its channel. */
    HAL_I2S_TX_MONO_DUPLICATE_ENABLE  = 1   /**< Right channel of the TX, sends the same data as the left.  */
} hal_i2s_tx_mode_t;

/** @brief Enable or disable twice the downsampling rate mode in the I2S RX link.
                 In this mode the sampling rate of the I2S TX is 48kHz while the sampling rate of the I2S RX is 24kHz. The I2S RX automatically drops 1 sample in each 2 input samples received. */
typedef enum {
    HAL_I2S_RX_DOWN_RATE_DISABLE = 0,  /**< Actual sampling rate of the I2S RX = sampling rate. */
    HAL_I2S_RX_DOWN_RATE_ENABLE  = 1   /**< Actual sampling rate of the I2S RX is half of the original sampling rate.(Invalid)*/
} hal_i2s_rx_down_rate_t;

/** @brief Enable or disable data swapping between right and left channels of the I2S link. */
typedef enum {
    HAL_I2S_LR_SWAP_DISABLE = 0,  /**< Disable the data swapping. */
    HAL_I2S_LR_SWAP_ENABLE  = 1   /**< Enable the data swapping.  */
} hal_i2s_lr_swap_t;

/** @brief Enable or disable word select clock inverting of the I2S link. */
typedef enum {
    HAL_I2S_WORD_SELECT_INVERSE_DISABLE = 0,  /**< Disable the word select clock inverting. */
    HAL_I2S_WORD_SELECT_INVERSE_ENABLE  = 1   /**< Enable the word select clock inverting.  */
} hal_i2s_word_select_inverse_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SPI_MASTER_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_MASTER
 * @{
 * @defgroup hal_spi_master_define Define
 * @{
 */

/** @brief  The maximum polling mode transaction size.
 *  There is no limitation for the maximum transaction size
 */
#define HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE  0xFFFFFFFF

/**
 * @}
 */

/**
 * @addtogroup hal_spi_master_enum Enum
 * @{
 */

/*****************************************************************************
* spi master
*****************************************************************************/
/** @brief This enum defines the SPI master port.
 *  This chip total has 1 SPI master port
 */
typedef enum {
    HAL_SPI_MASTER_0 = 0,                             /**< spi master port 0 */
    HAL_SPI_MASTER_MAX                                /**< spi master max port number\<invalid\> */
} hal_spi_master_port_t;


/** @brief selection of spi slave device connected to which cs pin of spi master */
typedef enum {
    HAL_SPI_MASTER_SLAVE_0 = 0,                       /**< spi slave device connect to spi master cs0 pin */
    HAL_SPI_MASTER_SLAVE_1 = 1,                       /**< spi slave device connect to spi master cs1 pin */
    HAL_SPI_MASTER_SLAVE_MAX                          /**< spi master max cs pin number\<invalid\> */
} hal_spi_master_slave_port_t;

/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_SLAVE
 * @{
 * @addtogroup hal_spi_slave_enum Enum
 * @{
 */

/*****************************************************************************
* spi slave
*****************************************************************************/
/** @brief This enum defines the SPI slave port. This chip just support one
 *  SPI slave port.
 */
typedef enum {
    HAL_SPI_SLAVE_0 = 0,                             /**< spi slave port 0 */
    HAL_SPI_SLAVE_MAX                                /**< spi slave max port number\<invalid\> */
} hal_spi_slave_port_t;

/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_RTC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup RTC
 * @{
 */

/*****************************************************************************
* rtc
*****************************************************************************/
/* NULL */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_EINT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 * @addtogroup hal_eint_enum Enum
 * @{
 */

/*****************************************************************************
* eint
*****************************************************************************/
/** @brief eint pin number */
typedef enum {
    HAL_EINT_NUMBER_0   = 0,
    HAL_EINT_NUMBER_1   = 1,
    HAL_EINT_NUMBER_2   = 2,
    HAL_EINT_NUMBER_3   = 3,
    HAL_EINT_NUMBER_4   = 4,
    HAL_EINT_NUMBER_5   = 5,
    HAL_EINT_NUMBER_6   = 6,
    HAL_EINT_NUMBER_19  = 19,
    HAL_EINT_NUMBER_20  = 20,
    HAL_EINT_NUMBER_21  = 21,
    HAL_EINT_NUMBER_22  = 22,
    HAL_WIC_NUMBER_0    = 32,
    HAL_WIC_NUMBER_1    = 33,
    HAL_WIC_NUMBER_2    = 34,
    HAL_WIC_NUMBER_3    = 35,
    HAL_WIC_NUMBER_4    = 36,
    HAL_WIC_NUMBER_5    = 37,
    HAL_WIC_NUMBER_6    = 38,
    HAL_WIC_NUMBER_7    = 39,
    HAL_WIC_NUMBER_8    = 40,
    HAL_WIC_NUMBER_9    = 41,
    HAL_WIC_NUMBER_10   = 42,
    HAL_WIC_NUMBER_11   = 43,
    HAL_EINT_NUMBER_MAX
} hal_eint_number_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GPT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * @addtogroup hal_gpt_enum Enum
 * @{
 */

/*****************************************************************************
* gpt
*****************************************************************************/
/** @brief GPT port */
typedef enum {
    HAL_GPT_0 = 0,                         /**< GPT port0: Used for tickless timer. The clock source is 32Khz.*/
    HAL_GPT_1 = 1,                         /**< GPT port1: User defined. The clock source is 32Khz*/
    HAL_GPT_2 = 2,                         /**< GPT port2: Use to set a millisecond delay and get 1/32Khz free count. The clock source is 32768Hz. */
    HAL_GPT_4 = 4,                         /**< GPT port4: Use to set a microsecond delay and get microsecond free count. The clock source is bus_clock. */
    HAL_GPT_MAX                            /**< GPT max port \<invalid\> */
} hal_gpt_port_t;

/** @brief GPT clock source */
typedef enum {
    HAL_GPT_CLOCK_SOURCE_32K = 0,           /**< 1 tick = 1/32768 second */
        
    HAL_GPT_CLOCK_SOURCE_1M  = 1,           /**< 1 tick = 1 micro second, when using this clock source, please do not use #hal_gpt_get_free_run_count() to caculate duaration count.*/
                                            /**< Instead, calculate the duration count by yourself, and notice that the rollback_value =  0xffffffff/(systemclock / 1000000)*/
    
    HAL_GPT_CLOCK_SOURCE_BUS = 2            /**< 1 tick = 1/systemclock second. User could use the api top_mcu_freq_get() defined in top.h to get the system clock*/
                                            /**< time_us = count / (systemclock / 1000000)*/                                     
} hal_gpt_clock_source_t;

/** @brief  The maximum time of millisecond timer.
  */
#define HAL_GPT_MAXIMUM_MS_TIMER_TIME    (130150523)

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif


#ifdef HAL_GDMA_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GDMA
 * @{
 * @addtogroup hal_gdma_enum Enum
 * @{
 */
/*****************************************************************************
* gdma
*****************************************************************************/
/** @brief gdma channel */
typedef enum {
    HAL_GDMA_CHANNEL_0 = 0,                        /**< gdma channel 0 */
    HAL_GDMA_CHANNEL_1 = 1,                        /**< gdma channel 1 */
    HAL_GDMA_CHANNEL_MAX                           /**< gdma max channel \<invalid\> */
} hal_gdma_channel_t;

/**
  * @}
  */

#ifdef  HAL_GDMA_WRAP_FLASH_ADDRESS_TO_VIRTUAL_ADDRESS
/** @brief  the gdma access flash address should be 0x3000000 instead of 0x1000000.
 */

/** Description gdma wrap flash address offset */
#define HAL_GDMA_WRAP_FLASH_ADDRESS_OFFSET          (28)

/** Description gdma wrap flash address high byte mask */
#define HAL_GDMA_WRAP_FLASH_ADDRESS_HIGH_BYTE_MASK  (0xfUL<<HAL_GDMA_WRAP_FLASH_ADDRESS_OFFSET)

/** Description gdma wrap flash address mask */
#define HAL_GDMA_WRAP_FLASH_ADDRESS_MASK            (0x1UL<<HAL_GDMA_WRAP_FLASH_ADDRESS_OFFSET)

/** Description gdma wrap flash virtual address mask */
#define HAL_GDMA_WRAP_FLASH_VIRTUAL_ADDRESS_MASK    (0x3UL<<HAL_GDMA_WRAP_FLASH_ADDRESS_OFFSET)

/**
 * @}
 */
#endif

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_PWM_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 * @addtogroup hal_pwm_enum Enum
 * @{
 */
/*****************************************************************************
* pwm
*****************************************************************************/
/** @brief pwm channel */
typedef enum {
    HAL_PWM_0  = 0,                        /**< pwm channel0 */
    HAL_PWM_1  = 1,                        /**< pwm channel1 */
    HAL_PWM_2  = 2,                        /**< pwm channel2 */
    HAL_PWM_3  = 3,                        /**< pwm channel3 */
    HAL_PWM_4  = 4,                        /**< pwm channel4 */
    HAL_PWM_5  = 5,                        /**< pwm channel5 */
    HAL_PWM_18 = 18,                       /**< pwm channel18 */
    HAL_PWM_19 = 19,                       /**< pwm channel19 */
    HAL_PWM_20 = 20,                       /**< pwm channel20 */
    HAL_PWM_21 = 21,                       /**< pwm channel21 */
    HAL_PWM_22 = 22,                       /**< pwm channel22 */
    HAL_PWM_23 = 23,                       /**< pwm channel23 */
    HAL_PWM_24 = 24,                       /**< pwm channel24 */
    HAL_PWM_25 = 25,                       /**< pwm channel25 */
    HAL_PWM_26 = 26,                       /**< pwm channel26 */
    HAL_PWM_27 = 27,                       /**< pwm channel27 */
    HAL_PWM_28 = 28,                       /**< pwm channel28 */
    HAL_PWM_29 = 29,                       /**< pwm channel29 */
    HAL_PWM_30 = 30,                       /**< pwm channel30 */
    HAL_PWM_31 = 31,                       /**< pwm channel31 */
    HAL_PWM_32 = 32,                       /**< pwm channel32 */
    HAL_PWM_33 = 33,                       /**< pwm channel33 */
    HAL_PWM_34 = 34,                       /**< pwm channel34 */
    HAL_PWM_35 = 35,                       /**< pwm channel35 */
    HAL_PWM_36 = 36,                       /**< pwm channel36 */
    HAL_PWM_37 = 37,                       /**< pwm channel37 */
    HAL_PWM_38 = 38,                       /**< pwm channel38 */
    HAL_PWM_39 = 39,                       /**< pwm channel39 */
    HAL_PWM_MAX                            /**< pwm max channel \<invalid\>*/
} hal_pwm_channel_t;

/** @brief pwm clock source seletion */
typedef enum {
    HAL_PWM_CLOCK_32KHZ  = 0,               /**< pwm clock source: Embedded 32KHz clock */
    HAL_PWM_CLOCK_2MHZ   = 1,               /**< pwm clock srouce: Embedded 2MHz  clock */
    HAL_PWM_CLOCK_20MHZ  = 2,               /**< pwm clock srouce: External 20MHz clock */
    HAL_PWM_CLOCK_26MHZ  = 3,               /**< pwm clock srouce: External 26MHz clock */
    HAL_PWM_CLOCK_40MHZ  = 4,               /**< pwm clock srouce: External 40MHz clock */
    HAL_PWM_CLOCK_52MHZ  = 5,               /**< pwm clock srouce: External 52MHz clock */
    HAL_PWM_CLOCK_MAX                       /**< pwm max clock \<invalid\>*/
} hal_pwm_source_clock_t ;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif



#ifdef HAL_GPC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPC
 * @{
 * @addtogroup hal_gpc_enum Enum
 * @{
 */
/** @brief gpc port */
typedef enum {
    HAL_GPC_0 = 0,                          /**< gpc port0 */
    HAL_GPC_MAX_PORT                        /**< gpc max port */
} hal_gpc_port_t;


/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif



#ifdef HAL_SLEEP_MANAGER_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SLEEP_MANAGER
 * @{
 * @addtogroup hal_sleep_manager_enum Enum
 * @{
 */
/*****************************************************************************
 * Enum
 *****************************************************************************/
/** @brief Sleep modes */
typedef enum {
    HAL_SLEEP_MODE_NONE = 0,        /**< no sleep */
    HAL_SLEEP_MODE_IDLE,            /**< idle state */
    HAL_SLEEP_MODE_SLEEP,           /**< wfi sleep state */
    HAL_SLEEP_MODE_LEGACY_SLEEP,    /**< legacy sleep state */
    HAL_SLEEP_MODE_DEEP_SLEEP,      /**< deep sleep */
    HAL_SLEEP_MODE_NUMBER           /**< for support range detection */
} hal_sleep_mode_t;
/**
 * @}
 * @}
 * @}
 */
#endif


#ifdef __cplusplus
}
#endif

#endif /* __HAL_PLATFORM_H__ */


