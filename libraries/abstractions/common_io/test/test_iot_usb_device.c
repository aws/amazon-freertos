/*
 * Amazon FreeRTOS Common IO V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*******************************************************************************
 * IOT Functional Unit Test - USB Device
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* USB device HAL driver include */
#include "iot_usb_device.h"

/* USB device class and common include */
#include "usb_types.h"
#include "usb_utils.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"

#include <stdio.h>
#include <time.h>

/* USB Config macro*/
#define testIotUsbDevice_CONFIG_NUM (1U)
#define testIotUsb_CONTROL_MAX_PACKET_SIZE (64U)
#define testIotUsbDevice_CONFIG_HID (1U)
#define testIotUsbDevice_CONFIG_SELF_POWER (1U)
#define testIotUsbDevice_CONFIG_ENDPOINTS (4U)
#define testIotUsbDevice_CONFIG_MAX_MESSAGES (8U)
#define testIotUsbDevice_CONFIG_DETACH_ENABLE (1U)
#define testIotUsbDevice_CONFIG_REMOTE_WAKEUP (0U)

/*-----------------------------------------------------------*/
/* usb ch9 */
#define testIotUsbDevice_STATUS_SIZE (0x02U)
#define testIotUsbDevice_INTERFACE_STATUS_SIZE (0x02U)
#define testIotUsbDevice_ENDPOINT_STATUS_SIZE (0x02U)
#define testIotUsbDevice_CONFIGURE_SIZE (0X01U)
#define testIotUsbDevice_INTERFACE_SIZE (0X01U)
#define testIotUsbDevice_GET_STATUS_DEVICE_MASK (0x03U)
#define testIotUsbDevice_GET_STATUS_INTERFACE_MASK (0x03U)
#define testIotUsbDevice_GET_STATUS_ENDPOINT_MASK (0x03U)
typedef enum
{
    eUSBDeviceControlPipeSetupStage = 0U,   /*!< Setup stage */
    eUSBDeviceControlPipeDataStage,         /*!< Data stage */
    eUSBDeviceControlPipeStatusStage,       /*!< status stage */
} USBDeviceControlReadWriteSequence_t;

/*! @brief USB device notification event for descriptor, configuration
    interface */
typedef enum
{
    eUSBDeviceSetConfigurationEvent = 9, /*!< Set configuration. */
    eUSBDeviceSetInterfaceEvent,     /*!< Set interface. */

    eUSBDeviceGetDeviceDescriptorEvent,          /*!< Event is triggered when get device descriptor. */
    eUSBDeviceGetConfigurationDescriptorEvent,   /*!< Event is triggered when get configuration descriptor. */
    eUSBDeviceGetStringDescriptorEvent,          /*!< Event is triggered when get string descriptor. */
    eUSBDeviceGetHidDescriptorEvent,             /*!< Event is triggered when get HID descriptor. */
    eUSBDeviceGetHidReportDescriptorEvent,       /*!< Event is triggered when get HID report descriptor. */
    eUSBDeviceGetHidPhysicalDescriptorEvent,     /*!< Event is triggered when get HID physical descriptor. */
    eUSBDeviceGetBOSDescriptorEvent,             /*!< Event is triggered when get BOS configuration descriptor. */
    eUSBDeviceGetDeviceQualifierDescriptorEvent, /*!< Event is triggered when get device qualifier descriptor. */
    eUSBDeviceVendorRequestEvent,                /*!< Event is triggered for Vendor request. */
    eUSBDeviceSetRemoteWakeupEvent,              /*!< Enable or disable remote wakeup function. */
    eUSBDeviceGetConfigurationEvent,             /*!< Get current configuration index */
    eUSBDeviceGetInterfaceEvent,                 /*!< Get current interface alternate setting value */
} USBDeviceCH9_t;
// usb class file
#define class_handle_t uint32_t

/*! @brief Available class types. */
typedef enum
{
    eUSBDeviceClassTypeHid = 1U,
    eUSBDeviceClassTypeCdc,
    eUSBDeviceClassTypeMsc,
    eUSBDeviceClassTypeAudio,
    eUSBDeviceClassTypePhdc,
    eUSBDeviceClassTypeVideo,
    eUSBDeviceClassTypePrinter,
    eUSBDeviceClassTypeDfu,
    eUSBDeviceClassTypeCcid,
} USBDeviceClassType_t;

/*! @brief Available common class events. */
typedef enum
{
    eUSBDeviceClassEventClassRequest = 1U,
    eUSBDeviceClassEventDeviceReset,
    eUSBDeviceClassEventSetConfiguration,
    eUSBDeviceClassEventSetInterface,
    eUSBDeviceClassEventSetEndpointHalt,
    eUSBDeviceClassEventClearEndpointHalt,
} USBDeviceClassEvent_t;

/*!
 * @brief Obtains the endpoint data structure.
 *
 * Define the endpoint data structure.
 *
 */
typedef struct
{
    uint8_t ucEndpointAddress; /*!< Endpoint address*/
    uint8_t ucTransferType;    /*!< Endpoint transfer type*/
    uint16_t usMaxPacketSize;  /*!< Endpoint maximum packet size */
    uint8_t ucInterval;        /*!< Endpoint interval*/
} USBDeviceEndpointStruct_t;

typedef struct
{
    uint8_t ucCount;                          /*!< How many endpoints in current interface*/
    USBDeviceEndpointStruct_t * pxEndpoint; /*!< Endpoint structure list*/
} USBDeviceEndpointList_t;

/*!
 * @brief Obtains the interface list data structure.
 *
 * Structure representing an interface.
 *
 */
typedef struct
{
    uint8_t ucAlternateSetting;                /*!< Alternate setting number*/
    USBDeviceEndpointList_t xEndpointList; /*!< Endpoints of the interface*/
    void * pvClassSpecific;                     /*!< Class specific structure handle*/
} USBDeviceInterfaceStruct_t;

/*!
 * @brief Obtains the interface data structure.
 *
 * Structure representing interface.
 *
 */
typedef struct
{
    uint8_t ucClassCode;                        /*!< Class code of the interface*/
    uint8_t ucSubclassCode;                     /*!< Subclass code of the interface*/
    uint8_t ucProtocolCode;                     /*!< Protocol code of the interface*/
    uint8_t ucInterfaceNumber;                  /*!< Interface number*/
    USBDeviceInterfaceStruct_t * pxInterface; /*!< Interface structure list*/
    uint8_t ucCount;                            /*!< Number of interfaces in the current interface*/
} USBDeviceInterfacesStruct_t;

/*!
 * @brief Obtains the interface group.
 *
 * Structure representing how many interfaces in one class type.
 *
 */
typedef struct
{
    uint8_t ucCount;                              /*!< Number of interfaces of the class*/
    USBDeviceInterfacesStruct_t * pxInterfaces; /*!< All interfaces*/
} USBDeviceInterfaceList_t;

typedef struct
{
    USBDeviceInterfaceList_t * pxInterfaceList; /*!< Interfaces of the class*/
    USBDeviceClassType_t xType;               /*!< Class type*/
    uint8_t ucConfigurations;                     /*!< Number of configurations of the class*/
} USBDeviceClassStruct_t;

typedef int32_t (*USBDeviceClassCallback_t)(class_handle_t xClassHandle,
                                                    uint32_t ulCallbackEvent,
                                                    void * pvEventParam);

/*!
 * @brief Obtains the device class information structure.
 *
 * Structure representing the device class information. This structure only can be stored in RAM space.
 *
 */
typedef struct
{
    USBDeviceClassCallback_t xClassCallback;  /*!< Class callback function to handle the device status-related event
                                                   for the specified type of class*/
    class_handle_t xClassHandle;                 /*!< The class handle of the class, filled by the common driver.*/
    USBDeviceClassStruct_t * pxClassInfomation; /*!< Detailed information of the class*/
} USBDeviceClassConfigStruct_t;

/*!
 * @brief Obtains the device class configuration structure.
 *
 * Structure representing the device class configuration information.
 *
 */
typedef struct
{
    USBDeviceClassConfigStruct_t * pxConfig; /*!< Array of class configuration structures */
    IotUsbDeviceCallback_t xDeviceCallback;     /*!< Device callback function */
    uint8_t ucCount;                            /*!< Number of class supported */
} USBDeviceClassConfigListStruct_t;

typedef struct
{
    USBSetupStruct_t * pxSetup; /*!< The pointer of the setup packet data. */
    uint8_t * pucBuffer;           /*!< Pass the buffer address. */
    uint32_t ulLength;           /*!< Pass the buffer length or requested length. */
    uint8_t ucIsSetup;           /*!< Indicates whether a setup packet is received. */
} USBDeviceControlRequestStruct_t;

/*! @brief Obtains the control get descriptor request common structure. */
typedef struct
{
    uint8_t * pucBuffer; /*!< Pass the buffer address. */
    uint32_t ulLength; /*!< Pass the buffer length. */
} USBDeviceGetDescriptorCommonStruct_t;

/*! @brief Obtains the control get device descriptor request structure. */
typedef struct
{
    uint8_t * pucBuffer; /*!< Pass the buffer address. */
    uint32_t ulLength; /*!< Pass the buffer length. */
} USBDeviceGetDeviceDescriptorStruct_t;

/*! @brief Obtains the control get device qualifier descriptor request structure. */
typedef struct
{
    uint8_t * pucBuffer; /*!< Pass the buffer address. */
    uint32_t ulLength; /*!< Pass the buffer length. */
} USBDeviceGetDeviceQualifierDescriptorStruct_t;

/*! @brief Obtains the control get configuration descriptor request structure. */
typedef struct
{
    uint8_t * pucBuffer;       /*!< Pass the buffer address. */
    uint32_t ulLength;       /*!< Pass the buffer length. */
    uint8_t ucConfiguration; /*!< The configuration number. */
} USBDeviceGetConfigurationDescriptorStruct_t;

/*! @brief Obtains the control get bos descriptor request structure. */
typedef struct
{
    uint8_t * pucBuffer; /*!< Pass the buffer address. */
    uint32_t ulLength; /*!< Pass the buffer length. */
} USBDeviceGetBosDescriptorStruct_t;

typedef struct
{
    uint8_t * pucBuffer;     /*!< Pass the buffer address. */
    uint32_t ulLength;     /*!< Pass the buffer length. */
    uint16_t usLanguageId; /*!< Language ID. */
    uint8_t ucStringIndex; /*!< String index. */
} USBDeviceGetStringDescriptorStruct_t;

/*! @brief Obtains the control get HID descriptor request structure. */
typedef struct
{
    uint8_t * pucBuffer;         /*!< Pass the buffer address. */
    uint32_t ulLength;         /*!< Pass the buffer length. */
    uint8_t ucInterfaceNumber; /*!< The interface number. */
} USBDeviceGetHidDescriptorStruct_t;

/*! @brief Obtains the control get HID report descriptor request structure. */
typedef struct
{
    uint8_t * pucBuffer;         /*!< Pass the buffer address. */
    uint32_t ulLength;         /*!< Pass the buffer length. */
    uint8_t ucInterfaceNumber; /*!< The interface number. */
} USBDeviceGetHidReportDescriptorStruct_t;

/*! @brief Obtains the control get HID physical descriptor request structure. */
typedef struct
{
    uint8_t * pucBuffer;         /*!< Pass the buffer address. */
    uint32_t ulLength;         /*!< Pass the buffer length. */
    uint8_t ucIndex;           /*!< Physical index */
    uint8_t ucInterfaceNumber; /*!< The interface number. */
} USBDeviceGetHidPhysicalDescriptorStruct_t;

/*! @brief Obtains the control get descriptor request common union. */
typedef union
{
    USBDeviceGetDescriptorCommonStruct_t xCommonDescriptor; /*!< Common structure. */
    USBDeviceGetDeviceDescriptorStruct_t xDeviceDescriptor; /*!< The structure to get device descriptor. */
    USBDeviceGetDeviceQualifierDescriptorStruct_t
        xDeviceQualifierDescriptor; /*!< The structure to get device qualifier descriptor. */
    USBDeviceGetConfigurationDescriptorStruct_t
        xConfigurationDescriptor;                                /*!< The structure to get configuration descriptor. */
    USBDeviceGetStringDescriptorStruct_t xStringDescriptor; /*!< The structure to get string descriptor. */
    USBDeviceGetHidDescriptorStruct_t xHidDescriptor;       /*!< The structure to get HID descriptor. */
    USBDeviceGetHidReportDescriptorStruct_t
        xHidReportDescriptor; /*!< The structure to get HID report descriptor. */
    USBDeviceGetHidPhysicalDescriptorStruct_t
        xHidPhysicalDescriptor; /*!< The structure to get HID physical descriptor. */
} USBDeviceGetDescriptorCommonUnion_t;

/*! @brief Define function type for class device instance initialization */
typedef int32_t (*usb_device_class_init_call_t)(uint8_t ucControllerId,
                                                     USBDeviceClassConfigStruct_t * pxClassConfig,
                                                     class_handle_t * pxClassHandle);
/*! @brief Define function type for class device instance deinitialization, internal */
typedef int32_t (*usb_device_class_deinit_call_t)(class_handle_t xHandle);
/*! @brief Define function type for class device instance Event change */
typedef int32_t (*usb_device_class_event_callback_t)(void * pvClassHandle, uint32_t ulEvent, void * pvParam);
/*callback function pointer structure for application to provide class parameters*/
typedef int32_t (*USBDeviceClassCallback_t)(class_handle_t xClassHandle,
                                                    uint32_t ulCallbackEvent,
                                                    void * pvEventParam);

static int32_t usb_device_hid_init(uint8_t ucControllerId,
                                      USBDeviceClassConfigStruct_t * pxConfig,
                                      class_handle_t * pxHandle);

static int32_t usb_device_hid_event(void * pvHandle, uint32_t ulEvent, void * pvParam);

static int32_t usb_device_hid_deinit(class_handle_t xHandle);

/*! @brief Define class driver interface structure. */
typedef struct
{
    usb_device_class_init_call_t xClassInit;     /*!< Class driver initialization- entry  of the class driver */
    usb_device_class_deinit_call_t xClassDeinit; /*!< Class driver de-initialization*/
    usb_device_class_event_callback_t xClassEventCallback; /*!< Class driver event callback*/
    USBDeviceClassType_t xType;                         /*!< Class type*/
} USBDeviceClassMap_t;

/*! @brief Structure holding common class state information */
typedef struct
{
    uint8_t *pucBuffer; /*!< Transferred buffer */
    uint32_t ulLength; /*!< Transferred data length */
    uint8_t ucSetupInProgress;      /*!< if in setup phase */
    IotUsbDeviceHandle_t xHandle;                          /*!< USB device handle*/
    volatile USBDeviceClassConfigListStruct_t * pxConfigList; /*!< USB device configure list*/
    uint8_t * pucSetupBuffer;                              /*!< Setup packet data buffer*/
    uint16_t usStandardTranscationBuffer;                /*!<
                                                        * This variable is used in:
                                                        *           get status request
                                                        *           get configuration request
                                                        *           get interface request
                                                        *           set interface request
                                                        *           get sync frame request
                                                        */
    volatile uint8_t ucControllerId;                              /*!< Controller ID*/
} USBDeviceCommonClassStruct_t;

/*! @brief Endpoint callback information structure */
typedef struct
{
    uint8_t *pucBuffer; /*!< Transferred buffer */
    uint32_t ulLength; /*!< Transferred data length */
    uint8_t ucSetupInProgress; /*!< Is in a setup phase */
} USBDeviceEndpointCallbackInformation_t;


static int32_t usb_device_class_init(uint8_t ucControllerId,
                                 USBDeviceClassConfigListStruct_t * pxConfigList,
                                 IotUsbDeviceHandle_t * pxHandle);

static int32_t usb_device_class_deinit(uint8_t ucControllerId);

static int32_t usb_device_class_event(IotUsbDeviceHandle_t xHandle, USBDeviceClassEvent_t xEvent, void * pvParam);

static int32_t usb_device_class_clallback(IotUsbDeviceHandle_t xHandle, uint32_t xEvent, void * pvParam);

typedef int32_t (*usb_standard_request_callback_t)(USBDeviceCommonClassStruct_t *classHandle,
                                                        USBSetupStruct_t *setup,
                                                        uint8_t **buffer,
                                                        uint32_t *length);

