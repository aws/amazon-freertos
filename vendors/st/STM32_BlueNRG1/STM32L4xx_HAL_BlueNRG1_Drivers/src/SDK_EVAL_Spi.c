/**
 * @file    SDK_EVAL_SPI_Driver.c
 * @author  AMS VMA RF application team
 * @version V1.0.3
 * @date    May 15, 2015
 * @brief   This file provides all the low level SPI API to access to BlueNRG module
 * @details It uses STM32L1 Cube APIs + Optimized APIs for SPI Transmit, Receive,
 *          Transmit & Receive (these APIs are needed for BLE communication
 *          throughput)
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
 *
 * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "clock.h" 
#include "gp_timer.h"
#include "stm32l4xx_hal_bluenrg1_spi.h"
#include "SDK_EVAL_Spi.h"
/** @addtogroup SDK_EVAL_STM32L
 * @{
 */

/** @addtogroup SDK_EVAL_Spi                    SDK EVAL Spi
 * @brief SPI functions implementation.
 * @details This file implements the BlueNRG Library SPI interface functions.
 * @{
 */

/** @defgroup STM32_BLUENRG_BLE_Private_Defines 
 * @{
 */

#define HEADER_SIZE 5
#define MAX_BUFFER_SIZE 255
#define TIMEOUT_DURATION 15

/**
 * @}
 */

/** @defgroup SPI_Private_Variables
 * @{
 */
SPI_HandleTypeDef SpiHandle;

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

/**
 * @}
 */

/** @defgroup SPI_Functions
 * @{
 */

/**
 * @brief  Basic SPI configuration according to the selected platform
 * @param  None
 * @retval status
 */
static int8_t SPI_Configure_Platform(void) {
	/* Enable GPIO Ports Clock */
	BNRG_SPI_RESET_CLK_ENABLE()
	;
	BNRG_SPI_SCLK_CLK_ENABLE()
	;
	BNRG_SPI_MISO_CLK_ENABLE()
	;
	BNRG_SPI_MOSI_CLK_ENABLE()
	;
	BNRG_SPI_CS_CLK_ENABLE()
	;
	BNRG_SPI_IRQ_CLK_ENABLE()
	;

	/* Enable SPI clock */
	BNRG_SPI_CLK_ENABLE()
	;

	return (0);
}/* end SPI_Configure_Platform() */

/**
 * @brief  Return SPI IRQ pin value
 * @param  None
 * @retval SPI IRQ pin value
 */
uint8_t SdkEvalSPI_Irq_Pin(void) {
	return (HAL_GPIO_ReadPin(BNRG_SPI_IRQ_PORT, BNRG_SPI_IRQ_PIN));

}/* end SdkEvalSPI_Irq_Pin() */

/**
 * @brief  Initializes the SPI
 * @param  xSpiMode: SPI mode - GPIO (polling) or EXTI (IRQ)
 * @retval status
 */
