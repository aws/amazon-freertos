/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
 *
 * Runtime Bluetooth configuration parameters
 *
 */
 /**
 * @addtogroup  wiced_bt_platform_group Bluetooth Stack Platform Interface
 *
 * Interface between Stack and platform.
 *
 * @{
 */
 
#pragma once

#include <stdarg.h>
#include "wiced_bt_types.h"
#include "wiced_data_types.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_cfg.h"

/** Wiced BT Stack Platform */
typedef struct
{
    /**
     * Exception callback
     */
    pf_wiced_exception pf_exception;

    /**
     * Platform function to allocate memory
     *
     * Called by stack code to allocate memory from the OS/Platform. 
     * Implementing function is expected to return memory allocated from 
     * the OS/Platform
     *
     * @param[in] size    : Size of memory to be allocated
     *
     * @return : Pointer to allocated memory
     */
    void * (*pf_os_malloc)(uint32_t size);

    /**
     * Platform memory free
     *
     * Called by stack code to free memory back to the OS/Platform.
     * Implementing function is expected to free the memory allocated 
     * using pf_os_malloc (refer #pf_os_malloc ) call from the OS/Platform
     *
     * @param[in] p_mem    : Ptr to memory to be freed
     *
     * @return : None
     */
    void   (*pf_os_free)(void* p_mem);

    /**
     * Platform function to get tick count
     *
     * Called by stack timer code to get the free running 64 bit tick count
     *
     * @param[in] None
     *
     * @return : 64 bit current tick count
     */
    uint64_t (*pf_get_tick_count_64)(void);

    /**
     * Platform function to set the next timeout
     *
     * Called by stack timer code set the next timeout 
     *
     * @param[in] abs_tick_count : 64 bit tick count instant at which the timeout has to occur
     *
     * @return : void
     */
    void   (*pf_set_next_timeout)(uint64_t  abs_tick_count);

    /** Stack lock */
    wiced_bt_lock_t stack_lock;

    /**
     * Platform function to get ACL buffer to send to lower
     *
     * Called by stack to get a buffer to fill in the data to be sent to 'transport' (BLE or BR/EDR)
     * of 'size'
     *
     * @param[in] transport : Transport on which the buffer is to be sent
     * @param[in] size      : Size of the buffer
     *
     * @return : Pointer to buffer which will be filled with data
     */
    uint8_t       *(*pf_get_acl_to_lower_buffer)(wiced_bt_transport_t transport, uint32_t size);

    /**
     * Platform function to write ACL buffer to lower
     *
     * Called by stack to send the buffer allocated using pf_get_acl_to_lower_buffer 
     * after filling it with the data to send.
     *
     * @param[in] transport : Transport on which the buffer is to be sent
     * @param[in] p_data    : Pointer received using pf_get_acl_to_lower_buffer
     * @param[in] len       : Length of data at p_data
     *
     * @return : wiced_result_t
     */
    wiced_result_t (*pf_write_acl_to_lower)(wiced_bt_transport_t transport, uint8_t* p_data, uint16_t len);

    /**
     * Platform function to write CMD buffer to lower
     *
     * Called by stack to send HCI CMD buffer to lower
     *
     * @param[in] p_cmd   : Pointer to HCI CMD data
     * @param[in] cmd_len : Length of data at p_cmd
     *
     * @return : wiced_result_t
     */
    wiced_result_t (*pf_write_cmd_to_lower)(uint8_t * p_cmd, uint16_t cmd_len);

    /**
     * Platform function to write SCO buffer to lower
     *
     * Called to send SCO CMD buffer to lower
     *
     * @param[in] handle   : HCI SCO handle
     * @param[in] p_data   : Pointer to SCO data
     * @param[in] len      : Length of data at p_data
     *
     * @return : wiced_result_t
     */
    wiced_result_t (*pf_write_sco_to_lower)(uint16_t handle, uint8_t* p_data, uint8_t len);

    /**
     * Callback function to trace HCI messages
     *
     * Called by stack to allow application to trace the HCI messages. 
     * Application/Porting code is expected to treat received data as read only, and make a
     * copy of the data to reference it outside of the callback
     * 
     * @param[in] type   : HCI event data type 
     * @param[in] len    : Length of data at p_data
     * @param[in] p_data : Pointer to data
     *
     * @return : void
     */
    void (*pf_hci_trace_cback_t)(wiced_bt_hci_trace_type_t type, uint16_t len, uint8_t* p_data);

    /**
     * Callback function to dump out trace messages
     * This interface function can be NULL if no debug tracing is supported 
     *
     * Called by stack to allow application to write debug trace messages
     *
     * @param[in] p_trace_buf   : Pointer to the trace buffer
     * @param[in] trace_buf_len : Length of the trace buffer
     * @param[in] trace_type    : Type of trace message
     *
     * @return : void
     */
    void (*pf_debug_trace)(char *p_trace_buf, int trace_buf_len, wiced_bt_trace_type_t trace_type);

    /** trace_buffer_len : Trace buffer len */
    int   trace_buffer_len;
    /**
     * trace_buffer     : Pointer to the trace buffer
     * Applications can set this to NULL to disable traces
     */
    char* trace_buffer;


    /** 
     * Used for additional controller initialization by the porting layer to be performed
     * after the HCI reset. Can be set to NULL if no additional initialization required
     */
    void (*pf_patch_download)(void);
} wiced_bt_stack_platform_t;

