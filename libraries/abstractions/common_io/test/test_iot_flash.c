/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*******************************************************************************
 * @file iot_test_flash.c
 * @brief Functional Unit Test - FLASH
 *******************************************************************************
 */

#include "unity.h"
#include "unity_fixture.h"
#include "iot_flash.h"
#include "iot_timer.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Default byte value in flash. */
#define testIotFLASH_DEFAULT_FLASH_BYTE           ( 0xFF )
/* Assuming that pagesize is greater than 32B */
#define testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE    ( 0x20 )
/* Use 512 bytes max */
#define testIotFLASH_DEFAULT_MAX_BUFFER_SIZE      ( 0x200 )

#define testIotFLASH_DEFAULT_DELAY_US             ( 100 )
#define testIotFLASH_INVALID_HANDLE               ( -1 )

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
extern int32_t ltestIotTimerInstance;

uint8_t ltestIotFlashInstance = 0; /** Default Flash instance for testing */

/* This offset is used for testing the flash test-cases
 * The data from offset to offset + 2 sectors will be corrupted */
uint32_t ultestIotFlashStartOffset = 0;

/*-----------------------------------------------------------*/
/** Static globals */
/*-----------------------------------------------------------*/
static uint8_t uctestIotFlashWriteBuffer[ testIotFLASH_DEFAULT_MAX_BUFFER_SIZE ];
static uint8_t uctestIotFlashReadBuffer[ testIotFLASH_DEFAULT_MAX_BUFFER_SIZE ];

static SemaphoreHandle_t xtestIotFlashSemaphore = NULL;
static SemaphoreHandle_t xtestIotFlashTimerSemaphore = NULL;

static const uint8_t uctestIotFlashWriteROBuffer[ 512 ] = { 0xAA };

