#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition)                                                         \
    do {                                                                         \
        if (!(condition)) {                                                      \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
            exit(EXIT_FAILURE);                                                  \
        }                                                                        \
    } while (0)

static void test_successful_load(void)
{
    AVLTree tree;
    avl_tree_init(&tree);

    CHECK(avl_insert(&tree, "OLD", "Old record"));
    CHECK(database_load(&tree, TEST_AIRPORTS_FILE));
    CHECK(tree.size == 4);
    CHECK(avl_find(&tree, "OLD") == NULL);
    const AVLNode* node = avl_find(&tree, "SVO");
    CHECK(node != NULL);
    CHECK(strcmp(node->name, "Sheremetyevo Airport") == 0);
    node = avl_find(&tree, "LED");
    CHECK(node != NULL);
    CHECK(strcmp(node->name, "Pulkovo: north terminal") == 0);
    node = avl_find(&tree, "DUP");
    CHECK(node != NULL);
    CHECK(strcmp(node->name, "First occurrence") == 0);

    avl_tree_delete(&tree);
    CHECK(tree.root == NULL && tree.size == 0);
}

static void test_failed_load(void)
{
    AVLTree tree;
    avl_tree_init(&tree);

    CHECK(avl_insert(&tree, "KEEP", "Keep this record"));
    CHECK(!database_load(&tree, ""));
    CHECK(tree.size == 1);
    const AVLNode* node = avl_find(&tree, "KEEP");
    CHECK(node != NULL);
    CHECK(strcmp(node->name, "Keep this record") == 0);

    avl_tree_delete(&tree);
    CHECK(tree.root == NULL && tree.size == 0);
}

int main(int argc, char** argv)
{
    CHECK(argc == 2);

    if (strcmp(argv[1], "load") == 0) {
        test_successful_load();
    } else if (strcmp(argv[1], "failure") == 0) {
        test_failed_load();
    } else {
        CHECK(false);
    }

    printf("Database test '%s' passed\n", argv[1]);
    return EXIT_SUCCESS;
}
