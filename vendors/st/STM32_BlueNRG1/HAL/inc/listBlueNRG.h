/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : list.h
* Author             : AMS - HEA&RF BU
* Version            : V1.0.0
* Date               : 19-July-2012
* Description        : Header file for linked list library.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#ifndef _LIST_H_
#define _LIST_H_

typedef struct _tListNode {
  struct _tListNode * next;
  struct _tListNode * prev;
}tListNode, *pListNode;

void list_init_head (tListNode * listHead);

uint8_t list_is_empty (tListNode * listHead);

void list_insert_head (tListNode * listHead, tListNode * node);

void list_insert_tail (tListNode * listHead, tListNode * node);

void list_remove_node (tListNode * node);

void list_remove_head (tListNode * listHead, tListNode ** node );

void list_remove_tail (tListNode * listHead, tListNode ** node );

void list_insert_node_after (tListNode * node, tListNode * ref_node);

void list_insert_node_before (tListNode * node, tListNode * ref_node);

int list_get_size (tListNode * listHead);

void list_get_next_node (tListNode * ref_node, tListNode ** node);

void list_get_prev_node (tListNode * ref_node, tListNode ** node);

#endif /* _LIST_H_ */
