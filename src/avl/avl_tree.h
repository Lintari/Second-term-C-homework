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

void avlTreeInit(AVLTree* tree);

bool avlInsert(AVLTree* tree, const char* iataCode, const char* name);

const AVLNode* avlFind(const AVLTree* tree, const char* iataCode);

bool avlDelete(AVLTree* tree, const char* iataCode);

void avlTreeDelete(AVLTree* tree);
