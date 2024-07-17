//-----------CHECK-IF-FILE-IS-.txt------------------

#define DATA 11
#include <bits/posix2_lim.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

void isTexrFile() {
    char extension[]     = ".txt";
    int fd_file;
    char *fileName = "file1.txt";
    struct stat stat;

    // Open file
    fd_file = open(fileName, O_RDONLY);
    if (fd_file < 0) {
        printf("Errore apertura file");
        /*
        printf("FILE: %s\n", fullpath);
        perror("Open file:");
        result.nb_files = -1;
        return (&result);
        */
    }
    int lastindex = strlen(fileName);
    // Get file info
    int ret = fstat(fd_file, &stat);
    // Check if it is a regular file and if it is a .txt file
    if (S_ISREG(stat.st_mode) && 
        fileName[lastindex-1] == extension[3] &&
        fileName[lastindex-2] == extension[2] &&
        fileName[lastindex-3] == extension[1] &&
        fileName[lastindex-4] == extension[0]) {
        
        // do something on file
    }
}

// conto il numero di righe che iniziano con una lettera minuscola e che hanno un numero di occorrenze del caratter c pari o superiore a occ
void contaOccorrenzeCarattere() {
    int dimLine, count_letters, nread, fd, ris, occ;
    char read_char, err[128], line_buffer[256], c;


    dimLine = 0;
    count_letters = 0;
    while ((nread = read(fd, &read_char, sizeof(char))) != 0) {
        if (nread < 0) {
            sprintf(err, "(PID %d) impossibile leggere dal file", getpid());
            perror(err);
            ris = -1;
            break;
        } else {
            if(read_char == '\n'){
                line_buffer[dimLine++] = '\0';
                if(line_buffer[0] >= 'a' && line_buffer[0] <= 'z'){
                    for(int i=0;i<dimLine;i++){
                        if(line_buffer[i] == c) count_letters++;
                    }
                    if (count_letters >= occ) ris++;
                }
                dimLine = 0;
                count_letters = 0;
            }
            else{
                line_buffer[dimLine++] = read_char;
            }  
        }
    }
}


void leggiFileperRiga() {
    int dimLine, count_letters, nread, fd;
    char read_char, err[128], line_buffer[256];

    // conto il numero di righe che iniziano con una lettera minuscola e che hanno un numero di occorrenze del caratter c pari o superiore a occ
    dimLine = 0;
    count_letters = 0;
    while ((nread = read(fd, &read_char, sizeof(char))) != 0) {
        if (nread < 0) {
            sprintf(err, "Impossibile leggere dal file");
            perror(err);
            break;
        } else {
            if(read_char == '\n'){
                line_buffer[dimLine++] = '\0';

                // esegui operazioni sulla linea

                dimLine = 0;
                count_letters = 0;
            }
            else{
                line_buffer[dimLine++] = read_char;
            }  
        }
    }
}

void whilegetchar() {
    char c;
    while ((c = getchar()) != EOF) {

        //alla fine del while 
        getchar();
    }
}


//formato gg/mm/aaaa
int checkData(char data[DATA]){
    int res = -1;


    if( strlen(data) == 10 &&
        data[2] == '/' && data[5] == '/'  &&
        data[0]>= '0' && data[0] <= '9' &&
        data[1]>= '0' && data[1] <= '9' &&
        data[3]>= '0' && data[3] <= '9' &&
        data[4]>= '0' && data[4] <= '9' &&
        data[6]>= '0' && data[6] <= '9' &&
        data[7]>= '0' && data[7] <= '9' &&
        data[8]>= '0' && data[8] <= '9' &&
        data[9]>= '0' && data[9] <= '9'
    ) res = 0;


    return res;
}
