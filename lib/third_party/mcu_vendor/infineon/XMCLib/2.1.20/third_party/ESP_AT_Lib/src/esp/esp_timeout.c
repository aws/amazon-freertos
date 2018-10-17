/**	
 * \file            esp_timeout.c
 * \brief           Timeout manager
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
#include "esp/esp_timeout.h"
#include "esp/esp_mem.h"

static esp_timeout_t* first_timeout;
static uint32_t last_timeout_time;

/**
 * \brief           Get time we have to wait before we can process next timeout
 * \return          Time in units of milliseconds to wait
 */
static uint32_t
get_next_timeout_diff(void) {
    uint32_t diff;
    if (!first_timeout) {
        return 0xFFFFFFFF;
    }
    diff = esp_sys_now() - last_timeout_time;   /* Get difference between current time and last process time */
    if (diff >= first_timeout->time) {          /* Are we over already? */
        return 0;                               /* We have to immediatelly process this timeout */
    }
    return first_timeout->time - diff;          /* Return remaining time for sleep */
}

/**
 * \brief           Process next timeout in a linked list
 */
static void
process_next_timeout(void) {
    uint32_t time;
    
    time = esp_sys_now();
    
    /*
     * Before calling timeout callback, update variable
     * to make sure we have correct timing in case 
     * callback creates timeout value again
     */
    last_timeout_time = time;                   /* Reset variable when we were last processed */
    
    if (first_timeout != NULL) {
        esp_timeout_t* to = first_timeout;
        
        /*
         * Before calling callback remove current timeout from list
         * to make sure we are safe in case callback function
         * adds a new timeout entry to list
         */
        first_timeout = first_timeout->next;    /* Set next timeout on a list as first timeout */
        to->fn(to->arg);                        /* Call user callback function */
        esp_mem_free(to);                       /* Free timeout memory */
        to = NULL;
    }
}

/**
 * \brief           Get next entry from message queue
 * \param[in]       b: Pointer to message queue to get element
 * \param[out]      m: Pointer to pointer to output variable
 * \param[in]       timeout: Maximal time to wait for message (0 = wait until message received)
 * \return          Time in milliseconds required for next message
 */
uint32_t
espi_get_from_mbox_with_timeout_checks(esp_sys_mbox_t* b, void** m, uint32_t timeout) {
    uint32_t wait_time;
    do {
        if (first_timeout == NULL) {            /* We have no timeouts ready? */
            return esp_sys_mbox_get(b, m, timeout); /* Get entry from message queue */
        }
        wait_time = get_next_timeout_diff();    /* Get time to wait for next timeout execution */
        if (wait_time == 0 || esp_sys_mbox_get(b, m, wait_time) == ESP_SYS_TIMEOUT) {
            ESP_THREAD_PROCESS_HOOK();          /* Process thread hook */
            ESP_CORE_PROTECT();
            process_next_timeout();             /* Process with next timeout */
            ESP_CORE_UNPROTECT();
        }
        break;
    } while (1);
    return wait_time;
}

/**
 * \brief           Add new timeout to processing list
 * \param[in]       time: Time in units of milliseconds for timeout execution
 * \param[in]       fn: Callback function to call when timeout expires
 * \param[in]       arg: Pointer to user specific argument to call when timeout callback function is executed
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_timeout_add(uint32_t time, esp_timeout_fn_t fn, void* arg) {
    esp_timeout_t* to;
    uint32_t now, diff = 0;
    
    to = esp_mem_calloc(1, sizeof(*to));        /* Allocate memory for timeout structure */
    if (to == NULL) {
        return espERR;
    }
    
    now = esp_sys_now();                        /* Get current time */
    if (first_timeout) {
        diff = now - last_timeout_time;         /* Get difference between current and last processed time */
    }
    
    /*
     * Since we want timeout value to start from NOW,
     * we have to add time when we last processed our timeouts
     */
    to->time = time + diff;
    to->arg = arg;
    to->fn = fn;
    
    /*
     * Add new timeout to proper place on linked list
     * and align times to have correct values between timeouts
     */
    if (first_timeout == NULL) {
        first_timeout = to;                     /* Set as first element */
        last_timeout_time = esp_sys_now();      /* Reset last timeout time to current time */
    } else {                                    /* Find where to place a new timeout */
        esp_timeout_t* t;
        /*
         * First check if we have to put new timeout
         * to beginning of linked list.
         * In this case just align new value for current first element
         */
        if (first_timeout->time > to->time) {
            first_timeout->time -= time;        /* Decrease first timeout value to match difference */
            to->next = first_timeout;           /* Set first timeout as next of new one */
            first_timeout = to;                 /* Set new timeout as first */
        } else {                                /* Go somewhere in between current list */
            for (t = first_timeout; t != NULL; t = t->next) {
                to->time -= t->time;            /* Decrease new timeout time by time in a linked list */
                /*
                 * Enter between 2 entries on a list in case:
                 * 
                 * - We reached end of linked list
                 * - Our time is less than diff between 2 entries in list
                 */
                if (t->next == NULL || t->next->time > to->time) {
                    if (t->next != NULL) {      /* Check if there is next element */
                        t->next->time -= to->time;  /* Decrease difference time to next one */
                    } else if (to->time > time) {
                        to->time = time + first_timeout->time;
                    }
                    to->next = t->next;         /* Change order of elements */
                    t->next = to;               /* Add new element to linked list */
                    break;
                }
            }
        }
    }
    esp_sys_mbox_putnow(&esp.mbox_process, NULL);   /* Write message to process queue to wakeup process thread and to start */
    return espOK;
}

/**
 * \brief           Remove callback from timeout list
 * \param[in]       fn: Callback function to identify timeout to remove
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_timeout_remove(esp_timeout_fn_t fn) {
    esp_timeout_t *t, *t_prev;
    
    for (t = first_timeout, t_prev = NULL; t != NULL;
            t_prev = t, t = t->next) {          /* Check all entries */
        if (t->fn == fn) {                      /* Do we have a match from callback point of view? */
            
            /*
             * We have to first increase
             * difference time between current and next one
             * to be aligned for correct wait time
             */
            if (t->next != NULL) {              /* Do we have next element? */
                t->next->time += t->time;       /* Increase timeout time for next element */
            }
            
            /*
             * In case we have previous element on a list,
             * set next element of previous to next of current one
             * otherwise we were first element so simply set 
             * next of current as first one
             */
            if (t_prev != NULL) {
                t_prev->next = t->next;
            } else {
                first_timeout = t->next;
            }
            esp_mem_free(t);
            t = NULL;
            return espOK;
        }
    }
    return espERR;
}
