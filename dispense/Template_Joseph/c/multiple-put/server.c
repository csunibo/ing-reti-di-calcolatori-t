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

#define MAX_DIR                 256
#define DIM_BUFF                100
#define LENGTH_FILE_NAME        64
#define LENGHT_PATENTE          6
#define LENGHT_TIPO             7
#define LENGHT_FOLDER_IMMAGINI  13
#define N                       7

#define max(a, b)        ((a) > (b) ? (a) : (b))

/********************************************************/

/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv) {
    int                 listenfd, connfd, udpfd, fd_file, nready, maxfdp1, res;
    const int           on   = 1;
    int                file_size, file_name_size;
    char                buff, file_name[LENGTH_FILE_NAME], dirName[PATH_MAX], patente[LENGHT_PATENTE], carBuff, path_file[PATH_MAX], c;
    fd_set              rset;
    int                 len, nread, nwrite, num, port;
    struct sockaddr_in  cliaddr, servaddr;
    struct stat statBuff;

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

            printf("Ricevuta richiesta di multiple-put di un direttorio\n");
            len = sizeof(struct sockaddr_in);
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
                if (errno == EINTR)
                    continue;
                else {
                    perror("accept");
                    exit(9);
                }
            }

            // se la consegna non prevede di ricreare il direttorio allora si evitano i controlli e si cancella la parte di scrittura dei caratteri S o C
            if (fork() == 0) { /* processo figlio che serve la richiesta di operazione */
                close(listenfd);
                printf("Dentro il figlio, pid=%i\n", getpid());

                    for (;;) {
                        if ((read(connfd, &dirName, sizeof(dirName))) <= 0) {
                            perror("read");
                            break;
                        }
                        printf("Direttorio da ricevere: %s\n", dirName);

                        // non controllo se mkdir fallisce perché di solito fallisce se il direttorio con quel nome esiste già sul file system
                        mkdir(dirName, 0777);

                        if ((dir = opendir(dirName)) == NULL) {
                            printf("Errore nella creazione del direttorio\n");
                            write(connfd, "N", 1);
                            continue;
                        }  

                        carBuff = 'S';
                        //scrivo S se il direttorio è stato creato correttamente
                        write(connfd, &carBuff, 1);

                        memset(file_name, 0, sizeof(file_name));

                        //a inizio del while leggo dimensione del nome del file
                        while (read(connfd, &file_name_size, sizeof(file_name_size)) > 0){
                            if(file_name_size == -1) break;

                            printf("[Debug File Name] Ricevuta dimensione (%d byte) del nome del file\n", file_name_size);

                            //leggo il nome del file
                            read(connfd, file_name, file_name_size);
                            printf("[Debug File Name] Ricevuto nome del file: %s\n", file_name);
                            //creo il path del file
                            sprintf(path_file, "%s/%s",dirName, file_name);

                            //controllo che il file non sia già presente sul file system, scrivo S per saltare il downlowd del file, C per confermarlo
                            if(stat(path_file, &statBuff) == 0){
                                //esiste, quindi non lo devo scaricare
                                printf("File già presente sul file system, notifico il client\n");
                                c = 'S';
                                write(connfd, &c, 1);
                                continue;
                            }

                            c = 'C';
                            write(connfd, &c, 1);

                            //apro il file
                            int fd = open(path_file, O_CREAT | O_RDWR, 0666);

                            //leggo la dimensione del file
                            read(connfd, &file_size, sizeof(file_size));
                            printf("[Debug File Size] Ricevuta dimensione del file: %d\n", file_size);

                            printf("Leggo il file...\n");

                            nread = 0;
                            nwrite = 0;
                            do {
                                nread += read(connfd,&buff, sizeof(buff));
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

                    }// for

                    printf("Figlio %i: chiudo connessione e termino\n", getpid());
                    close(connfd);
                    exit(0);
            } // figlio
            /* padre chiude la socket dell'operazione */
            close(connfd);
        } /* fine gestione richieste di multiple-get */

    }     /* ciclo for della select */
}