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
 * @brief HAL spi implementation on NRF52840 Development Kit 
 */

/* Nordic Board includes. */
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"

/* Main includes. */
#include "iot_spi.h"
#include "iot_spi_config.h"

#define IOT_SPI_BLOCKING_TIMEOUT    ( ( uint32_t ) 3000UL ) /* In milliseconds */
#define IOT_SPI_CLOSED              ( ( uint8_t ) 0 )
#define IOT_SPI_OPENED              ( ( uint8_t ) 1 )

typedef struct SpiContext {
    nrf_drv_spi_t        instance;
    nrf_drv_spi_config_t config;
} SpiContext_t;

typedef struct IotSPIDescriptor
{
    SpiContext_t const * pxSpiContext;          /* Nordic Handle */
    IotSPIMasterConfig_t xConfig;               /* Master Configuration */
    IotSPICallback_t     xSpiCallback;          /* Callback function */
    void *               pvUserContext;         /* User context passed in callback */
    uint8_t              sOpened;               /* Bit flags to track different states. */
} IotSPIDescriptor_t;
/*-----------------------------------------------------------*/

static const IotSPIMasterConfig_t xDefaultConfig = {
    .ulFreq       = NRF_DRV_SPI_FREQ_4M,
    .eMode        = eSPIMode0,
    .eSetBitOrder = eSPIMSBFirst,
    .ucDummyValue = 0xFF
};

static SpiContext_t xSpiContexts[] =
{
    {
        .instance = NRF_DRV_SPI_INSTANCE(0),
        .config   = {
            .sck_pin      = IOT_COMMON_IO_SPI_1_SCLK_PIN,                
            .mosi_pin     = IOT_COMMON_IO_SPI_1_MOSI_PIN,                
            .miso_pin     = IOT_COMMON_IO_SPI_1_MISO_PIN,                
            .ss_pin       = NRF_DRV_SPI_PIN_NOT_USED,                
            .irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY,         
            .orc          = xDefaultConfig.ucDummyValue,                                    
            .frequency    = xDefaultConfig.ulFreq,                     
            .mode         = xDefaultConfig.eMode,                      
            .bit_order    = xDefaultConfig.eMode == eSPIMSBFirst ? NRF_DRV_SPI_BIT_ORDER_MSB_FIRST : NRF_DRV_SPI_BIT_ORDER_LSB_FIRST,         
        }
    },
    {
        .instance = NRF_DRV_SPI_INSTANCE(1),
        .config   = {
            .sck_pin      = IOT_COMMON_IO_SPI_2_SCLK_PIN,                
            .mosi_pin     = IOT_COMMON_IO_SPI_2_MOSI_PIN,                
            .miso_pin     = IOT_COMMON_IO_SPI_2_MISO_PIN,                
            .ss_pin       = NRF_DRV_SPI_PIN_NOT_USED,                
            .irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY,         
            .orc          = xDefaultConfig.ucDummyValue,                                    
            .frequency    = xDefaultConfig.ulFreq,                     
            .mode         = xDefaultConfig.eMode,                      
            .bit_order    = xDefaultConfig.eMode == eSPIMSBFirst ? NRF_DRV_SPI_BIT_ORDER_MSB_FIRST : NRF_DRV_SPI_BIT_ORDER_LSB_FIRST,         
        }
    },
    {
        .instance = NRF_DRV_SPI_INSTANCE(2),
        .config   = {
            .sck_pin      = IOT_COMMON_IO_SPI_3_SCLK_PIN,                
            .mosi_pin     = IOT_COMMON_IO_SPI_3_MOSI_PIN,                
            .miso_pin     = IOT_COMMON_IO_SPI_3_MISO_PIN,                
            .ss_pin       = NRF_DRV_SPI_PIN_NOT_USED,                
            .irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY,         
            .orc          = xDefaultConfig.ucDummyValue,                                    
            .frequency    = xDefaultConfig.ulFreq,                     
            .mode         = xDefaultConfig.eMode,                      
            .bit_order    = xDefaultConfig.eMode == eSPIMSBFirst ? NRF_DRV_SPI_BIT_ORDER_MSB_FIRST : NRF_DRV_SPI_BIT_ORDER_LSB_FIRST,          
        }
    }
};

/*
 * Forward declare event handler passed to NRF SPI controller. 
 * Invokes user callbacks for async modes. 
 * Sync modes poll atomic flags to track state
 */
static volatile bool bTransferDone = true;
static void prvSpiEventHandler(nrf_drv_spi_evt_t const * pxEvent, void * pvContext);
static int32_t prvSpiTransfer(nrf_drv_spi_t const * const pxInstance, 
                              uint8_t const * pucTxBuffer,
                              size_t  xTxSize,
                              uint8_t const * pucRxBuffer,
                              size_t xRxSize,
                              bool   bSync);

