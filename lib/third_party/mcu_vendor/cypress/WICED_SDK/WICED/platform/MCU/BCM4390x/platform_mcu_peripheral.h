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
 * Defines BCM43909 common peripheral structures, macros, constants and declares BCM43909 peripheral API
 */
#pragma once

#include "platform_constants.h"
#include "platform_appscr4.h"
#include "platform_toolchain.h"
#include "platform_config.h"

#include "wiced_block_device.h"

#include "wwd_constants.h"
#include "RTOS/wwd_rtos_interface.h"

#include "ring_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_DDR_FUNCNAME(type)                     platform_ddr_init_##type
#define PLATFORM_DDR_FUNCDECL(type)                     platform_result_t PLATFORM_DDR_FUNCNAME(type)( void )
#define PLATFORM_DDR_STR_EXPAND( name )                 #name
#if PLATFORM_NO_DDR
    #define PLATFORM_DDR_FUNCCALL(type)
    //
    #define PLATFORM_DDR_BSS_SIZE                       (0x0)
    #define PLATFORM_DDR_BSS_SECTION(var)               var
    //
    #define PLATFORM_DDR_FREE_OFFSET                    (0x0)
#else
    #define PLATFORM_DDR_FUNCCALL(type)                 do {PLATFORM_DDR_FUNCDECL(type); PLATFORM_DDR_FUNCNAME(type)();} while (0)
    //
    #define PLATFORM_DDR_BSS_SIZE                       ( (unsigned long)&link_ddr_bss_end - (unsigned long)&link_ddr_bss_location )
    #define PLATFORM_DDR_BSS_SECTION_NAME( name )       ".ddr_bss."PLATFORM_DDR_STR_EXPAND( name )
    #define PLATFORM_DDR_BSS_SECTION( var )             SECTION( PLATFORM_DDR_BSS_SECTION_NAME( var ) ) var
    //
    #if PLATFORM_DDR_HEAP_SIZE_CONFIG
        #define PLATFORM_DDR_HEAP_SIZE                  ( (unsigned long)&link_ddr_heap_end - (unsigned long)&link_ddr_heap_location )
        #define PLATFORM_DDR_HEAP_SECTION_NAME( name )  ".ddr_heap."PLATFORM_DDR_STR_EXPAND( name )
        #define PLATFORM_DDR_HEAP_SECTION( var )        SECTION( PLATFORM_DDR_HEAP_SECTION_NAME( var ) ) var
    #endif
    //
    #if PLATFORM_DDR_CODE_AND_DATA_ENABLE
        #define PLATFORM_DDR_TEXT_SECTION_NAME( name )  ".ddr_text."PLATFORM_DDR_STR_EXPAND( name )
        #define PLATFORM_DDR_TEXT_SECTION( var )        SECTION( PLATFORM_DDR_TEXT_SECTION_NAME( var ) ) var
        #define PLATFORM_DDR_DATA_SECTION_NAME( name )  ".ddr_data."PLATFORM_DDR_STR_EXPAND( name )
        #define PLATFORM_DDR_DATA_SECTION( var )        SECTION( PLATFORM_DDR_DATA_SECTION_NAME( var ) ) var
    #endif
    #define PLATFORM_DDR_FREE_OFFSET                    ( (unsigned long)&link_ddr_free_location - PLATFORM_DDR_BASE(0x0) )
#endif
#ifndef PLATFORM_DDR_HEAP_SIZE
#define PLATFORM_DDR_HEAP_SIZE                          (0x0)
#endif
#ifndef PLATFORM_DDR_HEAP_SECTION
#define PLATFORM_DDR_HEAP_SECTION( var )
#endif
#ifndef PLATFORM_DDR_TEXT_SECTION
#define PLATFORM_DDR_TEXT_SECTION( var )                var
#endif
#ifndef PLATFORM_DDR_DATA_SECTION
#define PLATFORM_DDR_DATA_SECTION( var )                var
#endif


#define PLATFORM_DMA_DESCRIPTORS_STR_EXPAND( name )     #name
#define PLATFORM_DMA_DESCRIPTORS_SECTION_NAME( name )   ".dma."PLATFORM_DMA_DESCRIPTORS_STR_EXPAND( name )
#define PLATFORM_DMA_DESCRIPTORS_SECTION( var )         SECTION( PLATFORM_DMA_DESCRIPTORS_SECTION_NAME( var ) ) var

#define PLATFORM_CAPABILITY_ENAB(capability)            ((platform_capabilities_word & capability) != 0)
#define PLATFORM_FEATURE_ENAB(_FEATURE_)                (PLATFORM_CAPABILITY_ENAB(PLATFORM_CAPS_##_FEATURE_) && !(PLATFORM_NO_##_FEATURE_))

#if PLATFORM_WLAN_POWERSAVE
#define PLATFORM_WLAN_POWERSAVE_SET_DELAYED_RELEASE_MS(ms)     platform_wlan_powersave_set_delayed_release_milliseconds(ms)
#define PLATFORM_WLAN_POWERSAVE_RES_UP()                       platform_wlan_powersave_res_up()
#define PLATFORM_WLAN_POWERSAVE_RES_DOWN( check_ready, force ) platform_wlan_powersave_res_down( check_ready, force )
#define PLATFORM_WLAN_POWERSAVE_IS_RES_UP()                    platform_wlan_powersave_is_res_up()
#else
#define PLATFORM_WLAN_POWERSAVE_SET_DELAYED_RELEASE_MS(ms)
#define PLATFORM_WLAN_POWERSAVE_RES_UP()
#define PLATFORM_WLAN_POWERSAVE_RES_DOWN( check_ready, force )
#define PLATFORM_WLAN_POWERSAVE_IS_RES_UP()                    1
#endif

#if PLATFORM_WLAN_POWERSAVE && PLATFORM_WLAN_POWERSAVE_STATS
#define PLATFORM_WLAN_POWERSAVE_GET_STATS( counter)            platform_wlan_powersave_get_stats( counter )
#else
#define PLATFORM_WLAN_POWERSAVE_GET_STATS( counter)            ( (uint32_t)0 )
#endif

#if PLATFORM_ALP_CLOCK_RES_FIXUP
#define PLATFORM_ALP_CLOCK_RES_UP()                            PLATFORM_WLAN_POWERSAVE_RES_UP()
#define PLATFORM_ALP_CLOCK_RES_DOWN( check_ready, force )      PLATFORM_WLAN_POWERSAVE_RES_DOWN( check_ready, force )
#else
#define PLATFORM_ALP_CLOCK_RES_UP()
#define PLATFORM_ALP_CLOCK_RES_DOWN( check_ready, force )
#endif

