#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{

    FILE* input = fopen("input.csv", "r");
    if (file == NULL) {
        printf("не удалось создать input.csv \n");
        return 1;
    }

    FILE* output = fopen("output.txt", "w");
    if (output == NULL) {
        printf("не удалось создать output.txt\n");
        fclose(file);
        return 1;
    }
    fshow(input, output);
    fclose(input);
    fclose(output);
    return 0;
}
