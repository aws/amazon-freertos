/**
 * \file            esp.c
 * \brief           Main ESP core file
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
#include "esp/esp.h"
#include "esp/esp_mem.h"
#include "esp/esp_threads.h"
#include "system/esp_ll.h"

#if ESP_CFG_OS != 1
#error ESP_CFG_OS must be set to 1!
#endif

static espr_t           def_callback(esp_cb_t* cb);
static esp_cb_func_t    def_cb_link;

esp_t esp;

/**
 * \brief           Default callback function for events
 * \param[in]       cb: Pointer to callback data structure
 * \return          Member of \ref espr_t enumeration
 */
static espr_t
def_callback(esp_cb_t* cb) {
    return espOK;
}

/**
 * \brief           Init and prepare ESP stack
 * \note            When \ref ESP_CFG_RESET_ON_INIT is enabled, reset sequence will be sent to device.
 *                  In this case, `blocking` parameter indicates if we shall wait or not for response
 * \param[in]       cb_func: Event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_init(esp_cb_fn cb_func, uint32_t blocking) {
    esp.status.f.initialized = 0;               /* Clear possible init flag */
    
    def_cb_link.fn = cb_func ? cb_func : def_callback;
    esp.cb_func = &def_cb_link;                 /* Set callback function */
    
    esp.cb_server = NULL;                       /* Set default server callback function */
    
    esp_sys_init();                             /* Init low-level system */
    esp.ll.uart.baudrate = ESP_CFG_AT_PORT_BAUDRATE;    /* Set default baudrate value */
    esp_ll_init(&esp.ll);                       /* Init low-level communication */
    
    esp_sys_sem_create(&esp.sem_sync, 1);       /* Create new semaphore with unlocked state */
    esp_sys_mbox_create(&esp.mbox_producer, ESP_CFG_THREAD_PRODUCER_MBOX_SIZE); /* Producer message queue */
    esp_sys_thread_create(&esp.thread_producer, "esp_producer", esp_thread_producer, &esp, ESP_SYS_THREAD_SS, ESP_SYS_THREAD_PRIO);
    
    esp_sys_mbox_create(&esp.mbox_process, ESP_CFG_THREAD_PROCESS_MBOX_SIZE);   /* Consumer message queue */
    esp_sys_thread_create(&esp.thread_process,  "esp_process", esp_thread_process, &esp, ESP_SYS_THREAD_SS, ESP_SYS_THREAD_PRIO);

#if !ESP_CFG_INPUT_USE_PROCESS
    esp_buff_init(&esp.buff, ESP_CFG_RCV_BUFF_SIZE);    /* Init buffer for input data */
#endif /* !ESP_CFG_INPUT_USE_PROCESS */
    esp.status.f.initialized = 1;               /* We are initialized now */
    esp.status.f.dev_present = 1;               /* We assume device is present at this point */
    
    /*
     * Call reset command and call default
     * AT commands to prepare basic setup for device
     */
    espi_conn_init();                           /* Init connection module */
#if ESP_CFG_RESET_ON_INIT
    if (esp.status.f.dev_present) {             /* In case device exists */
        esp_reset_with_delay(ESP_CFG_RESET_DELAY_DEFAULT, blocking);    /* Send reset sequence with delay */
    }
#else
    ESP_UNUSED(blocking);                       /* Unused variable */
#endif /* ESP_CFG_RESET_ON_INIT */
    espi_send_cb(ESP_CB_INIT_FINISH);           /* Call user callback function */
    
    return espOK;
}

/**
 * \brief           Execute reset and send default commands
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_reset(uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_RESET;
    ESP_MSG_VAR_REF(msg).msg.reset.delay = 0;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 5000);    /* Send message to producer queue */
}

/**
 * \brief           Execute reset and send default commands with delay before first command
 * \param[in]       delay: Number of milliseconds to wait before initiating first command to device
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_reset_with_delay(uint32_t delay, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_RESET;
    ESP_MSG_VAR_REF(msg).msg.reset.delay = delay;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 5000);    /* Send message to producer queue */
}

/**
 * \brief           Execute restore command and set module to default values
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_restore(uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */

    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_RESTORE;

    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 5000);    /* Send message to producer queue */
}

