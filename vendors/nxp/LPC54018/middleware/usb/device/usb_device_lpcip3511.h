/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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

#ifndef __USB_DEVICE_LPC3511IP_H__
#define __USB_DEVICE_LPC3511IP_H__

#include "fsl_device_registers.h"

/*!
 * @addtogroup usb_device_controller_lpcip3511_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief The reserved buffer size, the buffer is for the memory copy if the application transfer buffer is
     ((not 64 bytes alignment) || (not in the same 64K ram) || (HS && OUT && not multiple of 4)) */
#define USB_DEVICE_IP3511_ENDPOINT_RESERVED_BUFFER_SIZE (5 * 1024)
/*! @brief Use one bit to represent one reserved 64 bytes to allocate the buffer by uint of 64 bytes. */
#define USB_DEVICE_IP3511_BITS_FOR_RESERVED_BUFFER ((USB_DEVICE_IP3511_ENDPOINT_RESERVED_BUFFER_SIZE + 63) / 64)
/*! @brief How many IPs support the reserved buffer */
#define USB_DEVICE_IP3511_RESERVED_BUFFER_FOR_COPY (USB_DEVICE_CONFIG_LPCIP3511FS + USB_DEVICE_CONFIG_LPCIP3511HS)
/*! @brief Prime all the double endpoint buffer at the same time, if the transfer length is larger than max packet size.
 */
#define USB_DEVICE_IP3511_DOUBLE_BUFFER_ENABLE (1u)
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511HS)) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
#define USB_LPC3511IP_Type USBHSD_Type
#define USB_DEVICE_IP3511_ENDPOINTS_NUM FSL_FEATURE_USBHSD_EP_NUM
#else
#define USB_LPC3511IP_Type USB_Type
#define USB_DEVICE_IP3511_ENDPOINTS_NUM FSL_FEATURE_USB_EP_NUM
#endif

/*! @brief Endpoint state structure */
typedef struct _usb_device_lpc3511ip_endpoint_state_struct
{
    uint8_t *transferBuffer;       /*!< Address of buffer containing the data to be transmitted */
    uint32_t transferLength;       /*!< Length of data to transmit. */
    uint32_t transferDone;         /*!< The data length has been transferred*/
    uint32_t transferPrimedLength; /*!< it may larger than transferLength, because the primed length may larger than the
                                      transaction length. */
    uint8_t *epPacketBuffer;       /*!< The max packet buffer for copying*/
    union
    {
        uint32_t state; /*!< The state of the endpoint */
        struct
        {
            uint32_t maxPacketSize : 11U; /*!< The maximum packet size of the endpoint */
            uint32_t stalled : 1U;        /*!< The endpoint is stalled or not */
            uint32_t transferring : 1U;   /*!< The endpoint is transferring */
            uint32_t zlt : 1U;            /*!< zlt flag */
            uint32_t stallPrimed : 1U;
            uint32_t epPacketCopyed : 1U;   /*!< whether use the copy buffer */
            uint32_t epControlDefault : 5u; /*!< The EP command/status 26~30 bits */
            uint32_t doubleBufferBusy : 2U; /*!< How many buffers are primed, for control endpoint it is not used */
            uint32_t producerOdd : 1U;      /*!< When priming one transaction, prime to this endpoint buffer */
            uint32_t consumerOdd : 1U;      /*!< When transaction is done, read result from this endpoint buffer */
            uint32_t endpointType : 2U;
            uint32_t reserved1 : 5U;
        } stateBitField;
    } stateUnion;
    union
    {
        uint16_t epBufferStatus;
        /* If double buff is disable, only epBufferStatusUnion[0] is used;
           For control endpoint, only epBufferStatusUnion[0] is used. */
        struct
        {
            uint16_t transactionLength : 11U;
            uint16_t epPacketCopyed : 1U;
        } epBufferStatusField;
    } epBufferStatusUnion[2];
} usb_device_lpc3511ip_endpoint_state_struct_t;