static int32_t USB_DeviceCh9GetStatus(USBDeviceCommonClassStruct_t *classHandle,
                                           USBSetupStruct_t *setup,
                                           uint8_t **buffer,
                                           uint32_t *length)
{
    int32_t error = IOT_USB_DEVICE_INVALID_REQUEST;
    uint8_t state;

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if ((eUsbDeviceStateAddress != state) && (eUsbDeviceStateConfigured != state))
    {
        return error;
    }

    if ((setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) == USB_REQUEST_TYPE_RECIPIENT_DEVICE)
    {
#if (defined(USB_DEVICE_CONFIG_OTG) && (USB_DEVICE_CONFIG_OTG))
        if (setup->wIndex == USB_REQUEST_STANDARD_GET_STATUS_OTG_STATUS_SELECTOR)
        {
            error =
                iot_usb_device_ioctl(classHandle->xHandle, eGetOtgStatus, &classHandle->usStandardTranscationBuffer);
            TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
            classHandle->usStandardTranscationBuffer = USB_SHORT_TO_LITTLE_ENDIAN(classHandle->usStandardTranscationBuffer);
            /* The device status length must be testIotUsbDevice_STATUS_SIZE. */
            *length = 1;
        }
        else /* Get the device status */
        {
#endif
            error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetStatus,
                                        &classHandle->usStandardTranscationBuffer);
            TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
            classHandle->usStandardTranscationBuffer =
                classHandle->usStandardTranscationBuffer & testIotUsbDevice_GET_STATUS_DEVICE_MASK;
            classHandle->usStandardTranscationBuffer = USB_SHORT_TO_LITTLE_ENDIAN(classHandle->usStandardTranscationBuffer);
            /* The device status length must be testIotUsbDevice_STATUS_SIZE. */
            *length = testIotUsbDevice_STATUS_SIZE;
#if (defined(USB_DEVICE_CONFIG_OTG) && (USB_DEVICE_CONFIG_OTG))
        }
#endif
    }
    else if ((setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) == USB_REQUEST_TYPE_RECIPIENT_INTERFACE)
    {
        /* Get the interface status */
        error = IOT_USB_DEVICE_SUCCESS;
        classHandle->usStandardTranscationBuffer = 0U;
        /* The interface status length must be testIotUsbDevice_INTERFACE_STATUS_SIZE. */
        *length = testIotUsbDevice_INTERFACE_STATUS_SIZE;
    }
    else if ((setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) == USB_REQUEST_TYPE_RECIPIENT_ENDPOINT)
    {
        /* Get the endpoint status */
        IotUsbDeviceEndpointInfo_t endpointInfo;
        endpointInfo.ucEndpointAddress = (uint8_t)setup->wIndex;
        endpointInfo.usEndpointStatus = eUsbDeviceEndpointStateIdle;
        error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetEndpointStatus, &endpointInfo);
        TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
        classHandle->usStandardTranscationBuffer = endpointInfo.usEndpointStatus & testIotUsbDevice_GET_STATUS_ENDPOINT_MASK;
        /* The endpoint status length must be testIotUsbDevice_ENDPOINT_STATUS_SIZE. */
        *length = testIotUsbDevice_ENDPOINT_STATUS_SIZE;
    }
    else
    {
    }
    *buffer = (uint8_t *)&classHandle->usStandardTranscationBuffer;

    return error;
}

static int32_t USB_DeviceCh9SetClearFeature(USBDeviceCommonClassStruct_t *classHandle,
                                                 USBSetupStruct_t *setup,
                                                 uint8_t **buffer,
                                                 uint32_t *length)
{
    int32_t error = IOT_USB_DEVICE_INVALID_REQUEST;
    uint8_t state;
    uint8_t isSet = 0U;

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if ((eUsbDeviceStateAddress != state) && (eUsbDeviceStateConfigured != state))
    {
        return error;
    }

    /* Identify the request is set or clear the feature. */
    if (eUSBStandardRequestSetFeature == setup->bRequest)
    {
        isSet = 1U;
    }

    if ((setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) == USB_REQUEST_TYPE_RECIPIENT_DEVICE)
    {
        /* Set or Clear the device feature. */
        if (eUSBStandardRequestFeatureSelectorDeviceRemoteWakeup == setup->wValue)
        {
#if ((defined(testIotUsbDevice_CONFIG_REMOTE_WAKEUP)) && (testIotUsbDevice_CONFIG_REMOTE_WAKEUP > 0U))
            error = iot_usb_device_ioctl(classHandle->xHandle, eRemoteWakeup, &isSet);
            TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
#endif
            /* Set or Clear the device remote wakeup feature. */
            error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceSetRemoteWakeupEvent, &isSet);
        }
#if    (defined(USB_DEVICE_CONFIG_USB20_TEST_MODE) && (USB_DEVICE_CONFIG_USB20_TEST_MODE > 0U))
        else if (eUSBStandardRequestFeatureSelectorDeviceTestMode == setup->wValue)
        {
            state = eUsbDeviceStateTestMode;
            error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceSetState, &state);
            TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
        }
#endif
#if (defined(USB_DEVICE_CONFIG_OTG) && (USB_DEVICE_CONFIG_OTG))
        else if (eUSBStandardRequestFeatureSelectorBHnpEnable == setup->wValue)
        {
            error = usb_device_class_clallback(classHandle->xHandle, kUSB_DeviceEventSetBHNPEnable, &isSet);
        }
#endif
        else
        {
        }
    }
    else if ((setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) == USB_REQUEST_TYPE_RECIPIENT_ENDPOINT)
    {
        /* Set or Clear the endpoint feature. */
        if (eUSBStandardRequestFeatureSelectorEndpointHalt == setup->wValue)
        {
            if (IOT_USB_CONTROL_ENDPOINT == (setup->wIndex & IOT_USB_ENDPOINT_NUMBER_MASK))
            {
                /* Set or Clear the control endpoint status(halt or not). */
                if (isSet)
                {
                    error = iot_usb_device_endpoint_stall(classHandle->xHandle, (uint8_t)setup->wIndex);
                    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
                }
                else
                {
                    error = iot_usb_device_endpoint_unstall(classHandle->xHandle, (uint8_t)setup->wIndex);
                    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
                }
            }

            /* Set or Clear the endpoint status feature. */
            if (isSet)
            {
                error = usb_device_class_event(classHandle->xHandle, eUSBDeviceClassEventSetEndpointHalt, &setup->wIndex);
            }
            else
            {
                error =
                    usb_device_class_event(classHandle->xHandle, eUSBDeviceClassEventClearEndpointHalt, &setup->wIndex);
            }
        }
        else
        {
        }
    }
    else
    {
    }

    return error;
}

static int32_t USB_DeviceCh9SetAddress(USBDeviceCommonClassStruct_t *classHandle,
                                            USBSetupStruct_t *setup,
                                            uint8_t **buffer,
                                            uint32_t *length)
{
    int32_t error = IOT_USB_DEVICE_INVALID_REQUEST;
    uint8_t state;

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if ((eUsbDeviceStateAddressing != state) && (eUsbDeviceStateAddress != state) &&
        (eUsbDeviceStateDefault != state) && (eUsbDeviceStateConfigured != state))
    {
        return error;
    }

    if (eUsbDeviceStateAddressing != state)
    {
        /* If the device address is not setting, pass the address and the device state will change to
         * eUSBDeviceStateAddressing internally. */
        state = setup->wValue & 0xFFU;
        error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceSetAddress, &state);
        TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
    }
    else
    {
        /* If the device address is setting, set device address and the address will be write into the controller
         * internally. */
        error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceSetAddress, NULL);
        TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
        /* And then change the device state to eUSBDeviceStateAddress. */
        if (IOT_USB_DEVICE_SUCCESS == error)
        {
            state = eUsbDeviceStateAddress;
            error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceSetState, &state);
            TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
        }
    }

    return error;
}

static int32_t USB_DeviceCh9GetDescriptor(USBDeviceCommonClassStruct_t *classHandle,
                                               USBSetupStruct_t *setup,
                                               uint8_t **buffer,
                                               uint32_t *length)
{
    USBDeviceGetDescriptorCommonUnion_t commonDescriptor;
    int32_t error = IOT_USB_DEVICE_INVALID_REQUEST;
    uint8_t state;
    uint8_t descriptorType = (uint8_t)((setup->wValue & 0xFF00U) >> 8U);
    uint8_t descriptorIndex = (uint8_t)((setup->wValue & 0x00FFU));

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if ((eUsbDeviceStateAddress != state) && (eUsbDeviceStateConfigured != state) &&
        (eUsbDeviceStateDefault != state))
    {
        return error;
    }
    commonDescriptor.xCommonDescriptor.ulLength = setup->wLength;
    if (eUSBDescriptorTypeDevice == descriptorType)
    {
        /* Get the device descriptor */
        error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetDeviceDescriptorEvent,
                                        &commonDescriptor.xDeviceDescriptor);
    }
    else if (eUSBDescriptorTypeConfigure == descriptorType)
    {
        /* Get the configuration descriptor */
        commonDescriptor.xConfigurationDescriptor.ucConfiguration = descriptorIndex;
        error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetConfigurationDescriptorEvent,
                                        &commonDescriptor.xConfigurationDescriptor);
    }
    else if (eUSBDescriptorTypeString == descriptorType)
    {
        /* Get the string descriptor */
        commonDescriptor.xStringDescriptor.ucStringIndex = descriptorIndex;
        commonDescriptor.xStringDescriptor.usLanguageId = setup->wIndex;
        error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetStringDescriptorEvent,
                                        &commonDescriptor.xStringDescriptor);
    }
#if (defined(testIotUsbDevice_CONFIG_HID) && (testIotUsbDevice_CONFIG_HID > 0U))
    else if (eUSBDescriptorTypeHid == descriptorType)
    {
        /* Get the hid descriptor */
        commonDescriptor.xHidDescriptor.ucInterfaceNumber = setup->wIndex;
        error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetHidDescriptorEvent,
                                        &commonDescriptor.xHidDescriptor);
    }
    else if (eUSBDescriptorTypeHidReport == descriptorType)
    {
        /* Get the hid report descriptor */
        commonDescriptor.xHidReportDescriptor.ucInterfaceNumber = setup->wIndex;
        error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetHidReportDescriptorEvent,
                                        &commonDescriptor.xHidReportDescriptor);
    }
    else if (eUSBDescriptorTypeHidPhysical == descriptorType)
    {
        /* Get the hid physical descriptor */
        commonDescriptor.xHidPhysicalDescriptor.ucIndex = descriptorIndex;
        commonDescriptor.xHidPhysicalDescriptor.ucInterfaceNumber = setup->wIndex;
        error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetHidPhysicalDescriptorEvent,
                                        &commonDescriptor.xHidPhysicalDescriptor);
    }
#endif
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
    else if (eUSBDescriptorTypeDeviceQualitier == descriptorType)
    {
        /* Get the device descriptor */
        error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetDeviceQualifierDescriptorEvent,
                                        &commonDescriptor.xDeviceDescriptor);
    }
#endif
#if (defined(USB_DEVICE_CONFIG_LPM_L1) && (USB_DEVICE_CONFIG_LPM_L1 > 0U))
    else if (eUSBDescriptorTypeBos == descriptorType)
    {
        /* Get the configuration descriptor */
        commonDescriptor.xConfigurationDescriptor.ucConfiguration = descriptorIndex;
        error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetBOSDescriptorEvent,
                                        &commonDescriptor.xConfigurationDescriptor);
    }
#endif
    else
    {
    }
    *buffer = commonDescriptor.xCommonDescriptor.pucBuffer;
    *length = commonDescriptor.xCommonDescriptor.ulLength;
    return error;
}

static int32_t USB_DeviceCh9GetConfiguration(USBDeviceCommonClassStruct_t *classHandle,
                                                  USBSetupStruct_t *setup,
                                                  uint8_t **buffer,
                                                  uint32_t *length)
{
    uint8_t state;
    int32_t error;

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if ((eUsbDeviceStateAddress != state) && ((eUsbDeviceStateConfigured != state)))
    {
        return IOT_USB_DEVICE_INVALID_REQUEST;
    }

    *length = testIotUsbDevice_CONFIGURE_SIZE;
    *buffer = (uint8_t *)&classHandle->usStandardTranscationBuffer;
    return usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetConfigurationEvent,
                                   &classHandle->usStandardTranscationBuffer);
}

static int32_t USB_DeviceCh9SetConfiguration(USBDeviceCommonClassStruct_t *classHandle,
                                                  USBSetupStruct_t *setup,
                                                  uint8_t **buffer,
                                                  uint32_t *length)
{
    uint8_t state;
    int32_t error;

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if ((eUsbDeviceStateAddress != state) && (eUsbDeviceStateConfigured != state))
    {
        return IOT_USB_DEVICE_INVALID_REQUEST;
    }

    /* The device state is changed to eUSBDeviceStateConfigured */
    state = eUsbDeviceStateConfigured;
    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceSetState, &state);
    TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
    if (!setup->wValue)
    {
        /* If the new configuration is zero, the device state is changed to eUSBDeviceStateAddress */
        state = eUsbDeviceStateAddress;
        error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceSetState, &state);
        TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
    }

    /* Notify the class layer the configuration is changed */
    usb_device_class_event(classHandle->xHandle, eUSBDeviceClassEventSetConfiguration, &setup->wValue);
    /* Notify the application the configuration is changed */
    return usb_device_class_clallback(classHandle->xHandle, eUSBDeviceSetConfigurationEvent, &setup->wValue);
}

static int32_t USB_DeviceCh9GetInterface(USBDeviceCommonClassStruct_t *classHandle,
                                              USBSetupStruct_t *setup,
                                              uint8_t **buffer,
                                              uint32_t *length)
{
    int32_t error = IOT_USB_DEVICE_INVALID_REQUEST;
    uint8_t state;

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if (state != eUsbDeviceStateConfigured)
    {
        return error;
    }
    *length = testIotUsbDevice_INTERFACE_SIZE;
    *buffer = (uint8_t *)&classHandle->usStandardTranscationBuffer;
    classHandle->usStandardTranscationBuffer = (uint16_t)(((uint32_t)setup->wIndex & 0xFFU) << 8U);
    /* The Bit[15~8] is used to save the interface index, and the alternate setting will be saved in Bit[7~0] by
     * application. */
    error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceGetInterfaceEvent,
                                    &classHandle->usStandardTranscationBuffer);
    classHandle->usStandardTranscationBuffer = USB_SHORT_TO_LITTLE_ENDIAN(classHandle->usStandardTranscationBuffer);
    return error;
}

static int32_t USB_DeviceCh9SetInterface(USBDeviceCommonClassStruct_t *classHandle,
                                              USBSetupStruct_t *setup,
                                              uint8_t **buffer,
                                              uint32_t *length)
{
    int32_t error = IOT_USB_DEVICE_INVALID_REQUEST;
    uint8_t state;

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if (state != eUsbDeviceStateConfigured)
    {
        return IOT_USB_DEVICE_INVALID_REQUEST;
    }
    classHandle->usStandardTranscationBuffer = ((setup->wIndex & 0xFFU) << 8U) | (setup->wValue & 0xFFU);
    /* Notify the class driver the alternate setting of the interface is changed. */
    /* The Bit[15~8] is used to save the interface index, and the alternate setting is saved in Bit[7~0]. */
    usb_device_class_event(classHandle->xHandle, eUSBDeviceClassEventSetInterface,
                         &classHandle->usStandardTranscationBuffer);
    /* Notify the application the alternate setting of the interface is changed. */
    /* The Bit[15~8] is used to save the interface index, and the alternate setting will is saved in Bit[7~0]. */
    return usb_device_class_clallback(classHandle->xHandle, eUSBDeviceSetInterfaceEvent,
                                   &classHandle->usStandardTranscationBuffer);
}

static int32_t USB_DeviceCh9SynchFrame(USBDeviceCommonClassStruct_t *classHandle,
                                            USBSetupStruct_t *setup,
                                            uint8_t **buffer,
                                            uint32_t *length)
{
    int32_t error = IOT_USB_DEVICE_INVALID_REQUEST;
    uint8_t state;

    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if (state != eUsbDeviceStateConfigured)
    {
        return error;
    }

    classHandle->usStandardTranscationBuffer = USB_SHORT_FROM_LITTLE_ENDIAN(setup->wIndex);
    /* Get the sync frame value */
    error =
        iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetSyncFrame, &classHandle->usStandardTranscationBuffer);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    *buffer = (uint8_t *)&classHandle->usStandardTranscationBuffer;
    *length = sizeof(classHandle->usStandardTranscationBuffer);

    return error;
}

