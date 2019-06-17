/**
 * \file            esp_netconn.c
 * \brief           API functions for sequential calls
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
#include "esp/esp_netconn.h"
#include "esp/esp_private.h"
#include "esp/esp_conn.h"
#include "esp/esp_mem.h"

#if ESP_CFG_NETCONN || __DOXYGEN__

/**
 * \brief           Sequential API structure
 */
typedef struct esp_netconn_t {
    struct esp_netconn_t* next;                 /*!< Linked list entry */

    esp_netconn_type_t type;                    /*!< Netconn type */
    esp_port_t listen_port;                     /*!< Port on which we are listening */
    
    size_t rcv_packets;                         /*!< Number of received packets so far on this connection */
    esp_conn_p conn;                            /*!< Pointer to actual connection */
    
    esp_sys_mbox_t mbox_accept;                 /*!< List of active connections waiting to be processed */
    esp_sys_mbox_t mbox_receive;                /*!< Message queue for receive mbox */
    
    uint8_t* buff;                              /*!< Pointer to buffer for \ref esp_netconn_write function. used only on TCP connection */
    size_t buff_len;                            /*!< Total length of buffer */
    size_t buff_ptr;                            /*!< Current buffer pointer for write mode */

#if ESP_CFG_NETCONN_RECEIVE_TIMEOUT || __DOXYGEN__
    uint32_t rcv_timeout;                       /*!< Receive timeout in unit of milliseconds */
#endif
} esp_netconn_t;

static uint8_t recv_closed = 0xFF, recv_not_present = 0xFF;
static esp_netconn_t* listen_api;               /*!< Main connection in listening mode */
static esp_netconn_t* netconn_list;             /*!< Linked list of netconn entries */

/**
 * \brief           Flush all mboxes and clear possible used memories
 * \param[in]       nc: Pointer to netconn to flush
 */
static void
flush_mboxes(esp_netconn_t* nc) {
    esp_pbuf_p pbuf;
    esp_netconn_t* new_nc;
    esp_core_lock();                            /* Protect ESP core */
    if (esp_sys_mbox_isvalid(&nc->mbox_receive)) {
        while (esp_sys_mbox_getnow(&nc->mbox_receive, (void **)&pbuf)) {
            if (pbuf != NULL && (uint8_t *)pbuf != (uint8_t *)&recv_closed) {
                esp_pbuf_free(pbuf);            /* Free received data buffers */
            }
        }
        esp_sys_mbox_delete(&nc->mbox_receive); /* Delete message queue */
        esp_sys_mbox_invalid(&nc->mbox_receive);/* Invalid handle */
    }
    if (esp_sys_mbox_isvalid(&nc->mbox_accept)) {
        while (esp_sys_mbox_getnow(&nc->mbox_accept, (void **)&new_nc)) {
            if (new_nc != NULL && (uint8_t *)new_nc != (uint8_t *)&recv_closed) {
                esp_netconn_close(new_nc);      /* Close netconn connection */
            }
        }
        esp_sys_mbox_delete(&nc->mbox_accept);  /* Delete message queue */
        esp_sys_mbox_invalid(&nc->mbox_accept); /* Invalid handle */
    }
    esp_core_unlock();                          /* Release protection */
}

/**
 * \brief           Callback function for every server connection
 * \param[in]       cb: Pointer to callback structure
 * \return          Member of \ref espr_t enumeration
 */