#if PLATFORM_USB_ALP_CLOCK_RES_FIXUP
#define PLATFORM_USB_ALP_CLOCK_RES_UP()                        PLATFORM_WLAN_POWERSAVE_RES_UP()
#define PLATFORM_USB_ALP_CLOCK_RES_DOWN( check_ready, force )  PLATFORM_WLAN_POWERSAVE_RES_DOWN( check_ready, force )
#else
#define PLATFORM_USB_ALP_CLOCK_RES_UP()
#define PLATFORM_USB_ALP_CLOCK_RES_DOWN( check_ready, force )
#endif

#ifdef DEBUG
#define PLATFORM_TIMEOUT_BEGIN( start_var_name ) \
    const uint32_t start_var_name = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP ); REFERENCE_DEBUG_ONLY_VARIABLE( start_var_name );
#define PLATFORM_TIMEOUT_SEC_ASSERT( assert_string, start_var_name, good_cond, seconds ) \
    wiced_assert( assert_string, ( platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP ) - ( start_var_name ) < platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ILP ) * ( seconds ) ) || ( good_cond ) );
#else
#define PLATFORM_TIMEOUT_BEGIN( start_var_name)
#define PLATFORM_TIMEOUT_SEC_ASSERT( assert_string, start_var_name, good_cond, seconds )
#endif

#ifdef DEBUG
#define PLATFORM_EXTERNAL_HEAP_FILENAME                 __FILE__
#define PLATFORM_EXTERNAL_HEAP_LINE                     __LINE__
#else
#define PLATFORM_EXTERNAL_HEAP_FILENAME                 NULL
#define PLATFORM_EXTERNAL_HEAP_LINE                     0
#endif

#define  platform_heap_malloc( heap, bytes )            platform_heap_malloc_record_caller( heap, bytes, PLATFORM_EXTERNAL_HEAP_FILENAME, PLATFORM_EXTERNAL_HEAP_LINE )
#define  platform_heap_memalign( heap, align, bytes )   platform_heap_memalign_record_caller( heap, align, bytes, PLATFORM_EXTERNAL_HEAP_FILENAME, PLATFORM_EXTERNAL_HEAP_LINE )
#define  platform_heap_realloc( heap, ptr, size )       platform_heap_realloc_record_caller( heap, ptr, size, PLATFORM_EXTERNAL_HEAP_FILENAME, PLATFORM_EXTERNAL_HEAP_LINE )
#define  platform_heap_free( heap, ptr )                platform_heap_free_record_caller( heap, ptr, PLATFORM_EXTERNAL_HEAP_FILENAME, PLATFORM_EXTERNAL_HEAP_LINE )

/******************************************************
 *                    Constants
 ******************************************************/

/* Default STDIO buffer size */
#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE    (64)
#endif

#define UART_CONSOLE_MASK    (0x01)

/* BCM4390x Platform Common Capabilities */
#define PLATFORM_CAPS_COMMON (PLATFORM_CAPS_I2C | PLATFORM_CAPS_UART | PLATFORM_CAPS_SPI)

#define GPIO_TOTAL_PIN_NUMBERS          (32)
#define PIN_FUNCTION_MAX_COUNT          (12)
#define PIN_FUNCTION_UNSUPPORTED        (-1)

/******************************************************
 *                   Enumerations
 ******************************************************/

