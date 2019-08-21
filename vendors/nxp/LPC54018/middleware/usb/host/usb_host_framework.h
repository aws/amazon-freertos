/*
 * The Clear BSD License
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _USB_HOST_CH9_H_
#define _USB_HOST_CH9_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup usb_host_drv
 * @{
 */

/*! @brief Request type */
typedef enum _usb_host_request_type
{
    kRequestDevice = 1U, /*!< Control request object is device */
    kRequestInterface,   /*!< Control request object is interface */
    kRequestEndpoint,    /*!< Control request object is endpoint */
} usb_host_request_type_t;

/*! @brief For USB_REQUEST_STANDARD_CLEAR_FEATURE and USB_REQUEST_STANDARD_SET_FEATURE */
typedef struct _usb_host_process_feature_param
{
    uint8_t requestType;         /*!< See the #usb_host_request_type_t */
    uint8_t featureSelector;     /*!< Set/cleared feature */
    uint8_t interfaceOrEndpoint; /*!< Interface or end pointer */
} usb_host_process_feature_param_t;

/*! @brief For USB_REQUEST_STANDARD_GET_DESCRIPTOR and USB_REQUEST_STANDARD_SET_DESCRIPTOR */
typedef struct _usb_host_process_descriptor_param
{
    uint8_t descriptorType;    /*!< See the usb_spec.h, such as the USB_DESCRIPTOR_TYPE_DEVICE */
    uint8_t descriptorIndex;   /*!< The descriptor index is used to select a specific descriptor (only for configuration
                                  and string descriptors) when several descriptors of the same type are implemented in a
                                  device */
    uint8_t languageId;        /*!< It specifies the language ID for string descriptors or is reset to zero for other
                                  descriptors */
    uint8_t *descriptorBuffer; /*!< Buffer pointer */
    uint16_t descriptorLength; /*!< Buffer data length */
} usb_host_process_descriptor_param_t;

/*! @brief For USB_REQUEST_STANDARD_GET_INTERFACE */
typedef struct _usb_host_get_interface_param
{
    uint8_t interface;                 /*!< Interface number */
    uint8_t *alternateInterfaceBuffer; /*!< Save the transfer result */
} usb_host_get_interface_param_t;

/*! @brief For USB_REQUEST_STANDARD_GET_STATUS */
typedef struct _usb_host_get_status_param
{
    uint16_t statusSelector; /*!< Interface number, the end pointer number or OTG status selector */
    uint8_t requestType;     /*!< See the #usb_host_request_type_t */
    uint8_t *statusBuffer;   /*!< Save the transfer result */
} usb_host_get_status_param_t;

/*! @brief For USB_REQUEST_STANDARD_SET_INTERFACE */
typedef struct _usb_host_set_interface_param
{
    uint8_t alternateSetting; /*!< Alternate setting value */
    uint8_t interface;        /*!< Interface number */
} usb_host_set_interface_param_t;

/*! @brief For USB_REQUEST_STANDARD_SYNCH_FRAME */
typedef struct _usb_host_synch_frame_param
{
    uint8_t endpoint;           /*!< Endpoint number */
    uint8_t *frameNumberBuffer; /*!< Frame number data buffer */
} usb_host_synch_frame_param_t;

/*! @}*/

/*******************************************************************************
 * API
 ******************************************************************************/

#endif /* _USB_HOST_CH9_H_ */
