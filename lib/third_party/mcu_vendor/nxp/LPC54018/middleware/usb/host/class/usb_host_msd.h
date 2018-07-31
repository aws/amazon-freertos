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

#ifndef _USB_HOST_MSD_H_
#define _USB_HOST_MSD_H_

/*******************************************************************************
 * MSD class private structure, enumeration, macro
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* CBW and CSW Macros */
#define USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT (0x00U)
#define USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN (0x80U)
#define USB_HOST_MSD_CBW_SIGNATURE (0x43425355U)
#define USB_HOST_MSD_CSW_SIGNATURE (0x53425355U)

/* UFI data bit macro */
#define USB_HOST_UFI_BLOCK_DATA_VALID_LENGTH (10U)
#define USB_HOST_UFI_LOGICAL_UNIT_POSITION (5U)
#define USB_HOST_UFI_CBW_LENGTH (31U)
#define USB_HOST_UFI_CSW_LENGTH (13U)
#define USB_HOST_UFI_MODE_SENSE_PAGE_CONTROL_SHIFT (6U)
#define USB_HOST_UFI_MODE_SENSE_PAGE_CODE_SHIFT (0U)
#define USB_HOST_UFI_START_STOP_UNIT_LOEJ_SHIFT (1U)
#define USB_HOST_UFI_START_STOP_UNIT_START_SHIFT (0U)
#define USB_HOST_UFI_SEND_DIAGNOSTIC_SELF_TEST_SHIFT (2U)

/*******************************************************************************
 * MSD class public structure, enumeration, macro, function
 ******************************************************************************/
/*!
 * @addtogroup usb_host_msc_drv
 * @{
 */

/*! @brief retry time when transfer fail, when all the retries fail the transfer callback with error status */
#define USB_HOST_MSD_RETRY_MAX_TIME (1U)
/*! @brief mass storage block size */
#define USB_HOST_MSD_BLOCK_SIZE (512U)

/*! @brief MSD class code */
#define USB_HOST_MSD_CLASS_CODE (8U)
/*! @brief MSD sub-class code */
#define USB_HOST_MSD_SUBCLASS_CODE_UFI (4U)
/*! @brief MSD sub-class code */
#define USB_HOST_MSD_SUBCLASS_CODE_SCSI (6U)
/*! @brief MSD protocol code */
#define USB_HOST_MSD_PROTOCOL_BULK (0x50U)

/*! @brief MSD class-specific request (mass storage reset) */
#define USB_HOST_HID_MASS_STORAGE_RESET (0xFFU)
/*! @brief MSD class-specific request (get maximum logical unit number) */
#define USB_HOST_HID_GET_MAX_LUN (0xFEU)

/*! @brief UFI command process status */
typedef enum _usb_host_msd_command_status
{
    kMSD_CommandIdle = 0,
    kMSD_CommandTransferCBW,
    kMSD_CommandTransferData,
    kMSD_CommandTransferCSW,
    kMSD_CommandDone,
    kMSD_CommandCancel,
    kMSD_CommandErrorDone,
} usb_host_msd_command_status_t;

/*! @brief MSC Bulk-Only command block wrapper (CBW) */
typedef struct _usb_host_cbw
{
    uint32_t CBWSignature; /*!< Signature that helps identify this data packet as a CBW. The signature field shall
                              contain the value 43425355h (little endian), indicating a CBW */
    uint32_t
        CBWTag; /*!< A Command Block Tag sent by the host. The device shall echo the contents of this field back to the
                   host in the dCSWTag field of the associated CSW */
    uint32_t CBWDataTransferLength; /*!< The number of bytes of data that the host expects to transfer on the Bulk-In or
                                       Bulk-Out endpoint during the execution of this command */
    uint8_t CBWFlags;               /*!<
                                         Bit 7 Direction - the device shall ignore this bit if the dCBWDataTransferLength field is
                                       zero, otherwise:
                                             0 = Data-Out from host to the device,
                                             1 = Data-In from the device to the host.
                                         Bit 6 Obsolete. The host shall set this bit to zero.
                                         Bits 5..0 Reserved - the host shall set these bits to zero.
                                     */
    uint8_t CBWLun;      /*!< The device Logical Unit Number (LUN) to which the command block is being sent */
    uint8_t CBWCBLength; /*!< The valid length of the CBWCB in bytes. This defines the valid length of the command
                            block. The only legal values are 1 through 16 (01h through 10h).*/
    uint8_t CBWCB[16];   /*!< The command block to be executed by the device*/
} usb_host_cbw_t;

