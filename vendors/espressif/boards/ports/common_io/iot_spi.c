// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "driver/spi_master.h"
#include "esp_timer.h"
#include <esp_err.h>
#include <iot_spi.h>
#include <iot_board_gpio.h>
#include <freertos/semphr.h>

#define WAIT_TICKS (10)
#define NULL_BUF_SZ (1024 * 4)

static const char* TAG="esp-iot-spi";

typedef enum {
    SPI_NONE,
    SPI_READ,
    SPI_WRITE,
    SPI_TRANSFER,
} spi_mode_t;

typedef struct {
    uint8_t spi_port_num;
    spi_device_handle_t handle;
    spi_bus_config_t spi_bus_config;
    IotSPIMasterConfig_t iot_spi_master_conf;
    spi_device_interface_config_t spi_device_config;
    uint32_t bytes_read;
    uint32_t bytes_written;
    esp_timer_handle_t tmr_handle;
    void (*func) ( IotSPITransactionStatus_t arg1, void * arg2);
    void * arg;
    char * null_tx;
    spi_transaction_t spi_transact;
    SemaphoreHandle_t spi_semph;
    spi_mode_t spi_op_mode;
    IotSPITransactionStatus_t status;
} spi_ctx_t;

static struct esp_spi_pin_config esp_spi_pin_map[ESP_MAX_SPI_PORTS] = ESP_SPI_PIN_MAP;
static volatile uint8_t spi_bit_mask;

void spi_callback_dispatch(void *args)
{
    spi_ctx_t *spi_ctx = (spi_ctx_t *)args;
    spi_device_get_trans_result(spi_ctx->handle, &spi_ctx->spi_transact, WAIT_TICKS);

    if (spi_ctx->func) {
        spi_ctx->func(spi_ctx->status, spi_ctx->arg);
    }

    xSemaphoreGive(spi_ctx->spi_semph);
}

IRAM_ATTR void spi_post_trans_cb(spi_transaction_t *args)
{
    spi_transaction_t  *spi_trans = (spi_transaction_t *)args;
    spi_ctx_t *spi_ctx = (spi_ctx_t *)spi_trans->user;

    switch (spi_ctx->spi_op_mode) {
        case SPI_READ:
            spi_ctx->bytes_read = (spi_trans->rxlength / 8);
            spi_ctx->bytes_written = 0;
            break;
        case SPI_WRITE:
            spi_ctx->bytes_read = 0;
            spi_ctx->bytes_written = (spi_trans->length / 8);
            break;
        case SPI_TRANSFER:
            spi_ctx->bytes_read = (spi_trans->rxlength / 8);
            spi_ctx->bytes_written = (spi_trans->length / 8);
            break;
        case SPI_NONE:
        default:
            return;
    }
    spi_ctx->status = eSPISuccess;
    esp_timer_start_once(spi_ctx->tmr_handle, 0);
}

