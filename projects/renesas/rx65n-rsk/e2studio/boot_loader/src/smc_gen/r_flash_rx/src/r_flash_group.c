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
* Copyright (C) 2016-2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_flash_group.c
* Description  : This module implements functions common to Flash Types 1, 3, and 4.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 05.10.2016 1.00    First Release
*              : 07.02.2017 1.10    Modified for RX65N-2M support.
*                                   Updated r_flash_control() FLASH_CMD_CONFIG_CLOCK; Added FLASH_CMD_BANK_TOGGLE,
*                                       FLASH_CMD_BANK_GET.
*                                   Added is_cf_addr(), is_cf_overflow(), and get_cf_addr_info().
*              : 31.10.2017 1.20    Added function r_flash_close().
*                                   Added error check FLASH_ERR_ALREADY_OPEN to r_flash_open().
*              : 13.03.2017 1.30    Fixed bug where FLASH_CMD_CONFIG_CLOCK did not run during Open().
*              : 06.09.2018 1.40    Modified r_flash_control() so FLASH_CMD_ACCESSWINDOW_SET will accept end address
*                                     of FLASH_CF_BLOCK_END.
*              : 25.10.2018 1.50    Added NON_CACHED commmand support to r_flash_control().
*              : 18.12.2018 1.60    Modified set_blankcheck_params() parameter checking to return error when get
*                                     code flash address and blankcheck not supported in code flash.
*              : 19.04.2019 4.00    Added support for GNUC and ICCRX.
*                                   Removed support for flash type 2.
*              : 19.07.2019 4.20    Modified get_cf_addr_info().
*                                   Added volatile to g_current_parameters.
*              : 09.09.2019 4.30    Fixed error processing for r_flash_erase(), r_flash_blankcheck(), r_flash_write().
*                                   Modified the switch statement of r_flash_control() to the if statement. 
*              : 27.09.2019 4.40    Added NULL check for "blank_check_result" in r_flash_blankcheck().
*              : 18.11,2019 4.50    Modified to use BSP API functions to enable/disable interrupt requests.
*                                   Modified where the error return code was not being checked when calling
*                                     FLASH_CMD_ROM_CACHE_ENABLE in set_non_cached_regs(). 
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
#if (FLASH_TYPE == 1)
#define ACCESS_BAD_ADDR_MASK    (0x000003FF)    // low order 10 bits ignored
#else
#define ACCESS_BAD_ADDR_MASK    (0x00001FFF)    // low order 13 bits ignored
#endif

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
#define FLASH_RETURN_IF_PCFG_NULL   if ((pcfg == NULL) || (pcfg == FIT_NO_PTR))     \
                                    {                                               \
                                        return FLASH_ERR_NULL_PTR;                  \
                                    }

#define FLASH_RETURN_IF_ROM_LT_32K  if (MCU_ROM_SIZE_BYTES < 32768)                 \
                                    {                                               \
                                        return FLASH_ERR_FAILURE;                   \
                                    }

                                    // See FLASH_SAS_xxx #defines
#define FLASH_RETURN_IF_BAD_SAS     if ((*pSwapInfo == 1) || (*pSwapInfo > 4))      \
                                    {                                               \
                                        return FLASH_ERR_PARAM;                     \
                                    }
#else
#define FLASH_RETURN_IF_PCFG_NULL   // parameter checking disabled
#define FLASH_RETURN_IF_ROM_LT_32K  // parameter checking disabled
#define FLASH_RETURN_IF_BAD_SAS     // parameter checking disabled
#endif


#if (((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1)) || FLASH_CFG_DATA_FLASH_BGO)
#define FLASH_RETURN_IF_BGO_AND_NO_CALLBACK     if ((flash_ready_isr_handler == FIT_NO_FUNC) || (flash_ready_isr_handler == NULL))  \
                                                {                                                                                   \
                                                    return FLASH_ERR_FAILURE;                                                       \
                                                }
#else
#define FLASH_RETURN_IF_BGO_AND_NO_CALLBACK     // in blocking mode (non-BGO)
#endif

#define FLASH_NON_CACHED_MASK   (FLASH_NON_CACHED_MASK_IF | FLASH_NON_CACHED_MASK_OA | FLASH_NON_CACHED_MASK_DM)


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct _flash_addr_info
{
    flash_block_address_t   size_boundary;
    flash_block_address_t   low_addr;
    uint32_t                block_size;
    bool                    on_block_boundary;
} flash_addr_info_t;


R_BSP_PRAGMA_UNPACK

typedef union
{
    unsigned long LONG;
    R_BSP_ATTRIB_STRUCT_BIT_ORDER_LEFT_6(
        unsigned long :11,
        unsigned long NCSZ:17,
        unsigned long NC3E:1,
        unsigned long NC2E:1,
        unsigned long NC1E:1,
        unsigned long :1
    ) BIT;
} ncrc_reg_t;

typedef struct _flash_non_cached_regs
{
    uint32_t    ncrg;
    ncrc_reg_t  ncrc;
} flash_non_cached_regs_t;

R_BSP_PRAGMA_PACKOPTION


/***********************************************************************************************************************
External functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
void (*flash_ready_isr_handler)(void *);        // Function pointer for Flash Ready ISR
void (*flash_error_isr_handler)(void *);        // Function pointer for Flash Error ISR
flash_int_cb_args_t g_flash_int_ready_cb_args;  // Callback argument structure for flash READY interrupt
flash_int_cb_args_t g_flash_int_error_cb_args;  // Callback argument structure for flash ERROR interrupt

/*Structure that holds the parameters for current operations*/
volatile current_param_t g_current_parameters = {
                                                  0,     /* Source Address */
                                                  0,     /* Destination Address */
                                                  0,     /* Total Count */
                                                  0,     /* Current Count */
                                                  FLASH_CUR_INVALID, /* Current Operation */
                                                  0,     /* Minimum Program Size */
                                                  0,     /* Wait Count for current operation */
                                                  false, /* DF BGO Disabled */
                                                  false, /* CF BGO Disabled */
                                                 };

static bool g_driver_opened=false;
static flash_err_t flash_interrupt_config(bool state, void *pcfg);
static flash_err_t get_erase_flash_type(flash_block_address_t block_start_address, uint32_t num_blocks, flash_type_t *type);
static bool is_cf_addr(uint32_t addr);
#if (FLASH_CFG_PARAM_CHECKING_ENABLE && FLASH_CFG_CODE_FLASH_ENABLE)
static bool is_cf_overflow(uint32_t addr, uint32_t num_bytes);
#if FLASH_ERASE_CF_ASCENDING_BLOCK_NUMS
static void get_cf_addr_info(uint32_t addr, flash_addr_info_t *info);
#endif
#endif
static flash_err_t set_erase_params(flash_block_address_t block_start_address, uint32_t num_blocks, flash_type_t flash_type);
#ifndef FLASH_NO_BLANK_CHECK
static flash_err_t set_blankcheck_params(uint32_t address, uint32_t num_bytes, flash_type_t flash_type);
#endif
static flash_err_t get_bc_pgm_flash_type(uint32_t address, uint32_t num_bytes, flash_type_t *flash_type);
static flash_err_t set_write_params(uint32_t address, uint32_t num_bytes, flash_type_t flash_type);
#if (FLASH_HAS_NON_CACHED_RANGES && FLASH_CFG_CODE_FLASH_ENABLE)
static flash_err_t set_non_cached_regs(flash_non_cached_t *p_cfg, flash_non_cached_regs_t *p_regs);
#endif