/*
 * BCM43909 supports pin multiplexing and function selection
 * in accordance with the below PinMux Table specification.
 *
 * Pads           0                   1              2                   3                   4              5                   6              7           8           9                 10                11
 * GPIO_0                             GPIO_0         FAST_UART_RX        I2C1_SDATA          PWM0           SPI1_MISO           PWM2           GPIO_12     GPIO_8                        PWM4              USB20H_CTL1
 * GPIO_1                             GPIO_1         FAST_UART_TX        I2C1_CLK            PWM1           SPI1_CLK            PWM3           GPIO_13     GPIO_9                        PWM5
 * GPIO_2                             GPIO_2                                                 GCI_GPIO_0                                                                TCK
 * GPIO_3                             GPIO_3                                                 GCI_GPIO_1                                                                TMS
 * GPIO_4                             GPIO_4                                                 GCI_GPIO_2                                                                TDI
 * GPIO_5                             GPIO_5                                                 GCI_GPIO_3                                                                TDO
 * GPIO_6                             GPIO_6                                                 GCI_GPIO_4                                                                TRST_L
 * GPIO_7                             GPIO_7         FAST_UART_RTS_OUT   PWM1                PWM3           SPI1_CS             I2C1_CLK       GPIO_15     GPIO_11     PMU_TEST_O                          PWM5
 * GPIO_8                             GPIO_8         SPI1_MISO           PWM2                PWM4           FAST_UART_RX                       GPIO_16     GPIO_12     TAP_SEL_P         I2C1_SDATA        PWM0
 * GPIO_9                             GPIO_9         SPI1_CLK            PWM3                PWM5           FAST_UART_TX                       GPIO_0      GPIO_13                       I2C1_CLK          PWM1
 * GPIO_10                            GPIO_10        SPI1_SISO           PWM4                I2C1_SDATA     FAST_UART_CTS_IN    PWM0           GPIO_1      GPIO_14     PWM2              SDIO_SEP_INT      SDIO_SEP_INT_0D
 * GPIO_11                            GPIO_11        SPI1_CS             PWM5                I2C1_CLK       FAST_UART_RTS_OUT   PWM1           GPIO_7      GPIO_15     PWM3
 * GPIO_12                            GPIO_12        I2C1_SDATA          FAST_UART_RX        SPI1_MISO      PWM2                PWM4           GPIO_8      GPIO_16     PWM0              SDIO_SEP_INT_0D   SDIO_SEP_INT
 * GPIO_13                            GPIO_13        I2C1_CLK            FAST_UART_TX        SPI1_CLK       PWM3                PWM5           GPIO_9      GPIO_0      PWM1
 * GPIO_14                            GPIO_14        PWM0                FAST_UART_CTS_IN    SPI1_SISO      I2C1_SDATA                         GPIO_10                 PWM4                                PWM2
 * GPIO_15                            GPIO_15        PWM1                FAST_UART_RTS_OUT   SPI1_CS        I2C1_CLK                           GPIO_11     GPIO_7      PWM5                                PWM3
 * GPIO_16                            GPIO_16        FAST_UART_CTS_IN    PWM0                PWM2           SPI1_SISO           I2C1_SDATA     GPIO_14     GPIO_10     RF_DISABLE_L      USB20H_CTL2       PWM4
 * sdio_clk       TEST_SDIO_CLK       SDIO_CLK                                                                                                                         SDIO_AOS_CLK
 * sdio_cmd       TEST_SDIO_CMD       SDIO_CMD                                                                                                                         SDIO_AOS_CMD
 * sdio_data_0    TEST_SDIO_DATA_0    SDIO_D0                                                                                                                          SDIO_AOS_D0
 * sdio_data_1    TEST_SDIO_DATA_1    SDIO_D1                                                                                                                          SDIO_AOS_D1
 * sdio_data_2    TEST_SDIO_DATA_2    SDIO_D2                                                                                                                          SDIO_AOS_D2
 * sdio_data_3    TEST_SDIO_DATA_3    SDIO_D3                                                                                                                          SDIO_AOS_D3
 * rf_sw_ctrl_5                       rf_sw_ctrl_5   GCI_GPIO_5
 * rf_sw_ctrl_6                       rf_sw_ctrl_6   UART_DBG_RX         SECI_IN
 * rf_sw_ctrl_7                       rf_sw_ctrl_7   UART_DBG_TX         SECI_OUT
 * rf_sw_ctrl_8                       rf_sw_ctrl_8   SECI_IN             UART_DBG_RX
 * rf_sw_ctrl_9                       rf_sw_ctrl_9   SECI_OUT            UART_DBG_TX
 * PWM0                               PWM0           GPIO_2              GPIO_18
 * PWM1                               PWM1           GPIO_3              GPIO_19
 * PWM2                               PWM2           GPIO_4              GPIO_20
 * PWM3                               PWM3           GPIO_5              GPIO_21
 * PWM4                               PWM4           GPIO_6              GPIO_22
 * PWM5                               PWM5           GPIO_8              GPIO_23
 * SPI0_MISO                          SPI0_MISO      GPIO_17             GPIO_24
 * SPI0_CLK                           SPI0_CLK       GPIO_18             GPIO_25
 * SPI0_SISO                          SPI0_SISO      GPIO_19             GPIO_26
 * SPI0_CS                            SPI0_CS        GPIO_20             GPIO_27
 * I2C0_SDATA                         I2C0_SDATA     GPIO_21             GPIO_28
 * I2C0_CLK                           I2C0_CLK       GPIO_22             GPIO_29
 * i2s_mclk0                          i2s_mclk0      GPIO_23             GPIO_0
 * i2s_sclk0                          i2s_sclk0      GPIO_24             GPIO_2
 * i2s_lrclk0                         i2s_lrclk0     GPIO_25             GPIO_3
 * i2s_sdatai0                        i2s_sdatai0    GPIO_26             GPIO_4
 * i2s_sdatao0                        i2s_sdatao0    GPIO_27             GPIO_5
 * i2s_sdatao1                        i2s_sdatao1    GPIO_28             GPIO_6
 * i2s_sdatai1                        i2s_sdatai1    GPIO_29             GPIO_8
 * i2s_mclk1                          i2s_mclk1      GPIO_30             GPIO_17
 * i2s_sclk1                          i2s_sclk1      GPIO_31             GPIO_30
 * i2s_lrclk1                         i2s_lrclk1     GPIO_0              GPIO_31
 */

/* 43909 pins (Note these are the pad names) */
typedef enum
{
    PIN_GPIO_0,
    PIN_GPIO_1,
    PIN_GPIO_2,
    PIN_GPIO_3,
    PIN_GPIO_4,
    PIN_GPIO_5,
    PIN_GPIO_6,
    PIN_GPIO_7,
    PIN_GPIO_8,
    PIN_GPIO_9,
    PIN_GPIO_10,
    PIN_GPIO_11,
    PIN_GPIO_12,
    PIN_GPIO_13,
    PIN_GPIO_14,
    PIN_GPIO_15,
    PIN_GPIO_16,
    PIN_SDIO_CLK,
    PIN_SDIO_CMD,
    PIN_SDIO_DATA_0,
    PIN_SDIO_DATA_1,
    PIN_SDIO_DATA_2,
    PIN_SDIO_DATA_3,
    PIN_UART0_CTS,
    PIN_UART0_RTS,
    PIN_UART0_RXD,
    PIN_UART0_TXD,
    PIN_PWM_0,
    PIN_PWM_1,
    PIN_PWM_2,
    PIN_PWM_3,
    PIN_PWM_4,
    PIN_PWM_5,
    PIN_RF_SW_CTRL_5,
    PIN_RF_SW_CTRL_6,
    PIN_RF_SW_CTRL_7,
    PIN_RF_SW_CTRL_8,
    PIN_RF_SW_CTRL_9,
    PIN_SPI_0_MISO,
    PIN_SPI_0_CLK,
    PIN_SPI_0_MOSI,
    PIN_SPI_0_CS,
    PIN_I2C0_SDATA,
    PIN_I2C0_CLK,
    PIN_I2S_MCLK0,
    PIN_I2S_SCLK0,
    PIN_I2S_LRCLK0,
    PIN_I2S_SDATAI0,
    PIN_I2S_SDATAO0,
    PIN_I2S_SDATAO1,
    PIN_I2S_SDATAI1,
    PIN_I2S_MCLK1,
    PIN_I2S_SCLK1,
    PIN_I2S_LRCLK1,
    PIN_SPI_1_CLK,
    PIN_SPI_1_MISO,
    PIN_SPI_1_MOSI,
    PIN_SPI_1_CS,
    PIN_I2C1_CLK,
    PIN_I2C1_SDATA,
    PIN_MAX  /* Denotes maximum value. Not a valid pin. */
} platform_pin_t;