static espr_t
esp_cb(esp_cb_t* cb) {
    esp_conn_p conn;
    esp_netconn_t* nc = NULL;
    uint8_t close = 0;
    
    conn = esp_conn_get_from_evt(cb);           /* Get connection from event */
    switch (cb->type) {
        /*
         * A new connection has been active
         * and should be handled by netconn API
         */
        case ESP_CB_CONN_ACTIVE: {              /* A new connection active is active */
            if (esp_conn_is_client(conn)) {     /* Was connection started by us? */
                nc = esp_conn_get_arg(conn);    /* Argument should be ready already */
                if (nc != NULL) {               /* Netconn should be set when connecting to server */
                    nc->conn = conn;            /* Save actual connection */
                } else {
                    close = 1;                  /* Close this connection, invalid netconn */
                }
            } else if (esp_conn_is_server(conn) && listen_api != NULL) {    /* Is the connection server type and we have known listening API? */
                /*
                 * Create a new netconn structure
                 * and set it as connection argument.
                 */
                nc = esp_netconn_new(ESP_NETCONN_TYPE_TCP); /* Create new API */
                ESP_DEBUGW(ESP_CFG_DBG_NETCONN | ESP_DBG_TYPE_TRACE | ESP_DBG_LVL_WARNING,
                    nc == NULL, "NETCONN: Cannot create new structure for incoming server connection!\r\n");
                
                if (nc != NULL) {
                    nc->conn = conn;            /* Set connection callback */
                    esp_conn_set_arg(conn, nc); /* Set argument for connection */
#if ESP_CFG_NETCONN_ACCEPT_ON_CONNECT
                    /*
                     * If user wants to write connection to accept mbox
                     * immediately after connection is active,
                     * process it here.
                     *
                     * In case there is no listening connection,
                     * simply close the connection
                     */
                    if (!esp_sys_mbox_isvalid(&listen_api->mbox_accept) ||
                        !esp_sys_mbox_putnow(&listen_api->mbox_accept, nc)) {
                        close = 1;
                    }
#endif /* ESP_CFG_NETCONN_ACCEPT_ON_CONNECT */
                } else {
                    close = 1;
                }
            } else {
                ESP_DEBUGW(ESP_CFG_DBG_NETCONN | ESP_DBG_TYPE_TRACE | ESP_DBG_LVL_WARNING, listen_api == NULL,
                    "NETCONN: Closing connection as there is no listening API in netconn!\r\n");
                close = 1;                      /* Close the connection at this point */
            }
            if (close) {
                esp_conn_close(conn, 0);        /* Close the connection */
                if (nc != NULL) {
                    esp_netconn_delete(nc);     /* Free memory for API */
                }
            }
            break;
        }
        
        /*
         * We have a new data received which
         * should have netconn structure as argument
         */
        case ESP_CB_CONN_DATA_RECV: {
            esp_pbuf_p pbuf;

            nc = esp_conn_get_arg(conn);        /* Get API from connection */
            pbuf = esp_evt_conn_data_recv_get_buff(cb); /* Get received buff */

            esp_conn_recved(conn, pbuf);        /* Notify stack about received data */
#if !ESP_CFG_NETCONN_ACCEPT_ON_CONNECT
            /*
             * Write data to listening connection accept mbox,
             * only when first data packet arrives
             *
             * Apply this only if connection is in server mode
             */
            if (!nc->rcv_packets && esp_conn_is_server(conn) &&
                listen_api != NULL) {           /* Is this our first packet? */
                if (esp_sys_mbox_isvalid(&listen_api->mbox_accept)) {
                    if (!esp_sys_mbox_putnow(&listen_api->mbox_accept, nc)) {
                        ESP_DEBUGF(ESP_CFG_DBG_NETCONN | ESP_DBG_TYPE_TRACE | ESP_DBG_LVL_DANGER,
                            "NETCONN: Cannot put server connection to accept mbox\r\n");
                        close = 1;
                    }
                } else {
                    ESP_DEBUGF(ESP_CFG_DBG_NETCONN, "NETCONN: Invalid accept mbox\r\n");
                    close = 1;
                }
            }
#endif /* !ESP_CFG_NETCONN_ACCEPT_ON_CONNECT */

            nc->rcv_packets++;                  /* Increase number of received packets */
            if (!close) {
                /*
                 * First increase reference number to prevent
                 * other thread to process the incoming packet
                 * and free it while we still need it here
                 *
                 * In case of problems writing packet to queue,
                 * simply force free to decrease reference counter back to previous value
                 */
                esp_pbuf_ref(pbuf);             /* Increase reference counter */
                if (!nc || !esp_sys_mbox_isvalid(&nc->mbox_receive) || 
                    !esp_sys_mbox_putnow(&nc->mbox_receive, pbuf)) {
                    ESP_DEBUGF(ESP_CFG_DBG_NETCONN, "NETCONN: Ignoring more data for receive!\r\n");
                    esp_pbuf_free(pbuf);        /* Free pbuf */
                    return espOKIGNOREMORE;     /* Return OK to free the memory and ignore further data */
                }
            }
            ESP_DEBUGF(ESP_CFG_DBG_NETCONN | ESP_DBG_TYPE_TRACE, "NETCONN: Written %d bytes to receive mbox\r\n",
                    (int)esp_pbuf_length(pbuf, 0));
            break;
        }
        
        /*
         * Connection was just closed
         */
        case ESP_CB_CONN_CLOSED: {
            nc = esp_conn_get_arg(conn);        /* Get API from connection */
            
            /*
             * In case we have a netconn available, 
             * simply write pointer to received variable to indicate closed state
             */
            if (nc != NULL && esp_sys_mbox_isvalid(&nc->mbox_receive)) {
                esp_sys_mbox_putnow(&nc->mbox_receive, (void *)&recv_closed);
            }
            
            break;
        }
        default:
            return espERR;
    }
    if (close) {
        esp_conn_close(conn, 0);                /* Close the connection */
        if (nc != NULL) {
            esp_netconn_delete(nc);             /* Free memory for API */
        }
    }
    return espOK;
}

