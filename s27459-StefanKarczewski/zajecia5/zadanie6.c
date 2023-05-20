/* Napisz prosty program chat-u, który będzie korzystał
 * z pliku w katalogu /tmp do przekazywania wiadomości.
 * Zastanów się jak to zrobić.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_FILE "/tmp/sop_chat_messages"

int send_message(FILE* messages_fd, const char* message) {
    int len;
    len = fprintf(messages_fd, "%s\n", message);
    fflush(messages_fd);
    return len;
}

int read_message(FILE* messages_fd, char* buf) {
    int len;
    len = fread(buf, sizeof(char), 100, messages_fd);
    getc(messages_fd);
    return len;
}

int main(int argc, char const* argv[]) {
    FILE *messages_write_fd, *messages_read_fd;
    static char input_buf[101], message_read_buf[101];
    int len;

    messages_write_fd = fopen(MESSAGE_FILE, "a");
    if (messages_write_fd == NULL) {
        perror("fopen");
        return 1;
    }

    messages_read_fd = fopen(MESSAGE_FILE, "r");
    if (messages_read_fd == NULL) {
        perror("fopen");
        fclose(messages_write_fd);
        return 1;
    }
    fseek(messages_read_fd, 0, SEEK_END);

    while (1) {
        printf("> ");
        /* Czytam max 100 znaków */
        scanf("%100[^\n]", input_buf);
        /* TODO: Wyczyść stdin, żeby część wiadomości, która wyszła poza max. 100 znaków
         * została usunięta */
        getc(stdin);

        /* Przed wysłaniem naszej wiadomości czytam wiadomość z pliku */
        len = read_message(messages_read_fd, message_read_buf);
        if (len == EOF) {
            perror("read_message");
            fclose(messages_write_fd);
            fclose(messages_read_fd);
            return 0;
        }
        if (len > 0) {
            printf("< %s", message_read_buf);
        }
        memset(message_read_buf, 0, 101 * sizeof(char));

        /* Teraz wysyłam wiadomość */
        len = send_message(messages_write_fd, input_buf);
        if (len < 0) {
            perror("send_message");
            fclose(messages_write_fd);
            fclose(messages_read_fd);
            return 1;
        }
        memset(input_buf, 0, 101 * sizeof(char));
        fseek(messages_read_fd, 0, SEEK_END);
    }

    fclose(messages_write_fd);
    fclose(messages_read_fd);
    return 0;
}
