/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : low_power.c
* Author             : AMS - VMA RF Application team 
* Version            : V1.0.0
* Date               : 24-June-2015
* Description        : Functions for low power management
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifdef SYSCLK_MSI 

/* Includes ------------------------------------------------------------------*/

#include "cube_hal.h"
#include "hal.h"
#include "user_timer.h"
#include "SDK_EVAL_Config.h"
#include "compiler.h"

WEAK_FUNCTION(void User_Timer_Enter_Sleep(void));
WEAK_FUNCTION(void User_Timer_Exit_Sleep(void));
WEAK_FUNCTION(uint8_t App_SleepMode_Check(void));

/**
  * @brief  System Power Configuration for Low Power mode
  * @note: It configure all the GPIOs on Analog Input mode and disable clocks
  * @retval None
  */
void SystemPower_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
#ifdef STM32L152xE
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
#endif 
  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  GPIO_InitStructure.Pin = GPIO_PIN_All;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
#ifdef STM32L152xE
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
#endif

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
#ifdef STM32L152xE
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
#endif
}

/**
  * @brief  Configure HCKL
  * @param  RCC_SYSCLK: HCLK frequency , 
  * @retval None
  */
void RCC_HCLKConfig(uint32_t RCC_SYSCLK)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_HCLK(RCC_SYSCLK));
  
  tmpreg = RCC->CFGR;
  
  /* Clear HPRE[3:0] bits */
  tmpreg &= ~RCC_CFGR_HPRE;
  
  /* Set HPRE[3:0] bits according to RCC_SYSCLK value */
  tmpreg |= RCC_SYSCLK;
  
  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Change MSI frequency
  * @param  freq: frequency range, 
  * @param  div2: if FALSE, HCLK will be SYSCLK, otherwise SYSCLK/2.
  * @retval None
  */
void ChangeMSIClock(uint32_t freq, BOOL div2)
{     
  /* To configure the MSI frequency */
  __HAL_RCC_MSI_RANGE_CONFIG(freq);
  
  if (div2)
  {
    RCC_HCLKConfig(RCC_CFGR_HPRE_DIV2); //TBR
    
  }
  else {
    RCC_HCLKConfig(RCC_CFGR_HPRE_DIV1); //TBR
  }
}

/**
  * @brief  Put the micro in sleep mode
  * @note   This function puts the micro in a sleep mode without disabling timer
  *         clock. It is called when a fast wake-up is needed.
  *         To be called only inside an atomic section.
  * @retval None
  */
void Enter_Sleep_Mode(void)
{    
  /* Request Wait For Interrupt */    
  HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}


/**
  * @brief  Put the micro in low power sleep mode
  * @note   This function puts the micro in a low power sleep mode without disabling timer
  *         clock. This function assumes that the system clock source is already MSI.
  * @retval None
  */
void Enter_LP_Sleep_Mode(void)
{   
    
  /* Enable the power down mode during Sleep mode */
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  /* Suspend Tick increment to prevent wakeup by Systick interrupt.         */
  /* Otherwise the Systick interrupt will wake up the device within 1ms     */
  /* (HAL time base).                                                       */
  HAL_SuspendTick();
  
  User_Timer_Enter_Sleep();
  
  /* Switch in MSI 65KHz, HCLK 32kHz */
  ChangeMSIClock(RCC_MSIRANGE_0,TRUE);
  
  /* Enter Sleep Mode for Interrupt  */
  HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  
  /* Switch in MSI 4MHz, HCLK 4MHz */
  ChangeMSIClock(RCC_MSIRANGE_6,FALSE);
  
  User_Timer_Exit_Sleep();
  
  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
    
}

void System_Sleep(void)
{
    ATOMIC_SECTION_BEGIN();
    if(App_SleepMode_Check()) {
      Enter_LP_Sleep_Mode();
    }
    ATOMIC_SECTION_END();
}
#endif /* SYSCLK_MSI */