/*! @brief MSC Bulk-Only command status wrapper (CSW) */
typedef struct _usb_host_csw
{
    uint32_t CSWSignature; /*!< Signature that helps identify this data packet as a CSW. The signature field shall
                              contain the value 53425355h (little endian), indicating CSW.*/
    uint32_t CSWTag; /*!< The device shall set this field to the value received in the dCBWTag of the associated CBW*/
    uint32_t CSWDataResidue; /*!< the difference between the amount of data expected as stated in the
                                dCBWDataTransferLength and the actual amount of relevant data processed by the device.*/
    uint8_t CSWStatus;       /*!<
                                   bCSWStatus indicates the success or failure of the command.
                                   00h - Command passed.
                                   01h - Command Failed.
                                   02h - Phase error.
                                   others - Reserved.
                             */
} usb_host_csw_t;

/*! @brief MSC UFI command information structure */
typedef struct _usb_host_msd_command
{
    usb_host_cbw_t cbwBlock;       /*!< CBW data block*/
    usb_host_csw_t cswBlock;       /*!< CSW data block*/
    uint8_t *dataBuffer;           /*!< Data buffer pointer*/
    uint32_t dataLength;           /*!< Data buffer length*/
    uint32_t dataSofar;            /*!< Successful transfer data length*/
    usb_host_transfer_t *transfer; /*!< The transfer is used for processing the UFI command*/
    uint8_t retryTime;     /*!< The UFI command residual retry time, when it reduce to zero the UFI command fail */
    uint8_t dataDirection; /*!< The data direction, its value is USB_OUT or USB_IN*/
} usb_host_msd_command_t;

/*! @brief MSD instance structure, MSD usb_host_class_handle pointer to this structure */
typedef struct _usb_host_msd_instance
{
    usb_host_handle hostHandle;                /*!< This instance's related host handle*/
    usb_device_handle deviceHandle;            /*!< This instance's related device handle*/
    usb_host_interface_handle interfaceHandle; /*!< This instance's related interface handle*/
    usb_host_pipe_handle controlPipe;          /*!< This instance's related device control pipe*/
    usb_host_pipe_handle outPipe;              /*!< MSD bulk out pipe*/
    usb_host_pipe_handle inPipe;               /*!< MSD bulk in pipe*/
    transfer_callback_t commandCallbackFn;     /*!< MSD UFI command callback function pointer*/
    void *commandCallbackParam;                /*!< MSD UFI command callback parameter*/
    transfer_callback_t controlCallbackFn;     /*!< MSD control transfer callback function pointer*/
    void *controlCallbackParam;                /*!< MSD control transfer callback parameter*/
    usb_host_transfer_t *controlTransfer;      /*!< Ongoing control transfer*/
    usb_host_msd_command_t msdCommand;         /*!< Ongoing MSD UFI command information*/
    uint8_t commandStatus;                     /*!< UFI command process status, see command_status_t*/
    uint8_t internalResetRecovery; /*!< 1 - class driver internal mass storage reset recovery is on-going; 0 -
                                      application call USB_HostMsdMassStorageReset to reset or there is no reset*/
} usb_host_msd_instance_t;

/*! @brief UFI standard sense data structure */
typedef struct _usb_host_ufi_sense_data
{
    uint8_t errorCode;      /*!< This field shall contain a value of 70h to indicate current errors*/
    uint8_t reserved1;      /*!< Reserved field*/
    uint8_t senseKey;       /*!< Provide a hierarchy of error or command result information*/
    uint8_t information[4]; /*!< This field is command-specific; it is typically used by some commands to return a
                               logical block address denoting where an error occurred*/
    uint8_t additionalSenseLength; /*!< The UFI device sets the value of this field to ten, to indicate that ten more
                                      bytes of sense data follow this field*/
    uint8_t reserved2[4];          /*!< Reserved field*/
    uint8_t additionalSenseCode;   /*!< Provide a hierarchy of error or command result information*/
    uint8_t additionalSenseCodeQualifier; /*!< Provide a hierarchy of error or command result information*/
    uint8_t reserved3[4];                 /*!< Reserved field*/
} usb_host_ufi_sense_data_t;

