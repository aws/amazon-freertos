/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define M24SR64_Y_RF_DISABLE_Pin GPIO_PIN_2
#define M24SR64_Y_RF_DISABLE_GPIO_Port GPIOE
#define USB_OTG_FS_OVRCR_EXTI3_Pin GPIO_PIN_3
#define USB_OTG_FS_OVRCR_EXTI3_GPIO_Port GPIOE
#define M24SR64_Y_GPO_Pin GPIO_PIN_4
#define M24SR64_Y_GPO_GPIO_Port GPIOE
#define SPSGRF_915_GPIO3_EXTI5_Pin GPIO_PIN_5
#define SPSGRF_915_GPIO3_EXTI5_GPIO_Port GPIOE
#define SPSGRF_915_GPIO3_EXTI5_EXTI_IRQn EXTI9_5_IRQn
#define SPBTLE_RF_IRQ_EXTI6_Pin GPIO_PIN_6
#define SPBTLE_RF_IRQ_EXTI6_GPIO_Port GPIOE
#define SPBTLE_RF_IRQ_EXTI6_EXTI_IRQn EXTI9_5_IRQn
#define BUTTON_EXTI13_Pin GPIO_PIN_13
#define BUTTON_EXTI13_GPIO_Port GPIOC
#define ARD_A5_Pin GPIO_PIN_0
#define ARD_A5_GPIO_Port GPIOC
#define ARD_A4_Pin GPIO_PIN_1
#define ARD_A4_GPIO_Port GPIOC
#define ARD_A3_Pin GPIO_PIN_2
#define ARD_A3_GPIO_Port GPIOC
#define ARD_A2_Pin GPIO_PIN_3
#define ARD_A2_GPIO_Port GPIOC
#define ARD_D1_Pin GPIO_PIN_0
#define ARD_D1_GPIO_Port GPIOA
#define ARD_D0_Pin GPIO_PIN_1
#define ARD_D0_GPIO_Port GPIOA
#define ARD_D10_Pin GPIO_PIN_2
#define ARD_D10_GPIO_Port GPIOA
#define ARD_D4_Pin GPIO_PIN_3
#define ARD_D4_GPIO_Port GPIOA
#define ARD_D7_Pin GPIO_PIN_4
#define ARD_D7_GPIO_Port GPIOA
#define ARD_D13_Pin GPIO_PIN_5
#define ARD_D13_GPIO_Port GPIOA
#define ARD_D12_Pin GPIO_PIN_6
#define ARD_D12_GPIO_Port GPIOA
#define ARD_D11_Pin GPIO_PIN_7
#define ARD_D11_GPIO_Port GPIOA
#define ARD_A1_Pin GPIO_PIN_4
#define ARD_A1_GPIO_Port GPIOC
#define ARD_A0_Pin GPIO_PIN_5
#define ARD_A0_GPIO_Port GPIOC
#define ARD_D3_Pin GPIO_PIN_0
#define ARD_D3_GPIO_Port GPIOB
#define ARD_D6_Pin GPIO_PIN_1
#define ARD_D6_GPIO_Port GPIOB
#define ARD_D8_Pin GPIO_PIN_2
#define ARD_D8_GPIO_Port GPIOB
#define DFSDM1_DATIN2_Pin GPIO_PIN_7
#define DFSDM1_DATIN2_GPIO_Port GPIOE
#define ISM43362_RST_Pin GPIO_PIN_8
#define ISM43362_RST_GPIO_Port GPIOE
#define DFSDM1_CKOUT_Pin GPIO_PIN_9
#define DFSDM1_CKOUT_GPIO_Port GPIOE
#define QUADSPI_CLK_Pin GPIO_PIN_10
#define QUADSPI_CLK_GPIO_Port GPIOE
#define QUADSPI_NCS_Pin GPIO_PIN_11
#define QUADSPI_NCS_GPIO_Port GPIOE
#define OQUADSPI_BK1_IO0_Pin GPIO_PIN_12
#define OQUADSPI_BK1_IO0_GPIO_Port GPIOE
#define QUADSPI_BK1_IO1_Pin GPIO_PIN_13
#define QUADSPI_BK1_IO1_GPIO_Port GPIOE
#define QUAD_SPI_BK1_IO2_Pin GPIO_PIN_14
#define QUAD_SPI_BK1_IO2_GPIO_Port GPIOE
#define QUAD_SPI_BK1_IO3_Pin GPIO_PIN_15
#define QUAD_SPI_BK1_IO3_GPIO_Port GPIOE
#define INTERNAL_I2C2_SCL_Pin GPIO_PIN_10
#define INTERNAL_I2C2_SCL_GPIO_Port GPIOB
#define INTERNAL_I2C2_SDA_Pin GPIO_PIN_11
#define INTERNAL_I2C2_SDA_GPIO_Port GPIOB
#define ISM43362_BOOT0_Pin GPIO_PIN_12
#define ISM43362_BOOT0_GPIO_Port GPIOB
#define ISM43362_WAKEUP_Pin GPIO_PIN_13
#define ISM43362_WAKEUP_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOB
#define SPSGRF_915_SDN_Pin GPIO_PIN_15
#define SPSGRF_915_SDN_GPIO_Port GPIOB
#define INTERNAL_UART3_TX_Pin GPIO_PIN_8
#define INTERNAL_UART3_TX_GPIO_Port GPIOD
#define INTERNAL_UART3_RX_Pin GPIO_PIN_9
#define INTERNAL_UART3_RX_GPIO_Port GPIOD
#define LPS22HB_INT_DRDY_EXTI0_Pin GPIO_PIN_10
#define LPS22HB_INT_DRDY_EXTI0_GPIO_Port GPIOD
#define LSM6DSL_INT1_EXTI11_Pin GPIO_PIN_11
#define LSM6DSL_INT1_EXTI11_GPIO_Port GPIOD
#define USB_OTG_FS_PWR_EN_Pin GPIO_PIN_12
#define USB_OTG_FS_PWR_EN_GPIO_Port GPIOD
#define SPBTLE_RF_SPI3_CSN_Pin GPIO_PIN_13
#define SPBTLE_RF_SPI3_CSN_GPIO_Port GPIOD
#define ARD_D2_Pin GPIO_PIN_14
#define ARD_D2_GPIO_Port GPIOD
#define HTS221_DRDY_EXTI15_Pin GPIO_PIN_15
#define HTS221_DRDY_EXTI15_GPIO_Port GPIOD
#define VL53L0X_XSHUT_Pin GPIO_PIN_6
#define VL53L0X_XSHUT_GPIO_Port GPIOC
#define VL53L0X_GPIO1_EXTI7_Pin GPIO_PIN_7
#define VL53L0X_GPIO1_EXTI7_GPIO_Port GPIOC
#define VL53L0X_GPIO1_EXTI7_EXTI_IRQn EXTI9_5_IRQn
#define LSM3MDL_DRDY_EXTI8_Pin GPIO_PIN_8
#define LSM3MDL_DRDY_EXTI8_GPIO_Port GPIOC
#define LSM3MDL_DRDY_EXTI8_EXTI_IRQn EXTI9_5_IRQn
#define LED3_WIFI__LED4_BLE_Pin GPIO_PIN_9
#define LED3_WIFI__LED4_BLE_GPIO_Port GPIOC
#define SPBTLE_RF_RST_Pin GPIO_PIN_8
#define SPBTLE_RF_RST_GPIO_Port GPIOA
#define USB_OTG_FS_VBUS_Pin GPIO_PIN_9
#define USB_OTG_FS_VBUS_GPIO_Port GPIOA
#define USB_OTG_FS_ID_Pin GPIO_PIN_10
#define USB_OTG_FS_ID_GPIO_Port GPIOA
#define USB_OTG_FS_DM_Pin GPIO_PIN_11
#define USB_OTG_FS_DM_GPIO_Port GPIOA
#define USB_OTG_FS_DP_Pin GPIO_PIN_12
#define USB_OTG_FS_DP_GPIO_Port GPIOA
#define SYS_JTMS_SWDIO_Pin GPIO_PIN_13
#define SYS_JTMS_SWDIO_GPIO_Port GPIOA
#define SYS_JTCK_SWCLK_Pin GPIO_PIN_14
#define SYS_JTCK_SWCLK_GPIO_Port GPIOA
#define ARD_D9_Pin GPIO_PIN_15
#define ARD_D9_GPIO_Port GPIOA
#define INTERNAL_SPI3_SCK_Pin GPIO_PIN_10
#define INTERNAL_SPI3_SCK_GPIO_Port GPIOC
#define INTERNAL_SPI3_MISO_Pin GPIO_PIN_11
#define INTERNAL_SPI3_MISO_GPIO_Port GPIOC
#define INTERNAL_SPI3_MOSI_Pin GPIO_PIN_12
#define INTERNAL_SPI3_MOSI_GPIO_Port GPIOC
#define PMOD_RESET_Pin GPIO_PIN_0
#define PMOD_RESET_GPIO_Port GPIOD
#define PMOD_SPI2_SCK_Pin GPIO_PIN_1
#define PMOD_SPI2_SCK_GPIO_Port GPIOD
#define PMOD_IRQ_EXTI12_Pin GPIO_PIN_2
#define PMOD_IRQ_EXTI12_GPIO_Port GPIOD
#define PMOD_UART2_CTS_Pin GPIO_PIN_3
#define PMOD_UART2_CTS_GPIO_Port GPIOD
#define PMOD_UART2_RTS_Pin GPIO_PIN_4
#define PMOD_UART2_RTS_GPIO_Port GPIOD
#define PMOD_UART2_TX_Pin GPIO_PIN_5
#define PMOD_UART2_TX_GPIO_Port GPIOD
#define PMOD_UART2_RX_Pin GPIO_PIN_6
#define PMOD_UART2_RX_GPIO_Port GPIOD
#define STSAFE_A100_RESET_Pin GPIO_PIN_7
#define STSAFE_A100_RESET_GPIO_Port GPIOD
#define SYS_JTD0_SWO_Pin GPIO_PIN_3
#define SYS_JTD0_SWO_GPIO_Port GPIOB
#define ARD_D5_Pin GPIO_PIN_4
#define ARD_D5_GPIO_Port GPIOB
#define SPSGRF_915_SPI3_CSN_Pin GPIO_PIN_5
#define SPSGRF_915_SPI3_CSN_GPIO_Port GPIOB
#define ST_LINK_UART1_TX_Pin GPIO_PIN_6
#define ST_LINK_UART1_TX_GPIO_Port GPIOB
#define ST_LINK_UART1_RX_Pin GPIO_PIN_7
#define ST_LINK_UART1_RX_GPIO_Port GPIOB
#define ARD_D15_Pin GPIO_PIN_8
#define ARD_D15_GPIO_Port GPIOB
#define ARD_D14_Pin GPIO_PIN_9
#define ARD_D14_GPIO_Port GPIOB
#define ISM43362_SPI3_CSN_Pin GPIO_PIN_0
#define ISM43362_SPI3_CSN_GPIO_Port GPIOE
#define ISM43362_DRDY_EXTI1_Pin GPIO_PIN_1
#define ISM43362_DRDY_EXTI1_GPIO_Port GPIOE

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
