/*
 * $ Copyright Cypress Semiconductor $
 */

/**
 * @file wiced_bt_dynamic_gattdb.c
 * @brief WICED API's to modify GATT database dynamically.
 */

/*
 * Context :
 * AFR API's are designed such that GATT database can be modified at run time and WICED API's are designed
 * to work with a compile time GATT database. Hence to support AFR API's this layer is required to
 * modify the existing database to add/remove db entries runtime.
 *
 * AFR API's that interact with GATT Db are,
 *
 *     -> pxAddService
 *     -> pxDeleteService
 *     -> pxAddIncludedService
 *     -> pxAddCharacteristic
 *     -> pxAddDescriptor
 *     -> pxStartService
 *     -> pxStopService (not mandatory to implement)
 *
 */

#include "wiced_bt_types.h"
#include "wiced_result.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_gatt.h"
#include "string.h"
#include "wiced_bt_dynamic_gattdb.h"
#include "wiced_memory.h"

wiced_bt_heap_t *gp_porting_heap = NULL;  //for dynamic GATT DB

/* GATT Db increment Step */
#define GATT_DB_INCR_STEP 1024

/*
    2   ->  BIT16_TO_8((UINT16)(handle)), \
    1   ->  LEGATTDB_PERM_READABLE, \
    1   ->  21, \
    2   ->  BIT16_TO_8(GATT_UUID_CHAR_DECLARE), \
    1   ->  (UINT8)(properties), \
    2   ->  BIT16_TO_8((UINT16)(handle_value)), \
    16  ->  uuid, \
    2   ->  BIT16_TO_8((UINT16)(handle_value)), \
    1   ->  (UINT8)(permission | LEGATTDB_PERM_SERVICE_UUID_128), \
    1   ->  (UINT8)(LEGATTDB_UUID128_SIZE), \
    16  ->  uuid
*/
#define MAX_CHAR_SIZE 45

/*
    2  ->  BIT16_TO_8((uint16_t)(handle)), \
    1  ->  (uint8_t)(permission | LEGATTDB_PERM_SERVICE_UUID_128), \
    1  ->  (uint8_t)(LEGATTDB_UUID128_SIZE), \
    1  ->  (uint8_t)(0), \
    16 ->  uuid
*/
#define MAX_DESC_SIZE 21

#define SERVICE_HANDLE_START 1

typedef struct dynamic_gattdb_service_data {
    uint8_t service_handle;
    uint8_t previous_entry_handle;
    uint8_t max_handle;
    uint16_t service_data_offset;
    uint8_t* service_data;
    wiced_bt_uuid_t uuid;
    struct dynamic_gattdb_service_data* next_service;
    struct dynamic_gattdb_service_data* prev_service;
} dynamic_gattdb_service_data_t;

/* GATT database pointer and length */
static uint8_t* gatt_server_dynamic_db = NULL;
static uint8_t* gatt_server_dynamic_db_cache = NULL;
static uint16_t gatt_server_dynamic_db_len = 0;
static uint16_t gatt_server_dynamic_db_allocated_len = 0;

static dynamic_gattdb_service_data_t* local_service_list = NULL;

typedef enum {
    SERVICE_POS_FIRST = 0,
    SERVICE_POS_MIDDLE,
    SERVICE_POS_LAST,
    SERVICE_POS_INVALID
} dynamic_gattdb_service_pos_t;

/*
    Get service entry corresponding to service_handle from local list
*/
static wiced_result_t dynamic_gattdb_get_service_entry( uint8_t service_handle, dynamic_gattdb_service_data_t** service_entry ) {

    dynamic_gattdb_service_data_t* list_runner = local_service_list;

    if( NULL == list_runner )
    { return WICED_ERROR; }

    /* Find the entry corresponding to service_handle */
    while( list_runner->service_handle != service_handle )
    { list_runner = list_runner->next_service; }

    /* return error if handle not found */
    if( service_handle != list_runner->service_handle )
    { return WICED_BADARG; }

    *service_entry = list_runner;

    return WICED_SUCCESS;
}

/*
    Is the characteristic or descriptor permission writable?
*/
static wiced_bool_t dynamic_gattdb_is_permission_writable( uint8_t permission ) {
    if( ( permission & ( LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ |
                         LEGATTDB_PERM_RELIABLE_WRITE | LEGATTDB_PERM_AUTH_WRITABLE ) ) == 0 ) {
        return 0;
    }

    return 1;
}

#if 0
/*
    Get information(position, start address, length) regarding the given service handle.
 */
