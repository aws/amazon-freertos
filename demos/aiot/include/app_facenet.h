/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
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
 *
 */
#ifndef _APP_FACENET_H_
    #define _APP_FACENET_H_

    #if __cplusplus
        extern "C" {
    #endif

    #include "fd_forward.h"
    #include "fr_forward.h"
    #include "image_util.h"
    #include "app_camera.h"

    #define ENROLL_CONFIRM_TIMES    1
    #define FACE_ID_SAVE_NUMBER     1

    void app_facenet_main();
    void vShowAvailableMemory( char message[] );

    #if __cplusplus
}
    #endif
#endif /* ifndef _APP_FACENET_H_ */

typedef enum
{
    WAIT_FOR_WAKEUP,
    STOP_REC,
    STOP_NN,
    START_RECOGNITION
} en_fsm_state;

extern en_fsm_state g_state;
