#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define MAX_BUFFER_SIZE 1024

void get_process_children(pid_t pid, const unsigned int depth) {
    char command[MAX_BUFFER_SIZE], output[MAX_BUFFER_SIZE];
    FILE *fp;
    int i;
    pid_t child_pid;

    /* Tworzę komendę, która zdobędzie dla mnie PID dzieci danego procesu */
    sprintf(command, "pgrep -P %d", pid);
    /* Wykonuję komendę w powłoce użytkownika */
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen");
        exit(1);
    }

    /* Czytam linia po linii wyjście z komendy `pgrep` - każda z nich to PID dziecka */
    while (fgets(output, sizeof(output), fp) != NULL) {
        child_pid = atoi(output);
        if (child_pid == 0) {
            perror("atoi");
            pclose(fp);
            return;
        }

        for (i = 0; i < depth; i++) {
            printf("│ ");
        }

        printf("├ %d\n", child_pid);
        get_process_children(child_pid, depth + 1);
    }

    pclose(fp);
}

int main(int argc, char const *argv[]) {
    pid_t pid;

    if (argc < 1) {
        printf("Podaj PID jako argument programu\n");
        return 1;
    }

    pid = atoi(argv[1]);
    if (pid == 0) {
        perror("atoi");
        return 1;
    }
    printf("%d\n", pid);
    get_process_children(pid, 0);

    return 0;
}
