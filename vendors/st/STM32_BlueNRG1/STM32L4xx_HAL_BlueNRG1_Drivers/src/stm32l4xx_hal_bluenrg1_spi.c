/**
  ******************************************************************************
  * @file    stm32l1xx_hal_bluenrg_spi.c
  * @author  AMS VMA RF Application Team
  * @version V1.0.0
  * @date    29-May-2015
  * @brief   HAL specific implementation for SPI communication with BlueNRG, 
  *          BlueNRG-MS for optimizing the BLE throughput
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void SPI_I2S_SendData(SPI_HandleTypeDef *hspi, uint8_t data)
{
  //hspi->Instance->DR = data;
  *((__IO uint8_t *)&hspi->Instance->DR) = data;
}

static  uint8_t SPI_I2S_ReceiveData(SPI_HandleTypeDef *hspi)
{
  //return hspi->Instance->DR;
  return (uint8_t)(READ_REG(hspi->Instance->DR));
}


/* Private macros ------------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/


/**
  * @brief  Transmit and Receive an amount of data in blocking mode 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer to be
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_Opt(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData, uint8_t Size)
{
  uint8_t i;
  
  for (i = 0; i < Size; i++) {
    SPI_I2S_SendData(hspi, *pTxData++); 
    while(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE) == RESET);
    while(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE) == RESET);
    *pRxData++ = SPI_I2S_ReceiveData(hspi);
  }
  
  return HAL_OK;
}

/**
  * @brief  Transmit an amount of data in blocking mode (optimized version)
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_Opt(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t Size)
{
  uint8_t i;
  
  for (i = 0; i < Size; i++) {
    SPI_I2S_SendData(hspi, *pTxData++); 
    while(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE) == RESET);
    while(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE) == RESET);
    SPI_I2S_ReceiveData(hspi);
  }
  
  return HAL_OK;
}

/**
  * @brief  Receive an amount of data in blocking mode (optimized version)
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive_Opt(SPI_HandleTypeDef *hspi, uint8_t *pRxData, uint8_t Size)
{
  uint8_t i;
  
  for (i = 0; i < Size; i++) {
    SPI_I2S_SendData(hspi, 0x00); 
    while(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE) == RESET);
    while(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE) == RESET);
    *pRxData++ = SPI_I2S_ReceiveData(hspi);
  }
  
  return HAL_OK;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
