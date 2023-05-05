#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    FILE *file;
    int n, i;
    int *items;

    if (argc < 2) {
        file = stdin;
    } else {
        file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("fopen");
            return 1;
        }
    }

    fscanf(file, "%d\n", &n);
    /* Alokuję tablicę na n elementów */
    items = malloc(n * sizeof(int));
    if (items == NULL) {
        perror("malloc");
        fclose(file);
        return 1;
    }

    /* Zczytuję po kolei n liczb */
    for (i = 0; i < n; i++) {
        fscanf(file, "%d\n", &items[i]);
    }

    /* Wypisuję wszystko z `items` od końca */
    for (i = n - 1; i >= 0; i--) {
        printf("%d\n", items[i]);
    }

    free(file);
    fclose(file);

    return 0;
}
