#include "avl_tree.h"
#include "database.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static char* skipSpaces(char* text)
{
    while (isspace((unsigned char)*text)) {
        text++;
    }
    return text;
}

static void printHelp(void)
{
    puts("Commands:");
    puts("  find <IATA>          Find an airport by code");
    puts("  add <IATA> <name>    Add an airport");
    puts("  delete <IATA>        Delete an airport");
    puts("  size                 Show the number of airports");
    puts("  help                 Show this help");
    puts("  exit                 Quit the program");
}

int main(int argc, char** argv)
{
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [airports.txt]\n", argv[0]);
        return 1;
    }

    const char* filename = argc == 2 ? argv[1] : "airports.txt";
    AVLTree tree;
    avlTreeInit(&tree);

    if (!databaseLoad(&tree, filename)) {
        fprintf(stderr, "Could not load airport database: %s\n", filename);
        avlTreeDelete(&tree);
        return 1;
    }

    printf("Loaded %zu airports from %s. Type 'help' for commands.\n", tree.size, filename);

    char line[2048];
    while (true) {
        printf("> ");
        fflush(stdout);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        line[strcspn(line, "\r\n")] = '\0';

        char* command = skipSpaces(line);
        if (*command == '\0') {
            continue;
        }

        char* args = command;
        while (*args != '\0' && !isspace((unsigned char)*args)) {
            args++;
        }
        if (*args != '\0') {
            *args++ = '\0';
            args = skipSpaces(args);
        }

        if (strcmp(command, "help") == 0) {
            printHelp();
        } else if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
            break;
        } else if (strcmp(command, "size") == 0) {
            if (*args != '\0') {
                puts("Usage: size");
            } else {
                printf("Tree size: %zu\n", tree.size);
            }
        } else if (strcmp(command, "find") == 0 || strcmp(command, "delete") == 0) {
            char* code = args;
            while (*args != '\0' && !isspace((unsigned char)*args)) {
                args++;
            }
            if (*args != '\0') {
                *args++ = '\0';
                args = skipSpaces(args);
            }

            if (*code == '\0' || *args != '\0') {
                printf("Usage: %s <IATA>\n", command);
            } else if (strcmp(command, "find") == 0) {
                const AVLNode* node = avlFind(&tree, code);
                if (node == NULL) {
                    printf("Airport not found: %s\n", code);
                } else {
                    printf("%s: %s\n", node->iata_code, node->name);
                }
            } else if (avlDelete(&tree, code)) {
                printf("Deleted: %s\n", code);
            } else {
                printf("Airport not found: %s\n", code);
            }
        } else if (strcmp(command, "add") == 0) {
            char* code = args;
            while (*args != '\0' && !isspace((unsigned char)*args)) {
                args++;
            }
            if (*args != '\0') {
                *args++ = '\0';
                args = skipSpaces(args);
            }

            size_t nameLength = strlen(args);
            while (nameLength > 0 && isspace((unsigned char)args[nameLength - 1])) {
                args[--nameLength] = '\0';
            }

            if (*code == '\0' || *args == '\0') {
                puts("Usage: add <IATA> <name>");
            } else if (avlFind(&tree, code) != NULL) {
                printf("Airport already exists: %s\n", code);
            } else if (avlInsert(&tree, code, args)) {
                printf("Added: %s\n", code);
            } else {
                puts("Could not add airport (memory allocation failed)");
            }
        } else {
            printf("Unknown command: %s (type 'help' for commands)\n", command);
        }
    }

    avlTreeDelete(&tree);
    return 0;
}
