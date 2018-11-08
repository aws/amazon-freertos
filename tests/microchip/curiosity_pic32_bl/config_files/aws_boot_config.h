/*
 * Amazon FreeRTOS Demo Bootloader V1.1.2  
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
 * @file aws_boot_config.h
 * @brief Bootloader configurations.
 */

#ifndef _AWS_BOOT_CONFIG_H_
#define _AWS_BOOT_CONFIG_H_

/**
 * @brief Enable default application execution.
 * This enables execution of application image at default address specified
 * in the partition table. The application slot used can be any of the OTA
 * image slots or a factory image slot if available on the platform.
 * @see aws_boot_partition module for partition info.
 */
#define bootconfigENABLE_DEFAULT_START                    ( 1U )

/**
 * @brief Enable crypto signature verification.
 * Enables crypto signature verification for application images when
 * device boots. If crypto signature verification fails for application image on
 * any OTA slot the application image header is erased. If bootconfigENABLE_ERASE_INVALID
 * is set entire bank is erased.
 */
#define bootconfigENABLE_CRYPTO_SIGNATURE_VERIFICATION    ( 1U )

/**
 * @brief Enable erase invalid
 * This enables full erase of the OTA application slot if the image is
 * marked invalid. This helps in erasing any code marked invalid due to
 * authentication failures.
 */
#define bootconfigENABLE_ERASE_INVALID                    ( 1U )

/**
 * @brief Validate Hardware ID
 * This enables validation of platform hardware ID and protect from executing
 * application built for other platforms or hardware versions.
 */
#define bootconfigENABLE_HWID_VALIDATION                  ( 0U )

/**
 * @brief Validate Addresses
 * This enables validation of addresses in the application descriptor and
 * should be always enabled.
 */
#define bootconfigENABLE_ADDRESS_VALIDATION               ( 1U )

/**
 * @brief Hardware ID
 * Unique hardware ID for the platform.
 */
#define bootconfigHARDWARE_ID                             ( 0x1U )

/**
 * @brief Enable watchdog
 * Enable watchdog timer to support test run of new OTA application image.
 * In case test run of the new ITA image fails the watchdog timer reset will
 * allow bootloader to roll back to previous image.
 */
#define bootconfigENABLE_WATCHDOG_TIMER                   ( 1U )


#endif /* _AWS_BOOT_CONFIG_H_ */
