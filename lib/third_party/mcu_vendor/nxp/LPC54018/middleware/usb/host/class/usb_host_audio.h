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

#ifndef __USB_HOST_AUDIO_H__
#define __USB_HOST_AUDIO_H__

/*******************************************************************************
 * Audio class private structure, enumerations, macros
 ******************************************************************************/
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Structure for an AUDIO class descriptor according to the 6.2.1 in Audio specification*/
#define AUDIO_FU_MUTE 0x01
#define AUDIO_FU_VOLUME 0x02
#define AUDIO_FU_BASS 0x03
#define AUDIO_FU_MID 0x04
#define AUDIO_FU_TREBLE 0x05
#define AUDIO_FU_GRAPHIC_EQ 0x06
#define AUDIO_FU_AGC 0x07
#define AUDIO_FU_DELAY 0x08
#define AUDIO_FU_BASS_BOOST 0x09

/* Audio class codes */
#define SET_COMMAND (0x00)
#define GET_COMMAND (0x80)
#define CUR_REQUEST (0x01)
#define MIN_REQUEST (0x02)
#define MAX_REQUEST (0x03)
#define RES_REQUEST (0x04)
#define MEM_REQUEST (0x05)
#define GET_STATUS (0xFF)

#define ITF_REQUEST (0x21)
#define EP_REQUEST (0x22)

#define AUDIO_FU_MUTE_MASK 0x01
#define AUDIO_FU_VOLUME_MASK 0x02
#define AUDIO_FU_BASS_MASK 0x04
#define AUDIO_FU_MID_MASK 0x08
#define AUDIO_FU_TREBLE_MASK 0x10
#define AUDIO_FU_GRAPHIC_EQ_MASK 0x20
#define AUDIO_FU_AGC_MASK 0x40
#define AUDIO_FU_DELAY_MASK 0x80
#define AUDIO_FU_BASS_BOOST_MASK 0x01

/* USB audio Endpoint Control Selectors */
#define AUDIO_EP_CONTROL_UNDEFINED (0x00)
#define AUDIO_SAMPLING_FREQ_CONTROL (0x01)
#define AUDIO_PITCH_CONTROL (0x02)

#define AUDIO_SAMPLING_FREQ_MASK (0x01)
#define AUDIO_PITCH_MASK (0x02)
typedef enum _fu_request_code
{
    kUSB_AudioCurMute = 0,
    kUSB_AudioCurVolume,
    kUSB_AudioMinVolume,
    kUSB_AudioMaxVolume,
    kUSB_AudioResVolume,
    NUMBER_OF_FEATURE_COMMANDS,
} fu_request_code_t;

typedef enum _ep_request_code
{
    kUSB_AudioCurPitch = 0,
    kUSB_AudioCurSamplingFreq,
    kUSB_AudioMinSamplingFreq,
    kUSB_AudioMaxSamplingFreq,
    kUSB_AudioResSamplingFreq,
    NUMBER_OF_ENDPOINT_COMMANDS,
} ep_request_code_t;

typedef union _audio_descriptor_union
{
    uint8_t *bufr;
    usb_descriptor_common_t *common;
    usb_descriptor_device_t *device;
    usb_descriptor_configuration_t *configuration;
    usb_descriptor_interface_t *interface;
    usb_descriptor_endpoint_t *endpoint;
} audio_descriptor_union_t;

/* Audio command structure */
typedef struct _usb_audio_request
{
    uint8_t controlMask;
    uint8_t typeRequest;
    uint8_t codeRequest;
    uint8_t requestValue;
    uint8_t length;
} usb_audio_request_t;

#define USB_DESC_SUBTYPE_AUDIO_CS_HEADER 0x01
#define USB_DESC_SUBTYPE_AUDIO_CS_IT 0x02
#define USB_DESC_SUBTYPE_AUDIO_CS_OT 0x03
#define USB_DESC_SUBTYPE_AUDIO_CS_FU 0x06
#define USB_DESC_CLASS_ENDPOINT_GENERAL 0x01
#define USB_DESC_SUBTYPE_AS_CS_GENERAL 0X01
#define USB_DESC_SUBTYPE_AS_CS_FORMAT_TYPE 0X02

