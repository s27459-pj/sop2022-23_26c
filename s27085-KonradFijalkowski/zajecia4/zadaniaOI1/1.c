#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define BUFFER_SIZE 1024

bool isSourceFileGiven(int numberOfParameters){
    return numberOfParameters == 2 ? true : false;
}

void readInReverseFromFile(FILE *fileToRead, int fileByteSize){
    int readCharNumber = 0;
    char readChar;

    while(readCharNumber<=fileByteSize){
        readCharNumber++;
        fseek(fileToRead, -readCharNumber, SEEK_END);
        readChar = fgetc(fileToRead);
        if(readChar == '\n') {
            readCharNumber++;
        }
            printf("%c", readChar);
        //https://stackoverflow.com/questions/21844064/size-of-string-literal-consisting-of-escaped-characters
    }
}

int main(int argc, char** argv){
    FILE *fileToRead;
    int fileByteSize;

    if(isSourceFileGiven(argc)) {
        fileToRead = fopen( argv[1], "r");
        if(fileToRead == NULL) {
            return 0;
        }
        fseek(fileToRead, 0, SEEK_END);
        fileByteSize = ftell(fileToRead);

        readInReverseFromFile(fileToRead, fileByteSize);
    }
    else{
        if(feof(stdin)){
            printf("Stdin contains eof\n");
        }
        int read;
        void *bufferContent = malloc(BUFFER_SIZE);

        fileToRead = fopen( "tempBuffer.txt", "w+");

        while(read = fread(bufferContent, 1, BUFFER_SIZE, stdin)){
            fwrite(bufferContent, 1, read, fileToRead);
        }

        fileByteSize = ftell(fileToRead);
        readInReverseFromFile(fileToRead, fileByteSize);

        remove("tempBuffer.txt");
        free(bufferContent);
    }
    fclose(fileToRead);
    return 0;
}
