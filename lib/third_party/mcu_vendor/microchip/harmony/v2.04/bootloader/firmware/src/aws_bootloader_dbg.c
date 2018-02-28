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

#include <sys/kmem.h>

#include "system_config.h"
#include "system_definitions.h"

#include "./aws_bootloader.h"
#include "./aws_bootloader_nvm.h"

#include "system/devcon/sys_devcon.h"
#include "peripheral/int/plib_int.h"


// defines

#if !defined(__PIC32MZ__)
#error "This bootloader is for a PIC32MZ platform only!"
#endif

#define AWS_BOOT_CRC_SEED           0xffffffff


#define AWS_BOOT_DEBUG_BASIC        0x01
#define AWS_BOOT_DEBUG_VALIDATE     0x02
#define AWS_BOOT_DEBUG_INVALIDATE   0x04
#define AWS_BOOT_DEBUG_LAUNCH       0x08
#define AWS_BOOT_DEBUG_COMMANDS     0x10

#define AWS_BOOT_DEBUG_MASK         (AWS_BOOT_DEBUG_BASIC | AWS_BOOT_DEBUG_VALIDATE | AWS_BOOT_DEBUG_INVALIDATE | AWS_BOOT_DEBUG_LAUNCH | AWS_BOOT_DEBUG_COMMANDS)
// #define AWS_BOOT_DEBUG_MASK         (0)

// a bootable application descriptor
typedef union
{
    AWS_BOOT_IMAGE_DCPT appDcpt;
    uint8_t             align[AWS_NVM_QUAD_SIZE * 2];   // make it take an integer # of quads
}_AWS_BOOT_ALIGN_DCPT;

typedef enum
{
    AWS_BOOT_STATE_VALIDATION   = 0,    // validation of the app image
    AWS_BOOT_STATE_START_EXEC,          // start launching of the app image into execution
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    AWS_BOOT_STATE_WAIT_EXEC,           // wait for a timeout to pass, until actually launching
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    // ....
    AWS_BOOT_STATE_ERROR,               // launching of the app image into execution
}AWS_BOOT_STATE;


// data

static AWS_BOOT_STATE   aws_boot_state;
static const AWS_BOOT_IMAGE_DCPT* app_img_low_ptr;
static const AWS_BOOT_IMAGE_DCPT* app_img_up_ptr;
static const AWS_BOOT_IMAGE_DCPT* app_img_to_exec;

static uint32_t t_start_exec;               // time when the execution phase started



// CRC calculator
static uint32_t     boot_crc_lfsr;          // the LFSR register

static const uint32_t boot_crc_tbl[256] =       // CRC calculating table for generator polynomial 0x04c11db7
{
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F, 0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B, 0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};


// proto 
// AWS bootloader status function
// returns the new state
typedef AWS_BOOT_STATE(*BootStateFnc)(void); 

