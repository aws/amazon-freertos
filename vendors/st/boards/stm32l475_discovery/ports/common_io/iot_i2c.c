/*
 * FreeRTOS Common IO V0.1.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_i2c.c
 * @brief HAL i2c implementation on STM32L4 Discovery Board
 */

/* The config header is always included first. */
#include "iot_config.h"

/* ST Board includes. */
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_i2c.h"
#include "stm32l475e_iot01.h"

/* Main includes. */
#include "iot_i2c.h"

/* FreeRTOS includes. */
#include "semphr.h"

/* Used by the "flags" member of IotI2CDescriptor_t. */
#define _FLAG_OPENED            ( 0x01 ) /* If the instance is already opened. */
#define _FLAG_SLAVE_ADDRESS     ( 0x02 ) /* If the slave address has been set. */
#define _FLAG_NO_STOP           ( 0x04 ) /* If the no stop has been requested. */

/* Initializer of the "flags" member of IotI2CDescriptor_t. */
#define _FLAG_INITIALIZER       ( 0 )

/* Initial state if slave address is unset. */
#define _UNSET_SLAVE_ADDRESS    ( 0xFFFFU )

/* Read or write operation. */
#define _I2C_READ_OP            ( 0 )
#define _I2C_WRITE_OP           ( 1 )

/* Read or write operation. */
#define _I2C_SYNC_OP            ( 0 )
#define _I2C_ASYNC_OP           ( 1 )

/* First bit: Tx busy; Second bit: Rx busy */
#define _I2C_STATE_BUSY_MASK    ( 0x3 )

/* Calculate the address of IotI2CDescriptor_t from the address of its 'xHandle'. */
#define _toDescriptor( pHandle )                 ( ( IotI2CDescriptor_t * ) ( ( uint8_t * ) pHandle - offsetof( IotI2CDescriptor_t, xHandle ) ) )

/* Helper macro function to check if a specific flag is set or not. */
#define _isFlagSet( pI2cDescriptor, flag )       ( ( ( pI2cDescriptor )->flags & ( flag ) ) > 0 )
#define _isFlagNotSet( pI2cDescriptor, flag )    ( ( ( pI2cDescriptor )->flags & ( flag ) ) == 0 )

typedef struct IotI2CDescriptor
{
    I2C_HandleTypeDef xHandle;     /* ST Handle */
    IotI2CConfig_t xConfig;        /* Bus Configuration */
    IotI2CCallback_t xCallback;    /* Callback function */
    void * pvUserContext;          /* User context passed in callback */
    SemaphoreHandle_t xSemphr;
    uint16_t usSlaveAddr;          /* Slave Address */
    uint16_t usTransmittedTxBytes; /* Number of Transmitted Bytes */
    uint16_t usReceivedRxBytes;    /* Number of Received Bytes */
    uint8_t flags;                 /* Bit flags to tract different states. */
} IotI2CDescriptor_t;

