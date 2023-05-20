/*
 * Napisz program, który uruchomi podproces za pomocą komendy fork.
 * Niech proces rodzica oczekuje na proces potomka.
 * Oba procesy powinny wypisać swój PID oraz PPID.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    pid_t p;
    int status;

    switch (p = fork()) {
        case -1:
            /* Błąd */
            perror("fork");
            return 1;
        case 0:
            /* Dziecko */
            printf("[D] PID: %d\n", getpid());
            printf("[D] PPID: %d\n", getppid());
            fflush(stdout);
            break;
        default:
            /* Rodzic:
             * Czekam na zakończenie procesu o pid zawartym w p.
             * fork() zwraca pid dziecka, więc korzystam z tej informacji. */
            waitpid(p, &status, 0);
            printf("[R] PID: %d\n", getpid());
            printf("[R] PPID: %d\n", getppid());
            fflush(stdout);
            break;
    }

    return 0;
}
