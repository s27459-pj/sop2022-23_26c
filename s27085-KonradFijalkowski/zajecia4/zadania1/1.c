#include <stdio.h>
int main() {
    int inputNum, sum = 0;
    do{
        printf("Podaj liczbe: ");
        scanf("%d", &inputNum);
        sum += inputNum;
    }while (inputNum != 0);

    /*printf("%d", sum); */
    return sum;

    /*teraz wystarczy użyć $? w linii komend, aby rozszerzyć tą zmienną do exit statusu ostatniej komendy,
     * a więc wartości zwracanej przez ten program*/
}