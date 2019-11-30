/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2014-2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_flash_rx.c
* Description  : This module implements the FLASH API
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 12.07.2014 1.00    First Release
*              : 13.01.2015 1.10    Updated for RX231
*              : 21.07.2016 1.20    Fixed initialization of g_current_parameters for Flash Type 3.
*              : 12.08.2016 2.00    Modified for BSPless operation (softwareLock()/Unlock()).
*              : 05.10.2016 3.00    Modified API functions to call either flash_api_xxx Flash Type 2 functions
*                                   or r_flash_xxx Flash Type 1, 3, 4 functions.
*              : 31.10.2017 3.10    Added function R_FLASH_Close().
*              : 23.02.2018 3.20    Removed unused variable warnings in R_FlashCodeCopy().
*              : 19.04.2019 4.00    Added support for GNUC and ICCRX.
*                                   Removed support for flash type 2.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_flash_rx_if.h"
#include "r_flash_rx.h"
#include "r_flash_fcu.h"
#include "r_flash_group.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
External functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
int32_t g_flash_lock;                                       // for locking the driver
flash_states_t g_flash_state = FLASH_UNINITIALIZED;         // for state in when driver locked
FCU_BYTE_PTR g_pfcu_cmd_area = (uint8_t*) FCU_COMMAND_AREA; // sequencer command pointer


/***********************************************************************************************************************
* Function Name: R_FLASH_Open
* Description  : Function will initialize the flash peripheral.
* Arguments    : void
* Return Value : FLASH_SUCCESS -
*                    API initialized successfully.
*                FLASH_ERR_BUSY -
*                    API has already been initialized and another operation is ongoing.
*                FLASH_ERR_FAILURE -
*                    Initialization failed.
*                    A RESET was performed on the Flash circuit to rectify any internal errors
***********************************************************************************************************************/
flash_err_t R_FLASH_Open(void)
{
    flash_err_t err;

    /* If going to write to code flash, copy essential routines to RAM
     * so can execute from there. Note that some MCUs can run from and
     * write to ROM simultaneously under certain circumstances (run from
     * one region and write to another).
     */
    R_FlashCodeCopy();


    /* Perform flash and driver initialization */
    err = r_flash_open();
    if (err == FLASH_SUCCESS)
    {
        g_flash_state = FLASH_READY;
    }

    return(err);
}


/******************************************************************************
* Function Name: R_FlashCodeCopy
* Description  : Copies Flash driver code necessary for code flash program/erase
*                operations from ROM to RAM.
* Arguments    : none
* Return Value : none
******************************************************************************/
void R_FlashCodeCopy(void)
{
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)

#if ((FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0) || (FLASH_IN_DUAL_BANK_MODE == 1))
    uint8_t * p_rom_section;    // ROM source location
    uint8_t * p_ram_section;    // RAM copy destination
    uint32_t  bytes_copied;
#endif /* ((FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0) || (FLASH_IN_DUAL_BANK_MODE == 1)) */

#if defined(__CCRX__) || defined(__GNUC__)

#if (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0)
    R_BSP_SECTION_OPERATORS_INIT(RPFRAM)
    R_BSP_SECTION_OPERATORS_INIT(PFRAM)
#endif /* (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0) */
#ifdef FLASH_IN_DUAL_BANK_MODE
    R_BSP_SECTION_OPERATORS_INIT(RPFRAM2)
    R_BSP_SECTION_OPERATORS_INIT(PFRAM2)
#endif /* FLASH_IN_DUAL_BANK_MODE */

#if (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0)
    /* Initialize pointers */
    p_ram_section = (uint8_t *)R_BSP_SECTOP(RPFRAM);
    p_rom_section = (uint8_t *)R_BSP_SECTOP(PFRAM);

    /* Copy code from ROM to RAM. */
    for (bytes_copied = 0; bytes_copied < R_BSP_SECSIZE(PFRAM); bytes_copied++)
    {
        p_ram_section[bytes_copied] = p_rom_section[bytes_copied];
    }
