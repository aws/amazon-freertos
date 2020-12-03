/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * Generic types
 *
 */
#pragma once
#include "wiced_data_types.h"
#include "wiced_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup  bt_types  Wiced BT Types
 *  @ingroup     gentypes
 *
 *  WICED BT Types.
 *
 *  @{
 */

#define BT_PSM_SDP                      0x0001 /**< Service Discovery Protocol) */
#define BT_PSM_RFCOMM                   0x0003 /**< RFCOMM with TS 07.10 */
#define BT_PSM_TCS                      0x0005 /**< (deprecated) */
#define BT_PSM_CTP                      0x0007 /**< (deprecated) */
#define BT_PSM_BNEP                     0x000F /**< Bluetooth Network Encapsulation Protocol */
#define BT_PSM_HIDC                     0x0011 /**< Human Interface Device (Control) */
#define BT_PSM_HIDI                     0x0013 /**< Human Interface Device (Interrupt) */
#define BT_PSM_UPNP                     0x0015 /**< Universal PlugNPlay */
#define BT_PSM_AVCTP                    0x0017 /**< Audio/Video Control Transport Protocol */
#define BT_PSM_AVDTP                    0x0019 /**< Audio/Video Distribution Transport Protocol */
#define BT_PSM_AVCTP_13                 0x001B /**< Advanced Control - Browsing */
#define BT_PSM_UDI_CP                   0x001D /**< Unrestricted Digital Information C-Plane  */
#define BT_PSM_ATT                      0x001F /**< Attribute Protocol  */
#define BT_PSM_3DS                      0x0021 /**< 3D sync */
#define BT_LE_PSM_IPSP                  0x0023 /**< Internet Protocol Support (IPv6/6LoWPAN) */
#define BT_LE_PSM_EATT                  0x0027 /**< Enhanced Attribute protocol */


#define BT_EIR_FLAGS_TYPE                   0x01    /**< EIR Flag Type */
#define BT_EIR_MORE_16BITS_UUID_TYPE        0x02    /**< EIR 16 bits UUID Type */
#define BT_EIR_COMPLETE_16BITS_UUID_TYPE    0x03    /**< EIR Complete 16 bits UUID Type */
#define BT_EIR_MORE_32BITS_UUID_TYPE        0x04    /**< EIR 32 bits UUID Type */
#define BT_EIR_COMPLETE_32BITS_UUID_TYPE    0x05    /**< EIR Complete 32 bits UUID Type */
#define BT_EIR_MORE_128BITS_UUID_TYPE       0x06    /**< EIR 128 bits UUID Type */
#define BT_EIR_COMPLETE_128BITS_UUID_TYPE   0x07    /**< EIR 128 bits UUID Type */
#define BT_EIR_SHORTENED_LOCAL_NAME_TYPE    0x08    /**< EIR Shorthand Local Name Type */
#define BT_EIR_COMPLETE_LOCAL_NAME_TYPE     0x09    /**< EIR Complete Local Name Type */
#define BT_EIR_TX_POWER_LEVEL_TYPE          0x0A    /**< EIR TX Power Level Type */
#define BT_EIR_OOB_BD_ADDR_TYPE             0x0C    /**< EIR OOB BDA Type */
#define BT_EIR_OOB_COD_TYPE                 0x0D    /**< EIR OOB COD Type */
#define BT_EIR_OOB_SSP_HASH_C_TYPE          0x0E    /**< EIR OOB SSP HASH C Type */
#define BT_EIR_OOB_SSP_RAND_R_TYPE          0x0F    /**< EIR OOB SSP RAND R Type */
#define BT_EIR_OOB_SSP_HASH_C_256_TYPE      0x1D    /**< EIR OOB SSP HASH C256 Type */
#define BT_EIR_OOB_SSP_RAND_R_256_TYPE      0x1E    /**< EIR OOB SSP RAND R256 Type */
#define BT_EIR_3D_SYNC_TYPE                 0x3D    /**< EIR 3D SYNC Type */
#define BT_EIR_MANUFACTURER_SPECIFIC_TYPE   0xFF    /**< EIR Manufacturer Specific Type*/

#define BT_OOB_COD_SIZE            3    /**< OOB Class of Device Size */
#define BT_OOB_HASH_C_SIZE         16   /**< OOB Hash C size */
#define BT_OOB_RAND_R_SIZE         16   /**< OOB Rand R size */
#define BT_OOB_HASH_C_256_SIZE     16   /**< OOB Hash C-256 Size */
#define BT_OOB_RAND_R_256_SIZE     16   /**< OOB Rand R-256 Size */

#ifndef WICED_BT_DEVICE_ADDRESS
#define WICED_BT_DEVICE_ADDRESS /**< Device Bluetooth Address */
#define BD_ADDR_LEN     6       /**< Device Bluetooth Address Length */
/** */
typedef uint8_t         wiced_bt_device_address_t[BD_ADDR_LEN]; /**< Device address length */
#endif

/** Pointer to Device Address */
typedef uint8_t *BD_ADDR_PTR;

#define BD_NAME_LEN     248     /**< Local Name Length */

typedef uint8_t wiced_bt_remote_name_t[BD_NAME_LEN + 1];        /**< Device name */

typedef uint8_t *wiced_bt_device_address_ptr_t;                 /**< Device address Pointer */

#define DEV_CLASS_LEN   3  /**< Device class Length */
typedef uint8_t         wiced_bt_dev_class_t[DEV_CLASS_LEN];    /**< Device class */

#define MAX_UUID_SIZE              16  /**< Maximum UUID size - 16 bytes, and structure to hold any type of UUID. */

#define BT_DB_HASH_LEN 16   /**< Database Hash length  */
typedef uint8_t wiced_bt_db_hash_t[BT_DB_HASH_LEN]; /**< BLE database hash */

#define WICED_BT_GATT_CLIENT_SUPPORTED_FEATURE_OCTET_LEN 1  /**< GATT Client Supported feature length */
/** GATT Client Support features */
typedef uint8_t wiced_bt_gatt_db_client_supported_feature_t[WICED_BT_GATT_CLIENT_SUPPORTED_FEATURE_OCTET_LEN];

/** UUID Type */
typedef struct
{
#define LEN_UUID_16     2   /**< 2 Byte UUID */
#define LEN_UUID_32     4   /**< 4 Byte UUID */
#define LEN_UUID_128    16  /**< 16 Byte UUID */

    uint16_t        len;     /**< UUID length */

    /** UUID Data */
    union
    {
        uint16_t    uuid16; /**< 16-bit UUID */
        uint32_t    uuid32; /**< 32-bit UUID */
        uint8_t     uuid128[MAX_UUID_SIZE]; /**< 128-bit UUID */
    } uu;

} wiced_bt_uuid_t;

#define BT_OCTET8_LEN    8      /**< octet length: 8 */
typedef uint8_t BT_OCTET8[BT_OCTET8_LEN];   /**< octet array: size 8 */

#define BT_OCTET16_LEN    16                /**< octet length: 16 */
typedef uint8_t BT_OCTET16[BT_OCTET16_LEN]; /**< octet array: size 16 */

#define BT_OCTET32_LEN    32    /**< octet length: 32 */
typedef uint8_t BT_OCTET32[BT_OCTET32_LEN];   /**< octet array: size 32 */

/** Bluetooth QoS defintions */
typedef struct {
    uint8_t         qos_flags;              /**< Quality of service flag */
    uint8_t         service_type;           /**< service type (NO_TRAFFIC, BEST_EFFORT, or GUARANTEED) */
    uint32_t        token_rate;             /**< token rate (bytes/second) */
    uint32_t        token_bucket_size;      /**< token bucket size (bytes) */
    uint32_t        peak_bandwidth;         /**< peak bandwidth (bytes/second) */
    uint32_t        latency;                /**< latency (microseconds) */
    uint32_t        delay_variation;        /**< delay variation (microseconds) */
} wiced_bt_flow_spec_t;

/* Values for swiced_bt_flow_spec_t service_type */
#define NO_TRAFFIC      0   /**< No Traffic */
#define BEST_EFFORT     1   /**< Best Effort */
#define GUARANTEED      2   /**< Guaranteed */

/**
 * @anchor WICED_BT_TRANSPORT_TYPE
 * @name Transport types
 * @{
 */
#define BT_TRANSPORT_BR_EDR         1       /**< BR/EDR transport */
#define BT_TRANSPORT_LE             2       /**< BLE transport */
typedef uint8_t wiced_bt_transport_t;       /**< Transport type (see @ref WICED_BT_TRANSPORT_TYPE "BT Transport Types") */
/** @} WICED_BT_TRANSPORT_TYPE */

/**
 * @anchor WICED_BT_DEVICE_TYPE
 * @name Device Types
 * @{
 */
#define BT_DEVICE_TYPE_BREDR        0x01    /**< BR/EDR device */
#define BT_DEVICE_TYPE_BLE          0x02    /**< LE device */
#define BT_DEVICE_TYPE_BREDR_BLE    0x03    /**< Dual Mode device */
typedef uint8_t wiced_bt_device_type_t;     /**< Bluetooth device type (see @ref WICED_BT_DEVICE_TYPE "BT Device Types") */
/** @} WICED_BT_DEVICE_TYPE */

/**
 * @anchor WICED_BT_ADDR_TYPE
 * @name Address Types
 * @{
 */
#define BLE_ADDR_PUBLIC             0x00        /**< Public address */
#define BLE_ADDR_RANDOM             0x01        /**< Random address */
#define BLE_ADDR_PUBLIC_ID          0x02        /**< Public ID      */
#define BLE_ADDR_RANDOM_ID          0x03        /**< Random ID      */

/** BLE device address type (see @ref WICED_BT_ADDR_TYPE "BT Address Types") */
typedef uint8_t wiced_bt_ble_address_type_t;

/** @} WICED_BT_ADDR_TYPE */

/** Wiced Ble Address structure */
typedef struct
{
    wiced_bt_ble_address_type_t type;   /**< BLE Address Type */
    wiced_bt_device_address_t   bda;    /**< BLE Address */
} wiced_bt_ble_address_t;

#define LINK_KEY_LEN    16      /**< Link Key Len */
typedef uint8_t wiced_bt_link_key_t[LINK_KEY_LEN];  /**< Link Key */

/** Define a Data Received Buffer structure, which is used to pass received data back
** up through the stack.
*/
typedef struct
{
    uint16_t            drb_data_len;               /**< The amount of data in the DRB              */
    uint16_t            drb_data_offset;            /**< The starting offset of the data in the DRB */
    uint8_t             drb_data[1];                /**< The actual received data                   */
} tDRB;
#define DRB_OVERHEAD_SIZE   (sizeof (uint16_t) + sizeof (uint16_t))     /**< length and offset */

#define EATT_CHANNELS_PER_TRANSACTION 5 /**< EATT Max Channel per TX */


/********************************************************************************
** Macros to get and put bytes to and from a stream (Little Endian format).
*/
/** Covert UINT128 to Stream array */
#define UINT128_TO_STREAM(p, u128) {*(p)++ = (uint8_t)(u128);       *(p)++ = (uint8_t)((u128) >> 8);*(p)++ = (uint8_t)((u128) >> 16); *(p)++ = (uint8_t)((u128) >> 24); \
                                    *(p)++ = (uint8_t)((u128) >> 32); *(p)++ = (uint8_t)((u128) >> 40);*(p)++ = (uint8_t)((u128) >> 48); *(p)++ = (uint8_t)((u128) >> 56); \
                                    *(p)++ = (uint8_t)((u128) >> 64); *(p)++ = (uint8_t)((u128) >> 72);*(p)++ = (uint8_t)((u128) >> 80); *(p)++ = (uint8_t)((u128) >> 88); \
                                    *(p)++ = (uint8_t)((u128) >> 96); *(p)++ = (uint8_t)((u128) >> 104);*(p)++ = (uint8_t)((u128) >> 112); *(p)++ = (uint8_t)((u128) >> 120); }