static int32_t USB_DeviceControlCallbackFeedback(IotUsbDeviceHandle_t handle,
                                                      USBSetupStruct_t *setup,
                                                      int32_t error,
                                                      USBDeviceControlReadWriteSequence_t stage,
                                                      uint8_t **buffer,
                                                      uint32_t *length)
{
    int32_t errorCode = IOT_USB_DEVICE_ERROR;
    uint8_t direction = eUSBTransferDirectionIn;

    if (IOT_USB_DEVICE_INVALID_REQUEST == error)
    {
        /* Stall the control pipe when the request is unsupported. */
        if ((!((setup->bmRequestType & USB_REQUEST_TYPE_TYPE_MASK) == USB_REQUEST_TYPE_TYPE_STANDARD)) &&
            ((setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT) && (setup->wLength) &&
            (eUSBDeviceControlPipeSetupStage == stage))
        {
            direction = eUSBTransferDirectionOut;
        }
        errorCode = iot_usb_device_endpoint_stall(
            handle,
            (IOT_USB_CONTROL_ENDPOINT) | (uint8_t)((uint32_t)direction << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT));
        TEST_ASSERT_EQUAL( errorCode, IOT_USB_DEVICE_SUCCESS);
    }
    else
    {
        if (*length > setup->wLength)
        {
            *length = setup->wLength;
        }
        errorCode = iot_usb_device_write_async(handle, (IOT_USB_CONTROL_ENDPOINT), *buffer, *length);
        TEST_ASSERT_EQUAL( errorCode, IOT_USB_DEVICE_SUCCESS);
        if ((IOT_USB_DEVICE_SUCCESS == errorCode) &&
            (USB_REQUEST_TYPE_DIR_IN == (setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK)))
        {
            errorCode = iot_usb_device_read_async(handle, (IOT_USB_CONTROL_ENDPOINT), (uint8_t *)NULL, 0U);
            TEST_ASSERT_EQUAL( errorCode, IOT_USB_DEVICE_SUCCESS);
        }
    }
    return errorCode;
}
/* The function list to handle the standard request. */
static const usb_standard_request_callback_t s_UsbDeviceStandardRequest[] = {
    USB_DeviceCh9GetStatus,
    USB_DeviceCh9SetClearFeature,
    (usb_standard_request_callback_t)NULL,
    USB_DeviceCh9SetClearFeature,
    (usb_standard_request_callback_t)NULL,
    USB_DeviceCh9SetAddress,
    USB_DeviceCh9GetDescriptor,
    (usb_standard_request_callback_t)NULL,
    USB_DeviceCh9GetConfiguration,
    USB_DeviceCh9SetConfiguration,
    USB_DeviceCh9GetInterface,
    USB_DeviceCh9SetInterface,
    USB_DeviceCh9SynchFrame,
};

static int32_t USB_DeviceControlCallback(IotUsbDeviceOperationStatus_t xStatus,
                                       void *callbackParam)
{
    USBSetupStruct_t *deviceSetup;
    USBDeviceCommonClassStruct_t *classHandle;
    USBDeviceEndpointCallbackInformation_t message;
    uint8_t *buffer = (uint8_t *)NULL;
    uint32_t length = 0U;
    int32_t error = IOT_USB_DEVICE_INVALID_REQUEST;
    uint8_t state;

    if (NULL == callbackParam)
    {
        return error;
    }

    classHandle = (USBDeviceCommonClassStruct_t *)callbackParam;
    deviceSetup = (USBSetupStruct_t *)&classHandle->pucSetupBuffer[0];
    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    message.pucBuffer = classHandle->pucBuffer;
    message.ulLength = classHandle->ulLength;
    message.ucSetupInProgress = classHandle->ucSetupInProgress;
    if (0xFFFFFFFFU == message.ulLength)
    {
        return error;
    }

    if (message.ucSetupInProgress)
    {
        if ((IOT_USB_SETUP_PACKET_SIZE != message.ulLength) || (NULL == message.pucBuffer))
        {
            /* If a invalid setup is received, the control pipes should be de-init and init again.
             * Due to the IP can not meet this require, it is reserved for feature.
             */
            /*
            iot_usb_device_endpoint_close(classHandle->xHandle,
                         IOT_USB_CONTROL_ENDPOINT | (eUSBTransferDirectionIn << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT));
            iot_usb_device_endpoint_close(classHandle->xHandle,
                         IOT_USB_CONTROL_ENDPOINT | (eUSBTransferDirectionOut << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT));
            usb_device_control_pipe_init(classHandle->xHandle, callbackParam);
            */
            return error;
        }
        /* Receive a setup request */
        USBSetupStruct_t *setup = (USBSetupStruct_t *)(message.pucBuffer);

        /* Copy the setup packet to the application buffer */
        deviceSetup->wValue = USB_SHORT_FROM_LITTLE_ENDIAN(setup->wValue);
        deviceSetup->wIndex = USB_SHORT_FROM_LITTLE_ENDIAN(setup->wIndex);
        deviceSetup->wLength = USB_SHORT_FROM_LITTLE_ENDIAN(setup->wLength);
        deviceSetup->bRequest = setup->bRequest;
        deviceSetup->bmRequestType = setup->bmRequestType;

        if ((deviceSetup->bmRequestType & USB_REQUEST_TYPE_TYPE_MASK) == USB_REQUEST_TYPE_TYPE_STANDARD)
        {
            /* Handle the standard request, only handle the request in request array. */
            if(deviceSetup->bRequest < (sizeof(s_UsbDeviceStandardRequest)/4))
            {
                if (s_UsbDeviceStandardRequest[deviceSetup->bRequest] != (usb_standard_request_callback_t)NULL)
                {
                    error = s_UsbDeviceStandardRequest[deviceSetup->bRequest](classHandle, deviceSetup, &buffer, &length);
                }
            }
        }
        else
        {
            if ((deviceSetup->wLength) &&
                ((deviceSetup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT))
            {
                /* Class or vendor request with the OUT data phase. */
                if ((deviceSetup->wLength) &&
                    ((deviceSetup->bmRequestType & USB_REQUEST_TYPE_TYPE_CLASS) == USB_REQUEST_TYPE_TYPE_CLASS))
                {
                    /* Get data buffer to receive the data from the host. */
                    USBDeviceControlRequestStruct_t controlRequest;
                    controlRequest.pucBuffer = (uint8_t *)NULL;
                    controlRequest.ucIsSetup = 1U;
                    controlRequest.pxSetup = deviceSetup;
                    controlRequest.ulLength = deviceSetup->wLength;
                    error = usb_device_class_event(classHandle->xHandle, eUSBDeviceClassEventClassRequest, &controlRequest);
                    length = controlRequest.ulLength;
                    buffer = controlRequest.pucBuffer;
                }
                else if ((deviceSetup->wLength) &&
                         ((deviceSetup->bmRequestType & USB_REQUEST_TYPE_TYPE_VENDOR) == USB_REQUEST_TYPE_TYPE_VENDOR))
                {
                    /* Get data buffer to receive the data from the host. */
                    USBDeviceControlRequestStruct_t controlRequest;
                    controlRequest.pucBuffer = (uint8_t *)NULL;
                    controlRequest.ucIsSetup = 1U;
                    controlRequest.pxSetup = deviceSetup;
                    controlRequest.ulLength = deviceSetup->wLength;
                    error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceVendorRequestEvent, &controlRequest);
                    length = controlRequest.ulLength;
                    buffer = controlRequest.pucBuffer;
                }
                else
                {
                }
                if (IOT_USB_DEVICE_SUCCESS == error)
                {
                    /* Prime an OUT transfer */
                    error = iot_usb_device_read_async(classHandle->xHandle, IOT_USB_CONTROL_ENDPOINT, buffer, deviceSetup->wLength);
                    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
                    return error;
                }
            }
            else
            {
                /* Class or vendor request with the IN data phase. */
                if (((deviceSetup->bmRequestType & USB_REQUEST_TYPE_TYPE_CLASS) == USB_REQUEST_TYPE_TYPE_CLASS))
                {
                    /* Get data buffer to response the host. */
                    USBDeviceControlRequestStruct_t controlRequest;
                    controlRequest.pucBuffer = (uint8_t *)NULL;
                    controlRequest.ucIsSetup = 1U;
                    controlRequest.pxSetup = deviceSetup;
                    controlRequest.ulLength = deviceSetup->wLength;
                    error = usb_device_class_event(classHandle->xHandle, eUSBDeviceClassEventClassRequest, &controlRequest);
                    length = controlRequest.ulLength;
                    buffer = controlRequest.pucBuffer;
                }
                else if (((deviceSetup->bmRequestType & USB_REQUEST_TYPE_TYPE_VENDOR) == USB_REQUEST_TYPE_TYPE_VENDOR))
                {
                    /* Get data buffer to response the host. */
                    USBDeviceControlRequestStruct_t controlRequest;
                    controlRequest.pucBuffer = (uint8_t *)NULL;
                    controlRequest.ucIsSetup = 1U;
                    controlRequest.pxSetup = deviceSetup;
                    controlRequest.ulLength = deviceSetup->wLength;
                    error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceVendorRequestEvent, &controlRequest);
                    length = controlRequest.ulLength;
                    buffer = controlRequest.pucBuffer;
                }
                else
                {
                }
            }
        }
        /* Send the response to the host. */
        error = USB_DeviceControlCallbackFeedback(classHandle->xHandle, deviceSetup, error, eUSBDeviceControlPipeSetupStage, &buffer,
                                                  &length);
    }
    else if (eUsbDeviceStateAddressing == state)
    {
        /* Set the device address to controller. */
        error = s_UsbDeviceStandardRequest[deviceSetup->bRequest](classHandle, deviceSetup, &buffer, &length);
    }
#if    (defined(USB_DEVICE_CONFIG_USB20_TEST_MODE) && (USB_DEVICE_CONFIG_USB20_TEST_MODE > 0U))
    else if (eUsbDeviceStateTestMode == state)
    {
        uint8_t portTestControl = (uint8_t)(deviceSetup->wIndex >> 8);
        /* Set the controller.into test mode. */
        error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceSetTestMode, &portTestControl);
        TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    }
#endif
    else if ((message.ulLength) && (deviceSetup->wLength) &&
             ((deviceSetup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT))
    {
        if (((deviceSetup->bmRequestType & USB_REQUEST_TYPE_TYPE_CLASS) == USB_REQUEST_TYPE_TYPE_CLASS))
        {
            /* Data received in OUT phase, and notify the class driver. */
            USBDeviceControlRequestStruct_t controlRequest;
            controlRequest.pucBuffer = message.pucBuffer;
            controlRequest.ucIsSetup = 0U;
            controlRequest.pxSetup = deviceSetup;
            controlRequest.ulLength = message.ulLength;
            error = usb_device_class_event(classHandle->xHandle, eUSBDeviceClassEventClassRequest, &controlRequest);
        }
        else if (((deviceSetup->bmRequestType & USB_REQUEST_TYPE_TYPE_VENDOR) == USB_REQUEST_TYPE_TYPE_VENDOR))
        {
            /* Data received in OUT phase, and notify the application. */
            USBDeviceControlRequestStruct_t controlRequest;
            controlRequest.pucBuffer = message.pucBuffer;

            controlRequest.ucIsSetup = 0U;
            controlRequest.pxSetup = deviceSetup;
            controlRequest.ulLength = message.ulLength;
            error = usb_device_class_clallback(classHandle->xHandle, eUSBDeviceVendorRequestEvent, &controlRequest);
        }
        else
        {
        }
        /* Send the response to the host. */
        error = USB_DeviceControlCallbackFeedback(classHandle->xHandle, deviceSetup, error, eUSBDeviceControlPipeDataStage, &buffer,
                                                  &length);
    }
    else
    {
    }
    return error;
}

static int32_t usb_device_control_pipe_init(IotUsbDeviceHandle_t handle, void *param)
{
    IotUsbDeviceEndpointConfig_t epInitStruct;
    IotUsbDeviceEndpointCallbackFn_t epCallback;
    int32_t error;

    epCallback = USB_DeviceControlCallback;

    epInitStruct.ucZlt = 1U;
    epInitStruct.ucTransferType = eUSBEndpointControl;
    epInitStruct.ucInterval = 0;
    epInitStruct.ucEndpointAddress = IOT_USB_CONTROL_ENDPOINT | (eUSBTransferDirectionIn << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT);
    epInitStruct.usMaxPacketSize = testIotUsb_CONTROL_MAX_PACKET_SIZE;
    /* Initialize the control IN pipe */
    error = iot_usb_device_endpoint_open(handle, &epInitStruct);
    TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
    iot_usb_device_set_endpoint_callback(handle, epInitStruct.ucEndpointAddress, epCallback, param);

    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        return error;
    }
    epInitStruct.ucEndpointAddress = IOT_USB_CONTROL_ENDPOINT | (eUSBTransferDirectionOut << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT);
    /* Initialize the control OUT pipe */
    error = iot_usb_device_endpoint_open(handle, &epInitStruct);
    TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
    iot_usb_device_set_endpoint_callback(handle, epInitStruct.ucEndpointAddress, epCallback, param);

    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        int32_t error_close;
        error_close = iot_usb_device_endpoint_close(handle,
                                 IOT_USB_CONTROL_ENDPOINT | (eUSBTransferDirectionIn << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT));
        TEST_ASSERT_EQUAL(error_close, IOT_USB_DEVICE_SUCCESS);
        return error;
    }

    return IOT_USB_DEVICE_SUCCESS;
}

/*-----------------------------------------------------------*/

/* The device class driver list. */
static const USBDeviceClassMap_t s_UsbDeviceClassInterfaceMap[] = {
#if ((defined(testIotUsbDevice_CONFIG_HID)) && (testIotUsbDevice_CONFIG_HID > 0U))
    {usb_device_hid_init, usb_device_hid_deinit, usb_device_hid_event, eUSBDeviceClassTypeHid},
#endif

    {(usb_device_class_init_call_t)NULL, (usb_device_class_deinit_call_t)NULL, (usb_device_class_event_callback_t)NULL,
     (USBDeviceClassType_t)0},
};

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static USBDeviceCommonClassStruct_t
    s_UsbDeviceCommonClassStruct[testIotUsbDevice_CONFIG_NUM];
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static uint8_t
    s_UsbDeviceSetupBuffer[testIotUsbDevice_CONFIG_NUM][USB_DATA_ALIGN_SIZE_MULTIPLE(IOT_USB_SETUP_PACKET_SIZE)];


static int32_t USB_DeviceClassAllocateHandle(uint8_t controllerId, USBDeviceCommonClassStruct_t **handle)
{
    uint32_t count;

    USB_EnterCritical();
    /* Check the controller is initialized or not. */
    for (count = 0U; count < testIotUsbDevice_CONFIG_NUM; count++)
    {
        if ((NULL != s_UsbDeviceCommonClassStruct[count].xHandle) &&
            (controllerId == s_UsbDeviceCommonClassStruct[count].ucControllerId))
        {
            USB_ExitCritical();
            return IOT_USB_DEVICE_ERROR;
        }
    }
    /* Get a free common class handle. */
    for (count = 0U; count < testIotUsbDevice_CONFIG_NUM; count++)
    {
        if (NULL == s_UsbDeviceCommonClassStruct[count].xHandle)
        {
            s_UsbDeviceCommonClassStruct[count].ucControllerId = controllerId;
            s_UsbDeviceCommonClassStruct[count].pucSetupBuffer = s_UsbDeviceSetupBuffer[count];
            *handle = &s_UsbDeviceCommonClassStruct[count];
            USB_ExitCritical();
            return IOT_USB_DEVICE_SUCCESS;
        }
    }

    USB_ExitCritical();
    return IOT_USB_DEVICE_BUSY;
}

static int32_t USB_DeviceClassFreeHandle(uint8_t controllerId)
{
    uint32_t count = 0U;

    USB_EnterCritical();
    for (; count < testIotUsbDevice_CONFIG_NUM; count++)
    {
        if ((NULL != s_UsbDeviceCommonClassStruct[count].xHandle) &&
            (controllerId == s_UsbDeviceCommonClassStruct[count].ucControllerId))
        {
            s_UsbDeviceCommonClassStruct[count].xHandle = NULL;
            s_UsbDeviceCommonClassStruct[count].pxConfigList = (USBDeviceClassConfigListStruct_t *)NULL;
            s_UsbDeviceCommonClassStruct[count].ucControllerId = 0U;
            USB_ExitCritical();
            return IOT_USB_DEVICE_SUCCESS;
        }
    }
    USB_ExitCritical();

    return IOT_USB_DEVICE_INVALID_VALUE;
}

