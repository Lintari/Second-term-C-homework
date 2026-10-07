#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

int filesIsSame(FILE* f1, FILE* f2)
{
    int c1, c2;

    c1 = fgetc(f1);
    c2 = fgetc(f2);

    while (c1 == c2 && c1 != EOF && c2 != EOF) {
        c1 = fgetc(f1);
        c2 = fgetc(f2);
    }
    return c1 == c2;
}

int getResult(const char* input, const char* output, const char* expected)
{
    FILE* fin = fopen(input, "r");
    if (fin == NULL)
        return -1;

    FILE* fout = fopen(output, "w+");
    if (fout == NULL) {
        fclose(fin);
        return -1;
    }

    FILE* fexp;

    if (fwriteTable(fin, fout) != 0)
        return -1;

    fexp = fopen(expected, "r");
    if (fexp == NULL) {
        fclose(fout);
        return -1;
    }

    fout = fopen(output, "r");

    int pass = filesIsSame(fout, fexp);

    fclose(fout);
    fclose(fexp);
    return pass;
}

int main(void)
{
    int passed = 0;
    printf("==============START TESTING================\n");

    int res = getResult("input_files/input1.csv", "output.txt", "expected_files/expected1.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 1 failed\n");
    else {
        passed += 1;
        printf("Test 1 passed\n");
    }

    res = getResult("input_files/input2.csv", "output.txt", "expected_files/expected2.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 2 failed\n");
    else {
        passed += 1;
        printf("Test 2 passed\n");
    }

    res = getResult("input_files/input3.csv", "output.txt", "expected_files/expected3.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 3 failed\n");
    else {
        passed += 1;
        printf("Test 3 passed\n");
    }

    res = getResult("input_files/input4.csv", "output.txt", "expected_files/expected4.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 4 failed\n");
    else {
        passed += 1;
        printf("Test 4 passed\n");
    }

    res = getResult("input_files/input5.csv", "output.txt", "expected_files/expected5.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 5 failed\n");
    else {
        passed += 1;
        printf("Test 5 passed\n");
    }

    res = getResult("input_files/input6.csv", "output.txt", "expected_files/expected6.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 6 failed\n");
    else {
        passed += 1;
        printf("Test 6 passed\n");
    }

    res = getResult("input_files/input7.csv", "output.txt", "expected_files/expected7.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 7 failed\n");
    else {
        passed += 1;
        printf("Test 7 passed\n");
    }

    res = getResult("input_files/input8.csv", "output.txt", "expected_files/expected8.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 8 failed\n");
    else {
        passed += 1;
        printf("Test 8 passed\n");
    }

    res = getResult("input_files/input9.csv", "output.txt", "expected_files/expected9.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 9 failed\n");
    else {
        passed += 1;
        printf("Test 9 passed\n");
    }

    res = getResult("input_files/input10.csv", "output.txt", "expected_files/expected10.txt");
    if (res == -1)
        printf("...something went wrong\n");
    else if (res == 0)
        printf("Test 10 failed\n");
    else {
        passed += 1;
        printf("Test 10 passed\n");
    }
    remove("output.txt");

    printf("Passed %d/10\n", passed);
    return 0;
}
