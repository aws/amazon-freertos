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

#include "usb_host_config.h"
#if (defined(USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI > 0U))
#include "usb_host.h"
#include "usb_host_hci.h"
#include "fsl_device_registers.h"
#include "usb_host_ohci.h"
#include "usb_host_devices.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static usb_status_t USB_HostOhciLinkGtdControlTr(usb_host_ohci_state_struct_t *usbHostState,
                                                 usb_host_ohci_pipe_struct_t *pipe,
                                                 usb_host_transfer_t *tr);
static usb_status_t USB_HostOhciLinkItdTr(usb_host_ohci_state_struct_t *usbHostState,
                                          usb_host_ohci_pipe_struct_t *pipe,
                                          usb_host_transfer_t *tr);
static usb_status_t USB_HostOhciLinkGtdTr(usb_host_ohci_state_struct_t *usbHostState,
                                          usb_host_ohci_pipe_struct_t *pipe,
                                          usb_host_transfer_t *tr);
static usb_status_t USB_HostOhciFreeItd(usb_host_ohci_state_struct_t *usbHostState,
                                        usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd);
static usb_status_t USB_HostOhciFreeGtd(usb_host_ohci_state_struct_t *usbHostState,
                                        usb_host_ohci_general_transfer_descritpor_struct_t *gtd);
static void USB_HostOhciTdDoneHandle(usb_host_ohci_state_struct_t *usbHostState,
                                     usb_host_ohci_pipe_struct_t *pipe,
                                     usb_host_transfer_t *currentTr,
                                     usb_host_ohci_general_transfer_descritpor_struct_t *gtd,
                                     usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* The hcca for interrupt transcation, 256-byte alignment*/
USB_CONTROLLER_DATA USB_RAM_ADDRESS_ALIGNMENT(256) static usb_host_ohci_hcca_struct_t
    s_UsbHostOhciHcca[USB_HOST_CONFIG_OHCI];
USB_CONTROLLER_DATA USB_RAM_ADDRESS_ALIGNMENT(32) static usb_host_ohci_td_struct_t
    s_UsbHostOhciTd[USB_HOST_CONFIG_OHCI];
static usb_host_ohci_state_struct_t s_UsbHostOhciState[USB_HOST_CONFIG_OHCI];

/*******************************************************************************
 * Code
 ******************************************************************************/
static void USB_HostOhciDisableIsr(usb_host_ohci_state_struct_t *usbHostState)
{
    USB_OSA_SR_ALLOC();

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    if (!usbHostState->isrLevel)
    {
        NVIC_DisableIRQ((IRQn_Type)usbHostState->isrNumber);
    }
    usbHostState->isrLevel++;
    USB_OSA_EXIT_CRITICAL();
}

static void USB_HostOhciEnableIsr(usb_host_ohci_state_struct_t *usbHostState)
{
    USB_OSA_SR_ALLOC();

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    usbHostState->isrLevel--;
    if (!usbHostState->isrLevel)
    {
        NVIC_EnableIRQ((IRQn_Type)usbHostState->isrNumber);
    }
    USB_OSA_EXIT_CRITICAL();
}

static void USB_HostOhciDelay(usb_host_ohci_state_struct_t *usbHostState, uint32_t ms)
{
    /* note: the max delay time cannot exceed half of max value (0x4000) */
    int32_t sofStart;
    int32_t SofEnd;
    uint32_t distance;

    sofStart =
        (int32_t)(s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber & USB_HOST_OHCI_FMNUMBER_FN_MASK);

    do
    {
        SofEnd =
            (int32_t)(s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber & USB_HOST_OHCI_FMNUMBER_FN_MASK);
        distance = (uint32_t)(SofEnd - sofStart + USB_HOST_OHCI_FMNUMBER_FN_MASK + 1) & USB_HOST_OHCI_FMNUMBER_FN_MASK;
    } while ((distance) < (ms)); /* compute the distance between sofStart and SofEnd */
}

static usb_status_t USB_HostOhciControlBus(usb_host_ohci_state_struct_t *usbHostState, uint8_t busControl)
{
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    uint32_t portStatus;
#endif
    usb_status_t status = kStatus_USB_Success;

    switch (busControl)
    {
        case kUSB_HostBusReset:
            break;

        case kUSB_HostBusRestart:
            break;

        case kUSB_HostBusEnableAttach:
            usbHostState->controlIsBusy = 0U;
            break;

        case kUSB_HostBusDisableAttach:
            break;

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
        case kUSB_HostBusSuspend:
            if (usbHostState->usbRegBase->HcRhPortStatus[0] & USB_HOST_OHCI_RHPORTSTATUS_CCS_MASK)
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;
                portStatus = usbHostState->usbRegBase->HcRhPortStatus[0];

                portStatus &= ~USB_HOST_OHCI_RHPORTSTATUS_WIC;
                portStatus |= USB_HOST_OHCI_RHPORTSTATUS_PSS_MASK;
                usbHostState->usbRegBase->HcRhPortStatus[0] = portStatus;

                portStatus = usbHostState->usbRegBase->HcControl;
                portStatus &= ~USB_HOST_OHCI_CONTROL_HCFS_MASK;
                portStatus |= USB_HOST_OHCI_CONTROL_HCFS(USB_HOST_OHCI_CONTROL_HCFS_SUSPEND) |
                              USB_HOST_OHCI_CONTROL_RWC_MASK | USB_HOST_OHCI_CONTROL_RWE_MASK;
                usbHostState->usbRegBase->HcControl = portStatus;

                usbHostState->matchTick = hostPointer->hwTick;
                usbHostState->busSuspendStatus = kBus_OhciStartSuspend;

                while ((hostPointer->hwTick - usbHostState->matchTick) <= 3)
                {
                }

                usbHostState->usbRegBase->HcRhStatus |= USB_HOST_OHCI_RHSTATUS_DRWE_MASK;
                /* call host callback function, function is initialized in USB_HostInit */
                hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                            kUSB_HostEventSuspended); /* call host callback function */
                usbHostState->busSuspendStatus = kBus_OhciSuspended;
            }
            else
            {
                status = kStatus_USB_Error;
            }
            break;

        case kUSB_HostBusResume:
            if (usbHostState->usbRegBase->HcRhPortStatus[0] & USB_HOST_OHCI_RHPORTSTATUS_CCS_MASK)
            {
                portStatus = usbHostState->usbRegBase->HcRhPortStatus[0];
                portStatus &= ~(USB_HOST_OHCI_RHPORTSTATUS_WIC);
                portStatus |= USB_HOST_OHCI_RHPORTSTATUS_POCI_MASK;
                usbHostState->usbRegBase->HcRhPortStatus[0] = portStatus;
                usbHostState->busSuspendStatus = kBus_OhciStartResume;
            }
            else
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;

                portStatus = usbHostState->usbRegBase->HcControl;
                portStatus &= ~(USB_HOST_OHCI_CONTROL_HCFS_MASK | USB_HOST_OHCI_CONTROL_RWE_MASK |
                                USB_HOST_OHCI_CONTROL_RWC_MASK);
                portStatus |= USB_HOST_OHCI_CONTROL_HCFS(USB_HOST_OHCI_CONTROL_HCFS_OPERATIONAL);
                usbHostState->usbRegBase->HcControl = portStatus;
                portStatus = usbHostState->usbRegBase->HcRhPortStatus[0];
                portStatus &= ~(USB_HOST_OHCI_RHPORTSTATUS_WIC);
                portStatus |= USB_HOST_OHCI_RHPORTSTATUS_PES_MASK;
                usbHostState->usbRegBase->HcRhPortStatus[0] = portStatus;
                hostPointer->suspendedDevice = NULL;
                usbHostState->busSuspendStatus = kBus_OhciIdle;
                status = kStatus_USB_Error;
            }
            break;
#endif
        default:
            status = kStatus_USB_Error;
            break;
    }
    return status;
}

static uint32_t USB_HostOhciBusTime(uint8_t speed, uint8_t pipeType, uint8_t direction, uint32_t dataLength)
{
    uint32_t result = (3167 + ((1000 * dataLength) * 7U * 8U / 6U)) / 1000;

    if (pipeType == USB_ENDPOINT_ISOCHRONOUS) /* iso */
    {
        if (speed == USB_SPEED_FULL)
        {
            if (direction == USB_IN)
            {
                result = 7268000 + 83540 * result + USB_HOST_OHCI_TIME_DELAY;
            }
            else
            {
                result = 6265000 + 83540 * result + USB_HOST_OHCI_TIME_DELAY;
            }
        }
        else
        {
        }
    }
    else /* interrupt */
    {
        if (speed == USB_SPEED_FULL)
        {
            result = 9107000 + 83540 * result + USB_HOST_OHCI_TIME_DELAY;
        }
        else if (speed == USB_SPEED_LOW)
        {
            if (direction == USB_IN)
            {
                result = 64060000 + 2000 * USB_HOST_OHCI_HUB_LS_SETUP_TIME_DELAY + 676670 * result +
                         USB_HOST_OHCI_TIME_DELAY;
            }
            else
            {
                result = 64107000 + 2000 * USB_HOST_OHCI_HUB_LS_SETUP_TIME_DELAY + 667000 * result +
                         USB_HOST_OHCI_TIME_DELAY;
            }
        }
        else
        {
        }
    }

    result /= 1000000;
    if (result == 0)
    {
        result = 1;
    }

    return result;
}

static usb_status_t USB_HostOhciGetPipe(usb_host_ohci_pipe_struct_t **pipeQueue, usb_host_ohci_pipe_struct_t **pipe)
{
    usb_status_t error = kStatus_USB_Busy;
    USB_OSA_SR_ALLOC();

    /* get a pipe instance */
    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    if (NULL != (*pipeQueue))
    {
        *pipe = *pipeQueue;
        *pipeQueue = (usb_host_ohci_pipe_struct_t *)((*pipe)->pipeCommon.next);
        error = kStatus_USB_Success;
    }
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();
    return error;
}

