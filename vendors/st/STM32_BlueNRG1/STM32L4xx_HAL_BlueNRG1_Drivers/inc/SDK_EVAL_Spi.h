/**
 * @file    SDK_EVAL_Spi_Driver.h
 * @author  AMS - AAS Division
 * @version V1.0.1
 * @date    March, 10 2014
 * @brief   This file contains definitions for Software Development Kit eval SPI devices
 * @details
 *
 * In this module there are API for the management of the SPI devices on the SDK Eval
 * motherboards.
 *
 * <b>Example:</b>
 * @code
 *
 *   SdkEvalSpiInit();
 *
 *   ...
 *
 * @endcode
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
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDK_EVAL_DRIVER_H
#define __SDK_EVAL_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SDK_EVAL_STM32L
 * @{
 */

/** @defgroup SPI_Private_TypesDefinitions
 * @{
 */

/**
 * @}
 */

/** @addtogroup SDK_EVAL_Spi                    SDK EVAL Spi
 * @brief SPI functions implementation.
 * @details This file implements the BlueNRG Library SPI interface functions.
 * @{
 */

/** @addtogroup SDK_EVAL_SPI_Exported_Types          SDK EVAL Spi Exported Types
 * @{
 */

/**
 * @brief  SPI Mode for SDK EVAL enumeration
 */
typedef enum {
	SPI_MODE_GPIO = 0, SPI_MODE_EXTI = 1, SPI_MODE_DMA = 2

} SdkEvalSpiMode;

/**
 * @}
 */

/** @defgroup SPI_Private_Defines
 * @{
 */

/**
 * @}
 */

/** @defgroup SDK_EVAL_Spi_Peripheral_Gpio
 * @{
 */

/**
 * @brief BlueNRG User Platform SPI & GPIO lines: to be customized for addressing user platform.
 *        Example configuration: STM32L152 Nucleo.
 */
/* BlueNRG SPI Port */
#define BNRG_SPI_INSTANCE              SPI3
/* BlueNRG SPI Clock */
#define BNRG_SPI_CLK_ENABLE()	       __SPI3_CLK_ENABLE()
#define BNRG_SPI_CLK_DISABLE()         __SPI3_CLK_DISABLE()

// SPI Configuration
#define BNRG_SPI_MODE		       SPI_MODE_MASTER
#define BNRG_SPI_DIRECTION	       SPI_DIRECTION_2LINES
#define BNRG_SPI_DATASIZE	       SPI_DATASIZE_8BIT
#define BNRG_SPI_CLKPOLARITY	   SPI_POLARITY_LOW
#define BNRG_SPI_CLKPHASE	       SPI_PHASE_2EDGE
#define BNRG_SPI_NSS		       SPI_NSS_SOFT
#define BNRG_SPI_FIRSTBIT	       SPI_FIRSTBIT_MSB
#define BNRG_SPI_TIMODE		       SPI_TIMODE_DISABLED
#define BNRG_SPI_CRCPOLYNOMIAL	   7

/* BlueNRG-1 SPI slave mode maximum baudrate is 1 MHz */
#define BNRG_SPI_BAUDRATEPRESCALER     SPI_BAUDRATEPRESCALER_64
#define BNRG_SPI_CRCCALCULATION	       SPI_CRCCALCULATION_DISABLED

/* BlueNRG SPI SCLK define values */
#define BNRG_SPI_SCLK_PIN        	INTERNAL_SPI3_SCK_Pin			/* PC.10 */
#define BNRG_SPI_SCLK_MODE	      	GPIO_MODE_AF_PP
#define BNRG_SPI_SCLK_PULL	      	GPIO_NOPULL
#define BNRG_SPI_SCLK_SPEED	      	GPIO_SPEED_HIGH
#define BNRG_SPI_SCLK_ALTERNATE	 	GPIO_AF6_SPI3
#define BNRG_SPI_SCLK_PORT	     	INTERNAL_SPI3_SCK_GPIO_Port
#define BNRG_SPI_SCLK_CLK_ENABLE()  __GPIOC_CLK_ENABLE()

/* Defines for MISO pin */
#define BNRG_SPI_MISO_PIN			INTERNAL_SPI3_MISO_Pin			/* PC.11 */
#define BNRG_SPI_MISO_MODE			GPIO_MODE_AF_PP
#define BNRG_SPI_MISO_PULL	      	GPIO_PULLUP
#define BNRG_SPI_MISO_SPEED	      	GPIO_SPEED_HIGH
#define BNRG_SPI_MISO_ALTERNATE	    GPIO_AF6_SPI3
#define BNRG_SPI_MISO_PORT	      	INTERNAL_SPI3_MISO_GPIO_Port
#define BNRG_SPI_MISO_CLK_ENABLE()  __GPIOC_CLK_ENABLE()

