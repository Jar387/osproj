#include <lib/bst.h>
#include <printk.h>
#include <stddef.h>
#include <mm/slab.h>

int
height(bst_node_t * node)
{
	return node ? node->height : 0;
}

int
balance_factor(bst_node_t * node)
{
	return node ? height(node->child1) - height(node->child2) : 0;
}

void
update_height(bst_node_t * node)
{
	if (node) {
		node->height =
		    1 + ((height(node->child1) > height(node->child2))
			 ? height(node->child1)
			 : height(node->child2));
	}
}

// Right Rotation (LL case)
bst_node_t *
rotate_right(bst_node_t * y)
{
	bst_node_t *x = y->child1;
	bst_node_t *T2 = x->child2;

	x->child2 = y;
	y->child1 = T2;

	if (T2)
		T2->parent = y;
	x->parent = y->parent;
	y->parent = x;

	update_height(y);
	update_height(x);
	return x;		// New root
}

// Left Rotation (RR case)
bst_node_t *
rotate_left(bst_node_t * x)
{
	bst_node_t *y = x->child2;
	bst_node_t *T2 = y->child1;

	y->child1 = x;
	x->child2 = T2;

	if (T2)
		T2->parent = x;
	y->parent = x->parent;
	x->parent = y;

	update_height(x);
	update_height(y);
	return y;		// New root
}

// Balance function
bst_node_t *
do_balance(bst_node_t * node)
{
	update_height(node);
	int balance = balance_factor(node);

	// Left Heavy (LL or LR)
	if (balance > 1) {
		if (balance_factor(node->child1) < 0) {
			node->child1 = rotate_left(node->child1);	// LR case
		}
		return rotate_right(node);	// LL case
	}
	// Right Heavy (RR or RL)
	if (balance < -1) {
		if (balance_factor(node->child2) > 0) {
			node->child2 = rotate_right(node->child2);	// RL case
		}
		return rotate_left(node);	// RR case
	}

	return node;		// Balanced
}

// Search function
bst_node_t *
bst_search(bst_node_t * root, unsigned int id)
{
	bst_node_t *curr = root;
	while (curr) {
		if (id == curr->id)
			return curr;
		curr = (id < curr->id) ? curr->child1 : curr->child2;
	}
	return NULL;		// Not found
}

// Helper function to find the minimum node in a subtree
bst_node_t *
find_min(bst_node_t * node)
{
	while (node && node->child1) {
		node = node->child1;
	}
	return node;
}

// Delete function with AVL balancing
bst_node_t *
bst_delete(bst_node_t ** root, unsigned int id)
{
	if (!root || !(*root))
		return NULL;

	bst_node_t *curr = *root;
	bst_node_t *parent = NULL;

	// Search for the node to delete
	while (curr && curr->id != id) {
		parent = curr;
		if (id < curr->id) {
			curr = curr->child1;
		} else {
			curr = curr->child2;
		}
	}
	if (!curr)
		return NULL;	// Node not found

	// Case 1: No children (leaf node)
	if (!curr->child1 && !curr->child2) {
		if (parent) {
			if (parent->child1 == curr)
				parent->child1 = NULL;
			else
				parent->child2 = NULL;
		} else {
			*root = NULL;	// Deleting the root node
		}
		kfree(curr);
	}
	// Case 2: One child
	else if (!curr->child1 || !curr->child2) {
		bst_node_t *child = curr->child1 ? curr->child1 : curr->child2;

		if (parent) {
			if (parent->child1 == curr)
				parent->child1 = child;
			else
				parent->child2 = child;
		} else {
			*root = child;	// Deleting root node with one child
		}
		child->parent = parent;
		kfree(curr);
	}
	// Case 3: Two children
	else {
		bst_node_t *successor = find_min(curr->child2);
		curr->id = successor->id;
		curr->data = successor->data;
		return bst_delete(&(curr->child2), successor->id);	// Recursively delete successor
	}

	// Rebalance from parent up to the root
	bst_node_t *rebal = parent;
	while (rebal) {
		rebal = do_balance(rebal);
		if (rebal->parent == NULL) {
			*root = rebal;	// Update root if necessary
			break;
		}
		rebal = rebal->parent;
	}

	return *root;
}
