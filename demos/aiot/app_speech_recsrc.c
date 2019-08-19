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

/* structure for sharing the audio buffer */
static src_cfg_t srcif;

/*containers for storing model data */
static const esp_sr_iface_t * pxModel = &SR_MODEL;
static model_iface_data_t * pxModelData;

/* queue that acts as an audio buffer */
QueueHandle_t xWordBuffer;


/*-----------------------------------------------------------*/

/**
 * @brief Speech Recognition task that detects if a word in the buffer is 'Alexa'
 */
void vWakeWordNNTask( void * arg )
{
    /* Initailize a buffer to store the sample from the datasize of the wake word */
    int audio_chunksize = pxModel->get_samp_chunksize( pxModelData );
    int16_t * buffer = malloc( audio_chunksize * sizeof( int16_t ) );
    assert( buffer );

    /* Read from the buffer until a match is found */
    while( 1 )
    {
        /* Wait for incomming messages from the recording task */
        xQueueReceive( xWordBuffer, buffer, portMAX_DELAY );

        /* Check if the word in the buffer matches "Alexa" and trigger the cleanup */
        int r = pxModel->detect( pxModelData, buffer );

        if( r )
        {
            assert( g_state == WAIT_FOR_WAKEUP );
            printf( "%s DETECTED.\n", pxModel->get_word_name( pxModelData, r ) );
            printf( "\nStarting Face Recognition\n" );
            g_state = START_RECOGNITION;     
        }
    }

    /* Wait for the recorder task to be cleaned */
    while( g_state != STOP_NN )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }

    /* Initiate clean up of resources used by the NN task and delete it */
    free( buffer );
    pxModel->destroy( pxModelData );
    vQueueDelete( xWordBuffer );

    /* Signal the start of facial recognition */
    g_state = START_RECOGNITION;
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

/**
 * @brief The function that initialized/deinitializes i2s drivers.
 *
 * @param[in] operation depicts operation type 1 for init and 0 for deinit
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
static void vI2sOperations( int operation )
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

    if( operation == 0 )
    {
        i2s_driver_install( 1, &i2s_config, 0, NULL );
        i2s_set_pin( 1, &pin_config );
        i2s_zero_dma_buffer( 1 );
    }
    else
    {
        i2s_driver_uninstall( 1 );
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief Recording task that takes samples from audio and places them into buffer queue
 *
 * @param[in] arg Reference to the queue and item size
 */
void vRecordingTask( void * arg )
{
    vI2sOperations( 0 );

    /* Initialize the sample from the configuration */
    src_cfg_t * cfg = ( src_cfg_t * ) arg;
    size_t samp_len = cfg->item_size * 2 * sizeof( int ) / sizeof( int16_t );
    int * samp = malloc( samp_len );
    size_t read_len = 0;

    /* Keep sampling the audio and adding it to the queue until the state changes */
    while( 1 )
    {
        while( g_state != WAIT_FOR_WAKEUP )
        {
            vTaskDelay( 1000 / portTICK_PERIOD_MS );
        }

        i2s_read( 1, samp, samp_len, &read_len, portMAX_DELAY );

        /* Generate the sample */
        for( int x = 0; x < cfg->item_size / 4; x++ )
        {
            int s1 = ( ( samp[ x * 4 ] + samp[ x * 4 + 1 ] ) >> 13 ) & 0x0000FFFF;
            int s2 = ( ( samp[ x * 4 + 2 ] + samp[ x * 4 + 3 ] ) << 3 ) & 0xFFFF0000;
            samp[ x ] = s1 | s2;
        }

        /* Push the sample to the queue for further processing */
        xQueueSend( *cfg->queue, samp, portMAX_DELAY );
    }

    while( g_state != STOP_REC )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }

    /* Initialize cleanup of resources */
    free( samp );
    vI2sOperations( 1 );

    /* Trigger state change to stop the recognititon task */
    g_state = STOP_NN;
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

/**
 * @brief Initializer for wake word detection. Initializes the audio buffer and creates tasks.
 */
void vSpeechWakeupInit()
{
    /*Initialize NN Model */
    pxModelData = pxModel->create( DET_MODE_95 );
    int audio_chunksize = pxModel->get_samp_chunksize( pxModelData );

    /*Initialize sound source */
    xWordBuffer = xQueueCreate( 2, ( audio_chunksize * sizeof( int16_t ) ) );
    srcif.queue = &xWordBuffer;
    srcif.item_size = audio_chunksize * sizeof( int16_t );

    /*Initialize the recording and recognition tasks */
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
}
/*-----------------------------------------------------------*/
