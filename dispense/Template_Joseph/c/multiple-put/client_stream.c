/*
Stream client
Client per richiedere l'invio di tutti i filedi un direttorio (multiple-get) 
*/

#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DIM_BUFF         100
#define LENGTH_FILE_NAME 64 
#define MAX_DIR 4096

int main(int argc, char *argv[]) {
    int                sd, nread, nwrite, port, fineDirettorio;
    char               c, ok, file_name[LENGTH_FILE_NAME], dirName[MAX_DIR], path_file[MAX_DIR], carBuff;
    int file_size, file_name_size;
    struct stat statBuff;
    struct hostent    *host;
    struct sockaddr_in servaddr;
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3) {
        printf("Error:%s serverAddress serverPort\n", argv[0]);
        exit(1);
    }
    printf("Client avviato\n");

    /* PREPARAZIONE INDIRIZZO SERVER ----------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host                = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(2);
    }

    nread = 0;
    while (argv[2][nread] != '\0') {
        if ((argv[2][nread] < '0') || (argv[2][nread] > '9')) {
            printf("Secondo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("Porta scorretta...");
        exit(2);
    }

    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
    servaddr.sin_port        = htons(port);

    /* CREAZIONE E CONNESSIONE SOCKET (BIND IMPLICITA) ----------------- */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("apertura socket ");
        exit(3);
    }
    printf("Creata la socket sd=%d\n", sd);

    if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0) {
        perror("Errore in connect");
        exit(4);
    }
    printf("Connect ok\n");

    /* CORPO DEL CLIENT: */
    /* ciclo di accettazione di richieste di file ------- */
    printf("Inserire nome del direttorio da inviare: ");

    while (gets(dirName)) {

        printf("Apro il direttorio %s\n", dirName);

        //apro il direttorio
        if ((dir = opendir(dirName)) == NULL) {
            printf("Direttorio inesistente\n");
            continue;
        }     
        
        if (write(sd, dirName, MAX_DIR) < 0) {
            perror("write");
            break;
        }
        printf("dirName %s inviata... \n", dirName);

        if (read(sd, &ok, 1) < 0) {
            perror("read");
            break;
        }

        if (ok == 'S'){
            printf("Apro il direttorio %s\n", dirName);

            while ((entry = readdir(dir)) != NULL) {
                if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

                char* file_name = entry->d_name;

                char path_file[PATH_MAX];
                snprintf(path_file, PATH_MAX, "%s/%s", dirName, file_name);
                printf("\n[Debug] fullpath: %s\n", path_file);

                if (lstat(path_file, &statbuf) == -1) {
                    perror("lstat");
                    exit(EXIT_FAILURE);
                }

                //controllo se il file è un file regolare
                if (S_ISREG(statbuf.st_mode)) {
                    int fd_file = open(path_file, O_RDONLY);
                    if (fd_file == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }                        

                    printf("File: %s", file_name);

                    //mando dimensione del nome del file
                    file_name_size = strlen(file_name);
                    printf("[Debug] Dimensione del nome del file: %d\n", file_name_size);
                    write(sd, &file_name_size, sizeof(file_name_size));

                    //mando il nome del file
                    write(sd, file_name, file_name_size);

                    //leggo risposta del server per sapere se mandare il file o meno
                    read(sd, &carBuff, 1);

                    if(carBuff == 'S'){
                        printf("File già esistente sul server, lo salto\n");
                        continue;
                    } 

                    //Mando la dimensione del file al server
                    file_size = statbuf.st_size;
                    printf("Dimensione del file: %d\n", file_size);

                    write(sd, &file_size, sizeof(file_size));

                    char byte_buff;
                    /* lettura dal file (a byte) e scrittura sulla socket */
                    printf("Leggo e invio il file %s\n", file_name);
                    while ((nread = read(fd_file, &byte_buff, sizeof(byte_buff))) > 0) {
                        if ((nwrite += write(sd, &byte_buff, sizeof(byte_buff))) < 0) {
                            perror("write");
                            break;
                        }
                    }
                    printf("Terminato invio  del file %s\n", file_name);
                    close(fd_file);
                }
            } //while
        } else if (ok == 'N')
            printf("Errore da parte del server nel creare il direttorio\n");
        else {
            printf("Errore di protocollo\n"); // controllare sempre che il protocollo sia rispettato
        }
        
        printf("Terminata multile-get del direttorio %s\n\n", dirName); 

        /* invio al server segnale di terminazione multiple-put: -1 */
        long term = -1;
        write(sd, &term, sizeof(term));
        closedir(dir);

        printf("Inserire nome del direttorio da inviare: ");
    } // while
    printf("\nClient: termino...\n");
    shutdown(sd, 0);
    shutdown(sd, 1);
    close(sd);
    exit(0);
}