static usb_status_t USB_HostOhciRemovePipe(usb_host_ohci_pipe_struct_t **pipeQueue, usb_host_ohci_pipe_struct_t *pipe)
{
    usb_host_ohci_pipe_struct_t *p = *pipeQueue;
    usb_host_ohci_pipe_struct_t *pre;

    USB_OSA_SR_ALLOC();

    /* get a pipe instance */
    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    pre = NULL;
    while (NULL != p)
    {
        if (p != pipe)
        {
            pre = p;
            p = (usb_host_ohci_pipe_struct_t *)p->pipeCommon.next;
        }
        else
        {
            if (NULL == pre)
            {
                *pipeQueue = (usb_host_ohci_pipe_struct_t *)p->pipeCommon.next;
            }
            else
            {
                pre->pipeCommon.next = p->pipeCommon.next;
            }
            break;
        }
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciInsertPipe(usb_host_ohci_pipe_struct_t **pipeQueue, usb_host_ohci_pipe_struct_t *pipe)
{
    usb_host_ohci_pipe_struct_t *p = *pipeQueue;

    USB_OSA_SR_ALLOC();

    /* get a pipe instance */
    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    while (NULL != p)
    {
        if (p != pipe)
        {
            p = (usb_host_ohci_pipe_struct_t *)p->pipeCommon.next;
        }
        else
        {
            break;
        }
    }
    if (NULL == p)
    {
        pipe->pipeCommon.next = (usb_host_pipe_t *)(*pipeQueue);
        *pipeQueue = pipe;
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciGetGtd(usb_host_ohci_state_struct_t *usbHostState,
                                       usb_host_ohci_general_transfer_descritpor_struct_t **gtd,
                                       uint32_t count)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *p = NULL;
    uint32_t *tmp;
    USB_OSA_SR_ALLOC();

    /* get a td */
    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    if (count > usbHostState->gtdCount)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Busy;
    }
    *gtd = NULL;
    while ((count) && (NULL != usbHostState->gtdList))
    {
        if (NULL == (*gtd))
        {
            *gtd = usbHostState->gtdList;
        }
        else
        {
            p->NextTD = (uint32_t)usbHostState->gtdList;
        }
        p = usbHostState->gtdList;

        usbHostState->gtdList = (usb_host_ohci_general_transfer_descritpor_struct_t *)usbHostState->gtdList->nextGtd;
        tmp = (uint32_t *)(p);
        usbHostState->gtdCount--;
        for (int i = 0; i < (sizeof(usb_host_ohci_general_transfer_descritpor_struct_t) / sizeof(uint32_t)); i++)
        {
            tmp[i] = 0U;
        }
        count--;
    }
    if (NULL == p)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Busy;
    }
    p->NextTD = 0U;
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciFreeGtd(usb_host_ohci_state_struct_t *usbHostState,
                                        usb_host_ohci_general_transfer_descritpor_struct_t *gtd)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *p;
    USB_OSA_SR_ALLOC();

    if (!gtd)
    {
        return kStatus_USB_Error;
    }

    /* free a td */
    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    p = usbHostState->gtdList;
    while (p)
    {
        if (p == gtd)
        {
            USB_OSA_EXIT_CRITICAL();
            return kStatus_USB_Success;
        }
        p = (usb_host_ohci_general_transfer_descritpor_struct_t *)p->nextGtd;
    }
    gtd->nextGtd = usbHostState->gtdList;
    usbHostState->gtdList = gtd;
    usbHostState->gtdCount++;
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
static usb_status_t USB_HostOhciGetItd(usb_host_ohci_state_struct_t *usbHostState,
                                       usb_host_ohci_isochronous_transfer_descritpor_struct_t **itd,
                                       uint32_t count)
{
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *p = NULL;
    uint32_t *tmp;
    USB_OSA_SR_ALLOC();

    /* free a td */
    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    if (count > usbHostState->itdCount)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Busy;
    }
    *itd = NULL;
    while ((count) && (NULL != usbHostState->itdList))
    {
        if (NULL == (*itd))
        {
            *itd = usbHostState->itdList;
        }
        else
        {
            p->NextTD = (uint32_t)usbHostState->itdList;
        }
        p = usbHostState->itdList;
        usbHostState->itdList = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)usbHostState->itdList->NextTD;
        tmp = (uint32_t *)(p);
        usbHostState->itdCount--;
        for (int i = 0; i < (sizeof(usb_host_ohci_isochronous_transfer_descritpor_struct_t) / sizeof(uint32_t)); i++)
        {
            tmp[i] = 0U;
        }
        count--;
    }
    if (NULL == p)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Busy;
    }
    p->NextTD = 0U;
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciFreeItd(usb_host_ohci_state_struct_t *usbHostState,
                                        usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd)
{
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *p;
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *pre;
    USB_OSA_SR_ALLOC();

    if (!itd)
    {
        return kStatus_USB_Error;
    }
    /* free a td */
    /* Enter critical */
    pre = NULL;
    USB_OSA_ENTER_CRITICAL();
    p = usbHostState->itdList;
    while (p)
    {
        if (p == itd)
        {
            USB_OSA_EXIT_CRITICAL();
            return kStatus_USB_Success;
        }
        pre = p;
        p = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)p->NextTD;
    }
    if (NULL == pre)
    {
        usbHostState->itdList = itd;
    }
    else
    {
        pre->NextTD = (uint32_t)itd;
    }
    itd->NextTD = 0U;
    usbHostState->itdCount++;
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}
#endif

static void USB_HostOhciLinkTdToDoneList(usb_host_ohci_state_struct_t *usbHostState)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *tdList;
    usb_host_ohci_general_transfer_descritpor_struct_t *tdListPos;
    tdList = (usb_host_ohci_general_transfer_descritpor_struct_t
                  *)(s_UsbHostOhciHcca[usbHostState->controllerId].HccaDoneHead & USB_HOST_OHCI_HD_MASK);

    while (tdList)
    {
        tdListPos = (usb_host_ohci_general_transfer_descritpor_struct_t *)(tdList->NextTD & USB_HOST_OHCI_HD_MASK);
        {
            if (usbHostState->tdDoneListTail)
            {
                usbHostState->tdDoneListTail->NextTD = (uint32_t)tdList;
            }
            else
            {
                usbHostState->tdDoneListHead = tdList;
            }
            usbHostState->tdDoneListTail = tdList;
        }
        tdList = tdListPos;
    }
    if (usbHostState->tdDoneListTail)
    {
        usbHostState->tdDoneListTail->NextTD = 0U;
    }
    usbHostState->writeBackCount++;
    s_UsbHostOhciHcca[usbHostState->controllerId].HccaDoneHead = 0U;
}

static usb_status_t USB_HostOhciPortChange(usb_host_ohci_state_struct_t *usbHostState)
{
    uint32_t portStatus;
    int i;
    uint32_t portCount = (usbHostState->usbRegBase->HcRhDescriptorA & USB_HOST_OHCI_RHDESCRIPTORA_NDP_MASK);

    for (i = 0; i < portCount; i++)
    {
        portStatus = usbHostState->usbRegBase->HcRhPortStatus[i];
        if (portStatus & USB_HOST_OHCI_RHPORTSTATUS_CSC_MASK)
        {
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
            if (kBus_OhciIdle != usbHostState->busSuspendStatus)
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;
                portStatus = usbHostState->usbRegBase->HcRhPortStatus[i];
                portStatus &= ~USB_HOST_OHCI_RHPORTSTATUS_WIC;
                usbHostState->usbRegBase->HcRhPortStatus[i] = portStatus | USB_HOST_OHCI_RHPORTSTATUS_PSSC_MASK;
                while ((usbHostState->usbRegBase->HcRhPortStatus[i] & USB_HOST_OHCI_RHPORTSTATUS_PSS_MASK))
                {
                }
                portStatus = usbHostState->usbRegBase->HcControl;
                portStatus &= ~(USB_HOST_OHCI_CONTROL_HCFS_MASK | USB_HOST_OHCI_CONTROL_RWE_MASK |
                                USB_HOST_OHCI_CONTROL_RWC_MASK);
                portStatus |= USB_HOST_OHCI_CONTROL_HCFS(USB_HOST_OHCI_CONTROL_HCFS_OPERATIONAL);
                usbHostState->usbRegBase->HcControl = portStatus;
                portStatus = usbHostState->usbRegBase->HcRhPortStatus[i];
                portStatus &= ~(USB_HOST_OHCI_RHPORTSTATUS_WIC);
                portStatus |= USB_HOST_OHCI_RHPORTSTATUS_PES_MASK;
                usbHostState->usbRegBase->HcRhPortStatus[i] = portStatus;
                hostPointer->suspendedDevice = NULL;
                usbHostState->busSuspendStatus = kBus_OhciIdle;
            }
#endif
            uint32_t sofStart =
                s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber & USB_HOST_OHCI_FMNUMBER_FN_MASK;
            uint32_t sof;
            while (1)
            {
                portStatus = usbHostState->usbRegBase->HcRhPortStatus[i];
                if (portStatus & USB_HOST_OHCI_RHPORTSTATUS_CSC_MASK)
                {
                    portStatus &= ~USB_HOST_OHCI_RHPORTSTATUS_WIC;
                    usbHostState->usbRegBase->HcRhPortStatus[i] = portStatus | USB_HOST_OHCI_RHPORTSTATUS_CSC_MASK;
                }
                sof = s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber & USB_HOST_OHCI_FMNUMBER_FN_MASK;
                if ((((sof + USB_HOST_OHCI_FMNUMBER_FN_MASK + 1) - sofStart) & USB_HOST_OHCI_FMNUMBER_FN_MASK) > 1)
                {
                    break;
                }
            }
        }

        portStatus = usbHostState->usbRegBase->HcRhPortStatus[i];

        if (portStatus & USB_HOST_OHCI_RHPORTSTATUS_CCS_MASK)
        {
            int index;
            if (kUSB_DeviceOhciPortDetached != usbHostState->portState[i].portStatus)
            {
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
                if ((kBus_OhciSuspended == usbHostState->busSuspendStatus) ||
                    (kBus_OhciStartResume == usbHostState->busSuspendStatus))
                {
                    if (portStatus & USB_HOST_OHCI_RHPORTSTATUS_PSSC_MASK)
                    {
                        usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;
                        usbHostState->busSuspendStatus = kBus_OhciStartResume;
                        /* call host callback function, function is initialized in USB_HostInit */
                        hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                    kUSB_HostEventDetectResume); /* call host callback function */
                        portStatus &= ~USB_HOST_OHCI_RHPORTSTATUS_WIC;
                        usbHostState->usbRegBase->HcRhPortStatus[i] = portStatus | USB_HOST_OHCI_RHPORTSTATUS_PSSC_MASK;
                        while ((usbHostState->usbRegBase->HcRhPortStatus[i] & USB_HOST_OHCI_RHPORTSTATUS_PSS_MASK))
                        {
                        }
                        portStatus = usbHostState->usbRegBase->HcControl;
                        portStatus &= ~(USB_HOST_OHCI_CONTROL_HCFS_MASK | USB_HOST_OHCI_CONTROL_RWE_MASK |
                                        USB_HOST_OHCI_CONTROL_RWC_MASK);
                        portStatus |= USB_HOST_OHCI_CONTROL_HCFS(USB_HOST_OHCI_CONTROL_HCFS_OPERATIONAL);
                        usbHostState->usbRegBase->HcControl = portStatus;
                        portStatus = usbHostState->usbRegBase->HcRhPortStatus[i];
                        portStatus &= ~(USB_HOST_OHCI_RHPORTSTATUS_WIC);
                        portStatus |= USB_HOST_OHCI_RHPORTSTATUS_PES_MASK;
                        usbHostState->usbRegBase->HcRhPortStatus[i] = portStatus;
                        /* call host callback function, function is initialized in USB_HostInit */
                        hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                    kUSB_HostEventResumed); /* call host callback function */
                        hostPointer->suspendedDevice = NULL;
                        usbHostState->busSuspendStatus = kBus_OhciIdle;
                    }
                }
#endif
                break;
            }
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
            usbHostState->busSuspendStatus = kBus_OhciIdle;
#endif
            for (index = 0; index < USB_HOST_OHCI_PORT_CONNECT_DEBOUNCE_DELAY; index++)
            {
                USB_HostOhciDelay(usbHostState, 1);
                if (!(usbHostState->usbRegBase->HcRhPortStatus[i] & USB_HOST_OHCI_RHPORTSTATUS_CCS_MASK))
                {
                    break;
                }
            }

            portStatus = usbHostState->usbRegBase->HcRhPortStatus[i];

            /* CCS is cleared ?*/
            if ((!(portStatus & USB_HOST_OHCI_RHPORTSTATUS_CCS_MASK)) ||
                (index < USB_HOST_OHCI_PORT_CONNECT_DEBOUNCE_DELAY))
            {
                usbHostState->portState[i].portStatus = kUSB_DeviceOhciPortDetached;
                continue;
            }

            portStatus &= ~USB_HOST_OHCI_RHPORTSTATUS_WIC;
            usbHostState->usbRegBase->HcRhPortStatus[i] = portStatus | USB_HOST_OHCI_RHPORTSTATUS_PRS_MASK;

            while (usbHostState->usbRegBase->HcRhPortStatus[i] & USB_HOST_OHCI_RHPORTSTATUS_PRS_MASK)
            {
                __ASM("nop");
            }

            usbHostState->portState[i].portSpeed =
                (usbHostState->usbRegBase->HcRhPortStatus[i] & USB_HOST_OHCI_RHPORTSTATUS_LSDA_MASK) ? USB_SPEED_LOW :
                                                                                                       USB_SPEED_FULL;
            usbHostState->portState[i].portStatus = kUSB_DeviceOhciPortPhyAttached;
            USB_OsaEventSet(usbHostState->ohciEvent, USB_HOST_OHCI_EVENT_ATTACH);
        }
        else
        {
            if (kUSB_DeviceOhciPortDetached == usbHostState->portState[i].portStatus)
            {
                continue;
            }
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
            usbHostState->busSuspendStatus = kBus_OhciIdle;
#endif
            usbHostState->portState[i].portStatus = kUSB_DeviceOhciPortPhyDetached;
            USB_OsaEventSet(usbHostState->ohciEvent, USB_HOST_OHCI_EVENT_DETACH);
        }
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciFindStartFrame(usb_host_ohci_state_struct_t *usbHostState,
                                               usb_host_ohci_pipe_struct_t *pipe)
{
    usb_host_ohci_pipe_struct_t *p;
    uint32_t startFrame;
    uint32_t frame;
    uint32_t total = 0U;

    pipe->startFrame = 0U;

    if (NULL == usbHostState->pipeListInUsing)
    {
        return kStatus_USB_Success;
    }

    for (startFrame = 0U; startFrame < pipe->pipeCommon.interval; startFrame++)
    {
        for (frame = startFrame; frame < USB_HOST_OHCI_HCCA_SIZE; frame++)
        {
            total = 0U;
            for (p = usbHostState->pipeListInUsing; NULL != p; p = (usb_host_ohci_pipe_struct_t *)p->pipeCommon.next)
            {
                if ((frame >= p->startFrame) && (0U == ((frame - p->startFrame) % p->pipeCommon.interval)))
                {
                    total += p->busTime;
                }
            }
            if (total > USB_HOST_OHCI_PERIODIC_BANDWIDTH)
            {
                break;
            }
        }
        if (frame >= USB_HOST_OHCI_HCCA_SIZE)
        {
            pipe->startFrame = startFrame;
            return kStatus_USB_Success;
        }
    }
    return kStatus_USB_BandwidthFail;
}

