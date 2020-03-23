/*
 * Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
 * All rights reserved.
 *
 * You may not use this file except in compliance with the terms and
 * conditions set forth in the accompanying LICENSE.TXT file.
 *
 * THESE MATERIALS ARE PROVIDED ON AN "AS IS" BASIS. ESPRESSIF/AMAZON SPECIFICALLY
 * DISCLAIMS, WITH RESPECT TO THESE MATERIALS, ALL WARRANTIES, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
 */

#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "driver/spi_master.h"
#include "freertos/timers.h"
#include <esp_err.h>
#include <iot_spi.h>
#include <board_gpio.h>

#define SEM_WAIT_TIME (60000 / portTICK_RATE_MS)
#define NULL_BUF_SZ (1024 * 4)

static const char* TAG="esp-iot-spi";

typedef struct {
    uint8_t spi_port_num;
    spi_device_handle_t handle;
    spi_bus_config_t spi_bus_config;
    IotSPIMasterConfig_t iot_spi_master_conf;
    spi_device_interface_config_t spi_device_config;
    uint8_t *read_buf;
    uint8_t *write_buf;
    uint32_t bytes_to_read;
    uint32_t bytes_to_write;
    void (*func) ( IotSPITransactionStatus_t arg1, void * arg2);
    void * arg;
    char * null_tx;
    spi_transaction_t spi_transact;
    SemaphoreHandle_t spi_rd_cb_wait;
    SemaphoreHandle_t spi_wr_cb_wait;
    SemaphoreHandle_t spi_transfer_cb_wait;
    unsigned spi_read_op_in_progress: 1;
    unsigned spi_write_op_in_progress: 1;
    unsigned spi_op_cancel_req: 1;
} spi_ctx_t;

