#include <stdio.h>
#include <math.h>
int main(){
    int wysokoscChoinki;
    printf("Podaj wysokosc choinki:");
    scanf_s("%d", &wysokoscChoinki);
    int dlugoscWiersza = wysokoscChoinki*2;
    for (int wiersz = 0; wiersz < wysokoscChoinki; ++wiersz){
        for (int poleNaZnak = 0; poleNaZnak <= dlugoscWiersza; ++poleNaZnak) {
            if(abs(poleNaZnak-wysokoscChoinki) <= (wiersz)) {
                printf("x");
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }
    return 0;
}