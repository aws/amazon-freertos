/*******************************************************************************
* \file cybt_prm.c
*
* \brief
* Provides API to download BT firmware.
*
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef CYBT_PRM_H
#define CYBT_PRM_H

/*****************************************************************************
 *                           Type Definitions
 *****************************************************************************/
/**
 * Patch ram download status
 */
enum
{
    CYBT_PRM_STS_CONTINUE = 0,
    CYBT_PRM_STS_COMPLETE,
    CYBT_PRM_STS_ABORT
};
typedef uint8_t cybt_prm_status_t;

/** 
 * patch ram format type
 */
#define CYBT_PRM_FORMAT_BIN    0x00
#define CYBT_PRM_FORMAT_HCD    0x01
typedef uint8_t cybt_prm_format_t;

/**
 * Patch ram download status callback
 * 
 * @param[out]    status : download status
 *
 * @returns       void
 */
typedef void (cybt_prm_cback_t) (cybt_prm_status_t status);

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 *                           Function Declarations
 *****************************************************************************/

/**
 * Register patch ram callback, and start the patch ram 
 *                  download process.
 * @param[in]      p_cb          : callback for download status
 * @param[in]      p_patch_buf   : address of patch ram buffer
 * @param[in]      patch_buf_len : length of patch ram buffer
 * @param[in]      address       : address of patch ram to be written,
 * @param[in]      format_type   : patch format type ( bin, hcd ...)
 *
 * @returns        true if successful,
 *                 false otherwise
 *
 */
bool cybt_prm_download (cybt_prm_cback_t *p_cb,
                               const uint8_t *p_patch_buf,
                               uint32_t patch_buf_len,
                               uint32_t address,
                               uint8_t  format_type
                              );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

