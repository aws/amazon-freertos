/*
 * Amazon FreeRTOS Demo Bootloader V1.4.4
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file aws_boot_pal.c
 * @brief Bootloader platform code.
 */

/* Microchip framework includes. */
#include "system_config.h"
#include "system_definitions.h"
#include "system/devcon/sys_devcon.h"
#include "peripheral/int/plib_int.h"
#include "bsp.h"

/* Bootloader includes. */
#include "aws_boot_types.h"
#include "aws_boot_partition.h"
#include "aws_boot_loader.h"
#include "aws_boot_log.h"


#if !defined( __PIC32MZ__ )
    #error "This bootloader is for a PIC32MZ platform only!"
#endif

/*-----------------------------------------------------------*/

void BOOT_PAL_LaunchApplication( const void * const pvLaunchAddress )
{
    void (* pfApplicationEntry)( void ) = ( void ( * )( void ) )pvLaunchAddress;

    /* Disable any interrupts. */
    PLIB_INT_Disable( INT_ID_0 );

    /* Clear instruction and data caches. */
    SYS_DEVCON_InstructionCacheFlush();
    SYS_DEVCON_DataCacheFlush();

    /* Launch...*/
    ( *pfApplicationEntry )();
}

/*-----------------------------------------------------------*/

void BOOT_PAL_LaunchApplicationDesc( const BOOTImageDescriptor_t * const pvLaunchDescriptor )
{
    DEFINE_BOOT_METHOD_NAME( "BOOT_PAL_LaunchApplicationDesc" );
    
    void * pvExecAddress = pvLaunchDescriptor->pvExecAddress;

    /* The microchip flash is organized in two memory banks and it should
     * be toggled depending on if we are executing from lower or upper bank.
     */
    if( pvLaunchDescriptor >= ( BOOTImageDescriptor_t * ) ( FLASH_DEVICE_BASE + FLASH_PARTITION_OFFSET_IMAGE_1 ) )
    {
        /* Executing from upper bank so toggle.*/
        AWS_NVM_ToggleFlashBanks();
        
        BOOT_LOG_L1( "\n[%s] Memory banks are swapped. \r\n", BOOT_METHOD_NAME );
    }

    BOOT_PAL_LaunchApplication( pvExecAddress );
}

/*-----------------------------------------------------------*/

BaseType_t BOOT_PAL_WatchdogInit( void )
{
    /*. The watchdog scaler is configured in system_init.c
     *  in the DEVCFG1 register.
     */

    return pdTRUE;
}

/*-----------------------------------------------------------*/

BaseType_t BOOT_PAL_WatchdogEnable( void )
{
    BaseType_t xReturn = pdFALSE;

    /* Turn on WDT. */
    WDTCONbits.ON = 1;

    /* Check if its on. */
    if( WDTCONbits.ON )
    {
        xReturn = pdTRUE;
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

BaseType_t BOOT_PAL_WatchdogDisable( void )
{
    BaseType_t xReturn = pdFALSE;

    /* Turn off WDT. */
    WDTCONbits.ON = 0;

    /* Check if its off. */
    if( !WDTCONbits.ON )
    {
        xReturn = pdTRUE;
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

void BOOT_PAL_NotifyBootError( void )
{
    uint32_t ulCntr = 0;
    uint32_t ulFastBlink = 10000000;

    /* Keep flashing LED. */
    for( ; ; )
    {
        /* Fast blink. */
        if( ulCntr++ == ulFastBlink )
        {
            BSP_LEDToggle( BSP_LED_1 );
            ulCntr = 0;
        }
    }
}