/**
 * \brief           Global event callback function
 * \param[in]       cb: Callback information and data
 * \return          \ref espOK on success, member of \ref espr_t otherwise
 */
static espr_t
esp_evt_func(esp_cb_t* cb) {
    switch (cb->type) {
        case ESP_CB_WIFI_DISCONNECTED: {        /* Wifi disconnected event */
            if (listen_api != NULL) {           /* Check if listen API active */
                esp_sys_mbox_putnow(&listen_api->mbox_accept, &recv_closed);
            }
            break;
        }
        case ESP_CB_DEVICE_PRESENT: {           /* Device present event */
            if (listen_api != NULL && !esp_device_is_present()) {   /* Check if device present */
                esp_sys_mbox_putnow(&listen_api->mbox_accept, &recv_not_present);
            }
        }
        default: break;
    }
    return espOK;
}

/**
 * \brief           Create new netconn connection
 * \param[in]       type: Type of netconn. This parameter can be a value of \ref esp_netconn_type_t enumeration
 * \return          New netconn connection
 */
esp_netconn_p
esp_netconn_new(esp_netconn_type_t type) {
    esp_netconn_t* a;
    
    esp_cb_register(esp_evt_func);              /* Register global event function */
    a = esp_mem_calloc(1, sizeof(*a));          /* Allocate memory for core object */
    if (a != NULL) {
        a->type = type;                         /* Save netconn type */
        if (!esp_sys_mbox_create(&a->mbox_accept, ESP_CFG_NETCONN_ACCEPT_QUEUE_LEN)) {  /* Allocate memory for accepting message box */
            ESP_DEBUGF(ESP_CFG_DBG_NETCONN | ESP_DBG_TYPE_TRACE | ESP_DBG_LVL_DANGER,
                "NETCONN: Cannot create accept MBOX\r\n");
            goto free_ret;
        }
        if (!esp_sys_mbox_create(&a->mbox_receive, ESP_CFG_NETCONN_RECEIVE_QUEUE_LEN)) {    /* Allocate memory for receiving message box */
            ESP_DEBUGF(ESP_CFG_DBG_NETCONN | ESP_DBG_TYPE_TRACE | ESP_DBG_LVL_DANGER,
                "NETCONN: Cannot create receive MBOX\r\n");
            goto free_ret;
        }
        if (netconn_list == NULL) {             /* Add new netconn to the existing list */
            netconn_list = a;
        } else {
            a->next = netconn_list;             /* Add it to beginning of the list */
            netconn_list = a;
        }
    }
    return a;
free_ret:
    if (esp_sys_mbox_isvalid(&a->mbox_accept)) {
        esp_sys_mbox_delete(&a->mbox_accept);
        esp_sys_mbox_invalid(&a->mbox_accept);
    }
    if (esp_sys_mbox_isvalid(&a->mbox_receive)) {
        esp_sys_mbox_delete(&a->mbox_receive);
        esp_sys_mbox_invalid(&a->mbox_receive);
    }
    if (a != NULL) {
        esp_mem_free(a);
        a = NULL;
    }
    return NULL;
}

/**
 * \brief           Delete netconn connection
 * \param[in]       nc: Netconn handle
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_delete(esp_netconn_p nc) {
    ESP_ASSERT("netconn != NULL", nc != NULL);  /* Assert input parameters */
    
    if (esp_sys_mbox_isvalid(&nc->mbox_accept)) {
        esp_sys_mbox_delete(&nc->mbox_accept);
        esp_sys_mbox_invalid(&nc->mbox_accept);
    }
    if (esp_sys_mbox_isvalid(&nc->mbox_receive)) {
        esp_sys_mbox_delete(&nc->mbox_receive);
        esp_sys_mbox_invalid(&nc->mbox_receive);
    }
    
    /* Remove netconn from linkedlist */
    if (netconn_list == nc) {
        netconn_list = netconn_list->next;      /* Remove first from linked list */
    } else if (netconn_list != NULL) {
        esp_netconn_p tmp, prev;
        /* Find element on the list */
        for (prev = netconn_list, tmp = netconn_list->next;
            tmp != NULL; prev = tmp, tmp = tmp->next) {
            if (nc == tmp) {
                prev->next = tmp->next;         /* Remove tmp from linked list */
                break;
            }
        }
    }

    esp_mem_free(nc);                           /* Free the memory */
    nc = NULL;
    return espOK;
}

