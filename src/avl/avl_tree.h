#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct AVLNode {
    char* iata_code;
    char* name;
    int balance;
    struct AVLNode* left;
    struct AVLNode* right;
} AVLNode;

typedef struct {
    AVLNode* root;
    size_t size;
} AVLTree;

void avl_tree_init(AVLTree* tree);

bool avl_insert(AVLTree* tree, const char* iata_code, const char* name);

const AVLNode* avl_find(const AVLTree* tree, const char* iata_code);

bool avl_delete(AVLTree* tree, const char* iata_code);

void avl_tree_delete(AVLTree* tree);