/*******************************************************************************
 * Audio class public structure, enumeration, macros, functions
 ******************************************************************************/
/*!
 * @addtogroup usb_host_audio_drv
 * @{
 */
/*! @brief Audio class code */
#define USB_AUDIO_CLASS_CODE 1
/*! @brief Audio class  control interface code*/
#define USB_AUDIO_SUBCLASS_CODE_CONTROL 1
/*! @brief Audio class  stream interface code*/
#define USB_AUDIO_SUBCLASS_CODE_AUDIOSTREAMING 2

/*! @brief AUDIO class-specific feature unit get current mute command*/
#define USB_AUDIO_GET_CUR_MUTE 0x80
/*! @brief AUDIO class-specific feature unit set current mute command*/
#define USB_AUDIO_SET_CUR_MUTE 0x00
/*! @brief AUDIO class-specific feature unit get current volume command*/
#define USB_AUDIO_GET_CUR_VOLUME 0x81
/*! @brief AUDIO class-specific feature unit set current volume command*/
#define USB_AUDIO_SET_CUR_VOLUME 0x01
/*! @brief AUDIO class-specific feature unit get minimum volume command*/
#define USB_AUDIO_GET_MIN_VOLUME 0x82
/*! @brief AUDIO class-specific feature unit set minimum volume command*/
#define USB_AUDIO_SET_MIN_VOLUME 0x02
/*! @brief AUDIO class-specific feature unit get maximum volume command*/
#define USB_AUDIO_GET_MAX_VOLUME 0x83
/*! @brief AUDIO class-specific feature unit set maximum volume command*/
#define USB_AUDIO_SET_MAX_VOLUME 0x03
/*! @brief AUDIO class-specific feature unit get resolution volume command*/
#define USB_AUDIO_GET_RES_VOLUME 0x84
/*! @brief AUDIO class-specific feature unit set resolution volume command*/
#define USB_AUDIO_SET_RES_VOLUME 0x04

/*! @brief AUDIO class-specific endpoint get current pitch control command*/
#define USB_AUDIO_GET_CUR_PITCH 0x80
/*! @brief AUDIO class-specific endpoint set current pitch control command*/
#define USB_AUDIO_SET_CUR_PITCH 0x00
/*! @brief AUDIO class-specific endpoint get current sampling frequency command*/
#define USB_AUDIO_GET_CUR_SAMPLING_FREQ 0x81
/*! @brief AUDIO class-specific endpoint set current sampling frequency command*/
#define USB_AUDIO_SET_CUR_SAMPLING_FREQ 0x01
/*! @brief AUDIO class-specific endpoint get minimum sampling frequency command*/
#define USB_AUDIO_GET_MIN_SAMPLING_FREQ 0x82
/*! @brief AUDIO class-specific endpoint set minimum sampling frequency command*/
#define USB_AUDIO_SET_MIN_SAMPLING_FREQ 0x02
/*! @brief AUDIO class-specific endpoint get maximum sampling frequency command*/
#define USB_AUDIO_GET_MAX_SAMPLING_FREQ 0x83
/*! @brief AUDIO class-specific endpoint set maximum sampling frequency command*/
#define USB_AUDIO_SET_MAX_SAMPLING_FREQ 0x03
/*! @brief AUDIO class-specific endpoint get resolution sampling frequency command*/
#define USB_AUDIO_GET_RES_SAMPLING_FREQ 0x84
/*! @brief AUDIO class-specific endpoint set resolution sampling frequency command*/
#define USB_AUDIO_SET_RES_SAMPLING_FREQ 0x04

/*! @brief Audio control interface header descriptor structure */
typedef struct _usb_audio_ctrl_header_desc
{
    uint8_t blength;            /*!< Total size of the header descriptor*/
    uint8_t bdescriptortype;    /*!< Descriptor type of audio header descriptor*/
    uint8_t bdescriptorsubtype; /*!< Subtype of an audio header descriptor*/
    uint8_t bcdcdc[2];          /*!< Audio Device Class Specification Release Number in Binary-Coded Decimal*/
    uint8_t wtotallength[2];    /*!< Total number of bytes returned for the class-specific AudioControl interface
                                   descriptor. Includes the combined length of this descriptor header and all unit and
                                   terminal descriptors.*/
    uint8_t bincollection;      /*!< The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface
                                   Collection to which this AudioControl interface belongs to*/
} usb_audio_ctrl_header_desc_t;

