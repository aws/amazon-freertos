#ifndef _AIOT_ESP_H_
#define _AIOT_ESP_H_
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_camera.h"

#define CONFIG_CAMERA_MODEL_ESP_EYE    ( 1 )
#define CONFIG_MTMN_LITE_FLOAT         ( 1 )
#define CONFIG_OV2640_SUPPORT          ( 1 )
#define ENROLL_CONFIRM_TIMES           ( 1 )
#define FACE_ID_SAVE_NUMBER            ( 1 )

#include "fd_forward.h"
#include "fr_forward.h"
#include "image_util.h"

typedef struct
{
    QueueHandle_t * queue;
    int item_size;     /*in bytes */
} src_cfg_t;

typedef enum
{
    WAIT_FOR_WAKEUP,
    STOP_REC,
    STOP_NN,
    START_RECOGNITION
} en_fsm_state;

extern en_fsm_state g_state;

void vRecordingTask( void * arg );
void vSpeechWakeupInit();
void app_facenet_main();
void vShowAvailableMemory( char message[] );
void vCameraInit();
void vCameraDeInit();
#endif /* ifndef _AIOT_ESP_H_ */