#endif /* (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0) */

#ifdef FLASH_IN_DUAL_BANK_MODE
    /* Initialize pointers */
    p_ram_section = (uint8_t *)R_BSP_SECTOP(RPFRAM2);
    p_rom_section = (uint8_t *)R_BSP_SECTOP(PFRAM2);

    /* Copy code from ROM to RAM. */
    for (bytes_copied = 0; bytes_copied < R_BSP_SECSIZE(PFRAM2); bytes_copied++)
    {
        p_ram_section[bytes_copied] = p_rom_section[bytes_copied];
    }
#endif /* FLASH_IN_DUAL_BANK_MODE */

#elif defined(__ICCRX__)

#if (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0)
    R_BSP_SECTION_OPERATORS_INIT(PFRAM)
    R_BSP_SECTION_OPERATORS_INIT(PFRAM_init);
#endif /* (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0) */
#ifdef FLASH_IN_DUAL_BANK_MODE
    R_BSP_SECTION_OPERATORS_INIT(PFRAM2)
    R_BSP_SECTION_OPERATORS_INIT(PFRAM2_init);
#endif /* FLASH_IN_DUAL_BANK_MODE */

#if (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0)
    /* Initialize pointers */
    p_ram_section = (uint8_t *) R_BSP_SECTOP(PFRAM);
    p_rom_section = (uint8_t *) R_BSP_SECTOP(PFRAM_init);

    /* Copy code from ROM to RAM. */
    for (bytes_copied = 0; bytes_copied < R_BSP_SECSIZE(PFRAM_init); bytes_copied++)
    {
        p_ram_section[bytes_copied] = p_rom_section[bytes_copied];
    }
#endif /* (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0) */

#ifdef FLASH_IN_DUAL_BANK_MODE
    /* Initialize pointers */
    p_rom_section = (uint8_t *)R_BSP_SECTOP(PFRAM2);
    p_ram_section = (uint8_t *)R_BSP_SECTOP(PFRAM2_init);

    /* Copy code from ROM to RAM. */
    for (bytes_copied = 0; bytes_copied < R_BSP_SECSIZE(PFRAM2_init); bytes_copied++)
    {
        p_ram_section[bytes_copied] = p_rom_section[bytes_copied];
    }
#endif /* FLASH_IN_DUAL_BANK_MODE */

#endif /* defined(__CCRX__) || defined(__GNUC__) */

#endif /* (FLASH_CFG_CODE_FLASH_ENABLE == 1) */
}


/***********************************************************************************************************************
* Function Name: R_FLASH_Close
* Description  : This function closes the flash driver.
* Arguments    : None
* Return Value : see called function
***********************************************************************************************************************/
flash_err_t R_FLASH_Close(void)
{
    return(r_flash_close());
}



/* FUNCTIONS WHICH MUST BE RUN FROM RAM FOLLOW */
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#define FLASH_PE_MODE_SECTION    R_BSP_ATTRIB_SECTION_CHANGE(P, FRAM)
#define FLASH_SECTION_CHANGE_END R_BSP_ATTRIB_SECTION_CHANGE_END
#else
#define FLASH_PE_MODE_SECTION
#define FLASH_SECTION_CHANGE_END
#endif

/***********************************************************************************************************************
* Function Name: R_FLASH_Erase
* Description  : This function erases the specified Code or Data Flash blocks.
* Arguments    : see called function
* Return Value : see called function
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t R_FLASH_Erase(flash_block_address_t block_start_address, uint32_t num_blocks)
{
    return(r_flash_erase(block_start_address, num_blocks));
}

#ifndef FLASH_NO_BLANK_CHECK
/***********************************************************************************************************************
* Function Name: R_FLASH_BlankCheck
* Description  : Function performs a blank check on the specified Flash area.
* Arguments:     see called function
* Return Value : see called function
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t R_FLASH_BlankCheck(uint32_t address, uint32_t num_bytes, flash_res_t *result)
{
    return(r_flash_blankcheck(address, num_bytes, result));
}

#endif


/***********************************************************************************************************************
* Function Name: R_FLASH_Write
* Description  : Function will write to the specified Code or Data Flash memory area.
* Arguments    : see called function
* Return Value : see called function
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t R_FLASH_Write(uint32_t src_address, uint32_t dest_address, uint32_t num_bytes)
{
    /* Call the MCU specific write function which handles control commands for the target MCU */
    return(r_flash_write(src_address, dest_address,  num_bytes));
}


