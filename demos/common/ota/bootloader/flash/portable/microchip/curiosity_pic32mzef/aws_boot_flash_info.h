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
 * @file aws_boot_flash_info.h
 * @brief Boot flash info definitions.
 */

#ifndef _AWS_BOOT_FLASH_INFO_H_
#define _AWS_BOOT_FLASH_INFO_H_

/*Microchip includes.*/
#include "system_definitions.h"

/**
 * @brief Flash device ID.
 */
#define FLASH_DEVICE_INTERNAL             ( 0U )

/**
 * @brief Flash device base.
 */
#define FLASH_DEVICE_BASE                 ( KVA0_TO_KVA1( __KSEG0_PROGRAM_MEM_BASE ) )

/**
 * @brief Flash partitions for OTA images.
 */
#define FLASH_PARTITION_IMAGE_0           ( 0U )
#define FLASH_PARTITION_IMAGE_1           ( 1U )

/**
 * @brief Flash partition offsets.
 */
#define FLASH_PARTITION_OFFSET_IMAGE_0    ( 0U )
#define FLASH_PARTITION_OFFSET_IMAGE_1    ( __KSEG0_PROGRAM_MEM_LENGTH / 2 )

/**
 * @brief Maximum flash partitions for OTA images.
 */
#define FLASH_PARTITIONS_OTA_MAX          ( 2U )

/**
 * @brief Flash partition size.
 */
#define FLASH_IMAGE_SIZE_MAX              ( __KSEG0_PROGRAM_MEM_LENGTH / 2 )

#endif /* ifndef _AWS_BOOT_FLASH_INFO_H_ */
