/** @file mdev_iflash.h
*
*  @brief iFlash Header
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/



#ifndef _MDEV_IFLASH_H_
#define _MDEV_IFLASH_H_

#include <mdev.h>
#include <lowlevel_drivers.h>
#include <wmlog.h>

int iflash_drv_init(void);
mdev_t *iflash_drv_open(const char *name, uint32_t flags);
int iflash_drv_close(mdev_t *dev);
int iflash_drv_eraseall(mdev_t *dev);
int iflash_drv_erase(mdev_t *dev, unsigned long start, unsigned long size);
int iflash_drv_read(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr);
int iflash_drv_write(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr);
int iflash_drv_get_id(mdev_t *dev, uint64_t *id);
#endif /* _MDEV_IFLASH_H_ */
