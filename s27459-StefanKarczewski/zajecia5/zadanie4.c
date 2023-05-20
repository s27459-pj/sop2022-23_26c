/*
 * Napisz program, który uruchomi 100 procesów w taki sposób, że każdy kolejny potomek
 * stworzy potomka. Niech każdy proces poczeka na zakończenie procesu potomka. Możesz
 * korzystać z oczekiwania na wejście lub z komendy sleep i zobacz czy drzewko procesów
 * jest takie jak się spodziewasz (możesz w tym celu ograniczyć liczbę procesów do 10).
 * Zobacz podpowiedź na początku tej strony.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    pid_t p;
    unsigned int processes_to_create = 100;

    /* Wpisuję PID głównego programu, aby móc go znaleźć w drzewie procesów.
     * Drzewo możemy zobaczyć w `htop` w widoku drzewa (f5),
     * lub za pomocą programu `pstree`. */
    printf("[R] PID: %d\tPPID: %d\n", getpid(), getppid());

    /* Tworzymy rekursywne drzewo procesów:
     * p1 -> p2 -> p3 -> p4 -> p5 -> ... */
    while (processes_to_create > 0) {
        switch (p = fork()) {
            case -1:
                perror("fork");
                return 1;
            case 0:
                /* Dziecko */
                printf("[D] PID: %d\tPPID: %d\n", getpid(), getppid());
                /* Zmniejszamy ilość procesów pozostałych do stworzenia
                 * i idziemy do kolejnej iteracji pętli.
                 * Wartość zmiennych zostanie zachowana w procesie, który stworzymy,
                 * więc będzie on wiedział ile jeszcze zostało mu procesów do
                 * utworzenia. */
                processes_to_create--;
                break;
            default:
                /* Rodzic */
                /* Czekamy na zakończenie procesów w kolejności od ostatnio utworzonego
                 * do najstarszego (od tyłu). */
                waitpid(p, NULL, 0);
                return 0;
        }
    }

    /* Ostatni utworzony proces czeka na wpisanie jakiegoś znaku na klawiaturze, abyśmy
     * mieli szansę przejrzeć drzewo procesów zanim wszystkie procesy się zakończą. */
    getchar();

    return 0;
}
