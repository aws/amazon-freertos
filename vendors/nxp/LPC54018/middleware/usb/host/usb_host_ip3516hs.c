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
#if (defined(USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS > 0U))
#include "usb_host.h"
#include "usb_host_hci.h"
#include "fsl_device_registers.h"
#include "usb_host_ip3516hs.h"
#include "usb_host_devices.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
#define USB_HOST_IP3516HS_TEST_DESCRIPTOR_LENGTH (18U)
#define USB_HOST_IP3516HS_PORTSC_PTC_J_STATE (0x01U)
#define USB_HOST_IP3516HS_PORTSC_PTC_K_STATE (0x02U)
#define USB_HOST_IP3516HS_PORTSC_PTC_SE0_NAK (0x03U)
#define USB_HOST_IP3516HS_PORTSC_PTC_PACKET (0x04U)
#define USB_HOST_IP3516HS_PORTSC_PTC_FORCE_ENABLE (0x05U)
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static usb_status_t USB_HostIp3516HsGetBuffer(usb_host_ip3516hs_state_struct_t *usbHostState,
                                              uint32_t length,
                                              uint32_t MaxPacketSize,
                                              uint32_t *index,
                                              uint32_t *bufferLength);
static usb_status_t USB_HostIp3516HsFreeBuffer(usb_host_ip3516hs_state_struct_t *usbHostState,
                                               uint32_t index,
                                               uint32_t bufferLength);
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
/*!
 * @brief suspend bus.
 *
 * @param usbHostState      ip3516hs instance pointer.
 */
static void USB_HostIp3516HsSuspendBus(usb_host_ip3516hs_state_struct_t *usbHostState);

/*!
 * @brief resume bus.
 *
 * @param usbHostState        ip3516hs instance pointer.
 */
static void USB_HostIp3516HsResumeBus(usb_host_ip3516hs_state_struct_t *ehciInstance);

extern usb_status_t USB_HostStandardSetGetDescriptor(usb_host_device_instance_t *deviceInstance,
                                                     usb_host_transfer_t *transfer,
                                                     void *param);

static void USB_HostIp3516HsDelay(usb_host_ip3516hs_state_struct_t *usbHostState, uint32_t ms);

#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_GLOBAL_DEDICATED_RAM USB_RAM_ADDRESS_ALIGNMENT(1024)
    usb_host_ip3516hs_ptd_struct_t s_UsbHostIp3516HsPtd[USB_HOST_CONFIG_IP3516HS];
static usb_host_ip3516hs_state_struct_t s_UsbHostIp3516HsState[USB_HOST_CONFIG_IP3516HS];
static uint8_t s_SlotMaxBandwidth[8] = {125, 125, 125, 125, 125, 125, 50, 0};
USB_GLOBAL_DEDICATED_RAM uint8_t s_UsbHostIp3516HsBufferArray[80][64];
/*******************************************************************************
 * Code
 ******************************************************************************/
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
usb_status_t USB_HostIp3516HsTestSetMode(usb_host_ip3516hs_state_struct_t *usbHostState, uint32_t testMode)
{
    uint32_t Ip3516HsortSC;

    Ip3516HsortSC = usbHostState->usbRegBase->PORTSC1;
    Ip3516HsortSC &= ~((uint32_t)USBHSH_PORTSC1_PTC_MASK);   /* clear test mode bits */
    Ip3516HsortSC |= (testMode << USBHSH_PORTSC1_PTC_SHIFT); /* set test mode bits */
    usbHostState->usbRegBase->PORTSC1 = Ip3516HsortSC;
    return kStatus_USB_Success;
}

static void USB_HostIp3516HsTestSuspendResume(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    uint8_t timeCount;
    timeCount = 15 * 8; /* 15s */
    while (timeCount--)
    {
        USB_HostIp3516HsDelay(usbHostState, 1000U);
    }
    USB_HostIp3516HsSuspendBus(usbHostState);
    timeCount = 15 * 8; /* 15s */
    while (timeCount--)
    {
        USB_HostIp3516HsDelay(usbHostState, 1000U);
    }

    USB_HostIp3516HsResumeBus(usbHostState);
}

static void USB_HostIp3516HsTestCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    USB_HostFreeTransfer(param, transfer);
    usb_host_ip3516hs_state_struct_t *usbHostState = (usb_host_ip3516hs_state_struct_t *)param;
    if (1U == usbHostState->complianceTestStart)
    {
        uint8_t timeCount;
        usbHostState->complianceTest = 0U;
        usbHostState->complianceTestStart = 0U;
        timeCount = 15 * 8; /* 15s */
        while (timeCount--)
        {
            USB_HostIp3516HsDelay(usbHostState, 1000U);
        }
        usb_echo("test_single_step_get_dev_desc_data finished\r\n");
    }
}

static void USB_HostIp3516HsTestSingleStepGetDeviceDesc(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                        usb_device_handle deviceHandle)
{
    usb_host_process_descriptor_param_t getDescriptorParam;
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    usb_host_transfer_t *transfer;
    uint8_t timeCount;

    /* disable periodic shedule */
    usbHostState->usbRegBase->USBCMD &= ~(USB_HOST_IP3516HS_USBCMD_INT_EN_MASK | USB_HOST_IP3516HS_USBCMD_ISO_EN_MASK);

    timeCount = 15 * 8; /* 15s */
    while (timeCount--)
    {
        USB_HostIp3516HsDelay(usbHostState, 1000U);
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(usbHostState->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return;
    }

    getDescriptorParam.descriptorLength = sizeof(usb_descriptor_device_t);
    getDescriptorParam.descriptorLength = 18;
    getDescriptorParam.descriptorBuffer = (uint8_t *)&deviceInstance->deviceDescriptor;
    getDescriptorParam.descriptorType = USB_DESCRIPTOR_TYPE_DEVICE;
    getDescriptorParam.descriptorIndex = 0;
    getDescriptorParam.languageId = 0;
    transfer->callbackFn = USB_HostIp3516HsTestCallback;
    transfer->callbackParam = usbHostState->hostHandle;
    transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_DIR_IN;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_GET_DESCRIPTOR;
    transfer->setupPacket->wIndex = 0;
    transfer->setupPacket->wLength = 0;
    transfer->setupPacket->wValue = 0;
    USB_HostStandardSetGetDescriptor(deviceInstance, transfer, &getDescriptorParam);
}

static void USB_HostIp3516HsTestSingleStepGetDeviceDescData(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                            usb_device_handle deviceHandle)
{
    static uint8_t buffer[USB_HOST_IP3516HS_TEST_DESCRIPTOR_LENGTH];

    usb_host_process_descriptor_param_t getDescriptorParam;
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    usb_host_transfer_t *transfer;

    /* disable periodic shedule */
    usbHostState->usbRegBase->USBCMD &= ~(USB_HOST_IP3516HS_USBCMD_INT_EN_MASK | USB_HOST_IP3516HS_USBCMD_ISO_EN_MASK);

    /* malloc one transfer */
    if (USB_HostMallocTransfer(usbHostState->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return;
    }
    usbHostState->complianceTestStart = 1;
    getDescriptorParam.descriptorLength = sizeof(usb_descriptor_device_t);
    getDescriptorParam.descriptorLength = 18;
    getDescriptorParam.descriptorBuffer = (uint8_t *)buffer;
    getDescriptorParam.descriptorType = USB_DESCRIPTOR_TYPE_DEVICE;
    getDescriptorParam.descriptorIndex = 0;
    getDescriptorParam.languageId = 0;
    transfer->callbackFn = USB_HostIp3516HsTestCallback;
    transfer->callbackParam = (void *)usbHostState;
    transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_DIR_IN;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_GET_DESCRIPTOR;
    transfer->setupPacket->wIndex = 0;
    transfer->setupPacket->wLength = 0;
    transfer->setupPacket->wValue = 0;
    USB_HostStandardSetGetDescriptor(deviceInstance, transfer, &getDescriptorParam);

    return;
}

void USB_HostIp3516HsTestModeInit(usb_device_handle deviceHandle)
{
    uint32_t productId;
    usb_host_device_instance_t *deviceInstance = (usb_host_device_instance_t *)deviceHandle;
    usb_host_ip3516hs_state_struct_t *usbHostState =
        (usb_host_ip3516hs_state_struct_t *)(((usb_host_instance_t *)(deviceInstance->hostHandle))->controllerHandle);

    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDevicePID, &productId);

    usb_echo("usb host Ip3516hs test mode init  product id:0x%x\r\n", productId);

    switch (productId)
    {
        case 0x0101U:
            USB_HostIp3516HsTestSetMode(usbHostState, USB_HOST_IP3516HS_PORTSC_PTC_SE0_NAK);
            break;
        case 0x0102U:
            USB_HostIp3516HsTestSetMode(usbHostState, USB_HOST_IP3516HS_PORTSC_PTC_J_STATE);
            break;
        case 0x0103U:
            USB_HostIp3516HsTestSetMode(usbHostState, USB_HOST_IP3516HS_PORTSC_PTC_K_STATE);
            break;
        case 0x0104U:
            USB_HostIp3516HsTestSetMode(usbHostState, USB_HOST_IP3516HS_PORTSC_PTC_PACKET);
            break;
        case 0x0105U:
            usb_echo("set test mode FORCE_ENALBE\r\n");
            USB_HostIp3516HsTestSetMode(usbHostState, USB_HOST_IP3516HS_PORTSC_PTC_FORCE_ENABLE);
            break;
        case 0x0106U:
            USB_HostIp3516HsTestSuspendResume(usbHostState);
            break;
        case 0x0107U:
            usb_echo("start test SINGLE_STEP_GET_DEV_DESC\r\n");
            USB_HostIp3516HsTestSingleStepGetDeviceDesc(usbHostState, deviceHandle);
            break;
        case 0x0108U:
            usb_echo("start test SINGLE_STEP_GET_DEV_DESC_DATA\r\n");
            USB_HostIp3516HsTestSingleStepGetDeviceDescData(usbHostState, deviceHandle);
            break;
        default:
            break;
    }

    return;
}

static void USB_HostIp3516HsSuspendBus(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    uint32_t portStatus;
    portStatus = usbHostState->usbRegBase->PORTSC1;
    USB_HostIp3516HsLock();
    if (portStatus & USB_HOST_IP3516HS_PORTSC1_PED_MASK)
    {
        portStatus = usbHostState->usbRegBase->PORTSC1;
        portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_SUS_L1_MASK);
        portStatus |= USB_HOST_IP3516HS_PORTSC1_SUSP_MASK;
        usbHostState->usbRegBase->PORTSC1 = portStatus;
    }

    USB_HostIp3516HsUnlock();
}

static void USB_HostIp3516HsResumeBus(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    uint32_t portStatus;

    USB_HostIp3516HsLock();
    /* Resume port */
    portStatus = usbHostState->usbRegBase->PORTSC1;
    if (portStatus & USB_HOST_IP3516HS_PORTSC1_PED_MASK)
    {
        portStatus = usbHostState->usbRegBase->PORTSC1;
        portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC);
        portStatus |= USB_HOST_IP3516HS_PORTSC1_FPR_MASK;
        usbHostState->usbRegBase->PORTSC1 = portStatus;
        /**resume 21ms = 168/8*/
        USB_HostIp3516HsDelay(usbHostState, 168U);

        portStatus = usbHostState->usbRegBase->PORTSC1;
        portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_FPR_MASK);
        portStatus |= USB_HOST_IP3516HS_PORTSC1_PP_MASK | USB_HOST_IP3516HS_PORTSC1_PED_MASK;
        usbHostState->usbRegBase->PORTSC1 = portStatus;
    }
    USB_HostIp3516HsUnlock();
}
#endif

static void USB_HostOhciDisableIsr(usb_host_ip3516hs_state_struct_t *usbHostState)
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

static void USB_HostOhciEnableIsr(usb_host_ip3516hs_state_struct_t *usbHostState)
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

static void USB_HostIp3516HsDelay(usb_host_ip3516hs_state_struct_t *usbHostState, uint32_t ms)
{
    /* note: the max delay time cannot exceed half of max value (0x4000) */
    int32_t sofStart;
    int32_t SofEnd;
    uint32_t distance;

    sofStart = (int32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                         USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);

    do
    {
        SofEnd = (int32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                           USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
        distance = (uint32_t)(
            (SofEnd - sofStart + (USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK >> USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT) + 1) &
            (USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK >> USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT));
    } while ((distance) < (ms)); /* compute the distance between sofStart and SofEnd */
}

static usb_status_t USB_HostIp3516HsControlBus(usb_host_ip3516hs_state_struct_t *usbHostState, uint8_t busControl)
{
    uint32_t portStatus = usbHostState->usbRegBase->PORTSC1;
    usb_status_t status = kStatus_USB_Success;

    switch (busControl)
    {
        case kUSB_HostBusReset:
            portStatus &= ~USB_HOST_IP3516HS_PORTSC1_WIC;
            usbHostState->usbRegBase->PORTSC1 = portStatus | USB_HOST_IP3516HS_PORTSC1_PR_MASK;

            USB_HostIp3516HsDelay(usbHostState, 200);

            portStatus = usbHostState->usbRegBase->PORTSC1;
            portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_PR_MASK);
            usbHostState->usbRegBase->PORTSC1 = portStatus;
            while (usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_PR_MASK)
            {
                __ASM("nop");
            }

            break;

        case kUSB_HostBusRestart:
            break;

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
        case kUSB_HostBusSuspend:
            if (usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_CCS_MASK)
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;

                portStatus = usbHostState->usbRegBase->PORTSC1;
                portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_SUS_L1_MASK);
                portStatus |= USB_HOST_IP3516HS_PORTSC1_SUSP_MASK;
                usbHostState->usbRegBase->PORTSC1 = portStatus;

                usbHostState->matchTick = hostPointer->hwTick;
                usbHostState->busSuspendStatus = kBus_Ip3516HsStartSuspend;

                while ((hostPointer->hwTick - usbHostState->matchTick) <= 5)
                {
                }
                portStatus = usbHostState->usbRegBase->USBCMD;
                portStatus &= ~USB_HOST_IP3516HS_USBCMD_RS_MASK;
                usbHostState->usbRegBase->USBCMD = portStatus;
                /* call host callback function, function is initialized in USB_HostInit */
                hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                            kUSB_HostEventSuspended); /* call host callback function */
                usbHostState->busSuspendStatus = kBus_Ip3516HsSuspended;
            }
            else
            {
                status = kStatus_USB_Error;
            }
            break;

        case kUSB_HostBusResume:
            if (usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_CCS_MASK)
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;

                usbHostState->busSuspendStatus = kBus_Ip3516HsStartResume;
                usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_RS_MASK;
                portStatus = usbHostState->usbRegBase->PORTSC1;
                portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC);
                portStatus |= USB_HOST_IP3516HS_PORTSC1_FPR_MASK;
                usbHostState->usbRegBase->PORTSC1 = portStatus;
                usbHostState->matchTick = hostPointer->hwTick;
                while ((hostPointer->hwTick - usbHostState->matchTick) <= 20)
                {
                }
                portStatus = usbHostState->usbRegBase->PORTSC1;
                portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_FPR_MASK);
                portStatus |= USB_HOST_IP3516HS_PORTSC1_PP_MASK | USB_HOST_IP3516HS_PORTSC1_PED_MASK;
                usbHostState->usbRegBase->PORTSC1 = portStatus;
                /* call host callback function, function is initialized in USB_HostInit */
                hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                            kUSB_HostEventResumed); /* call host callback function */
                hostPointer->suspendedDevice = NULL;
                usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
            }
            else
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;
                usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_RS_MASK;
                portStatus = usbHostState->usbRegBase->PORTSC1;
                portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_FPR_MASK);
                portStatus |= USB_HOST_IP3516HS_PORTSC1_PP_MASK | USB_HOST_IP3516HS_PORTSC1_PED_MASK;
                usbHostState->usbRegBase->PORTSC1 = portStatus;
                hostPointer->suspendedDevice = NULL;
                usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;

                status = kStatus_USB_Error;
            }
            break;