/***********************************************************************************************************************
* Function Name: R_FLASH_Control
* Description  : This function performs special configuration and operational commands.
* Arguments    : see called function
* Return Value : see called function
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t R_FLASH_Control(flash_cmd_t cmd, void *pcfg)
{
    /* Call the MCU specific control function which handles control commands for the target MCU */
    return(r_flash_control(cmd, pcfg));
}


/***********************************************************************************************************************
* Function Name: R_FLASH_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
uint32_t R_FLASH_GetVersion (void)
{
    /* These version macros are defined in r_flash_if.h. */
    return ((((uint32_t)FLASH_RX_VERSION_MAJOR) << 16) | (uint32_t)FLASH_RX_VERSION_MINOR);
}

/******************************************************************************
* Function Name: flash_lock_state
* Description  : Attempt to grab the flash state to perform an operation
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    : new_state -
*                    Which state to attempt to transition to
* Return Value : FLASH_SUCCESS -
*                    State was grabbed
*                FLASH_ERR_BUSY -
*                    Flash is busy with another operation
******************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t flash_lock_state (flash_states_t new_state)
{
    /* Attempt to get lock */
    /* NOTE: Do not use BSP lock so can build without BSP if desired */
    if (flash_softwareLock(&g_flash_lock) == true)
    {
        /* Lock taken, we can change state */
        g_flash_state = new_state;
        return FLASH_SUCCESS;
    }
    else
    {
        return FLASH_ERR_BUSY;      // another operation in progress
    }
}


/******************************************************************************
* Function Name: flash_release_state
* Description  : Release state so another flash operation can take place
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    : none
* Return Value : none
******************************************************************************/
FLASH_PE_MODE_SECTION
void flash_release_state (void)
{

    /* Done with current operation */
    g_flash_state = FLASH_READY;

    /* Release hold on lock */
    flash_softwareUnlock(&g_flash_lock);
}

/***********************************************************************************************************************
* Function Name: flash_softwareLock
* Description  : Attempt to acquire the lock that has been sent in.
* Arguments    : plock -
*                    Pointer to lock structure with lock to try and acquire.
* Return Value : true -
*                    Lock was acquired.
*                false -
*                    Lock was not acquired.
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
bool flash_softwareLock(int32_t * const plock)
{
    bool ret = false;

    /* Variable used in trying to acquire lock. Using the xchg instruction makes this atomic */
    int32_t is_locked = true;

    /* Try to acquire semaphore to obtain lock */
    R_BSP_EXCHANGE(&is_locked, plock);

    /* Check to see if semaphore was successfully taken */
    if (is_locked == false)
    {
        /* Lock obtained, return success. */
        ret = true;
    }
    else
    {
        /* Lock was not obtained, another task already has it. */
    }

    return ret;
} /* End of function flash_softwareLock() */


/***********************************************************************************************************************
* Function Name: flash_softwareUnlock
* Description  : Release hold on lock.
* Arguments    : plock -
*                    Pointer to lock structure with lock to release.
* Return Value : true -
*                    Lock was released.
*                false -
*                    Lock was not released.
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
bool flash_softwareUnlock(int32_t * const plock)
{
    /* Set lock back to unlocked. */
    *plock = false;

    return true;
} /* End of function flash_softwareUnlock() */


FLASH_SECTION_CHANGE_END /* end FLASH SECTION FRAM */
