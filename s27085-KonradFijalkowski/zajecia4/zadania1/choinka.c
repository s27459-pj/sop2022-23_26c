#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

void isInRadius(int parameter, int point, int radius){
    printf((abs(parameter-point) <= radius) ? "x" : " ");
}

int main(int argc, char** argv){
    int wysokoscChoinki = atoi(argv[1]);
    if (wysokoscChoinki < 0){
        printf("Widzieliście kiedyś taką choinke?. Wprowadź ponownie rozmiar.");
    }
    int dlugoscWiersza = wysokoscChoinki*2;
    for (int wiersz = 0; wiersz < wysokoscChoinki; ++wiersz){
        for (int poleNaZnak = 1; poleNaZnak <= dlugoscWiersza; ++poleNaZnak) {
            isInRadius(poleNaZnak, wysokoscChoinki, wiersz);
        }
        printf("\n");
    }
    int rozmiarPienka = wysokoscChoinki/3;
    if (rozmiarPienka == 0) {
        rozmiarPienka = 1;
    }
    for (int wiersz = 0; wiersz < rozmiarPienka; wiersz++) {
        for (int poleNaZnak = 1; poleNaZnak <= dlugoscWiersza; ++poleNaZnak) {
            isInRadius(poleNaZnak, wysokoscChoinki, wysokoscChoinki > 3);
        }
        printf("\n");
    }
    return 0;
}