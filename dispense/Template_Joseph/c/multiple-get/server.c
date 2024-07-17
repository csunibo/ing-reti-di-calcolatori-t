/* Server Select
 * 	Un solo figlio per tutti i file.
 */

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

#define DIM_BUFF                100
#define LENGTH_FILE_NAME        64
#define LENGHT_TARGA            8 
#define LENGHT_PATENTE          6
#define LENGHT_TIPO             7
#define LENGHT_FOLDER_IMMAGINI  13
#define N                       7

#define max(a, b)        ((a) > (b) ? (a) : (b))


void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv) {
    int                 listenfd, connfd, udpfd, fd_file, nready, maxfdp1, res;
    const int           on   = 1;
    long                file_size, file_name_size;
    char                buff[DIM_BUFF], nome_file[LENGTH_FILE_NAME], nome_dir[PATH_MAX], targa[LENGHT_TARGA], patente[LENGHT_PATENTE], carBuff;
    fd_set              rset;
    int                 len, nread, nwrite, num, port;
    struct sockaddr_in  cliaddr, servaddr;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    }
    nread = 0;
    while (argv[1][nread] != '\0') {
        if ((argv[1][nread] < '0') || (argv[1][nread] > '9')) {
            printf("Terzo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        printf("Porta scorretta...");
        exit(2);
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

    printf("Server avviato\n");

    /* CREAZIONE SOCKET TCP ------------------------------------------------------ */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("apertura socket TCP ");
        exit(1);
    }
    printf("Creata la socket TCP d'ascolto, fd=%d\n", listenfd);

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket TCP");
        exit(2);
    }
    printf("Set opzioni socket TCP ok\n");

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket TCP");
        exit(3);
    }
    printf("Bind socket TCP ok\n");

    if (listen(listenfd, 5) < 0) {
        perror("listen");
        exit(4);
    }
    printf("Listen ok\n");

    /* CREAZIONE SOCKET UDP ------------------------------------------------ */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpfd < 0) {
        perror("apertura socket UDP");
        exit(5);
    }
    printf("Creata la socket UDP, fd=%d\n", udpfd);

    if (setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket UDP");
        exit(6);
    }
    printf("Set opzioni socket UDP ok\n");

    if (bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket UDP");
        exit(7);
    }
    printf("Bind socket UDP ok\n");

    /* AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE -------------------------------- */
    signal(SIGCHLD, gestore);

    /* PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR ------------------------- */
    FD_ZERO(&rset);
    maxfdp1 = max(listenfd, udpfd) + 1;



    /* CICLO DI RICEZIONE EVENTI DALLA SELECT ----------------------------------- */
    for (;;) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);

        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;
            else {
                perror("select");
                exit(8);
            }
        }

        /* SOCKET CON CONNESSIONE: GESTIONE RICHIESTE DI MULTIPLE-GET DI UN DIRETTORIO ------------------------------------- */
        if (FD_ISSET(listenfd, &rset)) {
            DIR *dir;
            struct dirent *entry;
            struct stat statbuf;

            printf("Ricevuta richiesta di multiple-get di un direttorio\n");
            len = sizeof(struct sockaddr_in);
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
                if (errno == EINTR)
                    continue;
                else {
                    perror("accept");
                    exit(9);
                }
            }

            if (fork() == 0) { /* processo figlio che serve la richiesta di operazione */
                close(listenfd);
                printf("Dentro il figlio, pid=%i\n", getpid());


                    for (;;) {
                        if ((read(connfd, &targa, sizeof(targa))) <= 0) {
                            perror("read");
                            break;
                        }
                        printf("Richieste immagini per la targa %s\n", targa);

                        sprintf(nome_dir, "%s_img", targa);
                        printf("[Debug] nome_dir: %s\n", nome_dir);

                        /*
                        strcpy(nome_dir, targa);
                        strcat(nome_dir, "_img");
                        */

                        printf("Apro il direttorio %s\n", nome_dir);

                        if ((dir = opendir(nome_dir)) == NULL) {
                            printf("Direttorio inesistente\n");
                            write(connfd, "N", 1);
                            continue;
                        }       

                        carBuff = 'S';
                        //scrivo S se il direttorio esiste
                        write(connfd, &carBuff, 1);

                        while ((entry = readdir(dir)) != NULL) {
                            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

                            char* file_name = entry->d_name;

                            char path_file[PATH_MAX];
                            snprintf(path_file, PATH_MAX, "%s/%s", nome_dir, file_name);
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
                                printf("[Debug] Dimensione del nome del file: %ld\n", file_name_size);
                                write(connfd, &file_name_size, sizeof(file_name_size));

                                //mando il nome del file
                                write(connfd, file_name, file_name_size);

                                //leggo risposta del client per sapere se mandare il file o meno
                                read(connfd, &carBuff, 1);

                                if(carBuff == 'S'){
                                    printf("File già esistente sul client, lo salto\n");
                                    continue;
                                } 

                                //Mando la dimensione del file al client
                                file_size = statbuf.st_size;
                                printf("Dimensione del file: %ld\n", file_size);

                                write(connfd, &file_size, sizeof(file_size));

                                char byte_buff;
                                /* lettura dal file (a byte) e scrittura sulla socket */
                                printf("Leggo e invio il file %s\n", file_name);
                                while ((nread = read(fd_file, &byte_buff, sizeof(byte_buff))) > 0) {
                                    if ((nwrite += write(connfd, &byte_buff, sizeof(byte_buff))) < 0) {
                                        perror("write");
                                        break;
                                    }
                                }
                                printf("Terminato invio  del file %s\n", file_name);
                            }
                        } //while
                        printf("Terminata multile-get del direttorio %s\n\n", nome_dir); 

                        /* invio al client segnale di terminazione multiple-get: -1 */
                        long term = -1;
                        write(connfd, &term, sizeof(term));
                        close(fd_file);
                        closedir(dir);
                    }// for

                    printf("Figlio %i: chiudo connessione e termino\n", getpid());
                    close(connfd);
                    exit(0);
            } // figlio
            /* padre chiude la socket dell'operazione */
            close(connfd);
        } /* fine gestione richieste di multiple-get */

        /* GESTIONE RICHIESTE AGGIORNAMENTO STRUTTURA------------------------------------------ */
        
    }     /* ciclo for della select */
}