/***********************************************************************************************************************
* Function Name: r_flash_open
* Description  : Function will initialize the flash peripheral.
* Arguments    : void
* Return Value : FLASH_SUCCESS -
*                    API initialized successfully.
*                FLASH_ERR_BUSY -
*                    API has already been initialized and another operation is ongoing.
*                FLASH_ERR_ALREADY_OPEN
*                    Open() has been called twice without an intermediate Close().
*                FLASH_ERR_FAILURE
*                    Initialization failed.
***********************************************************************************************************************/
flash_err_t r_flash_open(void)
{
#ifdef FLASH_HAS_FCU
    flash_err_t err;
#endif


#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((MCU_CFG_FCLK_HZ > FLASH_FREQ_HI) || (MCU_CFG_FCLK_HZ < FLASH_FREQ_LO))
    {
        return FLASH_ERR_FREQUENCY;
    }
#endif
    /* Lock driver to initialize */
    if (FLASH_SUCCESS != flash_lock_state(FLASH_INITIALIZATION))
    {
        return FLASH_ERR_BUSY;      // should never happen
    }

    if (g_driver_opened == true)
    {
        flash_release_state();
        return FLASH_ERR_ALREADY_OPEN;
    }

    /* Initialize the FCU */
#ifdef FLASH_HAS_FCU
    err = flash_init_fcu();
    if (FLASH_SUCCESS != err)
    {
        return err;
    }
#endif

    /* Initialize global variables */
    g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_INITIALIZED;
    g_current_parameters.bgo_enabled_cf = FLASH_CFG_CODE_FLASH_BGO;
    g_current_parameters.bgo_enabled_df = FLASH_CFG_DATA_FLASH_BGO;

    /* Disable Interrupts. Enabled when callback set with Control() FLASH_CMD_SET_BGO_CALLBACK. */
    flash_interrupt_config(false, NULL);

#if ((FLASH_TYPE == 1) && !defined(FLASH_NO_DATA_FLASH))
    R_DF_Enable_DataFlashAccess();
#endif

    /* Unlock driver */
    g_driver_opened = true;
    flash_release_state();

    return FLASH_SUCCESS;
}


/***********************************************************************************************************************
 * Function Name: flash_interrupt_config
 * Description  : Function to enable of disable flash interrupts.
 * Arguments    : state -
 *                    true:  Enable Interrupts (BGO mode)
 *                    false: Disable interrupts (non-BGO mode)
 *               *pCallback -
 *                    Callback function called from ISR after operation completes (unused if state = false).
 * Return Value : FLASH_SUCCESS -
 *                    Interrupts configured successfully.
 *                FLASH_ERR_NULL_PTR -
 *                    Invalid callback function pointer passed when enabling interrupts
 ***********************************************************************************************************************/
flash_err_t flash_interrupt_config(bool state, void *pcfg)
{
    flash_interrupt_config_t *int_cfg = pcfg;

    if (true == state)
    {
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
        if ((int_cfg->int_priority < 1) || (int_cfg->int_priority > 15))
        {
            return FLASH_ERR_PARAM;
        }

        if ((NULL == int_cfg->pcallback) || (FIT_NO_FUNC == int_cfg->pcallback))
        {
            return FLASH_ERR_NULL_PTR;
        }
#endif
        /* Assign the callback if not NULL*/
        flash_ready_isr_handler = int_cfg->pcallback;
        flash_error_isr_handler = int_cfg->pcallback;

        /* Enable interrupts in FCU */
        FLASH_FCU_INT_ENABLE;

        /* Enable interrupts in ICU */
        IR(FCU,FRDYI)  = 0;                             // Clear Flash Ready Interrupt Request
        IPR(FCU,FRDYI) = int_cfg->int_priority;         // Set Flash Ready Interrupt Priority
        R_BSP_InterruptRequestEnable(VECT(FCU,FRDYI));  // Enable Flash Ready Interrupt

#ifdef FLASH_HAS_ERR_ISR
        IR(FCU,FIFERR)  = 0;                            // Clear Flash Error Interrupt Request
        IPR(FCU,FIFERR) = int_cfg->int_priority;        // Set Flash Error Interrupt Priority
        R_BSP_InterruptRequestEnable(VECT(FCU,FIFERR)); // Enable Flash Error Interrupt
#endif
    }

    else if (false == state)
    {
        /* Disable interrupts in FCU */
        FLASH_FCU_INT_DISABLE;

        /* Disable interrupts in MCU */
        IR(FCU,FRDYI) = 0;                          // Clear any pending Flash Ready Interrupt Request
#ifdef FLASH_HAS_ERR_ISR
        IR(FCU,FIFERR) = 0;                         // Clear any pending Flash Error Interrupt Request
#endif
    }
    else
    {
        return FLASH_ERR_PARAM;
    }

    return FLASH_SUCCESS;
}


/***********************************************************************************************************************
* Function Name: r_flash_close
* Description  : Function disables flash interrupts and marks driver as closed.
* Arguments    : void
* Return Value : FLASH_SUCCESS -
*                    API closed successfully.
*                FLASH_ERR_BUSY -
*                    Another operation is ongoing.
***********************************************************************************************************************/
flash_err_t r_flash_close(void)
{

    /* Lock driver */
    if (FLASH_SUCCESS != flash_lock_state(FLASH_UNINITIALIZED))
    {
        return FLASH_ERR_BUSY;
    }

    /* Disable interrupts */
    flash_interrupt_config(false, NULL);

    /* Show driver as closed and release hold on lock.
     * Do not use flash_release_state() because we do not want to set state to FLASH_READY.
     */
    g_driver_opened = false;
    flash_softwareUnlock(&g_flash_lock);

    return FLASH_SUCCESS;
}


/***********************************************************************************************************************
* Function Name: set_non_cached_regs
* Description  : This function sets the NCRGx and NCRCx registers to configure non-cached ROM ranges.
* Arguments    : p_cfg
*                    Pointer to non-cached range info.
*                p_regs -
*                    Pointer to registers to write to.
* Return Value : FLASH_SUCCESS -
*                    Range set successfully.
*                FLASH_ERR_PARAM -
*                    One of the arguments within the p_cfg structure was invalid.
***********************************************************************************************************************/
#if (FLASH_HAS_NON_CACHED_RANGES && FLASH_CFG_CODE_FLASH_ENABLE)
static flash_err_t set_non_cached_regs(flash_non_cached_t *p_cfg, flash_non_cached_regs_t *p_regs)
{
    flash_err_t err = FLASH_SUCCESS;
    uint32_t    size_reg_value;
    uint8_t     caching_was_enabled;

#if FLASH_CFG_PARAM_CHECKING_ENABLE
    if (((p_cfg->start_addr & 0xF) != 0)                        // address not on 16-byte boundary
     || ((p_cfg->start_addr & 0xFFC00000) != 0xFFC00000)        // address not ROM address
     || (p_cfg->size < FLASH_NON_CACHED_16_BYTES)               // size less than minimum 16 bytes
     || (p_cfg->size > MCU_ROM_SIZE_BYTES)                      // size greater than ROM size
     || ((-p_cfg->size & p_cfg->size) != p_cfg->size)           // size not power of 2 (check for more than 1 bit set)
     || ((p_cfg->start_addr + p_cfg->size) <= (uint32_t)FLASH_CF_BLOCK_INVALID) // area goes past end of ROM
     || ((p_cfg->type_mask & ~FLASH_NON_CACHED_MASK) != 0))     // unknown flag set
    {
        return FLASH_ERR_PARAM;
    }
#endif

    /* save current cache enable state and disable caching */
    caching_was_enabled = FLASH.ROMCE.BIT.ROMCEN;
    FLASH.ROMCE.BIT.ROMCEN = 0;

    /* set non-cached start address */
    p_regs->ncrg = p_cfg->start_addr;

    /* set non-cached size */
    size_reg_value = (p_cfg->size - 1) >> 4;
    p_regs->ncrc.BIT.NCSZ = (uint16_t) size_reg_value;

    /* clear previous type flags and set new ones */
    p_regs->ncrc.LONG &= ~FLASH_NON_CACHED_MASK;
    p_regs->ncrc.LONG |= p_cfg->type_mask;

    /* re-enable caching if that was its previous state */
    if (caching_was_enabled)
    {
        err = R_FLASH_Control(FLASH_CMD_ROM_CACHE_ENABLE, NULL);
    }

    return err;
}
#endif // FLASH_HAS_NON_CACHED_RANGES


/* FUNCTIONS WHICH MUST BE RUN FROM RAM FOLLOW */
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#define FLASH_PE_MODE_SECTION    R_BSP_ATTRIB_SECTION_CHANGE(P, FRAM)
#define FLASH_SECTION_CHANGE_END R_BSP_ATTRIB_SECTION_CHANGE_END
#else
#define FLASH_PE_MODE_SECTION
#define FLASH_SECTION_CHANGE_END
#endif