IotSPIHandle_t iot_spi_open( int32_t lSPIInstance )
{
    esp_err_t ret;
    if (lSPIInstance < 1 || lSPIInstance > 2 ) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return NULL;
    }
    if (!(0x01 & spi_bit_mask >> lSPIInstance)) {
        spi_ctx_t *spi_ctx = (spi_ctx_t *) calloc(1, sizeof(spi_ctx_t));
        if (spi_ctx == NULL) {
            ESP_LOGE(TAG, "Could not allocate memory for spi context");
            return NULL;
        }

        spi_ctx->spi_port_num = lSPIInstance;
        spi_ctx->spi_bus_config.flags = 0;
        spi_ctx->spi_bus_config.mosi_io_num = esp_spi_pin_map[lSPIInstance - 1].mosi_pin;
        spi_ctx->spi_bus_config.miso_io_num = esp_spi_pin_map[lSPIInstance - 1].miso_pin;
        spi_ctx->spi_bus_config.sclk_io_num = esp_spi_pin_map[lSPIInstance - 1].clk_pin;
        spi_ctx->spi_bus_config.quadhd_io_num = -1;
        spi_ctx->spi_bus_config.quadwp_io_num = -1;
        spi_ctx->spi_bus_config.max_transfer_sz = NULL_BUF_SZ; //since DMA is to be used
        spi_ctx->spi_device_config.mode = 0;
        spi_ctx->spi_device_config.clock_speed_hz = 1000000;
        spi_ctx->spi_device_config.duty_cycle_pos = 128; //50% duty cycle
        spi_ctx->spi_device_config.spics_io_num = esp_spi_pin_map[lSPIInstance - 1].cs_pin;
        spi_ctx->spi_device_config.cs_ena_pretrans = 1;
        spi_ctx->spi_device_config.cs_ena_posttrans = 1;
        spi_ctx->spi_device_config.flags = 0; //default MSB first for rx tx operation
        spi_ctx->spi_device_config.queue_size = 10;
        spi_ctx->spi_device_config.post_cb = spi_post_trans_cb;
        spi_ctx->spi_device_config.pre_cb = NULL;
        spi_ctx->spi_device_config.command_bits = 0;
        spi_ctx->spi_device_config.address_bits = 0;

        spi_ctx->null_tx = heap_caps_calloc(NULL_BUF_SZ, 1, MALLOC_CAP_DMA);
        if (spi_ctx->null_tx == NULL) {
            ESP_LOGE(TAG, "Failed to alloc null_tx !");
            goto err;
        }

        spi_ctx->spi_semph = xSemaphoreCreateBinary();
        if (spi_ctx->spi_semph == NULL) {
            ESP_LOGE(TAG, "Failed to create read binary semaphore");
            goto err;
        }

        esp_timer_create_args_t tmr_create_args = {
            .callback = spi_callback_dispatch,
            .arg = spi_ctx,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "SPI application CB",
        };

        if (esp_timer_create(&tmr_create_args, &spi_ctx->tmr_handle) != ESP_OK) {
            ESP_LOGE(TAG, "Timer creation failed");
            goto err;
        }

        ret = spi_bus_initialize(spi_ctx->spi_port_num, &spi_ctx->spi_bus_config, lSPIInstance);
        ret |= spi_bus_add_device(spi_ctx->spi_port_num, &spi_ctx->spi_device_config, &spi_ctx->handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "spi bus initialization failed");
            goto err;
        }

        /* Ensure first semaphore take could succeed */
        xSemaphoreGive(spi_ctx->spi_semph);

        IotSPIHandle_t iot_spi_handler = (void *) spi_ctx;
        spi_bit_mask = spi_bit_mask | BIT(lSPIInstance);
        return iot_spi_handler;
err:
        if (spi_ctx->null_tx) {
            free(spi_ctx->null_tx);
        }

        if (spi_ctx->spi_semph) {
            vSemaphoreDelete(spi_ctx->spi_semph);
        }

        if (spi_ctx->tmr_handle) {
            esp_timer_delete(spi_ctx->tmr_handle);
        }

        if (spi_ctx->handle) {
            spi_bus_remove_device(spi_ctx->handle);
            spi_bus_free(spi_ctx->spi_port_num);
        }

        free(spi_ctx);
        return NULL;
    } else {
        ESP_LOGE(TAG, "driver already open for given instance");
        return NULL;
    }
}

void iot_spi_set_callback(IotSPIHandle_t const pxSPIPeripheral, IotSPICallback_t xCallback, void * pvUserContext)
{
    if (pxSPIPeripheral == NULL || xCallback == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
    }
    spi_ctx_t *spi_ctx = (spi_ctx_t *) pxSPIPeripheral;

    spi_ctx->arg = pvUserContext;
    spi_ctx->func = (void *)xCallback;
}

