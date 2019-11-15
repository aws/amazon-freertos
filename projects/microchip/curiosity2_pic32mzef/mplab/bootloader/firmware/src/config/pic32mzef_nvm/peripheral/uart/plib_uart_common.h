/*******************************************************************************
  UART PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_uart_common.h

  Summary:
    UART PLIB Common Header File

  Description:
    This file has prototype of all the interfaces which are common for all the
    UART peripherals.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef PLIB_UART_COMMON_H
#define PLIB_UART_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


/* Provide C++ Compatibility */
#ifdef __cplusplus

    extern "C" {

#endif

#define UART_RXFIFO_DEPTH       9

typedef enum
{
    UART_PARITY_NONE = 0x00,

    UART_PARITY_EVEN = 0x02,

    UART_PARITY_ODD =  0x04,

      /* Force the compiler to reserve 32-bit space for each enum */
    UART_PARITY_INVALID = 0xFFFFFFFF

} UART_PARITY;

typedef enum
{
    UART_DATA_8_BIT = 0x00,

    UART_DATA_9_BIT = 0x06,

    /* Force the compiler to reserve 32-bit memory for each enum */
    UART_DATA_INVALID = 0xFFFFFFFF

} UART_DATA;

typedef enum
{
    UART_STOP_1_BIT = 0x00,

    UART_STOP_2_BIT = 0x01,

    /* Force the compiler to reserve 32-bit memory for each enum */
    UART_STOP_INVALID = 0xFFFFFFFF

} UART_STOP;

typedef struct
{
    uint32_t baudRate;

    UART_PARITY parity;

    UART_DATA dataWidth;

    UART_STOP stopBits;

} UART_SERIAL_SETUP;

// *****************************************************************************
/* UART Errors
*/

typedef enum
{
    UART_ERROR_NONE = 0,
    UART_ERROR_OVERRUN = 0x00000002,
    UART_ERROR_FRAMING = 0x00000004,
    UART_ERROR_PARITY  = 0x00000008

} UART_ERROR;

typedef void (* UART_CALLBACK)( uintptr_t context );

// *****************************************************************************
// *****************************************************************************
// Section: Local: **** Do Not Use ****
// *****************************************************************************
// *****************************************************************************

typedef struct
{
    uint8_t *               txBuffer;
    size_t                  txSize;
    size_t                  txProcessedSize;
    UART_CALLBACK           txCallback;
    uintptr_t               txContext;
    bool                    txBusyStatus;

    uint8_t *               rxBuffer;
    size_t                  rxSize;
    size_t                  rxProcessedSize;
    UART_CALLBACK           rxCallback;
    uintptr_t               rxContext;
    bool                    rxBusyStatus;

} UART_OBJECT ;

/* Provide C++ Compatibility */
#ifdef __cplusplus

    }

#endif

#endif // PLIB_UART_COMMON_H
