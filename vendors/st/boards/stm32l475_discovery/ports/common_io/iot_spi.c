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
 * @file iot_spi.c
 * @brief HAL spi implementation on STM32L4 Discovery Board
 */

/* ST Board includes. */
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"

/* Main includes. */
#include "iot_spi.h"

/* Total number of SPI instances on this ST microcontroller. */
#define IOT_SPI_BLOCKING_TIMEOUT    ( ( uint32_t ) 3000UL )
#define IOT_SPI_CLOSED              ( ( uint8_t ) 0 )
#define IOT_SPI_OPENED              ( ( uint8_t ) 1 )

typedef struct STM32_SPI_HalContext
{
    SPI_HandleTypeDef * pxSpi;
    IRQn_Type eIrqNum;
} STM32_SPI_HalContext_t;

typedef struct IotSPIDescriptor
{
    STM32_SPI_HalContext_t const * pxSpiContext; /* ST Handle */
    IotSPIMasterConfig_t xConfig;                /* Master Configuration */
    IotSPICallback_t xSpiCallback;               /* Callback function */
    void * pvUserContext;                        /* User context passed in callback */
    uint8_t sOpened;                             /* Bit flags to track different states. */
} IotSPIDescriptor_t;
/*-----------------------------------------------------------*/

extern uint32_t SystemCoreClock;

static SPI_HandleTypeDef xSpiHandleMap[] =
{
    {
        .Instance = SPI1,
        .Init =
        {
            .Mode              = SPI_MODE_MASTER,
            .Direction         = SPI_DIRECTION_2LINES,
            .DataSize          = SPI_DATASIZE_8BIT,
            .CLKPolarity       = SPI_POLARITY_LOW,
            .CLKPhase          = SPI_PHASE_1EDGE,
            .NSS               = SPI_NSS_SOFT,
            .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2,
            .FirstBit          = SPI_FIRSTBIT_MSB,
            .TIMode            = SPI_TIMODE_DISABLE,
            .CRCCalculation    = SPI_CRCCALCULATION_DISABLE,
            .CRCPolynomial     = 7,
            .CRCLength         = SPI_CRC_LENGTH_DATASIZE,
            .NSSPMode          = SPI_NSS_PULSE_ENABLE,
        },
        .pTxBuffPtr = NULL,
        .TxXferSize = 0,
        .TxXferCount = 0,
        .RxXferSize = 0,
        .RxXferCount = 0,
    },
    {
        .Instance = SPI2,
        .Init =
        {
            .Mode              = SPI_MODE_MASTER,
            .Direction         = SPI_DIRECTION_2LINES,
            .DataSize          = SPI_DATASIZE_8BIT,
            .CLKPolarity       = SPI_POLARITY_LOW,
            .CLKPhase          = SPI_PHASE_1EDGE,
            .NSS               = SPI_NSS_SOFT,
            .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2,
            .FirstBit          = SPI_FIRSTBIT_MSB,
            .TIMode            = SPI_TIMODE_DISABLE,
            .CRCCalculation    = SPI_CRCCALCULATION_DISABLE,
            .CRCPolynomial     = 7,
            .CRCLength         = SPI_CRC_LENGTH_DATASIZE,
            .NSSPMode          = SPI_NSS_PULSE_ENABLE,
        },
        .pTxBuffPtr = NULL,
        .TxXferSize = 0,
        .TxXferCount = 0,
        .RxXferSize = 0,
        .RxXferCount = 0,
    },
    {
        .Instance = SPI3,
        .Init =
        {
            .Mode              = SPI_MODE_MASTER,
            .Direction         = SPI_DIRECTION_2LINES,
            .DataSize          = SPI_DATASIZE_8BIT,
            .CLKPolarity       = SPI_POLARITY_LOW,
            .CLKPhase          = SPI_PHASE_1EDGE,
            .NSS               = SPI_NSS_SOFT,
            .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2,
            .FirstBit          = SPI_FIRSTBIT_MSB,
            .TIMode            = SPI_TIMODE_DISABLE,
            .CRCCalculation    = SPI_CRCCALCULATION_DISABLE,
            .CRCPolynomial     = 7,
            .CRCLength         = SPI_CRC_LENGTH_DATASIZE,
            .NSSPMode          = SPI_NSS_PULSE_ENABLE,
        },
        .pTxBuffPtr = NULL,
        .TxXferSize = 0,
        .TxXferCount = 0,
        .RxXferSize = 0,
        .RxXferCount = 0,
    }
};