/** Covert UINT64 to Stream array */
#define UINT64_TO_STREAM(p, u64) {*(p)++ = (uint8_t)(u64);       *(p)++ = (uint8_t)((u64) >> 8);*(p)++ = (uint8_t)((u64) >> 16); *(p)++ = (uint8_t)((u64) >> 24); \
                                    *(p)++ = (uint8_t)((u64) >> 32); *(p)++ = (uint8_t)((u64) >> 40);*(p)++ = (uint8_t)((u64) >> 48); *(p)++ = (uint8_t)((u64) >> 56);}
/** Covert UINT48 to Stream array */
#define UINT48_TO_STREAM(p, u48) {*(p)++ = (uint8_t)(u48); *(p)++ = (uint8_t)((u48) >> 8); *(p)++ = (uint8_t)((u48) >> 16); *(p)++ = (uint8_t)((u48) >> 24); *(p)++ = (uint8_t)((u48) >> 32); *(p)++ = (uint8_t)((u48) >> 40);}
/** Covert UINT40 to Stream array */
#define UINT40_TO_STREAM(p, u40) {*(p)++ = (uint8_t)(u40); *(p)++ = (uint8_t)((u40) >> 8); *(p)++ = (uint8_t)((u40) >> 16); *(p)++ = (uint8_t)((u40) >> 24); \
                                  *(p)++ = (uint8_t)((u40) >> 32);}
