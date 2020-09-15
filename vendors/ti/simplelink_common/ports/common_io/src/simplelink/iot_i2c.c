/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include <iot_i2c.h>

#include <ti/drivers/I2C.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>

#include <ti_drivers_config.h>

typedef struct
{
    I2C_Handle xTiI2C;                        /**< TI driver handle used with TI Driver API. */
    I2C_Transaction xTransaction;             /**< TI I2C transaction structure used with TI Driver API. */
    ClockP_Struct xTimeoutClock;              /**< TI clock structure used with TI Driver API. */
    IotI2CCallback_t xI2CCallback;            /**< User registered callback. */
    void * pvUserCallbackContext;             /**< User callback context. */
    IotI2CConfig_t xConfig;                   /**< Active I2C config. */
    IotI2CBusStatus_t xBusStatus;             /**< I2C bus status. */
    IotI2COperationStatus_t xOperationStatus; /**< I2C operation status. */
    SemaphoreP_Struct xSyncSem;               /**< Semaphore used for sync write/read calls */
    bool xIsAsync;                            /**< Async I2C operation flag. */
    int8_t cSlaveAddress;                     /**< I2C slave address */
} IotI2CDescriptor_t;


static void prvI2CCallback( I2C_Handle pxI2CHandle,
                            I2C_Transaction * pxTransaction,
                            bool xTransferStatus );
static void prvTimerCallback( uintptr_t puI2CDesc );
static int32_t prvDoTransfer( IotI2CDescriptor_t * pxI2CDesc,
                              uint8_t * const pvTxBuffer,
                              size_t xTxBytes,
                              uint8_t * const pvRxBuffer,
                              size_t xRxBytes );