/* 43909 pin function selection values (based on PinMux table) */
typedef enum
{
    PIN_FUNCTION_GPIO_0,
    PIN_FUNCTION_GPIO_1,
    PIN_FUNCTION_GPIO_2,
    PIN_FUNCTION_GPIO_3,
    PIN_FUNCTION_GPIO_4,
    PIN_FUNCTION_GPIO_5,
    PIN_FUNCTION_GPIO_6,
    PIN_FUNCTION_GPIO_7,
    PIN_FUNCTION_GPIO_8,
    PIN_FUNCTION_GPIO_9,
    PIN_FUNCTION_GPIO_10,
    PIN_FUNCTION_GPIO_11,
    PIN_FUNCTION_GPIO_12,
    PIN_FUNCTION_GPIO_13,
    PIN_FUNCTION_GPIO_14,
    PIN_FUNCTION_GPIO_15,
    PIN_FUNCTION_GPIO_16,
    PIN_FUNCTION_GPIO_17,
    PIN_FUNCTION_GPIO_18,
    PIN_FUNCTION_GPIO_19,
    PIN_FUNCTION_GPIO_20,
    PIN_FUNCTION_GPIO_21,
    PIN_FUNCTION_GPIO_22,
    PIN_FUNCTION_GPIO_23,
    PIN_FUNCTION_GPIO_24,
    PIN_FUNCTION_GPIO_25,
    PIN_FUNCTION_GPIO_26,
    PIN_FUNCTION_GPIO_27,
    PIN_FUNCTION_GPIO_28,
    PIN_FUNCTION_GPIO_29,
    PIN_FUNCTION_GPIO_30,
    PIN_FUNCTION_GPIO_31,
    PIN_FUNCTION_GCI_GPIO_0,
    PIN_FUNCTION_GCI_GPIO_1,
    PIN_FUNCTION_GCI_GPIO_2,
    PIN_FUNCTION_GCI_GPIO_3,
    PIN_FUNCTION_GCI_GPIO_4,
    PIN_FUNCTION_GCI_GPIO_5,
    PIN_FUNCTION_FAST_UART_RX,
    PIN_FUNCTION_FAST_UART_TX,
    PIN_FUNCTION_FAST_UART_CTS_IN,
    PIN_FUNCTION_FAST_UART_RTS_OUT,
    PIN_FUNCTION_UART_DBG_RX,
    PIN_FUNCTION_UART_DBG_TX,
    PIN_FUNCTION_SECI_IN,
    PIN_FUNCTION_SECI_OUT,
    PIN_FUNCTION_PWM0,
    PIN_FUNCTION_PWM1,
    PIN_FUNCTION_PWM2,
    PIN_FUNCTION_PWM3,
    PIN_FUNCTION_PWM4,
    PIN_FUNCTION_PWM5,
    PIN_FUNCTION_RF_SW_CTRL_5,
    PIN_FUNCTION_RF_SW_CTRL_6,
    PIN_FUNCTION_RF_SW_CTRL_7,
    PIN_FUNCTION_RF_SW_CTRL_8,
    PIN_FUNCTION_RF_SW_CTRL_9,
    PIN_FUNCTION_SPI0_CLK,
    PIN_FUNCTION_SPI0_MISO,
    PIN_FUNCTION_SPI0_MOSI,
    PIN_FUNCTION_SPI0_CS,
    PIN_FUNCTION_SPI1_CLK,
    PIN_FUNCTION_SPI1_MISO,
    PIN_FUNCTION_SPI1_MOSI,
    PIN_FUNCTION_SPI1_CS,
    PIN_FUNCTION_I2C0_SDATA,
    PIN_FUNCTION_I2C0_CLK,
    PIN_FUNCTION_I2C1_SDATA,
    PIN_FUNCTION_I2C1_CLK,
    PIN_FUNCTION_I2S_MCLK0,
    PIN_FUNCTION_I2S_SCLK0,
    PIN_FUNCTION_I2S_LRCLK0,
    PIN_FUNCTION_I2S_SDATAI0,
    PIN_FUNCTION_I2S_SDATAO0,
    PIN_FUNCTION_I2S_MCLK1,
    PIN_FUNCTION_I2S_SCLK1,
    PIN_FUNCTION_I2S_LRCLK1,
    PIN_FUNCTION_I2S_SDATAI1,
    PIN_FUNCTION_I2S_SDATAO1,
    PIN_FUNCTION_TEST_SDIO_CLK,
    PIN_FUNCTION_TEST_SDIO_CMD,
    PIN_FUNCTION_TEST_SDIO_DATA_0,
    PIN_FUNCTION_TEST_SDIO_DATA_1,
    PIN_FUNCTION_TEST_SDIO_DATA_2,
    PIN_FUNCTION_TEST_SDIO_DATA_3,
    PIN_FUNCTION_SDIO_CLK,
    PIN_FUNCTION_SDIO_CMD,
    PIN_FUNCTION_SDIO_D0,
    PIN_FUNCTION_SDIO_D1,
    PIN_FUNCTION_SDIO_D2,
    PIN_FUNCTION_SDIO_D3,
    PIN_FUNCTION_SDIO_AOS_CLK,
    PIN_FUNCTION_SDIO_AOS_CMD,
    PIN_FUNCTION_SDIO_AOS_D0,
    PIN_FUNCTION_SDIO_AOS_D1,
    PIN_FUNCTION_SDIO_AOS_D2,
    PIN_FUNCTION_SDIO_AOS_D3,
    PIN_FUNCTION_SDIO_SEP_INT,
    PIN_FUNCTION_SDIO_SEP_INT_0D,
    PIN_FUNCTION_USB20H_CTL1,
    PIN_FUNCTION_USB20H_CTL2,
    PIN_FUNCTION_RF_DISABLE_L,
    PIN_FUNCTION_TCK,
    PIN_FUNCTION_TMS,
    PIN_FUNCTION_TDI,
    PIN_FUNCTION_TDO,
    PIN_FUNCTION_TRST_L,
    PIN_FUNCTION_PMU_TEST_O,
    PIN_FUNCTION_TAP_SEL_P,
    PIN_FUNCTION_HW_DEFAULT,
    PIN_FUNCTION_SAME_AS_PIN,
    PIN_FUNCTION_UNKNOWN, /* Not a valid pin function */
    PIN_FUNCTION_MAX /* Denotes max value. Not a valid pin function */
} platform_pin_function_t;

/* 43909 pin function index values */
typedef enum
{
    PIN_FUNCTION_INDEX_0    = 0,
    PIN_FUNCTION_INDEX_1    = 1,
    PIN_FUNCTION_INDEX_2    = 2,
    PIN_FUNCTION_INDEX_3    = 3,
    PIN_FUNCTION_INDEX_4    = 4,
    PIN_FUNCTION_INDEX_5    = 5,
    PIN_FUNCTION_INDEX_6    = 6,
    PIN_FUNCTION_INDEX_7    = 7,
    PIN_FUNCTION_INDEX_8    = 8,
    PIN_FUNCTION_INDEX_9    = 9,
    PIN_FUNCTION_INDEX_10   = 10,
    PIN_FUNCTION_INDEX_11   = 11
} pin_function_index_t;

