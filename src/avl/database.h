#pragma once

#include "avl_tree.h"
#include <stdbool.h>

/* The tree must be initialized. Load code:name records, replacing it on success. */
bool database_load(AVLTree* tree, const char* filename);
