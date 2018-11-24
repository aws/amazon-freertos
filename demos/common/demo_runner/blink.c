/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "aws_hello_world.h"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO GPIO_NUM_2

typedef struct {
	gpio_num_t gpio;
	TaskHandle_t handler;
	TickType_t delay;
} blink_parameter_t;

void blink_task(void *pvParameter)
{
	blink_parameter_t* blinkParam;
	uint8_t gpio;
	TickType_t delay;

	blinkParam = ( blink_parameter_t* ) pvParameter;

	gpio = (uint8_t) blinkParam->gpio;
	delay = blinkParam->delay;

    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(gpio);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);

    configPRINTF(("gpio: %d\n", gpio));

    uint8_t on_off = 0;

    while(1) {
        /* Blink off (output low) */
        gpio_set_level(gpio, on_off);
        on_off = !on_off;
        vTaskDelay(delay);
        configPRINTF(("blink %s\n", on_off?"ON":"OFF"));

//        xTaskNotifyGive( xTaskMQTT );
    }
}

void blinkTask(void)
{
	blink_parameter_t gpio_conf = {
			.gpio = GPIO_NUM_2,
			.delay = 1000 / portTICK_PERIOD_MS,
    };

	BaseType_t status;
    status = xTaskCreate(&blink_task, "blink_task", ( configMINIMAL_STACK_SIZE * 4 ), &gpio_conf, ( tskIDLE_PRIORITY + 2 ), NULL);
    configPRINTF(("BaseType_t from creating blink task: %d\n", status));
}