/* 43909 pin function selection values (based on PinMux table) */
typedef enum
{
    PIN_FUNCTION_TYPE_GPIO,
    PIN_FUNCTION_TYPE_GCI_GPIO,
    PIN_FUNCTION_TYPE_FAST_UART,
    PIN_FUNCTION_TYPE_UART_DBG,
    PIN_FUNCTION_TYPE_SECI,
    PIN_FUNCTION_TYPE_PWM,
    PIN_FUNCTION_TYPE_RF_SW_CTRL,
    PIN_FUNCTION_TYPE_SPI0,
    PIN_FUNCTION_TYPE_SPI1,
    PIN_FUNCTION_TYPE_I2C0,
    PIN_FUNCTION_TYPE_I2C1,
    PIN_FUNCTION_TYPE_I2S,
    PIN_FUNCTION_TYPE_SDIO,
    PIN_FUNCTION_TYPE_USB,
    PIN_FUNCTION_TYPE_JTAG,
    PIN_FUNCTION_TYPE_MISC,
    PIN_FUNCTION_TYPE_UNKNOWN,
    PIN_FUNCTION_TYPE_MAX /* Denotes max value. Not a valid pin function type */
} platform_pin_function_type_t;

typedef struct
{
    platform_pin_function_type_t pin_function_type;
    platform_pin_function_t      pin_function;
} platform_pin_function_info_t;

typedef struct
{
    platform_pin_t                  pin_pad_name;
    uint8_t                         gci_chip_ctrl_reg;
    uint8_t                         gci_chip_ctrl_mask;
    uint8_t                         gci_chip_ctrl_pos;
    platform_pin_function_info_t    pin_function_selection[PIN_FUNCTION_MAX_COUNT];
} platform_pin_internal_config_t;

typedef enum
{
    UART_SLOW,
    UART_FAST,
    UART_GCI
} platform_uart_port_t;

typedef enum
{
    CLOCK_ILP,       /* Idle Low Power Clock */
    CLOCK_ALP,       /* Active Low Power Clock */
    CLOCK_HT,        /* High Throughput Clock */
    CLOCK_BACKPLANE, /* Whatever clock is backplane running now */
} platform_clock_t;

typedef enum
{
    PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP,
    PLATFORM_MCU_POWERSAVE_MODE_SLEEP,
    PLATFORM_MCU_POWERSAVE_MODE_MAX /* Denotes max value. Not a valid mode */
} platform_mcu_powersave_mode_t;

typedef enum
{
    PLATFORM_MCU_POWERSAVE_CLOCK_ALP_AVAILABLE,    /* ALP clock available to use by backplane and cores. APPS still can go to deep-sleep. */
    PLATFORM_MCU_POWERSAVE_CLOCK_HT_AVAILABLE,     /* HT clock available to use by backplane and cores. APPS still can go to deep-sleep. */
    PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_ILP, /* At least ILP clock is provided to backplane (if ALP/HT already requested they would be used). APPS remain powered-up. */
    PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_ALP, /* At least ALP clock is provided to backplane (if HT already requested it would be used). APPS remain powered-up. */
    PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_HT,  /* HT clock is provided to backplane. APPS remain powered-up. */
    PLATFORM_MCU_POWERSAVE_CLOCK_MAX               /* Denotes max value. Not a valid mode */
} platform_mcu_powersave_clock_t;

typedef enum
{
    PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_CONFIG_HIGH_IMPEDANCE,
    PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_CONFIG_PULL_UP,
    PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_CONFIG_PULL_DOWN
} platform_mcu_powersave_gpio_wakeup_config_t;

typedef enum
{
    PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_TRIGGER_RISING_EDGE,
    PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_TRIGGER_FALLING_EDGE,
} platform_mcu_powersave_gpio_wakeup_trigger_t;

typedef enum
{
    PLATFORM_GCI_GPIO_IRQ_TRIGGER_FAST_EDGE     = 0x1, /* Please keep elements as power of 2, they can be used to create bit mask */
    PLATFORM_GCI_GPIO_IRQ_TRIGGER_RISING_EDGE   = 0x2,
    PLATFORM_GCI_GPIO_IRQ_TRIGGER_FALLING_EDGE  = 0x4,
    PLATFORM_GCI_GPIO_IRQ_TRIGGER_LEVEL_HIGH    = 0x8
} platform_gci_gpio_irq_trigger_t;

typedef enum
{
    PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_ALWAYS,
    PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_NEVER,
    PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_IF_MCU_POWERSAVE_ENABLED,
    PLATFORM_TICK_POWERSAVE_MODE_MAX /* Denotes max value. Not a valid mode */
} platform_tick_powersave_mode_t;

typedef enum
{
    PLATFORM_TICK_SLEEP_FORCE_INTERRUPTS_OFF,
    PLATFORM_TICK_SLEEP_FORCE_INTERRUPTS_WLAN_ON,
    PLATFORM_TICK_SLEEP_FORCE_INTERRUPTS_ON
} platform_tick_sleep_force_interrupts_mode_t;

/* OTP regions */
typedef enum
{
    PLATFORM_OTP_HW_RGN   = 1,
    PLATFORM_OTP_SW_RGN   = 2,
    PLATFORM_OTP_CI_RGN   = 4,
    PLATFORM_OTP_FUSE_RGN = 8,
    PLATFORM_OTP_ALL_RGN  = 0xf /* From h/w region to end of OTP including checksum */
} platform_otp_region_t;

/* Platform Capabilities */
typedef enum
{
    PLATFORM_CAPS_SDIO = (1 << 0),
    PLATFORM_CAPS_GMAC = (1 << 1),
    PLATFORM_CAPS_USB  = (1 << 2),
    PLATFORM_CAPS_HSIC = (1 << 3),
    PLATFORM_CAPS_I2S  = (1 << 4),
    PLATFORM_CAPS_I2C  = (1 << 5),
    PLATFORM_CAPS_UART = (1 << 6),
    PLATFORM_CAPS_DDR  = (1 << 7),
    PLATFORM_CAPS_SPI  = (1 << 8),
    PLATFORM_CAPS_JTAG = (1 << 9)
} platform_caps_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef void (*platform_tick_sleep_idle_func)( void );

typedef platform_result_t (*platform_otp_cis_parse_callback_func)( void* context, uint8_t tag, uint8_t brcm_tag, uint16_t offset, uint8_t size );

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    platform_pin_t pin;
} platform_gpio_t;

typedef struct
{
    platform_uart_port_t   port;
    const platform_gpio_t* tx_pin;
    const platform_gpio_t* rx_pin;
    const platform_gpio_t* cts_pin;
    const platform_gpio_t* rts_pin;
    platform_clock_t       src_clk;
} platform_uart_t;

