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
#if ((defined USB_HOST_CONFIG_MSD) && (USB_HOST_CONFIG_MSD))
#include "usb_host.h"
#include "usb_host_msd.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* UFI command code */
#define UFI_FORMAT_UNIT (0x04U)
#define UFI_INQUIRY (0x12U)
#define UFI_START_STOP (0x1BU)
#define UFI_MODE_SELECT (0x55U)
#define UFI_MODE_SENSE (0x5AU)
#define UFI_MEDIUM_REMOVAL (0x1EU)
#define UFI_READ10 (0x28U)
#define UFI_READ12 (0xA8U)
#define UFI_READ_CAPACITY (0x25U)
#define UFI_READ_FORMAT_CAPACITY (0x23U)
#define UFI_REQUEST_SENSE (0x03U)
#define UFI_REZERO_UINT (0x01U)
#define UFI_SEEK (0x2BU)
#define UFI_SEND_DIAGNOSTIC (0x1DU)
#define UFI_TEST_UNIT_READY (0x00U)
#define UFI_VERIFY (0x2FU)
#define UFI_WRITE10 (0x2AU)
#define UFI_WRITE12 (0xAAU)
#define UFI_WRITE_VERIFY (0x2EU)

#define GET_BYTE_FROM_LE_LONG(b, n) \
    ((uint8_t)((USB_LONG_TO_LITTLE_ENDIAN(b)) >> (n * 8))) /* get the byte from the long value */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

extern usb_status_t USB_HostMsdCommand(usb_host_class_handle classHandle,
                                       uint8_t *buffer,
                                       uint32_t bufferLength,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam,
                                       uint8_t direction,
                                       uint8_t byteValues[10]);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

usb_status_t USB_HostMsdRead10(usb_host_class_handle classHandle,
                               uint8_t logicalUnit,
                               uint32_t blockAddress,
                               uint8_t *buffer,
                               uint32_t bufferLength,
                               uint32_t blockNumber,
                               transfer_callback_t callbackFn,
                               void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_READ10,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 3),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 2),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 1),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 0),
                          0x00,
                          GET_BYTE_FROM_LE_LONG(blockNumber, 1),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 0),
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN, ufiBytes);
}

usb_status_t USB_HostMsdRead12(usb_host_class_handle classHandle,
                               uint8_t logicalUnit,
                               uint32_t blockAddress,
                               uint8_t *buffer,
                               uint32_t bufferLength,
                               uint32_t blockNumber,
                               transfer_callback_t callbackFn,
                               void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_READ12,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 3),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 2),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 1),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 0),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 3),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 2),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 1),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 0)};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN, ufiBytes);
}

usb_status_t USB_HostMsdWrite10(usb_host_class_handle classHandle,
                                uint8_t logicalUnit,
                                uint32_t blockAddress,
                                uint8_t *buffer,
                                uint32_t bufferLength,
                                uint32_t blockNumber,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_WRITE10,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 3),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 2),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 1),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 0),
                          0x00,
                          GET_BYTE_FROM_LE_LONG(blockNumber, 1),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 0),
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT, ufiBytes);
}

usb_status_t USB_HostMsdWrite12(usb_host_class_handle classHandle,
                                uint8_t logicalUnit,
                                uint32_t blockAddress,
                                uint8_t *buffer,
                                uint32_t bufferLength,
                                uint32_t blockNumber,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_WRITE12,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 3),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 2),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 1),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 0),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 3),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 2),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 1),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 0)};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT, ufiBytes);
}

usb_status_t USB_HostMsdReadCapacity(usb_host_class_handle classHandle,
                                     uint8_t logicalUnit,
                                     uint8_t *buffer,
                                     uint32_t bufferLength,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_READ_CAPACITY,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN, ufiBytes);
}

usb_status_t USB_HostMsdTestUnitReady(usb_host_class_handle classHandle,
                                      uint8_t logicalUnit,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_TEST_UNIT_READY,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00};
    return USB_HostMsdCommand(classHandle, NULL, 0, callbackFn, callbackParam, USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT,
                              ufiBytes);
}

usb_status_t USB_HostMsdRequestSense(usb_host_class_handle classHandle,
                                     uint8_t logicalUnit,
                                     uint8_t *buffer,
                                     uint32_t bufferLength,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_REQUEST_SENSE,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          0x00,
                          0x00,
                          (uint8_t)bufferLength,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN, ufiBytes);
}

usb_status_t USB_HostMsdModeSelect(usb_host_class_handle classHandle,
                                   uint8_t logicalUnit,
                                   uint8_t *buffer,
                                   uint32_t bufferLength,
                                   transfer_callback_t callbackFn,
                                   void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_MODE_SELECT,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          GET_BYTE_FROM_LE_LONG(bufferLength, 1),
                          GET_BYTE_FROM_LE_LONG(bufferLength, 0),
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT, ufiBytes);
}

usb_status_t USB_HostMsdModeSense(usb_host_class_handle classHandle,
                                  uint8_t logicalUnit,
                                  uint8_t pageControl,
                                  uint8_t pageCode,
                                  uint8_t *buffer,
                                  uint32_t bufferLength,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_MODE_SENSE, (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          (uint8_t)(((uint32_t)pageCode << USB_HOST_UFI_MODE_SENSE_PAGE_CONTROL_SHIFT) |
                                    ((uint32_t)pageCode << USB_HOST_UFI_MODE_SENSE_PAGE_CODE_SHIFT)),
                          0x00, 0x00, 0x00, 0x00, GET_BYTE_FROM_LE_LONG(bufferLength, 1),
                          GET_BYTE_FROM_LE_LONG(bufferLength, 0), 0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN, ufiBytes);
}