static IotI2CDescriptor_t _i2cContexts[] =
{
    {
        .xHandle =
        {
            .Instance             = I2C1,
            .Init.Timing          = DISCOVERY_I2Cx_TIMING,
            .Init.OwnAddress1     = 0,
            .Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT,
            .Init.DualAddressMode = I2C_DUALADDRESS_DISABLE,
            .Init.OwnAddress2     = 0,
            .Init.GeneralCallMode = I2C_GENERALCALL_DISABLE,
            .Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE,
        },
        .xConfig ={ 0            },
        .xSemphr = NULL,
        .usSlaveAddr = _UNSET_SLAVE_ADDRESS,
        .usTransmittedTxBytes = 0,
        .usReceivedRxBytes = 0,
        .flags = _FLAG_INITIALIZER
    },

    {
        .xHandle =
        {
            .Instance             = I2C2,
            .Init.Timing          = DISCOVERY_I2Cx_TIMING,
            .Init.OwnAddress1     = 0,
            .Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT,
            .Init.DualAddressMode = I2C_DUALADDRESS_DISABLE,
            .Init.OwnAddress2     = 0,
            .Init.GeneralCallMode = I2C_GENERALCALL_DISABLE,
            .Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE,
        },
        .xConfig ={ 0            },
        .xSemphr = NULL,
        .usSlaveAddr = _UNSET_SLAVE_ADDRESS,
        .usTransmittedTxBytes = 0,
        .usReceivedRxBytes = 0,
        .flags = _FLAG_INITIALIZER
    },

    {
        .xHandle =
        {
            .Instance             = I2C3,
            .Init.Timing          = DISCOVERY_I2Cx_TIMING,
            .Init.OwnAddress1     = 0,
            .Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT,
            .Init.DualAddressMode = I2C_DUALADDRESS_DISABLE,
            .Init.OwnAddress2     = 0,
            .Init.GeneralCallMode = I2C_GENERALCALL_DISABLE,
            .Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE,
        },
        .xConfig ={ 0            },
        .xSemphr = NULL,
        .usSlaveAddr = _UNSET_SLAVE_ADDRESS,
        .usTransmittedTxBytes = 0,
        .usReceivedRxBytes = 0,
        .flags = _FLAG_INITIALIZER
    }
};



/*---------------------Private functions---------------------*/

static int32_t _toI2cStatus( HAL_StatusTypeDef halStatus,
                             uint8_t operation );

static int32_t _doMasterTransfer( IotI2CDescriptor_t * pI2cDescriptor,
                                  uint8_t * const pucBuffer,
                                  size_t xBytes,
                                  HAL_StatusTypeDef ( * pMasterSync )( I2C_HandleTypeDef * hi2c,
                                                                       uint16_t DevAddress,
                                                                       uint8_t * pData,
                                                                       uint16_t Size,
                                                                       uint32_t Timeout ),
                                  HAL_StatusTypeDef ( * pMasterIT )( I2C_HandleTypeDef * hi2c,
                                                                     uint16_t DevAddress,
                                                                     uint8_t * pData,
                                                                     uint16_t Size ),
                                  HAL_StatusTypeDef ( * pMasterSequentialIT )( I2C_HandleTypeDef * hi2c,
                                                                               uint16_t DevAddress,
                                                                               uint8_t * pData,
                                                                               uint16_t Size,
                                                                               uint32_t XferOptions ) );

/*--------------------API Implementation---------------------*/

IotI2CHandle_t iot_i2c_open( int32_t lI2CInstance )
{
    IotI2CDescriptor_t * pI2cDescriptor = NULL;
    HAL_StatusTypeDef halStatus = HAL_OK;

    if( ( lI2CInstance >= 0 ) && ( lI2CInstance < sizeof( _i2cContexts ) / sizeof( IotI2CDescriptor_t ) ) )
    {
        pI2cDescriptor = &_i2cContexts[ lI2CInstance ];
        I2C_HandleTypeDef * pHalHandle = &( pI2cDescriptor->xHandle );

        /* Not opened yet. */
        if( _isFlagNotSet( pI2cDescriptor, _FLAG_OPENED ) )
        {
            halStatus = HAL_I2C_Init( pHalHandle );

            if( halStatus == HAL_OK )
            {
                /* Configure Analog filter. */
                halStatus = HAL_I2CEx_ConfigAnalogFilter( pHalHandle, I2C_ANALOGFILTER_ENABLE );
            }

            if( halStatus == HAL_OK )
            {
                pI2cDescriptor->xSemphr = xSemaphoreCreateBinary();

                /* If semaphore creation fails, set returned handle to NULL. */
                if( pI2cDescriptor->xSemphr == NULL )
                {
                    pI2cDescriptor = NULL;
                }
            }
            else
            {
                /* In failure case, set returned handle to NULL. */
                pI2cDescriptor = NULL;
            }
        }
        else
        {
            /* Return NULL handle if opened more than once. */
            pI2cDescriptor = NULL;
        }

        if( pI2cDescriptor != NULL )
        {
            /* Set opened flag. */
            pI2cDescriptor->flags |= _FLAG_OPENED;
        }
    }

    return pI2cDescriptor;
}

