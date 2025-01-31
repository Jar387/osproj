#include <lib/bst.h>
#include <stddef.h>
#include <mm/slab.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Function to calculate the height of a node
static int height(bst_node_t * node) {
    return node == NULL ? -1 : node->height;
}

// Function to calculate the balance factor of a node
static int balance_factor(bst_node_t * node) {
    return node == NULL ? 0 : height(node->child1) - height(node->child2);
}

// Right rotation
static bst_node_t *rotate_right(bst_node_t * y) {
    bst_node_t *x = y->child1;
    bst_node_t *T2 = x->child2;

    // Perform rotation
    x->child2 = y;
    y->child1 = T2;

    // Update heights
    y->height = MAX(height(y->child1), height(y->child2)) + 1;
    x->height = MAX(height(x->child1), height(x->child2)) + 1;

    return x;
}

// Left rotation
static bst_node_t *rotate_left(bst_node_t * x) {
    bst_node_t *y = x->child2;
    bst_node_t *T2 = y->child1;

    // Perform rotation
    y->child1 = x;
    x->child2 = T2;

    // Update heights
    x->height = MAX(height(x->child1), height(x->child2)) + 1;
    y->height = MAX(height(y->child1), height(y->child2)) + 1;

    return y;
}

// Function to balance the tree at a given node
static bst_node_t *balance(bst_node_t * node) {
    int balance = balance_factor(node);

    // Left heavy (Right rotation)
    if (balance > 1) {
        if (balance_factor(node->child1) < 0) { // Left-Right case
            node->child1 = rotate_left(node->child1);
        }
        node = rotate_right(node);
    }
    // Right heavy (Left rotation)
    else if (balance < -1) {
        if (balance_factor(node->child2) > 0) { // Right-Left case
            node->child2 = rotate_right(node->child2);
        }
        node = rotate_left(node);
    }

    return node;
}

// Function to insert a node into the AVL tree
bst_node_t *bst_insert(bst_node_t ** root, void *data, unsigned int id) {
    if (*root == NULL) {
        *root = (bst_node_t *)kmalloc(sizeof(bst_node_t));
        if (*root == NULL) return NULL;
        (*root)->id = id;
        (*root)->data = data;
        (*root)->height = 0;
        (*root)->parent = (*root)->child1 = (*root)->child2 = NULL;
        return *root;
    }

    bst_node_t *node;
    if (id < (*root)->id) {
        node = bst_insert(&(*root)->child1, data, id);
        (*root)->child1->parent = *root;
    } else if (id > (*root)->id) {
        node = bst_insert(&(*root)->child2, data, id);
        (*root)->child2->parent = *root;
    } else {
        // Duplicate ids are not allowed
        return NULL;
    }

    // Update height of this ancestor node
    (*root)->height = MAX(height((*root)->child1), height((*root)->child2)) + 1;

    // Balance the node
    return balance(*root);
}

// Function to find the node with the minimum value
static bst_node_t *min_value_node(bst_node_t * node) {
    bst_node_t *current = node;
    while (current->child1 != NULL) {
        current = current->child1;
    }
    return current;
}

// Function to delete a node
static bst_node_t *__bst_delete(bst_node_t ** root, unsigned int id) {
    if (*root == NULL) return *root;

    if (id < (*root)->id) {
        (*root)->child1 = bst_delete(&(*root)->child1, id);
    } else if (id > (*root)->id) {
        (*root)->child2 = bst_delete(&(*root)->child2, id);
    } else {
        // Node to be deleted found
        if ((*root)->child1 == NULL) {
            bst_node_t *temp = (*root)->child2;
            kfree(*root);
            *root = temp;
        } else if ((*root)->child2 == NULL) {
            bst_node_t *temp = (*root)->child1;
            kfree(*root);
            *root = temp;
        } else {
            // Node has two children, get the inorder successor
            bst_node_t *temp = min_value_node((*root)->child2);
            (*root)->id = temp->id;
            (*root)->data = temp->data;
            (*root)->child2 = bst_delete(&(*root)->child2, temp->id);
        }
    }

    if (*root == NULL) return *root;

    // Update height of the current node
    (*root)->height = MAX(height((*root)->child1), height((*root)->child2)) + 1;

    // Balance the node
    return balance(*root);
}

void *bst_delete(bst_node_t ** root, unsigned int id){
    bst_node_t* node = __bst_delete(root, id);
    if(node!=NULL){
        return (node->data);
    }else{
        return NULL;
    }
}

// Function to search for a node with a specific id
static bst_node_t *__bst_search(bst_node_t * root, unsigned int id) {
    while (root != NULL) {
        if (id < root->id) {
            root = root->child1;
        } else if (id > root->id) {
            root = root->child2;
        } else {
            return root;
        }
    }
    return NULL;
}

void* bst_search(bst_node_t* root, unsigned int id){
    bst_node_t* node = __bst_search(root, id);
    if(node!=NULL){
        return (node->data);
    }else{
        return NULL;
    }
}
