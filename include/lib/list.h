#ifndef LIST_H
#define LIST_H

typedef struct list_node_t {
	void *data;
	struct list_node_t *prev;
	struct list_node_t *next;
} list_node_t;

list_node_t *list_insert(list_node_t * head, void *data);
void list_delete(list_node_t * obj);
void list_iter(list_node_t * head, void (*callback)(list_node_t *));

#endif