int32_t iot_spi_ioctl( IotSPIHandle_t const pxSPIPeripheral,
                       IotSPIIoctlRequest_t xSPIRequest,
                       void * const pvBuffer )
{
    if ( pxSPIPeripheral == NULL || pvBuffer == NULL ) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_SPI_INVALID_VALUE;
    }
    esp_err_t ret = ESP_OK;
    spi_ctx_t *spi_ctx = (spi_ctx_t *) pxSPIPeripheral;
    switch (xSPIRequest)
    {
    case eSPISetMasterConfig:
       if (!xSemaphoreTake(spi_ctx->spi_semph, WAIT_TICKS)) {
            ESP_LOGE(TAG, "SPI bus busy");
            return IOT_SPI_BUS_BUSY;
        }

        ret |= spi_bus_remove_device(spi_ctx->handle);
        ret |= spi_bus_free(spi_ctx->spi_port_num);

        if (ret == ESP_ERR_INVALID_ARG) {
            ESP_LOGE(TAG, "Invalid SPI handler %s", __func__);
            xSemaphoreGive(spi_ctx->spi_semph);
            return IOT_SPI_INVALID_VALUE;
        } else if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "device is already freed %s", __func__);
            xSemaphoreGive(spi_ctx->spi_semph);
            return IOT_SPI_INVALID_VALUE;
        }
        IotSPIMasterConfig_t *iot_spi_cfg = (IotSPIMasterConfig_t *) pvBuffer;
        spi_ctx->spi_device_config.mode = iot_spi_cfg->eMode;
        if (iot_spi_cfg->ulFreq == 0) {
            spi_ctx->spi_device_config.clock_speed_hz = 1000000; //default clk speed
        } else {
            spi_ctx->spi_device_config.clock_speed_hz = iot_spi_cfg->ulFreq;
        }
        spi_ctx->spi_device_config.duty_cycle_pos = 128; //50% duty cycle
        spi_ctx->spi_device_config.spics_io_num = esp_spi_pin_map[spi_ctx->spi_port_num - 1].cs_pin;
        spi_ctx->spi_device_config.cs_ena_pretrans = 1;
        spi_ctx->spi_device_config.cs_ena_posttrans = 1;
        if (iot_spi_cfg->eSetBitOrder == eSPILSBFirst) {
            spi_ctx->spi_device_config.flags = SPI_DEVICE_BIT_LSBFIRST; //LSB first for tx and rx operation
        } else {
            spi_ctx->spi_device_config.flags = 0; //default MSB first for rx tx operation
        }
        spi_ctx->spi_device_config.queue_size = 10;
        vTaskDelay(100);
        ret = spi_bus_initialize(spi_ctx->spi_port_num, &spi_ctx->spi_bus_config, 1);
        ret |= spi_bus_add_device(spi_ctx->spi_port_num, &spi_ctx->spi_device_config, &spi_ctx->handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPI adding of device failed");
            xSemaphoreGive(spi_ctx->spi_semph);
            return IOT_SPI_INVALID_VALUE;
        }
        memcpy(&spi_ctx->iot_spi_master_conf, iot_spi_cfg, sizeof(IotSPIMasterConfig_t));
        xSemaphoreGive(spi_ctx->spi_semph);
        break;

    case eSPIGetMasterConfig: {
        IotSPIMasterConfig_t *iot_spi_cfg = (IotSPIMasterConfig_t *) pvBuffer;
        memcpy(iot_spi_cfg, &spi_ctx->iot_spi_master_conf, sizeof(IotSPIMasterConfig_t));
        break;
    }

    case eSPIGetTxNoOfbytes: {
        uint32_t *txbytes = (uint32_t*) pvBuffer;
        txbytes = &spi_ctx->bytes_written;
        break;
    }

    case eSPIGetRxNoOfbytes: {
        uint32_t *rxbytes = (uint32_t*) pvBuffer;
        rxbytes = &spi_ctx->bytes_read;
        break;
    }

    default:
        return IOT_SPI_INVALID_VALUE;
    }
    return IOT_SPI_SUCCESS;
}

