/*******************************************************************************
  Console System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console_uart_definitions.h

  Summary:
    Console System Service local UART I/O device implementation.

  Description:
    This file contains the definitions required by the UART console I/O device.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END

#ifndef SYS_CONSOLE_UART_DEFINITIONS_H    /* Guard against multiple inclusion */
#define SYS_CONSOLE_UART_DEFINITIONS_H

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "system/int/sys_int.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

typedef struct
{
    char* pBuffer;
    size_t size;
} QElement;

typedef struct
{
    QElement* pQElementArr;
    QElement curQElement;
    uint32_t maxQElements;
    uint32_t inIndex;
    uint32_t outIndex;
} Queue;

typedef void (*SYS_CONSOLE_UART_PLIB_CALLBACK)( uintptr_t context );
typedef void (*SYS_CONSOLE_UART_PLIB_REGISTER_CALLBACK_READ)(SYS_CONSOLE_UART_PLIB_CALLBACK callback, uintptr_t context);
typedef void (*SYS_CONSOLE_UART_PLIB_REGISTER_CALLBACK_WRITE)(SYS_CONSOLE_UART_PLIB_CALLBACK callback, uintptr_t context);
typedef bool (*SYS_CONSOLE_UART_PLIB_READ)(void* buffer, size_t size);
typedef bool (*SYS_CONSOLE_UART_PLIB_WRITE)(void* buffer, size_t size);
typedef uint32_t (*SYS_CONSOLE_UART_PLIB_ERROR_GET)(void);

typedef struct
{
    SYS_CONSOLE_UART_PLIB_READ read;
    SYS_CONSOLE_UART_PLIB_WRITE write;
    SYS_CONSOLE_UART_PLIB_REGISTER_CALLBACK_READ readCallbackRegister;
    SYS_CONSOLE_UART_PLIB_REGISTER_CALLBACK_WRITE writeCallbackRegister;
    SYS_CONSOLE_UART_PLIB_ERROR_GET errorGet;
} SYS_CONSOLE_UART_PLIB_INTERFACE;

typedef struct
{
    int32_t         usartTxReadyInt;
    int32_t         usartTxCompleteInt;
    int32_t         usartRxCompleteInt;
    int32_t         usartErrorInt;
} SYS_CONSOLE_UART_MULTI_INT_SRC;

typedef union
{
    SYS_CONSOLE_UART_MULTI_INT_SRC              multi;
    int32_t                                     usartInterrupt;
} SYS_CONSOLE_UART_INT_SRC;

typedef struct
{
    bool                                        isSingleIntSrc;
    SYS_CONSOLE_UART_INT_SRC                    intSources;
} SYS_CONSOLE_UART_INTERRUPT_SOURCES;

typedef struct
{
    const SYS_CONSOLE_UART_PLIB_INTERFACE* uartPLIB;

    QElement* readQueueElementsArr;

    QElement* writeQueueElementsArr;

    uint32_t readQueueDepth;

    uint32_t writeQueueDepth;

    const SYS_CONSOLE_UART_INTERRUPT_SOURCES* interruptSources;

} SYS_CONSOLE_UART_INIT_DATA;

// DOM-IGNORE-BEGIN
#ifdef __cplusplus

    }

#endif
// DOM-IGNORE-END

#endif /* SYS_CONSOLE_UART_DEFINITIONS_H */