/**
 * Copyright (c) 2015 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef NFC_LAUNCHAPP_MSG_H__
#define NFC_LAUNCHAPP_MSG_H__

/** @file
 *
 * @defgroup nfc_launchapp_msg Launch app messages
 * @{
 * @ingroup nfc_ndef_messages
 *
 * @brief Generation of NFC NDEF messages that can be used to launch apps.
 *
 */

#include <stdint.h>
#include "nfc_ndef_msg.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @brief Function for encoding an NFC NDEF launch app message.
 *
 * This function encodes an NFC NDEF message into a buffer.
 *
 * @param[in]  p_android_package_name       Pointer to the Android package name string.
 *                                          If NULL, the Android Application Record will be skipped.
 * @param[in]  android_package_name_length  Length of the Android package name.
 * @param[in]  p_win_app_id                 Pointer to the Windows application ID string (GUID).
 *                                          If NULL, the Windows LaunchApp record will be skipped.
 * @param[in]  win_app_id_length            Length of the Windows application ID.
 * @param[out] p_buf                        Pointer to the buffer for the message.
 * @param[in,out] p_len                     Size of the available memory for the message as input.
 *                                          Size of the generated message as output.
 *
 * @retval NRF_SUCCESS              If the description was successfully created.
 * @retval NRF_ERROR_INVALID_PARAM  If both p_android_package_name and windows_application_id were
 *                                  invalid (equal to NULL).
 * @retval NRF_ERROR_NO_MEM         If the predicted message size is bigger than the provided
 *                                  buffer space.
 * @retval Other                    Other codes might be returned depending on
 *                                  the function @ref nfc_ndef_msg_encode
 */
ret_code_t nfc_launchapp_msg_encode(uint8_t const * p_android_package_name,
                                    uint8_t         android_package_name_length,
                                    uint8_t const * p_win_app_id,
                                    uint8_t         win_app_id_length,
                                    uint8_t       * p_buf,
                                    uint32_t      * p_len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

 #endif // NFC_LAUNCHAPP_MSG_H__