/***********************************************************************************************************************
* Function Name: r_flash_erase
* Description  : This function erases the specified Code or Data Flash blocks.
* Arguments    : uint32_t block_start_address -
*                    Start address of the first block.
*                uint32_t num_blocks -
*                    Number of blocks to erase.
*                    NOTE: See r_flash_rx_if.h if erasing occurs by FLASH_ERASE_CF_ASCENDING_ADDRESSES
*                    or by FLASH_ERASE_CF_ASCENDING_BLOCK_NUMS.
* Return Value : FLASH_SUCCESS -
*                    Erase completed successfully; successfully initialized in case of BGO mode.
*                FLASH_ERR_BLOCKS -
*                    Number of blocks exceeds max range or is 0
*                FLASH_ERR_ADDRESS -
*                    Start address is an invalid Code/Data Flash block start address
*                FLASH_ERR_BUSY -
*                    Flash peripheral is busy with another operation
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t r_flash_erase(flash_block_address_t block_start_address, uint32_t num_blocks)
{
    flash_err_t     err;
    flash_type_t    flash_type;


    /* Lock flash driver and set state to ERASING */
    if (FLASH_SUCCESS != flash_lock_state(FLASH_ERASING))
    {
        return FLASH_ERR_BUSY;
    }

    /* Get flash type (DF or CF) based upon start address */
    err = get_erase_flash_type(block_start_address, num_blocks, &flash_type);
    if (err != FLASH_SUCCESS)
    {
        flash_release_state();      // unlock driver
        return err;
    }

    /* Setup erase parameters */
    err = set_erase_params(block_start_address, num_blocks, flash_type);
    if (err != FLASH_SUCCESS)
    {
        flash_release_state();      // unlock driver
        return err;
    }

    /* Enter program/erase mode */
    err = flash_pe_mode_enter(flash_type);
    if (FLASH_SUCCESS != err)
    {
        flash_release_state();      // unlock driver
        return err;
    }

    /* Erase the Blocks */
    err = flash_erase((uint32_t)block_start_address, num_blocks);
    if (FLASH_SUCCESS != err)
    {
#if (FLASH_TYPE == 1)
        flash_reset();
        flash_pe_mode_exit();
        flash_release_state();      // unlock driver
#else
        flash_reset();
        flash_release_state();      // unlock driver
#endif
        return err;
    }

    /* If in blocking mode, operation complete. Exit PE mode and return. */
    if ((g_current_parameters.current_operation == FLASH_CUR_CF_ERASE)
     || (g_current_parameters.current_operation == FLASH_CUR_DF_ERASE))
    {
        err = flash_pe_mode_exit();
        if (FLASH_SUCCESS != err)
        {
            flash_reset();
        }

        flash_release_state();      // unlock driver
    }

    return err;     // If in BGO mode, interrupt does PE mode exit and driver unlock
}


/***********************************************************************************************************************
* Function Name: get_erase_flash_type
* Description  : This function performs parameter checking (if enabled) and loads the flash type (CF or DF) based
*                upon the start address provided.
*                NOTE: Addresses are not compared against lockbit or access window violations.
* Arguments    : block_start_address -
*                    Start address for erase or blankcheck
*                num_blocks -
*                    Number of blocks to erase or blankcheck
*                flash_type -
*                    Pointer to set to FLASH_TYPE_DATA_FLASH or FLASH_TYPE_CODE_FLASH
* Return Value : FLASH_SUCCESS -
*                    Valid start address and number of blocks; flash type set.
*                FLASH_ERR_BLOCKS -
*                    Number of blocks exceeds max range, is 0, or would exceed legal address range
*                FLASH_ERR_ADRRESS -
*                    Start address is an invalid Code/Data Flash block start address
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t get_erase_flash_type(flash_block_address_t block_start_address, uint32_t num_blocks, flash_type_t *flash_type)
{
    *flash_type = FLASH_TYPE_INVALID;


    /* CHECK FOR VALID DATA FLASH ADDRESS RANGE */

#ifndef FLASH_NO_DATA_FLASH
    if ((block_start_address >= FLASH_DF_BLOCK_0) && (block_start_address < FLASH_DF_BLOCK_INVALID ))
    {
        *flash_type = FLASH_TYPE_DATA_FLASH;

#if FLASH_CFG_PARAM_CHECKING_ENABLE
        /* Address within DF range. See if on block boundary. */
        if (block_start_address & (FLASH_DF_BLOCK_SIZE - 1))
        {
            return FLASH_ERR_ADDRESS;
        }

        /* Good start address. See if legal number of blocks */
        if ((num_blocks > FLASH_NUM_BLOCKS_DF) || (num_blocks <= 0))
        {
            return FLASH_ERR_BLOCKS;
        }

        /* Legal number. See if valid (does not go pass address boundary */
        if ((block_start_address + (num_blocks*FLASH_DF_BLOCK_SIZE)-1) >= (uint32_t)FLASH_DF_BLOCK_INVALID)
        {
            return FLASH_ERR_BLOCKS;
        }
#endif // FLASH_CFG_PARAM_CHECKING_ENABLE
    }
#endif // FLASH_NO_DATA_FLASH


    /* CHECK FOR VALID CODE FLASH ADDRESS RANGE */

    if (is_cf_addr(block_start_address))
    {
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
        *flash_type = FLASH_TYPE_CODE_FLASH;

#if FLASH_CFG_PARAM_CHECKING_ENABLE
        /* See if legal number of blocks */
        if ((num_blocks > FLASH_NUM_BLOCKS_CF) || (num_blocks <= 0))
        {
            return FLASH_ERR_BLOCKS;
        }

#if (defined(MCU_RX113) && (MCU_ROM_SIZE_BYTES > 0x40000L))
        /* For parts with CF > 256K, erase and blankcheck cannot cross 256k boundary */
        if ((block_start_address < (uint32_t)FLASH_CF_256KBOUNDARY)
         && ((block_start_address + (num_blocks * FLASH_CF_BLOCK_SIZE) - 1) > (uint32_t)FLASH_CF_256KBOUNDARY))
        {
                return FLASH_ERR_ADDRESS;
        }
#endif
#if FLASH_ERASE_CF_ASCENDING_ADDRESSES
        /* Legal number of blocks. See if valid (does not go pass address boundary */
        if ((block_start_address + (num_blocks*FLASH_CF_BLOCK_SIZE)-1) < (uint32_t)FLASH_CF_BLOCK_INVALID) // < for overflow/wrap-around
        {
            return FLASH_ERR_BLOCKS;
        }

        /* See if start address is on block boundary */
        if (block_start_address & (FLASH_CF_BLOCK_SIZE - 1))
        {
            return FLASH_ERR_ADDRESS;
        }
#endif
#if FLASH_ERASE_CF_ASCENDING_BLOCK_NUMS

        return check_cf_block_total(block_start_address, num_blocks);
#endif
#endif  // param checking
#else
        return FLASH_ERR_FAILURE;
#endif
    }


    /* INVALID START ADDRESS (NOT IN DF OR CF RANGE) */
#if FLASH_CFG_PARAM_CHECKING_ENABLE
    if (*flash_type == FLASH_TYPE_INVALID)
    {
        return FLASH_ERR_ADDRESS;
    }
#endif

    return FLASH_SUCCESS;
}