/* Define Test Group. */
TEST_GROUP( TEST_IOT_FLASH );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_FLASH )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_FLASH )
{
}

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
static void prvIotFlashEraseCallback( IotFlashOperationStatus_t pxStatus,
                                      void * pvUserContext )
{
    BaseType_t xHigherPriorityTaskWoken;

    xSemaphoreGiveFromISR( xtestIotFlashSemaphore, &xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/
static void prvIotTimerCallback( void * pvUserContext )
{
    int32_t lRetVal;

    BaseType_t xHigherPriorityTaskWoken;

    /* Suspend the flash write/erase */
    lRetVal = iot_flash_ioctl( ( IotFlashHandle_t ) pvUserContext,
                               eSuspendFlashProgramErase,
                               NULL );

    /* Check if suspend/resume is supported */
    if( lRetVal != IOT_FLASH_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
    }

    xSemaphoreGiveFromISR( xtestIotFlashTimerSemaphore, &xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/

static void prvIotFlashWriteDummyData( IotFlashHandle_t xFlashHandle,
                                       uint32_t offset,
                                       uint32_t size )
{
    uint32_t lRetVal;
    uint32_t i;
    uint32_t j;

    /* Fill out a buffer of pageSize to be written */
    for( i = 0; i < size; i += testIotFLASH_DEFAULT_MAX_BUFFER_SIZE )
    {
        /* Less the a full buffer left? */
        uint32_t remaining_size = ( ( i + testIotFLASH_DEFAULT_MAX_BUFFER_SIZE ) > size ) ? ( size - i ) : testIotFLASH_DEFAULT_MAX_BUFFER_SIZE;

        if( remaining_size > 0 )
        {
            for( j = 0; j < remaining_size; j++ )
            {
                uctestIotFlashWriteBuffer[ j ] = j;
            }

            /* Write full page of data */
            lRetVal = iot_flash_write_sync( xFlashHandle,
                                            offset + i,
                                            &uctestIotFlashWriteBuffer[ 0 ],
                                            remaining_size );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
        }
    }
}

/*-----------------------------------------------------------*/

static void prvIotFlashReadVerifyDummyData( IotFlashHandle_t xFlashHandle,
                                            uint32_t offset,
                                            uint32_t size )
{
    uint32_t lRetVal;
    uint32_t i;
    uint32_t j;

    for( i = 0; i < size; i += testIotFLASH_DEFAULT_MAX_BUFFER_SIZE )
    {
        /* Less the a full buffer left? */
        uint32_t remaining_size = ( ( i + testIotFLASH_DEFAULT_MAX_BUFFER_SIZE ) > size ) ? ( size - i ) : testIotFLASH_DEFAULT_MAX_BUFFER_SIZE;

        if( remaining_size > 0 )
        {
            /* Read back the data written */
            lRetVal = iot_flash_read_sync( xFlashHandle,
                                           offset + i,
                                           &uctestIotFlashReadBuffer[ 0 ],
                                           remaining_size );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Verify that  the data was written. */
            for( j = 0; j < remaining_size; j++ )
            {
                if( uctestIotFlashReadBuffer[ j ] != ( j & 0xFF ) )
                {
                    TEST_ASSERT_MESSAGE( 0, "Data was NOT written" );
                }
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void prvIotFlashReadVerifyErased( IotFlashHandle_t xFlashHandle,
                                         uint32_t offset,
                                         uint32_t size )
{
    uint32_t lRetVal;
    uint32_t i;
    uint32_t j;

    for( i = 0; i < size; i += testIotFLASH_DEFAULT_MAX_BUFFER_SIZE )
    {
        /* Less the a full buffer left? */
        uint32_t remaining_size = ( ( i + testIotFLASH_DEFAULT_MAX_BUFFER_SIZE ) > size ) ? ( size - i ) : testIotFLASH_DEFAULT_MAX_BUFFER_SIZE;

        if( remaining_size > 0 )
        {
            /* Read the data */
            lRetVal = iot_flash_read_sync( xFlashHandle,
                                           offset + i,
                                           &uctestIotFlashReadBuffer[ 0 ],
                                           remaining_size );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Verify that the data was erased. */
            for( j = 0; j < remaining_size; j++ )
            {
                if( uctestIotFlashReadBuffer[ j ] != testIotFLASH_DEFAULT_FLASH_BYTE )
                {
                    TEST_ASSERT_MESSAGE( 0, "Data was NOT erased" );
                }
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void prvIotFlashWriteReadVerify( IotFlashHandle_t xFlashHandle,
                                        uint32_t offset,
                                        uint32_t size )
{
    /* Fill out a buffer of pageSize to be written */
    prvIotFlashWriteDummyData( xFlashHandle, offset, size );

    /* Read back the data written */
    prvIotFlashReadVerifyDummyData( xFlashHandle, offset, size );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_FLASH )
{
    xtestIotFlashSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotFlashSemaphore );

    xtestIotFlashTimerSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotFlashTimerSemaphore );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashOpenClose );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashReadInfo );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseSector );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseMultipleSectors );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseFlashBlocks );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseFlashBlocksUnAlignedAddress );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseFlashBlocksUnAlignedSize );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseUnAlignedAddress );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseUnAlignedSize );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWritePartialPage );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWritePage );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWriteSector );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWriteAcrossSectors );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseInProgressRead );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseInProgressWrite );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWriteProtectWriteFailure );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWriteProtectEraseFailure );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWriteEraseReadCycle );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWritePageFromFlash );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashOpenCloseFuzz );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashGetInfoFuzz );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashIoctlFuzz );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWriteFuzz );
    RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashReadFuzz );

    /* TODO - These tests require suspend/resume functionality to be enabled
     * which requires int handlers to be moved to RAM
     */
    #if 0
        RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashWriteSuspendResume );
        RUN_TEST_CASE( TEST_IOT_FLASH, AFQP_IotFlashEraseSuspendResume );
    #endif
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_flash_open and iot_flash_close.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashOpenClose )
{
    IotFlashHandle_t xFlashHandle;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_flash_open, read the flash_info
 * validate the flash_info and iot_flash_close.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashReadInfo )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /*
         * Check the flash size, block size, sector size and page size
         */
        TEST_ASSERT_NOT_EQUAL( 0, pxFlashInfo->ulPageSize );
        TEST_ASSERT_GREATER_OR_EQUAL( pxFlashInfo->ulPageSize, pxFlashInfo->ulSectorSize );
        TEST_ASSERT_GREATER_OR_EQUAL( pxFlashInfo->ulSectorSize, pxFlashInfo->ulBlockSize );
        TEST_ASSERT_GREATER_OR_EQUAL( pxFlashInfo->ulBlockSize, pxFlashInfo->ulFlashSize );

        /* Make sure the flash size, block size, sector size and page size are the power of 2 */
        TEST_ASSERT_EQUAL( 0, pxFlashInfo->ulPageSize & ( pxFlashInfo->ulPageSize - 1 ) );
        TEST_ASSERT_EQUAL( 0, pxFlashInfo->ulSectorSize & ( pxFlashInfo->ulSectorSize - 1 ) );
        TEST_ASSERT_EQUAL( 0, pxFlashInfo->ulBlockSize & ( pxFlashInfo->ulBlockSize - 1 ) );
        TEST_ASSERT_EQUAL( 0, pxFlashInfo->ulFlashSize & ( pxFlashInfo->ulFlashSize - 1 ) );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}


