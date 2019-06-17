/**
  ******************************************************************************
  * @file    main.h
  * @author  AMS VMA Application Team
  * @version V1.0
  * @date    25-June-2015
  * @brief   Header for low_power.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOW_POWER_H
#define __LOW_POWER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifdef SYSCLK_MSI 
uint8_t App_SleepMode_Check(void);
void User_Timer_Enter_Sleep(void);
void User_Timer_Exit_Sleep(void);

void SystemPower_Config(void);
void Enter_Sleep_Mode(void);
void Enter_LP_Sleep_Mode(void);
void System_Sleep(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /*__MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