static const STM32_SPI_HalContext_t xSpiContexts[] =
{
    {
        .pxSpi = &xSpiHandleMap[ 0 ],
        .eIrqNum = SPI1_IRQn,
    },
    {
        .pxSpi = &xSpiHandleMap[ 1 ],
        .eIrqNum = SPI2_IRQn,
    },
    {
        .pxSpi = &xSpiHandleMap[ 2 ],
        .eIrqNum = SPI3_IRQn,
    }
};
/*-----------------------------------------------------------*/

static IotSPIDescriptor_t xSpi1 =
{
    .pxSpiContext     = &xSpiContexts[ 0 ],
    .xConfig          =
    {
        .ulFreq       = 0,
        .eMode        = eSPIMode0,
        .eSetBitOrder = eSPIMSBFirst,
        .ucDummyValue = 0
    },
    .xSpiCallback     = NULL,
    .pvUserContext    = NULL,
    .sOpened          = IOT_SPI_CLOSED,
};

static IotSPIDescriptor_t xSpi2 =
{
    .pxSpiContext     = &xSpiContexts[ 1 ],
    .xConfig          =
    {
        .ulFreq       = 0,
        .eMode        = eSPIMode0,
        .eSetBitOrder = eSPIMSBFirst,
        .ucDummyValue = 0
    },
    .xSpiCallback     = NULL,
    .pvUserContext    = NULL,
    .sOpened          = IOT_SPI_CLOSED,
};

static IotSPIDescriptor_t xSpi3 =
{
    .pxSpiContext     = &xSpiContexts[ 2 ],
    .xConfig          =
    {
        .ulFreq       = 0,
        .eMode        = eSPIMode0,
        .eSetBitOrder = eSPIMSBFirst,
        .ucDummyValue = 0
    },
    .xSpiCallback     = NULL,
    .pvUserContext    = NULL,
    .sOpened          = IOT_SPI_CLOSED,
};
/*-----------------------------------------------------------*/

static IotSPIHandle_t const pxSpis[] = { &xSpi1, &xSpi2, &xSpi3 };

/*--------------------API Implementation---------------------*/

IotSPIHandle_t iot_spi_open( int32_t lSpiInstance )
{
    IotSPIHandle_t xHandle = NULL;

    if( ( lSpiInstance >= 0 ) && ( lSpiInstance < sizeof( pxSpis ) / sizeof( IotSPIHandle_t ) ) )
    {
        xHandle = pxSpis[ lSpiInstance ];

        if( xHandle->sOpened == IOT_SPI_CLOSED )
        {
            if( HAL_SPI_Init( xHandle->pxSpiContext->pxSpi ) != HAL_OK )
            {
                xHandle = NULL;
            }
            else
            {
                xHandle->sOpened = IOT_SPI_OPENED;

                xHandle->xConfig.ulFreq = ( SystemCoreClock >> 1 ); /* Default prescaler is 2 and freq = clock / prescaler */
            }
        }
        else
        {
            xHandle = NULL;
        }
    }

    return xHandle;
}
/*-----------------------------------------------------------*/

void iot_spi_set_callback( IotSPIHandle_t const pxSPIPeripheral,
                           IotSPICallback_t xCallback,
                           void * pvUserContext )
{
    if( ( pxSPIPeripheral != NULL ) )
    {
        pxSPIPeripheral->xSpiCallback = xCallback;
        pxSPIPeripheral->pvUserContext = pvUserContext;
    }
}
/*-----------------------------------------------------------*/