/*! @brief Audio control interface input terminal descriptor structure */
typedef struct _usb_audio_ctrl_it_desc
{
    uint8_t blength;            /*!< Total size of the input terminal descriptor*/
    uint8_t bdescriptortype;    /*!< Descriptor type of audio input terminal descriptor*/
    uint8_t bdescriptorsubtype; /*!< Subtype of audio input terminal descriptor*/
    uint8_t bterminalid; /*!< Constant uniquely identifying the Terminal within the audio function. This value is used
                            in all requests to address this Terminal*/
    uint8_t wterminaltype[2];  /*!< Constant characterizing the type of Terminal*/
    uint8_t bassocterminal;    /*!< ID of the Output Terminal to which this Input Terminal is associated*/
    uint8_t bnrchannels;       /*!< Number of logical output channels in the Terminal's output audio channel cluster*/
    uint8_t wchannelconfig[2]; /*!< Describes the spatial location of the logical channels.*/
    uint8_t ichannelnames;     /*!< Index of a string descriptor, describing the name of the first logical channel*/
    uint8_t iterminal;         /*!<Index of a string descriptor, describing the Input Terminal*/
} usb_audio_ctrl_it_desc_t;

/*! @brief Audio control interface output terminal descriptor structure */
typedef struct _usb_audio_ctrl_ot_desc
{
    uint8_t blength;            /*!< Total size of the output terminal descriptor*/
    uint8_t bdescriptortype;    /*!< Descriptor type of audio output terminal descriptor*/
    uint8_t bdescriptorsubtype; /*!< Subtype of audio output terminal descriptor*/
    uint8_t bterminalid; /*!< Constant uniquely identifying the Terminal within the audio function. This value is used
                            in all requests to address this Terminal*/
    uint8_t wterminaltype[2]; /*!< Constant characterizing the type of Terminal*/
    uint8_t bassocterminal;   /*!< Constant, identifying the Input Terminal to which this Output Terminal is associated*/
    uint8_t bsourceid;        /*!< ID of the Unit or Terminal to which this Terminal is connected*/
    uint8_t iterminal;        /*!< Index of a string descriptor, describing the Output Terminal*/
} usb_audio_ctrl_ot_desc_t;

/*! @brief Audio control interface feature unit descriptor structure */
typedef struct _usb_audio_ctrl_fu_desc
{
    uint8_t blength;            /*!< Total size of the output terminal descriptor*/
    uint8_t bdescriptortype;    /*!< Descriptor type of audio output terminal descriptor*/
    uint8_t bdescriptorsubtype; /*!< Subtype of audio output terminal descriptor*/
    uint8_t bunitid;   /*!< Constant uniquely identifying the unit within the audio function. This value is used in all
                          requests to address this unit*/
    uint8_t bsourceid; /*!< ID of the Unit or Terminal to which this Feature Unit is connected*/
    uint8_t bcontrolsize; /*!< Size in bytes of an element of the bmaControls*/
} usb_audio_ctrl_fu_desc_t;

/*! @brief Audio as isochronous audio data endpoint descriptor structure */
typedef struct _usb_audio_stream_specific_iso_endp_desc
{
    uint8_t blength;            /*!< Total size of the descriptor*/
    uint8_t bdescriptortype;    /*!< Descriptor type of the descriptor*/
    uint8_t bdescriptorsubtype; /*!< Subtype of the descriptor*/
    uint8_t bmattributes;   /*!< A bit in the range D6..0 set to 1 indicates that the mentioned Control is supported by
                               this endpoint*/
    uint8_t blockdlayunits; /*!< Indicates the units used for the wLockDelay field*/
    uint8_t wlockdelay[2];  /*!< Indicates the time it takes this endpoint to reliably lock its internal clock recovery
                               circuitry. Units used depend on the value of the bLockDelayUnits field.*/
} usb_audio_stream_specific_iso_endp_desc_t;