#if ((defined(USB_HOST_CONFIG_LPM_L1)) && (USB_HOST_CONFIG_LPM_L1 > 0U))
        case kUSB_HostBusL1Sleep:
            if (usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_CCS_MASK)
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;

                portStatus = usbHostState->usbRegBase->USBCMD;
                portStatus &= ~(USB_HOST_IP3516HS_USBCMD_HIRD_MASK | USB_HOST_IP3516HS_USBCMD_LPM_RWU_MASK);
                portStatus |= (usbHostState->hirdValue << USB_HOST_IP3516HS_USBCMD_HIRD_SHIFT) |
                              (usbHostState->L1remoteWakeupEnable << USB_HOST_IP3516HS_USBCMD_LPM_RWU_SHIFT);
                usbHostState->usbRegBase->USBCMD = portStatus;
                portStatus = usbHostState->usbRegBase->PORTSC1;
                portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_SUS_L1_MASK |
                                USB_HOST_IP3516HS_PORTSC1_DEV_ADD_MASK);
                usb_host_device_instance_t *deviceInstance;

                usbHostState->busSuspendStatus = kBus_Ip3516HsL1StartSleep;

                deviceInstance = (usb_host_device_instance_t *)hostPointer->suspendedDevice;
                usbHostState->usbRegBase->PORTSC1 |=
                    USB_HOST_IP3516HS_PORTSC1_SUSP_MASK | USB_HOST_IP3516HS_PORTSC1_SUS_L1_MASK |
                    ((deviceInstance->setAddress << USB_HOST_IP3516HS_PORTSC1_DEV_ADD_SHIFT) &
                     USB_HOST_IP3516HS_PORTSC1_DEV_ADD_MASK);
#if (defined(FSL_FEATURE_USBHSH_VERSION) && (FSL_FEATURE_USBHSH_VERSION >= 300U))
#else
                while (!(usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_SUSP_MASK) &&
                       !(usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK))
                {
                    __NOP();
                }

                portStatus = usbHostState->usbRegBase->PORTSC1;

                if ((usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_SUSP_MASK) &&
                    (0x00 == (portStatus & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK)))
                {
                    usbHostState->matchTick = hostPointer->hwTick;
                    while ((hostPointer->hwTick - usbHostState->matchTick) <= 4)
                    {
                    }
                    portStatus = usbHostState->usbRegBase->USBCMD;
                    portStatus &= ~USB_HOST_IP3516HS_USBCMD_RS_MASK;
                    usbHostState->usbRegBase->USBCMD = portStatus;

                    usbHostState->busSuspendStatus = kBus_Ip3516HsL1Sleeped;
                    /* call host callback function, function is initialized in USB_HostInit */
                    hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                kUSB_HostEventL1Sleeped); /* call host callback function */
                }
                else if (0x02 == (portStatus & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK))
                {
                    usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                    /* call host callback function, function is initialized in USB_HostInit */
                    hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                kUSB_HostEventL1SleepNotSupport); /* call host callback function */
                }
                else if (0x01 == (portStatus & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK))
                {
                    usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                    /* call host callback function, function is initialized in USB_HostInit */
                    hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                kUSB_HostEventL1SleepNYET); /* call host callback function */
                }
                else if (0x03 == (portStatus & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK))
                {
                    usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                    /* call host callback function, function is initialized in USB_HostInit */
                    hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                kUSB_HostEventL1SleepError); /* call host callback function */
                }
                else
                {
                    usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                }
#endif
            }
            else
            {
                status = kStatus_USB_Error;
            }
            break;

        case kUSB_HostBusL1Resume:
            if (usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_CCS_MASK)
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;

                usbHostState->busSuspendStatus = kBus_Ip3516HsStartResume;
                usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_RS_MASK;
                portStatus = usbHostState->usbRegBase->PORTSC1;
                portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC);
                portStatus |= USB_HOST_IP3516HS_PORTSC1_FPR_MASK;
                usbHostState->usbRegBase->PORTSC1 = portStatus;
                while ((portStatus)&USB_HOST_IP3516HS_PORTSC1_FPR_MASK)
                {
                    portStatus = (usbHostState->usbRegBase->PORTSC1);
                    portStatus = portStatus & USB_HOST_IP3516HS_PORTSC1_FPR_MASK;
                }
                usbHostState->matchTick = hostPointer->hwTick;
                while ((hostPointer->hwTick - usbHostState->matchTick) <= 5)
                {
                }
                /* call host callback function, function is initialized in USB_HostInit */
                hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                            kUSB_HostEventL1Resumed); /* call host callback function */
                hostPointer->suspendedDevice = NULL;
                usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
            }
            else
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;
                usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_RS_MASK;
                portStatus = usbHostState->usbRegBase->PORTSC1;
                portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_FPR_MASK);
                portStatus |= USB_HOST_IP3516HS_PORTSC1_PP_MASK | USB_HOST_IP3516HS_PORTSC1_PED_MASK;
                usbHostState->usbRegBase->PORTSC1 = portStatus;
                hostPointer->suspendedDevice = NULL;
                usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;

                status = kStatus_USB_Error;
            }
            break;
#endif
#endif
        case kUSB_HostBusEnableAttach:
            if (usbHostState->usbRegBase->HCSPARAMS &
                USB_HOST_IP3516HS_HCSPARAMS_PPC_MASK) /* Ports have power port switches */
            {
                /* only has one port */
                uint32_t tmp = usbHostState->usbRegBase->PORTSC1;
                tmp &= (~USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_PP_MASK);
                usbHostState->usbRegBase->PORTSC1 = (tmp); /* turn on port power */

                tmp = usbHostState->usbRegBase->PORTSC1;
                tmp &= (~USB_HOST_IP3516HS_PORTSC1_WIC);
                usbHostState->usbRegBase->PORTSC1 = (tmp | USB_HOST_IP3516HS_PORTSC1_PP_MASK); /* turn on port power */
            }
            USB_HostIp3516HsControlBus(usbHostState, kUSB_HostBusReset);
            usbHostState->portState[0].portStatus = kUSB_DeviceIp3516HsPortDetached;
            break;
        case kUSB_HostBusDisableAttach:
            break;
        default:
            status = kStatus_USB_Error;
            break;
    }
    return status;
}