static AWS_BOOT_STATE   AWS_State_ValidateImages(void);
static AWS_BOOT_STATE   AWS_State_ExecuteImage(void);
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
static AWS_BOOT_STATE   AWS_State_WaitExecute(void);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
static AWS_BOOT_STATE   AWS_State_Error(void);

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
static int          BootCmdLoader(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdTime(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdSignature(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdCrc(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdRun(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdPrintDcpt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdVersionDcpt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdErase(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdExec(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          BootCmdFlags(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          TestQuad(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          TestCertFlash(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          DumpBuffer(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int          TestCrc(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

#if ((AWS_BOOT_DEBUG_MASK & (AWS_BOOT_DEBUG_COMMANDS | AWS_BOOT_DEBUG_INVALIDATE)) != 0)
static void         BootPrintDcpt(const AWS_BOOT_IMAGE_DCPT* pAppDcpt, const char* hdr_msg);
static bool         ParseAddressString(const char* addr_string, const uint8_t** ppAddr);
static void         AsciiHexToBin(const char* str, uint8_t* buff, int stringLen);
#endif  // ((AWS_BOOT_DEBUG_MASK & (AWS_BOOT_DEBUG_COMMANDS | AWS_BOOT_DEBUG_INVALIDATE)) != 0)

static void         AWS_BootCrcInit(uint32_t seed);
static void         AWS_BootCrcAddBuffer(const uint8_t* buff, int buffLen);
static __inline__ uint32_t AWS_BootCrcResult(void)
{
    return boot_crc_lfsr;
}

static bool         BootAppValidate(const AWS_BOOT_IMAGE_DCPT* pAppDcpt, bool isUpImage);
static bool         BootAppAddressValidate(const void* address);

static bool         BootInvalidateAppDcpt(const AWS_BOOT_IMAGE_DCPT* pAppDcpt);

static void         BootLaunchAppDcpt(const void* launch_add);


// const

// app start address needs to be >= than this limit
static const void* const start_address_limit = (const void*)(__KSEG0_PROGRAM_MEM_BASE + sizeof(AWS_BOOT_IMAGE_DCPT)); 
// app end address needs to be < than this limit
static const void* const end_address_limit = (const void*)(__KSEG0_PROGRAM_MEM_BASE + __KSEG0_PROGRAM_MEM_LENGTH / 2);


static const BootStateFnc   awsStateFncTbl[] = 
{
    AWS_State_ValidateImages,       // AWS_BOOT_STATE_VALIDATION
    AWS_State_ExecuteImage,         // AWS_BOOT_STATE_START_EXEC
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    AWS_State_WaitExecute,          // AWS_BOOT_STATE_WAIT_EXEC
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)

    // last error state
    AWS_State_Error                 // AWS_BOOT_STATE_ERROR
};

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
static bool     signature_enable =  false;      // if signature validation is enabled
static bool     crc_enable =  false;            // if CRC validation is enabled
static bool     run_img_validation = false;     // if image validation to be launched 
static bool     erase_enable =  false;          // if erasing an invalid image is to be enabled
static bool     exec_enable =  false;           // if executing the app image is to be enabled
static const uint8_t* exec_address = 0;         // if execution to be launched at certain address
static bool     flags_validate_enable = false;  // if validation of flags is enabled
static bool     flags_change_enable = false;    // if changing of flags is enabled

static bool     init_failed;                    // run time flag
static int      run_count;                      // run counter

static const SYS_CMD_DESCRIPTOR    bootCmdTbl[] = 
{
        {"loader",   BootCmdLoader,            ": Loader info"},
        {"time",     BootCmdTime,              ": Get current time info"},
        {"sign",     BootCmdSignature,         ": Enable/disable signature"},
        {"crc",      BootCmdCrc,               ": Enable/disable CRC"},
        {"run",      BootCmdRun,               ": Let bootloader run"},
        {"print",    BootCmdPrintDcpt,         ": Print a descriptor"},
        {"ver",      BootCmdVersionDcpt,       ": Print a descriptor version"},
        {"erase",    BootCmdErase,             ": Enable/disable erase"},
        {"exec",     BootCmdExec,              ": Enable/disable app execution"},
        {"flags",    BootCmdFlags,             ": Set the run flags"},
        // various tests and helpers
        {"test",     TestQuad,                 ": test quad"},
        {"dump",     DumpBuffer,               ": dump buffer"},
        {"cert",     TestCertFlash,            ": test cert flash"},
        {"testcrc",  TestCrc,                   ": test CRC"},
};
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)


bool AWS_Bootloader_Initialize(void)
{
    bool iniFail;
    // use uncached addresses to read directly from the program flash memory
    app_img_low_ptr = (const AWS_BOOT_IMAGE_DCPT*)KVA0_TO_KVA1(__KSEG0_PROGRAM_MEM_BASE);
    app_img_up_ptr = (const AWS_BOOT_IMAGE_DCPT*)KVA0_TO_KVA1(__KSEG0_PROGRAM_MEM_BASE + __KSEG0_PROGRAM_MEM_LENGTH / 2);
    app_img_to_exec = 0;

    while(true)
    {
        // check that you have a integer number of quads
        if(sizeof(AWS_BOOT_IMAGE_DCPT) != sizeof(((_AWS_BOOT_ALIGN_DCPT*)0)->align))
        {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_BASIC) != 0)
            SYS_ERROR(SYS_ERROR_ERROR, "AWS Boot: Build size mismatch. Failed to initialize!\r\n");
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_BASIC) != 0)
            iniFail = true;
            break;
        }

        // create command group
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
        if(!SYS_CMD_ADDGRP(bootCmdTbl, sizeof(bootCmdTbl)/sizeof(*bootCmdTbl), "boot", ": commands"))
        {
            SYS_ERROR(SYS_ERROR_ERROR, "AWS Boot: No Boot commands. Failed to initialize!\r\n");
            iniFail = true;
            break;
        }
        SYS_CONSOLE_MESSAGE("AWS Boot: Started. Waiting for a Boot command....\r\n");
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
    bool res;

    // basic sanity check
    if(aws_boot_state >= sizeof(awsStateFncTbl) / sizeof(*awsStateFncTbl))
    {
        aws_boot_state = AWS_BOOT_STATE_ERROR;
    }

    // dispatch
    aws_boot_state = (*awsStateFncTbl[aws_boot_state])();

}


static AWS_BOOT_STATE AWS_State_ValidateImages(void)
{
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    if(run_img_validation == false)
    {   // wait for command
        return AWS_BOOT_STATE_VALIDATION;
    }
    run_img_validation = false;

    if(exec_address != 0)
    {   // forced address
        const AWS_BOOT_IMAGE_DCPT* exec_img = (const AWS_BOOT_IMAGE_DCPT*)KVA0_TO_KVA1(exec_address);
        // not really important, but needs to be != 0
        app_img_to_exec = exec_img < app_img_up_ptr ? app_img_low_ptr : app_img_up_ptr;
        return AWS_BOOT_STATE_START_EXEC;
    }

#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

    bool isLowerValid = BootAppValidate(app_img_low_ptr, false);
    bool isUpperValid = BootAppValidate(app_img_up_ptr, true);

    if(isLowerValid == true && isUpperValid == true)
    {   // both images valid
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_MESSAGE("AWS Validate: valid signatures found in lower and upper\r\n");
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        // launch the more recent one
        app_img_to_exec = (app_img_low_ptr->version >= app_img_up_ptr->version) ? app_img_low_ptr : app_img_up_ptr;
    }
    else if(isLowerValid == true)
    {   
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_MESSAGE("AWS Validate: valid signature found in lower\r\n");
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        BootInvalidateAppDcpt(app_img_up_ptr);
        app_img_to_exec = app_img_low_ptr;
    }
    else if(isUpperValid == true)
    {   
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_MESSAGE("AWS Validate: valid signature found in upper\r\n");
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        BootInvalidateAppDcpt(app_img_low_ptr);
        app_img_to_exec = app_img_up_ptr;
    }
    else
    {   // both images are hosen!
        BootInvalidateAppDcpt(app_img_low_ptr);
        BootInvalidateAppDcpt(app_img_up_ptr);
        app_img_to_exec = 0;
    }

    if(app_img_to_exec != 0)
    {   // ok, we found an executable image
        return AWS_BOOT_STATE_START_EXEC;
    }

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    // stay in this state so we can reissue the "run command"
    return AWS_BOOT_STATE_VALIDATION;
#else
    return AWS_BOOT_STATE_ERROR;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
        
}

static AWS_BOOT_STATE AWS_State_ExecuteImage(void)
{
    bool changeFlags;

#if  ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    changeFlags = flags_change_enable;
#else
    changeFlags = true;    
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

    uint8_t header_flags = app_img_to_exec->header.img_flags;

    if(changeFlags && header_flags == AWS_BOOT_FLAG_IMG_NEW)
    {   // first launch of this app
        AWS_BOOT_IMAGE_DCPT copy_dcpt = *app_img_to_exec;
        copy_dcpt.header.img_flags = AWS_BOOT_FLAG_IMG_TEST;

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
        SYS_CONSOLE_PRINT("AWS Launch: Updating the flags: 0x%02x, to: 0x%02x, dcpt: 0x%08x\r\n", header_flags, copy_dcpt.header.img_flags, app_img_to_exec);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
       
        if(!AWS_NVM_QuadWordWrite(app_img_to_exec->header.align, copy_dcpt.header.align, sizeof(copy_dcpt) / AWS_NVM_QUAD_SIZE))
        {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
            SYS_CONSOLE_PRINT("AWS Launch: Failed to update the run flags: 0x%08x\r\n", app_img_to_exec);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
        }
        else
        {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
            SYS_CONSOLE_PRINT("AWS Launch: Updated the run flags: 0x%08x\r\n", app_img_to_exec);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
        }
    }
    else
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
        SYS_CONSOLE_PRINT("AWS Launch: Skipped updating the flags: 0x%02x, dcpt: 0x%08x\r\n", header_flags, app_img_to_exec);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    }


#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    if(app_img_to_exec != app_img_low_ptr)
    {
        SYS_CONSOLE_PRINT("AWS Launch:  Map PFM Up to Low: 0x%08x\r\n", app_img_to_exec);
    }
    else
    {
        SYS_CONSOLE_PRINT("AWS Launch:  No Map PFM needed, running from Low: 0x%08x\r\n", app_img_to_exec);
    }
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)

    if(app_img_to_exec != app_img_low_ptr)
    {   // we normally run from the lower panel
        AWS_NVM_ToggleFlashBanks(); 
        app_img_to_exec = app_img_low_ptr;
    }


#if  ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    const void* app_launch = exec_address == 0 ? (const void*)app_img_to_exec->entry_address : exec_address;

    if(exec_enable == false)
    {
        SYS_CONSOLE_PRINT("AWS Launch: skip start app at: 0x%08x\r\n", app_launch);
        // we'll relaunch again
        return AWS_BOOT_STATE_VALIDATION;
    }
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
    SYS_CONSOLE_PRINT("AWS Launch:  wait for app at: 0x%08x\r\n", app_launch);
    t_start_exec = SYS_TMR_TickCountGet();
    return AWS_BOOT_STATE_WAIT_EXEC;
#else
    BootLaunchAppDcpt(app_img_to_exec->entry_address);
    return AWS_BOOT_STATE_VALIDATION;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)

}

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)
static AWS_BOOT_STATE AWS_State_WaitExecute(void)
{
    // wait 2s for all the messages to go out
    uint32_t tCurr = SYS_TMR_TickCountGet();
    if((tCurr - t_start_exec) < 2 * SYS_TMR_TickCounterFrequencyGet())
    {
        return AWS_BOOT_STATE_WAIT_EXEC; 
    }

    // now launch
    const void* app_launch = exec_address == 0 ? (const void*)app_img_to_exec->entry_address : exec_address;
    exec_address = 0;
    BootLaunchAppDcpt(app_launch);
    return AWS_BOOT_STATE_VALIDATION;
}
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_LAUNCH) != 0)

static AWS_BOOT_STATE AWS_State_Error(void)
{
    return AWS_BOOT_STATE_ERROR;
}

// working routines

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

static int BootCmdSignature(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // sign 0 /1
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: sign <0/1>\r\n");
        return 0;
    }

    bool enableSign = signature_enable;

    if(argc > 1)
    {
        enableSign = atoi(argv[1]) != 0;
    }

    signature_enable = enableSign;

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "crc validation set to: %d\r\n", enableSign);
    return 0;
}

