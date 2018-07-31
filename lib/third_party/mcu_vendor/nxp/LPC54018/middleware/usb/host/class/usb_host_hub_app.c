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

#include "usb_host_config.h"
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))

#include "usb_host.h"
#include "usb_host_hub.h"
#include "usb_host_hci.h"
#include "usb_host_hub_app.h"
#include "usb_host_devices.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief HUB lock */
#define USB_HostHubLock() USB_OsaMutexLock(hubGlobal->hubMutex)
/*! @brief HUB unlock */
#define USB_HostHubUnlock() USB_OsaMutexUnlock(hubGlobal->hubMutex)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief prime interrupt in data.
 *
 * @param hubInstance   hub instance pointer.
 */
static void USB_HostHubGetInterruptStatus(usb_host_hub_instance_t *hubInstance);

/*!
 * @brief hub process state machine. hub is enable after the state machine.
 *
 * @param hubInstance  hub instance pointer.
 */
static void USB_HostHubProcess(usb_host_hub_instance_t *hubInstance);

/*!
 * @brief hub port attach process state machine. one device is attached to the port after the state machine.
 *
 * @param hubInstance  hub instance pointer.
 */
static void USB_HostHubProcessPortAttach(usb_host_hub_instance_t *hubInstance);

/*!
 * @brief hub port detach process state machine. one device is detached from the port after the state machine.
 *
 * @param hubInstance  hub instance pointer.
 */
static void USB_HostHubProcessPortDetach(usb_host_hub_instance_t *hubInstance);

/*!
 * @brief hub port process.
 *
 * @param hubInstance  hub instance pointer.
 */
static void USB_HostHubProcessPort(usb_host_hub_instance_t *hubInstance);

/*!
 * @brief hub interrupt in data process.
 *
 * @param hubInstance  hub instance pointer.
 */
static void USB_HostHubProcessData(usb_host_hub_global_t *hubGlobal, usb_host_hub_instance_t *hubInstance);

/*!
 * @brief hub control pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
void USB_HostHubControlCallback(void *param, uint8_t *data, uint32_t data_len, usb_status_t status);

/*!
 * @brief hub interrupt pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
void USB_HostHubInterruptInCallback(void *param, uint8_t *data, uint32_t data_len, usb_status_t status);

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))

static usb_host_hub_instance_t *USB_HostHubGetHubDeviceHandle(usb_host_handle hostHandle, uint8_t parentHubNo);

static usb_status_t USB_HostSendHubRequest(usb_device_handle deviceHandle,
                                           uint8_t requestType,
                                           uint8_t request,
                                           uint16_t wvalue,
                                           uint16_t windex,
                                           host_inner_transfer_callback_t callbackFn,
                                           void *callbackParam);

#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/

static usb_device_handle s_HubDeviceHandle;
static usb_host_interface_handle s_HubInterfaceHandle;
static usb_host_hub_global_t s_HubGlobalArray[USB_HOST_CONFIG_MAX_HOST];

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
static usb_host_configuration_t *s_HubConfiguration;
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

static usb_host_hub_global_t *USB_HostHubGetHubList(usb_host_handle hostHandle)
{
#if (USB_HOST_CONFIG_MAX_HOST == 1U)
    return &s_HubGlobalArray[0];
#else
    uint8_t index;
    for (index = 0; index < USB_HOST_CONFIG_MAX_HOST; ++index)
    {
        if (s_HubGlobalArray[index].hostHandle == hostHandle)
        {
            return &s_HubGlobalArray[index];
        }
    }
    /* There is no used usb_host_hub_global_t instance */
    for (index = 0; index < USB_HOST_CONFIG_MAX_HOST; ++index)
    {
        if (s_HubGlobalArray[index].hostHandle == NULL)
        {
            s_HubGlobalArray[index].hostHandle = hostHandle;
            return &s_HubGlobalArray[index];
        }
    }
    /* Look for the usb_host_hub_global_t instance that is not used any more */
    for (index = 0; index < USB_HOST_CONFIG_MAX_HOST; ++index)
    {
        if (s_HubGlobalArray[index].hubList == NULL)
        {
            s_HubGlobalArray[index].hostHandle = hostHandle;
            return &s_HubGlobalArray[index];
        }
    }
    return NULL;
#endif
}

static void USB_HostHubGetInterruptStatus(usb_host_hub_instance_t *hubInstance)
{
    if (hubInstance == NULL)
    {
        return;
    }

    /* there is no prime for control or interrupt */
    if (hubInstance->primeStatus != kPrimeNone)
    {
        return;
    }

    /* receive interrupt data */
    if (USB_HostHubInterruptRecv(hubInstance, hubInstance->hubBitmapBuffer, (hubInstance->portCount >> 3) + 1,
                                 USB_HostHubInterruptInCallback, hubInstance) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error in hub interrupt recv\r\n");
#endif
    }
    else
    {
        hubInstance->primeStatus = kPrimeInterrupt;
    }
}

