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
* File Name    : r_flash_rx.h
* Description  : This is a private header file used internally by the FLASH module. It should not be modified or
*                included by the user in their application.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           12.07.2014 1.00    First Release
*           13.01.2015 1.10    Updated for RX231
*           11.02.2016 1.20    Updated for Flash Type 4
*           25.05.2016 1.30    Added FLASH_CUR_CF_BGO_READ/WRITE_LOCKBIT for flash type 3.
*           08.08.2016 2.00    Modified for BSPless operation. Added prototypes for flash_softwareLock()/Unlock().
*                              Added support for RX230.
*           18.11.2016 2.10    Added support for RX24U.
*                              Added equates FLASH_HAS_ERR_ISR and FLASH_HAS_FCU.
*           05.10.2016 3.00    Moved Flash Type 2 specific equates/declarations to r_flash_type2_if.h.
*                              Updated exported variables and functions to reflect updated r_flash_rx.c contents.
*           06.02.2017 3.10    Added support for RX65N-2M (bank/application swap).
*           19.06.2017 3.20    Moved FLASH_HAS_xxx equates to r_flash_rx_if.h
*           31.10.2017 3.30    Added extern for g_flash_lock;
*           27.02.2018 3.40    Added RX66T.
*           23.10.2018 3.50    Added RX72T.
*           19.04.2019 4.00    Removed support for flash type 2.
*           07.06.2019 4.10    Added RX23W.
*           19.07.2019 4.20    Added RX72M.
*           09.09.2019 4.30    Added RX13T.
*           27.09.2019 4.40    Added RX23E-A.
*                              Added include path for rx230.
*           18.11.2019 4.50    Added RX66N, and RX72N.
***********************************************************************************************************************/

#ifndef FLASH_RX_HEADER_FILE
#define FLASH_RX_HEADER_FILE

#include <stdbool.h>
#include "r_flash_rx_if.h"

/* Memory specifics for the each MCU group */
#if   defined(MCU_RX111)
    #include "./src/targets/rx111/r_flash_rx111.h"
#elif defined(MCU_RX110)
    #include "./src/targets/rx110/r_flash_rx110.h"
#elif defined(MCU_RX113)
    #include "./src/targets/rx113/r_flash_rx113.h"
#elif defined(MCU_RX130)
    #include "./src/targets/rx130/r_flash_rx130.h"
#elif defined(MCU_RX13T)
    #include "./src/targets/rx13t/r_flash_rx13t.h"
#elif defined(MCU_RX230)
    #include "./src/targets/rx230/r_flash_rx230.h"
#elif defined(MCU_RX231)
    #include "./src/targets/rx231/r_flash_rx231.h"
#elif defined(MCU_RX23E_A)
    #include "./src/targets/rx23e-a/r_flash_rx23e-a.h"
#elif defined(MCU_RX23T)
    #include "./src/targets/rx23t/r_flash_rx23t.h"
#elif defined(MCU_RX23W)
    #include "./src/targets/rx23w/r_flash_rx23w.h"
#elif defined(MCU_RX24T)
    #include "./src/targets/rx24t/r_flash_rx24t.h"
#elif defined(MCU_RX24U)
    #include "./src/targets/rx24u/r_flash_rx24u.h"
#elif defined(MCU_RX64M)
    #include <targets/rx64m/r_flash_rx64m.h>
#elif defined(MCU_RX651) || defined(MCU_RX65N)
    #include "./src/targets/rx65n/r_flash_rx65n.h"
#elif defined(MCU_RX66T)
    #include "./src/targets/rx66t/r_flash_rx66t.h"
#elif defined(MCU_RX66N)
    #include "./src/targets/rx66n/r_flash_rx66n.h"
#elif defined(MCU_RX71M)
    #include "./src/targets/rx71m/r_flash_rx71m.h"
#elif defined(MCU_RX72T)
    #include "./src/targets/rx72t/r_flash_rx72t.h"
