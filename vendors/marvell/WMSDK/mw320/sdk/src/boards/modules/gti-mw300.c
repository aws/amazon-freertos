/** @file gti-mw300.c
*
*  @brief This file contains module specific configuration for the
*         RD-88MW300 module based on schematic dated 19th June 2014
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

#include <wmtypes.h>
#include <wmerrno.h>
#include <wm_os.h>
#include <board.h>
#include <lowlevel_drivers.h>

int board_main_xtal()
{
	/* MAINXTAL: 38.4MHZ */
	return 38400000;
}

int board_main_osc()
{
	return -WM_FAIL;
}

int board_antenna_switch_ctrl()
{
	return true;
}

struct pwr_table *board_region_pwr_tbl(board_country_code_t country)
{
	return NULL;
}