typedef struct
{
    wiced_ring_buffer_t*   rx_buffer;
    host_semaphore_type_t  rx_complete;
    host_semaphore_type_t  tx_complete;
    platform_uart_t*       interface;
    wiced_bool_t           hw_flow_control_is_on;
    uint32_t               rx_overflow;
} platform_uart_driver_t;

typedef enum
{
    UART_NO_INTERRUPT = 0,
    UART_RX_READY     = (1 << 0),
    UART_TX_READY     = (1 << 1),
} platform_uart_irq_status_t;

typedef enum
{
    BCM4390X_GSIO_0 = 0,
    BCM4390X_GSIO_1 = 1,
    BCM4390X_GSIO_2 = 2,
    BCM4390X_GSIO_3 = 3,
    BCM4390X_GSIO_MAX,
} platform_gsio_interface_t;

typedef enum
{
    BCM4390X_SPI_0 = BCM4390X_GSIO_0,
    BCM4390X_SPI_1 = BCM4390X_GSIO_1,
} platform_spi_port_t;

typedef enum
{
    BCM4390X_I2C_0 = BCM4390X_GSIO_2,
    BCM4390X_I2C_1 = BCM4390X_GSIO_3,
} platform_i2c_port_t;

struct spi_driver;
typedef struct spi_driver spi_driver_t;

/* GSIO SPI bus driver functions */
extern const spi_driver_t spi_gsio_driver;

/* Bit-Banging SPI bus driver functions */
extern const spi_driver_t spi_bb_driver;

typedef struct
{
    platform_spi_port_t                  port;
    const platform_gpio_t*               pin_mosi;
    const platform_gpio_t*               pin_miso;
    const platform_gpio_t*               pin_clock;
    const platform_gpio_t*               pin_cs;
    const spi_driver_t*                  driver;
} platform_spi_t;

typedef struct
{
    int junk;
} platform_spi_slave_driver_t;

typedef struct
{
    host_semaphore_type_t                   transfer_complete;
} platform_gspi_slave_driver_t;

struct adc_driver;
typedef struct adc_driver adc_driver_t;

typedef struct
{
    uint8_t                channel;
    const adc_driver_t*    driver;
} platform_adc_t;

#ifdef USING_EXTERNAL_ADC
/* MAX11615 ADC driver functions */
extern const adc_driver_t max11615_adc_driver;
#endif /* USING_EXTERNAL_ADC */

/* Internal ADC driver functions */
extern const adc_driver_t internal_adc_driver;

struct i2c_driver;
typedef struct i2c_driver i2c_driver_t;

/* GSIO I2C bus driver functions */
extern const i2c_driver_t i2c_gsio_driver;

/* Bit-Banging I2C bus driver functions */
extern const i2c_driver_t i2c_bb_driver;

typedef struct
{
    platform_i2c_port_t                  port;
    const platform_gpio_t*               pin_scl;
    const platform_gpio_t*               pin_sda;
    const i2c_driver_t*                  driver;
} platform_i2c_t;

typedef struct
{
    platform_pin_t                       pin;
    platform_pin_function_t              func;
    wiced_bool_t                         update_all;
    wiced_bool_t                         is_single_shot;
    wiced_bool_t                         invert;
    pwm_channel_ctrl_alignment_t         alignment;
    pwm_channel_dead_time_t              dead_time;
} platform_pwm_t;

typedef enum
{
    BCM4390X_I2S_0,
    BCM4390X_I2S_1,
    BCM4390X_I2S_MAX,
} platform_i2s_port_t;

typedef enum
{
    BCM4390X_AUDIO_PLL_AUDIO_SYNC   = 0,
    BCM4390X_AUDIO_PLL_MCLK1        = 1,
    BCM4390X_AUDIO_PLL_MCLK2        = 2,
    BCM4390X_AUDIO_PLL_MAX,
} platform_audio_pll_channels_t;

typedef enum
{
    PLATFORM_I2S_READ,
    PLATFORM_I2S_WRITE,
    PLATFORM_I2S_MAX,
} platform_i2s_direction_t;

typedef struct
{
    platform_i2s_port_t                 port;
    unsigned                            is_master   : 1;
    IRQn_Type                           irqn;
    /* Only applicable when is_master==1. */
    platform_audio_pll_channels_t       audio_pll_ch;
} platform_i2s_port_info_t;

typedef struct
{
    const platform_i2s_port_info_t      *port_info;
    platform_i2s_direction_t            stream_direction;
} platform_i2s_t;

typedef enum
{
    PLATFORM_HIBERNATION_CLOCK_EXTERNAL,
    PLATFORM_HIBERNATION_CLOCK_INTERNAL_128KHZ,
    PLATFORM_HIBERNATION_CLOCK_INTERNAL_32KHZ,
    PLATFORM_HIBERNATION_CLOCK_INTERNAL_16KHZ
} platform_hibernation_clock_t;

typedef struct
{
    platform_hibernation_clock_t clock;              /* Defines which clock - external or internal to use, and if internal then which frequency. */
    uint32_t                     hib_ext_clock_freq; /* Defines external clock frequency, if set 0 then use default configuration. */
    int32_t                      rc_code;            /* Defines RC parameter of internal clock (the higher value, the lower frequency). Has to be created by calibration algorithm and applied by firmware. Negative value means default. */
} platform_hibernation_t;

typedef enum
{
    PLATFORM_TICK_COMMAND_POWERSAVE_BEGIN,
    PLATFORM_TICK_COMMAND_POWERSAVE_END,
    PLATFORM_TICK_COMMAND_RELEASE_PMU_TIMER_BEGIN,
    PLATFORM_TICK_COMMAND_RELEASE_PMU_TIMER_END
} platform_tick_command_t;

typedef enum
{
    PLATFORM_BACKPLANE_APPS,
    PLATFORM_BACKPLANE_WLAN,
    PLATFORM_BACKPLANE_AON
} platform_backplane_t;

typedef enum
{
    PLATFORM_REFERENCE_CLOCK_CPU,
    PLATFORM_REFERENCE_CLOCK_BACKPLANE,
    PLATFORM_REFERENCE_CLOCK_ALP,
    PLATFORM_REFERENCE_CLOCK_ILP,
    PLATFORM_REFERENCE_CLOCK_FAST_UART
} platform_reference_clock_t;

