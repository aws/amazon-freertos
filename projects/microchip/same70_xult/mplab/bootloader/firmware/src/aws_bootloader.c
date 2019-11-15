/****************************************************************
  Module for Microchip Bootloader Library

  Company:
    Microchip Technology Inc.

  File Name:
    aws_bootloader.c

  Summary:
    The header file joins all header files used in the Bootloader Library
    and contains compile options and defaults.

  Description:
    This header file includes all the header files required to use the
    Microchip Bootloader Library. Library features and options defined
    in the Bootloader Library configurations will be included in each build.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
//#include "system_config.h"
//#include "system_definitions.h"
#include "definitions.h"                // SYS function prototypes

#include "aws_flash_config.h"
#include "aws_bootloader.h"
//#include "core_cm7.h"
#include "aws_nvm.h"
#include "aws_crc.h"
#include <string.h>




// defines

#define AWS_BOOT_DEBUG_BASIC            0x01
#define AWS_BOOT_DEBUG_VALIDATE         0x02
#define AWS_BOOT_DEBUG_LAUNCH           0x04
#define AWS_BOOT_DEBUG_LAUNCH_FLAGS     0x08
#define AWS_BOOT_DEBUG_COMMANDS         0x10
#define AWS_BOOT_DEBUG_FLASH            0x20

// all debug capabilities on
#define AWS_BOOT_DEBUG_MASK         (AWS_BOOT_DEBUG_BASIC | AWS_BOOT_DEBUG_VALIDATE | AWS_BOOT_DEBUG_LAUNCH | AWS_BOOT_DEBUG_LAUNCH_FLAGS)

static void run_Application(uint32_t launch_address);

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_BASIC) != 0)
static bool bootAssert(bool cond, const char* fname, int lineno)
{
    if(cond == false)
    {
        printf("Boot Assert FAILED in function: %s, line: %d\r\n", fname, lineno);
    }
    return cond;
}

#define BOOT_ASSERT(cond) bootAssert(cond, __func__, __LINE__)
#else
#define BOOT_ASSERT(cond)   (void)(cond)
#endif

typedef enum
{
    AWS_BOOT_STATE_VALIDATION   = 0,    // validation of the app image
    AWS_BOOT_STATE_START_EXEC,          // start launching of the app image into execution
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    AWS_BOOT_STATE_WAIT_EXEC,           // wait for a timeout to pass, until actually launching
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
#if (AWS_FLASH_SECTIONS == 3)
    AWS_BOOT_STATE_SAVE_RUN_SET_NEW,    // the Run app needs to be saved, then
                                        // New loaded app needs to be set in place and executed
    AWS_BOOT_STATE_SET_NEW,             // the New loaded app needs to be set in place and executed
    AWS_BOOT_STATE_RESTORE_SAVE,        // the Saved app needs to be restored and executed
#else
    AWS_BOOT_STATE_SET_NEW,             // the New loaded app needs to be set in place and executed
#endif // (AWS_FLASH_SECTIONS == 3)
    // ....
    AWS_BOOT_STATE_ERROR,               // some error condition occurred
    AWS_BOOT_STATE_FATAL,               // some fatal error condition occurred
    AWS_BOOT_STATE_RESET,               // will reset because of fatal condition
}AWS_BOOT_STATE;

// data

static AWS_BOOT_STATE               aws_boot_state;         // current state machine
static const AWS_BOOT_IMAGE_DCPT*   app_img_to_exec;        // image to be launched


static const uint32_t*              app_reset_addr_ovr = 0; // application reset address override
                                                            // if execution to be launched at certain address

static uint64_t                     t_start_exec;           // time when the execution phase started

AWS_BOOT_LAUNCH_DCPT                aws_launch_dcpt;        // descriptor used to launch the application

// proto 
// AWS bootloader status function
// returns the new state
typedef AWS_BOOT_STATE(*BootStateFnc)(void); 

static AWS_BOOT_STATE   AWS_State_ValidateImages(void);
static AWS_BOOT_STATE   AWS_State_ExecuteImage(void);
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
static AWS_BOOT_STATE   AWS_State_WaitExecute(void);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
#if (AWS_FLASH_SECTIONS == 3)
static AWS_BOOT_STATE   AWS_State_SaveRunSetNew(void);
static AWS_BOOT_STATE   AWS_State_RestoreSave(void);
#endif // (AWS_FLASH_SECTIONS == 3)
static AWS_BOOT_STATE   AWS_State_SetNew(void);
static AWS_BOOT_STATE   AWS_State_Error(void);
static AWS_BOOT_STATE   AWS_State_Fatal(void);
static AWS_BOOT_STATE   AWS_State_Reset(void);

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
static int          BootCmdLoader(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdTime(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdRun(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdPrintDcpt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdFlags(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          TestCertFlash(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          DumpCommand(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          CrcCommand(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          CommitCommand(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void         BootPrintDcpt(const AWS_BOOT_IMAGE_DCPT* pAppDcpt, const char* hdr_msg);
static bool         ParseAddressString(const char* addr_string, const uint8_t** ppAddr);
static void         AsciiHexToBin(const char* str, uint8_t* buff, int stringLen);
static const uint32_t* EraseCheck(const uint32_t* addr, int size_32);
static void         DumpBuffer(const uint8_t* dump_addr, int size);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

static bool         BootAppValidate(const AWS_BOOT_IMAGE_DCPT* pAppDcpt, uint32_t app_address);
static bool         BootAppAddressValidate(const void* address);

static bool         BootInvalidateAppDcpt(const AWS_BOOT_IMAGE_DCPT* pAppDcpt);

static void         BootLaunchApp(const void* reset_add);

static bool         BootEraseDstSpace(const AWS_BOOT_IMAGE_DCPT* pDstDcpt, uint32_t dstSpaceEnd);
static bool         BootCopyApp(const AWS_BOOT_IMAGE_DCPT* pSrcDcpt, uint32_t dstAppStart, uint32_t dstAppEnd, uint32_t srcAppStart);
static bool         BootCopyDcpt(const AWS_BOOT_IMAGE_DCPT* pDstDcpt, const AWS_BOOT_IMAGE_DCPT* pSrcDcpt);

// const data

// app start address needs to be >= than this limit
static const void* const start_address_limit = (const void*)AWS_FLASH_RUN_APP_START;
// app end address needs to be < than this limit
static const void* const end_address_limit = (const void*)(AWS_FLASH_RUN_APP_START + AWS_IMAGE_MAX_SIZE);


static const BootStateFnc   awsStateFncTbl[] = 
{
    AWS_State_ValidateImages,       // AWS_BOOT_STATE_VALIDATION
    AWS_State_ExecuteImage,         // AWS_BOOT_STATE_START_EXEC
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    AWS_State_WaitExecute,          // AWS_BOOT_STATE_WAIT_EXEC
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
#if (AWS_FLASH_SECTIONS == 3)
    AWS_State_SaveRunSetNew,        // AWS_BOOT_STATE_SAVE_RUN_SET_NEW
    AWS_State_SetNew,               // AWS_BOOT_STATE_SET_NEW
    AWS_State_RestoreSave,          // AWS_BOOT_STATE_RESTORE_SAVE
#else
    AWS_State_SetNew,               // AWS_BOOT_STATE_SET_NEW
#endif // (AWS_FLASH_SECTIONS == 3)

    // last error state
    AWS_State_Error,                // AWS_BOOT_STATE_ERROR
    AWS_State_Fatal,                // AWS_BOOT_STATE_FATAL
    AWS_State_Reset,                // AWS_BOOT_STATE_RESET
};

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
static bool     signature_enable =  false;      // if signature validation is enabled
static bool     crc_enable =  false;            // if CRC validation is enabled
static bool     erase_enable =  false;          // if erasing an invalid image is to be enabled
static bool     exec_enable =  false;           // if executing the app image is to be enabled
static bool     flags_validate_enable = false;  // if validation of flags is enabled
static bool     flags_change_enable = false;    // enable changing of the image flags

static bool     run_img_validation = false;     // if image validation to be launched 

static bool     init_failed;                    // run time flag
static int      run_count;                      // run counter

static const SYS_CMD_DESCRIPTOR    bootCmdTbl[] = 
{
        {"loader",   BootCmdLoader,            ": Loader info"},
        {"time",     BootCmdTime,              ": Get current time info"},
        {"run",      BootCmdRun,               ": Let bootloader run"},
        {"print",    BootCmdPrintDcpt,         ": Print a descriptor"},
        {"flags",    BootCmdFlags,             ": Set the run flags"},
        // various tests and helpers
        {"dump",     DumpCommand,              ": dump command"},
        {"calc_crc", CrcCommand,               ": crc command"},
        {"commit",   CommitCommand,            ": commit command"},
        {"cert",     TestCertFlash,            ": test cert flash"},
};
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)


bool AWS_Bootloader_Initialize(void)
{
    bool iniFail;
    app_img_to_exec = 0;
    app_reset_addr_ovr = 0;

    while(true)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_BASIC) != 0)
        // check that the calculated sizes are right
        if(!bootAssert(sizeof(AWS_NVM_PAGE_SIZE) == SIZEOF_AWS_BOOT_IMAGE_DCPT, __func__, __LINE__))
        {
            iniFail = true;
            break;
        }
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_BASIC) != 0)

        // create command group
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
        if(!SYS_CMD_ADDGRP(bootCmdTbl, sizeof(bootCmdTbl)/sizeof(*bootCmdTbl), "boot", ": commands"))
        {
            printf("AWS Boot: No Boot commands. Failed to initialize!\r\n");
            iniFail = true;
            break;
        }
        printf("AWS Boot: Started. Waiting for a Boot command....\r\n");
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

        iniFail = false;
        break;
    }

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    init_failed = iniFail;
    run_count = 0;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

    if(iniFail)
    {
        aws_boot_state = AWS_BOOT_STATE_ERROR;
        return false;
    }


    aws_boot_state = AWS_BOOT_STATE_VALIDATION;
    return true;
}

// bootloader task, doing the job
void  AWS_Bootloader_Task(void)
{
    // basic sanity check
    if(aws_boot_state >= sizeof(awsStateFncTbl) / sizeof(*awsStateFncTbl))
    {
        aws_boot_state = AWS_BOOT_STATE_FATAL;
    }
    // dispatch
    aws_boot_state = (*awsStateFncTbl[aws_boot_state])();
}


// AWS_BOOT_STATE_VALIDATION
static AWS_BOOT_STATE AWS_State_ValidateImages(void)
{
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    if(run_img_validation == false)
    {   // wait for command
        return AWS_BOOT_STATE_VALIDATION;
    }
    run_img_validation = false;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

    app_img_to_exec = 0;

    if(app_reset_addr_ovr != 0)
    {   // forced address in debug... 
        return AWS_BOOT_STATE_START_EXEC;
    }

    // no override; try to find a valid descriptor
    const AWS_BOOT_IMAGE_DCPT* pRunDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_RUN_DCPT_START;
    bool isRunValid = BootAppValidate(pRunDcpt, AWS_FLASH_RUN_APP_START);
    const AWS_BOOT_IMAGE_DCPT* pNewDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_NEW_DCPT_START;
    bool isNewValid = BootAppValidate(pNewDcpt, AWS_FLASH_NEW_APP_START);

    if(isRunValid == true)
    {   // run image valid
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: valid signature found in Run: %lu\r\n", pRunDcpt->version);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
    }
    else
    {
        BootInvalidateAppDcpt(pRunDcpt);
    }

    if(isNewValid)
    {   // new image pending and valid   
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: valid signature found in New, Run Version: %lu New Version: %lu\r\n",  pRunDcpt->version, pNewDcpt->version);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
    }
    else
    {
        BootInvalidateAppDcpt(pNewDcpt);
    }

    if(isRunValid && (isNewValid == false || pRunDcpt->version >= pNewDcpt->version))
    {   // execute the run app
        if(isNewValid)
        {
            BootInvalidateAppDcpt(pNewDcpt);
        }
        app_img_to_exec = pRunDcpt;
        return AWS_BOOT_STATE_START_EXEC;
    }

    if(isNewValid)
    {   // need to program and execute the New app
#if (AWS_FLASH_SECTIONS == 3)
        return isRunValid ? AWS_BOOT_STATE_SAVE_RUN_SET_NEW : AWS_BOOT_STATE_SET_NEW;
#else
        return AWS_BOOT_STATE_SET_NEW;
#endif // (AWS_FLASH_SECTIONS == 3)
    }

#if (AWS_FLASH_SECTIONS == 3)
    const AWS_BOOT_IMAGE_DCPT* pSaveDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_SAVE_DCPT_START;
    bool isSaveValid = BootAppValidate(pSaveDcpt, AWS_FLASH_SAVE_APP_START);
    if(isSaveValid)
    {   // restore the saved copy
        return AWS_BOOT_STATE_RESTORE_SAVE;
    }
    else
    {
        BootInvalidateAppDcpt(pSaveDcpt);
    }
#endif // (AWS_FLASH_SECTIONS == 3)

    // all images are hosen!

    // nothing to be done...except launch a default app if there's something there
#if (AWS_ENABLE_DEFAULT_START != 0)
    const uint32_t* ivt_address = (const uint32_t*)AWS_DEFAULT_FLASH_IMAGE_START;
    const uint32_t* defaultStart = (const uint32_t*)(*(ivt_address + 1) & 0xfffffffe);
    if(BootAppAddressValidate(defaultStart) && *defaultStart != 0xffffffff)
    {   // minimal sanity check
        app_reset_addr_ovr = ivt_address;
        return AWS_BOOT_STATE_START_EXEC;
    }
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    else
    {
        printf("AWS Launch Default: nothing valid at 0x%08x, : 0x%08x\r\n", (uint32_t)ivt_address, (uint32_t)defaultStart);
    }
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
#endif  // (AWS_ENABLE_DEFAULT_START != 0)


#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    // stay in this state so we can reissue the "run command"
    return AWS_BOOT_STATE_VALIDATION;
#else
    return AWS_BOOT_STATE_ERROR;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
        
}

// AWS_BOOT_STATE_START_EXEC
// launch image into execution
static AWS_BOOT_STATE AWS_State_ExecuteImage(void)
{
    const void* reset_add;  // address to start the application from

    if(app_reset_addr_ovr != 0)
    {   // have a forced reset address
        reset_add = app_reset_addr_ovr;
    }
    else if(app_img_to_exec != 0)
    {
        reset_add = (const void*)app_img_to_exec->start_address;

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
        bool change_flags = flags_change_enable;
#else
        bool change_flags = true;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

        if(change_flags)
        {   
            uint8_t header_flags = app_img_to_exec->header.img_flags;
            if(header_flags == AWS_BOOT_FLAG_IMG_NEW)
            {   // first launch of this app
                AWS_BOOT_IMAGE_DCPT copy_dcpt = *app_img_to_exec;
                copy_dcpt.header.img_flags = AWS_BOOT_FLAG_IMG_TEST;

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH_FLAGS) != 0)
                printf("AWS Launch: Updating the flags: 0x%02x, to: 0x%02x, dcpt: 0x%08x\r\n", (uint32_t)header_flags, (uint32_t)copy_dcpt.header.img_flags, (uint32_t)app_img_to_exec);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH_FLAGS) != 0)

                if(!AWS_FlashBlockWrite((uint8_t*)AWS_FLASH_RUN_DCPT_START, (uint8_t*)&copy_dcpt, sizeof(copy_dcpt)))
                {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH_FLAGS) != 0)
                    printf("AWS Launch: Failed to update the run flags: 0x%08x\r\n", (uint32_t)app_img_to_exec);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH_FLAGS) != 0)
                }
                else
                {
                    

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH_FLAGS) != 0)
                    
                    printf("AWS Launch: Updated the run flags and Reboot: 0x%08x\r\n", (uint32_t)app_img_to_exec);
                    
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH_FLAGS) != 0)
                    
                }
            }
            else
            {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH_FLAGS) != 0)
                printf("AWS Launch: Skipped updating the flags: 0x%02x, dcpt: 0x%08x\r\n", header_flags, app_img_to_exec);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH_FLAGS) != 0)
            }
        }
    }
    else
    {   // should never get here!
        BOOT_ASSERT(false);
        return AWS_BOOT_STATE_FATAL;
    }
    

#if  ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    if(exec_enable == false)
    {
        printf("AWS Launch: skip start app at: 0x%08x\r\n", reset_add);
        // we'll relaunch again
        return AWS_BOOT_STATE_VALIDATION;
    }
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    
    printf("AWS Launch:  wait for app at: 0x%08x\r\n", reset_add);
    
    t_start_exec = SYS_TIME_Counter64Get();
    return AWS_BOOT_STATE_WAIT_EXEC;
#else
    BootLaunchApp(reset_add);
    return AWS_BOOT_STATE_VALIDATION;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)

}

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
// AWS_BOOT_STATE_WAIT_EXEC
static AWS_BOOT_STATE AWS_State_WaitExecute(void)
{
    // wait 2s for all the messages to go out
    uint64_t tCurr = SYS_TIME_Counter64Get();
    if((tCurr - t_start_exec) < (2 * SYS_TIME_FrequencyGet()))
    {
        return AWS_BOOT_STATE_WAIT_EXEC;
    }

    // now launch
    const void* reset_add = app_reset_addr_ovr == 0 ? (const void*)app_img_to_exec->start_address : app_reset_addr_ovr;
    app_reset_addr_ovr = 0;

    BootLaunchApp(reset_add);
    return AWS_BOOT_STATE_VALIDATION;
}
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)


// AWS_BOOT_STATE_SAVE_RUN_SET_NEW
#if (AWS_FLASH_SECTIONS == 3)
static AWS_BOOT_STATE AWS_State_SaveRunSetNew(void)
{
    // there's a new application pending
    // but 1st we copy the Run app to the Save app space

    // We first check that we need to to the save, if not already there
    const AWS_BOOT_IMAGE_DCPT* pRunDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_RUN_DCPT_START;
    const AWS_BOOT_IMAGE_DCPT* pSaveDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_SAVE_DCPT_START;

    bool skipSave = false;
    bool isSaveValid = BootAppValidate(pSaveDcpt, AWS_FLASH_SAVE_APP_START);
    if(isSaveValid)
    {   // check that matches the Run app
        if(memcmp(pRunDcpt, pSaveDcpt, sizeof(*pRunDcpt)) == 0)
        {   // match
            skipSave = true;
        }
    }

    if(!skipSave)
    {  
        // 1. erase the Save space: descriptor + app
        BootEraseDstSpace(pSaveDcpt, AWS_FLASH_SAVE_APP_END);

        // 2. store the Run app over the Save app
        BootCopyApp(pRunDcpt, AWS_FLASH_SAVE_APP_START, AWS_FLASH_SAVE_APP_END, AWS_FLASH_RUN_APP_START);

        // 3. store Run dcpt over Save dcpt
        BootCopyDcpt(pSaveDcpt, pRunDcpt);
    }

    //  then jump to AWS_BOOT_STATE_SET_NEW
    return AWS_BOOT_STATE_SET_NEW;
}
#endif // (AWS_FLASH_SECTIONS == 3)

// AWS_BOOT_STATE_SET_NEW
static AWS_BOOT_STATE AWS_State_SetNew(void)
{
    // set the new application pending

    printf("To start Execution for New Image \r\n");
    // 1. erase the Run space: descriptor + app
    const AWS_BOOT_IMAGE_DCPT* pRunDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_RUN_DCPT_START;
    BootEraseDstSpace(pRunDcpt, AWS_FLASH_RUN_APP_END);
    
    printf("Erasing Completed for Running Image \r\n");
    
    // 2. store the New app over the Run app
    const AWS_BOOT_IMAGE_DCPT* pNewDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_NEW_DCPT_START;
    BootCopyApp(pNewDcpt, AWS_FLASH_RUN_APP_START, AWS_FLASH_RUN_APP_END, AWS_FLASH_NEW_APP_START);

    printf("Copy Completed for Running Image \r\n");
    
    // 3. store New dcpt over Run dcpt
    BootCopyDcpt(pRunDcpt, pNewDcpt);

    printf("Copy DSCR completed for Running Image \r\n");

    // 4. invalidate NDCPT
    // No harm in leaving the NDCPT in there.
    // It will be erased by the new OTA anyway

    // 5. all done; goto launch
    app_img_to_exec = pRunDcpt;
    printf("To start Execution for New Image #2 \r\n");
    return AWS_BOOT_STATE_START_EXEC;
}

// AWS_BOOT_STATE_RESTORE_SAVE
// restore the saved app over the run app space
#if (AWS_FLASH_SECTIONS == 3)
static AWS_BOOT_STATE AWS_State_RestoreSave(void)
{
    // 1. erase the Run space: descriptor + app
    const AWS_BOOT_IMAGE_DCPT* pRunDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_RUN_DCPT_START;
    BootEraseDstSpace(pRunDcpt, AWS_FLASH_RUN_APP_END);
    
    // 2. store the Save app over the Run app
    const AWS_BOOT_IMAGE_DCPT* pSaveDcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_SAVE_DCPT_START;
    BootCopyApp(pSaveDcpt, AWS_FLASH_RUN_APP_START, AWS_FLASH_RUN_APP_END, AWS_FLASH_SAVE_APP_START);

    // 3. store Save dcpt over Run dcpt
    BootCopyDcpt(pRunDcpt, pSaveDcpt);

    // 4. all done; goto launch
    app_img_to_exec = pRunDcpt;
    return AWS_BOOT_STATE_START_EXEC;
}
#endif // (AWS_FLASH_SECTIONS == 3)

// AWS_BOOT_STATE_ERROR
static AWS_BOOT_STATE AWS_State_Error(void)
{
    return AWS_BOOT_STATE_ERROR;
}

// AWS_BOOT_STATE_FATAL
static AWS_BOOT_STATE AWS_State_Fatal(void)
{
  
    printf("AWS Boot: Fatal condition! Will reset in 1 second!\r\n");
    t_start_exec = SYS_TIME_Counter64Get();
    return AWS_BOOT_STATE_RESET;
}

// AWS_BOOT_STATE_RESET
static AWS_BOOT_STATE AWS_State_Reset(void)
{
    uint64_t tCurr = 0;
    
    tCurr = SYS_TIME_Counter64Get();

    if((tCurr - t_start_exec) < 1 * SYS_TIME_FrequencyGet())
    {
        return AWS_BOOT_STATE_RESET;
    }

    NVIC_SystemReset();

    return AWS_BOOT_STATE_FATAL;
}

// working routines

// erase the Dst space: descriptor + app
// starts with the destination descriptor address up to dest space end
static bool BootEraseDstSpace(const AWS_BOOT_IMAGE_DCPT* pDstDcpt, uint32_t dstSpaceEnd)
{
    int32_t dstTotSize = dstSpaceEnd - (uint32_t)pDstDcpt; 
    int nErasePages = dstTotSize / AWS_NVM_PAGE_SIZE;
    BOOT_ASSERT((nErasePages % AWS_NVM_ERASE_PAGES_MIN) == 0);
    bool eraseRes = AWS_FlashPagesErase((const uint32_t*)pDstDcpt, nErasePages);
    BOOT_ASSERT(eraseRes == true);
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_FLASH) != 0)
        printf("AWS Flash erased dcpt: 0x%08x, to: 0x%08x\r\n", pDstDcpt, dstSpaceEnd);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_FLASH) != 0)

    return eraseRes;
}

// copy the Src app over the Dst app
// the destination space should have been previously erased
static bool BootCopyApp(const AWS_BOOT_IMAGE_DCPT* pSrcDcpt, uint32_t dstAppStart, uint32_t dstAppEnd, uint32_t srcAppStart)
{
    uint32_t srcAppSize = pSrcDcpt->end_address - pSrcDcpt->start_address; 
    uint32_t dstSpace = dstAppEnd - dstAppStart;
    BOOT_ASSERT(dstSpace >= srcAppSize);
    BOOT_ASSERT(srcAppSize%512);

    const uint8_t* dstCopyAdd = (const uint8_t*)dstAppStart;
    const uint8_t* srcCopyAdd = (const uint8_t*)srcAppStart;

    BOOT_ASSERT(dstCopyAdd + srcAppSize <= (const uint8_t*)dstAppEnd);
    printf("To Copy BootCopyApp \r\n");
    bool writeRes = AWS_FlashBlockWriteEx(dstCopyAdd, srcCopyAdd, srcAppSize);
    
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_FLASH) != 0)
        printf("AWS Flash copied app from: 0x%08x, to: 0x%08x, size: 0x%08x\r\n", srcCopyAdd, dstCopyAdd, srcAppSize);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_FLASH) != 0)
    BOOT_ASSERT(writeRes == true);
    return writeRes;
}


// copy an AWS_BOOT_IMAGE_DCPT from source to a destination
// the destination space should have been previously erased
static bool BootCopyDcpt(const AWS_BOOT_IMAGE_DCPT* pDstDcpt, const AWS_BOOT_IMAGE_DCPT* pSrcDcpt)
{
    bool writeRes = AWS_FlashBlockWriteEx((uint8_t*)pDstDcpt, (uint8_t*)pSrcDcpt, sizeof(*pSrcDcpt));
    BOOT_ASSERT(writeRes == true);

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_FLASH) != 0)
        printf("AWS Flash copied dcpt from: 0x%08x, to: 0x%08x\r\n", pSrcDcpt, pDstDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_FLASH) != 0)

    return writeRes;
}

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
static int BootCmdLoader(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "AWS Bootloader version: %d.%d\r\n", AWS_BOOTLOADER_VERSION >> 8, (uint8_t)AWS_BOOTLOADER_VERSION);
    return 0;
}

static int BootCmdTime(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "AWS Boot: current time: %d\r\n", SYS_TMR_TickCountGet() / SYS_TMR_TickCounterFrequencyGet());
    return 0;
}


static int BootCmdRun(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // run <addr>
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(init_failed)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Init failed! Cannot run!\r\n");
        return 0;
    }

    if(argc > 1)
    {
        const uint8_t* start_add;
        bool add_valid = ParseAddressString(argv[1], &start_add);

        if(!add_valid)
        {
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "Run: %s is not a valid hex address\r\n", argv[1]);
            return 0;
        }

        app_reset_addr_ovr = (const uint32_t*)start_add;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Will run the bootloader...count: %d, exec address: 0x%08x\r\n", run_count++, app_reset_addr_ovr);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Will run the bootloader...count: %d\r\n", run_count++);
    }

    run_img_validation = true;
    return 0;
}

static int BootCmdPrintDcpt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // print run/new/save
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    bool print_usage = false;

    while(true)
    {
        if(argc != 2)
        {
            print_usage = true;
            break;
        }

        if(strcmp(argv[1], "run") == 0)
        {
            BootPrintDcpt((const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_RUN_DCPT_START, "RunDcpt ");
        }
        else if(strcmp(argv[1], "new") == 0)
        {
            BootPrintDcpt((const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_NEW_DCPT_START, "NewDcpt");
        }
#if (AWS_FLASH_SECTIONS == 3)
        else if(strcmp(argv[1], "save") == 0)
        {
            BootPrintDcpt((const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_SAVE_DCPT_START, "SaveDcpt");
        }
#endif // (AWS_FLASH_SECTIONS == 3)
        else
        {
            print_usage = true;
        }

        break;
    }

    if(print_usage)
    {
#if (AWS_FLASH_SECTIONS == 3)
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: print run/new/save\r\n");
#else
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: print run/new\r\n");
#endif // (AWS_FLASH_SECTIONS == 3)
    }
    return 0;
}

static int BootCmdFlags(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // flags validate/change/run 0/1/2
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    bool enableSign = signature_enable;
    bool enableCrc = crc_enable;
    bool enableErase = erase_enable;
    bool enableExec = exec_enable;
    bool enableValidate = flags_validate_enable;
    bool enableChange = flags_change_enable;

    bool setRun = false;
    int run_flags = 0;

    bool print_usage = false;
    while(true)
    {
        if(argc != 1 && argc != 3)
        {
            print_usage = true;
            break;
        }

        if(argc > 1)
        {
            if(strcmp(argv[1], "sign") == 0)
            {
                enableSign = atoi(argv[2]) != 0;
            }
            else if(strcmp(argv[1], "crc") == 0)
            {
                enableCrc = atoi(argv[2]) != 0;
            }
            else if(strcmp(argv[1], "erase") == 0)
            {
                enableErase = atoi(argv[2]) != 0;
            }
            else if(strcmp(argv[1], "exec") == 0)
            {
                enableExec = atoi(argv[2]) != 0;
            }
            else if(strcmp(argv[1], "validate") == 0)
            {
                enableValidate = atoi(argv[2]) != 0;
            }
            else if(strcmp(argv[1], "change") == 0)
            {
                enableChange = atoi(argv[2]) != 0;
            }
            else if(strcmp(argv[1], "run") == 0)
            {
                run_flags = atoi(argv[2]);
                setRun = true;
            }
            else
            {
                print_usage = true;
            }
        }
        break;
    }

    if(print_usage)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: flags <sign/crc/erase/exec/validate/change/run 0/1/2>\r\n");
        return 0;
    }


    if(!setRun)
    {
        signature_enable        = enableSign;
        crc_enable              = enableCrc;
        erase_enable            = enableErase;
        exec_enable             = enableExec;
        flags_validate_enable   = enableValidate;
        flags_change_enable     = enableChange;

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "flags sign: %d, crc: %d, erase: %d, exec: %d, validate: %d, change: %d\r\n", enableSign, enableCrc, enableErase, enableExec, enableValidate, enableChange);
        return 0;
    }

    // set all the run flags
    const char* run_msg;
    if(run_flags == 0)
    {
        run_msg = " all execution flags off";
        signature_enable =  false; 
        crc_enable =  false;  
        erase_enable =  false;
        exec_enable =  false;
        flags_validate_enable = false;
        flags_change_enable = false;
    }
    else
    {
        signature_enable =  true; 
        crc_enable =  true;  
        exec_enable =  true;
        flags_validate_enable = true;
        flags_change_enable = true;

        if(run_flags == 1)
        {
            erase_enable =  true;
            run_msg = " all execution flags on";
        }
        else
        {
            erase_enable =  false;
            run_msg = " all execution flags on xcpt erase";
            run_flags = 2;
        }
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "run flags set to: %d, %s\r\n", run_flags, run_msg);
    return 0;
}

// convert ascii hex tring to bin
// input ascii string should have an even number of characters!
// buff should be large enough to hold the characters
// reverse the order
static void AsciiHexToBin(const char* str, uint8_t* buff, int stringLen)
{
    int ix;
    char hi, lo;
    int len = stringLen / 2;

    memset(buff, 0, len);
    uint8_t* end_buff = buff + len; 
    const char* p = str;
    for(ix = 0; ix < len; ix++, p += 2)
    {
        hi = *p;
        lo = *(p+1);

        if(hi > '9')
        {
            hi -= 7;
        }
        if(lo > '9')
        {
            lo -= 7;
        }

        *--end_buff =(hi << 4) | (lo & 0x0f); 
    }

}

static int DumpCommand(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // dump address size
    // ex: dump yyyyyyyy 20 

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc < 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: dump address <size>\r\n");
        return 0;
    }

    int size;

    if(argc >=3)
    {
        size = atoi(argv[2]);
    }
    else
    {
        size = 32;
    }

    const uint8_t* dump_addr;
    bool dump_add_valid = ParseAddressString(argv[1], &dump_addr);

    if(!dump_add_valid) 
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Please enter a valid hex address, not this: %s\r\n", argv[1]);
        return 0;
    }

    DumpBuffer(dump_addr, size);

    return 0;
}

static int CrcCommand(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // calc_crc st_addr end_addr

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: crc [start end)\r\n");
        return 0;
    }


    const uint8_t* start_addr;
    const uint8_t* end_addr;
    bool start_add_valid = ParseAddressString(argv[1], &start_addr);
    bool end_add_valid = ParseAddressString(argv[2], &end_addr);

    if(!start_add_valid || !end_add_valid) 
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Please enter valid hex addresses, not this: %s, %s\r\n", argv[1], argv[2]);
        return 0;
    }

    AWS_CrcInit();
    AWS_CrcAddBuffer(start_addr, end_addr - start_addr);
    uint32_t calc_crc = ~AWS_CrcResult();

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "Calculated crc [0x%08x, 0x%08x): 0x%08x\r\n", start_addr, end_addr, calc_crc);

    return 0;
}

static int CommitCommand(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // commit run/new/save <img_flags>

    AWS_BOOT_IMAGE_DCPT copy_dcpt;    
    const AWS_BOOT_IMAGE_DCPT* img_dcpt;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    uint8_t img_flags = AWS_BOOT_FLAG_IMG_VALID; // default value for the image flags
    bool print_usage = false;
    while(true)
    {
        if(argc != 2 && argc != 3)
        {
            print_usage = true;
            break;
        }

        if(strcmp(argv[1], "run") == 0)
        {
            img_dcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_RUN_DCPT_START;
        }
        else if(strcmp(argv[1], "new") == 0)
        {
            img_dcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_NEW_DCPT_START;
        }
#if (AWS_FLASH_SECTIONS == 3)
        else if(strcmp(argv[1], "save") == 0)
        {
            img_dcpt = (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_SAVE_DCPT_START;
        }
#endif // (AWS_FLASH_SECTIONS == 3)
        else
        {
            print_usage = true;
            break;
        }

        if(argc == 3)
        {
            img_flags = (uint8_t)atoi(argv[2]);
        }

        break;
    }

    if(print_usage)
    {
#if (AWS_FLASH_SECTIONS == 3)
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: commit run/new/save\r\n");
#else
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: commit run/new\r\n");
#endif // (AWS_FLASH_SECTIONS == 3)
        return 0;
    }


    copy_dcpt = *img_dcpt;

    uint8_t old_img_flags = copy_dcpt.header.img_flags; // save the old value

    // this should be an image launched in the test run!
    // mark the image as valid
    copy_dcpt.header.img_flags = (uint8_t)img_flags;
    if(AWS_FlashBlockWriteEx((const uint8_t*)img_dcpt, (const uint8_t*)&copy_dcpt, sizeof(copy_dcpt)))
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Committed dcpt: %s, address: 0x%08x\r\n", argv[1], img_dcpt);
        uint8_t new_img_flags = img_dcpt->header.img_flags; // read back 
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Committed flags with: 0x%02x, from: 0x%02x to: 0x%02x\r\n", img_flags, old_img_flags, new_img_flags);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Failed to committed dcpt: %s, address: 0x%08x!\r\n", argv[1], img_dcpt);
    }

    return 0;
}

static void DumpBuffer(const uint8_t* dump_addr, int size)
{

#define DUMP_LINE_SIZE    8   // # of characters per line

    int ix;
    char* pPrint;
    char printBuff[DUMP_LINE_SIZE * 2 + 1];

    printBuff[sizeof(printBuff) - 1] = 0;
    pPrint = printBuff;
    for(ix = 0; ix < size; ix++)
    {
        pPrint += sprintf(pPrint, "%02x", *dump_addr++);
        if(((ix + 1) % DUMP_LINE_SIZE) == 0)
        {   // end line
            *pPrint = 0;
            SYS_CONSOLE_PRINT("%s\r\n", printBuff);
            pPrint = printBuff;
        }
    }
}


// TestCertFlash code
static void TestCertErase(SYS_CMD_DEVICE_NODE* pCmdIO)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const uint32_t* certificatePtr = (const uint32_t*)(PKCS11_CERTIFICATE_SECTION_START_ADDRESS);

    unsigned int step1Res = AWS_FlashRegionProtect(PKCS11_CERTIFICATE_FLASH_MPU_REGION, PKCS11_CERTIFICATE_SECTION_SIZE, certificatePtr, false) ? 0 : 0x01;
    step1Res |= AWS_FlashRegionUnlock(certificatePtr) ? 0 : 0x02;
    step1Res |= AWS_FlashPagesErase(certificatePtr, PKCS11_CERTIFICATE_FLASH_PAGES) ? 0 : 0x04;

    // lock back
    unsigned int step2Res = AWS_FlashRegionLock(certificatePtr) ? 0 : 0x02;
    step2Res |= AWS_FlashRegionProtect(PKCS11_CERTIFICATE_FLASH_MPU_REGION, PKCS11_CERTIFICATE_SECTION_SIZE, certificatePtr, true) ? 0 : 0x01;

    if(step1Res != 0 || step2Res != 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Erase step 1 Failed! - unprotect: %d, unlock: %d, erase: %d\r\n", step1Res & 0x01, step1Res & 0x02, step1Res & 0x04);
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Erase step 2 Failed! - lock: %d, protect: %d\r\n", step2Res & 0x02, step2Res & 0x01);
        return;
    }

    // done; verify
    SCB_CleanDCache();
    const uint32_t* ptrFail = EraseCheck(certificatePtr, PKCS11_CERTIFICATE_SECTION_SIZE / sizeof(uint32_t));
    if(ptrFail != 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Erase: Failed erase verification: 0x%08x!\r\n", ptrFail);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Erase: Success: 0x%08x\r\n", certificatePtr);
    }

}

// checks that an area is empty/erased (only 0xff's)
// should be called with 32 bit aligned address
// returns 0 if fully erased
// otherwise returns a pointer where a different value found
static const uint32_t* EraseCheck(const uint32_t* addr, int size_32)
{
    int ix;

    for(ix = 0; ix < size_32; ix++)
    {
        if(*addr++ != 0xffffffff)
        {
            return addr - 1;
        }
    }

    return 0;
}


static uint32_t certificate_write_buffer[PKCS11_CERTIFICATE_SECTION_SIZE / sizeof(uint32_t)];

static void TestCertWrite(SYS_CMD_DEVICE_NODE* pCmdIO)
{
    int ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const uint32_t* certificatePtr = (const uint32_t*)(PKCS11_CERTIFICATE_SECTION_START_ADDRESS);

    unsigned int step1Res = AWS_FlashRegionProtect(PKCS11_CERTIFICATE_FLASH_MPU_REGION, PKCS11_CERTIFICATE_SECTION_SIZE, certificatePtr, false) ? 0 : 0x01;
    step1Res |= AWS_FlashRegionUnlock(certificatePtr) ? 0 : 0x02;

    if(step1Res != 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Write step 1 Failed! - unprotect: %d, unlock: %d\r\n", step1Res & 0x01, step1Res & 0x02);
        return;
    }

    // fill the page buffer with a random sequence   
    uint32_t* pDst = certificate_write_buffer;
    for(ix = 0; ix < sizeof(certificate_write_buffer) / sizeof(uint32_t); ix ++)
    {
        *pDst++ = rand();
    }
 
    // program the whole certificate buffer
    unsigned int step2Res =  AWS_FlashBlockWriteEx((const uint8_t*)certificatePtr, (uint8_t*)certificate_write_buffer, sizeof(certificate_write_buffer)) ? 0 : 0x04;

    // lock back
    step2Res |= AWS_FlashRegionLock(certificatePtr) ? 0 : 0x02;
    step2Res |= AWS_FlashRegionProtect(PKCS11_CERTIFICATE_FLASH_MPU_REGION, PKCS11_CERTIFICATE_SECTION_SIZE, certificatePtr, true) ? 0 : 0x01;

    if(step2Res != 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Write step 2 Failed! - lock: %d, protect: %d, write: %d\r\n", step2Res & 0x02, step2Res & 0x01, step2Res & 0x04);
        return;
    }

    // now verify the flash contents
    SCB_CleanDCache();
    const uint32_t* pSrc = certificate_write_buffer;
    const uint32_t* ptrFlash = certificatePtr;
    const uint32_t* ptrFail = 0;
    for(ix = 0; ix < sizeof(certificate_write_buffer) / sizeof(uint32_t); ix ++)
    {
        if(*ptrFlash ++ != *pSrc++)
        {
            ptrFail = ptrFlash - 1;
            break;
        }
    }
    
    if(ptrFail == 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Write: Success: 0x%08x!\r\n", certificatePtr);
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Write: Failed verification - address: 0x%08x, expected: 0x%08x, read: 0x%08x\r\n", ptrFail, *(pSrc - 1), *ptrFail);
    }
}


static int TestCertFlash(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // cert erase/write
    //

    const void* cmdIoParam = pCmdIO->cmdIoParam;

    bool toErase = false;
    bool toWrite = false;

    if(argc > 1)
    {
        if(strcmp(argv[1], "erase") == 0)
        {
            toErase = true;
        }
        else if(strcmp(argv[1], "write") == 0)
        {
            toWrite = true;
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: cert <erase/write>\r\n");
            return 0;
        }
    }
    else
    {
        toErase = true;
        toWrite = true;
    }

    if(toErase)
    {
        TestCertErase(pCmdIO);
    }

    if(toWrite)
    {
        TestCertWrite(pCmdIO);
    }



    return 0;
}

static bool ParseAddressString(const char* addr_string, const uint8_t** ppAddr)
{
    int ix;
    union
    {
        uint8_t*    addr;
        uint8_t     addr_buff[4];
    }s_addr;


    int len_addr = strlen(addr_string);
    if(len_addr > 8) 
    {
        return false;
    }

    for(ix = 0; ix < len_addr; ix++)
    {
        if(!isxdigit(addr_string[ix]))
        {
            return false;
        }
    }

    // use a 8 digits address
    char addr_buff[8];
    memset(addr_buff, '0', sizeof(addr_buff));
    memcpy(addr_buff + 8 - len_addr, addr_string, len_addr);

    AsciiHexToBin(addr_buff, s_addr.addr_buff, 8);
    *ppAddr = (const uint8_t*)s_addr.addr;
    
    return true;
}

static void BootPrintDcpt(const AWS_BOOT_IMAGE_DCPT* pAppDcpt, const char* hdr_msg)
{
    char sigBuff[sizeof(pAppDcpt->header.img_sign) + 1];

    const uint32_t* pErase = EraseCheck((const uint32_t*)pAppDcpt, sizeof(*pAppDcpt) / sizeof(uint32_t));
    if(pErase == 0)
    {
        SYS_CONSOLE_PRINT("AWS Dcpt: 0x%08x, %s is fully erased!\r\n", pAppDcpt, hdr_msg);
        return;
    }

    SYS_CONSOLE_PRINT("AWS Dcpt: 0x%08x, %s\r\n", pAppDcpt, hdr_msg);

    strncpy(sigBuff, pAppDcpt->header.img_sign, sizeof(sigBuff) - 1);
    sigBuff[sizeof(sigBuff) - 1] = 0;
    SYS_CONSOLE_PRINT("AWS Dcpt: signature: %s, flags: 0x%02x\r\n", sigBuff, pAppDcpt->header.img_flags);
    SYS_CONSOLE_PRINT("AWS Dcpt: version: %d\r\n", pAppDcpt->version);
    SYS_CONSOLE_PRINT("AWS Dcpt: start add: 0x%08x, end add: 0x%08x, entry add: 0x%08x\r\n", pAppDcpt->start_address, pAppDcpt->end_address, pAppDcpt->entry_address);
    SYS_CONSOLE_PRINT("AWS Dcpt: crc: 0x%08x\r\n", pAppDcpt->crc);

}

#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)


// validates the sanity and integrity of an app image
static bool BootAppValidate(const AWS_BOOT_IMAGE_DCPT* pAppDcpt, uint32_t app_address)
{
    bool validate_sign;
#if  ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    validate_sign = signature_enable;
#else
    validate_sign = true;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
    const char* dcptName;
    if(pAppDcpt == (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_RUN_DCPT_START)
    {
        dcptName = "RunDCPT";
    }
    else if(pAppDcpt == (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_NEW_DCPT_START)
    {
        dcptName = "NewDCPT";
    }
    else if(pAppDcpt == (const AWS_BOOT_IMAGE_DCPT*)AWS_FLASH_SAVE_DCPT_START)
    {
        dcptName = "SaveDCPT";
    }
    else
    {
        dcptName = "unknown DCPT";
    }
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)

    if(validate_sign && memcmp(pAppDcpt->header.img_sign, AWS_BOOT_IMAGE_SIGNATURE, sizeof(pAppDcpt->header.img_sign)) != 0)
    {

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: no valid signature in descr: %s-0x%08x\r\n", dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }
    else if(!validate_sign)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: Skipped Signature validation in descr: %s-0x%08x\r\n", dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
    }

    bool validate_flags;
#if  ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    validate_flags = flags_validate_enable;
#else
    validate_flags = true;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

    // validate the image flags
    uint8_t img_flags = pAppDcpt->header.img_flags;
    if(validate_flags && (img_flags != AWS_BOOT_FLAG_IMG_TEST && img_flags != AWS_BOOT_FLAG_IMG_NEW && img_flags != AWS_BOOT_FLAG_IMG_VALID))
    {   // cannot run the image; probably failed the validation
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: invalid image flags: 0x%02x in descr: %s-0x%08x\r\n", img_flags, dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }
    else if(!validate_flags)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: Skipped validating image flags: 0x%02x in descr: %s-0x%08x\r\n", img_flags, dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
    }

    // validate the addresses
    if(!BootAppAddressValidate(pAppDcpt->start_address))
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: invalid start address: 0x%08x in descr: %s-0x%08x\r\n", pAppDcpt->start_address, dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }

    if(!BootAppAddressValidate(pAppDcpt->end_address))
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: invalid end address: 0x%08x in descr: %s-0x%08x\r\n", pAppDcpt->end_address, dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }

    if(pAppDcpt->entry_address < pAppDcpt->start_address || pAppDcpt->entry_address >= pAppDcpt->end_address)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: invalid entry address: 0x%08x in descr: %s-0x%08x\r\n", pAppDcpt->entry_address, dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }

    // finally calculate the CRC

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    if(crc_enable == false)
    {   // ignore CRC validation
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: Skipped CRC validation in descr: %s-0x%08x\r\n", dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return true;
    }
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

    AWS_CrcInit();
    AWS_CrcAddBuffer((uint8_t*)app_address, pAppDcpt->end_address - pAppDcpt->start_address);
    uint32_t calc_crc = ~AWS_CrcResult();

    if(calc_crc != pAppDcpt->crc)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: invalid CRC in descr: %s-0x%08x, expected: 0x%08x, found: 0x%08x\r\n", dcptName, pAppDcpt, calc_crc, pAppDcpt->crc);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }
    else
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        printf("AWS Validate: Valid CRC found in descr: %s-0x%08x\r\n", dcptName, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
    }


    return true;

}


static bool BootAppAddressValidate(const void* address)
{
    return address >= start_address_limit && address < end_address_limit;
}

//  invalidate the app descriptor
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
static bool BootInvalidateAppDcpt(const AWS_BOOT_IMAGE_DCPT* pAppDcpt)
{
    AWS_BOOT_IMAGE_DCPT emptyDcpt;

    memset(&emptyDcpt, 0xff, sizeof(emptyDcpt));
    if(memcmp(&emptyDcpt, pAppDcpt, sizeof(emptyDcpt)) != 0)
    {   // not all FFs
        memset(&emptyDcpt, 0, sizeof(emptyDcpt));
        if(memcmp(&emptyDcpt, pAppDcpt, sizeof(emptyDcpt)) != 0)
        {   // not all 0's; erase descriptor!
            BootPrintDcpt(pAppDcpt, "before erase");
            if(erase_enable == true)
            {
                if(!AWS_FlashBlockWriteEx((const uint8_t*)pAppDcpt, (const uint8_t*)&emptyDcpt, sizeof(emptyDcpt)))
                {
                    printf("AWS Invalidate: Failed to erase descriptor 0x%08x\r\n", pAppDcpt);
                    return false;
                }
                else
                {
                    printf("AWS Invalidate: Erased descriptor 0x%08x\r\n", pAppDcpt);
                }
                BootPrintDcpt(pAppDcpt, "after erase");
            }
            else
            {
                printf("AWS Invalidate: Skipped erase descriptor 0x%08x\r\n", pAppDcpt);
            }
            return true;
        }
    }

    printf("AWS Invalidate:  Descriptor already erased: 0x%08x\r\n", pAppDcpt);

    return true;
}

#else
static bool BootInvalidateAppDcpt(const AWS_BOOT_IMAGE_DCPT* pAppDcpt)
{
    AWS_BOOT_IMAGE_DCPT emptyDcpt;

    memset(&emptyDcpt, 0xff, sizeof(emptyDcpt));
    if(memcmp(&emptyDcpt, pAppDcpt, sizeof(emptyDcpt)) != 0)
    {   // not all FFs
        memset(&emptyDcpt, 0, sizeof(emptyDcpt));
        if(memcmp(&emptyDcpt, pAppDcpt, sizeof(emptyDcpt)) != 0)
        {   // not all 0's; erase descriptor!
            if(!AWS_FlashBlockWriteEx((const uint8_t*)pAppDcpt, (const uint8_t*)&emptyDcpt, sizeof(emptyDcpt)))
            {
                return false;
            }
        }
    }
    // else either all FFs or all 0s

    return true;
}
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

// launches an application into execution
static void BootLaunchApp(const void* reset_add)
{
    // disable any interrupts    
    strncpy(aws_launch_dcpt.launch_sign, AWS_BOOT_LAUNCH_SIGNATURE, sizeof(aws_launch_dcpt.launch_sign));
    aws_launch_dcpt.launch_vector = reset_add;
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    char sigBuff[sizeof(aws_launch_dcpt.launch_sign) + 1];
    strncpy(sigBuff, aws_launch_dcpt.launch_sign, sizeof(sigBuff) - 1);
    sigBuff[sizeof(sigBuff) - 1] = 0;
    uint32_t tos = *((uint32_t*)reset_add);
    uint32_t launch_address = *((uint32_t*)reset_add + 1);

    printf("AWS Launch Dcpt: 0x%08x\r\n", &aws_launch_dcpt);
    printf("AWS Launch Dcpt: signature: %s\r\n", sigBuff);
    printf("AWS Launch Dcpt: vector: 0x%08x\r\n", aws_launch_dcpt.launch_vector);
    printf("AWS Launch Dcpt: tos: 0x%08x\r\n", tos);
    printf("AWS Launch Dcpt: address: 0x%08x\r\n", launch_address);
#endif // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    run_Application((uint32_t)reset_add);
    SYS_INT_Disable();
    SCB_CleanDCache();
    NVIC_SystemReset();

}


static void run_Application(uint32_t launch_address)
{
    uint32_t msp            = *(uint32_t *)(launch_address);
    uint32_t reset_vector   = *(uint32_t *)(launch_address + 4);

    if (msp == 0xffffffff)
    {
        return;
    }

    __set_MSP(msp);

    asm("bx %0"::"r" (reset_vector));
 
}
