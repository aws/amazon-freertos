/**
 * \file            esp_threads.c
 * \brief           OS threads implementations
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
#include "esp/esp_private.h"
#include "esp/esp_threads.h"
#include "esp/esp_parser.h"
#include "esp/esp_int.h"
#include "esp/esp_timeout.h"
#include "esp/esp.h"
#include "esp/esp_mem.h"
#include "system/esp_sys.h"

/**
 * \brief           User thread to process input packets from API functions
 */
void
esp_thread_producer(void* const arg) {
    esp_t* e = arg;                             /* Thread argument is main structure */
    esp_msg_t* msg;                             /* Message structure */
    espr_t res;
    uint32_t time;
    
    ESP_CORE_PROTECT();                         /* Protect system */
    while (1) {
        ESP_CORE_UNPROTECT();                   /* Unprotect system */
        time = esp_sys_mbox_get(&esp.mbox_producer, (void **)&msg, 0);  /* Get message from queue */
        ESP_THREAD_PRODUCER_HOOK();             /* Execute producer thread hook */
        ESP_CORE_PROTECT();                     /* Protect system */
        if (time == ESP_SYS_TIMEOUT || msg == NULL) {   /* Check valid message */
            continue;
        }
        
        /* For reset message, we can have delay! */
        if (CMD_IS_DEF(ESP_CMD_RESET) && msg->msg.reset.delay) {
            esp_delay(msg->msg.reset.delay);
        }
        
        /*
         * Try to call function to process this message
         * Usually it should be function to transmit data to AT port
         */
        e->msg = msg;
        if (msg->fn != NULL) {                  /* Check for callback processing function */
            ESP_CORE_UNPROTECT();               /* Release protection, think if this is necessary, probably shouldn't be here */
            esp_sys_sem_wait(&e->sem_sync, 120000); /* Lock semaphore, should be unlocked before! */
            ESP_CORE_PROTECT();                 /* Protect system again, think if this is necessary, probably shouldn't be here */
            res = msg->fn(msg);                 /* Process this message, check if command started at least */
            if (res == espOK) {                 /* We have valid data and data were sent */
                ESP_CORE_UNPROTECT();           /* Release protection */
                time = esp_sys_sem_wait(&e->sem_sync, msg->block_time); /* Wait for synchronization semaphore */
                ESP_CORE_PROTECT();              /* Protect system again */
                esp_sys_sem_release(&e->sem_sync);  /* Release protection and start over later */
                if (time == ESP_SYS_TIMEOUT) {  /* Sync timeout occurred? */
                    res = espTIMEOUT;           /* Timeout on command */
                }
            } else {
                esp_sys_sem_release(&e->sem_sync);  /* We failed, release semaphore automatically */
            }
        } else {
            res = espERR;                       /* Simply set error message */
        }
        if (res != espOK) {
            msg->res = res;                     /* Save response */
        }
        
        /*
         * In case message is blocking,
         * release semaphore and notify processing finished,
         * otherwise directly free memory of message structure
         */
        if (msg->is_blocking) {
            esp_sys_sem_release(&msg->sem);     /* Release semaphore only */
        } else {
            ESP_MSG_VAR_FREE(msg);              /* Release message structure */
        }
        esp.msg = NULL;
    }
}

/**
 * \brief           Thread for processing received data from device
 * 
 *                  This thread is also used to handle timeout events
 *                  in correct time order as it is never blocked by user command
 *
 * \sa              ESP_CFG_INPUT_USE_PROCESS
 */
void
esp_thread_process(void* const arg) {
    esp_msg_t* msg;
    uint32_t time;
    
#if !ESP_CFG_INPUT_USE_PROCESS
    ESP_CORE_PROTECT();                         /* Protect system */
    while (1) {
        ESP_CORE_UNPROTECT();                   /* Unprotect system */
        time = espi_get_from_mbox_with_timeout_checks(&esp.mbox_process, (void **)&msg, 10);    /* Get message from queue */
        ESP_THREAD_PROCESS_HOOK();              /* Execute process thread hook */
        ESP_CORE_PROTECT();                     /* Protect system */
        
        if (time == ESP_SYS_TIMEOUT || msg == NULL) {
            ESP_UNUSED(time);                   /* Unused variable */
        }
        espi_process_buffer();                  /* Process input data */
#else
    while (1) {
        /*
         * Check for next timeout event only here
         *
         * If there are no timeouts to process, we can wait unlimited time.
         * In case new timeout occurs, thread will wake up by writing new element to mbox process queue
         */
        time = espi_get_from_mbox_with_timeout_checks(&esp.mbox_process, (void **)&msg, 0);
        ESP_THREAD_PROCESS_HOOK();              /* Execute process thread hook */
        ESP_UNUSED(time);
#endif /* !ESP_CFG_INPUT_USE_PROCESS */
    }
}