typedef enum
{
    PLATFORM_WLAN_POWERSAVE_STATS_CALL_NUM,
    PLATFORM_WLAN_POWERSAVE_STATS_UP_TIME,
    PLATFORM_WLAN_POWERSAVE_STATS_WAIT_UP_TIME
} platform_wlan_powersave_stats_t;

typedef enum
{
    /*
     * MCU reset implemented as re-setting PMU registers to default state and sequencer to power-on state.
     * Sequencer means APPS domain is powered-down initially, then powered-up.
     * Whole state is lost, including SRAM.
     */
    PLATFORM_RESET_TYPE_POWERCYCLE,

    /*
     * MCU reset implemented as backplane reset, and PMU min/max res masks are restored.
     * Not whole state is lost - SRAM is preserved, some always-on domain state is preserved.
     */
    PLATFORM_RESET_TYPE_RESET
} platform_reset_type_t;

typedef enum
{
    PLATFORM_HEAP_SRAM,
    PLATFORM_HEAP_DDR
} platform_heap_type_t;

typedef struct
{
    wiced_block_device_write_mode_t write_mode;
    uint32_t                        offset;
} ddr_block_device_specific_data_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

extern uint32_t platform_capabilities_word;

extern void*    link_ddr_bss_location;
extern void*    link_ddr_bss_end;
extern void*    link_ddr_heap_location;
extern void*    link_ddr_heap_end;
extern void*    link_ddr_free_location;

extern const wiced_block_device_driver_t ddr_block_device_driver;

/******************************************************
 *               Function Declarations
 ******************************************************/

/* Call this from GPIO interrupt handler */
void              platform_gpio_irq             ( void );

/* Functions related to platform RTC driver */
platform_result_t platform_rtc_init             ( void );

/* Call this from UART interrupt handler */
void              platform_uart_irq             ( platform_uart_driver_t* driver );

/* Call this from ASCU interrupt handler */
void              platform_ascu_irq             ( uint32_t intr_status );

/* Functions related to ChipCommon */
void              platform_chipcommon_disable_irq ( void );
void              platform_chipcommon_enable_irq  ( void );

/*
 * Set specific GCI chipcontrol register.
 * Function atomically read register reg_offset, clear all bits found in clear_mask, then set all bits found in set_mask. And finally write back result.
*/
uint32_t          platform_gci_chipcontrol      ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

/*
 * Set specific GCI gpiocontrol register.
 * Function atomically read register reg_offset, clear all bits found in clear_mask, then set all bits found in set_mask. And finally write back result.
*/
uint32_t          platform_gci_gpiocontrol      ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

/*
 * Set specific GCI gpiostatus register.
 * Function atomically read register reg_offset, clear all bits found in clear_mask, then set all bits found in set_mask. And finally write back result.
*/
uint32_t          platform_gci_gpiostatus       ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

/*
 * Set specific GCI gpiowakemask register.
 * Function atomically read register reg_offset, clear all bits found in clear_mask, then set all bits found in set_mask. And finally write back result.
*/
uint32_t          platform_gci_gpiowakemask     ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

/*
 * Return value of specific GCI chipstatus register.
 */
uint32_t          platform_gci_chipstatus       ( uint8_t reg_offset );

/*
 * Set specific PMU chipcontrol register.
 * Function atomically read register reg_offset, clear all bits found in clear_mask, then set all bits found in set_mask. And finally write back result.
*/
uint32_t          platform_pmu_chipcontrol      ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

uint32_t          platform_pmu_res_updown_time  ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

uint32_t          platform_pmu_res_dep_mask     ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

/*
 * Set specific PMU regulator register.
 * Function atomically read register reg_offset, clear all bits found in clear_mask, then set all bits found in set_mask. And finally write back result.
*/
uint32_t          platform_pmu_regulatorcontrol ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

/*
 * Set specific PMU PLL control register.
 * Function atomically read register reg_offset, clear all bits found in clear_mask, then set all bits found in set_mask. And finally write back result.
*/
uint32_t          platform_pmu_pllcontrol       ( uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask );

#if defined(PLATFORM_4390X_OVERCLOCK)
void              platform_pmu_pllcontrol_mdiv_set( uint8_t channel, uint8_t divider );
uint8_t           platform_pmu_pllcontrol_mdiv_get( uint8_t channel );
#endif /* PLATFORM_4390X_OVERCLOCK */

/*
 * Modify generic register.
 * Function atomically read register reg_offset, clear all bits found in clear_mask, then set all bits found in set_mask. And finally write back result.
 */
uint32_t          platform_common_chipcontrol   ( volatile uint32_t* reg, uint32_t clear_mask, uint32_t set_mask );

/* Return default PWM clock. Platform code can redefine if want to choose another. */
platform_clock_t  platform_pwm_getclock         ( void );

platform_result_t platform_filesystem_init      ( void );

platform_result_t platform_watchdog_init        ( void );

platform_result_t platform_watchdog_deinit      ( void );

platform_result_t platform_backplane_init       ( void );

void              platform_backplane_print_cores( uint32_t backplane_mask );

void              platform_backplane_reset_cores( uint32_t* except_wrappers, uint32_t backplane_mask );

void              platform_irq_init             ( void );

platform_result_t platform_irq_remap_source     ( uint32_t wrapper_addr, uint8_t source_num, uint8_t bus_line_num );

platform_result_t platform_irq_remap_sink       ( uint8_t bus_line_num, uint8_t sink_num );

uint32_t          platform_irq_demuxer_hook     ( uint32_t irq_status );

void              platform_cores_powersave_init       ( void );

void              platform_cores_powersave_deinit     ( void );

platform_result_t platform_mcu_powersave_init         ( void );

void              platform_mcu_powersave_warmboot_init( void );

wiced_bool_t      platform_mcu_powersave_is_warmboot  ( void );

wiced_bool_t      platform_mcu_powersave_is_permitted ( void );

void              platform_mcu_powersave_sleep        ( uint32_t ticks, platform_tick_sleep_force_interrupts_mode_t mode );

void              platform_mcu_powersave_set_mode     ( platform_mcu_powersave_mode_t mode );

void              platform_mcu_powersave_set_tick_mode( platform_tick_powersave_mode_t mode );

void              platform_mcu_powersave_request_clock( platform_mcu_powersave_clock_t clock );

void              platform_mcu_powersave_release_clock( platform_mcu_powersave_clock_t clock );

platform_mcu_powersave_mode_t  platform_mcu_powersave_get_mode                 ( void );

platform_tick_powersave_mode_t platform_mcu_powersave_get_tick_mode            ( void );