/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test one sector erase and verify the contents are erased.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseSector )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase the sector before writing to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Fill a sector with dummy data and verify it */
        prvIotFlashWriteReadVerify( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulSectorSize );

        /* Erase one sector */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Read back the whole sector to make sure the contents are erased.
         */
        prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulSectorSize );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test erasing multiple sectors and verify the contents are erased.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseMultipleSectors )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase 2 sectors */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize * 2 );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Read back the two sectors to make sure the contents are erased.
         */
        prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulSectorSize * 2 );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test erasing various size blocks (32K, 64K)
 * and make sure erase is being done correctly.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseFlashBlocks )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    uint32_t ulMiddleOffset;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Make sure the offset is aligned to BlockSize */
        if( ( ultestIotFlashStartOffset & ( pxFlashInfo->ulBlockSize - 1 ) ) == 0 )
        {
            /* If Erase asyc is supported, register a callback */
            if( pxFlashInfo->ucAsyncSupported )
            {
                iot_flash_set_callback( xFlashHandle,
                                        prvIotFlashEraseCallback,
                                        NULL );
            }

            /* Erase the 2 sectors before writing to it. */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset,
                                               pxFlashInfo->ulSectorSize * 2 );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            if( pxFlashInfo->ucAsyncSupported )
            {
                /* Wait for the Erase to be completed and callback is called. */
                lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
                TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
            }

            ulMiddleOffset = ( ultestIotFlashStartOffset + ( pxFlashInfo->ulSectorSize / 2 ) );

            /*
             * Write dummy data starting at the middle of the sector to half of next sector
             * and read it back to verify the data.
             */
            prvIotFlashWriteReadVerify( xFlashHandle, ulMiddleOffset, pxFlashInfo->ulSectorSize );

            /* Erase a Block */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset,
                                               pxFlashInfo->ulBlockSize );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            if( pxFlashInfo->ucAsyncSupported )
            {
                /* Wait for the Erase to be completed and callback is called. */
                lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
                TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
            }

            /* Read back the block to make sure the contents are erased.
             */
            prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulBlockSize );
        }
        else
        {
            /* TEST_MESSAGE( "Start offset is not aligned with blockSize" ); */
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test erasing blockSize minus sector Size when address is
 * one sector greater than block alignemtnt, and make sure the sector not included in
 * the Address is not erased, but the rest is erased.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseFlashBlocksUnAlignedAddress )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    uint32_t ulMiddleOffset;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Make sure the offset is aligned to BlockSize */
        if( ( ultestIotFlashStartOffset & ( pxFlashInfo->ulBlockSize - 1 ) ) == 0 )
        {
            /* If Erase asyc is supported, register a callback */
            if( pxFlashInfo->ucAsyncSupported )
            {
                iot_flash_set_callback( xFlashHandle,
                                        prvIotFlashEraseCallback,
                                        NULL );
            }

            /* Erase the 2 sectors before writing to it. */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset,
                                               pxFlashInfo->ulSectorSize * 2 );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            if( pxFlashInfo->ucAsyncSupported )
            {
                /* Wait for the Erase to be completed and callback is called. */
                lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
                TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
            }

            ulMiddleOffset = ( ultestIotFlashStartOffset + ( pxFlashInfo->ulSectorSize / 2 ) );

            /*
             * Write dummy data starting at the middle of the sector to half of next sector
             * and read it back to verify the data.
             */
            prvIotFlashWriteReadVerify( xFlashHandle, ulMiddleOffset, pxFlashInfo->ulSectorSize );

            /* Erase a Block minus sector size starting at block boundry plus sector size */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset + pxFlashInfo->ulSectorSize,
                                               pxFlashInfo->ulBlockSize - pxFlashInfo->ulSectorSize );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            if( pxFlashInfo->ucAsyncSupported )
            {
                /* Wait for the Erase to be completed and callback is called. */
                lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
                TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
            }

            /* Read back the block to make sure the contents are erased.
             */
            prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset + pxFlashInfo->ulSectorSize,
                                         pxFlashInfo->ulBlockSize - pxFlashInfo->ulSectorSize );

            /* Read and make sure that the first sector written is not erased */
            ulMiddleOffset = ( ultestIotFlashStartOffset + ( pxFlashInfo->ulSectorSize / 2 ) );

            prvIotFlashReadVerifyDummyData( xFlashHandle, ulMiddleOffset, pxFlashInfo->ulSectorSize / 2 );
        }
        else
        {
            /* TEST_MESSAGE( "Start offset is not aligned with blockSize" ); */
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test erasing blockSize minus sector Size when size is
 * one sector less than block alignemtnt, and make sure the sector not included in
 * the block size is not erased, but the rest is erased.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseFlashBlocksUnAlignedSize )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    uint32_t ulOffset;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Make sure the offset is aligned to BlockSize */
        if( ( ultestIotFlashStartOffset & ( pxFlashInfo->ulBlockSize - 1 ) ) == 0 )
        {
            /* If Erase asyc is supported, register a callback */
            if( pxFlashInfo->ucAsyncSupported )
            {
                iot_flash_set_callback( xFlashHandle,
                                        prvIotFlashEraseCallback,
                                        NULL );
            }

            /* Erase the 2 sectors before writing to it. */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset,
                                               pxFlashInfo->ulSectorSize * 2 );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            if( pxFlashInfo->ucAsyncSupported )
            {
                /* Wait for the Erase to be completed and callback is called. */
                lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
                TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
            }

            ulOffset = ( ultestIotFlashStartOffset + ( pxFlashInfo->ulSectorSize / 2 ) );

            /*
             * Write dummy data starting at the middle of the sector to half of next sector
             * and read it back to verify the data.
             */
            prvIotFlashWriteReadVerify( xFlashHandle, ulOffset, pxFlashInfo->ulSectorSize );

            /* Also write the last sector in the block to make sure its not erased */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset + ( pxFlashInfo->ulBlockSize - pxFlashInfo->ulSectorSize ),
                                               pxFlashInfo->ulSectorSize );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            ulOffset = ( ultestIotFlashStartOffset + ( pxFlashInfo->ulBlockSize - pxFlashInfo->ulSectorSize ) );

            prvIotFlashWriteReadVerify( xFlashHandle, ulOffset, pxFlashInfo->ulSectorSize );

            /* Erase a Block minus sector size starting at block boundry */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset,
                                               pxFlashInfo->ulBlockSize - pxFlashInfo->ulSectorSize );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            if( pxFlashInfo->ucAsyncSupported )
            {
                /* Wait for the Erase to be completed and callback is called. */
                lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
                TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
            }

            /* Read back the block to make sure the contents are erased.
             */
            prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset,
                                         pxFlashInfo->ulBlockSize - pxFlashInfo->ulSectorSize );

            /* Read and make sure that the last sector written is not erased */
            ulOffset = ( ultestIotFlashStartOffset + ( pxFlashInfo->ulBlockSize - pxFlashInfo->ulSectorSize ) );
            prvIotFlashReadVerifyDummyData( xFlashHandle, ulOffset, pxFlashInfo->ulSectorSize );
        }
        else
        {
            /* TEST_MESSAGE( "Start offset is not aligned with blockSize" ); */
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test erase return an error when un-aligned address is
 * passed to erase function
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseUnAlignedAddress )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Try erasing sector where offset is not aligned. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset + 1,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test erase return an error when un-aligned size is
 * passed to erase function
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseUnAlignedSize )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Try erasing sector where offset is not aligned. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize + 1 );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to write to a page with multiple writes,
 * and make sure that the read contents are correct.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWritePartialPage )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    int8_t i;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase the sector before writing to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* fill out a buffer (Partial page) to be written */
        for( i = 0; i < testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE * 2; i++ )
        {
            uctestIotFlashWriteBuffer[ i ] = i;
        }

        /* Write partial page (< page size) */
        lRetVal = iot_flash_write_sync( xFlashHandle,
                                        ultestIotFlashStartOffset,
                                        &uctestIotFlashWriteBuffer[ 0 ],
                                        testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        /* Write partial page starting from the end of previous
         * write and make sure the second write does not overwrite the first
         */
        lRetVal = iot_flash_write_sync( xFlashHandle,
                                        ultestIotFlashStartOffset + testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE,
                                        &uctestIotFlashWriteBuffer[ testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE ],
                                        testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        /* Read back the data written */
        lRetVal = iot_flash_read_sync( xFlashHandle,
                                       ultestIotFlashStartOffset,
                                       &uctestIotFlashReadBuffer[ 0 ],
                                       testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE * 2 );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        /* Verify the data written. */
        for( i = 0; i < testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE * 2; i++ )
        {
            if( uctestIotFlashReadBuffer[ i ] != uctestIotFlashWriteBuffer[ i ] )
            {
                TEST_ASSERT_MESSAGE( 0, "Contents do NOT match when readback" );
            }
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test writing a page to flash and verify contents.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWritePage )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase the sector before writing to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Write the a full page and verify the data by reading it back */
        prvIotFlashWriteReadVerify( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test writing greater than page but less than or equal to sector
 * to flash and verify contents.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWriteSector )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase the sector before writing to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Write the a full sector and verify the data by reading it back */
        prvIotFlashWriteReadVerify( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulSectorSize );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test across 2 sectors and verify the data
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWriteAcrossSectors )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    uint32_t ulOffset;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase the 2 sectors before writing to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize * 2 );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        ulOffset = ( ultestIotFlashStartOffset + ( pxFlashInfo->ulSectorSize / 2 ) );

        /*
         * Try to write dummy data across twi sectors and read it back to verify the data.
         */
        prvIotFlashWriteReadVerify( xFlashHandle, ulOffset, pxFlashInfo->ulSectorSize );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test Read when erase in progress.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseInProgressRead )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    uint32_t ulSize;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Erase multiple sectors to get some time for read while erase. */
        ulSize = pxFlashInfo->ulSectorSize * 2;
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           ulSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        /* Try reading the data from the same sector(s) being erased. */
        lRetVal = iot_flash_read_sync( xFlashHandle,
                                       ultestIotFlashStartOffset,
                                       &uctestIotFlashReadBuffer[ 0 ],
                                       testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );

        if( lRetVal != IOT_FLASH_DEVICE_BUSY )
        {
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test Write when Flash Erase is in progress.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseInProgressWrite )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    uint32_t ulSize;
    int8_t i;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Erase multiple sectors to get some time for read while erase. */
        ulSize = pxFlashInfo->ulSectorSize * 2;
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           ulSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        /* Fill out a buffer of sectorSize to be written */
        for( i = 0; i < testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE; i++ )
        {
            uctestIotFlashWriteBuffer[ i ] = i;
        }

        /* Try writing the the same sector when erase is in progress. */
        lRetVal = iot_flash_write_sync( xFlashHandle,
                                        ultestIotFlashStartOffset,
                                        &uctestIotFlashWriteBuffer[ 0 ],
                                        testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );

        if( lRetVal != IOT_FLASH_DEVICE_BUSY )
        {
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
            /* Read back the data written */
            lRetVal = iot_flash_read_sync( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           &uctestIotFlashReadBuffer[ 0 ],
                                           testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Verify the data written. */
            for( i = 0; i < testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE; i++ )
            {
                if( uctestIotFlashReadBuffer[ i ] != uctestIotFlashWriteBuffer[ i ] )
                {
                    TEST_ASSERT_MESSAGE( 0, "Contents do NOT match when readback" );
                }
            }
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test Write failure when write protection is enabled.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWriteProtectWriteFailure )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    IotFlashWriteProtectConfig_t xFlashProtect;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase a sector before trying to write to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Make the sector as read only. */
        xFlashProtect.ulAddress = ultestIotFlashStartOffset;
        xFlashProtect.ulSize = pxFlashInfo->ulLockSupportSize;
        xFlashProtect.xProtectionLevel = eFlashReadOnly;

        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eSetFlashBlockProtection,
                                   ( void * const ) &xFlashProtect );

        /* Check if block protection is supported */
        if( lRetVal != IOT_FLASH_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Read back the block protection */
            lRetVal = iot_flash_ioctl( xFlashHandle,
                                       eGetFlashBlockProtection,
                                       ( void * const ) &xFlashProtect );

            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Write the a full page and verify the data is not written reading it back */
            prvIotFlashWriteDummyData( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );
            prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );
        }

        /* Now make the sector writeable */
        xFlashProtect.ulAddress = ultestIotFlashStartOffset;
        xFlashProtect.ulSize = pxFlashInfo->ulLockSupportSize;
        xFlashProtect.xProtectionLevel = eFlashReadWrite;
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eSetFlashBlockProtection,
                                   ( void * const ) &xFlashProtect );

        /* Check if block protection is supported */
        if( lRetVal != IOT_FLASH_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Read back the block protection */
            lRetVal = iot_flash_ioctl( xFlashHandle,
                                       eGetFlashBlockProtection,
                                       ( void * const ) &xFlashProtect );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Write the a full page and verify the data by reading it back */
            prvIotFlashWriteReadVerify( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test Erase failure when write protect is enabled.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWriteProtectEraseFailure )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    IotFlashWriteProtectConfig_t xFlashProtect;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase a sector before trying to write to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Verify that the page was erased */
        prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );

        /* Fill out a buffer of sectorSize to be written */
        prvIotFlashWriteDummyData( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );

        /* Make the sector as read only. */
        xFlashProtect.ulAddress = ultestIotFlashStartOffset;
        xFlashProtect.ulSize = pxFlashInfo->ulLockSupportSize;
        xFlashProtect.xProtectionLevel = eFlashReadOnly;
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eSetFlashBlockProtection,
                                   ( void * const ) &xFlashProtect );

        /* Check if block protection is supported */
        if( lRetVal != IOT_FLASH_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Read back the block protection */
            lRetVal = iot_flash_ioctl( xFlashHandle,
                                       eGetFlashBlockProtection,
                                       ( void * const ) &xFlashProtect );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Try erasing the sector. */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset,
                                               pxFlashInfo->ulSectorSize );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            if( pxFlashInfo->ucAsyncSupported )
            {
                /* Wait for the Erase to be completed and callback is called. */
                lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
                TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
            }

            /* Read back the data written */
            prvIotFlashReadVerifyDummyData( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );
        }

        /* Now make the sector as writeable. */
        xFlashProtect.ulAddress = ultestIotFlashStartOffset;
        xFlashProtect.ulSize = pxFlashInfo->ulLockSupportSize;
        xFlashProtect.xProtectionLevel = eFlashReadWrite;
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eSetFlashBlockProtection,
                                   ( void * const ) &xFlashProtect );

        /* Check if block protection is supported */
        if( lRetVal != IOT_FLASH_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Read back the block protection */
            lRetVal = iot_flash_ioctl( xFlashHandle,
                                       eGetFlashBlockProtection,
                                       ( void * const ) &xFlashProtect );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Try erasing the sector. */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset,
                                               pxFlashInfo->ulSectorSize );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            if( pxFlashInfo->ucAsyncSupported )
            {
                /* Wait for the Erase to be completed and callback is called. */
                lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
                TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
            }

            /* Read back the data written and verify that it is erased */
            prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}