/**
 * \brief           Sets WiFi mode to either station only, access point only or both
 * \param[in]       mode: Mode of operation. This parameter can be a value of \ref esp_mode_t enumeration
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_set_wifi_mode(esp_mode_t mode, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CWMODE;
    ESP_MSG_VAR_REF(msg).msg.wifi_mode.mode = mode; /* Set desired mode */
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 1000);    /* Send message to producer queue */
}

/**
 * \brief           Sets baudrate of AT port (usually UART)
 * \param[in]       baud: Baudrate in units of bits per second
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_set_at_baudrate(uint32_t baud, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_UART;
    ESP_MSG_VAR_REF(msg).msg.uart.baudrate = baud;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 2000);    /* Send message to producer queue */
}

/**
 * \brief           Enables or disables server mode
 * \param[in]       en: Set to `1` to enable server, `0` otherwise
 * \param[in]       port: Set port number used to listen on. Set to `0` to disable server
 * \param[in]       max_conn: Number of maximal connections populated by server
 * \param[in]       timeout: Time used to automatically close the connection in units of seconds.
 *                  Set to `0` to disable timeout feature (not recommended)
 * \param[in]       cb: Connection callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_set_server(uint8_t en, esp_port_t port, uint16_t max_conn, uint16_t timeout, esp_cb_fn cb, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_ASSERT("port > 0", port > 0);           /* Assert input parameters */

    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_TCPIP_CIPSERVER;
    if (en > 0) {
        ESP_MSG_VAR_REF(msg).cmd = ESP_CMD_TCPIP_CIPSERVERMAXCONN;  /* First command is to set maximal number of connections for server */
    }
    ESP_MSG_VAR_REF(msg).msg.tcpip_server.en = en;
    ESP_MSG_VAR_REF(msg).msg.tcpip_server.port = port;
    ESP_MSG_VAR_REF(msg).msg.tcpip_server.max_conn = max_conn;
    ESP_MSG_VAR_REF(msg).msg.tcpip_server.timeout = timeout;
    ESP_MSG_VAR_REF(msg).msg.tcpip_server.cb = cb;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 1000);    /* Send message to producer queue */
}

#if ESP_CFG_MODE_STATION || __DOXYGEN__

/**
 * \brief           Updated ESP software remotely
 * \note            ESP must be connected to access point to use this feature
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_update_sw(uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */

    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_TCPIP_CIUPDATE;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 180000);  /* Send message to producer queue */
}

#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */

#if ESP_CFG_DNS || __DOXYGEN__

/**
 * \brief           Get IP address from host name
 * \param[in]       host: Pointer to host name to get IP for
 * \param[out]      ip: Pointer to \ref esp_ip_t variable to save IP
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_dns_getbyhostname(const char* host, esp_ip_t* ip, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_ASSERT("host != NULL", host != NULL);   /* Assert input parameters */
    ESP_ASSERT("ip != NULL", ip != NULL);       /* Assert input parameters */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_TCPIP_CIPDOMAIN;
    ESP_MSG_VAR_REF(msg).msg.dns_getbyhostname.host = host;
    ESP_MSG_VAR_REF(msg).msg.dns_getbyhostname.ip = ip;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 20000);   /* Send message to producer queue */
}

#endif /* ESP_CFG_DNS || __DOXYGEN__ */

/**
 * \brief           Increase protection counter
 *
 *                  If lock was `0` before func call, lock is enabled and increased
 * \note            Function may be called multiple times to increase locks. 
 *                  User must take care of calling \ref esp_core_unlock function the same times to decrease lock
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_core_lock(void) {
    ESP_CORE_PROTECT();                         /* Lock ESP core */
    return espOK;
}

/**
 * \brief           Decrease protection counter
 *
 *                  If lock was non-zero before func call, it is decreased. In case of `lock = 0`, protection is disabled
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_core_unlock(void) {
    ESP_CORE_UNPROTECT();                       /* Unlock ESP core */
    return espOK;
}