static wiced_result_t dynamic_gattdb_get_service_info( uint8_t service_handle, dynamic_gattdb_service_pos_t* service_pos, uint8_t** service_start_addr, uint8_t* service_len ) {
    legattdb_getServiceBlob( service_handle, service_start_addr, service_len, service_pos );
    return WICED_SUCCESS;
}
#endif
/*
    Add Service to GATT Database

    Addresses three possible scenarios,
        1) First service in the Db - Allocate memory and copy the service data
        2) Non first service in the Db that fits in the already allocated memory - Append data to the existing buffer
        3) Non first service in the Db that does not fit in the already allocated memory - Allocate new buffer, copy over the existing Db, append the new entry

    When the first service id added, 1kb is allocated for db. When subsequent services are added, If this buffer is not enough, a new buffer(which is 1 kb more in size)
    will be used to replace the current buffer. If larger buffer cannot be allocated, returns WICED_OUT_OF_HEAP_SPACE.
 */
static wiced_result_t dynamic_gattdb_add_service_to_db( uint8_t* service_data, uint16_t service_data_len ) {

    uint32_t updated_db_len = gatt_server_dynamic_db_len + service_data_len;
    uint8_t* swap_ptr = NULL;

    /* Check if a buffer is already allocated for GATT Db, if not allocate 1kb */
    if( NULL == gatt_server_dynamic_db ) {

        /* allocate 1kb for GATT database and update gatt_server_dynamic_db_allocated_len */
        gatt_server_dynamic_db = ( uint8_t* )wiced_bt_get_buffer_from_heap( gp_porting_heap, GATT_DB_INCR_STEP );
                                                              
        if( NULL == gatt_server_dynamic_db )
        { return WICED_OUT_OF_HEAP_SPACE; }

        gatt_server_dynamic_db_len = 0;
        gatt_server_dynamic_db_cache = gatt_server_dynamic_db;
        gatt_server_dynamic_db_allocated_len = GATT_DB_INCR_STEP;

    }

    /* check if the new entry will fit in the existing Db */
    if( updated_db_len > gatt_server_dynamic_db_allocated_len ) {

        /* allocate new buffer with 1k more size than before */
        swap_ptr = ( uint8_t* )wiced_bt_get_buffer_from_heap( gp_porting_heap, GATT_DB_INCR_STEP );

        if( NULL == swap_ptr )
        { return WICED_OUT_OF_HEAP_SPACE; }

        gatt_server_dynamic_db_allocated_len += GATT_DB_INCR_STEP;

        /* copy the entire existing db to the newly allocated buffer and free the existing buffer */
        memcpy( ( void* )swap_ptr, ( void* )gatt_server_dynamic_db, gatt_server_dynamic_db_len );
        wiced_bt_free_buffer( (void *)gatt_server_dynamic_db_cache );
        gatt_server_dynamic_db = swap_ptr;
        gatt_server_dynamic_db_cache = gatt_server_dynamic_db;
    }

    /* copy the new entry at the end of the existing database and update gatt_server_dynamic_db_len */
    memcpy( ( void* )&gatt_server_dynamic_db[gatt_server_dynamic_db_len], ( void* )service_data, service_data_len );
    gatt_server_dynamic_db_len += service_data_len;


    

    /* register the new db */
    if( WICED_BT_GATT_SUCCESS != wiced_bt_gatt_db_init( gatt_server_dynamic_db, gatt_server_dynamic_db_len, NULL ) )
    { return WICED_ERROR; }

    return WICED_SUCCESS;
}

#if 0
/*
    Remove Service from GATT Database

    Addresses the following scenarios,
        1) First service being removed - re-register database with updated pointer (to avoid moving all the existing data) and length
        2) Last service being removed - update the db length and re-register db
        3) any other service being removed - find the service and move the data below to overwrite the service and update the db length
 */
static wiced_result_t dynamic_gattdb_remove_service_from_db( uint8_t service_handle ) {

    dynamic_gattdb_service_pos_t service_pos = SERVICE_POS_INVALID;
    uint8_t* service_start_addr = NULL, *service_end_addr = NULL;
    uint8_t service_len = 0, remaining_bytes = 0;
    wiced_result_t status = WICED_ERROR;

    /* get information about the service to be deleted */
    if( WICED_SUCCESS == dynamic_gattdb_get_service_info( service_handle, &service_pos, &service_start_addr, &service_len ) ) {

        switch( service_pos ) {

            case SERVICE_POS_FIRST:

                gatt_server_dynamic_db = gatt_server_dynamic_db + service_len ;
                status = WICED_SUCCESS;

                break;

            case SERVICE_POS_MIDDLE:

                remaining_bytes = gatt_server_dynamic_db_len - ( service_end_addr - gatt_server_dynamic_db );

                /* move up the services below */
                memcpy( ( void* )service_start_addr, ( void* )( service_start_addr + service_len ), remaining_bytes );
                status = WICED_SUCCESS;

                break;

            case SERVICE_POS_LAST:

                status = WICED_SUCCESS;

                break;

            default:
                status = WICED_ERROR;
                break;
        }

        /* register the updated db */
        if( WICED_BT_GATT_SUCCESS != wiced_bt_gatt_db_init( gatt_server_dynamic_db, ( gatt_server_dynamic_db_len - service_len ), NULL ) )
        { status = WICED_ERROR; }

        return status;
    }

    return WICED_ERROR;

}
#endif
/*
    Allocate memory based on the number of handles for this service
 */
