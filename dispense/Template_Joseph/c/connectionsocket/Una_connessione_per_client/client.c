/* client.c
*/

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DIM_BUFF    256
#define LINE_LENGTH 128
#define NAME_LENGTH 64
#define N 20
#define K 10

/********************************************************/
typedef struct {
    char nome[NAME_LENGTH];
    char stato[3];
    char utenti[K][NAME_LENGTH];
} Stato;

/********************************************************/


int main(int argc, char *argv[]) {
    int                sd, fd_sorg, fd_dest, nread, nwrite, port, line ,res;
    int                okTerminator;
    long               nrcv;
    char               buff[DIM_BUFF], car, c, nomeStanza[NAME_LENGTH];
    char               operazione[2];
    char               terminator = '\0';
    Stato              stanze[N];
    struct hostent    *host;
    struct sockaddr_in servaddr;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3) {
        printf("Error:%s serverAddress serverPort\n", argv[0]);
        exit(1);
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    /*
     * NOTA: gethostbyname restituisce gli indirizzi gia' in formato di rete
     */
    host = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(1);
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

    /* CREAZIONE SOCKET ------------------------------------ */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Open socket: ");
        exit(3);
    }
    printf("Client: created socket sd=%d\n", sd);

    /* Operazione di BIND implicita nella connect */
    if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0) {
        perror("connect");
        exit(3);
    }
    printf("Client: connect ok\n");


    /* CORPO DEL CLIENT:
     ciclo di accettazione di richieste da utente ------- */
    printf("Inserire:\n'V' per visualizzare lo stato delle stanze\n'S' per sospedere l'attività di una stanza\nEOF per termminare\n");


    while (gets(operazione)) {
        //controllo se la stringa contiene il terminatore, per sapere se la stringa scritta dall'utente eccede il numero di caratteri massimo che può contenere
        okTerminator = 0;
        for(int i=0; i<2; i++){
            if(operazione[i] == terminator){
                okTerminator = 1;
                break;
            }
        }

        if(!okTerminator){
            printf("La stringa inserita eccede il massimo dei caratteri consentiti\n\n");
            printf("Inserire:\n'V' per visualizzare lo stato delle stanze\n'S' per sospedere l'attività di una stanza\nEOF per termminare\n");
            continue;
        }
        
        if(strcmp(operazione, "V") == 0){
            c = operazione[0];
            nwrite = write(sd, &c, sizeof(c));

            //leggo la struttura
            nrcv = recv(sd, &stanze, sizeof(stanze), 0);

            if(nrcv != sizeof(stanze)){
                perror("Recive stanze");
                exit(EXIT_FAILURE);
            }

            printf("Stato delle stanze:\n");
            for (int i=0; i<N; i++) {
                printf("Nome: %s \tStato: %s \tUtenti:", stanze[i].nome, stanze[i].stato);
                for (int j=0; j<K; j++) {
                    printf(" %s ", stanze[i].utenti[j]);
                }
                printf("\n\n");
            }
        } //V
        else if(strcmp(operazione, "S") == 0){

            printf("Inserisci il nome della stanza da sospendere: ");
            gets(nomeStanza);

            c = operazione[0];
            nwrite = write(sd, &c, sizeof(c));

            if(nwrite != sizeof(c)){
                perror("Write operazione");
                exit(EXIT_FAILURE);
            }

            nwrite = write(sd, nomeStanza, NAME_LENGTH);

            if(nwrite !=  NAME_LENGTH){
                perror("Write nomeStanza");
                exit(EXIT_FAILURE);
            }

            nrcv = recv(sd, &res, sizeof(res), 0);
            
            if(res<0){
                printf("Impossibile sospendere la stanza %s: stanza non esistente o già sospesa\n", nomeStanza);
            }
            else {
                printf("Stanza %s sospesa correttamente\n", nomeStanza);
            }

        } //S
        else{
            printf("Operazione non valida\n");
        }


        printf("\nInserire:\n'V' per visualizzare lo stato delle stanze\n'S' per sospedere l'attività di una stanza\nEOF per termminare\n");
    } // while

    /* Chiusura socket in ricezione */
    close(sd);
    printf("\nClient: termino...\n");
    exit(0);
}
