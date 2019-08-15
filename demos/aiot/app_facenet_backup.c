/* ESPRESSIF MIT License
 * 
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 * 
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <string.h>
#include <math.h>
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "app_facenet.h"
#include "sdkconfig.h"

static const char *TAG = "app_process";
unsigned long ulVar = 10UL;

mtmn_config_t init_config()
{
    mtmn_config_t mtmn_config = {0};
    mtmn_config.type = FAST;
    mtmn_config.min_face = 80;
    mtmn_config.pyramid = 0.707;
    mtmn_config.pyramid_times = 4;
    mtmn_config.p_threshold.score = 0.3;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.p_threshold.candidate_number = 20;
    mtmn_config.r_threshold.score = 0.3;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 10;
    mtmn_config.o_threshold.score = 0.4;
    mtmn_config.o_threshold.nms = 0.3;
    mtmn_config.o_threshold.candidate_number = 1;

    return mtmn_config;
}

void task_process(void *arg)
{
    //#if 0
    size_t frame_num = 0;
    dl_matrix3du_t *image_matrix = NULL;
    camera_fb_t *fb = NULL;

    /* 1. Load configuration for detection */
    mtmn_config_t mtmn_config = init_config();
    int64_t memory = 0;

    memory = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    ESP_LOGI(TAG, "configTOTAL_HEAP_SIZE :%d", configTOTAL_HEAP_SIZE);
    ESP_LOGI(TAG, "real: :%d", xPortGetFreeHeapSize());
    ESP_LOGI(TAG, "Startup :%lld \n SPI", memory);
    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
    int64_t start_time = esp_timer_get_time();
#if 0
    do
    {   int64_t timer = esp_timer_get_time()-start_time/1000;
    ESP_LOGE(TAG, "Time: %lld", timer/1000);
        if ((timer%1000) == 0)
        {
            ESP_LOGE(TAG, "Time: %lld", timer);
        }
        
        //int64_t start_time = esp_timer_get_time();
        /* 2. Get one image with camera */
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed!");
            continue;
        }
        //int64_t fb_get_time = esp_timer_get_time();
        //ESP_LOGI(TAG, "Get one frame in %lld ms.", (fb_get_time - start_time) / 1000);

        
        /* 3. Allocate image matrix to store RGB data */
        image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
        // ESP_LOGI(TAG, "width: %d, height: %d",fb->width, fb->height );
        // memory = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        // ESP_LOGI(TAG, "After image capture :%lld", memory);
        
        /* 4. Transform image to RGB */
        uint32_t res = fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
        if (true != res)
        {
            ESP_LOGE(TAG, "fmt2rgb888 failed, fb: %d", fb->len);
            dl_matrix3du_free(image_matrix);
            continue;
        }

        esp_camera_fb_return(fb);

        /* 5. Do face detection */
         box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
        // ESP_LOGI(TAG, "Detection time consumption: %lldms", (esp_timer_get_time() - fb_get_time) / 1000);
        // ESP_LOGI(TAG, "SPI allocation" );
        // heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
        if (net_boxes)
        {
            frame_num++;
            ESP_LOGI(TAG, "DETECTED: %d\n", frame_num);
            free(net_boxes->score);
            free(net_boxes->box);
            free(net_boxes->landmark);
            free(net_boxes);
        }
        // else
        // {
        //     ESP_LOGI(TAG, "NOT DETECTED: %d\n", frame_num);
        // }
        

        dl_matrix3du_free(image_matrix);
        // memory = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        // ESP_LOGI(TAG, "After cleanup :%lld", memory);
        // ESP_LOGI(TAG, "exec");
        // heap_caps_print_heap_info(MALLOC_CAP_EXEC);
        // ESP_LOGI(TAG, "8bit");
        // heap_caps_print_heap_info(MALLOC_CAP_8BIT);
        // ESP_LOGI(TAG, "SPI after free" );
        // heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
    } while(1);
#endif
    xQueue = xQueueCreate(10, sizeof(int));
    if (xQueue != 0)
    {
        if (xQueueSendToBack(xQueue,
                             (void *)&ulVar,
                             (TickType_t)10) != pdPASS)
        {
            ESP_LOGI(TAG, "------NO------------");
        }
        ESP_LOGI(TAG, "--------YES----------");
    }

    while (1)
    {
    }
}

void app_facenet_main()
{
    xTaskCreatePinnedToCore(task_process, "process", 6 * 1024, NULL, 5, NULL, 1);
    //task_process(NULL);
}
