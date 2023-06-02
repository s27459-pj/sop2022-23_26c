/*
 * Przygotuj program, który będzie pozwalał na uruchomienie tylko jednej swojej
 * instancji. Dodatkowo w momencie próby uruchomienia drugiej instancji będzie
 * powiadamiał już uruchomiony proces o tym. Można to zrobić tak:
 *  1. Program uruchamia się i sprawdza, czy jest plik /tmp/lock
 *  2. Jeśli taki plik nie istnieje, wtedy:
 *     1. tworzony jest taki plik. Do niego zapisywany jest PID bieżącego procesu
 *     2. program instaluje obsługę sygnału USR1, która w momencie otrzymania sygnału
 *        wypisuje tekst: "Wyrkyto probe uruchomienia drugiej instancji programu."
 *     3. program instaluje obsługę sygnału generowanego przez CTRL+C (poszukaj jaki to
 *        sygnał), która w momencie otrzymania tego sygnału skasuje plik /tmp/lock i
 *        spowoduje wyjście z pętli nieskończonej (patrz następny punkt)
 *     4. program wykonuje pętlę nieskończoną zawierającą tylko instrukcję sleep.
 *  3. Jeśli taki plik istnieje, wtedy:
 *     1. wczytywana jest zawartość tego pliku - pid procesu uruchomionego
 *     2. generowany jest sygnał USR1 do procesu o pid takim jak wczytany
 *     3. program kończy pracę
 */

#define _XOPEN_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LOCK_FILE "/tmp/lock"

int acquire_lock(const char *file_path) {
    FILE *lock_file;
    int lock_fd;

    /* O_CREAT | O_EXCL zapewnia,
     * że plik będzie stworzony TYLKO jeśli jeszcze nie istnieje */
    lock_fd = open(file_path, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (lock_fd == -1) {
        /* Nie udało się zlockować */
        return -1;
    }

    /* Otwieram lock jako FILE* i zapisuję do niego PID */
    lock_file = fdopen(lock_fd, "w");
    if (lock_file == NULL) {
        perror("fdopen");
        close(lock_fd);
        return -2;
    }
    /* FILE* nie będzie nam już potrzebny, więc go zamykam */
    fprintf(lock_file, "%d\n", getpid());
    fclose(lock_file);

    return lock_fd;
}

void release_lock(const char *file_path) {
    if (remove(file_path) == -1) {
        perror("remove");
        return;
    }
}

void sigusr1_handler(int sig) {
    printf("Wykryto próbę uruchomienia drugiej instancji programu.\n");
    /* Domyślnie po wykonaniu zarejestrowanego handlera sygnału
     * jest on przywracany do domyślnej wartości,
     * więc musimy go re-instalować za każdym wykonaniem */
    signal(SIGUSR1, sigusr1_handler);
}

void sigint_handler(int sig) {
    printf("Wykryto CTRL+C. Kończę pracę.\n");
    release_lock(LOCK_FILE);
    exit(0);
}

void notify_other_instance(const char *file_path) {
    FILE *lock_file;
    pid_t other_instance_pid;

    lock_file = fopen(file_path, "r");
    if (lock_file == NULL) {
        perror("fopen");
        return;
    }

    /* Czytam PID otwartej instancji i wysyłam do niej SIGUSR1 */
    fscanf(lock_file, "%d", &other_instance_pid);
    printf("Program jest już uruchomiony (%d).\n", other_instance_pid);
    fclose(lock_file);
    kill(other_instance_pid, SIGUSR1);
}

int main() {
    int lock;

    lock = acquire_lock(LOCK_FILE);
    if (lock < 0) {
        notify_other_instance(LOCK_FILE);
        return 1;
    }

    printf(
        "Udało się zalockować %s, instaluję handlery dla SIGUSR1 i SIGINT...\n",
        LOCK_FILE
    );
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGINT, sigint_handler);

    while (1) {
        sleep(1);
    }

    return 0;
}
