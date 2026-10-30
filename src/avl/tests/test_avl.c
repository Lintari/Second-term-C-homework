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

enum { ITEM_COUNT = 96 };

static int check_node(const AVLNode* node, const char* lower, const char* upper, size_t* count)
{
    if (node == NULL) {
        return 0;
    }

    CHECK(lower == NULL || strcmp(node->iata_code, lower) > 0);
    CHECK(upper == NULL || strcmp(node->iata_code, upper) < 0);

    int left_height = check_node(node->left, lower, node->iata_code, count);
    int right_height = check_node(node->right, node->iata_code, upper, count);
    int expected_balance = right_height - left_height;

    CHECK(expected_balance >= -1 && expected_balance <= 1);
    CHECK(node->balance == expected_balance);
    (*count)++;

    return (left_height > right_height ? left_height : right_height) + 1;
}

static void check_tree(const AVLTree* tree, size_t expected_size)
{
    size_t count = 0;
    check_node(tree->root, NULL, NULL, &count);
    CHECK(count == expected_size);
    CHECK(tree->size == expected_size);
}

static void test_insert_rotations(void)
{
    static const char* const cases[][3] = {
        { "C", "B", "A" }, // LL
        { "A", "B", "C" }, // RR
        { "C", "A", "B" }, // LR
        { "A", "C", "B" } // RL
    };

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        AVLTree tree;
        avl_tree_init(&tree);

        for (size_t j = 0; j < 3; ++j) {
            CHECK(avl_insert(&tree, cases[i][j], cases[i][j]));
            check_tree(&tree, j + 1);
        }

        CHECK(strcmp(tree.root->iata_code, "B") == 0);
        avl_tree_delete(&tree);
        CHECK(tree.root == NULL && tree.size == 0);
    }
}

static unsigned int next_random(unsigned int* state)
{
    *state = *state * 1103515245u + 12345u;
    return *state;
}

static void shuffle(int* values, size_t count, unsigned int* state)
{
    for (size_t i = count; i > 1; --i) {
        size_t j = next_random(state) % i;
        int temp = values[i - 1];
        values[i - 1] = values[j];
        values[j] = temp;
    }
}

static void test_insert_find(void)
{
    AVLTree tree;
    int order[ITEM_COUNT];
    unsigned int random_state = 7;
    avl_tree_init(&tree);

    CHECK(!avl_insert(&tree, NULL, "Invalid"));
    CHECK(!avl_insert(&tree, "BAD", NULL));
    CHECK(avl_find(&tree, NULL) == NULL);
    CHECK(!avl_delete(&tree, NULL));

    for (int i = 0; i < ITEM_COUNT; ++i) {
        order[i] = i;
    }
    shuffle(order, ITEM_COUNT, &random_state);

    for (size_t i = 0; i < ITEM_COUNT; ++i) {
        char code[16];
        char name[24];
        snprintf(code, sizeof(code), "K%03d", order[i]);
        snprintf(name, sizeof(name), "Airport %03d", order[i]);
        CHECK(avl_insert(&tree, code, name));
        check_tree(&tree, i + 1);
    }

    CHECK(!avl_insert(&tree, "K010", "Duplicate"));
    CHECK(tree.size == ITEM_COUNT);
    const AVLNode* existing = avl_find(&tree, "K010");
    CHECK(existing != NULL);
    CHECK(strcmp(existing->name, "Airport 010") == 0);
    CHECK(avl_find(&tree, "missing") == NULL);
    CHECK(!avl_delete(&tree, "missing"));
    check_tree(&tree, ITEM_COUNT);

    avl_tree_delete(&tree);
}

static void test_delete(void)
{
    AVLTree tree;
    int order[ITEM_COUNT];
    unsigned int random_state = 19;
    avl_tree_init(&tree);

    for (int i = 0; i < ITEM_COUNT; ++i) {
        order[i] = i;
    }
    shuffle(order, ITEM_COUNT, &random_state);

    for (size_t i = 0; i < ITEM_COUNT; ++i) {
        char code[16];
        char name[24];
        snprintf(code, sizeof(code), "K%03d", order[i]);
        snprintf(name, sizeof(name), "Airport %03d", order[i]);
        CHECK(avl_insert(&tree, code, name));
    }
    check_tree(&tree, ITEM_COUNT);

    shuffle(order, ITEM_COUNT, &random_state);
    for (size_t i = 0; i < ITEM_COUNT; ++i) {
        char code[16];
        snprintf(code, sizeof(code), "K%03d", order[i]);
        CHECK(avl_delete(&tree, code));
        check_tree(&tree, ITEM_COUNT - i - 1);
        CHECK(avl_find(&tree, code) == NULL);
    }

    CHECK(tree.root == NULL);
    CHECK(!avl_delete(&tree, "K000"));
    avl_tree_delete(&tree);
}

int main(int argc, char** argv)
{
    CHECK(argc == 2);

    if (strcmp(argv[1], "rotations") == 0) {
        test_insert_rotations();
    } else if (strcmp(argv[1], "insert_find") == 0) {
        test_insert_find();
    } else if (strcmp(argv[1], "delete") == 0) {
        test_delete();
    } else {
        CHECK(false);
    }

    printf("AVL test '%s' passed\n", argv[1]);
    return EXIT_SUCCESS;
}