uint32_t g_service_handle = 1;
wiced_result_t wiced_bt_dynamic_gattdb_add_service( uint16_t num_of_handles, uint8_t* service_handle, wiced_bt_uuid_t* uuid, wiced_bool_t primary_service ) {

    dynamic_gattdb_service_data_t *temp = NULL, *list_runner = local_service_list;
    uint8_t* p = NULL;
    uint16_t service_type = 0;

    /* Allocate memory for a new entry */
    temp = ( dynamic_gattdb_service_data_t* )wiced_bt_get_buffer_from_heap( gp_porting_heap, sizeof( dynamic_gattdb_service_data_t ) );

    if( NULL == temp )
    { return WICED_OUT_OF_HEAP_SPACE; }

    /* Create a local service entry */
    if( NULL == local_service_list ) {

        temp->max_handle            = g_service_handle + num_of_handles;
        temp->prev_service          = NULL;
        temp->next_service          = NULL;
        temp->service_handle        = g_service_handle;
        temp->previous_entry_handle = temp->service_handle;

        /* Add the first entry to the list */
        local_service_list = temp;

    } else {

        /* goto the end of the list */
        while( list_runner->next_service )
        { list_runner = list_runner->next_service; }

        /* populate the new entry fields */
        temp->max_handle            = g_service_handle + num_of_handles;
        temp->next_service          = NULL;
        temp->service_handle        = g_service_handle;
        temp->previous_entry_handle = temp->service_handle;

        /* append it to the existing service list */
        list_runner->next_service = temp;
        temp->prev_service = list_runner;
    }

    temp->service_data          = NULL;
    temp->service_data_offset   = 0;
    memcpy( &temp->uuid, uuid, sizeof( wiced_bt_uuid_t ) );

    /* Allocate memory for the service data based on the number of handles */
    temp->service_data = ( uint8_t* )wiced_bt_get_buffer_from_heap( gp_porting_heap, num_of_handles * MAX_CHAR_SIZE );

    if( NULL == temp->service_data )
    { return WICED_OUT_OF_HEAP_SPACE; }

    if(g_service_handle != 0xFFFF)
        g_service_handle            += num_of_handles;
    else
        g_service_handle            = 0;

    /* assign service handle */
    *service_handle = temp->service_handle;

    service_type = ( primary_service ) ? GATT_UUID_PRI_SERVICE : GATT_UUID_SEC_SERVICE;

    /* Create the service entry */
    p = temp->service_data;

    UINT16_TO_STREAM( p, temp->service_handle );
    UINT8_TO_STREAM( p, LEGATTDB_PERM_READABLE );

    if( uuid->len == LEN_UUID_16 ) {
        UINT8_TO_STREAM( p, 4 );
        UINT16_TO_STREAM( p, service_type );
        UINT16_TO_STREAM( p, uuid->uu.uuid16 );
    } else if( uuid->len == LEN_UUID_128 ) {
        UINT8_TO_STREAM( p, 18 );
        UINT16_TO_STREAM( p, service_type );
        ARRAY_TO_STREAM( p, uuid->uu.uuid128, LEN_UUID_128 );
    } else
    { return WICED_BADARG; }

    temp->service_data_offset = ( p - temp->service_data );
    return WICED_SUCCESS;
}

