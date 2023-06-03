#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const* argv[]) {
    FILE* file;
    int i, offset = 0, read_chars = 0;
    unsigned char bytes[16];

    if (argc < 2) {
        printf("Reading from stdin\n");
        file = stdin;
    } else {
        printf("Loading file: %s\n", argv[1]);
        file = fopen(argv[1], "rb");
        if (file == NULL) {
            perror("fopen");
            return 1;
        }
    }

    /* Czytam bajty z pliku po 16 na raz */
    while ((read_chars = fread(&bytes, sizeof(char), 16, file)) > 0) {
        printf("%08X  ", offset);
        /* Wypisuję po kolei bajty w postaci szestnastkowej */
        for (i = 0; i < read_chars; i++) {
            printf("%02X ", bytes[i]);
        }
        /* Uzupełniam resztę bajtów spacjami (na końcu pliku) */
        for (i = 0; i < 16 - read_chars; i++) {
            printf("   ");
        }

        /* Wypisuję te same bajty w postaci tekstu:
         * - jeśli bajt jest drukowalny, to go wypisuję
         * - jeśli bajt jest znakiem nowej linii, to wypisuję spację
         * - w przeciwnym wypadku wypisuję znak `▒`
         */
        for (i = 0; i < read_chars; i++) {
            if (isprint(bytes[i])) {
                putchar(bytes[i]);
            } else if (bytes[i] == '\n') {
                putchar(' ');
            } else {
                printf("▒");
            }
        }

        putchar('\n');
        offset += 16;
        /* Resetuję */
        memset(bytes, 0, 16);
    }

    if (file != stdin) {
        printf("Closing file: %s\n", argv[1]);
        fclose(file);
    }

    return 0;
}
