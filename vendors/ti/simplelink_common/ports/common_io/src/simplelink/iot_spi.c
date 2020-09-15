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

/**
 * @file    iot_spi.c
 * @brief   This file contains the definitions of SPI APIs using TI drivers.
 *          The implementation does not support partial return.
 */
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

/* Common IO Header */
#include <iot_spi.h>

/* TI Drivers and DPL */
#include <ti/drivers/SPI.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#if ( defined DeviceFamily_CC13X2 ) || ( defined DeviceFamily_CC26X2 ) || ( defined DeviceFamily_CC13X2_CC26X2 )
#include <ti/drivers/spi/SPICC26X2DMA.h>
#endif

/* Driver config */
#include <ti_drivers_config.h>

/**
 * @brief TI Simplelink SPI HAL context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI Simplelink SPI driver.
 */
typedef struct
{
    SPI_Handle tiSpiHandle;          /**< TI driver handle used with TI Driver API. */
    SPI_Transaction xTransaction;    /**< TI driver transaction structure. */
    IotSPIMasterConfig_t xSpiConfig; /**< Current SPI configuration. */
    IotSPICallback_t xSpiCallback;   /**< User registered callback. */
    void * pvUserCallbackContext;    /**< User callback cotnext */
    bool xIsAsyncTransfer;           /**< Is asynchronous transfer. */
    size_t xLastBytesRead;           /**< Last number of bytes read. */
    size_t xLastBytesWritten;        /**< Last number of bytes written. */
    SemaphoreP_Struct xSyncSem;      /**< Semaphore used for sync write/read calls. */
} IotSpiDescriptor_t;

static void prvSpiCallback( SPI_Handle pxSpiHandle,
                            SPI_Transaction * transaction );
static int32_t prvDoTransferAsync( IotSPIHandle_t const pxSPIPeripheral,
                                   uint8_t * const pvTxBuffer,
                                   uint8_t * const pvRxBuffer,
                                   size_t xBytes );
static int32_t prvDoTransferSync( IotSPIHandle_t const pxSPIPeripheral,
                                  uint8_t * const pvTxBuffer,
                                  uint8_t * const pvRxBuffer,
                                  size_t xBytes );

/**
 * @brief Default SPI parameters structure used when opening the interface for
 *        the first time.
 */
const SPI_Params xSpiDefaultParams =
{
    SPI_MODE_CALLBACK, /* transferMode */
    SPI_WAIT_FOREVER,  /* transferTimeout */
    prvSpiCallback,    /* transferCallbackFxn */
    SPI_MASTER,        /* mode */
    1000000,           /* bitRate */
    8,                 /* dataSize */
    SPI_POL0_PHA0,     /* frameFormat */
    NULL               /* custom */
};

/**
 * @brief TI SPI Driver parameter to iot_spi.h mode LUT.
 */
static const uint32_t frameFormat[] =
{
    SPI_POL0_PHA0, /*!< SPI mode Polarity 0 Phase 0 */
    SPI_POL0_PHA1, /*!< SPI mode Polarity 0 Phase 1 */
    SPI_POL1_PHA0, /*!< SPI mode Polarity 1 Phase 0 */
    SPI_POL1_PHA1, /*!< SPI mode Polarity 1 Phase 1 */
};

/**
 * @brief Static SPI descriptor table
 */
