/*
 * 1. Korzystając z przykładów (albo samodzielnie, lub na podstawie manuala)
 *    przygotuj programy - klient oraz serwer.
 *  - Niech serwer będzie udostępniał wybrany plik.
 *  - Niech klient będzie miał możliwość pobrania wybranego przez klienta pliku.
 *  - Jako uproszczenie przyjmujemy że serwer nie dba o żadne zasady bezpieczeństwa.
 *  - Protokół komunikacji niech wygląda tak:
 *    |--------------KLIENT---------------|----------SERWER-----------|
 *    | długość_nazwy_pliku               >                           |
 *    | nazwa_pliku[długość_nazwy_pliku]  >                           |
 *    |                                   <  wielkość_pliku           |
 *    |                                   <  paczka_1[wielkość_pliku] |
 *    | --zamknij połączenie--            |  --zamknij połączenie--   |
 *    |-----------------------------------|---------------------------|
 *  - Serwer powinien działać w pętli.
 * 2. Niech serwer, w sytuacji, jeśli pliku nie udało się otworzyć,
 *    wysyła wielkość pliku jako -1.
 * 3. Niech serwer zakończy w elegancki sposób pracę w momencie naciśnięcia przez
 *    użytkownika Ctrl+C. Chodzi o zamknięcie gniazda nasłuchującego.
 * 4. Niech serwer zapisuje logi do pliku - to znaczy informacje o tym kto się podłączył
 *    (jego IP oraz port) oraz o jaki plik poprosił.
 */

#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE
#include <bits/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "protocol.h"

/* Zapisuję socket serwera w zmiennej globalnej,
 * abym mógł zamknąć go z handlera SIGINT */
static int server_sock_fd;
static bool server_active = true;

typedef struct ClientInfo {
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
} ClientInfo;

int read_filename(int sock_fd, char *filename, uint8_t *len) {
    ssize_t bytes_received;

    /* Odbieram długość nazwy pliku (1 bajt) */
    bytes_received = recv(sock_fd, len, sizeof(*len), 0);
    if (bytes_received == -1) {
        return -1;
    }
    if (*len <= 0 || *len > MAX_FILENAME_LENGTH) {
        return -2;
    }
    printf("> [długość nazwy pliku] %d\n", *len);
    usleep(100);

    /* Odbieram nazwę pliku (`len` bajtów) */
    bytes_received = recv(sock_fd, filename, *len, 0);
    if (bytes_received == -1) {
        return -3;
    }
    printf("> [nazwa pliku]         \"%s\"\n", filename);

    return 0;
}

int send_file_size(int sock_fd, int64_t file_size) {
    ssize_t bytes_sent;

    bytes_sent = send(sock_fd, (int8_t *)&file_size, sizeof(file_size), 0);
    if (bytes_sent == -1) {
        return -1;
    }
    printf("< [wielkość pliku]      %ld\n", file_size);

    return 0;
}

int send_file(int sock_fd, FILE *file) {
    ssize_t bytes_sent, bytes_to_send;
    char buffer[BUFFER_SIZE];
    int64_t file_size;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Wysyłam wielkość pliku (4 bajty) */
    if (send_file_size(sock_fd, file_size) < 0) {
        return -1;
    }

    /* Wysyłam plik (`file_size` bajtów) w partiach rozmiaru BUFFER_SIZE */
    while (file_size > 0) {
        bytes_to_send = file_size > sizeof(buffer) ? sizeof(buffer) : file_size;
        fread(buffer, sizeof(char), bytes_to_send, file);
        bytes_sent = send(sock_fd, buffer, bytes_to_send, 0);
        if (bytes_sent == -1) {
            return -2;
        }
        file_size -= bytes_sent;
    }
    printf("< [plik]\n");

    return 0;
}

int handle_client_connection(
    int client_sock_fd, ClientInfo client_info, FILE *log_file
) {
    uint8_t filename_len;
    char filename[MAX_FILENAME_LENGTH] = { 0 };
    FILE *file;

    /* Odbieram nazwę pliku */
    memset(filename, 0, sizeof(filename));
    if (read_filename(client_sock_fd, filename, &filename_len) < 0) {
        perror("read_filename");
        return -1;
    }
    printf("Klient poprosił o plik: \"%s\"\n", filename);
    fprintf(
        log_file,
        "%s:%s poprosił o plik: \"%s\"\n",
        client_info.host,
        client_info.port,
        filename
    );
    fflush(log_file);

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Plik nie istnieje\n");
        if (send_file_size(client_sock_fd, -1) < 0) {
            perror("send_file_size");
        }
        printf("  [koniec połączenia]\n");
        return -2;
    }

    /* Wysyłam plik */
    printf("Plik istnieje, wysyłam go klientowi...\n");
    if (send_file(client_sock_fd, file) < 0) {
        perror("send_file");
        return -3;
    }

    printf("  [koniec połączenia]\n");
    return 0;
}

void wait_for_client(int sock_fd, FILE *log_file) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int client_sock_fd;
    ClientInfo client_info;

    client_addr_len = sizeof(client_addr);
    printf("Czekam na połączenie...\n");
    client_sock_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sock_fd == -1) {
        /* Nie wypisuj błędu, jeśli został on spowodowany zatrzymaniem serwera */
        if (server_active) perror("accept");
        return;
    }

    getnameinfo(
        (struct sockaddr *)&client_addr,
        client_addr_len,
        client_info.host,
        sizeof(client_info.host),
        client_info.port,
        sizeof(client_info.port),
        NI_NUMERICSERV
    );

    printf("Nowy klient: %s:%s\n", client_info.host, client_info.port);
    handle_client_connection(client_sock_fd, client_info, log_file);
    close(client_sock_fd);
}

void handle_sigint(int sig) {
    printf("Zamykam serwer...\n");
    server_active = false;
    shutdown(server_sock_fd, SHUT_RDWR);
}

int main(int argc, const char *argv[]) {
    int port;
    struct sockaddr_in server_addr;
    FILE *log_file;

    if (argc != 2) {
        fprintf(stderr, "Sposób użycia: %s <port>\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);
    if (port <= 0) {
        fprintf(stderr, "Niepoprawny numer portu: %s\n", argv[1]);
        return 1;
    }

    /* Otwieram nowy socket ipv4 (AF_INET) TCP (SOCK_STREAM) */
    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1) {
        perror("socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;         /* Protokół ipv4 */
    server_addr.sin_port = htons(port);       /* Port wybrany przez użytkownika */
    server_addr.sin_addr.s_addr = INADDR_ANY; /* Każdy interfejs (0.0.0.0) */

    if (bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
        -1) {
        perror("bind");
        close(server_sock_fd);
        return 1;
    }
    if (listen(server_sock_fd, 10) == -1) {
        perror("listen");
        close(server_sock_fd);
        return 1;
    }

    log_file = fopen("server.log", "a");
    if (log_file == NULL) {
        perror("fopen");
        close(server_sock_fd);
        return 1;
    }

    printf("Nasłuchuję na porcie %d\n", port);
    printf("Aby zakończyć działanie programu, naciśnij Ctrl+C\n");
    signal(SIGINT, handle_sigint);
    while (server_active) {
        wait_for_client(server_sock_fd, log_file);
    }

    close(server_sock_fd);
    fclose(log_file);
    return 0;
}