/**
 * \brief           Register callback function for global (non-connection based) events
 * \param[in]       cb_fn: Callback function to call on specific event
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_cb_register(esp_cb_fn cb_fn) {
    espr_t res = espOK;
    esp_cb_func_t* func, *newFunc;
    
    ESP_ASSERT("cb_fn != NULL", cb_fn != NULL); /* Assert input parameters */
    
    ESP_CORE_PROTECT();                         /* Lock ESP core */
    
    /* Check if function already exists on list */
    for (func = esp.cb_func; func != NULL; func = func->next) {
        if (func->fn == cb_fn) {
            res = espERR;
            break;
        }
    }
    
    if (res == espOK) {
        newFunc = esp_mem_alloc(sizeof(*newFunc));  /* Get memory for new function */
        if (newFunc != NULL) {
            ESP_MEMSET(newFunc, 0x00, sizeof(*newFunc));/* Reset memory */
            newFunc->fn = cb_fn;                /* Set function pointer */
            if (esp.cb_func == NULL) {
                esp.cb_func = newFunc;          /* This should never happen! */
            } else {
                for (func = esp.cb_func; func->next != NULL; func = func->next) {}
                func->next = newFunc;           /* Set new function as next */
            }
            res = espOK;
        } else {
            res = espERRMEM;
        }
    }
    ESP_CORE_UNPROTECT();                       /* Unlock ESP core */
    return res;
}

/**
 * \brief           Unregister callback function for global (non-connection based) events
 * \note            Function must be first registered using \ref esp_cb_register
 * \param[in]       cb_fn: Callback function to call on specific event
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_cb_unregister(esp_cb_fn cb_fn) {
    esp_cb_func_t* func, *prev;
    ESP_ASSERT("cb_fn != NULL", cb_fn != NULL); /* Assert input parameters */
    
    ESP_CORE_PROTECT();                         /* Lock ESP core */
    for (prev = esp.cb_func, func = esp.cb_func->next; func != NULL; prev = func, func = func->next) {
        if (func->fn == cb_fn) {
            prev->next = func->next;
            esp_mem_free(func);
            func = NULL;
            break;
        }
    }
    ESP_CORE_UNPROTECT();                       /* Unlock ESP core */
    return espOK;
}

/**
 * \brief           Notify stack if device is present or not
 * 
 *                  Use this function to notify stack that device is not connected and not ready to communicate with host device
 * \param[in]       present: Flag indicating device is present
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_device_set_present(uint8_t present, uint32_t blocking) {
    espr_t res = espOK;
    ESP_CORE_PROTECT();                         /* Lock ESP core */
    esp.status.f.dev_present = ESP_U8(!!present);   /* Device is present */
    
    if (!esp.status.f.dev_present) {
        espi_reset_everything(1);               /* Reset everything */
    }
#if ESP_CFG_RESET_ON_INIT
    else  {                                     /* Is new device present? */
        ESP_CORE_UNPROTECT();                   /* Unlock ESP core */
        res = esp_reset_with_delay(ESP_CFG_RESET_DELAY_DEFAULT, blocking); /* Reset with delay */
        ESP_CORE_PROTECT();                     /* Lock ESP core */
    }
#else
    ESP_UNUSED(blocking);                       /* Unused variable */
#endif /* ESP_CFG_RESET_ON_INIT */
    
    espi_send_cb(ESP_CB_DEVICE_PRESENT);        /* Send present event */
    
    ESP_CORE_UNPROTECT();                       /* Unlock ESP core */
    return res;
}

/**
 * \brief           Check if device is present
 * \return          `1` on success, `0` otherwise
 */
uint8_t
esp_device_is_present(void) {
    uint8_t res;
    ESP_CORE_PROTECT();                         /* Lock ESP core */
    res = esp.status.f.dev_present;
    ESP_CORE_UNPROTECT();                       /* Unlock ESP core */
    return res;
}

/**
 * \brief           Delay for amount of milliseconds
 * \param[in]       ms: Milliseconds to delay
 */
void
esp_delay(uint32_t ms) {
    esp_sys_sem_t sem;
    if (ms != 0) {
        esp_sys_sem_create(&sem, 0);            /* Create semaphore in locked state */
        esp_sys_sem_wait(&sem, ms);             /* Wait for semaphore, timeout should occur */
        esp_sys_sem_release(&sem);              /* Release semaphore */
        esp_sys_sem_delete(&sem);               /* Delete semaphore */
    }
}
