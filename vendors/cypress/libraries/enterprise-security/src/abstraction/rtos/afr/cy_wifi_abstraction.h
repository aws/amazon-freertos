/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  Prototypes of functions for controlling the Wi-Fi system in Amazon FreeRTOS
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "iot_wifi.h"

#include "cy_supplicant_core_constants.h"
#include "cy_enterprise_security_interface.h"

typedef enum
{
    WIFI_NOT_CONNECTED,
    WIFI_CONNECTED
} wifi_connection_status_t;


int connect_ent( const char *ssid, uint8_t ssid_length, const char *password, uint8_t password_length, cy_supplicant_core_security_t auth_type );
int disconnect_ent( void );
void wifi_on_ent( void );
wifi_connection_status_t is_wifi_connected( void );

