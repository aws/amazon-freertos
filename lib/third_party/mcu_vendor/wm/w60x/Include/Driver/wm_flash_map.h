/**
 * @file    wm_flash_map.h
 *
 * @brief   flash zone map
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_FLASH_MAP_H__
#define __WM_FLASH_MAP_H__

/**FLASH MAP**/

/**Flash Base Address */
#define FLASH_BASE_ADDR						(0x8000000)

/**Upgrade image header area & System parameter area */
#define CODE_UPD_HEADER_ADDR				(FLASH_BASE_ADDR  + 0xFC000) 
#define TLS_FLASH_PARAM1_ADDR				(FLASH_BASE_ADDR  + 0xFD000)
#define TLS_FLASH_PARAM2_ADDR				(FLASH_BASE_ADDR  + 0xFE000)
#define TLS_FLASH_PARAM_RESTORE_ADDR		(FLASH_BASE_ADDR  + 0xFF000)
#define TLS_FLASH_END_ADDR					(FLASH_BASE_ADDR  + 0xFFFFF)

#define FLASH_KEY_PARAM_AREA_LEN			(0x2000)

#define SECBOOT_HEADER_ADDR 				(FLASH_BASE_ADDR  + FLASH_KEY_PARAM_AREA_LEN) 
#define SECBOOT_HEADER_AREA_LEN			(0x100)

#define SECBOOT_AREA_ADDR					(SECBOOT_HEADER_ADDR  + SECBOOT_HEADER_AREA_LEN) 
#define SECBOOT_AREA_LEN					(0x10000 - FLASH_KEY_PARAM_AREA_LEN - SECBOOT_HEADER_AREA_LEN)

/**Run-time image header area*/
#define CODE_RUN_HEADER_ADDR				(SECBOOT_AREA_ADDR  + SECBOOT_AREA_LEN)
#define CODE_RUN_HEADER_AREA_LEN			(0x100)

/**Run-time image area*/
#define CODE_RUN_START_ADDR				(CODE_RUN_HEADER_ADDR  + CODE_RUN_HEADER_AREA_LEN)
#define CODE_RUN_AREA_LEN					(512*1024 - CODE_RUN_HEADER_AREA_LEN)

/**Upgrade image area*/
#define CODE_UPD_START_ADDR				(CODE_RUN_START_ADDR  + CODE_RUN_AREA_LEN)
#define CODE_UPD_AREA_LEN					(384*1024)

/**Area can be used by User*/
#define USER_ADDR_START						(CODE_UPD_START_ADDR  + CODE_UPD_AREA_LEN)
#define TLS_FLASH_PARAM_DEFAULT  			(USER_ADDR_START)
#define USER_AREA_LEN						(CODE_UPD_HEADER_ADDR - USER_ADDR_START)

#define SIGNATURE_WORD      					0xa0ffff9f

#endif /*__WM_CONFIG_H__*/