void iot_i2c_set_callback( IotI2CHandle_t const pxI2CPeripheral,
                           IotI2CCallback_t xCallback,
                           void * pvUserContext )
{
    if( ( pxI2CPeripheral != NULL ) && ( ( pxI2CPeripheral->flags & _FLAG_OPENED ) == 1 ) )
    {
        pxI2CPeripheral->xCallback = xCallback;
        pxI2CPeripheral->pvUserContext = pvUserContext;
    }
    else
    {
        /* Silently returns if input handle is invalid. */
    }
}

int32_t iot_i2c_read_sync( IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t * const pucBuffer,
                           size_t xBytes )
{
    int32_t status = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pI2cDescriptor = pxI2CPeripheral;
    HAL_StatusTypeDef halStatus = HAL_OK;

    if( ( pI2cDescriptor == NULL ) ||
        ( _isFlagNotSet( pI2cDescriptor, _FLAG_OPENED ) ) ||
        ( pucBuffer == NULL ) ||
        ( xBytes == 0 ) )
    {
        status = IOT_I2C_INVALID_VALUE;
    }
    else if( _isFlagNotSet( pI2cDescriptor, _FLAG_SLAVE_ADDRESS ) )
    {
        status = IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }
    else
    {
        /* Initialize with the requested number of bytes. */
        pI2cDescriptor->usReceivedRxBytes = xBytes;

        halStatus = _doMasterTransfer( pI2cDescriptor, pucBuffer, xBytes, HAL_I2C_Master_Receive, NULL, HAL_I2C_Master_Sequential_Receive_IT );

        status = _toI2cStatus( halStatus, _I2C_READ_OP );
    }

    return status;
}

int32_t iot_i2c_write_sync( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pucBuffer,
                            size_t xBytes )
{
    int32_t status = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pI2cDescriptor = pxI2CPeripheral;
    HAL_StatusTypeDef halStatus = HAL_OK;

    if( ( pI2cDescriptor == NULL ) ||
        ( _isFlagNotSet( pI2cDescriptor, _FLAG_OPENED ) ) ||
        ( pucBuffer == NULL ) ||
        ( xBytes == 0 ) )
    {
        status = IOT_I2C_INVALID_VALUE;
    }
    else if( _isFlagNotSet( pI2cDescriptor, _FLAG_SLAVE_ADDRESS ) )
    {
        status = IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }
    else
    {
        /* Initialize with the requested number of bytes. */
        pI2cDescriptor->usTransmittedTxBytes = xBytes;

        halStatus = _doMasterTransfer( pI2cDescriptor, pucBuffer, xBytes, HAL_I2C_Master_Transmit, NULL, HAL_I2C_Master_Sequential_Transmit_IT );

        status = _toI2cStatus( halStatus, _I2C_WRITE_OP );
    }

    return status;
}

int32_t iot_i2c_read_async( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pucBuffer,
                            size_t xBytes )
{
    int32_t status = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pI2cDescriptor = pxI2CPeripheral;
    HAL_StatusTypeDef halStatus = HAL_OK;

    if( ( pI2cDescriptor == NULL ) ||
        ( _isFlagNotSet( pI2cDescriptor, _FLAG_OPENED ) ) ||
        ( pucBuffer == NULL ) ||
        ( xBytes == 0 ) )
    {
        status = IOT_I2C_INVALID_VALUE;
    }
    else if( _isFlagNotSet( pI2cDescriptor, _FLAG_SLAVE_ADDRESS ) )
    {
        status = IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }
    else
    {
        /* Initialize with the requested number of bytes. */
        pI2cDescriptor->usReceivedRxBytes = xBytes;

        halStatus = _doMasterTransfer( pI2cDescriptor, pucBuffer, xBytes, NULL, HAL_I2C_Master_Receive_IT, HAL_I2C_Master_Sequential_Receive_IT );

        status = _toI2cStatus( halStatus, _I2C_READ_OP );
    }

    return status;
}

