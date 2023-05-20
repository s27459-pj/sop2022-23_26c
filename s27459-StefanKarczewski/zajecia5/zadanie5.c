/*
 * Napisz program, który uruchomi 100 procesów potomnych w taki sposób,
 * że będzie jeden rodzic i 100 potomków.
 * Proces główny niech oczekuje na zakończenie wszystkich procesów potomnych.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char const* argv[]) {
    pid_t pid;
    int i;
    unsigned int processes_to_create = 10;
    pid_t* child_pids;

    /* Alokujemy pamięć , aby zapisać numery PID utworzonych dzieci */
    child_pids = malloc(sizeof(pid_t) * processes_to_create);
    if (child_pids == NULL) {
        perror("malloc");
        return 1;
    }

    /* Wpisuję PID głównego programu, aby móc go znaleźć w drzewie procesów.
     * Drzewo możemy zobaczyć w `htop` w widoku drzewa (f5),
     * lub za pomocą programu `pstree`. */
    printf("[R] PID: %d\tPPID: %d\n", getpid(), getppid());

    for (i = 0; i < processes_to_create; i++) {
        switch (pid = fork()) {
            case -1:
                perror("fork");
                return 1;
            case 0:
                /* Dziecko wypisuje swój PID oraz PPID */
                printf("[D] PID: %d\tPPID: %d\n", getpid(), getppid());
                return 0;
            default:
                /* Rodzic zapisuje PID każdego utworzonego dziecka */
                child_pids[i] = pid;
        }
    }

    /* Czekam na zakończenie każdego procesu z osobna
     * dopiero po utworzeniu wszystkich procesów. */
    for (i = 0; i < processes_to_create; i++) {
        waitpid(child_pids[i], NULL, 0);
    }

    return 0;
}
