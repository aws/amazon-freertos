/*******************************************************************************
  UART4 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_uart4.c

  Summary:
    UART4 PLIB Implementation File

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
#include "plib_uart4.h"

// *****************************************************************************
// *****************************************************************************
// Section: UART4 Implementation
// *****************************************************************************
// *****************************************************************************

UART_OBJECT uart4Obj;

void static UART4_ErrorClear( void )
{
    /* rxBufferLen = (FIFO level + RX register) */
    uint8_t rxBufferLen = UART_RXFIFO_DEPTH;
    uint8_t dummyData = 0u;

    /* If it's a overrun error then clear it to flush FIFO */
    if(U4STA & _U4STA_OERR_MASK)
    {
        U4STACLR = _U4STA_OERR_MASK;
    }

    /* Read existing error bytes from FIFO to clear parity and framing error flags */
    while(U4STA & (_U4STA_FERR_MASK | _U4STA_PERR_MASK))
    {
        dummyData = (uint8_t )(U4RXREG );
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

    /* Clear error interrupt flag */
    IFS5CLR = _IFS5_U4EIF_MASK;

    /* Clear up the receive interrupt flag so that RX interrupt is not
     * triggered for error bytes */
    IFS5CLR = _IFS5_U4RXIF_MASK;

    return;
}

void UART4_Initialize( void )
{
    /* Set up UxMODE bits */
    /* STSEL  = 0 */
    /* PDSEL = 0 */

    U4MODE = 0x8;

    /* Enable UART4 Receiver and Transmitter */
    U4STASET = (_U4STA_UTXEN_MASK | _U4STA_URXEN_MASK);

    /* BAUD Rate register Setup */
    U4BRG = 216;

    /* Disable Interrupts */
    IEC5CLR = _IEC5_U4EIE_MASK;

    IEC5CLR = _IEC5_U4RXIE_MASK;

    IEC5CLR = _IEC5_U4TXIE_MASK;

    /* Initialize instance object */
    uart4Obj.rxBuffer = NULL;
    uart4Obj.rxSize = 0;
    uart4Obj.rxProcessedSize = 0;
    uart4Obj.rxBusyStatus = false;
    uart4Obj.rxCallback = NULL;
    uart4Obj.txBuffer = NULL;
    uart4Obj.txSize = 0;
    uart4Obj.txProcessedSize = 0;
    uart4Obj.txBusyStatus = false;
    uart4Obj.txCallback = NULL;

    /* Turn ON UART4 */
    U4MODESET = _U4MODE_ON_MASK;
}

bool UART4_SerialSetup( UART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = false;
    uint32_t baud = setup->baudRate;
    uint32_t brgValHigh = 0;
    uint32_t brgValLow = 0;
    uint32_t brgVal = 0;
    uint32_t uartMode;

    if((uart4Obj.rxBusyStatus == true) || (uart4Obj.txBusyStatus == true))
    {
        /* Transaction is in progress, so return without updating settings */
        return status;
    }

    if (setup != NULL)
    {
        if(srcClkFreq == 0)
        {
            srcClkFreq = UART4_FrequencyGet();
        }

        /* Calculate BRG value */
        brgValLow = ((srcClkFreq / baud) >> 4) - 1;
        brgValHigh = ((srcClkFreq / baud) >> 2) - 1;

        /* Check if the baud value can be set with low baud settings */
        if((brgValHigh >= 0) && (brgValHigh <= UINT16_MAX))
        {
            brgVal =  (((srcClkFreq >> 2) + (baud >> 1)) / baud ) - 1;
            U4MODESET = _U4MODE_BRGH_MASK;
        }
        else if ((brgValLow >= 0) && (brgValLow <= UINT16_MAX))
        {
            brgVal = ( ((srcClkFreq >> 4) + (baud >> 1)) / baud ) - 1;
            U4MODECLR = _U4MODE_BRGH_MASK;
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
               /* Configure UART4 mode */
               uartMode = U4MODE;
               uartMode &= ~_U4MODE_PDSEL_MASK;
               U4MODE = uartMode | setup->dataWidth;
            }
        }
        else
        {
            /* Configure UART4 mode */
            uartMode = U4MODE;
            uartMode &= ~_U4MODE_PDSEL_MASK;
            U4MODE = uartMode | setup->parity ;
        }

        /* Configure UART4 mode */
        uartMode = U4MODE;
        uartMode &= ~_U4MODE_STSEL_MASK;
        U4MODE = uartMode | setup->stopBits ;

        /* Configure UART4 Baud Rate */
        U4BRG = brgVal;

        status = true;
    }

    return status;
}

