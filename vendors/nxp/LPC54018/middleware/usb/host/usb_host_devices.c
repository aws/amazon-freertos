/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
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

#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_hci.h"
#include "usb_host_devices.h"

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
#include "usb_host_hub.h"
#endif /* USB_HOST_CONFIG_HUB */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief enumeration transfer callback function.
 *
 * @param param      callback parameter.
 * @param transfer   the transfer.
 * @param status     transfer result status.
 */
static void USB_HostEnumerationTransferCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief process the new step state.
 *
 * @param deviceInstance    device instance pointer.
 *
 * @return kStatus_USB_Success or error codes
 */
static usb_status_t USB_HostProcessState(usb_host_device_instance_t *deviceInstance);

/*!
 * @brief process the previous step transfer result.
 *
 * @param deviceInstance    device instance pointer.
 *
 * @return kStatus_USB_Success or error codes
 */
static usb_status_t USB_HostProcessCallback(usb_host_device_instance_t *deviceInstance);

/*!
 * @brief notify the application event, the callback is registered when initializing host.
 *
 * @param deviceInstance   device instance pointer.
 * @param eventCode        event code.
 *
 * @return kStatus_USB_Success or error codes
 */
static usb_status_t USB_HostNotifyDevice(usb_host_device_instance_t *deviceInstance, uint32_t eventCode);

/*!
 * @brief allocate one address.
 *
 * @param hostInstance    host instance pointer.
 *
 * @return address, 0 is invalid.
 */
static uint8_t USB_HostAllocateDeviceAddress(usb_host_instance_t *hostInstance);

/*!
 * @brief release one address.
 *
 * @param hostInstance    host instance pointer.
 * @param address     releasing address.
 */
static void USB_HostReleaseDeviceAddress(usb_host_instance_t *hostInstance, uint8_t address);

/*!
 * @brief release device resource.
 *
 * @param hostInstance   host instance pointer.
 * @param deviceInstance    device instance pointer.
 */
static void USB_HostReleaseDeviceResource(usb_host_instance_t *hostInstance,
                                          usb_host_device_instance_t *deviceInstance);

/*!
 * @brief parse device configuration descriptor.
 *
 * @param deviceHandle    device handle.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostParseDeviceConfigurationDescriptor(usb_device_handle deviceHandle);

/*!
 * @brief remove device instance from host device list.
 *
 * @param hostHandle   host instance handle.
 * @param deviceHandle    device handle.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostRemoveDeviceInstance(usb_host_handle hostHandle, usb_device_handle deviceHandle);

/*!
 * @brief control the bus.
 *
 * This function control the host bus.
 *
 * @param[in] hostHandle     the host handle.
 * @param[in] controlType    the control code, please reference to bus_event_t.
 *
 * @retval kStatus_USB_Success              control successfully.
 * @retval kStatus_USB_InvalidHandle        The hostHandle is a NULL pointer.
 */
static usb_status_t USB_HostControlBus(usb_host_handle hostHandle, uint8_t controlType);

extern usb_status_t USB_HostStandardSetGetDescriptor(usb_host_device_instance_t *deviceInstance,
                                                     usb_host_transfer_t *transfer,
                                                     void *param);
extern usb_status_t USB_HostStandardSetAddress(usb_host_device_instance_t *deviceInstance,
                                               usb_host_transfer_t *transfer,
                                               void *param);
extern usb_status_t USB_HostCh9RequestCommon(usb_host_device_instance_t *deviceInstance,
                                             usb_host_transfer_t *transfer,
                                             uint8_t *buffer,
                                             uint32_t bufferLen);

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))

extern usb_status_t USB_HostHubDeviceEvent(usb_host_handle hostHandle,
                                           usb_device_handle deviceHandle,
                                           usb_host_configuration_handle configurationHandle,
                                           uint32_t eventCode);

extern uint32_t USB_HostHubGetHsHubNumber(usb_host_handle hostHandle, uint8_t parentHubNo);

extern uint32_t USB_HostHubGetHsHubPort(usb_host_handle hostHandle, uint8_t parentHubNo, uint8_t parentPortNo);

extern usb_status_t USB_HostHubRemovePort(usb_host_handle hostHandle, uint8_t hubNumber, uint8_t portNumber);

#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern usb_host_instance_t g_UsbHostInstance[USB_HOST_CONFIG_MAX_HOST];

