#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define FILE_NAME "/tmp/chat.tmp"
#define BUFFER_SIZE 50

void writeToFile();
void readFromFile();
/* OPIS DZIAŁANIA: 
*
*  program mozna odpalic w kilku instancjach, chat przedstawia pid procesu ktory pisze 
*  oraz jego wiadomosc:
*   [PID]34945: hi
*   [PID]35035: hello
*/

int main(int argc, char const *argv[])
{
    if (fork() == 0)
    {   
        readFromFile();
    } else {
        writeToFile();
    }
    
    return 0;
}

void readFromFile() {
    struct stat filestat;
    off_t lastSize;
    FILE *fp;
    char buff [BUFFER_SIZE];

    stat(FILE_NAME, &filestat);
    lastSize = filestat.st_size;
    
    while (stat(FILE_NAME, &filestat) == 0)
    {
        if (filestat.st_size != lastSize ) {
            fp = fopen(FILE_NAME,"r");
            fseek(fp, lastSize, SEEK_SET);
            while (fgets(buff, BUFFER_SIZE, fp) != NULL)
            {
                printf("%s", buff);
            }
            fclose(fp);
        }
    lastSize = filestat.st_size;
    sleep(1);
    }
}

void writeToFile() {
    FILE *fp;
    char buffer[BUFFER_SIZE];
    while(scanf("%s",buffer) > 0)
    {
        fp = fopen(FILE_NAME,"a");
        fprintf(fp,"[PID]%d: %s\n",getpid(),buffer);
        fclose(fp);
    }

}
