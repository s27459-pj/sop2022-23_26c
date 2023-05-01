#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    int n, i, j;
    if (argc < 2) {
        printf("Podaj liczbę jako argument programu\n");
        return 1;
    }
    /* elementy argv są ciągami znaków,
       więc parsuję argv[1] do faktycznego inta */
    n = atoi(argv[1]);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n - i; j++) putchar(' ');
        for (j = 0; j < i * 2 + 1; j++) putchar('*');
        putchar('\n');
    }

    return 0;
}