/** Covert UINT24 to Stream array */
#define UINT24_TO_STREAM(p, u24) {*(p)++ = (uint8_t)(u24); *(p)++ = (uint8_t)((u24) >> 8); *(p)++ = (uint8_t)((u24) >> 16);}
/** Covert UINT8 to Stream array */
#define UINT8_TO_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
/** Covert INT8 to Stream array */
#define INT8_TO_STREAM(p, u8)    {*(p)++ = (INT8)(u8);}
/** Covert 32 bytes to Stream array */
#define ARRAY32_TO_STREAM(p, a)  {register int ijk; for (ijk = 0; ijk < 32;           ijk++) *(p)++ = (uint8_t) a[31 - ijk];}
/** Covert 16 bytes to Stream array */
#define ARRAY16_TO_STREAM(p, a)  {register int ijk; for (ijk = 0; ijk < 16;           ijk++) *(p)++ = (uint8_t) a[15 - ijk];}
/** Covert 8 bytes to Stream array */
#define ARRAY8_TO_STREAM(p, a)   {register int ijk; for (ijk = 0; ijk < 8;            ijk++) *(p)++ = (uint8_t) a[7 - ijk];}
/** Covert LAP to Stream array */
#define LAP_TO_STREAM(p, a)      {register int ijk; for (ijk = 0; ijk < LAP_LEN;      ijk++) *(p)++ = (uint8_t) a[LAP_LEN - 1 - ijk];}
/** Covert Device class to Stream array */
#define DEVCLASS_TO_STREAM(p, a) {register int ijk; for (ijk = 0; ijk < DEV_CLASS_LEN;ijk++) *(p)++ = (uint8_t) a[DEV_CLASS_LEN - 1 - ijk];}
/** Covert array to Stream array */
#define ARRAY_TO_STREAM(p, a, len) {register int ijk; for (ijk = 0; ijk < len;        ijk++) *(p)++ = (uint8_t) a[ijk];}
/** Reverse Array */
#define REVERSE_ARRAY_TO_STREAM(p, a, len)  {register int ijk; for (ijk = 0; ijk < len; ijk++) *(p)++ = (uint8_t) a[len - 1 - ijk];}
/** Convert byte stream to UINT24 */
#define STREAM_TO_UINT24(u32, p) {u32 = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) ); (p) += 3;}
/** Convert byte stream to UINT40 */
#define STREAM_TO_UINT40(u40, p) {u40 = (((uint64_t)(*(p))) + ((((uint64_t)(*((p) + 1)))) << 8) + ((((uint64_t)(*((p) + 2)))) << 16) + ((((uint64_t)(*((p) + 3)))) << 24) + ((((uint64_t)(*((p) + 4)))) << 32)); (p) += 5;}
/** Convert byte stream to ARRAY32 */
#define STREAM_TO_ARRAY32(a, p)  {register int ijk; register uint8_t *_pa = (uint8_t *)a + 31; for (ijk = 0; ijk < 32; ijk++) *_pa-- = *p++;}
/** Convert byte stream to ARRAY16 */
#define STREAM_TO_ARRAY16(a, p)  {register int ijk; register uint8_t *_pa = (uint8_t *)a + 15; for (ijk = 0; ijk < 16; ijk++) *_pa-- = *p++;}
/** Convert byte stream to ARRAY8 */
#define STREAM_TO_ARRAY8(a, p)   {register int ijk; register uint8_t *_pa = (uint8_t *)a + 7; for (ijk = 0; ijk < 8; ijk++) *_pa-- = *p++;}
/** Convert byte stream to Device Class */
#define STREAM_TO_DEVCLASS(a, p) {register int ijk; register uint8_t *_pa = (uint8_t *)a + DEV_CLASS_LEN - 1; for (ijk = 0; ijk < DEV_CLASS_LEN; ijk++) *_pa-- = *p++;}
/** Convert byte stream to LAP */
#define STREAM_TO_LAP(a, p)      {register int ijk; register uint8_t *plap = (uint8_t *)a + LAP_LEN - 1; for (ijk = 0; ijk < LAP_LEN; ijk++) *plap-- = *p++;}
/** Convert byte stream to Array */
#define STREAM_TO_ARRAY(a, p, len) {register int ijk; for (ijk = 0; ijk < len; ijk++) ((uint8_t *) a)[ijk] = *p++;}
/** Reverse Stream to Array */
#define REVERSE_STREAM_TO_ARRAY(a, p, len) {register int ijk; register uint8_t *_pa = (uint8_t *)a + len - 1; for (ijk = 0; ijk < len; ijk++) *_pa-- = *p++;}

