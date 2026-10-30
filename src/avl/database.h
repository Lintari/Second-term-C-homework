#pragma once

#include "avl_tree.h"
#include <stdbool.h>

/* The tree must be initialized. Load code:name records, replacing it on success. */
bool databaseLoad(AVLTree* tree, const char* filename);
