/** @file mlan_init.h
 *
 *  @brief This file defines the FW initialization data
 *  structures.
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
 *
 *  MARVELL CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Material") are owned by Marvell International Ltd or its
 *  suppliers or licensors. Title to the Material remains with Marvell International Ltd
 *  or its suppliers and licensors. The Material contains trade secrets and
 *  proprietary and confidential information of Marvell or its suppliers and
 *  licensors. The Material is protected by worldwide copyright and trade secret
 *  laws and treaty provisions. No part of the Material may be used, copied,
 *  reproduced, modified, published, uploaded, posted, transmitted, distributed,
 *  or disclosed in any way without Marvell's prior express written permission.
 * 
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by Marvell in writing.
 *
 */

/******************************************************
Change log:
    10/13/2008: initial version
******************************************************/

#ifndef _MLAN_INIT_H_
#define _MLAN_INIT_H_

/** Tx buffer size for firmware download*/
#define FW_DNLD_TX_BUF_SIZE	620
/** Rx buffer size for firmware download*/
#define FW_DNLD_RX_BUF_SIZE       2048
/** Max firmware retry */
#define MAX_FW_RETRY        	3

/** Firmware has last block */
#define FW_HAS_LAST_BLOCK		0x00000004

/** Firmware data transmit size */
#define FW_DATA_XMIT_SIZE \
	sizeof(FWHeader) + DataLength + sizeof(t_u32)

/** FWHeader */
typedef struct _FWHeader
{
    /** FW download command */
    t_u32 dnld_cmd;
    /** FW base address */
    t_u32 base_addr;
    /** FW data length */
    t_u32 data_length;
    /** FW CRC */
    t_u32 crc;
} FWHeader;

/** FWData */
typedef struct _FWData
{
    /** FW data header */
    FWHeader fw_header;
    /** FW data sequence number */
    t_u32 seq_num;
    /** FW data buffer */
    t_u8 data[1];
} FWData;

/** FWSyncHeader */
typedef struct _FWSyncHeader
{
    /** FW sync header command */
    t_u32 cmd;
    /** FW sync header sequence number */
    t_u32 seq_num;
} FWSyncHeader;

#ifdef BIG_ENDIAN_SUPPORT
/** Convert sequence number and command fields of fwheader to correct endian format */
#define endian_convert_syncfwheader(x)  { \
		(x)->cmd = wlan_le32_to_cpu((x)->cmd); \
		(x)->seq_num = wlan_le32_to_cpu((x)->seq_num); \
	}
#else
/** Convert sequence number and command fields of fwheader to correct endian format */
#define endian_convert_syncfwheader(x)
#endif /* BIG_ENDIAN_SUPPORT */

#endif /* _MLAN_INIT_H_ */
