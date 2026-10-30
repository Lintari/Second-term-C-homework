#include "avl_tree.h"

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

enum { ItemCount = 96 };

// NOLINTNEXTLINE(misc-no-recursion): Validation computes child heights bottom-up.
static int checkNode(const AVLNode* node, const char* lower, const char* upper, size_t* count)
{
    if (node == NULL) {
        return 0;
    }

    CHECK(lower == NULL || strcmp(node->iata_code, lower) > 0);
    CHECK(upper == NULL || strcmp(node->iata_code, upper) < 0);

    int leftHeight = checkNode(node->left, lower, node->iata_code, count);
    int rightHeight = checkNode(node->right, node->iata_code, upper, count);
    int expectedBalance = rightHeight - leftHeight;

    CHECK(expectedBalance >= -1 && expectedBalance <= 1);
    CHECK(node->balance == expectedBalance);
    (*count)++;

    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

static void checkTree(const AVLTree* tree, size_t expectedSize)
{
    size_t count = 0;
    checkNode(tree->root, NULL, NULL, &count);
    CHECK(count == expectedSize);
    CHECK(tree->size == expectedSize);
}

static void testInsertRotations(void)
{
    static const char* const CASES[][3] = {
        { "C", "B", "A" }, // LL
        { "A", "B", "C" }, // RR
        { "C", "A", "B" }, // LR
        { "A", "C", "B" } // RL
    };

    for (size_t i = 0; i < sizeof(CASES) / sizeof(CASES[0]); ++i) {
        AVLTree tree;
        avlTreeInit(&tree);

        for (size_t j = 0; j < 3; ++j) {
            CHECK(avlInsert(&tree, CASES[i][j], CASES[i][j]));
            checkTree(&tree, j + 1);
        }

        CHECK(strcmp(tree.root->iata_code, "B") == 0);
        avlTreeDelete(&tree);
        CHECK(tree.root == NULL && tree.size == 0);
    }
}

static unsigned int nextRandom(unsigned int* state)
{
    *state = *state * 1103515245u + 12345u;
    return *state;
}

static void shuffle(int* values, size_t count, unsigned int* state)
{
    for (size_t i = count; i > 1; --i) {
        size_t j = nextRandom(state) % i;
        int temp = values[i - 1];
        values[i - 1] = values[j];
        values[j] = temp;
    }
}

static void testInsertFind(void)
{
    AVLTree tree;
    int order[ItemCount];
    unsigned int randomState = 7;
    avlTreeInit(&tree);

    CHECK(!avlInsert(&tree, NULL, "Invalid"));
    CHECK(!avlInsert(&tree, "BAD", NULL));
    CHECK(avlFind(&tree, NULL) == NULL);
    CHECK(!avlDelete(&tree, NULL));

    for (int i = 0; i < ItemCount; ++i) {
        order[i] = i;
    }
    shuffle(order, ItemCount, &randomState);

    for (size_t i = 0; i < ItemCount; ++i) {
        char code[16];
        char name[24];
        snprintf(code, sizeof(code), "K%03d", order[i]);
        snprintf(name, sizeof(name), "Airport %03d", order[i]);
        CHECK(avlInsert(&tree, code, name));
        checkTree(&tree, i + 1);
    }

    CHECK(!avlInsert(&tree, "K010", "Duplicate"));
    CHECK(tree.size == ItemCount);
    const AVLNode* existing = avlFind(&tree, "K010");
    CHECK(existing != NULL);
    CHECK(strcmp(existing->name, "Airport 010") == 0);
    CHECK(avlFind(&tree, "missing") == NULL);
    CHECK(!avlDelete(&tree, "missing"));
    checkTree(&tree, ItemCount);

    avlTreeDelete(&tree);
}

static void testDelete(void)
{
    AVLTree tree;
    int order[ItemCount];
    unsigned int randomState = 19;
    avlTreeInit(&tree);

    for (int i = 0; i < ItemCount; ++i) {
        order[i] = i;
    }
    shuffle(order, ItemCount, &randomState);

    for (size_t i = 0; i < ItemCount; ++i) {
        char code[16];
        char name[24];
        snprintf(code, sizeof(code), "K%03d", order[i]);
        snprintf(name, sizeof(name), "Airport %03d", order[i]);
        CHECK(avlInsert(&tree, code, name));
    }
    checkTree(&tree, ItemCount);

    shuffle(order, ItemCount, &randomState);
    for (size_t i = 0; i < ItemCount; ++i) {
        char code[16];
        snprintf(code, sizeof(code), "K%03d", order[i]);
        CHECK(avlDelete(&tree, code));
        checkTree(&tree, ItemCount - i - 1);
        CHECK(avlFind(&tree, code) == NULL);
    }

    CHECK(tree.root == NULL);
    CHECK(!avlDelete(&tree, "K000"));
    avlTreeDelete(&tree);
}

int main(int argc, char** argv)
{
    CHECK(argc == 2);

    if (strcmp(argv[1], "rotations") == 0) {
        testInsertRotations();
    } else if (strcmp(argv[1], "insert_find") == 0) {
        testInsertFind();
    } else if (strcmp(argv[1], "delete") == 0) {
        testDelete();
    } else {
        CHECK(false);
    }

    printf("AVL test '%s' passed\n", argv[1]);
    return EXIT_SUCCESS;
}