int8_t SdkEvalSpiInit(SdkEvalSpiMode xSpiMode) {
	int8_t retValue;
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure SPI initialization parameters depending on the selected
	 platform: BlueNRG EVal Kits or User Defined platforms */
	retValue = SPI_Configure_Platform();
	if (retValue == -1)
		/* BlueNRG Development Kit platfrom is not supported */
		return (retValue);

	/* SCLK */
	GPIO_InitStruct.Pin = BNRG_SPI_SCLK_PIN;
	GPIO_InitStruct.Mode = BNRG_SPI_SCLK_MODE;
	GPIO_InitStruct.Pull = BNRG_SPI_SCLK_PULL;
	GPIO_InitStruct.Speed = BNRG_SPI_SCLK_SPEED;
	GPIO_InitStruct.Alternate = BNRG_SPI_SCLK_ALTERNATE;
	HAL_GPIO_Init(BNRG_SPI_SCLK_PORT, &GPIO_InitStruct);

	/* MISO */
	GPIO_InitStruct.Pin = BNRG_SPI_MISO_PIN;
	GPIO_InitStruct.Mode = BNRG_SPI_MISO_MODE;
	GPIO_InitStruct.Pull = BNRG_SPI_MISO_PULL;
	GPIO_InitStruct.Speed = BNRG_SPI_MISO_SPEED;
	GPIO_InitStruct.Alternate = BNRG_SPI_MISO_ALTERNATE;
	HAL_GPIO_Init(BNRG_SPI_MISO_PORT, &GPIO_InitStruct);

	/* MOSI */
	GPIO_InitStruct.Pin = BNRG_SPI_MOSI_PIN;
	GPIO_InitStruct.Mode = BNRG_SPI_MOSI_MODE;
	GPIO_InitStruct.Pull = BNRG_SPI_MOSI_PULL;
	GPIO_InitStruct.Speed = BNRG_SPI_MOSI_SPEED;
	GPIO_InitStruct.Alternate = BNRG_SPI_MOSI_ALTERNATE;
	HAL_GPIO_Init(BNRG_SPI_MOSI_PORT, &GPIO_InitStruct);

	/* NSS/CSN/CS */
	GPIO_InitStruct.Pin = BNRG_SPI_CS_PIN;
	GPIO_InitStruct.Mode = BNRG_SPI_CS_MODE;
	GPIO_InitStruct.Pull = BNRG_SPI_CS_PULL;
	GPIO_InitStruct.Speed = BNRG_SPI_CS_SPEED;
	GPIO_InitStruct.Alternate = BNRG_SPI_CS_ALTERNATE;
	HAL_GPIO_Init(BNRG_SPI_CS_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

	/* IRQ -- INPUT */
	GPIO_InitStruct.Pin = BNRG_SPI_IRQ_PIN;
	GPIO_InitStruct.Mode = BNRG_SPI_IRQ_MODE;
	GPIO_InitStruct.Pull = BNRG_SPI_IRQ_PULL;
	GPIO_InitStruct.Speed = BNRG_SPI_IRQ_SPEED;
	GPIO_InitStruct.Alternate = BNRG_SPI_IRQ_ALTERNATE;
	HAL_GPIO_Init(BNRG_SPI_IRQ_PORT, &GPIO_InitStruct);

	SpiHandle.Instance = BNRG_SPI_INSTANCE;
	SpiHandle.Init.Mode = BNRG_SPI_MODE;
	SpiHandle.Init.Direction = BNRG_SPI_DIRECTION;
	SpiHandle.Init.DataSize = BNRG_SPI_DATASIZE;
	SpiHandle.Init.CLKPolarity = BNRG_SPI_CLKPOLARITY;
	SpiHandle.Init.CLKPhase = BNRG_SPI_CLKPHASE;
	SpiHandle.Init.NSS = BNRG_SPI_NSS;
	SpiHandle.Init.FirstBit = BNRG_SPI_FIRSTBIT;
	SpiHandle.Init.TIMode = BNRG_SPI_TIMODE;
	SpiHandle.Init.CRCPolynomial = BNRG_SPI_CRCPOLYNOMIAL;

	SpiHandle.Init.BaudRatePrescaler = BNRG_SPI_BAUDRATEPRESCALER;

	HAL_SPI_Init(&SpiHandle);

	if (xSpiMode == SPI_MODE_EXTI) {
		/* Configure the NVIC for SPI */
		HAL_NVIC_SetPriority(BNRG_SPI_EXTI_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(BNRG_SPI_EXTI_IRQn);
	} else if (xSpiMode == SPI_MODE_DMA) {
		/* Enable DMA clock */
		__HAL_RCC_DMA1_CLK_ENABLE()
		;

		/*##-3- Configure the DMA ##################################################*/
		/* Configure the DMA handler for Transmission process */
		hdma_tx.Instance = DMA1_Channel3;
		hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_tx.Init.Mode = DMA_NORMAL;
		hdma_tx.Init.Priority = DMA_PRIORITY_LOW;

		HAL_DMA_Init(&hdma_tx);

		/* Associate the initialized DMA handle to the the SPI handle */
		__HAL_LINKDMA(&SpiHandle, hdmatx, hdma_tx);

		/* Configure the DMA handler for Reception process */
		hdma_rx.Instance = DMA1_Channel2;

		hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_rx.Init.Mode = DMA_NORMAL;
		hdma_rx.Init.Priority = DMA_PRIORITY_HIGH;

		HAL_DMA_Init(&hdma_rx);

		/* Associate the initialized DMA handle to the the SPI handle */
		__HAL_LINKDMA(&SpiHandle, hdmarx, hdma_rx);

		/*##-4- Configure the NVIC for DMA #########################################*/
		/* NVIC configuration for DMA transfer complete interrupt (SPI3_TX) */
		HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 1, 1);
		HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

		/* NVIC configuration for DMA transfer complete interrupt (SPI3_RX) */
		HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

	}

	/* Added HAP to enable SPI since Optimized SPI Transmit, Receive and Transmit/Receive APIs are
	 used for BlueNRG, BlueNRG-MS SPI communication in order to get the best performance in terms of
	 BLE throughput */
	__HAL_SPI_ENABLE(&SpiHandle);

	return (0);
}/* end SdkEvalSpiInit() */

/**
 * @brief  Set GPIO pin in Hi-Z state.
 * @param  GPIOx: GPIO port
 *         GPIO_Pin: GPIo pin
 * @retval None
 */
static void GPIO_SetHiZ(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure GPIO pin as output open drain HI-Z state */
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);

}/* end GPIO_SetHiZ() */