static usb_status_t USB_HostOhciAddToPeriodicList(usb_host_ohci_state_struct_t *usbHostState,
                                                  usb_host_ohci_pipe_struct_t *pipe)
{
    uint32_t i;
    uint32_t dummy = (uint32_t)&s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_PERIODIC_ED_DUMMY];
    usb_host_ohci_endpoint_descritpor_struct_t *ed;
    usb_host_ohci_endpoint_descritpor_struct_t *pre;

    for (i = pipe->startFrame; i < USB_HOST_OHCI_HCCA_SIZE; i += pipe->pipeCommon.interval)
    {
        ed = (usb_host_ohci_endpoint_descritpor_struct_t *)s_UsbHostOhciHcca[usbHostState->controllerId]
                 .HccaInterrruptTable[i];
        pre = ed;

        while (NULL != pre)
        {
            if (pre == pipe->ed)
            {
                break;
            }
            pre = (usb_host_ohci_endpoint_descritpor_struct_t *)pre->NextED;
        }

        if (NULL != pre)
        {
            continue;
        }

        pre = NULL;
        while (dummy != (uint32_t)ed)
        {
            if (ed->pipe->pipeCommon.interval <= pipe->pipeCommon.interval)
            {
                if ((USB_ENDPOINT_ISOCHRONOUS == pipe->pipeCommon.pipeType) && (dummy != (uint32_t)ed))
                {
                }
                else
                {
                    break;
                }
            }
            pre = ed;
            ed = (usb_host_ohci_endpoint_descritpor_struct_t *)ed->NextED;
        }
        pipe->ed->NextED = (uint32_t)ed;
        if (NULL != pre)
        {
            pre->NextED = (uint32_t)pipe->ed;
        }
        else
        {
            s_UsbHostOhciHcca[usbHostState->controllerId].HccaInterrruptTable[i] = (uint32_t)pipe->ed;
        }
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciOpenControlBulkPipe(usb_host_ohci_state_struct_t *usbHostState,
                                                    usb_host_ohci_pipe_struct_t *pipe)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *gtd;
    usb_status_t error;

    error = USB_HostOhciGetGtd(usbHostState, &gtd, 1);
    if (kStatus_USB_Success != error)
    {
        return error;
    }

    gtd->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
    gtd->stateUnion.stateBitField.R = 1U;

    pipe->ed->TailP = (uint32_t)gtd;
    pipe->ed->HeadP = (uint32_t)gtd;
    if (USB_ENDPOINT_CONTROL == pipe->pipeCommon.pipeType)
    {
        pipe->ed->NextED = s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_CONTROL_ED_DUMMY].NextED;
        s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_CONTROL_ED_DUMMY].NextED = (uint32_t)pipe->ed;
        usbHostState->usbRegBase->HcControl |= USB_HOST_OHCI_CONTROL_CLE_MASK;
    }
    else
    {
        pipe->ed->NextED = s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_BULK_ED_DUMMY].NextED;
        s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_BULK_ED_DUMMY].NextED = (uint32_t)pipe->ed;
        usbHostState->usbRegBase->HcControl |= USB_HOST_OHCI_CONTROL_BLE_MASK;
    }

    return kStatus_USB_Success;
}