/** Convert byte stream to UINT8 */
#define STREAM_TO_UINT8(u8, p)   {u8 = (uint8_t)(*(p)); (p) += 1;}
/** Convert byte stream to INT8 */
#define STREAM_TO_INT8(i8, p)   {i8 = (INT8)(*(p)); (p) += 1;}
/** String copy */
#define BCM_STRNCPY_S(x1,x2,x3,x4)  strncpy((x1),(x3),(x4))

/* Based on the BT Controller ARM architecture, or possibly other hosts, we can optimize these macros. */
#if ((defined STACK_INSIDE_BT_CTRLR) && (STACK_INSIDE_BT_CTRLR == TRUE)) || (defined OPTIMISE_FOR_LITTLE_ENDIAN)
/** Covert UINT32 to Stream */
#define UINT32_TO_STREAM(p, u32) {*(uint32_t *)(p) = u32; (p) += 4;}
/** Covert UINT16 to Stream */
#define UINT16_TO_STREAM(p, u16) {*(uint16_t *)(p) = u16; (p) += 2;}
/** Covert Stream to UINT32 */
#define STREAM_TO_UINT32(u32, p) {u32 = *(uint32_t *)(p); (p) += 4;}
/** Covert Stream to UINT16 */
#define STREAM_TO_UINT16(u16, p) {u16 = *(uint16_t *)(p); (p) += 2;}
#else
/** Covert UINT32 to Stream */
#define UINT32_TO_STREAM(p, u32) {*(p)++ = (uint8_t)(u32); *(p)++ = (uint8_t)((u32) >> 8); *(p)++ = (uint8_t)((u32) >> 16); *(p)++ = (uint8_t)((u32) >> 24);}
/** Covert UINT16 to Stream */
#define UINT16_TO_STREAM(p, u16) {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}
/** Covert Stream to UINT32 */
#define STREAM_TO_UINT32(u32, p) {u32 = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) + ((((uint32_t)(*((p) + 3)))) << 24)); (p) += 4;}
/** Covert Stream to UINT16 */
#define STREAM_TO_UINT16(u16, p) {u16 = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); (p) += 2;}
#endif
/** Copy BDA to stream */
extern uint8_t *BTU_copyBdaToStream(uint8_t *pStream, uint8_t *pBDA);
/** Copy Stream to BDA */
extern uint8_t *BTU_copyStreamToBda(uint8_t *pBDA, uint8_t *pStream);