/*! @brief LPC USB controller (IP3511) state structure */
typedef struct _usb_device_lpc3511ip_state_struct
{
    /*!< control data buffer, must align with 64 */
    uint8_t *controlData;
    /*!< 8 bytes' setup data, must align with 64 */
    uint8_t *setupData;
    /*!< 4 bytes for zero length transaction, must align with 64 */
    uint8_t *zeroTransactionData;
    /* Endpoint state structures */
    usb_device_lpc3511ip_endpoint_state_struct_t endpointState[(USB_DEVICE_IP3511_ENDPOINTS_NUM * 2)];
    usb_device_handle deviceHandle; /*!< (4 bytes) Device handle used to identify the device object belongs to */
    USB_LPC3511IP_Type *registerBase;         /*!< (4 bytes) ip base address */
    volatile uint32_t *epCommandStatusList; /* endpoint list */
    uint8_t controllerId;                   /*!< Controller ID */
    uint8_t isResetting;                    /*!< Is doing device reset or not */
    uint8_t deviceSpeed;                    /*!< some controller support the HS */
#if ((defined(USB_DEVICE_IP3511_RESERVED_BUFFER_FOR_COPY)) && (USB_DEVICE_IP3511_RESERVED_BUFFER_FOR_COPY > 0U))
    uint8_t *epReservedBuffer;
    uint8_t epReservedBufferBits[(USB_DEVICE_IP3511_BITS_FOR_RESERVED_BUFFER + 7) / 8];
#endif
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511HS)) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
    uint8_t controllerSpeed;
#endif
} usb_device_lpc3511ip_state_struct_t;

/*!
 * @name USB device controller (IP3511) functions
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the USB device controller instance.
 *
 * This function initializes the USB device controller module specified by the controllerId.
 *
 * @param[in] controllerId      The controller ID of the USB IP. See the enumeration type usb_controller_index_t.
 * @param[in] handle            Pointer of the device handle used to identify the device object belongs to.
 * @param[out] controllerHandle An out parameter used to return the pointer of the device controller handle to the
 * caller.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceLpc3511IpInit(uint8_t controllerId,
                                     usb_device_handle handle,
                                     usb_device_controller_handle *controllerHandle);

/*!
 * @brief Deinitializes the USB device controller instance.
 *
 * This function deinitializes the USB device controller module.
 *
 * @param[in] controllerHandle   Pointer of the device controller handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceLpc3511IpDeinit(usb_device_controller_handle controllerHandle);

/*!
 * @brief Sends data through a specified endpoint.
 *
 * This function sends data through a specified endpoint.
 *
 * @param[in] controllerHandle Pointer of the device controller handle.
 * @param[in] endpointAddress  Endpoint index.
 * @param[in] buffer           The memory address to hold the data need to be sent.
 * @param[in] length           The data length need to be sent.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value indicates whether the sending request is successful or not. The transfer completion is
 * notified by the
 * corresponding callback function.
 * Currently, only one transfer request can be supported for a specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for a specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer can begin only when the previous transfer is done (a notification is obtained through the
 * endpoint
 * callback).
 */
usb_status_t USB_DeviceLpc3511IpSend(usb_device_controller_handle controllerHandle,
                                     uint8_t endpointAddress,
                                     uint8_t *buffer,
                                     uint32_t length);

/*!
 * @brief Receives data through a specified endpoint.
 *
 * This function receives data through a specified endpoint.
 *
 * @param[in] controllerHandle Pointer of the device controller handle.
 * @param[in] endpointAddress  Endpoint index.
 * @param[in] buffer           The memory address to save the received data.
 * @param[in] length           The data length to be received.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value indicates whether the receiving request is successful or not. The transfer completion is
 * notified by the
 * corresponding callback function.
 * Currently, only one transfer request can be supported for a specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for a specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer can begin only when the previous transfer is done (a notification is obtained through the
 * endpoint
 * callback).
 */
usb_status_t USB_DeviceLpc3511IpRecv(usb_device_controller_handle controllerHandle,
                                     uint8_t endpointAddress,
                                     uint8_t *buffer,
                                     uint32_t length);

/*!
 * @brief Cancels the pending transfer in a specified endpoint.
 *
 * The function is used to cancel the pending transfer in a specified endpoint.
 *
 * @param[in] controllerHandle  ointer of the device controller handle.
 * @param[in] ep                Endpoint address, bit7 is the direction of endpoint, 1U - IN, abd 0U - OUT.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceLpc3511IpCancel(usb_device_controller_handle controllerHandle, uint8_t ep);

/*!
 * @brief Controls the status of the selected item.
 *
 * The function is used to control the status of the selected item.
 *
 * @param[in] controllerHandle      Pointer of the device controller handle.
 * @param[in] type             The selected item. Please refer to enumeration type usb_device_control_type_t.
 * @param[in,out] param            The parameter type is determined by the selected item.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceLpc3511IpControl(usb_device_controller_handle controllerHandle,
                                        usb_device_control_type_t type,
                                        void *param);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* __USB_DEVICE_LPC3511IP_H__ */