#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
static usb_status_t USB_HostOhciOpenIsoPipe(usb_host_ohci_state_struct_t *usbHostState,
                                            usb_host_ohci_pipe_struct_t *pipe)
{
#if (defined(FSL_FEATURE_USBFSH_VERSION) && (FSL_FEATURE_USBFSH_VERSION >= 200U))
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd;
#endif
    usb_status_t error;
    USB_OSA_SR_ALLOC();

    pipe->busTime = USB_HostOhciBusTime(((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed,
                                        pipe->pipeCommon.pipeType, pipe->pipeCommon.direction,
                                        pipe->pipeCommon.maxPacketSize * pipe->pipeCommon.numberPerUframe);

    USB_OSA_ENTER_CRITICAL();
    error = USB_HostOhciFindStartFrame(usbHostState, pipe);
    if (kStatus_USB_Success != error)
    {
        USB_OSA_EXIT_CRITICAL();
        return error;
    }

#if (defined(FSL_FEATURE_USBFSH_VERSION) && (FSL_FEATURE_USBFSH_VERSION >= 200U))
    error = USB_HostOhciGetItd(usbHostState, &itd, 1);
    if (kStatus_USB_Success != error)
    {
        USB_OSA_EXIT_CRITICAL();
        return error;
    }
    itd->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;

    pipe->ed->TailP = (uint32_t)itd;
    pipe->ed->HeadP = (uint32_t)itd;
#else
    pipe->ed->TailP = (uint32_t)0;
    pipe->ed->HeadP = (uint32_t)0;
#endif

    usbHostState->usbRegBase->HcControl &= ~(USB_HOST_OHCI_CONTROL_PLE_MASK | USB_HOST_OHCI_CONTROL_IE_MASK);

    USB_HostOhciAddToPeriodicList(usbHostState, pipe);

    usbHostState->usbRegBase->HcControl |= USB_HOST_OHCI_CONTROL_PLE_MASK | USB_HOST_OHCI_CONTROL_IE_MASK;

    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}
#endif

static usb_status_t USB_HostOhciOpenInterruptPipe(usb_host_ohci_state_struct_t *usbHostState,
                                                  usb_host_ohci_pipe_struct_t *pipe)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *gtd;
    usb_status_t error;
    USB_OSA_SR_ALLOC();

    pipe->busTime = USB_HostOhciBusTime(((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed,
                                        pipe->pipeCommon.pipeType, pipe->pipeCommon.direction,
                                        pipe->pipeCommon.maxPacketSize * pipe->pipeCommon.numberPerUframe);

    USB_OSA_ENTER_CRITICAL();
    error = USB_HostOhciFindStartFrame(usbHostState, pipe);
    if (kStatus_USB_Success != error)
    {
        USB_OSA_EXIT_CRITICAL();
        return error;
    }

    error = USB_HostOhciGetGtd(usbHostState, &gtd, 1);
    if (kStatus_USB_Success != error)
    {
        USB_OSA_EXIT_CRITICAL();
        return error;
    }
    gtd->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
    gtd->stateUnion.stateBitField.R = 1U;

    pipe->ed->TailP = (uint32_t)gtd;
    pipe->ed->HeadP = (uint32_t)gtd;

    USB_HostOhciAddToPeriodicList(usbHostState, pipe);
    USB_OSA_EXIT_CRITICAL();

    usbHostState->usbRegBase->HcControl |= USB_HOST_OHCI_CONTROL_PLE_MASK;

    return kStatus_USB_Success;
}

static void USB_HostOhciTdDoneHandle(usb_host_ohci_state_struct_t *usbHostState,
                                     usb_host_ohci_pipe_struct_t *pipe,
                                     usb_host_transfer_t *currentTr,
                                     usb_host_ohci_general_transfer_descritpor_struct_t *gtd,
                                     usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd)
{
    usb_host_transfer_t *trPos;
    usb_host_transfer_t *trPre;
    usb_host_ohci_pipe_struct_t *p;
    uint32_t startFrame;
    uint32_t currentFrame;

    pipe->isDone = 0U;
    if (NULL == currentTr)
    {
#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
        if (itd)
        {
            itd->stateUnion.state = 0U;
            itd->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
            itd->NextTD = 0U;
            itd->BE = 0U;
            itd->BP0 = 0U;
            USB_HostOhciFreeItd(usbHostState, itd);
        }
        else
#endif
        {
            gtd->stateUnion.state = 0U;
            gtd->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
            gtd->stateUnion.stateBitField.R = 1U;
            gtd->CBP = 0U;
            gtd->NextTD = 0U;
            gtd->BE = 0U;
        }
        return;
    }

    if (USB_HOST_OHCI_CC_NO_ERROR != gtd->stateUnion.stateBitField.CC)
    {
        if (kStatus_USB_Success == currentTr->union2.frame)
        {
            currentTr->union2.frame = kStatus_USB_TransferFailed;
        }

        if (pipe->ed->HeadP & USB_HOST_OHCI_ED_HEADP_HALT_MASK)
        {
            currentTr->union2.frame = kStatus_USB_TransferStall;
        }
        pipe->ed->HeadP &= ~USB_HOST_OHCI_ED_HEADP_HALT_MASK;
    }

#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
    if (itd)
    {
        usb_host_ohci_isochronous_transfer_descritpor_struct_t *itdP;
        usb_host_ohci_isochronous_transfer_descritpor_struct_t *itdQ = NULL;
        itdP = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)currentTr->union1.unitHead;

        while (itdP)
        {
            if (itd == itdP)
            {
                if (itdQ)
                {
                    itdQ->nextItd = itdP->nextItd;
                }
                else
                {
                    currentTr->union1.unitHead = (uint32_t)itdP->nextItd;
                }
                break;
            }
            itdQ = itdP;
            itdP = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)itdP->nextItd;
        }
        USB_HostOhciFreeItd(usbHostState, itd);
    }
    else
#endif
    {
        usb_host_ohci_general_transfer_descritpor_struct_t *gtdP;
        usb_host_ohci_general_transfer_descritpor_struct_t *gtdQ = NULL;

        gtdP = (usb_host_ohci_general_transfer_descritpor_struct_t *)currentTr->union1.unitHead;
        while (gtdP)
        {
            if (gtd == gtdP)
            {
                if (gtdQ)
                {
                    gtdQ->nextGtd = gtdP->nextGtd;
                }
                else
                {
                    uint32_t tail = pipe->ed->TailP & USB_HOST_OHCI_ED_TAILP_MASK;
                    uint32_t head = pipe->ed->HeadP & USB_HOST_OHCI_ED_HEADP_MASK;
                    currentTr->union1.unitHead = (uint32_t)gtdP->nextGtd;
                    if (head != tail)
                    {
                        pipe->ed->HeadP &= ~USB_HOST_OHCI_ED_HEADP_MASK;
                        pipe->ed->HeadP |= (uint32_t)gtdP->NextTD;
                    }
                }
                break;
            }
            gtdQ = gtdP;
            gtdP = (usb_host_ohci_general_transfer_descritpor_struct_t *)gtdP->nextGtd;
        }
        if ((gtd->CBP) && (USB_HOST_OHCI_GTD_DP_SETUP != gtd->stateUnion.stateBitField.DP))
        {
            currentTr->transferLength -= ((((gtd->BE) ^ (gtd->CBP)) & 0xFFFFF000U) ? 0x00001000U : 0U) +
                                         ((gtd->BE) & 0x00000FFFU) - ((gtd->CBP) & 0x00000FFFU) + 1U;
        }
        currentTr->transferSofar = currentTr->transferLength;
        USB_HostOhciFreeGtd(usbHostState, gtd);
    }

    if (NULL == (void *)(currentTr->union1.unitHead))
    {
        if (!pipe->isCanceling)
        {
            if (USB_ENDPOINT_CONTROL == pipe->pipeCommon.pipeType)
            {
                if (pipe->ed->trListHead)
                {
                    USB_HostOhciLinkGtdControlTr(usbHostState, pipe, pipe->ed->trListHead);
                }
            }
        }
    }

    if (NULL == (void *)(currentTr->union1.unitHead))
    {
        uint8_t trIsFound = 0;
        trPre = NULL;
        trPos = pipe->ed->trListHead;

        while (trPos)
        {
            if (currentTr == trPos)
            {
                if (NULL == trPre)
                {
                    pipe->ed->trListHead = trPos->next;
                }
                else
                {
                    trPre->next = trPos->next;
                }
                trIsFound = 1;
                break;
            }
            trPre = trPos;
            trPos = trPos->next;
        }
        if (NULL == pipe->ed->trListHead)
        {
            pipe->ed->trListTail = NULL;
        }

        if (trIsFound)
        {
            pipe->cutOffTime = USB_HOST_OHCI_TRANSFER_TIMEOUT_GAP;

            if ((kStatus_USB_Success == ((usb_status_t)currentTr->union2.frame)) &&
                (USB_ENDPOINT_CONTROL == pipe->pipeCommon.pipeType) &&
                (USB_REQUEST_STANDARD_CLEAR_FEATURE == currentTr->setupPacket->bRequest) &&
                (USB_REQUEST_TYPE_RECIPIENT_ENDPOINT == currentTr->setupPacket->bmRequestType) &&
                (USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT ==
                 (USB_SHORT_FROM_LITTLE_ENDIAN_DATA(currentTr->setupPacket->wValue) & 0x00FFu)))
            {
                p = usbHostState->pipeListInUsing;
                while (p != NULL)
                {
                    /* only compute bulk and interrupt pipe */
                    if (((p->pipeCommon.endpointAddress |
                          (p->pipeCommon.direction << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)) ==
                         ((uint8_t)(USB_SHORT_FROM_LITTLE_ENDIAN_DATA(currentTr->setupPacket->wIndex)))) &&
                        (p->pipeCommon.deviceHandle == pipe->pipeCommon.deviceHandle))
                    {
                        break;
                    }
                    p = (usb_host_ohci_pipe_struct_t *)p->pipeCommon.next;
                }

                if ((NULL != p) && ((p->pipeCommon.pipeType == USB_ENDPOINT_INTERRUPT) ||
                                    (p->pipeCommon.pipeType == USB_ENDPOINT_BULK)))
                {
                    p->ed->stateUnion.stateBitField.K = 1U;
                    startFrame = s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber;
                    currentFrame = startFrame;

                    while (currentFrame == startFrame)
                    {
                        currentFrame = s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber;
                    }

                    p->ed->HeadP &= ~USB_HOST_OHCI_ED_HEADP_CARRY_MASK;

                    p->ed->stateUnion.stateBitField.K = 0U;
                }
            }

            currentTr->callbackFn(currentTr->callbackParam, currentTr,
                                  (usb_status_t)currentTr->union2.frame); /* transfer callback */
            if (USB_ENDPOINT_CONTROL == pipe->pipeCommon.pipeType)
            {
                usbHostState->controlIsBusy = 0U;
            }
            pipe->isBusy = 0U;
        }
        else
        {
        }

        if (0U == usbHostState->controlIsBusy)
        {
            usb_host_ohci_pipe_struct_t *p = usbHostState->pipeListInUsing;

            while (p)
            {
                if (USB_ENDPOINT_CONTROL == p->pipeCommon.pipeType)
                {
                    if (p->ed->trListHead)
                    {
                        /* initialize gtd for control/bulk/interrupt transfer */
                        if (kStatus_USB_Success == USB_HostOhciLinkGtdControlTr(usbHostState, p, p->ed->trListHead))
                        {
                            usbHostState->controlIsBusy = 1U;
                            break;
                        }
                    }
                }
                p = (usb_host_ohci_pipe_struct_t *)p->pipeCommon.next;
            }
        }

        if (pipe->ed->trListHead)
        {
            switch (pipe->pipeCommon.pipeType)
            {
                case USB_ENDPOINT_CONTROL:
                    break;
                case USB_ENDPOINT_BULK:
                case USB_ENDPOINT_INTERRUPT:
                    /* initialize gtd for control/bulk/interrupt transfer */
                    USB_HostOhciLinkGtdTr(usbHostState, pipe, pipe->ed->trListHead);
                    break;
#if ((defined USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD))
                case USB_ENDPOINT_ISOCHRONOUS:
                    /* initialize itd for iso transfer */
                    /* USB_HostOhciLinkItdTr(usbHostState, pipe, pipe->ed->trListHead); */
                    break;
#endif
                default:
                    break;
            }
        }
    }
}

static usb_status_t USB_HostOhciCancelPipe(usb_host_ohci_state_struct_t *usbHostState,
                                           usb_host_ohci_pipe_struct_t *pipe,
                                           usb_host_transfer_t *tr)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *gtd;
    usb_host_ohci_general_transfer_descritpor_struct_t *gtdPre;
    usb_host_ohci_general_transfer_descritpor_struct_t *gtdPos;
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd;
    usb_host_ohci_pipe_struct_t *currentPipe;
    usb_host_transfer_t *trCurrent;
    usb_host_transfer_t *trPos;
    usb_host_transfer_t *trPre;
    uint32_t startFrame;
    uint32_t currentFrame;

    pipe->ed->stateUnion.stateBitField.K = 1U;
    startFrame = s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber;
    currentFrame = startFrame;

    while (currentFrame == startFrame)
    {
        currentFrame = s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber;
    }

    USB_HostOhciLock();
    USB_HostOhciDisableIsr(usbHostState);
    pipe->isCanceling = 1U;
    pipe->ed->HeadP &= ~USB_HOST_OHCI_ED_HEADP_MASK;
    pipe->ed->HeadP |= pipe->ed->TailP;
    /* Fetch the td from token done list */
    gtd = (usb_host_ohci_general_transfer_descritpor_struct_t *)((uint32_t)usbHostState->tdDoneListHead);
    gtdPre = NULL;
    while (gtd)
    {
        if (((uint32_t)gtd) < ((uint32_t)&s_UsbHostOhciTd[usbHostState->controllerId].gtd[0]))
        {
            itd = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)gtd;
            trCurrent = itd->tr;
            currentPipe = itd->pipe;
        }
        else
        {
            itd = NULL;
            trCurrent = gtd->tr;
            currentPipe = gtd->pipe;
        }
        if (pipe == currentPipe)
        {
            if ((NULL == tr) || (tr == trCurrent))
            {
                gtdPos = (usb_host_ohci_general_transfer_descritpor_struct_t *)gtd->NextTD;
                USB_HostOhciTdDoneHandle(usbHostState, pipe, trCurrent, gtd, itd);
                if (NULL == gtdPre)
                {
                    usbHostState->tdDoneListHead = gtdPos;
                }
                else
                {
                    gtdPre->NextTD = (uint32_t)gtdPos;
                }
                gtd = gtdPos;
                continue;
            }
        }
        gtdPre = gtd;
        gtd = (usb_host_ohci_general_transfer_descritpor_struct_t *)gtd->NextTD;
    }

    trPre = NULL;
    trCurrent = pipe->ed->trListHead;

    while (NULL != trCurrent)
    {
        trPos = trCurrent->next;
        if ((NULL == tr) || (tr == trCurrent))
        {
            if (trCurrent->union1.unitHead)
            {
                while (trCurrent->union1.unitHead)
                {
                    gtd = (usb_host_ohci_general_transfer_descritpor_struct_t *)trCurrent->union1.unitHead;
                    if (USB_ENDPOINT_ISOCHRONOUS == pipe->pipeCommon.pipeType)
                    {
                        itd = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)gtd;
                    }
                    else
                    {
                        itd = NULL;
                    }
                    USB_HostOhciTdDoneHandle(usbHostState, pipe, trCurrent, gtd, itd);
                }
            }
            else
            {
                pipe->cutOffTime = USB_HOST_OHCI_TRANSFER_TIMEOUT_GAP;

                trCurrent->callbackFn(trCurrent->callbackParam, trCurrent,
                                      kStatus_USB_TransferCancel); /* transfer callback */

                pipe->isBusy = 0U;
            }
            if (NULL == trPre)
            {
                pipe->ed->trListHead = trPos;
            }
            else
            {
                trPre->next = trPos;
            }
        }
        else
        {
            trPre = trCurrent;
        }
        trCurrent = trPos;
    }
    if (NULL == pipe->ed->trListHead)
    {
        pipe->ed->trListTail = NULL;
    }
    pipe->isCanceling = 0U;
    USB_HostOhciEnableIsr(usbHostState);
    USB_HostOhciUnlock();
    pipe->ed->stateUnion.stateBitField.K = 0U;

    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciRemoveFromPeriodicList(usb_host_ohci_state_struct_t *usbHostState,
                                                       usb_host_ohci_pipe_struct_t *pipe)
{
    uint32_t i;
    usb_host_ohci_endpoint_descritpor_struct_t *ed;
    usb_host_ohci_endpoint_descritpor_struct_t *pre;

    for (i = pipe->startFrame; i < USB_HOST_OHCI_HCCA_SIZE; i += pipe->pipeCommon.interval)
    {
        pre = NULL;
        ed = (usb_host_ohci_endpoint_descritpor_struct_t *)s_UsbHostOhciHcca[usbHostState->controllerId]
                 .HccaInterrruptTable[i];

        while (NULL != ed)
        {
            if (ed == pipe->ed)
            {
                if (NULL == pre)
                {
                    s_UsbHostOhciHcca[usbHostState->controllerId].HccaInterrruptTable[i] = ed->NextED;
                }
                else
                {
                    pre->NextED = ed->NextED;
                }
                break;
            }
            pre = ed;
            ed = (usb_host_ohci_endpoint_descritpor_struct_t *)ed->NextED;
        }
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciCloseControlBulkPipe(usb_host_ohci_state_struct_t *usbHostState,
                                                     usb_host_ohci_pipe_struct_t *pipe)
{
    usb_host_ohci_endpoint_descritpor_struct_t *edHead;
    usb_host_ohci_endpoint_descritpor_struct_t *preEdHead;

    USB_HostOhciCancelPipe(usbHostState, pipe, NULL);

    pipe->ed->stateUnion.stateBitField.K = 1U;

    if (USB_ENDPOINT_CONTROL == pipe->pipeCommon.pipeType)
    {
        edHead = &s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_CONTROL_ED_DUMMY];
    }
    else
    {
        edHead = &s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_BULK_ED_DUMMY];
    }

    USB_HostOhciFreeGtd(
        usbHostState,
        (usb_host_ohci_general_transfer_descritpor_struct_t *)(pipe->ed->TailP & USB_HOST_OHCI_ED_HEADP_MASK));

    preEdHead = NULL;
    while ((edHead) && (edHead != pipe->ed))
    {
        preEdHead = edHead;
        edHead = (usb_host_ohci_endpoint_descritpor_struct_t *)edHead->NextED;
    }

    if ((NULL != preEdHead) && (NULL != edHead))
    {
        preEdHead->NextED = edHead->NextED;
    }

    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciCloseIsoInterruptPipe(usb_host_ohci_state_struct_t *usbHostState,
                                                      usb_host_ohci_pipe_struct_t *pipe)
{
    USB_HostOhciCancelPipe(usbHostState, pipe, NULL);

    pipe->ed->stateUnion.stateBitField.K = 1U;
    if (USB_ENDPOINT_ISOCHRONOUS == pipe->pipeCommon.pipeType)
    {
        USB_HostOhciFreeItd(
            usbHostState,
            (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)(pipe->ed->TailP & USB_HOST_OHCI_ED_HEADP_MASK));
    }
    else
    {
        USB_HostOhciFreeGtd(
            usbHostState,
            (usb_host_ohci_general_transfer_descritpor_struct_t *)(pipe->ed->TailP & USB_HOST_OHCI_ED_HEADP_MASK));
    }

    USB_HostOhciRemoveFromPeriodicList(usbHostState, pipe);

    return kStatus_USB_Success;
}

static void USB_HostOhciFillGtd(usb_host_ohci_pipe_struct_t *pipe,
                                usb_host_transfer_t *tr,
                                usb_host_ohci_general_transfer_descritpor_struct_t *gtd,
                                uint8_t *buffer,
                                uint32_t length,
                                uint8_t dirPid,
                                uint8_t dataToggle,
                                uint8_t ioc)
{
    gtd->CBP = (uint32_t)buffer;
    gtd->BE = (length) ? (gtd->CBP + length - 1U) : 0U;
    gtd->pipe = pipe;
    gtd->tr = tr;
    gtd->stateUnion.stateBitField.R = 1U;
    gtd->stateUnion.stateBitField.DP = dirPid;
    gtd->stateUnion.stateBitField.DI = ioc ? 0 : USB_HOST_OHCI_GTD_DI_NO_INTERRUPT;
    gtd->stateUnion.stateBitField.T = dataToggle;
    gtd->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
    gtd->length = length;
}

static usb_status_t USB_HostOhciLinkGtdControlTr(usb_host_ohci_state_struct_t *usbHostState,
                                                 usb_host_ohci_pipe_struct_t *pipe,
                                                 usb_host_transfer_t *tr)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *head;
    usb_host_ohci_general_transfer_descritpor_struct_t *p = NULL;
    usb_status_t error = kStatus_USB_Error;
    uint8_t dirPid = USB_HOST_OHCI_GTD_DP_SETUP;
    uint8_t statePid = 0U;

    USB_OSA_SR_ALLOC();

    USB_OSA_ENTER_CRITICAL();
    if ((pipe->isBusy) && (kStatus_UsbHostOhci_Idle == tr->setupStatus))
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Success;
    }
    pipe->isBusy = 1;
    pipe->isDone = 0U;
    USB_OSA_EXIT_CRITICAL();

    switch (tr->setupStatus)
    {
        case kStatus_UsbHostOhci_Idle:
            error = USB_HostOhciGetGtd(usbHostState, &p, 1);
            if (kStatus_USB_Success != error)
            {
                USB_OSA_ENTER_CRITICAL();
                pipe->isBusy = 0;
                USB_OSA_EXIT_CRITICAL();
                return error;
            }
            head = (usb_host_ohci_general_transfer_descritpor_struct_t *)pipe->ed->TailP;
            head->NextTD = (uint32_t)p;
            p = head;
            tr->union1.unitHead = (uint32_t)p;
            USB_HostOhciFillGtd(pipe, tr, p, (uint8_t *)tr->setupPacket, 8U, USB_HOST_OHCI_GTD_DP_SETUP, 0x02U, 0U);
            if (tr->transferLength)
            {
                tr->setupStatus = kStatus_UsbHostOhci_Setup;
            }
            else
            {
                tr->setupStatus = kStatus_UsbHostOhci_Data;
            }
            break;
        case kStatus_UsbHostOhci_Setup:
            error = USB_HostOhciGetGtd(usbHostState, &p, 1);
            if (kStatus_USB_Success != error)
            {
                USB_OSA_ENTER_CRITICAL();
                pipe->isBusy = 0;
                USB_OSA_EXIT_CRITICAL();
                return error;
            }

            head = (usb_host_ohci_general_transfer_descritpor_struct_t *)pipe->ed->TailP;
            head->NextTD = (uint32_t)p;
            p = head;
            tr->union1.unitHead = (uint32_t)p;
            dirPid = (tr->direction == USB_OUT) ? USB_HOST_OHCI_GTD_DP_OUT : USB_HOST_OHCI_GTD_DP_IN;
            USB_HostOhciFillGtd(pipe, tr, p, (uint8_t *)tr->transferBuffer, tr->transferLength, dirPid, 0x03U, 1U);
            tr->setupStatus = kStatus_UsbHostOhci_Data;
            break;
        case kStatus_UsbHostOhci_Data:
            error = USB_HostOhciGetGtd(usbHostState, &p, 1);
            if (kStatus_USB_Success != error)
            {
                USB_OSA_ENTER_CRITICAL();
                pipe->isBusy = 0;
                USB_OSA_EXIT_CRITICAL();
                return error;
            }
            head = (usb_host_ohci_general_transfer_descritpor_struct_t *)pipe->ed->TailP;
            head->NextTD = (uint32_t)p;
            p = head;
            tr->union1.unitHead = (uint32_t)p;
            if (tr->transferLength)
            {
                statePid = (tr->direction == USB_OUT) ? USB_HOST_OHCI_GTD_DP_IN : USB_HOST_OHCI_GTD_DP_OUT;
            }
            else
            {
                statePid = USB_HOST_OHCI_GTD_DP_IN;
            }
            USB_HostOhciFillGtd(pipe, tr, p, NULL, 0U, statePid, 0x03, 1U);
            tr->setupStatus = kStatus_UsbHostOhci_State;
            break;
        case kStatus_UsbHostOhci_State:
            tr->setupStatus = kStatus_UsbHostOhci_Idle;
            break;
        default:
            USB_OSA_ENTER_CRITICAL();
            pipe->isBusy = 0;
            USB_OSA_EXIT_CRITICAL();
            tr->setupStatus = kStatus_UsbHostOhci_Idle;
            break;
    }

    if (!pipe->isBusy)
    {
        return kStatus_USB_Error;
    }

    if (p)
    {
        p->stateUnion.stateBitField.DI = 0U;
        p->nextGtd = NULL;
        p = (usb_host_ohci_general_transfer_descritpor_struct_t *)p->NextTD;
        p->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
        p->stateUnion.stateBitField.R = 1U;
        p->pipe = pipe;
        tr->transferPipe = (usb_host_pipe_t *)pipe;
        pipe->ed->TailP = (uint32_t)p;

        usbHostState->usbRegBase->HcCommandStatus |= USB_HOST_OHCI_COMMAND_STATUS_CLF_MASK;
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciLinkGtdTr(usb_host_ohci_state_struct_t *usbHostState,
                                          usb_host_ohci_pipe_struct_t *pipe,
                                          usb_host_transfer_t *tr)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *head;
    usb_host_ohci_general_transfer_descritpor_struct_t *p;
    uint32_t remainingLength = tr->transferLength;
    uint32_t tdCount = 0U;
    uint32_t tdLength = 0U;
    uint32_t bufferAddress = (uint32_t)tr->transferBuffer;
    usb_status_t error;
    uint8_t dirPid = USB_HOST_OHCI_GTD_DP_OUT;

    USB_OSA_SR_ALLOC();

    USB_OSA_ENTER_CRITICAL();
    if (pipe->isBusy)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Success;
    }
    pipe->isBusy = 1;
    pipe->isDone = 0U;
    USB_OSA_EXIT_CRITICAL();

    while (remainingLength)
    {
        tdLength = USB_HOST_OHCI_GTD_MAX_LENGTH - (bufferAddress & USB_HOST_OHCI_GTD_BUFFER_ALIGN_MASK);
        if (tdLength < remainingLength)
        {
            tdLength -= tdLength % pipe->pipeCommon.maxPacketSize;
        }
        else
        {
            tdLength = remainingLength;
        }
        remainingLength -= tdLength;
        bufferAddress += tdLength;
        tdCount++;
    }

    if (!tdCount)
    {
        tdCount = 1U;
    }

    error = USB_HostOhciGetGtd(usbHostState, &p, tdCount);
    if (kStatus_USB_Success != error)
    {
        USB_OSA_ENTER_CRITICAL();
        pipe->isBusy = 0;
        USB_OSA_EXIT_CRITICAL();
        return error;
    }

    head = (usb_host_ohci_general_transfer_descritpor_struct_t *)pipe->ed->TailP;
    head->NextTD = (uint32_t)p;
    p = head;
    tr->union1.unitHead = (uint32_t)p;
    dirPid = (tr->direction == USB_OUT) ? USB_HOST_OHCI_GTD_DP_OUT : USB_HOST_OHCI_GTD_DP_IN;

    remainingLength = tr->transferLength;
    bufferAddress = (uint32_t)tr->transferBuffer;

    while (remainingLength)
    {
        tdLength = USB_HOST_OHCI_GTD_MAX_LENGTH - (bufferAddress & USB_HOST_OHCI_GTD_BUFFER_ALIGN_MASK);
        if (tdLength < remainingLength)
        {
            tdLength -= tdLength % pipe->pipeCommon.maxPacketSize;
        }
        else
        {
            tdLength = remainingLength;
        }
        remainingLength -= tdLength;
        if (remainingLength)
        {
            USB_HostOhciFillGtd(pipe, tr, p, (uint8_t *)bufferAddress, tdLength, dirPid, 0x00U, 0U);
            p->nextGtd = (usb_host_ohci_general_transfer_descritpor_struct_t *)p->NextTD;
            p = (usb_host_ohci_general_transfer_descritpor_struct_t *)p->NextTD;
        }
        else
        {
            USB_HostOhciFillGtd(pipe, tr, p, (uint8_t *)bufferAddress, tdLength, dirPid, 0x00U, 1U);
        }
        bufferAddress += tdLength;
    }

    if (0U == tr->transferLength)
    {
        USB_HostOhciFillGtd(pipe, tr, p, NULL, 0U, dirPid, 0x00U, 1U);
    }
    p->stateUnion.stateBitField.DI = 0U;
    p->nextGtd = NULL;
    p = (usb_host_ohci_general_transfer_descritpor_struct_t *)p->NextTD;
    p->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
    p->stateUnion.stateBitField.R = 1U;
    p->pipe = pipe;
    tr->transferPipe = (usb_host_pipe_t *)pipe;
    pipe->ed->TailP = (uint32_t)p;

    if (USB_ENDPOINT_BULK == pipe->pipeCommon.pipeType)
    {
        usbHostState->usbRegBase->HcCommandStatus |= USB_HOST_OHCI_COMMAND_STATUS_BLF_MASK;
    }
    else
    {
    }
    return kStatus_USB_Success;
}

