/** @file mlan_join.h
 *
 *  @brief This file defines the interface for the WLAN infrastructure
 *  and adhoc join routines.
 *
 *  Driver interface functions and type declarations for the join module
 *  implemented in mlan_join.c.  Process all start/join requests for
 *  both adhoc and infrastructure networks
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
 *  @sa mlan_join.c
 */

/******************************************************
Change log:
    10/13/2008: initial version
******************************************************/

#ifndef _MLAN_JOIN_H_
#define _MLAN_JOIN_H_

/** Size of buffer allocated to store the association response from firmware */
#define MRVDRV_ASSOC_RSP_BUF_SIZE  500

/** Size of buffer allocated to store IEs passed to firmware in the assoc req */
#define MRVDRV_GENIE_BUF_SIZE      256

#endif /* _MLAN_JOIN_H_ */