static int BootCmdCrc(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // crc 0 /1
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: crc <0/1>\r\n");
        return 0;
    }

    bool enableCrc = crc_enable;

    if(argc > 1)
    {
        enableCrc = atoi(argv[1]) != 0;
    }

    crc_enable = enableCrc;

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "crc validation set to: %d\r\n", enableCrc);
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

        exec_address = start_add;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Will run the bootloader...count: %d, exec address: 0x%08x\r\n", run_count++, exec_address);
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
    // print low/up
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    bool print_usage = false;

    while(true)
    {
        if(argc != 2)
        {
            print_usage = true;
            break;
        }

        if(strcmp(argv[1], "low") == 0)
        {
            BootPrintDcpt(app_img_low_ptr, "lower PFM");
        }
        else if(strcmp(argv[1], "up") == 0)
        {
            BootPrintDcpt(app_img_up_ptr, "upper PFM");
        }
        else
        {
            print_usage = true;
        }

        break;
    }

    if(print_usage)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: print low/up\r\n");
    }
    return 0;
}

static int BootCmdVersionDcpt(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // ver low/up
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    bool print_usage = false;

    const AWS_BOOT_IMAGE_DCPT* pDcpt = 0;

    while(true)
    {
        if(argc != 2)
        {
            print_usage = true;
            break;
        }

        if(strcmp(argv[1], "low") == 0)
        {
            pDcpt = app_img_low_ptr;
        }
        else if(strcmp(argv[1], "up") == 0)
        {
            pDcpt = app_img_up_ptr;
        }
        else
        {
            print_usage = true;
        }

        break;
    }

    if(print_usage)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: ver low/up\r\n");
    }

    if(pDcpt)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "ver %s is: 0x%08x\r\n", argv[1], pDcpt->version);
        bool bankSwap = AWS_FlashProgramIsSwapped();
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Flash bank 1 is now: %s\r\n", bankSwap ? "upper" : "lower");
    }

    return 0;
}