usb_status_t USB_HostMsdInquiry(usb_host_class_handle classHandle,
                                uint8_t logicalUnit,
                                uint8_t *buffer,
                                uint32_t bufferLength,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_INQUIRY,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          0x00,
                          0x00,
                          (uint8_t)bufferLength,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN, ufiBytes);
}

usb_status_t USB_HostMsdReadFormatCapacities(usb_host_class_handle classHandle,
                                             uint8_t logicalUnit,
                                             uint8_t *buffer,
                                             uint32_t bufferLength,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_READ_FORMAT_CAPACITY,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          GET_BYTE_FROM_LE_LONG(bufferLength, 1),
                          GET_BYTE_FROM_LE_LONG(bufferLength, 0),
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN, ufiBytes);
}

usb_status_t USB_HostMsdFormatUnit(usb_host_class_handle classHandle,
                                   uint8_t logicalUnit,
                                   uint8_t trackNumber,
                                   uint16_t interLeave,
                                   uint8_t *buffer,
                                   uint32_t bufferLength,
                                   transfer_callback_t callbackFn,
                                   void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_FORMAT_UNIT,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          trackNumber,
                          GET_BYTE_FROM_LE_LONG(interLeave, 1),
                          GET_BYTE_FROM_LE_LONG(interLeave, 0),
                          0x00,
                          0x00,
                          GET_BYTE_FROM_LE_LONG(bufferLength, 1),
                          GET_BYTE_FROM_LE_LONG(bufferLength, 0),
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT, ufiBytes);
}

usb_status_t USB_HostMsdPreventAllowRemoval(usb_host_class_handle classHandle,
                                            uint8_t logicalUnit,
                                            uint8_t prevent,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_MEDIUM_REMOVAL,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          0x00,
                          0x00,
                          prevent,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00};
    return USB_HostMsdCommand(classHandle, NULL, 0, callbackFn, callbackParam, USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT,
                              ufiBytes);
}

usb_status_t USB_HostMsdWriteAndVerify(usb_host_class_handle classHandle,
                                       uint8_t logicalUnit,
                                       uint32_t blockAddress,
                                       uint8_t *buffer,
                                       uint32_t bufferLength,
                                       uint32_t blockNumber,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_WRITE_VERIFY,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 3),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 2),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 1),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 0),
                          0x00,
                          GET_BYTE_FROM_LE_LONG(blockNumber, 1),
                          GET_BYTE_FROM_LE_LONG(blockNumber, 0),
                          0x00};
    return USB_HostMsdCommand(classHandle, buffer, bufferLength, callbackFn, callbackParam,
                              USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT, ufiBytes);
}

usb_status_t USB_HostMsdStartStopUnit(usb_host_class_handle classHandle,
                                      uint8_t logicalUnit,
                                      uint8_t loadEject,
                                      uint8_t start,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_START_STOP, (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION), 0x00, 0x00,
                          (uint8_t)(((uint32_t)loadEject << USB_HOST_UFI_START_STOP_UNIT_LOEJ_SHIFT) |
                                    ((uint32_t)start << USB_HOST_UFI_START_STOP_UNIT_START_SHIFT)),
                          0x00, 0x00, 0x00, 0x00, 0x00};
    return USB_HostMsdCommand(classHandle, NULL, 0, callbackFn, callbackParam, USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT,
                              ufiBytes);
}

usb_status_t USB_HostMsdVerify(usb_host_class_handle classHandle,
                               uint8_t logicalUnit,
                               uint32_t blockAddress,
                               uint16_t verificationLength,
                               transfer_callback_t callbackFn,
                               void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_VERIFY,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 3),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 2),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 1),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 0),
                          0x00,
                          GET_BYTE_FROM_LE_LONG(verificationLength, 1),
                          GET_BYTE_FROM_LE_LONG(verificationLength, 0),
                          0x00};
    return USB_HostMsdCommand(classHandle, NULL, 0, callbackFn, callbackParam, USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT,
                              ufiBytes);
}

usb_status_t USB_HostMsdRezeroUnit(usb_host_class_handle classHandle,
                                   uint8_t logicalUnit,
                                   transfer_callback_t callbackFn,
                                   void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_REZERO_UINT,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00,
                          0x00};
    return USB_HostMsdCommand(classHandle, NULL, 0, callbackFn, callbackParam, USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT,
                              ufiBytes);
}

usb_status_t USB_HostMsdSeek10(usb_host_class_handle classHandle,
                               uint8_t logicalUnit,
                               uint32_t blockAddress,
                               transfer_callback_t callbackFn,
                               void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_SEEK,
                          (uint8_t)(logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 3),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 2),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 1),
                          GET_BYTE_FROM_LE_LONG(blockAddress, 0),
                          0x00,
                          0x00,
                          0x00,
                          0x00};
    return USB_HostMsdCommand(classHandle, NULL, 0, callbackFn, callbackParam, USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT,
                              ufiBytes);
}

usb_status_t USB_HostMsdSendDiagnostic(usb_host_class_handle classHandle,
                                       uint8_t logicalUnit,
                                       uint8_t selfTest,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam)
{
    uint8_t ufiBytes[] = {UFI_REZERO_UINT,
                          (uint8_t)(((uint32_t)logicalUnit << USB_HOST_UFI_LOGICAL_UNIT_POSITION) |
                                    ((uint32_t)selfTest << USB_HOST_UFI_SEND_DIAGNOSTIC_SELF_TEST_SHIFT)),
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    return USB_HostMsdCommand(classHandle, NULL, 0, callbackFn, callbackParam, USB_HOST_MSD_CBW_FLAGS_DIRECTION_OUT,
                              ufiBytes);
}

#endif /* USB_HOST_CONFIG_MSD */
