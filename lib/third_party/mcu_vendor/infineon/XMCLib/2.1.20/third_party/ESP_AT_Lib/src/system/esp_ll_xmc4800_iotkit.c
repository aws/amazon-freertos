/**
 * \file            esp_ll_template.c
 * \brief           Low-level communication with ESP device template
 */

/*
 * Copyright (c) 2018 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of ESP-AT.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "system/esp_ll.h"
#include "esp/esp.h"
#include "esp/esp_mem.h"
#include "esp/esp_input.h"

#include "FreeRTOS.h"
#include "task.h"

#include <xmc_gpio.h>
#include <xmc_dma.h>
#include <xmc_uart.h>
#include <xmc_scu.h>

#define WIFI_IO_UART_BAUDRATE 115200
#define WIFI_IO_UART_CH XMC_UART1_CH0
#define WIFI_IO_UART_TX_PIN P2_14
#define WIFI_IO_UART_TX_PIN_AF P2_14_AF_U1C0_DOUT0
#define WIFI_IO_UART_RX_PIN P2_15
#define WIFI_IO_UART_RX_INPUT USIC1_C0_DX0_P2_15

#define RING_BUFFER_SIZE 4096

static uint8_t initialized = 0;

// Ring buffer using DMA
// The ring buffer is divided into two equal sized buffers
static volatile uint8_t ring_buffer[RING_BUFFER_SIZE];
static volatile uint8_t ring_buffer_cur = 0;

/* DMA linked list */
static __attribute__((aligned(32))) XMC_DMA_LLI_t dma_ll[2] =
{
  {
	.block_size = RING_BUFFER_SIZE / 2,
	.src_addr = (uint32_t)&(WIFI_IO_UART_CH->RBUF),
	.dst_addr = (uint32_t)&ring_buffer[0],
	.llp = &dma_ll[1],
	.enable_interrupt = true,
	.dst_transfer_width = XMC_DMA_CH_TRANSFER_WIDTH_8,
	.src_transfer_width = XMC_DMA_CH_TRANSFER_WIDTH_8,
	.dst_address_count_mode = XMC_DMA_CH_ADDRESS_COUNT_MODE_INCREMENT,
	.src_address_count_mode = XMC_DMA_CH_ADDRESS_COUNT_MODE_NO_CHANGE,
	.dst_burst_length = XMC_DMA_CH_BURST_LENGTH_1,
	.src_burst_length = XMC_DMA_CH_BURST_LENGTH_1,
	.transfer_flow = XMC_DMA_CH_TRANSFER_FLOW_P2M_DMA,
	.enable_dst_linked_list = true,
	.enable_src_linked_list = true,
  },
  {
	.block_size = RING_BUFFER_SIZE / 2,
	.src_addr = (uint32_t)&(WIFI_IO_UART_CH->RBUF),
	.dst_addr = (uint32_t)&ring_buffer[RING_BUFFER_SIZE / 2],
	.llp = &dma_ll[0],
	.enable_interrupt = true,
	.dst_transfer_width = XMC_DMA_CH_TRANSFER_WIDTH_8,
	.src_transfer_width = XMC_DMA_CH_TRANSFER_WIDTH_8,
	.dst_address_count_mode = XMC_DMA_CH_ADDRESS_COUNT_MODE_INCREMENT,
	.src_address_count_mode = XMC_DMA_CH_ADDRESS_COUNT_MODE_NO_CHANGE,
	.dst_burst_length = XMC_DMA_CH_BURST_LENGTH_1,
	.src_burst_length = XMC_DMA_CH_BURST_LENGTH_1,
	.transfer_flow = XMC_DMA_CH_TRANSFER_FLOW_P2M_DMA,
	.enable_dst_linked_list = true,
	.enable_src_linked_list = true,
  }
};