/*! @brief enumeration step process array */
static const usb_host_enum_process_entry_t s_EnumEntries[] = \
{
    /* kStatus_dev_initial */
    {
        0, 0, NULL,
    },
    /* kStatus_DEV_GetDes8 */
    {
        kStatus_DEV_SetAddress, kStatus_DEV_GetDes8, USB_HostProcessCallback,
    },
    /* kStatus_DEV_SetAddress */
    {
        kStatus_DEV_GetDes, kStatus_DEV_SetAddress, USB_HostProcessCallback,
    },
    /* kStatus_DEV_GetDes */
    {
        kStatus_DEV_GetCfg9, kStatus_DEV_GetDes, NULL,
    },
    /* kStatus_DEV_GetCfg9 */
    {
        kStatus_DEV_GetCfg, kStatus_DEV_GetCfg9, USB_HostProcessCallback,
    },
    /* kStatus_DEV_GetCfg */
    {
        kStatus_DEV_SetCfg, kStatus_DEV_GetCfg9, USB_HostProcessCallback,
    },
    /* kStatus_DEV_SetCfg */
    {
        kStatus_DEV_EnumDone, kStatus_DEV_SetCfg, NULL,
    },
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static void USB_HostEnumerationTransferCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    uint8_t nextStep = 0;
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)param;

    USB_HostFreeTransfer(deviceInstance->hostHandle, transfer); /* free transfer */

    if (status == kStatus_USB_Success)
    {
        nextStep = 1;
    }
    else if (status == kStatus_USB_TransferStall)
    {
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
        usb_echo("no response from device\r\n");
#endif                                        /* USB_HOST_CONFIG_COMPLIANCE_TEST */
        if (deviceInstance->stallRetries > 0) /* retry same transfer when stall */
        {
            deviceInstance->stallRetries--;
        }
        else /* process next state when all retries stall */
        {
            nextStep = 1;
        }
    }
    else if (status == kStatus_USB_TransferCancel)
    {
        return;
    }
    else
    {
        if (deviceInstance->enumRetries > 0) /* next whole retry */
        {
            deviceInstance->enumRetries--;
            deviceInstance->stallRetries = USB_HOST_CONFIG_ENUMERATION_MAX_STALL_RETRIES;
            deviceInstance->configurationValue = 0;
            deviceInstance->state = kStatus_DEV_GetDes8;
        }
        else
        {
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
            usb_echo("Device No Response\r\n");
#endif
            return;
        }
    }

    if (nextStep == 1)
    {
        deviceInstance->stallRetries = USB_HOST_CONFIG_ENUMERATION_MAX_STALL_RETRIES;
        if (s_EnumEntries[deviceInstance->state - 1].process == NULL)
        {
            deviceInstance->state = s_EnumEntries[deviceInstance->state - 1].successState; /* next state */
        }
        else
        {
            status = s_EnumEntries[deviceInstance->state - 1].process(
                deviceInstance);               /* process the previous state result */
            if (status == kStatus_USB_Success) /* process success */
            {
                deviceInstance->state = s_EnumEntries[deviceInstance->state - 1].successState;
            }
            else if (status == kStatus_USB_Retry) /* need retry */
            {
                deviceInstance->state = s_EnumEntries[deviceInstance->state - 1].retryState;
            }
            else if (status == kStatus_USB_NotSupported) /* device don't suport by the application */
            {
                return; /* unrecoverable fail */
            }
            else /* process error, next retry */
            {
                if (deviceInstance->enumRetries > 0) /* next whole retry */
                {
                    deviceInstance->enumRetries--;
                    deviceInstance->stallRetries = USB_HOST_CONFIG_ENUMERATION_MAX_STALL_RETRIES;
                    deviceInstance->configurationValue = 0;
                    deviceInstance->state = kStatus_DEV_GetDes8;
                }
                else
                {
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
                    usb_echo("Device No Response\r\n");
#endif
                    return; /* unrecoverable fail */
                }
            }
        }
    }

    if (USB_HostProcessState(deviceInstance) != kStatus_USB_Success) /* process the new state */
    {
#ifdef HOST_ECHO
        usb_echo("enumation setup error\r\n");
#endif
        return;
    }
}

