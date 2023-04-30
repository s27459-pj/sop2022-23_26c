#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

void isInRadius(int parameter, int point, int radius){
    if(abs(parameter-point) <= radius) {
        printf("x");
    }
    else {
        printf(" ");
    }
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
    int rozmiarPieńka = wysokoscChoinki/3;
    if (rozmiarPieńka == 0) {
        rozmiarPieńka = 1;
    }
    for (int wiersz = 0; wiersz < rozmiarPieńka; wiersz++) {
        for (int poleNaZnak = 1; poleNaZnak <= dlugoscWiersza; ++poleNaZnak) {
            isInRadius(poleNaZnak, wysokoscChoinki, wysokoscChoinki > 3);
        }
        printf("\n");
    }
    return 0;
}