/*-----------------------------------------------------------*/

static IotSPIDescriptor_t xSpi1 =
{
    .pxSpiContext  = &xSpiContexts[ 0 ],
    .xConfig       = xDefaultConfig,
    .xSpiCallback  = NULL,
    .pvUserContext = NULL,
    .sOpened       = IOT_SPI_CLOSED,
};

static IotSPIDescriptor_t xSpi2 =
{
    .pxSpiContext  = &xSpiContexts[ 1 ],
    .xConfig       = xDefaultConfig,
    .xSpiCallback  = NULL,
    .pvUserContext = NULL,
    .sOpened       = IOT_SPI_CLOSED,
};

static IotSPIDescriptor_t xSpi3 =
{
    .pxSpiContext  = &xSpiContexts[ 2 ],
    .xConfig       = xDefaultConfig,
    .xSpiCallback  = NULL,
    .pvUserContext = NULL,
    .sOpened       = IOT_SPI_CLOSED,
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
            const SpiContext_t * pxSpi = xHandle->pxSpiContext;
            if (NRF_SUCCESS != nrf_drv_spi_init(&pxSpi->instance, &pxSpi->config, prvSpiEventHandler, ( void * ) xHandle))
            {
                xHandle = NULL;
            }
            else
            {
                xHandle->sOpened = IOT_SPI_OPENED;
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
        SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;

        switch( xSPIRequest )
        {
            case eSPISetMasterConfig:

                if( !bTransferDone )
                {
                    lError = IOT_SPI_BUS_BUSY;
                }
                else
                {
                    IotSPIMasterConfig_t * pxUserConfig = ( IotSPIMasterConfig_t * ) pvBuffer;

                    /* Ensure valid user config inputs */
                    if (pxUserConfig->ulFreq != NRF_DRV_SPI_FREQ_125K &&
                        pxUserConfig->ulFreq != NRF_DRV_SPI_FREQ_250K &&
                        pxUserConfig->ulFreq != NRF_DRV_SPI_FREQ_500K &&
                        pxUserConfig->ulFreq != NRF_DRV_SPI_FREQ_1M &&
                        pxUserConfig->ulFreq != NRF_DRV_SPI_FREQ_2M &&
                        pxUserConfig->ulFreq != NRF_DRV_SPI_FREQ_4M &&
                        pxUserConfig->ulFreq != NRF_DRV_SPI_FREQ_8M)
                        {
                            return;
                        }


                    /* Prepare configuration per user request */
                    pxSpi->config.mode = (nrf_drv_spi_mode_t) pxUserConfig->eMode;
                    pxSpi->config.bit_order = pxUserConfig->eSetBitOrder == eSPIMSBFirst ? NRF_DRV_SPI_BIT_ORDER_MSB_FIRST : NRF_DRV_SPI_BIT_ORDER_LSB_FIRST;
                    pxSpi->config.frequency = pxUserConfig->ulFreq;
                    pxSpi->config.orc       = pxUserConfig->ucDummyValue;

                    /* Re-initialize the instance with updated config */
                    nrf_drv_spi_uninit(&pxSpi->instance);
                    if ( NRF_SUCCESS == nrf_drv_spi_init(&pxSpi->instance, 
                                                         &pxSpi->config, 
                                                         pxSPIPeripheral->xSpiCallback, 
                                                         ( void * )pxSPIPeripheral->pvUserContext))
                    {
                        lError = IOT_SPI_SUCCESS;
                    }

                    pxSPIPeripheral->xConfig = *( IotSPIMasterConfig_t * ) pvBuffer;
                }

                break;

            case eSPIGetMasterConfig:

                ( ( IotSPIMasterConfig_t * ) pvBuffer )->ulFreq       = pxSPIPeripheral->xConfig.ulFreq;
                ( ( IotSPIMasterConfig_t * ) pvBuffer )->eMode        = pxSPIPeripheral->xConfig.eMode;
                ( ( IotSPIMasterConfig_t * ) pvBuffer )->eSetBitOrder = pxSPIPeripheral->xConfig.eSetBitOrder;
                ( ( IotSPIMasterConfig_t * ) pvBuffer )->ucDummyValue = pxSPIPeripheral->xConfig.ucDummyValue;

                lError = IOT_SPI_SUCCESS;

                break;

            /* pxSpi->instance can be used to get NRF_SPIM_Type which contains ->TX->PTR that shows address of byte to send.
               Poiner arithmetic could be reveal n_sent/n_readm, but would need to save base address of last requested rxtx buffer.
               There's also ->TX->AMOUNT and if it's a running counter, that may be a candidate as well. See datasheet for SPIM details. 
               Unimplemented for now.*/
            case eSPIGetTxNoOfbytes:
            case eSPIGetRxNoOfbytes:
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
        const SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;

        lError = prvSpiTransfer(&pxSpi->instance, NULL, 0, pvBuffer, xBytes, true);
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
        const SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;

        lError = prvSpiTransfer(&pxSpi->instance, NULL, 0, pvBuffer, xBytes, false);
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
        const SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;

        lError = prvSpiTransfer(&pxSpi->instance, pvBuffer, xBytes, NULL, 0, true);
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
        const SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;

        lError = prvSpiTransfer(&pxSpi->instance, pvBuffer, xBytes, NULL, 0, false);
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
        const SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;

        lError = prvSpiTransfer(&pxSpi->instance, pvTxBuffer, xBytes, pvRxBuffer, xBytes, true);
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
        const SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;
        
        lError = prvSpiTransfer(&pxSpi->instance, pvTxBuffer, xBytes, pvRxBuffer, xBytes, false);
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
        const SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;
        int32_t lCancelStatus = iot_spi_cancel(pxSPIPeripheral);

        if (lCancelStatus == IOT_SPI_NOTHING_TO_CANCEL || lCancelStatus == IOT_SPI_SUCCESS) 
        {
            nrf_drv_spi_uninit(&pxSpi->instance);
            pxSPIPeripheral->sOpened = IOT_SPI_CLOSED;
        } 
        else
        {
            lError = lCancelStatus;
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
        const SpiContext_t * pxSpi = pxSPIPeripheral->pxSpiContext;

        if (!bTransferDone)
        {
            nrf_drv_spi_abort( &pxSpi->instance );

            CRITICAL_REGION_ENTER();
            bTransferDone = true;
            CRITICAL_REGION_EXIT();

            lError = IOT_SPI_SUCCESS;
        }
        else 
        {
            lError = IOT_SPI_NOTHING_TO_CANCEL;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

/*
 * Event handler passed to NRF SPI controller. Used for sync and async modes.
 * Note, pvContext is set to IotSPIHandle_t of corresponding SPI instance at open.
 */
void prvSpiEventHandler(nrf_drv_spi_evt_t const * pxEvent, void * pvContext)
{
    IotSPIHandle_t xHandle = ( IotSPIHandle_t ) pvContext;
    const SpiContext_t * pxSpi = xHandle->pxSpiContext;

    switch( pxEvent->type )
    {
        case NRF_DRV_SPI_EVENT_DONE:
            /* Adjust atomic flags. These are used by sync modes for polling and reproduces internal nRF_SDK blocking behaviour */
            CRITICAL_REGION_ENTER();
            bTransferDone = true;
            CRITICAL_REGION_EXIT();
            
            /* In case of async transfers, event handlers should have been set, and should now be invoked */
            if (xHandle->xSpiCallback != NULL)
            {
                xHandle->xSpiCallback( eSPISuccess, xHandle->pvUserContext );
            }
            break;

        default:
            break;
    }
}

/*
 * Transfer logic, with less parameter checking.
 * Maintains internal atomics and applies appropriate CommonIO error codes
 * Called by other CommonIO that check input parameters before calling.
*/
static int32_t prvSpiTransfer(nrf_drv_spi_t const * const pxInstance, 
                              uint8_t const * pucTxBuffer,
                              size_t  xTxSize,
                              uint8_t const * pucRxBuffer,
                              size_t xRxSize,
                              bool   bSync)
{
    int32_t lError = IOT_SPI_SUCCESS;

    if (bTransferDone)
    {
        /* There was no transfer in progress, so we can safely attempt a new transfer */
        CRITICAL_REGION_ENTER();
        bTransferDone = false;
        CRITICAL_REGION_EXIT();

        ret_code_t xTransferStatus = nrf_drv_spi_transfer( pxInstance, pucTxBuffer, xTxSize, pucRxBuffer, xRxSize );
        
        if (NRF_SUCCESS != xTransferStatus) 
        {
            CRITICAL_REGION_ENTER();
            bTransferDone = true;
            CRITICAL_REGION_EXIT();

            lError = IOT_SPI_TRANSFER_ERROR;
        }
        else
        {
            if (bSync)
            {
                /* For sync transfers, we block until they complete */   
                while(bTransferDone);
            }
        }
    }
    else
    {
        lError = IOT_SPI_BUS_BUSY;
    }

    return lError;
}
