/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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

#ifndef _USB_HOST_HUB_H_
#define _USB_HSOT_HUB_H_

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief USB host HUB maximum port count */
#define USB_HOST_HUB_MAX_PORT (7U)

/*! @brief HUB class code */
#define USB_HOST_HUB_CLASS_CODE (9U)
/*! @brief HUB sub-class code */
#define USB_HOST_HUB_SUBCLASS_CODE_NONE (0U)

/* HUB and PORT status according to Table 11-17 in chapter 11.*/
/*! @brief Local Power Status Change: This field indicates that a change has occurred in the HUB's Local Power Source */
#define C_HUB_LOCAL_POWER (0U)
/*! @brief Over-Current Change: This field indicates if a change has occurred in the Over-Current field*/
#define C_HUB_OVER_CURRENT (1U)
/*! @brief Current Connect Status: This field reflects whether or not a device is currently connected to this port*/
#define PORT_CONNECTION (0U)
/*! @brief Port Enabled/Disabled: Ports can be enabled by the USB System Software only. Ports
can be disabled by either a fault condition (disconnect event or other fault condition) or by the USB System
Software*/
#define PORT_ENABLE (1U)
/*! @brief Suspend: This field indicates whether or not the device on this port is suspended */
#define PORT_SUSPEND (2U)
/*! @brief this field indicate that the current drain on the port exceeds the specified maximum. */
#define PORT_OVER_CURRENT (3U)
/*! @brief This field is set when the host wishes to reset the attached device */
#define PORT_RESET (4U)
/*! @brief This field reflects a port's logical, power control state */
#define PORT_POWER (8U)
/*! @brief Low- Speed Device Attached: This is relevant only if a device is attached */
#define PORT_LOW_SPEED (9U)
/*! @brief High-speed Device Attached: This is relevant only if a device is attached */
#define PORT_HIGH_SPEED (10U)
/*! @brief Connect Status Change: Indicates a change has occurred in the port's Current Connect Status */
#define C_PORT_CONNECTION (16U)
/*! @brief Port Enable/Disable Change: This field is set to one when a port is disabled because of a Port_Error
 * condition */
#define C_PORT_ENABLE (17U)
/*! @brief Suspend Change: This field indicates a change in the host-visible suspend state of the attached device */
#define C_PORT_SUSPEND (18U)
/*! @brief Over-Current Indicator Change: This field applies only to HUBs that report over-current conditions on a
 * per-port basis */
#define C_PORT_OVER_CURRENT (19U)
/*! @brief Reset Change: This field is set when reset processing on this port is complete */
#define C_PORT_RESET (20U)

/*! @brief Get HUB think time value */
#define USB_HOST_HUB_DESCRIPTOR_CHARACTERISTICS_THINK_TIME_MASK (0x60U)
/*! @brief Get HUB think time value */
#define USB_HOST_HUB_DESCRIPTOR_CHARACTERISTICS_THINK_TIME_SHIFT (5U)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*! @brief HUB descriptor structure */
typedef struct _usb_host_hub_descriptor
{
    uint8_t blength;                /*!< Number of bytes in this descriptor*/
    uint8_t bdescriptortype;        /*!< Descriptor Type*/
    uint8_t bnrports;               /*!< Number of downstream facing ports that this HUB supports*/
    uint8_t whubcharacteristics[2]; /*!< HUB characteristics please reference to Table 11-13 in usb2.0 specification*/
    uint8_t bpwron2pwrgood;   /*!< Time (in 2 ms intervals) from the time the power-on sequence begins on a port until
                                 power is good on that port.*/
    uint8_t bhubcontrcurrent; /*!< Maximum current requirements of the HUB Controller electronics in mA*/
    uint8_t deviceremovable;  /*!< Indicates if a port has a removable device attached*/
} usb_host_hub_descriptor_t;

/*! @brief HUB port instance structure */
typedef struct _usb_host_hub_port_instance
{
    usb_device_handle deviceHandle; /*!< Device handle*/
    uint8_t portStatus;             /*!< Port running status*/
    uint8_t resetCount;             /*!< Port reset time*/
    uint8_t speed;                  /*!< Port's device speed*/
} usb_host_hub_port_instance_t;

