/*
 * Amazon FreeRTOS V201906.00 Major
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aiot_esp_config.h
 * @brief Contains configurations for the AIoT demo on ESP-EYE
 */

#ifndef _AIOT_ESP_H_
#define _AIOT_ESP_H_

/* FreeRTOS includes */
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/* System includes to initialize drivers and logs */
#include "esp_log.h"
#include "esp_system.h"
#include "esp_camera.h"

/**
 * @brief The camera board used in the setup
 */
#define CONFIG_CAMERA_MODEL_ESP_EYE    ( 1 )

/**
 * @brief Face detection model. Versions of MTMN are available :
 * MTMN lite in quantization (default)
 * MTMN lite in float
 * MTMN heavy in quantization
 */
#define CONFIG_MTMN_LITE_QUANT         ( 1 )

/**
 * @brief Face recognition model. Versions of FRMN are available :
 * FRMN1 (set)
 * FRMN2C
 * FRMN2
 * FRMN2P (default)
 */
#define CONFIG_FRMN1_QUANT             ( 1 )

/**
 * @brief The image sensor used in the board
 */
#define CONFIG_OV2640_SUPPORT          ( 1 )

/**
 * @brief Specifies the number of samples the user has to enroll
 */
#define ENROLL_CONFIRM_TIMES           ( 3 )

/**
 * @brief The number of faces the user wants to save
 */
#define FACE_ID_SAVE_NUMBER            ( 1 )

/* Include the esp-face modules
 * Need to call these after the configurations are set*/
#include "fd_forward.h"
#include "fr_forward.h"
#include "image_util.h"

/* Structure to pass the handle of the queue between the tasks */
typedef struct
{
    QueueHandle_t * queue;
    int item_size; /*in bytes */
} src_cfg_t;

/* Provides the states for task synchronization */
typedef enum
{
    WAIT_FOR_WAKEUP,
    STOP_REC,
    STOP_NN,
    START_RECOGNITION
} en_fsm_state;

extern en_fsm_state eMachineState;


void vRecordingTask( void * arg );
void vSpeechWakeupInit();
void app_facenet_main();
void vShowAvailableMemory( char message[] );
void vCameraInit();
void vCameraDeInit();
#endif /* ifndef _AIOT_ESP_H_ */