static const XMC_DMA_CH_CONFIG_t dma_ch_config =
{
  {
	.enable_interrupt = true,
	.dst_transfer_width = XMC_DMA_CH_TRANSFER_WIDTH_8,
	.src_transfer_width = XMC_DMA_CH_TRANSFER_WIDTH_8,
	.dst_address_count_mode = XMC_DMA_CH_ADDRESS_COUNT_MODE_INCREMENT,
	.src_address_count_mode = XMC_DMA_CH_ADDRESS_COUNT_MODE_NO_CHANGE,
	.dst_burst_length = XMC_DMA_CH_BURST_LENGTH_1,
	.src_burst_length = XMC_DMA_CH_BURST_LENGTH_1,
	.transfer_flow = XMC_DMA_CH_TRANSFER_FLOW_P2M_DMA,
  },
  .block_size = RING_BUFFER_SIZE / 2,
  .src_addr = (uint32_t)&(WIFI_IO_UART_CH->RBUF),
  .dst_addr = (uint32_t)&ring_buffer[0],
  .linked_list_pointer = &dma_ll[0],
  .transfer_type = XMC_DMA_CH_TRANSFER_TYPE_MULTI_BLOCK_SRCADR_LINKED_DSTADR_LINKED, /* Transfer type */
  .priority = XMC_DMA_CH_PRIORITY_0, /* Priority level */
  .src_handshaking = XMC_DMA_CH_SRC_HANDSHAKING_HARDWARE, /* Source handshaking */
  .src_peripheral_request = DMA0_PERIPHERAL_REQUEST_USIC1_SR0_0, /* Source peripheral trigger */
  .dst_handshaking = XMC_DMA_CH_DST_HANDSHAKING_SOFTWARE  /* Destination handshaking */
};

static void uart_task(void * pvParameters)
{
  uint32_t start = 0;
  uint32_t end;

  while (1)
  {
	end = (ring_buffer_cur * (RING_BUFFER_SIZE / 2)) + XMC_DMA_CH_GetTransferredData(XMC_DMA0, 0);

	if (start != end)
	{
	  if (start < end)
	  {
	    esp_input_process((const void*)&ring_buffer[start], end - start); /* Process input data in linear buffer phase */
	  }
	  else
	  {
        /*
         * In overflow mode we have to process twice:
         *  - Process data until end of buffer
         *  - Process data until current position on top of buffer
         */

		esp_input_process((const void*)&ring_buffer[start], RING_BUFFER_SIZE - start); /* Process input data in linear buffer phase */
		esp_input_process((const void*)&ring_buffer[0], end); /* Process input data in linear buffer phase */
	  }

	  start = end;
	}

	vTaskDelay(pdMS_TO_TICKS(1UL));
  }
}

static void dma_ch0_event_handler(XMC_DMA_CH_EVENT_t event)
{
  ring_buffer_cur ^= 1;
}

void GPDMA0_0_IRQHandler(void)
{
  XMC_DMA_IRQHandler(XMC_DMA0);
}


static void delay(uint32_t cycles)
{
  while (--cycles)
  {
    __NOP();
  }
}

void configure_uart(uint32_t baudrate) {
  XMC_UART_CH_CONFIG_t uart_config =
  {
	.baudrate = baudrate,
	.data_bits = 8,
	.stop_bits = 1,
  };

  if (!initialized)
  {
	// Enable pin
	XMC_SCU_HIB_EnableHibernateDomain();
	XMC_SCU_HIB_SetPinOutputLevel(XMC_SCU_HIB_IO_0, XMC_SCU_HIB_IO_OUTPUT_LEVEL_LOW);
    delay(SystemCoreClock / 1000);
	XMC_SCU_HIB_SetPinOutputLevel(XMC_SCU_HIB_IO_0, XMC_SCU_HIB_IO_OUTPUT_LEVEL_HIGH);

	/* Enable DMA module */
	XMC_DMA_Init(XMC_DMA0);
	XMC_DMA_CH_Init(XMC_DMA0, 0, &dma_ch_config);
	XMC_DMA_CH_EnableEvent(XMC_DMA0, 0, XMC_DMA_CH_EVENT_BLOCK_TRANSFER_COMPLETE);
	XMC_DMA_CH_SetEventHandler(XMC_DMA0, 0, dma_ch0_event_handler);
	XMC_DMA_CH_Enable(XMC_DMA0, 0);

	/* Enable DMA event handling */
	NVIC_SetPriority(GPDMA0_0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 62, 0));
	NVIC_EnableIRQ(GPDMA0_0_IRQn);

	BaseType_t xReturned;
	xReturned = xTaskCreate( uart_task,
                             "uart_task",
				             configMINIMAL_STACK_SIZE,
                             NULL,
							 tskIDLE_PRIORITY,
				             NULL);

	if ( xReturned != pdPASS )
	{
	  // error while creating UART task
	  while (1);
	}

    XMC_UART_CH_Init(WIFI_IO_UART_CH, &uart_config);

	XMC_GPIO_SetMode(WIFI_IO_UART_RX_PIN, XMC_GPIO_MODE_INPUT_PULL_UP);
	XMC_UART_CH_SetInputSource(WIFI_IO_UART_CH, XMC_UART_CH_INPUT_RXD, WIFI_IO_UART_RX_INPUT);

	/*Configure transmit FIFO*/
	XMC_USIC_CH_TXFIFO_Configure(WIFI_IO_UART_CH, 0U, XMC_USIC_CH_FIFO_SIZE_32WORDS, 0U);

    /* Set service request for receive interrupt */
	XMC_USIC_CH_SetInterruptNodePointer(WIFI_IO_UART_CH, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, 0U);
	XMC_USIC_CH_SetInterruptNodePointer(WIFI_IO_UART_CH, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, 0U);
	XMC_UART_CH_EnableEvent(WIFI_IO_UART_CH, XMC_UART_CH_EVENT_STANDARD_RECEIVE | XMC_UART_CH_EVENT_ALTERNATIVE_RECEIVE);

	XMC_UART_CH_Start(WIFI_IO_UART_CH);

	XMC_GPIO_SetMode(WIFI_IO_UART_TX_PIN, XMC_GPIO_MODE_OUTPUT_PUSH_PULL | WIFI_IO_UART_TX_PIN_AF);
  }
  else
  {
	XMC_UART_CH_DisableEvent(WIFI_IO_UART_CH, XMC_UART_CH_EVENT_STANDARD_RECEIVE | XMC_UART_CH_EVENT_ALTERNATIVE_RECEIVE);
	XMC_GPIO_SetMode(WIFI_IO_UART_TX_PIN, XMC_GPIO_MODE_INPUT_PULL_UP);
    XMC_UART_CH_Stop(WIFI_IO_UART_CH);

    XMC_UART_CH_Init(WIFI_IO_UART_CH, &uart_config);

	XMC_UART_CH_EnableEvent(WIFI_IO_UART_CH, XMC_UART_CH_EVENT_STANDARD_RECEIVE | XMC_UART_CH_EVENT_ALTERNATIVE_RECEIVE);

	XMC_UART_CH_Start(WIFI_IO_UART_CH);

	XMC_GPIO_SetMode(WIFI_IO_UART_TX_PIN, XMC_GPIO_MODE_OUTPUT_PUSH_PULL | WIFI_IO_UART_TX_PIN_AF);
  }
}