static int BootCmdErase(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // erase 0 /1
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: erase <0/1>\r\n");
        return 0;
    }

    bool enableErase = erase_enable;

    if(argc > 1)
    {
        enableErase = atoi(argv[1]) != 0;
    }

    erase_enable = enableErase;

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "erase image set to: %d\r\n", enableErase);
    return 0;
}

static int BootCmdExec(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // exec 0 /1
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc > 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: exec <0/1>\r\n");
        return 0;
    }

    bool enableExec = exec_enable;

    if(argc > 1)
    {
        enableExec = atoi(argv[1]) != 0;
    }

    exec_enable = enableExec;

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "exec app set to: %d\r\n", enableExec);
    return 0;
}

static int BootCmdFlags(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // flags validate/change/run 0/1/2
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc != 1 && argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: flags <validate/change/run 0/1/2>\r\n");
        return 0;
    }

    bool enableValidate = flags_validate_enable;
    bool enableChange = flags_change_enable;
    bool setRun = false;
    int run_flags = 0;

    if(argc > 1)
    {
        if(strcmp(argv[1], "validate") == 0)
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
    }

    if(!setRun)
    {
        flags_validate_enable = enableValidate;
        flags_change_enable = enableChange;

        (*pCmdIO->pCmdApi->print)(cmdIoParam, "flags validate: %d, change: %d\r\n", enableValidate, enableChange);
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

void UART_Write(const char* str)
{
    unsigned char c;

    while((c = *str++))
    {
        while(U1STAbits.UTXBF == 1);
        U1TXREG = c;
    }
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



static int TestQuad(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // test flash_address data_address size
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc != 4)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: test flash_add data_add size\r\n");
        return 0;
    }

    int size = atoi(argv[3]);

    bool flash_add_valid, data_add_valid;
    const uint8_t* flash_addr, *data_addr;

    flash_add_valid = ParseAddressString(argv[1], &flash_addr);
    data_add_valid = ParseAddressString(argv[2], &data_addr);


    if(!flash_add_valid || !data_add_valid) 
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Please enter valid hex addresses, not this: %s, %s\r\n", argv[1], argv[2]);
        return 0;
    }

    AWS_FlashProgramBlock(flash_addr, data_addr, size);

    return 0;

}