int32_t iot_i2c_write_async( IotI2CHandle_t const pxI2CPeripheral,
                             uint8_t * const pucBuffer,
                             size_t xBytes )
{
    int32_t status = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pI2cDescriptor = pxI2CPeripheral;
    HAL_StatusTypeDef halStatus = HAL_OK;

    if( ( pI2cDescriptor == NULL ) ||
        ( _isFlagNotSet( pI2cDescriptor, _FLAG_OPENED ) ) ||
        ( pucBuffer == NULL ) ||
        ( xBytes == 0 ) )
    {
        status = IOT_I2C_INVALID_VALUE;
    }
    else if( _isFlagNotSet( pI2cDescriptor, _FLAG_SLAVE_ADDRESS ) )
    {
        status = IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }
    else
    {
        /* Initialize with the requested number of bytes. */
        pI2cDescriptor->usTransmittedTxBytes = xBytes;

        halStatus = _doMasterTransfer( pI2cDescriptor, pucBuffer, xBytes, NULL, HAL_I2C_Master_Transmit_IT, HAL_I2C_Master_Sequential_Transmit_IT );

        status = _toI2cStatus( halStatus, _I2C_WRITE_OP );
    }

    return status;
}

int32_t iot_i2c_cancel( IotI2CHandle_t const pxI2CPeripheral )
{
    /* IotI2CDescriptor_t * pI2cDescriptor = pxI2CPeripheral;
     *
     * if( ( pI2cDescriptor == NULL ) ||
     *  ( _isFlagNotSet( pI2cDescriptor, _FLAG_OPENED ) ) )
     * {
     *  return IOT_I2C_INVALID_VALUE;
     * }
     *
     * if( pxI2CPeripheral->xHandle.State == HAL_I2C_STATE_READY )
     * {
     *  return IOT_I2C_NOTHING_TO_CANCEL;
     * } */

    /* TODO: after write_async or read_async, calling HAL_I2C_Master_Abort_IT immediately will leave the bus in a weird state,
     * so that any operation afterwards will return timeout or error.
     */
    return IOT_I2C_FUNCTION_NOT_SUPPORTED;
}

int32_t iot_i2c_ioctl( IotI2CHandle_t const pxI2CPeripheral,
                       IotI2CIoctlRequest_t xI2CRequest,
                       void * const pvBuffer )
{
    int32_t status = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pI2cDescriptor = pxI2CPeripheral;

    if( ( pI2cDescriptor == NULL ) ||
        ( _isFlagNotSet( pI2cDescriptor, _FLAG_OPENED ) ) )
    {
        status = IOT_I2C_INVALID_VALUE;
    }
    else
    {
        switch( xI2CRequest )
        {
            case eI2CSendNoStopFlag:

                pI2cDescriptor->flags |= _FLAG_NO_STOP;

                break;

            case eI2CSetSlaveAddr:

                if( pvBuffer == NULL )
                {
                    status = IOT_I2C_INVALID_VALUE;
                }
                else
                {
                    pI2cDescriptor->usSlaveAddr = *( uint16_t * ) pvBuffer;

                    /* Set slave address flag. */
                    pI2cDescriptor->flags |= _FLAG_SLAVE_ADDRESS;
                }

                break;

            case eI2CSetMasterConfig:

                if( pvBuffer == NULL )
                {
                    status = IOT_I2C_INVALID_VALUE;
                }
                /* TODO: here it assumes the state is busy if it is not ready, but other states might also be valid to set master config. */
                else if( pI2cDescriptor->xHandle.State != HAL_I2C_STATE_READY )
                {
                    status = IOT_I2C_BUSY;
                }
                else
                {
                    pI2cDescriptor->xConfig = *( IotI2CConfig_t * ) pvBuffer;
                }

                break;

            case eI2CGetMasterConfig:

                if( pvBuffer == NULL )
                {
                    status = IOT_I2C_INVALID_VALUE;
                }
                else
                {
                    *( IotI2CConfig_t * ) pvBuffer = pI2cDescriptor->xConfig;
                }

                break;

            case eI2CGetBusState:

                if( pvBuffer == NULL )
                {
                    status = IOT_I2C_INVALID_VALUE;
                }
                else
                {
                    *( IotI2CBusStatus_t * ) pvBuffer = pI2cDescriptor->xHandle.State == HAL_I2C_STATE_READY ? eI2CBusIdle : eI2cBusBusy;
                }

                break;

            /* TODO: have not tested */
            case eI2CBusReset:

                DISCOVERY_I2Cx_FORCE_RESET();

                DISCOVERY_I2Cx_RELEASE_RESET();

                break;

            case eI2CGetTxNoOfbytes:

                if( pvBuffer == NULL )
                {
                    status = IOT_I2C_INVALID_VALUE;
                }
                else
                {
                    *( uint16_t * ) pvBuffer = pxI2CPeripheral->usTransmittedTxBytes;
                }

                break;

            case eI2CGetRxNoOfbytes:

                if( pvBuffer == NULL )
                {
                    status = IOT_I2C_INVALID_VALUE;
                }
                else
                {
                    *( uint16_t * ) pvBuffer = pxI2CPeripheral->usReceivedRxBytes;
                }

                break;

            default:
                status = IOT_I2C_INVALID_VALUE;
        }
    }

    return status;
}