#if 0
wiced_result_t wiced_bt_dynamic_gattdb_delete_service( uint8_t service_handle ) {
    return dynamic_gattdb_remove_service_from_db( service_handle );
}
#endif
wiced_result_t wiced_bt_dynamic_gattdb_add_included_service( uint8_t parent_service_handle, uint8_t *service_handle, uint8_t included_service_attribute_handle,
        uint16_t end_group_handle, wiced_bt_uuid_t* uuid ) {

    dynamic_gattdb_service_data_t* list_runner = NULL;
    wiced_result_t status = WICED_ERROR;
    uint8_t* p = NULL;

    status = dynamic_gattdb_get_service_entry( parent_service_handle, &list_runner );

    if( WICED_SUCCESS != status )
    { return status; }

    /* Add include service data */
    p = list_runner->service_data + list_runner->service_data_offset;

    ++list_runner->previous_entry_handle;
    UINT16_TO_STREAM( p, list_runner->previous_entry_handle );

    UINT8_TO_STREAM( p, LEGATTDB_PERM_READABLE );

    if( uuid->len == LEN_UUID_16 ) {
        UINT8_TO_STREAM( p, 8 );
    } else if( uuid->len == LEN_UUID_128 ) {
        UINT8_TO_STREAM( p, 6 );
    } else {
        return WICED_BADARG;
    }

    UINT16_TO_STREAM( p, GATT_UUID_INCLUDE_SERVICE );
    UINT16_TO_STREAM( p, included_service_attribute_handle );
    UINT16_TO_STREAM( p, end_group_handle );

    if( uuid->len == LEN_UUID_16 ) {
        UINT16_TO_STREAM( p, uuid->uu.uuid16 );
    }

    list_runner->service_data_offset = ( p -  list_runner->service_data );

    *service_handle = list_runner->previous_entry_handle;

    return WICED_SUCCESS;
}

wiced_result_t wiced_bt_dynamic_gattdb_add_characteristic( uint8_t service_handle, uint8_t* char_handle, wiced_bt_uuid_t* uuid, uint8_t property, uint8_t permission ) {

    dynamic_gattdb_service_data_t* list_runner = NULL;
    wiced_result_t status = WICED_ERROR;
    uint8_t* p = NULL;
    wiced_bool_t permission_writable = dynamic_gattdb_is_permission_writable( permission );

    status = dynamic_gattdb_get_service_entry( service_handle, &list_runner );

    if( WICED_SUCCESS != status )
    { return status; }

    /* Add characteristic data */
    p = list_runner->service_data + list_runner->service_data_offset;

    ++list_runner->previous_entry_handle;
    UINT16_TO_STREAM( p, list_runner->previous_entry_handle );

    UINT8_TO_STREAM( p, LEGATTDB_PERM_READABLE );

    if( uuid->len == LEN_UUID_16 ) {
        UINT8_TO_STREAM( p, 7 );
    } else if( uuid->len == LEN_UUID_128 )
    { UINT8_TO_STREAM( p, 21 ); }
    else
    { return WICED_BADARG; }

    UINT16_TO_STREAM( p, GATT_UUID_CHAR_DECLARE );
    UINT8_TO_STREAM( p, property );

    ++list_runner->previous_entry_handle;
    UINT16_TO_STREAM( p, list_runner->previous_entry_handle );

    if( uuid->len == LEN_UUID_16 ) {
        UINT16_TO_STREAM( p, uuid->uu.uuid16 );
        UINT16_TO_STREAM( p, list_runner->previous_entry_handle );
        UINT8_TO_STREAM( p, permission );
        UINT8_TO_STREAM( p, LEGATTDB_UUID16_SIZE );
    } else if( uuid->len == LEN_UUID_128 ) {
        ARRAY_TO_STREAM( p, uuid->uu.uuid128, LEN_UUID_128 );
        UINT16_TO_STREAM( p, list_runner->previous_entry_handle );
        UINT8_TO_STREAM( p, permission | LEGATTDB_PERM_SERVICE_UUID_128 );
        UINT8_TO_STREAM( p, LEGATTDB_UUID128_SIZE );
    }

    if( permission_writable )
    { UINT8_TO_STREAM( p, 0 ); }

    if( uuid->len == LEN_UUID_16 )
    { UINT16_TO_STREAM( p, uuid->uu.uuid16 ); }
    else if( uuid->len == LEN_UUID_128 )
    { ARRAY_TO_STREAM( p, uuid->uu.uuid128, LEN_UUID_128 ); }

    list_runner->service_data_offset = ( p -  list_runner->service_data );

    *char_handle = list_runner->previous_entry_handle;

    return WICED_SUCCESS;
}

