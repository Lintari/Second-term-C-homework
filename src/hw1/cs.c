#include "functions.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192
#define MAX_COLS 256
#define MAX_ROWS 10000

int fplus(FILE* out, int* count, int size, int title)
{
    char symbol = title ? '=' : '-'; // первые два раза у нас = а потом -

    for (int column = 0; column < size; column++) {
        fputc('+', out);
        for (int i = 0; i < count[column] + 2; i++) {
            fputc(symbol, out);
        }
    }
    fprintf(out, "+\n");
    return 0;
}

static int is_number(const char* text)
{
    char* end;

    while (isspace(*text)) {
        text++;
    }
    if (*text == '\0') {
        return 0;
    }

    (void)strtod(text, &end);
    if (end == text) {
        return 0;
    }
    while (isspace(*end)) {
        end++;
    }
    return *end == '\0';
}

static void frow(FILE* out, char* line, const int* count, int size, int title) // выводим строчку
{
    char* field = line;

    for (int column = 0; column < size; column++) {
        char* comma = strchr(field, ',');
        if (comma != NULL) {
            *comma = '\0';
        }
        int ifnum = !title && is_number(field);
        if (ifnum) {
            fprintf(out, "| %*s ", count[column], field); // отцентровали как надо по условию
        } else {
            fprintf(out, "| %-*s ", count[column], field);
        }

        if (comma != NULL) {
            *comma = ',';
            field = comma + 1;
        } else {
            field += strlen(field);
        }
    }
    fputs("|\n", out);
}

int fdraw(FILE* input, FILE* out, int* count, int size)
{
    char line[MAX_LINE];
    int istitle = 1;
    fplus(out, count, size, 1);
    while ((fgets(line, sizeof(line), input)) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';
        frow(out, line, count, size, istitle);
        if (!istitle) {
            fplus(out, count, size, 0);
        }
    
        if (istitle) {
            fplus(out, count, size, 1);
        }
        istitle = 0;
    }
    return 0;
}

int fshow(FILE* input, FILE* out)
{
    int count[MAX_COLS] = { 0 }; // счетчик длины каждого столбца
    char line[MAX_LINE]; // буфер
    int c; // номер считывающегося символа со строки.
    int counter; // длина считываемого тсолбца.
    int amount; // количество столбцов  в текущей строке
    int glmax = 0; //  количество столбцов в принципе

    while ((fgets(line, sizeof(line), input)) != NULL) {
        c = 0;
        amount = 0;
        counter = 0;
        line[strcspn(line, "\n")] = '\0'; // чтобы не считать \n
        while (line[c] != '\0') {
            if (line[c] == ',') {
                count[amount] = (count[amount] < counter) ? counter : count[amount];
                amount++;
                counter = 0;
            } else {
                counter++;
            }
            c++;
        }

        count[amount] = (count[amount] < counter) ? counter : count[amount]; // отдеьнло смотрим последнее поле
        amount++;
        glmax = (glmax > amount) ? glmax : amount;
    }
    rewind(input);
    fdraw(input, out, count, glmax);

    return 0;
}