/***********************************************************************************************************************
* Function Name: is_cf_addr
* Description  : This function determines if the address passed in is a valid code flash address.
* Arguments    : addr -
*                    Address to check
* Return Value : true
*                    Address is a valid code flash address
*                false
*                    Address is not a code flash address
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
static bool is_cf_addr(uint32_t addr)
{
    bool    result = false;

#ifdef FLASH_IN_DUAL_BANK_MODE
    if (((addr >= (uint32_t)FLASH_CF_HI_BANK_LO_ADDR) && (addr <= (uint32_t)FLASH_CF_HI_BANK_HI_ADDR))
     || ((addr >= (uint32_t)FLASH_CF_LO_BANK_LO_ADDR) && (addr <= (uint32_t)FLASH_CF_LO_BANK_HI_ADDR)))
    {
        result = true;
    }
#else
    if ((addr > (uint32_t)FLASH_CF_BLOCK_INVALID) && (addr <= (uint32_t)FLASH_CF_BLOCK_END))
    {
        result = true;
    }
#endif

    return result;
}


#if (FLASH_CFG_PARAM_CHECKING_ENABLE && FLASH_CFG_CODE_FLASH_ENABLE)
/***********************************************************************************************************************
* Function Name: is_cf_overflow
* Description  : This function determines if number of bytes (minus 1) from the address is a valid code flash address.
*                NOTE: Address passed in is already known to be a good code flash address!
* Arguments    : addr -
*                    Address to check
*                num_bytes -
*                    Number of bytes added to address
* Return Value : true
*                    Resulting address out of legal code flash range
*                false
*                    Resulting address is in legal code flash range
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
static bool is_cf_overflow(uint32_t addr, uint32_t num_bytes)
{
    bool        result = false;
    uint32_t    result_addr = addr + num_bytes - 1;

    if (num_bytes > MCU_ROM_REGION_SIZE_BYTES)                      // num_bytes too large
    {
        result = true;
    }
    else
    {
#ifdef FLASH_IN_DUAL_BANK_MODE
        if (addr >= (uint32_t)FLASH_CF_HI_BANK_LO_ADDR)             // addr in hi bank
        {
            if (result_addr < (uint32_t)FLASH_CF_HI_BANK_LO_ADDR)   // address wrapped around
            {
                result = true;
            }
        }
        else // addr in low bank
        {
            if ((result_addr > (uint32_t)FLASH_CF_LO_BANK_HI_ADDR)
             || (result_addr < (uint32_t)FLASH_CF_LO_BANK_LO_ADDR))
            {
                result = true;
            }
        }
#else
        if (result_addr <= (uint32_t)FLASH_CF_BLOCK_INVALID)        // overflow/wrap-around
        {
            result = true;
        }
#endif
    }

    return result;
}


/***********************************************************************************************************************
* Function Name: get_cf_addr_info
* Description  : This function loads a structure with characteristics of the code flash area the address is located in.
*                NOTE: The address passed in is already known to be a good code flash address!
* Arguments    : addr -
*                    Address to check
*                info -
*                    Pointer to structure to load
*                    info.size_boundary     <- lowest small block size address for code flash/bank
*                    info.low_addr          <- lowest valid address for code flash/bank
*                    info.block.size        <- size of block passed address is located in
*                    info.on_block_boundary <- true or false if the passed address is on a block boundary
* Return Value : None
***********************************************************************************************************************/
#if FLASH_ERASE_CF_ASCENDING_BLOCK_NUMS
FLASH_PE_MODE_SECTION
static void get_cf_addr_info(uint32_t addr, flash_addr_info_t *info)
{

#ifdef FLASH_IN_DUAL_BANK_MODE
    if (addr >= (uint32_t)FLASH_CF_HI_BANK_LO_ADDR)
    {
        info->size_boundary = FLASH_CF_BLOCK_7;
        info->low_addr = FLASH_CF_HI_BANK_LO_ADDR;
    }
    else
    {
        info->size_boundary = FLASH_CF_LO_BANK_SMALL_BLOCK_ADDR;
        info->low_addr = FLASH_CF_LO_BANK_LO_ADDR;
    }
#else
    info->size_boundary = FLASH_CF_BLOCK_7;
    info->low_addr = (flash_block_address_t)((uint32_t)FLASH_CF_BLOCK_INVALID + 1);
#endif

    info->block_size = (addr >= info->size_boundary) ? FLASH_CF_SMALL_BLOCK_SIZE : FLASH_CF_MEDIUM_BLOCK_SIZE;
    info->on_block_boundary = ((addr & (info->block_size - 1)) != 0) ? false : true;

    return;
}
#endif

/***********************************************************************************************************************
* Function Name: check_cf_block_total
* Description  : This function checks to see if the start address plus the number of blocks
*                remain in valid code flash address range, and if the start address is on a
*                block boundary.
* Arguments    : block_start_address -
*                    Start address for erase or lock bit
*                num_blocks -
*                    Number of blocks to erase or lock
* Return Value : FLASH_SUCCESS -
*                    Valid start address and number of blocks
*                FLASH_ERR_BLOCKS -
*                    Number of blocks would exceed legal address range
*                FLASH_ERR_ADRRESS -
*                    Start address is not on block boundary
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t check_cf_block_total(flash_block_address_t block_start_address, uint32_t num_blocks)
{
#if FLASH_ERASE_CF_ASCENDING_BLOCK_NUMS
    uint32_t            tmp;
    flash_addr_info_t   info;


    /* Get info on address */
    get_cf_addr_info((uint32_t)block_start_address, &info);

    /* See if start address is on block boundary */
    if (info.on_block_boundary == false)
    {
        return FLASH_ERR_ADDRESS;
    }


    /* See if valid (does not go pass address boundary */

    if (block_start_address >= info.size_boundary)
    {
        /* set tmp = available number of small blocks to erase */
        tmp = ((block_start_address - info.size_boundary) / FLASH_CF_SMALL_BLOCK_SIZE) + 1;
        /* set num_blocks = number of large blocks to erase */
        num_blocks -= (tmp < num_blocks) ? tmp : num_blocks;
        /* adjusted start address for number of large blocks to erase */
        block_start_address = (flash_block_address_t)(info.size_boundary - FLASH_CF_MEDIUM_BLOCK_SIZE);
    }

    /* set num_blocks = large blocks to erase below start address */
    num_blocks--;
    /* determine if num_blocks goes pass address limit */
    if ((block_start_address - (num_blocks*FLASH_CF_MEDIUM_BLOCK_SIZE)) < info.low_addr)
    {
        return FLASH_ERR_BLOCKS;
    }
#endif  // ascending block numbers
    return FLASH_SUCCESS;
}
#endif // (FLASH_CFG_PARAM_CHECKING_ENABLE && FLASH_CFG_CODE_FLASH_ENABLE)


/***********************************************************************************************************************
* Function Name: set_erase_params
* Description  : This function loads the g_current_parameters structure used by MCU-specific erase function.
* Arguments    : block_start_address -
*                    Start address for erase
*                num_blocks -
*                    Number of blocks to erase
*                type -
*                    FLASH_TYPE_DATA_FLASH or FLASH_TYPE_CODE_FLASH
* Return Value : FLASH_SUCCESS -
*                    Parameters set
*                FLASH_ERR_FAILURE -
*                    Callback function not set and interrupts are configured
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t set_erase_params(flash_block_address_t block_start_address, uint32_t num_blocks, flash_type_t flash_type)
{

    FLASH_RETURN_IF_BGO_AND_NO_CALLBACK;


    /* SET START ADDRESS AND NUMBER BLOCKS */
    g_current_parameters.dest_addr = block_start_address;
    g_current_parameters.total_count = num_blocks;

    if (flash_type ==  FLASH_TYPE_DATA_FLASH)
    {
#ifndef FLASH_NO_DATA_FLASH
        /* SET OPERATION INTERRUPT MODE AND TIMEOUT (TO DETECT FLASH DEGRADATION) */

        if (g_current_parameters.bgo_enabled_df == true)
        {
            g_current_parameters.current_operation = FLASH_CUR_DF_BGO_ERASE;
        }
        else
        {
            g_current_parameters.current_operation = FLASH_CUR_DF_ERASE;
        }

        g_current_parameters.wait_cnt = WAIT_MAX_ERASE_DF;
#endif
    }
    else // CODE FLASH
    {
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
        /* SET OPERATION INTERRUPT MODE AND TIMEOUT (TO DETECT FLASH DEGRADATION) */

        if (g_current_parameters.bgo_enabled_cf == true)
        {
            g_current_parameters.current_operation = FLASH_CUR_CF_BGO_ERASE;
        }
        else
        {
            g_current_parameters.current_operation = FLASH_CUR_CF_ERASE;
        }

#if FLASH_ERASE_CF_ASCENDING_ADDRESSES
        g_current_parameters.wait_cnt = WAIT_MAX_ERASE_CF_1K;
#endif
#if FLASH_ERASE_CF_ASCENDING_BLOCK_NUMS
        if (block_start_address >= FLASH_CF_BLOCK_7)
        {
            g_current_parameters.wait_cnt = WAIT_MAX_ERASE_CF_8K;
        }
        else
        {
            g_current_parameters.wait_cnt = WAIT_MAX_ERASE_CF_32K;
        }
#endif
#endif  // code flash enabled
    }

    return FLASH_SUCCESS;
}