#elif defined(MCU_RX72M)
    #include "./src/targets/rx72m/r_flash_rx72m.h"
#elif defined(MCU_RX72N)
    #include "./src/targets/rx72n/r_flash_rx72n.h"
#else
    #error "!!! No 'targets' folder for this MCU Group !!!"
#endif


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define FCU_COMMAND_AREA                (0x007E0000)


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* These typedefs are used for guaranteeing correct accesses to memory. When
   working with the FCU sometimes byte or word accesses are required. */
typedef R_BSP_VOLATILE_EVENACCESS uint8_t  * FCU_BYTE_PTR;
typedef R_BSP_VOLATILE_EVENACCESS uint16_t * FCU_WORD_PTR;
typedef R_BSP_VOLATILE_EVENACCESS uint32_t * FCU_LONG_PTR;


typedef enum
{
    FLASH_UNINITIALIZED = 0,
    FLASH_INITIALIZATION,
    FLASH_READY,
    FLASH_WRITING,
    FLASH_ERASING,
    FLASH_BLANKCHECK,
    FLASH_GETSTATUS,
    FLASH_LOCK_BIT
} flash_states_t;


typedef enum _current
{
    FLASH_CUR_INVALID,
    FLASH_CUR_IDLE,
    FLASH_CUR_DF_ERASE,
    FLASH_CUR_DF_BGO_ERASE,
    FLASH_CUR_DF_BLANKCHECK,
    FLASH_CUR_DF_BGO_BLANKCHECK,
    FLASH_CUR_DF_WRITE,
    FLASH_CUR_DF_BGO_WRITE,
    FLASH_CUR_CF_ERASE,
    FLASH_CUR_CF_BGO_ERASE,
    FLASH_CUR_CF_BLANKCHECK,
    FLASH_CUR_CF_BGO_BLANKCHECK,
    FLASH_CUR_CF_WRITE,
    FLASH_CUR_CF_BGO_WRITE,
    FLASH_CUR_CF_TOGGLE_STARTUPAREA,
    FLASH_CUR_CF_ACCESSWINDOW,
    FLASH_CUR_CF_BGO_READ_LOCKBIT,
    FLASH_CUR_CF_BGO_WRITE_LOCKBIT,
    FLASH_CUR_FCU_INIT,
    FLASH_CUR_LOCKBIT_SET,
    FLASH_CUR_CF_TOGGLE_BANK,
    FLASH_CUR_STOP
} flash_cur_t;


typedef struct _current_parameters
{
    uint32_t    src_addr;               /* Source address for operation in progress */
    uint32_t    dest_addr;              /* Destination address for operation in progress */
    uint32_t    total_count;            /* Total number of bytes to write/erase */
    uint32_t    current_count;          /* Bytes of total completed */
    flash_cur_t current_operation;      /* Operation in progress, ie. FLASH_CUR_CF_ERASE */
    uint16_t    fcu_min_write_cnt;      /* Num 2-byte writes needed for min write size; differs for CF and DF */
    uint32_t    wait_cnt;               /* Worst case wait time for operation completion */
    bool        bgo_enabled_df;
    bool        bgo_enabled_cf;
} current_param_t;


typedef enum
{
    FLASH_LOCKBIT_MODE_NORMAL,
    FLASH_LOCKBIT_MODE_OVERRIDE,
    FLASH_LOCKBIT_MODE_END_ENUM
} lkbit_mode_t;


/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/
extern int32_t g_flash_lock;
extern flash_states_t g_flash_state;
extern FCU_BYTE_PTR g_pfcu_cmd_area;


/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
extern flash_err_t flash_lock_state(flash_states_t new_state);
extern void flash_release_state(void);
extern bool flash_softwareLock (int32_t * const plock);
extern bool flash_softwareUnlock (int32_t * const plock);


#endif /* FLASH_RX_HEADER_FILE */

/* end of file */