static struct esp_spi_pin_config esp_spi_pin_map[ESP_MAX_SPI_PORTS] = ESP_SPI_PIN_MAP;
static volatile uint8_t spi_bit_mask;

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
        spi_ctx->spi_device_config.queue_size = 3;
        spi_ctx->spi_device_config.post_cb = NULL;
        spi_ctx->spi_device_config.pre_cb = NULL;
        spi_ctx->spi_device_config.command_bits = 0;
        spi_ctx->spi_device_config.address_bits = 0;

        ret = spi_bus_initialize(spi_ctx->spi_port_num, &spi_ctx->spi_bus_config, lSPIInstance);
        ret |= spi_bus_add_device(spi_ctx->spi_port_num, &spi_ctx->spi_device_config, &spi_ctx->handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "spi bus initialization failed");
            return NULL;
        }

        spi_ctx->null_tx = heap_caps_calloc(NULL_BUF_SZ, 1, MALLOC_CAP_DMA);
        if (spi_ctx->null_tx == NULL) {
            ESP_LOGE(TAG, "Failed to alloc null_tx !");
            return NULL;
        }

        spi_ctx->spi_rd_cb_wait = xSemaphoreCreateBinary();
        if (spi_ctx->spi_rd_cb_wait == NULL) {
            ESP_LOGE(TAG, "Failed to create read binary semaphore");
            free(spi_ctx);
            return NULL;
        }

        spi_ctx->spi_wr_cb_wait = xSemaphoreCreateBinary();
        if (spi_ctx->spi_wr_cb_wait == NULL) {
            ESP_LOGD(TAG, "Failed to create write binary semaphore");
            vSemaphoreDelete(spi_ctx->spi_rd_cb_wait);
            free(spi_ctx);
            return NULL;
        }

        spi_ctx->spi_transfer_cb_wait = xSemaphoreCreateBinary();
        if (spi_ctx->spi_transfer_cb_wait == NULL) {
            ESP_LOGD(TAG, "Failed to create transfer binary semaphore");
            vSemaphoreDelete(spi_ctx->spi_rd_cb_wait);
            vSemaphoreDelete(spi_ctx->spi_wr_cb_wait);
            free(spi_ctx);
            return NULL;
        }

        /* Ensure first semaphore take could succeed */
        xSemaphoreGive(spi_ctx->spi_rd_cb_wait);
        xSemaphoreGive(spi_ctx->spi_wr_cb_wait);
        xSemaphoreGive(spi_ctx->spi_transfer_cb_wait);

        IotSPIHandle_t iot_spi_handler = (void *) spi_ctx;
        spi_bit_mask = spi_bit_mask | BIT(lSPIInstance);
        return iot_spi_handler;
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
        if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
            ESP_LOGE(TAG, "SPI bus busy");
            return IOT_SPI_BUS_BUSY;
        }

        ret = spi_bus_remove_device(spi_ctx->handle);
        ret |= spi_bus_free(spi_ctx->spi_port_num);

        if (ret == ESP_ERR_INVALID_ARG) {
            ESP_LOGE(TAG, "Invalid SPI handler %s", __func__);
            return IOT_SPI_INVALID_VALUE;
        } else if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "device is already freed %s", __func__);
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
        //memset(&spi_ctx->handle, 0, sizeof(spi_device_handle_t));
        spi_ctx->spi_device_config.queue_size = 3;
        vTaskDelay(100);
        ret = spi_bus_initialize(spi_ctx->spi_port_num, &spi_ctx->spi_bus_config, 1);
        ret |= spi_bus_add_device(spi_ctx->spi_port_num, &spi_ctx->spi_device_config, &spi_ctx->handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPI adding of device failed");
            return IOT_SPI_INVALID_VALUE;
        }
        //memset(&spi_ctx->null_tx, iot_spi_cfg->ucDummyValue, NULL_BUF_SZ);
        memcpy(&spi_ctx->iot_spi_master_conf, iot_spi_cfg, sizeof(IotSPIMasterConfig_t));
        break;

    case eSPIGetMasterConfig: {
        IotSPIMasterConfig_t *iot_spi_cfg = (IotSPIMasterConfig_t *) pvBuffer;
        memcpy(iot_spi_cfg, &spi_ctx->iot_spi_master_conf, sizeof(IotSPIMasterConfig_t));
        break;
    }

    case eSPIGetTxNoOfbytes: {
        uint32_t *txbytes = (uint32_t*) pvBuffer;
        txbytes = &spi_ctx->bytes_to_write;
        break;
    }

    case eSPIGetRxNoOfbytes: {
        uint32_t *rxbytes = (uint32_t*) pvBuffer;
        rxbytes = &spi_ctx->bytes_to_read;
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
    if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.length = xBytes * 8; //length need to be set in bits
    spi_ctx->spi_transact.tx_buffer = pvBuffer;

    ret = spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);
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
    if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.tx_buffer = spi_ctx->null_tx;
    spi_ctx->spi_transact.rxlength = xBytes * 8; //length needs to be set in bits
    spi_ctx->spi_transact.rx_buffer = pvBuffer;

    ret = spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read SPI sync data");
        return IOT_SPI_INVALID_VALUE;
    }

    return IOT_SPI_SUCCESS;
}

static void spi_write_async_cb(void *param, unsigned int ulparam)
{
    uint8_t op_status = 0;
    uint8_t bytes_to_write_at_one_time = 20;
    int bytes_write_tmp = 0;
    spi_ctx_t *spi_ctx = (spi_ctx_t *) param;
    size_t bytes_write = spi_ctx->bytes_to_write;
    uint8_t *write_buffer = spi_ctx->write_buf;

    ESP_LOGD(TAG, "SPI read async callback invoked %s", __func__);
    if (bytes_write < bytes_to_write_at_one_time) {
        bytes_to_write_at_one_time = bytes_write;
    }

    while (bytes_write > 0) {
        spi_ctx->spi_transact.length = bytes_to_write_at_one_time * 8; //length needs to be set in bits
        spi_ctx->spi_transact.tx_buffer = write_buffer;
        spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);
        bytes_write_tmp = bytes_to_write_at_one_time;
        if (spi_ctx->spi_op_cancel_req) {
            ESP_LOGD(TAG, "cancel operation recieved");
            break;
        }
        bytes_write -= bytes_write_tmp;
        write_buffer += bytes_write_tmp;
        spi_ctx->bytes_to_write = bytes_write;
    }
    if (spi_ctx->spi_op_cancel_req) {
        op_status = eSPITransferError;
    } else {
        op_status = eSPISuccess;
    }

    if (spi_ctx->func) {
        ESP_LOGD(TAG, "%s invoking callback %p", __func__, spi_ctx->func);
        spi_ctx->func(op_status, spi_ctx->arg);
    }

    spi_ctx->bytes_to_write = 0;
    spi_ctx->spi_write_op_in_progress = false;
    spi_ctx->spi_op_cancel_req = false;
    xSemaphoreGive(spi_ctx->spi_wr_cb_wait);
}