static uint32_t USB_HostIp3516HsBusTime(uint8_t speed, uint8_t pipeType, uint8_t direction, uint32_t dataLength)
{
    uint32_t result = (3167 + ((1000 * dataLength) * 7U * 8U / 6U)) / 1000;

    if (pipeType == USB_ENDPOINT_ISOCHRONOUS) /* iso */
    {
        if (speed == USB_SPEED_HIGH)
        {
            result = 2083 * (38 * 8 + result) + USB_HOST_IP3516HS_TIME_DELAY;
        }
        else if (speed == USB_SPEED_FULL)
        {
            if (direction == USB_IN)
            {
                result = 7268000 + 83540 * result + USB_HOST_IP3516HS_TIME_DELAY;
            }
            else
            {
                result = 6265000 + 83540 * result + USB_HOST_IP3516HS_TIME_DELAY;
            }
        }
        else
        {
        }
    }
    else
    {
        if (speed == USB_SPEED_HIGH)
        {
            result = 2083 * (55 * 8 + result) + USB_HOST_IP3516HS_TIME_DELAY;
        }
        else if (speed == USB_SPEED_FULL)
        {
            result = 9107000 + 83540 * result + USB_HOST_IP3516HS_TIME_DELAY;
        }
        else if (speed == USB_SPEED_LOW)
        {
            if (direction == USB_IN)
            {
                result = 64060000 + 2000 * USB_HOST_IP3516HS_HUB_LS_SETUP_TIME_DELAY + 676670 * result +
                         USB_HOST_IP3516HS_TIME_DELAY;
            }
            else
            {
                result = 64107000 + 2000 * USB_HOST_IP3516HS_HUB_LS_SETUP_TIME_DELAY + 667000 * result +
                         USB_HOST_IP3516HS_TIME_DELAY;
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

static usb_status_t USB_HostIp3516HsGetNewPipe(usb_host_ip3516hs_pipe_struct_t **pipeQueue,
                                               usb_host_ip3516hs_pipe_struct_t **pipe)
{
    usb_status_t error = kStatus_USB_Busy;
    USB_OSA_SR_ALLOC();

    /* get a pipe instance */
    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    if (NULL != (*pipeQueue))
    {
        *pipe = *pipeQueue;
        *pipeQueue = (usb_host_ip3516hs_pipe_struct_t *)((*pipe)->pipeCommon.next);
        error = kStatus_USB_Success;
    }
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();
    return error;
}

static usb_status_t USB_HostIp3516HsRemovePipe(usb_host_ip3516hs_pipe_struct_t **pipeQueue,
                                               usb_host_ip3516hs_pipe_struct_t *pipe)
{
    usb_host_ip3516hs_pipe_struct_t *p = *pipeQueue;
    usb_host_ip3516hs_pipe_struct_t *pre;

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
            p = (usb_host_ip3516hs_pipe_struct_t *)p->pipeCommon.next;
        }
        else
        {
            if (NULL != pre)
            {
                pre->pipeCommon.next = p->pipeCommon.next;
            }
            else
            {
                *pipeQueue = (usb_host_ip3516hs_pipe_struct_t *)p->pipeCommon.next;
            }
            break;
        }
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsInsertPipe(usb_host_ip3516hs_pipe_struct_t **pipeQueue,
                                               usb_host_ip3516hs_pipe_struct_t *pipe)
{
    usb_host_ip3516hs_pipe_struct_t *p = *pipeQueue;

    USB_OSA_SR_ALLOC();

    pipe->pipeCommon.next = NULL;
    /* get a pipe instance */
    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    while (NULL != p)
    {
        if (p != pipe)
        {
            p = (usb_host_ip3516hs_pipe_struct_t *)p->pipeCommon.next;
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

static usb_status_t USB_HostIp3516HsPortChange(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    uint32_t portStatus;
    int i = 0;

    for (i = 0; i < usbHostState->portNumber; i++)
    {
        portStatus = usbHostState->usbRegBase->PORTSC1;
        if (portStatus & USB_HOST_IP3516HS_PORTSC1_CSC_MASK)
        {
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
            if (kBus_Ip3516HsIdle != usbHostState->busSuspendStatus)
            {
                usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;
                usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_RS_MASK;
                portStatus = usbHostState->usbRegBase->PORTSC1;
                portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_FPR_MASK);
                portStatus |= USB_HOST_IP3516HS_PORTSC1_PP_MASK | USB_HOST_IP3516HS_PORTSC1_PED_MASK;
                usbHostState->usbRegBase->PORTSC1 = portStatus;
                hostPointer->suspendedDevice = NULL;
                usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
            }
#endif
            uint32_t sofStart =
                (uint32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                           USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
            uint32_t sof;
            while (1)
            {
                portStatus = usbHostState->usbRegBase->PORTSC1;
                if (portStatus & USB_HOST_IP3516HS_PORTSC1_CSC_MASK)
                {
                    portStatus &= ~USB_HOST_IP3516HS_PORTSC1_WIC;
                    usbHostState->usbRegBase->PORTSC1 = portStatus | USB_HOST_IP3516HS_PORTSC1_CSC_MASK;
                }
                sof = (uint32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                                 USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
                if ((((sof + USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK + 1) - sofStart) &
                     USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) > 1)
                {
                    break;
                }
            }
        }

        portStatus = usbHostState->usbRegBase->PORTSC1;

        if (portStatus & USB_HOST_IP3516HS_PORTSC1_CCS_MASK)
        {
            int index;
            if (kUSB_DeviceIp3516HsPortDetached != usbHostState->portState[i].portStatus)
            {
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
                if ((kBus_Ip3516HsSuspended == usbHostState->busSuspendStatus))
                {
                    usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;

                    usbHostState->busSuspendStatus = kBus_Ip3516HsStartResume;
                    /* call host callback function, function is initialized in USB_HostInit */
                    hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                kUSB_HostEventDetectResume); /* call host callback function */
                    usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_RS_MASK;
                    portStatus = usbHostState->usbRegBase->PORTSC1;
                    portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC);
                    portStatus |= USB_HOST_IP3516HS_PORTSC1_FPR_MASK;
                    usbHostState->usbRegBase->PORTSC1 = portStatus;
                    usbHostState->matchTick = hostPointer->hwTick;
                    while ((hostPointer->hwTick - usbHostState->matchTick) <= 20)
                    {
                    }
                    portStatus = usbHostState->usbRegBase->PORTSC1;
                    portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_FPR_MASK);
                    portStatus |= USB_HOST_IP3516HS_PORTSC1_PP_MASK | USB_HOST_IP3516HS_PORTSC1_PED_MASK;
                    usbHostState->usbRegBase->PORTSC1 = portStatus;
                    /* call host callback function, function is initialized in USB_HostInit */
                    hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                kUSB_HostEventResumed); /* call host callback function */
                    hostPointer->suspendedDevice = NULL;
                    usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                }
#if ((defined(USB_HOST_CONFIG_LPM_L1)) && (USB_HOST_CONFIG_LPM_L1 > 0U))
                if ((kBus_Ip3516HsL1Sleeped == usbHostState->busSuspendStatus))
                {
                    usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;

                    usbHostState->busSuspendStatus = kBus_Ip3516HsStartResume;
                    /* call host callback function, function is initialized in USB_HostInit */
                    hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                kUSB_HostEventDetectResume); /* call host callback function */
                    usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_RS_MASK;
                    usbHostState->matchTick = hostPointer->hwTick;

                    while ((portStatus)&USB_HOST_IP3516HS_PORTSC1_FPR_MASK)
                    {
                        portStatus = (usbHostState->usbRegBase->PORTSC1);
                        portStatus = portStatus & USB_HOST_IP3516HS_PORTSC1_FPR_MASK;
                        if ((hostPointer->hwTick - usbHostState->matchTick) >= 1)
                        {
                            break;
                        }
                    }

                    portStatus = usbHostState->usbRegBase->PORTSC1;
                    portStatus &= ~(USB_HOST_IP3516HS_PORTSC1_WIC | USB_HOST_IP3516HS_PORTSC1_FPR_MASK);
                    portStatus |= USB_HOST_IP3516HS_PORTSC1_PP_MASK | USB_HOST_IP3516HS_PORTSC1_PED_MASK;
                    usbHostState->usbRegBase->PORTSC1 = portStatus;
                    /* call host callback function, function is initialized in USB_HostInit */
                    hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                kUSB_HostEventL1Resumed); /* call host callback function */
                    hostPointer->suspendedDevice = NULL;
                    usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                }
#if (defined(FSL_FEATURE_USBHSH_VERSION) && (FSL_FEATURE_USBHSH_VERSION >= 300U))
                else if ((kBus_Ip3516HsL1StartSleep == usbHostState->busSuspendStatus))
                {
                    usb_host_instance_t *hostPointer = (usb_host_instance_t *)usbHostState->hostHandle;
                    portStatus = usbHostState->usbRegBase->PORTSC1;

                    if ((usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_SUSP_MASK) &&
                        (0x00 == (portStatus & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK)))
                    {
                        portStatus = usbHostState->usbRegBase->USBCMD;
                        portStatus &= ~USB_HOST_IP3516HS_USBCMD_RS_MASK;
                        usbHostState->usbRegBase->USBCMD = portStatus;

                        usbHostState->busSuspendStatus = kBus_Ip3516HsL1Sleeped;
                        /* call host callback function, function is initialized in USB_HostInit */
                        hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                    kUSB_HostEventL1Sleeped); /* call host callback function */
                    }
                    else if (0x02 == (portStatus & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK))
                    {
                        usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                        /* call host callback function, function is initialized in USB_HostInit */
                        hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                    kUSB_HostEventL1SleepNotSupport); /* call host callback function */
                    }
                    else if (0x01 == (portStatus & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK))
                    {
                        usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                        /* call host callback function, function is initialized in USB_HostInit */
                        hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                    kUSB_HostEventL1SleepNYET); /* call host callback function */
                    }
                    else if (0x03 == (portStatus & USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK))
                    {
                        usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                        /* call host callback function, function is initialized in USB_HostInit */
                        hostPointer->deviceCallback(hostPointer->suspendedDevice, NULL,
                                                    kUSB_HostEventL1SleepError); /* call host callback function */
                    }
                    else
                    {
                        usbHostState->busSuspendStatus = kBus_Ip3516HsIdle;
                    }
                }
#else

#endif
#endif
#endif
                break;
            }

            for (index = 0; index < USB_HOST_IP3516HS_PORT_CONNECT_DEBOUNCE_DELAY; index++)
            {
                USB_HostIp3516HsDelay(usbHostState, 8);
                if (!(usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_CCS_MASK))
                {
                    break;
                }
            }

            portStatus = usbHostState->usbRegBase->PORTSC1;

            /* CCS is cleared ?*/
            if ((!(portStatus & USB_HOST_IP3516HS_PORTSC1_CCS_MASK)) ||
                (index < USB_HOST_IP3516HS_PORT_CONNECT_DEBOUNCE_DELAY))
            {
                usbHostState->portState[i].portStatus = kUSB_DeviceIp3516HsPortDetached;
                continue;
            }

            USB_HostIp3516HsControlBus(usbHostState, kUSB_HostBusReset);
            USB_HostIp3516HsDelay(usbHostState, 81);
            usbHostState->portState[i].portSpeed =
                ((usbHostState->usbRegBase->PORTSC1 & USB_HOST_IP3516HS_PORTSC1_PSPD_MASK) >>
                 USB_HOST_IP3516HS_PORTSC1_PSPD_SHIFT);
            if (0x00 == usbHostState->portState[i].portSpeed)
            {
                usbHostState->portState[i].portSpeed = USB_SPEED_LOW;
            }
            else if (0x01 == usbHostState->portState[i].portSpeed)
            {
                usbHostState->portState[i].portSpeed = USB_SPEED_FULL;
            }
            else
            {
            }
            usbHostState->portState[i].portStatus = kUSB_DeviceIp3516HsPortPhyAttached;
            USB_OsaEventSet(usbHostState->ip3516HsEvent, USB_HOST_IP3516HS_EVENT_ATTACH);
        }
        else
        {
            if (kUSB_DeviceIp3516HsPortDetached == usbHostState->portState[i].portStatus)
            {
                continue;
            }

            if (kUSB_DeviceIp3516HsPortAttached == usbHostState->portState[i].portStatus)
            {
                usbHostState->portState[i].portStatus = kUSB_DeviceIp3516HsPortPhyDetached;
                USB_OsaEventSet(usbHostState->ip3516HsEvent, USB_HOST_IP3516HS_EVENT_DETACH);
            }
            else
            {
                usbHostState->portState[i].portStatus = kUSB_DeviceIp3516HsPortDetached;
            }
        }
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsFillSlotBusTime(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                    usb_host_ip3516hs_pipe_struct_t *pipe,
                                                    uint32_t startUFrame,
                                                    uint8_t *slotTime,
                                                    uint8_t *ssSlot,
                                                    uint8_t *csSlot)
{
    uint8_t slots = (pipe->pipeCommon.maxPacketSize + 187) / 188;
    uint8_t startSlot = startUFrame % 8;

    for (int i = 0; i < 8U; i++)
    {
        slotTime[i] = 0U;
    }
    *ssSlot = 0U;
    *csSlot = 0U;

    if (USB_ENDPOINT_ISOCHRONOUS == pipe->pipeCommon.pipeType)
    {
        if (USB_OUT == pipe->pipeCommon.direction)
        {
            if ((startSlot + slots) > 7U)
            {
                return kStatus_USB_Error;
            }
            for (int i = 0; i < (slots); i++)
            {
                slotTime[startSlot + i] = 125;
                *ssSlot |= 1U << (startSlot + i);
            }
            slotTime[startSlot + slots - 1] = ((pipe->pipeCommon.maxPacketSize % 188) * 125) / 188;
        }
        else
        {
            if ((startSlot + slots + 1) > 8U)
            {
                return kStatus_USB_Error;
            }
            *ssSlot = 1U << startSlot;
            for (int i = 0; i < (slots); i++)
            {
                slotTime[startSlot + 1 + i] = 125;
                *csSlot |= 1U << (startSlot + 1 + i);
            }
            slotTime[startSlot + 1 + slots - 1] = ((pipe->pipeCommon.maxPacketSize % 188) * 125) / 188;
        }
    }
    else if (USB_ENDPOINT_INTERRUPT == pipe->pipeCommon.pipeType)
    {
        if ((startSlot + 3 + 2) > 8U)
        {
            return kStatus_USB_Error;
        }
        *ssSlot = 1U << startSlot;
        for (int i = 0; i < 3; i++)
        {
            *csSlot |= 1U << (startSlot + 2 + i);
        }
        if (USB_OUT == pipe->pipeCommon.direction)
        {
            slotTime[startSlot] = pipe->busNoneHsTime;
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {
                slotTime[startSlot + 2 + i] = pipe->busNoneHsTime;
            }
        }
    }
    else
    {
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsFindStartFrame(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                   usb_host_ip3516hs_pipe_struct_t *pipe)
{
    usb_host_ip3516hs_pipe_struct_t *p;
    uint32_t startUFrame;
    uint32_t frame;
    uint32_t total = 0U;
    uint8_t slotTime[8];
    uint8_t speed = ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed;

    pipe->startUFrame = 0U;

    if (NULL == usbHostState->pipeListInUsing)
    {
        return kStatus_USB_Success;
    }

    for (startUFrame = 0U; startUFrame < pipe->pipeCommon.interval; startUFrame++)
    {
        for (frame = startUFrame; frame < USB_HOST_IP3516HS_MAX_UFRAME; frame += pipe->pipeCommon.interval)
        {
            total = pipe->busHsTime;
            for (p = usbHostState->pipeListInUsing; NULL != p;
                 p = (usb_host_ip3516hs_pipe_struct_t *)p->pipeCommon.next)
            {
                if ((frame >= p->startUFrame) && ((p->pipeCommon.pipeType == USB_ENDPOINT_ISOCHRONOUS) ||
                                                  (p->pipeCommon.pipeType == USB_ENDPOINT_INTERRUPT)))
                {
                    if (USB_SPEED_HIGH == speed)
                    {
                        if (0U == ((frame - p->startUFrame) % p->pipeCommon.interval))
                        {
                            total += p->busHsTime;
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        if (USB_OUT == p->pipeCommon.direction)
                        {
                            if (0U == ((frame - p->startUFrame) % p->pipeCommon.interval))
                            {
                                total += p->busHsTime;
                            }
                            else
                            {
                            }
                        }
                        else
                        {
                            for (int cs = 0; cs < 8U; cs++)
                            {
                                if (p->csSlot & (1U << cs))
                                {
                                    if (0U ==
                                        ((frame - ((p->startUFrame & 0xFFFFFFF8U) + cs)) % p->pipeCommon.interval))
                                    {
                                        total += p->busHsTime;
                                    }
                                    else
                                    {
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (total > USB_HOST_IP3516HS_PERIODIC_BANDWIDTH)
            {
                break;
            }
        }

        do
        {
            if ((frame >= USB_HOST_IP3516HS_MAX_UFRAME) && (USB_SPEED_HIGH != speed))
            {
                uint32_t hsHubNumber;
                usb_status_t error;
                uint8_t totalSlotTime[8];
                uint8_t deviceSlotTime[8];

                frame = startUFrame;
                error = USB_HostIp3516HsFillSlotBusTime(usbHostState, pipe, startUFrame, slotTime, &pipe->ssSlot,
                                                        &pipe->csSlot);
                if (kStatus_USB_Success != error)
                {
                    break;
                }

                for (int i = 0; i < 8U; i++)
                {
                    totalSlotTime[i] = 0U;
                }
                /* compute FS/LS bandwidth that blong to same high-speed hub, because FS/LS bandwidth is allocated from
                 * first parent high-speed hub */
                USB_HostHelperGetPeripheralInformation(pipe->pipeCommon.deviceHandle, kUSB_HostGetDeviceHSHubNumber,
                                                       &hsHubNumber);
                for (frame = startUFrame; frame < USB_HOST_IP3516HS_MAX_UFRAME; frame += pipe->pipeCommon.interval)
                {
                    total = 0U;
                    for (p = usbHostState->pipeListInUsing; NULL != p;
                         p = (usb_host_ip3516hs_pipe_struct_t *)p->pipeCommon.next)
                    {
                        if ((frame >= p->startUFrame) && ((p->pipeCommon.pipeType == USB_ENDPOINT_ISOCHRONOUS) ||
                                                          (p->pipeCommon.pipeType == USB_ENDPOINT_INTERRUPT)))
                        {
                            uint32_t deviceHsHubNumber;
                            USB_HostHelperGetPeripheralInformation(p->pipeCommon.deviceHandle,
                                                                   kUSB_HostGetDeviceHSHubNumber, &deviceHsHubNumber);
                            if ((deviceHsHubNumber == hsHubNumber) &&
                                (0U ==
                                 (((frame & 0xFFFFFFF8U) - (p->startUFrame & 0xFFFFFFF8U)) % p->pipeCommon.interval)))
                            {
                                uint8_t ss;
                                uint8_t cs;
                                USB_HostIp3516HsFillSlotBusTime(usbHostState, pipe, startUFrame, deviceSlotTime, &ss,
                                                                &cs);
                                for (int i = 0; i < 8U; i++)
                                {
                                    totalSlotTime[i] += deviceSlotTime[i];
                                }
                            }
                        }
                    }

                    for (int i = 0; i < 8U; i++)
                    {
                        if ((totalSlotTime[i] + slotTime[i]) > s_SlotMaxBandwidth[i])
                        {
                            break;
                        }
                    }
                }
            }
        } while (0);

        if (frame >= USB_HOST_IP3516HS_MAX_UFRAME)
        {
            pipe->startUFrame = startUFrame;
            return kStatus_USB_Success;
        }
    }

    return kStatus_USB_BandwidthFail;
}

static usb_status_t USB_HostIp3516BaudWidthCheck(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                 usb_host_ip3516hs_pipe_struct_t *pipe)
{
    usb_status_t error;
    uint8_t speed = ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed;
    USB_OSA_SR_ALLOC();

    pipe->busHsTime = USB_HostIp3516HsBusTime(USB_SPEED_HIGH, pipe->pipeCommon.pipeType, pipe->pipeCommon.direction,
                                              pipe->pipeCommon.maxPacketSize * pipe->pipeCommon.numberPerUframe);

    if (USB_SPEED_HIGH != speed)
    {
        uint32_t thinkTime;
        pipe->busNoneHsTime = USB_HostIp3516HsBusTime(speed, pipe->pipeCommon.pipeType, pipe->pipeCommon.direction,
                                                      pipe->pipeCommon.maxPacketSize);
        USB_HostHelperGetPeripheralInformation(pipe->pipeCommon.deviceHandle, kUSB_HostGetHubThinkTime,
                                               &thinkTime); /* Get the hub think time */
        pipe->busNoneHsTime += (thinkTime * 7 / (6 * 12));
    }
    USB_OSA_ENTER_CRITICAL();
    error = USB_HostIp3516HsFindStartFrame(usbHostState, pipe);
    USB_OSA_EXIT_CRITICAL();
    return error;
}

static usb_status_t USB_HostIp3516HsOpenControlBulkPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                        usb_host_ip3516hs_pipe_struct_t *pipe)
{
    usb_host_ip3516hs_atl_struct_t *atl;
    USB_OSA_SR_ALLOC();

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    for (int i = 0;
         i < (sizeof(s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl) / sizeof(usb_host_ip3516hs_atl_struct_t));
         i++)
    {
        if (0 == s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[i].control1Union.stateBitField.MaxPacketLength)
        {
            pipe->tdIndex = i;
            s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[i].control1Union.stateBitField.MaxPacketLength =
                pipe->pipeCommon.maxPacketSize;
            break;
        }
    }
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    if (0xFFU == pipe->tdIndex)
    {
        return kStatus_USB_Busy;
    }
    atl = &s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex];
    atl->control1Union.stateBitField.Mult = pipe->pipeCommon.numberPerUframe;
    atl->control2Union.stateBitField.EP = pipe->pipeCommon.endpointAddress;
    atl->control2Union.stateBitField.DeviceAddress =
        ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->setAddress;
    if (USB_SPEED_HIGH == usbHostState->portState[0].portSpeed)
    {
        atl->control2Union.stateBitField.S =
            (USB_SPEED_HIGH == ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed) ? 0U : 1U;
    }
    else
    {
        atl->control2Union.stateBitField.S = 0U;
    }
    atl->control2Union.stateBitField.RL = 0xFU;
    atl->stateUnion.stateBitField.NakCnt = 0xFU;
    atl->control2Union.stateBitField.SE =
        (USB_SPEED_LOW == ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed) ? 2U : 0U;
    atl->control2Union.stateBitField.PortNumber =
#if (defined(USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB > 0U))
        ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->hsHubPort;
#else
        0U;
#endif
    atl->control2Union.stateBitField.HubAddress =
#if (defined(USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB > 0U))
        ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->hsHubNumber;
#else
        0U;
#endif
    atl->dataUnion.dataBitField.I = 1U;
    atl->stateUnion.stateBitField.EpType = pipe->pipeCommon.pipeType;
    atl->stateUnion.stateBitField.DT = 0U;
    atl->stateUnion.stateBitField.P = 0U;

    return kStatus_USB_Success;
}

#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
static usb_status_t USB_HostIp3516HsOpenIsoPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                usb_host_ip3516hs_pipe_struct_t *pipe)
{
    usb_host_ip3516hs_ptl_struct_t *ptl;
    usb_host_ip3516hs_sptl_struct_t *sptl;
    usb_status_t error;
    USB_OSA_SR_ALLOC();

    error = USB_HostIp3516BaudWidthCheck(usbHostState, pipe);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    for (int i = 0; i < USB_HOST_CONFIG_IP3516HS_MAX_ISO; i++)
    {
        if (0 == s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[i].control1Union.stateBitField.MaxPacketLength)
        {
            pipe->tdIndex = i;
            s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[i].control1Union.stateBitField.MaxPacketLength =
                pipe->pipeCommon.maxPacketSize;
            break;
        }
    }
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    if (0xFFU == pipe->tdIndex)
    {
        return kStatus_USB_Busy;
    }

    ptl = &s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[pipe->tdIndex];
    sptl = (usb_host_ip3516hs_sptl_struct_t *)ptl;
    ptl->control1Union.stateBitField.Mult = pipe->pipeCommon.numberPerUframe;
    ptl->control2Union.stateBitField.EP = pipe->pipeCommon.endpointAddress;
    ptl->control2Union.stateBitField.DeviceAddress =
        ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->setAddress;
    if (USB_SPEED_HIGH == usbHostState->portState[0].portSpeed)
    {
        ptl->control2Union.stateBitField.S =
            (USB_SPEED_HIGH == ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed) ? 0U : 1U;
    }
    else
    {
        ptl->control2Union.stateBitField.S = 0U;
    }
    ptl->control2Union.stateBitField.RL = 0U;
    ptl->stateUnion.stateBitField.NakCnt = 0U;
    ptl->control2Union.stateBitField.SE =
        (USB_SPEED_LOW == ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed) ? 2U : 0U;
    if (ptl->control2Union.stateBitField.S)
    {
        sptl->control2Union.stateBitField.PortNumber =
#if (defined(USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB > 0U))
            ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->hsHubPort;
#else
            0U;
#endif
        sptl->control2Union.stateBitField.HubAddress =
#if (defined(USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB > 0U))
            ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->hsHubNumber;
#else
            0U;
#endif
    }
    else
    {
        sptl->control2Union.stateBitField.PortNumber = 0U;
        sptl->control2Union.stateBitField.HubAddress = 0U;
    }
    ptl->dataUnion.dataBitField.I = 1U;
    ptl->stateUnion.stateBitField.EpType = pipe->pipeCommon.pipeType;
    ptl->stateUnion.stateBitField.DT = 0U;
    ptl->stateUnion.stateBitField.P = 0U;

    return kStatus_USB_Success;
}
#endif

#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_INT) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
static usb_status_t USB_HostIp3516HsOpenInterruptPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                      usb_host_ip3516hs_pipe_struct_t *pipe)
{
    usb_host_ip3516hs_ptl_struct_t *ptl;
    usb_host_ip3516hs_sptl_struct_t *sptl;
    usb_status_t error;
    USB_OSA_SR_ALLOC();

    error = USB_HostIp3516BaudWidthCheck(usbHostState, pipe);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    for (int i = 0; i < USB_HOST_CONFIG_IP3516HS_MAX_INT; i++)
    {
        if (0 ==
            s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[i].control1Union.stateBitField.MaxPacketLength)
        {
            pipe->tdIndex = i;
            s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[i].control1Union.stateBitField.MaxPacketLength =
                pipe->pipeCommon.maxPacketSize;
            break;
        }
    }
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    if (0xFFU == pipe->tdIndex)
    {
        pipe->pipeCommon.open = 0U;
        /* release the pipe */
        USB_HostIp3516HsInsertPipe(&usbHostState->pipeList, pipe);
        return kStatus_USB_Busy;
    }
    ptl = &s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[pipe->tdIndex];
    sptl = (usb_host_ip3516hs_sptl_struct_t *)ptl;
    ptl->control1Union.stateBitField.Mult = pipe->pipeCommon.numberPerUframe;
    ptl->control2Union.stateBitField.EP = pipe->pipeCommon.endpointAddress;
    ptl->control2Union.stateBitField.DeviceAddress =
        ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->setAddress;
    if (USB_SPEED_HIGH == usbHostState->portState[0].portSpeed)
    {
        ptl->control2Union.stateBitField.S =
            (USB_SPEED_HIGH == ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed) ? 0U : 1U;
    }
    else
    {
        ptl->control2Union.stateBitField.S = 0U;
    }
    ptl->control2Union.stateBitField.RL = 0xFU;
    ptl->stateUnion.stateBitField.NakCnt = 0xFU;
    ptl->control2Union.stateBitField.SE =
        (USB_SPEED_LOW == ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed) ? 2U : 0U;
    if (ptl->control2Union.stateBitField.S)
    {
        sptl->control2Union.stateBitField.PortNumber =
#if (defined(USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB > 0U))
            ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->hsHubPort;
#else
            0U;
#endif
        sptl->control2Union.stateBitField.HubAddress =
#if (defined(USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB > 0U))
            ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->hsHubNumber;
#else
            0U;
#endif
    }
    else
    {
        sptl->control2Union.stateBitField.PortNumber = 0U;
        sptl->control2Union.stateBitField.HubAddress = 0U;
    }
    ptl->dataUnion.dataBitField.I = 1U;
    ptl->stateUnion.stateBitField.EpType = pipe->pipeCommon.pipeType;
    ptl->stateUnion.stateBitField.DT = 0U;
    ptl->stateUnion.stateBitField.P = 0U;

    return kStatus_USB_Success;
}
#endif

static usb_status_t USB_HostIp3516HsCancelPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                               usb_host_ip3516hs_pipe_struct_t *pipe,
                                               usb_host_transfer_t *tr)
{
    usb_host_transfer_t *trCurrent;
    usb_host_transfer_t *trPos;
    usb_host_transfer_t *trPre;
    uint32_t startUFrame;
    uint32_t currentUFrame;
    usb_status_t trStatus = kStatus_USB_Success;

    switch (pipe->pipeCommon.pipeType)
    {
        case USB_ENDPOINT_BULK:
        case USB_ENDPOINT_CONTROL:
            usbHostState->usbRegBase->ATL_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
            break;
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
        case USB_ENDPOINT_INTERRUPT:
            usbHostState->usbRegBase->INT_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
            break;
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
        case USB_ENDPOINT_ISOCHRONOUS:
            usbHostState->usbRegBase->ISO_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
            break;
#endif
        default:
            break;
    }
    startUFrame = (uint32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                             USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
    currentUFrame = startUFrame;

    while (currentUFrame == startUFrame)
    {
        currentUFrame = (uint32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                                   USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
    }

    USB_HostIp3516HsLock();
    trCurrent = pipe->trList;
    trPre = NULL;
    while (trCurrent)
    {
        trPos = trCurrent->next;
        if ((NULL == tr) || (tr == trCurrent))
        {
            switch (pipe->pipeCommon.pipeType)
            {
                case USB_ENDPOINT_BULK:
                case USB_ENDPOINT_CONTROL:
                    trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                    .atl[pipe->tdIndex]
                                                    .stateUnion.stateBitField.NrBytesToTransfer;
                    if (s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.H)
                    {
                        trStatus = kStatus_USB_TransferStall;
                    }
                    else if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .atl[pipe->tdIndex]
                                  .stateUnion.stateBitField.B) &&
                             (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .atl[pipe->tdIndex]
                                  .stateUnion.stateBitField.X))
                    {
                        trStatus = kStatus_USB_TransferFailed;
                    }
                    else
                    {
                        if ((trCurrent->transferSofar != trCurrent->transferLength) &&
                            (kStatus_USB_Success == trStatus))
                        {
                            trStatus = kStatus_USB_TransferCancel;
                        }
                    }
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.A = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].control1Union.stateBitField.V =
                        0U;
                    break;
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
                case USB_ENDPOINT_INTERRUPT:
                    trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                    .interrupt[pipe->tdIndex]
                                                    .stateUnion.stateBitField.NrBytesToTransfer;
                    if (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                            .interrupt[pipe->tdIndex]
                            .stateUnion.stateBitField.H)
                    {
                        trStatus = kStatus_USB_TransferStall;
                    }
                    else if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .interrupt[pipe->tdIndex]
                                  .stateUnion.stateBitField.B) &&
                             (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .interrupt[pipe->tdIndex]
                                  .stateUnion.stateBitField.X))
                    {
                        trStatus = kStatus_USB_TransferFailed;
                    }
                    else
                    {
                        if ((!trCurrent->transferSofar) && (kStatus_USB_Success == trStatus))
                        {
                            trStatus = kStatus_USB_TransferCancel;
                        }
                    }
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .interrupt[pipe->tdIndex]
                        .stateUnion.stateBitField.A = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .interrupt[pipe->tdIndex]
                        .control1Union.stateBitField.V = 0U;
                    break;
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
                case USB_ENDPOINT_ISOCHRONOUS:
                    trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                    .iso[pipe->tdIndex]
                                                    .stateUnion.stateBitField.NrBytesToTransfer;
                    if (s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[pipe->tdIndex].stateUnion.stateBitField.H)
                    {
                        trStatus = kStatus_USB_TransferStall;
                    }
                    else if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .iso[pipe->tdIndex]
                                  .stateUnion.stateBitField.B) &&
                             (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .iso[pipe->tdIndex]
                                  .stateUnion.stateBitField.X))
                    {
                        trStatus = kStatus_USB_TransferFailed;
                    }
                    else
                    {
                        if ((!trCurrent->transferSofar) && (kStatus_USB_Success == trStatus))
                        {
                            trStatus = kStatus_USB_TransferCancel;
                        }
                    }
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[pipe->tdIndex].stateUnion.stateBitField.A = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[pipe->tdIndex].control1Union.stateBitField.V =
                        0U;
                    break;
#endif
                default:
                    break;
            }

            if (pipe->bufferLength)
            {
                USB_HostIp3516HsFreeBuffer(usbHostState, pipe->bufferIndex, pipe->bufferLength);
                pipe->bufferLength = 0U;
            }
            if (NULL != trPre)
            {
                trPre->next = trPos;
            }
            else
            {
                pipe->trList = trPos;
            }
            pipe->cutOffTime = USB_HOST_IP3516HS_TRANSFER_TIMEOUT_GAP;
            pipe->isBusy = 0U;
            /* callback function is different from the current condition */
            trCurrent->callbackFn(trCurrent->callbackParam, trCurrent, trStatus); /* transfer callback */
        }
        else
        {
            trPre = trCurrent;
        }
        trCurrent = trPos;
    }
    USB_HostIp3516HsUnlock();
    switch (pipe->pipeCommon.pipeType)
    {
        case USB_ENDPOINT_BULK:
        case USB_ENDPOINT_CONTROL:
            usbHostState->usbRegBase->ATL_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
            break;
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
        case USB_ENDPOINT_INTERRUPT:
            usbHostState->usbRegBase->INT_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
            break;
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
        case USB_ENDPOINT_ISOCHRONOUS:
            usbHostState->usbRegBase->ISO_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
            break;
#endif
        default:
            break;
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsCloseControlBulkPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                         usb_host_ip3516hs_pipe_struct_t *pipe)
{
    USB_OSA_SR_ALLOC();

    USB_HostIp3516HsCancelPipe(usbHostState, pipe, NULL);

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].control1Union.stateBitField.V = 0U;
    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].control1Union.stateBitField.MaxPacketLength =
        0U;
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    return kStatus_USB_Success;
}

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
static usb_status_t USB_HostIp3516HsCloseIsoPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                 usb_host_ip3516hs_pipe_struct_t *pipe)
{
    USB_OSA_SR_ALLOC();

    USB_HostIp3516HsCancelPipe(usbHostState, pipe, NULL);

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[pipe->tdIndex].control1Union.stateBitField.V = 0U;
    s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[pipe->tdIndex].control1Union.stateBitField.MaxPacketLength =
        0U;
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    return kStatus_USB_Success;
}
#endif

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
static usb_status_t USB_HostIp3516HsCloseInterruptPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                       usb_host_ip3516hs_pipe_struct_t *pipe)
{
    USB_OSA_SR_ALLOC();

    USB_HostIp3516HsCancelPipe(usbHostState, pipe, NULL);

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();
    s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[pipe->tdIndex].control1Union.stateBitField.V = 0U;
    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
        .interrupt[pipe->tdIndex]
        .control1Union.stateBitField.MaxPacketLength = 0U;
    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    return kStatus_USB_Success;
}
#endif