static usb_status_t USB_HostProcessState(usb_host_device_instance_t *deviceInstance)
{
    usb_status_t status = kStatus_USB_Success;
    usb_host_process_descriptor_param_t getDescriptorParam;
    usb_host_transfer_t *transfer;

    /* malloc transfer */
    if (deviceInstance->state != kStatus_DEV_EnumDone)
    {
        if (USB_HostMallocTransfer(deviceInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        transfer->callbackFn = USB_HostEnumerationTransferCallback;
        transfer->callbackParam = deviceInstance;

        /* reset transfer fields */
        transfer->setupPacket->bmRequestType = 0x00;
        transfer->setupPacket->wIndex = 0;
        transfer->setupPacket->wLength = 0;
        transfer->setupPacket->wValue = 0;
    }

    switch (deviceInstance->state)
    {
        case kStatus_DEV_GetDes8:
        case kStatus_DEV_GetDes: /* get descriptor state */
            getDescriptorParam.descriptorLength = sizeof(usb_descriptor_device_t);
            if (deviceInstance->state == kStatus_DEV_GetDes8)
            {
                getDescriptorParam.descriptorLength = 8;
            }
            getDescriptorParam.descriptorBuffer = (uint8_t *)deviceInstance->deviceDescriptor;
            getDescriptorParam.descriptorType = USB_DESCRIPTOR_TYPE_DEVICE;
            getDescriptorParam.descriptorIndex = 0;
            getDescriptorParam.languageId = 0;

            transfer->setupPacket->bmRequestType |= USB_REQUEST_TYPE_DIR_IN;
            transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_GET_DESCRIPTOR;
            status = USB_HostStandardSetGetDescriptor(deviceInstance, transfer, &getDescriptorParam);
            break;
        case kStatus_DEV_SetAddress: /* set address state */
            transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_ADDRESS;
            status = USB_HostStandardSetAddress(deviceInstance, transfer, &deviceInstance->allocatedAddress);
            break;

        case kStatus_DEV_GetCfg9: /* get 9 bytes configuration state */
            getDescriptorParam.descriptorBuffer = deviceInstance->enumBuffer;
            getDescriptorParam.descriptorType = USB_DESCRIPTOR_TYPE_CONFIGURE;
            getDescriptorParam.descriptorIndex = deviceInstance->configurationValue;
            getDescriptorParam.descriptorLength = 9;
            getDescriptorParam.languageId = 0;

            transfer->setupPacket->bmRequestType |= USB_REQUEST_TYPE_DIR_IN;
            transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_GET_DESCRIPTOR;
            status = USB_HostStandardSetGetDescriptor(deviceInstance, transfer, &getDescriptorParam);
            break;

        case kStatus_DEV_GetCfg: /* get configuration state */
            getDescriptorParam.descriptorBuffer = deviceInstance->configurationDesc;
            getDescriptorParam.descriptorType = USB_DESCRIPTOR_TYPE_CONFIGURE;
            getDescriptorParam.descriptorIndex = deviceInstance->configurationValue;
            getDescriptorParam.descriptorLength = deviceInstance->configurationLen;
            getDescriptorParam.languageId = 0;

            transfer->setupPacket->bmRequestType |= USB_REQUEST_TYPE_DIR_IN;
            transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_GET_DESCRIPTOR;
            status = USB_HostStandardSetGetDescriptor(deviceInstance, transfer, &getDescriptorParam);
            break;

        case kStatus_DEV_SetCfg: /* set configuration state */
            transfer->setupPacket->wValue =
                USB_SHORT_TO_LITTLE_ENDIAN(deviceInstance->configuration.configurationDesc->bConfigurationValue);
            transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_CONFIGURATION;
            status = USB_HostCh9RequestCommon(deviceInstance, transfer, NULL, 0);
            break;

        case kStatus_DEV_EnumDone: /* enumeration done state */
            status = USB_HostNotifyDevice(deviceInstance,
                                          kUSB_HostEventEnumerationDone); /* notify device enumeration done */
            if (status == kStatus_USB_Success)
            {
                deviceInstance->state = kStatus_DEV_AppUsed;
            }
            break;

        default:
            break;
    }

    return status;
}

static usb_status_t USB_HostProcessCallback(usb_host_device_instance_t *deviceInstance)
{
    usb_host_pipe_t *pipe = (usb_host_pipe_t *)deviceInstance->controlPipe;
    usb_status_t status = kStatus_USB_Success;
    usb_descriptor_configuration_t *configureDesc;
    usb_host_instance_t *hostInstance = (usb_host_instance_t *)deviceInstance->hostHandle;

    switch (deviceInstance->state)
    {
        case kStatus_DEV_GetDes8: /* process get 8 bytes descriptor result */
            pipe->maxPacketSize = deviceInstance->deviceDescriptor->bMaxPacketSize0;
            /* the callbackFn is initialized in USB_HostGetControllerInterface */
            hostInstance->controllerTable->controllerIoctl(
                hostInstance->controllerHandle, kUSB_HostUpdateControlPacketSize, deviceInstance->controlPipe);
            break;

        case kStatus_DEV_SetAddress: /* process set address result */
            deviceInstance->setAddress = deviceInstance->allocatedAddress;
            /* the callbackFn is initialized in USB_HostGetControllerInterface */
            hostInstance->controllerTable->controllerIoctl(
                hostInstance->controllerHandle, kUSB_HostUpdateControlEndpointAddress, deviceInstance->controlPipe);
            break;

        case kStatus_DEV_GetDes: /* process set address result */
            /* NULL */
            break;

        case kStatus_DEV_GetCfg9: /* process get 9 bytes configuration result */
            configureDesc = (usb_descriptor_configuration_t *)&deviceInstance->enumBuffer[0];

            deviceInstance->configurationLen = USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(configureDesc->wTotalLength);
            if (deviceInstance->configurationDesc != NULL)
            {
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
                SDK_Free(deviceInstance->configurationDesc);
#else
                USB_OsaMemoryFree(deviceInstance->configurationDesc);
#endif
                deviceInstance->configurationDesc = NULL;
            }
            /* for KHCI, the start address and the length should be 4 byte align */
            if ((deviceInstance->configurationLen & 0x03) != 0)
            {
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
                deviceInstance->configurationDesc =
                    (uint8_t *)SDK_Malloc((deviceInstance->configurationLen & 0xFFFFFFFCu) + 4, USB_CACHE_LINESIZE);
#else
                deviceInstance->configurationDesc =
                    (uint8_t *)USB_OsaMemoryAllocate((deviceInstance->configurationLen & 0xFFFFFFFCu) + 4);
#endif
            }
            else
            {
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
                deviceInstance->configurationDesc =
                    (uint8_t *)SDK_Malloc(deviceInstance->configurationLen, USB_CACHE_LINESIZE);
#else
                deviceInstance->configurationDesc = (uint8_t *)USB_OsaMemoryAllocate(deviceInstance->configurationLen);
#endif
            }
            if (deviceInstance->configurationDesc == NULL)
            {
                return kStatus_USB_Error;
            }
            break;

        case kStatus_DEV_GetCfg: /* process get cofiguration result */
            if (((usb_descriptor_configuration_t *)deviceInstance->configurationDesc)->bMaxPower >
                USB_HOST_CONFIG_MAX_POWER)
            {
                return kStatus_USB_Error;
            }
            deviceInstance->configurationValue++;
            if (USB_HostParseDeviceConfigurationDescriptor(deviceInstance) !=
                kStatus_USB_Success) /* parse configuration descriptor */
            {
                return kStatus_USB_Error;
            }

            status = USB_HostNotifyDevice(deviceInstance, kUSB_HostEventAttach);

            if (status != kStatus_USB_Success)
            {
                /* next configuration */
                if (deviceInstance->configurationValue < deviceInstance->deviceDescriptor->bNumConfigurations)
                {
                    return kStatus_USB_Retry;
                }
                else
                {
                    USB_HostNotifyDevice(deviceInstance,
                                         kUSB_HostEventNotSupported); /* notify application device is not supported */
                    return kStatus_USB_NotSupported;
                }
            }
            break;

        case kStatus_DEV_SetCfg:
            /* NULL */
            break;

        default:
            break;
    }

    return status;
}

static usb_status_t USB_HostNotifyDevice(usb_host_device_instance_t *deviceInstance, uint32_t eventCode)
{
    usb_host_instance_t *hostInstance;
    usb_status_t status1 = kStatus_USB_Error;
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    usb_status_t status2 = kStatus_USB_Error;
    uint8_t haveHub;
    uint8_t haveNoHub;
    uint8_t interfaceIndex;
#endif /* USB_HOST_CONFIG_HUB */

    if (deviceInstance == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }
    hostInstance = (usb_host_instance_t *)deviceInstance->hostHandle;

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    haveHub = 0;
    haveNoHub = 0;
    for (interfaceIndex = 0; interfaceIndex < deviceInstance->configuration.interfaceCount; ++interfaceIndex)
    {
        if (((usb_descriptor_interface_t *)deviceInstance->configuration.interfaceList[interfaceIndex].interfaceDesc)
                ->bInterfaceClass == USB_HOST_HUB_CLASS_CODE)
        {
            haveHub = 1;
        }
        else
        {
            haveNoHub = 1;
        }
    }

    if ((haveNoHub == 1) && (hostInstance->deviceCallback != NULL))
    {
        /* call host callback function, function is initialized in USB_HostInit */
        status1 = hostInstance->deviceCallback(deviceInstance, &deviceInstance->configuration, eventCode);
    }
    if (haveHub)
    {
        status2 = USB_HostHubDeviceEvent(hostInstance, deviceInstance, &deviceInstance->configuration,
                                         eventCode); /* notify hub event */
    }

    if ((status1 == kStatus_USB_Success) || (status2 == kStatus_USB_Success)) /* the device is supported */
    {
        return kStatus_USB_Success;
    }
    else if (eventCode == kUSB_HostEventAttach) /* attach event */
    {
        status1 = kStatus_USB_NotSupported;
    }
    else
    {
        status1 = kStatus_USB_Error;
    }
#else
    if (hostInstance->deviceCallback != NULL)
    {
        /* call host callback function, function is initialized in USB_HostInit */
        status1 = hostInstance->deviceCallback(deviceInstance, &deviceInstance->configuration, eventCode);
    }
#endif
    return status1;
}

static uint8_t USB_HostAllocateDeviceAddress(usb_host_instance_t *hostInstance)
{
    uint8_t address = 0;
    uint8_t addressIndex;
    uint8_t addressBitIndex;
    for (addressIndex = 0; addressIndex < 8; ++addressIndex) /* find the idle address postion byte */
    {
        if (hostInstance->addressBitMap[addressIndex] != 0xFF)
        {
            break;
        }
    }
    if (addressIndex < 8)
    {
        for (addressBitIndex = 0; addressBitIndex < 8; ++addressBitIndex) /* find the idle address position bit */
        {
            if (!(hostInstance->addressBitMap[addressIndex] & (0x01u << addressBitIndex)))
            {
                hostInstance->addressBitMap[addressIndex] |= (0x01u << addressBitIndex); /* set the allocated bit */
                address = addressIndex * 8 + addressBitIndex + 1;                        /* the address minimum is 1 */
                break;
            }
        }
    }
    return address;
}

static void USB_HostReleaseDeviceAddress(usb_host_instance_t *hostInstance, uint8_t address)
{
    USB_HostLock();
    hostInstance->addressBitMap[(uint32_t)(address - 1) >> 3] &=
        (~(0x01u << (((uint32_t)address - 1) & 0x07U))); /* reset the allocated bit */
    USB_HostUnlock();
}

static usb_status_t USB_HostRemoveDeviceInstance(usb_host_handle hostHandle, usb_device_handle deviceHandle)
{
    usb_host_instance_t *hostInstance = (usb_host_instance_t *)hostHandle;
    usb_host_device_instance_t *currentInstance;
    usb_host_device_instance_t *prevInstance;
    if ((hostHandle == NULL) || (deviceHandle == NULL))
    {
        return kStatus_USB_InvalidHandle;
    }

    /* search and remove device instance */
    prevInstance = (usb_host_device_instance_t *)hostInstance->deviceList;
    if (prevInstance == deviceHandle)
    {
        hostInstance->deviceList = prevInstance->next;
        return kStatus_USB_Success;
    }
    else
    {
        currentInstance = prevInstance->next;
    }

    while (currentInstance != NULL)
    {
        if (currentInstance == deviceHandle)
        {
            prevInstance->next = currentInstance->next;
            return kStatus_USB_Success;
        }
        prevInstance = currentInstance;
        currentInstance = currentInstance->next;
    }

    return kStatus_USB_Success;
}

static void USB_HostReleaseDeviceResource(usb_host_instance_t *hostInstance, usb_host_device_instance_t *deviceInstance)
{
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    uint8_t level = 0;
#endif

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    if (deviceInstance == hostInstance->suspendedDevice)
    {
        hostInstance->suspendedDevice = NULL;
    }
#endif
    /* release device's address */
    if (deviceInstance->setAddress != 0)
    {
        USB_HostReleaseDeviceAddress(hostInstance, deviceInstance->setAddress);
    }
    else
    {
        if (deviceInstance->allocatedAddress != 0)
        {
            USB_HostReleaseDeviceAddress(hostInstance, deviceInstance->allocatedAddress);
        }
    }

    /* close control pipe */
    if (deviceInstance->controlPipe != NULL)
    {
        USB_HostCancelTransfer(hostInstance, deviceInstance->controlPipe, NULL);
        if (USB_HostClosePipe(hostInstance, deviceInstance->controlPipe) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        deviceInstance->controlPipe = NULL;
    }

    /* free configuration buffer */
    if (deviceInstance->configurationDesc != NULL)
    {
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
        SDK_Free(deviceInstance->configurationDesc);
#else
        USB_OsaMemoryFree(deviceInstance->configurationDesc);
#endif
    }

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    level = deviceInstance->level;
#endif
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
    SDK_Free(deviceInstance->deviceDescriptor);
#else
    USB_OsaMemoryFree(deviceInstance->deviceDescriptor);
#endif
    /* free device instance buffer */
    USB_OsaMemoryFree(deviceInstance);

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    /* enable controller attach if root hub */
    if (level == 1)
    {
        USB_HostControlBus(hostInstance, kUSB_HostBusEnableAttach);
    }
#else
    /* enable controller attach */
    USB_HostControlBus(hostInstance, kUSB_HostBusEnableAttach);
#endif
}

static usb_status_t USB_HostParseDeviceConfigurationDescriptor(usb_device_handle deviceHandle)
{
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    uint32_t endPos;
    usb_descriptor_union_t *unionDes;
    usb_host_interface_t *interfaceParse = NULL;
    usb_host_ep_t *epParse;
    uint8_t *buffer;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    buffer = (uint8_t *)&deviceInstance->configuration;
    /* clear the previous parse result, note: end_pos means buffer index here*/
    for (endPos = 0; endPos < sizeof(usb_host_configuration_t); endPos++)
    {
        buffer[endPos] = 0;
    }
    for (endPos = 0; endPos < USB_HOST_CONFIG_CONFIGURATION_MAX_INTERFACE; ++endPos)
    {
        deviceInstance->interfaceStatus[endPos] = 0;
    }

    /* parse configuration descriptor */
    unionDes = (usb_descriptor_union_t *)deviceInstance->configurationDesc;
    endPos = (uint32_t)(deviceInstance->configurationDesc + deviceInstance->configurationLen);

    if ((unionDes->common.bLength == USB_DESCRIPTOR_LENGTH_CONFIGURE) &&
        (unionDes->common.bDescriptorType == USB_DESCRIPTOR_TYPE_CONFIGURE))
    {
        /* configuration descriptor */
        deviceInstance->configuration.configurationDesc = (usb_descriptor_configuration_t *)unionDes;
        deviceInstance->configuration.configurationExtensionLength = 0;
        deviceInstance->configuration.configurationExtension = NULL;
        deviceInstance->configuration.interfaceCount = 0;
        unionDes = (usb_descriptor_union_t *)((uint32_t)unionDes + unionDes->common.bLength);
        while ((uint32_t)unionDes < endPos)
        {
            if (unionDes->common.bDescriptorType != USB_DESCRIPTOR_TYPE_INTERFACE)
            {
                if (deviceInstance->configuration.configurationExtension == NULL)
                {
                    deviceInstance->configuration.configurationExtension = (uint8_t *)unionDes;
                }
                if ((unionDes->common.bDescriptorType == 0x00) ||
                    (unionDes->common.bLength == 0x00)) /* the descriptor data is wrong */
                {
                    return kStatus_USB_Error;
                }
                deviceInstance->configuration.configurationExtensionLength += unionDes->common.bLength;
                unionDes = (usb_descriptor_union_t *)((uint32_t)unionDes + unionDes->common.bLength);
            }
            else
            {
                break;
            }
        }

        /* interface descriptor */
        deviceInstance->configuration.interfaceCount = 0;
        while ((uint32_t)unionDes < endPos)
        {
            if (unionDes->common.bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE)
            {
                if (unionDes->interface.bAlternateSetting == 0x00)
                {
                    if (deviceInstance->configuration.interfaceCount >= USB_HOST_CONFIG_CONFIGURATION_MAX_INTERFACE)
                    {
#ifdef HOST_ECHO
                        usb_echo(
                            "Unsupported Device attached\r\n too many interfaces in one configuration, please increase "
                            "the USB_HOST_CONFIG_CONFIGURATION_MAX_INTERFACE value\n");
#endif
                        return kStatus_USB_Error;
                    }
                    interfaceParse =
                        &deviceInstance->configuration.interfaceList[deviceInstance->configuration.interfaceCount];
                    deviceInstance->configuration.interfaceCount++;
                    interfaceParse->alternateSettingNumber = 0;
                    interfaceParse->epCount = 0;
                    interfaceParse->interfaceDesc = &unionDes->interface;
                    interfaceParse->interfaceExtensionLength = 0;
                    interfaceParse->interfaceExtension = NULL;
                    interfaceParse->interfaceIndex = unionDes->interface.bInterfaceNumber;
                    if (unionDes->common.bLength == 0x00) /* the descriptor data is wrong */
                    {
                        return kStatus_USB_Error;
                    }
                    unionDes = (usb_descriptor_union_t *)((uint32_t)unionDes + unionDes->common.bLength);
                    while ((uint32_t)unionDes < endPos)
                    {
                        if ((unionDes->common.bDescriptorType != USB_DESCRIPTOR_TYPE_INTERFACE) &&
                            (unionDes->common.bDescriptorType != USB_DESCRIPTOR_TYPE_ENDPOINT))
                        {
                            if (interfaceParse->interfaceExtension == NULL)
                            {
                                interfaceParse->interfaceExtension = (uint8_t *)unionDes;
                            }
                            if ((unionDes->common.bDescriptorType == 0x00) ||
                                (unionDes->common.bLength == 0x00)) /* the descriptor data is wrong */
                            {
                                return kStatus_USB_Error;
                            }
                            interfaceParse->interfaceExtensionLength += unionDes->common.bLength;
                            unionDes = (usb_descriptor_union_t *)((uint32_t)unionDes + unionDes->common.bLength);
                        }
                        else
                        {
                            break;
                        }
                    }

                    /* endpoint descriptor */
                    if (interfaceParse->interfaceDesc->bNumEndpoints != 0)
                    {
                        if ((unionDes->common.bDescriptorType != USB_DESCRIPTOR_TYPE_ENDPOINT) ||
                            (interfaceParse->interfaceDesc->bNumEndpoints > USB_HOST_CONFIG_INTERFACE_MAX_EP))
                        {
#ifdef HOST_ECHO
                            usb_echo("interface descriptor error\n");
#endif
                            return kStatus_USB_Error;
                        }
                        for (; interfaceParse->epCount < interfaceParse->interfaceDesc->bNumEndpoints;
                             (interfaceParse->epCount)++)
                        {
                            if (((uint32_t)unionDes >= endPos) ||
                                (unionDes->common.bDescriptorType != USB_DESCRIPTOR_TYPE_ENDPOINT))
                            {
#ifdef HOST_ECHO
                                usb_echo("endpoint descriptor error\n");
#endif
                                return kStatus_USB_Error;
                            }
                            epParse = (usb_host_ep_t *)&interfaceParse->epList[interfaceParse->epCount];
                            epParse->epDesc = (usb_descriptor_endpoint_t *)unionDes;
                            epParse->epExtensionLength = 0;
                            epParse->epExtension = NULL;
                            if (unionDes->common.bLength == 0x00) /* the descriptor data is wrong */
                            {
                                return kStatus_USB_Error;
                            }
                            unionDes = (usb_descriptor_union_t *)((uint32_t)unionDes + unionDes->common.bLength);
                            while ((uint32_t)unionDes < endPos)
                            {
                                if ((unionDes->common.bDescriptorType != USB_DESCRIPTOR_TYPE_ENDPOINT) &&
                                    (unionDes->common.bDescriptorType != USB_DESCRIPTOR_TYPE_INTERFACE))
                                {
                                    if (epParse->epExtension == NULL)
                                    {
                                        epParse->epExtension = (uint8_t *)unionDes;
                                    }
                                    if ((unionDes->common.bDescriptorType == 0x00) ||
                                        (unionDes->common.bLength == 0x00)) /* the descriptor data is wrong */
                                    {
                                        return kStatus_USB_Error;
                                    }
                                    epParse->epExtensionLength += unionDes->common.bLength;
                                    unionDes =
                                        (usb_descriptor_union_t *)((uint32_t)unionDes + unionDes->common.bLength);
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (interfaceParse == NULL)
                    {
                        return kStatus_USB_Error; /* in normal situation this cannot reach */
                    }
                    interfaceParse->alternateSettingNumber++;
                    if (interfaceParse->interfaceExtension == NULL)
                    {
                        interfaceParse->interfaceExtension = (uint8_t *)unionDes;
                    }
                    if (unionDes->common.bLength == 0x00) /* the descriptor data is wrong */
                    {
                        return kStatus_USB_Error;
                    }
                    interfaceParse->interfaceExtensionLength += unionDes->common.bLength;
                    unionDes = (usb_descriptor_union_t *)((uint32_t)unionDes + unionDes->common.bLength);
                    while ((uint32_t)unionDes < endPos)
                    {
                        if (unionDes->common.bDescriptorType != USB_DESCRIPTOR_TYPE_INTERFACE)
                        {
                            if ((unionDes->common.bDescriptorType == 0x00) ||
                                (unionDes->common.bLength == 0x00)) /* the descriptor data is wrong */
                            {
                                return kStatus_USB_Error;
                            }
                            interfaceParse->interfaceExtensionLength += unionDes->common.bLength;
                            unionDes = (usb_descriptor_union_t *)((uint32_t)unionDes + unionDes->common.bLength);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
            else
            {
                return kStatus_USB_Error;
            }
        }
    }

    for (endPos = 0; endPos < deviceInstance->configuration.interfaceCount; ++endPos)
    {
        deviceInstance->interfaceStatus[endPos] = kStatus_interface_Attached;
    }

    return kStatus_USB_Success;
}

usb_status_t USB_HostAttachDevice(usb_host_handle hostHandle,
                                  uint8_t speed,
                                  uint8_t hubNumber,
                                  uint8_t portNumber,
                                  uint8_t level,
                                  usb_device_handle *deviceHandle)
{
    usb_host_instance_t *hostInstance = (usb_host_instance_t *)hostHandle;
    usb_host_device_instance_t *newInstance;
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    usb_host_device_instance_t *currentInstance;
#endif
    uint8_t address;
    usb_host_pipe_init_t pipeInit;

    if (hostHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

/* check whether is the device attached? */
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    currentInstance = (usb_host_device_instance_t *)hostInstance->deviceList;
    while (currentInstance != NULL)
    {
        if ((currentInstance->hubNumber == hubNumber) && (currentInstance->portNumber == portNumber))
        {
            *deviceHandle = NULL;
#ifdef HOST_ECHO
            usb_echo("device has attached\r\n");
#endif
            return kStatus_USB_Busy;
        }
        else
        {
            currentInstance = currentInstance->next;
        }
    }
#else
    if (hostInstance->deviceList != NULL)
    {
        *deviceHandle = NULL;
        usb_echo("device has attached\r\n");
        return kStatus_USB_Busy;
    }
#endif

    /* Allocate new device instance */
    newInstance = (usb_host_device_instance_t *)USB_OsaMemoryAllocate(sizeof(usb_host_device_instance_t));
    if (newInstance == NULL)
    {
#ifdef HOST_ECHO
        usb_echo("allocate dev instance fail\r\n");
#endif
        return kStatus_USB_AllocFail;
    }

    /* new instance fields init */
    newInstance->hostHandle = hostHandle;
    newInstance->speed = speed;
    newInstance->stallRetries = USB_HOST_CONFIG_ENUMERATION_MAX_STALL_RETRIES;
    newInstance->enumRetries = USB_HOST_CONFIG_ENUMERATION_MAX_RETRIES;
    newInstance->setAddress = 0;
    newInstance->deviceAttachState = kStatus_device_Attached;
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
    newInstance->deviceDescriptor =
        (usb_descriptor_device_t *)SDK_Malloc(sizeof(usb_descriptor_device_t) + 9, USB_CACHE_LINESIZE);
#else
    newInstance->deviceDescriptor =
        (usb_descriptor_device_t *)USB_OsaMemoryAllocate(sizeof(usb_descriptor_device_t) + 9);
#endif
    if (newInstance->deviceDescriptor == NULL)
    {
#ifdef HOST_ECHO
        usb_echo("allocate newInstance->deviceDescriptor fail\r\n");
#endif
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
        SDK_Free(newInstance->deviceDescriptor);
#else
        USB_OsaMemoryFree(newInstance->deviceDescriptor);
#endif
        USB_OsaMemoryFree(newInstance);
        return kStatus_USB_AllocFail;
    }
    newInstance->enumBuffer = (uint8_t *)((uint8_t *)newInstance->deviceDescriptor + sizeof(usb_descriptor_device_t));
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    newInstance->hubNumber = hubNumber;
    newInstance->portNumber = portNumber;
    newInstance->level = level;

    if ((speed != USB_SPEED_HIGH) && (level > 1))
    {
        newInstance->hsHubNumber = USB_HostHubGetHsHubNumber(hostHandle, hubNumber);
        newInstance->hsHubPort = USB_HostHubGetHsHubPort(hostHandle, hubNumber, portNumber);
    }
    else
    {
        newInstance->hsHubNumber = hubNumber;
        newInstance->hsHubPort = portNumber;
    }
#endif /* USB_HOST_CONFIG_HUB */

    USB_HostLock();
    /* allocate address && insert to the dev list */
    address = USB_HostAllocateDeviceAddress(hostInstance);
    if (address == 0)
    {
#ifdef HOST_ECHO
        usb_echo("allocate address fail\r\n");
#endif
        USB_HostUnlock();
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
        SDK_Free(newInstance->deviceDescriptor);
#else
        USB_OsaMemoryFree(newInstance->deviceDescriptor);
#endif
        USB_OsaMemoryFree(newInstance);
        return kStatus_USB_Error;
    }
    newInstance->allocatedAddress = address;

    newInstance->next = (usb_host_device_instance_t *)hostInstance->deviceList;
    hostInstance->deviceList = newInstance;
    newInstance->state = kStatus_DEV_Initial;
    USB_HostUnlock();

    /* open control pipe */
    pipeInit.devInstance = newInstance;
    pipeInit.pipeType = USB_ENDPOINT_CONTROL;
    pipeInit.direction = 0;
    pipeInit.endpointAddress = 0;
    pipeInit.interval = 0;
    pipeInit.maxPacketSize = 8;
    pipeInit.numberPerUframe = 0;
    pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;
    if (USB_HostOpenPipe(hostHandle, &newInstance->controlPipe, &pipeInit) != kStatus_USB_Success)
    {
        /* don't need release resource, resource is released when detach */
        *deviceHandle = newInstance;
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
        SDK_Free(newInstance->deviceDescriptor);
#else
        USB_OsaMemoryFree(newInstance->deviceDescriptor);
#endif
        USB_OsaMemoryFree(newInstance);
        return kStatus_USB_Error;
    }

    /* start enumeration */
    newInstance->state = kStatus_DEV_GetDes8;
    USB_HostProcessState(newInstance); /* process enumeration state machine */

    *deviceHandle = newInstance;
    return kStatus_USB_Success;
}

usb_status_t USB_HostDetachDevice(usb_host_handle hostHandle, uint8_t hubNumber, uint8_t portNumber)
{
    usb_host_device_instance_t *deviceInstance;
    usb_host_instance_t *hostInstance = (usb_host_instance_t *)hostHandle;

    if (hostHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    USB_HostLock();
/* search for device instance handle */
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    deviceInstance = (usb_host_device_instance_t *)hostInstance->deviceList;
    while (deviceInstance != NULL)
    {
        if ((deviceInstance->hubNumber == hubNumber) && (deviceInstance->portNumber == portNumber))
        {
            break;
        }
        deviceInstance = deviceInstance->next;
    }
#else
    deviceInstance = (usb_host_device_instance_t *)hostInstance->deviceList;
#endif
    USB_HostUnlock();
    if (deviceInstance != NULL)
    {
        return USB_HostDetachDeviceInternal(hostHandle, deviceInstance); /* device instance detach */
    }
    return kStatus_USB_Success;
}

usb_status_t USB_HostDetachDeviceInternal(usb_host_handle hostHandle, usb_device_handle deviceHandle)
{
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    usb_host_instance_t *hostInstance = (usb_host_instance_t *)hostHandle;
    if ((hostHandle == NULL) || (deviceHandle == NULL))
    {
        return kStatus_USB_InvalidHandle;
    }

    deviceInstance->deviceAttachState = kStatus_device_Detached; /* mark the device is detached from host */

    if (deviceInstance->state >= kStatus_DEV_Initial) /* device instance is valid */
    {
        /* detach internally */
        if (deviceInstance->state < kStatus_DEV_AppUsed) /* enumeration is not done */
        {
            if (deviceInstance->controlPipe != NULL)
            {
                USB_HostCancelTransfer(hostInstance, deviceInstance->controlPipe, NULL);
            }

            /* remove device instance from host */
            USB_HostRemoveDeviceInstance(hostInstance, deviceInstance);
            USB_HostReleaseDeviceResource(hostInstance, deviceInstance);
        }
        else /* enumeration has be done and notifed application */
        {
            USB_HostNotifyDevice(deviceInstance, kUSB_HostEventDetach); /* notify application device detach */
        }
    }

    return kStatus_USB_Success;
}

uint8_t USB_HostGetDeviceAttachState(usb_device_handle deviceHandle)
{
    return deviceHandle ? ((usb_host_device_instance_t *)deviceHandle)->deviceAttachState : 0x0;
}

usb_status_t USB_HostValidateDevice(usb_host_handle hostHandle, usb_device_handle deviceHandle)
{
    usb_host_device_instance_t *searchDev;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }
    /* search for the device */
    searchDev = (usb_host_device_instance_t *)((usb_host_instance_t *)hostHandle)->deviceList;
    while ((searchDev != NULL) && ((usb_device_handle)searchDev != deviceHandle))
    {
        searchDev = searchDev->next;
    }

    if (searchDev)
    {
        return kStatus_USB_Success;
    }
    return kStatus_USB_Error;
}

static usb_status_t USB_HostControlBus(usb_host_handle hostHandle, uint8_t controlType)
{
    usb_status_t status = kStatus_USB_Success;
    usb_host_instance_t *hostInstance = (usb_host_instance_t *)hostHandle;

    if (hostHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }
    /* the callbackFn is initialized in USB_HostGetControllerInterface */
    status = hostInstance->controllerTable->controllerIoctl(hostInstance->controllerHandle, kUSB_HostBusControl,
                                                            &controlType);

    return status;
}

usb_status_t USB_HostOpenDeviceInterface(usb_device_handle deviceHandle, usb_host_interface_handle interfaceHandle)
{
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    usb_host_instance_t *hostInstance = NULL;
    uint8_t interfaceIndex;
    uint8_t index = 0;

    if ((deviceHandle == NULL) || (interfaceHandle == NULL))
    {
        return kStatus_USB_InvalidHandle;
    }

    hostInstance = (usb_host_instance_t *)deviceInstance->hostHandle;
    USB_HostLock();
    /* check host_instance valid? */
    for (; index < USB_HOST_CONFIG_MAX_HOST; ++index)
    {
        if ((g_UsbHostInstance[index].occupied == 1) &&
            ((usb_host_instance_t *)(&g_UsbHostInstance[index]) == (hostInstance)))
        {
            break;
        }
    }
    if (index >= USB_HOST_CONFIG_MAX_HOST)
    {
        USB_HostUnlock();
        return kStatus_USB_Error;
    }

    /* check deviceHandle valid? */
    if (USB_HostValidateDevice(hostInstance, deviceHandle) != kStatus_USB_Success)
    {
        USB_HostUnlock();
        return kStatus_USB_Error;
    }

    /* search interface and set the interface as opened */
    for (interfaceIndex = 0; interfaceIndex < deviceInstance->configuration.interfaceCount; ++interfaceIndex)
    {
        if (&deviceInstance->configuration.interfaceList[interfaceIndex] == interfaceHandle)
        {
            deviceInstance->interfaceStatus[interfaceIndex] = kStatus_interface_Opened;
            break;
        }
    }
    USB_HostUnlock();

    return kStatus_USB_Success;
}

usb_status_t USB_HostCloseDeviceInterface(usb_device_handle deviceHandle, usb_host_interface_handle interfaceHandle)
{
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    usb_host_instance_t *hostInstance = NULL;
    uint8_t interfaceIndex;
    uint8_t removeLabel = 1;
    uint8_t index = 0;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    hostInstance = (usb_host_instance_t *)deviceInstance->hostHandle;
    USB_HostLock();
    /* check host_instance valid? */
    for (; index < USB_HOST_CONFIG_MAX_HOST; ++index)
    {
        if ((g_UsbHostInstance[index].occupied == 1) &&
            ((usb_host_instance_t *)(&g_UsbHostInstance[index]) == (hostInstance)))
        {
            break;
        }
    }
    if (index >= USB_HOST_CONFIG_MAX_HOST)
    {
        USB_HostUnlock();
        return kStatus_USB_Error;
    }

    /* check deviceHandle valid? */
    if (USB_HostValidateDevice(hostInstance, deviceHandle) != kStatus_USB_Success)
    {
        USB_HostUnlock();
        return kStatus_USB_Error;
    }

    if (interfaceHandle != NULL)
    {
        /* search interface and set the interface as detached */
        for (interfaceIndex = 0; interfaceIndex < deviceInstance->configuration.interfaceCount; ++interfaceIndex)
        {
            if (&deviceInstance->configuration.interfaceList[interfaceIndex] == interfaceHandle)
            {
                deviceInstance->interfaceStatus[interfaceIndex] = kStatus_interface_Detached;
                break;
            }
        }
    }

    if (deviceInstance->deviceAttachState == kStatus_device_Detached) /* device is removed from host */
    {
        removeLabel = 1;
        /* check all the interfaces of the device are not opened */
        for (interfaceIndex = 0; interfaceIndex < deviceInstance->configuration.interfaceCount; ++interfaceIndex)
        {
            if (deviceInstance->interfaceStatus[interfaceIndex] == kStatus_interface_Opened)
            {
                removeLabel = 0;
                break;
            }
        }
        if (removeLabel == 1)
        {
            /* remove device instance from host */
            USB_HostRemoveDeviceInstance(hostInstance, deviceInstance);
        }
        USB_HostUnlock();

        if (removeLabel == 1)
        {
            USB_HostReleaseDeviceResource((usb_host_instance_t *)deviceInstance->hostHandle,
                                          deviceInstance); /* release device resource */
        }
    }
    else
    {
        USB_HostUnlock();
    }

    return kStatus_USB_Success;
}

usb_status_t USB_HostRemoveDevice(usb_host_handle hostHandle, usb_device_handle deviceHandle)
{
    usb_host_instance_t *hostInstance = (usb_host_instance_t *)hostHandle;
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    uint8_t interfaceIndex = 0;
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
    uint8_t level = 0;
    uint8_t devHubNo;
    uint8_t devPortNo;
#endif

    if ((hostHandle == NULL) || (deviceHandle == NULL))
    {
        return kStatus_USB_InvalidHandle;
    }
    if (deviceInstance->hostHandle != hostHandle)
    {
        return kStatus_USB_InvalidParameter;
    }

    if (USB_HostValidateDevice(hostInstance, deviceInstance) == kStatus_USB_Success) /* device is valid */
    {
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
        devHubNo = deviceInstance->hubNumber;
        devPortNo = deviceInstance->portNumber;
        level = deviceInstance->level;
#endif

        deviceInstance->deviceAttachState = kStatus_device_Detached;
        if (deviceInstance->state >= kStatus_DEV_Initial) /* device is valid */
        {
            if (deviceInstance->state < kStatus_DEV_AppUsed) /* enumeraion is not done or application don't use */
            {
                /* detach internally */
                USB_HostDetachDeviceInternal(hostHandle, deviceHandle);
            }
            else /* application use the device */
            {
                for (interfaceIndex = 0; interfaceIndex < deviceInstance->configuration.interfaceCount;
                     ++interfaceIndex)
                {
                    if (deviceInstance->interfaceStatus[interfaceIndex] == kStatus_interface_Opened)
                    {
#ifdef HOST_ECHO
                        usb_echo("error: there is class instance that is not deinited\r\n");
#endif
                        break;
                    }
                }
                /* remove device instance from host */
                USB_HostRemoveDeviceInstance(hostInstance, deviceInstance);
                USB_HostReleaseDeviceResource(hostInstance, deviceInstance); /* release resource */
            }
        }

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
        if (level == 1)
        {
            USB_HostControlBus(hostHandle, kUSB_HostBusReset);   /* reset controller port */
            USB_HostControlBus(hostHandle, kUSB_HostBusRestart); /* restart controller port */
        }
        else
        {
            USB_HostHubRemovePort(hostHandle, devHubNo, devPortNo); /* reset hub port */
        }
#else
        USB_HostControlBus(hostHandle, kUSB_HostBusReset);   /* reset controller port */
        USB_HostControlBus(hostHandle, kUSB_HostBusRestart); /* restart controller port */
#endif /* USB_HOST_CONFIG_HUB */
    }

    return kStatus_USB_Success;
}