/**
 * \brief           Connect to server as client
 * \param[in]       nc: Netconn handle
 * \param[in]       host: Pointer to host, such as domain name or IP address in string format
 * \param[in]       port: Target port to use
 * \return          espOK if successfully connected, member of \ref espr_t otherwise
 */
espr_t
esp_netconn_connect(esp_netconn_p nc, const char* host, esp_port_t port) {
    espr_t res;
    
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    ESP_ASSERT("host != NULL", host != NULL);   /* Assert input parameters */
    ESP_ASSERT("port > 0", port);               /* Assert input parameters */
    
    /*
     * Start a new connection as client and:
     *
     *  - Set current netconn structure as argument
     *  - Set netconn callback function for connection management
     *  - Start connection in blocking mode
     */
    res = esp_conn_start(NULL, (esp_conn_type_t)nc->type, host, port, nc, esp_cb, 1);
    return res;
}

/**
 * \brief           Bind a connection to specific port, can be only used for server connections
 * \param[in]       nc: Netconn handle
 * \param[in]       port: Port used to bind a connection to
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_bind(esp_netconn_p nc, esp_port_t port) {
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    
    /*
     * Enable server on port and set default netconn callback
     */
    return esp_set_server(1, port, ESP_CFG_MAX_CONNS, 100, esp_cb, 1);
}

/**
 * \brief           Listen on previously binded connection
 * \param[in]       nc: Netconn handle used to listen for new connections
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_listen(esp_netconn_p nc) {
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    ESP_ASSERT("nc->type must be TCP\r\n", nc->type == ESP_NETCONN_TYPE_TCP);   /* Assert input parameters */

    ESP_CORE_PROTECT();
    listen_api = nc;                            /* Set current main API in listening state */
    ESP_CORE_UNPROTECT();
    return espOK;
}

/**
 * \brief           Accept a new connection
 * \param[in]       nc: Netconn handle used as base connection to accept new clients
 * \param[out]      new_nc: Pointer to netconn handle to save new connection to
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_accept(esp_netconn_p nc, esp_netconn_p* new_nc) {
    esp_netconn_t* tmp;
    uint32_t time;
    
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    ESP_ASSERT("new_nc != NULL", new_nc != NULL);   /* Assert input parameters */
    ESP_ASSERT("nc->type must be TCP\r\n", nc->type == ESP_NETCONN_TYPE_TCP);   /* Assert input parameters */
    ESP_ASSERT("nc == listen_api\r\n", nc == listen_api);   /* Assert input parameters */
    
    *new_nc = NULL;
    time = esp_sys_mbox_get(&nc->mbox_accept, (void **)&tmp, 0);
    if (time == ESP_SYS_TIMEOUT) {
        return espERR;
    }
    if ((uint8_t *)tmp == (uint8_t *)&recv_closed) {
        ESP_CORE_PROTECT();
        listen_api = NULL;                      /* Disable listening at this point */
        ESP_CORE_UNPROTECT();
        return espERRWIFINOTCONNECTED;          /* Wifi disconnected */
    } else if ((uint8_t *)tmp == (uint8_t *)&recv_not_present) {
        ESP_CORE_PROTECT();
        listen_api = NULL;                      /* Disable listening at this point */
        ESP_CORE_UNPROTECT();
        return espERRNODEVICE;                  /* Device not present */
    }
    *new_nc = tmp;                              /* Set new pointer */
    return espOK;                               /* We have a new connection */
}

