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
#include "bsp_flash_config.h"

/**
  Please follow the next setp to configure the flash device:
    1, define the SPI/SPIQ/QPI command list;
    2, define the device information;
    3, Implement all of the interface;
    4, please define the MACRO to enable the flash device;
**/


#if defined(SF_FLASH_MICRON_N25Q128A)
/* just task MICRON_N25Q128A as a sample, don't need to enable it if use device N25Q128A. */

/************************************************************************************

    1, define the SPI/SPIQ/QPI command list; 

*************************************************************************************/
/*
    SFC driver send the command to flash for switch the flash mode to SPI mode:
    cmdlist_switchspi_micron[] = {
    EX_SPI,                        // EX_SPI: SPI mode 
    1, 0x06,                       // 1: 1 byte command     0x6:  write enable command  
    EX_SPI,                       //  EX_SPI: SPI mode 
    2, 0xB1, 0x1,                // 2: 2 bytes command    0xBl:  write status register comand    0x1: set 1 to QE bit   
    EX_SF_UNDEF               //command end string
};
*/
const unsigned char cmdlist_switchspi_micron[] = {
    EX_SPI, 1, 0x06, EX_SPI, 2, 0xB1, 0x1, EX_SF_UNDEF
};

/*
    SFC driver send the command to flash for switch the flash mode to SPIQ mode:
    cmdlist_switchspi_micron[] = {
    EX_SPIQ,                      // EX_SPIQ: SPIQ mode 
    1, 0x06,                       // 1: 1 byte command     0x6:  write enable command  
    EX_SPI,                       // EX_SPIQ: SPIQ mode 
    2, 0xB1, 0x1,                // 2: 2 bytes command    0xBl:  write status register comand    0x1: set 1 to QE bit   
    EX_SF_UNDEF               //command end string
};
*/
const unsigned char cmdlist_switchspiq_micron[] = {
    EX_SPIQ, 1, 0x06, EX_SPI, 2, 0xB1, 0x0, EX_SF_UNDEF
};

/*
    This is reserved for QPI mode. 
    As it doesn't support QPI mode on MT76*7 please just set with same as SPIQ.
};
*/
const unsigned char cmdlist_switchqpi_micron[] = {
    EX_SPIQ, 1, 0x06, EX_SPI, 2, 0xB1, 0x0, EX_SF_UNDEF
};

/************************************************************************************

    2, define the device information
     
*************************************************************************************/
const flash_info_t customer_flash_info={"N25Q128A",0X20,0XBA180000,65536,256,0X32,0X6B,0XEB};


/************************************************************************************
    Keep the function, don't need change anything.    
*************************************************************************************/
const unsigned char* customer_switch_spi_mode_command(void)
{
    //Flash switch SPI mode command
    return cmdlist_switchspi_micron;
}

/************************************************************************************
    Keep the function, don't need change anything.    
*************************************************************************************/
const unsigned char* customer_switch_spiq_mode_command(void)
{
    //Flash switch SPI Quad mode command
    return cmdlist_switchspiq_micron;
}

/************************************************************************************
    Keep the function, don't need change anything.   
    SFC driver will call this api to register flash  in hal_flash_init */
*************************************************************************************/
const flash_info_t* customer_flash_register(void)
{
    return (&customer_flash_info);
}

/************************************************************************************

    3, Implement all the follow API
     
*************************************************************************************/
/* return the suspend status bit of status register*/ 
unsigned char customer_flash_suspend_bit(void)
{   
    /* From device specification the suspend statut bit with in flag register on SF_FLASH_MICRON_N25Q128A.
           Note: difference vendor have different implement, please should check the flash device specification.
       */
    unsigned char suspend_bit = 0x42;
    return suspend_bit;
}

unsigned char customer_flash_read_SR2_comand(void)
{
    /* this command is used to read the QE bit, for N25Q128A the read flag register is 0x70. */
    unsigned char cmd = 0x70;
    return cmd;
}	
#elif defined(SF_FLASH_XXX_XXX_XXX)


/* 
  1,  If the default driver can't fulfill the selected speicial flash device, please add the code at this region.
  2,  please enable it  by define the MACRO SF_FLASH_XXX_XXX_XXX in the head of this file.
*/



#else
/* 
  Should not change the next code.
  if user doesn't select special flash device,  it will use it as the defualt.
*/

const unsigned char cmdlist_switchqpi_default[] = {
    EX_SPI, 1, 0x06, EX_SPI, 3, 0x01, 0x00, 0x02, EX_SPI, 1, 0x38, EX_SF_UNDEF
};
const unsigned char cmdlist_switchspi_default[] = {
    EX_QPI, 1, 0xFF, EX_SF_UNDEF
};
const unsigned char cmdlist_switchspiq_default[] = {
    EX_SPI, 1, 0x06, EX_SPI, 3, 0x01, 0x00, 0x02, EX_SF_UNDEF
};

const flash_info_t customer_flash_info;


/* suspend status bit in status register*/ 
unsigned char customer_flash_suspend_bit(void)
{   
    /* From device specification the suspend statut bit with in flag register. */
    unsigned char suspend_bit = 0x5;
    return suspend_bit;
}

const unsigned char* customer_switch_spi_mode_command(void)
{
    //Flash switch SPI mode command
	  return cmdlist_switchspi_default;
}

const unsigned char* customer_switch_spiq_mode_command(void)
{
    //Flash switch SPI Quad mode command
    return cmdlist_switchspiq_default;
}

unsigned char customer_flash_read_SR2_comand(void)
{
    //this command is used to read the QE bit 
    //read status register is 0x35
    unsigned char cmd = 0x5;
    return cmd;
}	


/*should call this api first in hal_flash_init to register  flash*/
const flash_info_t* customer_flash_register(void)
{
    return (const flash_info_t*)(NULL);
}
	
#endif