/*! @brief Audio standard as isochronous synch endpoint descriptor structure */
typedef struct _usb_audio_stream_synch_endp_desc
{
    uint8_t blength;           /*!< Total size of the descriptor*/
    uint8_t bdescriptortype;   /*!< Descriptor type of the endpoint descriptor*/
    uint8_t bendpointaddress;  /*!< The address of the endpoint on the USB device described by this descriptor*/
    uint8_t bmattributes;      /*!< D3..2: Synchronization type, D1..0: Transfer type*/
    uint8_t wmaxpacketsize[2]; /*!< Maximum packet size this endpoint is capable of sending or receiving when this
                                  configuration is selected*/
    uint8_t binterval;         /*!< Interval for polling endpoint for data transfers expressed in milliseconds*/
    uint8_t brefresh;      /*!< This field indicates the rate at which an isochronous synchronization pipe provides new
                              synchronization feedback data*/
    uint8_t bsynchaddress; /*!< Must be reset to zero*/
} usb_audio_stream_synch_endp_desc_t;

/*! @brief Audio class-specific as interface descriptor structure */
typedef struct _usb_audio_stream_spepific_as_intf_desc
{
    uint8_t blength;            /*!< Total size of the descriptor*/
    uint8_t bdescriptortype;    /*!< Descriptor type of the descriptor*/
    uint8_t bdescriptorsubtype; /*!< Subtype of the descriptor*/
    uint8_t bterminallink; /*!< The Terminal ID of the Terminal to which the endpoint of this interface is connected*/
    uint8_t bdelay;        /*!< Expressed in number of frames*/
    uint8_t wformattag[2]; /*!< The Audio Data Format that has to be used to communicate with this interface*/
} usb_audio_stream_spepific_as_intf_desc_t;

/* Format type descriptor */
/*! @brief audio  Format type descriptor structure */
typedef struct _usb_audio_stream_format_type_desc
{
    uint8_t blength;            /*!< Total size of the descriptor*/
    uint8_t bdescriptortype;    /*!< Descriptor type of the descriptor*/
    uint8_t bdescriptorsubtype; /*!< Subtype of the descriptor*/
    uint8_t bformattype;        /*!< Constant identifying the Format Type the AudioStreaming interface is using*/
    uint8_t bnrchannels;        /*!< Number of channels of device*/
    uint8_t bsubframesize;      /*!< Bytes per audio subframe*/
    uint8_t bbitresolution;     /*!< Bits per sample*/
    uint8_t bsamfreqtype;       /*!< Frequency supported*/
    uint8_t tsamfreq[1][3];     /*!< Sample frequency*/
} usb_audio_stream_format_type_desc_t;

