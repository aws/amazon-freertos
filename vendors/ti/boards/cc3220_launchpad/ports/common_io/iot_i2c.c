/*
 * Amazon FreeRTOS V0.1.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#include "iot_i2c.h"
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* source file: ti/simplelink_cc32xx_sdk_2_40_01_01/source/ti/drivers/I2C.c */
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC32XX.h>

typedef struct IotI2CDescriptor
{
    I2C_Handle handle;
    I2C_Transaction transaction;
    IotI2CConfig_t config;
    void * userContext;
    IotI2CCallback_t userCallback;
    SemaphoreHandle_t semphr;
    uint8_t flags;
} IotI2CDescriptor_t;

/* Used by the "flags" member of IotI2CDescriptor_t. */
#define _FLAG_NO_STOP    ( 0x01 )        /* If the no stop has been requested. */

#define _OP_READ         ( 1 )
#define _OP_WRITE        ( 2 )

#define _MODE_SYNC       ( 1 )
#define _MODE_ASYNC      ( 2 )

/* Helper macro function to check if a specific flag is set or not. */
#define _isFlagSet( pI2cDescriptor, flag )       ( ( ( pI2cDescriptor )->flags & ( flag ) ) > 0 )
#define _isFlagNotSet( pI2cDescriptor, flag )    ( ( ( pI2cDescriptor )->flags & ( flag ) ) == 0 )

/* IDLE and ERROR states are considered as idle; otherwise busy. */
#define _getState( pI2cDescriptor )                                                              \
    ( ( ( I2CCC32XX_Object * ) pxI2CPeripheral->handle->object )->mode == I2CCC32XX_IDLE_MODE || \
      ( ( I2CCC32XX_Object * ) pxI2CPeripheral->handle->object )->mode == I2CCC32XX_ERROR ? eI2CBusIdle : eI2cBusBusy )

static I2C_BitRate _frequencyToBitRate( uint32_t frequency );

static void _callbackInternal( I2C_Handle handle,
                               I2C_Transaction * pTransaction,
                               bool transferStatus );

static int32_t _doTransfer( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes,
                            uint8_t op,
                            uint8_t mode );

/* Only one I2C instance on this board. */
static IotI2CDescriptor_t _i2cDescriptor =
{
    .handle       = NULL,
    .transaction  =
    { 0 },
    .userCallback = NULL,
    .userContext  = NULL,
    .config       =
    { 0 },
    .semphr       = NULL,
    .flags        = 0
};



IotI2CHandle_t iot_i2c_open( int32_t lI2CInstance )
{
    /* Only one instance id is allowed: 0 */
    if( lI2CInstance != 0 )
    {
        return NULL;
    }

    IotI2CHandle_t ret = NULL;
    I2C_Params params =
    {
        .transferCallbackFxn = _callbackInternal,
        .bitRate             = I2C_100kHz,
        .transferMode        = I2C_MODE_CALLBACK,
        .custom              = NULL
    };

    /* If this instance is not opened yet. */
    if( _i2cDescriptor.handle == NULL )
    {
        /* init is idempotent. */
        I2C_init();

        _i2cDescriptor.handle = I2C_open( lI2CInstance, &params );

        /* If open succeeds. */
        if( _i2cDescriptor.handle != NULL )
        {
            _i2cDescriptor.semphr = xSemaphoreCreateBinary();

            /* If semaphore creation succeeds. */
            if( _i2cDescriptor.semphr != NULL )
            {
                ret = &_i2cDescriptor;
            }
        }
    }

    return ret;
}

void iot_i2c_set_callback( IotI2CHandle_t const pxI2CPeripheral,
                           IotI2CCallback_t xCallback,
                           void * pvUserContext )
{
    pxI2CPeripheral->userCallback = xCallback;
    pxI2CPeripheral->userContext = pvUserContext;
}

int32_t iot_i2c_read_sync( IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes )
{
    return _doTransfer( pxI2CPeripheral, pvBuffer, xBytes, _OP_READ, _MODE_SYNC );
}

int32_t iot_i2c_write_sync( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
    return _doTransfer( pxI2CPeripheral, pvBuffer, xBytes, _OP_WRITE, _MODE_SYNC );
}

int32_t iot_i2c_read_async( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
    return _doTransfer( pxI2CPeripheral, pvBuffer, xBytes, _OP_READ, _MODE_ASYNC );
}

int32_t iot_i2c_write_async( IotI2CHandle_t const pxI2CPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    return _doTransfer( pxI2CPeripheral, pvBuffer, xBytes, _OP_WRITE, _MODE_ASYNC );
}

