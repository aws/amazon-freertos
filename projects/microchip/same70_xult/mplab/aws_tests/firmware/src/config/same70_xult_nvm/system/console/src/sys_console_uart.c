/*******************************************************************************
  SYS UART CONSOLE Support Layer

  File Name:
    sys_console_uart.c

  Summary:
    SYS UART CONSOLE Support Layer

  Description:
    This file contains the SYS UART CONSOLE support layer logic.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/console/sys_console.h"
#include "sys_console_uart.h"
#include "configuration.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Variable Definitions
// *****************************************************************************
// *****************************************************************************

const SYS_CONSOLE_DEV_DESC sysConsoleUARTDevDesc =
{
    .consoleDevice = SYS_CONSOLE_DEV_USART,
    .intent = DRV_IO_INTENT_READWRITE,
    .init = Console_UART_Initialize,
    .read = Console_UART_Read,
    .write = Console_UART_Write,
    .callbackRegister = Console_UART_RegisterCallback,
    .task = Console_UART_Tasks,
    .status = Console_UART_Status,
    .flush = Console_UART_Flush
};

static CONSOLE_UART_DATA gConsoleUartData[SYS_CONSOLE_UART_MAX_INSTANCES];

#define CONSOLE_UART_GET_INSTANCE(index)    (index >= SYS_CONSOLE_UART_MAX_INSTANCES)? NULL : &gConsoleUartData[index]

static void Console_UART_DisableInterrupts(CONSOLE_UART_DATA* pConsoleUartData)
{
    bool interruptStatus;
    const SYS_CONSOLE_UART_INTERRUPT_SOURCES* intInfo = pConsoleUartData->interruptSources;
    const SYS_CONSOLE_UART_MULTI_INT_SRC* multiVector = &intInfo->intSources.multi;

    if (intInfo->isSingleIntSrc == true)
    {
        /* Disable USART interrupt */
        pConsoleUartData->usartInterruptStatus = SYS_INT_SourceDisable((INT_SOURCE)intInfo->intSources.usartInterrupt);
    }
    else
    {
        interruptStatus = SYS_INT_Disable();

        /* Disable USART interrupt sources */
        if(multiVector->usartTxReadyInt != -1)
        {
            pConsoleUartData->usartTxReadyIntStatus = SYS_INT_SourceDisable((INT_SOURCE)multiVector->usartTxReadyInt);
        }

        if(multiVector->usartTxCompleteInt != -1)
        {
            pConsoleUartData->usartTxCompleteIntStatus = SYS_INT_SourceDisable((INT_SOURCE)multiVector->usartTxCompleteInt);
        }

        if(multiVector->usartRxCompleteInt != -1)
        {
            pConsoleUartData->usartRxCompleteIntStatus = SYS_INT_SourceDisable((INT_SOURCE)multiVector->usartRxCompleteInt);
        }

        if(multiVector->usartErrorInt != -1)
        {
            pConsoleUartData->usartErrorIntStatus = SYS_INT_SourceDisable((INT_SOURCE)multiVector->usartErrorInt);
        }

        SYS_INT_Restore(interruptStatus);
    }
}

static void Console_UART_EnableInterrupts(CONSOLE_UART_DATA* pConsoleUartData)
{
    bool interruptStatus;
    const SYS_CONSOLE_UART_INTERRUPT_SOURCES* intInfo = pConsoleUartData->interruptSources;
    const SYS_CONSOLE_UART_MULTI_INT_SRC* multiVector = &intInfo->intSources.multi;

    if (intInfo->isSingleIntSrc == true)
    {
        /* Enable USART interrupt */
        SYS_INT_SourceRestore((INT_SOURCE)intInfo->intSources.usartInterrupt, pConsoleUartData->usartInterruptStatus);
    }
    else
    {
        interruptStatus = SYS_INT_Disable();
        /* Enable USART interrupt sources */

        if(multiVector->usartTxReadyInt != -1)
        {
            SYS_INT_SourceRestore((INT_SOURCE)multiVector->usartTxReadyInt, pConsoleUartData->usartTxReadyIntStatus);
        }

        if(multiVector->usartTxCompleteInt != -1)
        {
            SYS_INT_SourceRestore((INT_SOURCE)multiVector->usartTxCompleteInt, pConsoleUartData->usartTxCompleteIntStatus);
        }

        if(multiVector->usartRxCompleteInt != -1)
        {
            SYS_INT_SourceRestore((INT_SOURCE)multiVector->usartRxCompleteInt, pConsoleUartData->usartRxCompleteIntStatus);
        }

        if(multiVector->usartErrorInt != -1)
        {
            SYS_INT_SourceRestore((INT_SOURCE)multiVector->usartErrorInt, pConsoleUartData->usartErrorIntStatus);
        }

        SYS_INT_Restore(interruptStatus);
    }
}

