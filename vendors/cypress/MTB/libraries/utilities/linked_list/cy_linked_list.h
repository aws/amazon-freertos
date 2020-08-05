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
/**
 * @file
 * This is a generic linked list library with helper functions to add, insert, delete and find nodes in a list.
 * 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "cy_result_mw.h"
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
/** Results returned by linked list library */
#define CY_RSLT_MODULE_LINKED_LIST_ERR_CODE_START          (0) /** Linked list result code base */
#define CY_RSLT_LINKED_LIST_ERROR_BASE                     CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_LINKED_LIST_BASE, CY_RSLT_MODULE_LINKED_LIST_ERR_CODE_START)

#define CY_RSLT_LINKED_LIST_BADARG                         ((cy_rslt_t)(CY_RSLT_LINKED_LIST_ERROR_BASE + 1)) /** Linked list error bad argument */
#define CY_RSLT_LINKED_LIST_NOT_FOUND                      ((cy_rslt_t)(CY_RSLT_LINKED_LIST_ERROR_BASE + 2)) /** Linked list error not found */

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
/******************************************************************************/
/** \addtogroup group_linkedlist_structures *//** \{ */
/******************************************************************************/

/** Linked list node structure */
typedef struct linked_list_node cy_linked_list_node_t;

/******************************************************
 *                    Structures
 ******************************************************/
/** Linked list node object */
struct linked_list_node
{
    void*               data;    /**< Data object of the node */
    cy_linked_list_node_t* next; /**< Pointer to the next node in the list */
    cy_linked_list_node_t* prev; /**< Pointer to the previous node in the list */
};

/** Linked list object */
typedef struct
{
    uint32_t            count;    /**< Number of nodes in the list */
    cy_linked_list_node_t* front; /**< Pointer to the front of the list */
    cy_linked_list_node_t* rear;  /**< Pointer to the rear of the list */
} cy_linked_list_t;

/** Callback to compare the data in the node for find routine */
typedef bool (*cy_linked_list_compare_callback_t)( cy_linked_list_node_t* node_to_compare, void* user_data );

/** \} */
/******************************************************
 *                 Global Variables
 ******************************************************/

/*****************************************************************************/
/**
 *
 *  @addtogroup group_linkedlist_func
 *
 * This is a generic linked list library with helper functions to add, insert, delete and find nodes in a list.
 *
 *  @{
 */
/******************************************************
 *               Function Declarations
 ******************************************************/

 /** Initialize list
 *
 * @param [in]  list   - Pointer to a list
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 */
cy_rslt_t cy_linked_list_init( cy_linked_list_t* list );

 /** De-initialize list
 *
 * @param [in]  list   - Pointer to a list
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 */
cy_rslt_t cy_linked_list_deinit( cy_linked_list_t* list );

 /** Get the count for number of nodes present in list
 *
 * @param [in]  list   - Pointer to an initialized list
 * @param [out] count  - number of nodes present in list
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 */
cy_rslt_t cy_linked_list_get_count( cy_linked_list_t* list, uint32_t* count );

 /** Set the data for linked list node
 *
 * @param [in]  node   - linked list node for which data need to be set
 * @param [in]  data   - data value for node
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 */
cy_rslt_t cy_linked_list_set_node_data( cy_linked_list_node_t* node, const void* data );

 /** Get the front node in list
 *
 * @param [in]  list        - Pointer to an initialized list
 * @param [out] front_node  - front node in the list
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 */
cy_rslt_t cy_linked_list_get_front_node( cy_linked_list_t* list, cy_linked_list_node_t** front_node );

 /** Get the rear node in list
 *
 * @param [in]  list       - Pointer to an initialized list
 * @param [out] rear_node  - rear node in the list
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 */
cy_rslt_t cy_linked_list_get_rear_node( cy_linked_list_t* list, cy_linked_list_node_t** rear_node );

 /** Find particular node in the list
 *
 * @param [in]  list       - Pointer to an initialized list
 * @param [in]  callback   - callback to compare node data in the list
 * @param [in]  user_data  - user data
 * @param [out] node_found - node_found in the list
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 *                     - CY_RSLT_LINKED_LIST_NOT_FOUND
 */
cy_rslt_t cy_linked_list_find_node( cy_linked_list_t* list, cy_linked_list_compare_callback_t callback, void* user_data, cy_linked_list_node_t** node_found );

 /** Insert node at the front of the list
 *
 * @param [in]  list    - Pointer to an initialized list
 * @param [in]  node    - node to be added at the front of the list
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 */
cy_rslt_t cy_linked_list_insert_node_at_front( cy_linked_list_t* list, cy_linked_list_node_t* node );

 /** Insert node at the rear of the list
 *
 * @param [in]  list    - Pointer to an initialized list
 * @param [in]  node    - node to be added at the rear of the list
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 */
cy_rslt_t cy_linked_list_insert_node_at_rear( cy_linked_list_t* list, cy_linked_list_node_t* node );

 /** Insert node before reference node
 *
 * @param [in]  list              - Pointer to an initialized list
 * @param [in]  reference_node    - Pointer to a node before which you want to add new node
 * @param [in]  node_to_insert    - Pointer to a node to be inserted
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 *                     - CY_RSLT_LINKED_LIST_NOT_FOUND
 */
cy_rslt_t cy_linked_list_insert_node_before( cy_linked_list_t* list, cy_linked_list_node_t* reference_node, cy_linked_list_node_t* node_to_insert );

 /** Insert node after reference node
 *
 * @param [in]  list              - Pointer to an initialized list
 * @param [in]  reference_node    - Pointer to a node after which you want to add new node
 * @param [in]  node_to_insert    - Pointer to a node to be inserted
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 *                     - CY_RSLT_LINKED_LIST_NOT_FOUND
 */
cy_rslt_t cy_linked_list_insert_node_after( cy_linked_list_t* list, cy_linked_list_node_t* reference_node, cy_linked_list_node_t* node_to_insert );

 /** Remove the node from the list
 *
 * @param [in]  list              - Pointer to an initialized list
 * @param [in]  node              - Pointer to a linked list node to be deleted
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 *                     - CY_RSLT_LINKED_LIST_NOT_FOUND
 */
cy_rslt_t cy_linked_list_remove_node( cy_linked_list_t* list, cy_linked_list_node_t* node );

 /** Remove node from front of the list
 *
 * @param [in]  list              - Pointer to an initialized list
 * @param [out] removed_node      - removed node
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 *                     - CY_RSLT_LINKED_LIST_NOT_FOUND
 */
cy_rslt_t cy_linked_list_remove_node_from_front( cy_linked_list_t* list, cy_linked_list_node_t** removed_node );

 /** Remove node from rear of the list
 *
 * @param [in]  list              - Pointer to an initialized list
 * @param [out] removed_node      - removed node
 *
 *  @return on success - CY_RSLT_SUCCESS
 *          on failure - CY_RSLT_LINKED_LIST_BADARG
 *                     - CY_RSLT_LINKED_LIST_NOT_FOUND
 */
cy_rslt_t cy_linked_list_remove_node_from_rear( cy_linked_list_t* list, cy_linked_list_node_t** removed_node );

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
