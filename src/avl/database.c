#include "database.h"

#include <stdio.h>
#include <string.h>

bool database_load(AVLTree* tree, const char* filename)
{
    if (tree == NULL || filename == NULL) {
        return false;
    }

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }

    AVLTree loaded;
    avl_tree_init(&loaded);

    char line[1024];
    bool success = true;

    while (fgets(line, sizeof(line), file) != NULL) {
        size_t length = strlen(line);
        if (length > 0 && line[length - 1] != '\n' && !feof(file)) {
            success = false;
            break;
        }

        while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
            line[--length] = '\0';
        }

        if (length == 0) {
            continue;
        }

        char* separator = strchr(line, ':');
        if (separator == NULL || separator == line) {
            success = false;
            break;
        }

        *separator = '\0';
        const char* iata_code = line;
        const char* name = separator + 1;

        if (avl_find(&loaded, iata_code) != NULL) {
            continue;
        }
        if (!avl_insert(&loaded, iata_code, name)) {
            success = false;
            break;
        }
    }

    if (ferror(file)) {
        success = false;
    }
    fclose(file);

    if (!success) {
        avl_tree_delete(&loaded);
        return false;
    }

    avl_tree_delete(tree);
    *tree = loaded;
    return true;
}
