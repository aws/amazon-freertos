/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "cy_type_defs.h"
#include "cy_supplicant_core_constants.h"
#include "cy_supplicant_result.h"
#include "cy_supplicant_structures.h"
#include "cy_supplicant_host.h"
#include "cy_eap.h"
#include "cyabs_rtos.h"

/******************************************************
 *                  Constants
 ******************************************************/
#define SUPPLICANT_THREAD_STACK_SIZE         ( 4*1024 )
#define TLS_AGENT_THREAD_STACK_SIZE          ( 4*1024 )
#define SUPPLICANT_BUFFER_SIZE               ( 3500 )
#define SUPPLICANT_WORKSPACE_ARRAY_SIZE      ( 3 )
#define WLC_EVENT_MSG_LINK                   ( 0x01 )
#define EAPOL_PACKET_TIMEOUT                 ( 15000 ) /* Milliseconds */
#define SUPPLICANT_HANDSHAKE_ATTEMPT_TIMEOUT ( 30000 ) /* Milliseconds */
#define EAP_HANDSHAKE_TIMEOUT_IN_MSEC        ( 25000 ) /* Milliseconds */

/******************************************************
 *                 Macros
 ******************************************************/

/******************************************************
 *              Function Prototypes
 ******************************************************/

cy_rslt_t supplicant_start( supplicant_workspace_t* workspace );
cy_rslt_t supplicant_init_state(supplicant_workspace_t* workspace, eap_type_t eap_type );
void      supplicant_set_identity        ( supplicant_workspace_t* workspace, const uint8_t* eap_identity, uint32_t eap_identity_length );
void      supplicant_set_inner_identity  ( supplicant_workspace_t* workspace, eap_type_t eap_type, void* inner_identity );
cy_rslt_t supplicant_management_set_event_handler( supplicant_workspace_t* workspace, cy_bool_t enable );
cy_rslt_t supplicant_start( supplicant_workspace_t* workspace );
cy_rslt_t supplicant_stop( supplicant_workspace_t* workspace );
cy_rslt_t supplicant_init(supplicant_workspace_t* workspace, supplicant_connection_info_t *conn_info);
cy_rslt_t supplicant_deinit( supplicant_workspace_t* workspace );
cy_rslt_t supplicant_tls_calculate_overhead( supplicant_workspace_t* workspace, uint16_t available_space, uint16_t* header, uint16_t* footer );
cy_rslt_t supplicant_inner_packet_set_data( whd_driver_t whd_driver,supplicant_packet_t* packet, int32_t size );
cy_rslt_t supplicant_process_peap_event(supplicant_workspace_t* workspace, supplicant_packet_t packet);
cy_rslt_t supplicant_phase2_init( supplicant_workspace_t* workspace, eap_type_t type );
cy_rslt_t supplicant_phase2_start( supplicant_workspace_t* workspace );
cy_rslt_t supplicant_enable_tls( supplicant_workspace_t* supplicant, void* context );
void      supplicant_phase2_thread( cy_thread_arg_t arg );

#ifdef __cplusplus
} /*extern "C" */
#endif