uint32_t                       platform_mcu_powersave_get_clock_request_counter( platform_mcu_powersave_clock_t clock );

wiced_bool_t      platform_wlan_powersave_res_up   ( void );

wiced_bool_t      platform_wlan_powersave_res_down ( wiced_bool_t(*check_ready)(void), wiced_bool_t force );

wiced_bool_t      platform_wlan_powersave_is_res_up( void );

void              platform_wlan_powersave_res_event( void );

uint32_t          platform_wlan_powersave_get_stats( platform_wlan_powersave_stats_t which_counter );

uint32_t          platform_reference_clock_get_freq( platform_reference_clock_t clock );

void              platform_tick_sleep_clear_ext_wake( void );

void              platform_tick_timer_init      ( void );

uint32_t          platform_tick_sleep_rtos      ( platform_tick_sleep_idle_func idle_func, uint32_t ticks, wiced_bool_t powersave_permission );

uint32_t          platform_tick_sleep_force     ( platform_tick_sleep_idle_func idle_func, uint32_t ticks, platform_tick_sleep_force_interrupts_mode_t mode );

void              platform_tick_execute_command ( platform_tick_command_t command );

void              platform_sflash_init          ( void );

platform_result_t platform_hibernation_init     ( const platform_hibernation_t* hib );

platform_result_t platform_hibernation_calibrate_clock( const platform_hibernation_t* hib );

void              platform_mcu_specific_reset   ( platform_reset_type_t type ) NORETURN;

wiced_bool_t      platform_boot_is_reset        ( void );

wiced_bool_t      platform_is_init_completed    ( void );

uint32_t          platform_ddr_get_size         ( void );

platform_result_t platform_watchdog_kick_milliseconds           ( uint32_t milliseconds );
platform_result_t platform_watchdog_stop                        ( void );

platform_result_t platform_mcu_powersave_gpio_wakeup_enable     ( platform_mcu_powersave_gpio_wakeup_config_t config, platform_mcu_powersave_gpio_wakeup_trigger_t trigger );
void              platform_mcu_powersave_gpio_wakeup_ack        ( void );
void              platform_mcu_powersave_gpio_wakeup_disable    ( void );
platform_result_t platform_mcu_powersave_gci_gpio_wakeup_enable ( platform_pin_t gpio_pin, platform_mcu_powersave_gpio_wakeup_config_t config, platform_gci_gpio_irq_trigger_t trigger );
void              platform_mcu_powersave_gci_gpio_wakeup_disable( platform_pin_t gpio_pin );
wiced_bool_t      platform_mcu_powersave_gci_gpio_wakeup_ack    ( platform_pin_t gpio_pin );
platform_result_t platform_mcu_powersave_bluetooth_sleep_handler( const platform_gpio_t* bt_pin_device_wake );
platform_result_t platform_mcu_powersave_bluetooth_wakeup_handler( const platform_gpio_t* bt_pin_device_wake, const platform_gpio_t* bt_pin_host_wake );

void              platform_cpu_core_init        ( void );

void              platform_deep_sleep_init      ( void );

/* Functions related to platform OTP driver */
platform_result_t platform_otp_init             ( void );
platform_result_t platform_otp_status           ( uint32_t* status );
platform_result_t platform_otp_size             ( uint16_t* size );
platform_result_t platform_otp_read_bit         ( uint16_t bit_number, uint16_t* read_bit );
platform_result_t platform_otp_read_word        ( uint16_t word_number, uint16_t* read_word );
platform_result_t platform_otp_read_array       ( uint16_t byte_number, void* data, uint16_t byte_len );
platform_result_t platform_otp_get_region       ( platform_otp_region_t region, uint16_t* word_number, uint16_t* word_len );
platform_result_t platform_otp_read_region      ( platform_otp_region_t region, uint16_t* data, uint16_t* word_len );
platform_result_t platform_otp_newcis           ( uint16_t* newcis_bit );
platform_result_t platform_otp_isunified        ( wiced_bool_t* is_unified );
platform_result_t platform_otp_avsbitslen       ( uint16_t* avsbitslen );
platform_result_t platform_otp_cis_parse        ( platform_otp_region_t region, platform_otp_cis_parse_callback_func callback, void* context );
platform_result_t platform_otp_read_tag         ( platform_otp_region_t region, uint8_t tag, void* data, uint16_t* byte_len );
platform_result_t platform_otp_package_options  ( uint32_t* package_options );
platform_result_t platform_otp_read_word_unprotected   (uint32_t word_number, uint16_t *read_word);
platform_result_t platform_otp_read_bit_unprotected    (uint16_t bit_num, uint16_t *read_bit);

#ifdef OTP_DEBUG
platform_result_t platform_otp_dump             ( void );
platform_result_t platform_otp_dumpstats        ( void );
#endif

void*             platform_heap_malloc_record_caller  ( platform_heap_type_t heap, size_t bytes, const char* filename, int line );
void*             platform_heap_memalign_record_caller( platform_heap_type_t heap, size_t align, size_t bytes, const char* filename, int line );
void*             platform_heap_realloc_record_caller ( platform_heap_type_t heap, void* ptr, size_t size, const char* filename, int line );
void              platform_heap_free_record_caller    ( platform_heap_type_t heap, void* ptr, const char* filename, int line );

void              platform_udelay                     ( uint32_t usec );

void              platform_uart_toggle_txrx_interrupt ( platform_uart_port_t uart_port, wiced_bool_t tx, wiced_bool_t enable );
void              platform_uart_receive_byte          ( platform_uart_port_t port, uint8_t* dest );
wiced_bool_t      platform_uart_txrx_ready            ( platform_uart_port_t uart_port, wiced_bool_t tx );
wiced_bool_t      platform_uart_transmit_fifo_empty   ( platform_uart_port_t port );
uint32_t          platform_uart_irq_txrx_ready        ( platform_uart_port_t port );

platform_result_t platform_spi_core_clock_toggle      ( wiced_bool_t request );

platform_result_t platform_gspi_slave_init( platform_gspi_slave_driver_t *slave_driver );
void platform_gspi_slave_notify_master ( void );
platform_result_t platform_gspi_slave_receive_command( platform_gspi_slave_driver_t *slave_driver, uint32_t timeout_ms );
void platform_gspi_transfer_data( uint8_t *desc_address, uint8_t *source_address, uint32_t data_length );
void platform_gspi_slave_indicate_slave_ready( void );
void platform_gspi_set_shared_address( uint32_t address );

#ifdef __cplusplus
} /* extern "C" */
#endif
