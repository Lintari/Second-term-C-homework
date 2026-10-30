#include "avl_tree.h"
#include <stdlib.h>
#include <string.h>

static char* copy_str(const char* source) // for copying
{
    if (source == NULL) {
        return NULL;
    }
    size_t length = strlen(source) + 1; // bcz there's /0 in the end
    char* copy = malloc(length);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, source, length);
    return copy;
}

static AVLNode* create_node(const char* iata_code, const char* name)
{
    AVLNode* node = malloc(sizeof(*node));
    if (node == NULL) {
        return NULL;
    }
    node->iata_code = copy_str(iata_code);
    node->name = copy_str(name);
    node->balance = 0;
    node->left = NULL;
    node->right = NULL;
    if (node->iata_code == NULL || node->name == NULL) {
        free(node->iata_code);
        free(node->name);
        free(node);
        return NULL;
    }
    return node;
}

static AVLNode* rotate_left(AVLNode* root)
{
    AVLNode* new_root = root->right;
    int child_balance = new_root->balance;

    root->right = new_root->left;
    new_root->left = root;

    if (child_balance == 0) {
        root->balance = 1;
        new_root->balance = -1;
    } else {
        root->balance = 0;
        new_root->balance = 0;
    }

    return new_root;
}

static AVLNode* rotate_right(AVLNode* root)
{
    AVLNode* new_root = root->left;
    int child_balance = new_root->balance;

    root->left = new_root->right;
    new_root->right = root;

    if (child_balance == 0) {
        root->balance = -1;
        new_root->balance = 1;
    } else {
        root->balance = 0;
        new_root->balance = 0;
    }

    return new_root;
}

static AVLNode* rebalance(AVLNode* root, bool* height_shrank) // shrank bcz height might changed after deletions
{
    if (root == NULL) {
        return NULL;
    }

    if (height_shrank != NULL) {
        if (root->balance == 0) {
            *height_shrank = true;
            return root;
        }
        if (root->balance == 1 || root->balance == -1) {
            *height_shrank = false;
            return root;
        }
    }

    if (root->balance == 2) {
        int child_balance = root->right->balance;
        if (root->right->balance >= 0) {
            root = rotate_left(root);
            if (height_shrank != NULL) {
                *height_shrank = child_balance != 0;
            }
        } else {
            int middle_balance = root->right->left->balance;

            root->right = rotate_right(root->right);
            root = rotate_left(root);

            root->balance = 0;

            if (middle_balance == -1) {
                root->left->balance = 0;
                root->right->balance = 1;
            } else if (middle_balance == 0) {
                root->left->balance = 0;
                root->right->balance = 0;
            } else {
                root->left->balance = -1;
                root->right->balance = 0;
            }
            if (height_shrank != NULL) {
                *height_shrank = true;
            }
        }
    } else if (root->balance == -2) {
        int child_balance = root->left->balance;
        if (root->left->balance <= 0) {
            root = rotate_right(root);
            if (height_shrank != NULL) {
                *height_shrank = child_balance != 0;
            }
        } else {
            int middle_balance = root->left->right->balance;

            root->left = rotate_left(root->left);
            root = rotate_right(root);

            root->balance = 0;

            if (middle_balance == -1) {
                root->left->balance = 0;
                root->right->balance = 1;
            } else if (middle_balance == 0) {
                root->left->balance = 0;
                root->right->balance = 0;
            } else {
                root->left->balance = -1;
                root->right->balance = 0;
            }
            if (height_shrank != NULL) {
                *height_shrank = true;
            }
        }
    }

    return root;
}

static AVLNode* remove_min(AVLNode* root, AVLNode** min_node, bool* height_shrank)
{
    if (root->left == NULL) {
        *min_node = root;
        *height_shrank = true;
        return root->right;
    }

    root->left = remove_min(root->left, min_node, height_shrank);
    if (*height_shrank) {
        root->balance++;
        root = rebalance(root, height_shrank);
    }

    return root;
}

