/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"

/* USER CODE BEGIN Includes */

/* SDK includes */
#include "SDK_EVAL_Spi.h"
#include "SDK_EVAL_Gpio.h"

/* FreeRTOS includes */
#include <aws_ble.h>
#include "FreeRTOS.h"
#include "task.h"

/* Demo includes */
#include "aws_test_runner.h"

/* AWS System includes. */
#include "aws_system_init.h"
#include "aws_logging_task.h"
#include "aws_clientcredential.h"
#include "aws_ble_hal_common.h"

#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "semphr.h"

/* USER CODE END Includes */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 12 )
/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi3;
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t ret = 0;

static UART_HandleTypeDef xConsoleUart;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void vApplicationGetIdleTaskMemory(StaticTask_t ** ppxIdleTaskTCBBuffer, StackType_t ** ppxIdleTaskStackBuffer, uint32_t * pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t ** ppxTimerTaskTCBBuffer, StackType_t ** ppxTimerTaskStackBuffer, uint32_t * pulTimerTaskStackSize);
void vApplicationMallocFailedHook();
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName);
void vApplicationIdleHook(void);
void vApplicationDaemonTaskStartupHook(void);

void prvMiscInitialization(void);
static BTStatus_t prvBLEStackInit(void);
void vBLE_TickTask(void * pvParameters);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

#define	mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 32 )
#define	mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )
#define mainDEVICE_NICK_NAME                "BlueAWS"

#define mainBLE_SERVER_UUID             { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define	mainBLE_DEVICE_NAME                 "BlueAWS"
#define	ADV_INTERVAL_MIN_MS  1000
#define	ADV_INTERVAL_MAX_MS  1200

#define	BLE_TASK_PRIORITY		            ( tskIDLE_PRIORITY + 1 )
//#define	BLE_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
//#define BLE_TASK_STACK_SIZE               (configMINIMAL_STACK_SIZE)
#define BLE_TASK_STACK_SIZE                 (512*8)//4K
/* USER CODE END 0 */



/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void) {
	/* Initialize all configured peripherals */
	prvMiscInitialization();

	if (xTaskCreate(vBLE_TickTask, "vBLE_TickTask", BLE_TASK_STACK_SIZE, NULL, BLE_TASK_PRIORITY, NULL) != pdPASS) {
		__BKPT(0);
	}

	vTaskStartScheduler();

	while (1) {
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

void vBLE_TickTask(void * pvParameters) {
	/* The parameter value is expected to be 1 as 1 is passed in the
	 pvParameters value in the call to xTaskCreate() below.
	 configASSERT( ( ( uint32_t ) pvParameters ) == 1 );*/

	for (;;) {
		vTaskDelay(1);
		BTLE_StackTick(); /* Task code goes here. */
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();

	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 16;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000); /**Configure the Systick interrupt time */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);/**Configure the Systick */
	HAL_RCCEx_EnableMSIPLLMode(); /**Enable MSI Auto calibration */

	//HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);/* SysTick_IRQn interrupt configuration */
}

/**
 * @brief UART console initialization function.
 */
static void Console_UART_Init(void) {
	xConsoleUart.Instance = USART1;
	xConsoleUart.Init.BaudRate = 115200;
	xConsoleUart.Init.WordLength = UART_WORDLENGTH_8B;
	xConsoleUart.Init.StopBits = UART_STOPBITS_1;
	xConsoleUart.Init.Parity = UART_PARITY_NONE;
	xConsoleUart.Init.Mode = UART_MODE_TX_RX;
	xConsoleUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	xConsoleUart.Init.OverSampling = UART_OVERSAMPLING_16;
	xConsoleUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	xConsoleUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&xConsoleUart) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
}

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 PC0   ------> ADCx_IN1
 PC1   ------> ADCx_IN2
 PC2   ------> ADCx_IN3
 PC3   ------> ADCx_IN4
 PA0   ------> UART4_TX
 PA1   ------> UART4_RX
 PA3   ------> S_TIM2_CH4
 PA4   ------> ADCx_IN9
 PA5   ------> SPI1_SCK
 PA6   ------> SPI1_MISO
 PA7   ------> SPI1_MOSI
 PC4   ------> ADCx_IN13
 PC5   ------> ADCx_IN14
 PB1   ------> ADCx_IN16
 PE7   ------> S_DATAIN2DFSDM1
 PE9   ------> S_CKOUTDFSDM1
 PE10   ------> QUADSPI_CLK
 PE11   ------> QUADSPI_NCS
 PE12   ------> QUADSPI_BK1_IO0
 PE13   ------> QUADSPI_BK1_IO1
 PE14   ------> QUADSPI_BK1_IO2
 PE15   ------> QUADSPI_BK1_IO3
 PB10   ------> I2C2_SCL
 PB11   ------> I2C2_SDA
 PD8   ------> USART3_TX
 PD9   ------> USART3_RX
 PA9   ------> USB_OTG_FS_VBUS
 PA10   ------> USB_OTG_FS_ID
 PA11   ------> USB_OTG_FS_DM
 PA12   ------> USB_OTG_FS_DP
 PD1   ------> SPI2_SCK
 PD3   ------> USART2_CTS
 PD4   ------> USART2_RTS
 PD5   ------> USART2_TX
 PD6   ------> USART2_RX
 PB6   ------> USART1_TX
 PB7   ------> USART1_RX
 PB8   ------> I2C1_SCL
 PB9   ------> I2C1_SDA
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : LED1_Pin */
	GPIO_InitStruct.Pin = LED1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) { // TODO: Must be handled by FreeRTOS

	if (GPIO_Pin == SPBTLE_RF_IRQ_EXTI6_Pin)
		HCI_Isr();
}

