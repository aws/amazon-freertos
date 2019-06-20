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

#ifndef __HAL_USB_H__
#define __HAL_USB_H__

#include "hal_platform.h"

#ifdef HAL_USB_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup USB
 * @{
 * This section introduces the USB APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, enumerations, structures and functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b USB                        | Universal Serial Bus. USB was designed to standardize the connection of computer peripherals (including keyboards, pointing devices, digital cameras, printers, portable media players, disk drives and network adapters) to personal computers, both to communicate and to supply electric power. It has become commonplace on other devices, such as smart phones, PDAs and video game consoles.[3] USB has effectively replaced a variety of earlier interfaces, such as serial and parallel ports, as well as separate power chargers for portable devices. For more information, please refer to <a href="https://en.wikipedia.org/wiki/USB"> an introduction to the USB in Wikipedia</a>.|
 * |\b PIO                        | Programmed Input / Output. It is a transmission mode to use CPU to move data. |
 * @}
 * @}
 */

/**
* @addtogroup HAL
* @{
* @addtogroup USB
* @{
*
* @section HAL_USB_Features_Chapter Supported features.
*
* - \b USB \b 2.0 \b High-Speed \b Device \b Controller \n
*   In High-Speed mode, the USB controller is configured for supporting two endpoints to receive packets and four endpoints to
*   send packets except for endpoint 0.
*   These endpoints can be individually configured in the software to handle bulk transfers,
*   interrupt transfers or isochronous transfers. There are four DMA channels and the embedded RAM size is 2,304 bytes.
*   The embedded RAM can be dynamically configured for each endpoint.
* @}
* @}
*/

/**
* @addtogroup HAL
* @{
* @addtogroup USB
* @{
*
* @section HAL_USB_Architechture_Chapter Software architecture of the USB driver.
*
* The software architecture of the USB driver is shown in the diagram below.
* @image html hal_usb_archi.png
* @}
* @}
*/

