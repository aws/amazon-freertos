/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#pragma once

#include "wiced_result.h"
#include "wiced_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct linked_list_node linked_list_node_t;

/******************************************************
 *                    Structures
 ******************************************************/

#pragma pack(1)
struct linked_list_node
{
    void*               data;
    linked_list_node_t* next;
    linked_list_node_t* prev;
};

typedef struct
{
    uint32_t            count;
    linked_list_node_t* front;
    linked_list_node_t* rear;
} linked_list_t;
#pragma pack()

typedef wiced_bool_t (*linked_list_compare_callback_t)( linked_list_node_t* node_to_compare, void* user_data );

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

wiced_result_t linked_list_init( linked_list_t* list );

wiced_result_t linked_list_deinit( linked_list_t* list );

wiced_result_t linked_list_get_count( linked_list_t* list, uint32_t* count );

wiced_result_t linked_list_set_node_data( linked_list_node_t* node, const void* data );

wiced_result_t linked_list_get_front_node( linked_list_t* list, linked_list_node_t** front_node );

wiced_result_t linked_list_get_rear_node( linked_list_t* list, linked_list_node_t** rear_node );

wiced_result_t linked_list_find_node( linked_list_t* list, linked_list_compare_callback_t callback, void* user_data, linked_list_node_t** node_found );

wiced_result_t linked_list_insert_node_at_front( linked_list_t* list, linked_list_node_t* node );

wiced_result_t linked_list_insert_node_at_rear( linked_list_t* list, linked_list_node_t* node );

wiced_result_t linked_list_insert_node_before( linked_list_t* list, linked_list_node_t* reference_node, linked_list_node_t* node_to_insert );

wiced_result_t linked_list_insert_node_after( linked_list_t* list, linked_list_node_t* reference_node, linked_list_node_t* node_to_insert );

wiced_result_t linked_list_remove_node( linked_list_t* list, linked_list_node_t* node );

wiced_result_t linked_list_remove_node_from_front( linked_list_t* list, linked_list_node_t** removed_node );

wiced_result_t linked_list_remove_node_from_rear( linked_list_t* list, linked_list_node_t** removed_node );

#ifdef __cplusplus
} /* extern "C" */
#endif
