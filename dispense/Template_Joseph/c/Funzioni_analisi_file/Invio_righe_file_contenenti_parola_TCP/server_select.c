/**
 * server_select.c
 *  Il server discrimina due servizi con la select:
 *    + trasferimento delle linee del file che contengono la parola (TCP)
 *    + controllo dell'esistenza in un file di una linea con una parola specifica (UDP)
 **/

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h> 

#define LINE_LENGTH 1024
#define WORD_LENGTH 64
#define FILE_NAME_LENGTH 64
#define max(a, b)   ((a) > (b) ? (a) : (b))

/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/
typedef struct {
    char fileName[FILE_NAME_LENGTH];
    char parola[WORD_LENGTH];
} req_t;

/********************************************************/

int main(int argc, char **argv) {
    struct sockaddr_in cliaddr, servaddr;
    struct hostent    *hostTcp, *hostUdp;
    int                port, listen_sd, conn_sd, udp_sd, nread, maxfdp1, len ,file;
    int                ris ,nrecv, dimLine;
    const int          on = 1;
    fd_set             rset;
    req_t              req;
    int                fd_sorg_udp, count_letters, rightLine, rightFile;
    char               read_char, err[128], word_buffer[WORD_LENGTH];
    char           charBuff[2], fileNameTemp[LINE_LENGTH], line[LINE_LENGTH];
    char           separatori[32], fileName[FILE_NAME_LENGTH] ,parola[WORD_LENGTH];
    char            zero = 0;
    DIR           *dir1, *dir2, *dir3;
    struct dirent *dd1, *dd2;

    strcpy(separatori, " \t\r\n.,;:'\"()-`*&^#@!$\\|[]{}?/");

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port \n", argv[0]);
        exit(1);
    } else {
        nread = 0;
        while (argv[1][nread] != '\0') {
            if ((argv[1][nread] < '0') || (argv[1][nread] > '9')) {
                printf("Secondo argomento non intero\n");
                exit(2);
            }
            nread++;
        }
        port = atoi(argv[1]);
        if (port < 1024 || port > 65535) {
            printf("Porta scorretta...");
            exit(2);
        }
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

    /* CREAZIONE E SETTAGGI SOCKET TCP --------------------------------------- */
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0) {
        perror("creazione socket ");
        exit(3);
    }
    printf("Server: creata la socket d'ascolto per le richieste di ordinamento, fd=%d\n",
           listen_sd);

    if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket d'ascolto");
        exit(3);
    }
    printf("Server: set opzioni socket d'ascolto ok\n");

    if (bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket d'ascolto");
        exit(3);
    }
    printf("Server: bind socket d'ascolto ok\n");

    if (listen(listen_sd, 5) < 0) {
        perror("listen");
        exit(3);
    }
    printf("Server: listen ok\n");

    /* CREAZIONE E SETTAGGI SOCKET UDP --------------------------------------- */
    udp_sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sd < 0) {
        perror("apertura socket UDP");
        exit(4);
    }
    printf("Creata la socket UDP, fd=%d\n", udp_sd);

    if (setsockopt(udp_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket UDP");
        exit(4);
    }
    printf("Set opzioni socket UDP ok\n");

    if (bind(udp_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket UDP");
        exit(4);
    }
    printf("Bind socket UDP ok\n");

    signal(SIGCHLD, gestore);

    /* PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR ------------------------- */
    FD_ZERO(&rset);
    maxfdp1 = max(listen_sd, udp_sd) + 1;

    /* CICLO DI RICEZIONE RICHIESTE --------------------------------------------- */
    for (;;) {
        FD_SET(listen_sd, &rset);
        FD_SET(udp_sd, &rset);

        if ((nread = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("select");
                exit(5);
            }
        }
        /* CONTROLLO ESISTENZA PAROLA (UDP) ----------------------------------- */
        if (FD_ISSET(udp_sd, &rset)) {
            printf("Ricevuta richiesta di UDP: eliminazione di una parola\n");
            len = sizeof(struct sockaddr_in);

            if (recvfrom(udp_sd, &req, sizeof(req), 0, (struct sockaddr *)&cliaddr, &len) < 0) {
                perror("recvfrom ");
                continue;
            }

            /* Stampo a video le informazioni sul client */
            hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (hostUdp == NULL) {
                printf("Operazione richiesta da: %s\n", inet_ntoa(cliaddr.sin_addr));
            } else {
                printf("Operazione richiesta da: %s %i\n", hostUdp->h_name,
                       (unsigned)ntohs(cliaddr.sin_port));
            }

            printf("Operazione richiesta sul file: %s per la parola: %s\n", req.fileName, req.parola);

            // rightFile vale 1 se il file contiene la parola, -1 se il file non esiste, -2 se il file non contiene la parola
            rightFile = -1;

            /* Verifico file */
            if ((fd_sorg_udp = open(req.fileName, O_RDONLY)) < 0) {
                perror("Errore apertura file sorgente");
                
            }
            else{
                /* Se il file esiste */
                count_letters = 0;
                rightFile = -2;

                //leggo un carattere alla volta
                while ((nread = read(fd_sorg_udp, &read_char, sizeof(char))) != 0) {
                    if (nread < 0) {
                        sprintf(err, "(PID %d) impossibile leggere dal file", getpid());
                        perror(err);
                        break;
                    } else {
                        bool fineParola = false;

                        for(int i=0;i<strlen(separatori); i++){
                            if(read_char == separatori[i]){
                                fineParola = true;
                                break;
                            }
                        }
                        if (fineParola) {
                            word_buffer[count_letters] = '\0';
                            if (strcmp(word_buffer, req.parola)==0) { 
                                // parola uguale
                                rightFile = 1;
                                break;
                            }
                            count_letters = 0;
                        } else { // carattere
                            word_buffer[count_letters] = read_char;
                            count_letters++;
                        }
                    }
                } //while read
                if (rightFile) {
                    printf("Il file %s contiene la parola %s\n", req.fileName, req.parola);
                }
                else{
                    printf("Il file %s contiene la parola %s\n", req.fileName, req.parola);
                }
                        
                close(fd_sorg_udp);
            }
            // -1 se il file non contiene la parola o non esiste, 0 altrimenti
            if(sendto(udp_sd, &rightFile, sizeof(rightFile), 0, (struct sockaddr *)&cliaddr, len) < 0){
                perror("Sendto esito");
                exit(1);
            }
        }
        /* TRASFERIMENTO LINEE DEL FILE (TCP) ----------------------------- */
        if (FD_ISSET(listen_sd, &rset)) {
            printf("Ricevuta richiesta TCP: file del direttorio secondo livello\n");
            len = sizeof(cliaddr);

            if ((conn_sd = accept(listen_sd, (struct sockaddr *)&cliaddr, &len)) < 0) {
                if (errno == EINTR) {
                    perror("Forzo la continuazione della accept");
                    continue;
                } else {
                    perror("Error in accept: ");
                    exit(6);
                }
            }
            if (fork() == 0) {
                close(listen_sd);

                // Stampo a video le informazioni sul client
                hostTcp =
                    gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
                if (hostTcp == NULL) {
                    printf("Server (figlio): host client e' %s\n", inet_ntoa(cliaddr.sin_addr));
                } else {
                    printf("Server (figlio): host client e' %s \n", hostTcp->h_name);
                }

                // Leggo la richiesta del client (nome file e parola)
                while ((nrecv = recv(conn_sd, &req, sizeof(req), 0)) > 0) {
                    printf("Ricevuta richiesta di invio linee del file %s che contengono la parola %s\n", req.fileName, req.parola);

                    // ris vale 0 se il file esiste. -1 altrimenti
                    ris = 0;
                    /* Verifico file */
                    if ((file = open(req.fileName, O_RDONLY)) < 0) {
                        perror("Errore apertura file sorgente");
                        ris = -1;
                    }

                    /* Se il file esiste */
                    if (ris == 0){
                        if (send(conn_sd, &ris, sizeof(ris), 0) <= 0){
                            perror("Send ris");
                            exit(1);
                        }
                        rightLine = 0;
                        count_letters = 0;
                        dimLine = 0;
                        memset(line, 0, sizeof(line));
                        strcpy(line, "\0");

                        while ((nread = read(file, &read_char, sizeof(char))) != 0) {
                            if (nread < 0) {
                                sprintf(err, "(PID %d) impossibile leggere dal file", getpid());
                                perror(err);
                                ris = -1;
                                break;
                            } else {
                                int fineParola = 0;
                                write(1, &read_char, 1);
                                line[dimLine++] = read_char;

                                if(read_char == '\n'){
                                    if(rightLine){
                                        printf("Invio la stringa al client\n");

                                        if(write(conn_sd, line, strlen(line)) < 0){
                                            perror("Write line");
                                            exit(1);
                                        }
                                    }
                                    rightLine = 0;
                                    dimLine = 0;
                                    memset(line, 0, sizeof(line));
                                    strcpy(line, "\0");
                                }

                                else{
                                    for(int i=0;i<strlen(separatori); i++){
                                        if(read_char == separatori[i]){
                                            fineParola = 1;
                                            break;
                                        }
                                    }
                                    if (fineParola) {
                                        word_buffer[count_letters] = '\0';
                                        if (strcmp(word_buffer, req.parola) == 0) { // parola uguale
                                            rightLine = 1;
                                            printf("\n---------------------TROVATA OCCORRENZA DELLA PAROLA-----------------------------\n");
                                        }
                                        

                                        //printf("\n[Debug] linea: %s\n", line);
                                        count_letters = 0;
                                    } else { // carattere
                                        word_buffer[count_letters] = read_char;
                                        count_letters++;
                                    }
                                }
                            }
                        } //while read from file
                        if(write(conn_sd, &zero, sizeof(zero)) < 0){
                            perror("Write zero");
                            exit(1);
                        }
                    }
                    if(ris < 0){
                        if(write(conn_sd, &ris, sizeof(ris)) < 0){
                            perror("Write ris");
                            exit(1);
                        }
                    }
                } //while connection is open
                
                close(file);

                // Libero risorse
                printf("Figlio TCP terminato, libero risorse e chiudo. \n");
                close(conn_sd);
                exit(0);
            }               // if fork
            close(conn_sd); // padre
        }                   // if TCP
    }                       // for
} // main