/**
* @addtogroup HAL
* @{
* @addtogroup USB
* @{
*
* @section HAL_USB_Driver_Usage_Chapter How to use this driver.
*
* - Initialize the USB hardware. \n
*  - Step1: Call #hal_usb_init() to initialize the USB hardware if it has not been initialized after powering on.
*  - Step2: Call #hal_usb_configure_driver() to set up the HAL driver and register the callback functions.
*  - Step3: Call #hal_usb_reset_hardware() and #hal_usb_reset_fifo() to soft reset the hardware.
*  - Step4: Call #hal_usb_pull_up_dp_line() to pull USB D+ to 3.3v and start USB connection.
*  - sample code:
*    @code
*    usb_hw_init(void)
*    {
*        //USB hardware initialization.
*        hal_usb_init();
*
*        //Register reset and ep0 interrupt handlers for the driver settings.
*        hal_usb_configure_driver();
*        hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_RESET, 0, USB_Reset);
*        hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP0, 0, USB_Endpoint0_Hdlr);
*        hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_SUSPEND, 0, USB_Suspend);
*
*        //USB hardware soft reset.
*        hal_usb_reset_hardware();
*        //USB FIFO data reset.
*        hal_usb_reset_fifo();
*        //Set D+ to high.
*        hal_usb_pull_up_dp_line();
*    }
*    @endcode
*
* - De-initialize the USB hardware. \n
*  - Step1: Call #hal_usb_deinit() to power the USB hardware off.
*    @code
*    usb_hw_deinit(void)
*    {
*        //Power off the USB hardware.
*        hal_usb_deinit();
*    }
*    @endcode
*
* - Set up the USB endpoint. \n
*  - Step1: Call #hal_usb_init_tx_endpoint() to initialize the basic settings for the endpoint.
*  - Step2: Call #hal_usb_enable_tx_endpoint() to enable the endpoint interrupt.
*    @code
*    usb_ep_init(void)
*    {
*        int ep_num = 1;
*        int wMaxPacketSize = 512;
*        bool double_fifo = false;
*        bool flush_fifo = true;
*
*        //Initialize enpoint as BULK.
*        hal_usb_init_tx_endpoint(ep_num, wMaxPacketSize, HAL_USB_ENDPT_BULK, double_fifo);
*        hal_usb_init_rx_endpoint(ep_num, wMaxPacketSize, HAL_USB_ENDPT_BULK, double_fifo);
*        hal_usb_enable_tx_endpoint(ep_num, HAL_USB_ENDPT_BULK, HAL_USB_EP_USE_NO_DMA, flush_fifo);
*        hal_usb_enable_rx_endpoint(ep_num, HAL_USB_ENDPT_BULK, HAL_USB_EP_USE_NO_DMA, flush_fifo);
*    }
*    @endcode
*
* - USB PIO data read. \n
*  - Step1: Call #hal_usb_get_rx_packet_length() to get the data length in the USB FIFO.
*  - Step2: Call #hal_usb_read_endpoint_fifo() to read the USB FIFO data and save in a buffer.
*  - Step3: Call #hal_usb_set_endpoint_rx_ready() to notify that the RX is ready to receive data packet.
*    @code
*    usb_fifo_read(void)
*    {
*       int nCount;
*       int ep_num = 1;
*       char data_buffer[512];
*
*       //Read data count in FIFO.
*       nCount = hal_usb_get_rx_packet_length(ep_num);
*
*       //Read data from FIFO to data_buffer.
*       hal_usb_read_endpoint_fifo(ep_num, nCount, data_buffer);
*
*       //Notify the USB hardware that the RX is ready to receive the data packet.
*       hal_usb_set_endpoint_rx_ready(ep_num);
*    }
*    @endcode
*
* - USB PIO data write. \n
*  - Step1: Call #hal_usb_write_endpoint_fifo() to write data to the USB FIFO.
*  - Step2: Call #hal_usb_set_endpoint_tx_ready() to notify that the data packet is ready to send.
*    @code
*    #define DATA_LENGTH 10
*    usb_fifo_write(void)
*    {
*       int nCount = DATA_LENGTH;
*       int ep_num = 1;
*       char data_buffer[DATA_LENGTH];
*
*       //Write data to USB FIFO.
*       hal_usb_write_endpoint_fifo(ep_num, nCount, data_buffer);
*
*       //Notify the USB hardware that the data packet is ready to send.
*       hal_usb_set_endpoint_tx_ready(ep_num);
*    }
*    @endcode
*
* - USB Endpoint0 Handler. \n
*  - Step1: Call #hal_usb_read_endpoint_fifo() to read USB FIFO data and save in a buffer.
*  - Step2: Check parameter usb_cmd.bRequest
*           Call #hal_usb_set_address() to set the USB Address data
*           or call #hal_usb_set_endpoint_stall() to set/clear the endpoint stall.
*    @code
*    void USB_Endpoint0_Handler (void)
*    {
*       bool stall = false;
*       //Read ep0 data.
*       hal_usb_read_endpoint_fifo(0, 8, &usb_cmd);
*       if ((usb_cmd.bmRequestType & USB_CMD_TYPEMASK) == USB_CMD_STDREQ)
*       {
*           switch(usb_cmd.bRequest)
*           {
*            case USB_SET_ADDRESS:
*                 hal_usb_set_address(usb_cmd.FAddr, HAL_USB_SET_ADDR_DATA);
*                 break;
*            case USB_SET_FEATURE:
*                 if(usb_cmd.wValue) == USB_FTR_EPHALT)
*                 {
*                     stall = true;
*                     if(pcmd->wIndex&0x80)
*                        hal_usb_set_endpoint_stall((usb_cmd.wIndex& 0x0f), HAL_USB_EP_TX_DIR, stall);
*                     else
*                        hal_usb_set_endpoint_stall((usb_cmd.wIndex& 0x0f), HAL_USB_EP_RX_DIR, stall);
*                 }
*                 break;
*            case USB_CLEAR_FEATURE:
*                 if((usb_cmd.wValue) == USB_FTR_EPHALT))
*                 {
*                     if(pcmd->wIndex&0x80)
*                        hal_usb_set_endpoint_stall((usb_cmd.wIndex& 0x0f), HAL_USB_EP_TX_DIR, stall);
*                     else
*                        hal_usb_set_endpoint_stall((usb_cmd.wIndex& 0x0f), HAL_USB_EP_RX_DIR, stall);
*                 }
*                 break;
*           }
*       }
*    }
*    @endcode
*
* - USB DMA Usage. \n
*  - Step1: Call #hal_usb_get_dma_channel() to get a DMA channel.
*  - Step2: Call #hal_usb_start_dma_channel() to start the DMA.
*  - Step3: Call #hal_usb_stop_dma_channel() to stop the DMA.
*  - Step4: Call #hal_usb_release_dma_channel() to free the DMA channel.
*    @code
*    void USB_DMA_Example(void)
*    {
*        int ep_tx_num = 1;
*        int ep_rx_num = 1;
*        int ep_num = 0;
*        int nCount= hal_usb_get_rx_packet_length(ep_num);
*        uint8_t data[512];
*        bool tx_rx_use_the_same_dma_channel = false;
*        bool clear_dma_running_by_myself = false;
*        hal_usb_dma_handler_t callback_fun = NULL;
*
*        //============================================
*        //Allocate DMA channels and configure the DMA.
*        //============================================
*        //Get TX channel.
*        hal_usb_get_dma_channel(ep_tx_num, NULL, HAL_USB_EP_TX_DIR, tx_rx_use_the_same_dma_channel);
*        //Get RX channel.
*        hal_usb_get_dma_channel(NULL, ep_rx_num, HAL_USB_EP_RX_DIR, tx_rx_use_the_same_dma_channel);
*        //Setup the DMA.
*        hal_usb_start_dma_channel(ep_num, HAL_USB_EP_TX_DIR, HAL_USB_ENDPT_BULK, data, nCount, callback_fun, clear_dma_running_by_myself, HAL_USB_DMA1_TYPE);
*
*
*        //Do something if you need
*        //.............
*
*
*        //====================================
*        //Query the DMA status.
*        //====================================
*        if (hal_usb_is_dma_running())
*             printf("USB DMA is in progress.\n");
*        else
*             printf("USB DMA is complete.\n");
*
*
*
*
*        //Do something if you need
*        //.............
*
*
*        //==================================
*        //Stop and release DMA channels.
*        //==================================
*        //Stop the DMA.
*        hal_usb_stop_dma_channel(ep_num, HAL_USB_EP_TX_DIR);
*        //Free the TX channel.
*        hal_usb_release_dma_channel(ep_tx_num, NULL, HAL_USB_EP_TX_DIR, tx_rx_use_the_same_dma_channel);
*        //Free the RX channel.
*        hal_usb_release_dma_channel(NULL, ep_rx_num, HAL_USB_EP_RX_DIR, tx_rx_use_the_same_dma_channel);
*    }
*    @endcode
*/

