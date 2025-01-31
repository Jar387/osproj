#ifndef BST_H
#define BST_H

typedef struct bst_node_t {
	struct bst_node_t *parent;
	struct bst_node_t *child1;
	struct bst_node_t *child2;
	unsigned int id;
	void *data;
	int height;
} bst_node_t;

bst_node_t *bst_insert(bst_node_t ** root, void *data, unsigned int id);
void *bst_delete(bst_node_t ** root, unsigned int id);
void *bst_search(bst_node_t * root, unsigned int id);

#endif