static int DumpBuffer(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // dump address size
    // ex: dump yyyyyyyy 20 

#define DUMP_LINE_SIZE    8   // # of characters per line

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


    int ix;
    const uint8_t* pSrc;
    char* pPrint;
    char printBuff[DUMP_LINE_SIZE * 2 + 1];

    printBuff[sizeof(printBuff) - 1] = 0;
    pSrc = dump_addr;
    pPrint = printBuff;
    for(ix = 0; ix < size; ix++)
    {
        pPrint += sprintf(pPrint, "%02x", *pSrc++);
        if(((ix + 1) % DUMP_LINE_SIZE) == 0)
        {   // end line
            *pPrint = 0;
            SYS_CONSOLE_PRINT("%s\r\n", printBuff);
            pPrint = printBuff;
        }
    }

    return 0;
}


// TestCertFlash code

#define PKCS11_CERTIFICATE_SECTION_START_ADDRESS    (__UPPERBOOTALIASLASTPAGE_BASE)    
#define PKCS11_CERTIFICATE_SECTION_SIZE             (__UPPERBOOTALIASLASTPAGE_LENGTH)    

#include "peripheral/nvm/plib_nvm.h"


static uint32_t page_buffer[PKCS11_CERTIFICATE_SECTION_SIZE / sizeof(uint32_t)];


