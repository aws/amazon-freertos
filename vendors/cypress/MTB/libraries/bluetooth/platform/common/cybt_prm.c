/*******************************************************************************
* \file cybt_prm.c
*
* \brief
* Provides utilities for BT firmware download.
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

#include <string.h>
#include <stdbool.h>

#include "wiced_bt_dev.h"
#include "cyabs_rtos.h"

#include "cybt_prm.h"
#include "cybt_platform_trace.h"


/*****************************************************************************
 *                                Constants
 *****************************************************************************/
#define HCI_VSC_DOWNLOAD_MINI_DRV        (0xFC2E)
#define HCI_VSC_WRITE_RAM                (0xFC4C)
#define HCI_VSC_LAUNCH_RAM               (0xFC4E)

/* dest ram location */
#define CYBT_DEST_RAM_LOCATION           (0x00085D00)
#define CYBT_MINIDRV_2_PATCH_RAM_DELAY   (50)
#define CYBT_END_DELAY                   (250)  /* delay before sending any new command (ms) */

#define CYBT_MAX_PRM_LEN                 (250)


/*****************************************************************************
 *                           Type Definitions
 *****************************************************************************/
enum
{
    CYBT_PRM_ST_IDLE  = 0,
    CYBT_PRM_ST_INITIALIZING,
    CYBT_PRM_ST_INITIALIZING_DONE,
    CYBT_PRM_ST_LOADING_DATA,
    CYBT_PRM_ST_LOADING_DATA_DONE,
    CYBT_PRM_ST_LAUNCHING_RAM
};
typedef uint8_t cybt_prm_state_t;

/* Define a structure for CYBT PRM Control Block
*/
typedef struct
{
    cybt_prm_state_t      state;                /* download state */
    uint8_t               internal_patch;       /* internal patch or not */
    uint16_t              opcode;  /* current opcode being used for the download */
    uint32_t              patch_length;         /* total patch length need to download*/
    const uint8_t*        p_patch_data;         /* pointer to orginal patch data */
    uint32_t              tx_patch_length;      /* patch length send to controller */
    cybt_prm_cback_t     *p_cb;                /* call back for patch ram status */
    uint32_t              dest_ram;
    uint8_t               format_type;          /* patch format bin/hcd/... */
} cybt_prm_cb_t;


/******************************************************************************
 *                           Variables Definitions
 ******************************************************************************/
/* patch ram control block */
static cybt_prm_cb_t cybt_prm_cb;


/******************************************************************************
 *                          Function Declarations
 ******************************************************************************/
/* Vendor specified command complete callback */
static void cybt_prm_command_complete_cback(wiced_bt_dev_vendor_specific_command_complete_params_t *p_param);

/* Send next patch of data */
static void cybt_prm_send_next_patch(void);

/* Launching RAM */
static void cybt_prm_launch_ram_internal(void);


/******************************************************************************
 *                           Function Definitions
 ******************************************************************************/

/*******************************************************************************
**
** Function         cybt_prm_download
**
** Description      Register patch ram callback, and start the patch ram 
**                  download process.
** Input Param      p_cb - callback for download status
**                  p_patch_buf - address of patch ram buffer
**                  patch_buf_len - length of patch ram buffer
**                  address - address of patch ram to be written,
**                  format_type - patch format type ( bin, hcd ...)
**
** Returns          true if successful, otherwise false
**
*******************************************************************************/
bool cybt_prm_download (cybt_prm_cback_t *p_cb,
                               const uint8_t  *p_patch_buf,
                               uint32_t patch_buf_len,
                               uint32_t address,
                               uint8_t  format_type
                              )
{
    PRM_TRACE_DEBUG("cybt_prm_init()");

    memset(&cybt_prm_cb, 0, sizeof(cybt_prm_cb_t));

    cybt_prm_cb.opcode = HCI_VSC_DOWNLOAD_MINI_DRV;

    if (p_patch_buf)
    {
        cybt_prm_cb.p_patch_data = p_patch_buf;
        cybt_prm_cb.patch_length = patch_buf_len;
        if (cybt_prm_cb.patch_length == 0)
        {
            return false;
        }

        cybt_prm_cb.internal_patch = TRUE;
    }
    else
    {
        cybt_prm_cb.internal_patch = FALSE;
    }
    cybt_prm_cb.format_type = format_type;
    cybt_prm_cb.p_cb = p_cb;

    if( address )
    {
        cybt_prm_cb.dest_ram = address;
    }
    else
    {
        cybt_prm_cb.dest_ram = CYBT_DEST_RAM_LOCATION;
    }

    wiced_bt_dev_vendor_specific_command(cybt_prm_cb.opcode,
                                         0,
                                         NULL,
                                         cybt_prm_command_complete_cback
                                        );
    cybt_prm_cb.state = CYBT_PRM_ST_INITIALIZING;

    return true;
}