/*---------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t ** ppxIdleTaskTCBBuffer, StackType_t ** ppxIdleTaskStackBuffer, uint32_t * pulIdleTaskStackSize) {
	/* If the buffers to be provided to the Idle task are declared inside this function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task.
 * */
void vApplicationGetTimerTaskMemory(StaticTask_t ** ppxTimerTaskTCBBuffer, StackType_t ** ppxTimerTaskStackBuffer, uint32_t * pulTimerTaskStackSize) {
	/* If the buffers to be provided to the Timer task are declared inside this function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer. Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/**
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
void vApplicationMallocFailedHook() {
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

/**
 * @brief Loop forever if stack overflow is detected.
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1, this hook provides a location for applications to
 * define a response to a stack overflow. Use this hook to help identify that a stack overflow has occurred. */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName) {
	portDISABLE_INTERRUPTS();

	/* Loop forever */
	for (;;)
		;
}

void vApplicationIdleHook(void) {
	__WFI();
}

void vApplicationDaemonTaskStartupHook(void) {

        /* Create the task to run unit tests. */
        xTaskCreate( TEST_RUNNER_RunTests_task,
                "RunTests_task",
                mainTEST_RUNNER_TASK_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 5,
                NULL );

}

/*---------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Initializes the board.
 */
void prvMiscInitialization(void) {
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock. */
	SystemClock_Config();

	MX_GPIO_Init();

	/* UART console init. */
	Console_UART_Init();

	configPRINT_STRING("Hello\r\n");
	/* BLE SPI Init */
	SdkEvalSpiInit(SPI_MODE_EXTI);
	Sdk_Eval_Gpio_Init();

	prvBLEStackInit();

#if 0 /* SJ: TODO */
	/* RNG init function. */
	xHrng.Instance = RNG;

	if( HAL_RNG_Init( &xHrng ) != HAL_OK )
	{
		Error_Handler();
	}

	/* RTC init. */
	RTC_Init();
#endif
}

static BTStatus_t prvBLEStackInit(void) {

	// Initialize BLE
	return (BlueNRG_Stack_Initialization() == 0) ? eBTStatusSuccess : eBTStatusFail;

}

void vOutputChar(const char cChar, const TickType_t xTicksToWait) {
	(void) cChar;
	(void) xTicksToWait;
}
/*-----------------------------------------------------------*/

void vMainUARTPrintString(char * pcString) {
	const uint32_t ulTimeout = 3000UL;

	HAL_UART_Transmit(&xConsoleUart, (uint8_t *) pcString, strlen(pcString), ulTimeout);
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
		__BKPT(0);
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
