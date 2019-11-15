/*******************************************************************************
  UART1 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_uart1.c

  Summary:
    UART1 PLIB Implementation File

  Description:
    None

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

#include "device.h"
#include "plib_uart1.h"

// *****************************************************************************
// *****************************************************************************
// Section: UART1 Implementation
// *****************************************************************************
// *****************************************************************************


void static UART1_ErrorClear( void )
{
    /* rxBufferLen = (FIFO level + RX register) */
    uint8_t rxBufferLen = UART_RXFIFO_DEPTH;
    uint8_t dummyData = 0u;

    /* If it's a overrun error then clear it to flush FIFO */
    if(U1STA & _U1STA_OERR_MASK)
    {
        U1STACLR = _U1STA_OERR_MASK;
    }

    /* Read existing error bytes from FIFO to clear parity and framing error flags */
    while(U1STA & (_U1STA_FERR_MASK | _U1STA_PERR_MASK))
    {
        dummyData = (uint8_t )(U1RXREG );
        rxBufferLen--;

        /* Try to flush error bytes for one full FIFO and exit instead of
         * blocking here if more error bytes are received */
        if(rxBufferLen == 0u)
        {
            break;
        }
    }

    // Ignore the warning
    (void)dummyData;

    return;
}

void UART1_Initialize( void )
{
    /* Set up UxMODE bits */
    /* STSEL  = 0 */
    /* PDSEL = 0 */

    U1MODE = 0x8;

    /* Enable UART1 Receiver and Transmitter */
    U1STASET = (_U1STA_UTXEN_MASK | _U1STA_URXEN_MASK);

    /* BAUD Rate register Setup */
    U1BRG = 216;

    /* Turn ON UART1 */
    U1MODESET = _U1MODE_ON_MASK;
}

bool UART1_SerialSetup( UART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = false;
    uint32_t baud = setup->baudRate;
    uint32_t brgValHigh = 0;
    uint32_t brgValLow = 0;
    uint32_t brgVal = 0;
    uint32_t uartMode;

    if (setup != NULL)
    {
        if(srcClkFreq == 0)
        {
            srcClkFreq = UART1_FrequencyGet();
        }

        /* Calculate BRG value */
        brgValLow = ((srcClkFreq / baud) >> 4) - 1;
        brgValHigh = ((srcClkFreq / baud) >> 2) - 1;

        /* Check if the baud value can be set with low baud settings */
        if((brgValHigh >= 0) && (brgValHigh <= UINT16_MAX))
        {
            brgVal =  (((srcClkFreq >> 2) + (baud >> 1)) / baud ) - 1;
            U1MODESET = _U1MODE_BRGH_MASK;
        }
        else if ((brgValLow >= 0) && (brgValLow <= UINT16_MAX))
        {
            brgVal = ( ((srcClkFreq >> 4) + (baud >> 1)) / baud ) - 1;
            U1MODECLR = _U1MODE_BRGH_MASK;
        }
        else
        {
            return status;
        }

        if(setup->dataWidth == UART_DATA_9_BIT)
        {
            if(setup->parity != UART_PARITY_NONE)
            {
               return status;
            }
            else
            {
               /* Configure UART1 mode */
               uartMode = U1MODE;
               uartMode &= ~_U1MODE_PDSEL_MASK;
               U1MODE = uartMode | setup->dataWidth;
            }
        }
        else
        {
            /* Configure UART1 mode */
            uartMode = U1MODE;
            uartMode &= ~_U1MODE_PDSEL_MASK;
            U1MODE = uartMode | setup->parity ;
        }

        /* Configure UART1 mode */
        uartMode = U1MODE;
        uartMode &= ~_U1MODE_STSEL_MASK;
        U1MODE = uartMode | setup->stopBits ;

        /* Configure UART1 Baud Rate */
        U1BRG = brgVal;

        status = true;
    }

    return status;
}

bool UART1_Read(void* buffer, const size_t size )
{
    bool status = false;
    uint8_t* lBuffer = (uint8_t* )buffer;
    uint32_t errorStatus = 0;
    size_t processedSize = 0;

    if(lBuffer != NULL)
    {
        /* Clear errors before submitting the request.
         * ErrorGet clears errors internally. */
        UART1_ErrorGet();

        while( size > processedSize )
        {
            /* Error status */
            errorStatus = (U1STA & (_U1STA_OERR_MASK | _U1STA_FERR_MASK | _U1STA_PERR_MASK));

            if(errorStatus != 0)
            {
                break;
            }

            /* Receiver buffer has data */
            if((U1STA & _U1STA_URXDA_MASK) == _U1STA_URXDA_MASK)
            {
                *lBuffer++ = (U1RXREG );
                processedSize++;
            }
        }

        if(size == processedSize)
        {
            status = true;
        }
    }

    return status;
}

bool UART1_Write( void* buffer, const size_t size )
{
    bool status = false;
    uint8_t* lBuffer = (uint8_t*)buffer;
    size_t processedSize = 0;

    if(lBuffer != NULL)
    {
        while( size > processedSize )
        {
            if(!(U1STA & _U1STA_UTXBF_MASK))
            {
                U1TXREG = *lBuffer++;
                processedSize++;
            }
        }

        status = true;
    }

    return status;
}

UART_ERROR UART1_ErrorGet( void )
{
    UART_ERROR errors = UART_ERROR_NONE;
    uint32_t status = U1STA;

    errors = (UART_ERROR)(status & (_U1STA_OERR_MASK | _U1STA_FERR_MASK | _U1STA_PERR_MASK));

    if(errors != UART_ERROR_NONE)
    {
        UART1_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

void UART1_WriteByte(int data)
{
    while ((U1STA & _U1STA_UTXBF_MASK));

    U1TXREG = data;
}

bool UART1_TransmitterIsReady( void )
{
    bool status = false;

    if(!(U1STA & _U1STA_UTXBF_MASK))
    {
        status = true;
    }

    return status;
}

bool UART1_TransmitComplete( void )
{
    bool transmitComplete = false;

    if((U1STA & _U1STA_TRMT_MASK))
    {
        transmitComplete = true;
    }

    return transmitComplete;
}

int UART1_ReadByte( void )
{
    return(U1RXREG);
}

bool UART1_ReceiverIsReady( void )
{
    bool status = false;

    if(_U1STA_URXDA_MASK == (U1STA & _U1STA_URXDA_MASK))
    {
        status = true;
    }

    return status;
}
