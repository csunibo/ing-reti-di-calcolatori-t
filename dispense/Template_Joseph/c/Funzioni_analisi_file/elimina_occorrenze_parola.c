#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define LINE_LENGTH 256
#define WORD_LENGTH 64

int main(){

    int             res, file, fd_temp, count_letters, nread;
    char            fileName[256], fileNameTemp[LINE_LENGTH], read_char, err[128];
    char            word_buffer[WORD_LENGTH], parola[WORD_LENGTH], separatori[32];

    strcpy(fileName, "file.txt");
    strcpy(parola, "parola");
    strcpy(separatori, " \t\r\n.,;:'\"()-`*&^#@!$\\|[]{}?/");

    res = 0;
    /* Verifico file */
    if ((file = open(fileName, O_RDONLY)) < 0) {
        perror("Errore apertura file sorgente");
        res = -1;
    }

    /* Se il file esiste */
            if (res == 0) {
                /* Creo il file temporaneo */
                fileNameTemp[0] = '\0';
                strcat(fileNameTemp, fileName);
                strcat(fileNameTemp, "_temp");
                if ((fd_temp = open(fileNameTemp, O_CREAT | O_WRONLY, 0777)) < 0) {
                    perror("Errore apertura file");
                    res = -1;
                } else { // Se il file temporaneo è stato creato correttamente
                    /* Eliminazione della parola */

                    count_letters = 0;
                    while ((nread = read(file, &read_char, sizeof(char))) != 0) {
                        if (nread < 0) {
                            sprintf(err, "(PID %d) impossibile leggere dal file", getpid());
                            perror(err);
                            res = -1;
                            break;
                        } else {
                            // La presenza di un separatore indica la fine di una parola. Altri
                            // separatori? Punteggiatura? Sarebbe meglio avere un array di
                            // separatori, magari letto da un file di configurazione all'avvio del
                            // server.
                            int fineParola = 0;
                            for(int i=0;i<strlen(separatori); i++){
                                if(read_char == separatori[i]){
                                    fineParola = 1;
                                    break;
                                }
                            }
                            if (fineParola) {
                                word_buffer[count_letters] = '\0';
                                if (strcmp(word_buffer, parola)) { // parola diversa
                                    write(fd_temp, &word_buffer, strlen(word_buffer));
                                } else {
                                    res++;
                                }
                                count_letters = 0;

                                write(fd_temp, &read_char, 1);
                            } else { // carattere
                                word_buffer[count_letters] = read_char;
                                count_letters++;
                            }
                        }
                    }
                }
            }
            if (res >= 0) {
                printf("Nel file %s sono state eliminate le parole uguali a %s\n", fileName, parola);
            }

            close(file);
            close(fd_temp);

            // Rinomino il file temporaneo, sovrascrivendo il file originale
            rename(fileNameTemp, fileName);

}