/**
 * Initialize the platform interfaces, by providing porting functions specific to 
 * the underlying platform.
 *
 * @return   <b> WICED_BT_SUCCESS </b> : on success; \n
 *           <b> WICED_BT_ERROR  </b>  : if an error occurred
 */
extern wiced_result_t wiced_bt_stack_platform_initialize(wiced_bt_stack_platform_t * platform_interfaces);

/**
 * Called by the porting layer to process the incoming ACL data received from the 
 * remote bluetooth device
 *
 * @param[in] pData  : Pointer to the ACL data to be processed
 * @param[in] length : Length of the ACL data buffer
 *
 * @return    void 
 */
extern void wiced_bt_process_acl_data(uint8_t* pData, uint32_t length);

/**
 * Called by the porting layer to process the incoming HCI events from the local
 * bluetooth controller
 *
 * @param[in] pData  : Pointer to the HCI Events to be processed
 * @param[in] length : Length of the event buffer
 * @return    void
 */
extern void wiced_bt_process_hci_events(uint8_t* pData, uint32_t length);

/**
 * Called by the porting layer on expiry of the timer to process pending timers
 *
 * @return    void
 */
extern void wiced_bt_process_timer(void);


/**
 * Called by the lower layer transport driver to restart sending ACL data to the controller
 * Note: Porting layer API. 
 *     This API is expected to be invoked by the lower layer transport driver, to restart 
 *     transfers from the stack to the controller. 
 *     The lower tx layer is expected to have space for atleast one complete ACL buffer
 *     Typically used in cases where the lower Tx has lesser number of buffers than allowed by controller
 */
extern void wiced_bt_stack_indicate_lower_tx_complete(void);

/**
 * Called by the porting layer to complete/continue the reset process
 * Typically called after downloading firmware patches to the controller
 *
 * @return    void
 */
extern void wiced_bt_continue_reset(void);

/**
* Set the stack config. Invoked by the porting layer 
*
* @param[in] p_bt_new_cfg_settings : Stack configuration settings
*
* @return   0 if there is any error in the configuration otherwise the dynamic
*           memory size requirements of the stack for the configuration.
*            
*
*/
extern uint32_t wiced_bt_set_stack_config(const wiced_bt_cfg_settings_t* p_bt_new_cfg_settings);

/**
* Function prototype for the post Stack Init Callback.
*/
typedef void (*wiced_bt_internal_post_stack_init_cb)(void);


/**
* Function prototype for the HCI event monitor function that the application may suppply.
* The application MUST return TRUE if the it handled the event and does not want the stack to
* process the event. If the application returns FALSE, the stack will process the event.
*/
typedef wiced_bool_t (*wiced_bt_internal_stack_evt_handler_cb)(uint8_t* p_event);

/**
* Internal stack init 
*
* @param[in] mgmt_cback : Application BT Management callback
* @param[in] post_stack_cb : Internal post stack init callback
* @param[in] evt_handler_cb : Internal stack event handler
*
* @return    Dynamic memory size requirements of the stack for the configuration
*
*/
void wiced_bt_stack_init_internal(wiced_bt_management_cback_t mgmt_cback,
    wiced_bt_internal_post_stack_init_cb post_stack_cb,
    wiced_bt_internal_stack_evt_handler_cb evt_handler_cb);

/**
 * This function blocks until all de-initialisation procedures are complete.
 * It is recommended that the application disconnect any outstanding connections prior to invoking this function.
 *
 * @return    None
 */
void wiced_bt_stack_shutdown(void);
/**@} wiced_bt_platform_group */


