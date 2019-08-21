/** @file mw300_bg.h
*
*  @brief This file contains BG driver module header
*
*  (C) Copyright 2012-2018 Marvell International Ltd. All Rights Reserved
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

/****************************************************************************//*
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_BG_H__
#define __MW300_BG_H__

/* Includes ------------------------------------------------------------------ */
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  BG 
 *  @{
 */
  
/** @defgroup BG_Public_Types BG_Public_Types
 *  @{
 */

/*@} end of group BG_Public_Types definitions */

/** @defgroup BG_Public_Constants
 *  @{
 */ 

/*@} end of group BG_Public_Constants */

/** @defgroup BG_Public_Macro
 *  @{
 */

/*@} end of group BG_Public_Macro */

/** @defgroup BG_Public_FunctionDeclaration
 *  @brief BG functions statement
 *  @{
 */
void BG_PowerUp(void);
void BG_PowerDown(void);

/*@} end of group BG_Public_FunctionDeclaration */

/*@} end of group BG */

/*@} end of group MW300_Periph_Driver */
#endif
