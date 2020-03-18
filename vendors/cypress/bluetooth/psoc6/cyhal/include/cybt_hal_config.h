#pragma once

#include "port_bluetooth_platform.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (HCILP_INCLUDED == TRUE)

#ifndef HCILP_HOST_WAKE_GPIO
#error PIN_HOST_WAKE must be defined !
#endif
#ifndef HCILP_BT_WAKE_GPIO
#error PIN_BT_WAKE must be defined !
#endif
#endif

typedef enum
{
    PATCHRAM_DOWNLOAD_MODE_NO_MINIDRV_CMD,
    PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
} cybt_patchram_download_mode_t;

typedef struct
{
    cybt_patchram_download_mode_t   download_mode;
    int                             download_baudrate;
    int                             in_use;
    int                             featured_baudrate;
} cybt_patchram_config_t;

typedef enum
{
    CYBT_HCI_TRANSPORT_TYPE_SERIAL,
    CYBT_HCI_TRANSPORT_TYPE_I2C,
    CYBT_HCI_TRANSPORT_TYPE_SPI,
    CYBT_HCI_TRANSPORT_TYPE_NONE,
} cybt_hci_transport_type_t;

typedef void* cybt_hci_transport_handle_t;

typedef union
{
    struct {
        int baudrate;
    } serial;

    struct {
        int port;
    } i2c;
} cybt_hci_transport_config_t;

typedef struct
{
    cybt_hci_transport_type_t           type;
    cybt_hci_transport_handle_t         handle;
    int                                 is_async;
    cybt_hci_transport_config_t         config;

    cybt_hci_transport_handle_t (*setup)(void);
    int (*teardown)(cybt_hci_transport_handle_t handle);
    int (*read_bytes)(cybt_hci_transport_handle_t handle, uint8_t* buffer, uint32_t* read_length, uint32_t timeout);
    int (*write_bytes)(cybt_hci_transport_handle_t handle, const uint8_t* data, uint32_t length);
    int (*is_ready)(cybt_hci_transport_handle_t);
} cybt_transport_config_t;

typedef struct
{
    cybt_patchram_config_t          patchram;
    cybt_transport_config_t         transport;
} cybt_hal_config_t;

void cybt_hal_get_bluetooth_config(cybt_hal_config_t** config);
void cybt_hal_get_bluetooth_transport_config(cybt_transport_config_t** config);
void cybt_hal_get_bluetooth_patchram_config(cybt_patchram_config_t** config);

#ifdef __cplusplus
} /*extern "C" */
#endif