#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
static void USB_HostOhciFillItd(usb_host_ohci_pipe_struct_t *pipe,
                                usb_host_transfer_t *tr,
                                usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd,
                                uint8_t *buffer,
                                uint32_t length,
                                uint32_t startFrame)
{
    itd->BP0 = ((uint32_t)buffer) & USB_HOST_OHCI_ITD_BP0_MASK;
    itd->BE = (length) ? (((uint32_t)buffer) + length - 1U) : 0U;
    itd->stateUnion.stateBitField.SF = startFrame & USB_HOST_OHCI_FMNUMBER_FN_MASK;
    itd->stateUnion.stateBitField.DI = 0U;
    itd->stateUnion.stateBitField.FC = (length - 1) / pipe->pipeCommon.maxPacketSize;
    itd->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
    itd->pipe = pipe;
    itd->tr = tr;
    for (int i = 0; i < 8; i++)
    {
        itd->OffsetPSW[i] =
            (((uint32_t)buffer + i * pipe->pipeCommon.maxPacketSize) & 0xFFFU) | (USB_HOST_OHCI_CC_NOT_ACCESSED << 12U);
        if ((((uint32_t)buffer + i * pipe->pipeCommon.maxPacketSize) & USB_HOST_OHCI_ITD_BP0_MASK) != itd->BP0)
        {
            itd->OffsetPSW[i] |= 1U << 11;
        }
    }
    itd->length = length;
}