/**
 * @brief  Disable SPI
 * @param  None
 * @retval None
 */
void SdkEvalSpiDisable(void) {
	BNRG_SPI_CLK_DISABLE(); /*!< SPI disable */

	/* Configure pins as output open drain HI-Z state */
	GPIO_SetHiZ(BNRG_SPI_SCLK_PORT, BNRG_SPI_SCLK_PIN);
	GPIO_SetHiZ(BNRG_SPI_MOSI_PORT, BNRG_SPI_MOSI_PIN);
	GPIO_SetHiZ(BNRG_SPI_MISO_PORT, BNRG_SPI_MISO_PIN);
	GPIO_SetHiZ(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN);
	HAL_NVIC_DisableIRQ(BNRG_SPI_EXTI_IRQn);

}/* end SdkEvalSpiDisable() */

/**
 * @brief  Reports if the BlueNRG has data for the host micro.
 * @param  None
 * @retval TRUE if data are present, FALSE otherwise
 */
uint8_t BlueNRG_DataPresent(void) {
	if (HAL_GPIO_ReadPin(BNRG_SPI_IRQ_PORT, BNRG_SPI_IRQ_PIN) == GPIO_PIN_SET)
		return TRUE;
	else
		return FALSE;
} /* end BlueNRG_DataPresent() */

/**
 * @brief  Disable SPI IRQ
 * @param  None
 * @retval None
 */
void Disable_SPI_IRQ(void) {
	HAL_NVIC_DisableIRQ(BNRG_SPI_EXTI_IRQn);
}/* end Disable_SPI_IRQ() */

/**
 * @brief  Enable SPI IRQ
 * @param  None
 * @retval None
 */
void Enable_SPI_IRQ(void) {
	HAL_NVIC_EnableIRQ(BNRG_SPI_EXTI_IRQn);
}/* end Enable_SPI_IRQ() */

/**
 * @brief  Clear Pending SPI IRQ
 * @param  None
 * @retval None
 */
void Clear_SPI_IRQ(void) {
	HAL_NVIC_ClearPendingIRQ(BNRG_SPI_EXTI_IRQn);
}/* end Clear_SPI_IRQ() */

/**
 * @brief  Clear Exti line for SPI IRQ
 * @param  None
 * @retval None
 */
void Clear_SPI_EXTI_Flag() {
	__HAL_GPIO_EXTI_CLEAR_IT(BNRG_SPI_EXTI_PIN);
}/* end Clear_SPI_EXTI_Flag() */