/*! @brief Audio instance structure and audio usb_host_class_handle pointer to this structure */
typedef struct _audio_instance
{
    usb_host_handle hostHandle;                  /*!< This instance's related host handle*/
    usb_device_handle deviceHandle;              /*!< This instance's related device handle*/
    usb_host_interface_handle streamIntfHandle;  /*!< This instance's audio stream interface handle*/
    usb_host_interface_handle controlIntfHandle; /*!< This instance's control stream interface handle*/
    usb_audio_stream_spepific_as_intf_desc_t
        *asIntfDesc; /*!< Audio class class-specific as interface descriptor pointer*/
    usb_audio_stream_format_type_desc_t
        *formatTypeDesc; /*!< Audio class class-specific format type  descriptor pointer*/
    usb_audio_stream_specific_iso_endp_desc_t
        *isoEndpDesc;                  /*!< Audio class class-specific ISO audio data endpoint descriptor pointer*/
    usb_host_pipe_handle isoInPipe;    /*!< Audio class ISO in pipe*/
    usb_host_pipe_handle isoOutPipe;   /*!< Audio class ISO out pipe*/
    transfer_callback_t inCallbackFn;  /*!< Audio class ISO in transfer callback function*/
    void *inCallbackParam;             /*!< Audio class ISO in transfer callback parameter*/
    transfer_callback_t outCallbackFn; /*!< Audio class ISO out transfer callback function*/
    void *outCallbackParam;            /*!< Audio class ISO out transfer callback function*/
    usb_audio_ctrl_header_desc_t *headerDesc; /*!< Audio class header descriptor pointer*/
    usb_audio_ctrl_it_desc_t *itDesc;         /*!< Audio class input terminal descriptor pointer*/
    usb_audio_ctrl_ot_desc_t *otDesc;         /*!< Audio class output terminal descriptor pointer*/
    usb_audio_ctrl_fu_desc_t *fuDesc;         /*!< Audio class  feature unit descriptor pointer*/
    usb_host_pipe_handle controlPipe;         /*!< Audio class  device control pipe*/
    transfer_callback_t controlCallbackFn;    /*!< Audio control transfer callback function*/
    void *controlCallbackParam;               /*!< Audio control transfer callback function*/
    usb_host_transfer_t *controlTransfer;     /*!< On-going control transfer*/
    uint16_t inPacketSize;                    /*!< Audio ISO in maximum packet size*/
    uint16_t outPacketSize;                   /*!< Audio ISO out maximum packet size*/
    uint8_t isSetup;                          /*!< Whether the audio setup transfer is transmitting*/
    uint8_t isoEpNum;                         /*!< Audio stream ISO endpoint number*/
    uint8_t streamIfnum;                      /*!< Audio stream ISO interface number*/
} audio_instance_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name USB host audio class APIs
 * @{
 */

/*!
 * @brief Initializes the audio instance.
 *
 * This function allocates the resource for the audio instance.
 *
 * @param deviceHandle       The device handle.
 * @param classHandlePtr Return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostAudioInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandlePtr);

/*!
 * @brief Deinitializes the Audio instance.
 *
 * This function release the resource for audio instance.
 *
 * @param deviceHandle   The device handle.
 * @param classHandle The class handle.
 *
 * @retval kStatus_USB_Success        The device is deinitialized successfully.
 */
extern usb_status_t USB_HostAudioDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

/*!
 * @brief Sets the audio class stream interface.
 *
 * This function binds the interface with the audio instance.
 *
 * @param classHandle        The class handle.
 * @param interfaceHandle    The interface handle.
 * @param alternateSetting   The alternate setting value.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam      The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 * @retval kStatus_USB_Busy           Callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  Callback return status, the transfer is stalled by the device.
 * @retval kStatus_USB_Error          Callback return status, open pipe fail. See the USB_HostOpenPipe.
 */
extern usb_status_t USB_HostAudioStreamSetInterface(usb_host_class_handle classHandle,
                                                    usb_host_interface_handle interfaceHandle,
                                                    uint8_t alternateSetting,
                                                    transfer_callback_t callbackFn,
                                                    void *callbackParam);

/*!
 * @brief Sets the audio class control interface.
 *
 * This function binds the interface with the audio instance.
 *
 * @param classHandle        The class handle.
 * @param interfaceHandle    The interface handle.
 * @param alternateSetting   The alternate setting value.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam      The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 * @retval kStatus_USB_Busy           Callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  Callback return status, the transfer is stalled by the device.
 * @retval kStatus_USB_Error          Callback return status, open pipe fail. See USB_HostOpenPipe.
 */
extern usb_status_t USB_HostAudioControlSetInterface(usb_host_class_handle classHandle,
                                                     usb_host_interface_handle interfaceHandle,
                                                     uint8_t alternateSetting,
                                                     transfer_callback_t callbackFn,
                                                     void *callbackParam);

/*!
 * @brief Gets the pipe maximum packet size.
 *
 * @param classHandle The class handle.
 * @param pipeType    Its value is USB_ENDPOINT_CONTROL, USB_ENDPOINT_ISOCHRONOUS, USB_ENDPOINT_BULK or
 * USB_ENDPOINT_INTERRUPT.
 *                     See the usb_spec.h
 * @param direction    Pipe direction.
 *
 * @retval 0        The classHandle is NULL.
 * @retval max      Packet size.
 */
extern uint16_t USB_HostAudioPacketSize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction);

