/*
 * Basic leds blinking sample for FreeRTOS
 * Author: Sergey Shcherbakov <shchers@gmail.com>
 */

#include <avr/io.h>

/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>

/// Init all MCU internals
void mcu_init(void)
{
	// Configuring GPIO
	DDRB |= _BV(PORTB5);
	DDRD |= _BV(PORTD1);
}

/* Task to be created. */
void vTask1( void * pvParameters )
{
	for( ;; ) {
		PORTD |= _BV(PORTD1);
		vTaskDelay(500/portTICK_PERIOD_MS);
		PORTD &= ~_BV(PORTD1);
		vTaskDelay(500/portTICK_PERIOD_MS);
    }
}

/* Task to be created. */
void vTask2( void * pvParameters )
{
	for( ;; ) {
		PORTB |= _BV(PORTB5);
		vTaskDelay(250/portTICK_PERIOD_MS);
		PORTB &= ~_BV(PORTB5);
		vTaskDelay(250/portTICK_PERIOD_MS);
	}
}

int main(void)
{
	mcu_init();

	xTaskCreate(
		vTask1,						/* Function that implements the task. */
		"led1",						/* Text name for the task. */
		configMINIMAL_STACK_SIZE,   /* Stack size in words, not bytes. */
		NULL,						/* Parameter passed into the task. */
		tskIDLE_PRIORITY,			/* Priority at which the task is created. */
		NULL );						/* Used to pass out the created task's handle. */

	xTaskCreate(
		vTask2,						/* Function that implements the task. */
		"led2",						/* Text name for the task. */
		configMINIMAL_STACK_SIZE,	/* Stack size in words, not bytes. */
		NULL,						/* Parameter passed into the task. */
		tskIDLE_PRIORITY,			/* Priority at which the task is created. */
		NULL );						/* Used to pass out the created task's handle. */

	// Start FreeRTOS scheduler
	vTaskStartScheduler();

	// We should not pass here!
	for(;;);
}
