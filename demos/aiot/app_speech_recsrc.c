/* FreeRTOS includes */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/*Voice Wakeup include */
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "xtensa/core-macros.h"
#include "esp_partition.h"
#include "app_speech_srcif.h"
#include "driver/i2s.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "app_facenet.h"
#include "sdkconfig.h"
#include "esp_sr_iface.h"
#include "esp_sr_models.h"


#define SR_MODEL    esp_sr_wakenet3_quantized

static src_cfg_t srcif;
static const esp_sr_iface_t * pxModel = &SR_MODEL;
static model_iface_data_t * pxModelData;

QueueHandle_t xWordBuffer;
/*-----------------------------------------------------------*/

static void event_wakeup_detected( int r )
{
    assert( g_state == WAIT_FOR_WAKEUP );
    printf( "%s DETECTED.\n", pxModel->get_word_name( pxModelData, r ) );
    heap_caps_print_heap_info( MALLOC_CAP_INTERNAL );
    g_state = STOP_REC;
    printf( "\nState changed\n" );
}
/*-----------------------------------------------------------*/

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
/*-----------------------------------------------------------*/

static void i2s_init( int i )
{
    i2s_config_t i2s_config =
    {
        .mode                 = I2S_MODE_MASTER | I2S_MODE_RX, /*the mode must be set according to DSP configuration */
        .sample_rate          = 16000,                         /*must be the same as DSP configuration */
        .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,    /*must be the same as DSP configuration */
        .bits_per_sample      = 32,                            /*must be the same as DSP configuration */
        .communication_format = I2S_COMM_FORMAT_I2S,
        .dma_buf_count        = 3,
        .dma_buf_len          = 300,
        .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL2,
    };
    i2s_pin_config_t pin_config =
    {
        .bck_io_num   = 26, /* IIS_SCLK */
        .ws_io_num    = 32, /* IIS_LCLK */
        .data_out_num = -1, /* IIS_DSIN */
        .data_in_num  = 33  /* IIS_DOUT */
    };

    if( i == 0 )
    {
        i2s_driver_install( 1, &i2s_config, 0, NULL ); /*need to uninstall this */
        i2s_set_pin( 1, &pin_config );
        i2s_zero_dma_buffer( 1 );
    }
    else
    {
        i2s_driver_uninstall( 1 );
    }
}
/*-----------------------------------------------------------*/

void vRecordingTask( void * arg )
{
    int i = 0;

    printf( "\nRec Task Starts\n" );
    vShowAvailableMemory( "After Rec task starts" );
    i2s_init( 0 );
    vShowAvailableMemory( "After i2s init:" );
    src_cfg_t * cfg = ( src_cfg_t * ) arg;
    size_t samp_len = cfg->item_size * 2 * sizeof( int ) / sizeof( int16_t );

    int * samp = malloc( samp_len );
    printf( "sample length=%d\n", samp_len );
    size_t read_len = 0;

    while( g_state == WAIT_FOR_WAKEUP )
    {
        i2s_read( 1, samp, samp_len, &read_len, portMAX_DELAY );

        for( int x = 0; x < cfg->item_size / 4; x++ )
        {
            int s1 = ( ( samp[ x * 4 ] + samp[ x * 4 + 1 ] ) >> 13 ) & 0x0000FFFF;
            int s2 = ( ( samp[ x * 4 + 2 ] + samp[ x * 4 + 3 ] ) << 3 ) & 0xFFFF0000;
            samp[ x ] = s1 | s2;
        }

        xQueueSend( *cfg->queue, samp, portMAX_DELAY );

        if( i % 10 == 0 )
        {
            vShowAvailableMemory( "Between rec" );
        }

        i++;
    }

    while( g_state != STOP_REC )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }

    free( samp );
    vShowAvailableMemory( "Before i2s deinit:" );
    i2s_init( 1 );
    vShowAvailableMemory( "After i2s deinit:" );
    printf( "\nRec Task Ends\n" );
    vShowAvailableMemory( "After Rec task end" );
    g_state = STOP_NN;
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

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
/*-----------------------------------------------------------*/