int32_t iot_spi_ioctl( IotSPIHandle_t const pxSPIPeripheral,
                       IotSPIIoctlRequest_t xSPIRequest,
                       void * const pvBuffer )
{
    int32_t lError = IOT_SPI_INVALID_VALUE;

    if( ( pxSPIPeripheral != NULL ) && ( pvBuffer != NULL ) )
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        switch( xSPIRequest )
        {
            case eSPISetMasterConfig:

                if( HAL_SPI_GetState( pxSpi ) == HAL_SPI_STATE_BUSY )
                {
                    lError = IOT_SPI_BUS_BUSY;
                }
                else
                {
                    /* NOTE: HAL API sends dummy data as the same data in the master buffer. So dummy data does not need to be set in config. */
                    if( ( ( IotSPIMasterConfig_t * ) pvBuffer )->eSetBitOrder == eSPIMSBFirst )
                    {
                        LL_SPI_SetTransferBitOrder( pxSpi->Instance, SPI_FIRSTBIT_MSB );
                    }
                    else
                    {
                        LL_SPI_SetTransferBitOrder( pxSpi->Instance, SPI_FIRSTBIT_LSB );
                    }

                    switch( ( ( IotSPIMasterConfig_t * ) pvBuffer )->eMode )
                    {
                        case eSPIMode0:
                            LL_SPI_SetClockPolarity( pxSpi->Instance, SPI_POLARITY_LOW );
                            LL_SPI_SetClockPhase( pxSpi->Instance, SPI_PHASE_1EDGE );
                            break;

                        case eSPIMode1:
                            LL_SPI_SetClockPolarity( pxSpi->Instance, SPI_POLARITY_LOW );
                            LL_SPI_SetClockPhase( pxSpi->Instance, SPI_PHASE_2EDGE );
                            break;

                        case eSPIMode2:
                            LL_SPI_SetClockPolarity( pxSpi->Instance, SPI_POLARITY_HIGH );
                            LL_SPI_SetClockPhase( pxSpi->Instance, SPI_PHASE_1EDGE );
                            break;

                        case eSPIMode3:
                            LL_SPI_SetClockPolarity( pxSpi->Instance, SPI_POLARITY_HIGH );
                            LL_SPI_SetClockPhase( pxSpi->Instance, SPI_PHASE_2EDGE );
                            break;

                        default:
                            break;
                    }

                    uint32_t sysClkTmp = SystemCoreClock;
                    uint32_t preScaler = 0;
                    uint8_t divisor = 0;

                    while( ( sysClkTmp > ( ( IotSPIMasterConfig_t * ) pvBuffer )->ulFreq ) && ( divisor < 7 ) )
                    {
                        divisor++;
                        sysClkTmp = ( sysClkTmp >> 1 );
                    }

                    preScaler = ( ( ( divisor & 0x4 ) == 0 ) ? 0x0 : SPI_CR1_BR_2 ) |
                                ( ( ( divisor & 0x2 ) == 0 ) ? 0x0 : SPI_CR1_BR_1 ) |
                                ( ( ( divisor & 0x1 ) == 0 ) ? 0x0 : SPI_CR1_BR_0 );

                    LL_SPI_SetBaudRatePrescaler( pxSpi->Instance, preScaler );
                    lError = IOT_SPI_SUCCESS;
                    pxSPIPeripheral->xConfig = *( IotSPIMasterConfig_t * ) pvBuffer;
                }

                break;

            case eSPIGetMasterConfig:

                ( ( IotSPIMasterConfig_t * ) pvBuffer )->ulFreq = pxSPIPeripheral->xConfig.ulFreq;
                ( ( IotSPIMasterConfig_t * ) pvBuffer )->eMode = pxSPIPeripheral->xConfig.eMode;
                ( ( IotSPIMasterConfig_t * ) pvBuffer )->eSetBitOrder = pxSPIPeripheral->xConfig.eSetBitOrder;
                ( ( IotSPIMasterConfig_t * ) pvBuffer )->ucDummyValue = pxSPIPeripheral->xConfig.ucDummyValue;

                lError = IOT_SPI_SUCCESS;

                break;

            case eSPIGetTxNoOfbytes:

                *( int32_t * ) pvBuffer = ( int32_t ) ( pxSpi->TxXferSize - pxSpi->TxXferCount );
                lError = IOT_SPI_SUCCESS;

                break;

            case eSPIGetRxNoOfbytes:

                *( int32_t * ) pvBuffer = ( int32_t ) ( pxSpi->RxXferSize - pxSpi->RxXferCount );
                lError = IOT_SPI_SUCCESS;

                break;

            default:
                break;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_spi_read_sync( IotSPIHandle_t const pxSPIPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes )
{
    int32_t lError = IOT_SPI_SUCCESS;

    /* TODO: How to find a way to tell if slave is not selected */
    if( ( pxSPIPeripheral == NULL ) || ( pxSPIPeripheral->sOpened == IOT_SPI_CLOSED ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else if( ( pvBuffer == NULL ) || ( xBytes == 0 ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        if( HAL_SPI_GetState( pxSpi ) == HAL_SPI_STATE_BUSY_RX )
        {
            lError = IOT_SPI_BUS_BUSY;
        }
        else if( HAL_SPI_Receive( pxSpi, pvBuffer, ( uint16_t ) xBytes, IOT_SPI_BLOCKING_TIMEOUT ) != HAL_OK )
        {
            lError = IOT_SPI_TRANSFER_ERROR;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_spi_read_async( IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
    int32_t lError = IOT_SPI_SUCCESS;

    /* TODO: Add case for slave not selected? */
    if( ( pxSPIPeripheral == NULL ) || ( pxSPIPeripheral->sOpened == IOT_SPI_CLOSED ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else if( ( pvBuffer == NULL ) || ( xBytes == 0 ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        if( HAL_SPI_GetState( pxSpi ) == HAL_SPI_STATE_BUSY_RX )
        {
            lError = IOT_SPI_BUS_BUSY;
        }
        else
        {
            HAL_NVIC_SetPriority( ( IRQn_Type ) ( ( STM32_SPI_HalContext_t * ) pxSPIPeripheral->pxSpiContext )->eIrqNum, 1, 0 );
            HAL_NVIC_EnableIRQ( ( IRQn_Type ) ( ( STM32_SPI_HalContext_t * ) pxSPIPeripheral->pxSpiContext )->eIrqNum );

            if( HAL_SPI_Receive_IT( pxSpi, pvBuffer, ( uint16_t ) xBytes ) != HAL_OK )
            {
                lError = IOT_SPI_TRANSFER_ERROR;
            }
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_spi_write_sync( IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
    int32_t lError = IOT_SPI_SUCCESS;

    /* TODO: Add case for slave not selected? */
    if( ( pxSPIPeripheral == NULL ) || ( pxSPIPeripheral->sOpened == IOT_SPI_CLOSED ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else if( ( pvBuffer == NULL ) || ( xBytes == 0 ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        if( HAL_SPI_GetState( pxSpi ) == HAL_SPI_STATE_BUSY_TX )
        {
            lError = IOT_SPI_BUS_BUSY;
        }
        else if( HAL_SPI_Transmit( pxSpi, pvBuffer, ( uint16_t ) xBytes, IOT_SPI_BLOCKING_TIMEOUT ) != HAL_OK )
        {
            lError = IOT_SPI_TRANSFER_ERROR;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_spi_write_async( IotSPIHandle_t const pxSPIPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    int32_t lError = IOT_SPI_SUCCESS;

    /* TODO: Add case for slave not selected? */
    if( ( pxSPIPeripheral == NULL ) || ( pxSPIPeripheral->sOpened == IOT_SPI_CLOSED ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else if( ( pvBuffer == NULL ) || ( xBytes == 0 ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        if( HAL_SPI_GetState( pxSpi ) == HAL_SPI_STATE_BUSY_TX )
        {
            lError = IOT_SPI_BUS_BUSY;
        }
        else
        {
            HAL_NVIC_SetPriority( ( IRQn_Type ) ( ( STM32_SPI_HalContext_t * ) pxSPIPeripheral->pxSpiContext )->eIrqNum, 1, 0 );
            HAL_NVIC_EnableIRQ( ( IRQn_Type ) ( ( STM32_SPI_HalContext_t * ) pxSPIPeripheral->pxSpiContext )->eIrqNum );

            if( HAL_SPI_Transmit_IT( pxSpi, pvBuffer, ( uint16_t ) xBytes ) != HAL_OK )
            {
                lError = IOT_SPI_TRANSFER_ERROR;
            }
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_spi_transfer_sync( IotSPIHandle_t const pxSPIPeripheral,
                               uint8_t * const pvTxBuffer,
                               uint8_t * const pvRxBuffer,
                               size_t xBytes )
{
    int32_t lError = IOT_SPI_SUCCESS;

    /* TODO: Add case for slave not selected? */
    if( ( pxSPIPeripheral == NULL ) || ( pxSPIPeripheral->sOpened == IOT_SPI_CLOSED ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else if( ( pvTxBuffer == NULL ) || ( pvRxBuffer == NULL ) || ( xBytes == 0 ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        if( HAL_SPI_GetState( pxSpi ) == HAL_SPI_STATE_BUSY_TX_RX )
        {
            lError = IOT_SPI_BUS_BUSY;
        }
        else if( HAL_SPI_TransmitReceive( pxSpi, pvTxBuffer, pvRxBuffer, ( uint16_t ) xBytes, IOT_SPI_BLOCKING_TIMEOUT ) != HAL_OK )
        {
            lError = IOT_SPI_TRANSFER_ERROR;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_spi_transfer_async( IotSPIHandle_t const pxSPIPeripheral,
                                uint8_t * const pvTxBuffer,
                                uint8_t * const pvRxBuffer,
                                size_t xBytes )
{
    int32_t lError = IOT_SPI_SUCCESS;

    /* TODO: Add case for slave not selected? */
    if( ( pxSPIPeripheral == NULL ) || ( pxSPIPeripheral->sOpened == IOT_SPI_CLOSED ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else if( ( pvTxBuffer == NULL ) || ( pvRxBuffer == NULL ) || ( xBytes == 0 ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        if( HAL_SPI_GetState( pxSpi ) == HAL_SPI_STATE_BUSY_TX_RX )
        {
            lError = IOT_SPI_BUS_BUSY;
        }
        else
        {
            HAL_NVIC_SetPriority( ( IRQn_Type ) ( ( STM32_SPI_HalContext_t * ) pxSPIPeripheral->pxSpiContext )->eIrqNum, 1, 0 );
            HAL_NVIC_EnableIRQ( ( IRQn_Type ) ( ( STM32_SPI_HalContext_t * ) pxSPIPeripheral->pxSpiContext )->eIrqNum );

            if( HAL_SPI_TransmitReceive_IT( pxSpi, pvTxBuffer, pvRxBuffer, ( uint16_t ) xBytes ) != HAL_OK )
            {
                lError = IOT_SPI_TRANSFER_ERROR;
            }
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_spi_close( IotSPIHandle_t const pxSPIPeripheral )
{
    int32_t lError = IOT_SPI_SUCCESS;

    if( ( pxSPIPeripheral == NULL ) || ( pxSPIPeripheral->sOpened == IOT_SPI_CLOSED ) )
    {
        lError = IOT_SPI_INVALID_VALUE;
    }
    else
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        if( HAL_SPI_Abort( pxSpi ) != HAL_OK )
        {
            lError = IOT_SPI_BUS_BUSY;
        }
        else
        {
            /* HAL_SPI_DeInit returns OK as long as input is not NULL. */
            HAL_SPI_DeInit( pxSpi );
            pxSPIPeripheral->sOpened = IOT_SPI_CLOSED;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_spi_cancel( IotSPIHandle_t const pxSPIPeripheral )
{
    int32_t lError = IOT_SPI_INVALID_VALUE;

    if( ( pxSPIPeripheral != NULL ) && ( pxSPIPeripheral->sOpened == IOT_SPI_OPENED ) )
    {
        SPI_HandleTypeDef * pxSpi = pxSPIPeripheral->pxSpiContext->pxSpi;

        if( HAL_SPI_GetState( pxSpi ) == HAL_SPI_STATE_READY )
        {
            lError = IOT_SPI_NOTHING_TO_CANCEL;
        }
        else if( HAL_SPI_Abort( pxSpi ) == HAL_OK )
        {
            lError = IOT_SPI_SUCCESS;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

void HAL_SPI_TxRxCpltCallback( SPI_HandleTypeDef * hspi )
{
    if( hspi->Instance == SPI1 )
    {
        xSpi1.xSpiCallback( eSPISuccess, xSpi1.pvUserContext );
    }
    else if( hspi->Instance == SPI2 )
    {
        xSpi2.xSpiCallback( eSPISuccess, xSpi2.pvUserContext );
    }
    else if( hspi->Instance == SPI3 )
    {
        xSpi3.xSpiCallback( eSPISuccess, xSpi3.pvUserContext );
    }
}
/*-----------------------------------------------------------*/

void HAL_SPI_TxCpltCallback( SPI_HandleTypeDef * hspi )
{
    if( hspi->Instance == SPI1 )
    {
        xSpi1.xSpiCallback( eSPISuccess, xSpi1.pvUserContext );
    }
    else if( hspi->Instance == SPI2 )
    {
        xSpi2.xSpiCallback( eSPISuccess, xSpi2.pvUserContext );
    }
    else if( hspi->Instance == SPI3 )
    {
        xSpi3.xSpiCallback( eSPISuccess, xSpi3.pvUserContext );
    }
}
/*-----------------------------------------------------------*/

void HAL_SPI_RxCpltCallback( SPI_HandleTypeDef * hspi )
{
    if( hspi->Instance == SPI1 )
    {
        xSpi1.xSpiCallback( eSPISuccess, xSpi1.pvUserContext );
    }
    else if( hspi->Instance == SPI2 )
    {
        xSpi2.xSpiCallback( eSPISuccess, xSpi2.pvUserContext );
    }
    else if( hspi->Instance == SPI3 )
    {
        xSpi3.xSpiCallback( eSPISuccess, xSpi3.pvUserContext );
    }
}
/*-----------------------------------------------------------*/

void HAL_SPI_ErrorCallback( SPI_HandleTypeDef * hspi )
{
    if( hspi->Instance == SPI1 )
    {
        xSpi1.xSpiCallback( eSPITransferError, xSpi1.pvUserContext );
    }
    else if( hspi->Instance == SPI2 )
    {
        xSpi2.xSpiCallback( eSPITransferError, xSpi2.pvUserContext );
    }
    else if( hspi->Instance == SPI3 )
    {
        xSpi3.xSpiCallback( eSPITransferError, xSpi3.pvUserContext );
    }
}
/*-----------------------------------------------------------*/

void SPI1_IRQHandler( void )
{
    HAL_SPI_IRQHandler( xSpi1.pxSpiContext->pxSpi );
}
/*-----------------------------------------------------------*/

void SPI2_IRQHandler( void )
{
    HAL_SPI_IRQHandler( xSpi2.pxSpiContext->pxSpi );
}
/*-----------------------------------------------------------*/

void SPI3_IRQHandler( void )
{
    HAL_SPI_IRQHandler( xSpi3.pxSpiContext->pxSpi );
}
/*-----------------------------------------------------------*/