bool UART4_Read(void* buffer, const size_t size )
{
    bool status = false;
    uint8_t* lBuffer = (uint8_t* )buffer;

    if(lBuffer != NULL)
    {
        /* Check if receive request is in progress */
        if(uart4Obj.rxBusyStatus == false)
        {
            /* Clear errors before submitting the request.
             * ErrorGet clears errors internally. */
            UART4_ErrorGet();

            uart4Obj.rxBuffer = lBuffer;
            uart4Obj.rxSize = size;
            uart4Obj.rxProcessedSize = 0;
            uart4Obj.rxBusyStatus = true;
            status = true;

            /* Enable UART4_FAULT Interrupt */
            IEC5SET = _IEC5_U4EIE_MASK;

            /* Enable UART4_RX Interrupt */
            IEC5SET = _IEC5_U4RXIE_MASK;
        }
    }

    return status;
}

bool UART4_Write( void* buffer, const size_t size )
{
    bool status = false;
    uint8_t* lBuffer = (uint8_t*)buffer;

    if(lBuffer != NULL)
    {
        /* Check if transmit request is in progress */
        if(uart4Obj.txBusyStatus == false)
        {
            uart4Obj.txBuffer = lBuffer;
            uart4Obj.txSize = size;
            uart4Obj.txProcessedSize = 0;
            uart4Obj.txBusyStatus = true;
            status = true;

            /* Initiate the transfer by sending first byte */
            if(!(U4STA & _U4STA_UTXBF_MASK))
            {
                U4TXREG = *lBuffer;
                uart4Obj.txProcessedSize++;
            }

            IEC5SET = _IEC5_U4TXIE_MASK;
        }
    }

    return status;
}

UART_ERROR UART4_ErrorGet( void )
{
    UART_ERROR errors = UART_ERROR_NONE;
    uint32_t status = U4STA;

    errors = (UART_ERROR)(status & (_U4STA_OERR_MASK | _U4STA_FERR_MASK | _U4STA_PERR_MASK));

    if(errors != UART_ERROR_NONE)
    {
        UART4_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

void UART4_ReadCallbackRegister( UART_CALLBACK callback, uintptr_t context )
{
    uart4Obj.rxCallback = callback;

    uart4Obj.rxContext = context;
}

bool UART4_ReadIsBusy( void )
{
    return uart4Obj.rxBusyStatus;
}

size_t UART4_ReadCountGet( void )
{
    return uart4Obj.rxProcessedSize;
}

void UART4_WriteCallbackRegister( UART_CALLBACK callback, uintptr_t context )
{
    uart4Obj.txCallback = callback;

    uart4Obj.txContext = context;
}

bool UART4_WriteIsBusy( void )
{
    return uart4Obj.txBusyStatus;
}

size_t UART4_WriteCountGet( void )
{
    return uart4Obj.txProcessedSize;
}

void UART4_FAULT_InterruptHandler (void)
{
    /* Disable the fault interrupt */
    IEC5CLR = _IEC5_U4EIE_MASK;
    /* Disable the receive interrupt */
    IEC5CLR = _IEC5_U4RXIE_MASK;

    /* Clear rx status */
    uart4Obj.rxBusyStatus = false;

    /* Client must call UARTx_ErrorGet() function to clear the errors */
    if( uart4Obj.rxCallback != NULL )
    {
        uart4Obj.rxCallback(uart4Obj.rxContext);
    }
}

void UART4_RX_InterruptHandler (void)
{
    if(uart4Obj.rxBusyStatus == true)
    {
        while((_U4STA_URXDA_MASK == (U4STA & _U4STA_URXDA_MASK)) && (uart4Obj.rxSize > uart4Obj.rxProcessedSize) )
        {
            uart4Obj.rxBuffer[uart4Obj.rxProcessedSize++] = (uint8_t )(U4RXREG);
        }

        /* Clear UART4 RX Interrupt flag after reading data buffer */
        IFS5CLR = _IFS5_U4RXIF_MASK;

        /* Check if the buffer is done */
        if(uart4Obj.rxProcessedSize >= uart4Obj.rxSize)
        {
            uart4Obj.rxBusyStatus = false;

            /* Disable the receive interrupt */
            IEC5CLR = _IEC5_U4RXIE_MASK;

            if(uart4Obj.rxCallback != NULL)
            {
                uart4Obj.rxCallback(uart4Obj.rxContext);
            }
        }
    }
    else
    {
        // Nothing to process
        ;
    }
}

void UART4_TX_InterruptHandler (void)
{
    if(uart4Obj.txBusyStatus == true)
    {
        while((!(U4STA & _U4STA_UTXBF_MASK)) && (uart4Obj.txSize > uart4Obj.txProcessedSize) )
        {
            U4TXREG = uart4Obj.txBuffer[uart4Obj.txProcessedSize++];
        }

        /* Clear UART4TX Interrupt flag after writing to buffer */
        IFS5CLR = _IFS5_U4TXIF_MASK;

        /* Check if the buffer is done */
        if(uart4Obj.txProcessedSize >= uart4Obj.txSize)
        {
            uart4Obj.txBusyStatus = false;

            /* Disable the transmit interrupt, to avoid calling ISR continuously */
            IEC5CLR = _IEC5_U4TXIE_MASK;

            if(uart4Obj.txCallback != NULL)
            {
                uart4Obj.txCallback(uart4Obj.txContext);
            }
        }
    }
    else
    {
        // Nothing to process
        ;
    }
}