/** Convert BDA to stream */
#define BDADDR_TO_STREAM(p, a)   { p = BTU_copyBdaToStream(p, (uint8_t *)(a)); }
/** Convert stream to BDA */
#define STREAM_TO_BDADDR(a, p)   { p = BTU_copyStreamToBda((uint8_t *)(a), p);}


/********************************************************************************
** Macros to get and put bytes to and from a field (Little Endian format).
** These are the same as to stream, except the pointer is not incremented.
*/
/** Covert UINT32 to Field*/
#define UINT32_TO_FIELD(p, u32) {*(uint8_t *)(p) = (uint8_t)(u32); *((uint8_t *)(p)+1) = (uint8_t)((u32) >> 8); *((uint8_t *)(p)+2) = (uint8_t)((u32) >> 16); *((uint8_t *)(p)+3) = (uint8_t)((u32) >> 24);}
/** Covert UINT24 to Field*/
#define UINT24_TO_FIELD(p, u24) {*(uint8_t *)(p) = (uint8_t)(u24); *((uint8_t *)(p)+1) = (uint8_t)((u24) >> 8); *((uint8_t *)(p)+2) = (uint8_t)((u24) >> 16);}
/** Covert UINT16 to Field*/
#define UINT16_TO_FIELD(p, u16) {*(uint8_t *)(p) = (uint8_t)(u16); *((uint8_t *)(p)+1) = (uint8_t)((u16) >> 8);}
/** Covert UINT8 to Field*/
#define UINT8_TO_FIELD(p, u8)   {*(uint8_t *)(p) = (uint8_t)(u8);}


