#ifndef __WMSDK_LIST_H__
#define __WMSDK_LIST_H__

#include <stddef.h>
#include <wm_utils.h>

typedef struct list_head {
	struct list_head *next, *prev;
} list_head_t;

/*
 * Doubly linked list implementation.
 */

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	list_head_t name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(list_head_t *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(list_head_t *new,
			      list_head_t *prev,
			      list_head_t *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * Insert a new entry after the specified head.
 */
static inline void list_add(list_head_t *new, list_head_t *head)
{
	__list_add(new, head, head->next);
}

/**
 * Insert a new entry before the specified head.
 */
static inline void list_add_tail(list_head_t *new, list_head_t *head)
{
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 */
static inline void __list_del(list_head_t * prev, list_head_t * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void __list_del_entry(list_head_t *entry)
{
	__list_del(entry->prev, entry->next);
}

static inline void list_del(list_head_t *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(list_head_t *old,
				list_head_t *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 */
static inline int list_is_last(const list_head_t *list,
				const list_head_t *head)
{
	return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 */
static inline int list_empty(const list_head_t *head)
{
	return head->next == head;
}

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &list_head_t pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(__ptr, type, member) \
	(type *)((char *)__ptr - offsetof(type, member))

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &list_head_t to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_entry_type	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_type(position, position_type, head, member,m_type) \
	const m_type *__ptr = (head)->next;	\
	for (position = list_entry(__ptr, position_type, member); \
	     &position->member != (head); 	\
	     position = list_entry((const m_type *) position->member.next, position_type, member))

#endif /* __WMSDK_LIST_H__ */
