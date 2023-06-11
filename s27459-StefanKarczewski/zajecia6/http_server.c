/*
 * 1. Rozwiń/napisz serwer HTTP tak, aby nadawał się do udostępniania strony WWW.
 *    Serwer powinien obsługiwać:
 *  - lista zabronionych adresów
 *  - zapis logów zdarzeń (wraz z informacją kto się podłączył)
 *  2. Dodatkowo niech serwer pozwala na tworzenie wirtualnych hostów.
 *  - Niech serwer nie blokuje się w sytuacji kiedy jakiś klient przeciąga połączenie
 *    (aby nie blokowało ono innych połączeń).
 *  - Zastosuj fork.
 *
 * Ten program jest kopią server.c z modyfikacjami, aby korzystał z protokołu HTTP.
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
/* Lista zabronionych adresów */
static const char *BLOCKED_ADDRESSES[] = {
    "10.0.0.1", "20.0.0.1", "8.8.8.8", "8.4.4.8", "1.1.1.1"
};

typedef struct ClientInfo {
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
} ClientInfo;

bool is_crlf(char *buffer) { return buffer[0] == '\r' && buffer[1] == '\n'; }

int read_http_request(int client_sock_fd, char *start_line) {
    char buffer[BUFFER_SIZE] = { 0 };
    ssize_t bytes_read;
    bool reading_request = true;
    int i;

    while (reading_request) {
        bytes_read = read(client_sock_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            perror("read");
            return -1;
        }

        /* Czytam pierwszą linię requestu:
         * metoda adres protokół\r\n
         * https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages#start_line
         * Zakładam, że zmieści się ona w jednym buforze BUFFER_SIZE */
        for (i = 0; i < sizeof(buffer) - 1; i++) {
            /* Szukam \r\n */
            if ((i < (sizeof(buffer) - 1)) && is_crlf(&buffer[i])) {
                strncpy(start_line, buffer, i);
                reading_request = false;
                /* Przesuwam wskaźnik o 2, aby ominąć \r\n */
                i += 2;
                break;
            }
            /* Nie udało się znaleźć \r\n w tym buforze
             * FIXME: Wspieraj start-line w więcej niż jednym buforze */
            if (i == sizeof(buffer) - 1) {
                return -1;
            }
        }
    }

    return 0;
}

int handle_client_connection(
    int client_sock_fd, ClientInfo client_info, FILE *log_file
) {
    char buffer[BUFFER_SIZE], start_line[MAX_START_LINE_LENGTH] = { 0 };
    ssize_t bytes_sent;
    char *status, *document;

    if (read_http_request(client_sock_fd, start_line) < 0) return -1;

    fprintf(log_file, "[%s:%s]\t%s\n", client_info.host, client_info.port, start_line);
    fflush(log_file);

    status = "HTTP/1.1 200 OK";
    document = "<h1>SOP</h1>\n<pre>Lorem ipsum dolor sit amet</pre>";
    memset(buffer, 0, sizeof(buffer));
    sprintf(
        buffer,
        "%s\r\nContent-Length: %ld\r\n\r\n%s",
        status,
        strlen(document),
        document
    );

    bytes_sent = send(client_sock_fd, buffer, strlen(buffer), 0);
    if (bytes_sent == -1) {
        perror("send");
        return -2;
    }

    printf("[koniec połączenia]\n");
    return 0;
}

bool is_blocked_address(const char *address) {
    int i;
    for (i = 0; i < sizeof(BLOCKED_ADDRESSES) / sizeof(BLOCKED_ADDRESSES[0]); i++) {
        if (strcmp(address, BLOCKED_ADDRESSES[i]) == 0) {
            return true;
        }
    }
    return false;
}

int wait_for_client(int sock_fd, FILE *log_file) {
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
        return 0;
    }

    getnameinfo(
        (struct sockaddr *)&client_addr,
        client_addr_len,
        client_info.host,
        sizeof(client_info.host),
        client_info.port,
        sizeof(client_info.port),
        NI_NUMERICHOST | NI_NUMERICSERV
    );

    if (is_blocked_address(client_info.host)) {
        printf("Zablokowany adres: %s\n", client_info.host);
        printf("[koniec połączenia]\n");
        close(client_sock_fd);
        return 0;
    }

    printf("Nowy klient: %s:%s\n", client_info.host, client_info.port);
    if (fork() == 0) {
        handle_client_connection(client_sock_fd, client_info, log_file);
        return 1;
    }
    close(client_sock_fd);
    return 0;
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
        /* wait_for_client zwraca 1 w przypadku kiedy proces dziecka
         * zakończył obsługiwanie klienta. */
        if (wait_for_client(server_sock_fd, log_file) == 1) {
            break;
        }
    }

    close(server_sock_fd);
    fclose(log_file);
    return 0;
}