static void TestCertErase(SYS_CMD_DEVICE_NODE* pCmdIO)
{
    int ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const uint32_t* upperPtr = (const uint32_t*)KVA0_TO_KVA1(PKCS11_CERTIFICATE_SECTION_START_ADDRESS);

    AWS_UpperBootPageProtectionDisable();
    
    if(AWS_UpperBootPageIsProtected())
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Cert Erase: Failed to remove protection!\r\n");
        return;
    }

    if(!AWS_UpperBootPageErase(upperPtr))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Cert Erase: Failed to erase the flash!\r\n");
        return;
    }


    const uint32_t* ptrFail = 0;
    const uint32_t* ptrFlash = upperPtr;
    for(ix = 0; ix < PKCS11_CERTIFICATE_SECTION_SIZE / sizeof(uint32_t); ix++)
    {
        if(*ptrFlash++ != 0xffffffff)
        {
            ptrFail = ptrFlash - 1;
            break;
        }
    }

    if(ptrFail != 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Erase: Failed erase verification: 0x%08x!\r\n", ptrFail);
        return;
    }

    AWS_UpperBootPageProtectionEnable();
    
    if(!AWS_UpperBootPageIsProtected())
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Cert Erase: Failed to restore protection!\r\n");
        return;
    }

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Cert Erase: Success\r\n");

}

static void TestCertWrite(SYS_CMD_DEVICE_NODE* pCmdIO)
{
    int ix;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    const uint32_t* upperPtr = (const uint32_t*)KVA0_TO_KVA1(PKCS11_CERTIFICATE_SECTION_START_ADDRESS);

    AWS_UpperBootPageProtectionDisable();
    
    if(AWS_UpperBootPageIsProtected())
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Cert Program: Failed to remove protection!\r\n");
        return;
    }

    uint32_t* progr_buffer = (uint32_t*)KVA0_TO_KVA1(page_buffer);

    // fill the page buffer with a random sequence   
    uint32_t* pDst = progr_buffer;
    for(ix = 0; ix < sizeof(page_buffer) / sizeof(uint32_t); ix ++)
    {
        *pDst++ = rand();
    }
 

    // program the whole page
    int rowFail = -1;
    uint32_t* pSrc = progr_buffer;
    const uint32_t* ptrFlash = upperPtr;
    int nRows = PKCS11_CERTIFICATE_SECTION_SIZE / NVM_ROW_SIZE;

    for(ix = 0; ix < nRows; ix ++)
    {
        if(!AWS_UpperBootWriteRow(ptrFlash, pSrc))
        {
            rowFail = ix;
            break;
        }

        // advance the pointers
        pSrc += NVM_ROW_SIZE / sizeof(uint32_t);
        ptrFlash += NVM_ROW_SIZE / sizeof(uint32_t);
    }

    AWS_UpperBootPageProtectionEnable();

    if(rowFail >= 0)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Program: Failed programming, row: %d!\r\n", rowFail);
        return;
    }

    if(!AWS_UpperBootPageIsProtected())
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Cert Program: Failed to restore protection!\r\n");
        return;
    }

    // now verify the flash contents
    pSrc = progr_buffer;
    ptrFlash = upperPtr;
    const uint32_t* ptrFail = 0;
    for(ix = 0; ix < sizeof(page_buffer) / sizeof(uint32_t); ix ++)
    {
        if(*ptrFlash ++ != *pSrc++)
        {
            ptrFail = ptrFlash - 1;
            break;
        }
    }
    
    if(ptrFail == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Cert Program: Success!\r\n");
    }
    else
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Cert Program: Failed verification - address: 0x%08x, expected: 0x%08x, read: 0x%08x\r\n", ptrFail, *(pSrc - 1), *ptrFail);
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
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: cert <erase/program>\r\n");
            return;
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