static IotI2CDescriptor_t xI2C[ CONFIG_TI_DRIVERS_I2C_COUNT ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotI2CHandle_t iot_i2c_open( int32_t lI2CInstance )
{
    I2C_Handle xTiHandle = NULL;
    IotI2CHandle_t xHandle = NULL;
    I2C_Params xParams;
    ClockP_Params xClockParams;

    if( false == xIsInitialized )
    {
        I2C_init();
        xIsInitialized = true;
    }

    I2C_Params_init( &xParams );
    xParams.transferMode = I2C_MODE_CALLBACK;
    xParams.transferCallbackFxn = prvI2CCallback;
    xTiHandle = I2C_open( lI2CInstance, &xParams );

    if( xTiHandle )
    {
        xHandle = ( IotI2CHandle_t ) &xI2C[ lI2CInstance ];

        /* Initialize default values */
        xI2C[ lI2CInstance ].xTiI2C = xTiHandle;
        xI2C[ lI2CInstance ].cSlaveAddress = -1;
        xI2C[ lI2CInstance ].xBusStatus = eI2CBusIdle;
        xI2C[ lI2CInstance ].xOperationStatus = eI2CCompleted;
        xI2C[ lI2CInstance ].xConfig.ulMasterTimeout = I2C_WAIT_FOREVER;
        xI2C[ lI2CInstance ].xConfig.ulBusFreq = I2C_100kHz;
        xI2C[ lI2CInstance ].xTransaction.arg = xHandle;

        SemaphoreP_constructBinary( &( xI2C[ lI2CInstance ].xSyncSem ), 0 );

        ClockP_Params_init( &xClockParams );
        xClockParams.period = 0;
        xClockParams.startFlag = false;
        xClockParams.arg = ( uintptr_t ) xHandle;
        ClockP_construct( &( xI2C[ lI2CInstance ].xTimeoutClock ),
                          ( ClockP_Fxn ) & prvTimerCallback,
                          ~( 0 ), &xClockParams );
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

void iot_i2c_set_callback( IotI2CHandle_t const pxI2CPeripheral,
                           IotI2CCallback_t xCallback,
                           void * pvUserContext )
{
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxI2CPeripheral;

    if( pxI2CDesc )
    {
        pxI2CDesc->xI2CCallback = xCallback;
        pxI2CDesc->pvUserCallbackContext = pvUserContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_i2c_read_sync( IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t * const pucBuffer,
                           size_t xBytes )
{
    int32_t ret = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxI2CPeripheral;

    if( ( NULL == pucBuffer ) || ( 0 == xBytes ) )
    {
        ret = IOT_I2C_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransfer( pxI2CDesc, NULL, 0, pucBuffer, xBytes );

        if( IOT_I2C_SUCCESS == ret )
        {
            pxI2CDesc->xIsAsync = false;

            if( SemaphoreP_OK != SemaphoreP_pend( &( pxI2CDesc->xSyncSem ), pxI2CDesc->xConfig.ulMasterTimeout ) )
            {
                /* Semaphore timeout, stop the transaction and return "timeout".
                 * Protect this status change as we depend on it in the callback. */
                uint32_t key = HwiP_disable();
                pxI2CDesc->xOperationStatus = eI2CMasterTimeout;
                HwiP_restore( key );

                I2C_cancel( pxI2CDesc->xTiI2C );

                ret = IOT_I2C_BUS_TIMEOUT;
            }

            if( eI2CNackFromSlave == pxI2CDesc->xOperationStatus )
            {
                ret = IOT_I2C_NACK;
            }
            else if( eI2CDriverFailed == pxI2CDesc->xOperationStatus )
            {
                ret = IOT_I2C_READ_FAILED;
            }
        }

        pxI2CDesc->xBusStatus = eI2CBusIdle;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_i2c_write_sync( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pucBuffer,
                            size_t xBytes )
{
    int32_t ret = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxI2CPeripheral;

    if( ( NULL == pucBuffer ) || ( 0 == xBytes ) )
    {
        ret = IOT_I2C_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransfer( pxI2CDesc, pucBuffer, xBytes, NULL, 0 );

        if( IOT_I2C_SUCCESS == ret )
        {
            pxI2CDesc->xIsAsync = false;

            if( SemaphoreP_OK != SemaphoreP_pend( &( pxI2CDesc->xSyncSem ), pxI2CDesc->xConfig.ulMasterTimeout ) )
            {
                /* Semaphore timeout, stop the transaction and return "timeout".
                 * Protect this status change as we depend on it in the callback. */
                uint32_t key = HwiP_disable();
                pxI2CDesc->xOperationStatus = eI2CMasterTimeout;
                HwiP_restore( key );

                I2C_cancel( pxI2CDesc->xTiI2C );

                ret = IOT_I2C_BUS_TIMEOUT;
            }

            if( eI2CNackFromSlave == pxI2CDesc->xOperationStatus )
            {
                ret = IOT_I2C_NACK;
            }
            else if( eI2CDriverFailed == pxI2CDesc->xOperationStatus )
            {
                ret = IOT_I2C_WRITE_FAILED;
            }
        }

        pxI2CDesc->xBusStatus = eI2CBusIdle;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_i2c_read_async( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pucBuffer,
                            size_t xBytes )
{
    int32_t ret = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxI2CPeripheral;

    if( ( NULL == pucBuffer ) || ( 0 == xBytes ) )
    {
        ret = IOT_I2C_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransfer( pxI2CDesc, NULL, 0, pucBuffer, xBytes );

        if( IOT_I2C_SUCCESS == ret )
        {
            pxI2CDesc->xIsAsync = true;

            if( I2C_WAIT_FOREVER != pxI2CDesc->xConfig.ulMasterTimeout )
            {
                ClockP_start( &( pxI2CDesc->xTimeoutClock ) );
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_i2c_write_async( IotI2CHandle_t const pxI2CPeripheral,
                             uint8_t * const pucBuffer,
                             size_t xBytes )
{
    int32_t ret = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxI2CPeripheral;

    if( ( NULL == pucBuffer ) || ( 0 == xBytes ) )
    {
        ret = IOT_I2C_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransfer( pxI2CDesc, pucBuffer, xBytes, NULL, 0 );

        if( IOT_I2C_SUCCESS == ret )
        {
            pxI2CDesc->xIsAsync = true;

            if( I2C_WAIT_FOREVER != pxI2CDesc->xConfig.ulMasterTimeout )
            {
                ClockP_start( &( pxI2CDesc->xTimeoutClock ) );
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_i2c_ioctl( IotI2CHandle_t const pxI2CPeripheral,
                       IotI2CIoctlRequest_t xI2CRequest,
                       void * const pvBuffer )
{
    int32_t ret = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxI2CPeripheral;

    if( ( NULL == pxI2CDesc ) || ( NULL == pxI2CDesc->xTiI2C ) )
    {
        ret = IOT_I2C_INVALID_VALUE;
    }
    else
    {
        switch( xI2CRequest )
        {
            case eI2CSetSlaveAddr:
               {
                   if( NULL == pvBuffer )
                   {
                       ret = IOT_I2C_INVALID_VALUE;
                       break;
                   }

                   uint16_t usNewSlaveAddress = *( ( uint16_t * ) pvBuffer );

                   /* Only 7-bit mode is supported */
                   if( 0xFF <= usNewSlaveAddress )
                   {
                       ret = IOT_I2C_FUNCTION_NOT_SUPPORTED;
                       break;
                   }

                   pxI2CDesc->cSlaveAddress = ( 0xFF & usNewSlaveAddress );
                   break;
               }

            case eI2CSetMasterConfig:
               {
                   if( NULL == pvBuffer )
                   {
                       ret = IOT_I2C_INVALID_VALUE;
                       break;
                   }

                   IotI2CConfig_t * pxConfig = ( IotI2CConfig_t * ) pvBuffer;

                   if( eI2CBusIdle != pxI2CDesc->xBusStatus )
                   {
                       ret = IOT_I2C_BUSY;
                       break;
                   }

                   /* Map I2C busSpeed to the driver enum */
                   uint32_t ulBitRate = 0;

                   switch( pxConfig->ulBusFreq )
                   {
                       case IOT_I2C_STANDARD_MODE_BPS:
                           ulBitRate = I2C_100kHz;
                           break;

                       case IOT_I2C_FAST_MODE_BPS:
                           ulBitRate = I2C_400kHz;
                           break;

                       case IOT_I2C_FAST_MODE_PLUS_BPS:
                           ulBitRate = I2C_1000kHz;
                           break;

                       /* Potentially supported by the driver depending on the device used */
                       case 3300000:
                           ulBitRate = I2C_3330kHz;
                           break;

                       case IOT_I2C_HIGH_SPEED_BPS:
                           ulBitRate = I2C_3400kHz;
                           break;

                       default:
                           ret = IOT_I2C_INVALID_VALUE;
                           return ret;
                   }

                   uint32_t ulScalingFactor = 1000 / ClockP_getSystemTickPeriod();

                   if( ( ~( 0 ) / ulScalingFactor ) < pxConfig->ulMasterTimeout )
                   {
                       ret = IOT_I2C_INVALID_VALUE;
                       break;
                   }

                   pxI2CDesc->xConfig.ulBusFreq = pxConfig->ulBusFreq;
                   pxI2CDesc->xConfig.ulMasterTimeout = pxConfig->ulMasterTimeout * ulScalingFactor;

                   ClockP_setTimeout( &( pxI2CDesc->xTimeoutClock ), pxI2CDesc->xConfig.ulMasterTimeout );

                   /* Re-open the I2C driver with the new bus rate */
                   uint32_t i;

                   for( i = 0; i < CONFIG_TI_DRIVERS_I2C_COUNT; i++ )
                   {
                       if( &xI2C[ i ] == pxI2CDesc )
                       {
                           I2C_Params xParams;
                           I2C_Params_init( &xParams );
                           xParams.transferMode = I2C_MODE_CALLBACK;
                           xParams.transferCallbackFxn = prvI2CCallback;
                           xParams.bitRate = ulBitRate;

                           I2C_close( pxI2CDesc->xTiI2C );
                           pxI2CDesc->xTiI2C = I2C_open( i, &xParams );

                           if( NULL == pxI2CDesc->xTiI2C )
                           {
                               ret = IOT_I2C_INVALID_VALUE;
                           }

                           break;
                       }
                   }

                   break;
               }

            case eI2CGetMasterConfig:
               {
                   if( NULL == pvBuffer )
                   {
                       ret = IOT_I2C_INVALID_VALUE;
                       break;
                   }

                   IotI2CConfig_t * pxConfig = ( IotI2CConfig_t * ) pvBuffer;

                   uint32_t ulScalingFactor = 1000 / ClockP_getSystemTickPeriod();
                   pxConfig->ulMasterTimeout = pxI2CDesc->xConfig.ulMasterTimeout / ulScalingFactor;

                   pxConfig->ulBusFreq = pxI2CDesc->xConfig.ulBusFreq;

                   break;
               }

            case eI2CGetBusState:
               {
                   if( NULL == pvBuffer )
                   {
                       ret = IOT_I2C_INVALID_VALUE;
                       break;
                   }

                   IotI2CBusStatus_t * pxStatus = ( IotI2CBusStatus_t * ) pvBuffer;
                   *pxStatus = pxI2CDesc->xBusStatus;

                   break;
               }

            case eI2CGetTxNoOfbytes:
               {
                   if( NULL == pvBuffer )
                   {
                       ret = IOT_I2C_INVALID_VALUE;
                       break;
                   }

                   uint16_t * usTxBytes = ( uint16_t * ) pvBuffer;
                   *usTxBytes = 0;

                   if( pxI2CDesc->xTransaction.writeBuf )
                   {
                       if( eI2CBusIdle == pxI2CDesc->xBusStatus )
                       {
                           *usTxBytes = pxI2CDesc->xTransaction.writeCount;
                       }
                   }

                   break;
               }

            case eI2CGetRxNoOfbytes:
               {
                   if( NULL == pvBuffer )
                   {
                       ret = IOT_I2C_INVALID_VALUE;
                       break;
                   }

                   uint16_t * usRxBytes = ( uint16_t * ) pvBuffer;
                   *usRxBytes = 0;

                   if( pxI2CDesc->xTransaction.readBuf )
                   {
                       if( eI2CBusIdle == pxI2CDesc->xBusStatus )
                       {
                           *usRxBytes = pxI2CDesc->xTransaction.readCount;
                       }
                   }

                   break;
               }

            case eI2CBusReset:
            case eI2CSendNoStopFlag:
                ret = IOT_I2C_FUNCTION_NOT_SUPPORTED;
                break;

            default:
                ret = IOT_I2C_INVALID_VALUE;
                break;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_i2c_close( IotI2CHandle_t const pxI2CPeripheral )
{
    int32_t ret = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxI2CPeripheral;

    if( ( NULL == pxI2CDesc ) || ( NULL == pxI2CDesc->xTiI2C ) )
    {
        ret = IOT_I2C_INVALID_VALUE;
    }
    else
    {
        ClockP_stop( &( pxI2CDesc->xTimeoutClock ) );

        pxI2CDesc->xBusStatus = eI2CBusIdle;

        I2C_close( pxI2CDesc->xTiI2C );
        pxI2CDesc->xTiI2C = NULL;

        ClockP_destruct( &( pxI2CDesc->xTimeoutClock ) );
        SemaphoreP_destruct( &( pxI2CDesc->xSyncSem ) );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_i2c_cancel( IotI2CHandle_t const pxI2CPeripheral )
{
    int32_t ret = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxI2CPeripheral;

    if( ( NULL == pxI2CDesc ) || ( NULL == pxI2CDesc->xTiI2C ) )
    {
        ret = IOT_I2C_INVALID_VALUE;
    }
    else if( eI2CBusIdle == pxI2CDesc->xBusStatus )
    {
        ret = IOT_I2C_NOTHING_TO_CANCEL;
    }
    else
    {
        ClockP_stop( &( pxI2CDesc->xTimeoutClock ) );

        pxI2CDesc->xBusStatus = eI2CBusIdle;
        I2C_cancel( pxI2CDesc->xTiI2C );

        pxI2CDesc->xOperationStatus = eI2CDriverFailed;

        /* If it was an sync operation, release the semaphore */
        if( !pxI2CDesc->xIsAsync )
        {
            SemaphoreP_post( &pxI2CDesc->xSyncSem );
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static int32_t prvDoTransfer( IotI2CDescriptor_t * pxI2CDesc,
                              uint8_t * const pvTxBuffer,
                              size_t xTxBytes,
                              uint8_t * const pvRxBuffer,
                              size_t xRxBytes )
{
    int32_t ret = IOT_I2C_SUCCESS;

    if( ( NULL == pxI2CDesc ) || ( NULL == pxI2CDesc->xTiI2C ) )
    {
        ret = IOT_I2C_INVALID_VALUE;
    }
    else if( 0 > pxI2CDesc->cSlaveAddress )
    {
        ret = IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }
    else if( eI2CBusIdle != pxI2CDesc->xBusStatus )
    {
        ret = IOT_I2C_BUSY;
    }
    else
    {
        /* Perform a non-blocking pend on the semaphore to make sure it is cleared
         * before starting a new transaction. */
        SemaphoreP_pend( &( pxI2CDesc->xSyncSem ), SemaphoreP_NO_WAIT );

        pxI2CDesc->xTransaction.slaveAddress = pxI2CDesc->cSlaveAddress;
        pxI2CDesc->xTransaction.writeBuf = pvTxBuffer;
        pxI2CDesc->xTransaction.writeCount = xTxBytes;
        pxI2CDesc->xTransaction.readBuf = pvRxBuffer;
        pxI2CDesc->xTransaction.readCount = xRxBytes;

        bool xStatus = I2C_transfer( pxI2CDesc->xTiI2C, &pxI2CDesc->xTransaction );

        if( !xStatus )
        {
            if( NULL == pvTxBuffer )
            {
                ret = IOT_I2C_READ_FAILED;
            }
            else
            {
                ret = IOT_I2C_WRITE_FAILED;
            }
        }
        else
        {
            /* Reset operation status to "good" */
            pxI2CDesc->xOperationStatus = eI2CCompleted;

            /* Mark as busy */
            pxI2CDesc->xBusStatus = eI2cBusBusy;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void prvI2CCallback( I2C_Handle pxI2CHandle,
                            I2C_Transaction * pxTransaction,
                            bool xTransferStatus )
{
    /* Transfer argument should contain descriptor pointer */
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) pxTransaction->arg;

    /* If the bus is set to idle then the callbackk is because of a "cancel" (or close) call.
     * In that case, suppress it */
    if( eI2CBusIdle != pxI2CDesc->xBusStatus )
    {
        ClockP_stop( &( pxI2CDesc->xTimeoutClock ) );

        if( eI2CMasterTimeout != pxI2CDesc->xOperationStatus )
        {
            if( ( I2C_STATUS_DATA_NACK == pxTransaction->status ) ||
                ( I2C_STATUS_ADDR_NACK == pxTransaction->status ) )
            {
                pxI2CDesc->xOperationStatus = eI2CNackFromSlave;
            }
            else if( I2C_STATUS_SUCCESS != pxI2CDesc->xTransaction.status )
            {
                /* Group all other erros as a failure */
                pxI2CDesc->xOperationStatus = eI2CDriverFailed;
            }
            else
            {
                pxI2CDesc->xOperationStatus = eI2CCompleted;
            }

            if( !pxI2CDesc->xIsAsync )
            {
                SemaphoreP_post( &pxI2CDesc->xSyncSem );
            }
        }

        if( pxI2CDesc->xIsAsync && pxI2CDesc->xI2CCallback )
        {
            pxI2CDesc->xI2CCallback( pxI2CDesc->xOperationStatus, pxI2CDesc->pvUserCallbackContext );
        }

        pxI2CDesc->xBusStatus = eI2CBusIdle;
    }
}

/*-----------------------------------------------------------*/

static void prvTimerCallback( uintptr_t puI2CDesc )
{
    IotI2CDescriptor_t * pxI2CDesc = ( IotI2CDescriptor_t * ) puI2CDesc;

    /* We had an async timeout, set status to timeout and cancel the transaction.
     * As we do not mark the bus as idle, the I2C callback should be invoked by
     * the cancel API to post the user callback */
    pxI2CDesc->xOperationStatus = eI2CMasterTimeout;
    I2C_cancel( pxI2CDesc->xTiI2C );
}
