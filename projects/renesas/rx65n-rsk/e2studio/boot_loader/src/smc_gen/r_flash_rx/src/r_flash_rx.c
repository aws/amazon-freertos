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
*              : 09.09.2019 4.30    Added copy in the case of ICCRX big endian to the function R_FlashCodeCopy().
*              : 18.11.2019 4.50    Modified comment of API function to Doxygen style.
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
 *******************************************************************************************************************//**
 * @brief   The function initializes the Flash FIT module.
 *          This function must be called before calling any other API functions.
 * @retval  FLASH_SUCCESS          Flash FIT module initialized successfully.
 * @retval  FLASH_ERR_BUSY         A different flash process is being executed, try again later.
 * @retval  FLASH_ERR_ALREADY_OPEN Open() called twice without an intermediate Close().
 * @details This function initializes the Flash FIT module, and if FLASH_CFG_CODE_FLASH_ENABLE is 1,
 *          copies the API functions necessary for code flash erasing/reprogramming into RAM
 *          (not including vector table). 
 *          Note that this function must be called before any other API function.
 */
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
#if defined(__LIT)
    for (bytes_copied = 0; bytes_copied < R_BSP_SECSIZE(PFRAM_init); bytes_copied++)
    {
        p_ram_section[bytes_copied] = p_rom_section[bytes_copied];
    }
#elif defined(__BIG)
    for (bytes_copied = 0; bytes_copied < R_BSP_SECSIZE(PFRAM_init); bytes_copied+=4)
    {
        /* Copy over data 4 byte at a time. */
        p_ram_section[bytes_copied]   = p_rom_section[bytes_copied+3];
        p_ram_section[bytes_copied+1] = p_rom_section[bytes_copied+2];
        p_ram_section[bytes_copied+2] = p_rom_section[bytes_copied+1];
        p_ram_section[bytes_copied+3] = p_rom_section[bytes_copied];
    }
#endif /* defined(__LIT) */
#endif /* (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0) */

#ifdef FLASH_IN_DUAL_BANK_MODE
    /* Initialize pointers */
    p_ram_section = (uint8_t *)R_BSP_SECTOP(PFRAM2);
    p_rom_section = (uint8_t *)R_BSP_SECTOP(PFRAM2_init);

    /* Copy code from ROM to RAM. */
#if defined(__LIT)
    for (bytes_copied = 0; bytes_copied < R_BSP_SECSIZE(PFRAM2_init); bytes_copied++)
    {
        p_ram_section[bytes_copied] = p_rom_section[bytes_copied];
    }
#elif defined(__BIG)
    for (bytes_copied = 0; bytes_copied < R_BSP_SECSIZE(PFRAM2_init); bytes_copied+=4)
    {
        /* Copy over data 4 byte at a time. */
        p_ram_section[bytes_copied]   = p_rom_section[bytes_copied+3];
        p_ram_section[bytes_copied+1] = p_rom_section[bytes_copied+2];
        p_ram_section[bytes_copied+2] = p_rom_section[bytes_copied+1];
        p_ram_section[bytes_copied+3] = p_rom_section[bytes_copied];
    }
#endif /* defined(__LIT) */
#endif /* FLASH_IN_DUAL_BANK_MODE */

#endif /* defined(__CCRX__) || defined(__GNUC__) */

#endif /* (FLASH_CFG_CODE_FLASH_ENABLE == 1) */
}