static bool Console_UART_ResourceLock(CONSOLE_UART_DATA* pConsoleUartData)
{
    if(pConsoleUartData->inInterruptContext == false)
    {
        /* Grab a mutex. This is okay because we are not in an interrupt context */
        if(OSAL_MUTEX_Lock(&(pConsoleUartData->mutexTransferObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_FALSE)
        {
            return false;
        }
    }
    Console_UART_DisableInterrupts(pConsoleUartData);
    return true;
}

static void Console_UART_ResourceUnlock(CONSOLE_UART_DATA* pConsoleUartData)
{
    Console_UART_EnableInterrupts(pConsoleUartData);

    if(pConsoleUartData->inInterruptContext == false)
    {
        /* Release mutex */
        OSAL_MUTEX_Unlock(&(pConsoleUartData->mutexTransferObjects));
    }
}

static bool Console_UART_PushQueue(Queue* queue, QElement element)
{
    bool isSuccess = false;
    uint32_t inIndex = queue->inIndex;
    QElement* qElement = &queue->pQElementArr[inIndex];

    (inIndex+1) >= (queue->maxQElements)? inIndex = 0 : inIndex++;
    if (inIndex != queue->outIndex)
    {
        qElement->pBuffer = element.pBuffer;
        qElement->size = element.size;
        queue->inIndex = inIndex;
        isSuccess = true;
    }
    return isSuccess;
}

static bool Console_UART_PopQueue(Queue* queue, QElement* qElement)
{
    bool isSuccess = false;
    if (queue->inIndex != queue->outIndex)
    {
        *qElement = queue->pQElementArr[queue->outIndex];
        (queue->outIndex+1) >= (queue->maxQElements)? queue->outIndex = 0 : queue->outIndex++;
        isSuccess = true;
    }
    return isSuccess;
}

static void Console_UART_ReadSubmit(CONSOLE_UART_DATA* pConsoleUartData, QElement qElement)
{
    pConsoleUartData->readStatus = CONSOLE_UART_READ_BUSY;
    pConsoleUartData->uartPLIB->read(qElement.pBuffer, qElement.size);
}

ssize_t Console_UART_Read(uint32_t index, int fd, void* buf, size_t count)
{
    (void)fd;
    QElement qElement;
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);
    if (pConsoleUartData == NULL)
    {
        return 0;
    }

    qElement.pBuffer = buf;
    qElement.size = count;

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return 0;
    }

    if (pConsoleUartData->rdState == CONSOLE_UART_READ_STATE_IDLE)
    {
        /* Read is not busy. Submit read request now itself */
        pConsoleUartData->readQueue.curQElement = qElement;
        pConsoleUartData->rdState = CONSOLE_UART_READ_STATE_BUSY;
        Console_UART_ReadSubmit(pConsoleUartData, pConsoleUartData->readQueue.curQElement);
    }
    else
    {
        /* Read is busy. See if the request can be queued. */
        if (Console_UART_PushQueue(&pConsoleUartData->readQueue, qElement) == false)
        {
            count = 0;
        }
    }

    Console_UART_ResourceUnlock(pConsoleUartData);

    return count;
}

static void Console_UART_WriteSubmit(CONSOLE_UART_DATA* pConsoleUartData, QElement qElement)
{
    pConsoleUartData->writeStatus = CONSOLE_UART_WRITE_BUSY;
    pConsoleUartData->uartPLIB->write(qElement.pBuffer, qElement.size);
}