static usb_status_t USB_HostIp3516HsGetBuffer(usb_host_ip3516hs_state_struct_t *usbHostState,
                                              uint32_t length,
                                              uint32_t MaxPacketSize,
                                              uint32_t *index,
                                              uint32_t *bufferLength)
{
    uint32_t i;
    uint32_t startIndex = 0xFFFFFFFFU;
    uint32_t freeBufferSize = 0U;
    USB_OSA_SR_ALLOC();

    USB_OSA_ENTER_CRITICAL();
    for (i = 0U; i < (sizeof(s_UsbHostIp3516HsBufferArray) / (sizeof(uint8_t) * 64U)); i++)
    {
        if (usbHostState->bufferArrayBitMap[i >> 5] & (1U << (i % 32)))
        {
            freeBufferSize = 0U;
            startIndex = 0xFFFFFFFFU;
        }
        else
        {
            freeBufferSize += 64U;
            if (0xFFFFFFFFU == startIndex)
            {
                startIndex = i;
            }
            if (length <= freeBufferSize)
            {
                break;
            }
        }
    }
    if (0xFFFFFFFFU != startIndex)
    {
        *index = startIndex;
        *bufferLength = freeBufferSize;
        for (; startIndex <= i; startIndex++)
        {
            usbHostState->bufferArrayBitMap[startIndex >> 5] |= (1U << (startIndex % 32));
        }
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Success;
    }

    if (MaxPacketSize >= length)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Busy;
    }

    for (i = 0U; i < (sizeof(s_UsbHostIp3516HsBufferArray) / (sizeof(uint8_t) * 64U)); i++)
    {
        if (usbHostState->bufferArrayBitMap[i >> 5] & (1U << (i % 32)))
        {
            freeBufferSize = 0U;
            startIndex = 0xFFFFFFFFU;
        }
        else
        {
            freeBufferSize += 64U;
            if (0xFFFFFFFFU == startIndex)
            {
                startIndex = i;
            }
            if (length <= MaxPacketSize)
            {
                break;
            }
        }
    }
    if (0xFFFFFFFFU != startIndex)
    {
        *index = startIndex;
        *bufferLength = freeBufferSize;
        for (; startIndex <= i; startIndex++)
        {
            usbHostState->bufferArrayBitMap[startIndex >> 5] |= (1U << (startIndex % 32));
        }
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Success;
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Busy;
}

