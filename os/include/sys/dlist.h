#pragma once

#include <os/include/toolchain/toolchain.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _dnode {
	union {
		struct _dnode *head; /* ptr to head of list (sys_dlist_t) */
		struct _dnode *next; /* ptr to next node    (sys_dnode_t) */
	};
	union {
		struct _dnode *tail; /* ptr to tail of list (sys_dlist_t) */
		struct _dnode *prev; /* ptr to previous node (sys_dnode_t) */
	};
};

typedef struct _dnode sys_dlist_t;
typedef struct _dnode sys_dnode_t;

static inline void sys_dlist_init(sys_dlist_t *list)
{
	list->head = (sys_dnode_t *)list;
	list->tail = (sys_dnode_t *)list;
}

static inline void sys_dnode_init(sys_dnode_t *node)
{
	node->next = NULL;
	node->prev = NULL;
}

static inline bool sys_dlist_is_empty(sys_dlist_t *list)
{
	return list->head == list;
}

static inline bool sys_dlist_is_tail(sys_dlist_t *list, sys_dnode_t *node)
{
	return list->tail == node;
}

static inline bool sys_dlist_is_head(sys_dlist_t *list, sys_dnode_t *node)
{
	return list->head == node;
}

static inline bool sys_dnode_is_linked(const sys_dnode_t *node)
{
	return node->next != NULL;
}

static inline bool sys_dlist_has_multiple_nodes(sys_dlist_t *list)
{
	return list->head != list->tail;
}

static inline sys_dnode_t *sys_dlist_peek_head(sys_dlist_t *list)
{
	return sys_dlist_is_empty(list) ? NULL : list->head;
}

static inline sys_dnode_t *sys_dlist_peek_head_not_empty(sys_dlist_t *list)
{
	return list->head;
}

static inline sys_dnode_t *sys_dlist_peek_next_no_check(sys_dlist_t *list,
							sys_dnode_t *node)
{
	return (node == list->tail) ? NULL : node->next;
}

static inline sys_dnode_t *sys_dlist_peek_next(sys_dlist_t *list,
					       sys_dnode_t *node)
{
	return (node != NULL) ? sys_dlist_peek_next_no_check(list, node) : NULL;
}

static inline sys_dnode_t *sys_dlist_peek_prev_no_check(sys_dlist_t *list,
							sys_dnode_t *node)
{
	return (node == list->head) ? NULL : node->prev;
}

static inline sys_dnode_t *sys_dlist_peek_prev(sys_dlist_t *list,
					       sys_dnode_t *node)
{
	return (node != NULL) ? sys_dlist_peek_prev_no_check(list, node) : NULL;
}

static inline sys_dnode_t *sys_dlist_peek_tail(sys_dlist_t *list)
{
	return sys_dlist_is_empty(list) ? NULL : list->tail;
}

static inline void sys_dlist_append(sys_dlist_t *list, sys_dnode_t *node)
{
	sys_dnode_t *const tail = list->tail;

	node->next = list;
	node->prev = tail;

	tail->next = node;
	list->tail = node;
}

static inline void sys_dlist_prepend(sys_dlist_t *list, sys_dnode_t *node)
{
	sys_dnode_t *const head = list->head;

	node->next = head;
	node->prev = list;

	head->prev = node;
	list->head = node;
}

static inline void sys_dlist_insert(sys_dnode_t *successor, sys_dnode_t *node)
{
	sys_dnode_t *const prev = successor->prev;

	node->prev = prev;
	node->next = successor;
	prev->next = node;
	successor->prev = node;
}

/**
 * @brief insert node at position
 *
 * Insert a node in a location depending on a external condition. The cond()
 * function checks if the node is to be inserted _before_ the current node
 * against which it is checked.
 * This and other sys_dlist_*() functions are not thread safe.
 *
 * @param list the doubly-linked list to operate on
 * @param node the element to insert
 * @param cond a function that determines if the current node is the correct
 *             insert point
 * @param data parameter to cond()
 */

static inline void sys_dlist_insert_at(sys_dlist_t *list, sys_dnode_t *node,
	int (*cond)(sys_dnode_t *node, void *data), void *data)
{
	if (sys_dlist_is_empty(list)) {
		sys_dlist_append(list, node);
	} else {
		sys_dnode_t *pos = sys_dlist_peek_head(list);

		while ((pos != NULL) && (cond(pos, data) == 0)) {
			pos = sys_dlist_peek_next(list, pos);
		}
		if (pos != NULL) {
			sys_dlist_insert(pos, node);
		} else {
			sys_dlist_append(list, node);
		}
	}
}