/* Defines for MOSI pin */
#define BNRG_SPI_MOSI_PIN          	INTERNAL_SPI3_MOSI_Pin			/* PC.12*/
#define BNRG_SPI_MOSI_MODE	     	GPIO_MODE_AF_PP
#define BNRG_SPI_MOSI_PULL	     	GPIO_NOPULL
#define BNRG_SPI_MOSI_SPEED	      	GPIO_SPEED_HIGH
#define BNRG_SPI_MOSI_ALTERNATE	    GPIO_AF6_SPI3
#define BNRG_SPI_MOSI_PORT          INTERNAL_SPI3_MOSI_GPIO_Port
#define BNRG_SPI_MOSI_CLK_ENABLE()  __GPIOC_CLK_ENABLE()

/* BlueNRG SPI CS define values */
#define BNRG_SPI_CS_PIN				SPBTLE_RF_SPI3_CSN_Pin			/* PD.13 */
#define BNRG_SPI_CS_MODE			GPIO_MODE_OUTPUT_PP
#define BNRG_SPI_CS_PULL			GPIO_NOPULL
#define BNRG_SPI_CS_SPEED			GPIO_SPEED_HIGH
#define BNRG_SPI_CS_ALTERNATE		0
#define BNRG_SPI_CS_PORT			SPBTLE_RF_SPI3_CSN_GPIO_Port
#define BNRG_SPI_CS_CLK_ENABLE()	__GPIOD_CLK_ENABLE()

/* BlueNRG SPI IRQ line define values */
#define BNRG_SPI_IRQ_PIN			SPBTLE_RF_IRQ_EXTI6_Pin			/* PE.6 */
#define BNRG_SPI_IRQ_MODE			GPIO_MODE_IT_RISING
#define BNRG_SPI_IRQ_PULL			GPIO_PULLDOWN
#define BNRG_SPI_IRQ_SPEED			GPIO_SPEED_HIGH
#define BNRG_SPI_IRQ_ALTERNATE		0
#define BNRG_SPI_IRQ_PORT			SPBTLE_RF_IRQ_EXTI6_GPIO_Port
#define BNRG_SPI_IRQ_CLK_ENABLE()	__GPIOE_CLK_ENABLE()

// EXTI External Interrupt for SPI
#define BNRG_SPI_EXTI_IRQn			EXTI9_5_IRQn
#define BNRG_SPI_EXTI_IRQHandler	EXTI9_5_IRQHandler
#define BNRG_SPI_EXTI_PIN			SPBTLE_RF_IRQ_EXTI6_Pin
#define BNRG_SPI_EXTI_PORT			SPBTLE_RF_IRQ_EXTI6_GPIO_Port

/* BlueNRG SW reset line define values */
#define BNRG_SPI_RESET_PIN			SPBTLE_RF_RST_Pin				/* PA.8 */
#define BNRG_SPI_RESET_MODE	        GPIO_MODE_OUTPUT_PP
#define BNRG_SPI_RESET_PULL	        GPIO_NOPULL		// PULLUP
#define BNRG_SPI_RESET_SPEED		GPIO_SPEED_LOW	// HIGH
#define BNRG_SPI_RESET_ALTERNATE	0
#define BNRG_SPI_RESET_PORT			SPBTLE_RF_RST_GPIO_Port
#define BNRG_SPI_RESET_CLK_ENABLE()	__GPIOA_CLK_ENABLE()

/* end EVAL_V3 */

/**
 * @brief  Initializes the SPI for BlueNRG module
 * @param  xSpiMode : Spi mode (Polling or IRQ)
 * @retval Status
 */
int8_t SdkEvalSpiInit(SdkEvalSpiMode xSpiMode);
uint8_t SdkEvalSPI_Irq_Pin(void);

void Clear_SPI_EXTI_Flag();

void Enable_SPI_IRQ(void);
void Disable_SPI_IRQ(void);

uint8_t BlueNRG_SPI_Read_All(uint8_t *buffer, uint8_t buff_size);
int16_t BlueNRG_SPI_Write(uint8_t* data1, uint8_t* data2, uint16_t Nb_bytes1, uint16_t Nb_bytes2);

uint8_t BlueNRG_DataPresent(void);
void BlueNRG_IRQ_High(void);
void BlueNRG_Release_IRQ(void);
int16_t BlueNRG_SPI_Write_Bridge(uint8_t* data, uint16_t Nb_bytes);
int16_t BlueNRG_SPI_Read_Bridge(void);

void SdkEvalSpiMisoPinInit(void);
void SdkEvalSpiDtmMisoPinState(FunctionalState state);


#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