static usb_status_t USB_HostOhciLinkItdTr(usb_host_ohci_state_struct_t *usbHostState,
                                          usb_host_ohci_pipe_struct_t *pipe,
                                          usb_host_transfer_t *tr)
{
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *head;
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *p;
    uint32_t remainingLength = tr->transferLength;
    uint32_t tdCount = 0U;
    uint32_t tdLength = 0U;
    uint32_t bufferAddress = (uint32_t)tr->transferBuffer;
    uint32_t startFrame;
    uint32_t currentFrame;
    usb_status_t error;
    uint8_t transcation = 1U;

    if (!remainingLength)
    {
        return kStatus_USB_Success;
    }

    pipe->isDone = 0U;

    tdCount = ((remainingLength - 1) / (pipe->pipeCommon.maxPacketSize)) + 1;
    if (1U == pipe->pipeCommon.interval)
    {
        tdCount = ((tdCount - 1) >> 3) + 1;
#if (defined(FSL_FEATURE_USBFSH_VERSION) && (FSL_FEATURE_USBFSH_VERSION >= 200U))
#else
        tdCount++;
#endif
        transcation = 8U;
    }
    error = USB_HostOhciGetItd(usbHostState, &p, tdCount);
    if (kStatus_USB_Success != error)
    {
        return error;
    }

#if (defined(FSL_FEATURE_USBFSH_VERSION) && (FSL_FEATURE_USBFSH_VERSION >= 200U))
    head = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)pipe->ed->TailP;
    head->NextTD = (uint32_t)p;
    p = head;
#else
    head = p;
#endif
    tr->union1.unitHead = (uint32_t)p;

    currentFrame = s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber;
    startFrame = currentFrame;

    while (remainingLength)
    {
        tdLength = pipe->pipeCommon.maxPacketSize * transcation;
        if (tdLength > remainingLength)
        {
            tdLength = remainingLength;
        }
        USB_HostOhciFillItd(pipe, tr, p, (uint8_t *)bufferAddress, tdLength, startFrame);
        remainingLength -= tdLength;
        if (remainingLength)
        {
            p->nextItd = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)p->NextTD;
            p = p->nextItd;
            startFrame += transcation * pipe->pipeCommon.interval;
        }
        bufferAddress += tdLength;
    }
    p->nextItd = NULL;
    p = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)p->NextTD;
    p->stateUnion.stateBitField.CC = USB_HOST_OHCI_CC_NOT_ACCESSED;
    p->pipe = pipe;
    tr->transferPipe = (usb_host_pipe_t *)pipe;
    pipe->ed->HeadP &= ~USB_HOST_OHCI_ED_HEADP_MASK;
    pipe->ed->HeadP |= (uint32_t)head;
    pipe->ed->TailP = (uint32_t)p;

    return kStatus_USB_Success;
}

#endif

static usb_status_t USB_HostOhciTokenDone(usb_host_ohci_state_struct_t *usbHostState)
{
    usb_host_ohci_general_transfer_descritpor_struct_t *gtd;
    usb_host_ohci_general_transfer_descritpor_struct_t *gtdPos;
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd;
    usb_host_ohci_pipe_struct_t *pipe;
    usb_host_transfer_t *trCurrent;

    /* Enter critical */
    USB_HostOhciLock();
    USB_HostOhciDisableIsr(usbHostState);

    gtd = (usb_host_ohci_general_transfer_descritpor_struct_t *)((uint32_t)usbHostState->tdDoneListHead);
    while (gtd)
    {
        if (((uint32_t)gtd) < ((uint32_t)&s_UsbHostOhciTd[usbHostState->controllerId].gtd[0]))
        {
            itd = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)gtd;
            trCurrent = itd->tr;
            pipe = itd->pipe;
        }
        else
        {
            itd = NULL;
            trCurrent = gtd->tr;
            pipe = gtd->pipe;
        }

        gtdPos = (usb_host_ohci_general_transfer_descritpor_struct_t *)gtd->NextTD;
        USB_HostOhciTdDoneHandle(usbHostState, pipe, trCurrent, gtd, itd);
        if (usbHostState->tdDoneListTail == gtd)
        {
            break;
        }
        gtd = gtdPos;
    }
    usbHostState->tdDoneListHead = NULL;
    usbHostState->tdDoneListTail = NULL;
    /* Exit critical */
    USB_HostOhciEnableIsr(usbHostState);
    USB_HostOhciUnlock();
    return kStatus_USB_Success;
}

static usb_status_t USB_HostOhciSof(usb_host_ohci_state_struct_t *usbHostState)
{
    usb_host_ohci_pipe_struct_t *pipe;
#if (defined(FSL_FEATURE_USBFSH_VERSION) && (FSL_FEATURE_USBFSH_VERSION >= 200U))
#else
    usb_host_ohci_general_transfer_descritpor_struct_t *gtd;
    usb_host_ohci_general_transfer_descritpor_struct_t *gtdP;
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd;
    uint32_t head;
    uint32_t tail;
#endif

    /* Enter critical */
    USB_HostOhciLock();
    USB_HostOhciDisableIsr(usbHostState);
    pipe = usbHostState->pipeListInUsing;
    while (pipe)
    {
        if ((USB_ENDPOINT_CONTROL == pipe->pipeCommon.pipeType) || (USB_ENDPOINT_BULK == pipe->pipeCommon.pipeType))
        {
            if (pipe->ed->trListHead)
            {
                pipe->cutOffTime--;
                if (!pipe->cutOffTime)
                {
                    USB_HostOhciCancelPipe(usbHostState, pipe, pipe->ed->trListHead);
                }
            }
        }
        else
        {
        }

#if (defined(FSL_FEATURE_USBFSH_VERSION) && (FSL_FEATURE_USBFSH_VERSION >= 200U))
#else
        tail = pipe->ed->TailP & USB_HOST_OHCI_ED_TAILP_MASK;
        head = pipe->ed->HeadP & USB_HOST_OHCI_ED_HEADP_MASK;
        if ((pipe->isBusy) && (head == tail))
        {
            if ((pipe->isDone) && (pipe->currentTr == pipe->ed->trListHead))
            {
                if (pipe->startWriteBackCount != usbHostState->writeBackCount)
                {
                    pipe->isDone = 0U;
                    gtd = (usb_host_ohci_general_transfer_descritpor_struct_t *)pipe->ed->trListHead->union1.unitHead;
                    while (gtd)
                    {
                        gtdP = (usb_host_ohci_general_transfer_descritpor_struct_t *)gtd->nextGtd;
                        if (USB_ENDPOINT_ISOCHRONOUS == pipe->pipeCommon.pipeType)
                        {
                            itd = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)gtd;
                        }
                        else
                        {
                            itd = NULL;
                        }
                        /* usb_echo("Timeout pipe\r\n"); */
                        USB_HostOhciTdDoneHandle(usbHostState, pipe, pipe->ed->trListHead, gtd, itd);
                        gtd = gtdP;
                    }
                }
            }
            else
            {
                pipe->isDone = 1;
                pipe->currentTr = pipe->ed->trListHead;
                pipe->startWriteBackCount = usbHostState->writeBackCount;
            }
        }
        else
        {
            pipe->isDone = 0U;
        }
#endif
        pipe = (usb_host_ohci_pipe_struct_t *)pipe->pipeCommon.next;
    }
    /* Exit critical */
    USB_HostOhciEnableIsr(usbHostState);
    USB_HostOhciUnlock();

    return kStatus_USB_Success;
}

static usb_status_t UBS_HostOhciControllerReset(usb_host_ohci_state_struct_t *usbHostState)
{
    usbHostState->usbRegBase->HcCommandStatus = USB_HOST_OHCI_COMMAND_STATUS_HCR_MASK;
    while (usbHostState->usbRegBase->HcCommandStatus & USB_HOST_OHCI_COMMAND_STATUS_HCR_MASK)
    {
        __ASM("nop");
    }
    return kStatus_USB_Success;
}

static usb_status_t UBS_HostOhciControllerInit(usb_host_ohci_state_struct_t *usbHostState)
{
    usb_host_ohci_pipe_struct_t *pipe;
#if (defined(USB_HOST_CONFIG_OHCI_MAX_GTD) && (USB_HOST_CONFIG_OHCI_MAX_GTD > 0U))
    usb_host_ohci_general_transfer_descritpor_struct_t *gtd;
#endif
#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *itd;
#endif
    int i;
    USB_OSA_SR_ALLOC();

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();

    for (i = 0; i < (sizeof(s_UsbHostOhciTd[usbHostState->controllerId].ed) /
                     sizeof(usb_host_ohci_endpoint_descritpor_struct_t));
         i++)
    {
        s_UsbHostOhciTd[usbHostState->controllerId].ed[i].stateUnion.stateBitField.K = 1U;
    }

    for (i = 0; i < sizeof(usb_host_ohci_hcca_struct_t); i++)
    {
        *((uint8_t *)(&s_UsbHostOhciHcca[usbHostState->controllerId]) + i) = 0x00U;
    }

    usbHostState->pipeListInUsing = NULL;
    usbHostState->pipeList = &usbHostState->pipePool[0];
    pipe = usbHostState->pipeList;
    pipe->ed = &s_UsbHostOhciTd[usbHostState->controllerId].ed[3];
    for (i = 1; i < USB_HOST_CONFIG_OHCI_MAX_ED; i++)
    {
        pipe->pipeCommon.next = (usb_host_pipe_t *)&usbHostState->pipePool[i];
        pipe = (usb_host_ohci_pipe_struct_t *)pipe->pipeCommon.next;
        pipe->ed = &s_UsbHostOhciTd[usbHostState->controllerId].ed[3 + i];
    }
    pipe->pipeCommon.next = NULL;

#if (defined(USB_HOST_CONFIG_OHCI_MAX_GTD) && (USB_HOST_CONFIG_OHCI_MAX_GTD > 0U))
    usbHostState->gtdList = &s_UsbHostOhciTd[usbHostState->controllerId].gtd[0];
    gtd = usbHostState->gtdList;
    for (i = 1; i < USB_HOST_CONFIG_OHCI_MAX_GTD; i++)
    {
        gtd->nextGtd = &s_UsbHostOhciTd[usbHostState->controllerId].gtd[i];
        gtd = gtd->nextGtd;
    }
    gtd->nextGtd = NULL;
    usbHostState->gtdCount = USB_HOST_CONFIG_OHCI_MAX_GTD;
#endif

#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
    usbHostState->itdList = &s_UsbHostOhciTd[usbHostState->controllerId].itd[0];
    itd = usbHostState->itdList;
    for (i = 1; i < USB_HOST_CONFIG_OHCI_MAX_ITD; i++)
    {
        itd->NextTD = (uint32_t)&s_UsbHostOhciTd[usbHostState->controllerId].itd[i];
        itd = (usb_host_ohci_isochronous_transfer_descritpor_struct_t *)itd->NextTD;
    }
    itd->NextTD = (uint32_t)NULL;
    usbHostState->itdCount = USB_HOST_CONFIG_OHCI_MAX_ITD;
#endif
    usbHostState->tdDoneListTail = NULL;
    usbHostState->tdDoneListHead = NULL;
    usbHostState->writeBackCount = 0U;

    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    usbHostState->usbRegBase->HcControl = 0U;
    usbHostState->usbRegBase->HcControl &= ~USB_HOST_OHCI_CONTROL_HCFS_MASK;
    usbHostState->usbRegBase->HcControl |= USB_HOST_OHCI_CONTROL_HCFS(USB_HOST_OHCI_CONTROL_HCFS_OPERATIONAL);
    usbHostState->usbRegBase->HcFmInterval =
        (USB_HOST_OHCI_FMINTERVAL_FSMPS((6 * (USB_HOST_OHCI_SOF_INTERVAL - 210U)) / 7)) |
        (USB_HOST_OHCI_FMINTERVAL_FI(USB_HOST_OHCI_SOF_INTERVAL));
    usbHostState->usbRegBase->HcPeriodicStart = USB_HOST_OHCI_PERIODIC_START_PS(
        (uint32_t)((float)USB_HOST_OHCI_SOF_INTERVAL * (float)USB_HOST_OHCI_PERIODIC_BANDWIDTH_PERCENT));
    s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_CONTROL_ED_DUMMY].NextED = 0U;
    usbHostState->usbRegBase->HcControlHeadED =
        (uint32_t)&s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_CONTROL_ED_DUMMY];
    s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_BULK_ED_DUMMY].NextED = 0U;
    usbHostState->usbRegBase->HcBulkHeadED =
        (uint32_t)&s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_BULK_ED_DUMMY];
    s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_PERIODIC_ED_DUMMY].NextED = 0U;
    for (i = 0; i < USB_HOST_OHCI_HCCA_SIZE; i++)
    {
        s_UsbHostOhciHcca[usbHostState->controllerId].HccaInterrruptTable[i] =
            (uint32_t)&s_UsbHostOhciTd[usbHostState->controllerId].ed[USB_HOST_OHCI_PERIODIC_ED_DUMMY];
    }
    usbHostState->usbRegBase->HcHCCA = (uint32_t)&s_UsbHostOhciHcca[usbHostState->controllerId];
    usbHostState->usbRegBase->HcControl |= USB_HOST_OHCI_CONTROL_CBSR(USB_HOST_OHCI_CONTROL_CBSR_C1_B1);
    if (usbHostState->usbRegBase->HcRhDescriptorA & USB_HOST_OHCI_RHDESCRIPTORA_NPS_MASK)
    {
    }
    else
    {
/* Use global power to turn on the port power for all ports */
#if 0
        if (usbHostState->usbRegBase->HcRhDescriptorA & USB_HOST_OHCI_RHDESCRIPTORA_PSM_MASK)
        {
            usbHostState->usbRegBase->HcRhDescriptorB |= USB_HOST_OHCI_RHDESCRIPTORB_PPCM(0xFFFEU);
            for (i = 0; i < (usbHostState->usbRegBase->HcRhDescriptorA & USB_HOST_OHCI_RHDESCRIPTORA_NDP_MASK); i++)
            {
                uint32_t portStatus = usbHostState->usbRegBase->HcRhPortStatus[i];
                portStatus &= ~USB_HOST_OHCI_RHPORTSTATUS_WIC;
                portStatus |= USB_HOST_OHCI_RHPORTSTATUS_PPS_MASK;
                usbHostState->usbRegBase->HcRhPortStatus[i] = portStatus;
            }
            usbHostState->usbRegBase->HcRhStatus = USB_HOST_OHCI_RHSTATUS_LPSC_MASK;
        }
        else
        {
            usbHostState->usbRegBase->HcRhStatus = USB_HOST_OHCI_RHSTATUS_LPSC_MASK;
        }
#else
        usbHostState->usbRegBase->HcRhDescriptorB &= ~USB_HOST_OHCI_RHDESCRIPTORB_PPCM(0xFFFEU);
        usbHostState->usbRegBase->HcRhStatus = USB_HOST_OHCI_RHSTATUS_LPSC_MASK;
#endif
    }
    usbHostState->usbRegBase->HcInterruptDisable |= USB_HOST_OHCI_INTERRUPT_DISABLE_MASK;
    usbHostState->usbRegBase->HcInterruptEnable |=
        USB_HOST_OHCI_INTERRUPT_ENABLE_MIE_MASK | USB_HOST_OHCI_INTERRUPT_ENABLE_WDH_MASK |
        USB_HOST_OHCI_INTERRUPT_ENABLE_RHSC_MASK | USB_HOST_OHCI_INTERRUPT_ENABLE_SF_MASK;

    return kStatus_USB_Success;
}