int32_t BlueNRG_SPI_Write_Raw(uint8_t* data1, uint8_t Nb_bytes1) {
	uint32_t i;
	int32_t result = 0;

	unsigned char header_master[5] = { 0x0a, 0x00, 0x00, 0x00, 0x00 };
	unsigned char header_slave[5] = { 0xaa, 0x00, 0x00, 0x00, 0x00 };
	unsigned char read_char_buf[MAX_BUFFER_SIZE];

	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_RESET);

	/* Read the header */
	HAL_SPI_TransmitReceive(&SpiHandle, header_master, header_slave, HEADER_SIZE, TIMEOUT_DURATION);

	//  if (header_slave[0] == 0x02) {
	// SPI is ready
	if (header_slave[1] >= Nb_bytes1) {
		//  Buffer is big enough
		for (i = 0; i < Nb_bytes1; i++) {
			HAL_SPI_TransmitReceive(&SpiHandle, data1 + i, read_char_buf, 1, TIMEOUT_DURATION);
		}
	} else {
		// Buffer is too small
		result = -2;
	}
	//  } else {
	//    // SPI is not ready
	//    result = -1;
	//  }

	// Release CS line.
	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

	return result;
}

/**
 * @brief  Read from BlueNRG SPI buffer and store data into local buffer
 * @param  buffer:    buffer where data from SPI are stored
 *         buff_size: buffer size
 * @retval number of read bytes
 */
uint8_t BlueNRG_SPI_Read_All(uint8_t *buffer, uint8_t buff_size) {
	uint16_t byte_count;
	uint8_t len = 0;

	const uint8_t header_master[5] = { 0x0b, 0x00, 0x00, 0x00, 0x00 };
	uint8_t header_slave[5];

	__disable_irq();

	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_RESET);

	/* Read the header */
	HAL_SPI_TransmitReceive_Opt(&SpiHandle, header_master, header_slave, HEADER_SIZE);

	byte_count = ((uint16_t) header_slave[4]) << 8 | (uint16_t) header_slave[3];

	if (byte_count > 0) {
		if (byte_count > buff_size)
			byte_count = buff_size;

		HAL_SPI_Receive_Opt(&SpiHandle, buffer, byte_count);

		len = byte_count;
	}
	/* To be aligned to the SPI protocol. Can bring to a delay inside the frame, due to the BlueNRG-1 that needs to check if the header is received or not */
	while (SdkEvalSPI_Irq_Pin() != GPIO_PIN_RESET) {
	}

	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

	__enable_irq();

	return len;

}/* end BlueNRG_SPI_Read_All() */

/**
 * @brief  Write data from local buffer to SPI
 * @param  data1:    first data buffer to be written, used to send header of higher
 *                   level protocol
 *         data2:    second data buffer to be written, used to send payload of higher
 *                   level protocol
 *         Nb_bytes1: size of header to be written
 *         Nb_bytes2: size of payload to be written
 * @retval Number of payload bytes that has been sent. If 0, all bytes in the header has been
 *         written.
 */
int16_t BlueNRG_SPI_Write_Bridge(uint8_t* data, uint16_t Nb_bytes) {
	int16_t result = 0;
	uint16_t rx_bytes;
	struct timer t;

	const uint8_t header_master[5] = { 0x0A, 0x00, 0x00, 0x00, 0x00 };
	uint8_t header_slave[5] = { 0x00 };

	Timer_Set(&t, CLOCK_SECOND / 10);

	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_RESET);

	while (SdkEvalSPI_Irq_Pin() == GPIO_PIN_RESET) {
		if (Timer_Expired(&t)) {
			result = -2;
			goto failed;
		}
	}

	HAL_SPI_TransmitReceive_Opt(&SpiHandle, header_master, header_slave, HEADER_SIZE);

	rx_bytes = (((uint16_t) header_slave[2]) << 8) | ((uint16_t) header_slave[1]);

	if (rx_bytes < Nb_bytes) {
		result = -2;
		goto failed;
	}

	HAL_SPI_Transmit_Opt(&SpiHandle, data, Nb_bytes);

	result = Nb_bytes;

	failed: HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

	return result;
}/* end BlueNRG_SPI_Write() */

/**
 * @brief  Write data from local buffer to SPI
 * @param  data1:    first data buffer to be written, used to send header of higher
 *                   level protocol
 *         data2:    second data buffer to be written, used to send payload of higher
 *                   level protocol
 *         Nb_bytes1: size of header to be written
 *         Nb_bytes2: size of payload to be written
 * @retval Number of payload bytes that has been sent. If 0, all bytes in the header has been
 *         written.
 */
#define SPI_DMA_BUFFER_SIZE (255+4+5)
static uint8_t SpiDmaRxBuffer[SPI_DMA_BUFFER_SIZE];