static usb_status_t USB_HostIp3516HsFreeBuffer(usb_host_ip3516hs_state_struct_t *usbHostState,
                                               uint32_t index,
                                               uint32_t bufferLength)
{
    uint32_t i;
    uint32_t indexCount = (bufferLength - 1U) / 64U + 1U;
    if ((indexCount + index) > (sizeof(s_UsbHostIp3516HsBufferArray) / (sizeof(uint8_t) * 64U)))
    {
        indexCount = (sizeof(s_UsbHostIp3516HsBufferArray) / (sizeof(uint8_t) * 64U)) - index;
    }
    for (i = index; i < (index + indexCount); i++)
    {
        usbHostState->bufferArrayBitMap[i >> 5] &= ~(1U << (i % 32));
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsWriteControlPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                     usb_host_ip3516hs_pipe_struct_t *pipe,
                                                     usb_host_transfer_t *tr)
{
    usb_host_ip3516hs_atl_struct_t *atl = &s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex];
    uint8_t *bufferAddress;
    uint32_t transferLength;

#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
    uint8_t timeCount;
#endif
    USB_OSA_SR_ALLOC();
    if (1U == pipe->isBusy)
    {
        return kStatus_USB_Success;
    }
    USB_OSA_ENTER_CRITICAL();
    pipe->isBusy = 1U;
    USB_OSA_EXIT_CRITICAL();
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
    if (1U == usbHostState->complianceTest)
    {
        timeCount = 15 * 8; /* 15s */
        while (timeCount--)
        {
            USB_HostIp3516HsDelay(usbHostState, 1000U);
        }
    }
#endif

    switch (tr->setupStatus)
    {
        case kStatus_UsbHostIp3516Hs_Idle:
            if (pipe->bufferLength)
            {
                bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
            }
            else
            {
                transferLength = pipe->pipeCommon.maxPacketSize;
                if (kStatus_USB_Success == USB_HostIp3516HsGetBuffer(usbHostState, transferLength,
                                                                     pipe->pipeCommon.maxPacketSize, &pipe->bufferIndex,
                                                                     &pipe->bufferLength))
                {
                    bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
                }
                else
                {
                    USB_OSA_ENTER_CRITICAL();
                    pipe->isBusy = 2U;
                    USB_OSA_EXIT_CRITICAL();
                    return kStatus_USB_Success;
                }
            }
            transferLength = pipe->bufferLength;
            for (int i = 0; i < 8; i++)
            {
                bufferAddress[i] = ((uint8_t *)tr->setupPacket)[i];
            }
            atl->control2Union.stateBitField.RL = 0U;
            atl->stateUnion.stateBitField.P = 0U;
            atl->stateUnion.stateBitField.NakCnt = 0U;
            atl->stateUnion.stateBitField.Cerr = 0x3U;
            atl->dataUnion.dataBitField.NrBytesToTransfer = 8U;
            atl->stateUnion.stateBitField.NrBytesToTransfer = 0U;
            atl->dataUnion.dataBitField.DataStartAddress = ((uint32_t)(bufferAddress)) & 0x0000FFFFU;
            atl->dataUnion.dataBitField.I = 1U;
            atl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_SETUP;
            atl->stateUnion.stateBitField.DT = 0x00U;
            atl->stateUnion.stateBitField.SC = 0x00U;
            tr->setupStatus = kStatus_UsbHostIp3516Hs_Setup;
            atl->control1Union.stateBitField.V = 0x01U;
            atl->stateUnion.stateBitField.A = 0x01U;
            break;
        case kStatus_UsbHostIp3516Hs_Data2:
        case kStatus_UsbHostIp3516Hs_Setup:
            transferLength = (tr->transferLength - tr->transferSofar);
            if (transferLength > pipe->bufferLength)
            {
                transferLength = pipe->bufferLength;
            }
            bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
            if (USB_OUT == tr->direction)
            {
                for (int i = 0; i < transferLength; i++)
                {
                    bufferAddress[i] = tr->transferBuffer[tr->transferSofar + i];
                }
            }
            atl->control2Union.stateBitField.RL = 0U;
            atl->stateUnion.stateBitField.NakCnt = 0U;
            atl->stateUnion.stateBitField.Cerr = 0x3U;
            atl->dataUnion.dataBitField.NrBytesToTransfer = transferLength;
            atl->stateUnion.stateBitField.NrBytesToTransfer = 0U;
            atl->dataUnion.dataBitField.DataStartAddress = ((uint32_t)(bufferAddress)) & 0x0000FFFFU;
            atl->dataUnion.dataBitField.I = 1U;
            atl->stateUnion.stateBitField.SC = 0x00U;
            if (transferLength)
            {
                if (USB_IN == tr->direction)
                {
                    atl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_IN;
                    atl->stateUnion.stateBitField.P = 0x00U;
                }
                else
                {
                    atl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_OUT;
                }
                if (transferLength < (tr->transferLength - tr->transferSofar))
                {
                    tr->setupStatus = kStatus_UsbHostIp3516Hs_Data2;
                }
                else
                {
                    tr->setupStatus = kStatus_UsbHostIp3516Hs_Data;
                }
            }
            else
            {
                atl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_IN;
                tr->setupStatus = kStatus_UsbHostIp3516Hs_State;
            }
            atl->control1Union.stateBitField.V = 0x01U;
            atl->stateUnion.stateBitField.A = 0x01U;
            break;
        case kStatus_UsbHostIp3516Hs_Data:
            bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
            atl->control2Union.stateBitField.RL = 0U;
            atl->stateUnion.stateBitField.NakCnt = 0U;
            atl->stateUnion.stateBitField.Cerr = 0x3U;
            atl->dataUnion.dataBitField.NrBytesToTransfer = 0;
            atl->stateUnion.stateBitField.NrBytesToTransfer = 0U;
            atl->dataUnion.dataBitField.DataStartAddress = ((uint32_t)(bufferAddress)) & 0x0000FFFFU;
            atl->dataUnion.dataBitField.I = 1U;
            atl->stateUnion.stateBitField.DT = 1U;
            atl->stateUnion.stateBitField.P = 0x00U;
            atl->stateUnion.stateBitField.SC = 0x00U;
            if (USB_OUT == tr->direction)
            {
                atl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_IN;
            }
            else
            {
                atl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_OUT;
            }
            atl->control1Union.stateBitField.V = 0x01U;
            atl->stateUnion.stateBitField.A = 0x01U;
            tr->setupStatus = kStatus_UsbHostIp3516Hs_State;
            break;
        default:
            USB_OSA_ENTER_CRITICAL();
            pipe->isBusy = 0U;
            USB_OSA_EXIT_CRITICAL();
            tr->setupStatus = kStatus_UsbHostIp3516Hs_Idle;
            break;
    }
    if (!pipe->isBusy)
    {
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsWriteBulkPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                  usb_host_ip3516hs_pipe_struct_t *pipe,
                                                  usb_host_transfer_t *tr)
{
    usb_host_ip3516hs_atl_struct_t *atl = &s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex];
    uint32_t transferLength;
    uint8_t *bufferAddress;
    USB_OSA_SR_ALLOC();

    if (1U == pipe->isBusy)
    {
        return kStatus_USB_Success;
    }

    USB_OSA_ENTER_CRITICAL();
    pipe->isBusy = 1U;
    USB_OSA_EXIT_CRITICAL();

    transferLength = (tr->transferLength - tr->transferSofar);
    if (pipe->bufferLength)
    {
        bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
    }
    else
    {
        if (transferLength > 4096U)
        {
            transferLength = 4096U;
        }
        if (kStatus_USB_Success == USB_HostIp3516HsGetBuffer(usbHostState, transferLength,
                                                             pipe->pipeCommon.maxPacketSize, &pipe->bufferIndex,
                                                             &pipe->bufferLength))
        {
            bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
        }
        else
        {
            USB_OSA_ENTER_CRITICAL();
            pipe->isBusy = 2U;
            USB_OSA_EXIT_CRITICAL();
            return kStatus_USB_Success;
        }
    }
    if (transferLength > pipe->bufferLength)
    {
        transferLength = pipe->bufferLength;
    }

    if (USB_OUT == tr->direction)
    {
        for (int i = 0; i < transferLength; i++)
        {
            bufferAddress[i] = tr->transferBuffer[tr->transferSofar + i];
        }
    }

    atl->control2Union.stateBitField.RL = 0U;
    atl->stateUnion.stateBitField.NakCnt = 0U;
    atl->stateUnion.stateBitField.Cerr = 0x3U;
    atl->dataUnion.dataBitField.NrBytesToTransfer = transferLength;
    atl->stateUnion.stateBitField.NrBytesToTransfer = 0U;
    atl->dataUnion.dataBitField.DataStartAddress = ((uint32_t)(bufferAddress)) & 0x0000FFFFU;
    atl->dataUnion.dataBitField.I = 1U;
    atl->stateUnion.stateBitField.SC = 0x00U;
    if (USB_IN == tr->direction)
    {
        atl->stateUnion.stateBitField.P = 0x00U;
        atl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_IN;
    }
    else
    {
        atl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_OUT;
    }
    atl->control1Union.stateBitField.V = 0x01U;
    atl->stateUnion.stateBitField.A = 0x01U;
    return kStatus_USB_Success;
}

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
static usb_status_t USB_HostIp3516HsWriteIsoPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                 usb_host_ip3516hs_pipe_struct_t *pipe,
                                                 usb_host_transfer_t *tr)
{
    usb_host_ip3516hs_ptl_struct_t *ptl = &s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[pipe->tdIndex];
    usb_host_ip3516hs_sptl_struct_t *sptl = (usb_host_ip3516hs_sptl_struct_t *)ptl;
    uint32_t transferLength;
    uint32_t currentUFrame;
    uint32_t insertUFrame;
    uint8_t *bufferAddress;
    uint8_t speed = ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed;
    USB_OSA_SR_ALLOC();

    if (1U == pipe->isBusy)
    {
        return kStatus_USB_Success;
    }

    USB_OSA_ENTER_CRITICAL();
    pipe->isBusy = 1U;
    USB_OSA_EXIT_CRITICAL();

    transferLength = (tr->transferLength - tr->transferSofar);
    if (pipe->bufferLength)
    {
        bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
    }
    else
    {
        if (kStatus_USB_Success == USB_HostIp3516HsGetBuffer(usbHostState, transferLength,
                                                             pipe->pipeCommon.maxPacketSize, &pipe->bufferIndex,
                                                             &pipe->bufferLength))
        {
            bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
        }
        else
        {
            USB_OSA_ENTER_CRITICAL();
            pipe->isBusy = 2U;
            USB_OSA_EXIT_CRITICAL();
            return kStatus_USB_Success;
        }
    }
    if (transferLength > pipe->bufferLength)
    {
        transferLength = pipe->bufferLength;
    }

    if (USB_OUT == tr->direction)
    {
        for (int i = 0; i < transferLength; i++)
        {
            bufferAddress[i] = tr->transferBuffer[tr->transferSofar + i];
        }
    }

    if (USB_SPEED_HIGH != speed)
    {
        if (transferLength > pipe->pipeCommon.maxPacketSize)
        {
            transferLength = pipe->pipeCommon.maxPacketSize;
        }
    }
    ptl->control2Union.stateBitField.RL = 0U;
    ptl->stateUnion.stateBitField.NakCnt = 0U;
    ptl->stateUnion.stateBitField.Cerr = 0U;
    ptl->dataUnion.dataBitField.NrBytesToTransfer = transferLength;
    ptl->stateUnion.stateBitField.NrBytesToTransfer = 0U;
    ptl->dataUnion.dataBitField.DataStartAddress = ((uint32_t)(bufferAddress)) & 0x0000FFFFU;
    ptl->dataUnion.dataBitField.I = 1U;
    ptl->stateUnion.stateBitField.SC = 0x00U;

    ptl->stateUnion.stateBitField.H = 0U;
    ptl->stateUnion.stateBitField.X = 0U;
    ptl->stateUnion.stateBitField.B = 0U;
    if (USB_IN == tr->direction)
    {
        ptl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_IN;
    }
    else
    {
        ptl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_OUT;
    }

    ptl->statusUnion.status = 0U;
    ptl->isoInUnion1.isoIn = 0U;
    ptl->isoInUnion2.isoIn = 0U;
    ptl->isoInUnion3.isoIn = 0U;

    currentUFrame = (uint32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                               USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
    currentUFrame = currentUFrame & 0xFFU;

    insertUFrame = pipe->startUFrame + ((currentUFrame - pipe->startUFrame + USB_HOST_IP3516HS_PERIODIC_TRANSFER_GAP +
                                         pipe->pipeCommon.interval - 1U) &
                                        (~(pipe->pipeCommon.interval - 1U)));

    if (USB_SPEED_HIGH == speed)
    {
        ptl->control1Union.stateBitField.uFrame = insertUFrame & 0xF8U;
        for (int i = (insertUFrame & 0x07U); i < 8; i += pipe->pipeCommon.interval)
        {
            ptl->statusUnion.statusBitField.uSA |= 1U << i;
        }
    }
    else
    {
        if (ptl->control2Union.stateBitField.S)
        {
#if (defined(FSL_FEATURE_USBHSH_VERSION) && (FSL_FEATURE_USBHSH_VERSION >= 300U))
            if (USB_IN == tr->direction)
            {
                if (transferLength <= 192U)
                {
                    sptl->control1Union.stateBitField.MaxPacketLength = transferLength;
                }
                else
                {
                    sptl->control1Union.stateBitField.MaxPacketLength = 192U;
                }
            }
            else
#endif
            {
                if (transferLength <= 188U)
                {
#if (defined(FSL_FEATURE_USBHSH_VERSION) && (FSL_FEATURE_USBHSH_VERSION >= 300U))
                    sptl->control1Union.stateBitField.MaxPacketLength = transferLength;
#else
                    sptl->control1Union.stateBitField.MaxPacketLength = transferLength + 1U;
#endif
                }
                else
                {
                    sptl->control1Union.stateBitField.MaxPacketLength = 188U;
                }
            }
        }
        else
        {
            sptl->control1Union.stateBitField.MaxPacketLength = pipe->pipeCommon.maxPacketSize;
        }
        sptl->control1Union.stateBitField.uFrame = insertUFrame & 0xF8U;
        sptl->statusUnion.statusBitField.uSA = pipe->ssSlot;
        sptl->isoInUnion1.bitField.uSCS = pipe->csSlot;
    }

    ptl->control1Union.stateBitField.V = 0x01U;
    ptl->stateUnion.stateBitField.A = 0x01U;
    return kStatus_USB_Success;
}
#endif

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
static usb_status_t USB_HostIp3516HsWriteInterruptPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                       usb_host_ip3516hs_pipe_struct_t *pipe,
                                                       usb_host_transfer_t *tr)
{
    usb_host_ip3516hs_ptl_struct_t *ptl = &s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[pipe->tdIndex];
    usb_host_ip3516hs_sptl_struct_t *sptl = (usb_host_ip3516hs_sptl_struct_t *)ptl;
    uint32_t transferLength;
    uint32_t currentUFrame;
    uint32_t insertUFrame;
    uint8_t *bufferAddress;
    uint8_t speed = ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed;
    USB_OSA_SR_ALLOC();

    if (1U == pipe->isBusy)
    {
        return kStatus_USB_Success;
    }

    USB_OSA_ENTER_CRITICAL();
    pipe->isBusy = 1U;
    USB_OSA_EXIT_CRITICAL();

    transferLength = (tr->transferLength - tr->transferSofar);
    if (transferLength > pipe->pipeCommon.maxPacketSize)
    {
        transferLength = pipe->pipeCommon.maxPacketSize;
    }
    if (kStatus_USB_Success == USB_HostIp3516HsGetBuffer(usbHostState, transferLength, pipe->pipeCommon.maxPacketSize,
                                                         &pipe->bufferIndex, &pipe->bufferLength))
    {
        bufferAddress = (uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0];
    }
    else
    {
        USB_OSA_ENTER_CRITICAL();
        pipe->isBusy = 2U;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Success;
    }

    if (USB_OUT == tr->direction)
    {
        for (int i = 0; i < transferLength; i++)
        {
            bufferAddress[i] = tr->transferBuffer[tr->transferSofar + i];
        }
    }

    ptl->control2Union.stateBitField.RL = 0U;
    ptl->stateUnion.stateBitField.NakCnt = 0x0FU;
    ptl->stateUnion.stateBitField.Cerr = 0x3U;
    ptl->dataUnion.dataBitField.NrBytesToTransfer = transferLength;
    ptl->stateUnion.stateBitField.NrBytesToTransfer = 0U;
    ptl->dataUnion.dataBitField.DataStartAddress = ((uint32_t)(bufferAddress)) & 0x0000FFFFU;
    ptl->dataUnion.dataBitField.I = 1U;
    ptl->stateUnion.stateBitField.P = 0x00U;
    ptl->stateUnion.stateBitField.SC = 0x00U;
    if (USB_IN == tr->direction)
    {
        ptl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_IN;
    }
    else
    {
        ptl->stateUnion.stateBitField.Token = USB_HOST_IP3516HS_PTD_TOKEN_OUT;
    }

    ptl->statusUnion.status = 0U;
    ptl->isoInUnion1.isoIn = 0U;
    ptl->isoInUnion2.isoIn = 0U;
    ptl->isoInUnion3.isoIn = 0U;

    currentUFrame = (uint32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                               USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
    currentUFrame = currentUFrame & 0xFFU;

    insertUFrame = pipe->startUFrame + ((currentUFrame - pipe->startUFrame + USB_HOST_IP3516HS_PERIODIC_TRANSFER_GAP +
                                         pipe->pipeCommon.interval - 1U) &
                                        (~(pipe->pipeCommon.interval - 1U)));

    ptl->control1Union.stateBitField.uFrame = (insertUFrame & 0xF8U);
    switch (pipe->pipeCommon.interval)
    {
        case 1:
        case 2:
        case 4:
        case 8:
            break;
        case 16:
            ptl->control1Union.stateBitField.uFrame |= 1U;
            break;
        case 32:
            ptl->control1Union.stateBitField.uFrame |= 2U;
            break;
        case 64:
            ptl->control1Union.stateBitField.uFrame |= 3U;
            break;
        case 128:
            ptl->control1Union.stateBitField.uFrame |= 4U;
            break;
        case 256:
            ptl->control1Union.stateBitField.uFrame |= 5U;
            break;
        default:
            break;
    }

    if (USB_SPEED_HIGH == speed)
    {
        for (int i = (insertUFrame & 0x07U); i < 8; i += pipe->pipeCommon.interval)
        {
            ptl->statusUnion.statusBitField.uSA |= 1U << i;
        }
    }
    else
    {
        sptl->statusUnion.statusBitField.uSA = pipe->ssSlot;
        sptl->isoInUnion1.bitField.uSCS = pipe->csSlot;
    }

    ptl->control1Union.stateBitField.V = 0x01U;
    ptl->stateUnion.stateBitField.A = 0x01U;
    return kStatus_USB_Success;
}
#endif

usb_host_ip3516hs_pipe_struct_t *USB_HostIp3516HsGetPipe(usb_host_ip3516hs_state_struct_t *usbHostState,
                                                         uint8_t pipeType,
                                                         uint8_t tdIndex)
{
    usb_host_ip3516hs_pipe_struct_t *p = usbHostState->pipeListInUsing;
    USB_OSA_SR_ALLOC();

    USB_OSA_ENTER_CRITICAL();
    while (p != NULL)
    {
        if ((p->pipeCommon.pipeType == pipeType) && (p->tdIndex == tdIndex))
        {
            USB_OSA_EXIT_CRITICAL();
            return p;
        }
        p = (usb_host_ip3516hs_pipe_struct_t *)p->pipeCommon.next;
    }
    USB_OSA_EXIT_CRITICAL();
    return NULL;
}

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
static usb_status_t USB_HostIp3516HsCheckIsoTransferSofar(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    uint32_t tokenDoneBits = usbHostState->usbRegBase->ISO_PTD_DONE_MAP;
    usb_host_ip3516hs_pipe_struct_t *p;

    if (!tokenDoneBits)
    {
        return kStatus_USB_Success;
    }
    for (int i = 0; i < 32; i++)
    {
        if (tokenDoneBits & (1U << i))
        {
            if (s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[i].stateUnion.stateBitField.A)
            {
                continue;
            }
            p = USB_HostIp3516HsGetPipe(usbHostState, USB_ENDPOINT_ISOCHRONOUS, i);
            if (NULL != p)
            {
                p->isBusy = 0U;
#if 0
                if (NULL != p->trList)
                {
                    usb_host_transfer_t *trCurrent = p->trList;
                    if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[p->tdIndex].stateUnion.stateBitField.B) ||
                        (s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[p->tdIndex].stateUnion.stateBitField.X))
                    {
                        trCurrent->union1.transferResult = kStatus_USB_TransferFailed;
                    }
                    else
                    {
                        trCurrent->union1.transferResult = kStatus_USB_Success;
                    }
                    if (USB_IN == trCurrent->direction)
                    {
                        for (int i = 0; i < s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                .iso[p->tdIndex]
                                                .stateUnion.stateBitField.NrBytesToTransfer;
                             i++)
                        {
                            trCurrent->transferBuffer[i + trCurrent->transferSofar] =
                                ((uint8_t *)&s_UsbHostIp3516HsBufferArray[p->bufferIndex][0])[i];
                        }
                    }
                    trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                    .iso[p->tdIndex]
                                                    .stateUnion.stateBitField.NrBytesToTransfer;
                    if (p->trList)
                    {
                        p->currentTr = p->trList->next;
                    }
                    else
                    {
                        p->currentTr = NULL;
                    }
                    if (p->currentTr)
                    {
                        uint32_t trValue = (uint32_t)p->currentTr;
                        USB_HostIp3516HsWriteIsoPipe(usbHostState, p, (usb_host_transfer_t *)trValue);
                    }
                }
#endif
            }
        }
    }
    usbHostState->usbRegBase->ISO_PTD_DONE_MAP = tokenDoneBits;
    return kStatus_USB_Success;
}
#endif

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
static usb_status_t USB_HostIp3516HsCheckIntTransferSofar(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    uint32_t tokenDoneBits = usbHostState->usbRegBase->INT_PTD_DONE_MAP;
    usb_host_ip3516hs_pipe_struct_t *p;

    if (!tokenDoneBits)
    {
        return kStatus_USB_Success;
    }
    for (int i = 0; i < 32; i++)
    {
        if (tokenDoneBits & (1U << i))
        {
            if (s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[i].stateUnion.stateBitField.A)
            {
                continue;
            }
            p = USB_HostIp3516HsGetPipe(usbHostState, USB_ENDPOINT_INTERRUPT, i);
            if (NULL != p)
            {
                p->isBusy = 0U;
            }
        }
    }
    usbHostState->usbRegBase->INT_PTD_DONE_MAP = tokenDoneBits;
    return kStatus_USB_Success;
}
#endif

