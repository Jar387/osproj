#include <lib/list.h>

void
init_list(struct list_node *head)
{
	head->prev = NULL;
	head->next = NULL;
}

void
insert_next(struct list_node *this, struct list_node *insertee)
{
	insertee->prev = this;
	insertee->next = this->next;
	this->next->prev = insertee;
	this->next = insertee;
}

void
delete_this(struct list_node *deletee, void (*callback)(void *))
{
	deletee->prev->next = deletee->next;
	deletee->next->prev = deletee->prev;
	if (callback != NULL) {
		callback(deletee);
	}
}

void
decompose_list(struct list_node *head, void (*callback)(void *))
{
	head->prev->next = NULL;
	list_foreach(head, callback);
}

void
list_foreach(struct list_node *head, void (*callback)(void *))
{
	struct list_node *idx = head;
	while (idx != NULL) {
		callback(idx);
		idx = idx->next;
	}
}

void
list_foreach_reverse(struct list_node *head, void (*callback)(void *))
{
	struct list_node *idx = head;
	while (idx != NULL) {
		callback(idx);
		idx = idx->prev;
	}
}