/*! @brief UFI standard inquiry data structure */
typedef struct _usb_host_ufi_inquiry_data
{
    uint8_t peripheralDeviceType;      /*!< Identifies the device currently connected to the requested logical unit*/
    uint8_t removableMediaBit;         /*!< This shall be set to one to indicate removable media*/
    uint8_t version;                   /*!< Version*/
    uint8_t responseDataFormat;        /*!< A value of 01h shall be used for UFI device*/
    uint8_t additionalLength;          /*!< Specify the length in bytes of the parameters*/
    uint8_t reserved1[3];              /*!< Reserved field*/
    uint8_t vendorInformation[8];      /*!< Contains 8 bytes of ASCII data identifying the vendor of the product*/
    uint8_t productIdentification[16]; /*!< Contains 16 bytes of ASCII data as defined by the vendor*/
    uint8_t productRevisionLevel[4];   /*!< Contains 4 bytes of ASCII data as defined by the vendor*/
} usb_host_ufi_inquiry_data_t;

/*! @brief UFI read capacity data structure */
typedef struct _usb_host_ufi_read_capacity
{
    uint8_t lastLogicalBlockAddress[4]; /*!< The logical block number*/
    uint8_t blockLengthInBytes[4];      /*!< Block size*/
} usb_host_ufi_read_capacity_t;

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @name USB host MSD class APIs
 * @{
 */

/*!
 * @brief Initializes the MSD instance.
 *
 * This function allocates the resources for the MSD instance.
 *
 * @param[in] deviceHandle  The device handle.
 * @param[out] classHandle  Return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostMsdInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief Sets the interface.
 *
 * This function binds the interface with the MSD instance.
 *
 * @param[in] classHandle      The class handle.
 * @param[in] interfaceHandle  The interface handle.
 * @param[in] alternateSetting The alternate setting value.
 * @param[in] callbackFn       This callback is called after this function completes.
 * @param[in] callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 * @retval kStatus_USB_Success        Callback return status, the command succeeded.
 * @retval kStatus_USB_Busy           Callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  Callback return status, the transfer is stalled by the device.
 * @retval kStatus_USB_Error          Callback return status, open pipe fail. See the USB_HostOpenPipe.
 */
extern usb_status_t USB_HostMsdSetInterface(usb_host_class_handle classHandle,
                                            usb_host_interface_handle interfaceHandle,
                                            uint8_t alternateSetting,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Deinitializes the MSD instance.
 *
 * This function frees the resource for the MSD instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[in] classHandle    The class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
extern usb_status_t USB_HostMsdDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

/*!
 * @brief Mass storage reset.
 *
 * This function implements the mass storage reset request.
 *
 * @param[in] classHandle   The class handle.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostMsdMassStorageReset(usb_host_class_handle classHandle,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam);

/*!
 * @brief Gets the maximum logical unit number.
 *
 * This function implements the get maximum LUN request.
 *
 * @param[in] classHandle        The class handle.
 * @param[out] logicalUnitNumber Return logical unit number value.
 * @param[in] callbackFn         This callback is called after this function completes.
 * @param[in] callbackParam      The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdGetMaxLun(usb_host_class_handle classHandle,
                                         uint8_t *logicalUnitNumber,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*!
 * @brief Mass storage read (10).
 *
 * This function implements the UFI READ(10) command. This command requests that the UFI
 * device transfer data to the host.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] blockAddress   The start block address.
 * @param[out] buffer        Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] blockNumber    Read block number.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdRead10(usb_host_class_handle classHandle,
                                      uint8_t logicalUnit,
                                      uint32_t blockAddress,
                                      uint8_t *buffer,
                                      uint32_t bufferLength,
                                      uint32_t blockNumber,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam);

/*!
 * @brief Mass storage read (12).
 *
 * This function implements the UFI READ(12) command and requests that the UFI
 * device transfer data to the host.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] blockAddress   The start block address.
 * @param[out] buffer        Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] blockNumber    Read block number.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdRead12(usb_host_class_handle classHandle,
                                      uint8_t logicalUnit,
                                      uint32_t blockAddress,
                                      uint8_t *buffer,
                                      uint32_t bufferLength,
                                      uint32_t blockNumber,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam);

/*!
 * @brief Mass storage write (10).
 *
 * This function implements the UFI WRITE(10) command and requests that the UFI device
 * write the data transferred by the host to the medium.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] blockAddress   The start block address.
 * @param[in] buffer         Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] blockNumber    Write block number.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdWrite10(usb_host_class_handle classHandle,
                                       uint8_t logicalUnit,
                                       uint32_t blockAddress,
                                       uint8_t *buffer,
                                       uint32_t bufferLength,
                                       uint32_t blockNumber,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*!
 * @brief Mass storage write (12).
 *
 * This function implements the UFI WRITE(12) command and requests that the UFI device
 * write the data transferred by the host to the medium.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] blockAddress   The start block address.
 * @param[in] buffer         Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] blockNumber    Write block number.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdWrite12(usb_host_class_handle classHandle,
                                       uint8_t logicalUnit,
                                       uint32_t blockAddress,
                                       uint8_t *buffer,
                                       uint32_t bufferLength,
                                       uint32_t blockNumber,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*!
 * @brief Mass storage read capacity.
 *
 * This function implements the UFI READ CAPACITY command and allows the host to request
 * capacities of the currently installed medium.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[out] buffer        Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdReadCapacity(usb_host_class_handle classHandle,
                                            uint8_t logicalUnit,
                                            uint8_t *buffer,
                                            uint32_t bufferLength,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Mass storage test unit ready.
 *
 * This function implements the UFI TEST UNIT READY command and
 * checks if the UFI device is ready.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdTestUnitReady(usb_host_class_handle classHandle,
                                             uint8_t logicalUnit,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);

/*!
 * @brief mass storage request sense.
 *
 * This function implements the UFI REQUEST SENSE command, this command instructs
 * the UFI device to transfer sense data to the host for the specified logical unit.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[out] buffer        Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdRequestSense(usb_host_class_handle classHandle,
                                            uint8_t logicalUnit,
                                            uint8_t *buffer,
                                            uint32_t bufferLength,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Mass storage mode select.
 *
 * This function implements the UFI MODE SELECT command and allows the host
 * to specify medium or device parameters to the UFI device.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] buffer         Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdModeSelect(usb_host_class_handle classHandle,
                                          uint8_t logicalUnit,
                                          uint8_t *buffer,
                                          uint32_t bufferLength,
                                          transfer_callback_t callbackFn,
                                          void *callbackParam);

/*!
 * @brief Mass storage mode sense.
 *
 * This function implements the UFI MODE SENSE command and allows the UFI
 * device to report medium or device parameters to the host.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] pageControl    The page control field specifies the type of mode parameters to return.
 * @param[in] pageCode       Buffer pointer.
 * @param[out] buffer        Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdModeSense(usb_host_class_handle classHandle,
                                         uint8_t logicalUnit,
                                         uint8_t pageControl,
                                         uint8_t pageCode,
                                         uint8_t *buffer,
                                         uint32_t bufferLength,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*!
 * @brief Mass storage inquiry.
 *
 * This function implements the UFI INQUIRY command and requests that information regarding
 * parameters of the UFI device itself be sent to the host.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[out] buffer        Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdInquiry(usb_host_class_handle classHandle,
                                       uint8_t logicalUnit,
                                       uint8_t *buffer,
                                       uint32_t bufferLength,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*!
 * @brief Mass storage read format capacities.
 *
 * This function implements the UFI READ FORMAT CAPACITIES command and allows the host to request
 * a list of the possible capacities that can be formatted on the currently installed medium.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[out] buffer        Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdReadFormatCapacities(usb_host_class_handle classHandle,
                                                    uint8_t logicalUnit,
                                                    uint8_t *buffer,
                                                    uint32_t bufferLength,
                                                    transfer_callback_t callbackFn,
                                                    void *callbackParam);

/*!
 * @brief Mass storage format unit.
 *
 * This function implements the UFI FORMAT UNIT command and the host sends this command to physically format one
 * track of a diskette according to the selected options.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] trackNumber    This specifies which track is to be formatted.
 * @param[in] interLeave     This specifies the interleave that shall be used for formatting.
 * @param[in] buffer         Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdFormatUnit(usb_host_class_handle classHandle,
                                          uint8_t logicalUnit,
                                          uint8_t trackNumber,
                                          uint16_t interLeave,
                                          uint8_t *buffer,
                                          uint32_t bufferLength,
                                          transfer_callback_t callbackFn,
                                          void *callbackParam);

/*!
 * @brief Mass storage prevents/allows a medium removal.
 *
 * This function implements the UFI PREVENT-ALLOW MEDIUM REMOVAL command and notifies the FUI device
 * to enable or disable the removal of the medium in the logical unit.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] prevent        Prevent or allow
 *                           - 0: enable (allow) the removal of the medium
 *                           - 1: disable (prevent) removal of the medium
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdPreventAllowRemoval(usb_host_class_handle classHandle,
                                                   uint8_t logicalUnit,
                                                   uint8_t prevent,
                                                   transfer_callback_t callbackFn,
                                                   void *callbackParam);

/*!
 * @brief Mass storage write and verify.
 *
 * This function implements the UFI WRITE AND VERIFY command and requests that the UFI device
 * writes the data transferred by the host to the medium, then verifies the data on the medium.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] blockAddress   The start block address.
 * @param[in] buffer         Buffer pointer.
 * @param[in] bufferLength   The buffer length.
 * @param[in] blockNumber    Write and verify block number.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdWriteAndVerify(usb_host_class_handle classHandle,
                                              uint8_t logicalUnit,
                                              uint32_t blockAddress,
                                              uint8_t *buffer,
                                              uint32_t bufferLength,
                                              uint32_t blockNumber,
                                              transfer_callback_t callbackFn,
                                              void *callbackParam);

/*!
 * @brief Mass storage start stop unit.
 *
 * This function implements the UFI START-STOP UNIT command and instructs the UFI device
 * to enable or disable media access operations .
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] loadEject      A Load Eject (LoEj) bit of zero requests that no eject action be performed. A LoEj bit of
 * one, with the
 *                           Start bit cleared to zero, which instructs the UFI device to eject the media.
 * @param[in] start          A Start bit of one instructs the UFI device to enable media access operations. A Start bit
 * 							 of zero instructs the UFI device to disable media access operations.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdStartStopUnit(usb_host_class_handle classHandle,
                                             uint8_t logicalUnit,
                                             uint8_t loadEject,
                                             uint8_t start,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);

/*!
 * @brief Mass storage verify.
 *
 * This function implements the UFI VERIFY command and requests that the UFI device
 * verify the data on the medium.
 *
 * @param[in] classHandle        The class MSD handle.
 * @param[in] logicalUnit        Logical unit number.
 * @param[in] blockAddress       The start block address.
 * @param[in] verificationLength The data length that need to be verified.
 * @param[in] callbackFn         This callback is called after this command completes.
 * @param[in] callbackParam      The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdVerify(usb_host_class_handle classHandle,
                                      uint8_t logicalUnit,
                                      uint32_t blockAddress,
                                      uint16_t verificationLength,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam);

/*!
 * @brief Mass storage rezero.
 *
 * This function implements the UFI REZERO UNIT command. This command positions the head of
 * the drive to the cylinder 0.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdRezeroUnit(usb_host_class_handle classHandle,
                                          uint8_t logicalUnit,
                                          transfer_callback_t callbackFn,
                                          void *callbackParam);

/*!
 * @brief Mass storage seek(10).
 *
 * This function implements the UFI SEEK(10) command and requests that the UFI device
 * seek to the specified Logical Block Address.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] blockAddress   The start block address.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdSeek10(usb_host_class_handle classHandle,
                                      uint8_t logicalUnit,
                                      uint32_t blockAddress,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam);

/*!
 * @brief Mass storage send diagnostic.
 *
 * This function implements the UFI SEND DIAGNOSTIC command. This command requests the UFI device
 * to do a reset or perform a self-test.
 *
 * @param[in] classHandle    The class MSD handle.
 * @param[in] logicalUnit    Logical unit number.
 * @param[in] selfTest       0 = perform special diagnostic test; 1 = perform default self-test.
 * @param[in] callbackFn     This callback is called after this command completes.
 * @param[in] callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           The previous command is executing or there is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSend/USB_HostRecv.
 * @retval kStatus_USB_Success        Callback return status, the command succeed.
 * @retval kStatus_USB_MSDStatusFail  Callback return status, the CSW status indicate this command fail.
 * @retval kStatus_USB_Error          Callback return status, the command fail.
 */
extern usb_status_t USB_HostMsdSendDiagnostic(usb_host_class_handle classHandle,
                                              uint8_t logicalUnit,
                                              uint8_t selfTest,
                                              transfer_callback_t callbackFn,
                                              void *callbackParam);

/*! @}*/

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* _USB_HOST_MSD_H_ */