int16_t BlueNRG_SPI_Read_Bridge(void) {
	int16_t result = -2;
	uint16_t byte_count = 0;

	const uint8_t header_master[5] = { 0x0B, 0x00, 0x00, 0x00, 0x00 };
	uint8_t header_slave[5] = { 0x00 };

	if (SdkEvalSPI_Irq_Pin() == GPIO_PIN_RESET) {
		goto failed;
	}

	__disable_irq();

	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_RESET);

	HAL_SPI_TransmitReceive_Opt(&SpiHandle, header_master, header_slave, HEADER_SIZE);

	byte_count = ((uint16_t) header_slave[4]) << 8 | (uint16_t) header_slave[3];

	if (byte_count > 0) {
		if (byte_count > SPI_DMA_BUFFER_SIZE)
			byte_count = SPI_DMA_BUFFER_SIZE;

		HAL_SPI_Receive_Opt(&SpiHandle, SpiDmaRxBuffer, byte_count);

	}
	result = byte_count;

//  HAL_NVIC_DisableIRQ(UART_IRQ);
	for (uint16_t i = 0; i < byte_count; i++)
		putchar(SpiDmaRxBuffer[i]);
//  HAL_NVIC_EnableIRQ(UART_IRQ);

	__enable_irq();

	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

	failed:

	return result;
}/* end BlueNRG_SPI_Write() */

/**
 * @brief  Write data from local buffer to SPI
 * @param  data1:    first data buffer to be written, used to send header of higher
 *                   level protocol
 *         data2:    second data buffer to be written, used to send payload of higher
 *                   level protocol
 *         Nb_bytes1: size of header to be written
 *         Nb_bytes2: size of payload to be written
 * @retval Number of payload bytes that has been sent. If 0, all bytes in the header has been
 *         written.
 */
int16_t BlueNRG_SPI_Write(uint8_t* data1, uint8_t* data2, uint16_t Nb_bytes1, uint16_t Nb_bytes2) {
	int16_t result = 0;
	uint16_t tx_bytes;
	uint16_t rx_bytes;
	struct timer t;

	const uint8_t header_master[5] = { 0x0a, 0x00, 0x00, 0x00, 0x00 };
	uint8_t header_slave[5] = { 0x00 };

	Timer_Set(&t, CLOCK_SECOND / 10);

	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_RESET);

	while (SdkEvalSPI_Irq_Pin() == GPIO_PIN_RESET) {
		if (Timer_Expired(&t)) {
			result = -2;
			goto failed;
		}
	}

	HAL_SPI_TransmitReceive_Opt(&SpiHandle, header_master, header_slave, HEADER_SIZE);

	rx_bytes = (((uint16_t) header_slave[2]) << 8) | ((uint16_t) header_slave[1]);

	if (rx_bytes < Nb_bytes1) {
		result = -2;
		goto failed;
	}

	HAL_SPI_Transmit_Opt(&SpiHandle, data1, Nb_bytes1);

	rx_bytes -= Nb_bytes1;

	if (Nb_bytes2 > rx_bytes) {
		tx_bytes = rx_bytes;
	} else {
		tx_bytes = Nb_bytes2;
	}

	HAL_SPI_Transmit_Opt(&SpiHandle, data2, tx_bytes);

	result = tx_bytes;

	failed: HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

	return result;
}/* end BlueNRG_SPI_Write() */

void Hal_Write_Serial(const void* data1, const void* data2, uint16_t n_bytes1, uint16_t n_bytes2) {
	struct timer t;
	int ret;
	uint8_t data2_offset = 0;

	Timer_Set(&t, CLOCK_SECOND / 10);

	Disable_SPI_IRQ();

	while (1) {
		ret = BlueNRG_SPI_Write((uint8_t *) data1, (uint8_t *) data2 + data2_offset, n_bytes1, n_bytes2);

		if (ret >= 0) {
			n_bytes1 = 0;
			n_bytes2 -= ret;
			data2_offset += ret;
			if (n_bytes2 == 0)
				break;
		}

		if (Timer_Expired(&t)) {
			break;
		}
	}

	Enable_SPI_IRQ();
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
