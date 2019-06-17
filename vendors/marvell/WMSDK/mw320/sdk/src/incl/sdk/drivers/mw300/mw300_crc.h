/** @file mw300_crc.h
*
*  @brief This file contains CRC driver module header
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

#ifndef __MW300_CRC_H__
#define __MW300_CRC_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  CRC 
 *  @{
 */
  
/** @defgroup CRC_Public_Types CRC_Public_Types
 *  @brief CRC configuration structure type definition
 *  @{
 */
 
/**  
 *  @brief CRC mode type definition 
 */
typedef enum
{
  CRC_16_CCITT   = 0,                  /*!< CRC mode: CRC-16-CCITT */
  CRC_16_IBM     = 1,                  /*!< CRC mode: CRC-16-IBM */
  CRC_16_T10_DIF = 2,                  /*!< CRC mode: CRC-16-T10-DIF */
  CRC_32_IEEE    = 3,                  /*!< CRC mode: CRC-32-IEEE */
  CRC_16_DNP     = 4,                  /*!< CRC mode: CRC-16-DNP */
}CRC_Mode_Type;

/*@} end of group CRC_Public_Types definitions */


/** @defgroup CRC_Public_Constants
 *  @{
 */ 


/*@} end of group CRC_Public_Constants */

/** @defgroup CRC_Public_Macro
 *  @{
 */


/*@} end of group CRC_Public_Macro */

/** @defgroup CRC_Public_Function_Declaration
 *  @brief CRC functions statement
 *  @{
 */

void CRC_SetMode(CRC_Mode_Type mode);
void CRC_Enable(void);
void CRC_Disable(void);
void CRC_FeedData(uint32_t data);
void CRC_SetStreamLen(uint32_t strLen);
uint32_t CRC_GetResult(void);
uint32_t CRC_Calculate(const uint8_t *dataStr, uint32_t dataLen);
void CRC_Stream_Feed(const uint8_t *dataStr, uint32_t dataLen);

void CRC_IntMask(IntMask_Type newState);
IntStatus CRC_GetIntStatus(void);
FlagStatus CRC_GetStatus(void);
void CRC_IntClr(void);

void CRC_IRQHandler(void);

/*@} end of group CRC_Public_Function_Declaration */

/*@} end of group CRC  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_CRC_H__ */
