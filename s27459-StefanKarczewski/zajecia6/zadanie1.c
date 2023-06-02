/*
 * Przygotuj prosty programik, który:
 * 1. Wypisze na ekranie swój PID
 * 2. będzie czekał w pętli nieskończonej do momentu otrzymania
 *    sygnału użytkownika (SIGUSR1)
 * 3. W momencie otrzymania tego sygnału program powinien wypisać na ekranie,
 *    że dostał sygnał i się zakończyć.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigusr1_handler(int sig) {
    printf("Dostałem sygnał SIGUSR1\n");
    exit(0);
}

int main() {
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, sigusr1_handler);
    printf("Czekam na sygnały...\n");
    while (1) {
        sleep(1);
    }

    return 0;
}
