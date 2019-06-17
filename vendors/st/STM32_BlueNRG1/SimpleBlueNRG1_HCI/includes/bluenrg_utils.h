/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
* File Name          : bluenrg_utils.h
* Author             : AMS - VMA, RF Application Team
* Version            : V1.0.1
* Date               : 03-October-2014
* Description        : Header file for BlueNRG utility functions 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/**
 * @file  bluenrg_utils.h
 * @brief BlueNRG IFR updater & BlueNRG stack updater utility APIs description
 *
 * <!-- Copyright 2014 by STMicroelectronics.  All rights reserved.       *80*-->
**/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLUENRG_UTILS_H
#define __BLUENRG_UTILS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hal_types.h" 
#include "compiler.h"

/* Exported types ------------------------------------------------------------*/
typedef struct{
  uint8_t stack_mode;
  uint8_t day;
  uint8_t month;
  uint8_t year;
  uint16_t slave_sca_ppm;
  uint8_t master_sca;
  uint16_t hs_startup_time; /* In system time units*/
} IFR_config2_TypeDef;

/**
 * Structure inside IFR for configuration options. 
 */
typedef __packed struct{
  uint8_t cold_ana_act_config_table[64];
  uint8_t hot_ana_config_table[64];
  uint8_t stack_mode;
  uint8_t gpio_config;
  uint8_t rsrvd1[2];
  uint32_t rsrvd2[3];
  uint32_t max_conn_event_time;
  uint32_t ls_crystal_period;
  uint32_t ls_crystal_freq;
  uint16_t slave_sca_ppm;
  uint8_t master_sca;
  uint8_t rsrvd3;
  uint16_t hs_startup_time; /* In system time units*/
  uint8_t rsrvd4[2];
  uint32_t uid;
  uint8_t rsrvd5;
  uint8_t year;  
  uint8_t month;
  uint8_t day;
  uint32_t unused[5];
} PACKED IFR_config_TypeDef;

/* Exported constants --------------------------------------------------------*/
extern const IFR_config_TypeDef IFR_config;

/* Exported macros -----------------------------------------------------------*/
#define FROM_US_TO_SYS_TIME(us)      ((uint16_t)(us/2.4414)+1)
#define FROM_SYS_TIME_TO_US(sys)     ((uint16_t)(sys*2.4414))

/* Convert 2 digit BCD number to an integer */
#define BCD_TO_INT(bcd) ((bcd & 0xF) + ((bcd & 0xF0) >> 4)*10)

/* Convert 2 digit number to a BCD number */
#define INT_TO_BCD(n) ((((uint8_t)n/10)<<4) + (uint8_t)n%10)

/** 
  * Return values 
  */ 
#define BLE_UTIL_SUCCESS                 0
#define BLE_UTIL_UNSUPPORTED_VERSION     1
#define BLE_UTIL_WRONG_IMAGE_SIZE        2
#define BLE_UTIL_ACI_ERROR               3
#define BLE_UTIL_CRC_ERROR               4
#define BLE_UTIL_PARSE_ERROR             5
#define BLE_UTIL_WRONG_VERIFY            6

/* Exported functions ------------------------------------------------------- */ 
/**
  * @brief  Flash a new firmware using internal bootloader.
  * @param  fw_image     Pointer to the firmware image (raw binary data,
  *                      little-endian).
  * @param  fw_size      Size of the firmware image. The firmware image size shall
  *                      be multiple of 4 bytes.
  * @retval int      It returns 0 if successful, or a number not equal to 0 in
  *                  case of error (ACI_ERROR, UNSUPPORTED_VERSION,
  *                  WRONG_IMAGE_SIZE, CRC_ERROR)
  */
int program_device(const uint8_t *fw_image, uint32_t fw_size);

/**
  * @brief  Read raw data from IFR (3 64-bytes blocks).
  * @param  data     Pointer to the buffer that will contain the read data.
  *                  Its size must be 192 bytes. This data can be parsed by
  *                  parse_IFR_data_config().
  * @retval int      It returns 0 if successful, or a number not equal to 0 in
  *                  case of error (ACI_ERROR, UNSUPPORTED_VERSION)
  */
int read_IFR(uint8_t data[192]);

/**
  * @brief  Verify raw data from IFR (3 64-bytes blocks).
  * @param  ifr_data Pointer to the buffer that will contain the data to verify.
  *                  Its size must be 192 bytes.
  * @retval int      It returns 0 if successful, or a number not equal to 0 in
                     case of error (ACI_ERROR, BLE_UTIL_WRONG_VERIFY)
  */
uint8_t verify_IFR(const IFR_config_TypeDef *ifr_data);

/**
  * @brief  Program raw data to IFR (3 64-bytes blocks).
  * @param  ifr_image     Pointer to the buffer that will contain the data to program.
  *                  Its size must be 192 bytes.
  * @retval int      It returns 0 if successful
  */
int program_IFR(const IFR_config_TypeDef *ifr_image);

/**
  * @brief  Parse IFR raw data.
  * @param  data     Pointer to the raw data: last 64 bytes read from IFR sector.
  * @param  IFR_config     Data structure that will be filled with parsed data.
  * @retval None
  */
void parse_IFR_data_config(const uint8_t data[64], IFR_config2_TypeDef *IFR_config);

/**
  * @brief  Check for the correctness of parsed data.
  * @param  IFR_config     Data structure filled with parsed data.
  * @retval int      It returns 0 if successful, or PARSE_ERROR in case data is
  *                  not correct.
  */
int IFR_validate(IFR_config2_TypeDef *IFR_config);

/**
  * @brief  Modify IFR data. (Last 64-bytes block).
  * @param  IFR_config   Structure that contains the new parameters inside the
  *                      IFR configuration data.
  * @note   It is highly recommended to parse the IFR configuration from
  *         a working IFR block (this should be done with parse_IFR_data_config()).
  *         Then it is possible to write the new parameters inside the IFR_config
  *         structure.
  * @param  data     Pointer to the buffer that contains the original data. It
  *                  will be modified according to the new data in the IFR_config
  *                  structure. Then this data must be written in the last
  *                  64-byte block in the IFR.
  *                  Its size must be 64 bytes.
  * @retval None
  */
void change_IFR_data_config(IFR_config2_TypeDef *IFR_config, uint8_t data[64]);

/**
  * @brief  Get BlueNRG hardware and firmware version
  * @param  hwVersion This parameter returns the Hardware Version (i.e. CUT 3.0 = 0x30, CUT 3.1 = 0x31).
  * @param  fwVersion This parameter returns the Firmware Version in the format 0xJJMN
  *                   where JJ = Major Version number, M = Minor Version number and N = Patch Version number.
  * @retval Status of the call
  */
uint8_t getBlueNRGVersion(uint8_t *hwVersion, uint16_t *fwVersion);

/**
  * @brief  Get BlueNRG updater version
  * @param  version This parameter returns the updater version. If the updadter version is 0x03 
  *                 the chip has the updater old, needs to update the bootloader.
  * @retval Status of the call
  */
uint8_t getBlueNRGUpdaterVersion(uint8_t *version);

/**
  * @brief  Verifies if the bootloader is patched or not. This function shall be used to fix a bug on
  *         the HW bootloader related to the 32 MHz external crystal oscillator.
  * @retval TRUE if the HW bootloader is already patched, FALSE otherwise
  */
uint8_t isHWBootloader_Patched(void);
   
#ifdef __cplusplus
}
#endif

#endif /*__BLUENRG_UTILS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