/***********************************************************************************************************************
 * Function Name: R_FLASH_Close
 *******************************************************************************************************************//**
 * @brief   The function closes the Flash FIT module.
 * @retval  FLASH_SUCCESS  Flash FIT module closed successfully.
 * @retval  FLASH_ERR_BUSY A different flash process is being executed, try again later.
 * @details This function closes the Flash FIT module.
 *          It disables the flash interrupts (if enabled) and sets the driver to an uninitialized state.
 */
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
 *******************************************************************************************************************//**
 * @brief     This function is used to erase the specified block in code flash or data flash.
 * @param[in] block_start_address Specifies the start address of block to erase.
 *                                The enum flash_block_address_t is defined in the corresponding MCU's
 *                                r_flash_rx\src\targets\mcu\r_flash_mcu.h file.
 *                                The blocks are labeled in the same fashion as they are
 *                                in the device's Hardware Manual.
 *                                For example, the block located at address 0xFFFFC000 is called Block 7
 *                                in the RX113 hardware manual, therefore "FLASH_CF_BLOCK_7" should be passed
 *                                for this parameter. 
 *                                Similarly, to erase Data Flash Block 0 which is located at address 0x00100000,
 *                                this argument should be FLASH_DF_BLOCK_0.
 * @param[in] num_blocks          Specifies the number of blocks to be erased.
 *                                For type 1 parts, address + num_blocks cannot cross a 256K boundary.
 * @retval    FLASH_SUCCESS       Operation successful.
 *                                (if non-blocking mode is enabled, this means the operation was started successfully).
 * @retval    FLASH_ERR_BLOCKS    Invalid number of blocks specified.
 * @retval    FLASH_ERR_ADDRESS   Invalid address specified.
 * @retval    FLASH_ERR_BUSY      A different flash process is being executed, or the module is not initialized.
 * @retval    FLASH_ERR_FAILURE   Erasing failure. Sequencer has been reset.
 *                                Or callback function not registered (in non-blocking mode)
 * @details   Erases a contiguous number of code flash or data flash memory blocks.
 *            The block size varies depending on MCU types.
 *            For example, on the RX111 both code and data flash block sizes are 1Kbytes.
 *            On the RX231 and RX23T the block size for code flash is 2 Kbytes and for data flash is 1Kbyte
 *            (no data flash on the RX23T).
 *            The equates FLASH_CF_BLOCK_SIZE for code flash and FLASH_DF_BLOCK_SIZE
 *            for data flash are provided for these values.
 *            The enum flash_block_address_t is configured at compile time based on the memory configuration of
 *            the MCU device specified in the r_bsp module.
 *            When the API is used in non-blocking mode,
 *            the FRDYI interrupt occurs after blocks for the specified number are erased,
 *            and then the callback function is called.
 * @note      In order to erase a code flash block, the area to be erased needs to be in a rewritable area.
 *            FLASH_TYPE_1 uses access windows to identify this.
 *            The other flash types use lock bits which must be off for erasing.
 */
FLASH_PE_MODE_SECTION
flash_err_t R_FLASH_Erase(flash_block_address_t block_start_address, uint32_t num_blocks)
{
    return(r_flash_erase(block_start_address, num_blocks));
}

#ifndef FLASH_NO_BLANK_CHECK
/***********************************************************************************************************************
 * Function Name: R_FLASH_BlankCheck
 *******************************************************************************************************************//**
 * @brief      This function is used to determine if the specified area in either code flash or data flash
 *             is blank or not.
 * @param[in]  address            The address of the area to blank check.
 * @param[in]  num_bytes          This is the number of bytes to be checked.
 *                                The number of bytes specified must be a multiple of FLASH_DF_MIN_PGM_SIZE
 *                                for a data flash address or FLASH_CF_MIN_PGM_SIZE for a code flash address.
 *                                These equates are defined in r_flash_rx\src\targets\<mcu>\r_flash_<mcu>.h
 *                                and are MCU specific.
 *                                For type 1 parts, address + num_bytes cannot cross a 256K boundary.
 * @param[out] blank_check_result In blocking mode, specify the memory address to which the blank check results will be
 *                                stored. In non-blocking mode, specify any value since this parameter is not used.
 * @retval     FLASH_SUCCESS      Operation successful.
 *                                (in non-blocking mode, this means the operation was started successfully)
 * @retval     FLASH_ERR_FAILURE  Blank check failed. Sequencer has been reset, or callback function not registered.
 *                                (in non-blocking mode)
 * @retval     FLASH_ERR_BUSY     A different flash process is being executed or the module is not initialized.
 * @retval     FLASH_ERR_BYTES    num_bytes was either too large or not a multiple of the minimum programming size
 *                                or exceed the maximum range.
 * @retval     FLASH_ERR_ADDRESS  Invalid address was input or address not divisible by the minimum programming size.
 * @retval     FLASH_ERR_NULL_PTR blank_check_result for storing blank check results was NULL.
 * @details    The flash area to write the program into must be blank.
 *             The result of the blank check operation is placed into "blank_check_result"
 *             when operation is in blocking mode.
 *             This variable is of type flash_res_t which is defined in r_flash_rx_if.h..
 *             If the API is used in non-blocking mode,
 *             the result of the blank check is passed as the argument of the callback function
 *             after the blank check is complete.
 */
FLASH_PE_MODE_SECTION
flash_err_t R_FLASH_BlankCheck(uint32_t address, uint32_t num_bytes, flash_res_t *blank_check_result)
{
    return(r_flash_blankcheck(address, num_bytes, blank_check_result));
}

