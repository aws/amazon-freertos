/***********************************************************************
*
*  FILE        : boot_loader.c
*  DATE        : 2019-05-18
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
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

void main(void);
static int32_t secure_boot(void);
static int32_t firm_block_read(uint32_t *firmware, uint32_t offset);
static int32_t const_data_block_read(uint32_t *const_data, uint32_t offset);
static void bank_swap_with_software_reset(void);
static void software_reset(void);
static const uint8_t *get_status_string(uint8_t status);
static void my_sci_callback(void *pArgs);
static void my_flash_callback(void *event);

extern void my_sw_charget_function(void);
extern void my_sw_charput_function(uint8_t data);

static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank0 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;
static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank1 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;
static LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static LOAD_CONST_DATA_CONTROL_BLOCK load_const_data_control_block;
static uint32_t secure_boot_state = BOOT_LOADER_STATE_INITIALIZING;
static uint32_t flash_error_code;

/* Handle storage. */
sci_hdl_t     my_sci_handle;
SCI_RECEIVE_CONTROL_BLOCK sci_receive_control_block;
SCI_BUFFER_CONTROL sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_TOTAL_NUM];

static int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize);
const uint8_t code_signer_public_key[] = CODE_SIGNENR_PUBLIC_KEY_PEM;
const uint32_t code_signer_public_key_length = sizeof(code_signer_public_key);

void main(void)
{
    int32_t result_secure_boot;
    nop();

    while(1)
    {
		result_secure_boot = secure_boot();
		if (BOOT_LOADER_SUCCESS == result_secure_boot)
		{
			/* stop all interrupt completely */
			set_psw(0);
			uint32_t addr;
			addr = *(uint32_t*)USER_RESET_VECTOR_ADDRESS;
			((void (*)())addr)();
			while(1); /* infinite loop */
		}
		else if (BOOT_LOADER_FAIL == result_secure_boot)
		{
			while(1)
			{
				/* infinity loop */
			}
		}
		else if (BOOT_LOADER_IN_PROGRESS == result_secure_boot)
		{
			continue;
		}
		else
		{
			while(1)
			{
				/* infinite loop */
			}
		}
    }
}