static void USB_HostHubProcess(usb_host_hub_instance_t *hubInstance)
{
    uint8_t needPrimeInterrupt = 0; /* need to prime interrupt in transfer (0 - don't need; 1 - need) */
    uint8_t processSuccess = 0;     /* the code excute successfully (0 - fail; 1 - success) */
    uint32_t tmp = 0;
    usb_host_hub_descriptor_t *hubDescriptor;

    switch (hubInstance->hubStatus)
    {
        case kHubRunIdle:
        case kHubRunInvalid:
            break;

        case kHubRunWaitSetInterface:
            hubInstance->hubStatus = kHubRunGetDescriptor7; /* update as next state */
            /* get hub descriptor */
            if (USB_HostHubGetDescriptor(hubInstance, hubInstance->hubDescriptor, 7, USB_HostHubControlCallback,
                                         hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimeHubControl; /* control transfer is on-going */
                processSuccess = 1;
#ifdef HOST_ECHO
                usb_echo("hub get descriptor 7\r\n");
#endif
            }
            else
            {
#ifdef HOST_ECHO
                usb_echo("hub get descriptor 7 error\r\n");
#endif
                break;
            }
            break;

        case kHubRunGetDescriptor7:
            hubDescriptor = (usb_host_hub_descriptor_t *)&hubInstance->hubDescriptor[0];

            /* get the hub think time */
            USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetHubThinkTime, &tmp);
            hubInstance->totalThinktime = tmp;
            tmp = ((((uint32_t)hubDescriptor->whubcharacteristics[0] &
                     USB_HOST_HUB_DESCRIPTOR_CHARACTERISTICS_THINK_TIME_MASK) >>
                    USB_HOST_HUB_DESCRIPTOR_CHARACTERISTICS_THINK_TIME_SHIFT));
            /*
            00 - 8 FS bit times;
            01 - 16 FS bit times;
            10 - 24 FS bit times;
            11 - 32 FS bit times;
            */
            tmp = (tmp + 1) << 3;
            hubInstance->totalThinktime += tmp;

            /* get hub port number */
            hubInstance->portCount = hubDescriptor->bnrports;
            if (hubInstance->portCount > USB_HOST_HUB_MAX_PORT)
            {
#ifdef HOST_ECHO
                usb_echo("port number is bigger than USB_HOST_HUB_MAX_PORT\r\n");
#endif
                return;
            }

            hubInstance->hubStatus = kHubRunGetDescriptor; /* update as next state */
            /* get hub descriptor */
            if (USB_HostHubGetDescriptor(hubInstance, hubInstance->hubDescriptor, 7 + (hubInstance->portCount >> 3) + 1,
                                         USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimeHubControl; /* control transfer is on-going */
                processSuccess = 1;
#ifdef HOST_ECHO
                usb_echo("hub get descriptor\r\n");
#endif
            }
            else
            {
#ifdef HOST_ECHO
                usb_echo("hub get descriptor error\r\n");
#endif
                break;
            }
            break;

        case kHubRunGetDescriptor:
            /* malloc port instance for the hub's ports */
            hubInstance->portList = (usb_host_hub_port_instance_t *)USB_OsaMemoryAllocate(
                hubInstance->portCount * sizeof(usb_host_hub_port_instance_t));
            if (hubInstance->portList == NULL)
            {
#ifdef HOST_ECHO
                usb_echo("port list allocate fail\r\n");
#endif
                hubInstance->hubStatus = kHubRunInvalid;
                break;
            }
            /* TODO: port instance status -> can be removed.  app_status */

            hubInstance->hubStatus = kHubRunSetPortPower; /* update as next state */
            hubInstance->portIndex = 0;

            /* there is no significance, just for fixing misra error */
            if (hubInstance->hubStatus != kHubRunSetPortPower)
            {
                break;
            }

        case kHubRunSetPortPower:
            /* set PORT_POWER for all ports */
            if (hubInstance->portIndex < hubInstance->portCount)
            {
                hubInstance->portIndex++;
                if (USB_HostHubSetPortFeature(hubInstance, hubInstance->portIndex, PORT_POWER,
                                              USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimeHubControl; /* update as next state */
                    processSuccess = 1;
#ifdef HOST_ECHO
                    usb_echo("set port feature PORT_POWER\r\n");
#endif
                }
                else
                {
#ifdef HOST_ECHO
                    usb_echo("set port feature PORT_POWER fail\r\n");
#endif
                    needPrimeInterrupt = 1;
                    break;
                }
                break;
            }
            hubInstance->portProcess = 0;
            /* reset port information as default */
            for (tmp = 0; tmp < hubInstance->portCount; ++tmp)
            {
                hubInstance->portList[tmp].deviceHandle = NULL;
                hubInstance->portList[tmp].resetCount = USB_HOST_HUB_PORT_RESET_TIMES;
                hubInstance->portList[tmp].portStatus = kPortRunWaitPortChange;
            }
            hubInstance->hubStatus = kHubRunIdle;
            needPrimeInterrupt = 1;
            break;

        case kHubRunGetStatusDone: /* process hub status change */
            tmp = USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS((&hubInstance->hubStatusBuffer[2]));
            hubInstance->hubStatus = kHubRunIdle;
            if ((1 << C_HUB_LOCAL_POWER) & tmp) /* C_HUB_LOCAL_POWER */
            {
                if (USB_HostHubClearFeature(hubInstance, C_HUB_LOCAL_POWER, USB_HostHubControlCallback, hubInstance) ==
                    kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimeHubControl;
                    hubInstance->hubStatus = kHubRunClearDone;
                    processSuccess = 1;
                }
                else
                {
                    needPrimeInterrupt = 1;
                }
            }
            else if ((1 << C_HUB_OVER_CURRENT) & tmp) /* C_HUB_OVER_CURRENT */
            {
                if (USB_HostHubClearFeature(hubInstance, C_HUB_OVER_CURRENT, USB_HostHubControlCallback, hubInstance) ==
                    kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimeHubControl;
                    processSuccess = 1;
                    hubInstance->hubStatus = kHubRunClearDone;
                }
                else
                {
                    needPrimeInterrupt = 1;
                }
            }
            else
            {
                needPrimeInterrupt = 1;
            }
            break;

        case kHubRunClearDone:
            hubInstance->hubStatus = kHubRunIdle;
            needPrimeInterrupt = 1;
            break;

        default:
            break;
    }

    if (needPrimeInterrupt == 1) /* prime interrupt in transfer */
    {
        hubInstance->hubStatus = kHubRunIdle;
        USB_HostHubGetInterruptStatus(hubInstance);
    }
    else
    {
        if (processSuccess == 0)
        {
            hubInstance->hubStatus = kHubRunInvalid;
        }
    }
}

static void USB_HostHubProcessPort(usb_host_hub_instance_t *hubInstance)
{
    usb_host_hub_port_instance_t *portInstance = &hubInstance->portList[hubInstance->portProcess - 1];

    /* for device attach */
    if (portInstance->deviceHandle == NULL)
    {
        USB_HostHubProcessPortAttach(hubInstance);
    }
    else /* for device detach */
    {
        USB_HostHubProcessPortDetach(hubInstance);
    }
}

static void USB_HostHubProcessPortAttach(usb_host_hub_instance_t *hubInstance)
{
    usb_host_hub_port_instance_t *portInstance = &hubInstance->portList[hubInstance->portProcess - 1];
    uint8_t processSuccess = 0;
    uint32_t specStatus;
    uint8_t feature;
    uint32_t infoValue;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hubInstance->hostHandle);
    if (hubGlobal == NULL)
    {
        return;
    }

    switch (portInstance->portStatus)
    {
        case kPortRunIdle:
        case kPortRunInvalid:
            break;
        case kPortRunWaitPortChange:                                 /* (1) port is changed, and get port status */
            portInstance->portStatus = kPortRunCheckCPortConnection; /* update as next state */
            /* send class-specific request to get port status */
            if (USB_HostHubGetPortStatus(hubInstance, hubInstance->portProcess, hubInstance->portStatusBuffer, 4,
                                         USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimePortControl;
                processSuccess = 1;
            }
            break;

        case kPortRunCheckCPortConnection: /* (2) check port status, and clear the status bits */
            feature = 0;
            specStatus = USB_LONG_FROM_LITTLE_ENDIAN_ADDRESS((hubInstance->portStatusBuffer));
            if ((1 << C_PORT_CONNECTION) & specStatus)
            {
                portInstance->portStatus = kPortRunGetPortConnection; /* update as next state */
                /* clear C_PORT_CONNECTION */
                if (USB_HostHubClearPortFeature(hubInstance, hubInstance->portProcess, C_PORT_CONNECTION,
                                                USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimePortControl;
                    processSuccess = 1;
                }
                break;
            }
            else if ((1 << PORT_CONNECTION) & specStatus)
            {
                portInstance->portStatus = kPortRunWaitPortResetDone; /* update as next state */
                /* set PORT_RESET */
                if (USB_HostHubSetPortFeature(hubInstance, hubInstance->portProcess, PORT_RESET,
                                              USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimePortControl;
                    processSuccess = 1;
                    if (portInstance->resetCount > 0)
                    {
                        portInstance->resetCount--;
                    }
                }
                break;
            }
            else if ((1 << C_PORT_RESET) & specStatus)
            {
                feature = C_PORT_RESET; /* clear C_PORT_RESET */
#ifdef HOST_ECHO
                usb_echo("hub: C_PORT_RESET when detached\r\n");
#endif
            }
            else if ((1 << C_PORT_ENABLE) & specStatus)
            {
                feature = C_PORT_ENABLE; /* clear C_PORT_ENABLE */
#ifdef HOST_ECHO
                usb_echo("hub: C_PORT_ENABLE when detached\r\n");
#endif
            }
            else if ((1 << C_PORT_OVER_CURRENT) & specStatus)
            {
                feature = C_PORT_OVER_CURRENT; /* clear C_PORT_OVER_CURRENT */
#ifdef HOST_ECHO
                usb_echo("hub: C_PORT_OVER_CURRENT when detached\r\n");
#endif
            }
            else
            {
            }

            if (feature != 0)
            {
                portInstance->portStatus = kPortRunWaitPortChange; /* update as next state */
                /* clear feature */
                if (USB_HostHubClearPortFeature(hubInstance, hubInstance->portProcess, feature,
                                                USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimePortControl;
                    processSuccess = 1;
                }
            }
            break;

        case kPortRunGetPortConnection:                             /* (3) get port status */
            portInstance->portStatus = kPortRunCheckPortConnection; /* update as next state */
            /* get port status bits */
            if (USB_HostHubGetPortStatus(hubInstance, hubInstance->portProcess, hubInstance->portStatusBuffer, 4,
                                         USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimePortControl; /* control transfer is on-going */
                processSuccess = 1;
            }
            break;

        case kPortRunCheckPortConnection: /* (4) check PORT_CONNECTION bit */
            specStatus = USB_LONG_FROM_LITTLE_ENDIAN_ADDRESS(hubInstance->portStatusBuffer);
            if ((1 << PORT_CONNECTION) & specStatus)
            {
                portInstance->portStatus = kPortRunWaitPortResetDone; /* update as next state */
                /* set PORT_RESET */
                if (USB_HostHubSetPortFeature(hubInstance, hubInstance->portProcess, PORT_RESET,
                                              USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimePortControl;
                    processSuccess = 1;
                    if (portInstance->resetCount > 0)
                    {
                        portInstance->resetCount--;
                    }
                }
            }
            break;

        case kPortRunWaitPortResetDone:                        /* (5) wait port change */
            portInstance->portStatus = kPortRunWaitCPortReset; /* update as next state */
            processSuccess = 1;
            /* must wait the enumeration done, then operate the next port */
            USB_HostHubGetInterruptStatus(hubInstance);
            break;

        case kPortRunWaitCPortReset:                            /* (6) get port satus for checking C_PORT_RESET */
            portInstance->portStatus = KPortRunCheckCPortReset; /* update as next state */
            /* get port status bits */
            if (USB_HostHubGetPortStatus(hubInstance, hubInstance->portProcess, hubInstance->portStatusBuffer, 4,
                                         USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimePortControl;
                processSuccess = 1;
            }
            break;

        case KPortRunCheckCPortReset: /* (7) check C_PORT_RESET and clear C_PORT_RESET */
            specStatus = USB_LONG_FROM_LITTLE_ENDIAN_ADDRESS(hubInstance->portStatusBuffer);
            if ((1 << C_PORT_RESET) & specStatus)
            {
                if (portInstance->resetCount == 0)
                {
                    portInstance->portStatus = kPortRunPortAttached; /* update as next state */
                    /* get port's device speed */
                    if (specStatus & (1 << PORT_HIGH_SPEED))
                    {
                        portInstance->speed = USB_SPEED_HIGH;
                    }
                    else if (specStatus & (1 << PORT_LOW_SPEED))
                    {
                        portInstance->speed = USB_SPEED_LOW;
                    }
                    else
                    {
                        portInstance->speed = USB_SPEED_FULL;
                    }
                }
                else
                {
                    portInstance->portStatus = kPortRunResetAgain; /* update as next state */
                }

                /* clear C_PORT_RESET */
                if (USB_HostHubClearPortFeature(hubInstance, hubInstance->portProcess, C_PORT_RESET,
                                                USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimePortControl;
                    processSuccess = 1;
                }
            }
            break;

        case kPortRunResetAgain:                                    /* (8) reset again */
            portInstance->portStatus = kPortRunCheckPortConnection; /* check connection then reset again */
            if (USB_HostHubGetPortStatus(hubInstance, hubInstance->portProcess, hubInstance->portStatusBuffer, 4,
                                         USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimePortControl;
                processSuccess = 1;
            }
            break;

        case kPortRunPortAttached: /* (9) the port have one device attached */
            USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
            USB_HostAttachDevice(hubInstance->hostHandle, portInstance->speed, infoValue, hubInstance->portProcess,
                                 hubInstance->hubLevel + 1, &portInstance->deviceHandle);
            processSuccess = 1;
            hubInstance->portProcess = 0;
            hubGlobal->hubProcess = NULL;
            portInstance->resetCount = USB_HOST_HUB_PORT_RESET_TIMES;
            USB_HostHubGetInterruptStatus(hubInstance);
            break;
        default:
            break;
    }

    if (processSuccess == 0)
    {
        portInstance->portStatus = kPortRunWaitPortChange;
        hubInstance->portProcess = 0;
        hubGlobal->hubProcess = NULL;
        portInstance->resetCount = USB_HOST_HUB_PORT_RESET_TIMES;

        USB_HostHubGetInterruptStatus(hubInstance);
    }
}

static void USB_HostHubProcessPortDetach(usb_host_hub_instance_t *hubInstance)
{
    usb_host_hub_port_instance_t *portInstance = &hubInstance->portList[hubInstance->portProcess - 1];
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    usb_host_instance_t *hostPointer = (usb_host_instance_t *)hubInstance->hostHandle;
#endif
    uint8_t processSuccess = 0;
    uint32_t specStatus;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hubInstance->hostHandle);
    if (hubGlobal == NULL)
    {
        return;
    }

    switch (portInstance->portStatus)
    {
        case kPortRunIdle:
        case kPortRunInvalid:
            break;
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
        case kPortRunPortSuspended:
#endif
        case kPortRunPortAttached: /* (1) port is changed, then get port status */
            portInstance->portStatus = kPortRunCheckPortDetach;
            /* get port status */
            if (USB_HostHubGetPortStatus(hubInstance, hubInstance->portProcess, hubInstance->portStatusBuffer, 4,
                                         USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimePortControl;
                processSuccess = 1;
            }
            break;

        case kPortRunCheckPortDetach: /* (2) check port status bits */
            specStatus = USB_LONG_FROM_LITTLE_ENDIAN_ADDRESS(hubInstance->portStatusBuffer);
            portInstance->portStatus = kPortRunGetConnectionBit;
            if ((1 << C_PORT_CONNECTION) & specStatus) /* C_PORT_CONNECTION */
            {
                if (USB_HostHubClearPortFeature(hubInstance, hubInstance->portProcess, C_PORT_CONNECTION,
                                                USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimePortControl;
                    processSuccess = 1;
                }
                break;
            }
            else if ((1 << C_PORT_ENABLE) & specStatus) /* C_PORT_ENABLE */
            {
                if (USB_HostHubClearPortFeature(hubInstance, hubInstance->portProcess, C_PORT_ENABLE,
                                                USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimePortControl;
                    processSuccess = 1;
                }
                break;
            }
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
            else if ((1 << C_PORT_SUSPEND) & specStatus)
            {
                /* clear C_PORT_SUSPEND */
                if (USB_HostHubClearPortFeature(hubInstance, hubInstance->portProcess, C_PORT_SUSPEND,
                                                USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
                {
                    portInstance->portStatus = kPortRunClearCPortSuspend; /* update as next state */
                    hubInstance->primeStatus = kPrimePortControl;
                    processSuccess = 1;
                    break;
                }
            }
#endif
            else
            {
                /* don't break to check CONNECTION bit */
            }
        case kPortRunGetConnectionBit: /* (3) get port status */
            portInstance->portStatus = kPortRunCheckConnectionBit;
            if (USB_HostHubGetPortStatus(hubInstance, hubInstance->portProcess, hubInstance->portStatusBuffer, 4,
                                         USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimePortControl;
                processSuccess = 1;
            }
            break;

        case kPortRunCheckConnectionBit: /* (4) check port connection bit */
            specStatus = USB_LONG_FROM_LITTLE_ENDIAN_ADDRESS(hubInstance->portStatusBuffer);
            if ((1 << PORT_CONNECTION) & specStatus) /* PORT_CONNECTION */
            {
                portInstance->portStatus = kPortRunPortAttached;
#ifdef HOST_ECHO
                usb_echo("PORT_CONNECTION in attach for detach\r\n");
#endif
            }
            else
            {
                processSuccess = 1;
                /* port's device is detached */
                portInstance->portStatus = kPortRunWaitPortChange;
                USB_HostDetachDeviceInternal(hubInstance->hostHandle, portInstance->deviceHandle);
                portInstance->deviceHandle = NULL;
                hubGlobal->hubProcess = NULL;
                hubInstance->portProcess = 0;
                USB_HostHubGetInterruptStatus(hubInstance);
            }
            break;
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
        case kPortRunClearCPortSuspend:
            portInstance->portStatus = kPortRunCheckPortSuspend; /* update as next state */
            /* get port status bits */
            if (USB_HostHubGetPortStatus(hubInstance, hubInstance->portProcess, hubInstance->portStatusBuffer, 4,
                                         USB_HostHubControlCallback, hubInstance) == kStatus_USB_Success)
            {
                hubInstance->primeStatus = kPrimePortControl;
                processSuccess = 1;
            }
            break;
        case kPortRunCheckPortSuspend:
            specStatus = USB_LONG_FROM_LITTLE_ENDIAN_ADDRESS(hubInstance->portStatusBuffer);
            if ((1 << PORT_SUSPEND) & specStatus)
            {
                portInstance->portStatus = kPortRunPortSuspended; /* update as next state */
                /* call host callback function, function is initialized in USB_HostInit */
                hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                            kUSB_HostEventSuspended); /* call host callback function */
            }
            else
            {
                portInstance->portStatus = kPortRunPortAttached; /* update as next state */
                /* call host callback function, function is initialized in USB_HostInit */
                hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                            kUSB_HostEventResumed); /* call host callback function */
                hostPointer->suspendedDevice = NULL;
            }
            break;
#endif
        default:
            break;
    }

    if (processSuccess == 0)
    {
        portInstance->portStatus = kPortRunPortAttached;
        hubGlobal->hubProcess = NULL;
        hubInstance->portProcess = 0;
        USB_HostHubGetInterruptStatus(hubInstance);
    }
}

static void USB_HostHubProcessData(usb_host_hub_global_t *hubGlobal, usb_host_hub_instance_t *hubInstance)
{
    uint8_t needPrimeInterrupt = 1;
    uint8_t portIndex;

    /* process the port which status change */
    for (portIndex = 0; portIndex <= hubInstance->portCount; ++portIndex)
    {
        if ((0x01u << (portIndex & 0x07u)) & (hubInstance->hubBitmapBuffer[portIndex >> 3]))
        {
            if (portIndex == 0) /* hub status change */
            {
                if ((hubGlobal->hubProcess == NULL) ||
                    ((hubGlobal->hubProcess == hubInstance) && (hubInstance->portProcess == 0)))
                {
                    hubInstance->hubStatus = kHubRunGetStatusDone;
                    if (USB_HostHubGetStatus(hubInstance, hubInstance->hubStatusBuffer, 4, USB_HostHubControlCallback,
                                             hubInstance) != kStatus_USB_Success)
                    {
#ifdef HOST_ECHO
                        usb_echo("error in usb_class_hub_get_status\r\n");
#endif
                        hubInstance->hubStatus = kHubRunIdle;
                    }
                    else
                    {
                        hubInstance->primeStatus = kPrimeHubControl;
                        needPrimeInterrupt = 0;
                        return; /* return replace break because the misra */
                    }
                }
            }
            else /* port's status change */
            {
                /* process the on-going port or process one new port */
                if ((hubGlobal->hubProcess == NULL) ||
                    ((hubGlobal->hubProcess == hubInstance) && (hubInstance->portProcess == 0)) ||
                    ((hubGlobal->hubProcess == hubInstance) && (hubInstance->portProcess == portIndex)))
                {
                    if (hubInstance->controlTransfer == NULL)
                    {
                        hubGlobal->hubProcess = hubInstance;
                        hubInstance->portProcess = portIndex;
                        needPrimeInterrupt = 0;
                        USB_HostHubProcessPort(hubInstance);
                    }
                    break; /* process the port change in turn */
                }
            }
        }
    }

    if (needPrimeInterrupt == 1)
    {
        USB_HostHubGetInterruptStatus(hubInstance);
    }
}

void USB_HostHubControlCallback(void *param, uint8_t *data, uint32_t data_len, usb_status_t status)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)param;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hubInstance->hostHandle);
    if (hubGlobal == NULL)
    {
        return;
    }

    if (hubInstance->invalid == 1)
    {
        return;
    }
    if (status != kStatus_USB_Success)
    {
        /* if transfer fail, prime a new interrupt in transfer */
        hubInstance->primeStatus = kPrimeNone;
        hubGlobal->hubProcess = NULL;
        hubInstance->portProcess = 0;
        USB_HostHubGetInterruptStatus(hubInstance);
        return;
    }

    if (hubInstance->primeStatus == kPrimeHubControl) /* hub related control transfer */
    {
        hubInstance->primeStatus = kPrimeNone;
        USB_HostHubProcess(hubInstance);
    }
    else if (hubInstance->primeStatus == kPrimePortControl) /* hub's port related control transfer */
    {
        hubInstance->primeStatus = kPrimeNone;
        USB_HostHubProcessPort(hubInstance);
    }
    else
    {
    }
}

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
/*!
 * @brief get device's hub device instance.
 *
 * @param parent_hub_no  device's parent hub instance.
 *
 * @return think time value.
 */
static usb_host_hub_instance_t *USB_HostHubGetHubDeviceHandle(usb_host_handle hostHandle, uint8_t parentHubNo)
{
    usb_host_hub_instance_t *hubInstance;
    uint32_t deviceAddress;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hostHandle);
    if (hubGlobal == NULL)
    {
        return NULL;
    }
    hubInstance = hubGlobal->hubList;

    /* get parentHubNo's hub instance handle */
    while (hubInstance != NULL)
    {
        USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceAddress, &deviceAddress);
        if (parentHubNo == deviceAddress)
        {
            break;
        }
        hubInstance = hubInstance->next;
    }
    if (hubInstance != NULL)
    {
        return hubInstance;
    }
    return (usb_host_hub_instance_t *)NULL;
}

static void USB_HostSetHubRequestCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_instance_t *hostInstance = (usb_host_instance_t *)param;
    USB_HostFreeTransfer(param, transfer);

    if (kStatus_USB_Success == status)
    {
        /* call host callback function, function is initialized in USB_HostInit */
        hostInstance->deviceCallback(hostInstance->suspendedDevice, NULL,
                                     kUSB_HostEventSuspended); /* call host callback function */
    }
    else
    {
        /* call host callback function, function is initialized in USB_HostInit */
        hostInstance->deviceCallback(hostInstance->suspendedDevice, NULL,
                                     kUSB_HostEventNotSuspended); /* call host callback function */
    }
}

static void USB_HostClearHubRequestCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    USB_HostFreeTransfer(param, transfer);

    if (kStatus_USB_Success == status)
    {
    }
}

static void USB_HostHubRemoteWakeupCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hub_instance_t *hubInstance;
    usb_host_instance_t *hostInstance;
    usb_host_device_instance_t *deviceInstance;
    if (NULL == param)
    {
        return;
    }

    hubInstance = (usb_host_hub_instance_t *)param;

    hostInstance = (usb_host_instance_t *)hubInstance->hostHandle;
    if (NULL == hostInstance)
    {
        return;
    }

    USB_HostFreeTransfer(hostInstance, transfer);

    if (kStatus_USB_Success != status)
    {
        usb_echo("Transfer failed to set remote wakeup request to HUB.\r\n");
    }

    if (kStatus_USB_Success == status)
    {
        hubInstance->controlRetry = USB_HOST_HUB_REMOTE_WAKEUP_TIMES;
        hubInstance = hubInstance->next;
        while (hubInstance)
        {
            hubInstance->controlRetry = USB_HOST_HUB_REMOTE_WAKEUP_TIMES;
            if (hubInstance->supportRemoteWakeup)
            {
                usb_echo("Set HUB remote wakeup feature: level %d, address %d.\r\n",
                         ((usb_host_device_instance_t *)hubInstance->deviceHandle)->level,
                         ((usb_host_device_instance_t *)hubInstance->deviceHandle)->setAddress);
                status = USB_HostSendHubRequest(
                    hubInstance->deviceHandle,
                    USB_REQUEST_TYPE_RECIPIENT_DEVICE | USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_STANDARD,
                    USB_REQUEST_STANDARD_SET_FEATURE, USB_REQUEST_STANDARD_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP, 0,
                    USB_HostHubRemoteWakeupCallback, hubInstance);
                if (kStatus_USB_Success != status)
                {
                    usb_echo("Send set remote wakeup request to HUB failed.\r\n");
                }
                break;
            }
            hubInstance = hubInstance->next;
        }
    }
    else
    {
        if (hubInstance->controlRetry)
        {
            hubInstance->controlRetry--;
            usb_echo("Retry...\r\n", ((usb_host_device_instance_t *)hubInstance->deviceHandle)->level,
                     ((usb_host_device_instance_t *)hubInstance->deviceHandle)->setAddress);
            status = USB_HostSendHubRequest(
                hubInstance->deviceHandle,
                USB_REQUEST_TYPE_RECIPIENT_DEVICE | USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_STANDARD,
                USB_REQUEST_STANDARD_SET_FEATURE, USB_REQUEST_STANDARD_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP, 0,
                USB_HostHubRemoteWakeupCallback, hubInstance);
            if (kStatus_USB_Success != status)
            {
                usb_echo("Send set remote wakeup request to HUB failed.\r\n");
            }
        }
        else
        {
            usb_echo("Transfer failed to set remote wakeup request to HUB.\r\n");
        }
    }
    if (kStatus_USB_Success != status)
    {
        /* call host callback function, function is initialized in USB_HostInit */
        hostInstance->deviceCallback(hostInstance->suspendedDevice, NULL, kUSB_HostEventNotSuspended);
        return;
    }
    if (NULL == hubInstance)
    {
        status = kStatus_USB_Error;
        deviceInstance = (usb_host_device_instance_t *)hostInstance->suspendedDevice;
        if (NULL == deviceInstance)
        {
            usb_host_bus_control_t type = kUSB_HostBusSuspend;
            /* the callbackFn is initialized in USB_HostGetControllerInterface */
            status = hostInstance->controllerTable->controllerIoctl(hostInstance->controllerHandle, kUSB_HostBusControl,
                                                                    &type);
            if (kStatus_USB_Success != status)
            {
                usb_echo("Suspend USB BUS failed.\r\n");
            }
        }
        else
        {
            usb_host_hub_instance_t *hubInstance4Device =
                USB_HostHubGetHubDeviceHandle(hostInstance, deviceInstance->hubNumber);
            if (NULL != hubInstance4Device)
            {
                status = USB_HostSendHubRequest(
                    hubInstance4Device->deviceHandle,
                    USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_OTHER,
                    USB_REQUEST_STANDARD_SET_FEATURE, PORT_SUSPEND, deviceInstance->portNumber,
                    USB_HostSetHubRequestCallback, hostInstance);
                if (kStatus_USB_Success != status)
                {
                    usb_echo("Send suspend request to HUB is failed.\r\n");
                }
            }
            else
            {
                usb_echo("Invalid HUB instance of device.\r\n");
            }
        }
        if (kStatus_USB_Success != status)
        {
            /* call host callback function, function is initialized in USB_HostInit */
            hostInstance->deviceCallback(hostInstance->suspendedDevice, NULL, kUSB_HostEventNotSuspended);
            return;
        }
    }
}

static usb_status_t USB_HostSendHubRequest(usb_device_handle deviceHandle,
                                           uint8_t requestType,
                                           uint8_t request,
                                           uint16_t wvalue,
                                           uint16_t windex,
                                           host_inner_transfer_callback_t callbackFn,
                                           void *callbackParam)
{
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    usb_host_transfer_t *transfer;

    /* get transfer */
    if (USB_HostMallocTransfer(deviceInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }

    /* initialize transfer */
    transfer->transferBuffer = NULL;
    transfer->transferLength = 0U;
    transfer->callbackFn = callbackFn;
    transfer->callbackParam = callbackParam;
    transfer->setupPacket->bmRequestType = requestType;
    transfer->setupPacket->bRequest = request;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(wvalue);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(windex);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(0U);

    /* send transfer */
    if (USB_HostSendSetup(deviceInstance->hostHandle, deviceInstance->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("Error in sending hub set report!\r\n");
#endif
        USB_HostFreeTransfer(deviceInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}
#endif

void USB_HostHubInterruptInCallback(void *param, uint8_t *data, uint32_t data_len, usb_status_t status)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)param;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hubInstance->hostHandle);
    if (hubGlobal == NULL)
    {
        return;
    }

    if (hubInstance->invalid == 1)
    {
        return;
    }
    /* interrupt data received */
    hubInstance->primeStatus = kPrimeNone;
    if (status != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("hub interrupt in data callback error\r\n");
#endif
        /* prime nexe interrupt transfer */
        if (hubInstance->controlTransfer == NULL)
        {
            hubGlobal->hubProcess = NULL;
            hubInstance->portProcess = 0;
            USB_HostHubGetInterruptStatus(hubInstance);
        }
    }
    else
    {
        USB_HostHubProcessData(hubGlobal, hubInstance); /* process the interrupt data */
    }
}

/*!
 * @brief host hub callback function.
 *
 * This function should be called in the host callback function.
 *
 * @param hostHandle             host handle.
 * @param deviceHandle           device handle.
 * @param configurationHandle attached device's configuration descriptor information.
 * @param event_code           callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The configuration don't contain hub interface.
 */
usb_status_t USB_HostHubDeviceEvent(usb_host_handle hostHandle,
                                    usb_device_handle deviceHandle,
                                    usb_host_configuration_handle configurationHandle,
                                    uint32_t eventCode)
{
    usb_host_configuration_t *configuration;
    usb_host_interface_t *interface;
    uint8_t interfaceIndex;
    uint8_t id;
    usb_status_t status = kStatus_USB_Success;
    usb_host_class_handle hubClassHandle;
    usb_host_hub_instance_t *hubInstance;
    usb_host_hub_instance_t *prevInstance;
    uint32_t infoValue;
    usb_osa_status_t osaStatus;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hostHandle);
    if (hubGlobal == NULL)
    {
        return kStatus_USB_Error;
    }

    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            /* judge whether is configurationHandle supported */
            configuration = (usb_host_configuration_t *)configurationHandle;
            for (interfaceIndex = 0; interfaceIndex < configuration->interfaceCount; ++interfaceIndex)
            {
                interface = &configuration->interfaceList[interfaceIndex];
                id = interface->interfaceDesc->bInterfaceClass;
                if (id != USB_HOST_HUB_CLASS_CODE)
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceSubClass;
                if (id != USB_HOST_HUB_SUBCLASS_CODE_NONE)
                {
                    continue;
                }
                else
                {
                    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceLevel, &infoValue);
                    if (infoValue > 5)
                    {
#ifdef HOST_ECHO
                        usb_echo("Host can support max 5 level hubs\r\n");
#endif
                        continue;
                    }
                    /* the interface is hub */
                    s_HubDeviceHandle = deviceHandle;
                    s_HubInterfaceHandle = interface;
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
                    s_HubConfiguration = configuration;
#endif
                    return kStatus_USB_Success;
                }
            }
            status = kStatus_USB_NotSupported;
            break;

        case kUSB_HostEventEnumerationDone:
            /* the device enumeration is done */
            if ((s_HubDeviceHandle != NULL) && (s_HubInterfaceHandle != NULL))
            {
                /* print hub information */
                USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceLevel, &infoValue);
                usb_echo("hub attached:level=%d ", infoValue);
                USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
                usb_echo("address=%d\r\n", infoValue);

                /* initialize hub mutex */
                if (hubGlobal->hubMutex == (usb_osa_mutex_handle)NULL)
                {
                    osaStatus = USB_OsaMutexCreate(&hubGlobal->hubMutex);
                    if (osaStatus != kStatus_USB_OSA_Success)
                    {
                        hubGlobal->hubMutex = NULL;
#ifdef HOST_ECHO
                        usb_echo("hub mutex error\r\n");
#endif
                    }
                }

                /* initialize hub class instance */
                status = USB_HostHubInit(s_HubDeviceHandle, &hubClassHandle);
                hubInstance = (usb_host_hub_instance_t *)hubClassHandle;

                /* link hub instance to list */
                USB_HostHubLock();
                hubInstance->next = hubGlobal->hubList;
                hubGlobal->hubList = hubInstance;
                USB_HostHubUnlock();
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
                hubInstance->supportRemoteWakeup = 0U;
                hubInstance->controlRetry = USB_HOST_HUB_REMOTE_WAKEUP_TIMES;
                if (s_HubConfiguration->configurationDesc->bmAttributes &
                    USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_MASK)
                {
                    hubInstance->supportRemoteWakeup = 1U;
                }
#endif
                /* set hub instance's interface */
                if (status == kStatus_USB_Success)
                {
                    hubInstance->primeStatus = kPrimeHubControl;
                    hubInstance->hubStatus = kHubRunWaitSetInterface;
                    if (USB_HostHubSetInterface(hubClassHandle, s_HubInterfaceHandle, 0, USB_HostHubControlCallback,
                                                hubInstance) != kStatus_USB_Success)
                    {
                        hubInstance->hubStatus = kHubRunInvalid;
                    }
                }
            }
            break;

        case kUSB_HostEventDetach:
            /* the device is detached */
            hubInstance = NULL;

            /* get device's hub instance handle */
            USB_HostHubLock();
            prevInstance = hubGlobal->hubList;
            if (prevInstance->deviceHandle == deviceHandle)
            {
                hubInstance = prevInstance;
                hubGlobal->hubList = prevInstance->next;
            }
            else
            {
                hubInstance = prevInstance->next;
                while (hubInstance != NULL)
                {
                    if (hubInstance->deviceHandle == deviceHandle)
                    {
                        prevInstance->next = hubInstance->next;
                        break;
                    }
                    prevInstance = hubInstance;
                    hubInstance = hubInstance->next;
                }
            }
            USB_HostHubUnlock();

            if (hubInstance != NULL)
            {
                if (hubInstance == hubGlobal->hubProcess)
                {
                    hubGlobal->hubProcess = NULL;
                }
                /* print hub information */
                USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceLevel, &infoValue);
                usb_echo("hub detached:level=%d ", infoValue);
                USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
                usb_echo("address=%d\r\n", infoValue);
                hubInstance->invalid = 1;
                /* detach hub ports' devices */
                for (uint8_t portIndex = 0; portIndex < hubInstance->portCount; ++portIndex)
                {
                    if ((hubInstance->portList != NULL) && (hubInstance->portList[portIndex].deviceHandle != NULL))
                    {
                        USB_HostDetachDeviceInternal(hubInstance->hostHandle,
                                                     hubInstance->portList[portIndex].deviceHandle);
                        hubInstance->portList[portIndex].deviceHandle = NULL;
                    }
                }
                if (hubInstance->portList != NULL)
                {
                    USB_OsaMemoryFree(hubInstance->portList);
                }
                USB_HostHubDeinit(deviceHandle, hubInstance); /* de-initialize hub instance */
            }

            /* destory hub mutex if there is no hub instance */
            if (hubGlobal->hubList == NULL)
            {
                if (hubGlobal->hubMutex != NULL)
                {
                    USB_OsaMutexDestroy(hubGlobal->hubMutex);
                    hubGlobal->hubMutex = NULL;
                }
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
 * @brief remove attached device. called by USB_HostRemoveDevice.
 *
 * @param hubNumber   the device attached hub.
 * @param portNumber  the device attached port.
 *
 * @return kStatus_USB_Success or error codes.
 */
usb_status_t USB_HostHubRemovePort(usb_host_handle hostHandle, uint8_t hubNumber, uint8_t portNumber)
{
    usb_host_hub_instance_t *hubInstance;
    uint32_t infoValue;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hostHandle);
    if (hubGlobal == NULL)
    {
        return kStatus_USB_Error;
    }

    /* get hub number's hub instance handle */
    hubInstance = (usb_host_hub_instance_t *)hubGlobal->hubList;
    while (hubInstance != NULL)
    {
        USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
        if (infoValue == hubNumber)
        {
            break;
        }
        hubInstance = hubInstance->next;
    }

    /* set port's status as default, and reset port */
    if (hubInstance != NULL)
    {
        hubInstance->portList[portNumber - 1].deviceHandle = NULL;
        hubInstance->portList[portNumber - 1].portStatus = kPortRunInvalid;
        if (hubInstance->portProcess == portNumber)
        {
            hubInstance->portProcess = 0;
        }
        USB_HostHubSendPortReset(hubInstance, portNumber);
    }
    return kStatus_USB_Error;
}

/*!
 * @brief get device's high-speed hub's address.
 *
 * @param parent_hub_no device's parent hub's address.
 *
 * @return hub number.
 */
uint32_t USB_HostHubGetHsHubNumber(usb_host_handle hostHandle, uint8_t parentHubNo)
{
    usb_host_hub_instance_t *hubInstance;
    uint32_t deviceInfo;
    uint32_t hubNumber;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hostHandle);
    if (hubGlobal == NULL)
    {
        return kStatus_USB_Error;
    }
    hubInstance = hubGlobal->hubList;

    /* get parentHubNo's hub instance handle */
    while (hubInstance != NULL)
    {
        USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceAddress, &deviceInfo);
        if (parentHubNo == deviceInfo)
        {
            break;
        }
        hubInstance = hubInstance->next;
    }
    if (hubInstance != NULL)
    {
        USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceSpeed, &deviceInfo);
        if (deviceInfo == USB_SPEED_HIGH) /* parent hub is HS */
        {
            hubNumber = parentHubNo;
        }
        else /* parent hub is not HS */
        {
            USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceHSHubNumber,
                                                   &hubNumber);
        }
        return hubNumber;
    }
    return 0;
}

