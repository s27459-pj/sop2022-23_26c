/*
 * Napisz program, który:
 * - przedstawi swój numer PID, oraz PPID
 * - po wypisaniu, będzie czekał na standardowym wejściu na dane od użytkownika.
 * - Uruchom program - nic nie podawaj na wejściu (program powinien czekać w
 *   nieskończoność). Uruchom nowy terminal. W nim za pomocą komendy kill zamknij ten
 *   program korzystając z PID wypisanego przez niego na początku.
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void identify_process() {
    printf("PID: %d\tPPID: %d\n", getpid(), getppid());
    fflush(stdout);
}

int main(int argc, char const *argv[]) {
    int input;

    identify_process();
    scanf("%d", &input);

    return 0;
}
