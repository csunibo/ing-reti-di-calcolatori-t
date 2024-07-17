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
#define LENGHT_TARGA 8 
#define MAX_DIR 256

int main(int argc, char *argv[]) {
    int                sd, nread, nwrite, port, fineDirettorio;
    char               c, ok, file_name[LENGTH_FILE_NAME], targa[LENGHT_TARGA], dirName[MAX_DIR], path_file[MAX_DIR];
    long file_size, file_name_size;
    struct stat statBuff;
    struct hostent    *host;
    struct sockaddr_in servaddr;
    DIR *dir;

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
    printf("Inserire targa per scaricare le immagini: ");

    while (gets(targa)) {
        
        if (write(sd, targa, LENGHT_TARGA) < 0) {
            perror("write");
            break;
        }
        printf("Targa %s inviata... \n", targa);

        if (read(sd, &ok, 1) < 0) {
            perror("read");
            break;
        }

        if (ok == 'S') {
            printf("Ricevo i file\n");
            char buff;

            sprintf(dirName, "%s", targa);
            printf("[Debug] dirName: %s\n", dirName);
            
            // non controllo se mkdir fallisce perché di solito fallisce se il direttorio con quel nome esiste già sul file system
            mkdir(dirName, 0777);

            if ((dir = opendir(dirName)) == NULL) {
                printf("Errore nella creazione del direttorio\n");
                continue;
            }  

            memset(file_name, 0, sizeof(file_name));

            //a inizio del while leggo dimensione del nome del file
            while (read(sd, &file_name_size, sizeof(file_name_size)) > 0){
                if(file_name_size == -1) break;

                printf("[Debug File Name] Ricevuta dimensione (%ld byte) del nome del file\n", file_name_size);

                //leggo il nome del file
                read(sd, file_name, file_name_size);
                printf("[Debug File Name] Ricevuto nome del file: %s\n", file_name);
                //creo il path del file
                sprintf(path_file, "%s/%s",dirName, file_name);

                //controllo che il file non sia già presente sul file system, scrivo S per saltare il downlowd del file, C per confermarlo
                if(stat(path_file, &statBuff) == 0){
                    //esiste, quindi non lo devo scaricare
                    printf("File già presente sul file system, notifico il server\n");
                    c = 'S';
                    write(sd, &c, 1);
                    continue;
                }

                c = 'C';
                write(sd, &c, 1);

                //apro il file
                int fd = open(path_file, O_CREAT | O_RDWR, 0666);

                //leggo la dimensione del file
                read(sd, &file_size, sizeof(file_size));
                printf("[Debug File Size] Ricevuta dimensionde del file: %ld\n", file_size);

                printf("Leggo il file...\n");

                nread = 0;
                nwrite = 0;
                do {
                    nread += read(sd,&buff, sizeof(buff));
                    nwrite += write(fd, &buff, sizeof(buff));
                }while (nread<file_size);
                if(nread<0){
                    perror("read");
                }
                if(nwrite < 0){
                    perror("write");
                }
                printf("[Debug File Size] Numero di byte letti: %d\n", nread);
                printf("[Debug Filze Size] Numero di byte scritti: %d\n", nwrite);
                close(fd);
                
            }
            printf("Terminata ricezione\n\n");
        } else if (ok == 'N')
            printf("Direttorio inesistente\n");
        else {
            printf("Errore di protocollo\n"); // controllare sempre che il protocollo sia rispettato
        }
        printf("Inserire targa per scaricare immagini: ");
    } // while
    printf("\nClient: termino...\n");
    shutdown(sd, 0);
    shutdown(sd, 1);
    close(sd);
    exit(0);
}
