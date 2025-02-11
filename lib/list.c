#include <lib/list.h>
#include <printk.h>
#include <stddef.h>
#include <mm/slab.h>

list_node_t *
list_insert(list_node_t ** head, void *data)
{
	if (head == NULL) {
		return NULL;
	}
	list_node_t *node = (list_node_t *) kmalloc(sizeof (*node));
	node->data = data;
	node->prev = NULL;
	node->next = NULL;

	if ((*head) == NULL) {
		node->prev = node;
		node->next = node;
		(*head) = node;
	} else {
		node->next = (*head)->next;
		node->prev = (*head);
		(*head)->next->prev = node;
		(*head)->next = node;

	}
	return node;
}

void
list_delete(list_node_t * obj)
{
	if (obj == NULL) {
		return;
	}
	if (obj->prev != NULL && obj->next != NULL) {
		obj->prev->next = obj->next;
		obj->next->prev = obj->prev;
	}
	kfree(obj);
	obj->next = NULL;
	obj->prev = NULL;
	obj->data = NULL;
}

void
list_iter(list_node_t * head, int (*callback)(list_node_t *))
{
	if (head == NULL) {
		return;
	}

	list_node_t *curr = head;
	do {
		if (!curr)
			break;
		if (callback(curr)) {
			break;
		}
		curr = curr->next;
	} while (curr && curr != head);
}
