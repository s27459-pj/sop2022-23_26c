#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

bool isSourceFileGiven(int numberOfParameters){
    return numberOfParameters == 2 ? true : false;
}

int main(int argc, char** argv){
    FILE *fileToRead;
    FILE *fileToWrite;
    char *linesLengthInBytes = malloc(500*sizeof(char));
    char readLine[100];
    int fileByteSize;

    if(isSourceFileGiven(argc)) {
        fileToRead = fopen( argv[1], "r");
        if(fileToRead == NULL) {
            return 0;
        }
        fileToWrite = fopen("wynik.txt", "w");
        fseek(fileToRead, 0, SEEK_END);
        fileByteSize = ftell(fileToRead);
        int readCharNumber = 0;

        while(readCharNumber<fileByteSize){
            readCharNumber++;
            fseek(fileToRead, -readCharNumber, SEEK_END);
            if(!isdigit(fgetc(fileToRead))) {
                readCharNumber++;
            }
            fseek(fileToRead, -readCharNumber, SEEK_END);
            printf("%c", fgetc(fileToRead));
        }
    }

//    else{
//        while(fscanf(stdin, "%s", readLine) != EOF){
//            printf("%s\n", readLine);
//        }
//    }
    fclose(fileToRead);
    fclose(fileToWrite);
    return 0;
}