/*!
 * @brief create the USB host ohci instance.
 *
 * This function initializes the USB host ohci controller driver.
 *
 * @param controllerId        The controller id of the USB IP. Please refer to the enumeration usb_controller_index_t.
 * @param hostHandle         The host level handle.
 * @param controllerHandle  Return the controller instance handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_AllocFail             allocate memory fail.
 * @retval kStatus_USB_Error                 host mutex create fail, OHCI mutex or OHCI event create fail.
 *                                                         Or, OHCI IP initialize fail.
 */
usb_status_t USB_HostOhciCreate(uint8_t controllerId,
                                usb_host_handle hostHandle,
                                usb_host_controller_handle *controllerHandle)
{
    usb_host_ohci_state_struct_t *usbHostState;
    usb_status_t status = kStatus_USB_Success;
    uint32_t usb_base_addrs[] = USBFSH_BASE_ADDRS;
    uint8_t usb_irq[] = USBFSH_IRQS;

    if (((controllerId - kUSB_ControllerOhci0) >= (uint8_t)USB_HOST_CONFIG_OHCI) ||
        ((controllerId - kUSB_ControllerOhci0) >= (sizeof(usb_base_addrs) / sizeof(uint32_t))))
    {
        return kStatus_USB_ControllerNotFound;
    }
    usbHostState = &s_UsbHostOhciState[controllerId - kUSB_ControllerOhci0];
    /* Allocate the USB Host Pipe Descriptors */
    usbHostState->hostHandle = hostHandle;
    usbHostState->controllerId = controllerId - kUSB_ControllerOhci0;

    usbHostState->usbRegBase = (usb_host_ohci_hcor_struct_t *)usb_base_addrs[usbHostState->controllerId];
    usbHostState->isrNumber = (IRQn_Type)usb_irq[usbHostState->controllerId];

    USB_OsaEventCreate(&usbHostState->ohciEvent, 1U);
    if (NULL == usbHostState->ohciEvent)
    {
#ifdef HOST_ECHO
        usb_echo(" event create failed in USB_HostOhciCreate\n");
#endif
        return kStatus_USB_AllocFail;
    } /* Endif */

    if (kStatus_USB_OSA_Success != USB_OsaMutexCreate(&usbHostState->mutex))
    {
        USB_HostOhciDestory(usbHostState);
#ifdef HOST_ECHO
        usb_echo("USB_HostOhciCreate: create host mutex fail\r\n");
#endif
        return kStatus_USB_AllocFail;
    }

    /* enable Host clock, OTG clock and AHB clock */
    usbHostState->usbRegBase->PortMode &= ~USB_HOST_OHCI_PORTMODE_DEV_ENABLE_MASK;
    usbHostState->portNumber = (usbHostState->usbRegBase->HcRhDescriptorA & USB_HOST_OHCI_RHDESCRIPTORA_NDP_MASK);
    if (!usbHostState->portNumber)
    {
#ifdef HOST_ECHO
        usb_echo(" Port number is invalid in USB_HostOhciCreate\n");
#endif
        USB_HostOhciDestory(usbHostState);
        return kStatus_USB_Error;
    }

    usbHostState->portState = (usb_host_ohci_port_state_struct_t *)USB_OsaMemoryAllocate(
        sizeof(usb_host_ohci_port_state_struct_t) * usbHostState->portNumber); /* malloc host ohci port state */
    if (NULL == usbHostState->portState)
    {
        USB_HostOhciDestory(usbHostState);
#ifdef HOST_ECHO
        usb_echo("USB_HostOhciCreate: mem alloc fail\r\n");
#endif
        return kStatus_USB_Error;
    }

    UBS_HostOhciControllerReset(usbHostState);

    UBS_HostOhciControllerInit(usbHostState);

    *controllerHandle = (usb_host_handle)usbHostState;
    return status;
}

/*!
 * @brief destroy USB host ohci instance.
 *
 * This function de-initialize the USB host ohci controller driver.
 *
 * @param handle                                    the controller handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 */
usb_status_t USB_HostOhciDestory(usb_host_controller_handle controllerHandle)
{
    usb_host_ohci_state_struct_t *usbHostState = (usb_host_ohci_state_struct_t *)controllerHandle;

    usbHostState->usbRegBase->HcInterruptDisable |= USB_HOST_OHCI_INTERRUPT_DISABLE_MASK;
    usbHostState->usbRegBase->HcRhStatus |= USB_HOST_OHCI_RHSTATUS_LPS_MASK;
    usbHostState->usbRegBase->HcControl = 0U;

    if (NULL != usbHostState->portState)
    {
        USB_OsaMemoryFree(usbHostState->portState);
        usbHostState->portState = NULL;
    }

    if (NULL != usbHostState->mutex)
    {
        USB_OsaMutexDestroy(usbHostState->mutex);
        usbHostState->mutex = NULL;
    }

    if (NULL != usbHostState->ohciEvent)
    {
        USB_OsaEventDestroy(usbHostState->ohciEvent);
        usbHostState->ohciEvent = NULL;
    }

    return kStatus_USB_Success;
}

usb_status_t USB_HostOhciOpenPipe(usb_host_controller_handle controllerHandle,
                                  usb_host_pipe_handle *pipeHandle,
                                  usb_host_pipe_init_t *pipeInit)
{
    usb_host_ohci_pipe_struct_t *pipe = NULL;
    usb_status_t status;
    usb_host_ohci_state_struct_t *usbHostState = (usb_host_ohci_state_struct_t *)controllerHandle;

    status = USB_HostOhciGetPipe(&usbHostState->pipeList, &pipe);

    if ((kStatus_USB_Success != status) || (NULL == pipe))
    {
#ifdef HOST_ECHO
        usb_echo("ohci open pipe failed\r\n");
#endif
        return kStatus_USB_Busy;
    }

    /* initialize pipe informations */
    pipe->pipeCommon.deviceHandle = pipeInit->devInstance;
    pipe->pipeCommon.endpointAddress = pipeInit->endpointAddress;
    pipe->pipeCommon.direction = pipeInit->direction;
    pipe->pipeCommon.interval = pipeInit->interval;
    pipe->pipeCommon.maxPacketSize = pipeInit->maxPacketSize;
    pipe->pipeCommon.pipeType = pipeInit->pipeType;
    pipe->pipeCommon.numberPerUframe = 1U;
    pipe->pipeCommon.nakCount = pipeInit->nakCount;
    pipe->pipeCommon.nextdata01 = 0U;
    pipe->pipeCommon.currentCount = 0U;
    pipe->pipeCommon.open = 1U;
    pipe->ed->stateUnion.stateBitField.EN = pipeInit->endpointAddress;
    pipe->ed->stateUnion.stateBitField.D = (USB_OUT == pipeInit->direction) ? 1U : 2U;
    pipe->ed->stateUnion.stateBitField.FA = ((usb_host_device_instance_t *)pipeInit->devInstance)->setAddress;
    pipe->ed->stateUnion.stateBitField.S =
        (USB_SPEED_FULL == ((usb_host_device_instance_t *)pipeInit->devInstance)->speed) ? 0U : 1U;
    pipe->ed->stateUnion.stateBitField.F = 0U;
    pipe->ed->stateUnion.stateBitField.MPS = pipeInit->maxPacketSize;
    pipe->ed->stateUnion.stateBitField.K = 0U;
    pipe->ed->TailP = 0U;
    pipe->ed->HeadP = 0U;
    pipe->ed->pipe = pipe;
    pipe->ed->NextED = 0U;
    pipe->ed->trListHead = NULL;
    pipe->ed->trListTail = NULL;
    pipe->ed->dealTr = NULL;
    pipe->cutOffTime = USB_HOST_OHCI_TRANSFER_TIMEOUT_GAP;
    pipe->isCanceling = 0U;

    if (USB_ENDPOINT_ISOCHRONOUS == pipe->pipeCommon.pipeType)
    {
        pipe->ed->stateUnion.stateBitField.F = 1U;

#if 0
        pipe->pipeCommon.interval =
            (1 << (pipe->pipeCommon.interval - 1)); /* iso interval is the power of 2 */
        if (pipe->pipeCommon.interval > USB_HOST_OHCI_HCCA_SIZE)
        {
            pipe->pipeCommon.interval = USB_HOST_OHCI_HCCA_SIZE;
        }
#else
        pipe->pipeCommon.interval = 1U;
#endif
    }
    else if (USB_ENDPOINT_INTERRUPT == pipe->pipeCommon.pipeType)
    {
        int interval = 0;
        /* FS/LS interrupt interval should be the power of 2, it is used for ohci bandwidth */
        for (int p = 0; p < 16; p++)
        {
            if (pipe->pipeCommon.interval & (1U << p))
            {
                if (p > interval)
                {
                    interval = p;
                }
            }
        }
        pipe->pipeCommon.interval = 1U << interval;
        if (pipe->pipeCommon.interval > USB_HOST_OHCI_HCCA_SIZE)
        {
            pipe->pipeCommon.interval = USB_HOST_OHCI_HCCA_SIZE;
        }
    }
    else
    {
    }

    /* open pipe */
    switch (pipe->pipeCommon.pipeType)
    {
        case USB_ENDPOINT_CONTROL:
            pipe->ed->stateUnion.stateBitField.D = 0;
            status = USB_HostOhciOpenControlBulkPipe(usbHostState, pipe);
            break;
        case USB_ENDPOINT_BULK:
            status = USB_HostOhciOpenControlBulkPipe(usbHostState, pipe);
            break;

#if ((defined(USB_HOST_CONFIG_OHCI_MAX_ITD)) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
        case USB_ENDPOINT_ISOCHRONOUS:
            status = USB_HostOhciOpenIsoPipe(usbHostState, pipe);
            break;
#endif
        case USB_ENDPOINT_INTERRUPT:
            status = USB_HostOhciOpenInterruptPipe(usbHostState, pipe);
            break;

        default:
            status = kStatus_USB_Error;
            break;
    }

    if (status != kStatus_USB_Success)
    {
        pipe->ed->stateUnion.stateBitField.K = 1U;
        pipe->pipeCommon.open = 0U;
        /* release the pipe */
        USB_HostOhciInsertPipe(&usbHostState->pipeList, pipe);
        return status;
    }

    USB_HostOhciInsertPipe(&usbHostState->pipeListInUsing, pipe);

    *pipeHandle = pipe;
    return status;
}