/**
 * \brief           Write data to connection output buffers
 * \note            This function may only be used on TCP or SSL connections
 * \param[in]       nc: Netconn handle used to write data to
 * \param[in]       data: Pointer to data to write
 * \param[in]       btw: Number of bytes to write
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_write(esp_netconn_p nc, const void* data, size_t btw) {
    size_t len, sent;
    const uint8_t* d = data;
    espr_t res;
    
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    ESP_ASSERT("nc->type must be TCP or SSL\r\n", nc->type == ESP_NETCONN_TYPE_TCP || nc->type == ESP_NETCONN_TYPE_SSL);    /* Assert input parameters */
    
    /*
     * Several steps are done in write process
     *
     * 1. Check if buffer is set and check if there is something to write to it.
     *    1. In case buffer will be full after copy, send it and free memory.
     * 2. Check how many bytes we can write directly without needed to copy
     * 3. Try to allocate a new buffer and copy remaining input data to it
     * 4. In case buffer allocation fails, send data directly (may affect on speed and effectivenes)
     */
    
    /*
     * Step 1
     */
    if (nc->buff != NULL) {                     /* Is there a write buffer ready to accept more data? */
        len = ESP_MIN(nc->buff_len - nc->buff_ptr, btw);    /* Get number of bytes we can write to buffer */
        if (len) {
            ESP_MEMCPY(&nc->buff[nc->buff_ptr], data, len); /* Copy memory to temporary write buffer */
            d += len;
            nc->buff_ptr += len;
            btw -= len;
        }
        
        /*
         * Step 1.1
         */
        if (nc->buff_ptr == nc->buff_len) {
            res = esp_conn_send(nc->conn, nc->buff, nc->buff_len, &sent, 1);
            
            esp_mem_free(nc->buff);             /* Free memory */
            nc->buff = NULL;                    /* Invalidate buffer */
            if (res != espOK) {
                return res;
            }
        } else {
            return espOK;                       /* Buffer is not yet full yet */
        }
    }
    
    /*
     * Step 2
     */
    if (btw >= ESP_CFG_CONN_MAX_DATA_LEN) {
        size_t rem;
        rem = btw % ESP_CFG_CONN_MAX_DATA_LEN;  /* Get remaining bytes for max data length */
        res = esp_conn_send(nc->conn, d, btw - rem, &sent, 1);  /* Write data directly */
        if (res != espOK) {
            return res;
        }
        d += sent;                              /* Advance in data pointer */
        btw -= sent;                            /* Decrease remaining data to send */
    }
    
    if (!btw) {                                 /* Sent everything? */
        return espOK;
    }
    
    /*
     * Step 3
     */
    if (nc->buff == NULL) {                     /* Check if we should allocate a new buffer */
        nc->buff = esp_mem_alloc(ESP_CFG_CONN_MAX_DATA_LEN * sizeof(*nc->buff));
        nc->buff_len = ESP_CFG_CONN_MAX_DATA_LEN;   /* Save buffer length */
        nc->buff_ptr = 0;                       /* Save buffer pointer */
    }
    
    /*
     * Step 4
     */
    if (nc->buff != NULL) {                     /* Memory available? */
        ESP_MEMCPY(&nc->buff[nc->buff_ptr], d, btw);/* Copy data to buffer */
        nc->buff_ptr += btw;
    } else {                                    /* Still no memory available? */
        return esp_conn_send(nc->conn, data, btw, NULL, 1); /* Simply send directly blocking */
    }
    return espOK;
}

/**
 * \brief           Flush buffered data on netconn TCP connection
 * \note            This function may only be used on TCP or SSL connections
 * \param[in]       nc: Netconn handle to flush data
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_flush(esp_netconn_p nc) {
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    ESP_ASSERT("nc->type must be TCP or SSL\r\n", nc->type == ESP_NETCONN_TYPE_TCP || nc->type == ESP_NETCONN_TYPE_SSL);    /* Assert input parameters */

    /*
     * In case we have data in write buffer,
     * flush them out to network
     */
    if (nc->buff != NULL) {                     /* Check remaining data */
        if (nc->buff_ptr > 0) {                 /* Do we have data in current buffer? */
            esp_conn_send(nc->conn, nc->buff, nc->buff_ptr, NULL, 1);   /* Send data */
        }
        esp_mem_free(nc->buff);                 /* Free memory */
        nc->buff = NULL;                        /* Invalid memory */
    }
    return espOK;
}

/**
 * \brief           Send packet to specific IP and port
 * \param[in]       nc: Netconn handle used to send
 * \param[in]       data: Pointer to data to write
 * \param[in]       btw: Number of bytes to write
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_send(esp_netconn_p nc, const void* data, size_t btw) {
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    ESP_ASSERT("nc->type must be UDP\r\n", nc->type == ESP_NETCONN_TYPE_UDP);   /* Assert input parameters */
    
    return esp_conn_send(nc->conn, data, btw, NULL, 1);
}

