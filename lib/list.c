#include <lib/list.h>
#include <printk.h>
#include <stddef.h>
#include <mm/slab.h>

list_node_t *
list_insert(list_node_t * head, void *data)
{
	list_node_t *node = (list_node_t *) kmalloc(sizeof (*node));
	node->data = data;
	if (head == NULL) {
		// new list
		node->prev = NULL;
		node->next = NULL;
	} else {
		node->next = head->next;
		head->next->prev = node;
		head->next = node;
		node->prev = head;
	}
	return node;
}

void
list_delete(list_node_t * obj)
{
	if (obj->prev != NULL && obj->next != NULL) {
		obj->prev->next = obj->next;
		obj->next->prev = obj->prev;
	}
	kfree(obj);
}

void
list_iter(list_node_t * head, void (*callback)(list_node_t *))
{
	if(head == NULL) {
		return;
	}
	list_node_t *curr = head;
	do {
		callback(curr);
		curr = curr->next;
	} while (curr != head);
}