/*******************************************************************************
**
** Function         cybt_prm_command_complete_cback
**
** Description      This is the vendor specific comand complete event call back.
**
** Returns          void
**
*******************************************************************************/
static void cybt_prm_command_complete_cback(wiced_bt_dev_vendor_specific_command_complete_params_t *p_param)
{
    /* if it is not completed */
    if (p_param->p_param_buf[0] != 0)
    {
        if (cybt_prm_cb.p_cb)
            (cybt_prm_cb.p_cb)(CYBT_PRM_STS_ABORT);
        return;
    }

    switch (cybt_prm_cb.state)
    {
        case CYBT_PRM_ST_INITIALIZING:
             if (!(p_param->opcode == cybt_prm_cb.opcode))
             {
                  if (cybt_prm_cb.p_cb)
                      (cybt_prm_cb.p_cb)(CYBT_PRM_STS_ABORT);
                  return;
             }
             cybt_prm_cb.state = CYBT_PRM_ST_INITIALIZING_DONE;
             PRM_TRACE_DEBUG("CYBT_PRM_ST_INITIALIZING_DONE");

             cy_rtos_delay_milliseconds(CYBT_MINIDRV_2_PATCH_RAM_DELAY);

             if (!cybt_prm_cb.internal_patch)
             {
                  if (cybt_prm_cb.p_cb)
                      (cybt_prm_cb.p_cb)(CYBT_PRM_STS_CONTINUE);
             }
             else
             {
                  cybt_prm_cb.state = CYBT_PRM_ST_LOADING_DATA;
                  cybt_prm_send_next_patch();
             }
             break;
        case CYBT_PRM_ST_LOADING_DATA:
             if (!(p_param->opcode == HCI_VSC_WRITE_RAM))
             {
                  if (cybt_prm_cb.p_cb)
                  {
                      (cybt_prm_cb.p_cb)(CYBT_PRM_STS_ABORT);
                  }
                  return;
             }
             if (cybt_prm_cb.tx_patch_length >= cybt_prm_cb.patch_length)
             {
                 cybt_prm_cb.state = CYBT_PRM_ST_LOADING_DATA_DONE;
                 /* For internal patch, if all patches are sent */
                 if (cybt_prm_cb.internal_patch)
                 {
                     PRM_TRACE_DEBUG("Internal patch download completed, starting launching ram");
                     cybt_prm_cb.state = CYBT_PRM_ST_LAUNCHING_RAM;
                     cybt_prm_launch_ram_internal();
                 }
                 /* For external patch, send contiune to application back */
                 else
                 {
                     PRM_TRACE_DEBUG("External patch piece down, wait for next piece");
                     cybt_prm_cb.dest_ram += cybt_prm_cb.patch_length;
                     if (cybt_prm_cb.p_cb)
                         (cybt_prm_cb.p_cb)(CYBT_PRM_STS_CONTINUE);
                 }
             }
             else
             {
                 /* Have not complete this piece yet */
                 cybt_prm_send_next_patch();
             }
             break;
        case CYBT_PRM_ST_LAUNCHING_RAM:
             if (!(p_param->opcode == HCI_VSC_LAUNCH_RAM))
             {
                  if (cybt_prm_cb.p_cb)
                      (cybt_prm_cb.p_cb)(CYBT_PRM_STS_ABORT);
                  return;
             }

             PRM_TRACE_DEBUG("Launch RAM successful");

             cy_rtos_delay_milliseconds(CYBT_END_DELAY);

             if (cybt_prm_cb.p_cb)
                 (cybt_prm_cb.p_cb)(CYBT_PRM_STS_COMPLETE);

             memset(&cybt_prm_cb, 0, sizeof(cybt_prm_cb_t));

             break;
        default:
             break;
    }
}

