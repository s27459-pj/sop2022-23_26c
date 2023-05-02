#include <stdio.h>

int main() {
    int sum = 0, number;

    do {
        number = 0;
        printf("Podaj liczbę: ");
        scanf("%d", &number);
        sum += number;
    } while (number != 0);

    printf("Suma: %d\n", sum);

    return sum;
    /* Tą zwróconą wartość mogę potem dostać w bashu ze zmiennej `$?`:
       echo $? */
}
