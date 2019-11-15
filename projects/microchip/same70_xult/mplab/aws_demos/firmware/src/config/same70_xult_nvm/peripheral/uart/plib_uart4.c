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

#include "device.h"
#include "plib_uart4.h"

// *****************************************************************************
// *****************************************************************************
// Section: UART4 Implementation
// *****************************************************************************
// *****************************************************************************

UART_OBJECT uart4Obj;

void static UART4_ISR_RX_Handler( void )
{
    if(uart4Obj.rxBusyStatus == true)
    {
        while((UART_SR_RXRDY_Msk == (UART4_REGS->UART_SR& UART_SR_RXRDY_Msk)) && (uart4Obj.rxSize > uart4Obj.rxProcessedSize) )
        {
            uart4Obj.rxBuffer[uart4Obj.rxProcessedSize++] = (UART4_REGS->UART_RHR& UART_RHR_RXCHR_Msk);
        }

        /* Check if the buffer is done */
        if(uart4Obj.rxProcessedSize >= uart4Obj.rxSize)
        {
            uart4Obj.rxBusyStatus = false;

            /* Disable Read, Overrun, Parity and Framing error interrupts */
            UART4_REGS->UART_IDR = (UART_IDR_RXRDY_Msk | UART_IDR_FRAME_Msk | UART_IDR_PARE_Msk | UART_IDR_OVRE_Msk);

            if(uart4Obj.rxCallback != NULL)
            {
                uart4Obj.rxCallback(uart4Obj.rxContext);
            }
        }
    }
    else
    {
        /* Nothing to process */
        ;
    }
}

void static UART4_ISR_TX_Handler( void )
{
    if(uart4Obj.txBusyStatus == true)
    {
        while((UART_SR_TXRDY_Msk == (UART4_REGS->UART_SR & UART_SR_TXRDY_Msk)) && (uart4Obj.txSize > uart4Obj.txProcessedSize) )
        {
            UART4_REGS->UART_THR|= uart4Obj.txBuffer[uart4Obj.txProcessedSize++];
        }

        /* Check if the buffer is done */
        if(uart4Obj.txProcessedSize >= uart4Obj.txSize)
        {
            uart4Obj.txBusyStatus = false;
            UART4_REGS->UART_IDR = UART_IDR_TXEMPTY_Msk;

            if(uart4Obj.txCallback != NULL)
            {
                uart4Obj.txCallback(uart4Obj.txContext);
            }
        }
    }
    else
    {
        /* Nothing to process */
        ;
    }
}