static int32_t USB_DeviceClassGetHandleByControllerId(uint8_t controllerId,
                                                           USBDeviceCommonClassStruct_t **handle)
{
    uint32_t count = 0U;

    USB_EnterCritical();
    for (; count < testIotUsbDevice_CONFIG_NUM; count++)
    {
        if ((NULL != s_UsbDeviceCommonClassStruct[count].xHandle) &&
            (controllerId == s_UsbDeviceCommonClassStruct[count].ucControllerId))
        {
            *handle = &s_UsbDeviceCommonClassStruct[count];
            USB_ExitCritical();
            return IOT_USB_DEVICE_SUCCESS;
        }
    }
    USB_ExitCritical();
    return IOT_USB_DEVICE_INVALID_VALUE;
}

static int32_t USB_DeviceClassGetHandleByDeviceHandle(IotUsbDeviceHandle_t deviceHandle,
                                                           USBDeviceCommonClassStruct_t **handle)
{
    uint32_t count = 0U;

    USB_EnterCritical();
    for (; count < testIotUsbDevice_CONFIG_NUM; count++)
    {
        if (deviceHandle == s_UsbDeviceCommonClassStruct[count].xHandle)
        {
            *handle = &s_UsbDeviceCommonClassStruct[count];
            USB_ExitCritical();
            return IOT_USB_DEVICE_SUCCESS;
        }
    }
    USB_ExitCritical();
    return IOT_USB_DEVICE_INVALID_VALUE;
}

static int32_t usb_device_class_get_device_handle(uint8_t controllerId, IotUsbDeviceHandle_t *handle)
{
    uint32_t count = 0U;

    USB_EnterCritical();
    for (; count < testIotUsbDevice_CONFIG_NUM; count++)
    {
        if ((NULL != s_UsbDeviceCommonClassStruct[count].xHandle) &&
            (controllerId == s_UsbDeviceCommonClassStruct[count].ucControllerId))
        {
            *handle = s_UsbDeviceCommonClassStruct[count].xHandle;
            USB_ExitCritical();
            return IOT_USB_DEVICE_SUCCESS;
        }
    }
    USB_ExitCritical();
    return IOT_USB_DEVICE_INVALID_VALUE;
}

static int32_t usb_device_class_event(IotUsbDeviceHandle_t handle, USBDeviceClassEvent_t event, void *param)
{
    USBDeviceCommonClassStruct_t *classHandle;
    uint8_t mapIndex;
    uint8_t classIndex;
    int32_t errorReturn = IOT_USB_DEVICE_ERROR;
    int32_t error = IOT_USB_DEVICE_ERROR;

    if (NULL == param)
    {
        return IOT_USB_DEVICE_INVALID_VALUE;
    }

    /* Get the common class handle according to the device handle. */
    errorReturn = USB_DeviceClassGetHandleByDeviceHandle(handle, &classHandle);
    if (IOT_USB_DEVICE_SUCCESS != errorReturn)
    {
        return IOT_USB_DEVICE_INVALID_VALUE;
    }

    for (classIndex = 0U; classIndex < classHandle->pxConfigList->ucCount; classIndex++)
    {
        for (mapIndex = 0U; mapIndex < (sizeof(s_UsbDeviceClassInterfaceMap) / sizeof(USBDeviceClassMap_t));
             mapIndex++)
        {
            if (s_UsbDeviceClassInterfaceMap[mapIndex].xType ==
                classHandle->pxConfigList->pxConfig[classIndex].pxClassInfomation->xType)
            {
                /* Call class event callback of supported class */
                errorReturn = s_UsbDeviceClassInterfaceMap[mapIndex].xClassEventCallback(
                    (void *)classHandle->pxConfigList->pxConfig[classIndex].xClassHandle, event, param);
                /* Return the error code IOT_USB_DEVICE_INVALID_REQUEST immediately, when a class returns
                 * IOT_USB_DEVICE_INVALID_REQUEST. */
                if (IOT_USB_DEVICE_INVALID_REQUEST == errorReturn)
                {
                    return IOT_USB_DEVICE_INVALID_REQUEST;
                }
                /* For composite device, it should return IOT_USB_DEVICE_SUCCESS once a valid request has been handled */
                if (IOT_USB_DEVICE_SUCCESS == errorReturn)
                {
                    error = IOT_USB_DEVICE_SUCCESS;
                }
                break;
            }
        }
    }

    return error;
}

static int32_t usb_device_class_clallback(IotUsbDeviceHandle_t handle, uint32_t event, void *param)
{
    USBDeviceCommonClassStruct_t *classHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;

    /* Get the common class handle according to the device handle. */
    error = USB_DeviceClassGetHandleByDeviceHandle(handle, &classHandle);
    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        return error;
    }

    if (eUSBDeviceBusResetEvent == event)
    {
        /* Initialize the control pipes */
        usb_device_control_pipe_init(handle, classHandle);

        /* Notify the classes the USB bus reset signal detected. */
        usb_device_class_event(handle, eUSBDeviceClassEventDeviceReset, classHandle);
    }

    /* Call the application device callback function. deviceCallback is from the second parameter of
       usb_device_class_init */
    error = classHandle->pxConfigList->xDeviceCallback(handle, event, param);
    return error;
}

static int32_t usb_device_class_init(
    uint8_t controllerId,                              /*!< [IN] Controller ID */
    USBDeviceClassConfigListStruct_t *configList, /*!< [IN] Pointer to class configuration list */
    IotUsbDeviceHandle_t *handle                          /*!< [OUT] Pointer to the device handle */
    )
{
    USBDeviceCommonClassStruct_t *classHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;
    uint8_t mapIndex;
    uint8_t classIndex;

    if ((NULL == handle) || (NULL == configList) || ((IotUsbDeviceCallback_t)NULL == configList->xDeviceCallback))
    {
        return IOT_USB_DEVICE_INVALID_VALUE;
    }

    /* Allocate a common class driver handle. */
    error = USB_DeviceClassAllocateHandle(controllerId, &classHandle);
    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        return error;
    }

    /* Initialize the device stack. */
    classHandle->xHandle = iot_usb_device_open(controllerId);
    TEST_ASSERT_NOT_EQUAL( classHandle->xHandle, NULL);
    if (classHandle->xHandle == NULL)
    {
        int32_t error_close;
        error_close = iot_usb_device_close(classHandle->xHandle);
        TEST_ASSERT_EQUAL(error_close, IOT_USB_DEVICE_SUCCESS);
        USB_DeviceClassFreeHandle(controllerId);
        return error;
    }

    /* Save the configuration list */
    classHandle->pxConfigList = configList;
    classHandle->ucControllerId = controllerId;
    classHandle->pucSetupBuffer = s_UsbDeviceSetupBuffer[0];

    /* setup device callback */
    iot_usb_device_set_device_callback(classHandle->xHandle, usb_device_class_clallback, NULL);

    /* Initialize the all supported classes according to the configuration list. */
    for (classIndex = 0U; classIndex < classHandle->pxConfigList->ucCount; classIndex++)
    {
        for (mapIndex = 0U; mapIndex < (sizeof(s_UsbDeviceClassInterfaceMap) / sizeof(USBDeviceClassMap_t));
             mapIndex++)
        {
            if (classHandle->pxConfigList->pxConfig[classIndex].pxClassInfomation->xType ==
                s_UsbDeviceClassInterfaceMap[mapIndex].xType)
            {
                (void)s_UsbDeviceClassInterfaceMap[mapIndex].xClassInit(
                    controllerId, &classHandle->pxConfigList->pxConfig[classIndex],
                    &classHandle->pxConfigList->pxConfig[classIndex].xClassHandle);
            }
        }
    }

    *handle = classHandle->xHandle;
    return error;
}

static int32_t usb_device_class_deinit(uint8_t controllerId /*!< [IN] Controller ID */
                                   )
{
    USBDeviceCommonClassStruct_t *classHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;
    uint8_t mapIndex;
    uint8_t classIndex;

    /* Get the common class handle according to the controller id. */
    error = USB_DeviceClassGetHandleByControllerId(controllerId, &classHandle);

    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        return error;
    }

    /* De-initialize the all supported classes according to the configuration list. */
    for (classIndex = 0U; classIndex < classHandle->pxConfigList->ucCount; classIndex++)
    {
        for (mapIndex = 0U; mapIndex < (sizeof(s_UsbDeviceClassInterfaceMap) / sizeof(USBDeviceClassMap_t));
             mapIndex++)
        {
            if (classHandle->pxConfigList->pxConfig[classIndex].pxClassInfomation->xType ==
                s_UsbDeviceClassInterfaceMap[mapIndex].xType)
            {
                (void)s_UsbDeviceClassInterfaceMap[mapIndex].xClassDeinit(
                    classHandle->pxConfigList->pxConfig[classIndex].xClassHandle);
            }
        }
    }

    /* De-initialize the USB device stack. */
    error = iot_usb_device_close(classHandle->xHandle);
    TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
    if (IOT_USB_DEVICE_SUCCESS == error)
    {
        /* Free the common class handle. */
        (void)USB_DeviceClassFreeHandle(controllerId);
    }
    return error;
}

static int32_t usb_device_class_get_speed(uint8_t controllerId, /*!< [IN] Controller ID */
                                     uint8_t *speed        /*!< [OUT] Current speed */
                                     )
{
    USBDeviceCommonClassStruct_t *classHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;

    /* Get the common class handle according to the controller id. */
    error = USB_DeviceClassGetHandleByControllerId(controllerId, &classHandle);

    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        return error;
    }

    /* Get the current speed. */
    error = iot_usb_device_ioctl(classHandle->xHandle, eUSBDeviceGetSpeed, speed);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    return error;
}

/* usb hid class */
#define testIotUsbDevice_CONFIG_HID_CLASS_CODE (0x03U)
#define testIotUsbDevice_HID_REQUEST_GET_REPORT (0x01U)
#define testIotUsbDevice_HID_REQUEST_GET_REPORT_TYPE_INPUT (0x01U)
#define testIotUsbDevice_HID_REQUEST_GET_REPORT_TYPE_OUPUT (0x02U)
#define testIotUsbDevice_HID_REQUEST_GET_REPORT_TYPE_FEATURE (0x03U)
#define testIotUsbDevice_HID_REQUEST_GET_IDLE (0x02U)
#define testIotUsbDevice_HID_REQUEST_GET_PROTOCOL (0x03U)
#define testIotUsbDevice_HID_REQUEST_SET_REPORT (0x09U)
#define testIotUsbDevice_HID_REQUEST_SET_IDLE (0x0AU)
#define testIotUsbDevice_HID_REQUEST_SET_PROTOCOL (0x0BU)

/*! @brief Available common EVENT types in HID class callback */
typedef enum
{
    eUSBDeviceHidEventSendResponse = 0x01U, /*!< Send data completed or cancelled etc*/
    eUSBDeviceHidEventRecvResponse,         /*!< Data received or cancelled etc*/
    eUSBDeviceHidEventGetReport,            /*!< Get report request */
    eUSBDeviceHidEventGetIdle,              /*!< Get idle request */
    eUSBDeviceHidEventGetProtocol,          /*!< Get protocol request */
    eUSBDeviceHidEventSetReport,            /*!< Set report request */
    eUSBDeviceHidEventSetIdle,              /*!< Set idle request */
    eUSBDeviceHidEventSetProtocol,          /*!< Set protocol request */
    eUSBDeviceHidEventRequestReportBuffer,  /*!< Get buffer to save the data of the set report request. */
} USBDeviceHidEvent_t;

typedef struct
{
    uint8_t * pucReportBuffer; /*!< The report buffer address */
    uint32_t ulReportLength; /*!< The report data length */
    uint8_t ucReportType;    /*!< The report type */
    uint8_t ucReportId;      /*!< The report ID */
} USBDeviceHidReportStruct_t;;

/*! @brief The HID device class status structure */
typedef struct
{
    uint8_t *pucBuffer; /*!< Transferred buffer */
    uint32_t ulLength; /*!< Transferred data length */
    uint8_t ucSetupInProgress;      /*!< if in setup phase */
    IotUsbDeviceHandle_t pxHandle;                       /*!< The device handle */
    USBDeviceClassConfigStruct_t * pxConfigStruct; /*!< The configuration of the class. */
    USBDeviceInterfaceStruct_t * pxInterfaceHandle; /*!< Current interface handle */
    uint8_t * pucInterruptInPipeDataBuffer;             /*!< IN pipe data buffer backup when stall */
    uint32_t ulInterruptInPipeDataLen;                /*!< IN pipe data length backup when stall  */
    uint8_t * pucInterruptOutPipeDataBuffer;             /*!< OUT pipe data buffer backup when stall */
    uint32_t ulInterruptOutPipeDataLen;                /*!< OUT pipe data length backup when stall  */
    uint8_t ucConfiguration;                          /*!< Current configuration */
    uint8_t ucInterfaceNumber;                        /*!< The interface number of the class */
    uint8_t ucAlternate;                              /*!< Current alternate setting of the interface */
    uint8_t ucIdleRate;                               /*!< The idle rate of the HID device */
    uint8_t ucProtocol;                               /*!< Current protocol */
    uint8_t ucInterruptInPipeBusy;                    /*!< Interrupt IN pipe busy flag */
    uint8_t ucInterruptOutPipeBusy;                   /*!< Interrupt OUT pipe busy flag */
    uint8_t ucInterruptInPipeStall;                    /*!< Interrupt IN pipe stall flag */
    uint8_t ucInterruptOutPipeStall;                   /*!< Interrupt OUT pipe stall flag */
} USBDeviceHidStruct_t;

static int32_t USB_DeviceHidAllocateHandle(USBDeviceHidStruct_t **handle);
static int32_t USB_DeviceHidFreeHandle(USBDeviceHidStruct_t *handle);
static int32_t USB_DeviceHidInterruptIn(IotUsbDeviceOperationStatus_t xStatus,
                                             void *callbackParam);
static int32_t USB_DeviceHidInterruptOut(IotUsbDeviceOperationStatus_t xStatus,
                                              void *callbackParam);
static int32_t USB_DeviceHidEndpointsInit(USBDeviceHidStruct_t *hidHandle);
static int32_t USB_DeviceHidEndpointsDeinit(USBDeviceHidStruct_t *hidHandle);

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static USBDeviceHidStruct_t
    s_UsbDeviceHidHandle[testIotUsbDevice_CONFIG_HID];

static int32_t USB_DeviceHidAllocateHandle(USBDeviceHidStruct_t **handle)
{
    uint32_t count;
    for (count = 0U; count < testIotUsbDevice_CONFIG_HID; count++)
    {
        if (NULL == s_UsbDeviceHidHandle[count].pxHandle)
        {
            *handle = &s_UsbDeviceHidHandle[count];
            return IOT_USB_DEVICE_SUCCESS;
        }
    }

    return IOT_USB_DEVICE_BUSY;
}

static int32_t USB_DeviceHidFreeHandle(USBDeviceHidStruct_t *handle)
{
    handle->pxHandle = NULL;
    handle->pxConfigStruct = (USBDeviceClassConfigStruct_t *)NULL;
    handle->ucConfiguration = 0U;
    handle->ucAlternate = 0U;
    return IOT_USB_DEVICE_SUCCESS;
}

static int32_t USB_DeviceHidInterruptIn(IotUsbDeviceOperationStatus_t xStatus,
                                             void *callbackParam)
{
    USBDeviceHidStruct_t *hidHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;
    USBDeviceEndpointCallbackInformation_t message;

    /* Get the HID class handle */
    hidHandle = (USBDeviceHidStruct_t *)callbackParam;

    if (!hidHandle)
    {
        return IOT_USB_DEVICE_INVALID_HANDLE;
    }
    hidHandle->ucInterruptInPipeBusy = 0U;
    message.pucBuffer = hidHandle->pucBuffer;
    message.ulLength = hidHandle->ulLength;
    message.ucSetupInProgress = hidHandle->ucSetupInProgress;
    if ((NULL != hidHandle->pxConfigStruct) && (hidHandle->pxConfigStruct->xClassCallback))
    {
        /* Notify the application data sent by calling the hid class callback. xClassCallback is initialized
           in classInit of s_UsbDeviceClassInterfaceMap,it is from the second parameter of classInit */
        error =
            hidHandle->pxConfigStruct->xClassCallback((class_handle_t)hidHandle, eUSBDeviceHidEventSendResponse, &message);
    }

    return error;
}

