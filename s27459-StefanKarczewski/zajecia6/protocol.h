/* Stałe, które współdzielą serwer i klient */
#define BUFFER_SIZE         1024
#define MAX_FILENAME_LENGTH 256

/* Stałe dla serwera HTTP */
#define MAX_METHOD_LENGTH   10
#define MAX_URL_LENGTH      2000
#define MAX_PROTOCOL_LENGTH 10
#define MAX_START_LINE_LENGTH \
    (MAX_METHOD_LENGTH + MAX_URL_LENGTH + MAX_PROTOCOL_LENGTH + 2) /* 2 spacje */