ssize_t Console_UART_Write(uint32_t index, int fd, const void* buf, size_t count)
{
    (void)fd;
    QElement qElement;
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);
    if (pConsoleUartData == NULL)
    {
        return 0;
    }

    qElement.pBuffer = (char*)buf;
    qElement.size = count;

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return 0;
    }

    if (pConsoleUartData->wrState == CONSOLE_UART_WRITE_STATE_IDLE)
    {
        /* Write is not busy. Submit write request now itself. */
        pConsoleUartData->writeQueue.curQElement = qElement;
        pConsoleUartData->wrState = CONSOLE_UART_WRITE_STATE_BUSY;
        Console_UART_WriteSubmit(pConsoleUartData, pConsoleUartData->writeQueue.curQElement);
    }
    else
    {
        /* Write is busy. See if the request can be queued. */
        if (Console_UART_PushQueue(&pConsoleUartData->writeQueue, qElement) == false)
        {
            count = 0;
        }
    }

    Console_UART_ResourceUnlock(pConsoleUartData);

    return count;
}

void Console_UART_RegisterCallback(uint32_t index, SYS_CONSOLE_CALLBACK callback, SYS_CONSOLE_EVENT event)
{
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);
    if (pConsoleUartData == NULL)
    {
        return;
    }

    switch (event)
    {
        case SYS_CONSOLE_EVENT_READ_COMPLETE:
            pConsoleUartData->rdCallback = callback;
            break;
        case SYS_CONSOLE_EVENT_WRITE_COMPLETE:
            pConsoleUartData->wrCallback = callback;
            break;
        default:
            break;
    }
}

void Console_UART_Flush(uint32_t index)
{
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);
    if (pConsoleUartData == NULL)
    {
        return;
    }

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return;
    }

    /* Reset read and write queues */
    pConsoleUartData->readQueue.inIndex = pConsoleUartData->readQueue.outIndex;
    pConsoleUartData->writeQueue.inIndex = pConsoleUartData->writeQueue.outIndex;

    Console_UART_ResourceUnlock(pConsoleUartData);
}

static void Console_UART_ReadTasks(CONSOLE_UART_DATA* pConsoleUartData)
{
    switch(pConsoleUartData->rdState)
    {
        case CONSOLE_UART_READ_STATE_IDLE:
            break;
        case CONSOLE_UART_READ_STATE_BUSY:
            if ((pConsoleUartData->readStatus == CONSOLE_UART_READ_DONE) ||
                    (pConsoleUartData->readStatus == CONSOLE_UART_READ_ERROR))
            {
                /* Give a callback to the client (console/application) */
                if (pConsoleUartData->rdCallback != NULL)
                {
                    pConsoleUartData->rdCallback((void*)pConsoleUartData->readQueue.curQElement.pBuffer);
                }
                /* Check if more requests are pending in the queue. */
                if (Console_UART_PopQueue(&pConsoleUartData->readQueue, &pConsoleUartData->readQueue.curQElement) == true)
                {
                    Console_UART_ReadSubmit(pConsoleUartData, pConsoleUartData->readQueue.curQElement);
                }
                else
                {
                    /* Nothing to service. Enter idle state. */
                    pConsoleUartData->rdState = CONSOLE_UART_READ_STATE_IDLE;
                }
            }
            break;
        default:
            break;
    }
}

static void Console_UART_WriteTasks(CONSOLE_UART_DATA* pConsoleUartData)
{
    switch(pConsoleUartData->wrState)
    {
        case CONSOLE_UART_WRITE_STATE_IDLE:
            break;
        case CONSOLE_UART_WRITE_STATE_BUSY:
            if(pConsoleUartData->writeStatus == CONSOLE_UART_WRITE_DONE)
            {
                /* Give a callback to the client (console/application) */
                if (pConsoleUartData->wrCallback != NULL)
                {
                    pConsoleUartData->wrCallback((void*)pConsoleUartData->writeQueue.curQElement.pBuffer);
                }
                /* Check if more requests are pending in the queue. */
                if (Console_UART_PopQueue(&pConsoleUartData->writeQueue, &pConsoleUartData->writeQueue.curQElement) == true)
                {
                    Console_UART_WriteSubmit(pConsoleUartData, pConsoleUartData->writeQueue.curQElement);
                }
                else
                {
                    /* Nothing to service. Enter idle state. */
                    pConsoleUartData->wrState = CONSOLE_UART_WRITE_STATE_IDLE;
                }
            }
            break;
        default:
            break;
    }
}