static void spi_read_async_cb(void *param, unsigned int ulparam)
{
    uint8_t op_status = 0;
    uint8_t bytes_to_read_at_one_time = 20;
    int bytes_read_tmp = 0;
    spi_ctx_t *spi_ctx = (spi_ctx_t *) param;
    size_t bytes_read = spi_ctx->bytes_to_read;
    uint8_t *read_buffer = spi_ctx->read_buf;
    uint8_t *write_buffer = spi_ctx->write_buf;

    ESP_LOGD(TAG, "SPI read async callback invoked %s", __func__);
    if (bytes_read < bytes_to_read_at_one_time) {
        bytes_to_read_at_one_time = bytes_read;
    }

    while (bytes_read > 0) {
        spi_ctx->spi_transact.rxlength = bytes_to_read_at_one_time * 8; //length needs to be set in bits
        spi_ctx->spi_transact.length = bytes_to_read_at_one_time * 8; //length needs to be set in bits
        spi_ctx->spi_transact.tx_buffer = write_buffer;
        spi_ctx->spi_transact.rx_buffer = read_buffer;

        spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);
        bytes_read_tmp = bytes_to_read_at_one_time;
        if (spi_ctx->spi_op_cancel_req) {
            ESP_LOGD(TAG, "cancel operation recieved");
            break;
        }
        bytes_read -= bytes_read_tmp;
        read_buffer += bytes_read_tmp;
        spi_ctx->bytes_to_read = bytes_read;
    }
    if (spi_ctx->spi_op_cancel_req) {
        op_status = eSPITransferError;
    } else {
        op_status = eSPISuccess;
    }

    if (spi_ctx->func) {
        ESP_LOGD(TAG, "%s invoking callback %p", __func__, spi_ctx->func);
        spi_ctx->func(op_status, spi_ctx->arg);
    }

    spi_ctx->bytes_to_read = 0;
    spi_ctx->spi_read_op_in_progress = false;
    spi_ctx->spi_op_cancel_req = false;
    xSemaphoreGive(spi_ctx->spi_rd_cb_wait);
}