void UART4_InterruptHandler( void )
{
    /* Error status */
    uint32_t errorStatus = (UART4_REGS->UART_SR & (UART_SR_OVRE_Msk | UART_SR_FRAME_Msk | UART_SR_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Client must call UARTx_ErrorGet() function to clear the errors */

        /* Disable Read, Overrun, Parity and Framing error interrupts */
        UART4_REGS->UART_IDR = (UART_IDR_RXRDY_Msk | UART_IDR_FRAME_Msk | UART_IDR_PARE_Msk | UART_IDR_OVRE_Msk);

        uart4Obj.rxBusyStatus = false;

        /* UART errors are normally associated with the receiver, hence calling
         * receiver callback */
        if( uart4Obj.rxCallback != NULL )
        {
            uart4Obj.rxCallback(uart4Obj.rxContext);
        }
    }

    /* Receiver status */
    if(UART_SR_RXRDY_Msk == (UART4_REGS->UART_SR& UART_SR_RXRDY_Msk))
    {
        UART4_ISR_RX_Handler();
    }

    /* Transmitter status */
    if(UART_SR_TXRDY_Msk == (UART4_REGS->UART_SR & UART_SR_TXRDY_Msk))
    {
        UART4_ISR_TX_Handler();
    }
}


void static UART4_ErrorClear( void )
{
    uint8_t dummyData = 0u;

    UART4_REGS->UART_CR = UART_CR_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while( UART_SR_RXRDY_Msk == (UART4_REGS->UART_SR& UART_SR_RXRDY_Msk) )
    {
        dummyData = (UART4_REGS->UART_RHR& UART_RHR_RXCHR_Msk);
    }

    /* Ignore the warning */
    (void)dummyData;
}

void UART4_Initialize( void )
{
    /* Reset UART4 */
    UART4_REGS->UART_CR = (UART_CR_RSTRX_Msk | UART_CR_RSTTX_Msk | UART_CR_RSTSTA_Msk);

    /* Enable UART4 */
    UART4_REGS->UART_CR = (UART_CR_TXEN_Msk | UART_CR_RXEN_Msk);

    /* Configure UART4 mode */
    UART4_REGS->UART_MR = ((UART_MR_BRSRCCK_PERIPH_CLK) | (UART_MR_PAR_NO) | (0 << UART_MR_FILTER_Pos));

    /* Configure UART4 Baud Rate */
    UART4_REGS->UART_BRGR = UART_BRGR_CD(81);

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
}

UART_ERROR UART4_ErrorGet( void )
{
    UART_ERROR errors = UART_ERROR_NONE;
    uint32_t status = UART4_REGS->UART_SR;

    errors = (UART_ERROR)(status & (UART_SR_OVRE_Msk | UART_SR_PARE_Msk | UART_SR_FRAME_Msk));

    if(errors != UART_ERROR_NONE)
    {
        UART4_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

bool UART4_SerialSetup( UART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = false;
    uint32_t baud = setup->baudRate;
    uint32_t brgVal = 0;
    uint32_t uartMode;

    if((uart4Obj.rxBusyStatus == true) || (uart4Obj.txBusyStatus == true))
    {
        /* Transaction is in progress, so return without updating settings */
        return false;
    }
    if (setup != NULL)
    {
        if(srcClkFreq == 0)
        {
            srcClkFreq = UART4_FrequencyGet();
        }

        /* Calculate BRG value */
        brgVal = srcClkFreq / (16 * baud);

        /* If the target baud rate is acheivable using this clock */
        if (brgVal <= 65535)
        {
            /* Configure UART4 mode */
            uartMode = UART4_REGS->UART_MR;
            uartMode &= ~UART_MR_PAR_Msk;
            UART4_REGS->UART_MR = uartMode | setup->parity ;

            /* Configure UART4 Baud Rate */
            UART4_REGS->UART_BRGR = UART_BRGR_CD(brgVal);

            status = true;
        }
    }

    return status;
}

bool UART4_Read( void *buffer, const size_t size )
{
    bool status = false;

    uint8_t * lBuffer = (uint8_t *)buffer;

    if(NULL != lBuffer)
    {
        /* Clear errors before submitting the request.
         * ErrorGet clears errors internally. */
        UART4_ErrorGet();

        /* Check if receive request is in progress */
        if(uart4Obj.rxBusyStatus == false)
        {
            uart4Obj.rxBuffer = lBuffer;
            uart4Obj.rxSize = size;
            uart4Obj.rxProcessedSize = 0;
            uart4Obj.rxBusyStatus = true;
            status = true;

            /* Enable Read, Overrun, Parity and Framing error interrupts */
            UART4_REGS->UART_IER = (UART_IER_RXRDY_Msk | UART_IER_FRAME_Msk | UART_IER_PARE_Msk | UART_IER_OVRE_Msk);
        }
    }

    return status;
}

bool UART4_Write( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t * lBuffer = (uint8_t *)buffer;

    if(NULL != lBuffer)
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
            if(UART_SR_TXRDY_Msk == (UART4_REGS->UART_SR & UART_SR_TXRDY_Msk))
            {
                UART4_REGS->UART_THR = (UART_THR_TXCHR(*lBuffer) & UART_THR_TXCHR_Msk);
                uart4Obj.txProcessedSize++;
            }

            UART4_REGS->UART_IER = UART_IER_TXEMPTY_Msk;
        }
    }

    return status;
}

void UART4_WriteCallbackRegister( UART_CALLBACK callback, uintptr_t context )
{
    uart4Obj.txCallback = callback;

    uart4Obj.txContext = context;
}

void UART4_ReadCallbackRegister( UART_CALLBACK callback, uintptr_t context )
{
    uart4Obj.rxCallback = callback;

    uart4Obj.rxContext = context;
}

bool UART4_WriteIsBusy( void )
{
    return uart4Obj.txBusyStatus;
}

bool UART4_ReadIsBusy( void )
{
    return uart4Obj.rxBusyStatus;
}

size_t UART4_WriteCountGet( void )
{
    return uart4Obj.txProcessedSize;
}

size_t UART4_ReadCountGet( void )
{
    return uart4Obj.rxProcessedSize;
}