#ifdef __cplusplus
extern "C" {
#endif
/** @defgroup hal_usb_define Define
  * @{
  */
/** @brief  The maximum number of USB endpoints.*/
#define HAL_USB_MAX_NUMBER_ENDPOINT		4
/** @brief  The maximum number of USB TX endpoints.*/
#define HAL_USB_MAX_NUMBER_ENDPOINT_TX		4
/** @brief  The maximum number of USB RX endpoints.*/
#define HAL_USB_MAX_NUMBER_ENDPOINT_RX		2
/** @brief  The maximum number of USB DMA channels.*/
#define HAL_USB_MAX_NUMBER_DMA			4
/** @brief  The maximum packet size used in the control pipe, the same for both high-speed and full-speed.*/
#define HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0			64
/** @brief  The maximum packet size for high-speed bulk endpoints.*/
#define HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED	512
/** @brief  The maximum packet size for full-speed bulk endpoints. */
#define HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED	64
/** @brief  The maximum packet size for high-speed isochronous endpoints.*/
#define HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED	1024
/** @brief  The maximum packet size for full-speed isochronous endpoints.*/
#define HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_FULL_SPEED	 	  512
/** @brief  The maximum packet size for high-speed interrupt endpoints.*/
#define HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_HIGH_SPEED	 64
/** @brief  The maximum packet size for full-speed interrupt endpoints.*/
#define HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_FULL_SPEED	64
/**
  * @}
  */


/** @defgroup hal_usb_typedefs Typedefs.
  * @{
  */
/** @brief Interrupt handler function type.
  * It is used in #hal_usb_register_driver_callback(). For more details, please refer to how to use this driver section.
  */
typedef void (*hal_usb_driver_interrupt_handler_t)(void);
/** @brief Application DMA callback function type.
  * It is a callback registered inside #hal_usb_start_dma_channel(). The callback is called once the USB DMA is complete.
  */
typedef void (*hal_usb_dma_handler_t)(void);
/**
  * @}
  */


/** @defgroup hal_usb_enum Enum.
  * @{
  */
/** @brief USB status. */
typedef enum {
    HAL_USB_STATUS_INVALID_PARAMETER = -2,  /**<  An invalid parameter was given. */
    HAL_USB_STATUS_ERROR             = -1,  /**<  The function call failed. */
    HAL_USB_STATUS_OK                =  0   /**<  The function call was successful. */
} hal_usb_status_t;

/** @brief USB power state.
  */
typedef enum {
    HAL_USB_POWER_STATE_NORMAL = 0,   /**<  The maximum current consumption cannot exceed 500mA in HAL_USB_POWER_STATE_NORMAL state. */
    HAL_USB_POWER_STATE_SUSPEND       /**<  The maximum current consumption cannot exceed 2.5mA in HAL_USB_POWER_STATE_SUSPEND state. */
} hal_usb_power_state_t;


/** @brief USB DMA usage type. */
typedef enum {
    HAL_USB_DMA0_TYPE = 0, /**<  Single packet operation. */
    HAL_USB_DMA1_TYPE,     /**<  Multiple packets operation. */
} hal_usb_dma_type_t;

/** @brief USB DMA usage. */
typedef enum {
    HAL_USB_EP_USE_NO_DMA = 0,  /**<  Endpoint use with PIO mode. */
    HAL_USB_EP_USE_ONLY_DMA,    /**<  Endpoint use with DMA mode. */
} hal_usb_dma_usage_t;

/** @brief USB Test Mode. Used in the USB compliance test. */
typedef enum {
    HAL_USB_TEST_MODE_TYPE_NONE = 0,  /**<  NOT in test mode. */
    HAL_USB_TEST_MODE_TYPE_J,         /**<  The Test_J test mode. In this mode, the USB2.0 controller transmits a continuous J on the bus. */
    HAL_USB_TEST_MODE_TYPE_K,         /**<  The Test_K test mode. In this mode, the USB2.0 controller transmits a continuous K on the bus. */
    HAL_USB_TEST_MODE_TYPE_SE0_NAK,   /**<  The Test_SE0_NAK test mode. In this mode, the USB2.0 controller remains in High-speed mode but responds to any valid IN token with a NAK.*/
    HAL_USB_TEST_MODE_TYPE_PACKET     /**<  The Test_Packet test mode. In this mode, the USB2.0 controller repetitively transmits on the bus a 53-byte test packet, in the form which is defined in the Universal Serial Bus Specification Revision 2.0, Section 7.1.20. Note: The test packet has a fixed format and must be loaded into the Endpoint 0 FIFO before the test mode is entered. USB2.0 IP only*/
} hal_usb_test_mode_type_t;

/** @brief This is the direction of USB endpoint. The direction is based on the USB device's view. */
typedef enum {
    HAL_USB_EP_DIRECTION_TX = 0,    /**<  PC IN, such as BULK_IN is HAL_USB_EP_TX_DIR*/
    HAL_USB_EP_DIRECTION_RX         /**<  PC OUT, such as BULK_OUT is HAL_USB_EP_RX_DIR*/
} hal_usb_endpoint_direction_t;

/** @brief Endpoint type defined in USB 2.0 Specification. CTRL, BULK, INTR, ISO endpoint */
typedef enum {
    HAL_USB_EP_TRANSFER_CTRL = 0,   /**<  The transfer type defined in USB 2.0 Specification, control pipe. */
    HAL_USB_EP_TRANSFER_BULK,       /**<  The transfer type defined in USB 2.0 Specification, bulk pipe. */
    HAL_USB_EP_TRANSFER_INTR,       /**<  The transfer type defined in USB 2.0 Specification, interrupt pipe. */
    HAL_USB_EP_TRANSFER_ISO         /**<  The transfer type defined in USB 2.0 Specification, isochronous pipe. */
} hal_usb_endpoint_transfer_type_t;


/** @brief Endpoint state. */
typedef enum {
    HAL_USB_EP_STATE_DISABLE = 0,   /**< The endpoint is disabled. */
    HAL_USB_EP_STATE_FIFO,          /**< The endpoint operates in the PIO mode. */
    HAL_USB_EP_STATE_DMA            /**< The endpoint operates in the DMA mode. */
} hal_usb_endpoint_state_t;

/** @brief Endpoint 0 hardware control state. */
typedef enum {
    HAL_USB_EP0_DRV_STATE_READ_END = 0,      /**< Control pipe data is received. The hardware will acknowledge this data.*/
    HAL_USB_EP0_DRV_STATE_WRITE_RDY,         /**< Data is sent to the USB FIFO.*/
    HAL_USB_EP0_DRV_STATE_TRANSACTION_END,   /**< The contol pipe transaction is complete.*/
    HAL_USB_EP0_DRV_STATE_CLEAR_SENT_STALL   /**< Clear contol pipe stall status.*/
} hal_usb_endpoint_0_driver_state_t;


/** @brief Interrupt handler type. */
typedef enum {
    HAL_USB_DRV_HDLR_RESET = 0,  /**< USB bus reset callback function. */
    HAL_USB_DRV_HDLR_SUSPEND,    /**< USB bus suspended callback function. */
    HAL_USB_DRV_HDLR_RESUME,     /**< USB bus resumed callback function. */
    HAL_USB_DRV_HDLR_EP0,        /**< USB endpoint 0 callback function. */
    HAL_USB_DRV_HDLR_EP_TX,      /**< USB endpoint n TX callback function. */
    HAL_USB_DRV_HDLR_EP_RX       /**< USB endpoint n RX callback function. */
} hal_usb_driver_handler_type_t;

/** @brief The set address action may be different for different hardware.
 * This is the current state for the set address function.
 */
typedef enum {
    HAL_USB_SET_ADDR_DATA = 0,/**< Control pipe received a SET_ADDRESS data.*/
    HAL_USB_SET_ADDR_STATUS   /**< Control pipe acknowledged the SET_ADDRESS command status.*/
} hal_usb_set_address_state_t;
/**
  * @}
  */




/**
 * @brief This function initializes the USB hardware module.
 * Call this function before using the USB service.
 * It also enables the USB related clock tree and power source.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "Initialize the USB hardware" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_init(void);
/**
 * @brief This function initializes the USB HAL driver.
 * This function will initialize all required parameters, callback function and endpoint information in USB HAL driver.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "Initial USB hardware" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_configure_driver(void);

/**
 * @brief This function registers a callback function.
 * @param[in] type is the interrupt handler type. Please refer to #hal_usb_driver_handler_type_t.
 * @param[in] ep_num is the endpoint number. The interrupt handler must be binding to a specific endpoint number if it uses #HAL_USB_DRV_HDLR_EP_TX and #HAL_USB_DRV_HDLR_EP_RX.
 * @param[in] hdlr is the callback function. Please refer to #hal_usb_driver_interrupt_handler_t.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "Initialize the USB hardware" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_register_driver_callback(hal_usb_driver_handler_type_t type, uint32_t ep_num, hal_usb_driver_interrupt_handler_t hdlr);
/**
 * @brief This function pulls the USB DP pin to high.
 * It starts the USB connection flow. For more details on DP, please refer to the USB 2.0 Specification.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "Initialize the USB hardware" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_pull_up_dp_line(void);
/**
 * @brief This function applies the USB hardware soft reset.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "Initialize the USB hardware" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_reset_hardware(void);
/**
 * @brief This function applies USB FIFO address reset at all endpoints.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "Initialize the USB hardware" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_reset_fifo(void);
/**
 * @brief This function releases the USB hardware resource.
 * This function deinitializes the USB hardware, sets the USB hardware in low power mode and releases the USB related clock tree and power source.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "De-initialize the USB hardware" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_deinit(void);
/**
 * @brief This function configures the USB address.
 * In the USB 2.0 Specifications, the USB address is set by the USB control pipe. The address is 1~127(7-bits).
 * This address should be written with the address received through a SET_ADDRESS command,
 * which will then be used for decoding the function address in subsequent token packets.
 * @param[in] addr the USB address set by the USB control pipe through a SET_ADDRESS command.
 * @param[in] state the current state for the set address function. The address writes to hardware only in HAL_USB_SET_ADDR_STATUS state.
 * It must make sure the SET_ADDRESS command is completed.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "USB Endpoint0 Handler" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_set_address(uint8_t addr, hal_usb_set_address_state_t state);
/**
 * @brief This function initializes the USB endpoint in TX direction.
 * @param[in] ep_num is the endpoint to initialize.
 * @param[in] data_size is the maximum packet size at this endpoint. For more details about maximum packet size, please refer to USB 2.0 Specification.
 * @param[in] type is the endpoint type defined in USB 2.0 Specification, such as CTRL, BULK, INTR, ISO endpoint.
 * @param[in] double_fifo two FIFO buffers are allocated, if double_fifo is enabled.
 * It may cost more FIFO SRAM.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "USB endpoint setup" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_init_tx_endpoint(uint32_t ep_num, uint16_t data_size, hal_usb_endpoint_transfer_type_t type, bool double_fifo);
/**
 * @brief This function initialize the USB endpoint in RX direction.
 * @param[in] ep_num is the endpoint to initialize.
 * @param[in] data_size is the maximum packet size at this endpoint. For more details about maximum packet size, please refer to USB 2.0 Specification.
 * @param[in] type is the endpoint type defined in USB 2.0 Specification, such as CTRL, BULK, INTR, ISO endpoint.
 * @param[in] double_fifo allocates two FIFO buffers, if double_fifo is enabled.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "USB endpoint setup" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_init_rx_endpoint(uint32_t ep_num, uint16_t data_size, hal_usb_endpoint_transfer_type_t type, bool double_fifo);
/**
 * @brief This function enables the USB endpoint TX interrupt.
 * @param[in] ep_num is the endpoint to initialize.
 * @param[in] ep_type is the endpoint type defined in USB 2.0 Specification, such as CTRL, BULK, INTR, ISO endpoint.
 * @param[in] dma_usage_type is set to PIO or DMA mode.
 * @param[in] is_flush flushes the USB FIFO data when the function is executed.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "USB endpoint setup" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_enable_tx_endpoint(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, hal_usb_dma_usage_t dma_usage_type, bool is_flush);
/**
 * @brief This function enables the USB endpoint RX interrupt.
 * @param[in] ep_num is the endpoint to initialize.
 * @param[in] ep_type is the endpoint type defined in USB 2.0 Specification. Such as CTRL, BULK, INTR, ISO endpoint.
 * @param[in] dma_usage_type is set to PIO or DMA mode.
 * @param[in] is_flush flushes the USB FIFO data when the function is executed.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Please refer to "USB endpoint setup" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_enable_rx_endpoint(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, hal_usb_dma_usage_t dma_usage_type, bool is_flush);
/**
 * @brief This function disables the USB endpoint TX interrupt.
 * @param[in] ep_num is the endpoint number to disable.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code:
 * @code
 * void USB_endpoint_disable (void)
 * {
 *   int ep_num = 1;
 *	 hal_usb_disable_tx_endpoint(ep_num);
 * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_disable_tx_endpoint(uint32_t ep_num);
/**
 * @brief This function disables the USB endpoint RX interrupt.
 * @param[in] ep_num is the endpoint number to disable.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code:
 * @code
 * void USB_endpoint_disable (void)
 * {
 *   int ep_num = 1;
 *	 hal_usb_disable_rx_endpoint(ep_num);
 * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_disable_rx_endpoint(uint32_t ep_num);
/**
 * @brief This function clears the TX endpoint data toggle.
 * It's to force the endpoint data toggle to switch and the data packet to be cleared from the FIFO, regardless if the acknowledgement was received.
 * @param[in] ep_num is the endpoint number to clear data toggle.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code:
 * @code
 * void USB_endpoint_disable (void)
 * {
 *   int ep_num = 1;
 *	 hal_usb_clear_tx_endpoint_data_toggle(ep_num);
 * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_clear_tx_endpoint_data_toggle(uint32_t ep_num);
/**
 * @brief This function clears the TX endpoint data toggle.
 * It's to force the endpoint data toggle to switch and the data packet to be cleared from the FIFO, regardless if the acknowledgement was received.
 * @param[in] ep_num is the endpoint number to clear data toggle.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code:
 * @code
 * void USB_endpoint_disable (void)
 * {
 *   int ep_num = 1;
 *   hal_usb_clear_rx_endpoint_data_toggle(ep_num);
 * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_clear_rx_endpoint_data_toggle(uint32_t ep_num);
/**
 * @brief This function reads the USB data from FIFO.
 * Reading from these addresses unloads data from the RX FIFO for the corresponding endpoint.
 * @param[in] ep_num is the endpoint number.
 * @param[in] nBytes is the number of bytes to read.
 * @param[in] pDst is a pointer to the destination.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code please refers to "USB PIO data read." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_read_endpoint_fifo(uint32_t ep_num, uint16_t nBytes, void *pDst);
/**
 * @brief This function writes the USB data to FIFO.
 * Writing to these addresses loads data into the TX FIFO for the corresponding endpoint.
 * @param[in] ep_num is the endpoint number.
 * @param[in] nBytes is the number of bytes to write.
 * @param[in] pSrc is a pointer to the source.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code please refers to "USB PIO data write." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_write_endpoint_fifo(uint32_t ep_num, uint16_t nBytes, void *pSrc);
/**
 * @brief This function sets the USB endpoint n to stall, flush or clear data toggle.
 * This function is used to execute (1) endpoint stall, (2) FIFO flush and (3) clear data toggle.
 * @param[in] ep_num is the endpoint number.
 * @param[in] direction is the USB endpoint direction.
 * @param[in] stall_enable enables or disables the stall operation.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code please refers to "USB Endpoint0 Handler" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_set_endpoint_stall(uint32_t ep_num, hal_usb_endpoint_direction_t direction, bool stall_enable);
/**
 * @brief This function queries the stall status of the USB endpoint n.
 * It is usually used to response to the host endpoint 0 command: GET_STATUS
 * @param[in] ep_num is the endpoint number.
 * @param[in] direction is the USB endpoint direction.
 * @return True, if stall is enabled.
 * @par Example
 * Sample code please refers to "USB Endpoint0 Handler" in @ref HAL_USB_Driver_Usage_Chapter.
 * @code
 * void USB_Endpoint0_Hdlr(void)
 * {
 *   int ep_num = 1;
 *   bool status;
 *
 *   status = (uint16_t)hal_usb_get_endpoint_stall_status(ep_num, HAL_USB_EP_TX_DIR);
 * }
 * @endcode
 */
extern bool hal_usb_get_endpoint_stall_status(uint32_t ep_num, hal_usb_endpoint_direction_t direction);
/**
 * @brief This function gets the USB control pipe hardware status.
 * p_transaction_end: This bit is set when a control transaction ends before the DataEnd bit is set.
 * An interrupt will be generated and the FIFO flushed.
 * p_sent_stall: This bit is set when a STALL handshake is transmitted.
 * @param[out] p_transaction_end is the result of control pipe transaction end.
 * @param[out] p_sent_stall is the result of control pipe sent stall.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code:
 * @code
 * void USB_Endpoint0_Hdlr(void)
 * {
 *    bool b_transaction_end;
 *    bool b_sent_stall;
 *
 *    hal_usb_get_endpoint_0_status(&b_transaction_end, &b_sent_stall);
 * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_get_endpoint_0_status(bool *p_transaction_end, bool *p_sent_stall);
/**
 * @brief This function sets the USB control pipe hardware status.
 * @param[in] state is the endpoint0 hardware control state. Please refer to #hal_usb_endpoint_0_driver_state_t.
 * @param[in] stall is set to true to execute endpoint stall.
 * @param[in] end is set to true to execute transaction end.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code:
 * @code
 * // It shows 3 examples in this function call.
 * void USB_Endpoint0_State(void)
 * {
 *    bool transaction_end;
 *    bool sent_stall;
 *
 *    //example1: read data end and stall this transaction.
 *    sent_stall = true;
 *    transaction_end = false;
 *    hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, sent_stall, transaction_end);
 *
 *    //example2: write data ready and set this transaction completed.
 *    sent_stall = false;
 *    transaction_end = true;
 *    hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_WRITE_RDY, sent_stall, transaction_end);
 *
 *    //example3: transaction end and clear the transaction_end hardware status.
 *    sent_stall = false;
 *    transaction_end = true;
 *    hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_TRANSACTION_END, sent_stall, transaction_end);
 * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_update_endpoint_0_state(hal_usb_endpoint_0_driver_state_t state, bool stall, bool end);
/**
 * @brief This function gets the data packet length at the RX endpoint.
 * @param[in] ep_num is the endpoint number.
 * @return the length of packet size in bytes.
 * @par Example
 * For an example implementation, please refer to "USB PIO data read" in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern uint32_t hal_usb_get_rx_packet_length(uint32_t ep_num);
/**
 * @brief This function sets the USB packet ready once the data is fully written into FIFO.
 * Hardware will send the packet out if data is written to the FIFO and the TX packet is set to ready.
 * Null length packet (ZLP) will be sent if the data is not written to the FIFO.
 * @param[in] ep_num is the endpoint number.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code please refers to "USB PIO data write." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_set_endpoint_tx_ready(uint32_t ep_num);
/**
 * @brief This function sets the USB packet ready once the data is fully read from FIFO.
 * Hardware issues an interrupt if RX endpoint received a packet.
 * Hardware will start to receive the next packet if data is read from FIFO and RX packet is set to ready.
 * @param[in] ep_num is the endpoint number.
 * @return#HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * For an example implementation, please refer to "USB PIO data read." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_set_endpoint_rx_ready(uint32_t ep_num);
/**
 * @brief This function flushes data packet in the TX FIFO and clears data toggle by request.
 * @param[in] ep_num is the endpoint number.
 * @param[in] ep_type is the endpoint type defined in USB 2.0 Specification, such as CTRL, BULK, INTR, ISO endpoint.
 * @param[in] b_reset_toggle is set to true to execute clear data toggle.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code:
 * @code
 * void usb_fifo_flush(void)
 * {
 *        int ep_num = 1;
 *        bool clear_data_toggle = false;
 *
 *        hal_usb_clear_tx_endpoint_fifo(ep_num, HAL_USB_ENDPT_BULK, clear_data_toggle);
 * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_clear_tx_endpoint_fifo(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, bool b_reset_toggle);
/**
 * @brief This function flushes data packet in RX FIFO and clears the data toggle by request.
 * @param[in] ep_num is the endpoint number.
 * @param[in] ep_type is the endpoint type defined in USB 2.0 Specification, such as CTRL, BULK, INTR, ISO endpoint.
 * @param[in] b_reset_toggle is set to true to execute clear data toggle.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
  * @par Example
 * Sample code:
 * @code
 * void usb_fifo_flush(void)
 * {
 *        int ep_num = 1;
 *        bool clear_data_toggle = false;
 *
 *        hal_usb_clear_rx_endpoint_fifo(ep_num, HAL_USB_ENDPT_BULK, clear_data_toggle);
 * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_clear_rx_endpoint_fifo(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, bool b_reset_toggle);
/**
 * @brief This function allocates a USB DMA channel
 * For more information on the number of DMA channels on the LinkIt platform for RTOS, please refer to #HAL_USB_MAX_NUMBER_ENDPOINT.
 * @param[in] ep_tx_num is the endpoint number in TX direction.
 * @param[in] ep_rx_num is the endpoint number in RX direction.
 * @param[in] direction is the USB endpoint direction. This parameter is ignored, if the same channel is used.
 * @param[in] same_chan the TX and RX use the same DMA channel.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * An example implementation can be found in "USB DMA Usage." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_get_dma_channel(uint32_t ep_tx_num, uint32_t ep_rx_num, hal_usb_endpoint_direction_t direction, bool same_chan);
/**
 * @brief This function frees a USB DMA channel resource.
 * For more information on the number of DMA channels on the LinkIt platform for RTOS, please refer to #HAL_USB_MAX_NUMBER_ENDPOINT.
 * @param[in] ep_tx_num is the endpoint number in TX direction.
 * @param[in] ep_rx_num is the endpoint number in RX direction.
 * @param[in] direction is the USB endpoint direction. This parameter is ignored, if the same channel is used.
 * @param[in] same_chan the TX and RX use the same DMA channel.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * An example implementation can be found in "USB DMA Usage." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_release_dma_channel(uint32_t ep_tx_num, uint32_t ep_rx_num, hal_usb_endpoint_direction_t direction, bool same_chan);
/**
 * @brief This function sets up the USB DMA channel to transfer data.
 * @param[in] ep_num is the endpoint number.
 * @param[in] direction is the USB endpoint direction.
 * @param[in] ep_type is the endpoint type defined in USB 2.0 Specification. Such as CTRL, BULK, INTR, ISO endpoint.
 * @param[in] addr is a pointer to the address to apply the DMA transaction.
 * @param[in] length is the data length in bytes.
 * @param[in] callback is called once the DMA transaction is complete.
 * @param[in] callback_upd_run configures the DMA running state. Set true, if application callback function requests to clear the DMA running by itself.
 * @param[in] dma_type executes a single packet operation or multiple packets operation, #hal_usb_dma_type_t.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * An example implementation can be found in "USB DMA Usage." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_start_dma_channel(uint32_t ep_num, hal_usb_endpoint_direction_t direction, hal_usb_endpoint_transfer_type_t ep_type, void *addr, uint32_t length, hal_usb_dma_handler_t callback, bool callback_upd_run, hal_usb_dma_type_t dma_type);
/**
 * @brief This function stops the USB DMA channel and clears pending DMA interrupts.
 * After that, drop data in FIFO.
 * @param[in] ep_num is the endpoint number.
 * @param[in] direction is the USB endpoint direction.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * An example implementation can be found in "USB DMA Usage." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern hal_usb_status_t hal_usb_stop_dma_channel(uint32_t ep_num, hal_usb_endpoint_direction_t direction);
/**
 * @brief This function queries the DMA execution status.
 * @param[in] ep_num is the endpoint number.
 * @param[in] direction is the USB endpoint direction.
 * @return True, if the DMA is running.
 * @par Example
 * An example implementation can be found in "USB DMA Usage." in @ref HAL_USB_Driver_Usage_Chapter.
 */
extern bool hal_usb_is_dma_running(uint32_t ep_num, hal_usb_endpoint_direction_t direction);
/**
 * @brief This function queries current USB speed.
 * @return True, if using high-speed USB.
 * @par Example
 * Sample code:
 * @code
 * void get_usb_speed(void)
 * {
 *        if (hal_usb_is_high_speed())
 *           printf("USB is high speed mode\n");
 *        else
 *           printf("USB is full speed mode\n");
 * }
 * @endcode
 */
extern bool hal_usb_is_high_speed(void);
/**
 * @brief This function checks if the TX endpoint FIFO is empty or not.
 * @param[in] ep_num is the endpoint number.
 * @return False, if there is data in FIFO pending to send.
 * @par Example
 * Sample code:
 * @code
 * void query_fifo_status(void)
 * {
 *        int ep_num = 1;
 *
 *        if (hal_usb_is_endpoint_tx_empty(ep_num))
 *           printf("USB tx FIFO is empty\n");
 *        else
 *           printf("USB tx FIFO still has data\n");
 * }
 * @endcode
 */
extern bool hal_usb_is_endpoint_tx_empty(uint32_t ep_num);
/**
 * @brief This function checks if the RX endpoint FIFO is empty or not.
 * @param[in] ep_num is the endpoint number.
 * @return False, if there is data in FIFO pending to read.
 * @par Example
 * Sample code:
 * @code
 * void query_fifo_status(void)
 * {
 *        int ep_num = 1;
 *
 *        if (hal_usb_is_endpoint_rx_empty(ep_num))
 *           printf("USB rx FIFO is empty\n");
 *        else
 *           printf("USB rx FIFO still has data\n");
 * }
 * @endcode
 */
extern bool hal_usb_is_endpoint_rx_empty(uint32_t ep_num);
/**
 * @brief This function sets the USB hardware into test mode.
 * To activate a test mode, the USB 2.0 Specification defines the SET_FEATURE command as the desired
 * interface. The USB-IF offers a free High Speed electrical Test Tool (HSET) for Microsoft Windows OS,
 * to activate the various test modes and operations
 * @param[in] test_selector select the test function.
 * @return #HAL_USB_STATUS_OK, if the operation completed successfully.
 * @par Example
 * Sample code:
 * @code
 * void usb_test_mode(void)
 * {
 *        hal_usb_enter_test_mode(HAL_USB_TEST_MODE_TYPE_J);
 *        hal_usb_enter_test_mode(HAL_USB_TEST_MODE_TYPE_K);
 *        hal_usb_enter_test_mode(HAL_USB_TEST_MODE_TYPE_SE0_NAK);
 *        hal_usb_enter_test_mode(HAL_USB_TEST_MODE_TYPE_PACKET);
  * }
 * @endcode
 */
extern hal_usb_status_t hal_usb_enter_test_mode(hal_usb_test_mode_type_t test_selector);


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /* HAL_USB_MODULE_ENABLED */
#endif /* __HAL_USB_H__ */