static AVLNode* insert_recursive(AVLNode* root, const char* iata_code, const char* name, bool* inserted, bool* height_grew)
{
    if (root == NULL) {
        AVLNode* new_node = create_node(iata_code, name);

        if (new_node == NULL) {
            *inserted = false;
            *height_grew = false;
            return NULL;
        }

        *inserted = true;
        *height_grew = true;

        return new_node;
    }

    int cmp = strcmp(iata_code, root->iata_code);

    if (cmp == 0) {
        *inserted = false;
        *height_grew = false;
        return root;
    }
    if (cmp < 0) {
        root->left = insert_recursive(root->left, iata_code, name, inserted, height_grew);
        if (!*height_grew) {
            return root;
        }

        root->balance--;
    } else {
        root->right = insert_recursive(root->right, iata_code, name, inserted, height_grew);

        if (!*height_grew) {
            return root;
        }

        root->balance++;
    }

    if (root->balance == 0) {
        *height_grew = false;
    } else if (root->balance == 2 || root->balance == -2) {
        root = rebalance(root, NULL);
        *height_grew = false; // it keeps the height
    }

    return root;
}
static AVLNode* delete_recursive(AVLNode* root, const char* iata_code, bool* deleted, bool* height_shrank) // height sharnk is a check has height changed or no
{
    if (root == NULL) {
        *deleted = false;
        *height_shrank = false;
        return NULL;
    }

    int cmp = strcmp(iata_code, root->iata_code);

    if (cmp < 0) {
        root->left = delete_recursive(root->left, iata_code, deleted, height_shrank);
        if (*height_shrank) {
            root->balance++;
            root = rebalance(root, height_shrank);
        }
    } else if (cmp > 0) {
        root->right = delete_recursive(root->right, iata_code, deleted, height_shrank);
        if (*height_shrank) {
            root->balance--;
            root = rebalance(root, height_shrank);
        }
    } else {
        if (root->left == NULL) {
            AVLNode* child = root->right;

            free(root->iata_code);
            free(root->name);
            free(root);

            *deleted = true;
            *height_shrank = true;

            return child;
        } else if (root->right == NULL) {
            AVLNode* child = root->left;

            free(root->iata_code);
            free(root->name);
            free(root);

            *deleted = true;
            *height_shrank = true;

            return child;
        } else {
            AVLNode* heir = NULL;
            root->right = remove_min(root->right, &heir, height_shrank);

            free(root->iata_code);
            free(root->name);
            root->iata_code = heir->iata_code;
            root->name = heir->name;
            free(heir);

            *deleted = true;
            if (*height_shrank) {
                root->balance--;
                root = rebalance(root, height_shrank);
            }
        }
    }

    return root;
}

const AVLNode* avl_find(const AVLTree* tree, const char* iata_code)
{
    if (tree == NULL || iata_code == NULL) {
        return NULL;
    }

    AVLNode* current = tree->root;

    while (current != NULL) {
        int cmp = strcmp(iata_code, current->iata_code);

        if (cmp == 0) {
            return current;
        } else if (cmp < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    return NULL;
}

bool avl_insert(AVLTree* tree, const char* iata_code, const char* name) // addinfg  new elemtn
{
    if (tree == NULL || iata_code == NULL || name == NULL) {
        return false;
    }

    bool inserted = false;
    bool height_grew = false;

    tree->root = insert_recursive(tree->root, iata_code, name, &inserted, &height_grew);

    if (inserted) {
        tree->size++;
    }

    return inserted;
}

bool avl_delete(AVLTree* tree, const char* iata_code)
{
    if (tree == NULL || iata_code == NULL) {
        return false;
    }

    bool deleted = false;
    bool height_shrank = false;

    tree->root = delete_recursive(
        tree->root, iata_code, &deleted, &height_shrank);

    if (deleted) {
        tree->size--;
    }

    return deleted;
}

static void free_subtree(AVLNode* root)
{
    if (root == NULL) {
        return;
    }

    free_subtree(root->left);
    free_subtree(root->right);
    free(root->iata_code);
    free(root->name);
    free(root);
}

void avl_tree_delete(AVLTree* tree)
{
    if (tree == NULL) {
        return;
    }

    free_subtree(tree->root);
    tree->root = NULL;
    tree->size = 0;
}

void avl_tree_init(AVLTree* tree)
{
    if (tree == NULL) {
        return;
    }

    tree->root = NULL;
    tree->size = 0;
    return;
}