usb_status_t USB_HostOhciClosePipe(usb_host_controller_handle controllerHandle, usb_host_pipe_handle pipeHandle)
{
    usb_host_ohci_state_struct_t *usbHostState = (usb_host_ohci_state_struct_t *)controllerHandle;
    usb_host_ohci_pipe_struct_t *pipe = (usb_host_ohci_pipe_struct_t *)pipeHandle;

    switch (pipe->pipeCommon.pipeType)
    {
        case USB_ENDPOINT_BULK:
        case USB_ENDPOINT_CONTROL:
            USB_HostOhciCloseControlBulkPipe(usbHostState, pipe);
            break;

#if ((defined USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD))
        case USB_ENDPOINT_ISOCHRONOUS:
#endif
        case USB_ENDPOINT_INTERRUPT:
            USB_HostOhciCloseIsoInterruptPipe(usbHostState, pipe);
            break;

        default:
            break;
    }
    USB_HostOhciRemovePipe(&usbHostState->pipeListInUsing, pipe);
    pipe->pipeCommon.open = 0U;
    USB_HostOhciInsertPipe(&usbHostState->pipeList, pipe);

    return kStatus_USB_Success;
}

usb_status_t USB_HostOhciWritePipe(usb_host_controller_handle controllerHandle,
                                   usb_host_pipe_handle pipeHandle,
                                   usb_host_transfer_t *transfer)
{
    usb_host_ohci_state_struct_t *usbHostState = (usb_host_ohci_state_struct_t *)controllerHandle;
    usb_host_ohci_pipe_struct_t *pipe = (usb_host_ohci_pipe_struct_t *)pipeHandle;
    usb_status_t status = kStatus_USB_Error;
    transfer->setupStatus = kStatus_UsbHostOhci_Idle;
    transfer->union2.frame = 0U;
    USB_OSA_SR_ALLOC();

    switch (pipe->pipeCommon.pipeType)
    {
        case USB_ENDPOINT_CONTROL:
            if (usbHostState->controlIsBusy)
            {
                status = kStatus_USB_Success;
            }
            else
            {
                status = USB_HostOhciLinkGtdControlTr(usbHostState, pipe, transfer);

                if (kStatus_USB_Success == status)
                {
                    usbHostState->controlIsBusy = 1U;
                }
            }
            break;
        case USB_ENDPOINT_BULK:
        case USB_ENDPOINT_INTERRUPT:
            /* initialize gtd for control/bulk/interrupt transfer */
            status = USB_HostOhciLinkGtdTr(usbHostState, pipe, transfer);
            break;
#if ((defined USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD))
        case USB_ENDPOINT_ISOCHRONOUS:
            /* initialize itd for iso transfer */
            /* status = USB_HostOhciLinkItdTr(usbHostState, pipe, transfer); */
            if (transfer->transferLength)
            {
                status = kStatus_USB_Success;
                USB_OSA_ENTER_CRITICAL();
                if (NULL == pipe->ed->dealTr)
                {
                    if (NULL != pipe->ed->trListHead)
                    {
                        pipe->ed->dealTr = pipe->ed->trListHead;
                    }
                    else
                    {
                        pipe->ed->dealTr = transfer;
                    }
                }
                USB_OSA_EXIT_CRITICAL();
            }
            else
            {
                status = kStatus_USB_InvalidParameter;
            }
            break;
#endif
        default:
            break;
    }

    if (kStatus_USB_Success == status)
    {
        USB_OSA_ENTER_CRITICAL();
        if (pipe->ed->trListHead)
        {
            pipe->ed->trListTail->next = transfer;
        }
        else
        {
            pipe->ed->trListHead = transfer;
        }
        pipe->ed->trListTail = transfer;
        transfer->next = NULL;
        USB_OSA_EXIT_CRITICAL();
    }
    return status;
}

usb_status_t USB_HostOhciReadPipe(usb_host_controller_handle controllerHandle,
                                  usb_host_pipe_handle pipeHandle,
                                  usb_host_transfer_t *transfer)
{
    return USB_HostOhciWritePipe(controllerHandle, pipeHandle, transfer);
}

usb_status_t USB_HostOhciIoctl(usb_host_controller_handle controllerHandle, uint32_t ioctlEvent, void *ioctlParam)
{
    usb_host_ohci_state_struct_t *usbHostState = (usb_host_ohci_state_struct_t *)controllerHandle;
    usb_host_cancel_param_t *param;
    usb_host_ohci_pipe_struct_t *pipe;
    uint32_t deviceAddress;
    usb_status_t status = kStatus_USB_Success;

    if (controllerHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    switch (ioctlEvent)
    {
        case kUSB_HostCancelTransfer:
            /* cancel pipe or one transfer */
            param = (usb_host_cancel_param_t *)ioctlParam;
            status =
                USB_HostOhciCancelPipe(usbHostState, (usb_host_ohci_pipe_struct_t *)param->pipeHandle, param->transfer);
            break;

        case kUSB_HostBusControl:
            /* bus control */
            status = USB_HostOhciControlBus(usbHostState, *((uint8_t *)ioctlParam));
            break;

        case kUSB_HostGetFrameNumber:
            /* get frame number */
            *((uint32_t *)ioctlParam) = (s_UsbHostOhciHcca[usbHostState->controllerId].HccaFrameNumber);
            break;

        case kUSB_HostUpdateControlEndpointAddress:
            pipe = (usb_host_ohci_pipe_struct_t *)ioctlParam;
            pipe->ed->stateUnion.stateBitField.K = 1U;
            /* update address */
            USB_HostHelperGetPeripheralInformation(pipe->pipeCommon.deviceHandle, kUSB_HostGetDeviceAddress,
                                                   &deviceAddress);
            pipe->ed->stateUnion.stateBitField.FA = deviceAddress;
            pipe->ed->stateUnion.stateBitField.K = 0U;
            break;

        case kUSB_HostUpdateControlPacketSize:
            pipe = (usb_host_ohci_pipe_struct_t *)ioctlParam;
            pipe->ed->stateUnion.stateBitField.K = 1U;
            pipe->ed->stateUnion.stateBitField.MPS = pipe->pipeCommon.maxPacketSize;
            pipe->ed->stateUnion.stateBitField.K = 0U;
            break;

        case kUSB_HostPortAttachDisable:
            break;

        case kUSB_HostPortAttachEnable:
            break;

        default:
            break;
    }
    return status;
}

void USB_HostOhciTaskFunction(void *hostHandle)
{
    usb_host_ohci_state_struct_t *usbHostState;
    uint32_t bitSet;

    if (NULL == hostHandle)
    {
        return;
    }
    usbHostState = (usb_host_ohci_state_struct_t *)((usb_host_instance_t *)hostHandle)->controllerHandle;

    /* wait all event */
    if (kStatus_USB_OSA_Success == USB_OsaEventWait(usbHostState->ohciEvent, 0xFFU, 0, 0, &bitSet))
    {
        if (bitSet & USB_HOST_OHCI_EVENT_PORT_CHANGE)
        {
            USB_HostOhciPortChange(usbHostState);
        }
        if (bitSet & USB_HOST_OHCI_EVENT_TOKEN_DONE)
        {
            USB_HostOhciTokenDone(usbHostState);
        }
        if (bitSet & USB_HOST_OHCI_EVENT_SOF)
        {
            USB_HostOhciSof(usbHostState);
        }
        if (bitSet & USB_HOST_OHCI_EVENT_ATTACH)
        {
            for (int i = 0; i < usbHostState->portNumber; i++)
            {
                if (kUSB_DeviceOhciPortPhyAttached == usbHostState->portState[i].portStatus)
                {
                    void *deviceHandle;
                    if (kStatus_USB_Success == USB_HostAttachDevice(usbHostState->hostHandle,
                                                                    usbHostState->portState[i].portSpeed, 0, i, 1,
                                                                    &deviceHandle))
                    {
                        usbHostState->portState[i].portStatus = kUSB_DeviceOhciPortAttached;
                    }
                }
            }
        }
        if (bitSet & USB_HOST_OHCI_EVENT_DETACH)
        {
            for (int i = 0; i < usbHostState->portNumber; i++)
            {
                if (kUSB_DeviceOhciPortPhyDetached == usbHostState->portState[i].portStatus)
                {
                    usbHostState->portState[i].portStatus = kUSB_DeviceOhciPortDetached;
                    USB_HostDetachDevice(usbHostState->hostHandle, 0, i);
                }
            }
        }
    }
}

void USB_HostOhciIsrFunction(void *hostHandle)
{
    usb_host_ohci_state_struct_t *usbHostState;
    usb_host_ohci_pipe_struct_t *pipe;
    static uint32_t interruptStatus = 0U;
    static uint32_t sofCount = 0U;

    if (hostHandle == NULL)
    {
        return;
    }

    usbHostState = (usb_host_ohci_state_struct_t *)((usb_host_instance_t *)hostHandle)->controllerHandle;

    interruptStatus = usbHostState->usbRegBase->HcInterruptStatus;
    interruptStatus &= usbHostState->usbRegBase->HcInterruptEnable;

    if (interruptStatus & USB_HOST_OHCI_INTERRUPT_STATUS_WDH_MASK) /* Write back done head */
    {
        USB_HostOhciLinkTdToDoneList(usbHostState);
        USB_OsaEventSet(usbHostState->ohciEvent, USB_HOST_OHCI_EVENT_TOKEN_DONE);
    }

    if (interruptStatus & USB_HOST_OHCI_INTERRUPT_STATUS_SF_MASK) /* SOF interrupt */
    {
        for (pipe = usbHostState->pipeListInUsing; NULL != pipe;
             pipe = (usb_host_ohci_pipe_struct_t *)pipe->pipeCommon.next)
        {
            if ((USB_ENDPOINT_ISOCHRONOUS == pipe->pipeCommon.pipeType) && (NULL != pipe->ed->trListHead))
            {
                if ((NULL != pipe->ed->dealTr))
                {
                    uint32_t tail = pipe->ed->TailP & USB_HOST_OHCI_ED_TAILP_MASK;
                    uint32_t head = pipe->ed->HeadP & USB_HOST_OHCI_ED_HEADP_MASK;
                    if (head == tail)
                    {
                        if (kStatus_USB_Success == USB_HostOhciLinkItdTr(usbHostState, pipe, pipe->ed->dealTr))
                        {
                            pipe->ed->dealTr = pipe->ed->dealTr->next;
                        }
                    }
                }
                pipe->isDone = 0U;
            }
        }
        sofCount++;
        if (sofCount >= USB_HOST_OHCI_TRANSFER_SCAN_INTERVAL)
        {
            sofCount = 0U;
            USB_OsaEventSet(usbHostState->ohciEvent, USB_HOST_OHCI_EVENT_SOF);
        }
    }

    if (interruptStatus & USB_HOST_OHCI_INTERRUPT_STATUS_RHSC_MASK) /* port change detect interrupt */
    {
        USB_OsaEventSet(usbHostState->ohciEvent, USB_HOST_OHCI_EVENT_PORT_CHANGE);
    }

    usbHostState->usbRegBase->HcInterruptStatus = interruptStatus; /* clear interrupt */
}

#endif