#ifndef FLASH_NO_BLANK_CHECK

/***********************************************************************************************************************
* Function Name: r_flash_blankcheck
* Description  : This function executes RX1xx and RX23x specific functionality to blank check the specified Flash area.
*                The function will incrementally check the area from the start address onward.
*                The minimum number of bytes is 4 for CF and 1 for DF.
*                The max is 256K for CF, 8192 for DF.
*                For parts that may support > 256K CF, then the address range provided may not span a 256K boundary.
*                For example: with a 512K part one could not specify a range of 255K - 257K even though that is
*                a 2K range. That would require two seperate BlankCheck calls. One for 255-256, one for 256-257
* Arguments    : uint32_t address -
*                    Start address to perform blank check.
*                uint32_t num_bytes -
*                    Number of bytes to perform blank check on.
*                flash_res_t *result
*                    Set to FLASH_RES_BLANK or FLASH_RES_NOT_BLANK when not in BGO mode.
* Return Value : FLASH_SUCCESS -
*                    Operation completed successfully
*                FLASH_ERR_BYTES -
*                    Number of bytes is 0 or will exceed end of flash area.
*                FLASH_ERR_ADRRESS -
*                    Start address is invalid.
*                FLASH_ERR_BUSY
*                    Flash peripheral is busy with another operation or not initialized
*                FLASH_ERR_FAILURE -
*                    Operation failed for some other reason
*                FLASH_ERR_NULL_PTR -
*                    Blank check result pointer is NULL.
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t r_flash_blankcheck(uint32_t address, uint32_t num_bytes, flash_res_t *result)
{
    flash_err_t     err;
    flash_type_t    flash_type;


    /* Lock flash driver and set state to BLANKCHECK */
    if (FLASH_SUCCESS != flash_lock_state(FLASH_BLANKCHECK))
    {
        return FLASH_ERR_BUSY;
    }

    /* Get flash type (DF or CF) based upon start address */
    err = get_bc_pgm_flash_type(address, num_bytes, &flash_type);
    if (err != FLASH_SUCCESS)
    {
        flash_release_state();      // unlock driver
        return err;
    }

    /* Setup blankcheck parameters */
    err = set_blankcheck_params(address, num_bytes, flash_type);
    if (err != FLASH_SUCCESS)
    {
        flash_release_state();      // unlock driver
        return err;
    }

    /* Parameter check (blankcheck result parameter) */
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if (FLASH_TYPE_DATA_FLASH == flash_type)    // DATA FLASH
    {
        if (g_current_parameters.bgo_enabled_df == false)
        {
            if ((NULL == result) || (FIT_NO_PTR == result))
            {
                flash_release_state();      // unlock driver
                return FLASH_ERR_NULL_PTR;
            }
        }
    }
    else                                        // CODE FLASH
    {
        if (g_current_parameters.bgo_enabled_cf == false)
        {
            if ((NULL == result) || (FIT_NO_PTR == result))
            {
                flash_release_state();      // unlock driver
                return FLASH_ERR_NULL_PTR;
            }
        }
    }
#endif

    /* Enter program/erase mode */
    err = flash_pe_mode_enter(flash_type);
    if (FLASH_SUCCESS != err)
    {
        flash_reset();
        flash_release_state();      // unlock driver
        return err;
    }

    /* Start the blankcheck operation */
    err = flash_blankcheck(address, num_bytes, result);
    if (FLASH_SUCCESS != err)
    {
#if (FLASH_TYPE == 1)
        flash_reset();
        flash_pe_mode_exit();
        flash_release_state();      // unlock driver
#else
        flash_reset();
        flash_release_state();      // unlock driver
#endif
        return err;
    }

    /* If in non-BGO (blocking) mode, then blankcheck is complete. Exit PE mode before returning */
    if ((g_current_parameters.current_operation == FLASH_CUR_CF_BLANKCHECK)
     || (g_current_parameters.current_operation == FLASH_CUR_DF_BLANKCHECK))
    {
        err = flash_pe_mode_exit();
        if (FLASH_SUCCESS != err)
        {
            flash_reset();
        }

        flash_release_state();      // unlock driver
    }

    return err;
}


/***********************************************************************************************************************
* Function Name: set_blankcheck_params
* Description  : This function loads the g_current_parameters structure used by MCU-specific blank check function.
* Arguments    : address -
*                    Start address for blank check
*                num_bytes -
*                    Number of bytes to blank check
*                type -
*                    FLASH_TYPE_DATA_FLASH or FLASH_TYPE_CODE_FLASH
* Return Value : FLASH_SUCCESS -
*                    Parameters set
*                FLASH_ERR_FAILURE -
*                    Callback function not set and interrupts are configured
*                FLASH_ERR_ADDRESS -
*                    Address for code flash and blankcheck not supported on this device.
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t set_blankcheck_params(uint32_t address, uint32_t num_bytes, flash_type_t flash_type)
{
    FLASH_RETURN_IF_BGO_AND_NO_CALLBACK;
    flash_err_t err=FLASH_SUCCESS;


    /* SET START ADDRESS AND NUMBER BYTES */
    g_current_parameters.dest_addr = address;
    g_current_parameters.total_count = num_bytes;

    if (flash_type ==  FLASH_TYPE_DATA_FLASH)
    {
#ifndef FLASH_NO_DATA_FLASH
        /* SET OPERATION INTERRUPT MODE AND TIMEOUT (TO DETECT FLASH DEGRADATION) */

        if (g_current_parameters.bgo_enabled_df == true)
        {
            g_current_parameters.current_operation = FLASH_CUR_DF_BGO_BLANKCHECK;
        }
        else
        {
            g_current_parameters.current_operation = FLASH_CUR_DF_BLANKCHECK;
        }

        /* WAIT constant may be for 1, 2, or 4 bytes at a time */
        g_current_parameters.wait_cnt = (WAIT_MAX_BLANK_CHECK * (num_bytes / FLASH_DF_MIN_PGM_SIZE));
#endif
    }
    else // CODE FLASH
    {
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#if (FLASH_HAS_CF_BLANK_CHECK == 0)
        err = FLASH_ERR_ADDRESS;
#else
        /* SET OPERATION INTERRUPT MODE AND TIMEOUT (TO DETECT FLASH DEGRADATION) */

        if (g_current_parameters.bgo_enabled_cf == true)
        {
            g_current_parameters.current_operation = FLASH_CUR_CF_BGO_BLANKCHECK;
        }
        else
        {
            g_current_parameters.current_operation = FLASH_CUR_CF_BLANKCHECK;
        }

        /* CF 4-byte check takes same time as DF 1-byte check */
        g_current_parameters.wait_cnt = (WAIT_MAX_BLANK_CHECK * (num_bytes / FLASH_CF_MIN_PGM_SIZE));
#endif
#endif
    }

    return err;
}

#endif