static int TestCrc(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // testcrc start_add end_add
    //

    const void* cmdIoParam = pCmdIO->cmdIoParam;


    if(argc < 3)
    {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: testcrc start end\r\n");
            return 0;
    }

    const uint8_t* start_addr;
    bool start_add_valid = ParseAddressString(argv[1], &start_addr);
    const uint8_t* end_addr;
    bool end_add_valid = ParseAddressString(argv[2], &end_addr);

    if(!start_add_valid || !end_add_valid) 
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Please enter valid hex addresses, not this: %s\r\n", argv[1], argv[2]);
        return 0;
    }
   
    uint32_t nBytes =  end_addr - start_addr;
    AWS_BootCrcInit(AWS_BOOT_CRC_SEED);
    AWS_BootCrcAddBuffer(start_addr, nBytes);
    uint32_t calc_crc = AWS_BootCrcResult();

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "CRC over %d bytes: 0x%08x\r\n", nBytes, calc_crc);

    return 0;


}
static bool ParseAddressString(const char* addr_string, const uint8_t** ppAddr)
{
    int ix;
    union
    {
        uint8_t*    addr;
        char        addr_buff[4];
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


#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)


static void AWS_BootCrcInit(uint32_t seed)
{
    boot_crc_lfsr = seed;
}

static void AWS_BootCrcAddBuffer(const uint8_t* buff, int buffLen)
{
    while(buffLen--)
    {
        boot_crc_lfsr = (boot_crc_lfsr >> 8) ^ boot_crc_tbl[(boot_crc_lfsr & 0xff) ^ *buff++];
    }
}

// validates the sanity and integrity of an app image
static bool BootAppValidate(const AWS_BOOT_IMAGE_DCPT* pAppDcpt, bool isUpImage)
{
    bool validate_sign;
#if  ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    validate_sign = signature_enable;
#else
    validate_sign = true;
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)


    if(validate_sign && memcmp(pAppDcpt->header.img_sign, AWS_BOOT_IMAGE_SIGNATURE, sizeof(pAppDcpt->header.img_sign)) != 0)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: no valid signature in descr: 0x%08x\r\n", pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }
    else if(!validate_sign)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: Skipped Signature validation in descr: 0x%08x\r\n", pAppDcpt);
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
    if(validate_flags && (img_flags != AWS_BOOT_FLAG_IMG_NEW && img_flags != AWS_BOOT_FLAG_IMG_VALID))
    {   // cannot run the image; probably failed the validation
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: invalid image flags: 0x%02x in descr: 0x%08x\r\n", img_flags, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }
    else if(!validate_flags)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: Skipped validating image flags: 0x%02x in descr: 0x%08x\r\n", img_flags, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
    }

    // validate the addresses
    if(!BootAppAddressValidate(pAppDcpt->start_address))
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: invalid start address: 0x%08x in descr: 0x%08x\r\n", pAppDcpt->start_address, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }

    if(!BootAppAddressValidate(pAppDcpt->end_address))
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: invalid end address: 0x%08x in descr: 0x%08x\r\n", pAppDcpt->end_address, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }

    if(pAppDcpt->entry_address < pAppDcpt->start_address || pAppDcpt->entry_address >= pAppDcpt->end_address)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: invalid entry address: 0x%08x in descr: 0x%08x\r\n", pAppDcpt->entry_address, pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }

    // finally calculate the CRC

#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)
    if(crc_enable == false)
    {   // ignore CRC validation
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: Skipped CRC validation in descr: 0x%08x\r\n", pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return true;
    }
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_COMMANDS) != 0)

    const uint8_t* flash_start;
    AWS_BootCrcInit(AWS_BOOT_CRC_SEED);
    if(isUpImage)
    {
        flash_start = (const uint8_t*)KVA0_TO_KVA1(pAppDcpt->start_address + __KSEG0_PROGRAM_MEM_LENGTH / 2);
    }
    else
    {
        flash_start = (const uint8_t*)KVA0_TO_KVA1(pAppDcpt->start_address);
    }

    AWS_BootCrcAddBuffer(flash_start, pAppDcpt->end_address - pAppDcpt->start_address);
    uint32_t calc_crc = AWS_BootCrcResult();

    if(calc_crc != pAppDcpt->crc)
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: invalid CRC in descr: 0x%08x, expected: 0x%08x, found: 0x%08x\r\n", pAppDcpt, calc_crc, pAppDcpt->crc);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        return false;
    }
    else
    {
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("AWS Validate: Valid CRC found in descr: 0x%08x\r\n", pAppDcpt);
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_VALIDATE) != 0)
    }


    return true;

}


static bool BootAppAddressValidate(const void* address)
{
    return address >= start_address_limit && address < end_address_limit;
}