static void Console_UART_PLIBWriteHandler(uintptr_t context)
{
    CONSOLE_UART_DATA* pConsoleUartData = (CONSOLE_UART_DATA*)context;

    pConsoleUartData->inInterruptContext = true;

    pConsoleUartData->writeStatus = CONSOLE_UART_WRITE_DONE;

    Console_UART_WriteTasks(pConsoleUartData);

    pConsoleUartData->inInterruptContext = false;
}

static void Console_UART_PLIBReadHandler(uintptr_t context)
{
    CONSOLE_UART_DATA* pConsoleUartData = (CONSOLE_UART_DATA*)context;

    pConsoleUartData->inInterruptContext = true;

    if (pConsoleUartData->uartPLIB->errorGet() == 0)
    {
        pConsoleUartData->readStatus = CONSOLE_UART_READ_DONE;
    }
    else
    {
        pConsoleUartData->readStatus = CONSOLE_UART_READ_ERROR;
    }

    Console_UART_ReadTasks(pConsoleUartData);

    pConsoleUartData->inInterruptContext = false;
}

void Console_UART_Initialize(uint32_t index, const void* initData)
{
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);
    const SYS_CONSOLE_UART_INIT_DATA* consoleUsartInitData = (const SYS_CONSOLE_UART_INIT_DATA*)initData;

    if (pConsoleUartData == NULL)
    {
        return;
    }

    if(OSAL_MUTEX_Create(&(pConsoleUartData->mutexTransferObjects)) != OSAL_RESULT_TRUE)
    {
        return;
    }

    /* Assign the USART PLIB instance APIs to use */
    pConsoleUartData->uartPLIB = consoleUsartInitData->uartPLIB;

    /* Initialize read and write queues */
    pConsoleUartData->readQueue.maxQElements = consoleUsartInitData->readQueueDepth;
    pConsoleUartData->writeQueue.maxQElements = consoleUsartInitData->writeQueueDepth;
    pConsoleUartData->readQueue.pQElementArr = consoleUsartInitData->readQueueElementsArr;
    pConsoleUartData->writeQueue.pQElementArr = consoleUsartInitData->writeQueueElementsArr;
    pConsoleUartData->interruptSources = consoleUsartInitData->interruptSources;

    pConsoleUartData->readQueue.inIndex = pConsoleUartData->readQueue.outIndex;
    pConsoleUartData->writeQueue.inIndex = pConsoleUartData->writeQueue.outIndex;

    /* Register a callback with the underlying USART PLIB instance */
    pConsoleUartData->uartPLIB->writeCallbackRegister(Console_UART_PLIBWriteHandler, (uintptr_t)pConsoleUartData);
    pConsoleUartData->uartPLIB->readCallbackRegister(Console_UART_PLIBReadHandler, (uintptr_t)pConsoleUartData);

    /* Initialize state-machine */
    pConsoleUartData->rdState = CONSOLE_UART_READ_STATE_IDLE;
    pConsoleUartData->wrState = CONSOLE_UART_WRITE_STATE_IDLE;

    /* Initialize status indicators */
    pConsoleUartData->readStatus = CONSOLE_UART_READ_IDLE;
    pConsoleUartData->writeStatus = CONSOLE_UART_WRITE_IDLE;

    /* Initialize callback pointers */
    pConsoleUartData->rdCallback = NULL;
    pConsoleUartData->wrCallback = NULL;

    pConsoleUartData->status = SYS_CONSOLE_STATUS_CONFIGURED;
}

SYS_CONSOLE_STATUS Console_UART_Status(uint32_t index)
{
    SYS_CONSOLE_STATUS status = SYS_CONSOLE_STATUS_NOT_CONFIGURED;
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);

    if (pConsoleUartData == NULL)
    {
        return SYS_CONSOLE_STATUS_ERROR;
    }

    if ((pConsoleUartData->wrState == CONSOLE_UART_WRITE_STATE_BUSY) ||
            (pConsoleUartData->rdState == CONSOLE_UART_READ_STATE_BUSY))
    {
        status = SYS_CONSOLE_STATUS_BUSY;
    }
    else
    {
        status = pConsoleUartData->status;
    }

    return status;
}

void Console_UART_Tasks(uint32_t index, SYS_MODULE_OBJ object)
{
    /* Do nothing. */
}