/*!
 * @brief get device's high-speed hub's port number.
 *
 * @param parent_hub_no  device's parent hub's address.
 * @param parent_port_no device's parent port no.
 *
 * @return port number.
 */
uint32_t USB_HostHubGetHsHubPort(usb_host_handle hostHandle, uint8_t parentHubNo, uint8_t parentPortNo)
{
    usb_host_hub_instance_t *hubInstance;
    uint32_t deviceInfo;
    uint32_t hubPort;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hostHandle);
    if (hubGlobal == NULL)
    {
        return kStatus_USB_Error;
    }
    hubInstance = hubGlobal->hubList;

    /* get parentHubNo's hub instance handle */
    while (hubInstance != NULL)
    {
        USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceAddress, &deviceInfo);
        if (parentHubNo == deviceInfo)
        {
            break;
        }
        hubInstance = hubInstance->next;
    }
    if (hubInstance != NULL)
    {
        USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceSpeed, &deviceInfo);
        if (deviceInfo == USB_SPEED_HIGH) /* parent hub is HS */
        {
            hubPort = parentPortNo;
        }
        else /* parent hub is not HS */
        {
            USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceHSHubNumber, &hubPort);
        }
        return hubPort;
    }
    return 0;
}

/*!
 * @brief get device's hub total think time.
 *
 * @param parent_hub_no  device's parent hub's address.
 *
 * @return think time value.
 */