/*******************************************************************************
**
** Function         cybt_prm_send_next_patch
**
** Description      Send next patch of data: 250 bytes max
**
** Returns          void
**
*******************************************************************************/
static void cybt_prm_send_next_patch(void)
{
    uint8_t   write_ram_vsc[260], *p;
    uint32_t  len, offset = cybt_prm_cb.tx_patch_length;
    uint16_t  vsc_command;

    if (cybt_prm_cb.format_type == CYBT_PRM_FORMAT_HCD)
    {
        p = (uint8_t *)(cybt_prm_cb.p_patch_data + offset);

        STREAM_TO_UINT16 (vsc_command, p);
        STREAM_TO_UINT8  (len, p);

        wiced_bt_dev_vendor_specific_command(vsc_command,
                                             (uint8_t)len,
                                             p,
                                             cybt_prm_command_complete_cback
                                            );

        /* including 3 bytes header */
        cybt_prm_cb.tx_patch_length += (len + 3);

        if ( vsc_command == HCI_VSC_LAUNCH_RAM )
        {
            cybt_prm_cb.state = CYBT_PRM_ST_LAUNCHING_RAM;
        }
    }
    else
    {
        len = (cybt_prm_cb.patch_length - cybt_prm_cb.tx_patch_length);

        if (len > CYBT_MAX_PRM_LEN)
            len = CYBT_MAX_PRM_LEN;

        /* Address in little endian order */
        write_ram_vsc[0] =  (cybt_prm_cb.dest_ram + offset)        & 0xFF;
        write_ram_vsc[1] = ((cybt_prm_cb.dest_ram + offset) >>  8) & 0xFF;
        write_ram_vsc[2] = ((cybt_prm_cb.dest_ram + offset) >> 16) & 0xFF;
        write_ram_vsc[3] = ((cybt_prm_cb.dest_ram + offset) >> 24) & 0xFF;

        memcpy(write_ram_vsc + 4, cybt_prm_cb.p_patch_data + offset, len);

        wiced_bt_dev_vendor_specific_command (HCI_VSC_WRITE_RAM,
                                              (uint8_t) (len+4),
                                              write_ram_vsc,
                                              cybt_prm_command_complete_cback
                                             );

        cybt_prm_cb.tx_patch_length += len;
    }
}


/*******************************************************************************
**
** Function         cybt_prm_load_data
**
** Description      Download data to controller if application need to provide patch data.
**
** Input Param      p_patch_data            - pointer to patch data
**                  patch_data_len          - patch data len
**
** Returns          TRUE if successful, otherwise FALSE
**
*******************************************************************************/
bool cybt_prm_load_data (uint8_t  *p_patch_data,
                                 uint16_t patch_data_len
                                )
{
    /* Only load data if loading minidri or data piece is done */
    if (cybt_prm_cb.state != CYBT_PRM_ST_INITIALIZING_DONE
        && cybt_prm_cb.state != CYBT_PRM_ST_LOADING_DATA_DONE
       )
    {
        return false;
    }

    if (patch_data_len == 0)
    {
        return false;
    }

    cybt_prm_cb.tx_patch_length = 0;
    cybt_prm_cb.p_patch_data = p_patch_data;
    cybt_prm_cb.patch_length = patch_data_len;

    cybt_prm_cb.state = CYBT_PRM_ST_LOADING_DATA;
    /* state could be changed inside cybt_prm_send_next_patch() if file is HCD format */
    cybt_prm_send_next_patch();

    PRM_TRACE_DEBUG("cybt_prm_load_data(): send next patch");

    return true;
}

/*******************************************************************************
**
** Function         cybt_prm_launch_ram
**
** Description      After all patches are download, lauch ram
** Input Param
**
**
** Returns          TRUE if successful, otherwise FALSE
**
*******************************************************************************/
bool cybt_prm_launch_ram(void)
{
    /* Only launch ram if data piece is done */
    if (cybt_prm_cb.state != CYBT_PRM_ST_LOADING_DATA_DONE)
    {
        return false;
    }

    PRM_TRACE_DEBUG("Externl patch download completed, starting launching ram");

    cybt_prm_cb.state = CYBT_PRM_ST_LAUNCHING_RAM;
    cybt_prm_launch_ram_internal();

    return true;
}

/*******************************************************************************
**
** Function         cybt_prm_launch_ram
**
** Description      Launch RAM
**
** Returns          void
**
*******************************************************************************/
static void cybt_prm_launch_ram_internal(void)
{
    uint8_t write_launch_ram_vsc[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    wiced_bt_dev_vendor_specific_command(HCI_VSC_LAUNCH_RAM,
                                         4,
                                         write_launch_ram_vsc,
                                         cybt_prm_command_complete_cback
                                        );
}


/*******************************************************************************
**
** Function         cybt_prm_get_state
**
** Description      get patch ram state info
** Input Param
**
**
** Returns          patch ram state
**
*******************************************************************************/
uint8_t cybt_prm_get_state(void)
{
    return (cybt_prm_cb.state);
}

