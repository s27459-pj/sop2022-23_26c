/*
 * Napisz program, który będzie pobierał jeden argument z wiersza poleceń i będzie
 * obliczał w 2 procesach:
 * - Proces rodzica: Wartość sumy liczb od -1 do wartości zadanej
 *   jako pierwszy argument wykonania programu.
 * - Proces potomny: Wypisujący liczby nieparzyste od 1 do wartości zadanej
 *   jako pierwszy argument wykonania programu.
 * - Program powinien wypisać wyniki w kolejności takiej,
 *   że najpierw wypisze wynik proces potomny, a następnie proces rodzica.
 * - Nie korzystaj z komendy sleep.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void identify_process() {
    printf("PID: %d\tPPID: %d\n", getpid(), getppid());
    fflush(stdout);
}

int cumsum(int n) {
    /* Suma liczb (0..n) */

    int i, sum = 0;
    for (i = 0; i <= n; i++) {
        sum += i;
    }
    return sum;
}

int cumsum_odd(int n) {
    /* Suma liczb nieparzystych (1..n) */

    int i, sum = 0;
    for (i = 1; i <= n; i += 2) {
        sum += i;
    }
    return sum;
}

int main(int argc, char const *argv[]) {
    pid_t p;
    int status, n;

    if (argc != 2) {
        printf("Podaj jeden numer jako argument\n");
        return 1;
    }
    n = atoi(argv[1]);
    if (n == 0) {
        printf("Podaj poprawną liczbę jako argument\n");
        return 1;
    }

    switch (p = fork()) {
        case -1:
            perror("fork");
            return 1;
        case 0:
            /* Dziecko */
            identify_process();
            printf("Suma nieparzystych (1..%d) = %d\n", n, cumsum_odd(n));
            break;
        default:
            /* Rodzic */
            /* Najpierw czekam na zakończenie pracy dziecka */
            waitpid(p, &status, 0);
            identify_process();
            printf("Suma (0..%d) = %d\n", n, cumsum(n));
            break;
    }

    return 0;
}