//  invalidate the app descriptor
#if ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_INVALIDATE) != 0)
static void BootPrintDcpt(const AWS_BOOT_IMAGE_DCPT* pAppDcpt, const char* hdr_msg)
{
#define BOOT_PRINT_LINE_SIZE    8   // characters per line
    int ix;
    const uint8_t* pSrc;
    char* pPrint;
    char printBuff[(sizeof(*pAppDcpt) / BOOT_PRINT_LINE_SIZE) * 2 + 1];

    SYS_CONSOLE_PRINT("AWS Dcpt: 0x%08x, %s\r\n", pAppDcpt, hdr_msg);

    printBuff[sizeof(printBuff) - 1] = 0;
    pSrc = (const uint8_t*)pAppDcpt;
    pPrint = printBuff;
    for(ix = 0; ix < sizeof(*pAppDcpt); ix++)
    {
        pPrint += sprintf(pPrint, "%02x", *pSrc++);
        if(((ix + 1) % BOOT_PRINT_LINE_SIZE) == 0)
        {   // end line
            *pPrint = 0;
            SYS_CONSOLE_PRINT("AWS Dcpt: data: %s\r\n", printBuff);
            pPrint = printBuff;
        }
    }
}

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
                if(!AWS_NVM_QuadWordWrite(pAppDcpt->header.align, emptyDcpt.header.align, sizeof(emptyDcpt) / AWS_NVM_QUAD_SIZE))
                {
                    SYS_CONSOLE_PRINT("AWS Invalidate: Failed to erase descriptor 0x%08x\r\n", pAppDcpt);
                    return false;
                }
                else
                {
                    SYS_CONSOLE_PRINT("AWS Invalidate: Erased descriptor 0x%08x\r\n", pAppDcpt);
                }
                BootPrintDcpt(pAppDcpt, "after erase");
            }
            else
            {
                SYS_CONSOLE_PRINT("AWS Invalidate: Skipped erase descriptor 0x%08x\r\n", pAppDcpt);
            }
            return true;
        }
    }

    SYS_CONSOLE_PRINT("AWS Invalidate:  Descriptor already erased: 0x%08x\r\n", pAppDcpt);

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
            if(!AWS_NVM_QuadWordWrite(pAppDcpt->header.align, emptyDcpt.header.align, sizeof(emptyDcpt) / AWS_NVM_QUAD_SIZE))
            {
                return false;
            }
        }
    }
    // else either all FFs or all 0s

    return true;
}
#endif  // ((AWS_BOOT_DEBUG_MASK & AWS_BOOT_DEBUG_INVALIDATE) != 0)

// launches an application into execution
// swaps the flash panels if needed
static void BootLaunchAppDcpt(const void* launch_add)
{
    void (*app_entry)(void) = (void(*)(void))launch_add;

    // disable any interrupts    
    PLIB_INT_Disable(INT_ID_0);
    SYS_DEVCON_InstructionCacheFlush();
    SYS_DEVCON_DataCacheFlush();

    // and launch...
    (*app_entry)();

}


#if 0
// functions used to calculate the boot_crc_tbl
// for generator polynomial 0x04c11db7

static void BootCrcCalculateTable(uint32_t generator)
{
    int ix, jx;
    
    for(ix = 0; ix < sizeof(boot_crc_tbl) / sizeof(*boot_crc_tbl); ix++)
    {
        boot_crc_tbl[ix] = BootCrcReverse(ix, 8) << 24;
        for(jx = 0; jx < 8; jx++)
        {
            boot_crc_tbl[ix] = (boot_crc_tbl[ix] << 1) ^ (boot_crc_tbl[ix] & (1 << 31) ? generator : 0);
        }

        boot_crc_tbl[ix] = BootCrcReverse(boot_crc_tbl[ix], 32);
    }
}


static uint32_t BootCrcReverse(uint32_t value, uint8_t rounds)
{
    // mirror reverse bits
    int ix;
    uint32_t mirror_val=0;

    for(ix = 1; ix < rounds + 1; ix++)
    {
        if((value & 1) != 0)
        {
            mirror_val |= 1 << (rounds - ix);
        }

        value >>= 1;
    }

    return mirror_val;

}

#endif

