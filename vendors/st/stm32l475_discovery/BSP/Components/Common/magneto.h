/**
  ******************************************************************************
  * @file    magneto.h
  * @author  MCD Application Team
  * @brief   This header file contains the functions prototypes for the MAGNETO driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAGNETO_H
#define __MAGNETO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */
    
/** @addtogroup MAGNETO
  * @{
  */

/** @defgroup MAGNETO_Exported_Types
  * @{
  */ 

/** @defgroup MAGNETO_Config_structure  Magnetometer Configuration structure
  * @{
  */
typedef struct
{
  uint8_t Register1;
  uint8_t Register2;
  uint8_t Register3;
  uint8_t Register4;
  uint8_t Register5;
}MAGNETO_InitTypeDef;
/**
  * @}
  */

/** @defgroup MAGNETO_Driver_structure  Magnetometer Driver structure
  * @{
  */
typedef struct
{  
  void      (*Init)(MAGNETO_InitTypeDef);
  void      (*DeInit)(void); 
  uint8_t   (*ReadID)(void);
  void      (*Reset)(void);
  void      (*LowPower)(uint16_t);
  void      (*ConfigIT)(void);
  void      (*EnableIT)(uint8_t);
  void      (*DisableIT)(uint8_t);
  uint8_t   (*ITStatus)(uint16_t);
  void      (*ClearIT)(void);
  void      (*FilterConfig)(uint8_t);
  void      (*FilterCmd)(uint8_t);
  void      (*GetXYZ)(int16_t *);
}MAGNETO_DrvTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __MAGNETO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