int32_t iot_spi_write_async(IotSPIHandle_t const pxSPIPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    if (pxSPIPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG,"Invalid arguments %s",__func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t * spi_ctx = (spi_ctx_t *) pxSPIPeripheral;
    if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.length = xBytes * 8;
    spi_ctx->spi_transact.tx_buffer = pvBuffer;
    spi_ctx->bytes_to_write = xBytes;
    spi_ctx->write_buf = pvBuffer;

    if (!xSemaphoreTake(spi_ctx->spi_wr_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire write sem", __func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx->spi_write_op_in_progress = true;
    //Create timer with 0 wait time
    if (xTimerPendFunctionCall(spi_write_async_cb, (void *)spi_ctx, 0, 1) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to queue pend function cb on timer for write async", __func__);
        spi_ctx->spi_write_op_in_progress = false;
        xSemaphoreGive(spi_ctx->spi_wr_cb_wait);
        return IOT_SPI_INVALID_VALUE;
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
    if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.tx_buffer = spi_ctx->null_tx;
    spi_ctx->spi_transact.length = xBytes * 8;
    spi_ctx->spi_transact.rxlength = xBytes * 8;
    spi_ctx->spi_transact.rx_buffer = pvBuffer;
    spi_ctx->bytes_to_read = xBytes;
    spi_ctx->read_buf = pvBuffer;
    spi_ctx->write_buf = spi_ctx->null_tx;

    if (!xSemaphoreTake(spi_ctx->spi_rd_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx->spi_read_op_in_progress = true;
    //Create timer with 0 wait time
    if (xTimerPendFunctionCall(spi_read_async_cb, (void *)spi_ctx, 0, 1) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to queue Pend function cb on timer for read async", __func__);
        spi_ctx->spi_read_op_in_progress = false;
        xSemaphoreGive(spi_ctx->spi_rd_cb_wait);
        return IOT_SPI_INVALID_VALUE;
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
    if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.rxlength = xBytes * 8; //length needs to be set in bits
    spi_ctx->spi_transact.length = xBytes * 8;   //length needs to be set in bits
    spi_ctx->spi_transact.tx_buffer = pvTxBuffer;
    spi_ctx->spi_transact.rx_buffer = pvRxBuffer;

    ret = spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to transfer SPI data");
        return IOT_SPI_INVALID_VALUE;
    }

    return IOT_SPI_SUCCESS;
}
//iot_tests test 16 spi
static void spi_read_write_transfer_cb(void *param, unsigned int ulparam)
{
    uint8_t op_status = 0;
    uint8_t bytes_to_read_write_at_one_time = 20;
    int bytes_read_write_tmp = 0;
    spi_ctx_t *spi_ctx = (spi_ctx_t *) param;
    size_t bytes_read_write = spi_ctx->bytes_to_write;
    uint8_t *write_buffer = spi_ctx->write_buf;
    uint8_t *read_buffer = spi_ctx->read_buf;

    ESP_LOGE(TAG, "SPI read async callback invoked %s", __func__);
    if (bytes_read_write < bytes_to_read_write_at_one_time) {
        bytes_to_read_write_at_one_time = bytes_read_write;
    }
    while (bytes_read_write > 0) {
        spi_ctx->spi_transact.length = bytes_to_read_write_at_one_time * 8; //length needs to be set in bits
        spi_ctx->spi_transact.rxlength = bytes_to_read_write_at_one_time * 8; //length needs to be set in bits
        spi_ctx->spi_transact.tx_buffer = write_buffer;
        spi_ctx->spi_transact.rx_buffer = read_buffer;
        spi_device_transmit(spi_ctx->handle, &spi_ctx->spi_transact);
        bytes_read_write_tmp = bytes_to_read_write_at_one_time;
        if (spi_ctx->spi_op_cancel_req) {
            ESP_LOGD(TAG, "cancel operation recieved");
            break;
        }
        bytes_read_write -= bytes_read_write_tmp;
        write_buffer += bytes_read_write_tmp;
        read_buffer += bytes_read_write_tmp;
        spi_ctx->bytes_to_write = bytes_read_write;
    }
    if (spi_ctx->spi_op_cancel_req) {
        op_status = eSPITransferError;
    } else {
        op_status = eSPISuccess;
    }

    if (spi_ctx->func) {
        ESP_LOGD(TAG, "%s invoking callback %p", __func__, spi_ctx->func);
        spi_ctx->func(op_status, spi_ctx->arg);
    }

    spi_ctx->bytes_to_write = 0;
    spi_ctx->spi_write_op_in_progress = false;
    spi_ctx->bytes_to_read = 0;
    spi_ctx->spi_read_op_in_progress = false;
    spi_ctx->spi_op_cancel_req = false;
    xSemaphoreGive(spi_ctx->spi_transfer_cb_wait);
}

int32_t iot_spi_transfer_async(IotSPIHandle_t const pxSPIPeripheral, uint8_t * const pvTxBuffer, uint8_t * const pvRxBuffer, size_t xBytes)
{
    if (pxSPIPeripheral == NULL || pvTxBuffer == NULL || pvRxBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG,"Invalid arguments %s",__func__);
        return IOT_SPI_INVALID_VALUE;
    }

    spi_ctx_t * spi_ctx = (spi_ctx_t *) pxSPIPeripheral;
    if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
        ESP_LOGE(TAG,"SPI bus is busy %s", __func__);
        return IOT_SPI_BUS_BUSY;
    }

    memset(&spi_ctx->spi_transact, 0, sizeof(spi_transaction_t));
    spi_ctx->spi_transact.rxlength = xBytes * 8; //length needs to be set in bits
    spi_ctx->spi_transact.length = xBytes * 8;   //length needs to be set in bits
    spi_ctx->spi_transact.tx_buffer = pvTxBuffer;
    spi_ctx->spi_transact.rx_buffer = pvRxBuffer;
    spi_ctx->spi_read_op_in_progress = true;
    spi_ctx->spi_write_op_in_progress = true;
    spi_ctx->bytes_to_read = xBytes;
    spi_ctx->bytes_to_write = xBytes;
    spi_ctx->write_buf = pvTxBuffer;
    spi_ctx->read_buf = pvRxBuffer;

    if (!xSemaphoreTake(spi_ctx->spi_transfer_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
        return IOT_SPI_INVALID_VALUE;
    }

    if (xTimerPendFunctionCall(spi_read_write_transfer_cb, (void *)spi_ctx, 0, 0) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to create timer for read async", __func__);
        spi_ctx->spi_read_op_in_progress = true;
        spi_ctx->spi_write_op_in_progress = true;
        xSemaphoreGive(spi_ctx->spi_transfer_cb_wait);
        return IOT_SPI_INVALID_VALUE;
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
        if (spi_ctx->spi_read_op_in_progress) {
            spi_ctx->spi_op_cancel_req = true;
            /* Ensure no active operations on SPI before flusing all data */
            if (!xSemaphoreTake(spi_ctx->spi_rd_cb_wait, SEM_WAIT_TIME)) {
                ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
                return IOT_SPI_INVALID_VALUE;
            }
            xSemaphoreGive(spi_ctx->spi_rd_cb_wait);
        }
        if (spi_ctx->spi_write_op_in_progress) {
            if (!xSemaphoreTake(spi_ctx->spi_wr_cb_wait, SEM_WAIT_TIME)) {
                ESP_LOGE(TAG, "%s: failed to acquire write sem", __func__);
                return IOT_SPI_INVALID_VALUE;
            }
            xSemaphoreGive(spi_ctx->spi_wr_cb_wait);
        }
        ret = spi_bus_remove_device(spi_ctx->handle);
        ret |= spi_bus_free(spi_ctx->spi_port_num);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to close SPI");
            return IOT_SPI_INVALID_VALUE;
        }
        if (spi_ctx->spi_rd_cb_wait) {
            vSemaphoreDelete(spi_ctx->spi_rd_cb_wait);
        }
        if (spi_ctx->spi_wr_cb_wait) {
            vSemaphoreDelete(spi_ctx->spi_wr_cb_wait);
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
    if (pxSPIPeripheral == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_SPI_INVALID_VALUE;
    }
    spi_ctx_t *spi_ctx = (spi_ctx_t *) pxSPIPeripheral;
#if 0
    if (!spi_ctx->spi_read_op_in_progress && !spi_ctx->spi_write_op_in_progress) {
        return IOT_SPI_NOTHING_TO_CANCEL;
    } else if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
        spi_ctx->spi_op_cancel_req = true;
        ESP_LOGD(TAG, "operation cancel request: %d\n", spi_ctx->spi_op_cancel_req);
        /* Ensure no active operations on SPI before flusing all data */
        spi_ctx->spi_op_cancel_req = false;

        return IOT_SPI_SUCCESS;
    }
#endif
    if (spi_ctx->spi_read_op_in_progress || spi_ctx->spi_write_op_in_progress) {
        ESP_LOGE(TAG, "spi read/write operation cancel not supported \n");
        return IOT_SPI_FUNCTION_NOT_SUPPORTED;
    }
    return IOT_SPI_FUNCTION_NOT_SUPPORTED;
}
