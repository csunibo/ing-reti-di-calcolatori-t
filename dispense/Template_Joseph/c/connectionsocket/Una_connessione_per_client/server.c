/* server.c
il server gestisce in modo parallelo l’interazione col client generando, all’arrivo di ogni richiesta, un nuovo figlio che gestisca l’interazione col client fino alla chiusura della connessione da parte del client. Il figlio con una prima lettura discrimina il tipo di funzionalità richiesto, poi gestisce opportunamente invii e ricezioni per l’operazione richiesta e si pone in attesa di nuove richieste dallo stesso client; alla lettura dell’EOF il figlio termina.
*/

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
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

#define DIM_DIR     80
#define LINE_LENGTH 128
#define NAME_LENGTH 64
#define N 20
#define K 10

/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
    printf("stato figlio: %d\n", stato >> 8);
}

/********************************************************/


/********************************************************/
typedef struct {
    char nome[NAME_LENGTH];
    char stato[3];
    char utenti[K][NAME_LENGTH];
} Stato;

/********************************************************/





int main(int argc, char **argv) {
    int                listen_sd, conn_sd, nread;
    long               nsend;
    int                port, len, lineCount;
    char               ccar, err[LINE_LENGTH], nomeStanza[NAME_LENGTH], c;
    const int          on = 1;
    Stato              stanze[N];
    struct sockaddr_in cliaddr, servaddr;
    struct hostent    *host;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    } else {
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
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

    /* CREAZIONE E SETTAGGI SOCKET D'ASCOLTO --------------------------------------- */
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0) {
        perror("creazione socket ");
        exit(1);
    }
    printf("Server: creata la socket d'ascolto per le richieste di ordinamento, fd=%d\n",
           listen_sd);

    if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket d'ascolto");
        exit(1);
    }
    printf("Server: set opzioni socket d'ascolto ok\n");

    if (bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket d'ascolto");
        exit(1);
    }
    printf("Server: bind socket d'ascolto ok\n");

    if (listen(listen_sd, 5) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Server: listen ok\n");

    //Inizializzo stato delle stanze
    for (int i=0; i<N; i++) {
        strcpy(stanze[i].nome, "L");
        strcpy(stanze[i].stato, "L");
        for (int j=0; j<K; j++) {
            strcpy(stanze[i].utenti[j], "L");
        }
    }

    strcpy(stanze[0].nome, "Auto");
    strcpy(stanze[0].stato, "P");
    strcpy(stanze[0].utenti[0], "Gian");
    strcpy(stanze[0].utenti[1], "Marco");

    strcpy(stanze[1].nome, "Park");
    strcpy(stanze[1].stato, "M");
    strcpy(stanze[1].utenti[0], "Maria");
    strcpy(stanze[1].utenti[1], "Anna");

    /* AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE,
     * Quali altre primitive potrei usare? E' portabile su tutti i sistemi?
     * Pregi/Difetti?
     * Alcune risposte le potete trovare nel materiale aggiuntivo!
     */
    signal(SIGCHLD, gestore);

    /* CICLO DI RICEZIONE RICHIESTE --------------------------------------------- */
    for (;;) {
        len = sizeof(cliaddr);
        if ((conn_sd = accept(listen_sd, (struct sockaddr *)&cliaddr, &len)) < 0) {
            if (errno == EINTR) {
                perror("Forzo la continuazione della accept");
                continue;
            } else
                exit(1);
        }

        if (fork() == 0) { // figlio
            close(listen_sd);

            //CHIUDERE LATO LETTURA O SCRITTURA SE IL FIGLIO NON LO USA

            /* chiudo l'input */
            /*if (shutdown(conn_sd, SHUT_RD)<0){
                if (errno==EINTR) continue;
                else {
                    perror("shutdown(read)"); 
                    exit(8);
                }
            }*/

            /* chiudo l'output */
            /*if (shutdown(conn_sd, SHUT_WR)<0){
                if (errno==EINTR) continue;
                else {
                    perror("shutdown(read)"); 
                    exit(8);
                }
            }*/
                

            host = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (host == NULL) {
                printf("Server (figlio): host client e': %s\n", inet_ntoa(cliaddr.sin_addr));
            } else {
                printf("Server (figlio): host client e' %s \n", host->h_name);
            }

            // Leggo il carattere dell'operazione
            while ((nread = read(conn_sd, &c, sizeof(c)) > 0)) {
                printf("Server (figlio), operazione: %c\n", c);

                // 'V' per visualizzare lo stato delle stanze
                // 'S' per sospendere l'attività di una stanza

                if(c == 'V'){
                    printf("Server (figlio), invio la struttura...\n");

                    nsend = send(conn_sd, &stanze, sizeof(stanze), 0);

                    if(nsend != sizeof(stanze)){
                        perror("Errore nella send");
                        exit(EXIT_FAILURE);
                    }
                } //V
                else{
                    nread = read(conn_sd, nomeStanza, NAME_LENGTH);

                    if(nread != NAME_LENGTH){
                        perror("Read nomeStanza");
                        exit(EXIT_FAILURE);
                    }

                    printf("Server (figlio), sospendo la stanza %s\n", nomeStanza);

                    int res = -1;

                    for (int i=0; i<N; i++) {
                        if(strcmp(stanze[i].nome, nomeStanza) == 0 && stanze[i].stato[0] != 'S'){
                            stanze[i].stato[1] = stanze[i].stato[0];
                            stanze[i].stato[0] = 'S';
                            stanze[i].stato[2] = '\0';
                            res = 0;
                            break;
                        }
                    }

                    //mando l'esito al client
                    nsend = send(conn_sd, &res, sizeof(res), 0);

                    if(nsend != sizeof(res)){
                        perror("Send res");
                        exit(EXIT_FAILURE);
                    }
                } //S

            } //while
            
            // Chiudo la socket
            printf("Server (figlio): termino...\n");
            close(conn_sd);
            exit(0);

        } // figlio

        // Il padre chiude socket di connessione, non quella di ascolto
        close(conn_sd);

    } // ciclo for infinito
}