static inline void sys_dlist_remove(sys_dnode_t *node)
{
	sys_dnode_t *const prev = node->prev;
	sys_dnode_t *const next = node->next;

	prev->next = next;
	next->prev = prev;
	sys_dnode_init(node);
}

static inline sys_dnode_t *sys_dlist_get(sys_dlist_t *list)
{
	sys_dnode_t *node = NULL;

	if (!sys_dlist_is_empty(list)) {
		node = list->head;
		sys_dlist_remove(node);
	}

	return node;
}

#define SYS_DLIST_FOR_EACH_NODE(__dl, __dn)				\
	for (__dn = sys_dlist_peek_head(__dl); __dn != NULL;		\
	     __dn = sys_dlist_peek_next(__dl, __dn))

static inline size_t sys_dlist_len(sys_dlist_t *list)
{
	size_t len = 0;
	sys_dnode_t *node = NULL;

	SYS_DLIST_FOR_EACH_NODE(list, node) {
		len++;
	}
	return len;
}

#define SYS_DLIST_ITERATE_FROM_NODE(__dl, __dn) \
	for (__dn = __dn ? sys_dlist_peek_next_no_check(__dl, __dn) \
			 : sys_dlist_peek_head(__dl); \
	     __dn != NULL; \
	     __dn = sys_dlist_peek_next(__dl, __dn))

#define SYS_DLIST_FOR_EACH_NODE_SAFE(__dl, __dn, __dns)			\
	for (__dn = sys_dlist_peek_head(__dl),				\
		     __dns = sys_dlist_peek_next(__dl, __dn);		\
	     __dn != NULL; __dn = __dns,				\
		     __dns = sys_dlist_peek_next(__dl, __dn))

/**
 * @brief Provide the primitive to resolve the container of a list node
 * Note: it is safe to use with NULL pointer nodes
 *
 * @param __dn A pointer on a sys_dnode_t to get its container
 * @param __cn Container struct type pointer
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_CONTAINER(__dn, __cn, __n) \
	((__dn != NULL) ? CONTAINER_OF(__dn, __typeof__(*__cn), __n) : NULL)

/**
 * @brief Provide the primitive to peek container of the list head
 *
 * @param __dl A pointer on a sys_dlist_t to peek
 * @param __cn Container struct type pointer
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_PEEK_HEAD_CONTAINER(__dl, __cn, __n) \
	SYS_DLIST_CONTAINER(sys_dlist_peek_head(__dl), __cn, __n)

/**
 * @brief Provide the primitive to peek the next container
 *
 * @param __dl A pointer on a sys_dlist_t to peek
 * @param __cn Container struct type pointer
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_PEEK_NEXT_CONTAINER(__dl, __cn, __n) \
	((__cn != NULL) ? \
	 SYS_DLIST_CONTAINER(sys_dlist_peek_next(__dl, &(__cn->__n)),	\
				      __cn, __n) : NULL)

/**
 * @brief Provide the primitive to iterate on a list under a container
 * Note: the loop is unsafe and thus __cn should not be detached
 *
 * User _MUST_ add the loop statement curly braces enclosing its own code:
 *
 *     SYS_DLIST_FOR_EACH_CONTAINER(l, c, n) {
 *         <user code>
 *     }
 *
 * @param __dl A pointer on a sys_dlist_t to iterate on
 * @param __cn A pointer to peek each entry of the list
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_FOR_EACH_CONTAINER(__dl, __cn, __n)			\
	for (__cn = SYS_DLIST_PEEK_HEAD_CONTAINER(__dl, __cn, __n);     \
	     __cn != NULL;                                              \
	     __cn = SYS_DLIST_PEEK_NEXT_CONTAINER(__dl, __cn, __n))

/**
 * @brief Provide the primitive to safely iterate on a list under a container
 * Note: __cn can be detached, it will not break the loop.
 *
 * User _MUST_ add the loop statement curly braces enclosing its own code:
 *
 *     SYS_DLIST_FOR_EACH_CONTAINER_SAFE(l, c, cn, n) {
 *         <user code>
 *     }
 *
 * @param __dl A pointer on a sys_dlist_t to iterate on
 * @param __cn A pointer to peek each entry of the list
 * @param __cns A pointer for the loop to run safely
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_FOR_EACH_CONTAINER_SAFE(__dl, __cn, __cns, __n)	\
	for (__cn = SYS_DLIST_PEEK_HEAD_CONTAINER(__dl, __cn, __n),	\
	     __cns = SYS_DLIST_PEEK_NEXT_CONTAINER(__dl, __cn, __n);    \
	     __cn != NULL; __cn = __cns,				\
	     __cns = SYS_DLIST_PEEK_NEXT_CONTAINER(__dl, __cn, __n))

#ifdef __cplusplus
}
#endif