int32_t iot_i2c_close( IotI2CHandle_t const pxI2CPeripheral )
{
    int32_t status = IOT_I2C_SUCCESS;
    IotI2CDescriptor_t * pI2cDescriptor = pxI2CPeripheral;

    if( ( pI2cDescriptor == NULL ) ||
        ( ( pI2cDescriptor->flags & _FLAG_OPENED ) == 0 ) )
    {
        status = IOT_I2C_INVALID_VALUE;
    }
    else
    {
        /* HAL_I2C_DeInit always returns HAL_OK as long as the handle is not NULL. */
        HAL_I2C_DeInit( &pI2cDescriptor->xHandle );

        pI2cDescriptor->xCallback = NULL;
        pI2cDescriptor->pvUserContext = NULL;
        pI2cDescriptor->usSlaveAddr = _UNSET_SLAVE_ADDRESS;
        pI2cDescriptor->usTransmittedTxBytes = 0;
        pI2cDescriptor->usReceivedRxBytes = 0;
        pI2cDescriptor->flags = _FLAG_INITIALIZER;
        pI2cDescriptor->xConfig.ulBusFreq = 0;
        pI2cDescriptor->xConfig.ulMasterTimeout = 0;

        vSemaphoreDelete( pI2cDescriptor->xSemphr );
    }

    return status;
}

/*-----------------------------------------------------------*/

/*
 * Actual ISR registered in vector table.
 */
void I2C1_EV_IRQHandler( void )
{
    HAL_I2C_EV_IRQHandler( &( _i2cContexts[ 0 ].xHandle ) );
}

void I2C1_ER_IRQHandler( void )
{
    HAL_I2C_ER_IRQHandler( &( _i2cContexts[ 0 ].xHandle ) );
}

void I2C2_EV_IRQHandler( void )
{
    HAL_I2C_EV_IRQHandler( &( _i2cContexts[ 1 ].xHandle ) );
}

void I2C2_ER_IRQHandler( void )
{
    HAL_I2C_ER_IRQHandler( &( _i2cContexts[ 1 ].xHandle ) );
}

void I2C3_EV_IRQHandler( void )
{
    HAL_I2C_EV_IRQHandler( &( _i2cContexts[ 2 ].xHandle ) );
}

void I2C3_ER_IRQHandler( void )
{
    HAL_I2C_ER_IRQHandler( &( _i2cContexts[ 2 ].xHandle ) );
}

/*-------------ST API Implementation Overrides---------------*/

