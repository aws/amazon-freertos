/*
 * $ Copyright Cypress Semiconductor $
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WICED_GPIO_1,
    WICED_GPIO_2,
    WICED_GPIO_3,
    WICED_GPIO_4,
    WICED_GPIO_5,
    WICED_GPIO_6,
    WICED_GPIO_7,
    WICED_GPIO_8,
    WICED_GPIO_9,
    WICED_GPIO_10,
    WICED_GPIO_11,
    WICED_GPIO_12,
    WICED_GPIO_13,
    WICED_GPIO_14,
    WICED_GPIO_15,
    WICED_GPIO_16,
    WICED_GPIO_17,
    WICED_GPIO_18,
    WICED_GPIO_19,
    WICED_GPIO_20,
    WICED_GPIO_21,
    WICED_GPIO_22,
    WICED_GPIO_23,
    WICED_GPIO_24,
    WICED_GPIO_25,
    WICED_GPIO_26,
    WICED_GPIO_27,
    WICED_GPIO_28,
    WICED_GPIO_29,
    WICED_GPIO_30,
    WICED_GPIO_31,
    WICED_GPIO_32,
    WICED_GPIO_33,
    WICED_GPIO_34,
    WICED_GPIO_35,
    WICED_GPIO_36,
    WICED_GPIO_37,
    WICED_GPIO_38,
    WICED_GPIO_39,
    WICED_GPIO_40,
    WICED_GPIO_41,
    WICED_GPIO_42,
    WICED_GPIO_43,
    WICED_GPIO_44,
    WICED_GPIO_45,
    WICED_GPIO_46,
    WICED_GPIO_47,
    WICED_GPIO_48,
    WICED_GPIO_49,
    WICED_GPIO_50,
    WICED_GPIO_51,
    WICED_GPIO_52,
    WICED_GPIO_53,
    WICED_GPIO_54,
    WICED_GPIO_55,
    WICED_GPIO_56,
    WICED_GPIO_57,
    WICED_GPIO_58,
    WICED_GPIO_59,
    WICED_GPIO_60,
    WICED_GPIO_61,
    WICED_GPIO_62,
    WICED_GPIO_63,
    WICED_GPIO_64,
    WICED_GPIO_65,
    WICED_GPIO_66,
    WICED_GPIO_67,
    WICED_GPIO_68,
    WICED_GPIO_69,
    WICED_GPIO_70,
    WICED_GPIO_71,
    WICED_GPIO_72,
    WICED_GPIO_73,
    WICED_GPIO_74,
    WICED_GPIO_75,
    WICED_GPIO_76,
    WICED_GPIO_77,
    WICED_GPIO_78,
    WICED_GPIO_79,
    WICED_GPIO_80,
    WICED_GPIO_81,
    WICED_GPIO_82,
    WICED_GPIO_83,
    WICED_GPIO_84,
    WICED_GPIO_85,
    WICED_GPIO_86,
    WICED_GPIO_87,
    WICED_GPIO_88,
    WICED_GPIO_89,
    WICED_GPIO_90,
    WICED_GPIO_91,
    WICED_GPIO_92,
    WICED_GPIO_93,
    WICED_GPIO_94,
    WICED_GPIO_95,
    WICED_GPIO_96,
    WICED_GPIO_97,
    WICED_GPIO_98,
    WICED_GPIO_99,
    WICED_GPIO_100,
    WICED_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    WICED_GPIO_32BIT = 0x7FFFFFFF,
} wiced_gpio_t;

typedef enum
{
    WICED_UART_1,
    WICED_UART_2,
    WICED_UART_3,
    WICED_UART_4,
    WICED_UART_5,
    WICED_UART_6,
    WICED_UART_7,
    WICED_UART_8,
    WICED_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
    WICED_UART_32BIT = 0x7FFFFFFF,
} wiced_uart_t;

#ifdef __cplusplus
} /*extern "C" */
#endif
