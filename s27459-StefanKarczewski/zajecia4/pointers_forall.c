#include <stdio.h>
#include <stdlib.h>

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
int sum(int a, int b) { return a + b; }
int forAll(int* items, int n, int (*f)(int, int)) {
    int i, v = items[0];
    for (i = 1; i < n; i++) {
        v = f(v, items[i]);
    }
    return v;
}

int main(int argc, char const* argv[]) {
    int (*functions[3])(int, int) = { min, max, sum };
    int function_id, element_count, i = 0;
    int* items;

    if (argc < 3) {
        printf("Usage: %s <functionId> <elementCount> <elements...>\n", argv[0]);
        return 1;
    }
    /* Sprawdzam, czy function_id jest poprawne */
    function_id = atoi(argv[1]);
    if (function_id < 0 || function_id > 2) {
        printf("Available functions: min (0), max (1), sum (2)\n");
        return 1;
    }
    /* Sprawdzam, czy ilość elementów jest poprawna */
    element_count = atoi(argv[2]);
    if (element_count < 1) {
        printf("Please input a positive number of elements\n");
        return 1;
    }
    /* Sprawdzam, czy podano odpowiednią ilość elementów */
    if (argc < 3 + element_count) {
        printf("Please input %d elements\n", element_count);
        return 1;
    }
    /* Alokuję tablicę na liczby o odpowiedniej wielkości */
    items = malloc(element_count * sizeof(int));
    if (items == NULL) {
        perror("malloc");
        return 1;
    }
    for (i = 0; i < element_count; i++) {
        items[i] = atoi(argv[3 + i]);
    }

    /* Korzystam z funkcji forAll - przekazuję wskaźnik na odczytane liczby */
    printf("%d\n", forAll(items, element_count, functions[function_id]));

    return 0;
}