/********************************************************************************
** Macros to get and put bytes to and from a stream (Big Endian format)
*/
/** Covert UINT128 to Stream */
#define UINT128_TO_BE_STREAM(p, u128) {*(p)++ = (uint8_t)(u128)>> 120;       *(p)++ = (uint8_t)((u128) >> 112);*(p)++ = (uint8_t)((u128) >> 104); *(p)++ = (uint8_t)((u128) >> 96); \
                                    *(p)++ = (uint8_t)((u128) >> 88); *(p)++ = (uint8_t)((u128) >> 80);*(p)++ = (uint8_t)((u128) >> 72); *(p)++ = (uint8_t)((u128) >> 64); \
                                    *(p)++ = (uint8_t)((u128) >> 56); *(p)++ = (uint8_t)((u128) >> 48);*(p)++ = (uint8_t)((u128) >> 40); *(p)++ = (uint8_t)((u128) >> 32); \
                                    *(p)++ = (uint8_t)((u128) >> 24); *(p)++ = (uint8_t)((u128) >> 16);*(p)++ = (uint8_t)((u128) >> 8); *(p)++ = (uint8_t)(u128); }
/** Covert UINT64 to Stream */
#define UINT64_TO_BE_STREAM(p, u64) {*(p)++ = (uint8_t)((u64) >> 56); *(p)++ = (uint8_t)((u64) >> 48);*(p)++ = (uint8_t)((u64) >> 40); *(p)++ = (uint8_t)((u64) >> 32); \
                                     *(p)++ = (uint8_t)((u64) >> 24); *(p)++ = (uint8_t)((u64) >> 16);*(p)++ = (uint8_t)((u64) >> 8); *(p)++ = (uint8_t)(u64); }
/** Covert UINT32 to Stream */
#define UINT32_TO_BE_STREAM(p, u32) {*(p)++ = (uint8_t)((u32) >> 24);  *(p)++ = (uint8_t)((u32) >> 16); *(p)++ = (uint8_t)((u32) >> 8); *(p)++ = (uint8_t)(u32); }
/** Covert UINT24 to Stream */
#define UINT24_TO_BE_STREAM(p, u24) {*(p)++ = (uint8_t)((u24) >> 16); *(p)++ = (uint8_t)((u24) >> 8); *(p)++ = (uint8_t)(u24);}
/** Covert UINT16 to Stream */
#define UINT16_TO_BE_STREAM(p, u16) {*(p)++ = (uint8_t)((u16) >> 8); *(p)++ = (uint8_t)(u16);}
/** Covert UINT8 to Stream */
#define UINT8_TO_BE_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
/** Covert Array to Stream */
#define ARRAY_TO_BE_STREAM(p, a, len) {register int ijk; for (ijk = 0; ijk < len; ijk++) *(p)++ = (uint8_t) a[ijk];}

