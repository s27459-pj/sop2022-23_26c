#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

bool isSourceFileGiven(int numberOfParameters){
    return numberOfParameters == 2 ? true : false;
}

void printDigitsInReverse(int number){
    if(number<0){
        number = abs(number);
        do{
            printf("%d", number%10);
            number = number/10;
        }while(number%10 != 0);
        printf("-");
    }
    else {
        do {
            printf("%d", number % 10);
            number = number / 10;
        } while (number % 10 != 0);
    }
};

int main(int argc, char** argv){
    FILE *fileToRead;
    int numberCount;
    int *numbers;
    int i;

    if(isSourceFileGiven(argc)) {
        fileToRead = fopen(argv[1], "r");
        if(fileToRead == NULL) {
            return 0;
        }
        fscanf(fileToRead, "%d", &numberCount);
        numbers = malloc(sizeof (int)*numberCount);

        i = 0;
        while(i<numberCount){
            fscanf(fileToRead, "%d", &numbers[i]);
            i++;
        }
    }
    else {
        fscanf(stdin, "%d", &numberCount);
        numbers = malloc(sizeof (int)*numberCount);
        i = 0;
        while(i<numberCount){
            fscanf(stdin, "%d", &numbers[i]);
            i++;
        }
    }
    for (i = numberCount-1; i >= 0; i--){
        printDigitsInReverse(numbers[i]);
        if(i != 0){
            printf("\n");
        }
    }
    free(numbers);
    fclose(fileToRead);
    return 0;
}