static usb_status_t USB_HostIp3516HsCheckAtlTransferSofar(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    uint32_t tokenDoneBits = usbHostState->usbRegBase->ATL_PTD_DONE_MAP;
    usb_host_ip3516hs_pipe_struct_t *p;

    if (!tokenDoneBits)
    {
        return kStatus_USB_Success;
    }
    for (int i = 0; i < 32; i++)
    {
        if (tokenDoneBits & (1U << i))
        {
            p = USB_HostIp3516HsGetPipe(usbHostState, USB_ENDPOINT_CONTROL, i);
            if (NULL != p)
            {
                p->isBusy = 0U;
            }
            p = USB_HostIp3516HsGetPipe(usbHostState, USB_ENDPOINT_BULK, i);
            if ((NULL != p))
            {
                p->isBusy = 0U;
            }
        }
    }
    usbHostState->usbRegBase->ATL_PTD_DONE_MAP = tokenDoneBits;
    return kStatus_USB_Success;
}

static void USB_HostIp3516HsCheckGetBufferFailedPipe(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    usb_host_ip3516hs_pipe_struct_t *pipe = usbHostState->pipeListInUsing;

    while (pipe)
    {
        if (2U == pipe->isBusy)
        {
            if (pipe->trList)
            {
                switch (pipe->pipeCommon.pipeType)
                {
                    case USB_ENDPOINT_BULK:
                        USB_HostIp3516HsWriteBulkPipe(usbHostState, pipe, pipe->trList);
                        break;

                    case USB_ENDPOINT_CONTROL:
                        USB_HostIp3516HsWriteControlPipe(usbHostState, pipe, pipe->trList);
                        break;

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
                    case USB_ENDPOINT_ISOCHRONOUS:
                        USB_HostIp3516HsWriteIsoPipe(usbHostState, pipe, pipe->trList);
                        break;
#endif

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
                    case USB_ENDPOINT_INTERRUPT:
                        USB_HostIp3516HsWriteInterruptPipe(usbHostState, pipe, pipe->trList);
                        break;
#endif

                    default:
                        break;
                }
            }
        }

        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
    }
}

static usb_status_t USB_HostIp3516HsTokenDone(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    usb_host_ip3516hs_pipe_struct_t *pipe;
    usb_host_transfer_t *trCurrent;
    usb_host_ip3516hs_pipe_struct_t *p;
    uint32_t startUFrame;
    uint32_t currentUFrame;
    usb_status_t trStatus;
    uint8_t trDone;

    /* Enter critical */
    USB_HostIp3516HsLock();
    USB_HostOhciDisableIsr(usbHostState);
    pipe = usbHostState->pipeListInUsing;
    while (NULL != pipe)
    {
        trStatus = kStatus_USB_Success;
        trCurrent = pipe->trList;
        if (NULL == trCurrent)
        {
        }
        else
        {
            trDone = 0;
            switch (pipe->pipeCommon.pipeType)
            {
                case USB_ENDPOINT_CONTROL:
                    if (pipe->isBusy)
                    {
                        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
                        continue;
                    }

                    switch (trCurrent->setupStatus)
                    {
                        case kStatus_UsbHostIp3516Hs_Idle:
                            break;
                        case kStatus_UsbHostIp3516Hs_Setup:
                            break;
                        case kStatus_UsbHostIp3516Hs_Data2:
                            if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                     .atl[pipe->tdIndex]
                                     .stateUnion.stateBitField.NrBytesToTransfer) &&
                                (!(s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                       .atl[pipe->tdIndex]
                                       .stateUnion.stateBitField.NrBytesToTransfer %
                                   pipe->pipeCommon.maxPacketSize)))
                            {
                            }
                            else
                            {
                                trCurrent->setupStatus = kStatus_UsbHostIp3516Hs_Data;
                            }
                            if (USB_IN == trCurrent->direction)
                            {
                                for (int i = 0; i < s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                        .atl[pipe->tdIndex]
                                                        .stateUnion.stateBitField.NrBytesToTransfer;
                                     i++)
                                {
                                    trCurrent->transferBuffer[i + trCurrent->transferSofar] =
                                        ((uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0])[i];
                                }
                            }
                            trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                            .atl[pipe->tdIndex]
                                                            .stateUnion.stateBitField.NrBytesToTransfer;
                            break;
                        case kStatus_UsbHostIp3516Hs_Data:
                            if (USB_IN == trCurrent->direction)
                            {
                                for (int i = 0; i < s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                        .atl[pipe->tdIndex]
                                                        .stateUnion.stateBitField.NrBytesToTransfer;
                                     i++)
                                {
                                    trCurrent->transferBuffer[i + trCurrent->transferSofar] =
                                        ((uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0])[i];
                                }
                            }
                            trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                            .atl[pipe->tdIndex]
                                                            .stateUnion.stateBitField.NrBytesToTransfer;
                            break;
                        case kStatus_UsbHostIp3516Hs_State:
                            break;
                        default:
                            break;
                    }

                    if (!s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.A)
                    {
                        trDone = 1;
                    }

                    if (s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.H)
                    {
                        trStatus = kStatus_USB_TransferStall;
                    }
                    else if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .atl[pipe->tdIndex]
                                  .stateUnion.stateBitField.B) ||
                             (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .atl[pipe->tdIndex]
                                  .stateUnion.stateBitField.X))
                    {
                        trStatus = kStatus_USB_TransferFailed;
                    }
                    else
                    {
                        if (trDone)
                        {
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
                            if (1U == usbHostState->complianceTestStart)
                            {
                                usbHostState->complianceTest++;
                            }
#endif
                            USB_HostIp3516HsWriteControlPipe(usbHostState, pipe, pipe->trList);
                        }
                    }

                    if (pipe->isBusy)
                    {
                        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
                        continue;
                    }

                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.H = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.X = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.B = 0U;
                    break;
                case USB_ENDPOINT_BULK:
                    if (pipe->isBusy)
                    {
                        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
                        continue;
                    }

                    if (USB_IN == trCurrent->direction)
                    {
                        for (int i = 0; i < s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                .atl[pipe->tdIndex]
                                                .stateUnion.stateBitField.NrBytesToTransfer;
                             i++)
                        {
                            trCurrent->transferBuffer[i + trCurrent->transferSofar] =
                                ((uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0])[i];
                        }
                    }
                    trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                    .atl[pipe->tdIndex]
                                                    .stateUnion.stateBitField.NrBytesToTransfer;

                    if (!s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.A)
                    {
                        trDone = 1;
                    }

                    if (s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.H)
                    {
                        trStatus = kStatus_USB_TransferStall;
                    }
                    else if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .atl[pipe->tdIndex]
                                  .stateUnion.stateBitField.B) ||
                             (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .atl[pipe->tdIndex]
                                  .stateUnion.stateBitField.X))
                    {
                        trStatus = kStatus_USB_TransferFailed;
                    }
                    else
                    {
                        if (trDone)
                        {
                            if ((!(pipe->trList->transferLength % pipe->pipeCommon.maxPacketSize)) &&
                                (pipe->trList->transferSofar < pipe->trList->transferLength))
                            {
                                USB_HostIp3516HsWriteBulkPipe(usbHostState, pipe, pipe->trList);
                            }
                        }
                    }

                    if (pipe->isBusy)
                    {
                        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
                        continue;
                    }

                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.H = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.X = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[pipe->tdIndex].stateUnion.stateBitField.B = 0U;

                    break;
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
                case USB_ENDPOINT_INTERRUPT:
                    if (pipe->isBusy)
                    {
                        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
                        continue;
                    }

                    if (!s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                             .interrupt[pipe->tdIndex]
                             .stateUnion.stateBitField.A)
                    {
                        trDone = 1;
                    }

                    if (USB_IN == trCurrent->direction)
                    {
                        for (int i = 0; i < s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                .interrupt[pipe->tdIndex]
                                                .stateUnion.stateBitField.NrBytesToTransfer;
                             i++)
                        {
                            trCurrent->transferBuffer[i + trCurrent->transferSofar] =
                                ((uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0])[i];
                        }
                    }
                    trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                    .interrupt[pipe->tdIndex]
                                                    .stateUnion.stateBitField.NrBytesToTransfer;

                    if (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                            .interrupt[pipe->tdIndex]
                            .stateUnion.stateBitField.H)
                    {
                        trStatus = kStatus_USB_TransferStall;
                    }
                    else if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .interrupt[pipe->tdIndex]
                                  .stateUnion.stateBitField.B) ||
                             (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                  .interrupt[pipe->tdIndex]
                                  .stateUnion.stateBitField.X))
                    {
                        trStatus = kStatus_USB_TransferFailed;
                    }
                    else
                    {
                    }
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .interrupt[pipe->tdIndex]
                        .stateUnion.stateBitField.H = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .interrupt[pipe->tdIndex]
                        .stateUnion.stateBitField.X = 0U;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .interrupt[pipe->tdIndex]
                        .stateUnion.stateBitField.B = 0U;
                    if ((!trCurrent->transferSofar) && (kStatus_USB_Success == trStatus))
                    {
                        trStatus = kStatus_USB_TransferFailed;
                    }
                    break;
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
                case USB_ENDPOINT_ISOCHRONOUS:
#if 1
                    if (pipe->isBusy)
                    {
                        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
                        continue;
                    }
                    if (USB_IN == trCurrent->direction)
                    {
                        for (int i = 0; i < s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                .iso[pipe->tdIndex]
                                                .stateUnion.stateBitField.NrBytesToTransfer;
                             i++)
                        {
                            trCurrent->transferBuffer[i + trCurrent->transferSofar] =
                                ((uint8_t *)&s_UsbHostIp3516HsBufferArray[pipe->bufferIndex][0])[i];
                        }
                    }
                    trCurrent->transferSofar += s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                                    .iso[pipe->tdIndex]
                                                    .stateUnion.stateBitField.NrBytesToTransfer;

                    if ((s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                             .iso[pipe->tdIndex]
                             .stateUnion.stateBitField.B) ||
                        (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                             .iso[pipe->tdIndex]
                             .stateUnion.stateBitField.X))
                    {
                        trCurrent->union1.transferResult = kStatus_USB_TransferFailed;
                    }
                    else
                    {
                        trCurrent->union1.transferResult = kStatus_USB_Success;
                    }

                    if (pipe->trList)
                    {
                        pipe->currentTr = pipe->trList->next;
                    }
                    else
                    {
                        pipe->currentTr = NULL;
                    }

                    if (pipe->currentTr)
                    {
                        uint32_t trValue = (uint32_t)pipe->currentTr;
                        USB_HostIp3516HsWriteIsoPipe(usbHostState, pipe, (usb_host_transfer_t *)trValue);
                    }
#else
                    if (pipe->trList == pipe->currentTr)
                    {
                        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
                        continue;
                    }
#endif
                    trDone = 1;

                    trStatus = (usb_status_t)trCurrent->union1.transferResult;

                    if ((!trCurrent->transferSofar) && (kStatus_USB_Success == trStatus))
                    {
                        trStatus = kStatus_USB_TransferFailed;
                    }
                    break;
