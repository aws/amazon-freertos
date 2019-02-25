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

/** @file
 * Defines macros to generate checkpoints
 */

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                      Macros
 ******************************************************/

#if defined(WICED_BOOT_CHECKPOINT_ADDR) && defined(WICED_BOOT_CHECKPOINT_BASE_VAL)

#define WICED_BOOT_CHECKPOINT_ENABLED 1

#define WICED_BOOT_CHECKPOINT_WRITE_C(val) \
    do \
    { \
        volatile uint32_t *reg = (uint32_t*)(WICED_BOOT_CHECKPOINT_ADDR); \
        *reg = (WICED_BOOT_CHECKPOINT_BASE_VAL) + (val); \
        __asm__ __volatile__("DMB"); \
    } while(0) \

#define WICED_BOOT_CHECKPOINT_WRITE_ASM(val, scratch_reg1, scratch_reg2) \
    LDR scratch_reg1, =(WICED_BOOT_CHECKPOINT_ADDR); \
    LDR scratch_reg2, =((WICED_BOOT_CHECKPOINT_BASE_VAL) + (val)); \
    STR scratch_reg2, [scratch_reg1]; \
    DMB

#else

#define WICED_BOOT_CHECKPOINT_ENABLED 0
#define WICED_BOOT_CHECKPOINT_WRITE_C(val)
#define WICED_BOOT_CHECKPOINT_WRITE_ASM(val, scratch_reg1, scratch_reg2)

#endif

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