/***********************************************************************************************************************
* Function Name: r_flash_write
* Description  : Function will write to the specified Code or Data Flash memory area.
* Arguments    : uint32_t src_address -
*                    Source buffer address.
*                uint32_t dest_address -
*                    Destination address.
*                uint32_t num_bytes
*                    Number of bytes to be written
* Return Value : FLASH_SUCCESS -
*                    Write completed successfully; successfully initialized in case of BGO mode.
*                FLASH_ERR_BYTES -
*                    Number of bytes exceeds max range or is 0 or is not a valid multiple of the minimum programming
*                    size for the specified flash
*                FLASH_ERR_ADDRESS -
*                    src/dest address is an invalid Code/Data Flash block start address or
*                    address is not aligned with the minimum programming size for the Code/Data Flash
*                    For Code Flash programming the src address HAS to be a RAM address since ROM cannot be accessed
*                    during Code Flash programming
*                FLASH_ERR_BUSY -
*                    Flash peripheral is busy with another operation or not initialized
*                FLASH_ERR_CMD_LOCKED -
*                    The FCU entered a command locked state and cannot process the operation.
*                    A RESET was performed on the FCU to recover from this state.
*                FLASH_ERR_LOCKBIT_SET
*                    Operation failed because the lockbit was set.
*                    A RESET was performed on the FCU to recover from this state.
*                FLASH_ERR_FAILURE
*                    Code Flash Write operation attempted in BGO mode. This is temporarily not supported
*                    Operation failed for some other reason; RESET was performed on the FCU to recover from this state.
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t r_flash_write(uint32_t src_address, uint32_t dest_address, uint32_t num_bytes)
{
    flash_err_t     err;
    flash_type_t    flash_type;


    /* Lock flash driver and set state to WRITING */
    if (FLASH_SUCCESS != flash_lock_state(FLASH_WRITING))
    {
        return FLASH_ERR_BUSY;
    }

    /* Get flash type (DF or CF) based upon start address */
    err = get_bc_pgm_flash_type(dest_address, num_bytes, &flash_type);
    if (err != FLASH_SUCCESS)
    {
        flash_release_state();      // unlock driver
        return err;
    }

    /* Setup write parameters */
    err = set_write_params(dest_address, num_bytes, flash_type);
    if (err != FLASH_SUCCESS)
    {
        flash_release_state();      // unlock driver
        return err;
    }

    /* Enter program/erase mode */
    err = flash_pe_mode_enter(flash_type);
    if (FLASH_SUCCESS != err)
    {
        flash_release_state();      // unlock driver
        return err;
    }


    /* Start the write operation */
    err = flash_write(src_address, dest_address, num_bytes);
    if (FLASH_SUCCESS != err)
    {
#if (FLASH_TYPE == 1)
        flash_reset();
        flash_pe_mode_exit();
        flash_release_state();      // unlock driver
#else
        flash_reset();
        flash_release_state();      // unlock driver
#endif
        return err;
    }

    /* If in non-BGO (blocking) mode, then write is complete. Exit PE mode before returning */
    if ((g_current_parameters.current_operation == FLASH_CUR_CF_WRITE)
     || (g_current_parameters.current_operation == FLASH_CUR_DF_WRITE))
    {
        err = flash_pe_mode_exit();
        if (FLASH_SUCCESS != err)
        {
            flash_reset();
        }

        flash_release_state();      // unlock driver
    }

    return err;
}


/***********************************************************************************************************************
* Function Name: get_bc_pgm_flash_type
* Description  : This function performs parameter checking (if enabled) and loads the flash type (CF or DF) based
*                upon the start address provided.
*                NOTE: Flash type 1 supports blank checking on code flash.
* Arguments    : address -
*                    Start address for blank check or program operation
*                num_bytes -
*                    Number of bytes to blank check or program operation
*                flash_type -
*                    Pointer to set to FLASH_TYPE_DATA_FLASH or FLASH_TYPE_CODE_FLASH
* Return Value : FLASH_SUCCESS -
*                    Valid start address and number of bytes; flash type is set.
*                FLASH_ERR_BYTES -
*                    Number of bytes exceeds max range, is 0, or would exceed legal address range
*                FLASH_ERR_ADRRESS -
*                    Start address is an invalid Code/Data flash start address
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t get_bc_pgm_flash_type(uint32_t address, uint32_t num_bytes, flash_type_t *flash_type)
{

    *flash_type = FLASH_TYPE_INVALID;


    /* CHECK FOR VALID DATA FLASH ADDRESS RANGE */
#ifndef FLASH_NO_DATA_FLASH
    if ((address >= FLASH_DF_BLOCK_0) && (address < FLASH_DF_BLOCK_INVALID ))
    {
        *flash_type = FLASH_TYPE_DATA_FLASH;

#if FLASH_CFG_PARAM_CHECKING_ENABLE
        if (((address + num_bytes - 1) >= FLASH_DF_BLOCK_INVALID)   // num_bytes too large
         ||  (num_bytes > MCU_RAM_SIZE_BYTES)
         ||  (address & (FLASH_DF_MIN_PGM_SIZE-1))                  // address not on min pgm size boundary
         ||  (num_bytes & (FLASH_DF_MIN_PGM_SIZE-1))                // num_bytes not multiple of min pgm size
         ||  (num_bytes == 0))
        {
            return FLASH_ERR_BYTES;
        }
#endif
    }
#endif // no data flash


    /* CHECK FOR VALID CODE FLASH ADDRESS RANGE */

    if (is_cf_addr(address))
    {
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
        *flash_type = FLASH_TYPE_CODE_FLASH;

#if FLASH_CFG_PARAM_CHECKING_ENABLE
#if (defined(MCU_RX113) && (MCU_ROM_SIZE_BYTES > 0x40000L))
        if (g_flash_state == FLASH_BLANKCHECK)
        {
            /* For parts with CF > 256K, erase and blankcheck cannot cross 256k boundary */
            if ((address < (uint32_t)FLASH_CF_256KBOUNDARY)
             && (((address + num_bytes- 1) > (uint32_t)FLASH_CF_256KBOUNDARY) || (address + num_bytes- 1) <= (uint32_t)FLASH_CF_BLOCK_INVALID))
            {
                return FLASH_ERR_ADDRESS;
            }
        }
#endif
        if ((is_cf_overflow(address, num_bytes))            // address + num_bytes out of range
         || (address & (FLASH_CF_MIN_PGM_SIZE-1))           // address not on min pgm size boundary
         || (num_bytes & (FLASH_CF_MIN_PGM_SIZE-1))         // num_bytes not multiple of min pgm size
         || (num_bytes == 0))
        {
            return FLASH_ERR_BYTES;
        }
#endif  // param checking
#else
        return FLASH_ERR_FAILURE;   // code flash address, but code flash disabled
#endif
    }


    /* INVALID START ADDRESS (NOT IN DF OR CF RANGE) */
#if FLASH_CFG_PARAM_CHECKING_ENABLE
    if (*flash_type == FLASH_TYPE_INVALID)
    {
        return FLASH_ERR_ADDRESS;
    }
#endif

    return FLASH_SUCCESS;
}


/***********************************************************************************************************************
* Function Name: set_write_params
* Description  : This function initializes driver internal structures to perform a write operation.
* Arguments    : _address -
*                    Start address for write
*                num_bytes -
*                    Number of bytes to write
*                type -
*                    FLASH_TYPE_DATA_FLASH or FLASH_TYPE_CODE_FLASH
* Return Value : FLASH_SUCCESS -
*                    Parameters set
*                FLASH_ERR_FAILURE -
*                    Callback function not set and interrupts are configured
***********************************************************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t set_write_params(uint32_t address, uint32_t num_bytes, flash_type_t flash_type)
{

    FLASH_RETURN_IF_BGO_AND_NO_CALLBACK;


    /* SET START ADDRESS AND NUMBER BYTES */
    g_current_parameters.dest_addr = address;
    g_current_parameters.total_count = num_bytes;

    if (flash_type ==  FLASH_TYPE_DATA_FLASH)
    {
#ifndef FLASH_NO_DATA_FLASH
        /* SET OPERATION INTERRUPT MODE AND TIMEOUT (TO DETECT FLASH DEGRADATION) */

        if (g_current_parameters.bgo_enabled_df == true)
        {
            g_current_parameters.current_operation = FLASH_CUR_DF_BGO_WRITE;
        }
        else
        {
            g_current_parameters.current_operation = FLASH_CUR_DF_WRITE;
        }

        g_current_parameters.wait_cnt = (WAIT_MAX_DF_WRITE * (num_bytes / FLASH_DF_MIN_PGM_SIZE));
#ifdef FLASH_HAS_FCU
        g_current_parameters.fcu_min_write_cnt = (FLASH_DF_MIN_PGM_SIZE >> 1);
#endif
#endif
    }
    else // CODE FLASH
    {
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
        /* SET OPERATION INTERRUPT MODE AND TIMEOUT (TO DETECT FLASH DEGRADATION) */

        if (g_current_parameters.bgo_enabled_cf == true)
        {
            g_current_parameters.current_operation = FLASH_CUR_CF_BGO_WRITE;
        }
        else
        {
            g_current_parameters.current_operation = FLASH_CUR_CF_WRITE;
        }

        g_current_parameters.wait_cnt = (WAIT_MAX_ROM_WRITE * (num_bytes / FLASH_CF_MIN_PGM_SIZE));
#ifdef FLASH_HAS_FCU
        g_current_parameters.fcu_min_write_cnt = (FLASH_CF_MIN_PGM_SIZE >> 1);
#endif
#endif
    }

    return FLASH_SUCCESS;
}


