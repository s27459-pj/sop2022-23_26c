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

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "protocol.h"

int send_filename(int sock_fd, const char *filename) {
    ssize_t bytes_sent;
    uint8_t len = strlen(filename);

    /* Wysyłam długość nazwy pliku (1 bajt) */
    bytes_sent = send(sock_fd, &len, sizeof(len), 0);
    if (bytes_sent == -1) {
        return -1;
    }
    printf("< [długość nazwy pliku] %d\n", len);

    /* Wysyłam nazwę pliku (`len` bajtów) */
    bytes_sent = send(sock_fd, filename, len, 0);
    if (bytes_sent == -1) {
        return -2;
    }
    printf("< [nazwa pliku]         \"%s\"\n", filename);

    return 0;
}

int read_file_size(int sock_fd, int64_t *file_size) {
    ssize_t bytes_received;

    bytes_received = recv(sock_fd, file_size, sizeof(*file_size), 0);
    if (bytes_received == -1) {
        return -1;
    }
    printf("> [wielkość pliku]      %ld\n", *file_size);

    return 0;
}

int read_file(int sock_fd, FILE *file, int64_t file_size) {
    ssize_t bytes_to_read, bytes_received;
    char buffer[BUFFER_SIZE];

    while (file_size > 0) {
        bytes_to_read = file_size > sizeof(buffer) ? sizeof(buffer) : file_size;
        bytes_received = recv(sock_fd, buffer, bytes_to_read, 0);
        if (bytes_received == -1) {
            return -2;
        }
        fwrite(buffer, sizeof(char), bytes_to_read, file);
        file_size -= bytes_received;
    }
    printf("> [plik]\n");

    return 0;
}

int main(int argc, const char *argv[]) {
    int port, sock_fd;
    const char *hostname, *filename;
    struct sockaddr_in server_addr;
    struct hostent *server_host;
    FILE *file;
    int64_t file_size;

    if (argc != 4) {
        fprintf(stderr, "Sposób użycia: %s <host> <port> <nazwa_pliku>\n", argv[0]);
        return 1;
    }
    hostname = argv[1];
    port = atoi(argv[2]);
    if (port <= 0) {
        fprintf(stderr, "Niepoprawny numer portu: %s\n", argv[1]);
        return 1;
    }
    filename = argv[3];
    if (strlen(filename) > MAX_FILENAME_LENGTH) {
        fprintf(
            stderr,
            "Nazwa pliku jest za długa (maksymalnie %d znaków)\n",
            MAX_FILENAME_LENGTH
        );
        return 1;
    }

    server_host = gethostbyname(hostname);
    if (server_host == NULL) {
        printf("Nieznany host: %s\n", hostname);
        return 1;
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;   /* Protokół ipv4 */
    server_addr.sin_port = htons(port); /* Port wybrany przez użytkownika */
    bcopy(                              /* Host wybrany przez użytkownika */
          (char *)server_host->h_addr_list[0],
          (char *)&server_addr.sin_addr.s_addr,
          server_host->h_length
    );

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sock_fd);
        return 1;
    }

    printf("Podłączono do %s:%d\n", hostname, port);
    /* Wysyłam serwerowi nazwę pliku */
    if (send_filename(sock_fd, filename) < 0) {
        perror("send_filename");
        close(sock_fd);
        return 1;
    }

    /* Odbieram wielkość pliku */
    if (read_file_size(sock_fd, &file_size) < 0) {
        close(sock_fd);
        return 1;
    }
    /* Jeśli otrzymam wielkość pliku jako -1, to kończę pracę programu */
    if (file_size == -1) {
        printf("Plik nie istnieje\n");
        printf("  [koniec połączenia]\n");
        close(sock_fd);
        return 1;
    }

    printf("Zapisuję plik %s\n", filename);
    /* Otwieram pusty plik do zapisu */
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        close(sock_fd);
        return 1;
    }
    /* Odbieram plik od serwera */
    if (read_file(sock_fd, file, file_size) < 0) {
        perror("read_file");
        close(sock_fd);
        return 1;
    }

    printf("  [koniec połączenia]\n");
    fclose(file);
    close(sock_fd);
    return 0;
}
