/*******************************************************************************
  USART1 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart1.c

  Summary:
    USART1 PLIB Implementation File

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
#include "plib_usart1.h"

// *****************************************************************************
// *****************************************************************************
// Section: USART1 Implementation
// *****************************************************************************
// *****************************************************************************

USART_OBJECT usart1Obj;

void static USART1_ISR_RX_Handler( void )
{
    if(usart1Obj.rxBusyStatus == true)
    {
        while((US_CSR_USART_RXRDY_Msk == (USART1_REGS->US_CSR& US_CSR_USART_RXRDY_Msk)) && (usart1Obj.rxSize > usart1Obj.rxProcessedSize) )
        {
            usart1Obj.rxBuffer[usart1Obj.rxProcessedSize++] = (USART1_REGS->US_RHR& US_RHR_RXCHR_Msk);
        }

        /* Check if the buffer is done */
        if(usart1Obj.rxProcessedSize >= usart1Obj.rxSize)
        {

            usart1Obj.rxBusyStatus = false;

            /* Disable Read, Overrun, Parity and Framing error interrupts */
            USART1_REGS->US_IDR = (US_IDR_USART_RXRDY_Msk | US_IDR_USART_FRAME_Msk | US_IDR_USART_PARE_Msk | US_IDR_USART_OVRE_Msk);

            if(usart1Obj.rxCallback != NULL)
            {
                usart1Obj.rxCallback(usart1Obj.rxContext);
            }
        }
    }
    else
    {
        /* Nothing to process */
        ;
    }

    return;
}

void static USART1_ISR_TX_Handler( void )
{
    if(usart1Obj.txBusyStatus == true)
    {
        while((US_CSR_USART_TXEMPTY_Msk == (USART1_REGS->US_CSR& US_CSR_USART_TXEMPTY_Msk)) && (usart1Obj.txSize > usart1Obj.txProcessedSize) )
        {
            USART1_REGS->US_THR|= usart1Obj.txBuffer[usart1Obj.txProcessedSize++];
        }

        /* Check if the buffer is done */
        if(usart1Obj.txProcessedSize >= usart1Obj.txSize)
        {
            usart1Obj.txBusyStatus = false;
            USART1_REGS->US_IDR = US_IDR_USART_TXEMPTY_Msk;

            if(usart1Obj.txCallback != NULL)
            {
                usart1Obj.txCallback(usart1Obj.txContext);
            }
        }
    }
    else
    {
        /* Nothing to process */
        ;
    }

    return;
}