/*! @brief HUB instance structure */
typedef struct _usb_host_hub_instance
{
    struct _usb_host_hub_instance *next;       /*!< Next HUB instance*/
    usb_host_handle hostHandle;                /*!< Host handle*/
    usb_device_handle deviceHandle;            /*!< Device handle*/
    usb_host_interface_handle interfaceHandle; /*!< Interface handle*/
    usb_host_pipe_handle controlPipe;          /*!< Control pipe handle*/
    usb_host_pipe_handle interruptPipe;        /*!< HUB interrupt in pipe handle*/
    usb_host_hub_port_instance_t *portList;    /*!< HUB's port instance list*/
    usb_host_transfer_t *controlTransfer;      /*!< Control transfer in progress*/
    transfer_callback_t inCallbackFn;          /*!< Interrupt in callback*/
    void *inCallbackParam;                     /*!< Interrupt in callback parameter*/
    transfer_callback_t controlCallbackFn;     /*!< Control callback*/
    void *controlCallbackParam;                /*!< Control callback parameter*/
                                               /*   HUB property */
    uint16_t totalThinktime;                   /*!< HUB total think time*/
    uint8_t hubLevel;                          /*!< HUB level, the root HUB's level is 1*/

    /* HUB application parameter */
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
    uint8_t *hubDescriptor;    /*!< HUB descriptor buffer*/
    uint8_t *hubBitmapBuffer;  /*!< HUB receiving bitmap data buffer*/
    uint8_t *hubStatusBuffer;  /*!< HUB status buffer*/
    uint8_t *portStatusBuffer; /*!< Port status buffer*/
#else
    uint8_t hubDescriptor[7 + (USB_HOST_HUB_MAX_PORT >> 3) + 1]; /*!< HUB descriptor buffer*/
    uint8_t hubBitmapBuffer[(USB_HOST_HUB_MAX_PORT >> 3) + 1];   /*!< HUB receiving bitmap data buffer*/
    uint8_t hubStatusBuffer[4];                                  /*!< HUB status buffer*/
    uint8_t portStatusBuffer[4];                                 /*!< Port status buffer*/
#endif
    uint8_t hubStatus;                                           /*!< HUB instance running status*/
    uint8_t portCount;                                           /*!< HUB port count*/
    uint8_t portIndex;                                           /*!< Record the index when processing ports in turn*/
    uint8_t portProcess;                                         /*!< The port that is processing*/
    uint8_t primeStatus;                                         /*!< Data prime transfer status*/
    uint8_t invalid; /*!< 0/1, when invalid, cannot send transfer to the class*/
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    uint8_t supportRemoteWakeup; /*!< The HUB supports remote wakeup or not*/
    uint8_t controlRetry;        /*!< Retry count for set remote wakeup feature*/
#endif
} usb_host_hub_instance_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Initializes the HUB instance.
 *
 * This function allocates the resource for HUB instance.
 *
 * @param deviceHandle  The device handle.
 * @param classHandle   Return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostHubInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief Sets interface.
 *
 * This function binds the interfaces with the HUB instance.
 *
 * @param classHandle      The class handle.
 * @param interfaceHandle  The interface handle.
 * @param alternateSetting The alternate setting value.
 * @param callbackFn       This callback is called after this function completes.
 * @param callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Open pipe fail. See the USB_HostOpenPipe.
 *                                    Or send transfer fail. See the USB_HostSendSetup,
 */
extern usb_status_t USB_HostHubSetInterface(usb_host_class_handle classHandle,
                                            usb_host_interface_handle interfaceHandle,
                                            uint8_t alternateSetting,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Deinitializes the HUB instance.
 *
 * This function releases the resource for HUB instance.
 *
 * @param deviceHandle   The device handle.
 * @param classHandle    The class handle.
 *
 * @retval kStatus_USB_Success        The device is deinitialized successfully.
 */
extern usb_status_t USB_HostHubDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

/*!
 * @brief Receives data.
 *
 * This function implements the HUB receiving data.
 *
 * @param classHandle   The class handle.
 * @param buffer        The buffer pointer.
 * @param bufferLength  The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostRecv.
 */
extern usb_status_t USB_HostHubInterruptRecv(usb_host_class_handle classHandle,
                                             uint8_t *buffer,
                                             uint16_t bufferLength,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);

/*!
 * @brief Port reset setup.
 *
 * This function sends the HUB port reset transfer.
 *
 * @param classHandle   The class handle.
 * @param portNumber    Port number.
 *
 * @retval kStatus_USB_Success        Send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHubSendPortReset(usb_host_class_handle classHandle, uint8_t portNumber);

/*!
 * @brief HUB get descriptor.
 *
 * This function implements get HUB descriptor-specific request.
 *
 * @param classHandle   The class handle.
 * @param buffer        The buffer pointer.
 * @param bufferLength  The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHubGetDescriptor(usb_host_class_handle classHandle,
                                             uint8_t *buffer,
                                             uint16_t bufferLength,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);

/*!
 * @brief HUB clear feature.
 *
 * This function implements clear HUB feature specific request.
 *
 * @param classHandle   the class handle.
 * @param buffer        the buffer pointer.
 * @param bufferLength  the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSendSetup.
 */
extern usb_status_t USB_HostHubClearFeature(usb_host_class_handle classHandle,
                                            uint8_t feature,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief HUB get status.
 *
 * This function implements the get HUB status-specific request.
 *
 * @param classHandle   The class handle.
 * @param buffer        The buffer pointer.
 * @param bufferLength  The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHubGetStatus(usb_host_class_handle classHandle,
                                         uint8_t *buffer,
                                         uint16_t bufferLength,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*!
 * @brief HUB set feature.
 *
 * This function implements the set HUB feature-specific request.
 *
 * @param classHandle   The class handle.
 * @param buffer        The buffer pointer.
 * @param bufferLength  The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHubSetPortFeature(usb_host_class_handle classHandle,
                                              uint8_t portNumber,
                                              uint8_t feature,
                                              transfer_callback_t callbackFn,
                                              void *callbackParam);

/*!
 * @brief HUB clear port feature.
 *
 * This function implements the clear HUB port feature-specific request.
 *
 * @param classHandle   The class handle.
 * @param buffer        The buffer pointer.
 * @param bufferLength  The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHubClearPortFeature(usb_host_class_handle classHandle,
                                                uint8_t portNumber,
                                                uint8_t feature,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam);

/*!
 * @brief HUB port get status.
 *
 * This function implements the get HUB port status-specific request.
 *
 * @param classHandle   The class handle.
 * @param buffer        The buffer pointer.
 * @param bufferLength  The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHubGetPortStatus(usb_host_class_handle classHandle,
                                             uint8_t portNumber,
                                             uint8_t *buffer,
                                             uint16_t bufferLength,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);

#ifdef __cplusplus
}
#endif

#endif /* USB_HOST_CONFIG_HUB */

#endif /* _USB_HSOT_HUB_H_ */
