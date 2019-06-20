/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal_platform.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sleep_driver.h"

#ifdef HAL_SD_MODULE_ENABLED
extern void sd_backup_all(void);
extern void sd_restore_all(void);
#endif

#ifdef HAL_SDIO_MODULE_ENABLED
extern void sdio_backup_all(void);
extern void sdio_restore_all(void);
#endif

#ifdef HAL_PWM_MODULE_ENABLED
extern void pwm_backup_register_call_back(void);
extern void pwm_restore_register_call_back(void);
#endif

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
extern void spim_backup_register_callback(void);
extern void spim_restore_register_callback(void);
#endif

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
extern void hal_i2c_backup_all_register(void);
extern void hal_i2c_restore_all_register(void);
#endif

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
extern void i2s_enter_suspend(void);
extern void i2s_enter_resume(void);
#endif

extern void bt_driver_restore_btif(void);
/*Weak symbol declaration for btif callback */
void default_bt_driver_restore_btif(void)
{
    return;
}
#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_bt_driver_restore_btif=_default_bt_driver_restore_btif")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
    #pragma weak bt_driver_restore_btif = default_bt_driver_restore_btif
#else
    #error "Unsupported Platform"
#endif

void hal_module_sleep_register_callback(void)
{
    uint32_t register_mode = 0;
//Example
//   hal_cm4_topsm_register_suspend_cb(i2c_sleep_cb, "i2c_xxx");
//   hal_cm4_topsm_register_resume_cb(i2c_wakeup_cb, "i2c_xxx");

#if 0
    /*register pwm callback*/
#ifdef HAL_PWM_MODULE_ENABLED
    sleepdrv_register_suspend_cb((sleepdrv_cb)pwm_backup_register_call_back, NULL);
    sleepdrv_register_resume_cb((sleepdrv_cb)pwm_restore_register_call_back, NULL);
#endif

    /*register spi callback*/
#ifdef HAL_SPI_MASTER_MODULE_ENABLED
    sleepdrv_register_suspend_cb((sleepdrv_cb)spim_backup_register_callback, NULL);
    sleepdrv_register_resume_cb((sleepdrv_cb)spim_restore_register_callback, NULL);
#endif

#endif
    /*register i2c callback*/
#ifdef  HAL_I2C_MASTER_MODULE_ENABLED
    register_mode = ((1 << HAL_SLEEP_MODE_LEGACY_SLEEP) | (1 << HAL_SLEEP_MODE_DEEP_SLEEP));
    sleepdrv_register_suspend_cb((sleepdrv_cb)hal_i2c_backup_all_register, NULL, register_mode);
    sleepdrv_register_resume_cb((sleepdrv_cb)hal_i2c_restore_all_register, NULL, register_mode);
#endif

#if 0
    /*register sd callback*/
#ifdef HAL_SD_MODULE_ENABLED
    sleepdrv_register_suspend_cb((sleepdrv_cb)sd_backup_all, NULL);
    sleepdrv_register_resume_cb((sleepdrv_cb)sd_restore_all, NULL);
#endif

    /*register sdio callback*/
#ifdef HAL_SDIO_MODULE_ENABLED
    sleepdrv_register_suspend_cb((sleepdrv_cb)sdio_backup_all, NULL);
    sleepdrv_register_resume_cb((sleepdrv_cb)sdio_restore_all, NULL);
#endif
#endif

#ifdef HAL_I2S_MODULE_ENABLED
    register_mode = ((1 << HAL_SLEEP_MODE_LEGACY_SLEEP) | (1 << HAL_SLEEP_MODE_DEEP_SLEEP));
    sleepdrv_register_suspend_cb((sleepdrv_cb)i2s_enter_suspend, NULL, register_mode);
    sleepdrv_register_resume_cb((sleepdrv_cb)i2s_enter_resume, NULL, register_mode);
#endif

    register_mode = (1 << HAL_SLEEP_MODE_DEEP_SLEEP);
    sleepdrv_register_resume_cb((sleepdrv_cb)bt_driver_restore_btif, NULL, register_mode);
}

#endif

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
#include "flash_map.h"
#include "xflash_map.h"

  uint32_t get_crash_logging_base_address(void)
 {
 
   return CRASH_SYSLOG_BASE;
 
 }
  
 
  uint32_t get_crash_logging_length(void)
 {
 
   return CRASH_SYSLOG_LENGTH;
 
 }

 uint32_t get_normal_logging_base_address(void)
{

  return NORMAL_SYSLOG_BASE;

}


 uint32_t get_normal_logging_length(void)
{

  return NORMAL_SYSLOG_LENGTH;

}

  uint32_t get_crash_context_base(void)
{

  return CRASH_CONTEXT_BASE;

}

uint32_t get_crash_context_length(void)
{

  return CRASH_CONTEXT_LENGTH;

}

  uint32_t get_n9_crash_context_base(void)
{

  return CRASH_N9_DUMP_BASE;

}

uint32_t get_n9_crash_context_length(void)
{

  return CRASH_N9_DUMP_LENGTH;

}


#if defined(SERIAL_NOR_WITH_SPI_SUPPORT)

  uint32_t get_ext_crash_context_base(void)
{

  return CRASH_CONTEXT_EXT_BASE;

}

uint32_t get_ext_crash_context_length(void)
{

  return CRASH_CONTEXT_EXT_LENGTH;

}

  uint32_t get_n9_ext_crash_context_base(void)
{

  return CRASH_N9_DUMP_EXT_BASE;

}

uint32_t get_n9_ext_crash_context_length(void)
{

  return CRASH_N9_DUMP_EXT_LENGTH;

}

#else 

  uint32_t get_ext_crash_context_base(void)
{

  return 0;

}

uint32_t get_ext_crash_context_length(void)
{

  return 0;

}

  uint32_t get_n9_ext_crash_context_base(void)
{

  return 0;

}

uint32_t get_n9_ext_crash_context_length(void)
{

  return 0;

}


#endif

#endif


