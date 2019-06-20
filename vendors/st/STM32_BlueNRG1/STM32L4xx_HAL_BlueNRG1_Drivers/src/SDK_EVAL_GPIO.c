/**
 * @file    SDK_EVAL_Gpio.c
 * @author  AMS VMA RF application team
 * @version V1.0.0
 * @date    February 28, 2017
 * @brief   This file provides all the low level UART API to access to BlueNRG module
 * @details It uses STM32L1 Cube APIs + Optimized APIs for UART Transmit, Receive,
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
 * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "SDK_EVAL_Gpio.h"

/**
 * @brief  Configure the selected GPIO
 * @param  None.
 * @retval None.
 */
void Sdk_Eval_Gpio_Init(void) {
	/* Reset pin */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = BNRG_RESET_PIN;
	GPIO_InitStruct.Mode = BNRG_RESET_MODE;
	GPIO_InitStruct.Pull = BNRG_RESET_PULL;
	GPIO_InitStruct.Speed = BNRG_RESET_SPEED;
	GPIO_InitStruct.Alternate = BNRG_RESET_ALTERNATE;
	HAL_GPIO_Init(BNRG_RESET_PORT, &GPIO_InitStruct);
	BlueNRG_Power_Down(); /*Added to avoid spurious interrupt from the BlueNRG */

}/* end SdkEval_IO_Config() */

/**
 * @brief  Reset the BlueNRG
 * @param  None
 * @retval None
 */
void BlueNRG_RST(void) {
	HAL_GPIO_WritePin(BNRG_RESET_PORT, BNRG_RESET_PIN, GPIO_PIN_RESET);
	/* 5 ms delay */
	HAL_Delay(5);
	HAL_GPIO_WritePin(BNRG_RESET_PORT, BNRG_RESET_PIN, GPIO_PIN_SET);
	HAL_Delay(5);

	__NOP();
}/* end BlueNRG_RST() */

/**
 * @brief  Power down the BlueNRG (reset).
 * @param  None
 * @retval None
 */
void BlueNRG_Power_Down(void) {
	HAL_GPIO_WritePin(BNRG_RESET_PORT, BNRG_RESET_PIN, GPIO_PIN_RESET);

}/* end BlueNRG_Power_Down() */

/**
 * @brief  Power up the BlueNRG (release reset).
 * @param  None
 * @retval None
 */
void BlueNRG_Power_Up(void) {
	HAL_GPIO_WritePin(BNRG_RESET_PORT, BNRG_RESET_PIN, GPIO_PIN_SET);

	HAL_Delay(5);
}/* end BlueNRG_Power_Up() */


/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