#endif
                default:
                    break;
            }
            if (trDone)
            {
                pipe->trList = trCurrent->next;

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
                if ((USB_ENDPOINT_ISOCHRONOUS != pipe->pipeCommon.pipeType) && (pipe->bufferLength))
#else
                if ((pipe->bufferLength))
#endif
                {
                    USB_HostIp3516HsFreeBuffer(usbHostState, pipe->bufferIndex, pipe->bufferLength);
                    pipe->bufferLength = 0U;
                }
                pipe->cutOffTime = USB_HOST_IP3516HS_TRANSFER_TIMEOUT_GAP;

                if ((kStatus_USB_Success == (trStatus)) && (USB_ENDPOINT_CONTROL == pipe->pipeCommon.pipeType) &&
                    (USB_REQUEST_STANDARD_CLEAR_FEATURE == trCurrent->setupPacket->bRequest) &&
                    (USB_REQUEST_TYPE_RECIPIENT_ENDPOINT == trCurrent->setupPacket->bmRequestType) &&
                    (USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT ==
                     (USB_SHORT_FROM_LITTLE_ENDIAN_DATA(trCurrent->setupPacket->wValue) & 0x00FFu)))
                {
                    p = usbHostState->pipeListInUsing;
                    while (p != NULL)
                    {
                        /* only compute bulk and interrupt pipe */
                        if ((p->pipeCommon.deviceHandle == pipe->pipeCommon.deviceHandle) &&
                            ((p->pipeCommon.endpointAddress |
                              (p->pipeCommon.direction << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)) ==
                             ((uint8_t)(USB_SHORT_FROM_LITTLE_ENDIAN_DATA(trCurrent->setupPacket->wIndex)))))
                        {
                            break;
                        }
                        p = (usb_host_ip3516hs_pipe_struct_t *)p->pipeCommon.next;
                    }

                    if ((NULL != p) && ((USB_ENDPOINT_BULK == p->pipeCommon.pipeType) ||
                                        (USB_ENDPOINT_INTERRUPT == p->pipeCommon.pipeType)))
                    {
                        if (USB_ENDPOINT_BULK == p->pipeCommon.pipeType)
                        {
                            usbHostState->usbRegBase->ATL_PTD_SKIP_MAP |= 1U << p->tdIndex;
                        }
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
                        else if (USB_ENDPOINT_INTERRUPT == p->pipeCommon.pipeType)
                        {
                            usbHostState->usbRegBase->INT_PTD_SKIP_MAP |= 1U << p->tdIndex;
                        }
#endif
                        else
                        {
                        }

                        startUFrame = (uint32_t)(
                            (usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                            USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
                        currentUFrame = startUFrame;

                        while (currentUFrame == startUFrame)
                        {
                            currentUFrame = (uint32_t)(
                                (usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                                USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
                        }

                        if (USB_ENDPOINT_BULK == p->pipeCommon.pipeType)
                        {
                            s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                .atl[p->tdIndex]
                                .stateUnion.stateBitField.DT = 0U;
                            usbHostState->usbRegBase->ATL_PTD_SKIP_MAP &= ~(1U << p->tdIndex);
                        }
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
                        else if (USB_ENDPOINT_INTERRUPT == p->pipeCommon.pipeType)
                        {
                            s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                                .interrupt[p->tdIndex]
                                .stateUnion.stateBitField.DT = 0U;
                            usbHostState->usbRegBase->INT_PTD_SKIP_MAP &= ~(1U << p->tdIndex);
                        }
#endif
                        else
                        {
                        }
                    }
                }
                /* callback function is different from the current condition */
                trCurrent->callbackFn(trCurrent->callbackParam, trCurrent, trStatus); /* transfer callback */
                USB_HostIp3516HsCheckGetBufferFailedPipe(usbHostState);
                if (NULL != pipe->trList)
                {
                    switch (pipe->pipeCommon.pipeType)
                    {
                        case USB_ENDPOINT_BULK:
                            USB_HostIp3516HsWriteBulkPipe(usbHostState, pipe, pipe->trList);
                            break;

                        case USB_ENDPOINT_CONTROL:
                            USB_HostIp3516HsWriteControlPipe(usbHostState, pipe, pipe->trList);
                            break;

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
                        case USB_ENDPOINT_ISOCHRONOUS:
                            USB_HostIp3516HsWriteIsoPipe(usbHostState, pipe, pipe->trList);
                            break;
#endif

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
                        case USB_ENDPOINT_INTERRUPT:
                            USB_HostIp3516HsWriteInterruptPipe(usbHostState, pipe, pipe->trList);
                            break;
#endif

                        default:
                            break;
                    }
                }
            }
        }
        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
    }
    USB_HostOhciEnableIsr(usbHostState);
    USB_HostIp3516HsUnlock();
    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsSof(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    usb_host_ip3516hs_pipe_struct_t *pipe;

    /* Enter critical */
    USB_HostIp3516HsLock();
    pipe = usbHostState->pipeListInUsing;
    while (pipe)
    {
        if ((USB_ENDPOINT_CONTROL == pipe->pipeCommon.pipeType) || (USB_ENDPOINT_BULK == pipe->pipeCommon.pipeType))
        {
            if (pipe->trList)
            {
                pipe->cutOffTime--;
                if (!pipe->cutOffTime)
                {
                    USB_HostIp3516HsCancelPipe(usbHostState, pipe, pipe->trList);
                }
            }
        }
        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
    }
    /* Exit critical */
    USB_HostIp3516HsUnlock();

    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsControllerReset(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    usbHostState->usbRegBase->USBCMD = USB_HOST_IP3516HS_USBCMD_HCRESET_MASK;
    while (usbHostState->usbRegBase->USBCMD & USB_HOST_IP3516HS_USBCMD_HCRESET_MASK)
    {
        __ASM("nop");
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostIp3516HsControllerInit(usb_host_ip3516hs_state_struct_t *usbHostState)
{
    usb_host_ip3516hs_pipe_struct_t *pipe;
#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_GTD) && (USB_HOST_CONFIG_IP3516HS_MAX_GTD > 0U))
    usb_host_ip3516hs_atl_struct_t *gtd;
#endif
#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_ITD) && (USB_HOST_CONFIG_IP3516HS_MAX_ITD > 0U))
    usb_host_ip3516hs_isochronous_transfer_descritpor_struct_t *itd;
#endif
    uint32_t i;
    uint32_t interruptState;
    USB_OSA_SR_ALLOC();

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();

    for (i = 0U; i < (sizeof(usbHostState->bufferArrayBitMap) / (sizeof(uint32_t))); i++)
    {
        usbHostState->bufferArrayBitMap[i] = 0xFFFFFFFFU;
    }

    for (i = 0U; i < (sizeof(s_UsbHostIp3516HsBufferArray) / (sizeof(uint8_t) * 64)); i++)
    {
        usbHostState->bufferArrayBitMap[i >> 5] &= ~(1U << (i % 32));
    }

    for (i = 0U;
         i < (sizeof(s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl) / sizeof(usb_host_ip3516hs_atl_struct_t));
         i++)
    {
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[i].control1Union.controlState = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[i].control2Union.controlState = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[i].dataUnion.data = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[i].stateUnion.state = 0U;
    }
    usbHostState->usbRegBase->ATL_PTD_BASE_ADDRESS =
        (((uint32_t)(&s_UsbHostIp3516HsPtd[usbHostState->controllerId].atl[0])) &
         USB_HOST_IP3516HS_ATL_PTD_ATL_BASE_MASK);
    usbHostState->usbRegBase->LAST_PTD_IN_USE = (i - 1U) & USB_HOST_IP3516HS_LAST_PTD_IN_USE_ATL_LAST_MASK;
    usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_ATL_EN_MASK;
    interruptState = USB_HOST_IP3516HS_USBINTR_ATL_IRQ_E_MASK;

#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
    for (i = 0U; i < USB_HOST_CONFIG_IP3516HS_MAX_ISO; i++)
    {
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[i].control1Union.controlState = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[i].control2Union.controlState = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[i].dataUnion.data = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[i].stateUnion.state = 0U;
    }
    usbHostState->usbRegBase->ISO_PTD_BASE_ADDRESS =
        (((uint32_t)(&s_UsbHostIp3516HsPtd[usbHostState->controllerId].iso[0])) &
         USB_HOST_IP3516HS_ISO_PTD_ISO_BASE_MASK);
    usbHostState->usbRegBase->LAST_PTD_IN_USE |=
        ((USB_HOST_CONFIG_IP3516HS_MAX_ISO - 1U) << USB_HOST_IP3516HS_LAST_PTD_IN_USE_ISO_LAST_SHIFT) &
        USB_HOST_IP3516HS_LAST_PTD_IN_USE_ISO_LAST_MASK;
    usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_ISO_EN_MASK;
    interruptState |= USB_HOST_IP3516HS_USBINTR_ISO_IRQ_E_MASK;
#endif

#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_INT) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
    for (i = 0U; i < USB_HOST_CONFIG_IP3516HS_MAX_INT; i++)
    {
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[i].control1Union.controlState = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[i].control2Union.controlState = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[i].dataUnion.data = 0U;
        s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[i].stateUnion.state = 0U;
    }
    usbHostState->usbRegBase->INT_PTD_BASE_ADDRESS =
        (((uint32_t)(&s_UsbHostIp3516HsPtd[usbHostState->controllerId].interrupt[0])) &
         USB_HOST_IP3516HS_INT_PTD_INT_BASE_MASK);
    usbHostState->usbRegBase->LAST_PTD_IN_USE |=
        ((USB_HOST_CONFIG_IP3516HS_MAX_INT - 1U) << USB_HOST_IP3516HS_LAST_PTD_IN_USE_INT_LAST_SHIFT) &
        USB_HOST_IP3516HS_LAST_PTD_IN_USE_INT_LAST_MASK;
    usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_INT_EN_MASK;
    interruptState |= USB_HOST_IP3516HS_USBINTR_INT_IRQ_E_MASK;
#endif

    usbHostState->pipeListInUsing = NULL;
    usbHostState->pipeList = &usbHostState->pipePool[0];
    pipe = usbHostState->pipeList;
    for (i = 1U; i < USB_HOST_CONFIG_IP3516HS_MAX_PIPE; i++)
    {
        pipe->pipeCommon.next = (usb_host_pipe_t *)&usbHostState->pipePool[i];
        pipe = (usb_host_ip3516hs_pipe_struct_t *)pipe->pipeCommon.next;
    }
    pipe->pipeCommon.next = NULL;

    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    if (usbHostState->usbRegBase->HCSPARAMS & USB_HOST_IP3516HS_HCSPARAMS_PPC_MASK) /* Ports have power port switches */
    {
        /* only has one port */
        uint32_t tmp = usbHostState->usbRegBase->PORTSC1;
        tmp &= (~USB_HOST_IP3516HS_PORTSC1_WIC);
        usbHostState->usbRegBase->PORTSC1 = (tmp | USB_HOST_IP3516HS_PORTSC1_PP_MASK); /* turn on port power */
    }
    usbHostState->usbRegBase->DATA_PAYLOAD_BASE_ADDRESS =
        (uint32_t)(&s_UsbHostIp3516HsBufferArray[0][0]) & USB_HOST_IP3516HS_DAT_BASE_MASK;

    usbHostState->usbRegBase->USBSTS = 0xFFFFFFFFU;

    usbHostState->usbRegBase->USBINTR =
        interruptState | USB_HOST_IP3516HS_USBINTR_PCDE_MASK | USB_HOST_IP3516HS_USBINTR_SOF_E_MASK;

    usbHostState->usbRegBase->USBCMD |= USB_HOST_IP3516HS_USBCMD_RS_MASK;

    return kStatus_USB_Success;
}

/*!
 * @brief create the USB host ip3516hs instance.
 *
 * This function initializes the USB host ip3516hs controller driver.
 *
 * @param controllerId        The controller id of the USB IP. Please refer to the enumeration usb_controller_index_t.
 * @param hostHandle         The host level handle.
 * @param controllerHandle  Return the controller instance handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_AllocFail             allocate memory fail.
 * @retval kStatus_USB_Error                 host mutex create fail, IP3516HS mutex or IP3516HS event create fail.
 *                                                         Or, IP3516HS IP initialize fail.
 */
usb_status_t USB_HostIp3516HsCreate(uint8_t controllerId,
                                    usb_host_handle hostHandle,
                                    usb_host_controller_handle *controllerHandle)
{
    usb_host_ip3516hs_state_struct_t *usbHostState;
    uint32_t usb_base_addrs[] = USBHSH_BASE_ADDRS;
    uint8_t usb_irq[] = USBHSH_IRQS;
    usb_status_t status = kStatus_USB_Success;

    if (((controllerId - kUSB_ControllerIp3516Hs0) >= (uint8_t)USB_HOST_CONFIG_IP3516HS) ||
        ((controllerId - kUSB_ControllerIp3516Hs0) >= (sizeof(usb_base_addrs) / sizeof(uint32_t))))
    {
        return kStatus_USB_ControllerNotFound;
    }
    usbHostState = &s_UsbHostIp3516HsState[controllerId - kUSB_ControllerIp3516Hs0];
    /* Allocate the USB Host Pipe Descriptors */
    usbHostState->hostHandle = hostHandle;
    usbHostState->controllerId = controllerId - kUSB_ControllerIp3516Hs0;

    usbHostState->usbRegBase = (usb_host_ip3516hs_register_struct_t *)usb_base_addrs[usbHostState->controllerId];
    usbHostState->isrNumber = (IRQn_Type)usb_irq[usbHostState->controllerId];

    USB_OsaEventCreate(&usbHostState->ip3516HsEvent, 1U);
    if (NULL == usbHostState->ip3516HsEvent)
    {
#ifdef HOST_ECHO
        usb_echo(" Event create failed in USB_HostIp3516HsCreate\n");
#endif
        return kStatus_USB_AllocFail;
    } /* Endif */

    if (kStatus_USB_OSA_Success != USB_OsaMutexCreate(&usbHostState->mutex))
    {
        USB_HostIp3516HsDestory(usbHostState);
#ifdef HOST_ECHO
        usb_echo("USB_HostIp3516HsCreate: create host mutex fail\r\n");
#endif
        return kStatus_USB_AllocFail;
    }

    usbHostState->portNumber = (usbHostState->usbRegBase->HCSPARAMS & USB_HOST_IP3516HS_HCSPARAMS_N_PORTS_MASK);
    if (!usbHostState->portNumber)
    {
#ifdef HOST_ECHO
        usb_echo(" Port number is invalid in USB_HostIp3516HsCreate\n");
#endif
        USB_HostIp3516HsDestory(usbHostState);
        return kStatus_USB_Error;
    }

    usbHostState->portState = (usb_host_ip3516hs_port_state_struct_t *)USB_OsaMemoryAllocate(
        sizeof(usb_host_ip3516hs_port_state_struct_t) * usbHostState->portNumber); /* malloc host ip3516hs port state */
    if (NULL == usbHostState->portState)
    {
#ifdef HOST_ECHO
        usb_echo(" Mem alloc failed in USB_HostIp3516HsCreate\n");
#endif
        USB_HostIp3516HsDestory(usbHostState);
        return kStatus_USB_Error;
    }

    USB_HostIp3516HsControllerReset(usbHostState);

    USB_HostIp3516HsControllerInit(usbHostState);

    *controllerHandle = (usb_host_handle)usbHostState;
    return status;
}

/*!
 * @brief destroy USB host ip3516hs instance.
 *
 * This function de-initialize the USB host ip3516hs controller driver.
 *
 * @param handle                                    the controller handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 */
usb_status_t USB_HostIp3516HsDestory(usb_host_controller_handle controllerHandle)
{
    usb_host_ip3516hs_state_struct_t *usbHostState = (usb_host_ip3516hs_state_struct_t *)controllerHandle;

    usbHostState->usbRegBase->USBINTR = 0U;
    usbHostState->usbRegBase->USBSTS = 0xFFFFFFFFU;

    usbHostState->usbRegBase->USBCMD &= ~USB_HOST_IP3516HS_USBCMD_RS_MASK;

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

    if (NULL != usbHostState->ip3516HsEvent)
    {
        USB_OsaEventDestroy(usbHostState->ip3516HsEvent);
        usbHostState->ip3516HsEvent = NULL;
    }

    return kStatus_USB_Success;
}

usb_status_t USB_HostIp3516HsOpenPipe(usb_host_controller_handle controllerHandle,
                                      usb_host_pipe_handle *pipeHandle,
                                      usb_host_pipe_init_t *pipeInit)
{
    usb_host_ip3516hs_pipe_struct_t *pipe = NULL;
    usb_host_ip3516hs_state_struct_t *usbHostState = (usb_host_ip3516hs_state_struct_t *)controllerHandle;
    usb_status_t status;

    status = USB_HostIp3516HsGetNewPipe(&usbHostState->pipeList, &pipe);
    if ((kStatus_USB_Success != status) || (NULL == pipe))
    {
#ifdef HOST_ECHO
        usb_echo("ip3516hs open pipe failed\r\n");
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
    pipe->pipeCommon.numberPerUframe = (pipeInit->numberPerUframe + 1U) & 0x03U;
    pipe->pipeCommon.nakCount = pipeInit->nakCount;
    pipe->pipeCommon.nextdata01 = 0U;
    pipe->pipeCommon.currentCount = 0U;
    pipe->pipeCommon.open = 1U;
    pipe->tdIndex = 0xFFU;
    pipe->cutOffTime = USB_HOST_IP3516HS_TRANSFER_TIMEOUT_GAP;
    pipe->startUFrame = 0U;
    pipe->csSlot = 0U;
    pipe->isBusy = 0U;
    pipe->bufferLength = 0U;
    pipe->currentTr = NULL;

    if (USB_ENDPOINT_ISOCHRONOUS == pipe->pipeCommon.pipeType)
    {
#if 0
        pipe->pipeCommon.interval =
            (1 << (pipe->pipeCommon.interval - 1)); /* iso interval is the power of 2 */
#else
        pipe->pipeCommon.interval = 1U;
#endif
    }
    else if (USB_ENDPOINT_INTERRUPT == pipe->pipeCommon.pipeType)
    {
        if (USB_SPEED_HIGH != ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed)
        {
            int interval = 0;
            /* FS/LS interrupt interval should be the power of 2, it is used for ip3516hs bandwidth */
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
        }
        else
        {
            pipe->pipeCommon.interval =
                (1 << (pipe->pipeCommon.interval - 1)); /* HS interrupt interval is the power of 2 */
        }
    }
    else
    {
        pipe->pipeCommon.interval = 0U;
    }

    if (USB_SPEED_HIGH != ((usb_host_device_instance_t *)pipe->pipeCommon.deviceHandle)->speed)
    {
        pipe->pipeCommon.interval = pipe->pipeCommon.interval << 3;
    }

    if (USB_HOST_IP3516HS_MAX_FRAME < pipe->pipeCommon.interval)
    {
        pipe->pipeCommon.interval = USB_HOST_IP3516HS_MAX_FRAME;
    }
    /* open pipe */
    switch (pipe->pipeCommon.pipeType)
    {
        case USB_ENDPOINT_CONTROL:
        case USB_ENDPOINT_BULK:
            status = USB_HostIp3516HsOpenControlBulkPipe(usbHostState, pipe);
            break;

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
        case USB_ENDPOINT_ISOCHRONOUS:
            status = USB_HostIp3516HsOpenIsoPipe(usbHostState, pipe);
            break;
#endif

#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_INT) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
        case USB_ENDPOINT_INTERRUPT:
            status = USB_HostIp3516HsOpenInterruptPipe(usbHostState, pipe);
            break;
#endif
        default:
            status = kStatus_USB_Error;
            break;
    }

    if (status != kStatus_USB_Success)
    {
        pipe->pipeCommon.open = 0U;
        /* release the pipe */
        USB_HostIp3516HsInsertPipe(&usbHostState->pipeList, pipe);
        return status;
    }

    USB_HostIp3516HsInsertPipe(&usbHostState->pipeListInUsing, pipe);

    *pipeHandle = pipe;
    return status;
}

usb_status_t USB_HostIp3516HsClosePipe(usb_host_controller_handle controllerHandle, usb_host_pipe_handle pipeHandle)
{
    usb_host_ip3516hs_state_struct_t *usbHostState = (usb_host_ip3516hs_state_struct_t *)controllerHandle;
    usb_host_ip3516hs_pipe_struct_t *pipe = (usb_host_ip3516hs_pipe_struct_t *)pipeHandle;

    switch (pipe->pipeCommon.pipeType)
    {
        case USB_ENDPOINT_BULK:
        case USB_ENDPOINT_CONTROL:
            USB_HostIp3516HsCloseControlBulkPipe(usbHostState, pipe);
            break;

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
        case USB_ENDPOINT_ISOCHRONOUS:
            USB_HostIp3516HsCloseIsoPipe(usbHostState, pipe);
            break;
#endif

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
        case USB_ENDPOINT_INTERRUPT:
            USB_HostIp3516HsCloseInterruptPipe(usbHostState, pipe);
            break;
#endif

        default:
            break;
    }
    USB_HostIp3516HsRemovePipe(&usbHostState->pipeListInUsing, pipe);
    pipe->pipeCommon.open = 0U;
    USB_HostIp3516HsInsertPipe(&usbHostState->pipeList, pipe);

    return kStatus_USB_Success;
}

usb_status_t USB_HostIp3516HsWritePipe(usb_host_controller_handle controllerHandle,
                                       usb_host_pipe_handle pipeHandle,
                                       usb_host_transfer_t *transfer)
{
    usb_host_ip3516hs_state_struct_t *usbHostState = (usb_host_ip3516hs_state_struct_t *)controllerHandle;
    usb_host_ip3516hs_pipe_struct_t *pipe = (usb_host_ip3516hs_pipe_struct_t *)pipeHandle;
    usb_host_transfer_t *trPre;
    usb_status_t status = kStatus_USB_Success;
    uint8_t trListEmpty = 0U;
    USB_OSA_SR_ALLOC();

    transfer->setupStatus = kStatus_UsbHostIp3516Hs_Idle;
    transfer->next = NULL;
    transfer->transferSofar = 0U;

    USB_OSA_ENTER_CRITICAL();
    if (NULL == pipe->trList)
    {
        pipe->trList = transfer;
        pipe->currentTr = transfer;
        trListEmpty = 1U;
    }
    else
    {
        trPre = pipe->trList;
        while (NULL != trPre->next)
        {
            trPre = trPre->next;
        }
        trPre->next = transfer;
    }
    USB_OSA_EXIT_CRITICAL();

    if (trListEmpty)
    {
        switch (pipe->pipeCommon.pipeType)
        {
            case USB_ENDPOINT_BULK:
                status = USB_HostIp3516HsWriteBulkPipe(usbHostState, pipe, transfer);
                break;

            case USB_ENDPOINT_CONTROL:
                status = USB_HostIp3516HsWriteControlPipe(usbHostState, pipe, transfer);
                break;

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
            case USB_ENDPOINT_ISOCHRONOUS:
                status = USB_HostIp3516HsWriteIsoPipe(usbHostState, pipe, transfer);
                break;
#endif

#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
            case USB_ENDPOINT_INTERRUPT:
                status = USB_HostIp3516HsWriteInterruptPipe(usbHostState, pipe, transfer);
                break;
#endif

            default:
                break;
        }
    }
    return status;
}

usb_status_t USB_HostIp3516HsReadPipe(usb_host_controller_handle controllerHandle,
                                      usb_host_pipe_handle pipeHandle,
                                      usb_host_transfer_t *transfer)
{
    return USB_HostIp3516HsWritePipe(controllerHandle, pipeHandle, transfer);
}

usb_status_t USB_HostIp3516HsIoctl(usb_host_controller_handle controllerHandle, uint32_t ioctlEvent, void *ioctlParam)
{
    usb_host_ip3516hs_state_struct_t *usbHostState = (usb_host_ip3516hs_state_struct_t *)controllerHandle;
    usb_host_cancel_param_t *param;
    usb_host_ip3516hs_pipe_struct_t *pipe;
    uint32_t deviceAddress;
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
#if ((defined(USB_HOST_CONFIG_LPM_L1)) && (USB_HOST_CONFIG_LPM_L1 > 0U))
    uint8_t *lpmParam;
#endif
#endif
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
            status = USB_HostIp3516HsCancelPipe(usbHostState, (usb_host_ip3516hs_pipe_struct_t *)param->pipeHandle,
                                                param->transfer);
            break;
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
#if ((defined(USB_HOST_CONFIG_LPM_L1)) && (USB_HOST_CONFIG_LPM_L1 > 0U))
        case kUSB_HostL1Config:
            lpmParam = (uint8_t *)ioctlParam;
            usbHostState->hirdValue = (*lpmParam) & 0xFU;
            usbHostState->L1remoteWakeupEnable = (((*lpmParam) & 0x80U) >> 7);
            break;
#endif
#endif
        case kUSB_HostBusControl:
            /* bus control */
            status = USB_HostIp3516HsControlBus(usbHostState, *((uint8_t *)ioctlParam));
            break;

        case kUSB_HostGetFrameNumber:
            /* get frame number */
            *((uint32_t *)ioctlParam) =
                (uint32_t)((usbHostState->usbRegBase->FLADJ_FRINDEX & USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK) >>
                           USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT);
            break;

        case kUSB_HostUpdateControlEndpointAddress:
            pipe = (usb_host_ip3516hs_pipe_struct_t *)ioctlParam;
            /* update address */
            USB_HostHelperGetPeripheralInformation(pipe->pipeCommon.deviceHandle, kUSB_HostGetDeviceAddress,
                                                   &deviceAddress);
            switch (pipe->pipeCommon.pipeType)
            {
                case USB_ENDPOINT_BULK:
                case USB_ENDPOINT_CONTROL:
                    usbHostState->usbRegBase->ATL_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .atl[pipe->tdIndex]
                        .control2Union.stateBitField.DeviceAddress = deviceAddress;
                    usbHostState->usbRegBase->ATL_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
                    break;
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
                case USB_ENDPOINT_INTERRUPT:
                    usbHostState->usbRegBase->INT_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .interrupt[pipe->tdIndex]
                        .control2Union.stateBitField.DeviceAddress = deviceAddress;
                    usbHostState->usbRegBase->INT_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
                    break;
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
                case USB_ENDPOINT_ISOCHRONOUS:
                    usbHostState->usbRegBase->ISO_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .iso[pipe->tdIndex]
                        .control2Union.stateBitField.DeviceAddress = deviceAddress;
                    usbHostState->usbRegBase->ISO_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
                    break;
#endif
                default:
                    break;
            }
            break;

        case kUSB_HostUpdateControlPacketSize:
            pipe = (usb_host_ip3516hs_pipe_struct_t *)ioctlParam;
            switch (pipe->pipeCommon.pipeType)
            {
                case USB_ENDPOINT_BULK:
                case USB_ENDPOINT_CONTROL:
                    usbHostState->usbRegBase->ATL_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .atl[pipe->tdIndex]
                        .control1Union.stateBitField.MaxPacketLength = pipe->pipeCommon.maxPacketSize;
                    usbHostState->usbRegBase->ATL_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
                    break;
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
                case USB_ENDPOINT_INTERRUPT:
                    usbHostState->usbRegBase->INT_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .interrupt[pipe->tdIndex]
                        .control1Union.stateBitField.MaxPacketLength = pipe->pipeCommon.maxPacketSize;
                    usbHostState->usbRegBase->INT_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
                    break;
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
                case USB_ENDPOINT_ISOCHRONOUS:
                    usbHostState->usbRegBase->ISO_PTD_SKIP_MAP |= 1U << pipe->tdIndex;
                    s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                        .iso[pipe->tdIndex]
                        .control1Union.stateBitField.MaxPacketLength = pipe->pipeCommon.maxPacketSize;
                    if (s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                            .iso[pipe->tdIndex]
                            .control2Union.stateBitField.S)
                    {
                        s_UsbHostIp3516HsPtd[usbHostState->controllerId]
                            .iso[pipe->tdIndex]
                            .control1Union.stateBitField.MaxPacketLength++;
                    }
                    usbHostState->usbRegBase->ISO_PTD_SKIP_MAP &= ~(1U << pipe->tdIndex);
                    break;
#endif
                default:
                    break;
            }
            break;

        case kUSB_HostPortAttachDisable:
            break;

        case kUSB_HostPortAttachEnable:
            if ((*(uint8_t *)ioctlParam) < usbHostState->portNumber)
            {
                usbHostState->portState[*(uint8_t *)ioctlParam].portStatus = kUSB_DeviceIp3516HsPortDetached;
            }
            break;

        default:
            break;
    }
    return status;
}

void USB_HostIp3516HsTaskFunction(void *hostHandle)
{
    usb_host_ip3516hs_state_struct_t *usbHostState;
    uint32_t bitSet;

    if (NULL == hostHandle)
    {
        return;
    }
    usbHostState = (usb_host_ip3516hs_state_struct_t *)((usb_host_instance_t *)hostHandle)->controllerHandle;

    /* wait all event */
    if (kStatus_USB_OSA_Success == USB_OsaEventWait(usbHostState->ip3516HsEvent, 0xFFU, 0, 0, &bitSet))
    {
        if (bitSet & USB_HOST_IP3516HS_EVENT_PORT_CHANGE)
        {
            USB_HostIp3516HsPortChange(usbHostState);
        }
        if (bitSet & USB_HOST_IP3516HS_EVENT_ISO_TOKEN_DONE)
        {
            USB_HostIp3516HsTokenDone(usbHostState);
        }
        if (bitSet & USB_HOST_IP3516HS_EVENT_INT_TOKEN_DONE)
        {
            USB_HostIp3516HsTokenDone(usbHostState);
        }
        if (bitSet & USB_HOST_IP3516HS_EVENT_ATL_TOKEN_DONE)
        {
            USB_HostIp3516HsTokenDone(usbHostState);
        }
        if (bitSet & USB_HOST_IP3516HS_EVENT_SOF)
        {
            USB_HostIp3516HsSof(usbHostState);
        }
        if (bitSet & USB_HOST_IP3516HS_EVENT_ATTACH)
        {
            for (int i = 0; i < usbHostState->portNumber; i++)
            {
                if (kUSB_DeviceIp3516HsPortPhyAttached == usbHostState->portState[i].portStatus)
                {
                    void *deviceHandle;
                    if (kStatus_USB_Success == USB_HostAttachDevice(usbHostState->hostHandle,
                                                                    usbHostState->portState[i].portSpeed, 0, i, 1,
                                                                    &deviceHandle))
                    {
                        usbHostState->portState[i].portStatus = kUSB_DeviceIp3516HsPortAttached;
                    }
                }
            }
        }
        if (bitSet & USB_HOST_IP3516HS_EVENT_DETACH)
        {
            for (int i = 0; i < usbHostState->portNumber; i++)
            {
                if (kUSB_DeviceIp3516HsPortPhyDetached == usbHostState->portState[i].portStatus)
                {
                    USB_HostDetachDevice(usbHostState->hostHandle, 0, i);
                }
            }
        }
    }
}

void USB_HostIp3516HsIsrFunction(void *hostHandle)
{
    usb_host_ip3516hs_state_struct_t *usbHostState;
    static uint32_t interruptStatus = 0U;
    static uint32_t sofCount = 0U;

    if (hostHandle == NULL)
    {
        return;
    }

    usbHostState = (usb_host_ip3516hs_state_struct_t *)((usb_host_instance_t *)hostHandle)->controllerHandle;

    interruptStatus = usbHostState->usbRegBase->USBSTS;
    interruptStatus &= usbHostState->usbRegBase->USBINTR;

    if (interruptStatus & USB_HOST_IP3516HS_USBSTS_ISO_IRQ_MASK) /* Write back done head */
    {
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO)) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
        USB_HostIp3516HsCheckIsoTransferSofar(usbHostState);
        USB_OsaEventSet(usbHostState->ip3516HsEvent, USB_HOST_IP3516HS_EVENT_ISO_TOKEN_DONE);
#endif
    }

    if (interruptStatus & USB_HOST_IP3516HS_USBSTS_INT_IRQ_MASK) /* Write back done head */
    {
#if ((defined(USB_HOST_CONFIG_IP3516HS_MAX_INT)) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
        USB_HostIp3516HsCheckIntTransferSofar(usbHostState);
        USB_OsaEventSet(usbHostState->ip3516HsEvent, USB_HOST_IP3516HS_EVENT_INT_TOKEN_DONE);
#endif
    }

    if (interruptStatus & USB_HOST_IP3516HS_USBSTS_ATL_IRQ_MASK) /* Write back done head */
    {
        USB_HostIp3516HsCheckAtlTransferSofar(usbHostState);
        USB_OsaEventSet(usbHostState->ip3516HsEvent, USB_HOST_IP3516HS_EVENT_ATL_TOKEN_DONE);
    }

    if (interruptStatus & USB_HOST_IP3516HS_USBSTS_SOF_IRQ_MASK) /* SOF interrupt */
    {
        sofCount++;
        if (sofCount >= USB_HOST_IP3516HS_TRANSFER_SCAN_INTERVAL)
        {
            sofCount = 0U;
            USB_OsaEventSet(usbHostState->ip3516HsEvent, USB_HOST_IP3516HS_EVENT_SOF);
        }
    }

    if (interruptStatus & USB_HOST_IP3516HS_USBINTR_PCDE_MASK) /* port change detect interrupt */
    {
        USB_OsaEventSet(usbHostState->ip3516HsEvent, USB_HOST_IP3516HS_EVENT_PORT_CHANGE);
    }

    usbHostState->usbRegBase->USBSTS = interruptStatus; /* clear interrupt */
}

#endif
