#ifndef LIST_H
#define LIST_H

#include <stddef.h>

#define PARENT_PTR(child_ptr, parent_type, child_member) \
    ((parent_type *)((char *)(child_ptr) - offsetof(parent_type, child_member)))

struct list_node {
	struct list_node *prev;
	struct list_node *next;
};

void init_list(struct list_node *head);
void insert_next(struct list_node *this, struct list_node *insertee);
void delete_this(struct list_node *deletee, void (*callback)(void *));
void decompose_list(struct list_node *head, void (*callback)(void *));
void list_foreach(struct list_node *head, void (*callback)(void *));
void list_foreach_reverse(struct list_node *head, void (*callback)(void *));

#endif
