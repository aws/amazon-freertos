#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "xtensa/core-macros.h"
#include "esp_partition.h"
#include "app_speech_srcif.h"
#include "sdkconfig.h"
#include "esp_sr_iface.h"
#include "esp_sr_models.h"
#include "app_facenet.h"

#define SR_MODEL    esp_sr_wakenet3_quantized

static src_cfg_t srcif;
static const esp_sr_iface_t * pxModel = &SR_MODEL;
static model_iface_data_t * pxModelData;

QueueHandle_t xWordBuffer;

static void event_wakeup_detected( int r )
{
    assert( g_state == WAIT_FOR_WAKEUP );
    printf( "%s DETECTED.\n", pxModel->get_word_name( pxModelData, r ) );
    heap_caps_print_heap_info( MALLOC_CAP_INTERNAL );
    g_state = STOP_REC;
    printf( "\nState changed\n" );
}

void vWakeWordNNTask( void * arg )
{
    printf( "\nNN Task Starts\n" );
    int audio_chunksize = pxModel->get_samp_chunksize( pxModelData );
    int16_t * buffer = malloc( audio_chunksize * sizeof( int16_t ) );
    assert( buffer );
    vShowAvailableMemory( "After buffer alloc in NN task" );

    while( g_state == WAIT_FOR_WAKEUP )
    {
        xQueueReceive( xWordBuffer, buffer, portMAX_DELAY );
        int r = pxModel->detect( pxModelData, buffer );

        if( r )
        {
            event_wakeup_detected( r );
            xQueueReceive( xWordBuffer,
                           buffer,
                           ( TickType_t ) 10 ); /*so that the rec task is not blocked on the queue */
        }
    }

    while( g_state != STOP_NN )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }

    vShowAvailableMemory( "After NN" );
    /*free(buffer); */
    g_state = START_RECOGNITION;
    printf( "\nNN Task Ends\n" );

    pxModel->destroy( pxModelData );
    vQueueDelete( xWordBuffer );
    vShowAvailableMemory( "After NN cleanup" );
    vTaskDelete( NULL );
}

void vSpeechWakeupInit()
{
    vShowAvailableMemory( "Before pxModel creation" );
    /*Initialize NN Model */
    pxModelData = pxModel->create( DET_MODE_95 );

    int audio_chunksize = pxModel->get_samp_chunksize( pxModelData );

    /*Initialize sound source */
    xWordBuffer = xQueueCreate( 2, ( audio_chunksize * sizeof( int16_t ) ) );
    srcif.queue = &xWordBuffer;
    srcif.item_size = audio_chunksize * sizeof( int16_t );
    printf( "%d\n", srcif.item_size );

    vShowAvailableMemory( "Before voice task creation" );
    xTaskCreatePinnedToCore( &vRecordingTask,
                             "rec",
                             3 * 1024,
                             ( void * ) &srcif,
                             5,
                             NULL,
                             1 );

    xTaskCreatePinnedToCore( &vWakeWordNNTask,
                             "nn",
                             2 * 1024,
                             NULL,
                             5,
                             NULL,
                             1 );
    vShowAvailableMemory( "After voice task creation" );
}
