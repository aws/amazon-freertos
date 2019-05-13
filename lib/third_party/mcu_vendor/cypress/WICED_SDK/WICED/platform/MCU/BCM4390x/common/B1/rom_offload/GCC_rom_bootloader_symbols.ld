/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

hwcrypto_compute_sha256hmac_inner_outer_hashcontext = 0x0045f41e + 1;
hwcrypto_core_enable                               = 0x0045f45c + 1;
hwcrypto_dcache_clean_dma_input                    = 0x0045f468 + 1;
hwcrypto_split_dma_data                            = 0x0045f4b4 + 1;
hwcrypto_unprotected_blocking_dma_transfer         = 0x0045f4dc + 1;
platform_hwcrypto_aes128cbc_decrypt                = 0x0045f5c4 + 1;
platform_hwcrypto_aescbc_decrypt_sha256_hmac       = 0x0045f642 + 1;
platform_hwcrypto_execute                          = 0x0045f6e4 + 1;
platform_hwcrypto_init                             = 0x0045f910 + 1;
platform_hwcrypto_sha256_hash                      = 0x0045f930 + 1;
platform_hwcrypto_sha256_hmac                      = 0x0045f984 + 1;
platform_hwcrypto_sha256_hmac_final                = 0x0045fa02 + 1;
platform_hwcrypto_sha256_hmac_init                 = 0x0045fa9a + 1;
platform_hwcrypto_sha256_hmac_update               = 0x0045fb22 + 1;
platform_hwcrypto_sha256_incremental               = 0x0045fb8e + 1;

SECTIONS
{
    .rom.rodata 0x0 (NOLOAD) :
    {
        . = 0x0047d0b8;
        *(.rodata.chipcommon_gpio_mapping)

        . = 0x47d1b8;
        *(.rodata.pin_internal_config)

        . = 0x0047f8fc;
        *(.rodata.crc32_table)

        . = 0x004800fc;
        *(.rodata.pmu1_xtaltab0_960)
    }
}