wiced_result_t wiced_bt_dynamic_gattdb_add_descriptor( uint8_t service_handle, uint8_t* desc_handle, wiced_bt_uuid_t* uuid, uint8_t permission ) {

    dynamic_gattdb_service_data_t* list_runner = NULL;
    wiced_result_t status = WICED_ERROR;
    uint8_t* p = NULL;
    wiced_bool_t permission_writable = dynamic_gattdb_is_permission_writable( permission );

    status = dynamic_gattdb_get_service_entry( service_handle, &list_runner );

    if( WICED_SUCCESS != status )
    { return status; }

    /* Add descriptor data */
    p = list_runner->service_data + list_runner->service_data_offset;

    ++list_runner->previous_entry_handle;
    UINT16_TO_STREAM( p, list_runner->previous_entry_handle );

    if( uuid->len == LEN_UUID_16 ) {
        UINT8_TO_STREAM( p, permission );
        UINT8_TO_STREAM( p, LEGATTDB_UUID16_SIZE );
    } else if( uuid->len == LEN_UUID_128 ) {
        UINT8_TO_STREAM( p, permission | LEGATTDB_PERM_SERVICE_UUID_128 );
        UINT8_TO_STREAM( p, LEGATTDB_UUID128_SIZE );
    } else {
        return WICED_BADARG;
    }

    if( permission_writable ) {
        UINT8_TO_STREAM( p, 0 );
    }

    if( uuid->len == LEN_UUID_16 ) {
        UINT16_TO_STREAM( p, uuid->uu.uuid16 );
    } else if( uuid->len == LEN_UUID_128 ) {
        ARRAY_TO_STREAM( p, uuid->uu.uuid128, LEN_UUID_128 );
    }

    list_runner->service_data_offset = ( p -  list_runner->service_data );

    *desc_handle = list_runner->previous_entry_handle;

    return WICED_SUCCESS;
}

wiced_result_t wiced_bt_dynamic_gattdb_start_service( uint8_t service_handle ) {

    dynamic_gattdb_service_data_t* list_runner = NULL;
    wiced_result_t status = WICED_ERROR;

    status = dynamic_gattdb_get_service_entry( service_handle, &list_runner );

    if( WICED_SUCCESS != status )
    { return status; }

    /* Add to the GATT database */
    dynamic_gattdb_add_service_to_db( list_runner->service_data, list_runner->service_data_offset );

    /* delink the entry from the local service list and free the memory allocated */
    if( NULL == list_runner->prev_service /* first entry */ ) {

        if( NULL != list_runner->next_service )  {
            list_runner->next_service->prev_service = NULL;
        }

        local_service_list = list_runner->next_service;

    } else if( NULL == list_runner->next_service /* last entry */ ) {

        list_runner->prev_service->next_service = NULL;

    } else {

        /* medial entry */
        list_runner->prev_service->next_service = list_runner->next_service;
        list_runner->next_service->prev_service = list_runner->prev_service;
    }

    wiced_bt_free_buffer( (void *)list_runner->service_data);
    wiced_bt_free_buffer( (void *)list_runner);

    return WICED_SUCCESS;
}

wiced_result_t wiced_bt_dynamic_gattdb_util_get_UUID( uint8_t service_handle, wiced_bt_uuid_t* service_UUID ) {
    dynamic_gattdb_service_data_t* list_runner = NULL;
    wiced_result_t status = WICED_ERROR;

    status = dynamic_gattdb_get_service_entry( service_handle, &list_runner );

    if( WICED_SUCCESS != status )
    { return status; }

    memcpy( service_UUID, &list_runner->uuid, sizeof( wiced_bt_uuid_t ) );
    return WICED_SUCCESS;
}

uint16_t wiced_bt_dynamic_gattdb_util_get_service_end_handle( uint8_t service_handle ) {
    dynamic_gattdb_service_data_t* list_runner = NULL;
    wiced_result_t status = WICED_ERROR;

    status = dynamic_gattdb_get_service_entry( service_handle, &list_runner );

    if( WICED_SUCCESS != status )
    { return 0; }

    return ( list_runner->max_handle - 1 );
}

/**
 * Function     wiced_bt_gatt_db_reset
 *
 * Clear the gatt database
 *
 * @return      Return WICED_TRUE if database is cleared, otherwise WICED_FALSE
 */
wiced_bool_t wiced_bt_gatt_db_reset( void ) {
    if( gatt_server_dynamic_db != NULL ) {
        wiced_bt_free_buffer( (void *)gatt_server_dynamic_db);
        gatt_server_dynamic_db = NULL;
        gatt_server_dynamic_db_cache = NULL;
    }

    gatt_server_dynamic_db_len     = 0;
    gatt_server_dynamic_db_allocated_len = 0;

    wiced_bt_delete_heap(gp_porting_heap);
    return WICED_TRUE;
}

/**
 * Function     wiced_bt_dynamic_gattdb_init
 *
 * initial dynamic gatt database
 *
 */
void wiced_bt_dynamic_gattdb_init( void )
{
    gp_porting_heap = wiced_bt_create_heap ("porting", NULL, 0x1000, NULL, WICED_FALSE);
}
