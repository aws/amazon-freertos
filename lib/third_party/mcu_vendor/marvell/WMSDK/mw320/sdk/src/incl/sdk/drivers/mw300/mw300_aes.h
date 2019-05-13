/** @file mw300_aes.h
*
*  @brief This file contains AES driver module header
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

#ifndef __MW300_AES_H__
#define __MW300_AES_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  AES 
 *  @{
 */
  
/** @defgroup AES_Public_Types AES_Public_Types
 *  @brief AES configuration structure type definition
 *  @{
 */
 
/**  
 *  @brief AES mode type definition 
 */
typedef enum
{
  AES_MODE_ECB    = 0,	                /*!< AES mode: ECB */
  AES_MODE_CBC    = 1,                  /*!< AES mode: CBC */
  AES_MODE_CTR    = 2,                  /*!< AES mode: CTR */
  AES_MODE_CCM    = 5,                  /*!< AES mode: CCM */ 
  AES_MODE_MMO    = 6,                  /*!< AES mode: MMO */
  AES_MODE_BYPASS = 7,                  /*!< AES mode: Bypass */
}AES_Mode_Type;

/**  
 *  @brief AES enc/dec select type definition 
 */
typedef enum
{
  AES_MODE_ENCRYPTION    = 0,           /*!< AES ENC/DEC select: ENCRYPTION */
  AES_MODE_DECRYPTION    = 1,           /*!< AES ENC/DEC select: DECRYPTION */
}AES_EncDecSel_Type;

/**  
 *  @brief AES key size type definition 
 */
typedef enum
{
  AES_KEY_BYTES_16       = 0,           /*!< AES key size select: 16 bytes */
  AES_KEY_BYTES_32       = 1,           /*!< AES key size select: 32 bytes */
  AES_KEY_BYTES_24       = 2,           /*!< AES key size select: 24 bytes */
}AES_KeySize_Type;

/**  
 *  @brief AES MIC size type definition 
 */
typedef enum
{
  AES_MIC_BYTES_0        = 0,           /*!< AES MIC length: 0 bytes */
  AES_MIC_BYTES_4        = 1,           /*!< AES MIC length: 4 bytes */
  AES_MIC_BYTES_8        = 2,           /*!< AES MIC length: 8 bytes */
  AES_MIC_BYTES_16       = 3,           /*!< AES MIC length: 16 bytes */    
}AES_MICLen_Type;

/**  
 *  @brief AES interrupt type definition 
 */
typedef enum
{
  AES_INT_OUTFIFO_EMPTY,                /*!< AES output FIFO empty int flag */
  AES_INT_INFIFO_FULL,                  /*!< AES input FIFO full int flag */
  AES_INT_DONE,                         /*!< AES done int flag */
  AES_INT_ALL,                          /*!< All AES interrupt flag */
}AES_INT_Type; 

/**  
 *  @brief AES status type definition 
 */
typedef enum
{
  AES_STATUS_OUTFIFO_EMPTY,             /*!< AES output FIFO empty flag */
  AES_STATUS_INFIFO_FULL,               /*!< AES input FIFO full flag */
  AES_STATUS_OUTFIFO_RDY,               /*!< AES output FIFO can be read flag */
  AES_STATUS_DONE,                      /*!< AES done flag */
  AES_STATUS_ERROR_0,                   /*!< Input stream size is less than 16 bytes in CBC&CTR mode */
  AES_STATUS_ERROR_1,                   /*!< Data is more than 2^13-1 bytes in MMO mode 
					     Data is not multiple of 16 bytes in ECB mode*/
  AES_STATUS_ERROR_2,                   /*!< MIC mismatch during decryption in CCM* mode */
}AES_Status_Type; 

/**  
 *  @brief AES config type definition 
 */
typedef struct
{
  AES_Mode_Type mode;                   /*!< AES mode */
  AES_EncDecSel_Type encDecSel;         /*!< AES ENC/DEC select */
  uint32_t initVect[4];                 /*!< AES initial vector variable */
  AES_KeySize_Type keySize;             /*!< key size variable */
  uint32_t key[8];                      /*!< AES key */
  uint32_t aStrLen;                     /*!< AES A_string length */
  uint32_t mStrLen;                     /*!< AES M_string length */
  AES_MICLen_Type micLen;               /*!< AES MIC length */
  FunctionalState micEn;                /*!< AES MIC Enable/Disable control */ 
}AES_Config_Type;

/*@} end of group AES_Public_Types definitions */


/** @defgroup AES_Public_Constants
 *  @{
 */ 

/*@} end of group AES_Public_Constants */

/** @defgroup AES_Public_Macro
 *  @{
 */


/*@} end of group AES_Public_Macro */

/** @defgroup AES_Public_Function_Declaration
 *  @brief AES functions statement
 *  @{
 */

void AES_Init(AES_Config_Type * aesConfig);
void AES_Reset(void);
void AES_Enable(void);
void AES_Disable(void);
void AES_SetCTRCntMod(uint32_t cntmod);
void AES_OutmsgCmd(FunctionalState state);
void AES_DmaCmd(FunctionalState state);
void AES_FeedData(uint32_t data);
uint32_t AES_ReadData(void);
void AES_ReadMIC(uint32_t* micData, uint8_t len);
void AES_ClrInputFIFO(void);
void AES_ClrOutputFIFO(void);
uint32_t AES_GetCTRCntMod(void);
void AES_ReadOutputVector(uint32_t *pIV);
void AES_SetIV(const uint32_t *pIV);
void AES_SetMsgLen(uint32_t msgLen);
void AES_IntMask(AES_INT_Type intType, IntMask_Type maskState);
IntStatus AES_GetIntStatus(AES_INT_Type intType);
FlagStatus AES_GetStatus(AES_Status_Type statusType);

void AES_IntClr(AES_INT_Type intType);

void AES_IRQHandler(void);

/*@} end of group AES_Public_Function_Declaration */

/*@} end of group AES  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_AES_H__ */