uint32_t USB_HostHubGetTotalThinkTime(usb_host_handle hostHandle, uint8_t parentHubNo)
{
    usb_host_hub_instance_t *hubInstance;
    uint32_t deviceAddress;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hostHandle);
    if (hubGlobal == NULL)
    {
        return kStatus_USB_Error;
    }
    hubInstance = hubGlobal->hubList;

    /* get parentHubNo's hub instance handle */
    while (hubInstance != NULL)
    {
        USB_HostHelperGetPeripheralInformation(hubInstance->deviceHandle, kUSB_HostGetDeviceAddress, &deviceAddress);
        if (parentHubNo == deviceAddress)
        {
            break;
        }
        hubInstance = hubInstance->next;
    }
    if (hubInstance != NULL)
    {
        return hubInstance->totalThinktime;
    }
    return 0;
}

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
/*!
 * @brief Suspend the device.
 *
 * @param hostHandle  Host instance.
 *
 * @return kStatus_USB_Success or error codes.
 *
 */
usb_status_t USB_HostHubSuspendDevice(usb_host_handle hostHandle)
{
    usb_host_instance_t *hostInstance;
    usb_host_hub_instance_t *hubInstance;
    usb_status_t status = kStatus_USB_Error;
    usb_host_hub_global_t *hubGlobal = USB_HostHubGetHubList(hostHandle);
    if (hubGlobal == NULL)
    {
        return kStatus_USB_Error;
    }
    hubInstance = hubGlobal->hubList;

    if (NULL == hostHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    hostInstance = (usb_host_instance_t *)hostHandle;
    if (NULL == hubInstance)
    {
        usb_host_bus_control_t type = kUSB_HostBusSuspend;
        /* the callbackFn is initialized in USB_HostGetControllerInterface */
        status =
            hostInstance->controllerTable->controllerIoctl(hostInstance->controllerHandle, kUSB_HostBusControl, &type);
        if (kStatus_USB_Success != status)
        {
            usb_echo("Suspend USB BUS failed.\r\n");
        }
        return status;
    }
    /* Scan HUB instance handle */
    while (hubInstance != NULL)
    {
        hubInstance->controlRetry = USB_HOST_HUB_REMOTE_WAKEUP_TIMES;
        if (hubInstance->supportRemoteWakeup)
        {
            usb_echo("Set HUB remote wakeup feature: level %d, address %d.\r\n",
                     ((usb_host_device_instance_t *)hubInstance->deviceHandle)->level,
                     ((usb_host_device_instance_t *)hubInstance->deviceHandle)->setAddress);
            status = USB_HostSendHubRequest(
                hubInstance->deviceHandle,
                USB_REQUEST_TYPE_RECIPIENT_DEVICE | USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_STANDARD,
                USB_REQUEST_STANDARD_SET_FEATURE, USB_REQUEST_STANDARD_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP, 0,
                USB_HostHubRemoteWakeupCallback, hubInstance);
            break;
        }
        hubInstance = hubInstance->next;
    }
    if (NULL == hubInstance)
    {
        usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)hostInstance->suspendedDevice;
        if (NULL == deviceInstance)
        {
            usb_host_bus_control_t type = kUSB_HostBusSuspend;
            /* the callbackFn is initialized in USB_HostGetControllerInterface */
            status = hostInstance->controllerTable->controllerIoctl(hostInstance->controllerHandle, kUSB_HostBusControl,
                                                                    &type);
            if (kStatus_USB_Success != status)
            {
                usb_echo("Suspend USB BUS failed.\r\n");
            }
        }
        else
        {
            usb_host_hub_instance_t *hubInstance4Device =
                USB_HostHubGetHubDeviceHandle(hostHandle, deviceInstance->hubNumber);
            if (NULL != hubInstance4Device)
            {
                status = USB_HostSendHubRequest(
                    hubInstance4Device->deviceHandle,
                    USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_OTHER,
                    USB_REQUEST_STANDARD_SET_FEATURE, PORT_SUSPEND, deviceInstance->portNumber,
                    USB_HostSetHubRequestCallback, hostInstance);
                if (kStatus_USB_Success != status)
                {
                    usb_echo("Send suspend request to HUB is failed.\r\n");
                }
            }
            else
            {
                usb_echo("Invalid HUB instance of device.\r\n");
            }
        }
    }
    return status;
}

/*!
 * @brief Resume the device.
 *
 * @param hostHandle  Host instance.
 *
 * @return kStatus_USB_Success or error codes.
 *
 */
usb_status_t USB_HostHubResumeDevice(usb_host_handle hostHandle)
{
    usb_host_instance_t *hostInstance;
    usb_host_device_instance_t *deviceInstance;
    usb_status_t status = kStatus_USB_Error;

    if (NULL == hostHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    hostInstance = (usb_host_instance_t *)hostHandle;

    deviceInstance = (usb_host_device_instance_t *)hostInstance->suspendedDevice;
    if (NULL == deviceInstance)
    {
        return kStatus_USB_InvalidHandle;
    }

    status = USB_HostSendHubRequest(
        USB_HostHubGetHubDeviceHandle(hostHandle, deviceInstance->hubNumber)->deviceHandle,
        USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_OTHER,
        USB_REQUEST_STANDARD_CLEAR_FEATURE, PORT_SUSPEND, deviceInstance->portNumber, USB_HostClearHubRequestCallback,
        hostInstance);

    return status;
}
#endif

#endif /* USB_HOST_CONFIG_HUB */
