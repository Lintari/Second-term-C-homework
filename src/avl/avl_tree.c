#include "avl_tree.h"
#include <stdlib.h>
#include <string.h>

static char* copyStr(const char* source) // for copying
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

static AVLNode* createNode(const char* iataCode, const char* name)
{
    AVLNode* node = malloc(sizeof(*node));
    if (node == NULL) {
        return NULL;
    }
    node->iata_code = copyStr(iataCode);
    node->name = copyStr(name);
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

static AVLNode* rotateLeft(AVLNode* root)
{
    AVLNode* newRoot = root->right;
    int childBalance = newRoot->balance;

    root->right = newRoot->left;
    newRoot->left = root;

    if (childBalance == 0) {
        root->balance = 1;
        newRoot->balance = -1;
    } else {
        root->balance = 0;
        newRoot->balance = 0;
    }

    return newRoot;
}

static AVLNode* rotateRight(AVLNode* root)
{
    AVLNode* newRoot = root->left;
    int childBalance = newRoot->balance;

    root->left = newRoot->right;
    newRoot->right = root;

    if (childBalance == 0) {
        root->balance = -1;
        newRoot->balance = 1;
    } else {
        root->balance = 0;
        newRoot->balance = 0;
    }

    return newRoot;
}

static AVLNode* rebalance(AVLNode* root, bool* heightShrank) // shrank bcz height might changed after deletions
{
    if (root == NULL) {
        return NULL;
    }

    if (heightShrank != NULL) {
        if (root->balance == 0) {
            *heightShrank = true;
            return root;
        }
        if (root->balance == 1 || root->balance == -1) {
            *heightShrank = false;
            return root;
        }
    }

    if (root->balance == 2) {
        int childBalance = root->right->balance;
        if (root->right->balance >= 0) {
            root = rotateLeft(root);
            if (heightShrank != NULL) {
                *heightShrank = childBalance != 0;
            }
        } else {
            int middleBalance = root->right->left->balance;

            root->right = rotateRight(root->right);
            root = rotateLeft(root);

            root->balance = 0;

            if (middleBalance == -1) {
                root->left->balance = 0;
                root->right->balance = 1;
            } else if (middleBalance == 0) {
                root->left->balance = 0;
                root->right->balance = 0;
            } else {
                root->left->balance = -1;
                root->right->balance = 0;
            }
            if (heightShrank != NULL) {
                *heightShrank = true;
            }
        }
    } else if (root->balance == -2) {
        int childBalance = root->left->balance;
        if (root->left->balance <= 0) {
            root = rotateRight(root);
            if (heightShrank != NULL) {
                *heightShrank = childBalance != 0;
            }
        } else {
            int middleBalance = root->left->right->balance;

            root->left = rotateLeft(root->left);
            root = rotateRight(root);

            root->balance = 0;

            if (middleBalance == -1) {
                root->left->balance = 0;
                root->right->balance = 1;
            } else if (middleBalance == 0) {
                root->left->balance = 0;
                root->right->balance = 0;
            } else {
                root->left->balance = -1;
                root->right->balance = 0;
            }
            if (heightShrank != NULL) {
                *heightShrank = true;
            }
        }
    }

    return root;
}

// NOLINTNEXTLINE(misc-no-recursion): Removing the leftmost node is a recursive tree traversal.
static AVLNode* removeMin(AVLNode* root, AVLNode** minNode, bool* heightShrank)
{
    if (root->left == NULL) {
        *minNode = root;
        *heightShrank = true;
        return root->right;
    }

    root->left = removeMin(root->left, minNode, heightShrank);
    if (*heightShrank) {
        root->balance++;
        root = rebalance(root, heightShrank);
    }

    return root;
}

// NOLINTNEXTLINE(misc-no-recursion): Recursive insertion follows one AVL search path.
static AVLNode* insertRecursive(AVLNode* root, const char* iataCode, const char* name, bool* inserted, bool* heightGrew)
{
    if (root == NULL) {
        AVLNode* newNode = createNode(iataCode, name);

        if (newNode == NULL) {
            *inserted = false;
            *heightGrew = false;
            return NULL;
        }

        *inserted = true;
        *heightGrew = true;

        return newNode;
    }

    int cmp = strcmp(iataCode, root->iata_code);

    if (cmp == 0) {
        *inserted = false;
        *heightGrew = false;
        return root;
    }
    if (cmp < 0) {
        root->left = insertRecursive(root->left, iataCode, name, inserted, heightGrew);
        if (!*heightGrew) {
            return root;
        }

        root->balance--;
    } else {
        root->right = insertRecursive(root->right, iataCode, name, inserted, heightGrew);

        if (!*heightGrew) {
            return root;
        }

        root->balance++;
    }

    if (root->balance == 0) {
        *heightGrew = false;
    } else if (root->balance == 2 || root->balance == -2) {
        root = rebalance(root, NULL);
        *heightGrew = false; // it keeps the height
    }

    return root;
}
// NOLINTNEXTLINE(misc-no-recursion): Recursive deletion follows one AVL search path.
static AVLNode* deleteRecursive(AVLNode* root, const char* iataCode, bool* deleted, bool* heightShrank) // height sharnk is a check has height changed or no
{
    if (root == NULL) {
        *deleted = false;
        *heightShrank = false;
        return NULL;
    }

    int cmp = strcmp(iataCode, root->iata_code);

    if (cmp < 0) {
        root->left = deleteRecursive(root->left, iataCode, deleted, heightShrank);
        if (*heightShrank) {
            root->balance++;
            root = rebalance(root, heightShrank);
        }
    } else if (cmp > 0) {
        root->right = deleteRecursive(root->right, iataCode, deleted, heightShrank);
        if (*heightShrank) {
            root->balance--;
            root = rebalance(root, heightShrank);
        }
    } else {
        if (root->left == NULL) {
            AVLNode* child = root->right;

            free(root->iata_code);
            free(root->name);
            free(root);

            *deleted = true;
            *heightShrank = true;

            return child;
        } else if (root->right == NULL) {
            AVLNode* child = root->left;

            free(root->iata_code);
            free(root->name);
            free(root);

            *deleted = true;
            *heightShrank = true;

            return child;
        } else {
            AVLNode* heir = NULL;
            root->right = removeMin(root->right, &heir, heightShrank);

            free(root->iata_code);
            free(root->name);
            root->iata_code = heir->iata_code;
            root->name = heir->name;
            free(heir);

            *deleted = true;
            if (*heightShrank) {
                root->balance--;
                root = rebalance(root, heightShrank);
            }
        }
    }

    return root;
}

const AVLNode* avlFind(const AVLTree* tree, const char* iataCode)
{
    if (tree == NULL || iataCode == NULL) {
        return NULL;
    }

    AVLNode* current = tree->root;

    while (current != NULL) {
        int cmp = strcmp(iataCode, current->iata_code);

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

bool avlInsert(AVLTree* tree, const char* iataCode, const char* name) // addinfg  new elemtn
{
    if (tree == NULL || iataCode == NULL || name == NULL) {
        return false;
    }

    bool inserted = false;
    bool heightGrew = false;

    tree->root = insertRecursive(tree->root, iataCode, name, &inserted, &heightGrew);

    if (inserted) {
        tree->size++;
    }

    return inserted;
}

bool avlDelete(AVLTree* tree, const char* iataCode)
{
    if (tree == NULL || iataCode == NULL) {
        return false;
    }

    bool deleted = false;
    bool heightShrank = false;

    tree->root = deleteRecursive(
        tree->root, iataCode, &deleted, &heightShrank);

    if (deleted) {
        tree->size--;
    }

    return deleted;
}

// NOLINTNEXTLINE(misc-no-recursion): Freeing a tree recursively visits both subtrees.
static void freeSubtree(AVLNode* root)
{
    if (root == NULL) {
        return;
    }

    freeSubtree(root->left);
    freeSubtree(root->right);
    free(root->iata_code);
    free(root->name);
    free(root);
}

void avlTreeDelete(AVLTree* tree)
{
    if (tree == NULL) {
        return;
    }

    freeSubtree(tree->root);
    tree->root = NULL;
    tree->size = 0;
}

void avlTreeInit(AVLTree* tree)
{
    if (tree == NULL) {
        return;
    }

    tree->root = NULL;
    tree->size = 0;
}