int32_t iot_spi_write_sync(IotSPIHandle_t const pxSPIPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    esp_err_t ret;
    if (pxSPIPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG,"Invalid arguments %s",__func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t * spi_ctx = (spi_ctx_t *) pxSPIPeripheral;

    if (!xSemaphoreTake(spi_ctx->spi_semph, WAIT_TICKS)) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.length = xBytes * 8; //length need to be set in bits
    spi_ctx->spi_transact.tx_buffer = pvBuffer;

    ret = spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);

    spi_ctx->bytes_written = (spi_ctx->spi_transact.length / 8);
    spi_ctx->bytes_read = 0;

    xSemaphoreGive(spi_ctx->spi_semph);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write SPI sync data");
        return IOT_SPI_INVALID_VALUE;
    }

    return IOT_SPI_SUCCESS;
}

int32_t iot_spi_read_sync(IotSPIHandle_t const pxSPIPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    esp_err_t ret;
    if (pxSPIPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG,"Invalid arguments %s",__func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t * spi_ctx = (spi_ctx_t *) pxSPIPeripheral;

    if (!xSemaphoreTake(spi_ctx->spi_semph, WAIT_TICKS)) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.tx_buffer = spi_ctx->null_tx;
    spi_ctx->spi_transact.rxlength = xBytes * 8; //length needs to be set in bits
    spi_ctx->spi_transact.rx_buffer = pvBuffer;

    ret = spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);

    spi_ctx->bytes_read = (spi_ctx->spi_transact.rxlength / 8);
    spi_ctx->bytes_written = 0;

    xSemaphoreGive(spi_ctx->spi_semph);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read SPI sync data");
        return IOT_SPI_INVALID_VALUE;
    }

    return IOT_SPI_SUCCESS;
}