/**
 * \brief           Send packet to specific IP and port
 * \note            Use this function in case of UDP type netconn
 * \param[in]       nc: Netconn handle used to send
 * \param[in]       ip: Pointer to IP address
 * \param[in]       port: Port number used to send data
 * \param[in]       data: Pointer to data to write
 * \param[in]       btw: Number of bytes to write
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_sendto(esp_netconn_p nc, const esp_ip_t* ip, esp_port_t port, const void* data, size_t btw) {
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    ESP_ASSERT("nc->type must be UDP\r\n", nc->type == ESP_NETCONN_TYPE_UDP);   /* Assert input parameters */
    
    return esp_conn_sendto(nc->conn, ip, port, data, btw, NULL, 1);
}

/**
 * \brief           Receive data from connection
 * \param[in]       nc: Netconn handle used to receive from
 * \param[in]       pbuf: Pointer to pointer to save new receive buffer to
 * \return          \ref espOK when new data ready, \ref espCLOSED when connection closed by remote side,
 *                  \ref espTIMEOUT when receive timeout occurs or any other member of \ref espr_t otherwise
 */
espr_t
esp_netconn_receive(esp_netconn_p nc, esp_pbuf_p* pbuf) {    
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */
    ESP_ASSERT("pbuf != NULL", pbuf != NULL);   /* Assert input parameters */

    *pbuf = NULL;
#if ESP_CFG_NETCONN_RECEIVE_TIMEOUT
    /*
     * Wait for new received data for up to specific timeout
     * or throw error for timeout notification
     */
    if (esp_sys_mbox_get(&nc->mbox_receive, (void **)pbuf, nc->rcv_timeout) == ESP_SYS_TIMEOUT) {
        return espTIMEOUT;
    }
#else /* ESP_CFG_NETCONN_RECEIVE_TIMEOUT */
    /*
     * Forever wait for new receive packet
     */
    esp_sys_mbox_get(&nc->mbox_receive, (void **)pbuf, 0);
#endif /* !ESP_CFG_NETCONN_RECEIVE_TIMEOUT */

    /* Check if connection closed */
    if ((uint8_t *)(*pbuf) == (uint8_t *)&recv_closed) {
        return espCLOSED;
    }
    return espOK;                               /* We have data available */
}

/**
 * \brief           Close a netconn connection
 * \param[in]       nc: Netconn handle to close
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_netconn_close(esp_netconn_p nc) {
    ESP_ASSERT("nc != NULL", nc != NULL);       /* Assert input parameters */

    esp_netconn_flush(nc);                      /* Flush data and ignore result */
//    esp_conn_set_arg(nc->conn, NULL);           /* Reset argument */
    esp_conn_close(nc->conn, 1);                /* Close the connection */
    flush_mboxes(nc);                           /* Flush message queues */
    return espOK;
}

/**
 * \brief           Get connection number used for netconn
 * \param[in]       nc: Pointer to handle for connection
 * \return          -1 on failure, number otherwise
 */
int8_t
esp_netconn_getconnnum(esp_netconn_p nc) {
    if (nc != NULL && nc->conn != NULL) {
        return esp_conn_getnum(nc->conn);
    }
    return -1;
}

#if ESP_CFG_NETCONN_RECEIVE_TIMEOUT || __DOXYGEN__

/**
 * \brief           Set timeout value for receiving data
 * \param[in]       nc: Netconn handle
 * \param[in]       timeout: Timeout in units of milliseconds. Set to 0 to disable timeout (wait forever, default value)
 */
void
esp_netconn_set_receive_timeout(esp_netconn_p nc, uint32_t timeout) {
    nc->rcv_timeout = timeout;
}

/**
 * \brief           Get netconn receive timeout value
 * \param[in]       nc: Netconn handle
 * \return          Timeout in units of milliseconds. When returned value is 0, timeout is disabled (wait forever)
 */
uint32_t
esp_netconn_get_receive_timeout(esp_netconn_p nc) {
    return nc->rcv_timeout;                     /* Return receive timeout */
}

#endif /* ESP_CFG_NETCONN_RECEIVE_TIMEOUT || __DOXYGEN__ */

#endif /* ESP_CFG_NETCONN || __DOXYGEN__ */
