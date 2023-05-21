/*
 * Napisz program, który wygeneruje drzewko procesów wyglądające tak
 * (oczywiście procesy nie będą się nazywały A, B, ...):
 *
 *  A \
 *    |\B \
 *    |   |\D
 *    |   \ E
 *    \ C \
 *        |\F
 *        \ G
 *
 *  A jest rodzicem 2 procesów - B oraz C, a te są odpowiednio rodzicami D i E,
 *  oraz F i G.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void spawn_n_children(int n, int depth) {
    int i;
    pid_t pid, *children_pids;

    children_pids = malloc(n * sizeof(pid_t));
    if (children_pids == NULL) {
        perror("malloc");
        return;
    }

    for (i = 0; i < n; i++) {
        switch (pid = fork()) {
            case -1:
                perror("fork");
                return;
            case 0:
                /* Jeśli pozostały nam jeszcze drzewa procesów do wygenerowania,
                 * kontynuujemy rekursywnie z tą samą logiką */
                if (depth > 0) spawn_n_children(n, depth - 1);
                /* Jeśli depth == 0, to znaczy że dotarliśmy do liścia */
                return;
            default:
                children_pids[i] = pid;
        }
    }

    /* Czekam na zakończenie procesów po skończeniu tworzenia całego drzewa */
    for (i = 0; i < n; i++) {
        waitpid(children_pids[i], NULL, 0);
    }

    free(children_pids);
}

int main(int argc, char const *argv[]) {
    /* Ilość procesów na każdym poziomie drzewa */
    int n_children;
    /* Jak dużo poziomów zagnieżdżenia tworzymy dla każdej gałęzi */
    int depth;

    /* Pozwalam na wprowadzenie n_children oraz depth,
     * ale ustawiam też domyślne wartości */
    n_children = (argc > 1) ? atoi(argv[1]) : 2;
    depth = (argc > 2) ? atoi(argv[2]) : 1;

    printf("Master PID: %d\n", getpid());
    printf("Run pstree -p %d to see the tree\n", getpid());

    spawn_n_children(n_children, depth);

    getchar();

    return 0;
}