static IotSpiDescriptor_t xSpi[ CONFIG_TI_DRIVERS_SPI_COUNT ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotSPIHandle_t iot_spi_open( int32_t lSPIInstance )
{
    SPI_Handle xTiHandle = NULL;
    IotSPIHandle_t xHandle = NULL;

    if( false == xIsInitialized )
    {
        SPI_init();
        xIsInitialized = true;
    }

    xTiHandle = SPI_open( lSPIInstance, ( SPI_Params * ) &xSpiDefaultParams );

    if( xTiHandle )
    {
        xHandle = ( IotSPIHandle_t ) &xSpi[ lSPIInstance ];

        /* Initialize default values */
        xSpi[ lSPIInstance ].tiSpiHandle = xTiHandle;
        xSpi[ lSPIInstance ].xSpiConfig.ulFreq = xSpiDefaultParams.bitRate;
        xSpi[ lSPIInstance ].xSpiConfig.eMode = eSPIMode0;
        xSpi[ lSPIInstance ].xSpiConfig.eSetBitOrder = eSPIMSBFirst;
        xSpi[ lSPIInstance ].xSpiConfig.ucDummyValue = 0xFF;
        xSpi[ lSPIInstance ].xSpiCallback = NULL;
        xSpi[ lSPIInstance ].pvUserCallbackContext = NULL;
        xSpi[ lSPIInstance ].xLastBytesRead = 0;
        xSpi[ lSPIInstance ].xLastBytesWritten = 0;
        xSpi[ lSPIInstance ].xTransaction.status = SPI_TRANSFER_COMPLETED;
        xSpi[ lSPIInstance ].xTransaction.arg = xHandle;
        xSpi[ lSPIInstance ].xIsAsyncTransfer = false;

        SemaphoreP_constructBinary( &( xSpi[ lSPIInstance ].xSyncSem ), 0 );
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

void iot_spi_set_callback( IotSPIHandle_t const pxSPIPeripheral,
                           IotSPICallback_t xCallback,
                           void * pvUserContext )
{
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( pxSpiDesc )
    {
        pxSpiDesc->xSpiCallback = xCallback;
        pxSpiDesc->pvUserCallbackContext = pvUserContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_spi_ioctl( IotSPIHandle_t const pxSPIPeripheral,
                       IotSPIIoctlRequest_t xSPIRequest,
                       void * const pvBuffer )
{
    int32_t ret = IOT_SPI_SUCCESS;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pvBuffer ) || ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        switch( xSPIRequest )
        {
            case eSPISetMasterConfig:
               {
                   IotSPIMasterConfig_t * pxConfig = ( IotSPIMasterConfig_t * ) pvBuffer;

                   if( ( SPI_TRANSFER_STARTED == pxSpiDesc->xTransaction.status ) ||
                       ( SPI_TRANSFER_QUEUED == pxSpiDesc->xTransaction.status ) )
                   {
                       ret = IOT_SPI_BUS_BUSY;
                   }
                   else
                   {
                       /* Driver/Hardware only support MSB first
                        * The check is not used for now based on feedback from
                        * Amazon. */
                       #if 0
                           if( eSPIMSBFirst != pxConfig->eSetBitOrder )
                           {
                               ret = IOT_SPI_INVALID_VALUE;
                               break;
                           }
                       #endif

                       SPI_Params xNewParams = xSpiDefaultParams;
                       xNewParams.frameFormat = frameFormat[ pxConfig->eMode ];
                       xNewParams.bitRate = pxConfig->ulFreq;

                       /* Close and re-open the same instance */
                       uint32_t i;

                       for( i = 0; i < CONFIG_TI_DRIVERS_SPI_COUNT; i++ )
                       {
                           if( &xSpi[ i ] == pxSpiDesc )
                           {
                               SPI_close( xSpi[ i ].tiSpiHandle );
                               xSpi[ i ].tiSpiHandle = SPI_open( i, &xNewParams );

                               xSpi[ i ].xSpiConfig.ulFreq = pxConfig->ulFreq;
                               xSpi[ i ].xSpiConfig.eMode = pxConfig->eMode;
                               xSpi[ i ].xSpiConfig.eSetBitOrder = pxConfig->eSetBitOrder;
                               xSpi[ i ].xSpiConfig.ucDummyValue = pxConfig->ucDummyValue;

                               if( NULL == xSpi[ i ].tiSpiHandle )
                               {
                                   xSpi[ i ].tiSpiHandle = SPI_open( i, ( SPI_Params * ) &xSpiDefaultParams );
                                   ret = IOT_SPI_INVALID_VALUE;
                               }

                               break;
                           }
                       }

#if ( defined DeviceFamily_CC13X2 ) || ( defined DeviceFamily_CC26X2 ) || ( defined DeviceFamily_CC13X2_CC26X2 )
                       /* NOTE: This makes the port device specific to CC13x2/CC26X2.
                        * To support all simplelink devices, this features need to be
                        * excluded. As read APIs specify that the master WILL send
                        * the dummy byte on read only, we have no chooise but to make the port
                        * device specific for now. */
                       SPICC26X2DMA_Object * const xObject = pxSpiDesc->tiSpiHandle->object;
                       xObject->txScratchBuf = pxConfig->ucDummyValue;
#endif
                   }

                   break;
               }

            case eSPIGetMasterConfig:
               {
                   IotSPIMasterConfig_t * pxConfig = ( IotSPIMasterConfig_t * ) pvBuffer;

                   pxConfig->ulFreq = pxSpiDesc->xSpiConfig.ulFreq;
                   pxConfig->eMode = pxSpiDesc->xSpiConfig.eMode;
                   pxConfig->eSetBitOrder = pxSpiDesc->xSpiConfig.eSetBitOrder;
                   pxConfig->ucDummyValue = pxSpiDesc->xSpiConfig.ucDummyValue;

                   break;
               }

            case eSPIGetTxNoOfbytes:
                *( uint16_t * ) pvBuffer = pxSpiDesc->xLastBytesWritten;
                break;

            case eSPIGetRxNoOfbytes:
                *( uint16_t * ) pvBuffer = pxSpiDesc->xLastBytesRead;
                break;

            default:
                ret = IOT_SPI_INVALID_VALUE;
                break;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_read_sync( IotSPIHandle_t const pxSPIPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes )
{
    int32_t ret = IOT_SPI_READ_FAILED;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pvBuffer ) || ( 0 == xBytes ) ||
        ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransferSync( pxSPIPeripheral, NULL, pvBuffer, xBytes );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_read_async( IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
    int32_t ret = IOT_SPI_READ_FAILED;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pvBuffer ) || ( 0 == xBytes ) ||
        ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransferAsync( pxSPIPeripheral, NULL, pvBuffer, xBytes );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_write_sync( IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
    int32_t ret = IOT_SPI_WRITE_FAILED;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pvBuffer ) || ( 0 == xBytes ) ||
        ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransferSync( pxSPIPeripheral, pvBuffer, NULL, xBytes );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_write_async( IotSPIHandle_t const pxSPIPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    int32_t ret = IOT_SPI_WRITE_FAILED;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pvBuffer ) || ( 0 == xBytes ) ||
        ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransferAsync( pxSPIPeripheral, pvBuffer, NULL, xBytes );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_transfer_sync( IotSPIHandle_t const pxSPIPeripheral,
                               uint8_t * const pvTxBuffer,
                               uint8_t * const pvRxBuffer,
                               size_t xBytes )
{
    int32_t ret = IOT_SPI_TRANSFER_ERROR;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pvRxBuffer ) || ( NULL == pvTxBuffer ) ||
        ( 0 == xBytes ) || ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransferSync( pxSPIPeripheral, pvTxBuffer, pvRxBuffer, xBytes );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_transfer_async( IotSPIHandle_t const pxSPIPeripheral,
                                uint8_t * const pvTxBuffer,
                                uint8_t * const pvRxBuffer,
                                size_t xBytes )
{
    int32_t ret = IOT_SPI_TRANSFER_ERROR;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pvRxBuffer ) || ( NULL == pvTxBuffer ) ||
        ( 0 == xBytes ) || ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        ret = prvDoTransferAsync( pxSPIPeripheral, pvTxBuffer, pvRxBuffer, xBytes );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_close( IotSPIHandle_t const pxSPIPeripheral )
{
    int32_t ret = IOT_SPI_SUCCESS;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        SPI_close( pxSpiDesc->tiSpiHandle );
        SemaphoreP_destruct( &( pxSpiDesc->xSyncSem ) );
        pxSpiDesc->tiSpiHandle = NULL;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_cancel( IotSPIHandle_t const pxSPIPeripheral )
{
    int32_t ret = IOT_SPI_SUCCESS;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( ( NULL == pxSpiDesc ) || ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        if( ( SPI_TRANSFER_STARTED != pxSpiDesc->xTransaction.status ) &&
            ( SPI_TRANSFER_QUEUED != pxSpiDesc->xTransaction.status ) &&
            ( SPI_TRANSFER_PEND_CSN_ASSERT != pxSpiDesc->xTransaction.status ) )
        {
            ret = IOT_SPI_NOTHING_TO_CANCEL;
        }
        else
        {
            SPI_transferCancel( pxSpiDesc->tiSpiHandle );
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_spi_select_slave( int32_t lSPIInstance,
                              int32_t lSPISlave )
{
    int32_t ret = IOT_SPI_SUCCESS;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) &xSpi[ lSPIInstance ];

    if( ( NULL == pxSpiDesc ) || ( NULL == pxSpiDesc->tiSpiHandle ) )
    {
        ret = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        /* Is up to the implementation to decide on how this APi works.
         * As the port already is device specifc, we can use the
         * "control" API to change the HW CS assignment.
         * lSPISlave is assumed to be an available DIO number */
        int16_t sStatus = SPI_control( pxSpiDesc->tiSpiHandle, SPICC26X2DMA_CMD_SET_CSN_PIN, &lSPISlave );

        if( SPI_STATUS_ERROR == sStatus )
        {
            ret = IOT_SPI_INVALID_VALUE;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static int32_t prvDoTransfer( IotSPIHandle_t const pxSPIPeripheral,
                              uint8_t * const pvTxBuffer,
                              uint8_t * const pvRxBuffer,
                              size_t xBytes )
{
    int32_t ret = IOT_SPI_TRANSFER_ERROR;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    if( NULL == pvRxBuffer )
    {
        ret = IOT_SPI_WRITE_FAILED;
    }
    else if( NULL == pvTxBuffer )
    {
        ret = IOT_SPI_READ_FAILED;
    }
    else if( ( SPI_TRANSFER_STARTED == pxSpiDesc->xTransaction.status ) ||
             ( SPI_TRANSFER_QUEUED == pxSpiDesc->xTransaction.status ) )
    {
        ret = IOT_SPI_BUS_BUSY;
    }
    else
    {
        pxSpiDesc->xTransaction.txBuf = pvTxBuffer;
        pxSpiDesc->xTransaction.rxBuf = pvRxBuffer;
        pxSpiDesc->xTransaction.count = xBytes;

        if( SPI_transfer( pxSpiDesc->tiSpiHandle, &pxSpiDesc->xTransaction ) )
        {
            if( NULL == pvRxBuffer )
            {
                pxSpiDesc->xLastBytesRead = 0;
            }

            if( NULL == pvTxBuffer )
            {
                pxSpiDesc->xLastBytesWritten = 0;
            }

            if( ( SPI_TRANSFER_STARTED == pxSpiDesc->xTransaction.status ) ||
                ( SPI_TRANSFER_QUEUED == pxSpiDesc->xTransaction.status ) ||
                ( SPI_TRANSFER_PEND_CSN_ASSERT == pxSpiDesc->xTransaction.status ) )
            {
                ret = IOT_SPI_SUCCESS;
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static int32_t prvDoTransferAsync( IotSPIHandle_t const pxSPIPeripheral,
                                   uint8_t * const pvTxBuffer,
                                   uint8_t * const pvRxBuffer,
                                   size_t xBytes )
{
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    pxSpiDesc->xIsAsyncTransfer = true;

    return prvDoTransfer( pxSPIPeripheral, pvTxBuffer, pvRxBuffer, xBytes );
}

/*-----------------------------------------------------------*/

static int32_t prvDoTransferSync( IotSPIHandle_t const pxSPIPeripheral,
                                  uint8_t * const pvTxBuffer,
                                  uint8_t * const pvRxBuffer,
                                  size_t xBytes )
{
    int32_t ret = IOT_SPI_TRANSFER_ERROR;
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) pxSPIPeripheral;

    pxSpiDesc->xIsAsyncTransfer = false;

    ret = prvDoTransfer( pxSPIPeripheral, pvTxBuffer, pvRxBuffer, xBytes );

    if( ret == IOT_SPI_SUCCESS )
    {
        if( SemaphoreP_OK == SemaphoreP_pend( &( pxSpiDesc->xSyncSem ), SemaphoreP_WAIT_FOREVER ) )
        {
            if( SPI_TRANSFER_COMPLETED != pxSpiDesc->xTransaction.status )
            {
                ret = IOT_SPI_TRANSFER_ERROR;

                if( NULL == pvRxBuffer )
                {
                    ret = IOT_SPI_WRITE_FAILED;
                }

                if( NULL == pvTxBuffer )
                {
                    ret = IOT_SPI_READ_FAILED;
                }
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void prvSpiCallback( SPI_Handle pxSpiHandle,
                            SPI_Transaction * transaction )
{
    IotSpiDescriptor_t * pxSpiDesc = ( IotSpiDescriptor_t * ) transaction->arg;

    if( pxSpiDesc->xIsAsyncTransfer )
    {
        if( pxSpiDesc->xSpiCallback )
        {
            IotSPITransactionStatus_t status = eSPISuccess;

            if( ( SPI_TRANSFER_CANCELED == transaction->status ) ||
                ( SPI_TRANSFER_FAILED == transaction->status ) )
            {
                if( NULL == transaction->txBuf )
                {
                    status = eSPIWriteError;
                }
                else if( NULL == transaction->rxBuf )
                {
                    status = eSPIReadError;
                }
                else
                {
                    status = eSPITransferError;
                }
            }

            pxSpiDesc->xSpiCallback( status, pxSpiDesc->pvUserCallbackContext );
        }
    }
    else
    {
        SemaphoreP_post( &( pxSpiDesc->xSyncSem ) );
    }
}