void USART1_InterruptHandler( void )
{
    /* Error status */
    uint32_t errorStatus = (USART1_REGS->US_CSR & (US_CSR_USART_OVRE_Msk | US_CSR_USART_FRAME_Msk | US_CSR_USART_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Client must call USARTx_ErrorGet() function to clear the errors */

        /* Disable Read, Overrun, Parity and Framing error interrupts */
        USART1_REGS->US_IDR = (US_IDR_USART_RXRDY_Msk | US_IDR_USART_FRAME_Msk | US_IDR_USART_PARE_Msk | US_IDR_USART_OVRE_Msk);

        usart1Obj.rxBusyStatus = false;

        /* USART errors are normally associated with the receiver, hence calling
         * receiver callback */
        if( usart1Obj.rxCallback != NULL )
        {
            usart1Obj.rxCallback(usart1Obj.rxContext);
        }
    }

    /* Receiver status */
    if(US_CSR_USART_RXRDY_Msk == (USART1_REGS->US_CSR & US_CSR_USART_RXRDY_Msk))
    {
        USART1_ISR_RX_Handler();
    }

    /* Transmitter status */
    if(US_CSR_USART_TXRDY_Msk == (USART1_REGS->US_CSR & US_CSR_USART_TXRDY_Msk))
    {
        USART1_ISR_TX_Handler();
    }

    return;
}


void static USART1_ErrorClear( void )
{
    uint8_t dummyData = 0u;

    USART1_REGS->US_CR |= US_CR_USART_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while( US_CSR_USART_RXRDY_Msk == (USART1_REGS->US_CSR& US_CSR_USART_RXRDY_Msk) )
    {
        dummyData = (USART1_REGS->US_RHR& US_RHR_RXCHR_Msk);
    }

    /* Ignore the warning */
    (void)dummyData;

    return;
}

void USART1_Initialize( void )
{
    /* Reset USART1 */
    USART1_REGS->US_CR = (US_CR_USART_RSTRX_Msk | US_CR_USART_RSTTX_Msk | US_CR_USART_RSTSTA_Msk);

    /* Enable USART1 */
    USART1_REGS->US_CR = (US_CR_USART_TXEN_Msk | US_CR_USART_RXEN_Msk);

    /* Configure USART1 mode */
    USART1_REGS->US_MR = (US_MR_USART_USCLKS_MCK | US_MR_USART_CHRL_8_BIT | US_MR_USART_PAR_NO | US_MR_USART_NBSTOP_1_BIT | (0 << US_MR_USART_OVER_Pos));

    /* Configure USART1 Baud Rate */
    USART1_REGS->US_BRGR = US_BRGR_CD(81);

    /* Initialize instance object */
    usart1Obj.rxBuffer = NULL;
    usart1Obj.rxSize = 0;
    usart1Obj.rxProcessedSize = 0;
    usart1Obj.rxBusyStatus = false;
    usart1Obj.rxCallback = NULL;
    usart1Obj.txBuffer = NULL;
    usart1Obj.txSize = 0;
    usart1Obj.txProcessedSize = 0;
    usart1Obj.txBusyStatus = false;
    usart1Obj.txCallback = NULL;

    return;
}

USART_ERROR USART1_ErrorGet( void )
{
    USART_ERROR errors = USART_ERROR_NONE;
    uint32_t status = USART1_REGS->US_CSR;

    errors = (USART_ERROR)(status & (US_CSR_USART_OVRE_Msk | US_CSR_USART_PARE_Msk | US_CSR_USART_FRAME_Msk));

    if(errors != USART_ERROR_NONE)
    {
        USART1_ErrorClear();
    }

    /* All errors are cleared, but send the previous error state */
    return errors;
}

bool USART1_SerialSetup( USART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    uint32_t baud = setup->baudRate;
    uint32_t brgVal = 0;
    uint32_t overSampVal = 0;
    uint32_t usartMode;
    bool status = false;

    if((usart1Obj.rxBusyStatus == true) || (usart1Obj.txBusyStatus == true))
    {
        /* Transaction is in progress, so return without updating settings */
        return false;
    }

    if (setup != NULL)
    {
        baud = setup->baudRate;
        if(srcClkFreq == 0)
        {
            srcClkFreq = USART1_FrequencyGet();
        }

        /* Calculate BRG value */
        if (srcClkFreq >= (16 * baud))
        {
            brgVal = (srcClkFreq / (16 * baud));
        }
        else if (srcClkFreq >= (8 * baud))
        {
            brgVal = (srcClkFreq / (8 * baud));
            overSampVal = US_MR_USART_OVER(1);
        }
        else
        {
            return false;
        }
        if (brgVal > 65535)
        {
            /* The requested baud is so low that the ratio of srcClkFreq to baud exceeds the 16-bit register value of CD register */
            return false;
        }

        /* Configure USART1 mode */
        usartMode = USART1_REGS->US_MR;
        usartMode &= ~(US_MR_USART_CHRL_Msk | US_MR_USART_MODE9_Msk | US_MR_USART_PAR_Msk | US_MR_USART_NBSTOP_Msk | US_MR_USART_OVER_Msk);
        USART1_REGS->US_MR = usartMode | ((uint32_t)setup->dataWidth | (uint32_t)setup->parity | (uint32_t)setup->stopBits | (uint32_t)overSampVal);

        /* Configure USART1 Baud Rate */
        USART1_REGS->US_BRGR = US_BRGR_CD(brgVal);
        status = true;
    }

    return status;
}

bool USART1_Read( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t * lBuffer = (uint8_t *)buffer;

    if(NULL != lBuffer)
    {
        /* Clear errors before submitting the request.
         * ErrorGet clears errors internally. */
        USART1_ErrorGet();

        /* Check if receive request is in progress */
        if(usart1Obj.rxBusyStatus == false)
        {
            usart1Obj.rxBuffer = lBuffer;
            usart1Obj.rxSize = size;
            usart1Obj.rxProcessedSize = 0;
            usart1Obj.rxBusyStatus = true;
            status = true;

            /* Enable Read, Overrun, Parity and Framing error interrupts */
            USART1_REGS->US_IER = (US_IER_USART_RXRDY_Msk | US_IER_USART_FRAME_Msk | US_IER_USART_PARE_Msk | US_IER_USART_OVRE_Msk);
        }
    }

    return status;
}

bool USART1_Write( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t * lBuffer = (uint8_t *)buffer;

    if(NULL != lBuffer)
    {
        /* Check if transmit request is in progress */
        if(usart1Obj.txBusyStatus == false)
        {
            usart1Obj.txBuffer = lBuffer;
            usart1Obj.txSize = size;
            usart1Obj.txProcessedSize = 0;
            usart1Obj.txBusyStatus = true;
            status = true;

            /* Initiate the transfer by sending first byte */
            if(US_CSR_USART_TXRDY_Msk == (USART1_REGS->US_CSR & US_CSR_USART_TXRDY_Msk))
            {
                USART1_REGS->US_THR = (US_THR_TXCHR(*lBuffer) & US_THR_TXCHR_Msk);
                usart1Obj.txProcessedSize++;
            }

            USART1_REGS->US_IER = US_IER_USART_TXEMPTY_Msk;
        }
    }

    return status;
}


void USART1_WriteCallbackRegister( USART_CALLBACK callback, uintptr_t context )
{
    usart1Obj.txCallback = callback;

    usart1Obj.txContext = context;
}

void USART1_ReadCallbackRegister( USART_CALLBACK callback, uintptr_t context )
{
    usart1Obj.rxCallback = callback;

    usart1Obj.rxContext = context;
}

bool USART1_WriteIsBusy( void )
{
    return usart1Obj.txBusyStatus;
}

bool USART1_ReadIsBusy( void )
{
    return usart1Obj.rxBusyStatus;
}

size_t USART1_WriteCountGet( void )
{
    return usart1Obj.txProcessedSize;
}

size_t USART1_ReadCountGet( void )
{
    return usart1Obj.rxProcessedSize;
}