static int32_t USB_DeviceHidInterruptOut(IotUsbDeviceOperationStatus_t xStatus,
                                              void *callbackParam)
{
    USBDeviceHidStruct_t *hidHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;
    USBDeviceEndpointCallbackInformation_t message;

    /* Get the HID class handle */
    hidHandle = (USBDeviceHidStruct_t *)callbackParam;

    if (!hidHandle)
    {
        return IOT_USB_DEVICE_INVALID_HANDLE;
    }
    hidHandle->ucInterruptOutPipeBusy = 0U;
    message.pucBuffer = hidHandle->pucBuffer;
    message.ulLength = hidHandle->ulLength;
    message.ucSetupInProgress = hidHandle->ucSetupInProgress;
    if ((NULL != hidHandle->pxConfigStruct) && (hidHandle->pxConfigStruct->xClassCallback))
    {
        /* Notify the application data sent by calling the hid class callback. xClassCallback is initialized
           in classInit of s_UsbDeviceClassInterfaceMap,it is from the second parameter of classInit */
        error =
            hidHandle->pxConfigStruct->xClassCallback((class_handle_t)hidHandle, eUSBDeviceHidEventRecvResponse, &message);
    }

    return error;
}


static int32_t USB_DeviceHidEndpointsInit(USBDeviceHidStruct_t *hidHandle)
{
    USBDeviceInterfaceList_t *interfaceList;
    USBDeviceInterfaceStruct_t *interface = (USBDeviceInterfaceStruct_t *)NULL;
    int32_t error = IOT_USB_DEVICE_ERROR;

    /* Check the configuration is valid or not. */
    if (!hidHandle->ucConfiguration)
    {
        return error;
    }

    if (hidHandle->ucConfiguration > hidHandle->pxConfigStruct->pxClassInfomation->ucConfigurations)
    {
        return error;
    }

    /* Get the interface list of the new configuration. */
    if (NULL == hidHandle->pxConfigStruct->pxClassInfomation->pxInterfaceList)
    {
        return error;
    }
    interfaceList = &hidHandle->pxConfigStruct->pxClassInfomation->pxInterfaceList[hidHandle->ucConfiguration - 1U];

    /* Find interface by using the alternate setting of the interface. */
    for (int count = 0U; count < interfaceList->ucCount; count++)
    {
        if (testIotUsbDevice_CONFIG_HID_CLASS_CODE == interfaceList->pxInterfaces[count].ucClassCode)
        {
            for (int index = 0U; index < interfaceList->pxInterfaces[count].ucCount; index++)
            {
                if (interfaceList->pxInterfaces[count].pxInterface[index].ucAlternateSetting == hidHandle->ucAlternate)
                {
                    interface = &interfaceList->pxInterfaces[count].pxInterface[index];
                    break;
                }
            }
            hidHandle->ucInterfaceNumber = interfaceList->pxInterfaces[count].ucInterfaceNumber;
            break;
        }
    }
    if (!interface)
    {
        /* Return error if the interface is not found. */
        return error;
    }

    /* Keep new interface handle. */
    hidHandle->pxInterfaceHandle = interface;

    /* Initialize the endpoints of the new interface. */
    for (int count = 0U; count < interface->xEndpointList.ucCount; count++)
    {
        IotUsbDeviceEndpointConfig_t epInitStruct;
        IotUsbDeviceEndpointCallbackFn_t epCallback;
        epInitStruct.ucZlt = 0U;
        epInitStruct.ucInterval = interface->xEndpointList.pxEndpoint[count].ucInterval;
        epInitStruct.ucEndpointAddress = interface->xEndpointList.pxEndpoint[count].ucEndpointAddress;
        epInitStruct.usMaxPacketSize = interface->xEndpointList.pxEndpoint[count].usMaxPacketSize;
        epInitStruct.ucTransferType = interface->xEndpointList.pxEndpoint[count].ucTransferType;

        if (eUSBTransferDirectionIn == ((epInitStruct.ucEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                       USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
        {
            epCallback = USB_DeviceHidInterruptIn;
            hidHandle->pucInterruptInPipeDataBuffer = (uint8_t*)IOT_USB_UNINITIALIZED_VAL_32;
            hidHandle->ucInterruptInPipeStall = 0U;
            hidHandle->ulInterruptInPipeDataLen = 0U;
        }
        else
        {
            epCallback = USB_DeviceHidInterruptOut;
            hidHandle->pucInterruptOutPipeDataBuffer = (uint8_t*)IOT_USB_UNINITIALIZED_VAL_32;
            hidHandle->ucInterruptOutPipeStall = 0U;
            hidHandle->ulInterruptOutPipeDataLen = 0U;
        }

        error = iot_usb_device_endpoint_open(hidHandle->pxHandle, &epInitStruct);
        TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
        iot_usb_device_set_endpoint_callback(hidHandle->pxHandle, epInitStruct.ucEndpointAddress, epCallback, hidHandle);
    }
    return error;
}

static int32_t USB_DeviceHidEndpointsDeinit(USBDeviceHidStruct_t *hidHandle)
{
    int32_t error = IOT_USB_DEVICE_ERROR;

    if (!hidHandle->pxInterfaceHandle)
    {
        return error;
    }
    /* De-initialize all endpoints of the interface */
    for (int count = 0U; count < hidHandle->pxInterfaceHandle->xEndpointList.ucCount; count++)
    {
        error = iot_usb_device_endpoint_close(hidHandle->pxHandle,
                                         hidHandle->pxInterfaceHandle->xEndpointList.pxEndpoint[count].ucEndpointAddress);
        TEST_ASSERT_EQUAL(error, IOT_USB_DEVICE_SUCCESS);
    }
    hidHandle->pxInterfaceHandle = NULL;
    return error;
}

static int32_t usb_device_hid_event(void *handle, uint32_t event, void *param)
{
    USBDeviceHidStruct_t *hidHandle;
    USBDeviceHidReportStruct_t report;
    int32_t error = IOT_USB_DEVICE_ERROR;
    uint16_t interfaceAlternate;
    uint8_t *temp8;
    uint8_t alternate;

    if ((!param) || (!handle))
    {
        return IOT_USB_DEVICE_INVALID_HANDLE;
    }

    /* Get the hid class handle. */
    hidHandle = (USBDeviceHidStruct_t *)handle;

    switch (event)
    {
        case eUSBDeviceClassEventDeviceReset:
            /* Bus reset, clear the configuration. */
            hidHandle->ucConfiguration = 0U;
            hidHandle->ucInterruptInPipeBusy = 0U;
            hidHandle->ucInterruptOutPipeBusy = 0U;
            hidHandle->pxInterfaceHandle = NULL;
            break;
        case eUSBDeviceClassEventSetConfiguration:
            /* Get the new configuration. */
            temp8 = ((uint8_t *)param);
            if (!hidHandle->pxConfigStruct)
            {
                break;
            }
            if (*temp8 == hidHandle->ucConfiguration)
            {
                break;
            }

            /* De-initialize the endpoints when current configuration is none zero. */
            if (hidHandle->ucConfiguration)
            {
                error = USB_DeviceHidEndpointsDeinit(hidHandle);
            }
            /* Save new configuration. */
            hidHandle->ucConfiguration = *temp8;
            /* Clear the alternate setting value. */
            hidHandle->ucAlternate = 0U;

            /* Initialize the endpoints of the new current configuration by using the alternate setting 0. */
            error = USB_DeviceHidEndpointsInit(hidHandle);
            break;
            case eUSBDeviceClassEventSetInterface:
            if (!hidHandle->pxConfigStruct)
            {
                break;
            }
            /* Get the new alternate setting of the interface */
            interfaceAlternate = *((uint16_t *)param);
            /* Get the alternate setting value */
            alternate = (uint8_t)(interfaceAlternate & 0xFFU);

            /* Whether the interface belongs to the class. */
            if (hidHandle->ucInterfaceNumber != ((uint8_t)(interfaceAlternate >> 8U)))
            {
                break;
            }
            /* Only handle new alternate setting. */
            if (alternate == hidHandle->ucAlternate)
            {
                break;
            }
            /* De-initialize old endpoints */
            error = USB_DeviceHidEndpointsDeinit(hidHandle);
            hidHandle->ucAlternate = alternate;
            /* Initialize new endpoints */
            error = USB_DeviceHidEndpointsInit(hidHandle);
            break;
        case eUSBDeviceClassEventSetEndpointHalt:
            if ((!hidHandle->pxConfigStruct) || (!hidHandle->pxInterfaceHandle))
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = ((uint8_t *)param);
            for (int count = 0U; count < hidHandle->pxInterfaceHandle->xEndpointList.ucCount; count++)
            {
                if (*temp8 == hidHandle->pxInterfaceHandle->xEndpointList.pxEndpoint[count].ucEndpointAddress)
                {
                    /* Only stall the endpoint belongs to the class */
                    if (eUSBTransferDirectionIn == ((hidHandle->pxInterfaceHandle->xEndpointList.pxEndpoint[count].ucEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                       USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                       hidHandle->ucInterruptInPipeStall = 1U;
                    }
                    else
                    {
                        hidHandle->ucInterruptOutPipeStall = 1U;
                    }
                    error = iot_usb_device_endpoint_stall(hidHandle->pxHandle, *temp8);
                    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
                }
            }
            break;
            case eUSBDeviceClassEventClearEndpointHalt:
            if ((!hidHandle->pxConfigStruct) || (!hidHandle->pxInterfaceHandle))
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = ((uint8_t *)param);
            for (int count = 0U; count < hidHandle->pxInterfaceHandle->xEndpointList.ucCount; count++)
            {
                if (*temp8 == hidHandle->pxInterfaceHandle->xEndpointList.pxEndpoint[count].ucEndpointAddress)
                {
                    /* Only un-stall the endpoint belongs to the class */
                    error = iot_usb_device_endpoint_unstall(hidHandle->pxHandle, *temp8);
                    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
                    if (eUSBTransferDirectionIn == (((*temp8) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                    USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        if (hidHandle->ucInterruptInPipeStall)
                        {
                            hidHandle->ucInterruptInPipeStall = 0U;
                            if ((uint8_t*)IOT_USB_UNINITIALIZED_VAL_32 != hidHandle->pucInterruptInPipeDataBuffer)
                            {
                                error = iot_usb_device_write_async(hidHandle->pxHandle, (hidHandle->pxInterfaceHandle->xEndpointList.pxEndpoint[count].ucEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK),
                                                              hidHandle->pucInterruptInPipeDataBuffer, hidHandle->ulInterruptInPipeDataLen);
                                TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
                                if (IOT_USB_DEVICE_SUCCESS != error)
                                {
                                    hidHandle->pucBuffer = hidHandle->pucInterruptInPipeDataBuffer;
                                    hidHandle->ulLength = hidHandle->ulInterruptInPipeDataLen;
                                    hidHandle->ucSetupInProgress = 0U;
                                    USB_DeviceHidInterruptIn(eUsbDeviceWriteFailed, handle);
                                }
                                hidHandle->pucInterruptInPipeDataBuffer = (uint8_t*)IOT_USB_UNINITIALIZED_VAL_32;
                                hidHandle->ulInterruptInPipeDataLen = 0U;
                            }
                        }
                    }
                    else
                    {
                        if (hidHandle->ucInterruptOutPipeStall)
                        {
                            hidHandle->ucInterruptOutPipeStall = 0U;
                            if ((uint8_t*)IOT_USB_UNINITIALIZED_VAL_32 != hidHandle->pucInterruptOutPipeDataBuffer)
                            {
                                error = iot_usb_device_read_async(hidHandle->pxHandle, (hidHandle->pxInterfaceHandle->xEndpointList.pxEndpoint[count].ucEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK),
                                                              hidHandle->pucInterruptOutPipeDataBuffer, hidHandle->ulInterruptOutPipeDataLen);
                                TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
                                if (IOT_USB_DEVICE_SUCCESS != error)
                                {
                                    hidHandle->pucBuffer = hidHandle->pucInterruptOutPipeDataBuffer;
                                    hidHandle->ulLength = hidHandle->ulInterruptOutPipeDataLen;
                                    hidHandle->ucSetupInProgress = 0U;
                                    USB_DeviceHidInterruptOut(eUsbDeviceReadFailed, handle);
                                }
                                hidHandle->pucInterruptOutPipeDataBuffer = (uint8_t*)IOT_USB_UNINITIALIZED_VAL_32;;
                                hidHandle->ulInterruptOutPipeDataLen = 0U;
                            }
                        }
                    }
                }
            }
            break;
        case eUSBDeviceClassEventClassRequest:
            if (param)
            {
                /* Handle the hid class specific request. */
                USBDeviceControlRequestStruct_t *controlRequest = (USBDeviceControlRequestStruct_t *)param;

                if ((controlRequest->pxSetup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) !=
                    USB_REQUEST_TYPE_RECIPIENT_INTERFACE)
                {
                    break;
                }

                if ((controlRequest->pxSetup->wIndex & 0xFFU) != hidHandle->ucInterfaceNumber)
                {
                    break;
                }

                switch (controlRequest->pxSetup->bRequest)
                {
                    case testIotUsbDevice_HID_REQUEST_GET_REPORT:
                        /* Get report request */
                        report.ucReportType = (controlRequest->pxSetup->wValue & 0xFF00U) >> 0x08U;
                        report.ucReportId = (controlRequest->pxSetup->wValue & 0x00FFU);
                        /* xClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                           it is from the second parameter of classInit */
                        error = hidHandle->pxConfigStruct->xClassCallback((class_handle_t)hidHandle,
                                                                       eUSBDeviceHidEventGetReport, &report);
                        controlRequest->pucBuffer = report.pucReportBuffer;
                        controlRequest->ulLength = report.ulReportLength;
                        break;
                    case testIotUsbDevice_HID_REQUEST_GET_IDLE:
                        /* Get idle request, xClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                           it is from the second parameter of classInit */
                        error = hidHandle->pxConfigStruct->xClassCallback(
                            (class_handle_t)hidHandle, eUSBDeviceHidEventGetIdle, &hidHandle->ucIdleRate);
                        controlRequest->pucBuffer = &hidHandle->ucIdleRate;
                        break;
                    case testIotUsbDevice_HID_REQUEST_GET_PROTOCOL:
                        /* Get protocol request, xClassCallback is initialized in classInit of
                           s_UsbDeviceClassInterfaceMap,
                           it is from the second parameter of classInit */
                        error = hidHandle->pxConfigStruct->xClassCallback(
                            (class_handle_t)hidHandle, eUSBDeviceHidEventGetIdle, &hidHandle->ucProtocol);
                        controlRequest->pucBuffer = &hidHandle->ucProtocol;
                        break;
                    case testIotUsbDevice_HID_REQUEST_SET_REPORT:
                        /* Set report request */
                        report.ucReportType = (controlRequest->pxSetup->wValue & 0xFF00U) >> 0x08U;
                        report.ucReportId = (controlRequest->pxSetup->wValue & 0x00FFU);
                        if (controlRequest->ucIsSetup)
                        {
                            report.ulReportLength = controlRequest->ulLength;
                            /* xClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                               it is from the second parameter of classInit */
                            error = hidHandle->pxConfigStruct->xClassCallback(
                                (class_handle_t)hidHandle, eUSBDeviceHidEventRequestReportBuffer, &report);
                            controlRequest->pucBuffer = report.pucReportBuffer;
                            controlRequest->ulLength = report.ulReportLength;
                        }
                        else
                        {
                            report.pucReportBuffer = controlRequest->pucBuffer;
                            report.ulReportLength = controlRequest->ulLength;
                            /* xClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                               it is from the second parameter of classInit */
                            error = hidHandle->pxConfigStruct->xClassCallback((class_handle_t)hidHandle,
                                                                           eUSBDeviceHidEventSetReport, &report);
                        }
                        break;
                    case testIotUsbDevice_HID_REQUEST_SET_IDLE:
                        /* Set idle request */
                        {
                            hidHandle->ucIdleRate = (controlRequest->pxSetup->wValue & 0xFF00U) >> 0x08U;
                            /* xClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                               it is from the second parameter of classInit */
                            error = hidHandle->pxConfigStruct->xClassCallback(
                                (class_handle_t)hidHandle, eUSBDeviceHidEventSetIdle, &hidHandle->ucIdleRate);
                        }
                        break;
                    case testIotUsbDevice_HID_REQUEST_SET_PROTOCOL:
                        /* Set protocol request */
                        {
                            hidHandle->ucProtocol = (controlRequest->pxSetup->wValue & 0x00FFU);
                            /* xClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                               it is from the second parameter of classInit */
                            error = hidHandle->pxConfigStruct->xClassCallback(
                                (class_handle_t)hidHandle, eUSBDeviceHidEventSetProtocol, &hidHandle->ucProtocol);
                        }
                        break;
                    default:
                        error = IOT_USB_DEVICE_INVALID_REQUEST;
                        break;
                }
            }
            break;
        default:
            break;
    }
    return error;
}

static int32_t usb_device_hid_init(uint8_t controllerId, USBDeviceClassConfigStruct_t *config, class_handle_t *handle)
{
    USBDeviceHidStruct_t *hidHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;

    /* Allocate a hid class handle. */
    error = USB_DeviceHidAllocateHandle(&hidHandle);

    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        return error;
    }

    /* Get the device handle according to the controller id. */
    error = usb_device_class_get_device_handle(controllerId, &hidHandle->pxHandle);

    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        return error;
    }

    if (!hidHandle->pxHandle)
    {
        return IOT_USB_DEVICE_INVALID_HANDLE;
    }
    /* Save the configuration of the class. */
    hidHandle->pxConfigStruct = config;
    /* Clear the configuration value. */
    hidHandle->ucConfiguration = 0U;
    hidHandle->ucAlternate = 0xffU;

    *handle = (class_handle_t)hidHandle;
    return error;
}

static int32_t usb_device_hid_deinit(class_handle_t handle)
{
    USBDeviceHidStruct_t *hidHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;

    hidHandle = (USBDeviceHidStruct_t *)handle;

    if (!hidHandle)
    {
        return IOT_USB_DEVICE_INVALID_HANDLE;
    }
    /* De-initialzie the endpoints. */
    error = USB_DeviceHidEndpointsDeinit(hidHandle);
    /* Free the hid class handle. */
    USB_DeviceHidFreeHandle(hidHandle);
    return error;
}

static int32_t usb_device_hid_send(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    USBDeviceHidStruct_t *hidHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;

    if (!handle)
    {
        return IOT_USB_DEVICE_INVALID_HANDLE;
    }
    hidHandle = (USBDeviceHidStruct_t *)handle;

    if (hidHandle->ucInterruptInPipeBusy)
    {
        return IOT_USB_DEVICE_BUSY;
    }
    hidHandle->ucInterruptInPipeBusy = 1U;

    if (hidHandle->ucInterruptInPipeStall)
    {
        hidHandle->pucInterruptInPipeDataBuffer = buffer;
        hidHandle->ulInterruptInPipeDataLen = length;
        return IOT_USB_DEVICE_SUCCESS;
    }
    error = iot_usb_device_write_async(hidHandle->pxHandle, ep, buffer, length);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        hidHandle->ucInterruptInPipeBusy = 0U;
    }
    return error;
}

static int32_t usb_device_hid_recv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    USBDeviceHidStruct_t *hidHandle;
    int32_t error = IOT_USB_DEVICE_ERROR;

    if (!handle)
    {
        return IOT_USB_DEVICE_INVALID_HANDLE;
    }
    hidHandle = (USBDeviceHidStruct_t *)handle;

    if (hidHandle->ucInterruptOutPipeBusy)
    {
        return IOT_USB_DEVICE_BUSY;
    }
    hidHandle->ucInterruptOutPipeBusy = 1U;

    if (hidHandle->ucInterruptOutPipeStall)
    {
        hidHandle->pucInterruptOutPipeDataBuffer = buffer;
        hidHandle->ulInterruptOutPipeDataLen = length;
        return IOT_USB_DEVICE_SUCCESS;
    }
    error = iot_usb_device_read_async(hidHandle->pxHandle, ep, buffer, length);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    if (IOT_USB_DEVICE_SUCCESS != error)
    {
        hidHandle->ucInterruptOutPipeBusy = 0U;
    }
    return error;
}

/* usb descriptor*/
#define testIotUsbDevice_SPECIFIC_BCD_VERSION (0x0200U)
#define testIotUsbDevice_DEMO_BCD_VERSION (0x0101U)

#define testIotUsbDevice_CLASS (0x00U)
#define testIotUsbDevice_SUBCLASS (0x00U)
#define testIotUsbDevice_PROTOCOL (0x00U)

#define testIotUsbDevice_MAX_POWER (0x32U)

#define testIotUsbDevice_DESCRIPTOR_LENGTH_CONFIGURATION_ALL (sizeof(g_UsbDeviceConfigurationDescriptor))
#define testIotUsbDevice_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT (sizeof(g_UsbDeviceHidGenericReportDescriptor))
#define testIotUsbDevice_DESCRIPTOR_LENGTH_HID (9U)
#define testIotUsbDevice_DESCRIPTOR_LENGTH_STRING0 (sizeof(g_UsbDeviceString0))
#define testIotUsbDevice_DESCRIPTOR_LENGTH_STRING1 (sizeof(g_UsbDeviceString1))
#define testIotUsbDevice_DESCRIPTOR_LENGTH_STRING2 (sizeof(g_UsbDeviceString2))

#define testIotUsbDevice_CONFIGURATION_COUNT (1U)
#define testIotUsbDevice_STRING_COUNT (3U)
#define testIotUsbDevice_LANGUAGE_COUNT (1U)

#define testIotUsbDevice_HID_GENERIC_CONFIGURE_INDEX (1U)
#define testIotUsbDevice_HID_GENERIC_INTERFACE_COUNT (1U)
/*IN lenght is same with out lenght, in case in length is not equal to out length, pleae check*/
/*USB_DeviceHidSend/Recv to make sure the length parameter is right*/
#define testIotUsbDevice_HID_GENERIC_IN_BUFFER_LENGTH (32U)
#define testIotUsbDevice_HID_GENERIC_OUT_BUFFER_LENGTH (32U)
#define testIotUsbDevice_HID_GENERIC_ENDPOINT_COUNT (2U)
#define testIotUsbDevice_HID_GENERIC_INTERFACE_INDEX (0U)
#define testIotUsbDevice_HID_GENERIC_ENDPOINT_IN (1U)
#define testIotUsbDevice_HID_GENERIC_ENDPOINT_OUT (2U)

#define testIotUsbDevice_HID_GENERIC_CLASS (0x03U)
#define testIotUsbDevice_HID_GENERIC_SUBCLASS (0x00U)
#define testIotUsbDevice_HID_GENERIC_PROTOCOL (0x00U)

#define testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE (32U)
#define testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE (32U)
#define testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_OUT_INTERVAL (0x05U) /* 2^(5-1) = 4ms */
#define testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_INTERVAL (0x02U)

#define testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE (32U)
#define testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE (32U)
#define testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_IN_INTERVAL (0x05U) /* 2^(4-1) = 1ms */
#define testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_INTERVAL (0x0AU)

typedef struct _usb_hid_generic_struct
{
    IotUsbDeviceHandle_t deviceHandle;
    class_handle_t hidHandle;
    uint8_t *buffer[2];
    uint8_t bufferIndex;
    uint8_t idleRate;
    uint8_t speed;
    uint8_t attach;
    uint8_t currentConfiguration;
    uint8_t currentInterfaceAlternateSetting[testIotUsbDevice_HID_GENERIC_INTERFACE_COUNT];
} usb_hid_generic_struct_t;

/* hid generic endpoint information */
USBDeviceEndpointStruct_t g_UsbDeviceHidGenericEndpoints[testIotUsbDevice_HID_GENERIC_ENDPOINT_COUNT] = {
    /* HID generic interrupt IN pipe */
    {
        testIotUsbDevice_HID_GENERIC_ENDPOINT_IN | (eUSBTransferDirectionIn << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        eUSBEndpointInterrupt, testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE, 0,
    },
    /* HID generic interrupt OUT pipe */
    {
        testIotUsbDevice_HID_GENERIC_ENDPOINT_OUT | (eUSBTransferDirectionOut << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        eUSBEndpointInterrupt, testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE, 0,
    }};

/* HID generic interface information */
USBDeviceInterfaceStruct_t g_UsbDeviceHidGenericInterface[] = {{
    0U, /* The alternate setting of the interface */
    {
        testIotUsbDevice_HID_GENERIC_ENDPOINT_COUNT, /* Endpoint count */
        g_UsbDeviceHidGenericEndpoints, /* Endpoints handle */
    },
    NULL,
}};

USBDeviceInterfacesStruct_t g_UsbDeviceHidGenericInterfaces[testIotUsbDevice_HID_GENERIC_INTERFACE_COUNT] = {
    {
        testIotUsbDevice_HID_GENERIC_CLASS,           /* HID generic class code */
        testIotUsbDevice_HID_GENERIC_SUBCLASS,        /* HID generic subclass code */
        testIotUsbDevice_HID_GENERIC_PROTOCOL,        /* HID generic protocol code */
        testIotUsbDevice_HID_GENERIC_INTERFACE_INDEX, /* The interface number of the HID generic */
        g_UsbDeviceHidGenericInterface,  /* Interfaces handle */
        sizeof(g_UsbDeviceHidGenericInterface) / sizeof(USBDeviceInterfacesStruct_t),
    },
};

USBDeviceInterfaceList_t g_UsbDeviceHidGenericInterfaceList[testIotUsbDevice_CONFIGURATION_COUNT] = {
    {
        testIotUsbDevice_HID_GENERIC_INTERFACE_COUNT, /* The interface count of the HID generic */
        g_UsbDeviceHidGenericInterfaces, /* The interfaces handle */
    },
};

USBDeviceClassStruct_t g_UsbDeviceHidGenericConfig = {
    g_UsbDeviceHidGenericInterfaceList, /* The interface list of the HID generic */
    eUSBDeviceClassTypeHid,            /* The HID class type */
    testIotUsbDevice_CONFIGURATION_COUNT,     /* The configuration count */
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceHidGenericReportDescriptor[] = {
    0x05U, 0x81U, /* Usage Page (Vendor defined)*/
    0x09U, 0x82U, /* Usage (Vendor defined) */
    0xA1U, 0x01U, /* Collection (Application) */
    0x09U, 0x83U, /* Usage (Vendor defined) */

    0x09U, 0x84U, /* Usage (Vendor defined) */
    0x15U, 0x80U, /* Logical Minimum (-128) */
    0x25U, 0x7FU, /* Logical Maximum (127) */
    0x75U, 0x08U, /* Report Size (8U) */
    0x95U, 0x08U, /* Report Count (8U) */
    0x81U, 0x02U, /* Input(Data, Variable, Absolute) */

    0x09U, 0x84U, /* Usage (Vendor defined) */
    0x15U, 0x80U, /* Logical Minimum (-128) */
    0x25U, 0x7FU, /* Logical Maximum (127) */
    0x75U, 0x08U, /* Report Size (8U) */
    0x95U, 0x08U, /* Report Count (8U) */
    0x91U, 0x02U, /* Output(Data, Variable, Absolute) */
    0xC0U,        /* End collection */
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceDescriptor[] = {
    eUSBDescriptorLengthDevice, /* Size of this descriptor in bytes */
    eUSBDescriptorTypeDevice,   /* DEVICE Descriptor Type */
    USB_SHORT_GET_LOW(testIotUsbDevice_SPECIFIC_BCD_VERSION),
    USB_SHORT_GET_HIGH(testIotUsbDevice_SPECIFIC_BCD_VERSION), /* USB Specification Release Number in
                                                            Binary-Coded Decimal (i.e., 2.10 is 210H). */
    testIotUsbDevice_CLASS,                                    /* Class code (assigned by the USB-IF). */
    testIotUsbDevice_SUBCLASS,                                 /* Subclass code (assigned by the USB-IF). */
    testIotUsbDevice_PROTOCOL,                                 /* Protocol code (assigned by the USB-IF). */
    testIotUsb_CONTROL_MAX_PACKET_SIZE,                         /* Maximum packet size for endpoint zero
                                                            (only 8, 16, 32, or 64 are valid) */

    0xC9U, 0x1FU,                                        /* Vendor ID (assigned by the USB-IF) */
    0xA2U, 0x00U,                                        /* Product ID (assigned by the manufacturer) */
    USB_SHORT_GET_LOW(testIotUsbDevice_DEMO_BCD_VERSION),
    USB_SHORT_GET_HIGH(testIotUsbDevice_DEMO_BCD_VERSION), /* Device release number in binary-coded decimal */
    0x01U,                                           /* Index of string descriptor describing manufacturer */
    0x02U,                                           /* Index of string descriptor describing product */
    0x00U,                                           /* Index of string descriptor describing the
                                                        device's serial number */
    testIotUsbDevice_CONFIGURATION_COUNT,                  /* Number of possible configurations */
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceConfigurationDescriptor[] = {
    eUSBDescriptorLengthConfigure, /* Size of this descriptor in bytes */
    eUSBDescriptorTypeConfigure,   /* CONFIGURATION Descriptor Type */
    USB_SHORT_GET_LOW(eUSBDescriptorLengthConfigure + eUSBDescriptorLengthInterface + testIotUsbDevice_DESCRIPTOR_LENGTH_HID +
                      eUSBDescriptorLengthEndpoint + eUSBDescriptorLengthEndpoint),
    USB_SHORT_GET_HIGH(eUSBDescriptorLengthConfigure + eUSBDescriptorLengthInterface + testIotUsbDevice_DESCRIPTOR_LENGTH_HID +
                       eUSBDescriptorLengthEndpoint +
                       eUSBDescriptorLengthEndpoint), /* Total length of data returned for this configuration. */
    testIotUsbDevice_HID_GENERIC_INTERFACE_COUNT,                    /* Number of interfaces supported by this configuration */
    testIotUsbDevice_HID_GENERIC_CONFIGURE_INDEX,                    /* Value to use as an argument to the
                                                           SetConfiguration() request to select this configuration */
    0x00U,                                              /* Index of string descriptor describing this configuration */
    (USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_MASK) |
        (testIotUsbDevice_CONFIG_SELF_POWER << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_SHIFT) |
        (testIotUsbDevice_CONFIG_REMOTE_WAKEUP << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_SHIFT),
    /* Configuration characteristics
         D7: Reserved (set to one)
         D6: Self-powered
         D5: Remote Wakeup
         D4...0: Reserved (reset to zero)
    */
    testIotUsbDevice_MAX_POWER,            /* Maximum power consumption of the USB
                                      * device from the bus in this specific
                                      * configuration when the device is fully
                                      * operational. Expressed in 2 mA units
                                      *  (i.e., 50 = 100 mA).
                                      */
    eUSBDescriptorLengthInterface, /* Size of this descriptor in bytes */
    eUSBDescriptorTypeInterface,   /* INTERFACE Descriptor Type */
    testIotUsbDevice_HID_GENERIC_INTERFACE_INDEX, /* Number of this interface. */
    0x00U,                           /* Value used to select this alternate setting
                                        for the interface identified in the prior field */
    testIotUsbDevice_HID_GENERIC_ENDPOINT_COUNT,  /* Number of endpoints used by this
                                        interface (excluding endpoint zero). */
    testIotUsbDevice_HID_GENERIC_CLASS,           /* Class code (assigned by the USB-IF). */
    testIotUsbDevice_HID_GENERIC_SUBCLASS,        /* Subclass code (assigned by the USB-IF). */
    testIotUsbDevice_HID_GENERIC_PROTOCOL,        /* Protocol code (assigned by the USB). */
    0x00U,                           /* Index of string descriptor describing this interface */

    testIotUsbDevice_DESCRIPTOR_LENGTH_HID,      /* Numeric expression that is the total size of the
                                       HID descriptor. */
    eUSBDescriptorTypeHid,        /* Constant name specifying type of HID
                                       descriptor. */
    0x00U, 0x01U,                   /* Numeric expression identifying the HID Class
                                       Specification release. */
    0x00U,                          /* Numeric expression identifying country code of
                                       the localized hardware */
    0x01U,                          /* Numeric expression specifying the number of
                                       class descriptors(at least one report descriptor) */
    eUSBDescriptorTypeHidReport, /* Constant name identifying type of class descriptor. */
    USB_SHORT_GET_LOW(testIotUsbDevice_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT),
    USB_SHORT_GET_HIGH(testIotUsbDevice_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT),
    /* Numeric expression that is the total size of the
       Report descriptor. */
    eUSBDescriptorLengthEndpoint, /* Size of this descriptor in bytes */
    eUSBDescriptorTypeEndpoint,   /* ENDPOINT Descriptor Type */
    testIotUsbDevice_HID_GENERIC_ENDPOINT_IN | (eUSBTransferDirectionIn << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    /* The address of the endpoint on the USB device
       described by this descriptor. */
    eUSBEndpointInterrupt, /* This field describes the endpoint's attributes */
    USB_SHORT_GET_LOW(testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE),
    USB_SHORT_GET_HIGH(testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE),
    /* Maximum packet size this endpoint is capable of
       sending or receiving when this configuration is
       selected. */
    testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_INTERVAL, /* Interval for polling endpoint for data transfers. */

    eUSBDescriptorLengthEndpoint, /* Size of this descriptor in bytes */
    eUSBDescriptorTypeEndpoint,   /* ENDPOINT Descriptor Type */
    testIotUsbDevice_HID_GENERIC_ENDPOINT_OUT | (eUSBTransferDirectionOut << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    /* The address of the endpoint on the USB device
       described by this descriptor. */
    eUSBEndpointInterrupt, /* This field describes the endpoint's attributes */
    USB_SHORT_GET_LOW(testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE),
    USB_SHORT_GET_HIGH(testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE),
    /* Maximum packet size this endpoint is capable of
       sending or receiving when this configuration is
       selected. */
    testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_INTERVAL, /* Interval for polling endpoint for data transfers. */
};

#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceQualifierDescriptor[] = {
    eUSBDescriptorLengthDeviceQualitier, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_DEVICE_QUALITIER,   /* DEVICE Descriptor Type */
    USB_SHORT_GET_LOW(testIotUsbDevice_SPECIFIC_BCD_VERSION),
    USB_SHORT_GET_HIGH(testIotUsbDevice_SPECIFIC_BCD_VERSION), /* USB Specification Release Number in
                                                            Binary-Coded Decimal (i.e., 2.10 is 210H). */
    testIotUsbDevice_CLASS,                                    /* Class code (assigned by the USB-IF). */
    testIotUsbDevice_SUBCLASS,                                 /* Subclass code (assigned by the USB-IF). */
    testIotUsbDevice_PROTOCOL,                                 /* Protocol code (assigned by the USB-IF). */
    testIotUsb_CONTROL_MAX_PACKET_SIZE,                         /* Maximum packet size for endpoint zero
                                                            (only 8, 16, 32, or 64 are valid) */
    0x00U,                                               /* Number of Other-speed Configurations */
    0x00U,                                               /* Reserved for future use, must be zero */
};
#endif
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString0[] = {
    2U + 2U, eUSBDescriptorTypeString, 0x09U, 0x04U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString1[] = {
    2U + 2U * 18U, eUSBDescriptorTypeString,
    'N',           0x00U,
    'X',           0x00U,
    'P',           0x00U,
    ' ',           0x00U,
    'S',           0x00U,
    'E',           0x00U,
    'M',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    'O',           0x00U,
    'N',           0x00U,
    'D',           0x00U,
    'U',           0x00U,
    'C',           0x00U,
    'T',           0x00U,
    'O',           0x00U,
    'R',           0x00U,
    'S',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString2[] = {
    2U + 2U * 18U, eUSBDescriptorTypeString,
    'H',           0x00U,
    'I',           0x00U,
    'D',           0x00U,
    ' ',           0x00U,
    'G',           0x00U,
    'E',           0x00U,
    'N',           0x00U,
    'E',           0x00U,
    'R',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    ' ',           0x00U,
    'D',           0x00U,
    'E',           0x00U,
    'V',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    'E',           0x00U,
};

uint32_t g_UsbDeviceStringDescriptorLength[testIotUsbDevice_STRING_COUNT] = {
    sizeof(g_UsbDeviceString0), sizeof(g_UsbDeviceString1), sizeof(g_UsbDeviceString2),
};

uint8_t *g_UsbDeviceStringDescriptorArray[testIotUsbDevice_STRING_COUNT] = {
    g_UsbDeviceString0, g_UsbDeviceString1, g_UsbDeviceString2,
};

USBLanguage_t g_UsbDeviceLanguage[testIotUsbDevice_LANGUAGE_COUNT] = {{
    g_UsbDeviceStringDescriptorArray, g_UsbDeviceStringDescriptorLength, (uint16_t)0x0409U,
}};

USBLanguageList_t g_UsbDeviceLanguageList = {
    g_UsbDeviceString0, sizeof(g_UsbDeviceString0), g_UsbDeviceLanguage, testIotUsbDevice_LANGUAGE_COUNT,
};

static int32_t USB_DeviceGetDeviceDescriptor(IotUsbDeviceHandle_t handle,
                                           USBDeviceGetDeviceDescriptorStruct_t *deviceDescriptor)
{
    deviceDescriptor->pucBuffer = g_UsbDeviceDescriptor;
    deviceDescriptor->ulLength = eUSBDescriptorLengthDevice;
    return IOT_USB_DEVICE_SUCCESS;
}
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
/* Get device qualifier descriptor request */
int32_t USB_DeviceGetDeviceQualifierDescriptor(
    IotUsbDeviceHandle_t handle, USBDeviceGetDeviceQualifierDescriptorStruct_t *deviceQualifierDescriptor)
{
    deviceQualifierDescriptor->pucBuffer = g_UsbDeviceQualifierDescriptor;
    deviceQualifierDescriptor->ulLength = eUSBDescriptorLengthDeviceQualitier;
    return IOT_USB_DEVICE_SUCCESS;
}
#endif
/* Get device configuration descriptor request */
static int32_t USB_DeviceGetConfigurationDescriptor(
    IotUsbDeviceHandle_t handle, USBDeviceGetConfigurationDescriptorStruct_t *configurationDescriptor)
{
    if (testIotUsbDevice_HID_GENERIC_CONFIGURE_INDEX > configurationDescriptor->ucConfiguration)
    {
        configurationDescriptor->pucBuffer = g_UsbDeviceConfigurationDescriptor;
        configurationDescriptor->ulLength = testIotUsbDevice_DESCRIPTOR_LENGTH_CONFIGURATION_ALL;
        return IOT_USB_DEVICE_SUCCESS;
    }
    return IOT_USB_DEVICE_INVALID_REQUEST;
}

/* Get device string descriptor request */
static int32_t USB_DeviceGetStringDescriptor(IotUsbDeviceHandle_t handle,
                                           USBDeviceGetStringDescriptorStruct_t *stringDescriptor)
{
    if (stringDescriptor->ucStringIndex == 0U)
    {
        stringDescriptor->pucBuffer = (uint8_t *)g_UsbDeviceLanguageList.languageString;
        stringDescriptor->ulLength = g_UsbDeviceLanguageList.stringLength;
    }
    else
    {
        uint8_t languageId = 0U;
        uint8_t languageIndex = testIotUsbDevice_STRING_COUNT;

        for (; languageId < testIotUsbDevice_LANGUAGE_COUNT; languageId++)
        {
            if (stringDescriptor->usLanguageId == g_UsbDeviceLanguageList.languageList[languageId].languageId)
            {
                if (stringDescriptor->ucStringIndex < testIotUsbDevice_STRING_COUNT)
                {
                    languageIndex = stringDescriptor->ucStringIndex;
                }
                break;
            }
        }
        if (testIotUsbDevice_STRING_COUNT == languageIndex)
        {
            return IOT_USB_DEVICE_INVALID_REQUEST;
        }
        stringDescriptor->pucBuffer = (uint8_t *)g_UsbDeviceLanguageList.languageList[languageId].string[languageIndex];
        stringDescriptor->ulLength = g_UsbDeviceLanguageList.languageList[languageId].length[languageIndex];
    }
    return IOT_USB_DEVICE_SUCCESS;
}

/* Get hid descriptor request */
static int32_t USB_DeviceGetHidDescriptor(IotUsbDeviceHandle_t handle, USBDeviceGetHidDescriptorStruct_t *hidDescriptor)
{
    if (testIotUsbDevice_HID_GENERIC_INTERFACE_INDEX == hidDescriptor->ucInterfaceNumber)
    {
        hidDescriptor->pucBuffer =
            &g_UsbDeviceConfigurationDescriptor[eUSBDescriptorLengthConfigure + eUSBDescriptorLengthInterface];
        hidDescriptor->ulLength = testIotUsbDevice_DESCRIPTOR_LENGTH_HID;

    }
    else
    {
        return IOT_USB_DEVICE_INVALID_REQUEST;
    }
    return IOT_USB_DEVICE_SUCCESS;
}

static int32_t USB_DeviceGetHidReportDescriptor(IotUsbDeviceHandle_t handle,
                                              USBDeviceGetHidReportDescriptorStruct_t *hidReportDescriptor)
{
    if (testIotUsbDevice_HID_GENERIC_INTERFACE_INDEX == hidReportDescriptor->ucInterfaceNumber)
    {
        hidReportDescriptor->pucBuffer = g_UsbDeviceHidGenericReportDescriptor;
        hidReportDescriptor->ulLength = testIotUsbDevice_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT;
    }
    else
    {
        return IOT_USB_DEVICE_INVALID_REQUEST;
    }
    return IOT_USB_DEVICE_SUCCESS;
}

/* Get hid physical descriptor request */
static int32_t USB_DeviceGetHidPhysicalDescriptor(IotUsbDeviceHandle_t handle,
                                                USBDeviceGetHidPhysicalDescriptorStruct_t *hidPhysicalDescriptor)
{
    return IOT_USB_DEVICE_INVALID_REQUEST;
}

static int32_t USB_DeviceSetSpeed(IotUsbDeviceHandle_t handle, uint8_t speed)
{
    USBDescriptorUnion_t *descriptorHead;
    USBDescriptorUnion_t *descriptorTail;

    descriptorHead = (USBDescriptorUnion_t *)&g_UsbDeviceConfigurationDescriptor[0];
    descriptorTail =
        (USBDescriptorUnion_t *)(&g_UsbDeviceConfigurationDescriptor[testIotUsbDevice_DESCRIPTOR_LENGTH_CONFIGURATION_ALL - 1U]);

    while (descriptorHead < descriptorTail)
    {
        if (descriptorHead->common.bDescriptorType == eUSBDescriptorTypeEndpoint)
        {
            if (eUSBDeviceSpeedHigh == speed)
            {
                if (((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                    (testIotUsbDevice_HID_GENERIC_ENDPOINT_IN ==
                     (descriptorHead->endpoint.bEndpointAddress & IOT_USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                         (testIotUsbDevice_HID_GENERIC_ENDPOINT_OUT ==
                          (descriptorHead->endpoint.bEndpointAddress & IOT_USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_OUT_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else
                {
                }
            }
            else
            {
                if (((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                    (testIotUsbDevice_HID_GENERIC_ENDPOINT_IN ==
                     (descriptorHead->endpoint.bEndpointAddress & IOT_USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                         (testIotUsbDevice_HID_GENERIC_ENDPOINT_OUT ==
                          (descriptorHead->endpoint.bEndpointAddress & IOT_USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else
                {
                }
            }
        }
        descriptorHead = (USBDescriptorUnion_t *)((uint8_t *)descriptorHead + descriptorHead->common.bLength);
    }

    for (int i = 0U; i < testIotUsbDevice_HID_GENERIC_ENDPOINT_COUNT; i++)
    {
        if (eUSBDeviceSpeedHigh == speed)
        {
            if (g_UsbDeviceHidGenericEndpoints[i].ucEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK)
            {
                g_UsbDeviceHidGenericEndpoints[i].usMaxPacketSize = testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE;
            }
            else
            {
                g_UsbDeviceHidGenericEndpoints[i].usMaxPacketSize = testIotUsbDevice_HS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE;
            }
        }
        else
        {
            if (g_UsbDeviceHidGenericEndpoints[i].ucEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK)
            {
                g_UsbDeviceHidGenericEndpoints[i].usMaxPacketSize = testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE;
            }
            else
            {
                g_UsbDeviceHidGenericEndpoints[i].usMaxPacketSize = testIotUsbDevice_FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE;
            }
        }
    }

    return IOT_USB_DEVICE_SUCCESS;
}

static int32_t USB_DeviceHidGenericCallback(class_handle_t handle, uint32_t event, void *param);
static int32_t USB_DeviceCallback(IotUsbDeviceHandle_t handle, uint32_t event, void *param);
static uint32_t s_GenericBuffer0[testIotUsbDevice_HID_GENERIC_OUT_BUFFER_LENGTH];
static uint32_t s_GenericBuffer1[testIotUsbDevice_HID_GENERIC_OUT_BUFFER_LENGTH];

usb_hid_generic_struct_t g_UsbDeviceHidGeneric;
extern USBDeviceClassStruct_t g_UsbDeviceHidGenericConfig;
/* Set class configurations */
USBDeviceClassConfigStruct_t g_UsbDeviceHidConfig[1] = {{
    USB_DeviceHidGenericCallback, /* HID generic class callback pointer */
    (class_handle_t)NULL,         /* The HID class handle, This field is set by USB_DeviceClassInit */
    &g_UsbDeviceHidGenericConfig, /* The HID mouse configuration, including class code, subcode, and protocol, class
                             type,
                             transfer type, endpoint address, max packet size, etc.*/
}};

/* Set class configuration list */
USBDeviceClassConfigListStruct_t g_UsbDeviceHidConfigList = {
    g_UsbDeviceHidConfig, /* Class configurations */
    USB_DeviceCallback,   /* Device callback pointer */
    1U,                   /* Class count */
};

#define testIotUSBDevice_CANCEL_TEST_STR_SIZE (10240)
#define testIotUSBDevice_TEST_WAIT_TIME (1000) /* in ms */
static uint8_t testStringWrite[] = "send from device\r\n";
static uint8_t testStringRead[] ="receive from host\r\n";
static uint8_t testStringCancelTransfer[testIotUSBDevice_CANCEL_TEST_STR_SIZE] = {0};
uint8_t uctestIotUsbDeviceControllerId = 6;
static uint8_t uctestIotSendComplete = 0;
static SemaphoreHandle_t xtestIotUsbDeviceSemaphore = NULL;

static void USB_DeviceWriteAsyncTest(void)
{
    int32_t error = IOT_USB_DEVICE_ERROR;
    int32_t lRetVal;
    /* write a big string to host and check if write complete.*/
    error = iot_usb_device_write_async(g_UsbDeviceHidGeneric.deviceHandle, testIotUsbDevice_HID_GENERIC_ENDPOINT_IN, testStringWrite, sizeof(testStringWrite));
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS );
    lRetVal = xSemaphoreTake( xtestIotUsbDeviceSemaphore, testIotUSBDevice_TEST_WAIT_TIME);
    TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
}

/* usb host needs to send one string first by using CLI, then usb device to read and compare */
static void USB_DeviceReadAsyncTest(void)
{
    int32_t error = IOT_USB_DEVICE_ERROR;
    int32_t lRetVal;
    error = iot_usb_device_read_async(g_UsbDeviceHidGeneric.deviceHandle, testIotUsbDevice_HID_GENERIC_ENDPOINT_OUT, (uint8_t *)&g_UsbDeviceHidGeneric.buffer[g_UsbDeviceHidGeneric.bufferIndex][0], testIotUsbDevice_HID_GENERIC_OUT_BUFFER_LENGTH);
    lRetVal = xSemaphoreTake( xtestIotUsbDeviceSemaphore, portMAX_DELAY);
    TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    TEST_ASSERT_EQUAL_STRING( testStringRead, &g_UsbDeviceHidGeneric.buffer[g_UsbDeviceHidGeneric.bufferIndex][0]);
}

static void USB_DeviceWriteSyncTest(void)
{
    int32_t error = IOT_USB_DEVICE_ERROR;
    error = iot_usb_device_write_sync(g_UsbDeviceHidGeneric.deviceHandle, testIotUsbDevice_HID_GENERIC_ENDPOINT_IN, testStringWrite, sizeof(testStringWrite));
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
}

/* usb host needs to send one string first by using CLI, then usb device to read and compare */
static void USB_DeviceReadSyncTest(void)
{
    int32_t error = IOT_USB_DEVICE_ERROR;
    error = iot_usb_device_read_sync(g_UsbDeviceHidGeneric.deviceHandle, testIotUsbDevice_HID_GENERIC_ENDPOINT_OUT, (uint8_t *)&g_UsbDeviceHidGeneric.buffer[g_UsbDeviceHidGeneric.bufferIndex][0], testIotUsbDevice_HID_GENERIC_OUT_BUFFER_LENGTH);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    TEST_ASSERT_EQUAL_STRING( testStringRead, &g_UsbDeviceHidGeneric.buffer[g_UsbDeviceHidGeneric.bufferIndex][0]);
}

static void USB_DeviceCancelTransferTest(void)
{
    uctestIotSendComplete = 0;
    int32_t error = IOT_USB_DEVICE_ERROR;
    for (int i=0; i<testIotUSBDevice_CANCEL_TEST_STR_SIZE; i++)
    {
        testStringCancelTransfer[i] = i;
    }

    /* write a string to host and check the send complete flag from callback.*/
    error = iot_usb_device_write_async(g_UsbDeviceHidGeneric.deviceHandle, testIotUsbDevice_HID_GENERIC_ENDPOINT_IN, testStringCancelTransfer, sizeof(testStringCancelTransfer));
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    /* cancel  transfer */
    error = iot_usb_device_endpoint_cancel_transfer(g_UsbDeviceHidGeneric.deviceHandle, testIotUsbDevice_HID_GENERIC_ENDPOINT_IN);
    if (error != IOT_USB_DEVICE_FUNCTION_NOT_SUPPORTED)
    {
        TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    }
}

static void USB_DeviceIoctlTest(void)
{
    int32_t error = IOT_USB_DEVICE_ERROR;
    uint8_t state;
    /* test eUSBDeviceGetState */
    error = iot_usb_device_ioctl(g_UsbDeviceHidGeneric.deviceHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    TEST_ASSERT_EQUAL( state, eUsbDeviceStateConfigured);
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* test eUSBDeviceSetState */
    state = eUsbDeviceStateTestMode;
    error = iot_usb_device_ioctl(g_UsbDeviceHidGeneric.deviceHandle, eUSBDeviceSetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    error = iot_usb_device_ioctl(g_UsbDeviceHidGeneric.deviceHandle, eUSBDeviceGetState, &state);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    TEST_ASSERT_EQUAL( state, eUsbDeviceStateTestMode);
}

static void USB_DeviceStallUnstallTest(void)
{
    int32_t error = IOT_USB_DEVICE_ERROR;
    /* stall ep */
    error = iot_usb_device_endpoint_stall(g_UsbDeviceHidGeneric.deviceHandle, testIotUsbDevice_HID_GENERIC_ENDPOINT_IN);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);

    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* check ep status */
    IotUsbDeviceEndpointInfo_t endpointInfo;
    endpointInfo.ucEndpointAddress = testIotUsbDevice_HID_GENERIC_ENDPOINT_IN;
    endpointInfo.usEndpointStatus = eUsbDeviceEndpointStateIdle;
    error = iot_usb_device_ioctl(g_UsbDeviceHidGeneric.deviceHandle, eUSBDeviceGetEndpointStatus, &endpointInfo);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    TEST_ASSERT_EQUAL(endpointInfo.usEndpointStatus, eUsbDeviceEndpointStateStalled);

    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* unstall the endpoint */
    error = iot_usb_device_endpoint_unstall(g_UsbDeviceHidGeneric.deviceHandle, testIotUsbDevice_HID_GENERIC_ENDPOINT_IN);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);

    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* check ep status */
    error = iot_usb_device_ioctl(g_UsbDeviceHidGeneric.deviceHandle, eUSBDeviceGetEndpointStatus, &endpointInfo);
    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    TEST_ASSERT_EQUAL(endpointInfo.usEndpointStatus, eUsbDeviceEndpointStateIdle);
}

static void USB_DeviceGetSpeedTest(void)
{
    uint8_t speed;
    int32_t error = IOT_USB_DEVICE_ERROR;

    error = iot_usb_device_ioctl(g_UsbDeviceHidGeneric.deviceHandle, eUSBDeviceGetSpeed, &speed);

    TEST_ASSERT_EQUAL( error, IOT_USB_DEVICE_SUCCESS);
    TEST_ASSERT_EQUAL( speed, eUSBDeviceSpeedHigh);
}

static int32_t USB_DeviceHidGenericCallback(class_handle_t handle, uint32_t event, void *param)
{
    int32_t error = IOT_USB_DEVICE_ERROR;
    BaseType_t xHigherPriorityTaskWoken;

    switch (event) {
        case eUSBDeviceHidEventSendResponse:
            uctestIotSendComplete = 1;
            xSemaphoreGiveFromISR(xtestIotUsbDeviceSemaphore, &xHigherPriorityTaskWoken);
            break;
        case eUSBDeviceHidEventRecvResponse:
            xSemaphoreGiveFromISR(xtestIotUsbDeviceSemaphore, &xHigherPriorityTaskWoken);
            break;
        case eUSBDeviceHidEventGetReport:
            error = IOT_USB_DEVICE_INVALID_REQUEST;
            break;
        case eUSBDeviceHidEventSetReport:
        case eUSBDeviceHidEventRequestReportBuffer:
            error = IOT_USB_DEVICE_INVALID_REQUEST;
            break;
        case eUSBDeviceHidEventGetIdle:
        case eUSBDeviceHidEventGetProtocol:
            break;
        case eUSBDeviceHidEventSetIdle:
            error = IOT_USB_DEVICE_SUCCESS;
            break;
        case eUSBDeviceHidEventSetProtocol:
            break;
        default:
            break;
    }

    return error;
}

static int32_t USB_DeviceCallback(IotUsbDeviceHandle_t handle, uint32_t event, void *param)
{
    int32_t error = IOT_USB_DEVICE_SUCCESS;
    uint8_t *temp8 = (uint8_t *)param;
    uint16_t *temp16 = (uint16_t *)param;

    switch (event)
    {
        case eUSBDeviceBusResetEvent:
        {
            /* USB bus reset signal detected */
            g_UsbDeviceHidGeneric.attach = 0U;
            g_UsbDeviceHidGeneric.currentConfiguration = 0U;
            /* Get USB speed to configure the device, including max packet size and interval of the endpoints. */
            if (IOT_USB_DEVICE_SUCCESS == usb_device_class_get_speed(uctestIotUsbDeviceControllerId, &g_UsbDeviceHidGeneric.speed))
            {
                USB_DeviceSetSpeed(handle, g_UsbDeviceHidGeneric.speed);
            }
        }
        break;
        case eUSBDeviceSetConfigurationEvent:
            if (0U == (*temp8))
            {
                g_UsbDeviceHidGeneric.attach = 0U;
                g_UsbDeviceHidGeneric.currentConfiguration = 0U;
            }
            else if (testIotUsbDevice_HID_GENERIC_CONFIGURE_INDEX == (*temp8))
            {
                /* Set device configuration request */
                g_UsbDeviceHidGeneric.attach = 1U;
                g_UsbDeviceHidGeneric.currentConfiguration = *temp8;
            }
            else
            {
                error = IOT_USB_DEVICE_INVALID_REQUEST;
            }
            break;
        case eUSBDeviceSetInterfaceEvent:
            if (g_UsbDeviceHidGeneric.attach)
            {
                /* Set device interface request */
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);
                if (interface < testIotUsbDevice_HID_GENERIC_INTERFACE_COUNT)
                {
                    g_UsbDeviceHidGeneric.currentInterfaceAlternateSetting[interface] = alternateSetting;
                }
            }
            break;
        case eUSBDeviceGetConfigurationEvent:
            if (param)
            {
                /* Get current configuration request */
                *temp8 = g_UsbDeviceHidGeneric.currentConfiguration;
                error = IOT_USB_DEVICE_SUCCESS;
            }
            break;
        case eUSBDeviceGetInterfaceEvent:
            if (param)
            {
                /* Get current alternate setting of the interface request */
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                if (interface < testIotUsbDevice_HID_GENERIC_INTERFACE_COUNT)
                {
                    *temp16 = (*temp16 & 0xFF00U) | g_UsbDeviceHidGeneric.currentInterfaceAlternateSetting[interface];
                    error = IOT_USB_DEVICE_SUCCESS;
                }
                else
                {
                    error = IOT_USB_DEVICE_INVALID_REQUEST;
                }
            }
            break;
        case eUSBDeviceGetDeviceDescriptorEvent:
            if (param)
            {
                /* Get device descriptor request */
                error = USB_DeviceGetDeviceDescriptor(handle, (USBDeviceGetDeviceDescriptorStruct_t *)param);
            }
            break;
        case eUSBDeviceGetConfigurationDescriptorEvent:
            if (param)
            {
                /* Get device configuration descriptor request */
                error = USB_DeviceGetConfigurationDescriptor(handle,
                                                             (USBDeviceGetConfigurationDescriptorStruct_t *)param);
            }
            break;
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
        case eUSBDeviceGetDeviceQualifierDescriptorEvent:
            if (param)
            {
                /* Get device descriptor request */
                error = USB_DeviceGetDeviceQualifierDescriptor(
                    handle, (USBDeviceGetDeviceQualifierDescriptorStruct_t *)param);
            }
            break;
#endif
        case eUSBDeviceGetStringDescriptorEvent:
            if (param)
            {
                /* Get device string descriptor request */
                error = USB_DeviceGetStringDescriptor(handle, (USBDeviceGetStringDescriptorStruct_t *)param);
            }
            break;
        case eUSBDeviceGetHidDescriptorEvent:
            if (param)
            {
                /* Get hid descriptor request */
                error = USB_DeviceGetHidDescriptor(handle, (USBDeviceGetHidDescriptorStruct_t *)param);
            }
            break;
        case eUSBDeviceGetHidReportDescriptorEvent:
            if (param)
            {
                /* Get hid report descriptor request */
                error =
                    USB_DeviceGetHidReportDescriptor(handle, (USBDeviceGetHidReportDescriptorStruct_t *)param);
            }
            break;
        case eUSBDeviceGetHidPhysicalDescriptorEvent:
            if (param)
            {
                /* Get hid physical descriptor request */
                error = USB_DeviceGetHidPhysicalDescriptor(handle,
                                                           (USBDeviceGetHidPhysicalDescriptorStruct_t *)param);
            }
            break;
        default:
            break;
    }

    return error;
}

static void USB_DeviceHidAppInit(void)
{
    /* Set HID generic to default state */
    g_UsbDeviceHidGeneric.speed = eUSBDeviceSpeedHigh;
    g_UsbDeviceHidGeneric.attach = 0U;
    g_UsbDeviceHidGeneric.hidHandle = (class_handle_t)NULL;
    g_UsbDeviceHidGeneric.deviceHandle = NULL;
    g_UsbDeviceHidGeneric.buffer[0] = (uint8_t *)&s_GenericBuffer0[0];
    g_UsbDeviceHidGeneric.buffer[1] = (uint8_t *)&s_GenericBuffer1[0];

    if (IOT_USB_DEVICE_SUCCESS !=
        usb_device_class_init(uctestIotUsbDeviceControllerId, &g_UsbDeviceHidConfigList, &g_UsbDeviceHidGeneric.deviceHandle))
    {
        return ;
    }
    else
    {
        g_UsbDeviceHidGeneric.hidHandle = g_UsbDeviceHidConfigList.pxConfig->xClassHandle;
    }

    iot_usb_device_connect(g_UsbDeviceHidGeneric.deviceHandle);
}

static void USB_DeviceHidAppDeinit(void)
{
    iot_usb_device_disconnect(g_UsbDeviceHidGeneric.deviceHandle);
    g_UsbDeviceHidGeneric.hidHandle = (class_handle_t)NULL;
    g_UsbDeviceHidGeneric.attach = 0U;
    usb_device_class_deinit(uctestIotUsbDeviceControllerId);
    for (int i=0; i<testIotUsbDevice_HID_GENERIC_OUT_BUFFER_LENGTH; i++)
    {
        s_GenericBuffer0[i] = 0;
        s_GenericBuffer1[i] = 0;
    }
}

/* Define Test Group. */
TEST_GROUP( TEST_IOT_USB_DEVICE );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_USB_DEVICE )
{
    xtestIotUsbDeviceSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotUsbDeviceSemaphore);
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_USB_DEVICE )
{
}

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_USB_DEVICE )
{
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceOpenClose );
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceOpenOpenClose );
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceOpenCloseClose );
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceHidAttach );
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceHidDetach);
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceEndpointStallUnstall);
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceGetSpeed);
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceWriteAsync);
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceReadAsync);
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceWriteSync);
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceReadSync);
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceEndpointCancelTransfer);
    RUN_TEST_CASE( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceIoctl);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_usb_device_open and iot_usb_device_close.
 *
 */
TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceOpenClose )
{
    IotUsbDeviceHandle_t xUsbDeviceHandle = NULL;
    int32_t lRetVal;

    /* Open usb_device to initialize hardware. */
    xUsbDeviceHandle = iot_usb_device_open(uctestIotUsbDeviceControllerId);
    TEST_ASSERT_NOT_EQUAL( NULL, xUsbDeviceHandle );

    /* Close usb_device to deinit hardware. */
    lRetVal = iot_usb_device_close( xUsbDeviceHandle );
    TEST_ASSERT_EQUAL( IOT_USB_DEVICE_SUCCESS, lRetVal);

    /* Test close NULL handle */
    lRetVal = iot_usb_device_close( NULL );
    TEST_ASSERT_EQUAL( IOT_USB_DEVICE_INVALID_VALUE, lRetVal);
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test duplicate iot_usb_device_open and single iot_usb_device_close.
 */
TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceOpenOpenClose )
{
    IotUsbDeviceHandle_t xUsbDeviceHandle1 = NULL;
    IotUsbDeviceHandle_t xUsbDeviceHandle2 = NULL;
    int32_t lRetVal;

    /* Open usb_device to initialize hardware. */
    xUsbDeviceHandle1 = iot_usb_device_open(uctestIotUsbDeviceControllerId);
    TEST_ASSERT_NOT_EQUAL( NULL, xUsbDeviceHandle1 );

    xUsbDeviceHandle2 = iot_usb_device_open(uctestIotUsbDeviceControllerId);
    TEST_ASSERT_EQUAL( NULL, xUsbDeviceHandle2 );

    /* Close usb_device to deinit hardware. */
    lRetVal = iot_usb_device_close( xUsbDeviceHandle1 );
    TEST_ASSERT_EQUAL( IOT_USB_DEVICE_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test single iot_usb_device_open and duplicate iot_usb_device_close.
 */
TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceOpenCloseClose )
{
    IotUsbDeviceHandle_t xUsbDeviceHandle = NULL;
    int32_t lRetVal;

    /* Open usb_device to initialize hardware. */
    xUsbDeviceHandle = iot_usb_device_open(uctestIotUsbDeviceControllerId);
    TEST_ASSERT_NOT_EQUAL( NULL, xUsbDeviceHandle );

    /* Close usb_device to deinit hardware. */
    lRetVal = iot_usb_device_close( xUsbDeviceHandle );
    TEST_ASSERT_EQUAL( IOT_USB_DEVICE_SUCCESS, lRetVal);

    lRetVal = iot_usb_device_close( xUsbDeviceHandle );
    TEST_ASSERT_EQUAL( IOT_USB_DEVICE_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test hid class.
 */
TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceHidAttach)
{
    USB_DeviceHidAppInit();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to detach device from host
 */
static void USB_DeviceHidAppDetach(void)
{
    USB_DeviceHidAppDeinit();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test hid class.
 */
TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceHidDetach)
{
    USB_DeviceHidAppDetach();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test endpoint stall and unstall.
 */
TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceEndpointStallUnstall)
{
    USB_DeviceHidAppInit();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    USB_DeviceStallUnstallTest();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* detach for next test */
    USB_DeviceHidAppDetach();

}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test getspeed.
 */
TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceGetSpeed)
{
    USB_DeviceHidAppInit();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    USB_DeviceGetSpeedTest();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* detach for next test */
    USB_DeviceHidAppDetach();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test write and read async.
 */

TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceWriteAsync)
{
    USB_DeviceHidAppInit();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    USB_DeviceWriteAsyncTest();
    /* detach for next test */
    USB_DeviceHidAppDetach();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test read async.
 */
TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceReadAsync)
{
    USB_DeviceHidAppInit();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    USB_DeviceReadAsyncTest();
    /* detach for next test */
    USB_DeviceHidAppDetach();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test write sync.
 */

TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceWriteSync)
{
    USB_DeviceHidAppInit();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    USB_DeviceWriteSyncTest();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* detach for next test */
    USB_DeviceHidAppDetach();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test read sync.
 */

TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceReadSync)
{
    USB_DeviceHidAppInit();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    USB_DeviceReadSyncTest();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* detach for next test */
    USB_DeviceHidAppDetach();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test iot_usb_device_endpoint_cancel_transfer.
 */

TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceEndpointCancelTransfer)
{
    USB_DeviceHidAppInit();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME*2);
    USB_DeviceCancelTransferTest();
    TEST_ASSERT_EQUAL( uctestIotSendComplete, 0);
    /* detach for next test */
    USB_DeviceHidAppDetach();
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to test ioctl. Some ioctl test is already included somewhere else,
 * e.g. eUSBDeviceGetSpeed, eUSBDeviceGetEndpointStatus, not tested here.
 */

TEST( TEST_IOT_USB_DEVICE, AFQP_IotUsbDeviceIoctl)
{
    USB_DeviceHidAppInit();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    USB_DeviceIoctlTest();
    vTaskDelay(testIotUSBDevice_TEST_WAIT_TIME);
    /* detach for next test */
    USB_DeviceHidAppDetach();
}
