#include "database.h"

#include <stdio.h>
#include <string.h>

bool databaseLoad(AVLTree* tree, const char* filename)
{
    if (tree == NULL || filename == NULL) {
        return false;
    }

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }

    AVLTree loaded;
    avlTreeInit(&loaded);

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
        const char* iataCode = line;
        const char* name = separator + 1;

        if (avlFind(&loaded, iataCode) != NULL) {
            continue;
        }
        if (!avlInsert(&loaded, iataCode, name)) {
            success = false;
            break;
        }
    }

    if (ferror(file)) {
        success = false;
    }
    fclose(file);

    if (!success) {
        avlTreeDelete(&loaded);
        return false;
    }

    avlTreeDelete(tree);
    *tree = loaded;
    return true;
}