int32_t iot_i2c_ioctl( IotI2CHandle_t const pxI2CPeripheral,
                       IotI2CIoctlRequest_t xI2CRequest,
                       void * const pvBuffer )
{
    int32_t ret = IOT_I2C_SUCCESS;

    switch( xI2CRequest )
    {
        case eI2CSetMasterConfig:

            if( pvBuffer == NULL )
            {
                ret = IOT_I2C_INVALID_VALUE;
            }
            else if( _getState( pxI2CPeripheral ) == eI2cBusBusy )
            {
                ret = IOT_I2C_BUSY;
            }
            else
            {
                IotI2CConfig_t * pConfig = ( IotI2CConfig_t * ) pvBuffer;
                pxI2CPeripheral->config = *pConfig;
                /* Although I2CCC32XX.h says application should NOT access member of I2CCC32XX_Object, this is the only way found. */
                ( ( I2CCC32XX_Object * ) pxI2CPeripheral->handle->object )->bitRate = _frequencyToBitRate( pConfig->ulBusFreq );
            }

            break;

        case eI2CGetMasterConfig:

            if( pvBuffer == NULL )
            {
                ret = IOT_I2C_INVALID_VALUE;
            }
            else
            {
                IotI2CConfig_t * config = ( IotI2CConfig_t * ) pvBuffer;
                *config = pxI2CPeripheral->config;
            }

            break;

        case eI2CSetSlaveAddr:

            if( pvBuffer == NULL )
            {
                ret = IOT_I2C_INVALID_VALUE;
            }
            else
            {
                uint8_t * address = ( uint8_t * ) pvBuffer;
                pxI2CPeripheral->transaction.slaveAddress = ( uint_least8_t ) *address;
            }

            break;

        case eI2CSendNoStopFlag:
            pxI2CPeripheral->flags |= _FLAG_NO_STOP;
            break;

        case eI2CGetRxNoOfbytes:
            *( uint16_t * ) pvBuffer = pxI2CPeripheral->transaction.readCount;
            break;

        case eI2CGetTxNoOfbytes:
            *( uint16_t * ) pvBuffer = pxI2CPeripheral->transaction.writeCount;
            break;

        case eI2CGetBusState:
            *( IotI2CBusStatus_t * ) pvBuffer = _getState( pxI2CPeripheral );
            break;

        default:
            ret = IOT_I2C_INVALID_VALUE;
            break;
    }

    return ret;
}

int32_t iot_i2c_close( IotI2CHandle_t const pxI2CPeripheral )
{
    if( ( pxI2CPeripheral == NULL ) || ( pxI2CPeripheral->handle == NULL ) )
    {
        return IOT_I2C_INVALID_VALUE;
    }

    I2C_close( pxI2CPeripheral->handle );
    /*pxI2CPeripheral->handle = NULL; */
    memset( pxI2CPeripheral, 0, sizeof( IotI2CDescriptor_t ) );

    return IOT_I2C_SUCCESS;
}

int32_t iot_i2c_cancel( IotI2CHandle_t const pxI2CPeripheral )
{
    if( ( pxI2CPeripheral == NULL ) || ( pxI2CPeripheral->handle == NULL ) )
    {
        return IOT_I2C_INVALID_VALUE;
    }

    I2C_cancel( pxI2CPeripheral->handle );

    return IOT_I2C_SUCCESS;
}

static int32_t _doTransfer( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes,
                            uint8_t op,
                            uint8_t mode )
{
    if( ( pxI2CPeripheral == NULL ) || ( pvBuffer == NULL ) || ( xBytes <= 0 ) )
    {
        return IOT_I2C_INVALID_VALUE;
    }

    if( pxI2CPeripheral->transaction.slaveAddress == 0 )
    {
        return IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }

    if( _getState( pxI2CPeripheral ) == eI2cBusBusy )
    {
        return IOT_I2C_BUSY;
    }

    int32_t ret = IOT_I2C_SUCCESS;
    bool status = false;

    if( op == _OP_READ )
    {
        pxI2CPeripheral->transaction.readBuf = pvBuffer;
        pxI2CPeripheral->transaction.readCount = xBytes;
    }
    else
    {
        pxI2CPeripheral->transaction.writeBuf = pvBuffer;
        pxI2CPeripheral->transaction.writeCount = xBytes;
    }

    /* If sending STOP condition. */
    if( _isFlagNotSet( pxI2CPeripheral, _FLAG_NO_STOP ) )
    {
        status = I2C_transfer( pxI2CPeripheral->handle,
                               &pxI2CPeripheral->transaction );

        if( status )
        {
            /* If sync, wait for the transfer to finish. */
            if( ( mode == _MODE_SYNC ) && ( xSemaphoreTake( pxI2CPeripheral->semphr, pdMS_TO_TICKS( pxI2CPeripheral->config.ulMasterTimeout ) ) == pdFALSE ) )
            {
                /* If callback is not invoked with timeout, return timeout. */
                ret = IOT_I2C_BUS_TIMEOUT;
            }
        }
        else
        {
            ret = IOT_I2C_NACK;
        }

        pxI2CPeripheral->transaction.writeBuf = NULL;
        pxI2CPeripheral->transaction.readBuf = NULL;
    }
    else
    {
        pxI2CPeripheral->flags &= ~_FLAG_NO_STOP;

        /* If async, invoke user's callback. */
        if( ( mode == _MODE_ASYNC ) && ( pxI2CPeripheral->userCallback != NULL ) )
        {
            pxI2CPeripheral->userCallback( eI2CCompleted, pxI2CPeripheral->userContext );
        }
    }

    return ret;
}

static void _callbackInternal( I2C_Handle handle,
                               I2C_Transaction * pTransaction,
                               bool transferStatus )
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if( _i2cDescriptor.userCallback != NULL )
    {
        _i2cDescriptor.userCallback( transferStatus ? eI2CCompleted : eI2CDriverFailed, _i2cDescriptor.userContext );
    }

    _i2cDescriptor.transaction.writeBuf = NULL;
    _i2cDescriptor.transaction.readBuf = NULL;

    xSemaphoreGiveFromISR( _i2cDescriptor.semphr, &higherPriorityTaskWoken );
    portYIELD_FROM_ISR( higherPriorityTaskWoken );
}

/*
 * -----------------------------------------------------------------------------------
 */

static I2C_BitRate _frequencyToBitRate( uint32_t frequency )
{
    I2C_BitRate ebitRate;

    if( frequency >= 1000000 )
    {
        ebitRate = I2C_1000kHz;
    }

    if( frequency >= 400000 )
    {
        ebitRate = I2C_400kHz;
    }
    else
    {
        ebitRate = I2C_100kHz;
    }

    return ebitRate;
}