#endif


/***********************************************************************************************************************
 * Function Name: R_FLASH_Write
 *******************************************************************************************************************//**
 * @brief     This function is used to write data to code flash or data flash.
 * @param[in] src_address       This is the first address  of the buffer containing the data to write to Flash.
 * @param[in] dest_address      This is the first address  of the code flash or data flash area to rewrite data.
 *                              The address specified must be divisible by the minimum programming size.
 *                              See Description below for important restrictions regarding this parameter.
 * @param[in] num_bytes         The number of bytes contained in the buffer specified with src_address.
 *                              This number must be a multiple of the minimum programming size
 *                              for memory area you are writing to.
 * @retval    FLASH_SUCCESS     Operation successful.
 *                              (in non-blocking mode, this means the operation was started successfully)
 * @retval    FLASH_ERR_FAILURE Programming failed.
 *                              Possibly the destination address under access window or lockbit control;
 *                              or callback function not present(in non-blocking mode)
 * @retval    FLASH_ERR_BUSY    A different flash process is being executed or the module is not initialized.
 * @retval    FLASH_ERR_BYTES   Number of bytes provided was not a multiple of the minimum programming size
 *                              or exceed the maximum range.
 * @retval    FLASH_ERR_ADDRESS Invalid address was input or address not divisible by the minimum programming size.
 * @details   Writes data to flash memory. Before writing to any flash area, the area must already be erased.
 *            When performing a write the user must make sure to start the write on an address divisible
 *            by the minimum programming size and make the number of bytes to write be a multiple of the minimum
 *            programming size. The minimum programming size differs depending on what MCU package is being used and
 *            whether the code flash or data flash is being written to.
 *            An area to write data to code flash must be rewritable area (access window or lockbit allowed).
 *            When the API is used in non-blocking mode, the callback function is called when all write operations
 *            are complete.
 * @note      FLASH_DF_MIN_PGM_SIZE defines the minimum data flash program size.
 *            FLASH_CF_MIN_PGM_SIZE defines the minimum code flash program size.
 */
FLASH_PE_MODE_SECTION
flash_err_t R_FLASH_Write(uint32_t src_address, uint32_t dest_address, uint32_t num_bytes)
{
    /* Call the MCU specific write function which handles control commands for the target MCU */
    return(r_flash_write(src_address, dest_address,  num_bytes));
}


/***********************************************************************************************************************
 * Function Name: R_FLASH_Control
 *******************************************************************************************************************//**
 * @brief         This function implements all functionality except for programming, erasing, and black check.
 * @param[in]     cmd                   Command to execute.
 * @param[in,out] pcfg                  Configuration parameters required by the specific command.
 *                                      This maybe NULL if the command does not require it.
 * @retval        FLASH_SUCCESS         Operation successful.
 *                                      (in non-blocking mode, this means the operations was started successfully)
 * @retval        FLASH_ERR_ADDRESS     Address is an invalid Code/Data Flash block start address.
 * @retval        FLASH_ERR_NULL_PTR    pcfg was NULL for a command that expects a configuration structure.
 * @retval        FLASH_ERR_BUSY        A different flash process is being executed or the module is not initialized.
 * @retval        FLASH_ERR_CMD_LOCKED  The flash control circuit was in a command locked state and was reset.
 * @retval        FLASH_ERR_ACCESSW     Access window error: Incorrect area specified.
 * @retval        FLASH_ERR_PARAM       Invalid command.
 * @details       This function is an expansion function that implements non-core functionality of the sequencer.
 *                Depending on the command type a different argument type has to be passed.
 *                See the "R_FLASH_Control" chapter in the application note for details.
 */
FLASH_PE_MODE_SECTION
flash_err_t R_FLASH_Control(flash_cmd_t cmd, void *pcfg)
{
    /* Call the MCU specific control function which handles control commands for the target MCU */
    return(r_flash_control(cmd, pcfg));
}


/***********************************************************************************************************************
 * Function Name: R_FLASH_GetVersion
 *******************************************************************************************************************//**
 * @brief   Returns the current version of the Flash FIT module.
 * @return  Version of the Flash FIT module.
 * @details This function will return the version of the currently installed FIT module. 
 *          The version number is encoded where the top 2 bytes are the major version number
 *          and the bottom 2 bytes are the minor version number.
 *          For example, Version 4.25 would be returned as 0x00040019.
 */
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
