/*
 * boot_loader.h
 *
 *  Created on: Mar 26, 2020
 *      Author: a5111060
 */

#ifndef BOOT_LOADER_H_
#define BOOT_LOADER_H_

#include <stdio.h>
#include <string.h>
#include "r_smc_entry.h"
#include "r_flash_rx_if.h"
#include "r_sci_rx_if.h"

#include "r_sci_rx_pinset.h"

#include "base64_decode.h"
#include "code_signer_public_key.h"

/* tinycrypto */
#include "tinycrypt/sha256.h"
#include "tinycrypt/ecc.h"
#include "tinycrypt/ecc_dsa.h"
#include "tinycrypt/constants.h"

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_13
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_51

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
#define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_52

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 6

#define BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS FLASH_DF_BLOCK_0
#define BOOT_LOADER_CONST_DATA_BLOCK_NUM 256

#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH 0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH 0x100
#define INITIAL_FIRMWARE_FILE_NAME "userprog.rsu"

#define FLASH_INTERRUPT_PRIORITY 14	/* 0(low) - 15(high) */
#define SCI_INTERRUPT_PRIORITY 15	/* 0(low) - 15(high) */

/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/


#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)
#define BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_DF - BOOT_LOADER_CONST_DATA_BLOCK_NUM)
#define USER_RESET_VECTOR_ADDRESS (BOOT_LOADER_LOW_ADDRESS - 4)

#define BOOT_LOADER_SUCCESS         (0)
#define BOOT_LOADER_FAIL            (-1)
#define BOOT_LOADER_GOTO_INSTALL    (-2)
#define BOOT_LOADER_IN_PROGRESS     (-3)

#define BOOT_LOADER_STATE_INITIALIZING								1
#define BOOT_LOADER_STATE_BANK1_CHECK								2
#define BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT			3
#define BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE		4
#define BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT			5
#define BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE		6
#define BOOT_LOADER_STATE_BANK0_CHECK								7
#define BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT		8
#define BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE	9
#define BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1		10
#define BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1	11
#define BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2		12
#define BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2	13
#define BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_WAIT				14
#define BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE			15
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT		16
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE	17
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT		18
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_COMPLETE	19
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT		20
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE	21
#define BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_WAIT				22
#define BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_COMPLETE			23
#define BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_WAIT				24
#define BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE			25
#define BOOT_LOADER_STATE_BANK0_UPDATE_CHECK						26
#define BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT		27
#define BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE	28
#define BOOT_LOADER_STATE_FINALIZE									29
#define BOOT_LOADER_STATE_FATAL_ERROR								200

#define BOOT_LOADER_SCI_CONTROL_BLOCK_A (0)
#define BOOT_LOADER_SCI_CONTROL_BLOCK_B (1)
#define BOOT_LOADER_SCI_CONTROL_BLOCK_TOTAL_NUM (2)

#define BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY (0)
#define BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL  (1)

#define LIFECYCLE_STATE_BLANK		(0xff)
#define LIFECYCLE_STATE_TESTING		(0xfe)
#define LIFECYCLE_STATE_INSTALLING	(0xfc)
#define LIFECYCLE_STATE_VALID		(0xf8)
#define LIFECYCLE_STATE_INVALID		(0xf0)

#define MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT 3
#define SHA1_HASH_LENGTH_BYTE_SIZE 20

#define FLASH_DF_TOTAL_BLOCK_SIZE (FLASH_DF_BLOCK_INVALID - FLASH_DF_BLOCK_0)

#define INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE "hash-sha256"
#define INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE "sig-sha256-ecdsa"

#if defined(UNITY_TEST)
#define __STATIC
#else
#define  __STATIC static
#endif

#if !defined(MY_BSP_CFG_SERIAL_TERM_SCI)
#error "Error! Need to define MY_BSP_CFG_SERIAL_TERM_SCI in r_bsp_config.h"
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (0)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI0()
#define SCI_CH_serial_term          SCI_CH0
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (1)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI1()
#define SCI_CH_serial_term          SCI_CH1
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (2)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI2()
#define SCI_CH_serial_term          SCI_CH2
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (3)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI3()
#define SCI_CH_serial_term          SCI_CH3
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (4)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI4()
#define SCI_CH_serial_term          SCI_CH4
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (5)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI5()
#define SCI_CH_serial_term          SCI_CH5
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (6)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI6()
#define SCI_CH_serial_term          SCI_CH6
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (7)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI7()
#define SCI_CH_serial_term          SCI_CH7
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (8)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI8()
#define SCI_CH_serial_term          SCI_CH8
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (9)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI9()
#define SCI_CH_serial_term          SCI_CH9
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (10)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI10()
#define SCI_CH_serial_term          SCI_CH10
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (11)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI11()
#define SCI_CH_serial_term          SCI_CH11
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (12)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI12()
#define SCI_CH_serial_term          SCI_CH12
#else
#error "Error! Invalid setting for MY_BSP_CFG_SERIAL_TERM_SCI in r_bsp_config.h"
#endif

typedef struct _load_firmware_control_block {
    uint32_t flash_buffer[FLASH_CF_MEDIUM_BLOCK_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
}LOAD_FIRMWARE_CONTROL_BLOCK;

typedef struct _load_const_data_control_block {
    uint32_t flash_buffer[FLASH_DF_TOTAL_BLOCK_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
}LOAD_CONST_DATA_CONTROL_BLOCK;

typedef struct _sci_buffer_control {
   uint8_t buffer[FLASH_CF_MEDIUM_BLOCK_SIZE];
   uint32_t buffer_occupied_byte_size;
   uint32_t buffer_full_flag;
}SCI_BUFFER_CONTROL;

typedef struct _sci_receive_control_block {
   SCI_BUFFER_CONTROL * p_sci_buffer_control;
   uint32_t total_byte_size;
   uint32_t current_state;
}SCI_RECEIVE_CONTROL_BLOCK;

typedef struct _firmware_update_control_block
{
	uint8_t magic_code[7];
    uint8_t image_flag;
    uint8_t signature_type[32];
    uint32_t signature_size;
    uint8_t signature[256];
    uint32_t dataflash_flag;
    uint32_t dataflash_start_address;
    uint32_t dataflash_end_address;
    uint8_t reserved1[200];
    uint32_t sequence_number;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t execution_address;
    uint32_t hardware_id;
    uint8_t reserved2[236];
}FIRMWARE_UPDATE_CONTROL_BLOCK;

#endif /* BOOT_LOADER_H_ */