/**
 * @brief Initializes low level hardware
 */
void HAL_I2C_MspInit( I2C_HandleTypeDef * hi2c )
{
    GPIO_InitTypeDef xGpio_init_structure;

    /* Enable GPIO clock. */
    DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

    /* Configure I2C Tx, Rx as alternate function */
    xGpio_init_structure.Pin = DISCOVERY_I2Cx_SCL_PIN | DISCOVERY_I2Cx_SDA_PIN;
    xGpio_init_structure.Mode = GPIO_MODE_AF_OD;
    xGpio_init_structure.Pull = GPIO_PULLUP;
    xGpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    xGpio_init_structure.Alternate = DISCOVERY_I2Cx_SCL_SDA_AF;
    HAL_GPIO_Init( DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, &xGpio_init_structure );

    /* Configure the I2C peripheral */
    DISCOVERY_I2Cx_CLK_ENABLE();

    DISCOVERY_I2Cx_FORCE_RESET();

    DISCOVERY_I2Cx_RELEASE_RESET();

    /* Enable and set I2Cx Interrupt to a lower priority */
    HAL_NVIC_SetPriority( DISCOVERY_I2Cx_EV_IRQn, 0x0F, 0 );
    HAL_NVIC_EnableIRQ( DISCOVERY_I2Cx_EV_IRQn );
}

void HAL_I2C_MspDeInit( I2C_HandleTypeDef * hi2c )
{
    GPIO_InitTypeDef xGpio_init_structure;

    /* Configure I2C Tx, Rx as alternate function */
    xGpio_init_structure.Pin = DISCOVERY_I2Cx_SCL_PIN | DISCOVERY_I2Cx_SDA_PIN;
    HAL_GPIO_DeInit( DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, xGpio_init_structure.Pin );

    DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_DISABLE();

    DISCOVERY_I2Cx_CLK_DISABLE();
}

void HAL_I2C_MasterTxCpltCallback( I2C_HandleTypeDef * hi2c )
{
    IotI2CDescriptor_t * pDescriptor = _toDescriptor( hi2c );
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    /* Subtract not transferred yet. */
    pDescriptor->usTransmittedTxBytes -= hi2c->XferCount;

    if( pDescriptor->xCallback != NULL )
    {
        pDescriptor->xCallback( eI2CCompleted, pDescriptor->pvUserContext );
    }

    xSemaphoreGiveFromISR( pDescriptor->xSemphr, &higherPriorityTaskWoken );
    portYIELD_FROM_ISR( higherPriorityTaskWoken );
}

void HAL_I2C_MasterRxCpltCallback( I2C_HandleTypeDef * hi2c )
{
    IotI2CDescriptor_t * pDescriptor = _toDescriptor( hi2c );
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    /* Subtract not transferred yet. */
    pDescriptor->usReceivedRxBytes -= hi2c->XferCount;

    if( pDescriptor->xCallback != NULL )
    {
        pDescriptor->xCallback( eI2CCompleted, pDescriptor->pvUserContext );
    }

    xSemaphoreGiveFromISR( pDescriptor->xSemphr, &higherPriorityTaskWoken );
    portYIELD_FROM_ISR( higherPriorityTaskWoken );
}

void HAL_I2C_ErrorCallback( I2C_HandleTypeDef * hi2c )
{
    IotI2CDescriptor_t * pDescriptor = _toDescriptor( hi2c );
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if( pDescriptor->xCallback != NULL )
    {
        pDescriptor->xCallback( eI2CCompleted, pDescriptor->pvUserContext );
    }

    xSemaphoreGiveFromISR( pDescriptor->xSemphr, &higherPriorityTaskWoken );
    portYIELD_FROM_ISR( higherPriorityTaskWoken );
}

/*---------------------Private functions---------------------*/