/**
 * \brief           Send data to ESP device, function called from ESP stack when we have data to send
 * \param[in]       data: Pointer to data to send
 * \param[in]       len: Number of bytes to send
 * \return          Number of bytes sent
 */
static uint16_t send_data(const void* data, uint16_t len) {
  for (int i = 0; i < len; ++i)
  {
    while (XMC_USIC_CH_TXFIFO_IsFull(WIFI_IO_UART_CH) == true);
    XMC_UART_CH_Transmit(WIFI_IO_UART_CH, *(uint8_t *)data);
    data++;
  }

  return len;
}

/**
 * \brief           Callback function called from initialization process
 *
 * \note            This function may be called multiple times if AT baudrate is changed from application.
 *                  It is important that every configuration except AT baudrate is configured only once!
 *
 * \note            This function may be called from different threads in ESP stack when using OS.
 *                  When \ref ESP_CFG_INPUT_USE_PROCESS is set to 1, this function may be called from user UART thread.
 *
 * \param[in,out]   ll: Pointer to \ref esp_ll_t structure to fill data for communication functions
 * \param[in]       baudrate: Baudrate to use on AT port
 * \return          espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_ll_init(esp_ll_t* ll) {
    /*
     * Step 1: Configure memory for dynamic allocations
     */
    static uint8_t memory[ESP_MEM_SIZE];             /* Create memory for dynamic allocations with specific size */

    /*
     * Create region(s) of memory.
     * If device has internal/external memory available,
     * multiple memories may be used
     */
    esp_mem_region_t mem_regions[] = {
        { memory, sizeof(memory) }
    };
    if (!initialized) {
        esp_mem_assignmemory(mem_regions, ESP_ARRAYSIZE(mem_regions));  /* Assign memory for allocations to ESP library */
    }
    
    /*
     * Step 2: Set AT port send function to use when we have data to transmit
     */
    if (!initialized) {
        ll->send_fn = send_data;                /* Set callback function to send data */
    }

    /*
     * Step 3: Configure AT port to be able to send/receive data to/from ESP device
     */
    configure_uart(ll->uart.baudrate);          /* Initialize UART for communication */
    initialized = 1;
    return espOK;
}

/**
 * \brief           Callback function to de-init low-level communication part
 * \param[in,out]   ll: Pointer to \ref esp_ll_t structure to fill data for communication functions
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_ll_deinit(esp_ll_t* ll) {
    initialized = 0;                            /* Clear initialized flag */
    return espOK;
}