/*!
 * @brief Audio stream receive data.
 *
 * This function implements the audio receiving data.
 *
 * @param classHandle      The class handle.
 * @param buffer            The buffer pointer.
 * @param bufferLen          The buffer length.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy               There is no idle transfer.
 * @retval kStatus_USB_Error              Pipe is not initialized.
 *                                                       Or, send transfer fail. See the USB_HostRecv.
 */
extern usb_status_t USB_HostAudioStreamRecv(usb_host_class_handle classHandle,
                                            uint8_t *buffer,
                                            uint32_t bufferLen,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Audio stream send data.
 *
 * This function implements the audio sending data.
 *
 * @param classHandle      The class handle.
 * @param buffer            The buffer pointer.
 * @param bufferLen          The buffer length.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          Receive request successfully.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy               There is no idle transfer.
 * @retval kStatus_USB_Error              pipe is not initialized.
 *                                                       Or, send transfer fail. See the USB_HostSend.
 */
extern usb_status_t USB_HostAudioStreamSend(usb_host_class_handle classHandle,
                                            uint8_t *buffer,
                                            uint32_t bufferLen,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Gets the audio stream current altsetting descriptor.
 *
 * This function implements the get audio stream current altsetting descriptor.
 *
 * @param classHandle               The class handle.
 * @param asIntfDesc                The pointer of class-specific AS interface descriptor.
 * @param formatTypeDesc       		The pointer of format type descriptor.
 * @param isoEndpDesc               The pointer of specific ISO endp descriptor.
 *
 * @retval kStatus_USB_Success          Get the audio stream current altsetting descriptor request successfully.
 * @retval kStatus_USB_InvalidHandle   The classHandle is NULL pointer.
 *
 */
extern usb_status_t USB_HostAudioStreamGetCurrentAltsettingDescriptors(
    usb_host_class_handle classHandle,
    usb_audio_stream_spepific_as_intf_desc_t **asIntfDesc,
    usb_audio_stream_format_type_desc_t **formatTypeDesc,
    usb_audio_stream_specific_iso_endp_desc_t **isoEndpDesc);

/*!
 * @brief The USB audio feature unit request.
 *
 * This function implements the USB audio feature unit request.
 *
 * @param classHandle      The class handle.
 * @param channelNo        The channel number of audio feature unit.
 * @param buf              The feature unit request buffer pointer.
 * @param cmdCode          The feature unit command code, for example USB_AUDIO_GET_CUR_MUTE, and so on.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success            Feature unit request successfully.
 * @retval kStatus_USB_InvalidHandle    The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy                There is no idle transfer.
 * @retval kStatus_USB_Error                Send transfer fail. See the USB_HostSendSetup.
 *
 */
extern usb_status_t USB_HostAudioFeatureUnitRequest(usb_host_class_handle classHandle,
                                                    uint8_t channelNo,
                                                    void *buf,
                                                    uint32_t cmdCode,
                                                    transfer_callback_t callbackFn,
                                                    void *callbackParam);

/*!
 * @brief The USB audio endpoint request.
 *
 * This function implements the USB audio endpoint request.
 *
 * @param classHandle      The class handle.
 * @param buf              The feature unit buffer pointer.
 * @param cmdCode          The feature unit command code, for example USB_AUDIO_GET_CUR_PITCH, and so on.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam   The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          Endpoint request successfully.
 * @retval kStatus_USB_InvalidHandle    The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy               There is no idle transfer.
 * @retval kStatus_USB_Error               Send transfer fail. See the USB_HostSendSetup.
 *
 */
extern usb_status_t USB_HostAudioEndpointRequest(usb_host_class_handle classHandle,
                                                 void *buf,
                                                 uint32_t cmdCode,
                                                 transfer_callback_t callbackFn,
                                                 void *callbackParam);

/*! @}*/
#ifdef __cplusplus
}
#endif
/*! @}*/
#endif /* __USB_HOST_AUDIO_H__ */