/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test Erase-Read-Write-Read-Erase-Read cycle
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWriteEraseReadCycle )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    uint32_t idx;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Erase a sector before trying to write to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        /* Read back the data and make sure that data is erased first */
        prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );

        for( idx = 0; idx < 5; idx++ )
        {
            /* Write to this sector and veirfy */
            prvIotFlashWriteReadVerify( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );

            /* Get the flash information. */
            pxFlashInfo = iot_flash_getinfo( xFlashHandle );
            TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

            /* Erase the sector. */
            lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                               ultestIotFlashStartOffset,
                                               pxFlashInfo->ulSectorSize );
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

            /* Read back the data and make sure that data is erased */
            prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulPageSize );
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}


/**
 * @brief Test Function to test Write susepnd and resume operations
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWriteSuspendResume )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;
    uint32_t lStatus;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Setup a timer to suspend the write */
        xTimerHandle = iot_timer_open( ltestIotTimerInstance );
        TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase the 2 sectors before writing to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize * 2 );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Set up the timer callback */
        iot_timer_set_callback( xTimerHandle,
                                prvIotTimerCallback,
                                xFlashHandle );

        /* Set up the timer delay */
        lRetVal = iot_timer_delay( xTimerHandle,
                                   testIotFLASH_DEFAULT_DELAY_US );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Start the timer */
        lRetVal = iot_timer_start( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Fill out a sector with data */
        prvIotFlashWriteDummyData( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulSectorSize );

        /* Wait for the Delay callback to be called. */
        lRetVal = xSemaphoreTake( xtestIotFlashTimerSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Verify that current write is suspended */
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eGetFlashStatus,
                                   ( void * const ) &lStatus );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eFlashProgramSuspended, lStatus );

        /* Resume the current write operation */
        /* Suspend the flash write/erase */
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eResumeFlashProgramErase,
                                   NULL );

        /* Check if suspend/resume is supported */
        if( lRetVal != IOT_FLASH_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
        }

        /* Verify that current write is resumed */
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eGetFlashStatus,
                                   ( void * const ) &lStatus );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eFlashIdle, lStatus );

        /* Read the data back to make sure Resume succeded and data is written */
        prvIotFlashReadVerifyDummyData( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulSectorSize );

        lRetVal = iot_timer_close( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Erase susepnd and resume operations
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseSuspendResume )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;
    uint32_t lStatus;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Setup a timer to suspend the write */
        xTimerHandle = iot_timer_open( ltestIotTimerInstance );
        TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* Set up the timer callback */
        iot_timer_set_callback( xTimerHandle,
                                prvIotTimerCallback,
                                xFlashHandle );

        /* Set up the timer delay */
        lRetVal = iot_timer_delay( xTimerHandle,
                                   testIotFLASH_DEFAULT_DELAY_US );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Start the timer */
        lRetVal = iot_timer_start( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase the 2 sectors before writing to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize * 2 );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );


        /* Wait for the Delay callback to be called. */
        lRetVal = xSemaphoreTake( xtestIotFlashTimerSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Verify that current erase is suspended */
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eGetFlashStatus,
                                   ( void * const ) &lStatus );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eFlashEraseSuspended, lStatus );

        /* Resume the current erase operation */
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eResumeFlashProgramErase,
                                   NULL );

        /* Check if suspend/resume is supported */
        if( lRetVal != IOT_FLASH_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
        }

        /* Verify that current erase is resumed */
        lRetVal = iot_flash_ioctl( xFlashHandle,
                                   eGetFlashStatus,
                                   ( void * const ) &lStatus );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eFlashIdle, lStatus );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Read the data back to make sure Resume succeded and data is erased */
        prvIotFlashReadVerifyErased( xFlashHandle, ultestIotFlashStartOffset, pxFlashInfo->ulSectorSize * 2 );

        lRetVal = iot_timer_close( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test writing a page to flash and verify contents.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWritePageFromFlash )
{
    IotFlashHandle_t xFlashHandle;
    IotFlashInfo_t * pxFlashInfo;
    int32_t lRetVal;
    uint32_t i;
    uint32_t j;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Get the flash information. */
        pxFlashInfo = iot_flash_getinfo( xFlashHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, pxFlashInfo );

        /* If Erase asyc is supported, register a callback */
        if( pxFlashInfo->ucAsyncSupported )
        {
            iot_flash_set_callback( xFlashHandle,
                                    prvIotFlashEraseCallback,
                                    NULL );
        }

        /* Erase the sector before writing to it. */
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           pxFlashInfo->ulSectorSize );
        TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

        if( pxFlashInfo->ucAsyncSupported )
        {
            /* Wait for the Erase to be completed and callback is called. */
            lRetVal = xSemaphoreTake( xtestIotFlashSemaphore, portMAX_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }

        /* Write full page of data */
        for( i = 0; i < pxFlashInfo->ulPageSize; i += testIotFLASH_DEFAULT_MAX_BUFFER_SIZE )
        {
            /* Less the a full buffer left? */
            uint32_t remaining_size = ( ( i + testIotFLASH_DEFAULT_MAX_BUFFER_SIZE ) > pxFlashInfo->ulPageSize ) ? ( pxFlashInfo->ulPageSize - i ) : testIotFLASH_DEFAULT_MAX_BUFFER_SIZE;

            if( remaining_size > 0 )
            {
                for( j = 0; j < remaining_size; j++ )
                {
                    uctestIotFlashWriteBuffer[ j ] = j;
                }

                /* Write full page of data */
                lRetVal = iot_flash_write_sync( xFlashHandle,
                                                ultestIotFlashStartOffset + i,
                                                uctestIotFlashWriteROBuffer,
                                                remaining_size );
                TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
            }
        }

        for( i = 0; i < pxFlashInfo->ulPageSize; i += testIotFLASH_DEFAULT_MAX_BUFFER_SIZE )
        {
            /* Less the a full buffer left? */
            uint32_t remaining_size = ( ( i + testIotFLASH_DEFAULT_MAX_BUFFER_SIZE ) > pxFlashInfo->ulPageSize ) ? ( pxFlashInfo->ulPageSize - i ) : testIotFLASH_DEFAULT_MAX_BUFFER_SIZE;

            if( remaining_size > 0 )
            {
                /* Read back the data written */
                lRetVal = iot_flash_read_sync( xFlashHandle,
                                               ultestIotFlashStartOffset + i,
                                               &uctestIotFlashReadBuffer[ 0 ],
                                               remaining_size );
                TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

                /* Verify the data is erased. */
                for( i = 0; i < remaining_size; i++ )
                {
                    if( uctestIotFlashReadBuffer[ i ] != uctestIotFlashWriteROBuffer[ i ] )
                    {
                        TEST_ASSERT_MESSAGE( 0, "Data was NOT erased" );
                    }
                }
            }
        }
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test fuzzing of iot_flash_open and iot_flash_close.
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashOpenCloseFuzz )
{
    IotFlashHandle_t xFlashHandle, xFlashHandleTmp;
    int32_t lRetVal;

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* open the same intance twice */
        xFlashHandleTmp = iot_flash_open( ltestIotFlashInstance );
        TEST_ASSERT_EQUAL( NULL, xFlashHandleTmp );

        /* Open flash to initialize hardware with invalid handle. */
        xFlashHandleTmp = iot_flash_open( testIotFLASH_INVALID_HANDLE );
        TEST_ASSERT_EQUAL( NULL, xFlashHandleTmp );
    }

    /* Close with valid handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );

    /* Close with stale handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test fuzzing of iot_flash_getinfo
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashGetInfoFuzz )
{
    IotFlashInfo_t * pxFlashInfo;

    /* Close with stale handle */
    pxFlashInfo = iot_flash_getinfo( NULL );
    TEST_ASSERT_EQUAL( NULL, pxFlashInfo );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test fuzzing of iot_flash_ioctl
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashIoctlFuzz )
{
    IotFlashHandle_t xFlashHandle;
    int32_t lRetVal;
    uint32_t lStatus;

    /* Call ioctl with null handle */
    lRetVal = iot_flash_ioctl( NULL, eGetFlashStatus, ( void * const ) &lStatus );
    TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Call ioctl with handle, but invalid enum */
        lRetVal = iot_flash_ioctl( xFlashHandle, -1, NULL );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test fuzzing of iot_flash_erase
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashEraseFuzz )
{
    IotFlashHandle_t xFlashHandle;
    int32_t lRetVal;

    /* Erase one sector with NULL handle*/
    lRetVal = iot_flash_erase_sectors( NULL,
                                       ultestIotFlashStartOffset,
                                       testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
    TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Erase one sector with invalid start address*/
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           -1,
                                           testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );

        /* Erase one sector with invalid length*/
        lRetVal = iot_flash_erase_sectors( xFlashHandle,
                                           ultestIotFlashStartOffset,
                                           -1 );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test fuzzing of iot_flash_write
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashWriteFuzz )
{
    IotFlashHandle_t xFlashHandle;
    int32_t lRetVal;

    /* Write partial page with invalid handle */
    lRetVal = iot_flash_write_sync( NULL,
                                    ultestIotFlashStartOffset,
                                    &uctestIotFlashWriteBuffer[ 0 ],
                                    testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
    TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Write partial page with invalid start address */
        lRetVal = iot_flash_write_sync( xFlashHandle,
                                        -1,
                                        &uctestIotFlashWriteBuffer[ 0 ],
                                        testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );

        /* Write partial page with invalid buffer */
        lRetVal = iot_flash_write_sync( xFlashHandle,
                                        ultestIotFlashStartOffset,
                                        NULL,
                                        testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test fuzzing of iot_flash_read
 *
 */
TEST( TEST_IOT_FLASH, AFQP_IotFlashReadFuzz )
{
    IotFlashHandle_t xFlashHandle;
    int32_t lRetVal;

    /* Read partial page with invalid handle */
    lRetVal = iot_flash_read_sync( NULL,
                                   ultestIotFlashStartOffset,
                                   &uctestIotFlashReadBuffer[ 0 ],
                                   testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
    TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );

    /* Open the flash instance */
    xFlashHandle = iot_flash_open( ltestIotFlashInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xFlashHandle );

    if( TEST_PROTECT() )
    {
        /* Read partial page with invalid start address */
        lRetVal = iot_flash_read_sync( xFlashHandle,
                                       -1,
                                       &uctestIotFlashReadBuffer[ 0 ],
                                       testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );

        /* Read partial page with invalid buffer */
        lRetVal = iot_flash_read_sync( xFlashHandle,
                                       ultestIotFlashStartOffset,
                                       NULL,
                                       testIotFLASH_DEFAULT_PARTIAL_PAGE_SIZE );
        TEST_ASSERT_EQUAL( IOT_FLASH_INVALID_VALUE, lRetVal );
    }

    /* Close flash handle */
    lRetVal = iot_flash_close( xFlashHandle );
    TEST_ASSERT_EQUAL( IOT_FLASH_SUCCESS, lRetVal );
}
