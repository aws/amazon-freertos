/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *  Provides generic clm blob file download functionality
 */
/* For ModusToolBox builds */
#if defined(WWD_DOWNLOAD_CLM_BLOB)
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "wwd_assert.h"
#include "wwd_debug.h"
#include "resources.h"
#include "internal/wwd_sdpcm.h"
#include "internal/wwd_internal.h"
#include "internal/wwd_clm.h"
#include "network/wwd_buffer_interface.h"
#include "network/wwd_network_constants.h"

/******************************************************
 * @cond       Constants
 ******************************************************/

/*
 * MAX_CHUNK_LEN is the amount of the clm file we send in each
 * ioctl.  It is relatively small because dongles (FW) have a small
 * maximum size input payload restriction for ioctl's ... something
 * like 1900'ish bytes.  Separately WWD's local ioctl buffer, used
 * for holding both the input and output data, has a compiled in
 * max size.  This value, WICED_PAYLOAD_MTU, is commonly 1500 bytes.
 * When WICED_PAYLOAD_MTU is smaller than 1500, shrink MAX_CHUNK_LEN accordingly.
*/

#define MAX_CHUNK_LEN ( (WICED_PAYLOAD_MTU < 1500 ? WICED_PAYLOAD_MTU : 1500) - 100 )

/******************************************************
 * @cond       Macros
 ******************************************************/

#define CHECK_IOCTL_BUFFER( buff )  if ( buff == NULL ) {  wiced_assert("Allocation failed\n", 0 == 1); return WWD_BUFFER_ALLOC_FAIL; }
#define CHECK_RETURN( expr )  { wwd_result_t check_res = (expr); if ( check_res != WWD_SUCCESS ) { wiced_assert("Command failed\n", 0 == 1); return check_res; } }


/*
Generic interface for downloading required data onto the dongle
*/
static int wwd_download2dongle(wwd_interface_t interface, const char *iovar, uint16_t flag, uint16_t dload_type,
    unsigned char *dload_buf, uint32_t len)
{
    wl_dload_data_t *dload_ptr = (wl_dload_data_t *)dload_buf;
    unsigned int dload_data_offset;
    wiced_buffer_t buffer;
    uint32_t*      iov_data;

    dload_data_offset = offsetof(wl_dload_data_t, data);
    dload_ptr->flag = (DLOAD_HANDLER_VER << DLOAD_FLAG_VER_SHIFT) | flag;
    dload_ptr->dload_type = dload_type;
    dload_ptr->len = htod32(len - dload_data_offset);

    dload_ptr->crc = 0;

    wiced_assert("dload buffer too large", len < 0xffffffff - 8 );
    len = len + 8 - (len%8);

    iov_data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t)len, iovar );
    CHECK_IOCTL_BUFFER( iov_data );
    memcpy( iov_data, dload_ptr, len);
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface ) );
    return WWD_SUCCESS;
}

wwd_result_t wwd_process_clm_data(void)
{
    wwd_result_t ret = WWD_SUCCESS;
    uint32_t clm_blob_size;
    uint32_t datalen;
    unsigned int size2alloc, data_offset;
    unsigned char *chunk_buf;
    uint16_t dl_flag = DL_BEGIN;
    unsigned int cumulative_len = 0;
    unsigned int chunk_len;
    uint32_t size_read;

    /* clm file size is the initial datalen value which is decremented */
    clm_blob_size = resource_get_size( &wifi_firmware_clm_blob );
    datalen = clm_blob_size;

    data_offset = offsetof(wl_dload_data_t, data);
    size2alloc = data_offset + MAX_CHUNK_LEN;

    if ((chunk_buf = (unsigned char *)malloc(size2alloc)) != NULL) {
        memset(chunk_buf, 0, size2alloc);

        do {
            if (datalen >= MAX_CHUNK_LEN)
                chunk_len = MAX_CHUNK_LEN;
            else
                chunk_len = datalen;

            // check size_read is full value also and resource read return
            if ((ret = resource_read(&wifi_firmware_clm_blob, cumulative_len, chunk_len, &size_read, chunk_buf + data_offset)) != WWD_SUCCESS) {
                break;
            }

            if (size_read != chunk_len) {
                wiced_assert("During CLM download, resource_read() returned less of the file than should be available\n", 1 == 0);
                ret = WWD_CLM_BLOB_DLOAD_ERROR;
                break;
            }

            cumulative_len += chunk_len;

            if (datalen - chunk_len == 0)
                dl_flag |= DL_END;

            ret = wwd_download2dongle(WWD_STA_INTERFACE, IOVAR_STR_CLMLOAD, dl_flag, DL_TYPE_CLM,
                chunk_buf, data_offset + chunk_len);
            dl_flag &= (uint16_t)~DL_BEGIN;

            datalen = datalen - chunk_len;
        } while ((datalen > 0) && (ret == WWD_SUCCESS));

        free(chunk_buf);
        if ( ret != WWD_SUCCESS )
        {
            wwd_result_t ret_clmload_status;
            wiced_buffer_t buffer;
            wiced_buffer_t response;
            void *data;

            WPRINT_WWD_DEBUG(("clmload (%ld byte file) failed with return %d; ", clm_blob_size, ret));
            data = (int*)wwd_sdpcm_get_iovar_buffer( &buffer, 4, IOVAR_STR_CLMLOAD_STATUS );
            CHECK_IOCTL_BUFFER( data );
            ret_clmload_status = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE );
            if ( ret_clmload_status != WWD_SUCCESS )
            {
                WPRINT_WWD_DEBUG(("clmload_status failed with return %d\n", ret_clmload_status));
            }
            else
            {
                uint32_t *clmload_status = (uint32_t *)host_buffer_get_current_piece_data_pointer( response );

                if ( clmload_status != NULL )
                {
                    WPRINT_WWD_DEBUG(("clmload_status is %lu\n", *clmload_status));
                    host_buffer_release( response, WWD_NETWORK_RX );
                }
            }
        }
    } else {
        ret = WWD_MALLOC_FAILURE;
    }

    return ret;
}
#endif /* defined(WWD_DOWNLOAD_CLM_BLOB) */
