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

static void testSuccessfulLoad(void)
{
    AVLTree tree;
    avlTreeInit(&tree);

    CHECK(avlInsert(&tree, "OLD", "Old record"));
    CHECK(databaseLoad(&tree, TEST_AIRPORTS_FILE));
    CHECK(tree.size == 4);
    CHECK(avlFind(&tree, "OLD") == NULL);
    const AVLNode* node = avlFind(&tree, "SVO");
    CHECK(node != NULL);
    CHECK(strcmp(node->name, "Sheremetyevo Airport") == 0);
    node = avlFind(&tree, "LED");
    CHECK(node != NULL);
    CHECK(strcmp(node->name, "Pulkovo: north terminal") == 0);
    node = avlFind(&tree, "DUP");
    CHECK(node != NULL);
    CHECK(strcmp(node->name, "First occurrence") == 0);

    avlTreeDelete(&tree);
    CHECK(tree.root == NULL && tree.size == 0);
}

static void testFailedLoad(void)
{
    AVLTree tree;
    avlTreeInit(&tree);

    CHECK(avlInsert(&tree, "KEEP", "Keep this record"));
    CHECK(!databaseLoad(&tree, ""));
    CHECK(tree.size == 1);
    const AVLNode* node = avlFind(&tree, "KEEP");
    CHECK(node != NULL);
    CHECK(strcmp(node->name, "Keep this record") == 0);

    avlTreeDelete(&tree);
    CHECK(tree.root == NULL && tree.size == 0);
}

int main(int argc, char** argv)
{
    CHECK(argc == 2);

    if (strcmp(argv[1], "load") == 0) {
        testSuccessfulLoad();
    } else if (strcmp(argv[1], "failure") == 0) {
        testFailedLoad();
    } else {
        CHECK(false);
    }

    printf("Database test '%s' passed\n", argv[1]);
    return EXIT_SUCCESS;
}