int32_t iot_spi_write_async(IotSPIHandle_t const pxSPIPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    if (pxSPIPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG,"Invalid arguments %s",__func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t * spi_ctx = (spi_ctx_t *) pxSPIPeripheral;

    if (!xSemaphoreTake(spi_ctx->spi_semph, WAIT_TICKS)) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.length = xBytes * 8;
    spi_ctx->spi_transact.tx_buffer = pvBuffer;
    spi_ctx->spi_transact.user = spi_ctx;

    spi_ctx->spi_op_mode = SPI_WRITE;

    if (spi_device_queue_trans(spi_ctx->handle, &spi_ctx->spi_transact, WAIT_TICKS) != ESP_OK) {
        spi_ctx->spi_op_mode = SPI_NONE;
        spi_ctx->bytes_written = 0;
        spi_ctx->bytes_read = 0;
        xSemaphoreGive(spi_ctx->spi_semph);
        return IOT_SPI_TRANSFER_ERROR;
    }

    return IOT_SPI_SUCCESS;
}

int32_t iot_spi_read_async(IotSPIHandle_t const pxSPIPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    if (pxSPIPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG,"Invalid arguments %s",__func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t * spi_ctx = (spi_ctx_t *) pxSPIPeripheral;

    if (!xSemaphoreTake(spi_ctx->spi_semph, WAIT_TICKS)) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.tx_buffer = spi_ctx->null_tx;
    spi_ctx->spi_transact.length = xBytes * 8;
    spi_ctx->spi_transact.rxlength = xBytes * 8;
    spi_ctx->spi_transact.rx_buffer = pvBuffer;
    spi_ctx->spi_transact.user = spi_ctx;

    spi_ctx->spi_op_mode = SPI_READ;

    if (spi_device_queue_trans(spi_ctx->handle, &spi_ctx->spi_transact, WAIT_TICKS) != ESP_OK) {
        spi_ctx->spi_op_mode = SPI_NONE;
        spi_ctx->bytes_written = 0;
        spi_ctx->bytes_read = 0;
        xSemaphoreGive(spi_ctx->spi_semph);
        return IOT_SPI_TRANSFER_ERROR;
    }

    return IOT_SPI_SUCCESS;
}

int32_t iot_spi_transfer_sync( IotSPIHandle_t const pxSPIPeripheral, uint8_t * const pvTxBuffer, uint8_t * const pvRxBuffer, size_t xBytes )
{
    esp_err_t ret;
    if (pxSPIPeripheral == NULL || pvTxBuffer == NULL || pvRxBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG,"Invalid arguments %s",__func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t * spi_ctx = (spi_ctx_t *) pxSPIPeripheral;

    if (!xSemaphoreTake(spi_ctx->spi_semph, WAIT_TICKS)) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.rxlength = xBytes * 8; //length needs to be set in bits
    spi_ctx->spi_transact.length = xBytes * 8;   //length needs to be set in bits
    spi_ctx->spi_transact.tx_buffer = pvTxBuffer;
    spi_ctx->spi_transact.rx_buffer = pvRxBuffer;

    ret = spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);

    spi_ctx->bytes_read = (spi_ctx->spi_transact.rxlength / 8);
    spi_ctx->bytes_written = (spi_ctx->spi_transact.length / 8);

    xSemaphoreGive(spi_ctx->spi_semph);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to transfer SPI data");
        return IOT_SPI_INVALID_VALUE;
    }

    return IOT_SPI_SUCCESS;
}

int32_t iot_spi_transfer_async(IotSPIHandle_t const pxSPIPeripheral, uint8_t * const pvTxBuffer, uint8_t * const pvRxBuffer, size_t xBytes)
{
    if (pxSPIPeripheral == NULL || pvTxBuffer == NULL || pvRxBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG,"Invalid arguments %s",__func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t * spi_ctx = (spi_ctx_t *) pxSPIPeripheral;

    if (!xSemaphoreTake(spi_ctx->spi_semph, WAIT_TICKS)) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.rxlength = xBytes * 8; //length needs to be set in bits
    spi_ctx->spi_transact.length = xBytes * 8;   //length needs to be set in bits
    spi_ctx->spi_transact.tx_buffer = pvTxBuffer;
    spi_ctx->spi_transact.rx_buffer = pvRxBuffer;
    spi_ctx->spi_transact.user = spi_ctx;

    spi_ctx->spi_op_mode = SPI_TRANSFER;

    if (spi_device_queue_trans(spi_ctx->handle, &spi_ctx->spi_transact, WAIT_TICKS) != ESP_OK) {
        spi_ctx->spi_op_mode = SPI_NONE;
        spi_ctx->bytes_written = 0;
        spi_ctx->bytes_read = 0;
        xSemaphoreGive(spi_ctx->spi_semph);
        return IOT_SPI_TRANSFER_ERROR;
    }

    return IOT_SPI_SUCCESS;
}

int32_t iot_spi_close( IotSPIHandle_t const pxSPIPeripheral )
{
    esp_err_t ret;
    if (pxSPIPeripheral == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t *spi_ctx = (spi_ctx_t *) pxSPIPeripheral;
    if (0x01 & spi_bit_mask >> spi_ctx->spi_port_num) {
        // Block till the transactions are complete
        xSemaphoreTake(spi_ctx->spi_semph, portMAX_DELAY);
        ret |= spi_bus_remove_device(spi_ctx->handle);
        ret |= spi_bus_free(spi_ctx->spi_port_num);

        if (spi_ctx->spi_semph) {
            vSemaphoreDelete(spi_ctx->spi_semph);
        }

        spi_bit_mask = spi_bit_mask & ~(BIT(spi_ctx->spi_port_num));
        free(pxSPIPeripheral);
        return (ret == ESP_OK) ? IOT_SPI_SUCCESS : IOT_SPI_INVALID_VALUE;
    } else {
        return IOT_SPI_INVALID_VALUE;
    }
}

int32_t iot_spi_cancel( IotSPIHandle_t const pxSPIPeripheral )
{
    return IOT_SPI_FUNCTION_NOT_SUPPORTED;
}