static int32_t _toI2cStatus( HAL_StatusTypeDef halStatus,
                             uint8_t operation )
{
    int32_t status = IOT_I2C_INVALID_VALUE;

    switch( halStatus )
    {
        case HAL_OK:
            status = IOT_I2C_SUCCESS;
            break;

        case HAL_ERROR:
            status = operation == _I2C_READ_OP ? IOT_I2C_READ_FAILED : IOT_I2C_WRITE_FAILED;
            break;

        case HAL_BUSY:
            status = IOT_I2C_BUSY;
            break;

        case HAL_TIMEOUT:
            status = IOT_I2C_BUS_TIMEOUT;
            break;
    }

    return status;
}

static int32_t _doMasterTransfer( IotI2CDescriptor_t * pI2cDescriptor,
                                  uint8_t * const pucBuffer,
                                  size_t xBytes,
                                  HAL_StatusTypeDef ( * pMasterSync )( I2C_HandleTypeDef * hi2c,
                                                                       uint16_t DevAddress,
                                                                       uint8_t * pData,
                                                                       uint16_t Size,
                                                                       uint32_t Timeout ),
                                  HAL_StatusTypeDef ( * pMasterIT )( I2C_HandleTypeDef * hi2c,
                                                                     uint16_t DevAddress,
                                                                     uint8_t * pData,
                                                                     uint16_t Size ),
                                  HAL_StatusTypeDef ( * pMasterSequentialIT )( I2C_HandleTypeDef * hi2c,
                                                                               uint16_t DevAddress,
                                                                               uint8_t * pData,
                                                                               uint16_t Size,
                                                                               uint32_t XferOptions ) )
{
    HAL_StatusTypeDef halStatus = HAL_OK;
    uint8_t ucAsyncCall = 1;

    uint8_t ongoingTrans = ( pI2cDescriptor->xHandle.PreviousState & _I2C_STATE_BUSY_MASK ) > 0;

    /* Do not send stop condition. */
    if( _isFlagSet( pI2cDescriptor, _FLAG_NO_STOP ) )
    {
        /* If the previous state is ready, then there is no ongoing transaction; otherwise, there is. */
        halStatus = pMasterSequentialIT( &( pI2cDescriptor->xHandle ), pI2cDescriptor->usSlaveAddr, pucBuffer, xBytes, ongoingTrans ? I2C_NEXT_FRAME : I2C_FIRST_FRAME );

        /* Clear no stop flag. */
        pI2cDescriptor->flags &= ~_FLAG_NO_STOP;
    }
    else
    {
        if( ongoingTrans )
        {
            /* There is an ongoing transaction; end the transaction. */
            halStatus = pMasterSequentialIT( &( pI2cDescriptor->xHandle ), pI2cDescriptor->usSlaveAddr, pucBuffer, xBytes, I2C_LAST_FRAME );
        }
        else
        {
            if( pMasterSync != NULL )
            {
                /* No ongoing transaction; simply make a blocking call. */
                halStatus = pMasterSync( &( pI2cDescriptor->xHandle ), pI2cDescriptor->usSlaveAddr, pucBuffer, xBytes, pI2cDescriptor->xConfig.ulMasterTimeout );

                /* No async call is made. */
                ucAsyncCall = 0;
            }
            else
            {
                /* No ongoing transaction; simple make a interrupted call. */
                halStatus = pMasterIT( &( pI2cDescriptor->xHandle ), pI2cDescriptor->usSlaveAddr, pucBuffer, xBytes );
            }
        }
    }

    /* An async driver call has been made so it must wait for the callback before return,
     * only if the transfer has succeeded and this comes from a sync API call. */
    if( ( halStatus == HAL_OK ) && ( pMasterSync != NULL ) && ( ucAsyncCall == 1 ) )
    {
        if( xSemaphoreTake( pI2cDescriptor->xSemphr, pdMS_TO_TICKS( pI2cDescriptor->xConfig.ulMasterTimeout ) ) == pdFALSE )
        {
            /* If callback is not invoked with timeout, return timeout. */
            halStatus = HAL_TIMEOUT;
        }
    }

    return halStatus;
}