static int32_t secure_boot(void)
{
    flash_err_t flash_api_error_code = FLASH_SUCCESS;
    int32_t secure_boot_error_code = BOOT_LOADER_IN_PROGRESS;
    uint32_t bank_info = 255;
    flash_interrupt_config_t cb_func_info;
	FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_tmp = (FIRMWARE_UPDATE_CONTROL_BLOCK*)load_firmware_control_block.flash_buffer;
	int32_t verification_result = -1;

    switch(secure_boot_state)
    {
    	case BOOT_LOADER_STATE_INITIALIZING:
			R_SCI_PinSet_serial_term();

    	    sci_cfg_t   my_sci_config;
    	    sci_err_t   my_sci_err;

    	    /* Set up the configuration data structure for asynchronous (UART) operation. */
    	    my_sci_config.async.baud_rate    = MY_BSP_CFG_SERIAL_TERM_SCI_BITRATE;
    	    my_sci_config.async.clk_src      = SCI_CLK_INT;
    	    my_sci_config.async.data_size    = SCI_DATA_8BIT;
    	    my_sci_config.async.parity_en    = SCI_PARITY_OFF;
    	    my_sci_config.async.parity_type  = SCI_EVEN_PARITY;
    	    my_sci_config.async.stop_bits    = SCI_STOPBITS_1;
    	    my_sci_config.async.int_priority = MY_BSP_CFG_SERIAL_TERM_SCI_INTERRUPT_PRIORITY;

    	    /* OPEN ASYNC CHANNEL
    	    *  Provide address of the configure structure,
    	    *  the callback function to be assigned,
    	    *  and the location for the handle to be stored.*/
    	    my_sci_err = R_SCI_Open(SCI_CH_serial_term, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &my_sci_handle);

    	    /* If there were an error this would demonstrate error detection of API calls. */
    	    if (SCI_SUCCESS != my_sci_err)
    	    {
    	        nop(); // Your error handling code would go here.
				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
				secure_boot_error_code = BOOT_LOADER_FAIL;
				break;
    	    }

    	    load_firmware_control_block.progress = 0;
    	    load_firmware_control_block.offset = 0;

    	    flash_api_error_code = R_FLASH_Open();
    	    if (FLASH_SUCCESS == flash_api_error_code)
    	    {
    	        /* nothing to do */
    	    }
    	    else
    	    {
    	        printf("R_FLASH_Open() returns error. %d.\r\n", flash_error_code);
    	        printf("system error.\r\n");
				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
				secure_boot_error_code = BOOT_LOADER_FAIL;
    	    }

    	    /* startup system */
    	    printf("-------------------------------------------------\r\n");
    	    printf("RX65N secure boot program\r\n");
    	    printf("-------------------------------------------------\r\n");

    	    printf("Checking flash ROM status.\r\n");

    	    printf("bank 0 status = 0x%x [%s]\r\n", firmware_update_control_block_bank0->image_flag, get_status_string(firmware_update_control_block_bank0->image_flag));
    	    printf("bank 1 status = 0x%x [%s]\r\n", firmware_update_control_block_bank1->image_flag, get_status_string(firmware_update_control_block_bank1->image_flag));

    	    R_FLASH_Control(FLASH_CMD_BANK_GET, &bank_info);
    	    printf("bank info = %d. (start bank = %d)\r\n", bank_info, (bank_info ^ 0x01));

    		cb_func_info.pcallback = my_flash_callback;
    		cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
    	    R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);
    	    secure_boot_state = BOOT_LOADER_STATE_BANK1_CHECK;
    		break;

    	case BOOT_LOADER_STATE_BANK1_CHECK:
    		if(firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_TESTING)
    		{
    	    	memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);

    	    	printf("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
    	    	printf("bank1(temporary area) on code flash integrity check...");

				/* Firmware verification for the signature type. */
				if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
				{
					uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
				    /* Hash message */
				    struct tc_sha256_state_struct xCtx;
				    tc_sha256_init(&xCtx);
				    tc_sha256_update(&xCtx,
				    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
							(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
				    tc_sha256_final(hash_sha256, &xCtx);
	    	        verification_result = memcmp(firmware_update_control_block_bank1->signature, hash_sha256, sizeof(hash_sha256));
	    	    }
	    	    else if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
	    	    {
					verification_result = firmware_verification_sha256_ecdsa(
														(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
														(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
														firmware_update_control_block_bank1->signature,
														firmware_update_control_block_bank1->signature_size);
				}
				else
				{
					verification_result = -1;
				}

    	        if(0 == verification_result)
    	        {
    	            printf("OK\r\n");
    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_VALID;
    	        }
    	        else
    	        {
    	            printf("NG\r\n");
    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_INVALID;
    	        }
    	    	printf("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
    	    	printf("bank1(temporary area) block0 erase (to update LIFECYCLE_STATE)...");
    	    	flash_api_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
    	        if (FLASH_SUCCESS != flash_api_error_code)
    	        {
    	            printf("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
    	            printf("system error.\r\n");
					secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
					secure_boot_error_code = BOOT_LOADER_FAIL;
    	            break;
    	        }
    			secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT;
    		}
    		else
    		{
				if (firmware_update_control_block_bank0->image_flag == LIFECYCLE_STATE_VALID)
				{
					secure_boot_state = BOOT_LOADER_STATE_BANK0_UPDATE_CHECK;
	    		}
	    		else
	    		{
	    			secure_boot_state = BOOT_LOADER_STATE_BANK0_CHECK;
				}
    		}
			break;

    	case BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT:
    		/* this state will be update by flash callback */
    		break;

    	case BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE:
	        if (FLASH_SUCCESS == flash_error_code)
	        {
	            printf("OK\r\n");
	        }
	        else
	        {
	            printf("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
	            printf("system error.\r\n");
	            secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
	            break;
	        }
	        printf("bank1(temporary area) block0 write (to update LIFECYCLE_STATE)...");
	        flash_api_error_code = R_FLASH_Write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
			if (FLASH_SUCCESS != flash_api_error_code)
			{
				printf("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
				printf("system error.\r\n");
				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
				secure_boot_error_code = BOOT_LOADER_FAIL;
				break;
			}
			secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT;
			break;

    	case BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT:
    		/* this state will be update by flash callback */
    		break;

    	case BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE:
	        if (FLASH_SUCCESS == flash_error_code)
	        {
	            printf("OK\r\n");
	        }
	        else
	        {
	            printf("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
	            printf("system error.\r\n");
				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
				secure_boot_error_code = BOOT_LOADER_FAIL;
	            break;
	        }
	        printf("swap bank...\r\n");
			R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
			bank_swap_with_software_reset();
			while(1);
			break;

    	case BOOT_LOADER_STATE_BANK0_CHECK:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2:
    	case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
    	case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_COMPLETE:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
    	case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
    	case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_WAIT:
    	case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_COMPLETE:
    	case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
    	case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE:
		case BOOT_LOADER_STATE_BANK0_UPDATE_CHECK:
		case BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
		case BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE:
    	case BOOT_LOADER_STATE_FINALIZE:
    	    switch(firmware_update_control_block_bank0->image_flag)
    	    {
    	        case LIFECYCLE_STATE_BLANK:
    	        	switch(secure_boot_state)
					{
    	        		case BOOT_LOADER_STATE_BANK0_CHECK:
							printf("start installing user program.\r\n");
							if (firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_INSTALLING)
							{
								secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE;
							}
							else
							{
								printf("erase bank1 secure boot mirror area...");
								flash_api_error_code = R_FLASH_Erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
								if(FLASH_SUCCESS != flash_api_error_code)
								{
									printf("NG\r\n");
									printf("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
									secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
									secure_boot_error_code = BOOT_LOADER_FAIL;
									break;
								}
								secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT;
							}
							break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE:
							if (firmware_update_control_block_bank1->image_flag != LIFECYCLE_STATE_INSTALLING)
							{
	    	        	        if (FLASH_SUCCESS == flash_error_code)
	    	        	        {
	    	        	            printf("OK\r\n");
	    	        	        }
	    	        	        else
	    	        	        {
	    	        	            printf("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
	    	        	            printf("system error.\r\n");
	    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
	    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
	    	        	            break;
	    	        	        }
	    	        	    }
    	        	        printf("copy secure boot (part1) from bank0 to bank1...");
    	        	        flash_api_error_code = R_FLASH_Write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE);
							if(FLASH_SUCCESS != flash_api_error_code)
							{
								printf("NG\r\n");
								printf("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
								secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
								secure_boot_error_code = BOOT_LOADER_FAIL;
								break;
							}
							secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1;
							break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1:
    	        	        if (FLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            printf("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            printf("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            printf("system error.\r\n");
    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
    	        	            break;
    	        	        }
    	        	        printf("copy secure boot (part2) from bank0 to bank1...");
							if(BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM > 0)
							{
								flash_api_error_code = R_FLASH_Write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, 8 * FLASH_CF_SMALL_BLOCK_SIZE);
								if(FLASH_SUCCESS != flash_api_error_code)
								{
									printf("NG\r\n");
									printf("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
									secure_boot_error_code = BOOT_LOADER_FAIL;
									break;
								}
							}
							else
							{
								secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2;
							}
							secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2;
							break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2:
    	        	        if (FLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            printf("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            printf("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            printf("system error.\r\n");
    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
    	        	            break;
    	        	        }
							if (firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_INSTALLING)
							{
								memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
			    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_VALID;
				    	    	printf("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
				    	    	printf("bank1(temporary area) block0 erase (to update LIFECYCLE_STATE)...");
				    	    	flash_api_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
				    	        if (FLASH_SUCCESS != flash_api_error_code)
				    	        {
				    	            printf("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
				    	            printf("system error.\r\n");
									secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
									secure_boot_error_code = BOOT_LOADER_FAIL;
				    	            break;
				    	        }
				    			secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT;
							}
							else
							{
								printf("========== install user program phase ==========\r\n");
								printf("erase install area (data flash): ");
								flash_api_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
								if (FLASH_SUCCESS != flash_api_error_code)
								{
									printf("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
									printf("system error.\r\n");
									secure_boot_error_code = BOOT_LOADER_FAIL;
									break;
								}
								secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_WAIT;
							}
							break;

    	        		case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE:
    	        	        if (FLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            printf("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            printf("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            printf("system error.\r\n");
    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
    	        	            break;
    	        	        }
							printf("erase install area (code flash): ");
							flash_api_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								printf("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
								printf("system error.\r\n");
								secure_boot_error_code = BOOT_LOADER_FAIL;
								break;
							}
							secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT;
							break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE:
    	        	        if (FLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            printf("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            printf("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            printf("system error.\r\n");
    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
    	        	            break;
    	        	        }
    	        	        sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
    	        	        sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
							sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A];
							sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_A;
    	        	        printf("send \"%s\" via UART.\r\n", INITIAL_FIRMWARE_FILE_NAME);
							secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT;
    	        	        break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT:
							/* install code flash area */
							if(!firm_block_read(load_firmware_control_block.flash_buffer, load_firmware_control_block.offset))
							{
								flash_api_error_code = R_FLASH_Write((uint32_t)load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
								if (FLASH_SUCCESS != flash_api_error_code)
								{
									printf("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
									printf("system error.\r\n");
									secure_boot_error_code = BOOT_LOADER_FAIL;
									break;
								}
								secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT;
							}
							break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
    	        	        if (FLASH_SUCCESS != flash_error_code)
    	        	        {
    	        	            printf("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            printf("system error.\r\n");
    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
    	        	            break;
    	        	        }

							load_firmware_control_block.offset += FLASH_CF_MEDIUM_BLOCK_SIZE;
							load_firmware_control_block.progress = (uint32_t)(((float)(load_firmware_control_block.offset)/(float)((FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))*100));
							printf("installing firmware...%d%(%d/%dKB).\r", load_firmware_control_block.progress, load_firmware_control_block.offset/1024, (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER)/1024);
							if(load_firmware_control_block.offset < (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
							{
								/* one more loop */
								secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT;
							}
							else if(load_firmware_control_block.offset == (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
							{
								printf("\n");
								printf("completed installing firmware.\r\n");
				    	    	printf("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
			    	            printf("bank1(temporary area) on code flash integrity check...");

								/* Firmware verification for the signature type. */
								if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
								{
									uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
								    /* Hash message */
								    struct tc_sha256_state_struct xCtx;
								    tc_sha256_init(&xCtx);
								    tc_sha256_update(&xCtx,
								    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
											(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
								    tc_sha256_final(hash_sha256, &xCtx);
	    							verification_result = memcmp(firmware_update_control_block_bank1->signature, hash_sha256, sizeof(hash_sha256));
					    	    }
					    	    else if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
					    	    {
									verification_result = firmware_verification_sha256_ecdsa(
																		(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																		(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																		firmware_update_control_block_bank1->signature,
																		firmware_update_control_block_bank1->signature_size);
								}
								else
								{
									verification_result = -1;
								}

								if(0 == verification_result)
								{
									printf("OK\r\n");
									load_const_data_control_block.offset = 0;
									secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_WAIT;
								}
								else
								{
									printf("NG\r\n");
	    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
	    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
								}
							}
							else
							{
								printf("\n");
								printf("fatal error occurred.\r\n");
    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
							}
							break;

						case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_WAIT:
							/* install data flash area */
							if(!const_data_block_read(load_const_data_control_block.flash_buffer, load_const_data_control_block.offset))
							{
								secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_COMPLETE;
							}
							break;

						case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_COMPLETE:
							flash_api_error_code = R_FLASH_Write((uint32_t)&load_const_data_control_block.flash_buffer[load_const_data_control_block.offset/4], (uint32_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS + load_const_data_control_block.offset, FLASH_DF_BLOCK_SIZE);
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								printf("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
								printf("system error.\r\n");
								secure_boot_error_code = BOOT_LOADER_FAIL;
								break;
							}
							secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_WAIT;
							break;

						case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
    	            		/* this state will be update by flash callback */
							break;
						
						case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE:
    	        	        if (FLASH_SUCCESS != flash_error_code)
    	        	        {
    	        	            printf("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            printf("system error.\r\n");
    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
    	        	            break;
    	        	        }

							load_const_data_control_block.offset += FLASH_DF_BLOCK_SIZE;
							load_const_data_control_block.progress = (uint32_t)(((float)(load_const_data_control_block.offset)/(float)((FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))*100));
							static uint32_t previous_offset = 0;
							if(previous_offset != (load_const_data_control_block.offset/1024))
							{
								printf("installing const data...%d%(%d/%dKB).\r", load_const_data_control_block.progress, load_const_data_control_block.offset/1024, (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER)/1024);
								previous_offset = load_const_data_control_block.offset/1024;
							}
							if(load_const_data_control_block.offset < (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))
							{
								/* one more loop */
								secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_READ_COMPLETE;
							}
							else if(load_const_data_control_block.offset == (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))
							{
								printf("\n");
								printf("completed installing const data.\r\n");
								printf("software reset...\r\n");
								R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
								software_reset();
							}
							else
							{
								printf("\n");
								printf("fatal error occurred.\r\n");
    	        				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
    	        				secure_boot_error_code = BOOT_LOADER_FAIL;
							}
							break;
					}
    				break;
    	        case LIFECYCLE_STATE_TESTING:
    	            printf("illegal status\r\n");
    	            printf("swap bank...");
    	            R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
    	            bank_swap_with_software_reset();
    	            while(1);
    	            break;

    	        case LIFECYCLE_STATE_VALID:
					switch(secure_boot_state)
					{
						case BOOT_LOADER_STATE_BANK0_UPDATE_CHECK:
			    	    	printf("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank0->signature_type);
		    	            printf("bank0(execute area) on code flash integrity check...");
							/* Firmware verification for the signature type. */
							if (!strcmp((const char *)firmware_update_control_block_bank0->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
							{
							    /* Hash message */
								uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
							    struct tc_sha256_state_struct xCtx;
							    tc_sha256_init(&xCtx);
							    tc_sha256_update(&xCtx,
							    		(uint8_t*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
										(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
							    tc_sha256_final(hash_sha256, &xCtx);
				    	        verification_result = memcmp(firmware_update_control_block_bank0->signature, hash_sha256, sizeof(hash_sha256));
				    	    }
				    	    else if (!strcmp((const char *)firmware_update_control_block_bank0->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
				    	    {
								verification_result = firmware_verification_sha256_ecdsa(
																	(const uint8_t *)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																	(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																	firmware_update_control_block_bank0->signature,
																	firmware_update_control_block_bank0->signature_size);
							}
							else
							{
								verification_result = -1;
							}

							if(0 == verification_result)
		    	            {
		    	                printf("OK\r\n");
		    	            	if(firmware_update_control_block_bank1->image_flag != LIFECYCLE_STATE_BLANK)
		    	            	{
		    	                    printf("erase install area (code flash): ");
		    	                    flash_api_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
		    	                    if (FLASH_SUCCESS != flash_api_error_code)
		    	                    {
		    	                        printf("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
		    	                        printf("system error.\r\n");
										secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
										secure_boot_error_code = BOOT_LOADER_FAIL;
					    	            break;
					    	        }
									secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT;
								}
								else
								{
									secure_boot_state = BOOT_LOADER_STATE_FINALIZE;
								}
		    	            }
		    	            else
		    	            {
		    					printf("NG.\r\n");
		    					printf("Code flash is completely broken.\r\n");
		    					printf("Please erase all code flash.\r\n");
		    					printf("And, write secure boot using debugger.\r\n");
								secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
		    					secure_boot_error_code = BOOT_LOADER_FAIL;
		    	            }
		    	            break;

				    	case BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
				    		/* this state will be update by flash callback */
				    		break;

		    	        case BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE:
					        if (FLASH_SUCCESS == flash_error_code)
					        {
					            printf("OK\r\n");
								secure_boot_state = BOOT_LOADER_STATE_FINALIZE;
					        }
					        else
					        {
					            printf("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
					            printf("system error.\r\n");
					            secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
					        }
		    	        	
		    	        	break;
		    	        
		    	        case BOOT_LOADER_STATE_FINALIZE:
	    	                printf("jump to user program\r\n");
	    	                R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
	    	                secure_boot_error_code = BOOT_LOADER_SUCCESS;
		    	        	break;
		    	    }
   	            	break;

    	        default:
    	            printf("illegal flash rom status code 0x%x.\r\n", firmware_update_control_block_bank0->image_flag);
        	    	printf("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
    	            printf("bank1(temporary area) on code flash integrity check...");

					/* Firmware verification for the signature type. */
					if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
					{
						uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
					    /* Hash message */
					    struct tc_sha256_state_struct xCtx;
					    tc_sha256_init(&xCtx);
					    tc_sha256_update(&xCtx,
					    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
								(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
					    tc_sha256_final(hash_sha256, &xCtx);
						verification_result = memcmp(firmware_update_control_block_bank1->signature, hash_sha256, sizeof(hash_sha256));
		    	    }
		    	    else if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
		    	    {
						verification_result = firmware_verification_sha256_ecdsa(
															(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
															(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
															firmware_update_control_block_bank1->signature,
															firmware_update_control_block_bank1->signature_size);
					}
					else
					{
						printf("This Firmware Verification Type is not implemented yet.\r\n");
						verification_result = -1;
					}

					if(0 == verification_result)
    	            {
    	                printf("OK\r\n");
    	                R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
    	                bank_swap_with_software_reset();
    	            }
    	            else
    	            {
    	                printf("NG\r\n");
    	                R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
    	                software_reset();
    	            }
    	            break;
    	    }
    }
    return secure_boot_error_code;
}

static void software_reset(void)
{
	/* stop all interrupt completely */
    set_psw(0);
    R_BSP_InterruptsDisable();
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    SYSTEM.SWRR = 0xa501;
    while(1);   /* software reset */
}

static void bank_swap_with_software_reset(void)
{
	/* stop all interrupt completely */
    set_psw(0);
    R_BSP_InterruptsDisable();
    R_FLASH_Control(FLASH_CMD_BANK_TOGGLE, NULL);
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    SYSTEM.SWRR = 0xa501;
    while(1);   /* software reset */
}


/***********************************************************************************************************************
* Function Name: firm_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static int32_t firm_block_read(uint32_t *firmware, uint32_t offset)
{
	int32_t error_code = -1;
	if (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag)
	{
		memcpy(firmware, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer, FLASH_CF_MEDIUM_BLOCK_SIZE);
		sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
		error_code = 0;
	}
	else if  (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag)
	{
		memcpy(firmware, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer, FLASH_CF_MEDIUM_BLOCK_SIZE);
		sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
		error_code = 0;
	}
	else
	{
		error_code = -1;
	}
	return error_code;
}

/***********************************************************************************************************************
* Function Name: const_data_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static int32_t const_data_block_read(uint32_t *const_data, uint32_t offset)
{
	int32_t error_code = -1;
	if (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag)
	{
		memcpy(const_data, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer, FLASH_CF_MEDIUM_BLOCK_SIZE);
		sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
		error_code = 0;
	}
	else if  (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag)
	{
		memcpy(const_data, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer, FLASH_CF_MEDIUM_BLOCK_SIZE);
		sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
		error_code = 0;
	}
	else
	{
		error_code = -1;
	}
	return error_code;
}

/*****************************************************************************
* Function Name: my_sci_callback
* Description  : This is a template for an SCI Async Mode callback function.
* Arguments    : pArgs -
*                pointer to sci_cb_p_args_t structure cast to a void. Structure
*                contains event and associated data.
* Return Value : none
******************************************************************************/
uint32_t error_count1 = 0;
uint32_t error_count2 = 0;
uint32_t rcv_count1 = 0;
uint32_t rcv_count2 = 0;

static void my_sci_callback(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
    	if(sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size < sizeof(sci_receive_control_block.p_sci_buffer_control->buffer))
    	{
			R_SCI_Receive(p_args->hdl, &sci_receive_control_block.p_sci_buffer_control->buffer[sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size++], 1);
			if (sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size == sizeof(sci_receive_control_block.p_sci_buffer_control->buffer))
			{
				sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size = 0;
				sci_receive_control_block.p_sci_buffer_control->buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL;
				sci_receive_control_block.total_byte_size += FLASH_CF_MEDIUM_BLOCK_SIZE;
				if (BOOT_LOADER_SCI_CONTROL_BLOCK_A == sci_receive_control_block.current_state)
				{
					sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_B;
					sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B];
				}
				else
				{
					sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_A;
					sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A];
				}
			}
        	rcv_count1++;
    	}
    	rcv_count2++;
    }
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	nop();
    	error_count1++;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	nop();
    	error_count2++;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	nop();
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	nop();
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */

/***********************************************************************************************************************
* Function Name: my_flash_callback
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static void my_flash_callback(void *event)
{
	uint32_t event_code = FLASH_ERR_FAILURE;
	event_code = *((uint32_t*)event);

    if((event_code == FLASH_INT_EVENT_WRITE_COMPLETE) || (event_code == FLASH_INT_EVENT_ERASE_COMPLETE))
    {
    	flash_error_code = FLASH_SUCCESS;
    }

	switch(secure_boot_state)
	{
		case BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT:
			secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE;
			break;
		case BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT:
			secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE;
			break;
		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
			secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE;
			break;
		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
			secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1;
			break;
		case BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
			secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2;
			break;
		case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
			secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE;
			break;
		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
			secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE;
			break;
		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
			secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE;
			break;
		case BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
			secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE;
			break;
		case BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
			secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE;
			break;
		default:
			break;
	}
}

/***********************************************************************************************************************
 * Function Name: my_sw_charput_function
 * Description  : char data output API
 * Arguments    : data -
 *                    Send data with SCI
 * Return Value : none
 **********************************************************************************************************************/
void my_sw_charput_function(uint8_t data)
{
    uint32_t arg = 0;
    /* do not call printf()->charput in interrupt context */
    do
    {
        /* Casting void pointer is used for address. */
        R_SCI_Control(my_sci_handle, SCI_CMD_TX_Q_BYTES_FREE, (void*)&arg);
    }
    while (SCI_CFG_CH8_TX_BUFSIZ != arg);
    /* Casting uint8_t pointer is used for address. */
    R_SCI_Send(my_sci_handle, (uint8_t*)&data, 1);

    return;
}
/***********************************************************************************************************************
 End of function my_sw_charput_function
 **********************************************************************************************************************/

void my_sw_charget_function(void)
{

}

static const uint8_t *get_status_string(uint8_t status)
{
	static const uint8_t status_string[][32] = {{"LIFECYCLE_STATE_BLANK"},
	                                            {"LIFECYCLE_STATE_TESTING"},
	                                            {"LIFECYCLE_STATE_INSTALLING"},
	                                            {"LIFECYCLE_STATE_VALID"},
	                                            {"LIFECYCLE_STATE_INVALID"},
	                                            {"LIFECYCLE_STATE_UNKNOWN"}};
	const uint8_t *tmp;

	if(status == LIFECYCLE_STATE_BLANK)
	{
		tmp = status_string[0];
	}
	else if(status == LIFECYCLE_STATE_TESTING)
	{
		tmp = status_string[1];
	}
	else if(status == LIFECYCLE_STATE_INSTALLING)
	{
		tmp = status_string[2];
	}
	else if(status == LIFECYCLE_STATE_VALID)
	{
		tmp = status_string[3];
	}
	else if(status == LIFECYCLE_STATE_INVALID)
	{
		tmp = status_string[4];
	}
	else
	{
		tmp = status_string[5];
	}
	return tmp;
}

static int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize)
{
    int32_t xResult = -1;
    uint8_t pucHash[TC_SHA256_DIGEST_SIZE];
    uint8_t data_length;
    uint8_t public_key[64];
    uint8_t binary[256];
    uint8_t *head_pointer, *current_pointer, *tail_pointer;;

    /* Hash message */
    struct tc_sha256_state_struct xCtx;
    tc_sha256_init(&xCtx);
    tc_sha256_update(&xCtx, pucData, ulSize);
    tc_sha256_final(pucHash, &xCtx);

    /* extract public key from code_signer_public_key (pem format) */
    head_pointer = (uint8_t*)strstr((char *)code_signer_public_key, "-----BEGIN PUBLIC KEY-----");
    if(head_pointer)
    {
    	head_pointer += strlen("-----BEGIN PUBLIC KEY-----");
        tail_pointer = (uint8_t*)strstr((char *)code_signer_public_key, "-----END PUBLIC KEY-----");
    	base64_decode(head_pointer, binary, tail_pointer - head_pointer);
    	current_pointer = binary;
		data_length = *(current_pointer + 1);
    	while(1)
    	{
    		switch(*current_pointer)
    		{
    			case 0x30: /* found "SEQUENCE" */
    				current_pointer += 2;
    				break;
    			case 0x03: /* found BIT STRING (maybe public key) */
        			if(*(current_pointer + 1) == 0x42)
        			{
        				memcpy(public_key, current_pointer + 4, 64);
						/* Verify signature */
						if(uECC_verify(public_key, pucHash, TC_SHA256_DIGEST_SIZE, pucSignature, uECC_secp256r1()))
						{
							xResult = 0;
						}
        			}
    				current_pointer += *(current_pointer + 1) + 2;
					break;
    			default:
    				current_pointer += *(current_pointer + 1) + 2;
    				break;
    		}
			if((current_pointer - binary) > data_length)
			{
				/* parsing error */
				break;
			}
    	}
    }
    return xResult;
}