/*****************************************************************************
* Function Name: r_flash_control
* Description  : This function performs special configuration and operational commands.
* Arguments    : cmd -
*                    Command to perform
*                *pcfg -
*                    Pointer to optional argument (command specific)
*
*                Command                                  Argument
*                FLASH_CMD_RESET------------------------  NULL
*                FLASH_CMD_SET_BGO_CALLBACK-------------  flash_interrupt_config_t *
*                FLASH_CMD_STATUS_GET-------------------  NULL
*                FLASH_CMD_ROM_CACHE_ENABLE-------------  NULL
*                FLASH_CMD_ROM_CACHE_DISABLE------------  NULL
*                FLASH_CMD_ROM_CACHE_STATUS-------------  uint8_t *
*                FLASH_CMD_SET_NON_CACHED_RANGE0--------  flash_non_cached_t *
*                FLASH_CMD_SET_NON_CACHED_RANGE1--------  flash_non_cached_t *
*                FLASH_CMD_GET_NON_CACHED_RANGE0--------  flash_non_cached_t *
*                FLASH_CMD_GET_NON_CACHED_RANGE1--------  flash_non_cached_t *
*                FLASH_CMD_SWAPSTATE_GET----------------  uint8_t *
*                FLASH_CMD_SWAPSTATE_SET----------------  uint8_t *
*                FLASH_CMD_SWAPFLAG_GET-----------------  uint8_t *
*                FLASH_CMD_SWAPFLAG_TOGGLE--------------  NULL
*                FLASH_CMD_ACCESSWINDOW_GET-------------  flash_access_window_config_t *
*                FLASH_CMD_ACCESSWINDOW_SET-------------  flash_access_window_config_t *
*                FLASH_CMD_LOCKBIT_ENABLE---------------  NULL
*                FLASH_CMD_LOCKBIT_DISABLE--------------  NULL
*                FLASH_CMD_LOCKBIT_READ-----------------  flash_lockbit_config_t *
*                FLASH_CMD_LOCKBIT_WRITE----------------  flash_lockbit_config_t *
*                FLASH_CMD_CONFIG_CLOCK-----------------  uint32_t *
*                FLASH_CMD_BANK_TOGGLE------------------  NULL
*                FLASH_CMD_BANK_GET---------------------  flash_bank_t *
*
* Return Value : FLASH_SUCCESS -
*                    Operation completed successfully.
*                FLASH_ERR_PARAM -
*                    Unknown command or invalid argument
*                FLASH_ERR_NULL_PTR -
*                    Pointer was NULL for a command that expects a configuration structure
*                FLASH_ERR_BUSY -
*                    Flash peripheral is busy with another operation or not initialized
*                FLASH_ERR_ACCESSW -
*                    Bad address provided.
*                FLASH_ERR_FAILURE -
*                    Callback function not set, ROM too small, Hardware operation failed
*                FLASH_ERR_FREQUENCY -
*                    Illegal flash frequency
******************************************************************************/
FLASH_PE_MODE_SECTION
flash_err_t r_flash_control(flash_cmd_t cmd, void *pcfg)
{
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#if (FLASH_HAS_BOOT_SWAP)
    uint8_t *pSwapInfo = pcfg;
#endif
#if (FLASH_HAS_CF_ACCESS_WINDOW)
    flash_access_window_config_t *pAccessInfo = pcfg;
#endif
#if (FLASH_HAS_SEQUENTIAL_CF_BLOCKS_LOCK)
    flash_lockbit_config_t *pLockbitCfg = pcfg;
#endif
#if (FLASH_HAS_ROM_CACHE)
    uint8_t *pCacheStatus = pcfg;
#endif
#if (FLASH_HAS_NON_CACHED_RANGES)
    flash_non_cached_t *pNonCached = pcfg;
    uint32_t            size_reg_value;
#endif
#if (FLASH_IN_DUAL_BANK_MODE)
    uint32_t        banksel_val;
    flash_bank_t    *pBank = pcfg;
#endif
#endif // FLASH_CFG_CODE_FLASH_ENABLE

#if (FLASH_HAS_FCU)
    uint32_t *pFlashClkHz = pcfg, speed_mhz;
#endif
    flash_err_t err = FLASH_SUCCESS;


    if (FLASH_CMD_RESET == cmd)
    {
#if ((FLASH_TYPE == 1) && ((FLASH_CFG_CODE_FLASH_BGO == 1) || (FLASH_CFG_DATA_FLASH_BGO == 1)))
        /* cannot do reset if command in progress */
        if ((FLASH_ERASING == g_flash_state) || (FLASH_BLANKCHECK == g_flash_state))
        {
            flash_stop();   // can abort an outstanding erase or blankcheck command
        }

        while (FLASH_READY != g_flash_state)
            ;
#endif
        flash_reset();
        flash_release_state();
        return err;
    }

    /* Normally return BUSY when not in READY state. But allow for exceptions. */
    if (FLASH_INITIALIZATION == g_flash_state)
    {
        if (FLASH_CMD_CONFIG_CLOCK != cmd)      // allow clock config during Open()
        {
            return FLASH_ERR_BUSY;
        }
    }
    else if ((FLASH_READY != g_flash_state) && (FLASH_CMD_STATUS_GET != cmd))
    {
        return FLASH_ERR_BUSY;      // allow get status any time
    }

    /* Process commands */

    if (FLASH_CMD_STATUS_GET == cmd)
    {
        /* GET OPERATION COMPLETE STATUS */
        err = flash_get_status();
    }

#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1)) || (FLASH_CFG_DATA_FLASH_BGO == 1)
    else if (FLASH_CMD_SET_BGO_CALLBACK == cmd)
    {
        /* SET USER CALLBACK FUNCTION */
        FLASH_RETURN_IF_PCFG_NULL;
        err = flash_interrupt_config (true, pcfg);
    }
#endif // ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1)) || (FLASH_CFG_DATA_FLASH_BGO == 1)

#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)

#if (FLASH_HAS_ROM_CACHE == 1)
    else if (FLASH_CMD_ROM_CACHE_ENABLE == cmd)
    {
        FLASH.ROMCIV.BIT.ROMCIV = 1;                // start invalidation
        while (0 != FLASH.ROMCIV.BIT.ROMCIV)        // wait for invalidation to complete
        {
            R_BSP_NOP();
        }
        FLASH.ROMCE.BIT.ROMCEN = 1;                 // enable cache
        if (1 != FLASH.ROMCE.BIT.ROMCEN)
        {
            err = FLASH_ERR_FAILURE;
        }
    }
    else if (FLASH_CMD_ROM_CACHE_DISABLE == cmd)
    {
        FLASH.ROMCE.BIT.ROMCEN = 0;                 // disable cache
    }
    else if (FLASH_CMD_ROM_CACHE_STATUS == cmd)
    {
        *pCacheStatus = FLASH.ROMCE.BIT.ROMCEN;     // enabled/disabled state
    }
#endif // (FLASH_HAS_ROM_CACHE == 1)

#if (FLASH_HAS_NON_CACHED_RANGES == 1)
    else if (FLASH_CMD_SET_NON_CACHED_RANGE0 == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        err = set_non_cached_regs(pNonCached, (flash_non_cached_regs_t *)&FLASH.NCRG0);
    }
    else if (FLASH_CMD_SET_NON_CACHED_RANGE1 == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        err = set_non_cached_regs(pNonCached, (flash_non_cached_regs_t *)&FLASH.NCRG1);
    }
    else if (FLASH_CMD_GET_NON_CACHED_RANGE0 == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        pNonCached->start_addr = FLASH.NCRG0;
        size_reg_value = (uint32_t)FLASH.NCRC0.BIT.NCSZ + 1;
        pNonCached->size = (flash_non_cached_size_t) (size_reg_value << 4);
        pNonCached->type_mask = FLASH.NCRC0.LONG & FLASH_NON_CACHED_MASK;
    }
    else if (FLASH_CMD_GET_NON_CACHED_RANGE1 == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        pNonCached->start_addr = FLASH.NCRG1;
        size_reg_value = (uint32_t)FLASH.NCRC1.BIT.NCSZ + 1;
        pNonCached->size = (flash_non_cached_size_t) (size_reg_value << 4);
        pNonCached->type_mask = FLASH.NCRC1.LONG & FLASH_NON_CACHED_MASK;
    }
#endif // (FLASH_HAS_NON_CACHED_RANGES == 1)

#if (FLASH_HAS_BOOT_SWAP == 1)
#if (FLASH_IN_DUAL_BANK_MODE == 0)
    else if (FLASH_CMD_SWAPSTATE_GET == cmd)
    {
        /* GET CURRENT STARTUP AREA (NOT NECESSARILY PRESERVED THROUGH RESET */
        FLASH_RETURN_IF_PCFG_NULL;
        FLASH_RETURN_IF_ROM_LT_32K;
        *pSwapInfo = R_CF_GetCurrentSwapState();
    }
    else if (FLASH_CMD_SWAPSTATE_SET == cmd)
    {
        /* SET CURRENT STARTUP AREA (NOT NECESSARILY PRESERVED THROUGH RESET */
        FLASH_RETURN_IF_PCFG_NULL;
        FLASH_RETURN_IF_ROM_LT_32K;
        FLASH_RETURN_IF_BAD_SAS;
        R_CF_SetCurrentSwapState(*pSwapInfo);
    }
#endif // (FLASH_IN_DUAL_BANK_MODE == 0)
    else if (FLASH_CMD_SWAPFLAG_GET == cmd)
    {
        /* GET CURRENT STARTUP AREA EFFECTIVE AT RESET */
        FLASH_RETURN_IF_PCFG_NULL;
        FLASH_RETURN_IF_ROM_LT_32K;
        *pSwapInfo = R_CF_GetCurrentStartupArea();
    }
    else if (FLASH_CMD_SWAPFLAG_TOGGLE == cmd)
    {
        /* TOGGLE CURRENT STARTUP AREA EFFECTIVE AT RESET */
        FLASH_RETURN_IF_BGO_AND_NO_CALLBACK;
        FLASH_RETURN_IF_ROM_LT_32K;
        /* Lock the driver */
        if (FLASH_SUCCESS != flash_lock_state(FLASH_WRITING))       // TODO: ACCESS STATE?
        {
            return FLASH_ERR_BUSY;
        }
        err = R_CF_ToggleStartupArea();
#if (FLASH_CFG_CODE_FLASH_BGO == 0)
        flash_release_state();              // Unlock driver
#endif // (FLASH_CFG_CODE_FLASH_BGO == 0)
    }
#endif // (FLASH_HAS_BOOT_SWAP == 1)

#if (FLASH_HAS_CF_ACCESS_WINDOW == 1)
    else if (FLASH_CMD_ACCESSWINDOW_GET == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        err = R_CF_GetAccessWindow(pAccessInfo);
    }
    else if (FLASH_CMD_ACCESSWINDOW_SET == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        FLASH_RETURN_IF_BGO_AND_NO_CALLBACK;
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
        /* When providing block numbers, the start address is the larger block number (lower address)
         *   access_info.start_addr =  (uint32_t)FLASH_CF_BLOCK_3;     // ie. This will allow writing block 3
         *   access_info.end_addr =  (uint32_t)(FLASH_CF_BLOCK_2);
         *   access_info.start_addr =  (uint32_t)FLASH_CF_BLOCK_8;     // ie. This will allow writing blocks 3 - 8
         *   access_info.end_addr =  (uint32_t)(FLASH_CF_BLOCK_2);
         */
        if ((pAccessInfo->start_addr > pAccessInfo->end_addr)
         || ((uint32_t)FLASH_CF_LOWEST_VALID_BLOCK > pAccessInfo->start_addr)
         || (0 != (ACCESS_BAD_ADDR_MASK & pAccessInfo->start_addr))
         || ((0 != (ACCESS_BAD_ADDR_MASK & pAccessInfo->end_addr)) && ((uint32_t)FLASH_CF_BLOCK_END != pAccessInfo->end_addr)))
        {
            return(FLASH_ERR_ACCESSW);
        }
#endif // (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
        /* Lock the driver */
        if (FLASH_SUCCESS != flash_lock_state(FLASH_WRITING))       // TODO: ACCESS STATE?
        {
            return FLASH_ERR_BUSY;
        }
        err = R_CF_SetAccessWindow(pAccessInfo);
#if (FLASH_CFG_CODE_FLASH_BGO == 0)
        flash_release_state();              // Unlock the driver
#endif // (FLASH_CFG_CODE_FLASH_BGO == 0)
    }
#endif // (FLASH_HAS_CF_ACCESS_WINDOW == 1)

#if (FLASH_HAS_SEQUENTIAL_CF_BLOCKS_LOCK == 1)
    else if (FLASH_CMD_LOCKBIT_ENABLE == cmd)
    {
        g_lkbit_mode = FLASH_LOCKBIT_MODE_NORMAL;
    }
    else if (FLASH_CMD_LOCKBIT_DISABLE == cmd)
    {
        g_lkbit_mode = FLASH_LOCKBIT_MODE_OVERRIDE;
    }
    else if (FLASH_CMD_LOCKBIT_READ == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        FLASH_RETURN_IF_BGO_AND_NO_CALLBACK;
        err = flash_lockbit_read(pLockbitCfg->block_start_address, &(pLockbitCfg->result));
    }
    else if (FLASH_CMD_LOCKBIT_WRITE == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        FLASH_RETURN_IF_BGO_AND_NO_CALLBACK;
        err = flash_api_lockbit_set(pLockbitCfg->block_start_address, pLockbitCfg->num_blocks);
    }
#endif // (FLASH_HAS_SEQUENTIAL_CF_BLOCKS_LOCK == 1)

#endif // (FLASH_CFG_CODE_FLASH_ENABLE == 1)

#if (FLASH_HAS_FCU == 1)
    else if (FLASH_CMD_CONFIG_CLOCK == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        if ((FLASH_FREQ_HI < *pFlashClkHz) || (FLASH_FREQ_LO > *pFlashClkHz))
        {
            err = FLASH_ERR_FREQUENCY;
        }
        else
        {
            speed_mhz = *pFlashClkHz / 1000000;
            if (0 != (*pFlashClkHz % 1000000))
            {
                speed_mhz++;    // must round up to nearest MHz
            }
            FLASH.FPCKAR.WORD = (uint16_t)(0x1E00) + (uint16_t)speed_mhz;
#if ((FLASH_TYPE == 4) && (MCU_DATA_FLASH_SIZE_BYTES != 0))
            FLASH.EEPFCLK = (uint8_t)speed_mhz;
#endif // ((FLASH_TYPE == 4) && (MCU_DATA_FLASH_SIZE_BYTES != 0))
        }
    }
#endif // (FLASH_HAS_FCU == 1)

#if (FLASH_IN_DUAL_BANK_MODE == 1)
    else if (FLASH_CMD_BANK_TOGGLE == cmd)
    {
        err = flash_toggle_banksel_reg();
    }
    else if (FLASH_CMD_BANK_GET == cmd)
    {
        FLASH_RETURN_IF_PCFG_NULL;
        banksel_val = *((uint32_t *)BANKSEL_ADDR);
        *pBank = (0 == (banksel_val & BANKSWP_MASK)) ? FLASH_BANK0_FFE00000 : FLASH_BANK0_FFF00000;
    }
#endif // (FLASH_IN_DUAL_BANK_MODE == 1)

    else
    {
        err = FLASH_ERR_PARAM;
    }

    return err;
}

FLASH_SECTION_CHANGE_END /* end FLASH SECTION FRAM */
