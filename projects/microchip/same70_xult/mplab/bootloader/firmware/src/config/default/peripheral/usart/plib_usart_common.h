/*******************************************************************************
  USART PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart.h

  Summary:
    USART PLIB Global Header File

  Description:
    None

*******************************************************************************/

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

#ifndef PLIB_USART_COMMON_H
#define PLIB_USART_COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

typedef enum
{
    USART_ERROR_NONE = 0,
    USART_ERROR_OVERRUN = US_CSR_USART_OVRE_Msk,
    USART_ERROR_PARITY = US_CSR_USART_PARE_Msk,
    USART_ERROR_FRAMING = US_CSR_USART_FRAME_Msk

} USART_ERROR;

typedef enum
{
    USART_DATA_5_BIT = US_MR_USART_CHRL_5_BIT,
    USART_DATA_6_BIT = US_MR_USART_CHRL_6_BIT,
    USART_DATA_7_BIT = US_MR_USART_CHRL_7_BIT,
    USART_DATA_8_BIT = US_MR_USART_CHRL_8_BIT,
    USART_DATA_9_BIT = US_MR_USART_MODE9_Msk,

    /* Force the compiler to reserve 32-bit memory for each enum */
    USART_DATA_INVALID = 0xFFFFFFFF

} USART_DATA;

typedef enum
{
    USART_PARITY_NONE = US_MR_USART_PAR_NO,
    USART_PARITY_ODD = US_MR_USART_PAR_ODD,
    USART_PARITY_EVEN = US_MR_USART_PAR_EVEN,
    USART_PARITY_MARK = US_MR_USART_PAR_MARK,
    USART_PARITY_SPACE = US_MR_USART_PAR_SPACE,
    USART_PARITY_MULTIDROP = US_MR_USART_PAR_MULTIDROP,

    /* Force the compiler to reserve 32-bit memory for each enum */
    USART_PARITY_INVALID = 0xFFFFFFFF

} USART_PARITY;

typedef enum
{
    USART_STOP_1_BIT = US_MR_USART_NBSTOP_1_BIT,
    USART_STOP_1_5_BIT = US_MR_USART_NBSTOP_1_5_BIT,
    USART_STOP_2_BIT = US_MR_USART_NBSTOP_2_BIT,

    /* Force the compiler to reserve 32-bit memory for each enum */
    USART_STOP_INVALID = 0xFFFFFFFF

} USART_STOP;

typedef struct
{
    uint32_t baudRate;
    USART_PARITY parity;
    USART_DATA dataWidth;
    USART_STOP stopBits;

} USART_SERIAL_SETUP;

typedef void (* USART_CALLBACK)( uintptr_t context );


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
    USART_CALLBACK          txCallback;
    uintptr_t               txContext;
    bool                    txBusyStatus;

    uint8_t *               rxBuffer;
    size_t                  rxSize;
    size_t                  rxProcessedSize;
    USART_CALLBACK          rxCallback;
    uintptr_t               rxContext;
    bool                    rxBusyStatus;

} USART_OBJECT ;

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_USART_COMMON_H
