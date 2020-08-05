/*
 * Copyright 2019-2020 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file
 * 
 * Linked list helper library
 */

#include "string.h"
#include "cy_linked_list.h"

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

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

cy_rslt_t cy_linked_list_init( cy_linked_list_t* list )
{
    if ( list == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    memset( list, 0, sizeof( *list ) );
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_deinit( cy_linked_list_t* list )
{
    cy_linked_list_node_t* current;

    if ( list == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    /* Traverse through all nodes and detach them */
    current = list->front;

    while ( current != NULL )
    {
        cy_linked_list_node_t* next = current->next;

        /* Detach node from the list */
        current->prev = NULL;
        current->next = NULL;

        /* Move to the next node */
        current = next;
    }

    memset( list, 0, sizeof( *list ) );
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_get_count( cy_linked_list_t* list, uint32_t* count )
{
    if ( list == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    *count = list->count;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_set_node_data( cy_linked_list_node_t* node, const void* data )
{
    if ( node == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    node->data = (void*)data;
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_get_front_node( cy_linked_list_t* list, cy_linked_list_node_t** front_node )
{
    if ( list == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    if ( list->count == 0 )
    {
        *front_node = NULL;
        return CY_RSLT_LINKED_LIST_NOT_FOUND;
    }

    *front_node = list->front;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_get_rear_node( cy_linked_list_t* list, cy_linked_list_node_t** rear_node )
{
    if ( list == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    if ( list->count == 0 )
    {
        *rear_node = NULL;
        return CY_RSLT_LINKED_LIST_NOT_FOUND;
    }

    *rear_node = list->rear;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_find_node( cy_linked_list_t* list, cy_linked_list_compare_callback_t callback, void* user_data, cy_linked_list_node_t** node_found )
{
    cy_linked_list_node_t* current;

    if ( list == NULL || callback == NULL || node_found == NULL || list->count == 0 )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    current = list->front;

    while ( current != NULL )
    {
        if ( callback( current, user_data ) == true )
        {
            *node_found = current;
            return CY_RSLT_SUCCESS;
        }

        current = current->next;
    }

    return CY_RSLT_LINKED_LIST_NOT_FOUND;
}

cy_rslt_t cy_linked_list_insert_node_at_front( cy_linked_list_t* list, cy_linked_list_node_t* node )
{
    if ( list == NULL || node == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    if ( list->count == 0 )
    {
        list->front = node;
        list->rear  = node;
        node->prev  = NULL;
        node->next  = NULL;
    }
    else
    {
        node->prev        = NULL;
        node->next        = list->front;
        list->front->prev = node;
        list->front       = node;
    }

    list->count++;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_insert_node_at_rear( cy_linked_list_t* list, cy_linked_list_node_t* node )
{
    if ( list == NULL || node == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    if ( list->count == 0 )
    {
        list->front = node;
        list->rear  = node;
        node->prev  = NULL;
        node->next  = NULL;
    }
    else
    {
        node->next       = NULL;
        node->prev       = list->rear;
        list->rear->next = node;
        list->rear       = node;
    }

    list->count++;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_insert_node_before( cy_linked_list_t* list, cy_linked_list_node_t* reference_node, cy_linked_list_node_t* node_to_insert )
{
    /* WARNING: User must make sure that reference_node is in the list */
    if ( list == NULL || reference_node == NULL || node_to_insert == NULL || list->count == 0 )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    if ( list->count == 0 )
    {
        return CY_RSLT_LINKED_LIST_NOT_FOUND;
    }

    if ( reference_node == list->front )
    {
        return cy_linked_list_insert_node_at_front( list, node_to_insert );
    }
    else
    {
        node_to_insert->prev       = reference_node->prev;
        node_to_insert->prev->next = node_to_insert;
        node_to_insert->next       = reference_node;
        reference_node->prev       = node_to_insert;

        list->count++;

        return CY_RSLT_SUCCESS;
    }
}

cy_rslt_t cy_linked_list_insert_node_after( cy_linked_list_t* list, cy_linked_list_node_t* reference_node, cy_linked_list_node_t* node_to_insert )
{
    /* WARNING: User must make sure that reference_node is in the list */
    if ( list == NULL || reference_node == NULL || node_to_insert == NULL || list->count == 0 )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    if ( reference_node == list->rear )
    {
        return cy_linked_list_insert_node_at_rear( list, node_to_insert );
    }
    else
    {
        node_to_insert->prev       = reference_node;
        node_to_insert->next       = reference_node->next;
        reference_node->next->prev = node_to_insert;
        reference_node->next       = node_to_insert;

        list->count++;

        return CY_RSLT_SUCCESS;
    }
}

cy_rslt_t cy_linked_list_remove_node( cy_linked_list_t* list, cy_linked_list_node_t* node )
{
    /* WARNING: User must make sure that node to remove is in the list */
    if ( list == NULL || node == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    if ( list->count == 0 )
    {
        return CY_RSLT_LINKED_LIST_NOT_FOUND;
    }

    if ( list->count == 1 )
    {
        list->front = NULL;
        list->rear  = NULL;
    }
    else if ( node == list->front )
    {
        cy_linked_list_node_t* removed_node;

        return cy_linked_list_remove_node_from_front( list, &removed_node );
    }
    else if ( node == list->rear )
    {
        cy_linked_list_node_t* removed_node;

        return cy_linked_list_remove_node_from_rear( list, &removed_node );
    }
    else
    {
        /* Check for possible 'null' pointer exception, when cy_linked_list_remove_node() is called with the same 'node' for more than once */
        if (node->prev == NULL || node->next == NULL)
        {
            return CY_RSLT_LINKED_LIST_BADARG;
        }
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    /* Make sure that detach node does not point to some arbitrary memory location */
    node->prev = NULL;
    node->next = NULL;

    list->count--;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_remove_node_from_front( cy_linked_list_t* list, cy_linked_list_node_t** removed_node )
{
    if ( list == NULL || removed_node == NULL || list->count == 0 )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    *removed_node = list->front;

    if ( list->count == 1 )
    {
        list->front = NULL;
        list->rear  = NULL;
    }
    else
    {
        list->front       = list->front->next;
        list->front->prev = NULL;
    }

    /* Make sure node does not point to some arbitrary memory location */
    (*removed_node)->prev = NULL;
    (*removed_node)->next = NULL;

    list->count--;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_linked_list_remove_node_from_rear( cy_linked_list_t* list, cy_linked_list_node_t** removed_node )
{
    if ( list == NULL || removed_node == NULL )
    {
        return CY_RSLT_LINKED_LIST_BADARG;
    }

    if ( list->count == 0 )
    {
        return CY_RSLT_LINKED_LIST_NOT_FOUND;
    }

    *removed_node = list->rear;

    if ( list->count == 1 )
    {
        list->front = NULL;
        list->rear  = NULL;
    }
    else
    {
        list->rear       = list->rear->prev;
        list->rear->next = NULL;
    }

    /* Make sure node does not point to some arbitrary memory location */
    (*removed_node)->prev = NULL;
    (*removed_node)->next = NULL;

    list->count--;

    return CY_RSLT_SUCCESS;
}

#ifdef __cplusplus
}
#endif