/** Stream to UINT8 */
#define BE_STREAM_TO_UINT8(u8, p)   {u8 = (uint8_t)(*(p)); (p) += 1;}
/** Stream to UINT16 */
#define BE_STREAM_TO_UINT16(u16, p) {u16 = (uint16_t)(((uint16_t)(*(p)) << 8) + (uint16_t)(*((p) + 1))); (p) += 2;}
/** Stream to UINT24 */
#define BE_STREAM_TO_UINT24(u32, p) {u32 = (((uint32_t)(*((p) + 2))) + ((uint32_t)(*((p) + 1)) << 8) + ((uint32_t)(*(p)) << 16)); (p) += 3;}
/** Stream to UINT32 */
#define BE_STREAM_TO_UINT32(u32, p) {u32 = ((uint32_t)(*((p) + 3)) + ((uint32_t)(*((p) + 2)) << 8) + ((uint32_t)(*((p) + 1)) << 16) + ((uint32_t)(*(p)) << 24)); (p) += 4;}
/** Stream to UINT64 */
#define BE_STREAM_TO_UINT64(u64, p) {u64 = ((UINT64)(*((p) + 7)) + ((UINT64)(*((p) + 6)) << 8) + ((UINT64)(*((p) + 5)) << 16) + ((UINT64)(*((p) + 4)) << 24) + \
                                           ((UINT64)(*((p) + 3)) << 32) + ((UINT64)(*((p) + 2)) << 40) + ((UINT64)(*((p) + 1)) << 48) + ((UINT64)(*(p)) << 56)); (p) += 8;}
/** Covert Array to Stream*/
#define BE_STREAM_TO_ARRAY(p, a, len) {register int ijk; for (ijk = 0; ijk < len; ijk++) ((uint8_t *) a)[ijk] = *p++;}


/********************************************************************************
** Macros to get and put bytes to and from a field (Big Endian format).
** These are the same as to stream, except the pointer is not incremented.
*/
/** UINT32 to Field */
#define UINT32_TO_BE_FIELD(p, u32) {*(uint8_t *)(p) = (uint8_t)((u32) >> 24);  *((uint8_t *)(p)+1) = (uint8_t)((u32) >> 16); *((uint8_t *)(p)+2) = (uint8_t)((u32) >> 8); *((uint8_t *)(p)+3) = (uint8_t)(u32); }
/** UINT24 to Field */
#define UINT24_TO_BE_FIELD(p, u24) {*(uint8_t *)(p) = (uint8_t)((u24) >> 16); *((uint8_t *)(p)+1) = (uint8_t)((u24) >> 8); *((uint8_t *)(p)+2) = (uint8_t)(u24);}
/** UINT16 to Field */
#define UINT16_TO_BE_FIELD(p, u16) {*(uint8_t *)(p) = (uint8_t)((u16) >> 8); *((uint8_t *)(p)+1) = (uint8_t)(u16);}
/** UINT8 to Field */
#define UINT8_TO_BE_FIELD(p, u8)   {*(uint8_t *)(p) = (uint8_t)(u8);}

/* Macros to Print the BD_ADDRESS */
/** Expand BDA */
#define EXPAND_BDADDR(bd)                 bd[0],bd[1],bd[2],bd[3],bd[4],bd[5]
/** Format BDA */
#define BDADDR_FORMAT                     "%02x:%02x:%02x:%02x:%02x:%02x"
/** Number of Elements */
#define NUM_ELEMENTS(x)                   (sizeof(x) / sizeof((x)[0]))

#ifndef MIN
/** Min */
#define MIN( a, b )                                             ( ( (a) < (b) ) ? (a) : (b) )
#endif

#ifndef MAX
/** Max */
#define MAX( a, b )                                             ( ( (a) > (b) ) ? (a) : (b) )
#endif

#if defined(_WIN32) || defined(_WIN64)
/** Declare 4 byte align for Windows */
    #define WICED_DECLARE_ALIGNED4(datatype, var) __declspec(align(4)) datatype var
#elif defined(__ICCARM__)
/** Declare 4 byte align for IAR */
    #define WICED_DECLARE_ALIGNED4(datatype, var) _Pragma("data_alignment=4") datatype var
#else
/** Declare 4 byte align for GCC */
    #define WICED_DECLARE_ALIGNED4(datatype, var) datatype var __attribute__((aligned(4)))
#endif

/** Wiced BT Trace Type */
typedef enum {
    WICED_BT_TRACE_DEBUG,
    WICED_BT_TRACE_ERROR,
    WICED_BT_TRACE_WARN,
    WICED_BT_TRACE_API,
    WICED_BT_TRACE_EVENT,
}wiced_bt_trace_type_t;

/**@} bt_types */

#ifdef __cplusplus
}
#endif
